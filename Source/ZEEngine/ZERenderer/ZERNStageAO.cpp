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
#include "ZEMath/ZEMath.h"
#include "ZEMath/ZEAngle.h"

#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRSampler.h"
#include "ZEGraphics/ZEGROutput.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRBuffer.h"
#include "ZEGraphics/ZEGRTexture.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZERNStageID.h"
#include "ZERNRenderer.h"

#define ZERN_AODF_CONSTANT_BUFFER	1
#define ZERN_AODF_SHADER			2
#define ZERN_AODF_RENDER_STATE		4
#define ZERN_AODF_TEXTURE			8
#define ZERN_AODF_OUTPUT			16

static ZEUInt ConvertSampleCount(ZERNSSAOSampleCount SampleCount)
{
	switch (SampleCount)
	{
		case ZERN_AOSC_LOW:
			return 8;
		default:
		case ZERN_AOSC_MEDIUM:
			return 16;
		case ZERN_AOSC_HIGH:
			return 32;
	}
}

void ZERNStageAO::CreateRandomVectors()
{
	const ZEUInt RandomVectorSize = 4;
	ZEUInt Size = RandomVectorSize * RandomVectorSize;

	RandomVectors.SetCount(Size);

	for (ZEUInt I = 0; I < Size; I++)
	{
		ZEVector3 Vector = ZEVector3(ZERandom::GetFloat(), ZERandom::GetFloat(), ZERandom::GetFloat());
		ZEVector3::Normalize(Vector, Vector);

		RandomVectors[I] = ZEVector4(Vector, 0.0f);
	}

	RandomVectorsTexture = ZEGRTexture::CreateResource(ZEGR_TT_2D, RandomVectorSize, RandomVectorSize, 1, ZEGR_TF_R32G32B32A32_FLOAT, ZEGR_RU_IMMUTABLE, ZEGR_RBF_SHADER_RESOURCE, 1, 1, RandomVectors.GetConstCArray());
}

void ZERNStageAO::CreateSphereSamples()
{
	for (ZEUInt I = 0; I < Constants.SampleCount; I++)
	{
		float Z = ZERandom::GetFloatPositive() * 2.0f - 1.0f;
		float Theta = ZERandom::GetFloatPositive() * ZE_PIx2;
		float Radius = ZEMath::Sqrt(ZEMath::Max(0.0f, 1.0f - Z * Z));

		Constants.SphereSamples[I].x = Radius * ZEAngle::Cos(Theta);
		Constants.SphereSamples[I].y = Radius * ZEAngle::Sin(Theta);
		Constants.SphereSamples[I].z = Z;
		Constants.SphereSamples[I].w = 0.0f;
	}
}

bool ZERNStageAO::UpdateShaders()
{
	if (!DirtyFlags.GetFlags(ZERN_AODF_SHADER))
		return true;

	ZEGRShaderCompileOptions Options;
	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNSSAO.hlsl";
	Options.Model = ZEGR_SM_5_0;
	Options.Definitions.Add(ZEGRShaderDefinition("SAMPLE_COUNT", ZEString::FromInt32(ZEGRGraphicsModule::SAMPLE_COUNT)));

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNScreenCover_VertexShader_PositionTexcoords";
	ScreenCoverPositionTexCoordVertexShader = ZEGRShader::Compile(Options);
	zeCheckError(ScreenCoverPositionTexCoordVertexShader == NULL, false, "Can not set vertex shader");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNSSAO_ResolveDepth_PixelShader";
	ResolveAndClampDepthPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(ResolveAndClampDepthPixelShader == NULL, false, "Can not set pixel shader");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNSSAO_SSAO_PixelShader_Main";
	SSAOPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(SSAOPixelShader == NULL, false, "Can not set pixel shader");

	Options.Definitions.Add(ZEGRShaderDefinition("DEINTERLEAVED"));

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNSSAO_ResolveDepth_PixelShader";
	ResolveAndLinearizeDepthPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(ResolveAndLinearizeDepthPixelShader == NULL, false, "Can not set pixel shader");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNSSAO_DeinterleaveDepth_PixelShader";
	DeinterleaveDepthPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(DeinterleaveDepthPixelShader == NULL, false, "Can not set pixel shader");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNSSAO_SSAO_PixelShader_Main";
	DeinterleaveSSAOPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(DeinterleaveSSAOPixelShader == NULL, false, "Can not set pixel shader");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNSSAO_ReinterleaveSSAO_PixelShader";
	ReinterleaveSSAOPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(ReinterleaveSSAOPixelShader == NULL, false, "Can not set pixel shader");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNSSAO_CrossBilateralBlurX_PixelShader";
	CrossBilateralBlurXPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(CrossBilateralBlurXPixelShader == NULL, false, "Can not set pixel shader");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNSSAO_CrossBilateralBlurY_PixelShader";
	CrossBilateralBlurYPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(CrossBilateralBlurYPixelShader == NULL, false, "Can not set pixel shader");

	DirtyFlags.UnraiseFlags(ZERN_AODF_SHADER);
	DirtyFlags.RaiseFlags(ZERN_AODF_RENDER_STATE);

	return true;
}

