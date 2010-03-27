//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ModelMeshLOD.cpp
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

#include "ModelMeshLOD.h"
#include "Model.h"
#include "Graphics/Renderer.h"

void ZEModelMeshLOD::ResetMaterial()
{
	RenderOrder.Material = Owner->GetModelResource()->Materials[LODResource->MaterialId];
}

void ZEModelMeshLOD::SetMaterial(const ZEMaterial* Material)
{
	RenderOrder.Material = Material;
}

const ZEMaterial* ZEModelMeshLOD::GetMaterial()
{
	return RenderOrder.Material;
}

bool ZEModelMeshLOD::IsSkinned()
{
	return Skinned;
}

void ZEModelMeshLOD::Draw(ZERenderer* Renderer, const ZESmartArray<const ZERLLight*>& Lights)
{
	if (VertexBuffer == NULL)
		return;

	if (Skinned)
	{
		RenderOrder.BoneTransforms.SetCount(LODResource->AffectingBoneIds.GetCount());
		for (size_t I = 0; I < LODResource->AffectingBoneIds.GetCount(); I++)
			RenderOrder.BoneTransforms[I] = Owner->GetBones()[LODResource->AffectingBoneIds[I]].GetVertexTransform();

		ZEMatrix4x4::Multiply(RenderOrder.WorldMatrix, this->OwnerMesh->GetLocalTransform(), Owner->GetWorldTransform());
	}
	else
		RenderOrder.WorldMatrix = OwnerMesh->GetWorldTransform();

	RenderOrder.Lights.Clear();
	RenderOrder.Lights.MassAdd(Lights.GetConstCArray(), Lights.GetCount());
	Renderer->AddToRenderList(&RenderOrder);
}

void ZEModelMeshLOD::Initialize(ZEModel* Model, ZEModelMesh* Mesh,  const ZEModelResourceMeshLOD* LODResource)
{
	Owner = Model;
	OwnerMesh = Mesh;
	this->LODResource = LODResource;

	Skinned = LODResource->Vertices.GetCount() == 0 ? true : false;

	RenderOrder.SetZero();
	RenderOrder.Flags = ZE_ROF_ENABLE_VIEWPROJECTION_TRANSFORM | ZE_ROF_ENABLE_WORLD_TRANSFORM | ZE_ROF_ENABLE_ZCULLING | (Skinned ? ZE_ROF_SKINNED : 0);
	RenderOrder.PrimitiveType = ZE_ROPT_TRIANGLE;
	RenderOrder.VertexBuffer = VertexBuffer = LODResource->GetSharedVertexBuffer();
	RenderOrder.PrimitiveCount = Skinned ? LODResource->SkinnedVertices.GetCount() / 3: LODResource->Vertices.GetCount() / 3;
	RenderOrder.VertexDeclaration = Skinned ? ZESkinnedModelVertex::GetVertexDeclaration() : ZEModelVertex::GetVertexDeclaration();
	RenderOrder.Material = Owner->GetModelResource()->Materials[LODResource->MaterialId];
}

void ZEModelMeshLOD::Deinitialize()
{
	Owner = NULL;
	OwnerMesh = NULL;
	RenderOrder.SetZero();
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
	RenderOrder.SetZero();
	VertexBuffer = NULL;
	LODResource = NULL;
	Material = NULL;
}

ZEModelMeshLOD::~ZEModelMeshLOD()
{
	Deinitialize();
}
