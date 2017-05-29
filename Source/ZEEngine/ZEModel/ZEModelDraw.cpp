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

bool ZEMDInstanceTag::Update()
{
	if (!Dirty)
		return true;
	
	Dirty = false;

	if (Draw == NULL || Draw->GetModel() == NULL)
		return false;

	struct  
	{
		const ZEGRBuffer* VertexBuffer;
		const ZEGRBuffer* IndexBuffer;
		const ZERNMaterial* Material;
		ZEUInt32 VertexOffset;
		ZEUInt32 VertexCount;
		ZEUInt32 IndexOffset;
		ZEUInt32 IndexCount;
	} HashData;

	HashData.VertexBuffer = Draw->GetModel()->GetVertexBuffer(Draw->GetLOD()->GetVertexType());
	HashData.IndexBuffer = Draw->GetModel()->GetIndexBuffer(Draw->GetLOD()->GetIndexType());
	HashData.Material = Draw->GetMaterial();
	HashData.VertexOffset = Draw->GetVertexOffset();
	HashData.VertexCount = Draw->GetVertexCount();
	HashData.IndexOffset = Draw->GetIndexOffset();
	HashData.IndexCount = Draw->GetIndexCount();

	Hash = GenerateHash(&HashData, sizeof(HashData));

	return true;
}

bool ZEMDInstanceTag::Check(const ZERNInstanceTag* Other) const
{
	const ZEMDInstanceTag* OtherMD = static_cast<const ZEMDInstanceTag*>(Other);
	return (Draw->GetModel()->GetVertexBuffer(Draw->GetLOD()->GetVertexType()) == OtherMD->Draw->GetModel()->GetVertexBuffer(OtherMD->Draw->GetLOD()->GetVertexType()) &&
		Draw->GetModel()->GetIndexBuffer(Draw->GetLOD()->GetIndexType()) == OtherMD->Draw->GetModel()->GetIndexBuffer(OtherMD->Draw->GetLOD()->GetIndexType()) &&
		Draw->GetMaterial() == OtherMD->Draw->GetMaterial() &&
		Draw->GetVertexOffset() == OtherMD->Draw->GetVertexOffset() &&
		Draw->GetVertexCount() == OtherMD->Draw->GetVertexCount() && 
		Draw->GetIndexOffset() == OtherMD->Draw->GetIndexOffset() &&
		Draw->GetIndexCount() == OtherMD->Draw->GetIndexCount());
}

bool ZEModelDraw::Load(const ZEMDResourceDraw* Resource)
{
	if (Resource == NULL)
		return false;

	this->Resource = Resource;

	SetMaterial(Resource->GetMaterial());
	SetVertexOffset(Resource->GetVertexOffset());
	SetVertexCount(Resource->GetVertexCount());
	SetIndexOffset(Resource->GetIndexOffset());
	SetIndexCount(Resource->GetIndexCount());

	return InstanceTag.Update();
}

const ZEMDInstanceTag* ZEModelDraw::GetInstanceTag() const
{
	if (!InstanceTag.Update())
		return NULL;
	
	if (GetLOD()->GetVertexType() == ZEMD_VT_SKINNED)
		return NULL;
	
	return &InstanceTag;
}

void ZEModelDraw::SetLOD(ZEModelMeshLOD* LOD)
{
	if (this->LOD == LOD)
		return;

	this->LOD = LOD;

	InstanceTag.Dirty = true;
}

bool ZEModelDraw::Unload()
{
	Resource = NULL;
	ConstantBuffer.Release();

	DirtyConstants = true;
	InstanceTag.Dirty = true;

	return true;
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

void ZEModelDraw::SetVertexOffset(ZEUInt32 Offset)
{
	if (VertexOffset == Offset)
		return;

	VertexOffset = Offset;

	InstanceTag.Dirty = true;
}

ZEUInt32 ZEModelDraw::GetVertexOffset() const 
{
	return VertexOffset;
}

void ZEModelDraw::SetVertexCount(ZEUInt32 Count)
{
	if (VertexCount == Count)
		return;

	VertexCount = Count;

	InstanceTag.Dirty = true;
}

ZEUInt32 ZEModelDraw::GetVertexCount() const 
{
	return VertexCount;
}

void ZEModelDraw::SetIndexOffset(ZEUInt32 Offset)
{
	if (IndexOffset == Offset)
		return;

	IndexOffset = Offset;
	
	InstanceTag.Dirty = true;
}

ZEUInt32 ZEModelDraw::GetIndexOffset() const
{
	return IndexOffset;
}

void ZEModelDraw::SetIndexCount(ZEUInt32 Count)
{
	if (IndexCount == Count)
		return;

	IndexCount = Count;

	InstanceTag.Dirty = true;
}

ZEUInt32 ZEModelDraw::GetIndexCount() const
{
	return IndexCount;
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

void ZEModelDraw::SetMaterial(const ZERNMaterial* Material)
{
	if (this->Material == Material)
		return;

	this->Material = Material;

	InstanceTag.Dirty = true;
}

const ZERNMaterial* ZEModelDraw::GetMaterial() const
{
	return Material;
}

void ZEModelDraw::ResetMaterial()
{
	SetMaterial(Resource->GetMaterial());
}

void ZEModelDraw::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	if (GetMesh() == NULL || GetModel() == NULL || GetMaterial() == NULL)
		return;
	
	if (!GetMaterial()->SetupMaterial(Parameters->Context, Parameters->Stage, Command->Instances.GetCount() > 0, GetLODTransition()))
		return;

	ZEGRContext* Context = Parameters->Context;
	Context->SetVertexBuffer(0, GetModel()->GetVertexBuffer(GetLOD()->GetVertexType()));
	Context->SetIndexBuffer(GetModel()->GetIndexBuffer(GetLOD()->GetIndexType()));

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
			Context->DrawInstanced(GetVertexCount(), GetVertexOffset(), (ZEUInt)Command->Instances.GetCount() + 1, InstanceOffset);
		else
			Context->DrawIndexedInstanced(GetIndexCount(), GetIndexOffset(), GetVertexOffset(), (ZEUInt)Command->Instances.GetCount() + 1, InstanceOffset);
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
			Context->Draw(GetVertexCount(), GetVertexOffset());
		else
			Context->DrawIndexed(GetIndexCount(), GetIndexOffset(), GetVertexOffset());
	}
	
	GetMaterial()->CleanupMaterial(Parameters->Context, Parameters->Stage);
}

ZEModelDraw::ZEModelDraw()
{
	LOD = NULL;
	VertexOffset = 0;
	VertexCount = 0;
	IndexOffset = 0;
	IndexCount = 0;
	Resource = NULL;
	RenderCommand.Callback = ZEDelegateMethod(ZERNCommandCallback, ZEModelDraw, Render, this);
	InstanceTag.Draw = this;
	InstanceTag.Dirty = true;
	RenderCommand.InstanceTag = &InstanceTag;
	DirtyConstants = true;

	Constants.Color = ZEVector3::One;
	Constants.Opacity = 1.0f;
	Constants.LODTransition = ZEGR_FALSE;
}

ZEModelDraw::~ZEModelDraw()
{
	Unload();
}
