//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelResource.cpp
 ------------------------------------------------------------------------------
 Copyright (C) 2008-2021 Yiğit Orçun GÖKBULUT. All rights reserved.

 This file is part of the Zinek Engine  Software. Zinek Engine Software and the
 accompanying  materials are  made available  under the  terms of Zinek  Engine
 Commercial License or the GNU General Public License Version 3.

                      ZINEK ENGINE COMMERCIAL LICENSE
 Licensees  holding  valid  Zinek Engine Commercial  License(s) may  use  Zinek
 Engine  Software in  accordance  with   the  commercial  license  agreement(s)
 (which can only be  issued  by  copyright  owner "Yiğit  Orçun  GÖKBULUT") and
 provided with the Software  or, alternatively,  in  accordance with the  terms
 contained  in  a  written  agreement  between  you  and  copyright  owner. For
 licensing  terms  and conditions  please  contact  with  copyright owner.

                    GNU GENERAL PUBLIC LICENSE VERSION 3
 This program is free software: you can  redistribute it and/or modify it under
 the terms of the GNU General Public  License as published by the Free Software
 Foundation, either  version 3 of  the License, or  (at your option)  any later
 version. This program is  distributed in the hope that  it will be useful, but
 WITHOUT ANY WARRANTY; without even the  implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. See  the GNU General Public License for more
 details. You  should have received  a copy of the  GNU General  Public License
 along with this program. If not, see <https://www.gnu.org/licenses/>.

 Copyright Owner Information:
  Name: Yiğit Orçun GÖKBULUT
  Contact: orcun.gokbulut@gmail.com
  Github: https://www.github.com/orcun-gokbulut/ZE
*******************************************************************************/
//ZE_SOURCE_PROCESSOR_END()

#include "ZEError.h"
#include "ZEModelResource.h"
#include "ZEModelFileFormat.h"
#include "ZECore/ZEResourceManager.h"
#include "ZEGraphics/ZEVertexBuffer.h"
#include "ZEGraphics/ZEFixedMaterial.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZETexture/ZETexture2DResource.h"
#include "ZEFile/ZEFileInfo.h"
#include "ZEFile/ZEPathUtils.h"

#include <memory.h>
#include <string.h>
#include <float.h>
#include "ZEML/ZEMLSerialReader.h"

static ZEString ConstructResourcePath(const ZEString& Path)
{
	ZEString NewString = Path;
	ZESize ConstLength = strlen("resources\\") - 1;

	if (Path[0] == '\\' || Path[0] == '/')
		NewString = NewString.SubString(1, Path.GetLength() - 1);

	// If it is guaranteed that there is no "resources\\" string in beginning
	if (NewString.GetLength() - 1 < ConstLength)
	{
		NewString.Insert(0, "resources\\");
		return NewString;
	}
	// Else check if there is "resources\\" in the beginning
	else if (_stricmp("resources\\", Path.SubString(0, ConstLength)) != 0)
	{
		NewString.Insert(0, "resources\\");
		return NewString;
	}

	return NewString;
}

ZEStaticVertexBuffer* ZEModelResourceMeshLOD::GetSharedVertexBuffer() const
{
	if (SharedVertexBuffer == NULL)
		if (this->SkinnedVertices.GetCount() != 0)
		{
			((ZEModelResourceMeshLOD*)this)->SharedVertexBuffer = zeGraphics->CreateStaticVertexBuffer();
			SharedVertexBuffer->Create(sizeof(ZESkinnedModelVertex) * SkinnedVertices.GetCount());
			
			void* Buffer = SharedVertexBuffer->Lock();
				memcpy(Buffer, SkinnedVertices.GetConstCArray(), sizeof(ZESkinnedModelVertex) * SkinnedVertices.GetCount());
			SharedVertexBuffer->Unlock();
		}
		else if (Vertices.GetCount() != 0)
		{
			((ZEModelResourceMeshLOD*)this)->SharedVertexBuffer = zeGraphics->CreateStaticVertexBuffer();
			SharedVertexBuffer->Create(sizeof(ZEModelVertex) * Vertices.GetCount());
			
			void* Buffer = SharedVertexBuffer->Lock();
				memcpy(Buffer, Vertices.GetConstCArray(), sizeof(ZEModelVertex) * Vertices.GetCount());
			SharedVertexBuffer->Unlock();
		}

	return SharedVertexBuffer;
}

ZEStaticVertexBuffer* ZEModelResourceMeshLOD::CreatePrivateVertexBuffer() const
{
	ZEStaticVertexBuffer* VertexBuffer = NULL;
	if (SharedVertexBuffer == NULL)
		if (this->SkinnedVertices.GetCount() != 0)
		{
			VertexBuffer = zeGraphics->CreateStaticVertexBuffer();
			VertexBuffer->Create(sizeof(ZESkinnedModelVertex) * SkinnedVertices.GetCount());

			void* Buffer = VertexBuffer->Lock();
				memcpy(Buffer, SkinnedVertices.GetConstCArray(), sizeof(ZESkinnedModelVertex) * SkinnedVertices.GetCount());
			VertexBuffer->Unlock();
		}
		else if (Vertices.GetCount() != 0)
		{
			VertexBuffer = zeGraphics->CreateStaticVertexBuffer();
			VertexBuffer->Create(sizeof(ZEModelVertex) * Vertices.GetCount());
			
			void* Buffer = VertexBuffer->Lock();
				memcpy(Buffer, Vertices.GetConstCArray(), sizeof(ZEModelVertex) * Vertices.GetCount());
			VertexBuffer->Unlock();
		}
	return VertexBuffer;
}

ZEModelResourceMeshLOD::ZEModelResourceMeshLOD()
{
	SharedVertexBuffer = NULL;
	LODLevel = 0;
	MaterialId = -1;
}

ZEModelResourceMeshLOD::~ZEModelResourceMeshLOD()
{
	if (SharedVertexBuffer != NULL)
		SharedVertexBuffer->Destroy();
}


//////////////////////////////////////////////////////////////// READ ////////////////////////////////////////////////////////////////

static const ZETexture2D* ManageModelMaterialTextures(char* FileName, ZESmartArray<ZETexture2DResource*>& TextureResources)
{
	if (strncmp(FileName, "", ZE_MDLF_MAX_FILENAME_SIZE) == 0)
		return NULL;

	for (ZESize I = 0; I < TextureResources.GetCount(); I++)
	{
		//if (strnicmp(TextureResources[I]->GetFileName(), FileName, ZE_MDLF_MAX_FILENAME_SIZE) == 0)
		if (TextureResources[I]->GetFileName() == ZEString(FileName))
			return TextureResources[I]->GetTexture();
	}

	ZETexture2DResource* NewTextureResource = ZETexture2DResource::LoadSharedResource(FileName);
	if (NewTextureResource == NULL)
	{
		zeError("Can not load texture file. (FileName : \"%s\")", FileName);
		return NULL;
	}
	TextureResources.Add(NewTextureResource);
	return NewTextureResource->GetTexture();
}

