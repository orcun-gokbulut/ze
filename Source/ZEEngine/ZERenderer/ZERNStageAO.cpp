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
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZERNFilter.h"
#include "ZERNStageID.h"

#define ZERN_AODF_CONSTANT_BUFFER	1
#define ZERN_AODF_SHADER			2
#define ZERN_AODF_RENDER_STATE		4
#define ZERN_AODF_TEXTURE			8

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

	ZEArray<ZEVector4> RandomVectors;
	RandomVectors.SetCount(Size);

	for (ZEUInt I = 0; I < Size; I++)
	{
		ZEVector3 Vector = ZEVector3(ZERandom::GetFloat(), ZERandom::GetFloat(), ZERandom::GetFloat());
		ZEVector3::Normalize(Vector, Vector);

		RandomVectors[I] = ZEVector4(Vector, 0.0f);
	}

	RandomVectorsTexture = ZEGRTexture2D::CreateInstance(RandomVectorSize, RandomVectorSize, 1, ZEGR_TF_R32G32B32A32_FLOAT, ZEGR_RU_GPU_READ_ONLY, ZEGR_RBF_SHADER_RESOURCE, 1, 1, RandomVectors.GetConstCArray());
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
	Options.Definitions.Add(ZEGRShaderDefinition("SAMPLE_COUNT", ZEString(ZEGRGraphicsModule::SAMPLE_COUNT)));

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNScreenCover_VertexShader_PositionTexcoords";
	ScreenCoverPositionTexCoordVertexShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNSSAO_ResolveAndScale_PixelShader";
	ResolveAndScalePixelShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNSSAO_PixelShader_Main";
	SSAOPixelShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNSSAO_CrossBilateralBlurX_PixelShader";
	CrossBilateralBlurXPixelShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNSSAO_CrossBilateralBlurY_PixelShader";
	CrossBilateralBlurYPixelShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNSSAO_Blend_PixelShader";
	BlendPixelShader = ZEGRShader::Compile(Options);

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

	ZEGRDepthStencilState DepthStencilStateAlwaysTestWrite;
	DepthStencilStateAlwaysTestWrite.SetDepthTestEnable(true);
	DepthStencilStateAlwaysTestWrite.SetDepthWriteEnable(true);
	DepthStencilStateAlwaysTestWrite.SetDepthFunction(ZEGR_CF_ALWAYS);

	RenderState.SetDepthStencilState(DepthStencilStateAlwaysTestWrite);

	RenderState.SetShader(ZEGR_ST_PIXEL, ResolveAndScalePixelShader);
	ResolveAndScaleRenderStateData = RenderState.Compile();
	zeCheckError(ResolveAndScaleRenderStateData == NULL, false, "Cannot set render state.");

	ZEGRDepthStencilState DepthStencilStateTestNoWrite;
	DepthStencilStateTestNoWrite.SetDepthTestEnable(true);
	DepthStencilStateTestNoWrite.SetDepthWriteEnable(false);
	DepthStencilStateTestNoWrite.SetDepthFunction(ZEGR_CF_LESS);

	RenderState.SetDepthStencilState(DepthStencilStateTestNoWrite);

	RenderState.SetShader(ZEGR_ST_PIXEL, SSAOPixelShader);
	SSAORenderStateData = RenderState.Compile();
	zeCheckError(SSAORenderStateData == NULL, false, "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_PIXEL, CrossBilateralBlurXPixelShader);
	CrossBilateralBlurXRenderStateData = RenderState.Compile();
	zeCheckError(CrossBilateralBlurXRenderStateData == NULL, false, "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_PIXEL, CrossBilateralBlurYPixelShader);
	CrossBilateralBlurYRenderStateData = RenderState.Compile();
	zeCheckError(CrossBilateralBlurYRenderStateData == NULL, false, "Cannot set render state.");

	ZEGRBlendState BlendState;
	BlendState.SetBlendEnable(true);
	ZEGRBlendRenderTarget RenderTarget0_MultiplyOcclusion = BlendState.GetRenderTarget(0);
	RenderTarget0_MultiplyOcclusion.SetBlendEnable(true);
	RenderTarget0_MultiplyOcclusion.SetSource(ZEGR_BO_ZERO);
	RenderTarget0_MultiplyOcclusion.SetDestination(ZEGR_BO_SRC_COLOR);
	RenderTarget0_MultiplyOcclusion.SetOperation(ZEGR_BE_ADD);
	BlendState.SetRenderTargetBlend(0, RenderTarget0_MultiplyOcclusion);

	RenderState.SetBlendState(BlendState);

	RenderState.SetShader(ZEGR_ST_PIXEL, BlendPixelShader);
	BlendRenderStateData = RenderState.Compile();
	zeCheckError(BlendRenderStateData == NULL, false, "Cannot set render state.");

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

bool ZERNStageAO::UpdateInputOutputs()
{
	DepthTexture = GetPrevOutput(ZERN_SO_DEPTH);
	if (DepthTexture == NULL)
		return false;

	NormalTexture = GetPrevOutput(ZERN_SO_NORMAL);
	if (NormalTexture == NULL)
		return false;

	AccumulationTexture = GetPrevOutput(ZERN_SO_ACCUMULATION);
	if (AccumulationTexture == NULL)
		return false;

	ZEUInt Width = AccumulationTexture->GetWidth();
	ZEUInt Height = AccumulationTexture->GetHeight();
	if (Constants.WidthHeight.x != Width || Constants.WidthHeight.y != Height)
	{
		Constants.WidthHeight.x = Width;
		Constants.WidthHeight.y = Height;
		Constants.InvWidthHeight.x = 1.0f / Width;
		Constants.InvWidthHeight.y = 1.0f / Height;

		DirtyFlags.RaiseFlags(ZERN_AODF_TEXTURE | ZERN_AODF_CONSTANT_BUFFER);
	}

	return true;
}

bool ZERNStageAO::UpdateTextures()
{
	if (!DirtyFlags.GetFlags(ZERN_AODF_TEXTURE))
		return true;

	ZEUInt Width = Constants.WidthHeight.x;
	ZEUInt Height = Constants.WidthHeight.y;

	OcclusionMap = ZEGRTexture2D::CreateInstance(Width, Height, 1, ZEGR_TF_R16G16_FLOAT);
	BlurTempTexture = ZEGRTexture2D::CreateInstance(Width, Height, 1, ZEGR_TF_R16G16_FLOAT);
	ResolvedScaledDepthTexture = ZEGRTexture2D::CreateInstance(Width, Height, 1, ZEGR_TF_D32_FLOAT, DepthTexture->GetResourceUsage(), DepthTexture->GetResourceBindFlags());

	DirtyFlags.UnraiseFlags(ZERN_AODF_TEXTURE);

	return true;
}

bool ZERNStageAO::Update()
{
	if (!UpdateShaders())
		return false;

	if (!UpdateRenderStates())
		return false;

	if (!UpdateInputOutputs())
		return false;

	if (!UpdateTextures())
		return false;

	if (!UpdateConstantBuffers())
		return false;

	return true;
}

void ZERNStageAO::ResolveAndScaleDepth(ZEGRContext* Context)
{
	Viewport.SetWidth((float)ResolvedScaledDepthTexture->GetWidth());
	Viewport.SetHeight((float)ResolvedScaledDepthTexture->GetHeight());

	Context->SetRenderState(ResolveAndScaleRenderStateData);
	Context->SetRenderTargets(0, NULL, ResolvedScaledDepthTexture->GetDepthStencilBuffer());
	Context->SetViewports(1, &Viewport);

	Context->Draw(3, 0);
}

void ZERNStageAO::GenerateOcclusionMap(ZEGRContext* Context, const ZEGRDepthStencilBuffer* DepthStencilBuffer)
{
	Viewport.SetWidth((float)OcclusionMap->GetWidth());
	Viewport.SetHeight((float)OcclusionMap->GetHeight());

	const ZEGRRenderTarget* RenderTarget = OcclusionMap->GetRenderTarget();
	const ZEGRTexture* Textures[] = {RandomVectorsTexture, ResolvedScaledDepthTexture};

	Context->SetRenderState(SSAORenderStateData);
	Context->SetRenderTargets(1, &RenderTarget, DepthStencilBuffer);
	Context->SetTextures(ZEGR_ST_PIXEL, 6, 2, Textures);
	Context->SetViewports(1, &Viewport);

	Context->Draw(3, 0);
}

void ZERNStageAO::ApplyBlur(ZEGRContext* Context, const ZEGRTexture2D* InputTexture, const ZEGRDepthStencilBuffer* DepthStencilBuffer)
{
	Viewport.SetWidth((float)(InputTexture->GetWidth()));
	Viewport.SetHeight((float)(InputTexture->GetHeight()));

	const ZEGRRenderTarget* TempRenderTarget = BlurTempTexture->GetRenderTarget();
	const ZEGRRenderTarget* OutputRenderTarget = InputTexture->GetRenderTarget();

	Context->SetRenderState(CrossBilateralBlurXRenderStateData);
	Context->SetRenderTargets(1, &TempRenderTarget, DepthStencilBuffer);
	Context->SetTextures(ZEGR_ST_PIXEL, 5, 1, reinterpret_cast<const ZEGRTexture**>(&InputTexture));
	Context->SetViewports(1, &Viewport);
	Context->Draw(3, 0);

	Context->SetRenderState(CrossBilateralBlurYRenderStateData);
	Context->SetRenderTargets(1, &OutputRenderTarget, DepthStencilBuffer);
	Context->SetTextures(ZEGR_ST_PIXEL, 5, 1, reinterpret_cast<const ZEGRTexture**>(&BlurTempTexture));
	Context->SetViewports(1, &Viewport);
	Context->Draw(3, 0);
}

void ZERNStageAO::BlendWithAccumulation(ZEGRContext* Context, const ZEGRTexture2D* InputTexture)
{
	Viewport.SetWidth((float)AccumulationTexture->GetWidth());
	Viewport.SetHeight((float)AccumulationTexture->GetHeight());

	const ZEGRRenderTarget* RenderTarget = AccumulationTexture->GetRenderTarget();

	Context->SetRenderState(BlendRenderStateData);
	Context->SetRenderTargets(1, &RenderTarget, DepthTexture->GetDepthStencilBuffer(true));
	Context->SetTextures(ZEGR_ST_PIXEL, 5, 1, reinterpret_cast<const ZEGRTexture**>(&InputTexture));
	Context->SetViewports(1, &Viewport);

	Context->Draw(3, 0);
}

bool ZERNStageAO::InitializeSelf()
{
	if (!ZERNStage::InitializeSelf())
		return false;

	Filter.Initialize();

	CreateSphereSamples();
	CreateRandomVectors();

	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(Constants));

	return Update();
}

void ZERNStageAO::DeinitializeSelf()
{
	DirtyFlags.RaiseAll();

	ScreenCoverPositionTexCoordVertexShader.Release();
	SSAOPixelShader.Release();
	CrossBilateralBlurXPixelShader.Release();
	CrossBilateralBlurYPixelShader.Release();
	ResolveAndScalePixelShader.Release();
	BlendPixelShader.Release();

	SSAORenderStateData.Release();
	CrossBilateralBlurXRenderStateData.Release();
	CrossBilateralBlurYRenderStateData.Release();
	ResolveAndScaleRenderStateData.Release();
	BlendRenderStateData.Release();

	ConstantBuffer.Release();

	OcclusionMap.Release();
	BlurTempTexture.Release();
	RandomVectorsTexture.Release();
	ResolvedScaledDepthTexture.Release();

	DepthTexture = NULL;
	NormalTexture = NULL;
	AccumulationTexture = NULL;

	ZERNStage::DeinitializeSelf();
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

const ZEGRTexture2D* ZERNStageAO::GetOutput(ZERNStageBuffer Output) const
{
	if (GetEnabled() && Output == ZERN_SO_AMBIENT_OCCLUSION)
		return OcclusionMap;

	return ZERNStage::GetOutput(Output);
}

bool ZERNStageAO::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	if (!Update())
		return false;
	
	Context->ClearDepthStencilBuffer(ResolvedScaledDepthTexture->GetDepthStencilBuffer(), true, true, 0.0f, 0x00);
	Context->ClearRenderTarget(OcclusionMap->GetRenderTarget(), ZEVector4::One);

	Context->SetConstantBuffers(ZEGR_ST_PIXEL, 8, 1, ConstantBuffer.GetPointerToPointer());
	const ZEGRTexture* Textures[] = {NormalTexture, DepthTexture};
	Context->SetTextures(ZEGR_ST_PIXEL, 3, 2, Textures);

	const ZEGRDepthStencilBuffer* ReadOnlyDepthBuffer = ResolvedScaledDepthTexture->GetDepthStencilBuffer(true);

	ResolveAndScaleDepth(Context);
	GenerateOcclusionMap(Context, ReadOnlyDepthBuffer);
	ApplyBlur(Context, OcclusionMap, ReadOnlyDepthBuffer);
	BlendWithAccumulation(Context, OcclusionMap);

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

	DepthTexture = NULL;
	NormalTexture = NULL;
	AccumulationTexture = NULL;

	Constants.SampleCount = ConvertSampleCount(SampleCount);
	Constants.OcclusionRadius = 2.0f;
	Constants.NormalBias = 0.05f;
	Constants.Intensity = 16.0f;

	Constants.WidthHeight = ZEVector2::Zero;
	Constants.InvWidthHeight = ZEVector2::Zero;

	Constants.KernelRadius = 3;
	Constants.BlurSharpness = 8.0f;
	Constants.DistanceThreshold = 100.0f;
}
