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
#include "ZEGraphics\ZERenderer.h"
#include "ZEGame\ZEDrawParameters.h"

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

void ZEModelMeshLOD::Draw(ZEDrawParameters* DrawParameters)
{
	if (VertexBuffer == NULL)
		return;

	if (Skinned)
	{
		RenderOrder.BoneTransforms.SetCount(LODResource->AffectingBoneIds.GetCount());
		for (size_t I = 0; I < LODResource->AffectingBoneIds.GetCount(); I++)
			ZEMatrix4x4::Multiply(RenderOrder.BoneTransforms[I], this->OwnerMesh->GetLocalTransform(), Owner->GetBones()[LODResource->AffectingBoneIds[I]].GetVertexTransform());

		RenderOrder.WorldMatrix = Owner->GetWorldTransform();
	}
	else
		RenderOrder.WorldMatrix = OwnerMesh->GetWorldTransform();

	RenderOrder.Lights.Clear();
	RenderOrder.Lights.MassAdd(DrawParameters->Lights.GetConstCArray(), DrawParameters->Lights.GetCount());
	DrawParameters->Renderer->AddToRenderList(&RenderOrder);

	/*NormalRenderOrder.WorldMatrix = OwnerMesh->GetWorldTransform();
	DrawParameters->Renderer->AddToRenderList(&NormalRenderOrder);*/
}

#include "ZEGraphics\ZESimpleMaterial.h"
void ZEModelMeshLOD::Initialize(ZEModel* Model, ZEModelMesh* Mesh,  const ZEModelResourceMeshLOD* LODResource)
{
	Owner = Model;
	OwnerMesh = Mesh;
	this->LODResource = LODResource;

	Skinned = LODResource->Vertices.GetCount() == 0 ? true : false;

	RenderOrder.SetZero();
	RenderOrder.Flags = ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM | ZE_ROF_ENABLE_WORLD_TRANSFORM | ZE_ROF_ENABLE_Z_CULLING | (Skinned ? ZE_ROF_SKINNED : 0);
	RenderOrder.PrimitiveType = ZE_ROPT_TRIANGLE;
	RenderOrder.VertexBuffer = VertexBuffer = LODResource->GetSharedVertexBuffer();
	RenderOrder.PrimitiveCount = Skinned ? LODResource->SkinnedVertices.GetCount() / 3: LODResource->Vertices.GetCount() / 3;
	RenderOrder.VertexDeclaration = Skinned ? ZESkinnedModelVertex::GetVertexDeclaration() : ZEModelVertex::GetVertexDeclaration();
	RenderOrder.Material = Owner->GetModelResource()->Materials[LODResource->MaterialId];

	/*
	if (NormalMaterial == NULL)
		NormalMaterial = ZESimpleMaterial::CreateInstance();

	float NormalSize = 0.025f;
	bool ShowNormal = true;
	bool ShowTangent = true;
	bool ShowBinormal = true;

	if (Skinned)
		for (size_t I = 0; I < LODResource->SkinnedVertices.GetCount(); I++)
		{
			// Normal
			if (ShowNormal)
			{
				NormalCanvas.SetColor(ZEVector4(0.0f, 1.0f, 0.0f, 0.0f));
				NormalCanvas.AddLine(LODResource->SkinnedVertices[I].Position, LODResource->SkinnedVertices[I].Position + LODResource->SkinnedVertices[I].Normal * NormalSize);
			}

			//Binormal
			if (ShowBinormal)
			{
				NormalCanvas.SetColor(ZEVector4(1.0f, 0.0f, 0.0f, 0.0f));
				NormalCanvas.AddLine(LODResource->SkinnedVertices[I].Position, LODResource->SkinnedVertices[I].Position + LODResource->SkinnedVertices[I].Binormal * NormalSize);
			}

			//Tangent
			if (ShowTangent)
			{
				NormalCanvas.SetColor(ZEVector4(0.0f, 0.0f, 1.0f, 0.0f));
				NormalCanvas.AddLine(LODResource->SkinnedVertices[I].Position, LODResource->SkinnedVertices[I].Position + LODResource->SkinnedVertices[I].Tangent * NormalSize);
			}
		}
	else
		for (size_t I = 0; I < LODResource->Vertices.GetCount(); I++)
		{
			// Normal
			if (ShowNormal)
			{
				NormalCanvas.SetColor(ZEVector4(0.0f, 1.0f, 0.0f, 0.0f));
				NormalCanvas.AddLine(LODResource->Vertices[I].Position, LODResource->Vertices[I].Position + LODResource->Vertices[I].Normal * NormalSize);
			}

			//Binormal
			if (ShowBinormal)
			{
				NormalCanvas.SetColor(ZEVector4(1.0f, 0.0f, 0.0f, 0.0f));
				NormalCanvas.AddLine(LODResource->Vertices[I].Position, LODResource->Vertices[I].Position + LODResource->Vertices[I].Binormal * NormalSize);
			}

			//Tangent
			if (ShowTangent)
			{
				NormalCanvas.SetColor(ZEVector4(0.0f, 0.0f, 1.0f, 0.0f));
				NormalCanvas.AddLine(LODResource->Vertices[I].Position, LODResource->Vertices[I].Position + LODResource->Vertices[I].Tangent * NormalSize);
			}
		}

		NormalRenderOrder.SetZero();
		NormalRenderOrder.Flags = ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM | ZE_ROF_ENABLE_WORLD_TRANSFORM | ZE_ROF_ENABLE_Z_CULLING | (Skinned ? ZE_ROF_SKINNED : 0);
		NormalRenderOrder.PrimitiveType = ZE_ROPT_LINE;
		NormalRenderOrder.VertexBuffer = &NormalCanvas;
		NormalRenderOrder.PrimitiveCount = NormalCanvas.Vertices.GetCount() / 2;
		NormalRenderOrder.VertexDeclaration = ZECanvasVertex::GetVertexDeclaration();
		NormalRenderOrder.Material = NormalMaterial;*/
}

void ZEModelMeshLOD::Deinitialize()
{
	if (NormalMaterial != NULL)
	{
		NormalMaterial->Destroy();
		NormalMaterial = NULL;
	}

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
	NormalMaterial = NULL;
}

ZEModelMeshLOD::~ZEModelMeshLOD()
{
	Deinitialize();
}



