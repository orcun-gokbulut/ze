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
#include "ZEGraphics/ZEGRVertexBuffer.h"
#include "ZERenderer/ZERNFixedMaterial.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZETexture/ZETexture2DResource.h"
#include "ZEFile/ZEFileInfo.h"
#include "ZEML/ZEMLReader.h"

#include <memory.h>
#include <string.h>
#include <float.h>
#include "ZEGraphics/ZEGRVertexLayout.h"

ZEGRVertexLayout ZEModelVertex::VertexLayout;
ZEGRVertexLayout ZESkinnedModelVertex::VertexLayout;

ZEGRVertexLayout* ZEModelVertex::GetVertexLayout()
{
	if (VertexLayout.GetElementCount() == 0)
	{
		static ZEGRVertexElement ElementArray[] = 
		{
			{ZEGR_VES_POSITION,	0, ZEGR_VET_FLOAT3, 0, 0,  ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_NORMAL,	0, ZEGR_VET_FLOAT3, 0, 12, ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_TANGENT,	0, ZEGR_VET_FLOAT3, 0, 24, ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_TEXCOORD,	0, ZEGR_VET_FLOAT2, 0, 36, ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_BINORMAL,	0, ZEGR_VET_FLOAT3, 0, 44, ZEGR_VU_PER_VERTEX, 0}
		};

		VertexLayout.SetElements(ElementArray, 4);
	}

	return &VertexLayout;
}


ZEGRVertexLayout* ZESkinnedModelVertex::GetVertexLayout()
{
	if (VertexLayout.GetElementCount() == 0)
	{
		static ZEGRVertexElement ElementArray[] = 
		{
			{ZEGR_VES_POSITION,	0, ZEGR_VET_FLOAT3, 0, 0,  ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_NORMAL,	0, ZEGR_VET_FLOAT3, 0, 12, ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_TANGENT,	0, ZEGR_VET_FLOAT3, 0, 24, ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_BINORMAL,	0, ZEGR_VET_FLOAT3, 0, 36, ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_TEXCOORD,	0, ZEGR_VET_FLOAT2, 0, 48, ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_BLEND_INDEXES, 0, ZEGR_VET_UINT4,	 0, 56, ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_BLEND_WEIGHTS, 0, ZEGR_VET_FLOAT4, 0, 60, ZEGR_VU_PER_VERTEX, 0},
		};

		VertexLayout.SetElements(ElementArray, 4);
	}

	return &VertexLayout;
}

//////////////////////////////////////////////////////////////// READ ////////////////////////////////////////////////////////////////
const ZEGRTexture2D* ZEModelResource::ManageModelMaterialTextures(const ZEString& FileName)
{
	if (strncmp(FileName, "", ZE_MDLF_MAX_FILENAME_SIZE) == 0)
		return NULL;

	for (ZESize I = 0; I < TextureResources.GetCount(); I++)
	{
		//if (strnicmp(TextureResources[I]->GetFileName(), FileName, ZE_MDLF_MAX_FILENAME_SIZE) == 0)
		if (TextureResources[I]->GetFileName() == FileName)
			return TextureResources[I]->GetTexture2D();
	}

	ZETexture2DResource* NewTextureResource = ZETexture2DResource::LoadSharedResource(FileName);
	if (NewTextureResource == NULL)
	{
		zeError("Can not load texture file. (FileName : \"%s\")", FileName.GetValue());
		return NULL;
	}
	TextureResources.Add(NewTextureResource);
	return NewTextureResource->GetTexture2D();
}

bool ZEModelResource::ReadMaterials(ZEMLReaderNode* MaterialsNode)
{
	if (MaterialsNode == NULL)
		return false;

	if (!MaterialsNode->IsValid())
		return false;

	ZESize SubNodeCount = (ZESize)MaterialsNode->GetSubNodeCount("Material");

	for (ZESize I = 0; I < SubNodeCount; I++)
	{
		ZEMLReaderNode MaterialNode = MaterialsNode->GetSubNode("Material", I);

		ZEString MaterialPath = ZEFileInfo(this->GetFileName()).GetParentDirectory() + "/" + MaterialNode.ReadString("FilePath");
		
		if (!ZEFileInfo(MaterialPath).IsFile())
			return false;

		ZERNFixedMaterial* CurrentMaterial = ZERNFixedMaterial::CreateInstance();
		CurrentMaterial->ReadFromFile(MaterialPath);
		Materials.Add(CurrentMaterial);

	}

	return true;
}

