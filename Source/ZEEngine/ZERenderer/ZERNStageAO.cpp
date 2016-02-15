//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageAO.cpp
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

#include "ZERNStageAO.h"

#include "ZERandom.h"
#include "ZEDS/ZEArray.h"
#include "ZEMath/ZEAngle.h"
#include "ZERNFilter.h"
#include "ZERNRenderer.h"
#include "ZERNStageHDR.h"
#include "ZERNStageGBuffer.h"
#include "ZERNStageLighting.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRSampler.h"
#include "ZEGraphics/ZEGROutput.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"

#define ZERN_AODF_CONSTANT_BUFFER	1
#define ZERN_AODF_SHADER			2
#define ZERN_AODF_RENDER_STATE		4
#define ZERN_AODF_TEXTURE			8

void ZERNStageAO::CreateRandomVectors()
{
	const ZEUInt Size = 128 * 128 * 4;
	ZEUInt8 Vectors[Size];
	for(ZEUInt I = 0; I < Size; I += 4)
	{
		Vectors[I]		= (ZEUInt8)(ZERandom::GetFloatPositive() * 255.0f + 0.5f);
		Vectors[I + 1]	= (ZEUInt8)(ZERandom::GetFloatPositive() * 255.0f + 0.5f);
		Vectors[I + 2]	= (ZEUInt8)(ZERandom::GetFloatPositive() * 255.0f + 0.5f);
		Vectors[I + 3]	= 0;
	}

	RandomVectorsTexture.Release();
	RandomVectorsTexture = ZEGRTexture2D::CreateInstance(128, 128, 1, 1, 1, ZEGR_TF_R8G8B8A8_UNORM);
	RandomVectorsTexture->UpdateSubResource(0, 0, &Vectors[0], 128 * 4);
}

void ZERNStageAO::CreateOffsetVectors()
{
	ZEVector4 Samples[] =
	{
		ZEVector4(-1.0f, -1.0f, -1.0f, 0.0f),
		ZEVector4(-1.0f, 1.0f, -1.0f, 0.0f),
		ZEVector4(1.0f, 1.0f, -1.0f, 0.0f),
		ZEVector4(1.0f, -1.0f, -1.0f, 0.0f),

		ZEVector4(-1.0f, -1.0f, 1.0f, 0.0f),
		ZEVector4(-1.0f, 1.0f, 1.0f, 0.0f),
		ZEVector4(1.0f, 1.0f, 1.0f, 0.0f),
		ZEVector4(1.0f, -1.0f, 1.0f, 0.0f),

		ZEVector4(-1.0f, 0.0f, 0.0f, 0.0f),
		ZEVector4(1.0f, 0.0f, 0.0f, 0.0f),
		ZEVector4(0.0f, -1.0f, 0.0f, 0.0f),
		ZEVector4(0.0f, 1.0f, 0.0f, 0.0f),
		ZEVector4(0.0f, 0.0f, -1.0f, 0.0f),
		ZEVector4(0.0f, 0.0f, 1.0f, 0.0f),
	};

	for (ZEUInt I = 0; I < 14; ++I)
	{
		Samples[I].NormalizeSelf();
		Constants.OffsetVectors[I] = Samples[I] * (ZERandom::GetFloatPositive() * 0.75f + 0.25f);
	}
}

bool ZERNStageAO::Update()
{
	if(!UpdateConstantBuffers())
		return false;

	if(!UpdateTextures())
		return false;

	if(!UpdateShaders())
		return false;

	if(!UpdateRenderStates())
		return false;

	return true;
}

bool ZERNStageAO::UpdateConstantBuffers()
{
	if(!DirtyFlags.GetFlags(ZERN_AODF_CONSTANT_BUFFER))
		return true;

	ConstantBuffer->SetData(&Constants);
	FilterConstantBuffer->SetData(&FilterConstants);

	DirtyFlags.UnraiseFlags(ZERN_AODF_CONSTANT_BUFFER);

	return true;
}

