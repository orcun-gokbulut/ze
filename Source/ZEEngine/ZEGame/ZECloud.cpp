//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECloud.cpp
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

#include "ZECloud.h"

#include "ZEGraphics/ZEGRSampler.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZECamera.h"
#include "ZERenderer/ZERNCuller.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNStagePostProcess.h"
#include "ZERenderer/ZERNShaderSlots.h"
#include "ZERenderer/ZERNFilter.h"
#include "ZERenderer/ZERNStageGBuffer.h"
#include "ZETexture/ZETexture2DResource.h"

#define ZE_CDF_SHADERS			1
#define ZE_CDF_RENDER_STATES	2
#define ZE_CDF_CONSTANT_BUFFER	4

ZEGRVertexLayout GetPositionTexcoordVertexLayout()
{
	static ZEGRVertexLayout VertexLayout;
	if (VertexLayout.GetElementCount() == 0)
	{
		ZEGRVertexElement ElementArray[] = 
		{
			{ZEGR_VES_POSITION, 0, ZEGR_VET_FLOAT3, 0, 0, ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_TEXCOORD, 0, ZEGR_VET_FLOAT2, 0, 12, ZEGR_VU_PER_VERTEX, 0}
		};

		VertexLayout.SetElements(ElementArray, 2);
	}

	return VertexLayout;
}

void ZECloud::CreatePlane()
{
	struct Vertex
	{
		ZEVector3 Position;
		ZEVector2 Texcoord;
	};

	float Height = 0.5f;

	Vertex Vertices[16] = 
	{
		{ ZEVector3(-10.0f, 0.0f, 10.0f), ZEVector2(0.0f, 0.0f) },
		{ ZEVector3(-5.0f, 0.0f, 10.0f), ZEVector2(0.25f, 0.0f) },
		{ ZEVector3(5.0f, 0.0f, 10.0f), ZEVector2(0.75f, 0.0f) },
		{ ZEVector3(10.0f, 0.0f, 10.0f), ZEVector2(1.0f, 0.0f) },

		{ ZEVector3(-10.0f, 0.0f, 5.0f), ZEVector2(0.0f, 0.25f) },
		{ ZEVector3(-5.0f, Height, 5.0f), ZEVector2(0.25f, 0.25f) },
		{ ZEVector3(5.0f, Height, 5.0f), ZEVector2(0.75f, 0.25f) },
		{ ZEVector3(10.0f, 0.0f, 5.0f), ZEVector2(1.0f, 0.25f) },

		{ ZEVector3(-10.0f, 0.0f, -5.0f), ZEVector2(0.0f, 0.5f) },
		{ ZEVector3(-5.0f, Height, -5.0f), ZEVector2(0.25f, 0.5f) },
		{ ZEVector3(5.0f, Height, -5.0f), ZEVector2(0.75f, 0.5f) },
		{ ZEVector3(10.0f, 0.0f, -5.0f), ZEVector2(1.0f, 0.5f) },

		{ ZEVector3(-10.0f, 0.0f, -10.0f), ZEVector2(0.0f, 1.0f) },
		{ ZEVector3(-5.0f, 0.0f, -10.0f), ZEVector2(0.25f, 1.0f) },
		{ ZEVector3(5.0f, 0.0f, -10.0f), ZEVector2(0.75f, 1.0f) },
		{ ZEVector3(10.0f, 0.0f, -10.0f), ZEVector2(1.0f, 1.0f) }
	};

	PlaneVertexBuffer = ZEGRVertexBuffer::Create(16, sizeof(Vertex));

	void* Data;
	PlaneVertexBuffer->Lock(&Data);
	memcpy(Data, &Vertices, sizeof(Vertex) * 16);
	PlaneVertexBuffer->Unlock();
}