#define ZE_MFSC_SKINTRANSFORM				1
#define ZE_MFSC_DIFFUSEMAP					2
#define ZE_MFSC_NORMALMAP					4
#define ZE_MFSC_SPECULARMAP					8
#define ZE_MFSC_EMMISIVEMAP					16
#define ZE_MFSC_OCAPASITYMAP				32
#define ZE_MFSC_DETAILDIFFUSEMAP			64
#define ZE_MFSC_DETAILNORMALMAP				128
#define ZE_MFSC_REFLECTION					256
#define ZE_MFSC_REFRACTION					512
#define ZE_MFSC_LIGHTMAP					1024
#define ZE_MFSC_DISTORTIONMAP				2048

static bool ReadMaterialsFromFile(ZEModelResource* Model, ZEMLSerialReader* NodeReader)
{
// 	while (NodeReader->Read())
// 	{	
// 		if (NodeReader->GetItemName() != "Material")
// 			break;

	while (NodeReader->GetItemName() == "Materials" || NodeReader->GetItemName() == "Material")
	{
		if (NodeReader->GetItemName() == "Materials")
			NodeReader->Read();

		ZEVariant NameValue, FilePathValue;

		ZEMLSerialListItem MaterialList[] = 
		{
			ZEML_LIST_PROPERTY("Name",		NameValue,		ZE_VRT_STRING,	true),
			ZEML_LIST_PROPERTY("FilePath",	FilePathValue,	ZE_VRT_STRING,	true)
		};

		if (!NodeReader->ReadPropertyList(MaterialList, 2))
			return false;

		ZEString MaterialPath = ZEFileInfo::GetParentDirectory(Model->GetFileName()) + ZEPathUtils::GetSeperator() + FilePathValue.GetString();

		if (!ZEFileInfo::IsFile(MaterialPath))
			return false;

		ZEFixedMaterial* CurrentMaterial = ZEFixedMaterial::CreateInstance();
		CurrentMaterial->ReadFromFile(MaterialPath);
		Model->Materials.Add(CurrentMaterial);

		if (NodeReader->GetItemName() != "Material")
			NodeReader->Read();
	}

	NodeReader->GoToCurrentPointer();

	return true;
}

static bool ReadPhysicalBodyFromFile(ZEModelResourcePhysicalBody* Body, ZEMLSerialReader* NodeReader)
{

	ZEVariant Enabled, Type, Mass, MassCenter, LinearDamping, AngularDamping;
	ZEMLSerialPointer PhysicalShapesNodePointer;

	ZEMLSerialListItem PhysicalBodyList[] = 
	{
		ZEML_LIST_PROPERTY("Enabled",			Enabled,		ZE_VRT_BOOLEAN,		true),
		ZEML_LIST_PROPERTY("Type",				Type,			ZE_VRT_INTEGER_32,	true),
		ZEML_LIST_PROPERTY("Mass",				Mass,			ZE_VRT_FLOAT,		true),
		ZEML_LIST_PROPERTY("MassCenter",		MassCenter,		ZE_VRT_VECTOR3,		true),
		ZEML_LIST_PROPERTY("LinearDamping",		LinearDamping,	ZE_VRT_FLOAT,		true),
		ZEML_LIST_PROPERTY("AngularDamping",	AngularDamping, ZE_VRT_FLOAT,		true),
		ZEML_LIST_NODE("PhysicalShapes",		PhysicalShapesNodePointer,			true)
	};

	if (!NodeReader->ReadPropertyList(PhysicalBodyList, 7))
		return false;

	Body->Enabled = Enabled;
	Body->Type = (ZEModelResourcePhysicalBodyType)Type.GetInt32();
	Body->Mass = Mass;
	Body->MassCenter = MassCenter;
	Body->LinearDamping = LinearDamping;
	Body->AngularDamping = AngularDamping;

	NodeReader->SeekPointer(PhysicalShapesNodePointer);

	while (NodeReader->GetItemName() == "PhysicalShapes" || NodeReader->GetItemName() == "PhysicalShape")
	{
		if(NodeReader->GetItemName() == "PhysicalShapes")
			NodeReader->Read();

		ZEVariant ShapePosition, ShapeRotation, ShapeRestitution, ShapeDynamicFriction, ShapeStaticFriction, ShapeType;
		ZEMLSerialPointer ShapeNodePointer;

		ZEMLSerialListItem PhysicalShapeList [] = 
		{
			ZEML_LIST_PROPERTY("Position",			ShapePosition,			ZE_VRT_VECTOR3,		true),
			ZEML_LIST_PROPERTY("Rotation",			ShapeRotation,			ZE_VRT_QUATERNION,	true),
			ZEML_LIST_PROPERTY("Restitution",		ShapeRestitution,		ZE_VRT_FLOAT,		true),
			ZEML_LIST_PROPERTY("DynamicFriction",	ShapeDynamicFriction,	ZE_VRT_FLOAT,		true),
			ZEML_LIST_PROPERTY("StaticFriction",	ShapeStaticFriction,	ZE_VRT_FLOAT,		true),
			ZEML_LIST_PROPERTY("Type",				ShapeType,				ZE_VRT_INTEGER_32,	true),
			ZEML_LIST_NODE("Shape",					ShapeNodePointer,							true)
		};

		if (!NodeReader->ReadPropertyList(PhysicalShapeList, 7))
			return false;

		ZEModelResourcePhysicalShape* Shape = Body->Shapes.Add();

		Shape->Position = ShapePosition;
		Shape->Rotation = ShapeRotation;
		Shape->Restitution = ShapeRestitution;
		Shape->DynamicFriction = ShapeDynamicFriction;
		Shape->StaticFriction = ShapeStaticFriction;
		Shape->Type = (ZEPhysicalShapeType)ShapeType.GetInt32();

		NodeReader->SeekPointer(ShapeNodePointer);

		ZEVariant ShapeHeight, ShapeWidth, ShapeLength, ShapeRadius;
		ZEMLSerialPointer VerticesDataPointer;

		ZEMLSerialListItem ActualShapeList [] = 
		{
			ZEML_LIST_PROPERTY("Height", ShapeHeight,			ZE_VRT_FLOAT,	false),
			ZEML_LIST_PROPERTY("Width", ShapeWidth,				ZE_VRT_FLOAT,	false),
			ZEML_LIST_PROPERTY("Length", ShapeLength,			ZE_VRT_FLOAT,	false),
			ZEML_LIST_PROPERTY("Radius", ShapeRadius,			ZE_VRT_FLOAT,	false),
			ZEML_LIST_DATA("Vertices", VerticesDataPointer,		ZE_VRT_FLOAT,	false)
		};

		NodeReader->ReadPropertyList(ActualShapeList, 5);

		switch (Shape->Type)
		{
			case ZE_MFPST_BOX:
			{
				Shape->Box.Width = ShapeWidth;
				Shape->Box.Height = ShapeHeight;
				Shape->Box.Length = ShapeLength;
				break;
			}
			case ZE_MFPST_SPHERE:
			{
				Shape->Sphere.Radius = ShapeRadius;
				break;
			}
			case ZE_MFPST_CAPSULE:
			{
				Shape->Capsule.Radius = ShapeRadius;
				Shape->Capsule.Height = ShapeHeight;
				break;
			}
			case ZE_MFPST_CYLINDER:
			{
				Shape->Cylinder.Radius = ShapeRadius;
				Shape->Cylinder.Height = ShapeHeight;
				break;
			}
			case ZE_MFPST_CONVEX:
			{
				NodeReader->SeekPointer(VerticesDataPointer);
				Shape->Convex.Vertices.SetCount((ZESize)NodeReader->GetDataSize() / sizeof(ZEVector3));
				NodeReader->GetData(Shape->Convex.Vertices.GetCArray(), (ZESize)NodeReader->GetDataSize());
				break;
			}
			default:
			{
				zeError("Unsupported physical shape type. (Physical Shape : %d)", Shape->Type);
				return false;
			}
		}

		NodeReader->Read();
	}

	return true;
}