bool ZERNStageAO::UpdateRenderStates()
{
	if (!DirtyFlags.GetFlags(ZERN_AODF_RENDER_STATE))
		return true;

	ZEGRRenderState RenderState;

	RenderState.SetShader(ZEGR_ST_VERTEX, ScreenCoverPositionTexCoordVertexShader);

	ZEGRDepthStencilState DepthStencilStateTestWriteAlways;
	DepthStencilStateTestWriteAlways.SetDepthTestEnable(true);
	DepthStencilStateTestWriteAlways.SetDepthWriteEnable(true);
	DepthStencilStateTestWriteAlways.SetDepthFunction(ZEGR_CF_ALWAYS);

	RenderState.SetDepthStencilState(DepthStencilStateTestWriteAlways);

	RenderState.SetShader(ZEGR_ST_PIXEL, ResolveAndClampDepthPixelShader);
	ResolveAndClampDepthRenderStateData = RenderState.Compile();
	zeCheckError(ResolveAndClampDepthRenderStateData == NULL, false, "Cannot set render state.");

	ZEGRDepthStencilState DepthStencilStateTestNoWrite;
	DepthStencilStateTestNoWrite.SetDepthTestEnable(true);
	DepthStencilStateTestNoWrite.SetDepthWriteEnable(false);
	DepthStencilStateTestNoWrite.SetDepthFunction(ZEGR_CF_LESS);

	RenderState.SetDepthStencilState(DepthStencilStateTestNoWrite);

	RenderState.SetShader(ZEGR_ST_PIXEL, SSAOPixelShader);
	SSAORenderStateData = RenderState.Compile();
	zeCheckError(SSAORenderStateData == NULL, false, "Cannot set render state.");

	RenderState.SetDepthStencilState(DepthStencilStateTestWriteAlways);

	RenderState.SetShader(ZEGR_ST_PIXEL, ResolveAndLinearizeDepthPixelShader);
	ResolveAndLinearizeDepthRenderStateData = RenderState.Compile();
	zeCheckError(ResolveAndLinearizeDepthRenderStateData == NULL, false, "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_PIXEL, DeinterleaveDepthPixelShader);
	DeinterleaveDepthRenderStateData = RenderState.Compile();
	zeCheckError(DeinterleaveDepthRenderStateData == NULL, false, "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_PIXEL, DeinterleaveSSAOPixelShader);
	DeinterleaveSSAORenderStateData = RenderState.Compile();
	zeCheckError(DeinterleaveSSAORenderStateData == NULL, false, "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_PIXEL, ReinterleaveSSAOPixelShader);
	ReinterleaveSSAORenderStateData = RenderState.Compile();
	zeCheckError(ReinterleaveSSAORenderStateData == NULL, false, "Cannot set render state.");

	RenderState.SetDepthStencilState(DepthStencilStateTestNoWrite);

	RenderState.SetShader(ZEGR_ST_PIXEL, CrossBilateralBlurXPixelShader);
	CrossBilateralBlurXRenderStateData = RenderState.Compile();
	zeCheckError(CrossBilateralBlurXRenderStateData == NULL, false, "Cannot set render state.");

	ZEGRBlendState BlendState;
	BlendState.SetBlendEnable(true);
	ZEGRBlendRenderTarget RenderTarget0_MultiplyOcclusion = BlendState.GetRenderTarget(0);
	RenderTarget0_MultiplyOcclusion.SetBlendEnable(true);
	RenderTarget0_MultiplyOcclusion.SetSource(ZEGR_BO_ZERO);
	RenderTarget0_MultiplyOcclusion.SetDestination(ZEGR_BO_SRC_COLOR);
	RenderTarget0_MultiplyOcclusion.SetOperation(ZEGR_BE_ADD);
	BlendState.SetRenderTargetBlend(0, RenderTarget0_MultiplyOcclusion);

	RenderState.SetBlendState(BlendState);

	RenderState.SetShader(ZEGR_ST_PIXEL, CrossBilateralBlurYPixelShader);
	CrossBilateralBlurYRenderStateData = RenderState.Compile();
	zeCheckError(CrossBilateralBlurYRenderStateData == NULL, false, "Cannot set render state.");

	DirtyFlags.UnraiseFlags(ZERN_AODF_RENDER_STATE);

	return true;
}

bool ZERNStageAO::UpdateConstantBuffers()
{
	if (!DirtyFlags.GetFlags(ZERN_AODF_CONSTANT_BUFFER))
		return true;

	ConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZERN_AODF_CONSTANT_BUFFER);

	return true;
}

bool ZERNStageAO::UpdateTextures()
{
	if (!DirtyFlags.GetFlags(ZERN_AODF_TEXTURE))
		return true;

	ZEUInt Width = GetRenderer()->GetOutputTexture()->GetWidth();
	ZEUInt Height = GetRenderer()->GetOutputTexture()->GetHeight();

	AmbientOcclusionTexture = ZEGRTexture::CreateResource(ZEGR_TT_2D, Width, Height, 1, ZEGR_TF_R16G16_FLOAT);
	BlurTempTexture = ZEGRTexture::CreateResource(ZEGR_TT_2D, Width, Height, 1, ZEGR_TF_R16G16_FLOAT);
	
	if (UseDeinterleavedTexturing)
		ResolvedDepthTexture = ZEGRTexture::CreateResource(ZEGR_TT_2D, Width, Height, 1, ZEGR_TF_R32_FLOAT);
	else
		ResolvedDepthTexture = ZEGRTexture::CreateResource(ZEGR_TT_2D, Width, Height, 1, ZEGR_TF_D32_FLOAT, ZEGR_RU_STATIC, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_DEPTH_STENCIL);

	DeinterleavedDepthtexture = ZEGRTexture::CreateResource(ZEGR_TT_2D, Width / 4, Height / 4, 1, ZEGR_TF_R32_FLOAT, ZEGR_RU_STATIC, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_RENDER_TARGET, 16);
	DeinterleavedAmbientOcclusionTexture = ZEGRTexture::CreateResource(ZEGR_TT_2D, Width / 4, Height / 4, 1, ZEGR_TF_R16G16_FLOAT, ZEGR_RU_STATIC, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_RENDER_TARGET, 16);

	DirtyFlags.UnraiseFlags(ZERN_AODF_TEXTURE);

	return true;
}

bool ZERNStageAO::Update()
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

void ZERNStageAO::ResolveAndClampDepth(ZEGRContext* Context)
{
	Viewport.SetWidth((float)ResolvedDepthTexture->GetWidth());
	Viewport.SetHeight((float)ResolvedDepthTexture->GetHeight());

	Context->SetRenderState(ResolveAndClampDepthRenderStateData);
	Context->SetRenderTargets(0, NULL, ResolvedDepthTexture->GetDepthStencilBuffer());
	Context->SetViewports(1, &Viewport);

	Context->Draw(3, 0);
}

void ZERNStageAO::AmbientOcclusion(ZEGRContext* Context, const ZEGRDepthStencilBuffer* DepthStencilBuffer)
{
	Viewport.SetWidth((float)AmbientOcclusionTexture->GetWidth());
	Viewport.SetHeight((float)AmbientOcclusionTexture->GetHeight());

	const ZEGRRenderTarget* RenderTarget = AmbientOcclusionTexture->GetRenderTarget();
	const ZEGRTexture* Textures[] = {RandomVectorsTexture, ResolvedDepthTexture};

	Context->SetRenderState(SSAORenderStateData);
	Context->SetRenderTargets(1, &RenderTarget, DepthStencilBuffer);
	Context->SetTextures(ZEGR_ST_PIXEL, 6, 2, Textures);
	Context->SetViewports(1, &Viewport);

	Context->Draw(3, 0);
}

void ZERNStageAO::ResolveAndLinearizeDepth(ZEGRContext* Context)
{
	Viewport.SetWidth((float)ResolvedDepthTexture->GetWidth());
	Viewport.SetHeight((float)ResolvedDepthTexture->GetHeight());

	const ZEGRRenderTarget* RenderTarget = ResolvedDepthTexture->GetRenderTarget();

	Context->SetRenderState(ResolveAndLinearizeDepthRenderStateData);
	Context->SetRenderTargets(1, &RenderTarget, NULL);
	Context->SetViewports(1, &Viewport);

	Context->Draw(3, 0);
}

void ZERNStageAO::DeinterleaveDepth(ZEGRContext* Context)
{
	Viewport.SetWidth((float)DeinterleavedDepthtexture->GetWidth());
	Viewport.SetHeight((float)DeinterleavedDepthtexture->GetHeight());

	Context->SetRenderState(DeinterleaveDepthRenderStateData);
	Context->SetTexture(ZEGR_ST_PIXEL, 7, ResolvedDepthTexture);
	Context->SetViewports(1, &Viewport);

	ZEArray<const ZEGRRenderTarget*> RenderTargets;
	RenderTargets.SetCount(16);
	for (ZEUInt I = 0; I < 16; I++)
		RenderTargets[I] = DeinterleavedDepthtexture->GetRenderTarget(I);

	for (ZEUInt I = 0; I < 2; I++)
	{
		DeinterleavedConstants.Offset = ZEVector2(1.0f, I * 2 + 1);
		DeinterleavedConstantBuffer->SetData(&DeinterleavedConstants);

		Context->SetRenderTargets(8, &RenderTargets[I * 8], NULL);
		Context->Draw(3, 0);
	}

}

void ZERNStageAO::DeinterleaveAmbientOcclusion(ZEGRContext* Context)
{
	Viewport.SetWidth((float)DeinterleavedAmbientOcclusionTexture->GetWidth());
	Viewport.SetHeight((float)DeinterleavedAmbientOcclusionTexture->GetHeight());

	Context->SetRenderState(DeinterleaveSSAORenderStateData);
	Context->SetTexture(ZEGR_ST_PIXEL, 8, DeinterleavedDepthtexture);
	Context->SetViewports(1, &Viewport);

	for (ZEUInt I = 0; I < 16; I++)
	{
		DeinterleavedConstants.RandomVector = RandomVectors[I].ToVector3();
		DeinterleavedConstants.DepthArrayIndex = I;
		DeinterleavedConstants.Offset = ZEVector2(I % 4, I / 4) + ZEVector2(0.5f, 0.5f);
		DeinterleavedConstantBuffer->SetData(&DeinterleavedConstants);

		const ZEGRRenderTarget* RenderTarget = DeinterleavedAmbientOcclusionTexture->GetRenderTarget(I);
		Context->SetRenderTargets(1, &RenderTarget, NULL);
		Context->Draw(3, 0);
	}
}

void ZERNStageAO::ReinterleaveAmbientOcclusion(ZEGRContext* Context)
{
	Viewport.SetWidth((float)AmbientOcclusionTexture->GetWidth());
	Viewport.SetHeight((float)AmbientOcclusionTexture->GetHeight());

	const ZEGRRenderTarget* RenderTarget = AmbientOcclusionTexture->GetRenderTarget();

	Context->SetRenderState(ReinterleaveSSAORenderStateData);
	Context->SetRenderTargets(1, &RenderTarget, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 9, DeinterleavedAmbientOcclusionTexture);
	Context->SetViewports(1, &Viewport);

	Context->Draw(3, 0);
}

void ZERNStageAO::ApplyBlur(ZEGRContext* Context, const ZEGRTexture* InputTexture, const ZEGRDepthStencilBuffer* DepthStencilBuffer)
{
	Viewport.SetWidth((float)(InputTexture->GetWidth()));
	Viewport.SetHeight((float)(InputTexture->GetHeight()));

	const ZEGRRenderTarget* TempRenderTarget = BlurTempTexture->GetRenderTarget();

	Context->SetRenderState(CrossBilateralBlurXRenderStateData);
	Context->SetRenderTargets(1, &TempRenderTarget, DepthStencilBuffer);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, InputTexture);
	Context->SetViewports(1, &Viewport);
	Context->Draw(3, 0);

	Viewport.SetWidth((float)(AccumulationTexture->GetWidth()));
	Viewport.SetHeight((float)(AccumulationTexture->GetHeight()));

	const ZEGRRenderTarget* RenderTarget = AccumulationTexture->GetRenderTarget();

	Context->SetRenderState(CrossBilateralBlurYRenderStateData);
	Context->SetRenderTargets(1, &RenderTarget, DepthTexture->GetDepthStencilBuffer(true));
	Context->SetTexture(ZEGR_ST_PIXEL, 5, BlurTempTexture);
	Context->SetViewports(1, &Viewport);
	Context->Draw(3, 0);
}

bool ZERNStageAO::InitializeInternal()
{
	if (!ZERNStage::InitializeInternal())
		return false;

	CreateSphereSamples();
	CreateRandomVectors();

	ConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, sizeof(Constants), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);
	DeinterleavedConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, sizeof(DeinterleavedConstants), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);

	return Update();
}