bool ZERNStageAO::UpdateShaders()
{
	if(!DirtyFlags.GetFlags(ZERN_AODF_SHADER))
		return true;

	ZEGRShaderCompileOptions Options;
	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNSSAO.hlsl";
	Options.Model = ZEGR_SM_5_0;

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNScreenCover_VertexShader_Position";
	VertexShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNScreenCover_VertexShader_PositionTexcoords";
	BlendVertexShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNSSAO_PixelShader_Main";
	PixelShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNSSAO_Blend_PixelShader";
	BlendPixelShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNSSAO_Filter_PixelShader";
	FilterPixelShader = ZEGRShader::Compile(Options);

	DirtyFlags.UnraiseFlags(ZERN_AODF_SHADER);
	DirtyFlags.RaiseFlags(ZERN_AODF_RENDER_STATE);

	return true;
}

bool ZERNStageAO::UpdateRenderStates()
{
	if(!DirtyFlags.GetFlags(ZERN_AODF_RENDER_STATE))
		return true;

	ZEGRRenderState RenderState;

	ZEGRDepthStencilState DepthStencilState;
	DepthStencilState.SetDepthTestEnable(false);
	DepthStencilState.SetDepthWriteEnable(false);

	RenderState.SetDepthStencilState(DepthStencilState);

	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);
	RenderState.SetShader(ZEGR_ST_VERTEX, VertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, PixelShader);

	RenderStateData = RenderState.Compile();
	zeCheckError(RenderStateData == NULL, false, "Cannot set render state.");

	ZEGRBlendState BlendState;
	BlendState.SetBlendEnable(true);
	ZEGRBlendRenderTarget BlendRenderTarget = BlendState.GetRenderTarget(0);
	BlendRenderTarget.SetBlendEnable(true);
	BlendRenderTarget.SetSource(ZEGR_BO_ZERO);
	BlendRenderTarget.SetDestination(ZEGR_BO_SRC_COLOR);
	BlendRenderTarget.SetOperation(ZEGR_BE_ADD);

	BlendState.SetRenderTargetBlend(0, BlendRenderTarget);

	RenderState.SetBlendState(BlendState);
	RenderState.SetShader(ZEGR_ST_VERTEX, BlendVertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, BlendPixelShader);

	BlendRenderStateData = RenderState.Compile();
	zeCheckError(BlendRenderStateData == NULL, false, "Cannot set render state.");

	BlendState.SetToDefault();
	RenderState.SetBlendState(BlendState);

	RenderState.SetShader(ZEGR_ST_VERTEX, VertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, FilterPixelShader);

	FilterRenderStateData = RenderState.Compile();
	zeCheckError(FilterRenderStateData == NULL, false, "Cannot set render state.");

	DirtyFlags.UnraiseFlags(ZERN_AODF_RENDER_STATE);

	return true;
}
bool ZERNStageAO::UpdateTextures()
{
	if(!DirtyFlags.GetFlags(ZERN_AODF_TEXTURE))
		return true;

	ZEUInt TextureWidth = (ZEUInt)((float)Width / Constants.DownScale);
	ZEUInt TextureHeight = (ZEUInt)((float)Height / Constants.DownScale);

	OcclusionMap.Release();
	BlurTexture.Release();
	OcclusionMap = ZEGRTexture2D::CreateInstance(TextureWidth, TextureHeight, 1, 1, 1,ZEGR_TF_R16G16B16A16_FLOAT, true);
	BlurTexture = ZEGRTexture2D::CreateInstance(TextureWidth, TextureHeight, 1, 1, 1, ZEGR_TF_R16G16B16A16_FLOAT, true);

	DirtyFlags.UnraiseFlags(ZERN_AODF_TEXTURE);

	return true;
}