static void CalculateBoundingBox(ZEModelResourceMesh* Mesh)
{
	Mesh->BoundingBox.Min = ZEVector3(FLT_MAX, FLT_MAX, FLT_MAX);
	Mesh->BoundingBox.Max = ZEVector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (ZESize I = 0; I < Mesh->LODs.GetCount(); I++)
	{
		ZEModelResourceMeshLOD* CurrentLOD = &Mesh->LODs[I];
		if (Mesh->IsSkinned)
		{
			for (ZESize N = 0; N < CurrentLOD->SkinnedVertices.GetCount(); N++)
			{
				ZEVector3& Position = CurrentLOD->SkinnedVertices[N].Position;

				if (Position.x < Mesh->BoundingBox.Min.x) Mesh->BoundingBox.Min.x = Position.x;
				if (Position.y < Mesh->BoundingBox.Min.y) Mesh->BoundingBox.Min.y = Position.y;
				if (Position.z < Mesh->BoundingBox.Min.z) Mesh->BoundingBox.Min.z = Position.z;
				if (Position.x > Mesh->BoundingBox.Max.x) Mesh->BoundingBox.Max.x = Position.x;
				if (Position.y > Mesh->BoundingBox.Max.y) Mesh->BoundingBox.Max.y = Position.y;
				if (Position.z > Mesh->BoundingBox.Max.z) Mesh->BoundingBox.Max.z = Position.z;
			}
		}
		else
		{
			for (ZESize N = 0; N < CurrentLOD->Vertices.GetCount(); N++)
			{
				ZEVector3& Position = CurrentLOD->Vertices[N].Position;

				if (Position.x < Mesh->BoundingBox.Min.x) Mesh->BoundingBox.Min.x = Position.x;
				if (Position.y < Mesh->BoundingBox.Min.y) Mesh->BoundingBox.Min.y = Position.y;
				if (Position.z < Mesh->BoundingBox.Min.z) Mesh->BoundingBox.Min.z = Position.z;
				if (Position.x > Mesh->BoundingBox.Max.x) Mesh->BoundingBox.Max.x = Position.x;
				if (Position.y > Mesh->BoundingBox.Max.y) Mesh->BoundingBox.Max.y = Position.y;
				if (Position.z > Mesh->BoundingBox.Max.z) Mesh->BoundingBox.Max.z = Position.z;
			}
		}
	}
}

