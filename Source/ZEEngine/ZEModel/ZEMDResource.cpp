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
#include "ZERenderer/ZERNStandardMaterial.h"
#include "ZEResource/ZERSTemplates.h"
#include "ZEMDResourceDraw.h"

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

		ZEMDResourceMesh* Mesh = ZEMDResourceMesh::CreateInstance();
		if (!Mesh->Unserialize(MeshNode))
		{
			Mesh->Destroy();
			return false;
		}

		AddMesh(Mesh);
		SetLocalLoadProgress(I, SubNodeCount, 10, 50);
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

		ZEMDResourceBone* Bone = ZEMDResourceBone::CreateInstance();
		if (!Bone->Unserialize(BoneNode))
		{
			Bone->Destroy();
			return false;
		}

		AddBone(Bone);
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

		ZEMDResourceHelper* Helper = ZEMDResourceHelper::CreateInstance();
		if (!Helper->Unserialize(HelperNode))
			return false;

		AddHelper(Helper);
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

		ZEMDResourceAnimation* Animation = ZEMDResourceAnimation::CreateInstance();
		if (!Animation->Unserialize(AnimationNode))
		{
			Animation->Destroy();
			return false;
		}

		AddAnimation(Animation);
		SetLocalLoadProgress(I, SubNodeCount, 60, 95);
	}

	return true;
}


ZETaskResult ZEMDResource::LoadInternal()
{
	ZEMLReader Reader;
	if (!Reader.Open(GetFileName()))
		return ZE_TR_FAILED;
	SetLocalLoadProgress(5);

	ZEMLReaderNode ModelNode = Reader.GetRootNode();

	zeCheckError(!ModelNode.IsValid(), ZE_TR_FAILED, "Cannot load model resource. Invalid Model node. File Name: \"%s\".", GetFileName().ToCString());
	zeCheckError(ModelNode.GetName() != "ZEModel", ZE_TR_FAILED, "Cannot load model resource. Invalid Model node name. File Name: \"%s\".", GetFileName().ToCString());

	ZEUInt8 MajorVersion = ModelNode.ReadUInt8("MajorVersion");
	ZEUInt8 MinorVersion = ModelNode.ReadUInt8("MinorVersion");
	if (MajorVersion != 2)
	{
		zeError("Cannot load model resource. Major version mismatch. Please run converter tool on this file. Current Version: 2.0. File Version: %d.%d, File Name: \"%s\".", MajorVersion, MinorVersion, GetFileName().ToCString());
		return ZE_TR_FAILED;
	}

	if (MinorVersion != 0)
	{
		zeError("Model resource minor version mismatch. This can may cause some problems. Current Version: 2.0. File Version: %d.%d, File Name: \"%s\".", MajorVersion, MinorVersion, GetFileName().ToCString());
		return ZE_TR_FAILED;
	}

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
	SetLocalLoadProgress(10);

	ZEMLReaderNode MeshesNode = ModelNode.GetNode("Meshes");
	if (MeshesNode.IsValid())
	{
		if (!ReadMeshes(MeshesNode))
			return ZE_TR_FAILED;
	}
	SetLocalLoadProgress(50);

	ZEMLReaderNode BonesNode = ModelNode.GetNode("Bones");
	if (BonesNode.IsValid())
	{
		if (!ReadBones(BonesNode))
			return ZE_TR_FAILED;
	}
	SetLocalLoadProgress(55);

	ZEMLReaderNode HelpersNode = ModelNode.GetNode("Helpers");
	if (HelpersNode.IsValid())
	{
		if (!ReadHelpers(HelpersNode))
			return ZE_TR_FAILED;
	}
	SetLocalLoadProgress(60);

	ZEMLReaderNode AnimationsNode = ModelNode.GetNode("Animations");
	if (AnimationsNode.IsValid())
	{
		if (!ReadAnimations(AnimationsNode))
			return ZE_TR_FAILED;
	}

	SetLocalLoadProgress(100);

	return ZE_TR_DONE;
}

