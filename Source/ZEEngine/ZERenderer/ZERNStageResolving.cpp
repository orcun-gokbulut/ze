//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageResolving.cpp
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

#include "ZERNStageResolving.h"

#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZERNStageID.h"

#define ZERN_SRDF_RENDER_STATE		1
#define ZERN_SRDF_CONSTANT_BUFFER	2

bool ZERNStageResolving::InitializeInternal()
{
	if (!ZERNStage::InitializeInternal())
		return false;

	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(Constants));

	return Update();
}

bool ZERNStageResolving::DeinitializeInternal()
{
	ResolveGBuffersRenderStateData.Release();
	ResolveCustomRenderStateData.Release();

	ConstantBuffer.Release();

	ResolvedInputTexture.Release();
	ResolvedNormalTexture.Release();
	ResolvedDepthTexture.Release();

	InputTexture = NULL;
	NormalTexture = NULL;
	DepthTexture = NULL;
	EmissiveTexture = NULL;
	DiffuseTexture = NULL;

	return ZERNStage::DeinitializeInternal();
}

bool ZERNStageResolving::UpdateRenderStates()
{
	if (!DirtyFlags.GetFlags(ZERN_SRDF_RENDER_STATE))
		return true;

	ZEGRShaderCompileOptions ShaderCompileOptions;
	ShaderCompileOptions.Definitions.Add(ZEGRShaderDefinition("SAMPLE_COUNT", ZEGRGraphicsModule::SAMPLE_COUNT));
	ShaderCompileOptions.Model = ZEGR_SM_5_0;
	ShaderCompileOptions.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNResolving.hlsl";

	ShaderCompileOptions.Type = ZEGR_ST_VERTEX;
	ShaderCompileOptions.EntryPoint = "ZERNScreenCover_VertexShader_Position";
	ZEHolder<ZEGRShader> ScreenCoverVertexShader = ZEGRShader::Compile(ShaderCompileOptions);
	zeCheckError(ScreenCoverVertexShader == NULL, false, "Cannot compile vertex shader");

	ShaderCompileOptions.Type = ZEGR_ST_PIXEL;
	ShaderCompileOptions.EntryPoint = "ZERNResolving_ResolveCustom_PixelShader_Main";
	ZEHolder<ZEGRShader> ResolveCustomPixelShader = ZEGRShader::Compile(ShaderCompileOptions);
	zeCheckError(ResolveCustomPixelShader == NULL, false, "Cannot compile pixel shader");

	ShaderCompileOptions.Type = ZEGR_ST_PIXEL;
	ShaderCompileOptions.EntryPoint = "ZERNResolving_ResolveGBuffers_PixelShader_Main";
	ZEHolder<ZEGRShader> ResolveGBuffersPixelShader = ZEGRShader::Compile(ShaderCompileOptions);
	zeCheckError(ResolveGBuffersPixelShader == NULL, false, "Cannot compile pixel shader");

	ShaderCompileOptions.Type = ZEGR_ST_PIXEL;
	ShaderCompileOptions.EntryPoint = "ZERNResolving_ResolveAllGBuffers_PixelShader_Main";
	ZEHolder<ZEGRShader> ResolveAllGBuffersPixelShader = ZEGRShader::Compile(ShaderCompileOptions);
	zeCheckError(ResolveAllGBuffersPixelShader == NULL, false, "Cannot compile pixel shader");

	ZEGRRenderState RenderState;
	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);
	RenderState.SetShader(ZEGR_ST_VERTEX, ScreenCoverVertexShader);

	RenderState.SetShader(ZEGR_ST_PIXEL, ResolveCustomPixelShader);
	ResolveCustomRenderStateData = RenderState.Compile();
	zeCheckError(ResolveCustomRenderStateData == NULL, false, "Cannot set render state");

	RenderState.SetShader(ZEGR_ST_PIXEL, ResolveGBuffersPixelShader);
	ResolveGBuffersRenderStateData = RenderState.Compile();
	zeCheckError(ResolveGBuffersRenderStateData == NULL, false, "Cannot set render state");

	RenderState.SetShader(ZEGR_ST_PIXEL, ResolveAllGBuffersPixelShader);
	ResolveAllGBuffersRenderStateData = RenderState.Compile();
	zeCheckError(ResolveAllGBuffersRenderStateData == NULL, false, "Cannot set render state");

	DirtyFlags.UnraiseFlags(ZERN_SRDF_RENDER_STATE);

	return true;
}

