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

bool ZERNCommandDraw::AddSubCommand(ZERNCommand* Command)
{
	if (Command->GetClass() != ZERNCommandDraw::Class())
		return false;
	
	ZERNCommandDraw* CommandDraw = static_cast<ZERNCommandDraw*>(Command);
	if (Material != CommandDraw->Material)
		return false;

	bool InstancingEnabled = Material->GetInstancingEnabled();
	
	ZELink<ZERNCommand>* DestLink = SubCommands.GetFirst();
	ZELink<ZERNCommand>* SrcLink = NULL;
	while ((SrcLink = CommandDraw->SubCommands.Pop()) != NULL)
	{
		bool InstanceFound = false;
		if (InstancingEnabled)
		{
			ZERNCommandDraw* SrcCommandDraw = static_cast<ZERNCommandDraw*>(SrcLink->GetItem());

			if (SrcCommandDraw->BoneConstantBuffer == NULL)
			{
				for (; DestLink != NULL; DestLink = DestLink->GetNext())
				{
					ZERNCommandDraw* DestCommandDraw = static_cast<ZERNCommandDraw*>(DestLink->GetItem());

					if (DestCommandDraw->BoneConstantBuffer != NULL)
						continue;

					if (DestCommandDraw->Geometry == SrcCommandDraw->Geometry)
					{
						DestCommandDraw->Instances.MergeBegin(SrcCommandDraw->Instances);
						InstanceFound = true;
						break;
					}
				}
			}
		}

		if (!InstanceFound)
			SubCommands.AddBegin(SrcLink);
	}

	return true;
}

void ZERNCommandDraw::Reset()
{
	ZERNCommand::Reset();
	
	zeCheckError(InstanceLink.GetInUse(), ZE_VOID, "Subcommand link in use");

	Instances.AddBegin(&InstanceLink);
}

void ZERNCommandDraw::Clear()
{
	ZERNCommand::Clear();
	Instances.Clear();
}

void ZERNCommandDraw::Execute(const ZERNRenderParameters* RenderParameters)
{
	ZEGRContext* Context = RenderParameters->Context;
	const ZERNStage* Stage = RenderParameters->Stage;

	if (!Material->SetupMaterial(Context, Stage))
		return;

	ze_for_each(Command, SubCommands)
	{
		ZERNCommandDraw* CommandDraw = static_cast<ZERNCommandDraw*>(Command.GetPointer());

		ZEUInt InstanceOffset = 0;
		ZEUInt InstanceCount = CommandDraw->Instances.GetCount();

		if (Material->GetInstancingEnabled())
		{
			static ZEUInt Index = 0;

			ZEGRBuffer* InstanceVertexBuffer = RenderParameters->Renderer->InstanceVertexBuffer;
			ZEGRResourceMapType MapType = ZEGR_RMT_WRITE_NO_OVERWRITE;
			if ((Index + InstanceCount) >= InstanceVertexBuffer->GetElementCount())
			{
				Index = 0;
				MapType = ZEGR_RMT_WRITE_DISCARD;
			}

			InstanceOffset = Index;

			ZERNInstanceData* InstanceBuffer;
			InstanceVertexBuffer->Map(MapType, reinterpret_cast<void**>(&InstanceBuffer));

			ze_for_each(Instance, CommandDraw->Instances)
				InstanceBuffer[Index++] = Instance->InstanceData;

			InstanceVertexBuffer->Unmap();

			Context->SetVertexBuffer(1, InstanceVertexBuffer);
		}
		else
		{
			Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, CommandDraw->TransformConstantBuffer);
			Context->SetConstantBuffer(ZEGR_ST_PIXEL, ZERN_SHADER_CONSTANT_DRAW_MATERIAL, CommandDraw->DrawConstantBuffer);
			if (CommandDraw->BoneConstantBuffer != NULL)
				Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_BONE_TRANSFORMS, CommandDraw->BoneConstantBuffer);
		}

		const ZERNGeometry* Geometry = CommandDraw->Geometry;
		Context->SetVertexBuffer(0, Geometry->VertexBuffer);
		if (Geometry->IndexBuffer != NULL)
		{
			Context->SetIndexBuffer(Geometry->IndexBuffer);
			Context->DrawIndexedInstanced(Geometry->IndexCount, Geometry->IndexOffset, Geometry->VertexOffset, InstanceCount, InstanceOffset);
		}
		else
		{
			Context->DrawInstanced(Geometry->VertexCount, Geometry->VertexOffset, InstanceCount, InstanceOffset);
		}
	}

	Material->CleanupMaterial(Context, Stage);
}

ZERNCommandDraw::ZERNCommandDraw() : InstanceLink(this)
{

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
	SetGeometry(Resource->GetGeometry());

	return true;
}

bool ZEModelDraw::Unload()
{
	Resource = NULL;
	ConstantBuffer.Release();

	DirtyConstants = true;

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
}

ZEUInt32 ZEModelDraw::GetIndexCount() const
{
	return IndexCount;
}