static bool ReadMeshesFromFile(ZEModelResource* Model, ZEMLSerialReader* NodeReader)
{

	while (NodeReader->GetItemName() == "Meshes" || NodeReader->GetItemName() == "Mesh")
	{
		if(NodeReader->GetItemName() == "Meshes")
			NodeReader->Read();

		ZEVariant NameValue, PositionValue, RotationValue, ScaleValue, IsSkinnedValue;
		ZEMLSerialPointer BoundingBoxNodePointer, PhysicalBodyNodePointer, LODsNodePointer;

		ZEMLSerialListItem MeshList[] = 
		{
			ZEML_LIST_PROPERTY("Name",			NameValue,				ZE_VRT_STRING,		true),
			ZEML_LIST_NODE("BoundingBox",		BoundingBoxNodePointer,						true),
			ZEML_LIST_PROPERTY("Position",		PositionValue,			ZE_VRT_VECTOR3,		true),
			ZEML_LIST_PROPERTY("Rotation",		RotationValue,			ZE_VRT_QUATERNION,	true),
			ZEML_LIST_PROPERTY("Scale",			ScaleValue,				ZE_VRT_VECTOR3,		true),
			ZEML_LIST_PROPERTY("IsSkinned",		IsSkinnedValue,			ZE_VRT_BOOLEAN,		true),
			ZEML_LIST_NODE("PhysicalBody",		PhysicalBodyNodePointer,					false),
			ZEML_LIST_NODE("LODs",				LODsNodePointer,							true)
		};

		if (!NodeReader->ReadPropertyList(MeshList, 8))
			return false;

		ZEModelResourceMesh* Mesh = Model->Meshes.Add();

		strncpy(Mesh->Name, NameValue.GetString(), ZE_MDLF_MAX_NAME_SIZE);
		Mesh->Position = PositionValue;
		Mesh->Rotation = RotationValue;
		Mesh->Scale = ScaleValue;
		Mesh->IsSkinned = IsSkinnedValue;

		NodeReader->SeekPointer(BoundingBoxNodePointer);

		ZEVariant BBoxMinValue, BBoxMaxValue;

		ZEMLSerialListItem BoundingBoxList[] = 
		{
			ZEML_LIST_PROPERTY("Min", BBoxMinValue,	ZE_VRT_VECTOR3,	true),
			ZEML_LIST_PROPERTY("Max", BBoxMaxValue,	ZE_VRT_VECTOR3,	true)
		};

		if(!NodeReader->ReadPropertyList(BoundingBoxList, 2))
			return false;

		Mesh->BoundingBox.Min = BBoxMinValue;
		Mesh->BoundingBox.Max = BBoxMaxValue;

		NodeReader->SeekPointer(LODsNodePointer);

		while (NodeReader->GetItemName() == "LODs" || NodeReader->GetItemName() == "LOD")
		{
			if (NodeReader->GetItemName() == "LODs")
				NodeReader->Read();

			ZEVariant LODLevelValue, MaterialIdValue;
			ZEMLSerialPointer VerticesDataPointer, AffectingBoneIdsDataPointer;

			ZEMLSerialListItem LODList[] = 
			{
				ZEML_LIST_PROPERTY("LODLevel",		LODLevelValue,		ZE_VRT_INTEGER_32,	true),
				ZEML_LIST_PROPERTY("MaterialId",	MaterialIdValue,	ZE_VRT_INTEGER_32,	true),
				ZEML_LIST_DATA("Vertices",			VerticesDataPointer,					false),
				ZEML_LIST_DATA("AffectingBoneIds",	AffectingBoneIdsDataPointer,			false)
			};

			if (!NodeReader->ReadPropertyList(LODList, 4))
				return false;

			ZEModelResourceMeshLOD* LOD = Mesh->LODs.Add();

			LOD->LODLevel = LODLevelValue;
			LOD->MaterialId = MaterialIdValue;

			if (Mesh->IsSkinned)
			{
				if (AffectingBoneIdsDataPointer == -1)
					return false;

				NodeReader->SeekPointer(VerticesDataPointer);
				LOD->SkinnedVertices.SetCount((ZESize)NodeReader->GetDataSize() / sizeof(ZESkinnedModelVertex));
				NodeReader->GetData(LOD->SkinnedVertices.GetCArray(), (ZESize)NodeReader->GetDataSize());

				NodeReader->SeekPointer(AffectingBoneIdsDataPointer);
				LOD->AffectingBoneIds.SetCount((ZESize)NodeReader->GetDataSize() / sizeof(ZEUInt32));
				NodeReader->GetData(LOD->AffectingBoneIds.GetCArray(), (ZESize)NodeReader->GetDataSize());

			}
			else
			{
				if (VerticesDataPointer == -1)
					return false;

				NodeReader->SeekPointer(VerticesDataPointer);
				LOD->Vertices.SetCount((ZESize)NodeReader->GetDataSize() / sizeof(ZEModelVertex));
				NodeReader->GetData(LOD->Vertices.GetCArray(), (ZESize)NodeReader->GetDataSize());
			}

			NodeReader->Read();
		}

		CalculateBoundingBox(Mesh);

		if (PhysicalBodyNodePointer != -1)
		{
			NodeReader->SeekPointer(PhysicalBodyNodePointer);
			ReadPhysicalBodyFromFile(&Mesh->PhysicalBody, NodeReader);
		}
		else
		{
			Mesh->PhysicalBody.Shapes.Clear();
			memset(&Mesh->PhysicalBody, 0, sizeof(ZEModelResourcePhysicalBody));
		}

		if (NodeReader->GetItemName() != "Mesh")
			NodeReader->Read();

	}

	NodeReader->GoToCurrentPointer();

	return true;
}

