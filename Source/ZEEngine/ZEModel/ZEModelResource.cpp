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

#include "ZEModelResource.h"

#include "ZEError.h"
#include "ZEML/ZEMLReader.h"
#include "ZEML/ZEMLWriter.h"
#include "ZEGraphics/ZEFixedMaterial.h"
#include "ZEFile/ZEFileInfo.h"

static void CalculateBoundingBox(ZEModelResourceMesh* Mesh)
{
	if (Mesh == NULL)
		return;

	ZEAABBox BoundingBox;
	BoundingBox.Min = ZEVector3(FLT_MAX, FLT_MAX, FLT_MAX);
	BoundingBox.Max = ZEVector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	ze_for_each(CurrentLOD, Mesh->GetLODs())
	{
		if (Mesh->GetSkinned())
		{
			const ZEArray<ZESkinnedModelVertex>& Vertices = CurrentLOD->GetVerticesSkin();
			for (ZESize N = 0; N < Vertices.GetCount(); N++)
			{
				const ZEVector3& Position = Vertices[N].Position;

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
			const ZEArray<ZEModelVertex>& Vertices = CurrentLOD->GetVertices();
			for (ZESize N = 0; N < Vertices.GetCount(); N++)
			{
				const ZEVector3& Position = Vertices[N].Position;

				if (Position.x < BoundingBox.Min.x) BoundingBox.Min.x = Position.x;
				if (Position.y < BoundingBox.Min.y) BoundingBox.Min.y = Position.y;
				if (Position.z < BoundingBox.Min.z) BoundingBox.Min.z = Position.z;
				if (Position.x > BoundingBox.Max.x) BoundingBox.Max.x = Position.x;
				if (Position.y > BoundingBox.Max.y) BoundingBox.Max.y = Position.y;
				if (Position.z > BoundingBox.Max.z) BoundingBox.Max.z = Position.z;
			}
		}
	}

	Mesh->SetBoundingBox(BoundingBox);
}

bool ZEModelResource::ProcessBones(ZEModelResourceBone* Bone, ZEInt BoneId)
{
	ZEMatrix4x4 Temp;
	ZEMatrix4x4::CreateOrientation(Temp, Bone->GetPosition(), Bone->GetRotation(), Bone->GetScale());
	Bone->SetLocalTransform(Temp);

	if (Bone->GetParentBone() != -1)
	{
		zeCheckError(Bone->GetParentBone() >= Bones.GetCount(), false, "Invalid Bone index.");

		ZEModelResourceBone* ParentBone = Bones[(ZESize)Bone->GetParentBone()];
		ZEMatrix4x4::Multiply(Temp, ParentBone->GetForwardTransform(), Bone->GetLocalTransform());
		Bone->SetForwardTransform(Temp);

		Temp.InverseSelf();
		Bone->SetInverseTransform(Temp);
	}
	else
	{
		Bone->SetForwardTransform(Bone->GetLocalTransform());
		Bone->SetInverseTransform(Bone->GetLocalTransform().Inverse());
	}

	for (ZESize I = 0; I < Bones.GetCount(); I++)
	{
		if (Bones[I]->GetParentBone() == BoneId)
		{
			if (!ProcessBones(Bones[I], (ZEInt)I))
				return false;
		}
	}
}

bool ZEModelResource::ReadMeshes(const ZEMLReaderNode& MeshesNode)
{
	zeCheckError(!MeshesNode.IsValid(), false, "Invalid Meshes node.");
	zeCheckError(MeshesNode.GetName() == "Meshes", false, "Invalid Meshes node name.");

	ZESize SubNodeCount = MeshesNode.GetNodeCount("Mesh");
	for (ZESize I = 0; I < SubNodeCount; I++)
	{
		ZEMLReaderNode MeshNode = MeshesNode.GetNode("Mesh", I);
		if (!MeshNode.IsValid())
			return false;

		ZEPointer<ZEModelResourceMesh> Mesh = new ZEModelResourceMesh();
		if (!Mesh->Load(MeshNode))
			return false;

		AddMesh(Mesh);
	}

	return true;
}

bool ZEModelResource::ReadBones(const ZEMLReaderNode& BonesNode)
{
	zeCheckError(!BonesNode.IsValid(), false, "Invalid Bones node.");
	zeCheckError(BonesNode.GetName() == "Bones", false, "Invalid Bones node name.");

	ZESize SubNodeCount = BonesNode.GetNodeCount("Bone");
	for (ZESize I = 0; I < SubNodeCount; I++)
	{
		ZEMLReaderNode BoneNode = BonesNode.GetNode("Bone", I);
		if (!BoneNode.IsValid())
			return false;

		ZEPointer<ZEModelResourceBone> Bone = new ZEModelResourceBone();
		if (!Bone->Load(BoneNode))
			return false;

		AddBone(Bone);
	}

	for (ZESize I = 0; I < Bones.GetCount(); I++)
	{
		if (Bones[I]->GetParentBone() == -1)
		{
			if (!ProcessBones(Bones[I], (ZEInt)I))
				return false;
		}
	}

	return true;
}

bool ZEModelResource::ReadHelpers(const ZEMLReaderNode& HelpersNode)
{
	zeCheckError(!HelpersNode.IsValid(), false, "Invalid Helpers node.");
	zeCheckError(HelpersNode.GetName() == "Helpers", false, "Invalid Helpers node name.");

	ZESize SubNodeCount = HelpersNode.GetNodeCount("Helper");
	for (ZESize I = 0; I < SubNodeCount; I++)
	{
		ZEMLReaderNode HelperNode = HelpersNode.GetNode("Helper", I);
		if (!HelperNode.IsValid())
			return false;

		ZEPointer<ZEModelResourceHelper> Helper = new ZEModelResourceHelper();
		if (!Helper->Load(HelperNode))
			return false;

		AddHelper(Helper);
	}

	return true;
}

bool ZEModelResource::ReadAnimations(const ZEMLReaderNode& AnimationsNode)
{
	zeCheckError(!AnimationsNode.IsValid(), false, "Invalid Animations node.");
	zeCheckError(AnimationsNode.GetName() == "Animations", false, "Invalid Animations node name.");

	ZESize SubNodeCount = AnimationsNode.GetNodeCount("Animation");
	for (ZESize I = 0; I < SubNodeCount; I++)
	{
		ZEMLReaderNode AnimationNode = AnimationsNode.GetNode("Animation", I);

		if (!AnimationNode.IsValid())
			return false;

		ZEPointer<ZEModelResourceAnimation> Animation = new ZEModelResourceAnimation();
		if (!Animation->Load(AnimationNode))
			return false;

		AddAnimation(Animation);
	}

	return true;
}

bool ZEModelResource::ReadMaterials(const ZEMLReaderNode& MaterialsNode)
{
	zeCheckError(!MaterialsNode.IsValid(), false, "Invalid Materials node.");
	zeCheckError(MaterialsNode.GetName() == "Materials", false, "Invalid Materials node name.");

	ZESize SubNodeCount = MaterialsNode.GetNodeCount("Material");
	for (ZESize I = 0; I < SubNodeCount; I++)
	{
		ZEMLReaderNode MaterialNode = MaterialsNode.GetNode("Material", I);
		ZEString MaterialPath = ZEFileInfo(this->GetFilePath()).GetParentDirectory() + "/" + MaterialNode.ReadString("FilePath");

		if (!ZEFileInfo(MaterialPath).IsFile())
			return false;

		ZEFixedMaterial* CurrentMaterial = ZEFixedMaterial::CreateInstance();
		CurrentMaterial->ReadFromFile(MaterialPath);
		Materials.Add(CurrentMaterial);
	}

	return true;
}

bool ZEModelResource::LoadInternal(const ZERSLoadingOptions* Option)
{
	enum ModelResourceState
	{
		ZEMD_RSL_NONE,
		ZEMD_RLS_STRUCTURE,
		ZEMD_RLS_MESH_DATA
	} State;

	switch(State)
	{
		case ZEMD_RSL_NONE:
		case ZEMD_RLS_STRUCTURE:
		case ZEMD_RLS_MESH_DATA:
	}

	// Load Internal Structure and Start Loading Materials
	// Load Meshes

	return false;
}

bool ZEModelResource::Load(const ZEMLReaderNode& ModelNode)
{
	zeCheckError(!ModelNode.IsValid(), false, "Invalid Model node.");
	zeCheckError(ModelNode.GetName() != "ZEModel", false, "Invalid Model node name.");


	ZEMLReaderNode UserDefinedBoundingBoxNode = ModelNode.GetNode("UserDefinedBoundingBox");
	if (UserDefinedBoundingBoxNode.IsValid())
	{
		UserDefinedBoundingBox.Min = UserDefinedBoundingBoxNode.ReadVector3("Min");
		UserDefinedBoundingBox.Max = UserDefinedBoundingBoxNode.ReadVector3("Max");
		BoundingBoxIsUserDefined = true;
	}
	else
	{
		BoundingBoxIsUserDefined = false;
	}

	ZEMLReaderNode MeshesNode = ModelNode.GetNode("Meshes");
	if (MeshesNode.IsValid())
	{
		if (!ReadMeshes(MeshesNode))
			return false;
	}

	ZEMLReaderNode BonesNode = ModelNode.GetNode("Bones");
	if (BonesNode.IsValid())
	{
		if (!ReadBones(BonesNode))
			return false;
	}

	ZEMLReaderNode HelpersNode = ModelNode.GetNode("Helpers");
	if (HelpersNode.IsValid())
	{
		if (!ReadHelpers(HelpersNode))
			return false;
	}

	ZEMLReaderNode AnimationsNode = ModelNode.GetNode("Animations");
	if (AnimationsNode.IsValid())
	{
		if (!ReadAnimations(AnimationsNode))
			return false;
	}

	ZEMLReaderNode MaterialsNode = ModelNode.GetNode("Materials");
	if (MaterialsNode.IsValid())
	{
		if (!ReadMaterials(MaterialsNode))
			return false;
	}

	return true;
}

bool ZEModelResource::GetUserDefinedBoundingBoxEnabled() const
{
	return BoundingBoxIsUserDefined;
}

const ZEAABBox& ZEModelResource::GetUserDefinedBoundingBox() const
{
	return UserDefinedBoundingBox;
}

const ZEList2<ZEModelResourceMesh>& ZEModelResource::GetMeshes() const
{
	return Meshes;
}

void ZEModelResource::AddMesh(ZEModelResourceMesh* Mesh)
{
	zeCheckError(Mesh->Link.GetInUse(), ZE_VOID, "Mesh already added to a resource.");
	Meshes.AddEnd(&Mesh->Link);
}

void ZEModelResource::RemoveMesh(ZEModelResourceMesh* Mesh)
{
	Meshes.Remove(&Mesh->Link);
}

const ZEList2<ZEModelResourceBone>& ZEModelResource::GetBones() const
{
	return Bones;
}

void ZEModelResource::AddBone(ZEModelResourceBone* Bone)
{
	zeCheckError(Bone->Link.GetInUse(), ZE_VOID, "Bone already added to a resource.");
	Bones.AddEnd(&Bone->Link);
}

void ZEModelResource::RemoveBone(ZEModelResourceBone* Bone)
{
	Bones.Remove(&Bone->Link);
}

const ZEList2<ZEModelResourceAnimation>& ZEModelResource::GetAnimations() const
{
	return Animations;
}

void ZEModelResource::AddAnimation(ZEModelResourceAnimation* Animation)
{
	zeCheckError(Animation->Link.GetInUse(), ZE_VOID, "Animation already added to a resource.");
	Animations.AddEnd(&Animation->Link);
}

void ZEModelResource::RemoveAnimation(ZEModelResourceAnimation* Animation)
{
	Animations.Remove(&Animation->Link);
}

const ZEList2<ZEModelResourceHelper>& ZEModelResource::GetHelpers() const
{
	return Helpers;
}

void ZEModelResource::AddHelper(ZEModelResourceHelper* Helper)
{
	zeCheckError(Helper->Link.GetInUse(), ZE_VOID, "Helper is already added to a resource.");
	Helpers.AddEnd(&Helper->Link);
}

void ZEModelResource::RemoveHelper(ZEModelResourceHelper* Helper)
{
	Helpers.Remove(&Helper->Link);
}

ZEModelResource::ZEModelResource()
{
	SetLoadMethod(ZERS_LM_ASYNC);
}

ZEModelResource::~ZEModelResource()
{
	ze_for_each(Mesh, Meshes)
	{
		RemoveMesh(&(*Mesh));
		delete &(*Mesh);
	}

	ze_for_each(Bone, Bones)
	{
		RemoveBone(&(*Bone));
		delete &(*Bone);
	}

	ze_for_each(Animation, Animations)
	{
		RemoveAnimation(&(*Animation));
		delete &(*Animation);
	}

	ze_for_each(Helper, Helpers)
	{
		RemoveHelper(&(*Helper));
		delete &(*Helper);
	}
}
