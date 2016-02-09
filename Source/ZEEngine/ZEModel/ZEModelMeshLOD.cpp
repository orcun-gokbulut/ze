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

ZEHolder<const ZERNMaterial> ZEModelMeshLOD::GetMaterial()
{
	return Material;
}

const ZEModelResourceMeshLOD* ZEModelMeshLOD::GetLODResource()
{
	return LODResource;
}

ZEInt32 ZEModelMeshLOD::GetDrawStartDistance()
{
	return DrawStartDistance;
}

ZEInt32 ZEModelMeshLOD::GetDrawEndDistance()
{
	return DrawEndDistance;
}

bool ZEModelMeshLOD::IsSkinned()
{
	return Skinned;
}

void ZEModelMeshLOD::Render(const ZERNRenderParameters* RenderParameters, const ZERNCommand* Command)
{
	ZEGRContext* Context = RenderParameters->Context;
	Material->SetupMaterial(Context, RenderParameters->Stage);

	//Context->SetIndexBuffer(IndexBuffer);
	//Context->SetVertexBuffer(1, VertexBufferNormals);
	if (Skinned)
	{
		Context->SetVertexBuffers(0, 1, &VertexBufferSkin);
		Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_SKIN, ConstantBufferSkin);
		ConstantBuffer->SetData(&Owner->GetWorldTransform());
	}
	else
	{
		Context->SetVertexBuffers(0, 1, &VertexBuffer);
		ConstantBuffer->SetData(&OwnerMesh->GetWorldTransform());
	}

	Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, ConstantBuffer);
	Context->Draw((ZEUInt)LODResource->VertexCount, 0);

	Material->CleanupMaterial(Context, RenderParameters->Stage);
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

	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(ZEMatrix4x4));

	Skinned = LODResource->VertexBufferSkin.IsNull() ? false : true;
	if(Skinned)
	{
		ZESize BoneCount = LODResource->AffectingBoneIds.GetCount();
		ConstantBufferSkin = ZEGRConstantBuffer::Create(sizeof(ZEMatrix4x4) * BoneCount);

		ZEArray<ZEMatrix4x4> BoneTransforms;
		BoneTransforms.Resize(BoneCount);
		for (ZESize I = 0; I < BoneCount; I++)
			ZEMatrix4x4::Multiply(BoneTransforms[I], Owner->GetBones()[LODResource->AffectingBoneIds[I]].GetVertexTransform(), OwnerMesh->GetLocalTransform());

		ConstantBufferSkin->SetData(&BoneTransforms[0]);

		static_cast<ZERNFixedMaterial*>(const_cast<ZERNMaterial*>(Material.GetPointer()))->SetSkinningEnabled(Skinned);
	}
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