static bool ReadPhysicalJointFromFile(ZEModelResourcePhysicalJoint* Joint, ZEMLSerialReader* NodeReader)
{

	ZEVariant Enabled, Body1Id, Body2Id;
	ZEVariant UseGlobalAnchorAxis, LocalAnchor1, LocalAxis1, LocalAnchor2, LocalAxis2, GlobalAnchor, GlobalAxis;
	ZEVariant JointType, CollideBodies, Breakable, BreakForce, BreakTorque;
	ZEVariant XMotion, YMotion, ZMotion, LinearLimitValue, LinearLimitRestitution, LinearLimitSpring, LinearLimitDamping;
	ZEVariant TwistMotion, TwistLowLimitValue, TwistLowLimitRestitution, TwistLowLimitSpring, TwistLowLimitDamping, TwistHighLimitValue, TwistHighLimitRestitution, TwistHighLimitSpring, TwistHighLimitDamping;
	ZEVariant Swing1Motion, Swing1LimitValue, Swing1LimitRestitution, Swing1LimitSpring, Swing1LimitDamping;
	ZEVariant Swing2Motion, Swing2LimitValue, Swing2LimitRestitution, Swing2LimitSpring, Swing2LimitDamping;
	ZEVariant LinearXMotor, LinearXMotorForce, LinearXMotorSpring, LinearXMotorDamper;
	ZEVariant LinearYMotor, LinearYMotorForce, LinearYMotorSpring, LinearYMotorDamper;
	ZEVariant LinearZMotor, LinearZMotorForce, LinearZMotorSpring, LinearZMotorDamper;
	ZEVariant AngularSwingMotor, AngularSwingMotorForce, AngularSwingMotorSpring, AngularSwingMotorDamper;
	ZEVariant AngularTwistMotor, AngularTwistMotorForce, AngularTwistMotorSpring, AngularTwistMotorDamper;
	ZEVariant AngularSlerpMotor, AngularSlerpMotorForce, AngularSlerpMotorSpring, AngularSlerpMotorDamper;
	ZEVariant MotorTargetPosition, MotorTargetVelocity, MotorTargetOrientation, MotorTargetAngularVelocity;

	ZEMLSerialListItem JointList[] = 
	{
		ZEML_LIST_PROPERTY("Enabled",						Enabled,						ZE_VRT_BOOLEAN,		true),
		ZEML_LIST_PROPERTY("Body1Id",						Body1Id,						ZE_VRT_INTEGER_32,	true),
		ZEML_LIST_PROPERTY("Body2Id",						Body2Id,						ZE_VRT_INTEGER_32,	true),

		ZEML_LIST_PROPERTY("UseGlobalAnchorAxis",			UseGlobalAnchorAxis,			ZE_VRT_BOOLEAN,		true),
		ZEML_LIST_PROPERTY("LocalAnchor1",					LocalAnchor1,					ZE_VRT_VECTOR3,		false),
		ZEML_LIST_PROPERTY("LocalAxis1",					LocalAxis1,						ZE_VRT_QUATERNION,	false),
		ZEML_LIST_PROPERTY("LocalAnchor2",					LocalAnchor2,					ZE_VRT_VECTOR3,		false),
		ZEML_LIST_PROPERTY("LocalAxis2",					LocalAxis2,						ZE_VRT_QUATERNION,	false),
		ZEML_LIST_PROPERTY("GlobalAnchor",					GlobalAnchor,					ZE_VRT_VECTOR3,		false),
		ZEML_LIST_PROPERTY("GlobalAxis",					GlobalAxis,						ZE_VRT_QUATERNION,	false),

		ZEML_LIST_PROPERTY("JointType",						JointType,						ZE_VRT_INTEGER_32,	true),
		ZEML_LIST_PROPERTY("CollideBodies",					CollideBodies,					ZE_VRT_BOOLEAN,		true),
		ZEML_LIST_PROPERTY("Breakable",						Breakable,						ZE_VRT_BOOLEAN,		true),
		ZEML_LIST_PROPERTY("BreakForce",					BreakForce,						ZE_VRT_FLOAT,		true),
		ZEML_LIST_PROPERTY("BreakTorque",					BreakTorque,					ZE_VRT_FLOAT,		true),

		ZEML_LIST_PROPERTY("XMotion",						XMotion,						ZE_VRT_INTEGER_32,	true),
		ZEML_LIST_PROPERTY("YMotion",						YMotion,						ZE_VRT_INTEGER_32,	true),
		ZEML_LIST_PROPERTY("ZMotion",						ZMotion,						ZE_VRT_INTEGER_32,	true),
		ZEML_LIST_PROPERTY("LinearLimitValue",				LinearLimitValue,				ZE_VRT_FLOAT,		true),
		ZEML_LIST_PROPERTY("LinearLimitRestitution",		LinearLimitRestitution,			ZE_VRT_FLOAT,		true),
		ZEML_LIST_PROPERTY("LinearLimitSpring",				LinearLimitSpring,				ZE_VRT_FLOAT,		true),
		ZEML_LIST_PROPERTY("LinearLimitDamping",			LinearLimitDamping,				ZE_VRT_FLOAT,		true),

		ZEML_LIST_PROPERTY("TwistMotion",					TwistMotion,					ZE_VRT_INTEGER_32,	true),
		ZEML_LIST_PROPERTY("TwistLowLimitValue",			TwistLowLimitValue,				ZE_VRT_FLOAT,		true),
		ZEML_LIST_PROPERTY("TwistLowLimitRestitution",		TwistLowLimitRestitution,		ZE_VRT_FLOAT,		true),
		ZEML_LIST_PROPERTY("TwistLowLimitSpring",			TwistLowLimitSpring,			ZE_VRT_FLOAT,		true),
		ZEML_LIST_PROPERTY("TwistLowLimitDamping",			TwistLowLimitDamping,			ZE_VRT_FLOAT,		true),
		ZEML_LIST_PROPERTY("TwistHighLimitValue",			TwistHighLimitValue,			ZE_VRT_FLOAT,		true),
		ZEML_LIST_PROPERTY("TwistHighLimitRestitution",		TwistHighLimitRestitution,		ZE_VRT_FLOAT,		true),
		ZEML_LIST_PROPERTY("TwistHighLimitSpring",			TwistHighLimitSpring,			ZE_VRT_FLOAT,		true),
		ZEML_LIST_PROPERTY("TwistHighLimitDamping",			TwistHighLimitDamping,			ZE_VRT_FLOAT,		true),

		ZEML_LIST_PROPERTY("Swing1Motion",					Swing1Motion,					ZE_VRT_INTEGER_32,	true),
		ZEML_LIST_PROPERTY("Swing1LimitValue",				Swing1LimitValue,				ZE_VRT_FLOAT,		true),
		ZEML_LIST_PROPERTY("Swing1LimitRestitution",		Swing1LimitRestitution,			ZE_VRT_FLOAT,		true),
		ZEML_LIST_PROPERTY("Swing1LimitSpring",				Swing1LimitSpring,				ZE_VRT_FLOAT,		true),
		ZEML_LIST_PROPERTY("Swing1LimitDamping",			Swing1LimitDamping,				ZE_VRT_FLOAT,		true),

		ZEML_LIST_PROPERTY("Swing2Motion",					Swing2Motion,					ZE_VRT_INTEGER_32,	true),
		ZEML_LIST_PROPERTY("Swing2LimitValue",				Swing2LimitValue,				ZE_VRT_FLOAT,		true),
		ZEML_LIST_PROPERTY("Swing2LimitRestitution",		Swing2LimitRestitution,			ZE_VRT_FLOAT,		true),
		ZEML_LIST_PROPERTY("Swing2LimitSpring",				Swing2LimitSpring,				ZE_VRT_FLOAT,		true),
		ZEML_LIST_PROPERTY("Swing2LimitDamping",			Swing2LimitDamping,				ZE_VRT_FLOAT,		true),

		ZEML_LIST_PROPERTY("LinearXMotor",					LinearXMotor,					ZE_VRT_INTEGER_32,	true),
		ZEML_LIST_PROPERTY("LinearXMotorForce",				LinearXMotorForce,				ZE_VRT_FLOAT,		true),
		ZEML_LIST_PROPERTY("LinearXMotorSpring",			LinearXMotorSpring,				ZE_VRT_FLOAT,		true),
		ZEML_LIST_PROPERTY("LinearXMotorDamper",			LinearXMotorDamper,				ZE_VRT_FLOAT,		true),

		ZEML_LIST_PROPERTY("LinearYMotor",					LinearYMotor,					ZE_VRT_INTEGER_32,	true),
		ZEML_LIST_PROPERTY("LinearYMotorForce",				LinearYMotorForce,				ZE_VRT_FLOAT,		true),
		ZEML_LIST_PROPERTY("LinearYMotorSpring",			LinearYMotorSpring,				ZE_VRT_FLOAT,		true),
		ZEML_LIST_PROPERTY("LinearYMotorDamper",			LinearYMotorDamper,				ZE_VRT_FLOAT,		true),

		ZEML_LIST_PROPERTY("LinearZMotor",					LinearZMotor,					ZE_VRT_INTEGER_32,	true),
		ZEML_LIST_PROPERTY("LinearZMotorForce",				LinearZMotorForce,				ZE_VRT_FLOAT,		true),
		ZEML_LIST_PROPERTY("LinearZMotorSpring",			LinearZMotorSpring,				ZE_VRT_FLOAT,		true),
		ZEML_LIST_PROPERTY("LinearZMotorDamper",			LinearZMotorDamper,				ZE_VRT_FLOAT,		true),

		ZEML_LIST_PROPERTY("AngularSwingMotor",				AngularSwingMotor,				ZE_VRT_INTEGER_32,	true),
		ZEML_LIST_PROPERTY("AngularSwingMotorForce",		AngularSwingMotorForce,			ZE_VRT_FLOAT,		true),
		ZEML_LIST_PROPERTY("AngularSwingMotorSpring",		AngularSwingMotorSpring,		ZE_VRT_FLOAT,		true),
		ZEML_LIST_PROPERTY("AngularSwingMotorDamper",		AngularSwingMotorDamper,		ZE_VRT_FLOAT,		true),

		ZEML_LIST_PROPERTY("AngularTwistMotor",				AngularTwistMotor,				ZE_VRT_INTEGER_32,	true),
		ZEML_LIST_PROPERTY("AngularTwistMotorForce",		AngularTwistMotorForce,			ZE_VRT_FLOAT,		true),
		ZEML_LIST_PROPERTY("AngularTwistMotorSpring",		AngularTwistMotorSpring,		ZE_VRT_FLOAT,		true),
		ZEML_LIST_PROPERTY("AngularTwistMotorDamper",		AngularTwistMotorDamper,		ZE_VRT_FLOAT,		true),

		ZEML_LIST_PROPERTY("AngularSlerpMotor",				AngularSlerpMotor,				ZE_VRT_INTEGER_32,	true),
		ZEML_LIST_PROPERTY("AngularSlerpMotorForce",		AngularSlerpMotorForce,			ZE_VRT_FLOAT,		true),
		ZEML_LIST_PROPERTY("AngularSlerpMotorSpring",		AngularSlerpMotorSpring,		ZE_VRT_FLOAT,		true),
		ZEML_LIST_PROPERTY("AngularSlerpMotorDamper",		AngularSlerpMotorDamper,		ZE_VRT_FLOAT,		true),

		ZEML_LIST_PROPERTY("MotorTargetPosition",			MotorTargetPosition,			ZE_VRT_VECTOR3,		true),
		ZEML_LIST_PROPERTY("MotorTargetVelocity",			MotorTargetVelocity,			ZE_VRT_VECTOR3,		true),
		ZEML_LIST_PROPERTY("MotorTargetOrientation",		MotorTargetOrientation,			ZE_VRT_VECTOR3,		true),
		ZEML_LIST_PROPERTY("MotorTargetAngularVelocity",	MotorTargetAngularVelocity,		ZE_VRT_VECTOR3,		true),
	};

	if (!NodeReader->ReadPropertyList(JointList, 69))
		return false;

	Joint->Enabled = Enabled;
	Joint->Body1Id = Body1Id;
	Joint->Body2Id = Body2Id;

	Joint->UseGlobalAnchorAxis = UseGlobalAnchorAxis;

	if (UseGlobalAnchorAxis)
	{
		Joint->GlobalAnchor = GlobalAnchor;
		Joint->GlobalAxis = GlobalAxis;
	}
	else
	{
		Joint->LocalAnchor1 = LocalAnchor1;
		Joint->LocalAxis1 = LocalAxis1;
		Joint->LocalAnchor2 = LocalAnchor2;
		Joint->LocalAxis2 = LocalAxis2;
	}

	Joint->JointType = (ZEPhysicalJointType)JointType.GetInt32();
	Joint->CollideBodies = CollideBodies;
	Joint->Breakable = Breakable;
	Joint->BreakForce = BreakForce;
	Joint->BreakTorque = BreakTorque;

	Joint->XMotion = (ZEPhysicalJointMotion)XMotion.GetInt32();
	Joint->YMotion = (ZEPhysicalJointMotion)YMotion.GetInt32();
	Joint->ZMotion = (ZEPhysicalJointMotion)ZMotion.GetInt32();
	Joint->LinearLimitValue = LinearLimitValue;
	Joint->LinearLimitRestitution = LinearLimitRestitution;
	Joint->LinearLimitSpring = LinearLimitSpring;
	Joint->LinearLimitDamping = LinearLimitDamping;

	Joint->TwistMotion = (ZEPhysicalJointMotion)TwistMotion.GetInt32();
	Joint->TwistLowLimitValue = TwistLowLimitValue;
	Joint->TwistLowLimitRestitution = TwistLowLimitRestitution;
	Joint->TwistLowLimitSpring = TwistLowLimitSpring;
	Joint->TwistLowLimitDamping = TwistLowLimitDamping;
	Joint->TwistHighLimitValue = TwistHighLimitValue;
	Joint->TwistHighLimitRestitution = TwistHighLimitRestitution;
	Joint->TwistHighLimitSpring = TwistHighLimitSpring;
	Joint->TwistHighLimitDamping = TwistHighLimitDamping;

	Joint->Swing1Motion = (ZEPhysicalJointMotion)Swing1Motion.GetInt32();
	Joint->Swing1LimitValue = Swing1LimitValue;
	Joint->Swing1LimitRestitution = Swing1LimitRestitution;
	Joint->Swing1LimitSpring = Swing1LimitSpring;
	Joint->Swing1LimitDamping = Swing1LimitDamping;

	Joint->Swing2Motion = (ZEPhysicalJointMotion)Swing2Motion.GetInt32();
	Joint->Swing2LimitValue = Swing2LimitValue;
	Joint->Swing2LimitRestitution = Swing2LimitRestitution;
	Joint->Swing2LimitSpring = Swing2LimitSpring;
	Joint->Swing2LimitDamping = Swing2LimitDamping;

	Joint->LinearXMotor = (ZEPhysicalJointMotorType)LinearXMotor.GetInt32();
	Joint->LinearXMotorForce = LinearXMotorForce;
	Joint->LinearXMotorSpring = LinearXMotorSpring;
	Joint->LinearXMotorDamper = LinearXMotorDamper;

	Joint->LinearYMotor = (ZEPhysicalJointMotorType)LinearYMotor.GetInt32();
	Joint->LinearYMotorForce = LinearYMotorForce;
	Joint->LinearYMotorSpring = LinearYMotorSpring;
	Joint->LinearYMotorDamper = LinearYMotorDamper;

	Joint->LinearZMotor = (ZEPhysicalJointMotorType)LinearZMotor.GetInt32();
	Joint->LinearZMotorForce = LinearZMotorForce;
	Joint->LinearZMotorSpring = LinearZMotorSpring;
	Joint->LinearZMotorDamper = LinearZMotorDamper;

	Joint->AngularSwingMotor = (ZEPhysicalJointMotorType)AngularSwingMotor.GetInt32();
	Joint->AngularSwingMotorForce = AngularSwingMotorForce;
	Joint->AngularSwingMotorSpring = AngularSwingMotorSpring;
	Joint->AngularSwingMotorDamper = AngularSwingMotorDamper;

	Joint->AngularTwistMotor = (ZEPhysicalJointMotorType)AngularTwistMotor.GetInt32();
	Joint->AngularTwistMotorForce = AngularTwistMotorForce;
	Joint->AngularTwistMotorSpring = AngularTwistMotorSpring;
	Joint->AngularTwistMotorDamper = AngularTwistMotorDamper;

	Joint->AngularSlerpMotor = (ZEPhysicalJointMotorType)AngularSlerpMotor.GetInt32();
	Joint->AngularSlerpMotorForce = AngularSlerpMotorForce;
	Joint->AngularSlerpMotorSpring = AngularSlerpMotorSpring;
	Joint->AngularSlerpMotorDamper = AngularSlerpMotorDamper;

	Joint->MotorTargetPosition = MotorTargetPosition;
	Joint->MotorTargetVelocity = MotorTargetVelocity;
	Joint->MotorTargetOrientation = MotorTargetOrientation;
	Joint->MotorTargetAngularVelocity = MotorTargetAngularVelocity;

	return true;
}