bool ZEModelResource::ReadPhysicalBody(ZEModelResourcePhysicalBody* Body, ZEMLReaderNode* PhysicalBodyNode)
{
	if (PhysicalBodyNode == NULL)
		return false;

	if (!PhysicalBodyNode->IsValid())
		return false;

	Body->Enabled = PhysicalBodyNode->ReadBoolean("Enabled");
	Body->Type = (ZEModelResourcePhysicalBodyType)PhysicalBodyNode->ReadInt32("Type");
	Body->IsKinematic = PhysicalBodyNode->ReadBoolean("IsKinematic");
	Body->Mass = PhysicalBodyNode->ReadFloat("Mass");
	Body->MassCenter = PhysicalBodyNode->ReadVector3("MassCenter");
	Body->LinearDamping = PhysicalBodyNode->ReadFloat("LinearDamping");
	Body->AngularDamping = PhysicalBodyNode->ReadFloat("AngularDamping");

	ZEMLReaderNode PhysicalShapesNode = PhysicalBodyNode->GetSubNode("PhysicalShapes");

	if (!PhysicalShapesNode.IsValid())
		return false;

	ZESize SubNodeCount = PhysicalShapesNode.GetSubNodeCount("PhysicalShape");

	for (ZESize I = 0; I < SubNodeCount; I++)
	{
		ZEMLReaderNode PhysicalShapeNode = PhysicalShapesNode.GetSubNode("PhysicalShape", I);

		if (!PhysicalShapeNode.IsValid())
			return false;

		ZEModelResourcePhysicalShape* Shape = Body->Shapes.Add();
		
		Shape->Position = PhysicalShapeNode.ReadVector3("Position");
		Shape->Rotation = PhysicalShapeNode.ReadQuaternion("Rotation");
		Shape->Restitution = PhysicalShapeNode.ReadFloat("Restitution");
		Shape->DynamicFriction = PhysicalShapeNode.ReadFloat("DynamicFriction");
		Shape->StaticFriction = PhysicalShapeNode.ReadFloat("StaticFriction");
		Shape->Type = (ZEModelResourcePhysicalShapeType)PhysicalShapeNode.ReadInt32("Type");
		
		if (PhysicalShapeNode.IsPropertyExists("UserDefinedProperties"))
			Shape->UserDefinedProperties = PhysicalShapeNode.ReadString("UserDefinedProperties");

		ZEMLReaderNode ShapeNode = PhysicalShapeNode.GetSubNode("Shape");

		switch (Shape->Type)
		{
			case ZE_MRPST_BOX:
			{
				Shape->Box.Width = PhysicalShapeNode.ReadFloat("Width");
				Shape->Box.Height = PhysicalShapeNode.ReadFloat("Height");
				Shape->Box.Length = PhysicalShapeNode.ReadFloat("Length");
				break;
			}
			case ZE_MRPST_SPHERE:
			{
				Shape->Sphere.Radius = PhysicalShapeNode.ReadFloat("Radius");
				break;
			}
			case ZE_MRPST_CAPSULE:
			{
				Shape->Capsule.Radius = PhysicalShapeNode.ReadFloat("Radius");
				Shape->Capsule.Height = PhysicalShapeNode.ReadFloat("Height");
				break;
			}
			case ZE_MRPST_CYLINDER:
			{
				Shape->Cylinder.Radius = PhysicalShapeNode.ReadFloat("Radius");
				Shape->Cylinder.Height = PhysicalShapeNode.ReadFloat("Height");
				break;
			}
			case ZE_MRPST_CONVEX:
			{
				if (!PhysicalShapeNode.IsPropertyExists("Vertices"))
					return false;

				Shape->Convex.Vertices.SetCount(PhysicalShapeNode.ReadDataSize("Vertices") / sizeof(ZEVector3));
				PhysicalShapeNode.ReadData("Vertices", Shape->Convex.Vertices.GetCArray(), PhysicalShapeNode.ReadDataSize("Vertices"));
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

static void CalculateBoundingBox(ZEAABBox& BoundingBox, void* Vertices, ZESize VertexCount, bool IsSkinned)
{
	if (Vertices == NULL)
		return;

	if (IsSkinned)
	{
		for (ZESize I = 0; I < VertexCount; I++)
		{
			ZEVector3& Position = ((ZESkinnedModelVertex*)Vertices)[I].Position;

			if (Position.x < BoundingBox.Min.x) BoundingBox.Min.x = Position.x;
			if (Position.y < BoundingBox.Min.y) BoundingBox.Min.y = Position.y;
			if (Position.z < BoundingBox.Min.z) BoundingBox.Min.z = Position.z;
			if (Position.x > BoundingBox.Max.x) BoundingBox.Max.x = Position.x;
			if (Position.y > BoundingBox.Max.y) BoundingBox.Max.y = Position.y;
			if (Position.z > BoundingBox.Max.z) BoundingBox.Max.z = Position.z;
		}
	}
	else
	{
		for (ZESize I = 0; I < VertexCount; I++)
		{
			ZEVector3& Position = ((ZESkinnedModelVertex*)Vertices)[I].Position;

			if (Position.x < BoundingBox.Min.x) BoundingBox.Min.x = Position.x;
			if (Position.y < BoundingBox.Min.y) BoundingBox.Min.y = Position.y;
			if (Position.z < BoundingBox.Min.z) BoundingBox.Min.z = Position.z;
			if (Position.x > BoundingBox.Max.x) BoundingBox.Max.x = Position.x;
			if (Position.y > BoundingBox.Max.y) BoundingBox.Max.y = Position.y;
			if (Position.z > BoundingBox.Max.z) BoundingBox.Max.z = Position.z;
		}
	}
}

bool ZEModelResource::ReadMeshes(ZEMLReaderNode* MeshesNode)
{
	if (MeshesNode == NULL)
		return false;

	if (!MeshesNode->IsValid())
		return false;

	ZESize SubNodeCount = (ZESize)MeshesNode->GetSubNodeCount("Mesh");

	for (ZESize I = 0; I < SubNodeCount; I++)
	{
		ZEMLReaderNode MeshNode = MeshesNode->GetSubNode("Mesh", I);

		if (!MeshNode.IsValid())
			return false;

		ZEModelResourceMesh* Mesh = Meshes.Add();

		Mesh->Name = MeshNode.ReadString("Name");

		if (MeshNode.IsPropertyExists("ParentMesh"))
			Mesh->ParentMesh = MeshNode.ReadInt32("ParentMesh");
		else
			Mesh->ParentMesh = -1;

		Mesh->Position = MeshNode.ReadVector3("Position");
		Mesh->Rotation = MeshNode.ReadQuaternion("Rotation");
		Mesh->Scale = MeshNode.ReadVector3("Scale");
		Mesh->IsVisible = MeshNode.ReadBoolean("IsVisible");
		Mesh->IsSkinned = MeshNode.ReadBoolean("IsSkinned");
		Mesh->BoundingBox.Min = ZEVector3(FLT_MAX, FLT_MAX, FLT_MAX);
		Mesh->BoundingBox.Max = ZEVector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

		ZEMLReaderNode BoundingBoxNode = MeshNode.GetSubNode("BoundingBox");
		Mesh->BoundingBox.Max = BoundingBoxNode.ReadVector3("Max");
		Mesh->BoundingBox.Min = BoundingBoxNode.ReadVector3("Min");

		if (MeshNode.IsPropertyExists("UserDefinedProperties"))
			Mesh->UserDefinedProperties = MeshNode.ReadString("UserDefinedProperties");

		ZEMLReaderNode LODsNode = MeshNode.GetSubNode("LODs");

		if (!LODsNode.IsValid())
			return false;

		ZESize LODCount = (ZESize)LODsNode.GetSubNodeCount("LOD");

		for (ZESize I = 0; I < LODCount; I++)
		{
			ZEMLReaderNode LODNode = LODsNode.GetSubNode("LOD", I);

			if (!LODNode.IsValid())
				return false;

			ZEModelResourceMeshLOD* LOD = Mesh->LODs.Add();

			LOD->LODLevel = LODNode.ReadInt32("LODLevel");
			LOD->MaterialId = LODNode.ReadInt32("MaterialId");

			if (Mesh->IsSkinned)
			{
				if (!LODNode.IsPropertyExists("Vertices"))
					return false;

				if (!LODNode.IsPropertyExists("AffectingBoneIds"))
					return false;

				LOD->AffectingBoneIds.SetCount(LODNode.ReadDataSize("AffectingBoneIds") / sizeof(ZEUInt32));
				LODNode.ReadData("AffectingBoneIds", LOD->AffectingBoneIds.GetCArray(), LODNode.ReadDataSize("AffectingBoneIds"));

				LOD->VertexCount = (ZEUInt32)(LODNode.ReadDataSize("Vertices") / sizeof(ZESkinnedModelVertex));
				LOD->TriangleCount = LOD->VertexCount / 3;
				LOD->VertexBuffer = ZEGRVertexBuffer::Create(sizeof(ZESkinnedModelVertex), LOD->VertexCount);
				void* Buffer = NULL;
				LOD->VertexBuffer->Lock(&Buffer);
					LODNode.ReadData("Vertices", Buffer, LODNode.ReadDataSize("Vertices"));
					CalculateBoundingBox(Mesh->BoundingBox, Buffer, LOD->VertexCount, true);
				LOD->VertexBuffer->Unlock();
			}
			else
			{
				if (!LODNode.IsPropertyExists("Vertices"))
					return false;

				LOD->VertexCount = (ZEUInt32)(LODNode.ReadDataSize("Vertices") / sizeof(ZEModelVertex));
				LOD->TriangleCount = LOD->VertexCount / 3;
				LOD->VertexBuffer = ZEGRVertexBuffer::Create(sizeof(ZEModelVertex), LOD->VertexCount);
				void* Buffer = NULL;
				LOD->VertexBuffer->Lock(&Buffer);
					LODNode.ReadData("Vertices", Buffer, LODNode.ReadDataSize("Vertices"));
					CalculateBoundingBox(Mesh->BoundingBox, Buffer, LOD->VertexCount, false);
				LOD->VertexBuffer->Unlock();
			}
		}

		
		if (MeshNode.IsSubNodeExists("PhysicalBody"))
		{
			ZEMLReaderNode PhysicalBodyNode = MeshNode.GetSubNode("PhysicalBody");
			
			if (!ReadPhysicalBody(&Mesh->PhysicalBody, &PhysicalBodyNode))
				return false;
		}
		else
		{
			Mesh->PhysicalBody.Shapes.Clear();
			memset(&Mesh->PhysicalBody, 0, sizeof(ZEModelResourcePhysicalBody));
		}

	}

	return true;
}

bool ZEModelResource::ReadPhysicalJoint(ZEModelResourcePhysicalJoint* Joint, ZEMLReaderNode* PhysicalJointNode)
{
	if (PhysicalJointNode == NULL)
		return false;

	if (!PhysicalJointNode->IsValid())
		return false;

	Joint->Enabled = PhysicalJointNode->ReadBoolean("Enabled");
	Joint->Body1Id = PhysicalJointNode->ReadInt32("Body1Id");
	Joint->Body2Id = PhysicalJointNode->ReadInt32("Body2Id");

	Joint->UseGlobalAnchorAxis = PhysicalJointNode->ReadBoolean("UseGlobalAnchorAxis");

	if (Joint->UseGlobalAnchorAxis)
	{
		Joint->GlobalAnchor = PhysicalJointNode->ReadVector3("GlobalAnchor");
		Joint->GlobalAxis = PhysicalJointNode->ReadQuaternion("GlobalAxis");
	}
	else
	{
		Joint->LocalAnchor1 = PhysicalJointNode->ReadVector3("LocalAnchor1");
		Joint->LocalAxis1 = PhysicalJointNode->ReadQuaternion("LocalAxis1");
		Joint->LocalAnchor2 = PhysicalJointNode->ReadVector3("LocalAnchor2");
		Joint->LocalAxis2 = PhysicalJointNode->ReadQuaternion("LocalAxis2");
	}

	Joint->JointType = (ZEPhysicalJointType)PhysicalJointNode->ReadInt32("JointType");
	Joint->CollideBodies = PhysicalJointNode->ReadBoolean("CollideBodies");
	Joint->Breakable = PhysicalJointNode->ReadBoolean("Breakable");
	Joint->BreakForce = PhysicalJointNode->ReadFloat("BreakForce");
	Joint->BreakTorque = PhysicalJointNode->ReadFloat("BreakTorque");

	Joint->XMotion = (ZEPhysicalJointMotion)PhysicalJointNode->ReadInt32("XMotion");
	Joint->YMotion = (ZEPhysicalJointMotion)PhysicalJointNode->ReadInt32("YMotion");
	Joint->ZMotion = (ZEPhysicalJointMotion)PhysicalJointNode->ReadInt32("ZMotion");
	Joint->LinearLimitValue = PhysicalJointNode->ReadFloat("LinearLimitValue");
	Joint->LinearLimitRestitution = PhysicalJointNode->ReadFloat("LinearLimitRestitution");
	Joint->LinearLimitSpring = PhysicalJointNode->ReadFloat("LinearLimitSpring");
	Joint->LinearLimitDamping = PhysicalJointNode->ReadFloat("LinearLimitDamping");

	Joint->TwistMotion = (ZEPhysicalJointMotion)PhysicalJointNode->ReadInt32("TwistMotion");
	Joint->TwistLowLimitValue = PhysicalJointNode->ReadFloat("TwistLowLimitValue");
	Joint->TwistLowLimitRestitution = PhysicalJointNode->ReadFloat("TwistLowLimitRestitution");
	Joint->TwistLowLimitSpring = PhysicalJointNode->ReadFloat("TwistLowLimitSpring");
	Joint->TwistLowLimitDamping = PhysicalJointNode->ReadFloat("TwistLowLimitDamping");
	Joint->TwistHighLimitValue = PhysicalJointNode->ReadFloat("TwistHighLimitValue");
	Joint->TwistHighLimitRestitution = PhysicalJointNode->ReadFloat("TwistHighLimitRestitution");
	Joint->TwistHighLimitSpring = PhysicalJointNode->ReadFloat("TwistHighLimitSpring");
	Joint->TwistHighLimitDamping = PhysicalJointNode->ReadFloat("TwistHighLimitDamping");

	Joint->Swing1Motion = (ZEPhysicalJointMotion)PhysicalJointNode->ReadInt32("Swing1Motion");
	Joint->Swing1LimitValue = PhysicalJointNode->ReadFloat("Swing1LimitValue");
	Joint->Swing1LimitRestitution = PhysicalJointNode->ReadFloat("Swing1LimitRestitution");
	Joint->Swing1LimitSpring = PhysicalJointNode->ReadFloat("Swing1LimitSpring");
	Joint->Swing1LimitDamping = PhysicalJointNode->ReadFloat("Swing1LimitDamping");

	Joint->Swing2Motion = (ZEPhysicalJointMotion)PhysicalJointNode->ReadInt32("Swing2Motion");
	Joint->Swing2LimitValue = PhysicalJointNode->ReadFloat("Swing2LimitValue");
	Joint->Swing2LimitRestitution = PhysicalJointNode->ReadFloat("Swing2LimitRestitution");
	Joint->Swing2LimitSpring = PhysicalJointNode->ReadFloat("Swing2LimitSpring");
	Joint->Swing2LimitDamping = PhysicalJointNode->ReadFloat("Swing2LimitDamping");

	Joint->LinearXMotor = (ZEPhysicalJointMotorType)PhysicalJointNode->ReadInt32("LinearXMotor");
	Joint->LinearXMotorForce = PhysicalJointNode->ReadFloat("LinearXMotorForce");
	Joint->LinearXMotorSpring = PhysicalJointNode->ReadFloat("LinearXMotorSpring");
	Joint->LinearXMotorDamper = PhysicalJointNode->ReadFloat("LinearXMotorDamper");

	Joint->LinearYMotor = (ZEPhysicalJointMotorType)PhysicalJointNode->ReadInt32("LinearYMotor");
	Joint->LinearYMotorForce = PhysicalJointNode->ReadFloat("LinearYMotorForce");
	Joint->LinearYMotorSpring = PhysicalJointNode->ReadFloat("LinearYMotorSpring");
	Joint->LinearYMotorDamper = PhysicalJointNode->ReadFloat("LinearYMotorDamper");

	Joint->LinearZMotor = (ZEPhysicalJointMotorType)PhysicalJointNode->ReadInt32("LinearZMotor");
	Joint->LinearZMotorForce = PhysicalJointNode->ReadFloat("LinearZMotorForce");
	Joint->LinearZMotorSpring = PhysicalJointNode->ReadFloat("LinearZMotorSpring");
	Joint->LinearZMotorDamper = PhysicalJointNode->ReadFloat("LinearZMotorDamper");

	Joint->AngularSwingMotor = (ZEPhysicalJointMotorType)PhysicalJointNode->ReadInt32("AngularSwingMotor");
	Joint->AngularSwingMotorForce = PhysicalJointNode->ReadFloat("AngularSwingMotorForce");
	Joint->AngularSwingMotorSpring = PhysicalJointNode->ReadFloat("AngularSwingMotorSpring");
	Joint->AngularSwingMotorDamper = PhysicalJointNode->ReadFloat("AngularSwingMotorDamper");

	Joint->AngularTwistMotor = (ZEPhysicalJointMotorType)PhysicalJointNode->ReadInt32("AngularTwistMotor");
	Joint->AngularTwistMotorForce = PhysicalJointNode->ReadFloat("AngularTwistMotorForce");
	Joint->AngularTwistMotorSpring = PhysicalJointNode->ReadFloat("AngularTwistMotorSpring");
	Joint->AngularTwistMotorDamper = PhysicalJointNode->ReadFloat("AngularTwistMotorDamper");

	Joint->AngularSlerpMotor = (ZEPhysicalJointMotorType)PhysicalJointNode->ReadInt32("AngularSlerpMotor");
	Joint->AngularSlerpMotorForce = PhysicalJointNode->ReadFloat("AngularSlerpMotorForce");
	Joint->AngularSlerpMotorSpring = PhysicalJointNode->ReadFloat("AngularSlerpMotorSpring");
	Joint->AngularSlerpMotorDamper = PhysicalJointNode->ReadFloat("AngularSlerpMotorDamper");

	Joint->MotorTargetPosition = PhysicalJointNode->ReadVector3("MotorTargetPosition");
	Joint->MotorTargetVelocity = PhysicalJointNode->ReadVector3("MotorTargetVelocity");
	Joint->MotorTargetOrientation = PhysicalJointNode->ReadQuaternion("MotorTargetOrientation");
	Joint->MotorTargetAngularVelocity = PhysicalJointNode->ReadVector3("MotorTargetAngularVelocity");

	return true;
}

void ZEModelResource::ProcessBones(ZEModelResourceBone* Bone, ZEInt BoneId)
{
	ZEMatrix4x4::CreateOrientation(Bone->LocalTransform, Bone->Position, Bone->Rotation, Bone->Scale);

	if (Bone->ParentBone != -1)
	{

		ZEMatrix4x4::Multiply(Bone->ForwardTransform, Bones[(ZESize)Bone->ParentBone].ForwardTransform, Bone->LocalTransform);
		ZEMatrix4x4::Inverse(Bone->InverseTransform, Bone->ForwardTransform);
	}
	else
	{
		Bone->ForwardTransform = Bone->LocalTransform;
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

bool ZEModelResource::ReadBones(ZEMLReaderNode* BonesNode)
{
	if (BonesNode == NULL)
		return false;

	if (!BonesNode->IsValid())
		return false;

	ZESize SubNodeCount = (ZESize)BonesNode->GetSubNodeCount();

	for (ZESize I = 0; I < SubNodeCount; I++)
	{
		ZEMLReaderNode BoneNode = BonesNode->GetSubNode("Bone", I);

		if (!BoneNode.IsValid())
			return false;

		ZEModelResourceBone* Bone = Bones.Add();

		Bone->Name = BoneNode.ReadString("Name");
		Bone->ParentBone = BoneNode.ReadInt32("ParentBone");
		Bone->Position = BoneNode.ReadVector3("RelativePosition");
		Bone->Rotation = BoneNode.ReadQuaternion("RelativeRotation");
		Bone->Scale = BoneNode.ReadVector3("RelativeScale");

		if (BoneNode.IsPropertyExists("UserDefinedProperties"))
			Bone->UserDefinedProperties = BoneNode.ReadString("UserDefinedProperties");

		ZEMLReaderNode BoundingBoxNode = BoneNode.GetSubNode("BoundingBox");
		Bone->BoundingBox.Max = BoundingBoxNode.ReadVector3("Max");
		Bone->BoundingBox.Min = BoundingBoxNode.ReadVector3("Min");

 		memset(&Bone->PhysicalJoint, 0, sizeof(ZEModelResourcePhysicalJoint));

		if (BoneNode.IsSubNodeExists("PhysicalJoint"))
		{
			ZEMLReaderNode PhysicalJointNode = BoneNode.GetSubNode("PhysicalJoint");

			if (!ReadPhysicalJoint(&Bone->PhysicalJoint, &PhysicalJointNode))
				return false;
		}

		memset(&Bone->PhysicalBody, 0, sizeof(ZEModelResourcePhysicalBody));

		if (BoneNode.IsSubNodeExists("PhysicalBody"))
		{
			ZEMLReaderNode PhysicalBodyNode = BoneNode.GetSubNode("PhysicalBody");
			
			if (!ReadPhysicalBody(&Bone->PhysicalBody, &PhysicalBodyNode))
				return false;
		}

	}

	for (ZESize I = 0; I < Bones.GetCount(); I++)
	{
		if (Bones[I].ParentBone == -1)
		{
			ProcessBones(&Bones[I], (ZEInt)I);
		}
	}

	return true;
}

bool ZEModelResource::ReadHelpers(ZEMLReaderNode* HelpersNode)
{
	if (HelpersNode == NULL)
		return false;

	if (!HelpersNode->IsValid())
		return false;

	ZESize SubNodeCount = (ZESize)HelpersNode->GetSubNodeCount();

	for (ZESize I = 0; I < SubNodeCount; I++)
	{
		ZEMLReaderNode HelperNode = HelpersNode->GetSubNode("Helper", I);

		if (!HelperNode.IsValid())
			return false;

		ZEModelResourceHelper* Helper = Helpers.Add();

		memset(Helper, 0, sizeof(ZEModelResourceHelper));

		Helper->Name = HelperNode.ReadString("Name");
		Helper->Position = HelperNode.ReadVector3("Position");
		Helper->Rotation = HelperNode.ReadQuaternion("Rotation");
		Helper->Scale = HelperNode.ReadVector3("Scale");

		if (HelperNode.IsPropertyExists("UserDefinedProperties"))
			Helper->UserDefinedProperties = HelperNode.ReadString("UserDefinedProperties");

		Helper->OwnerType = (ZEModelResourceHelperOwnerType)HelperNode.ReadInt32("OwnerType");
		Helper->OwnerId = HelperNode.ReadInt32("OwnerId");

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

	return true;
}

bool ZEModelResource::ReadAnimations(ZEMLReaderNode* AnimationsNode)
{
	if (AnimationsNode == NULL)
		return false;

	if (!AnimationsNode->IsValid())
		return false;

	ZESize SubNodeCount = (ZESize)AnimationsNode->GetSubNodeCount();

	for (ZESize I = 0; I < SubNodeCount; I++)
	{
		ZEMLReaderNode AnimationNode = AnimationsNode->GetSubNode("Animation", I);

		if (!AnimationNode.IsValid())
			return false;

		ZEModelResourceAnimation* Animation = Animations.Add();

		Animation->Name = AnimationNode.ReadString("Name");

		ZEUInt BoneKeyCount = AnimationNode.ReadUInt32("BoneKeyCount");
		ZEUInt MeshKeyCount = AnimationNode.ReadUInt32("MeshKeyCount");
		ZEUInt FrameKeyCount = BoneKeyCount + MeshKeyCount;

		Animation->Frames.SetCount(((ZESize)AnimationNode.ReadDataSize("Frames") / sizeof(ZEModelResourceAnimationKey)) / (ZESize)FrameKeyCount);

		for (ZESize I = 0; I < Animation->Frames.GetCount(); I++)
		{
			ZEModelResourceAnimationFrame* CurrentAnimationFrame = &Animation->Frames[I];
		
			CurrentAnimationFrame->BoneKeys.SetCount(BoneKeyCount);
		
			if (BoneKeyCount != 0)
				AnimationNode.ReadData("Frames", CurrentAnimationFrame->BoneKeys.GetCArray(), BoneKeyCount * sizeof(ZEModelResourceAnimationKey), (I * FrameKeyCount) * sizeof(ZEModelResourceAnimationKey));
		
			CurrentAnimationFrame->MeshKeys.SetCount(MeshKeyCount);
		
			if (MeshKeyCount != 0)
				AnimationNode.ReadData("Frames", CurrentAnimationFrame->MeshKeys.GetCArray(), MeshKeyCount * sizeof(ZEModelResourceAnimationKey), (I * FrameKeyCount) * sizeof(ZEModelResourceAnimationKey) + BoneKeyCount * sizeof(ZEModelResourceAnimationKey));
		}
	}

	return true;
}

bool ZEModelResource::ReadModelFromFile(ZEFile* ResourceFile)
{
	ZEMLReader ModelReader;
	
	if (!ModelReader.Open(ResourceFile))
		return false;

	ZEMLReaderNode ModelNode = ModelReader.GetRootNode();
	if (!ModelNode.IsValid())
		return false;

	if (ModelNode.GetName() != "ZEModel")
		return false;

	if (ModelNode.IsSubNodeExists("UserDefinedBoundingBox"))
	{
		ZEMLReaderNode UserDefinedBoundingBoxNode = ModelNode.GetSubNode("UserDefinedBoundingBox");
		UserDefinedBoundingBox.Min = UserDefinedBoundingBoxNode.ReadVector3("Min");
		UserDefinedBoundingBox.Max = UserDefinedBoundingBoxNode.ReadVector3("Max");
		BoundingBoxIsUserDefined = true;
	}
	else
	{
		BoundingBoxIsUserDefined = false;
	}

	if (ModelNode.IsSubNodeExists("Bones"))
	{
		ZEMLReaderNode BonesNode = ModelNode.GetSubNode("Bones");

		if (!ReadBones(&BonesNode))
			return false;
	}

	if (ModelNode.IsSubNodeExists("Meshes"))
	{
		ZEMLReaderNode MeshesNode = ModelNode.GetSubNode("Meshes");

		if (!ReadMeshes(&MeshesNode))
			return false;
	}

	if (ModelNode.IsSubNodeExists("Helpers"))
	{
		ZEMLReaderNode HelpersNode = ModelNode.GetSubNode("Helpers");

		if (!ReadHelpers(&HelpersNode))
			return false;
	}

	if (ModelNode.IsSubNodeExists("Animations"))
	{
		ZEMLReaderNode AnimationsNode = ModelNode.GetSubNode("Animations");

		if (!ReadAnimations(&AnimationsNode))
			return false;
	}

	ZEMLReaderNode MaterialsNode = ModelNode.GetSubNode("Materials");

	if (!ReadMaterials(&MaterialsNode))
		return false;

	ModelReader.Close();

	return true;
}

const char* ZEModelResource::GetResourceType() const
{
	return "Model";
}

bool ZEModelResource::GetUserDefinedBoundingBoxEnabled() const
{
	return BoundingBoxIsUserDefined;
}

const ZEAABBox& ZEModelResource::GetUserDefinedBoundingBox() const
{
	return UserDefinedBoundingBox;
}

const ZESmartArray<ZETexture2DResource*>& ZEModelResource::GetTextures() const
{
	return TextureResources;
}

const ZEArray<ZERNMaterial*>& ZEModelResource::GetMaterials() const
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
	ZEModelResource* Resource = (ZEModelResource*)zeResources->GetResource(FileName);
	if (Resource != NULL)
		return Resource;
	else
	{
		Resource = LoadResource(FileName);
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
	ZEModelResource* Resource = (ZEModelResource*)zeResources->GetResource(FileName);
	if (Resource != NULL)
		Resource->Cached = true;
	else
	{
		Resource = LoadResource(FileName);
		Resource->Cached = true;
		Resource->ReferenceCount = 0;
		zeResources->AddResource(Resource);
	}
}

ZEModelResource* ZEModelResource::LoadResource(const ZEString& FileName)
{
	bool Result;
	ZEFile ResourceFile;
	Result = ResourceFile.Open(FileName, ZE_FOM_READ, ZE_FCM_NONE);
	if (Result)
	{
		ZEModelResource* NewResource = new ZEModelResource();
		NewResource->SetFileName(FileName);
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
