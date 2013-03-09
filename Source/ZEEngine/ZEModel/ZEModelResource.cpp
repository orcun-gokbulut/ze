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

ZE_OBJECT_IMPL(ZEModelResource)

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

const ZETexture2D* ZEModelResource::ManageModelMaterialTextures(const ZEString& FileName)
{
	if (strncmp(FileName, "", ZE_MDLF_MAX_FILENAME_SIZE) == 0)
		return NULL;

	for (ZESize I = 0; I < TextureResources.GetCount(); I++)
	{
		//if (strnicmp(TextureResources[I]->GetFileName(), FileName, ZE_MDLF_MAX_FILENAME_SIZE) == 0)
		if (TextureResources[I]->GetFileName() == FileName)
			return TextureResources[I]->GetTexture();
	}

	ZETexture2DResource* NewTextureResource = ZETexture2DResource::LoadSharedResource(FileName);
	if (NewTextureResource == NULL)
	{
		zeError("Can not load texture file. (FileName : \"%s\")", FileName.GetValue());
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

bool ZEModelResource::ReadMaterials(ZEMLSerialReader* NodeReader)
{
	ZESize SubItemCount = (ZESize)NodeReader->GetSubItemCount();
	for (ZESize I = 0; I < SubItemCount; I++)
	{
		if (!NodeReader->Read())
		{
			zeError("Can not read ZEML node.");
			return false;
		}

		if (NodeReader->GetItemType() != ZEML_IT_NODE || NodeReader->GetItemName() != "Material")
			continue;

		ZEValue NameValue, FilePathValue;

		ZEMLSerialListItem MaterialList[] = 
		{
			ZEML_LIST_PROPERTY("Name",		NameValue,		ZE_VRT_STRING,	true),
			ZEML_LIST_PROPERTY("FilePath",	FilePathValue,	ZE_VRT_STRING,	true)
		};

		if (!NodeReader->ReadPropertyList(MaterialList, 2))
			return false;

		ZEString MaterialPath = ZEFileInfo::GetParentDirectory(this->GetFileName()) + ZEPathUtils::GetSeperator() + FilePathValue.GetString();

		if (!ZEFileInfo::IsFile(MaterialPath))
			return false;

		ZEFixedMaterial* CurrentMaterial = ZEFixedMaterial::CreateInstance();
		CurrentMaterial->ReadFromFile(MaterialPath);
		Materials.Add(CurrentMaterial);
	}

	NodeReader->GoToCurrentPointer();

	return true;
}

bool ZEModelResource::ReadPhysicalBody(ZEModelResourcePhysicalBody* Body, ZEMLSerialReader* NodeReader)
{

	ZEValue Enabled, Type, IsKinematic, Mass, MassCenter, LinearDamping, AngularDamping;
	ZEMLSerialPointer PhysicalShapesNodePointer;

	ZEMLSerialListItem PhysicalBodyList[] = 
	{
		ZEML_LIST_PROPERTY("Enabled",					Enabled,					ZE_VRT_BOOLEAN,		true),
		ZEML_LIST_PROPERTY("Type",						Type,						ZE_VRT_INTEGER_32,	true),
		ZEML_LIST_PROPERTY("IsKinematic",				IsKinematic,				ZE_VRT_BOOLEAN,		true),
		ZEML_LIST_PROPERTY("Mass",						Mass,						ZE_VRT_FLOAT,		true),
		ZEML_LIST_PROPERTY("MassCenter",				MassCenter,					ZE_VRT_VECTOR3,		true),
		ZEML_LIST_PROPERTY("LinearDamping",				LinearDamping,				ZE_VRT_FLOAT,		true),
		ZEML_LIST_PROPERTY("AngularDamping",			AngularDamping,				ZE_VRT_FLOAT,		true),
		ZEML_LIST_NODE("PhysicalShapes",				PhysicalShapesNodePointer,						true)
	};

	if (!NodeReader->ReadPropertyList(PhysicalBodyList, 8))
		return false;

	Body->Enabled = Enabled;
	Body->Type = (ZEModelResourcePhysicalBodyType)Type.GetInt32();
	Body->IsKinematic = IsKinematic;
	Body->Mass = Mass;
	Body->MassCenter = MassCenter;
	Body->LinearDamping = LinearDamping;
	Body->AngularDamping = AngularDamping;

	NodeReader->SeekPointer(PhysicalShapesNodePointer);

	ZESize SubItemCount = (ZESize)NodeReader->GetSubItemCount();
	for (ZESize I = 0; I < SubItemCount; I++)
	{
		if (!NodeReader->Read())
		{
			zeError("Can not read ZEML node.");
			return false;
		}

		if (NodeReader->GetItemType() != ZEML_IT_NODE || NodeReader->GetItemName() != "PhysicalShape")
			continue;

		ZEValue ShapePosition, ShapeRotation, ShapeRestitution, ShapeDynamicFriction, ShapeStaticFriction, ShapeType, UserDefinedProperties;
		ZEMLSerialPointer ShapeNodePointer;

		ZEMLSerialListItem PhysicalShapeList [] = 
		{
			ZEML_LIST_PROPERTY("Position",					ShapePosition,			ZE_VRT_VECTOR3,		true),
			ZEML_LIST_PROPERTY("Rotation",					ShapeRotation,			ZE_VRT_QUATERNION,	true),
			ZEML_LIST_PROPERTY("Restitution",				ShapeRestitution,		ZE_VRT_FLOAT,		true),
			ZEML_LIST_PROPERTY("DynamicFriction",			ShapeDynamicFriction,	ZE_VRT_FLOAT,		true),
			ZEML_LIST_PROPERTY("StaticFriction",			ShapeStaticFriction,	ZE_VRT_FLOAT,		true),
			ZEML_LIST_PROPERTY("UserDefinedProperties",		UserDefinedProperties,	ZE_VRT_STRING,		false),
			ZEML_LIST_PROPERTY("Type",						ShapeType,				ZE_VRT_INTEGER_32,	true),
			ZEML_LIST_NODE("Shape",							ShapeNodePointer,							true)
		};

		if (!NodeReader->ReadPropertyList(PhysicalShapeList, 8))
			return false;

		ZEModelResourcePhysicalShape* Shape = Body->Shapes.Add();

		Shape->Position = ShapePosition;
		Shape->Rotation = ShapeRotation;
		Shape->Restitution = ShapeRestitution;
		Shape->DynamicFriction = ShapeDynamicFriction;
		Shape->StaticFriction = ShapeStaticFriction;
		Shape->Type = (ZEModelResourcePhysicalShapeType)ShapeType.GetInt32();

		if (UserDefinedProperties.GetType() == ZE_VRT_STRING)
			Shape->UserDefinedProperties = UserDefinedProperties.GetString();

		NodeReader->SeekPointer(ShapeNodePointer);

		ZEValue ShapeHeight, ShapeWidth, ShapeLength, ShapeRadius;
		ZEMLSerialPointer VerticesDataPointer;

		ZEMLSerialListItem ActualShapeList [] = 
		{
			ZEML_LIST_PROPERTY("Height",	ShapeHeight,			ZE_VRT_FLOAT,	false),
			ZEML_LIST_PROPERTY("Width",		ShapeWidth,				ZE_VRT_FLOAT,	false),
			ZEML_LIST_PROPERTY("Length",	ShapeLength,			ZE_VRT_FLOAT,	false),
			ZEML_LIST_PROPERTY("Radius",	ShapeRadius,			ZE_VRT_FLOAT,	false),
			ZEML_LIST_DATA("Vertices",		VerticesDataPointer,					false)
		};

		NodeReader->ReadPropertyList(ActualShapeList, 5);

		switch (Shape->Type)
		{
			case ZE_MRPST_BOX:
			{
				Shape->Box.Width = ShapeWidth;
				Shape->Box.Height = ShapeHeight;
				Shape->Box.Length = ShapeLength;
				break;
			}
			case ZE_MRPST_SPHERE:
			{
				Shape->Sphere.Radius = ShapeRadius;
				break;
			}
			case ZE_MRPST_CAPSULE:
			{
				Shape->Capsule.Radius = ShapeRadius;
				Shape->Capsule.Height = ShapeHeight;
				break;
			}
			case ZE_MRPST_CYLINDER:
			{
				Shape->Cylinder.Radius = ShapeRadius;
				Shape->Cylinder.Height = ShapeHeight;
				break;
			}
			case ZE_MRPST_CONVEX:
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

bool ZEModelResource::ReadMeshes(ZEMLSerialReader* NodeReader)
{

	ZESize SubItemCount = (ZESize)NodeReader->GetSubItemCount();
	for (ZESize I = 0; I < SubItemCount; I++)
	{
		if (!NodeReader->Read())
		{
			zeError("Can not read ZEML node.");
			return false;
		}

		if (NodeReader->GetItemType() != ZEML_IT_NODE || NodeReader->GetItemName() != "Mesh")
			continue;

		ZEValue NameValue, PositionValue, RotationValue, ScaleValue, IsVisibleValue, IsSkinnedValue, UserDefinedPropertiesValue;
		ZEMLSerialPointer BoundingBoxNodePointer, PhysicalBodyNodePointer, LODsNodePointer;

		ZEMLSerialListItem MeshList[] = 
		{
			ZEML_LIST_PROPERTY("Name",						NameValue,						ZE_VRT_STRING,		true),
			ZEML_LIST_NODE("BoundingBox",					BoundingBoxNodePointer,								true),
			ZEML_LIST_PROPERTY("Position",					PositionValue,					ZE_VRT_VECTOR3,		true),
			ZEML_LIST_PROPERTY("Rotation",					RotationValue,					ZE_VRT_QUATERNION,	true),
			ZEML_LIST_PROPERTY("Scale",						ScaleValue,						ZE_VRT_VECTOR3,		true),
			ZEML_LIST_PROPERTY("IsVisible",					IsVisibleValue,					ZE_VRT_BOOLEAN,		true),
			ZEML_LIST_PROPERTY("IsSkinned",					IsSkinnedValue,					ZE_VRT_BOOLEAN,		true),
			ZEML_LIST_PROPERTY("UserDefinedProperties",		UserDefinedPropertiesValue,		ZE_VRT_STRING,		false),
			ZEML_LIST_NODE("PhysicalBody",					PhysicalBodyNodePointer,							false),
			ZEML_LIST_NODE("LODs",							LODsNodePointer,									true)
		};

		if (!NodeReader->ReadPropertyList(MeshList, 10))
			return false;

		ZEModelResourceMesh* Mesh = Meshes.Add();

		strncpy(Mesh->Name, NameValue.GetString(), ZE_MDLF_MAX_NAME_SIZE);
		Mesh->Position = PositionValue;
		Mesh->Rotation = RotationValue;
		Mesh->Scale = ScaleValue;
		Mesh->IsVisible = IsVisibleValue;
		Mesh->IsSkinned = IsSkinnedValue;

		if (UserDefinedPropertiesValue.GetType() == ZE_VRT_STRING)
			Mesh->UserDefinedProperties = UserDefinedPropertiesValue.GetString();

		NodeReader->SeekPointer(BoundingBoxNodePointer);

		ZEValue BBoxMinValue, BBoxMaxValue;

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

		ZESize SubItemCount = (ZESize)NodeReader->GetSubItemCount();
		for (ZESize J = 0; J < SubItemCount; J++)
		{
			if (!NodeReader->Read())
			{
				zeError("Can not read ZEML node.");
				return false;
			}

			if (NodeReader->GetItemType() != ZEML_IT_NODE || NodeReader->GetItemName() != "LOD")
				continue;

			ZEValue LODLevelValue, MaterialIdValue;
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

		}

		CalculateBoundingBox(Mesh);

		if (PhysicalBodyNodePointer != -1)
		{
			NodeReader->SeekPointer(PhysicalBodyNodePointer);
			ReadPhysicalBody(&Mesh->PhysicalBody, NodeReader);
		}
		else
		{
			Mesh->PhysicalBody.Shapes.Clear();
			memset(&Mesh->PhysicalBody, 0, sizeof(ZEModelResourcePhysicalBody));
		}

	}

	NodeReader->GoToCurrentPointer();

	return true;
}

bool ZEModelResource::ReadPhysicalJoint(ZEModelResourcePhysicalJoint* Joint, ZEMLSerialReader* NodeReader)
{

	ZEValue Enabled, Body1Id, Body2Id;
	ZEValue UseGlobalAnchorAxis, LocalAnchor1, LocalAxis1, LocalAnchor2, LocalAxis2, GlobalAnchor, GlobalAxis;
	ZEValue JointType, CollideBodies, Breakable, BreakForce, BreakTorque;
	ZEValue XMotion, YMotion, ZMotion, LinearLimitValue, LinearLimitRestitution, LinearLimitSpring, LinearLimitDamping;
	ZEValue TwistMotion, TwistLowLimitValue, TwistLowLimitRestitution, TwistLowLimitSpring, TwistLowLimitDamping, TwistHighLimitValue, TwistHighLimitRestitution, TwistHighLimitSpring, TwistHighLimitDamping;
	ZEValue Swing1Motion, Swing1LimitValue, Swing1LimitRestitution, Swing1LimitSpring, Swing1LimitDamping;
	ZEValue Swing2Motion, Swing2LimitValue, Swing2LimitRestitution, Swing2LimitSpring, Swing2LimitDamping;
	ZEValue LinearXMotor, LinearXMotorForce, LinearXMotorSpring, LinearXMotorDamper;
	ZEValue LinearYMotor, LinearYMotorForce, LinearYMotorSpring, LinearYMotorDamper;
	ZEValue LinearZMotor, LinearZMotorForce, LinearZMotorSpring, LinearZMotorDamper;
	ZEValue AngularSwingMotor, AngularSwingMotorForce, AngularSwingMotorSpring, AngularSwingMotorDamper;
	ZEValue AngularTwistMotor, AngularTwistMotorForce, AngularTwistMotorSpring, AngularTwistMotorDamper;
	ZEValue AngularSlerpMotor, AngularSlerpMotorForce, AngularSlerpMotorSpring, AngularSlerpMotorDamper;
	ZEValue MotorTargetPosition, MotorTargetVelocity, MotorTargetOrientation, MotorTargetAngularVelocity;

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
		ZEML_LIST_PROPERTY("MotorTargetOrientation",		MotorTargetOrientation,			ZE_VRT_QUATERNION,	true),
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

void ZEModelResource::ProcessBones(ZEModelResourceBone* Bone, ZEInt BoneId)
{
	ZEMatrix4x4::CreateOrientation(Bone->RelativeTransform, Bone->RelativePosition, Bone->RelativeRotation, Bone->RelativeScale);

	if (Bone->ParentBone != -1)
	{

		ZEMatrix4x4::Multiply(Bone->ForwardTransform, Bones[(ZESize)Bone->ParentBone].ForwardTransform, Bone->RelativeTransform);
		ZEMatrix4x4::Inverse(Bone->InverseTransform, Bone->ForwardTransform);
	}
	else
	{
		Bone->ForwardTransform = Bone->RelativeTransform;
		ZEMatrix4x4::Inverse(Bone->InverseTransform, Bone->ForwardTransform);
	}

	for (ZESize I = 0; I < Bones.GetCount(); I++)
	{
		if (Bones[I].ParentBone == BoneId)
		{
			ProcessBones(&Bones[I], (ZEInt)I);
		}
	}
}

bool ZEModelResource::ReadBones(ZEMLSerialReader* NodeReader)
{

	ZESize SubItemCount = (ZESize)NodeReader->GetSubItemCount();
	for (ZESize I = 0; I < SubItemCount; I++)
	{
		if (!NodeReader->Read())
		{
			zeError("Can not read ZEML node.");
			return false;
		}

		if (NodeReader->GetItemType() != ZEML_IT_NODE || NodeReader->GetItemName() != "Bone")
			continue;

		ZEValue NameValue, ParentBoneValue, RelativePositionValue, RelativeRotationValue, RelativeScaleValue, UserDefinedPropertiesValue;
		ZEMLSerialPointer BoundingBoxNodePointer, PhysicalJointNodePointer, PhysicalBodyNodePointer;

		ZEMLSerialListItem BoneList[] = 
		{
			ZEML_LIST_PROPERTY("Name",					NameValue,					ZE_VRT_STRING,		true),
			ZEML_LIST_PROPERTY("ParentBone",			ParentBoneValue,			ZE_VRT_INTEGER_32,	true),
			ZEML_LIST_NODE("BoundingBox",				BoundingBoxNodePointer,							true),
			ZEML_LIST_PROPERTY("RelativePosition",		RelativePositionValue,		ZE_VRT_VECTOR3,		true),
			ZEML_LIST_PROPERTY("RelativeRotation",		RelativeRotationValue,		ZE_VRT_QUATERNION,	true),
			ZEML_LIST_PROPERTY("RelativeScale",			RelativeScaleValue,			ZE_VRT_VECTOR3,		true),
			ZEML_LIST_PROPERTY("UserDefinedProperties",	UserDefinedPropertiesValue,	ZE_VRT_STRING,		false),
			ZEML_LIST_NODE("PhysicalJoint",				PhysicalJointNodePointer,						true),
			ZEML_LIST_NODE("PhysicalBody",				PhysicalBodyNodePointer,						false)
		};

		if (!NodeReader->ReadPropertyList(BoneList, 9))
			return false;

		ZEModelResourceBone* Bone = Bones.Add();

		strncpy(Bone->Name, NameValue.GetString(), ZE_MDLF_MAX_NAME_SIZE);
		Bone->ParentBone = ParentBoneValue;
		Bone->RelativePosition = RelativePositionValue;
		Bone->RelativeRotation = RelativeRotationValue;
		Bone->RelativeScale = RelativeScaleValue;

		if (UserDefinedPropertiesValue.GetType() == ZE_VRT_STRING)
			Bone->UserDefinedProperties = UserDefinedPropertiesValue.GetString();

		NodeReader->SeekPointer(BoundingBoxNodePointer);

		ZEValue BBoxMinValue, BBoxMaxValue;

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
		ReadPhysicalJoint(&Bone->PhysicalJoint, NodeReader);

		memset(&Bone->PhysicalBody, 0, sizeof(ZEModelResourcePhysicalBody));

		if (PhysicalBodyNodePointer != -1)
		{
			NodeReader->SeekPointer(PhysicalBodyNodePointer);
			ReadPhysicalBody(&Bone->PhysicalBody, NodeReader);
		}
	}

	for (ZESize I = 0; I < Bones.GetCount(); I++)
	{
		if (Bones[I].ParentBone == -1)
		{
			ProcessBones(&Bones[I], (ZEInt)I);
		}
	}

	NodeReader->GoToCurrentPointer();

	return true;
}

bool ZEModelResource::ReadHelpers(ZEMLSerialReader* NodeReader)
{
	ZESize SubItemCount = (ZESize)NodeReader->GetSubItemCount();
	for (ZESize I = 0; I < SubItemCount; I++)
	{
		if (!NodeReader->Read())
		{
			zeError("Can not read ZEML node.");
			return false;
		}

		if (NodeReader->GetItemType() != ZEML_IT_NODE || NodeReader->GetItemName() != "Helper")
			continue;

		ZEValue NameValue, OwnerTypeValue, OwnerIdValue, PositionValue, RotationValue, ScaleValue, UserDefinedPropertiesValue;

		ZEMLSerialListItem HelperList[] = 
		{
			ZEML_LIST_PROPERTY("Name",						NameValue,						ZE_VRT_STRING,		true),
			ZEML_LIST_PROPERTY("OwnerType",					OwnerTypeValue,					ZE_VRT_INTEGER_32,	true),
			ZEML_LIST_PROPERTY("OwnerId",					OwnerIdValue,					ZE_VRT_INTEGER_32,	true),
			ZEML_LIST_PROPERTY("Position",					PositionValue,					ZE_VRT_VECTOR3,		true),
			ZEML_LIST_PROPERTY("Rotation",					RotationValue,					ZE_VRT_QUATERNION,	true),
			ZEML_LIST_PROPERTY("Scale",						ScaleValue,						ZE_VRT_VECTOR3,		true),
			ZEML_LIST_PROPERTY("UserDefinedProperties",		UserDefinedPropertiesValue,		ZE_VRT_STRING,		false)
		};

		if (!NodeReader->ReadPropertyList(HelperList, 7))
			return false;

		ZEModelResourceHelper* Helper = Helpers.Add();

		memset(Helper, 0, sizeof(ZEModelResourceHelper));

		strncpy(Helper->Name, NameValue.GetString(), ZE_MDLF_MAX_NAME_SIZE);
		Helper->Position = PositionValue;
		Helper->Rotation = RotationValue;
		Helper->Scale = ScaleValue;

		if (UserDefinedPropertiesValue.GetType() == ZE_VRT_STRING)
			Helper->UserDefinedProperties = UserDefinedPropertiesValue.GetString();

		Helper->OwnerType = (ZEModelResourceHelperOwnerType)OwnerTypeValue.GetInt32();
		Helper->OwnerId = OwnerIdValue;

		switch (Helper->OwnerType)
		{
			case ZE_MRHOT_MESH:
				{
					Helper->OwnerMesh = &Meshes[Helper->OwnerId];
					break;
				}
			case ZE_MRHOT_BONE:
				{
					Helper->OwnerBone = &Bones[Helper->OwnerId];
					break;
				}
			default:
				break;
		}
	}

	NodeReader->GoToCurrentPointer();

	return true;
}

bool ZEModelResource::ReadAnimations(ZEMLSerialReader* NodeReader)
{

	ZESize SubItemCount = (ZESize)NodeReader->GetSubItemCount();
	for (ZESize I = 0; I < SubItemCount; I++)
	{
		if (!NodeReader->Read())
		{
			zeError("Can not read ZEML node.");
			return false;
		}

		if (NodeReader->GetItemType() != ZEML_IT_NODE || NodeReader->GetItemName() != "Animation")
			continue;

		ZEValue NameValue, BoneKeyCountValue, MeshKeyCountValue;
		ZEMLSerialPointer FramesDataPointer;

		ZEMLSerialListItem AnimationList[] = 
		{
			ZEML_LIST_PROPERTY("Name",			NameValue,			ZE_VRT_STRING,					true),
			ZEML_LIST_PROPERTY("BoneKeyCount",	BoneKeyCountValue,	ZE_VRT_UNSIGNED_INTEGER_32,		true),
			ZEML_LIST_PROPERTY("MeshKeyCount",	MeshKeyCountValue,	ZE_VRT_UNSIGNED_INTEGER_32,		true),
			ZEML_LIST_DATA("Frames",			FramesDataPointer,									true)
		};

		if (!NodeReader->ReadPropertyList(AnimationList, 4))
			return false;

		ZEModelResourceAnimation* Animation = Animations.Add();

		strncpy(Animation->Name, NameValue.GetString(), ZE_MDLF_MAX_NAME_SIZE);
		
		ZEUInt BoneKeyCount = BoneKeyCountValue;
		ZEUInt MeshKeyCount = MeshKeyCountValue;
		ZEUInt FrameKeyCount = BoneKeyCount + MeshKeyCount;

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

	}

	NodeReader->GoToCurrentPointer();

	return true;
}

bool ZEModelResource::ReadModelFromFile(ZEFile* ResourceFile)
{

	ZEMLSerialReader NodeReader(ResourceFile);
	
	if (!NodeReader.Read())
		return false;

	if (NodeReader.GetItemName() != "ZEModel")
		return false;

	ZEMLSerialPointer BonesNodePointer, MeshesNodePointer, HelpersNodePointer, MaterialsNodePointer, AnimationsNodePointer;

	ZEMLSerialListItem ModelList[] = 
	{
		ZEML_LIST_NODE("Bones",			BonesNodePointer,		false),
		ZEML_LIST_NODE("Meshes",		MeshesNodePointer,		false),
		ZEML_LIST_NODE("Helpers",		HelpersNodePointer,		false),
		ZEML_LIST_NODE("Animations",	AnimationsNodePointer,	false),
		ZEML_LIST_NODE("Materials",		MaterialsNodePointer,	true)
	};

	if (!NodeReader.ReadPropertyList(ModelList, 5))
		return false;

	if (BonesNodePointer != -1)
	{
		NodeReader.SeekPointer(BonesNodePointer);

		if (!ReadBones(&NodeReader))
			return false;
	}

	if (MeshesNodePointer != -1)
	{
		NodeReader.SeekPointer(MeshesNodePointer);

		if (!ReadMeshes(&NodeReader))
			return false;
	}

	if (HelpersNodePointer != -1)
	{
		NodeReader.SeekPointer(HelpersNodePointer);

		if (!ReadHelpers(&NodeReader))
			return false;
	}

	if (AnimationsNodePointer != -1)
	{
		NodeReader.SeekPointer(AnimationsNodePointer);

		if (!ReadAnimations(&NodeReader))
			return false;
	}

	NodeReader.SeekPointer(MaterialsNodePointer);

	if (!ReadMaterials(&NodeReader))
		return false;
	
	return true;
}

const char* ZEModelResource::GetResourceType() const
{
	return "Model";
}

const ZESmartArray<ZETexture2DResource*>& ZEModelResource::GetTextures() const
{
	return TextureResources;
}

const ZEArray<ZEMaterial*>& ZEModelResource::GetMaterials() const
{
	return Materials;
}

const ZEArray<ZEModelResourceBone>& ZEModelResource::GetBones() const
{
	return Bones;
}

const ZEArray<ZEModelResourceMesh>& ZEModelResource::GetMeshes() const
{
	return Meshes;
}

const ZEArray<ZEModelResourceHelper>& ZEModelResource::GetHelpers() const
{
	return Helpers;
}

const ZEArray<ZEModelResourceAnimation>& ZEModelResource::GetAnimations() const
{
	return Animations;
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
		if (!NewResource->ReadModelFromFile(&ResourceFile))
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
