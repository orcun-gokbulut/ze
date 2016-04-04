//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageDebug.cpp
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

#include "ZERNStageDebug.h"

#include "ZERNStageID.h"
#include "ZEDS/ZEString.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"
#include "ZEGraphics/ZEGRShaderCompileOptions.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRVertexBuffer.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZEModel/ZEModelResource.h"
#include "ZERNCommand.h"
#include "ZEGame/ZEEntity.h"
#include "ZEModel/ZEModel.h"

#define ZERN_SDDF_SHADERS			1
#define ZERN_SDDF_RENDER_STATES		2
#define ZERN_SDDF_CONSTANT_BUFFERS	4

ZEGRVertexLayout GetPositionVertexLayout()
{
	static ZEGRVertexLayout VertexLayout;
	if (VertexLayout.GetElementCount() == 0)
	{
		ZEGRVertexElement ElementArray[] = 
		{
			{ZEGR_VES_POSITION, 0, ZEGR_VET_FLOAT3, 0, 0, ZEGR_VU_PER_VERTEX, 0},
		};

		VertexLayout.SetElements(ElementArray, 1);
	}

	return VertexLayout;
}

bool ZERNStageDebug::UpdateShaders()
{
	if (!DirtyFlags.GetFlags(ZERN_SDDF_SHADERS))
		return true;

	ZEGRShaderCompileOptions Options;

	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNDebug.hlsl";
	Options.Model = ZEGR_SM_5_0;

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNDebug_VertexShader_Main";
	VertexShader = ZEGRShader::Compile(Options);
	zeCheckError(VertexShader == NULL, false, "Cannot set vertex shader.");

	Options.Type = ZEGR_ST_GEOMETRY;
	Options.EntryPoint = "ZERNDebug_GeometryShader_Main";
	GeometryShader = ZEGRShader::Compile(Options);
	zeCheckError(GeometryShader == NULL, false, "Cannot set geometry shader.");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNDebug_PixelShader_Main";
	PixelShader = ZEGRShader::Compile(Options);
	zeCheckError(PixelShader == NULL, false, "Cannot set pixel shader.");

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNDebug_BoundingBox_VertexShader_Main";
	BoundingBoxVertexShader = ZEGRShader::Compile(Options);
	zeCheckError(VertexShader == NULL, false, "Cannot set vertex shader.");

	Options.Type = ZEGR_ST_GEOMETRY;
	Options.EntryPoint = "ZERNDebug_BoundingBox_GeometryShader_Main";
	BoundingBoxGeometryShader = ZEGRShader::Compile(Options);
	zeCheckError(GeometryShader == NULL, false, "Cannot set geometry shader.");

	DirtyFlags.UnraiseFlags(ZERN_SDDF_SHADERS);
	DirtyFlags.RaiseFlags(ZERN_SDDF_RENDER_STATES);

	return true;
}

bool ZERNStageDebug::UpdateRenderStates()
{
	if (!DirtyFlags.GetFlags(ZERN_SDDF_RENDER_STATES))
		return true;

	ZEGRRenderState RenderState = ZERNStageDebug::GetRenderState();
	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);
	RenderState.SetVertexLayout(*ZEModelVertex::GetVertexLayout());

	RenderState.SetShader(ZEGR_ST_VERTEX, VertexShader);
	RenderState.SetShader(ZEGR_ST_GEOMETRY, GeometryShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, PixelShader);

	RenderStateData = RenderState.Compile();
	zeCheckError(RenderStateData == NULL, false, "Cannot set debug render state.");

	RenderState.SetPrimitiveType(ZEGR_PT_LINE_LIST);
	RenderState.SetVertexLayout(GetPositionVertexLayout());

	RenderState.SetShader(ZEGR_ST_VERTEX, BoundingBoxVertexShader);
	RenderState.SetShader(ZEGR_ST_GEOMETRY, BoundingBoxGeometryShader);

	BoundingBoxRenderStateData = RenderState.Compile();
	zeCheckError(BoundingBoxRenderStateData == NULL, false, "Cannot set debug render state.");

	DirtyFlags.UnraiseFlags(ZERN_SDDF_RENDER_STATES);

	return true;
}

bool ZERNStageDebug::UpdateConstantBuffers()
{
	if (!DirtyFlags.GetFlags(ZERN_SDDF_CONSTANT_BUFFERS))
		return true;

	ConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZERN_SDDF_CONSTANT_BUFFERS);

	return true;
}

bool ZERNStageDebug::Update()
{
	if (!UpdateShaders())
		return false;

	if (!UpdateRenderStates())
		return false;

	if (!UpdateConstantBuffers())
		return false;

	return true;
}

#include "ZEModel/ZEModel.h"
#include "ZEModel/ZEModelMesh.h"
#include "ZEModel/ZEModelMeshLOD.h"
#include "ZEDS/ZEArray.h"

bool ZERNStageDebug::SetupBoundingBoxVertexBuffer()
{
	const ZEList2<ZERNCommand>& Commands = GetCommands();

	ZESmartArray<ZEVector3> Vertices;
	ze_for_each(Command, Commands)
	{
		ZEEntity* Entity = Command->Entity;
		//if (Entity->GetClass() == ZEModel::Class())
		{
			ZEAABBox Box = Entity->GetWorldBoundingBox();
			Vertices.Add(Box.Min);
			Vertices.Add(Box.Max);
		}
	}

	ZESize VertexCount = Vertices.GetCount();
	if (VertexCount == 0)
		return false;

	if (BoundingBoxVertexBuffer == NULL || 
		BoundingBoxVertexBuffer->GetVertexCount() != VertexCount)
	{
		BoundingBoxVertexBuffer.Release();
		BoundingBoxVertexBuffer = ZEGRVertexBuffer::Create(VertexCount, sizeof(ZEVector3));
		
		void* Data;
		BoundingBoxVertexBuffer->Lock(&Data);
		memcpy(Data, &Vertices[0], sizeof(ZEVector3) * VertexCount);
		BoundingBoxVertexBuffer->Unlock();
	}

	return true;
}