ZETaskResult ZEMDResource::UnloadInternal()
{
	while (Meshes.GetFirst() != NULL)
	{
		ZEMDResourceMesh* Mesh = Meshes.GetFirst()->GetItem();
		RemoveMesh(Mesh);
		delete Mesh;
	}

	while (Bones.GetFirst() != NULL)
	{
		ZEMDResourceBone* Bone = Bones.GetFirst()->GetItem();
		RemoveBone(Bone);
		delete Bone;
	}

	while (Animations.GetFirst() != NULL)
	{
		ZEMDResourceAnimation* Animation = Animations.GetFirst()->GetItem();
		RemoveAnimation(Animation);
		delete Animation;
	}

	while (Helpers.GetFirst() != NULL)
	{
		ZEMDResourceHelper* Helper = Helpers.GetFirst()->GetItem();
		RemoveHelper(Helper);
		delete Helper;
	}

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
	zeCheckError(Mesh == NULL, ZE_VOID, "Cannot add mesh. Mesh is NULL.");
	zeCheckError(Mesh->Resource != NULL, ZE_VOID, "Cannot add mesh. Mesh already added to a resource.");

	Mesh->Resource = this;
	Meshes.AddEnd(&Mesh->Link);

	for (ZESize I = 0; I < Mesh->LODs.GetCount(); I++)
	{
		ZEMDResourceLOD* CurrentLOD = Mesh->LODs[I];
		for (ZESize N = 0; N < CurrentLOD->GetDraws().GetCount(); N++)
			RegisterExternalResource(CurrentLOD->GetDraws()[N].GetMaterial());
	}
}

void ZEMDResource::RemoveMesh(ZEMDResourceMesh* Mesh)
{
	zeCheckError(Mesh == NULL, ZE_VOID, "Cannot add mesh. Mesh is NULL.");
	zeCheckError(Mesh->Resource != this, ZE_VOID, "Cannot remove mesh. Mesh does not belong to this resource.");

	for (ZESize I = 0; I < Mesh->LODs.GetCount(); I++)
	{
		ZEMDResourceLOD* CurrentLOD = Mesh->LODs[I];
		for (ZESize N = 0; N < CurrentLOD->GetDraws().GetCount(); N++)
			UnregisterExternalResource(CurrentLOD->GetDraws()[N].GetMaterial());
	}

	Mesh->Resource = NULL;
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
	zeCheckError(Bone == NULL, ZE_VOID, "Cannot add bone. Bone is NULL.");
	zeCheckError(Bone->Resource != NULL, ZE_VOID, "Cannot add bone. Bone already added to a resource.");

	Bone->Resource = this;
	Bones.AddEnd(&Bone->Link);
}

void ZEMDResource::RemoveBone(ZEMDResourceBone* Bone)
{
	zeCheckError(Bone == NULL, ZE_VOID, "Cannot add bone. Bone is NULL.");
	zeCheckError(Bone->Resource != this, ZE_VOID, "Cannot remove bone. Bone does not belong to this resource.");

	Bones.Remove(&Bone->Link);
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
	zeCheckError(Animation == NULL, ZE_VOID, "Cannot add animation. Animation is NULL.");
	zeCheckError(Animation->Resource != NULL, ZE_VOID, "Animation already added to a resource.");

	Animation->Resource = this;
	Animations.AddEnd(&Animation->Link);
}

void ZEMDResource::RemoveAnimation(ZEMDResourceAnimation* Animation)
{
	zeCheckError(Animation == NULL, ZE_VOID, "Cannot remove animation. Animation is NULL.");
	zeCheckError(Animation->Resource != this, ZE_VOID, "Cannot remove animation. Animation does not belong to this resource.");

	Animation->Resource = NULL;
	Animations.Remove(&Animation->Link);
}

const ZEList2<const ZEMDResourceHelper>& ZEMDResource::GetHelpers() const
{
	return Helpers.ToInspector();
}

void ZEMDResource::AddHelper(ZEMDResourceHelper* Helper)
{
	zeCheckError(Helper == NULL, ZE_VOID, "Cannot add helper. Helper is NULL.");
	zeCheckError(Helper->Resource != NULL, ZE_VOID, "Helper already added to a resource.");

	Helpers.AddEnd(&Helper->Link);
}

void ZEMDResource::RemoveHelper(ZEMDResourceHelper* Helper)
{
	zeCheckError(Helper == NULL, ZE_VOID, "Cannot remove helper. Helper is NULL.");
	zeCheckError(Helper->Resource != this, ZE_VOID, "Cannot remove helper. Helper does not belong to this resource.");

	Helpers.Remove(&Helper->Link);
}

ZEMDResource::ZEMDResource()
{
	Register();
}

ZEMDResource::~ZEMDResource()
{
	Unregister();
}

ZERSHolder<ZEMDResource> ZEMDResource::LoadResource(const ZEString& FileName)
{
	return ZERSTemplates::LoadResource<ZEMDResource>(FileName, ZERSTemplates::InstanciatorFunction<ZEMDResource>);
}

ZERSHolder<const ZEMDResource> ZEMDResource::LoadResourceShared(const ZEString& FileName)
{
	return ZERSTemplates::LoadResourceShared<ZEMDResource>(FileName, ZERSTemplates::InstanciatorFunction<ZEMDResource>);
}