bool ZECloud::UpdateShaders()
{
	if (!DirtyFlags.GetFlags(ZE_CDF_SHADERS))
		return true;

	ZEGRShaderCompileOptions Options;
	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNCloud.hlsl";
	Options.Model = ZEGR_SM_5_0;

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNCloud_Plane_VertexShader_Main";
	PlaneVertexShader = ZEGRShader::Compile(Options);
	zeCheckError(PlaneVertexShader == NULL, false, "Cloud plane vertex shader cannot compile");

	Options.Type = ZEGR_ST_HULL;
	Options.EntryPoint = "ZERNCloud_Plane_HullShader_Main";
	PlaneHullShader = ZEGRShader::Compile(Options);
	zeCheckError(PlaneHullShader == NULL, false, "Cloud plane hull shader cannot compile");

	Options.Type = ZEGR_ST_DOMAIN;
	Options.EntryPoint = "ZERNCloud_Plane_DomainShader_Main";
	PlaneDomainShader = ZEGRShader::Compile(Options);
	zeCheckError(PlaneDomainShader == NULL, false, "Cloud plane domain shader cannot compile");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNCloud_Plane_PixelShader_Main";
	PlanePixelShader = ZEGRShader::Compile(Options);
	zeCheckError(PlanePixelShader == NULL, false, "Cloud plane pixel shader cannot compile");

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNScreenCover_VertexShader_Position";
	BlurVertexShader = ZEGRShader::Compile(Options);
	zeCheckError(BlurVertexShader == NULL, false, "Cloud blur vertex shader cannot compile");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNCloud_Blur_PixelShader_Main";
	BlurPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(BlurPixelShader == NULL, false, "Cloud blur pixel shader cannot compile");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNCloud_Lighting_PixelShader_Main";
	LightingPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(LightingPixelShader == NULL, false, "Cloud lighting pixel shader cannot compile");

	DirtyFlags.UnraiseFlags(ZE_CDF_SHADERS);
	DirtyFlags.RaiseFlags(ZE_CDF_RENDER_STATES);

	return true;
}

bool ZECloud::UpdateRenderStates()
{
	if (!DirtyFlags.GetFlags(ZE_CDF_RENDER_STATES))
		return true;

	ZEGRRenderState RenderState = ZERNStagePostProcess::GetRenderState();
	RenderState.SetPrimitiveType(ZEGR_PT_16_CONTROL_POINT_PATCHLIST);
	RenderState.SetVertexLayout(GetPositionTexcoordVertexLayout());

	ZEGRRasterizerState RasterizerStateFrontCCW;
	RasterizerStateFrontCCW.SetFrontIsCounterClockwise(true);

	RenderState.SetRasterizerState(RasterizerStateFrontCCW);

	ZEGRDepthStencilState DepthStencilStateTestNoWrite;
	DepthStencilStateTestNoWrite.SetDepthFunction(ZEGR_CF_GREATER_EQUAL);
	DepthStencilStateTestNoWrite.SetDepthTestEnable(true);
	DepthStencilStateTestNoWrite.SetDepthWriteEnable(false);
	DepthStencilStateTestNoWrite.SetStencilTestEnable(true);
	DepthStencilStateTestNoWrite.SetFrontStencilFail(ZEGR_SO_REPLACE);
	DepthStencilStateTestNoWrite.SetFrontStencilDepthFail(ZEGR_SO_KEEP);
	DepthStencilStateTestNoWrite.SetFrontStencilPass(ZEGR_SO_REPLACE);
	DepthStencilStateTestNoWrite.SetFrontStencilFunction(ZEGR_CF_ALWAYS);

	RenderState.SetDepthStencilState(DepthStencilStateTestNoWrite);

	ZEGRBlendState BlendStateAdditive;
	BlendStateAdditive.SetBlendEnable(true);
	ZEGRBlendRenderTarget BlendRenderTargetAdditive = BlendStateAdditive.GetRenderTarget(0);
	BlendRenderTargetAdditive.SetSource(ZEGRBlend::ZEGR_BO_ONE);
	BlendRenderTargetAdditive.SetDestination(ZEGRBlend::ZEGR_BO_ONE);
	BlendRenderTargetAdditive.SetOperation(ZEGRBlendOperation::ZEGR_BE_ADD);
	BlendRenderTargetAdditive.SetBlendEnable(true);
	BlendStateAdditive.SetRenderTargetBlend(0, BlendRenderTargetAdditive);

	RenderState.SetBlendState(BlendStateAdditive);

	RenderState.SetShader(ZEGR_ST_VERTEX, PlaneVertexShader);
	RenderState.SetShader(ZEGR_ST_HULL, PlaneHullShader);
	RenderState.SetShader(ZEGR_ST_DOMAIN, PlaneDomainShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, PlanePixelShader);

	PlaneRenderStateData = RenderState.Compile();
	zeCheckError(PlaneRenderStateData == NULL, false, "Cloud plane render state cannot compile");

	RenderState = ZEGRRenderState::Default;

	ZEGRDepthStencilState DepthStencilStateStencilTest;
	DepthStencilStateStencilTest.SetDepthFunction(ZEGR_CF_ALWAYS);
	DepthStencilStateStencilTest.SetDepthTestEnable(false);
	DepthStencilStateStencilTest.SetDepthWriteEnable(false);
	DepthStencilStateStencilTest.SetStencilTestEnable(true);
	DepthStencilStateStencilTest.SetStencilWriteMask(0);
	DepthStencilStateStencilTest.SetFrontStencilFail(ZEGR_SO_KEEP);
	DepthStencilStateStencilTest.SetFrontStencilDepthFail(ZEGR_SO_KEEP);
	DepthStencilStateStencilTest.SetFrontStencilPass(ZEGR_SO_KEEP);
	DepthStencilStateStencilTest.SetFrontStencilFunction(ZEGR_CF_EQUAL);

	RenderState.SetDepthStencilState(DepthStencilStateStencilTest);

	RenderState.SetShader(ZEGR_ST_VERTEX, BlurVertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, BlurPixelShader);

	BlurRenderStateData = RenderState.Compile();
	zeCheckError(BlurRenderStateData == NULL, false, "Cloud blur render state cannot compile");

	RenderState.SetShader(ZEGR_ST_PIXEL, LightingPixelShader);

	LightingRenderStateData = RenderState.Compile();
	zeCheckError(LightingRenderStateData == NULL, false, "Cloud lighting render state cannot compile");

	DirtyFlags.UnraiseFlags(ZE_CDF_RENDER_STATES);

	return true;
}

