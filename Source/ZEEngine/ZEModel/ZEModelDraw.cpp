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

#include "ZEGraphics/ZEGRBuffer.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZERenderer/ZERNMaterial.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNShaderSlots.h"
#include "ZERenderer/ZERNRenderer.h"

void ZEMDInstanceTag::Update()
{
	if (Draw->GetLOD() == NULL)
		return;

	struct  
	{
		const ZEGRBuffer* VertexBuffer;
		const ZEGRBuffer* IndexBuffer;
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
	const ZEMDInstanceTag* OtherMD = static_cast<const ZEMDInstanceTag*>(Other);
	return (Draw->GetLOD()->VertexBuffer == OtherMD->Draw->GetLOD()->VertexBuffer &&
		Draw->GetLOD()->IndexBuffer == OtherMD->Draw->GetLOD()->IndexBuffer &&
		Draw->Material == OtherMD->Draw->Material &&
		Draw->Offset == OtherMD->Draw->Offset &&
		Draw->Count == OtherMD->Draw->Count);
}

void ZEModelDraw::SetLOD(ZEModelMeshLOD* LOD)
{
	this->LOD = LOD;
	InstanceTag.Update();
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

void ZEModelDraw::SetColor(const ZEVector3& Color)
{
	if (Constants.Color == Color)
		return;

	Constants.Color = Color;

	DirtyConstants = true;
}

const ZEVector3& ZEModelDraw::GetColor() const
{
	return Constants.Color;
}

void ZEModelDraw::SetOpacity(float Opacity)
{
	if (Constants.Opacity == Opacity)
		return;

	Constants.Opacity = Opacity;

	DirtyConstants = true;
}

float ZEModelDraw::GetOpacity() const
{
	return Constants.Opacity;
}

void ZEModelDraw::SetLODTransition(bool LODTransition)
{
	if (Constants.LODTransition == (ZEBool32)LODTransition)
		return;

	Constants.LODTransition = (ZEBool32)LODTransition;

	DirtyConstants = true;
}

bool ZEModelDraw::GetLODTransition() const
{
	return (bool)Constants.LODTransition;
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
	Context->SetVertexBuffer(0, GetLOD()->GetVertexBuffer());

	if (GetLOD()->GetIndexType() != ZEMD_VIT_NONE)
		Context->SetIndexBuffer(GetLOD()->GetIndexBuffer());

	if (!GetMaterial()->SetupMaterial(Parameters->Context, Parameters->Stage, (Command->Instances.GetCount() > 0)))
		return;

	if (Command->Instances.GetCount() > 0)
	{
		ZERNRenderer* Renderer = Parameters->Renderer;
		static ZEUInt Index = 0;
		ZERNInstanceData* InstanceBuffer;
		if ((Index + Command->Instances.GetCount() + 1) >= Renderer->InstanceVertexBuffer->GetElementCount())
		{
			Index = 0;
			Renderer->InstanceVertexBuffer->Map(ZEGR_RMT_WRITE_DISCARD, reinterpret_cast<void**>(&InstanceBuffer));
		}
		else
		{
			Renderer->InstanceVertexBuffer->Map(ZEGR_RMT_WRITE_NO_OVERWRITE, reinterpret_cast<void**>(&InstanceBuffer));
		}

		ZEUInt InstanceOffset = Index;
		ze_for_each(Instance, Command->Instances)
		{
			const ZEModelDraw* InstanceDraw = static_cast<const ZEMDInstanceTag*>(Instance->InstanceTag)->Draw;
			InstanceBuffer[Index].WorldTransform = InstanceDraw->GetMesh()->GetWorldTransform();
			InstanceBuffer[Index].WorldTransformInverseTranspose = InstanceDraw->GetMesh()->GetInvWorldTransform().Transpose();
			InstanceBuffer[Index].DrawColor = ZEVector4(InstanceDraw->Constants.Color, InstanceDraw->Constants.Opacity);
			InstanceBuffer[Index].DrawLODTransition.w = InstanceDraw->Constants.LODTransition;
			Index++;
		}
		const ZEModelDraw* InstanceDraw = static_cast<const ZEMDInstanceTag*>(Command->InstanceTag)->Draw;
		InstanceBuffer[Index].WorldTransform = InstanceDraw->GetMesh()->GetWorldTransform();
		InstanceBuffer[Index].WorldTransformInverseTranspose = InstanceDraw->GetMesh()->GetInvWorldTransform().Transpose();
		InstanceBuffer[Index].DrawColor = ZEVector4(InstanceDraw->Constants.Color, InstanceDraw->Constants.Opacity);
		InstanceBuffer[Index].DrawLODTransition.w = InstanceDraw->Constants.LODTransition;
		Index++;
		Renderer->InstanceVertexBuffer->Unmap();

		Context->SetVertexBuffer(1, Renderer->InstanceVertexBuffer);

		if (GetLOD()->GetIndexType() == ZEMD_VIT_NONE)
			Context->DrawInstanced(GetCount(), GetOffset(), (ZEUInt)Command->Instances.GetCount() + 1, InstanceOffset);
		else
			Context->DrawIndexedInstanced(GetCount(), GetOffset(), 0, (ZEUInt)Command->Instances.GetCount() + 1, InstanceOffset);
	}
	else
	{
		GetMesh()->UpdateConstantBuffer();

		if (GetLOD()->GetVertexType() == ZEMD_VT_SKINNED)
		{
			GetModel()->UpdateConstantBufferBoneTransforms();

			ZEGRBuffer* ConstantBuffers[] = {GetMesh()->ConstantBuffer, GetModel()->ConstantBufferBoneTransforms};
			Context->SetConstantBuffers(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, 2, ConstantBuffers);
		}
		else
		{
			Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, GetMesh()->ConstantBuffer);
		}

		if (DirtyConstants)
		{
			if (ConstantBuffer == NULL)
				ConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, sizeof(Constants), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);

			ConstantBuffer->SetData(&Constants);
			DirtyConstants = false;
		}

		Context->SetConstantBuffer(ZEGR_ST_PIXEL, ZERN_SHADER_CONSTANT_DRAW_MATERIAL, ConstantBuffer);

		if (GetLOD()->GetIndexType() == ZEMD_VIT_NONE)
			Context->Draw(GetCount(), GetOffset());
		else
			Context->DrawIndexed(GetCount(), GetOffset(), 0);
	}
	
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
	DirtyConstants = true;

	//ConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, sizeof(Constants), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);

	Constants.Color = ZEVector3::One;
	Constants.Opacity = 1.0f;
	Constants.LODTransition = ZEGR_FALSE;
}