void ZERNStageAO::GenerateOcclusionMap(ZERNRenderer* Renderer, ZEGRContext* Context)
{
	ZERNStageGBuffer* StageGBuffer = (ZERNStageGBuffer*)Renderer->GetStage(ZERN_STAGE_GBUFFER);
	if(StageGBuffer == NULL)
		return;

	const ZEGRRenderTarget* RenderTarget = OcclusionMap->GetRenderTarget();

	Context->SetRenderState(RenderStateData);
	Context->SetRenderTargets(1, &RenderTarget, NULL);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, ConstantBuffer);
	Context->SetSampler(ZEGR_ST_PIXEL, 0, SamplerPointBorder.GetPointer());
	Context->SetSampler(ZEGR_ST_PIXEL, 1, SamplerPointWrap.GetPointer());
	Context->SetSampler(ZEGR_ST_PIXEL, 2, SamplerPointClamp.GetPointer());
	Context->SetTexture(ZEGR_ST_PIXEL, 0, StageGBuffer->GetDepthMap());
	Context->SetTexture(ZEGR_ST_PIXEL, 2, StageGBuffer->GetNormalMap());
	Context->SetTexture(ZEGR_ST_PIXEL, 5, RandomVectorsTexture);
	Context->SetVertexBuffers(0, 0, NULL);

	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, (float)OcclusionMap->GetWidth(), (float)OcclusionMap->GetHeight()));

	Context->Draw(3, 0);

	Context->SetRenderTargets(0, NULL, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, NULL);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, NULL);
}

void ZERNStageAO::ApplyBlur(ZEGRContext* Context)
{
	const ZEGRRenderTarget* BlurRenderTarget = BlurTexture->GetRenderTarget();
	const ZEGRRenderTarget* OcclusionRenderTarget = OcclusionMap->GetRenderTarget();

	Context->SetRenderState(FilterRenderStateData);
	Context->SetSampler(ZEGR_ST_PIXEL, 2, SamplerPointClamp.GetPointer());
	Context->SetSampler(ZEGR_ST_PIXEL, 3, SamplerLinearClamp.GetPointer());
	Context->SetVertexBuffers(0, 0, NULL);

	Context->SetRenderTargets(1, &BlurRenderTarget, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 6, OcclusionMap);

	SetFilterKernelValues(&HorizontalValues[0], (ZEUInt)HorizontalValues.GetCount());
	FilterConstantBuffer->SetData(&FilterConstants);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 9, FilterConstantBuffer);

	Context->Draw(3, 0);

	Context->SetRenderTargets(1, &OcclusionRenderTarget, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 6, BlurTexture);

	SetFilterKernelValues(&VerticalValues[0], (ZEUInt)VerticalValues.GetCount());
	FilterConstantBuffer->SetData(&FilterConstants);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 9, FilterConstantBuffer);

	Context->Draw(3, 0);

	Context->SetRenderTargets(1, &BlurRenderTarget, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 6, OcclusionMap);

	SetFilterKernelValues(&HorizontalValues[0], (ZEUInt)HorizontalValues.GetCount());
	FilterConstantBuffer->SetData(&FilterConstants);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 9, FilterConstantBuffer);

	Context->Draw(3, 0);

	Context->SetRenderTargets(1, &OcclusionRenderTarget, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 6, BlurTexture);

	SetFilterKernelValues(&VerticalValues[0], (ZEUInt)VerticalValues.GetCount());
	FilterConstantBuffer->SetData(&FilterConstants);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 9, FilterConstantBuffer);

	Context->Draw(3, 0);

	Context->SetRenderTargets(0, NULL, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 2, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 6, NULL);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 9, NULL);
}