static void ProcessBones(ZEModelResource* Model, ZEModelResourceBone* Bone, ZEInt BoneId)
{
	ZEMatrix4x4::CreateOrientation(Bone->RelativeTransform, Bone->RelativePosition, Bone->RelativeRotation, Bone->RelativeScale);

	if (Bone->ParentBone != -1)
	{

		ZEMatrix4x4::Multiply(Bone->ForwardTransform, Model->Bones[(ZESize)Bone->ParentBone].ForwardTransform, Bone->RelativeTransform);
		ZEMatrix4x4::Inverse(Bone->InverseTransform, Bone->ForwardTransform);
	}
	else
	{
		Bone->ForwardTransform = Bone->RelativeTransform;
		ZEMatrix4x4::Inverse(Bone->InverseTransform, Bone->ForwardTransform);
	}

	for (ZESize I = 0; I < Model->Bones.GetCount(); I++)
	{
		if (Model->Bones[I].ParentBone == BoneId)
		{
			ProcessBones(Model, &Model->Bones[I], (ZEInt)I);
		}
	}
}

static bool ReadBonesFromFile(ZEModelResource* Model, ZEMLSerialReader* NodeReader)
{

	while (NodeReader->GetItemName() == "Bones" || NodeReader->GetItemName() == "Bone")
	{
		if(NodeReader->GetItemName() == "Bones")
			NodeReader->Read();

		ZEVariant NameValue, ParentBoneValue, RelativePositionValue, RelativeRotationValue, RelativeScaleValue;
		ZEMLSerialPointer BoundingBoxNodePointer, PhysicalJointNodePointer, PhysicalBodyNodePointer;

		ZEMLSerialListItem BoneList[] = 
		{
			ZEML_LIST_PROPERTY("Name",				NameValue,					ZE_VRT_STRING,		true),
			ZEML_LIST_PROPERTY("ParentBone",		ParentBoneValue,			ZE_VRT_INTEGER_32,	true),
			ZEML_LIST_NODE("BoundingBox",			BoundingBoxNodePointer,							true),
			ZEML_LIST_PROPERTY("RelativePosition",	RelativePositionValue,		ZE_VRT_VECTOR3,		true),
			ZEML_LIST_PROPERTY("RelativeRotation",	RelativeRotationValue,		ZE_VRT_QUATERNION,	true),
			ZEML_LIST_PROPERTY("RelativeScale",		RelativeScaleValue,			ZE_VRT_VECTOR3,		true),
			ZEML_LIST_NODE("PhysicalJoint",			PhysicalJointNodePointer,						true),
			ZEML_LIST_NODE("PhysicalBody",			PhysicalBodyNodePointer,						false)
		};

		if (!NodeReader->ReadPropertyList(BoneList, 8))
			return false;

		ZEModelResourceBone* Bone = Model->Bones.Add();

		strncpy(Bone->Name, NameValue.GetString(), ZE_MDLF_MAX_NAME_SIZE);
		Bone->ParentBone = ParentBoneValue;
		Bone->RelativePosition = RelativePositionValue;
		Bone->RelativeRotation = RelativeRotationValue;
		Bone->RelativeScale = RelativeScaleValue;

		NodeReader->SeekPointer(BoundingBoxNodePointer);

		ZEVariant BBoxMinValue, BBoxMaxValue;

		ZEMLSerialListItem BoundingBoxList[] = 
		{
			ZEML_LIST_PROPERTY("Min", BBoxMinValue, ZE_VRT_VECTOR3,		true),
			ZEML_LIST_PROPERTY("Max", BBoxMaxValue, ZE_VRT_VECTOR3,		true)
		};

		if(!NodeReader->ReadPropertyList(BoundingBoxList, 2))
			return false;

		Bone->BoundingBox.Min = BBoxMinValue;
		Bone->BoundingBox.Max = BBoxMaxValue;

		memset(&Bone->PhysicalJoint, 0, sizeof(ZEModelResourcePhysicalJoint));

		NodeReader->SeekPointer(PhysicalJointNodePointer);
		ReadPhysicalJointFromFile(&Bone->PhysicalJoint, NodeReader);

		memset(&Bone->PhysicalBody, 0, sizeof(ZEModelResourcePhysicalBody));

		if (PhysicalBodyNodePointer != -1)
		{
			NodeReader->SeekPointer(PhysicalBodyNodePointer);
			ReadPhysicalBodyFromFile(&Bone->PhysicalBody, NodeReader);
		}

		if (NodeReader->GetItemName() != "Bone")
			NodeReader->Read();
	}

	for (ZESize I = 0; I < Model->Bones.GetCount(); I++)
	{
		if (Model->Bones[I].ParentBone == -1)
		{
			ProcessBones(Model, &Model->Bones[I], (ZEInt)I);
		}
	}

	NodeReader->GoToCurrentPointer();

	return true;
}