bool ZECloud::UpdateConstantBuffers()
{
	if (!DirtyFlags.GetFlags(ZE_CDF_CONSTANT_BUFFER))
		return true;

	ConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZE_CDF_CONSTANT_BUFFER);

	return true;
}

bool ZECloud::Update()
{
	if (!UpdateShaders())
		return false;

	if (!UpdateRenderStates())
		return false;

	if (!UpdateConstantBuffers())
		return false;

	return true;
}

void ZECloud::RenderClouds(ZEGRContext* Context, ZEGRTexture2D* OutputTexture, ZEGRDepthStencilBuffer* DepthStencilBuffer)
{
	const ZEGRRenderTarget* RenderTarget = OutputTexture->GetRenderTarget();

	Context->SetStencilRef(1.0f);
	Context->SetRenderState(PlaneRenderStateData);
	Context->SetRenderTargets(1, &RenderTarget, DepthStencilBuffer);
	Context->SetSampler(ZEGR_ST_PIXEL, 0, SamplerLinearWrap);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, CloudTexture->GetTexture());

	Context->Draw(PlaneVertexBuffer->GetVertexCount(), 0);

	Context->SetRenderTargets(0, NULL, NULL);
	Context->SetSampler(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, NULL);
}

void ZECloud::ApplyBlur(ZEGRContext* Context, ZEGRTexture2D* OutputTexture, ZEGRDepthStencilBuffer* DepthStencilBuffer)
{
	const ZEGRRenderTarget* TempRenderTarget = OutputTexture->GetRenderTarget();
	const ZEGRRenderTarget* BlurredRenderTarget = BlurredTexture->GetRenderTarget();

	Context->SetStencilRef(1.0f);
	Context->SetRenderState(BlurRenderStateData);
	Context->SetSampler(ZEGR_ST_PIXEL, 0, SamplerLinearWrap);

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 9, VerticalConstantBuffer);
	Context->SetRenderTargets(1, &BlurredRenderTarget, DepthStencilBuffer);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, OutputTexture);
	Context->Draw(3, 0);

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 9, HorizontalConstantBuffer);
	Context->SetRenderTargets(1, &TempRenderTarget, DepthStencilBuffer);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, BlurredTexture);
	Context->Draw(3, 0);

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 9, VerticalConstantBuffer);
	Context->SetRenderTargets(1, &BlurredRenderTarget, DepthStencilBuffer);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, OutputTexture);
	Context->Draw(3, 0);

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 9, HorizontalConstantBuffer);
	Context->SetRenderTargets(1, &TempRenderTarget, DepthStencilBuffer);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, BlurredTexture);
	Context->Draw(3, 0);

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 9, NULL);
	Context->SetRenderTargets(0, NULL, NULL);
	Context->SetSampler(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, NULL);
}

