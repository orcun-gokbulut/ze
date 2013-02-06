//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelMeshLOD.cpp
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

#include "ZEModelMeshLOD.h"
#include "ZEModel.h"
#include "ZERenderer/ZERenderer.h"
#include "ZEGame/ZEDrawParameters.h"

void ZEModelMeshLOD::ResetMaterial()
{
	RenderCommand.Material = Owner->GetModelResource()->Materials[(ZESize)LODResource->MaterialId];
}

void ZEModelMeshLOD::SetMaterial(const ZEMaterial* Material)
{
	RenderCommand.Material = (ZEMaterial*)Material;
}

const ZEMaterial* ZEModelMeshLOD::GetMaterial()
{
	return RenderCommand.Material;
}

bool ZEModelMeshLOD::IsSkinned()
{
	return Skinned;
}

void ZEModelMeshLOD::Draw(ZEDrawParameters* DrawParameters, float DistanceSquare)
{
	if (VertexBuffer == NULL)
		return;

	if (Skinned)
	{
		RenderCommand.BoneTransforms.SetCount(LODResource->AffectingBoneIds.GetCount());
		for (ZESize I = 0; I < LODResource->AffectingBoneIds.GetCount(); I++)
		{
			ZEMatrix4x4::Multiply(RenderCommand.BoneTransforms[I], Owner->GetBones()[(ZESize)LODResource->AffectingBoneIds[I]].GetVertexTransform(), this->OwnerMesh->GetLocalTransform());
		}

		RenderCommand.WorldMatrix = Owner->GetWorldTransform();
	}
	else
	{
		RenderCommand.WorldMatrix = OwnerMesh->GetWorldTransform();
	}

	RenderCommand.Order = DistanceSquare;

	DrawParameters->Renderer->AddToRenderList(&RenderCommand);
}

void ZEModelMeshLOD::Initialize(ZEModel* Model, ZEModelMesh* Mesh,  const ZEModelResourceMeshLOD* LODResource)
{
	/*
	Owner = Model;
	OwnerMesh = Mesh;
	this->LODResource = LODResource;

	Skinned = LODResource->Vertices.GetCount() == 0 ? true : false;

	RenderCommand.SetZero();
	RenderCommand.Priority = 3;
	RenderCommand.Flags = ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM | ZE_ROF_ENABLE_WORLD_TRANSFORM | ZE_ROF_ENABLE_Z_CULLING | (Skinned ? ZE_ROF_SKINNED : 0);
	RenderCommand.PrimitiveType = ZE_ROPT_TRIANGLE_LIST;
	RenderCommand.VertexBuffer = VertexBuffer = LODResource->GetSharedVertexBuffer();
	RenderCommand.PrimitiveCount = Skinned ? LODResource->SkinnedVertices.GetCount() / 3: LODResource->Vertices.GetCount() / 3;
	RenderCommand.VertexDeclaration = Skinned ? ZESkinnedModelVertex::GetVertexDeclaration() : ZEModelVertex::GetVertexDeclaration();
	RenderCommand.Material = Owner->GetModelResource()->Materials[(ZESize)LODResource->MaterialId];
	*/
}

void ZEModelMeshLOD::Deinitialize()
{
	Owner = NULL;
	OwnerMesh = NULL;
	RenderCommand.SetZero();
	VertexBuffer = NULL;
	LODResource = NULL;
	Material = NULL;
	Skinned = false;
}

ZEModelMeshLOD::ZEModelMeshLOD()
{
	Skinned = false;
	Owner = NULL;
	OwnerMesh = NULL;
	RenderCommand.SetZero();
	VertexBuffer = NULL;
	LODResource = NULL;
	Material = NULL;
}

ZEModelMeshLOD::~ZEModelMeshLOD()
{
	Deinitialize();
}