static bool ReadAnimationsFromFile(ZEModelResource* Model, ZEMLSerialReader* NodeReader)
{

	while (NodeReader->GetItemName() == "Animations" || NodeReader->GetItemName() == "Animation")
	{
		if(NodeReader->GetItemName() == "Animations")
			NodeReader->Read();

		ZEVariant NameValue, BoneKeyCountValue, MeshKeyCountValue;
		ZEMLSerialPointer FramesDataPointer;

		ZEMLSerialListItem AnimationList[] = 
		{
			ZEML_LIST_PROPERTY("Name",			NameValue,			ZE_VRT_STRING,		true),
			ZEML_LIST_PROPERTY("BoneKeyCount",	BoneKeyCountValue,	ZE_VRT_INTEGER_32,	true),
			ZEML_LIST_PROPERTY("MeshKeyCount",	MeshKeyCountValue,	ZE_VRT_INTEGER_32,	true),
			ZEML_LIST_DATA("Frames",			FramesDataPointer,						true)
		};

		if (!NodeReader->ReadPropertyList(AnimationList, 4))
			return false;

		ZEModelResourceAnimation* Animation = Model->Animations.Add();

		strncpy(Animation->Name, NameValue.GetString(), ZE_MDLF_MAX_NAME_SIZE);
		
		ZEInt BoneKeyCount = BoneKeyCountValue;
		ZEInt MeshKeyCount = MeshKeyCountValue;
		ZEInt FrameKeyCount = BoneKeyCount + MeshKeyCount;

		NodeReader->SeekPointer(FramesDataPointer);
		Animation->Frames.SetCount(((ZESize)NodeReader->GetDataSize() / sizeof(ZEModelResourceAnimationKey)) / (ZESize)FrameKeyCount);

		for (ZESize I = 0; I < Animation->Frames.GetCount(); I++)
		{
			ZEModelResourceAnimationFrame* CurrentAnimationFrame = &Animation->Frames[I];

			CurrentAnimationFrame->BoneKeys.SetCount(BoneKeyCount);

			if (BoneKeyCount != 0)
				NodeReader->GetData(CurrentAnimationFrame->BoneKeys.GetCArray(), BoneKeyCount * sizeof(ZEModelResourceAnimationKey), (I * FrameKeyCount) * sizeof(ZEModelResourceAnimationKey));

			CurrentAnimationFrame->MeshKeys.SetCount(MeshKeyCount);

			if (MeshKeyCount != 0)
				NodeReader->GetData(CurrentAnimationFrame->MeshKeys.GetCArray(), MeshKeyCount * sizeof(ZEModelResourceAnimationKey), (I * (FrameKeyCount + BoneKeyCount)) * sizeof(ZEModelResourceAnimationKey));
		}

		if (NodeReader->GetItemName() != "Animation")
			NodeReader->Read();
	}

	NodeReader->GoToCurrentPointer();

	return true;
}