void ZECloud::LightingClouds(ZEGRContext* Context, ZEGRTexture2D* OutputTexture, ZEGRDepthStencilBuffer* DepthStencilBuffer)
{
	const ZEGRRenderTarget* RenderTarget = OutputTexture->GetRenderTarget();
	Context->SetStencilRef(1.0f);
	Context->SetRenderState(LightingRenderStateData);
	Context->SetTexture(ZEGR_ST_PIXEL, 6, TempTexture);
	Context->SetRenderTargets(1, &RenderTarget, DepthStencilBuffer);

	Context->Draw(3, 0);

	Context->SetRenderTargets(0, NULL, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 6, NULL);
}

bool ZECloud::InitializeSelf()
{
	if (!ZEEntity::InitializeSelf())
		return false;

	CreatePlane();

	PlaneConstantBuffer = ZEGRConstantBuffer::Create(sizeof(ZEMatrix4x4));
	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(Constants));
	VerticalConstantBuffer = ZEGRConstantBuffer::Create(sizeof(ZEVector4) * 11);
	HorizontalConstantBuffer = ZEGRConstantBuffer::Create(sizeof(ZEVector4) * 11);

	ZEGRSamplerDescription SamplerDescriptionLinearWrap;
	SamplerDescriptionLinearWrap.AddressU = ZEGR_TAM_WRAP;
	SamplerDescriptionLinearWrap.AddressV = ZEGR_TAM_WRAP;
	SamplerLinearWrap = ZEGRSampler::GetSampler(SamplerDescriptionLinearWrap);

	ZERNFilter::GenerateGaussianKernel(HorizontalValues, 11, 2.0f);
	ZERNFilter::GenerateGaussianKernel(VerticalValues, 11, 2.0f, false);

	VerticalConstantBuffer->SetData(&VerticalValues[0]);
	HorizontalConstantBuffer->SetData(&HorizontalValues[0]);

	return true;
}

bool ZECloud::DeinitializeSelf()
{
	PlaneVertexShader.Release();
	PlaneHullShader.Release();
	PlaneDomainShader.Release();
	PlanePixelShader.Release();
	PlaneRenderStateData.Release();
	PlaneVertexBuffer.Release();
	PlaneConstantBuffer.Release();
	
	SamplerLinearWrap.Release();

	return ZEEntity::DeinitializeSelf();
}

ZECloud::ZECloud()
{
	DirtyFlags.RaiseAll();

	RenderCommand.Entity = this;
	RenderCommand.Priority = 3;
	RenderCommand.StageMask = ZERN_STAGE_POST_EFFECT;

	CloudTexture = NULL;

	Constants.PlaneSubdivision = 5.0f;
	Constants.CloudCoverage = 1;
	Constants.CloudDensity = 2.0f;
	Constants.SunDirection = ZEVector3(-1.0f);
	Constants.SunIntensity = 1.0f;
}

ZEDrawFlags ZECloud::GetDrawFlags() const
{
	return ZE_DF_DRAW;
}

void ZECloud::SetCloudTexture(const ZEString& FileName)
{
	ZETextureOptions TextureOption = 
	{
		ZE_TCT_NONE,
		ZE_TCQ_LOW,
		ZE_TDS_NONE,
		ZE_TFC_DISABLED,
		ZE_TMM_DISABLED,
		1
	};

	CloudTexture = ZETexture2DResource::LoadResource(FileName, &TextureOption);
}

const ZEString& ZECloud::GetCloudTexture() const
{
	return CloudTexture == NULL ? ZEString::Empty : CloudTexture->GetFileName();
}

