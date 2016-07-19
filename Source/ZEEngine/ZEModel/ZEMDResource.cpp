//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMDResource.cpp
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

#include "ZEMDResource.h"

#include "ZEMDResourceMesh.h"
#include "ZEMDResourceBone.h"
#include "ZEMDResourceAnimation.h"
#include "ZEMDResourceHelper.h"
#include "ZEMDResourceLOD.h"

#include "ZEError.h"
#include "ZEResource/ZERSHolder.h"
#include "ZEFile/ZEFileInfo.h"
#include "ZEML/ZEMLReader.h"
#include "ZEML/ZEMLWriter.h"
#include "ZERenderer/ZERNFixedMaterial.h"
#include "ZEResource/ZERSTemplates.h"

static void CalculateBoundingBox(ZEMDResourceMesh* Mesh)
{
	if (Mesh == NULL)
		return;

	ZEAABBox BoundingBox;
	BoundingBox.Min = ZEVector3(FLT_MAX, FLT_MAX, FLT_MAX);
	BoundingBox.Max = ZEVector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	ze_for_each(CurrentLOD, Mesh->GetLODs())
	{
		if (CurrentLOD->GetVertexType() == ZEMD_VT_SKINNED)
		{
			const ZEArray<ZEMDVertexSkin>& Vertices = CurrentLOD->GetVerticesSkin();
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
			const ZEArray<ZEMDVertex>& Vertices = CurrentLOD->GetVertices();
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

bool ZEMDResource::ReadMeshes(const ZEMLReaderNode& MeshesNode)
{
	zeCheckError(!MeshesNode.IsValid(), false, "Invalid Meshes node.");
	zeCheckError(MeshesNode.GetName() != "Meshes", false, "Invalid Meshes node name.");

	ZESize SubNodeCount = MeshesNode.GetNodeCount("Mesh");
	for (ZESize I = 0; I < SubNodeCount; I++)
	{
		ZEMLReaderNode MeshNode = MeshesNode.GetNode("Mesh", I);
		if (!MeshNode.IsValid())
			return false;

		ZEPointer<ZEMDResourceMesh> Mesh = new ZEMDResourceMesh();
		if (!Mesh->Load(MeshNode))
			return false;

		AddMesh(Mesh.Transfer());
	}

	return true;
}

bool ZEMDResource::ReadBones(const ZEMLReaderNode& BonesNode)
{
	zeCheckError(!BonesNode.IsValid(), false, "Invalid Bones node.");
	zeCheckError(BonesNode.GetName() != "Bones", false, "Invalid Bones node name.");

	ZESize SubNodeCount = BonesNode.GetNodeCount("Bone");
	for (ZESize I = 0; I < SubNodeCount; I++)
	{
		ZEMLReaderNode BoneNode = BonesNode.GetNode("Bone", I);
		if (!BoneNode.IsValid())
			return false;

		ZEPointer<ZEMDResourceBone> Bone = new ZEMDResourceBone();
		if (!Bone->Load(BoneNode))
			return false;

		AddBone(Bone.Transfer());
	}

	return true;
}

bool ZEMDResource::ReadHelpers(const ZEMLReaderNode& HelpersNode)
{
	zeCheckError(!HelpersNode.IsValid(), false, "Invalid Helpers node.");
	zeCheckError(HelpersNode.GetName() != "Helpers", false, "Invalid Helpers node name.");

	ZESize SubNodeCount = HelpersNode.GetNodeCount("Helper");
	for (ZESize I = 0; I < SubNodeCount; I++)
	{
		ZEMLReaderNode HelperNode = HelpersNode.GetNode("Helper", I);
		if (!HelperNode.IsValid())
			return false;

		ZEPointer<ZEMDResourceHelper> Helper = new ZEMDResourceHelper();
		if (!Helper->Load(HelperNode))
			return false;

		AddHelper(Helper.Transfer());
	}

	return true;
}

bool ZEMDResource::ReadAnimations(const ZEMLReaderNode& AnimationsNode)
{
	zeCheckError(!AnimationsNode.IsValid(), false, "Invalid Animations node.");
	zeCheckError(AnimationsNode.GetName() != "Animations", false, "Invalid Animations node name.");

	ZESize SubNodeCount = AnimationsNode.GetNodeCount("Animation");
	for (ZESize I = 0; I < SubNodeCount; I++)
	{
		ZEMLReaderNode AnimationNode = AnimationsNode.GetNode("Animation", I);

		if (!AnimationNode.IsValid())
			return false;

		ZEPointer<ZEMDResourceAnimation> Animation = new ZEMDResourceAnimation();
		if (!Animation->Load(AnimationNode))
			return false;

		AddAnimation(Animation.Transfer());
	}

	return true;
}

bool ZEMDResource::ReadMaterials(const ZEMLReaderNode& MaterialsNode)
{
	zeCheckError(!MaterialsNode.IsValid(), false, "Invalid Materials node.");
	zeCheckError(MaterialsNode.GetName() != "Materials", false, "Invalid Materials node name.");

	ZEArray<ZEHolder<ZERNFixedMaterial>> Materials;
	ZESize SubNodeCount = MaterialsNode.GetNodeCount("Material");
	for (ZESize I = 0; I < SubNodeCount; I++)
	{
		ZEMLReaderNode MaterialNode = MaterialsNode.GetNode("Material", I);
		ZEString MaterialPath = ZEFileInfo(this->GetFileName()).GetParentDirectory() + "/" + MaterialNode.ReadString("FilePath");

		if (!ZEFileInfo(MaterialPath).IsFile())
			return false;

		ZEHolder<ZERNFixedMaterial> CurrentMaterial = ZERNFixedMaterial::CreateInstance();
		CurrentMaterial->ReadFromFile(MaterialPath);
		CurrentMaterial->Initialize();
		Materials.Add(CurrentMaterial);
	}

	ze_for_each(Mesh, Meshes)
	{
		ze_for_each(LOD, Mesh->GetLODs())
		{
			if (LOD->MaterialID >= 0 && LOD->MaterialID <= Materials.GetCount())
				LOD->SetMaterial(Materials[LOD->MaterialID].Cast<ZERNMaterial>());
		}
	}
	return true;
}

ZETaskResult ZEMDResource::LoadInternal()
{
	ZEMLReader Reader;
	if (!Reader.Open(GetFileName()))
		return ZE_TR_FAILED;

	ZEMLReaderNode ModelNode = Reader.GetRootNode();

	zeCheckError(!ModelNode.IsValid(), ZE_TR_FAILED, "Invalid Model node.");
	zeCheckError(ModelNode.GetName() != "ZEModel", ZE_TR_FAILED, "Invalid Model node name.");

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
			return ZE_TR_FAILED;
	}

	ZEMLReaderNode BonesNode = ModelNode.GetNode("Bones");
	if (BonesNode.IsValid())
	{
		if (!ReadBones(BonesNode))
			return ZE_TR_FAILED;
	}

	ZEMLReaderNode HelpersNode = ModelNode.GetNode("Helpers");
	if (HelpersNode.IsValid())
	{
		if (!ReadHelpers(HelpersNode))
			return ZE_TR_FAILED;
	}

	ZEMLReaderNode AnimationsNode = ModelNode.GetNode("Animations");
	if (AnimationsNode.IsValid())
	{
		if (!ReadAnimations(AnimationsNode))
			return ZE_TR_FAILED;
	}

	ZEMLReaderNode MaterialsNode = ModelNode.GetNode("Materials");
	if (MaterialsNode.IsValid())
	{
		if (!ReadMaterials(MaterialsNode))
			return ZE_TR_FAILED;
	}

	return ZE_TR_DONE;
}

ZETaskResult ZEMDResource::UnloadInternal()
{
	return ZE_TR_DONE;
}

bool ZEMDResource::GetUserDefinedBoundingBoxEnabled() const
{
	return BoundingBoxIsUserDefined;
}

const ZEAABBox& ZEMDResource::GetUserDefinedBoundingBox() const
{
	return UserDefinedBoundingBox;
}

const ZEList2<ZEMDResourceMesh>& ZEMDResource::GetMeshes()
{
	return Meshes;
}

const ZEList2<const ZEMDResourceMesh>& ZEMDResource::GetMeshes() const
{
	return Meshes.ToInspector();
}

void ZEMDResource::AddMesh(ZEMDResourceMesh* Mesh)
{
	zeCheckError(Mesh->Link.GetInUse(), ZE_VOID, "Mesh already added to a resource.");
	Meshes.AddEnd(&Mesh->Link);
}

void ZEMDResource::RemoveMesh(ZEMDResourceMesh* Mesh)
{
	Meshes.Remove(&Mesh->Link);
}

const ZEList2<ZEMDResourceBone>& ZEMDResource::GetBones() 
{
	return Bones;
}

const ZEList2<const ZEMDResourceBone>& ZEMDResource::GetBones() const
{
	return Bones.ToInspector();
}

void ZEMDResource::AddBone(ZEMDResourceBone* Bone)
{
	zeCheckError(Bone->Link.GetInUse(), ZE_VOID, "Bone already added to a resource.");
	Bones.AddEnd(&Bone->Link);
	AddChildResource(Bone);
}

void ZEMDResource::RemoveBone(ZEMDResourceBone* Bone)
{
	Bones.Remove(&Bone->Link);
	RemoveChildResource(Bone);
}

const ZEList2<ZEMDResourceAnimation>& ZEMDResource::GetAnimations()
{
	return Animations;
}

const ZEList2<const ZEMDResourceAnimation>& ZEMDResource::GetAnimations() const
{
	return Animations.ToInspector();
}

void ZEMDResource::AddAnimation(ZEMDResourceAnimation* Animation)
{
	zeCheckError(Animation->Link.GetInUse(), ZE_VOID, "Animation already added to a resource.");
	Animations.AddEnd(&Animation->Link);
	AddChildResource(Animation);
}

void ZEMDResource::RemoveAnimation(ZEMDResourceAnimation* Animation)
{
	Animations.Remove(&Animation->Link);
	RemoveChildResource(Animation);
}

const ZEList2<const ZEMDResourceHelper>& ZEMDResource::GetHelpers() const
{
	return Helpers.ToInspector();
}

void ZEMDResource::AddHelper(ZEMDResourceHelper* Helper)
{
	zeCheckError(Helper->Link.GetInUse(), ZE_VOID, "Helper is already added to a resource.");
	Helpers.AddEnd(&Helper->Link);
	AddChildResource(Helper);
}

void ZEMDResource::RemoveHelper(ZEMDResourceHelper* Helper)
{
	Helpers.Remove(&Helper->Link);
	RemoveChildResource(Helper);
}

ZEMDResource::ZEMDResource()
{

}

ZEMDResource::~ZEMDResource()
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

ZERSHolder<ZEMDResource> ZEMDResource::LoadResource(const ZEString& FileName)
{
	return ZERSTemplates::LoadResource<ZEMDResource>(FileName);
}

ZERSHolder<const ZEMDResource> ZEMDResource::LoadResourceShared(const ZEString& FileName)
{
	return ZERSTemplates::LoadResourceShared<ZEMDResource>(FileName);
}