static bool ReadModelFromFile(ZEModelResource* Model, ZEFile* ResourceFile)
{

	ZEMLSerialReader NodeReader(ResourceFile);
	
	if (!NodeReader.Read())
		return false;

	if (NodeReader.GetItemName() != "ZEModel")
		return false;

	ZEMLSerialPointer BonesNodePointer, MeshesNodePointer, MaterialsNodePointer, AnimationsNodePointer;

	ZEMLSerialListItem ModelList[] = 
	{
		ZEML_LIST_NODE("Bones",			BonesNodePointer,		false),
		ZEML_LIST_NODE("Meshes",		MeshesNodePointer,		false),
		ZEML_LIST_NODE("Animations",	AnimationsNodePointer,	false),
		ZEML_LIST_NODE("Materials",		MaterialsNodePointer,	true)
	};

	if (!NodeReader.ReadPropertyList(ModelList, 4))
		return false;

	if (BonesNodePointer != -1)
	{
		NodeReader.SeekPointer(BonesNodePointer);

		if (!ReadBonesFromFile(Model, &NodeReader))
			return false;
	}

	if (MeshesNodePointer != -1)
	{
		NodeReader.SeekPointer(MeshesNodePointer);

		if (!ReadMeshesFromFile(Model, &NodeReader))
			return false;
	}

	if (AnimationsNodePointer != -1)
	{
		NodeReader.SeekPointer(AnimationsNodePointer);

		if (!ReadAnimationsFromFile(Model, &NodeReader))
			return false;
	}

	NodeReader.SeekPointer(MaterialsNodePointer);

	if (!ReadMaterialsFromFile(Model, &NodeReader))
		return false;
	
	return true;
}

const char* ZEModelResource::GetResourceType() const
{
	return "Model";
}


ZEModelResource* ZEModelResource::LoadSharedResource(const ZEString& FileName)
{
	ZEString NewPath = ConstructResourcePath(FileName);

	ZEModelResource* Resource = (ZEModelResource*)zeResources->GetResource(NewPath);
	
	if (Resource != NULL)
		return Resource;
	else
	{
		Resource = LoadResource(NewPath);
		if (Resource != NULL)
		{
			Resource->Shared = true;
			Resource->Cached = false;
			Resource->ReferenceCount = 1;
			zeResources->AddResource(Resource);
			return Resource;
		}
		else
			return NULL;
	}
}

void ZEModelResource::CacheResource(const ZEString& FileName)
{
	ZEString NewPath = ConstructResourcePath(FileName);

	ZEModelResource* Resource = (ZEModelResource*)zeResources->GetResource(NewPath);
	if (Resource != NULL)
		Resource->Cached = true;
	else
	{
		Resource = LoadResource(NewPath);
		Resource->Cached = true;
		Resource->ReferenceCount = 0;
		zeResources->AddResource(Resource);
	}
}

ZEModelResource* ZEModelResource::LoadResource(const ZEString& FileName)
{
	ZEString NewPath = ConstructResourcePath(FileName);

	bool Result;
	ZEFile ResourceFile;

	Result = ResourceFile.Open(NewPath, ZE_FOM_READ, ZE_FCM_NONE);
	if (Result)
	{
		ZEModelResource* NewResource = new ZEModelResource();
		NewResource->SetFileName(NewPath);
		NewResource->Cached = false;
		NewResource->ReferenceCount = 0;
		if (!ReadModelFromFile(NewResource, &ResourceFile))
		{
			zeError("Can not load model file. (FileName : \"%s\")", FileName.ToCString());
			ResourceFile.Close();
			delete NewResource;
			return NULL;
		}
		else
		{
			ResourceFile.Close();
			return NewResource;
		}
	}
	else
	{
		zeError("Model file does not exists. (FileName : \"%s\")", FileName.ToCString());
		return NULL;
	}
}

ZEModelResource::~ZEModelResource()
{
	for (ZESize I = 0; I < TextureResources.GetCount(); I++)
		TextureResources[I]->Release();
}