bool ZERNStageAO::DeinitializeInternal()
{
	DirtyFlags.RaiseAll();

	ScreenCoverPositionTexCoordVertexShader.Release();

	ResolveAndClampDepthPixelShader.Release();
	SSAOPixelShader.Release();

	ResolveAndLinearizeDepthPixelShader.Release();
	DeinterleaveDepthPixelShader.Release();
	DeinterleaveSSAOPixelShader.Release();
	ReinterleaveSSAOPixelShader.Release();

	CrossBilateralBlurXPixelShader.Release();
	CrossBilateralBlurYPixelShader.Release();

	ResolveAndClampDepthRenderStateData.Release();
	SSAORenderStateData.Release();

	ResolveAndLinearizeDepthRenderStateData.Release();
	DeinterleaveDepthRenderStateData.Release();
	DeinterleaveSSAORenderStateData.Release();
	ReinterleaveSSAORenderStateData.Release();

	CrossBilateralBlurXRenderStateData.Release();
	CrossBilateralBlurYRenderStateData.Release();

	ConstantBuffer.Release();
	DeinterleavedConstantBuffer.Release();

	AmbientOcclusionTexture.Release();
	BlurTempTexture.Release();
	RandomVectorsTexture.Release();
	ResolvedDepthTexture.Release();
	DeinterleavedDepthtexture.Release();
	DeinterleavedAmbientOcclusionTexture.Release();

	DepthTexture = NULL;
	NormalTexture = NULL;
	AccumulationTexture = NULL;

	return ZERNStage::DeinitializeInternal();
}