bool ZERNStageResolving::UpdateConstantBuffers()
{
	if (!DirtyFlags.GetFlags(ZERN_SRDF_CONSTANT_BUFFER))
		return true;

	ConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZERN_SRDF_CONSTANT_BUFFER);

	return true;
}

bool ZERNStageResolving::UpdateInputOutputs()
{
	InputTexture = GetPrevOutput(ZERN_SO_COLOR);
	if (InputTexture == NULL)
		return false;

	if (InputTexture->GetSampleCount() > 1)
	{
		ZEUInt Width = InputTexture->GetWidth();
		ZEUInt Height = InputTexture->GetHeight();

		if (ResolvedInputTexture == NULL || 
			ResolvedInputTexture->GetWidth() != Width || ResolvedInputTexture->GetHeight() != Height)
			ResolvedInputTexture = ZEGRTexture2D::CreateInstance(Width, Height, 1, InputTexture->GetFormat());
	}
	else
	{
		ResolvedInputTexture.Release();
	}

	if (ResolveAllGbuffers)
	{
		EmissiveTexture = GetPrevOutput(ZERN_SO_GBUFFER_EMISSIVE);
		if (EmissiveTexture == NULL)
			return false;

		DiffuseTexture = GetPrevOutput(ZERN_SO_GBUFFER_DIFFUSE);
		if (DiffuseTexture == NULL)
			return false;

		if (EmissiveTexture->GetSampleCount() > 1)
		{
			ZEUInt Width = EmissiveTexture->GetWidth();
			ZEUInt Height = EmissiveTexture->GetHeight();

			if (ResolvedGbufferEmissive == NULL || 
				ResolvedGbufferEmissive->GetWidth() != Width || ResolvedGbufferEmissive->GetHeight() != Height)
			{
				ResolvedGbufferEmissive = ZEGRTexture2D::CreateInstance(Width, Height, 1, EmissiveTexture->GetFormat());
				ResolvedGbufferDiffuse = ZEGRTexture2D::CreateInstance(Width, Height, 1, DiffuseTexture->GetFormat());
			}
		}
	}

	NormalTexture = GetPrevOutput(ZERN_SO_NORMAL);
	if (NormalTexture == NULL)
		return false;

	DepthTexture = GetPrevOutput(ZERN_SO_TRANSPARENT_DEPTH);
	if (DepthTexture == NULL)
		return false;

	if (DepthTexture->GetSampleCount() > 1)
	{
		ZEUInt Width = DepthTexture->GetWidth();
		ZEUInt Height = DepthTexture->GetHeight();

		if (ResolvedDepthTexture == NULL || 
			ResolvedDepthTexture->GetWidth() != Width || ResolvedDepthTexture->GetHeight() != Height)
		{
			ResolvedNormalTexture = ZEGRTexture2D::CreateInstance(Width, Height, 1, NormalTexture->GetFormat());
			ResolvedDepthTexture = ZEGRTexture2D::CreateInstance(Width, Height, 1, DepthTexture->GetFormat(), DepthTexture->GetResourceUsage(), DepthTexture->GetResourceBindFlags());
		}
	}
	
	return true;
}

bool ZERNStageResolving::Update()
{
	if (!UpdateRenderStates())
		return false;
	
	if (!UpdateConstantBuffers())
		return false;

	if (!UpdateInputOutputs())
		return false;

	return true;
}

ZEInt ZERNStageResolving::GetId() const
{
	return ZERN_STAGE_RESOLVING;
}

const ZEString& ZERNStageResolving::GetName() const
{
	static const ZEString Name = "Stage resolving";
	return Name;
}

void ZERNStageResolving::SetCustomResolveFilter(ZERNResolveFilterType FilterType)
{
	if (Constants.FilterType == FilterType)
		return;

	Constants.FilterType = FilterType;

	DirtyFlags.RaiseFlags(ZERN_SRDF_CONSTANT_BUFFER);
}