bool ZERNStageDebug::InitializeSelf()
{
	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(Constants));

	return true;
}

void ZERNStageDebug::DeinitializeSelf()
{
	VertexShader.Release();
	GeometryShader.Release();
	PixelShader.Release();
	RenderStateData.Release();
	ConstantBuffer.Release();

	BoundingBoxVertexShader.Release();
	BoundingBoxGeometryShader.Release();
	BoundingBoxRenderStateData.Release();
	BoundingBoxVertexBuffer.Release();

	DepthMap.Release();
}

ZEInt ZERNStageDebug::GetId() const
{
	return ZERN_STAGE_DEBUG;
}

const ZEString& ZERNStageDebug::GetName() const
{
	static const ZEString Name = "Debug";
	return Name;
}

void ZERNStageDebug::SetShowNormalVectors(bool ShowNormalVectors)
{
	if (Constants.ShowNormalVectors == (ZEBool32)ShowNormalVectors)
		return;

	Constants.ShowNormalVectors = (ZEBool32)ShowNormalVectors;

	DirtyFlags.RaiseFlags(ZERN_SDDF_CONSTANT_BUFFERS);
}

bool ZERNStageDebug::GetShowNormalVectors() const
{
	return (bool)Constants.ShowNormalVectors;
}

void ZERNStageDebug::SetShowBoundingBox(bool ShowBoundingBox)
{
	if (Constants.ShowBoundingBox == (ZEBool32)ShowBoundingBox)
		return;

	Constants.ShowBoundingBox = (ZEBool32)ShowBoundingBox;

	DirtyFlags.RaiseFlags(ZERN_SDDF_CONSTANT_BUFFERS);
}

bool ZERNStageDebug::GetShowBoundingBox() const
{
	return (bool)Constants.ShowBoundingBox;
}

void ZERNStageDebug::SetShowWireframe(bool ShowWireframe)
{
	if (Constants.ShowWireframe == (ZEBool32)ShowWireframe)
		return;

	Constants.ShowWireframe = (ZEBool32)ShowWireframe;

	DirtyFlags.RaiseFlags(ZERN_SDDF_CONSTANT_BUFFERS);
}

bool ZERNStageDebug::GetShowWireframe() const
{
	return (bool)Constants.ShowWireframe;
}

void ZERNStageDebug::SetCullBackface(bool CullBackface)
{
	if (Constants.CullBackface == (ZEBool32)CullBackface)
		return;

	Constants.CullBackface = (ZEBool32)CullBackface;

	DirtyFlags.RaiseFlags(ZERN_SDDF_CONSTANT_BUFFERS);
}

bool ZERNStageDebug::GetCullBackface() const
{
	return (bool)Constants.CullBackface;
}

bool ZERNStageDebug::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	if (!Update())
		return false;

	ZEGRRenderTarget* RenderTarget = GetRenderer()->GetOutputRenderTarget();
	if (RenderTarget == NULL)
		return false;

	ZEUInt Width = RenderTarget->GetWidth();
	ZEUInt Height = RenderTarget->GetHeight();

	if (DepthMap == NULL || 
		DepthMap->GetWidth() != Width || DepthMap->GetHeight() != Height)
	{
		DepthMap.Release();
		DepthMap = ZEGRTexture2D::CreateInstance(Width, Height, 1, ZEGR_TF_D24_UNORM_S8_UINT, ZEGR_RU_GPU_READ_WRITE_CPU_WRITE, ZEGR_RBF_DEPTH_STENCIL);
	}

	const ZEGRDepthStencilBuffer* DepthStencilBuffer = DepthMap->GetDepthStencilBuffer();

	Context->ClearDepthStencilBuffer(DepthStencilBuffer, true, false, 0.0f, 0x00);

	Context->SetConstantBuffer(ZEGR_ST_GEOMETRY, 8, ConstantBuffer);
	Context->SetRenderTargets(1, &RenderTarget, DepthStencilBuffer);
	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, Width, Height));

	if (Constants.ShowBoundingBox)
	{
		if (SetupBoundingBoxVertexBuffer())
		{
			Context->SetRenderState(BoundingBoxRenderStateData);
			Context->SetVertexBuffers(0, 1, BoundingBoxVertexBuffer.GetPointerToPointer());
			Context->Draw(BoundingBoxVertexBuffer->GetVertexCount(), 0);

			Context->SetVertexBuffers(0, 0, NULL);
			CleanUp(Context);
		}

		return false;
	}
	else
	{
		Context->SetRenderState(RenderStateData);
	}

	return true;
}

void ZERNStageDebug::CleanUp(ZEGRContext* Context)
{
	Context->SetConstantBuffer(ZEGR_ST_GEOMETRY, 8, NULL);
	Context->SetRenderTargets(0, NULL, NULL);

	ZERNStage::CleanUp(Context);
}

ZERNStageDebug::ZERNStageDebug()
{
	DirtyFlags.RaiseAll();

	Constants.ShowNormalVectors = false;
	Constants.ShowBoundingBox = false;
	Constants.ShowWireframe = true;
	Constants.CullBackface = false;
}