void ZECloud::SetSunDirection(const ZEVector3& SunDirection)
{
	if (Constants.SunDirection == SunDirection)
		return;

	Constants.SunDirection = SunDirection;

	DirtyFlags.RaiseFlags(ZE_CDF_CONSTANT_BUFFER);
}

const ZEVector3& ZECloud::GetSunDirection() const
{
	return Constants.SunDirection;
}

void ZECloud::SetCloudCoverage(ZEUInt CloudCoverage)
{
	if (Constants.CloudCoverage == CloudCoverage)
		return;

	Constants.CloudCoverage = CloudCoverage;

	DirtyFlags.RaiseFlags(ZE_CDF_CONSTANT_BUFFER);
}

ZEUInt ZECloud::GetCloudCoverage() const
{
	return Constants.CloudCoverage;
}

void ZECloud::SetCloudDensity(float CloudDensity)
{
	if (Constants.CloudDensity == CloudDensity)
		return;

	Constants.CloudDensity = CloudDensity;

	DirtyFlags.RaiseFlags(ZE_CDF_CONSTANT_BUFFER);
}

float ZECloud::GetCloudDensity() const
{
	return Constants.CloudDensity;
}

bool ZECloud::PreRender(const ZERNCullParameters* CullParameters)
{
	CullParameters->Renderer->AddCommand(&RenderCommand);

	return true;
}

void ZECloud::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	if (!Update())
		return;

	ZEGRRenderTarget* RenderTarget = Parameters->Renderer->GetOutputRenderTarget();
	if (TempTexture == NULL || 
		TempTexture->GetWidth() != RenderTarget->GetWidth() || TempTexture->GetHeight() != RenderTarget->GetHeight())
	{
		TempTexture.Release();
		TempTexture = ZEGRTexture2D::CreateInstance(RenderTarget->GetWidth(), RenderTarget->GetHeight(), 1, 1, 1, ZEGR_TF_R11G11B10_FLOAT, true);

		BlurredTexture.Release();
		BlurredTexture = ZEGRTexture2D::CreateInstance(RenderTarget->GetWidth(), RenderTarget->GetHeight(), 1, 1, 1, ZEGR_TF_R11G11B10_FLOAT, true);
	}

	ZEGRContext* Context = Parameters->Context;

	ZEMatrix4x4 WorldMatrix;
	ZEMatrix4x4::CreateOrientation(WorldMatrix, Parameters->View->Position, ZEVector3::One);
	
	PlaneConstantBuffer->SetData(&WorldMatrix);

	Context->SetConstantBuffer(ZEGR_ST_DOMAIN, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, PlaneConstantBuffer);
	ZEGRVertexBuffer* VertexBuffer = PlaneVertexBuffer;
	Context->SetVertexBuffers(0, 1, &VertexBuffer);
	
	Context->SetConstantBuffer(ZEGR_ST_ALL, 8, ConstantBuffer);

	ZERNStagePostProcess* StagePostProcess = static_cast<ZERNStagePostProcess*>(Parameters->Stage);
	ZERNStageGBuffer* StageGBuffer = static_cast<ZERNStageGBuffer*>(Parameters->Renderer->GetStage(ZERN_STAGE_GBUFFER));

	Context->ClearDepthStencilBuffer(StageGBuffer->GetDepthMap()->GetDepthStencilBuffer(), false, true, 0.0f, 0x00);

	ZEGRRenderTarget* PrevRenderTarget;
	ZEGRDepthStencilBuffer* PrevDepthStencilBuffer;
	Context->GetRenderTargets(1, &PrevRenderTarget, &PrevDepthStencilBuffer);

	RenderClouds(Context, StagePostProcess->GetOutputTexture(), PrevDepthStencilBuffer);
	//ApplyBlur(Context, StagePostProcess->GetOutputTexture(), PrevDepthStencilBuffer);
	//LightingClouds(Context, StagePostProcess->GetOutputTexture(), PrevDepthStencilBuffer);

	Context->SetConstantBuffer(ZEGR_ST_DOMAIN, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, NULL);
	Context->SetVertexBuffers(0, 0, NULL);
	Context->SetConstantBuffer(ZEGR_ST_ALL, 8, NULL);
}

ZECloud::~ZECloud()
{

}

ZECloud* ZECloud::CreateInstance()
{
	return new ZECloud();
}
