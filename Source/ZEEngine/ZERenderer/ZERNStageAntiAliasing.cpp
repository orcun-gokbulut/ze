//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageAntiAliasing.cpp
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

#include "ZERNStageAntiAliasing.h"

#include "ZEGraphics/ZEGRBuffer.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRSampler.h"
#include "ZEGraphics/ZEGRTexture.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZERNRenderer.h"
#include "ZERNShaderSlots.h"
#include "ZERNStageID.h"
#include "ZERNStageSMAAAreaTexture.h"
#include "ZERNStageSMAASearchTexture.h"

#define ZERN_AADF_CONSTANT_BUFFER	1
#define ZERN_AADF_SHADER			2
#define ZERN_AADF_RENDER_STATE		4
#define ZERN_AADF_TEXTURE			8
#define ZERN_AADF_OUTPUT			16

bool ZERNStageAntiAliasing::UpdateShaders()
{
	if (!DirtyFlags.GetFlags(ZERN_AADF_SHADER))
		return true;

	ZEGRShaderCompileOptions Options;
	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNSMAA.hlsl";
	Options.Model = ZEGR_SM_4_0;
	Options.Definitions.Add(ZEGRShaderDefinition("SAMPLE_COUNT", ZEString(ZEGRGraphicsModule::SAMPLE_COUNT)));

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNSMAA_EdgeDetection_VertexShader";
	EdgeDetectionVertexShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNSMAA_BlendingWeightCalculation_VertexShader";
	BlendingWeightCalculationVertexShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNSMAA_NeighborhoodBlending_VertexShader";
	NeighborhoodBlendingVertexShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNSMAA_EdgeDetection_PixelShader";
	EdgeDetectionPixelShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNSMAA_BlendingWeightCalculation_PixelShader";
	BlendingWeightCalculationPixelShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNSMAA_NeighborhoodBlending_PixelShader";
	NeighborhoodBlendingPixelShader = ZEGRShader::Compile(Options);

	DirtyFlags.UnraiseFlags(ZERN_AADF_SHADER);
	DirtyFlags.RaiseFlags(ZERN_AADF_RENDER_STATE);

	return true;
}

bool ZERNStageAntiAliasing::UpdateRenderStates()
{
	if (!DirtyFlags.GetFlags(ZERN_AADF_RENDER_STATE))
		return true;

	ZEGRDepthStencilState GenerateStencilState;
	GenerateStencilState.SetDepthTestEnable(false);
	GenerateStencilState.SetDepthWriteEnable(false);
	GenerateStencilState.SetStencilTestEnable(true);
	GenerateStencilState.SetFrontStencilPass(ZEGR_SO_REPLACE);

	ZEGRDepthStencilState TestStencilState;
	TestStencilState.SetDepthTestEnable(false);
	TestStencilState.SetDepthWriteEnable(false);
	TestStencilState.SetStencilTestEnable(true);
	TestStencilState.SetFrontStencilFunction(ZEGR_CF_EQUAL);

	ZEGRDepthStencilState DisableStencilState;
	DisableStencilState.SetDepthTestEnable(false);
	DisableStencilState.SetDepthWriteEnable(false);
	DisableStencilState.SetStencilTestEnable(false);

	ZEGRRenderState RenderState;
	RenderState.SetDepthStencilState(GenerateStencilState);
	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);
	RenderState.SetShader(ZEGR_ST_VERTEX, EdgeDetectionVertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, EdgeDetectionPixelShader);
	EdgeDetectionPassRenderStateData = RenderState.Compile();
	zeCheckError(EdgeDetectionPassRenderStateData == NULL, false, "Cannot set render state.");


	RenderState.SetDepthStencilState(TestStencilState);
	RenderState.SetShader(ZEGR_ST_VERTEX, BlendingWeightCalculationVertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, BlendingWeightCalculationPixelShader);
	BlendingWeightCalculationPassRenderStateData = RenderState.Compile();
	zeCheckError(BlendingWeightCalculationPassRenderStateData == NULL, false, "Cannot set render state.");


	RenderState.SetDepthStencilState(DisableStencilState);
	RenderState.SetShader(ZEGR_ST_VERTEX, NeighborhoodBlendingVertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, NeighborhoodBlendingPixelShader);
	NeighborhoodBlendingPassRenderStateData= RenderState.Compile();
	zeCheckError(NeighborhoodBlendingPassRenderStateData == NULL, false, "Cannot set render state.");
	
	DirtyFlags.UnraiseFlags(ZERN_AADF_RENDER_STATE);

	return true;
}

