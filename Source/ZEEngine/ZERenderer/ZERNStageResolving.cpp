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
#include "ZEGraphics/ZEGRBuffer.h"
#include "ZEGraphics/ZEGRTexture.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZERNRenderer.h"

#define ZERN_SRDF_RENDER_STATE		1
#define ZERN_SRDF_CONSTANT_BUFFER	2
#define ZERN_SRDF_OUTPUT0			4
#define ZERN_SRDF_OUTPUT1			8
#define ZERN_SRDF_OUTPUT2			16
#define ZERN_SRDF_OUTPUT3			32
#define ZERN_SRDF_OUTPUT4			64
#define ZERN_SRDF_OUTPUT_ALL		(ZERN_SRDF_OUTPUT0 | ZERN_SRDF_OUTPUT1 | ZERN_SRDF_OUTPUT2 | ZERN_SRDF_OUTPUT3 | ZERN_SRDF_OUTPUT4)

bool ZERNStageResolving::InitializeInternal()
{
	if (!ZERNStage::InitializeInternal())
		return false;

	ConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, sizeof(Constants), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);

	return Update();
}

bool ZERNStageResolving::DeinitializeInternal()
{
	DirtyFlags.RaiseAll();

	ResolveGBuffersRenderStateData.Release();
	ResolveAllGBuffersRenderStateData.Release();
	ResolveCustomRenderStateData.Release();

	ConstantBuffer.Release();

	ResolvedInputTexture.Release();
	ResolvedGBuffer1.Release();
	ResolvedGBuffer2.Release();
	ResolvedGBuffer3.Release();
	ResolvedDepthTexture.Release();

	InputTexture = NULL;
	GBuffer1 = NULL;
	GBuffer2 = NULL;
	GBuffer3 = NULL;
	DepthTexture = NULL;

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

bool ZERNStageResolving::Update()
{
	if (!UpdateRenderStates())
		return false;
	
	if (!UpdateConstantBuffers())
		return false;

	return true;
}

void ZERNStageResolving::CreateOutput(const ZEString& Name)
{
	if (Name == CustomInputname)
	{
		if (DirtyFlags.GetFlags(ZERN_SRDF_OUTPUT0))
		{
			ResolvedInputTexture = ZEGRTexture::CreateResource(ZEGR_TT_2D, InputTexture->GetWidth(), InputTexture->GetHeight(), 1, InputTexture->GetFormat());
			DirtyFlags.UnraiseFlags(ZERN_SRDF_OUTPUT0);
		}
	}
	else if (Name == "GBuffer1")
	{
		if (DirtyFlags.GetFlags(ZERN_SRDF_OUTPUT1))
		{
				ResolvedGBuffer1 = ZEGRTexture::CreateResource(ZEGR_TT_2D, GBuffer1->GetWidth(), GBuffer1->GetHeight(), 1, GBuffer1->GetFormat());

			DirtyFlags.UnraiseFlags(ZERN_SRDF_OUTPUT1);
		}
	}
	else if (Name == "GBuffer2")
	{
		if (DirtyFlags.GetFlags(ZERN_SRDF_OUTPUT2))
		{
			if (ResolveAllGbuffers)
				ResolvedGBuffer2 = ZEGRTexture::CreateResource(ZEGR_TT_2D, GBuffer2->GetWidth(), GBuffer2->GetHeight(), 1, GBuffer2->GetFormat());
			else
				ResolvedGBuffer2 = NULL;

			DirtyFlags.UnraiseFlags(ZERN_SRDF_OUTPUT2);
		}
	}
	else if (Name == "GBuffer3")
	{
		if (DirtyFlags.GetFlags(ZERN_SRDF_OUTPUT3))
		{
			if (ResolveAllGbuffers)
				ResolvedGBuffer3 = ZEGRTexture::CreateResource(ZEGR_TT_2D, GBuffer3->GetWidth(), GBuffer3->GetHeight(), 1, GBuffer3->GetFormat());
			else
				ResolvedGBuffer3 = NULL;

			DirtyFlags.UnraiseFlags(ZERN_SRDF_OUTPUT3);
		}
	}
	else if (Name == "DepthTexture")
	{
		if (DirtyFlags.GetFlags(ZERN_SRDF_OUTPUT4))
		{
			ResolvedDepthTexture = ZEGRTexture::CreateResource(ZEGR_TT_2D, DepthTexture->GetWidth(), DepthTexture->GetHeight(), 1, DepthTexture->GetFormat(), DepthTexture->GetResourceUsage(), DepthTexture->GetResourceBindFlags());
			DirtyFlags.UnraiseFlags(ZERN_SRDF_OUTPUT4);
		}
	}
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
	if (this->ResolveAllGbuffers == ResolveAllGbuffers)
		return;

	this->ResolveAllGbuffers = ResolveAllGbuffers;

	GetRenderer()->MarkDirtyPipeline();
	DirtyFlags.RaiseFlags(ZERN_SRDF_OUTPUT1 | ZERN_SRDF_OUTPUT2);
}

bool ZERNStageResolving::GetResolveAllGbuffers() const
{
	return ResolveAllGbuffers;
}

void ZERNStageResolving::SetCustomInput(const ZEString& Inputname)
{
	if (CustomInputname == Inputname)
		return;

	if (Inputname == ZEString::Empty)
	{
		RemoveInputResource(CustomInputname);
		RemoveOutputResource(CustomInputname);
	}
	else
	{
		DirtyFlags.RaiseFlags(ZERN_SRDF_OUTPUT0);

		AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&InputTexture), Inputname, ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV);
		AddOutputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&ResolvedInputTexture), Inputname, ZERN_SRUT_WRITE, ZERN_SRCF_CREATE_OWN | ZERN_SRCF_GET_OUTPUT);
	}

	CustomInputname = Inputname;
}

