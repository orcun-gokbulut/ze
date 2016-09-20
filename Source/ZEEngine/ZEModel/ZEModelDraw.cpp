//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelDraw.cpp
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

#include "ZEModelDraw.h"

#include "ZEModel.h"
#include "ZEModelMesh.h"
#include "ZEModelMeshLOD.h"
#include "ZEMDResourceDraw.h"

#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRVertexBuffer.h"
#include "ZEGraphics/ZEGRIndexBuffer.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZERenderer/ZERNMaterial.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNShaderSlots.h"

void ZEMDInstanceTag::Update()
{
	if (Draw->GetLOD() == NULL)
		return;

	struct  
	{
		const ZEGRVertexBuffer* VertexBuffer;
		const ZEGRIndexBuffer* IndexBuffer;
		const ZERNMaterial* Material;
		ZEUInt32 Offset;
		ZEUInt32 Count;
	} HashData;

	HashData.VertexBuffer = Draw->GetLOD()->GetVertexBuffer();
	HashData.IndexBuffer = Draw->GetLOD()->GetIndexBuffer();
	HashData.Material = Draw->GetMaterial();
	HashData.Offset = Draw->Offset;
	HashData.Count = Draw->Count;

	Hash = GenerateHash(&HashData, sizeof(HashData));
}

bool ZEMDInstanceTag::Check(const ZERNInstanceTag* Other) const
{
	if (!ZERNInstanceTag::Check(Other))
		return false;

	const ZEMDInstanceTag* OtherMD = static_cast<const ZEMDInstanceTag*>(Other);
	return (Draw->GetLOD()->VertexBuffer == OtherMD->Draw->GetLOD()->VertexBuffer &&
		Draw->GetLOD()->IndexBuffer == OtherMD->Draw->GetLOD()->IndexBuffer &&
		Draw->Material == OtherMD->Draw->Material &&
		Draw->Offset == OtherMD->Draw->Offset &&
		Draw->Count == OtherMD->Draw->Count);
}

ZEModel* ZEModelDraw::GetModel()
{
	if (GetLOD() == NULL)
		return NULL;

	return GetLOD()->GetModel();
}

const ZEModel* ZEModelDraw::GetModel() const
{
	if (GetLOD() == NULL)
		return NULL;

	return GetLOD()->GetModel();
}

ZEModelMesh* ZEModelDraw::GetMesh()
{
	if (GetLOD() == NULL)
		return NULL;

	return GetLOD()->GetMesh();
}

const ZEModelMesh* ZEModelDraw::GetMesh() const
{
	if (GetLOD() == NULL)
		return NULL;

	return GetLOD()->GetMesh();
}

ZEModelMeshLOD* ZEModelDraw::GetLOD()
{
	return LOD;
}

const ZEModelMeshLOD* ZEModelDraw::GetLOD() const
{
	return LOD;
}

void ZEModelDraw::SetOffset(ZEUInt32 Offset)
{
	this->Offset = Offset;
	InstanceTag.Update();
}

ZEUInt32 ZEModelDraw::GetOffset() const
{
	return Offset;
}

void ZEModelDraw::SetCount(ZEUInt32 Count)
{
	this->Count = Count;
	InstanceTag.Update();
}

ZEUInt32 ZEModelDraw::GetCount() const
{
	return Count;
}

void ZEModelDraw::SetMaterial(ZEHolder<const ZERNMaterial> Material)
{
	if (this->Material == Material)
		return;

	this->Material = Material;
	InstanceTag.Update();
}

ZEHolder<const ZERNMaterial> ZEModelDraw::GetMaterial() const
{
	return Material;
}

void ZEModelDraw::ResetMaterial()
{
	SetMaterial(Resource->GetMaterial());
}

void ZEModelDraw::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	if (GetLOD() == NULL)
		return;

	if (GetLOD()->GetMesh() == NULL)
		return;

	if (GetLOD()->VertexBuffer.IsNull())
		return;

	if (GetLOD()->GetIndexType() != ZEMD_VIT_NONE && GetLOD()->IndexBuffer.IsNull())
		return;

	ZEGRContext* Context = Parameters->Context;
	Context->SetVertexBuffer(0, GetLOD()->VertexBuffer);

	if (GetLOD()->GetIndexType() != ZEMD_VIT_NONE)
		Context->SetIndexBuffer(GetLOD()->GetIndexBuffer());

	if (!GetMaterial()->SetupMaterial(Parameters->Context, Parameters->Stage))
		return;

	if (Command->Instances.GetCount() != 0)
	{
		ze_for_each(Instance, Command->Instances)
		{
			ZEMDInstanceTag* ModelInstance = static_cast<ZEMDInstanceTag*>(Instance->InstanceTag);

			Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, ModelInstance->Draw->GetMesh()->ConstantBuffer);
		
			if (GetLOD()->GetIndexType() == ZEMD_VIT_NONE)
				Context->Draw(GetCount(), GetOffset());
			else
				Context->DrawIndexed(GetCount(), GetOffset(), 0);
		}
	}

	if (GetLOD()->GetVertexType() == ZEMD_VT_SKINNED)
	{
		ZEGRConstantBuffer* ConstantBuffers[] = {GetLOD()->GetMesh()->ConstantBuffer, GetMesh()->GetModel()->ConstantBufferBoneTransforms};
		Context->SetConstantBuffers(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, 2, ConstantBuffers);
	}
	else
	{
		Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, GetMesh()->ConstantBuffer);
	}

	if (GetLOD()->GetIndexType() == ZEMD_VIT_NONE)
		Context->Draw(GetCount(), GetOffset());
	else
		Context->DrawIndexed(GetCount(), GetOffset(), 0);
	
	GetMaterial()->CleanupMaterial(Parameters->Context, Parameters->Stage);
}

ZEModelDraw::ZEModelDraw()
{
	LOD = NULL;
	Offset = 0;
	Count = 0;
	Resource = NULL;
	RenderCommand.Callback = ZEDelegateMethod(ZERNCommandCallback, ZEModelDraw, Render, this);
	InstanceTag.Draw = this;
	RenderCommand.InstanceTag = &InstanceTag;
}