bool ZERNStageAntiAliasing::UpdateTextures()
{
	if (!DirtyFlags.GetFlags(ZERN_AADF_TEXTURE))
		return true;

	ZEUInt Width = GetRenderer()->GetOutputTexture()->GetWidth();
	ZEUInt Height = GetRenderer()->GetOutputTexture()->GetHeight();

	EdgeTexture = ZEGRTexture::CreateResource(ZEGR_TT_2D, Width, Height, 1, ZEGR_TF_R8G8B8A8_UNORM);
	EdgeRenderTarget = EdgeTexture->GetRenderTarget();

	BlendTexture = ZEGRTexture::CreateResource(ZEGR_TT_2D, Width, Height, 1, ZEGR_TF_R8G8B8A8_UNORM);
	BlendRenderTarget = BlendTexture->GetRenderTarget();

	DirtyFlags.UnraiseFlags(ZERN_AADF_TEXTURE);

	return true;
}

bool ZERNStageAntiAliasing::UpdateConstantBuffers()
{
	if (!DirtyFlags.GetFlags(ZERN_AADF_CONSTANT_BUFFER))
		return true;

	if (ConstantBuffer == NULL)
		ConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, sizeof(Constants), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);

	ConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZERN_AADF_CONSTANT_BUFFER);

	return true;
}

bool ZERNStageAntiAliasing::Update()
{
	if (!UpdateShaders())
		return false;

	if (!UpdateRenderStates())
		return false;

	if (!UpdateTextures())
		return false;

	if (!UpdateConstantBuffers())
		return false;

	return true;
}

void ZERNStageAntiAliasing::ClearTextures(ZEGRContext* Context)
{
	Context->ClearRenderTarget(EdgeRenderTarget, ZEVector4::Zero);
	Context->ClearRenderTarget(BlendRenderTarget, ZEVector4::Zero);
}

void ZERNStageAntiAliasing::DoEdgeDetection(ZEGRContext* Context)
{
	Context->SetRenderState(EdgeDetectionPassRenderStateData);
	Context->SetRenderTargets(1, &EdgeRenderTarget, DepthTexture->GetDepthStencilBuffer());

	Context->Draw(3, 0);
}

void ZERNStageAntiAliasing::DoBlendingWeightCalculation(ZEGRContext* Context)
{
	Context->SetRenderState(BlendingWeightCalculationPassRenderStateData);
	Context->SetRenderTargets(1, &BlendRenderTarget, DepthTexture->GetDepthStencilBuffer());
	Context->SetTexture(ZEGR_ST_PIXEL, 9, EdgeTexture);

	Context->Draw(3, 0);
}

void ZERNStageAntiAliasing::DoNeighborhoodBlending(ZEGRContext* Context)
{
	const ZEGRRenderTarget* RenderTarget = OutputTexture->GetRenderTarget();

	Context->SetRenderState(NeighborhoodBlendingPassRenderStateData);
	Context->SetRenderTargets(1, &RenderTarget, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 10, BlendTexture);

	Context->Draw(3, 0);
}

bool ZERNStageAntiAliasing::InitializeInternal()
{
	if (!ZERNStage::InitializeInternal())
		return false;

	SamplerLinearClamp = ZEGRSampler::GetDefaultSampler();

	ZEGRSamplerDescription SamplerDescriptionPointClamp;
	SamplerDescriptionPointClamp.MinFilter = ZEGR_TFM_POINT;
	SamplerDescriptionPointClamp.MagFilter = ZEGR_TFM_POINT;
	SamplerDescriptionPointClamp.MipFilter = ZEGR_TFM_POINT;
	SamplerPointClamp = ZEGRSampler::GetSampler(SamplerDescriptionPointClamp);

	AreaTexture = ZEGRTexture::CreateResource(ZEGR_TT_2D, AREATEX_WIDTH, AREATEX_HEIGHT, 1, ZEGR_TF_R8G8_UNORM, ZEGR_RU_IMMUTABLE, ZEGR_RBF_SHADER_RESOURCE, 1, 1, areaTexBytes);
	SearchTexture = ZEGRTexture::CreateResource(ZEGR_TT_2D, SEARCHTEX_WIDTH, SEARCHTEX_HEIGHT, 1, ZEGR_TF_R8_UNORM, ZEGR_RU_IMMUTABLE, ZEGR_RBF_SHADER_RESOURCE, 1, 1, searchTexBytes);

	return Update();
}

