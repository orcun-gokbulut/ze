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
#include "ZEMath/ZEAngle.h"
#include "ZEPointer/ZEHolder.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNFixedMaterial.h"
#include "ZERenderer/ZERNShaderSlots.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRVertexBuffer.h"

void ZEModelMeshLOD::SetMaterial(ZEHolder<const ZERNMaterial> Material)
{
	this->Material = Material;
}

ZEHolder<const ZERNMaterial> ZEModelMeshLOD::GetMaterial() const
{
	return Material;
}

const ZEModelResourceMeshLOD* ZEModelMeshLOD::GetLODResource() const
{
	return LODResource;
}

ZEInt32 ZEModelMeshLOD::GetDrawStartDistance() const
{
	return DrawStartDistance;
}

ZEInt32 ZEModelMeshLOD::GetDrawEndDistance() const
{
	return DrawEndDistance;
}

bool ZEModelMeshLOD::IsSkinned() const
{
	return Skinned;
}

void ZEModelMeshLOD::Render(const ZERNRenderParameters* RenderParameters, const ZERNCommand* Command)
{
	ZEGRContext* Context = RenderParameters->Context;
	const ZERNStage* Stage = RenderParameters->Stage;
	if (!Material->SetupMaterial(Context, Stage))
	{
		zeError("Cannot setup material");
		return;
	}

	if (Skinned)
	{
		Owner->UpdateConstantBufferBoneTransforms();
		Context->SetConstantBuffers(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_BONE_TRANSFORMS, 1, Owner->ConstantBufferBoneTransforms.GetPointerToPointer());
		Context->SetVertexBuffers(0, 1, VertexBufferSkin.GetPointerToPointer());
	}
	else
	{
		Context->SetVertexBuffers(0, 1, VertexBuffer.GetPointerToPointer());
	}

	OwnerMesh->UpdateConstantBuffer();
	Context->SetConstantBuffers(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, 1, OwnerMesh->ConstantBuffer.GetPointerToPointer());

	Context->Draw(LODResource->VertexCount, 0);

	Material->CleanupMaterial(Context, Stage);
}

void ZEModelMeshLOD::Initialize(ZEModel* Model, ZEModelMesh* Mesh,  const ZEModelResourceMeshLOD* LODResource)
{
	Owner = Model;
	OwnerMesh = Mesh;
	this->LODResource = LODResource;
	DrawStartDistance = LODResource->LODStartDistance;
	DrawEndDistance = LODResource->LODEndDistance;
	Material = Model->GetModelResource()->GetMaterials()[(ZESize)LODResource->MaterialId].GetPointer();
	VertexBuffer = LODResource->VertexBuffer;
	VertexBufferNormals = LODResource->VertexBufferNormals;
	VertexBufferSkin = LODResource->VertexBufferSkin;
	IndexBuffer = LODResource->IndexBuffer;
	Skinned = !LODResource->VertexBufferSkin.IsNull();

	if (Skinned)
		static_cast<ZERNFixedMaterial*>(const_cast<ZERNMaterial*>(Material.GetPointer()))->SetSkinningEnabled(Skinned);
}

void ZEModelMeshLOD::Deinitialize()
{
	Owner = NULL;
	OwnerMesh = NULL;
	DrawStartDistance = 0;
	DrawEndDistance = 0;
	VertexBuffer = NULL;
	LODResource = NULL;
	Material = NULL;
	Skinned = false;
}

void ZEModelMeshLOD::ResetMaterial()
{
	SetMaterial(Owner->GetModelResource()->GetMaterials()[(ZESize)LODResource->MaterialId]);
}

ZEModelMeshLOD::ZEModelMeshLOD()
{
	Skinned = false;
	Owner = NULL;
	OwnerMesh = NULL;
	DrawStartDistance = 0;
	DrawEndDistance = 0;
	VertexBuffer = NULL;
	LODResource = NULL;
	Material = NULL;
}

ZEModelMeshLOD::~ZEModelMeshLOD()
{
	Deinitialize();
}