void ZEModelDraw::SetColor(const ZEVector3& Color)
{
	if (this->Color == Color)
		return;

	this->Color = Color;

	DirtyConstants = true;
}

const ZEVector3& ZEModelDraw::GetColor() const
{
	return Color;
}

void ZEModelDraw::SetOpacity(float Opacity)
{
	if (this->Opacity == Opacity)
		return;

	this->Opacity = Opacity;

	DirtyConstants = true;
}

float ZEModelDraw::GetOpacity() const
{
	return Opacity;
}

void ZEModelDraw::SetMaterial(const ZERNMaterial* Material)
{
	if (this->Material == Material)
		return;

	this->Material = Material;
}

const ZERNMaterial* ZEModelDraw::GetMaterial() const
{
	return Material;
}

void ZEModelDraw::ResetMaterial()
{
	SetMaterial(Resource->GetMaterial());
}

void ZEModelDraw::SetGeometry(const ZERNGeometry* Geometry)
{
	this->Geometry = Geometry;
}

const ZERNGeometry* ZEModelDraw::GetGeometry() const
{
	return Geometry;
}

const ZEGRBuffer* ZEModelDraw::GetConstantBuffer() const
{
	if (DirtyConstants)
	{
		if (ConstantBuffer == NULL)
			ConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, sizeof(Constants), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);

		Constants.Color = GetColor() * GetLOD()->GetColor();
		Constants.Opacity = GetOpacity() * GetLOD()->GetOpacity();
		Constants.LODTransition = GetLOD()->GetLODTransition();

		ConstantBuffer->SetData(&Constants);
		DirtyConstants = false;
	}

	return ConstantBuffer;
}

bool ZEModelDraw::PreRender(const ZERNPreRenderParameters* PreRenderParameters)
{
	if (GetMaterial() == NULL || GetGeometry() == NULL || !GetMaterial()->IsLoaded())
		return false;

	if (!GetMaterial()->GetInstancingEnabled())
		GetMesh()->UpdateConstantBuffer();

	if (GetLOD()->GetVertexType() == ZEMD_VT_SKINNED)
		GetModel()->UpdateConstantBufferBoneTransforms();

	ZERNInstanceData InstanceData;
	InstanceData.WorldTransform = GetMesh()->GetWorldTransform();
	InstanceData.WorldTransformInverseTranspose = GetMesh()->GetInvWorldTransform().Transpose();
	InstanceData.DrawColor = ZEVector4(GetColor() * GetLOD()->GetColor(), GetOpacity() * GetLOD()->GetOpacity());
	InstanceData.DrawLODTransition.w = GetLOD()->GetLODTransition();

	if (PreRenderParameters->Type != ZERN_RT_SHADOW)
	{
		RenderCommand.Entity = GetModel();
		RenderCommand.Order = (GetMaterial()->GetStageMask() & ZERN_STAGE_FORWARD_TRANSPARENT) ? -GetMesh()->DrawOrder : GetMesh()->DrawOrder;
		RenderCommand.TransformConstantBuffer = GetMesh()->ConstantBuffer;
		RenderCommand.DrawConstantBuffer = GetConstantBuffer();
		RenderCommand.BoneConstantBuffer = GetModel()->ConstantBufferBoneTransforms;
		RenderCommand.StageMask = GetMaterial()->GetStageMask();
		RenderCommand.Material = GetMaterial();
		RenderCommand.Geometry = GetGeometry();
		RenderCommand.InstanceData = InstanceData;

		RenderCommand.Reset();
		PreRenderParameters->CommandList->AddCommand(&RenderCommand);
	}
	else
	{
		RenderCommandShadow.Entity = GetModel();
		RenderCommandShadow.Order = (GetMaterial()->GetStageMask() & ZERN_STAGE_FORWARD_TRANSPARENT) ? -GetMesh()->DrawOrder : GetMesh()->DrawOrder;
		RenderCommandShadow.TransformConstantBuffer = GetMesh()->ConstantBuffer;
		RenderCommandShadow.DrawConstantBuffer = GetConstantBuffer();
		RenderCommandShadow.BoneConstantBuffer = GetModel()->ConstantBufferBoneTransforms;
		RenderCommandShadow.StageMask = GetMaterial()->GetStageMask();
		RenderCommandShadow.Material = GetMaterial();
		RenderCommandShadow.Geometry = GetGeometry();
		RenderCommandShadow.InstanceData = InstanceData;

		RenderCommandShadow.Reset();
		PreRenderParameters->CommandList->AddCommand(&RenderCommandShadow);
	}

	return true;
}

ZEModelDraw::ZEModelDraw()
{
	LOD = NULL;
	VertexOffset = 0;
	VertexCount = 0;
	IndexOffset = 0;
	IndexCount = 0;
	Resource = NULL;
	DirtyConstants = true;

	Color = ZEVector3::One;
	Opacity = 1.0f;

	Constants.Color = ZEVector3::One;
	Constants.Opacity = 1.0f;
	Constants.LODTransition = ZEGR_FALSE;
}

ZEModelDraw::~ZEModelDraw()
{
	Unload();
}