ZERNResolveFilterType ZERNStageResolving::GetCustomResolveFilter() const
{
	return Constants.FilterType;
}

void ZERNStageResolving::SetResolveAllGbuffers(bool ResolveAllGbuffers)
{
	this->ResolveAllGbuffers = ResolveAllGbuffers;
}

bool ZERNStageResolving::GetResolveAllGbuffers() const
{
	return ResolveAllGbuffers;
}

bool ZERNStageResolving::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	if (ZEGRGraphicsModule::SAMPLE_COUNT <= 1)
		return false;

	if (!Update())
		return false;

	if (ResolvedInputTexture != NULL)
	{
		Context->SetConstantBuffers(ZEGR_ST_PIXEL, 8, 1, ConstantBuffer.GetPointerToPointer());
		Context->SetRenderState(ResolveCustomRenderStateData);
		const ZEGRRenderTarget* RenderTarget = ResolvedInputTexture->GetRenderTarget();
		Context->SetRenderTargets(1, &RenderTarget, NULL);
		Context->SetTextures(ZEGR_ST_PIXEL, 5, 1, reinterpret_cast<const ZEGRTexture**>(&InputTexture));
		Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f,(float)ResolvedInputTexture->GetWidth(), (float)ResolvedInputTexture->GetHeight()));
		Context->Draw(3, 0);
	}

	Context->ClearDepthStencilBuffer(ResolvedDepthTexture->GetDepthStencilBuffer(), true, true, 0.0f, 0x00);

	if (ResolveAllGbuffers)
	{
		Context->SetRenderState(ResolveAllGBuffersRenderStateData);
		const ZEGRRenderTarget* RenderTargets[] = {ResolvedGbufferEmissive->GetRenderTarget(), ResolvedGbufferDiffuse->GetRenderTarget(), ResolvedNormalTexture->GetRenderTarget()};
		Context->SetRenderTargets(3, RenderTargets, ResolvedDepthTexture->GetDepthStencilBuffer());
	}
	else
	{
		Context->SetRenderState(ResolveGBuffersRenderStateData);
		const ZEGRRenderTarget* RenderTarget = ResolvedNormalTexture->GetRenderTarget();
		Context->SetRenderTargets(1, &RenderTarget, ResolvedDepthTexture->GetDepthStencilBuffer());
	}
	
	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f,(float)ResolvedDepthTexture->GetWidth(), (float)ResolvedDepthTexture->GetHeight()));
	Context->Draw(3, 0);

	return true;
}

void ZERNStageResolving::CleanUp(ZEGRContext* Context)
{
	ZERNStage::CleanUp(Context);
}

const ZEGRTexture2D* ZERNStageResolving::GetOutput(ZERNStageBuffer Output) const
{
	if (GetEnabled())
	{
		if (Output == ZERN_SO_COLOR && ResolvedInputTexture != NULL)
			return ResolvedInputTexture;

		else if (Output == ZERN_SO_GBUFFER_EMISSIVE && ResolvedGbufferEmissive != NULL)
			return ResolvedGbufferEmissive;

		else if (Output == ZERN_SO_GBUFFER_DIFFUSE && ResolvedGbufferDiffuse != NULL)
			return ResolvedGbufferDiffuse;

		else if (Output == ZERN_SO_NORMAL && ResolvedNormalTexture != NULL)
			return ResolvedNormalTexture;

		else if ((Output == ZERN_SO_DEPTH || Output == ZERN_SO_TRANSPARENT_DEPTH) && ResolvedDepthTexture != NULL)
			return ResolvedDepthTexture;
	}

	return ZERNStage::GetOutput(Output);
}

ZERNStageResolving::ZERNStageResolving()
{
	DirtyFlags.RaiseAll();

	InputTexture = NULL;
	NormalTexture = NULL;
	DepthTexture = NULL;
	EmissiveTexture = NULL;
	DiffuseTexture = NULL;

	ResolveAllGbuffers = false;

	Constants.FilterType = ZERN_FILTER_BOX;
}