void ZERNStageAO::CreateOutput(const ZEString& Name)
{
	ZEUInt Width = GetRenderer()->GetOutputTexture()->GetWidth();
	ZEUInt Height = GetRenderer()->GetOutputTexture()->GetHeight();

	if (Name == "ColorTexture")
	{
		if (DirtyFlags.GetFlags(ZERN_AODF_OUTPUT) || AccumulationTexture == GetRenderer()->GetOutputTexture())
		{
			AccumulationTexture = ZEGRTexture::CreateResource(ZEGR_TT_2D, Width, Height, 1, ZEGR_TF_R11G11B10_FLOAT, ZEGR_RU_STATIC, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_RENDER_TARGET, 1, ZEGRGraphicsModule::SAMPLE_COUNT).GetPointer();
			DirtyFlags.UnraiseFlags(ZERN_AODF_OUTPUT);
		}
	}
}

ZEInt ZERNStageAO::GetId() const
{
	return ZERN_STAGE_AO;
}

const ZEString& ZERNStageAO::GetName() const
{
	static const ZEString Name = "Stage Ambient Occlusion";
	return Name;
}

void ZERNStageAO::SetSampleCount(ZERNSSAOSampleCount SampleCount)
{
	if (this->SampleCount == SampleCount)
		return;

	this->SampleCount = SampleCount;
	Constants.SampleCount = ConvertSampleCount(SampleCount);
	CreateSphereSamples();

	DirtyFlags.RaiseFlags(ZERN_AODF_CONSTANT_BUFFER);
}