void ZERNStageAO::BlendWithAccumulationBuffer(ZERNRenderer* Renderer, ZEGRContext* Context)
{
	ZERNStageGBuffer* StageGBuffer = (ZERNStageGBuffer*)Renderer->GetStage(ZERN_STAGE_GBUFFER);
	if(StageGBuffer == NULL)
		return;

	const ZEGRRenderTarget* RenderTarget = StageGBuffer->GetAccumulationMap()->GetRenderTarget();

	Context->SetRenderState(BlendRenderStateData);
	Context->SetRenderTargets(1, &RenderTarget, NULL);
	Context->SetSampler(ZEGR_ST_PIXEL, 3, SamplerLinearClamp.GetPointer());
	Context->SetTexture(ZEGR_ST_PIXEL, 6, OcclusionMap);
	Context->SetVertexBuffers(0, 0, NULL);

	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, (float)RenderTarget->GetWidth(), (float)RenderTarget->GetHeight()));

	Context->Draw(3, 0);

	Context->SetRenderTargets(0, NULL, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 0, NULL);
}

bool ZERNStageAO::InitializeSelf()
{
	if (!ZERNStage::InitializeSelf())
		return false;

	DirtyFlags.RaiseAll();

	CreateOffsetVectors();
	CreateRandomVectors();

	ZERNFilter::GenerateGaussianKernel(HorizontalValues, FilterConstants.KernelSize, 2.0f);
	ZERNFilter::GenerateGaussianKernel(VerticalValues, FilterConstants.KernelSize, 2.0f, false);

	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(SSAOConstants));
	FilterConstantBuffer = ZEGRConstantBuffer::Create(sizeof(SSAOFilterConstants));

	ZEGRSamplerDescription LinearSamplerDescription;
	SamplerLinearClamp = ZEGRSampler::GetSampler(LinearSamplerDescription);

	ZEGRSamplerDescription SamplerPointBorderDescription;
	SamplerPointBorderDescription.MinFilter = ZEGR_TFM_POINT;
	SamplerPointBorderDescription.MagFilter = ZEGR_TFM_POINT;
	SamplerPointBorderDescription.MipFilter = ZEGR_TFM_POINT;
	SamplerPointBorderDescription.AddressU = ZEGR_TAM_BORDER;
	SamplerPointBorderDescription.AddressV = ZEGR_TAM_BORDER;
	SamplerPointBorderDescription.BorderColor = ZEVector4(1.0f, 0.0f, 0.0f, 0.0f);
	SamplerPointBorder = ZEGRSampler::GetSampler(SamplerPointBorderDescription);

	ZEGRSamplerDescription SamplerPointWrapDescription;
	SamplerPointWrapDescription.MinFilter = ZEGR_TFM_POINT;
	SamplerPointWrapDescription.MagFilter = ZEGR_TFM_POINT;
	SamplerPointWrapDescription.MipFilter = ZEGR_TFM_POINT;
	SamplerPointWrapDescription.AddressU = ZEGR_TAM_WRAP;
	SamplerPointWrapDescription.AddressV = ZEGR_TAM_WRAP;
	SamplerPointWrap = ZEGRSampler::GetSampler(SamplerPointWrapDescription);

	ZEGRSamplerDescription SamplerPointClampDescription;
	SamplerPointClampDescription.MinFilter = ZEGR_TFM_POINT;
	SamplerPointClampDescription.MagFilter = ZEGR_TFM_POINT;
	SamplerPointClampDescription.MipFilter = ZEGR_TFM_POINT;
	SamplerPointClamp = ZEGRSampler::GetSampler(SamplerPointClampDescription);

	return true;
}

void ZERNStageAO::DeinitializeSelf()
{
	VertexShader.Release();
	PixelShader.Release();
	RenderStateData.Release();
	OcclusionMap.Release();
	BlurTexture.Release();
	BlendRenderStateData.Release();
	BlendPixelShader.Release();
	RandomVectorsTexture.Release();
	FilterConstantBuffer.Release();
	FilterPixelShader.Release();
	FilterRenderStateData.Release();

	HorizontalValues.Clear();
	VerticalValues.Clear();

	ZERNStage::DeinitializeSelf();
}

ZEInt ZERNStageAO::GetId() const
{
	return ZERN_STAGE_AO;
}

const ZEString& ZERNStageAO::GetName() const
{
	static const ZEString Name = "AO Stage";
	return Name;
}