void ZERNStageResolving::Resized(ZEUInt Width, ZEUInt Height)
{
	DirtyFlags.RaiseFlags(ZERN_SRDF_OUTPUT_ALL);
}

bool ZERNStageResolving::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	//TODO: Check in renderer
	if (ZEGRGraphicsModule::SAMPLE_COUNT <= 1)
		return false;

	if (!Update())
		return false;

	if (ResolvedInputTexture != NULL)
	{
		Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, ConstantBuffer);
		Context->SetRenderState(ResolveCustomRenderStateData);
		const ZEGRRenderTarget* RenderTarget = ResolvedInputTexture->GetRenderTarget();
		Context->SetRenderTargets(1, &RenderTarget, NULL);
		Context->SetTexture(ZEGR_ST_PIXEL, 5, InputTexture);
		Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f,(float)ResolvedInputTexture->GetWidth(), (float)ResolvedInputTexture->GetHeight()));
		Context->Draw(3, 0);
	}

	if (ResolvedDepthTexture == NULL || ResolvedGBuffer1 == NULL)
		return false;

	Context->ClearDepthStencilBuffer(ResolvedDepthTexture->GetDepthStencilBuffer(), true, true, 0.0f, 0x00);

	if (ResolveAllGbuffers)
	{
		if (ResolvedGBuffer2 == NULL || ResolvedGBuffer3 == NULL)
			return false;

		const ZEGRTexture* Textures[] = {GBuffer1, GBuffer2, GBuffer3, DepthTexture};
		Context->SetTextures(ZEGR_ST_PIXEL, 1, 4, Textures);
		Context->SetRenderState(ResolveAllGBuffersRenderStateData);
		const ZEGRRenderTarget* RenderTargets[] = {ResolvedGBuffer1->GetRenderTarget(), ResolvedGBuffer2->GetRenderTarget(), ResolvedGBuffer3->GetRenderTarget()};
		Context->SetRenderTargets(3, RenderTargets, ResolvedDepthTexture->GetDepthStencilBuffer());
	}
	else
	{
		Context->SetTexture(ZEGR_ST_PIXEL, 1, GBuffer1);
		Context->SetTexture(ZEGR_ST_PIXEL, 4, DepthTexture);
		Context->SetRenderState(ResolveGBuffersRenderStateData);
		const ZEGRRenderTarget* RenderTarget = ResolvedGBuffer1->GetRenderTarget();
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

ZERNStageResolving::ZERNStageResolving()
{
	DirtyFlags.RaiseAll();

	ResolveAllGbuffers = false;

	Constants.FilterType = ZERN_FILTER_BOX;

	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&GBuffer1), "GBuffer1", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV);
	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&GBuffer2), "GBuffer2", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV);
	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&GBuffer3), "GBuffer3", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV);
	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&DepthTexture), "DepthTexture", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV);

	AddOutputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&ResolvedGBuffer1), "GBuffer1", ZERN_SRUT_WRITE, ZERN_SRCF_CREATE_OWN);
	AddOutputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&ResolvedGBuffer2), "GBuffer2", ZERN_SRUT_WRITE, ZERN_SRCF_CREATE_OWN);
	AddOutputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&ResolvedGBuffer3), "GBuffer3", ZERN_SRUT_WRITE, ZERN_SRCF_CREATE_OWN);
	AddOutputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&ResolvedDepthTexture), "DepthTexture", ZERN_SRUT_WRITE, ZERN_SRCF_CREATE_OWN);
}

ZERNStageResolving::~ZERNStageResolving()
{
	Deinitialize();
}