ZERNSSAOSampleCount ZERNStageAO::GetSampleCount() const
{
	return SampleCount;
}

void ZERNStageAO::SetOcclusionRadius(float Radius)
{
	if (Constants.OcclusionRadius == Radius)
		return;

	Constants.OcclusionRadius = Radius;

	DirtyFlags.RaiseFlags(ZERN_AODF_CONSTANT_BUFFER);
}

float ZERNStageAO::GetOcclusionRadius() const
{
	return Constants.OcclusionRadius;
}

void ZERNStageAO::SetNormalBias(float NormalBias)
{
	if (Constants.NormalBias == NormalBias)
		return;

	Constants.NormalBias = NormalBias;

	DirtyFlags.RaiseFlags(ZERN_AODF_CONSTANT_BUFFER);
}

float ZERNStageAO::GetNormalBias() const
{
	return Constants.NormalBias;
}

void ZERNStageAO::SetIntensity(float Intensity)
{
	if (Constants.Intensity == Intensity)
		return;

	Constants.Intensity = Intensity;

	DirtyFlags.RaiseFlags(ZERN_AODF_CONSTANT_BUFFER);
}

float ZERNStageAO::GetIntensity() const
{
	return Constants.Intensity;
}

void ZERNStageAO::SetDistanceThreshold(float DistanceThreshold)
{
	if (Constants.DistanceThreshold == DistanceThreshold)
		return;

	Constants.DistanceThreshold = DistanceThreshold;

	DirtyFlags.RaiseFlags(ZERN_AODF_CONSTANT_BUFFER);
}