const ZEGRTexture2D* ZERNStageAO::GetOcclusionMap() const
{
	return OcclusionMap;
}

void ZERNStageAO::SetOcclusionRadius(float Radius)
{
	if(Constants.OcclusionRadius == Radius)
		return;

	Constants.OcclusionRadius = Radius;

	DirtyFlags.RaiseFlags(ZERN_AODF_CONSTANT_BUFFER);
}

float ZERNStageAO::GetOcclusionRadius() const
{
	return Constants.OcclusionRadius;
}

void ZERNStageAO::SetMinDepthBias(float MinDepthBias)
{
	if(Constants.MinDepthBias == MinDepthBias)
		return;

	Constants.MinDepthBias = MinDepthBias;

	DirtyFlags.RaiseFlags(ZERN_AODF_CONSTANT_BUFFER);
}

float ZERNStageAO::GetMinDepthBias() const
{
	return Constants.MinDepthBias;
}

void ZERNStageAO::SetIntensity(float Intensity)
{
	if(Constants.Intensity == Intensity)
		return;

	Constants.Intensity = Intensity;

	DirtyFlags.RaiseFlags(ZERN_AODF_CONSTANT_BUFFER);
}

float ZERNStageAO::GetIntensity() const
{
	return Constants.Intensity;
}

void ZERNStageAO::SetFilterKernelValues(const ZEVector4* Values, ZEUInt KernelSize)
{
	if(memcmp(FilterConstants.KernelValues, Values, KernelSize * sizeof(ZEVector4)) == 0)
		return;

	memcpy(FilterConstants.KernelValues, Values, KernelSize * sizeof(ZEVector4));
	FilterConstants.KernelSize = KernelSize;

	DirtyFlags.RaiseFlags(ZERN_AODF_CONSTANT_BUFFER);
}

const ZEVector4* ZERNStageAO::GetFilterKernelValues() const
{
	return FilterConstants.KernelValues;
}

void ZERNStageAO::SetOcclusionMapDownScale(float DownScale)
{
	if(Constants.DownScale == DownScale)
		return;

	Constants.DownScale = DownScale;

	DirtyFlags.RaiseFlags(ZERN_AODF_TEXTURE | ZERN_AODF_CONSTANT_BUFFER);
}

float ZERNStageAO::GetOcclusionMapDownScale() const
{
	return Constants.DownScale;
}

bool ZERNStageAO::Setup(ZERNRenderer* Renderer, ZEGRContext* Context, ZEList2<ZERNCommand>& Commands)
{
	if (!ZERNStage::Setup(Renderer, Context, Commands))
		return false;

	ZEGRRenderTarget* RenderTarget = Renderer->GetOutputRenderTarget();
	if(RenderTarget == NULL)
		return false;

	ZEUInt CurrentWidth = RenderTarget->GetWidth();
	ZEUInt CurrentHeight = RenderTarget->GetHeight();

	if (Width != CurrentWidth || Height != CurrentHeight)
	{
		DirtyFlags.RaiseFlags(ZERN_AODF_TEXTURE);
		Width = CurrentWidth;
		Height = CurrentHeight;
	}

	if (!Update())
		return false;

	GenerateOcclusionMap(Renderer, Context);
	ApplyBlur(Context);
	BlendWithAccumulationBuffer(Renderer, Context);

	return true;
}

void ZERNStageAO::CleanUp(ZERNRenderer* Renderer, ZEGRContext* Context)
{
	Context->SetRenderTargets(0, NULL, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 2, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 6, NULL);

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, NULL);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 9, NULL);
	
	ZERNStage::CleanUp(Renderer, Context);
}

ZERNStageAO::ZERNStageAO()
{
	Width = 0;
	Height = 0;

	Constants.OcclusionRadius = 0.25f;
	Constants.MinDepthBias = 0.1f;
	Constants.Intensity = 16.0f;
	Constants.DownScale = 2.0f;

	FilterConstants.KernelSize = 9;
}