bool ZERNStageAntiAliasing::DeinitializeInternal()
{
	DirtyFlags.RaiseAll();

	EdgeDetectionVertexShader.Release();
	EdgeDetectionPixelShader.Release();
	
	BlendingWeightCalculationVertexShader.Release();
	BlendingWeightCalculationPixelShader.Release();

	NeighborhoodBlendingVertexShader.Release();
	NeighborhoodBlendingPixelShader.Release();

	EdgeDetectionPassRenderStateData.Release();
	BlendingWeightCalculationPassRenderStateData.Release();
	NeighborhoodBlendingPassRenderStateData.Release();

	ConstantBuffer.Release();

	EdgeTexture.Release();
	BlendTexture.Release();
	AreaTexture.Release();
	SearchTexture.Release();

	SamplerLinearClamp.Release();
	SamplerPointClamp.Release();

	InputTexture = NULL;
	GBufferNormal = NULL;
	DepthTexture = NULL;
	OutputTexture = NULL;

	EdgeRenderTarget = NULL;
	BlendRenderTarget = NULL;

	return ZERNStage::DeinitializeInternal();
}

void ZERNStageAntiAliasing::CreateOutput(const ZEString& Name)
{
	if (Name == "ColorTexture")
	{
		if (DirtyFlags.GetFlags(ZERN_AADF_OUTPUT))
		{
			OutputTexture = ZEGRTexture::CreateResource(ZEGR_TT_2D, InputTexture->GetWidth(), InputTexture->GetHeight(), 1, InputTexture->GetFormat()).GetPointer();
			DirtyFlags.UnraiseFlags(ZERN_AADF_OUTPUT);
		}
	}
}

ZEInt ZERNStageAntiAliasing::GetId() const
{
	return ZERN_STAGE_ANTI_ALIASING;
}

const ZEString& ZERNStageAntiAliasing::GetName() const
{
	static const ZEString Name = "Stage Anti Aliasing";
	return Name;
}

void ZERNStageAntiAliasing::Resized(ZEUInt Width, ZEUInt Height)
{
	Constants.OutputSize.x = Width;
	Constants.OutputSize.y = Height;

	Viewport.SetWidth((float)Width);
	Viewport.SetHeight((float)Height);

	DirtyFlags.RaiseFlags(ZERN_AADF_TEXTURE | ZERN_AADF_OUTPUT | ZERN_AADF_CONSTANT_BUFFER);
}

bool ZERNStageAntiAliasing::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	if (!Update())
		return false;

	Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_POST_PROCESSOR, ConstantBuffer);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, ZERN_SHADER_CONSTANT_POST_PROCESSOR, ConstantBuffer);
	const ZEGRSampler* Samplers[] = {SamplerLinearClamp, SamplerPointClamp};
	Context->SetSamplers(ZEGR_ST_PIXEL, 0, 2, Samplers);
	Context->SetStencilRef(1);
	const ZEGRTexture* Textures[] = {InputTexture, GBufferNormal, AreaTexture, SearchTexture};
	Context->SetTextures(ZEGR_ST_PIXEL, 5, 4, Textures);
	Context->SetViewports(1, &Viewport);

	ClearTextures(Context);
	DoEdgeDetection(Context);
	DoBlendingWeightCalculation(Context);
	DoNeighborhoodBlending(Context);

	return true;
}

void ZERNStageAntiAliasing::CleanUp(ZEGRContext* Context)
{
	ZERNStage::CleanUp(Context);
}

ZERNStageAntiAliasing::ZERNStageAntiAliasing()
{
	DirtyFlags.RaiseAll();

	memset(&Constants, 0, sizeof(Constants));

	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&InputTexture), "ColorTexture", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV | ZERN_SRCF_REQUIRED);
	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&GBufferNormal), "GBufferNormal", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV);
	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&DepthTexture), "DepthTexture", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV | ZERN_SRCF_REQUIRED);

	AddOutputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&OutputTexture), "ColorTexture", ZERN_SRUT_WRITE, ZERN_SRCF_CREATE_OWN | ZERN_SRCF_GET_OUTPUT);

	EdgeRenderTarget = NULL;
	BlendRenderTarget = NULL;
}