float ZERNStageAO::GetDistanceThreshold() const
{
	return Constants.DistanceThreshold;
}

void ZERNStageAO::SetDeinterleavedTexturing(bool UseDeinterleavedTexturing)
{
	if (this->UseDeinterleavedTexturing == UseDeinterleavedTexturing)
		return;

	this->UseDeinterleavedTexturing = UseDeinterleavedTexturing;

	DirtyFlags.RaiseFlags(ZERN_AODF_TEXTURE);
}

bool ZERNStageAO::GetDeinterleavedTexturing() const
{
	return UseDeinterleavedTexturing;
}

void ZERNStageAO::Resized(ZEUInt Width, ZEUInt Height)
{
	Constants.WidthHeight.x = Width;
	Constants.WidthHeight.y = Height;
	Constants.InvWidthHeight.x = 1.0f / Width;
	Constants.InvWidthHeight.y = 1.0f / Height;

	DirtyFlags.RaiseFlags(ZERN_AODF_TEXTURE | ZERN_AODF_CONSTANT_BUFFER | ZERN_AODF_OUTPUT);
}

bool ZERNStageAO::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	if (!Update())
		return false;

	Context->ClearRenderTarget(AmbientOcclusionTexture->GetRenderTarget(), ZEVector4::One);

	const ZEGRBuffer* ConstantBuffers[] = {ConstantBuffer, DeinterleavedConstantBuffer};
	Context->SetConstantBuffers(ZEGR_ST_PIXEL, 8, 2, ConstantBuffers);

	const ZEGRTexture* Textures[] = {NormalTexture, DepthTexture};
	Context->SetTextures(ZEGR_ST_PIXEL, 3, 2, Textures);

	const ZEGRDepthStencilBuffer* ReadonlyDepthStencilBuffer = NULL;

	if (!UseDeinterleavedTexturing)
	{
		Context->ClearDepthStencilBuffer(ResolvedDepthTexture->GetDepthStencilBuffer(), true, true, 0.0f, 0x00);

		ReadonlyDepthStencilBuffer = ResolvedDepthTexture->GetDepthStencilBuffer(true);

		ResolveAndClampDepth(Context);
		AmbientOcclusion(Context, ReadonlyDepthStencilBuffer);
	}
	else
	{
		ResolveAndLinearizeDepth(Context);
		DeinterleaveDepth(Context);
		DeinterleaveAmbientOcclusion(Context);
		ReinterleaveAmbientOcclusion(Context);
	}

	ApplyBlur(Context, AmbientOcclusionTexture, ReadonlyDepthStencilBuffer);

	return true;
}

void ZERNStageAO::CleanUp(ZEGRContext* Context)
{	
	ZERNStage::CleanUp(Context);
}

ZERNStageAO::ZERNStageAO()
{
	DirtyFlags.RaiseAll();

	SampleCount = ZERN_AOSC_MEDIUM;

	UseDeinterleavedTexturing = true;

	Constants.SampleCount = ConvertSampleCount(SampleCount);
	Constants.OcclusionRadius = 2.0f;
	Constants.NormalBias = 0.05f;
	Constants.Intensity = 16.0f;

	Constants.WidthHeight = ZEVector2::Zero;
	Constants.InvWidthHeight = ZEVector2::Zero;

	Constants.KernelRadius = 3;
	Constants.BlurSharpness = 8.0f;
	Constants.DistanceThreshold = 100.0f;

	memset(&DeinterleavedConstants, 0, sizeof(DeinterleavedConstants));

	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&DepthTexture), "DepthTexture", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV | ZERN_SRCF_REQUIRED);
	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&NormalTexture), "GBuffer3", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV);

	AddOutputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&AccumulationTexture), "ColorTexture", ZERN_SRUT_WRITE, ZERN_SRCF_GET_FROM_PREV | ZERN_SRCF_CREATE_OWN | ZERN_SRCF_GET_OUTPUT);
}

ZERNStageAO::~ZERNStageAO()
{
	Deinitialize();
}
