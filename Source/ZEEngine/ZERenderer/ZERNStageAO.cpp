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
#include "ZEMath/ZEMath.h"
#include "ZEMath/ZEAngle.h"

#include "ZERNFilter.h"
#include "ZERNRenderer.h"
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
	RandomVectorsTexture = ZEGRTexture2D::CreateInstance(128, 128, 1, ZEGR_TF_R8G8B8A8_UNORM, ZEGR_RU_GPU_READ_WRITE_CPU_WRITE, ZEGR_RBF_SHADER_RESOURCE);
	RandomVectorsTexture->UpdateSubResource(0, 0, NULL, &Vectors[0], 128 * 4);
}

void ZERNStageAO::CreateSphereSamples()
{
	for(ZEUInt I = 0; I < Constants.SampleCount; I++)
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

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNScreenCover_VertexShader_Position";
	ScreenCoverVertexShaderPosition = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNSSAO_PixelShader_Main";
	SSAOPixelShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNSSAO_Filter_PixelShader";
	FilterPixelShader = ZEGRShader::Compile(Options);

	DirtyFlags.UnraiseFlags(ZERN_AODF_SHADER);
	DirtyFlags.RaiseFlags(ZERN_AODF_RENDER_STATE);

	return true;
}

bool ZERNStageAO::UpdateRenderStates()
{
	if (!DirtyFlags.GetFlags(ZERN_AODF_RENDER_STATE))
		return true;

	ZEGRRenderState RenderState;
	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);

	ZEGRDepthStencilState DepthStencilState;
	DepthStencilState.SetDepthTestEnable(false);
	DepthStencilState.SetDepthWriteEnable(false);

	RenderState.SetDepthStencilState(DepthStencilState);
	RenderState.SetShader(ZEGR_ST_VERTEX, ScreenCoverVertexShaderPosition);
	
	RenderState.SetShader(ZEGR_ST_PIXEL, SSAOPixelShader);

	SSAORenderStateData = RenderState.Compile();
	zeCheckError(SSAORenderStateData == NULL, false, "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_PIXEL, FilterPixelShader);

	FilterRenderStateData = RenderState.Compile();
	zeCheckError(FilterRenderStateData == NULL, false, "Cannot set render state.");

	DirtyFlags.UnraiseFlags(ZERN_AODF_RENDER_STATE);

	return true;
}

bool ZERNStageAO::UpdateConstantBuffers()
{
	if (!DirtyFlags.GetFlags(ZERN_AODF_CONSTANT_BUFFER))
		return true;

	SSAOConstantBuffer->SetData(&Constants);
	FilterConstantBuffer->SetData(&FilterConstants);

	DirtyFlags.UnraiseFlags(ZERN_AODF_CONSTANT_BUFFER);

	return true;
}

bool ZERNStageAO::UpdateInputOutputs()
{
	InputDepth = GetPrevOutput(ZERN_SO_DEPTH);
	InputNormal = GetPrevOutput(ZERN_SO_NORMAL);

	if (InputDepth == NULL || InputNormal == NULL)
		return false;

	ZEUInt CurrentWidth = InputDepth->GetWidth();
	ZEUInt CurrentHeight = InputDepth->GetHeight();
	if (Width != CurrentWidth || Height != CurrentHeight)
	{
		Width = CurrentWidth;
		Height = CurrentHeight;

		DirtyFlags.RaiseFlags(ZERN_AODF_TEXTURE);
	}

	return true;
}

bool ZERNStageAO::UpdateTextures()
{
	if (!DirtyFlags.GetFlags(ZERN_AODF_TEXTURE))
		return true;

	ZEUInt TextureWidth = static_cast<ZEUInt>(Width / Constants.DownScale);
	ZEUInt TextureHeight = static_cast<ZEUInt>(Height / Constants.DownScale);

	OcclusionMap = ZEGRTexture2D::CreateInstance(TextureWidth, TextureHeight, 1, ZEGR_TF_R16G16B16A16_FLOAT, ZEGR_RU_GPU_READ_WRITE_CPU_WRITE);
	BlurTexture = ZEGRTexture2D::CreateInstance(TextureWidth, TextureHeight, 1, ZEGR_TF_R16G16B16A16_FLOAT, ZEGR_RU_GPU_READ_WRITE_CPU_WRITE);

	DirtyFlags.UnraiseFlags(ZERN_AODF_TEXTURE);

	return true;
}

bool ZERNStageAO::Update()
{
	if (!UpdateShaders())
		return false;

	if (!UpdateRenderStates())
		return false;

	if (!UpdateConstantBuffers())
		return false;

	if (!UpdateInputOutputs())
		return false;

	if (!UpdateTextures())
		return false;

	return true;
}

void ZERNStageAO::GenerateOcclusionMap(ZEGRContext* Context)
{
	Viewport.SetWidth((float)Width);
	Viewport.SetHeight((float)Height);

	const ZEGRRenderTarget* RenderTarget = OcclusionMap->GetRenderTarget();

	Context->SetRenderState(SSAORenderStateData);
	Context->SetRenderTargets(1, &RenderTarget, NULL);
	Context->SetConstantBuffers(ZEGR_ST_PIXEL, 8, 1, SSAOConstantBuffer.GetPointerToPointer());
	Context->SetSamplers(ZEGR_ST_PIXEL, 0, 1, SamplerPointWrap.GetPointerToPointer());
	Context->SetTextures(ZEGR_ST_PIXEL, 5, 1, reinterpret_cast<ZEGRTexture**>(&RandomVectorsTexture));

	Context->SetViewports(1, &Viewport);

	Context->Draw(3, 0);

	Context->SetRenderTargets(0, NULL, NULL);
	Context->SetSamplers(ZEGR_ST_PIXEL, 0, 1, NULL);
	Context->SetTextures(ZEGR_ST_PIXEL, 5, 1, NULL);
	Context->SetConstantBuffers(ZEGR_ST_PIXEL, 8, 1, NULL);
}

void ZERNStageAO::ApplyBlur(ZEGRContext* Context)
{
	const ZEGRRenderTarget* BlurRenderTarget = BlurTexture->GetRenderTarget();
	const ZEGRRenderTarget* OcclusionRenderTarget = OcclusionMap->GetRenderTarget();

	Viewport.SetWidth((float)(OcclusionRenderTarget->GetWidth()));
	Viewport.SetHeight((float)(OcclusionRenderTarget->GetHeight()));

	Context->SetRenderState(FilterRenderStateData);
	Context->SetViewports(1, &Viewport);

	Context->SetRenderTargets(1, &BlurRenderTarget, NULL);
	Context->SetTextures(ZEGR_ST_PIXEL, 6, 1, reinterpret_cast<ZEGRTexture**>(&OcclusionMap));

	SetFilterKernelValues(&HorizontalValues[0], HorizontalValues.GetCount());
	FilterConstantBuffer->SetData(&FilterConstants);
	Context->SetConstantBuffers(ZEGR_ST_PIXEL, 9, 1, FilterConstantBuffer.GetPointerToPointer());

	Context->Draw(3, 0);

	Context->SetRenderTargets(1, &OcclusionRenderTarget, NULL);
	Context->SetTextures(ZEGR_ST_PIXEL, 6, 1, reinterpret_cast<ZEGRTexture**>(&BlurTexture));

	SetFilterKernelValues(&VerticalValues[0], VerticalValues.GetCount());
	FilterConstantBuffer->SetData(&FilterConstants);
	Context->SetConstantBuffers(ZEGR_ST_PIXEL, 9, 1, FilterConstantBuffer.GetPointerToPointer());

	Context->Draw(3, 0);

	Context->SetRenderTargets(1, &BlurRenderTarget, NULL);
	Context->SetTextures(ZEGR_ST_PIXEL, 6, 1, reinterpret_cast<ZEGRTexture**>(&OcclusionMap));

	SetFilterKernelValues(&HorizontalValues[0], HorizontalValues.GetCount());
	FilterConstantBuffer->SetData(&FilterConstants);
	Context->SetConstantBuffers(ZEGR_ST_PIXEL, 9, 1, FilterConstantBuffer.GetPointerToPointer());

	Context->Draw(3, 0);

	Context->SetRenderTargets(1, &OcclusionRenderTarget, NULL);
	Context->SetTextures(ZEGR_ST_PIXEL, 6, 1, reinterpret_cast<ZEGRTexture**>(&BlurTexture));

	SetFilterKernelValues(&VerticalValues[0], VerticalValues.GetCount());
	FilterConstantBuffer->SetData(&FilterConstants);
	Context->SetConstantBuffers(ZEGR_ST_PIXEL, 9, 1, FilterConstantBuffer.GetPointerToPointer());

	Context->Draw(3, 0);

	Context->SetRenderTargets(0, NULL, NULL);
	Context->SetTextures(ZEGR_ST_PIXEL, 6, 1, NULL);
	Context->SetConstantBuffers(ZEGR_ST_PIXEL, 9, 1, NULL);
}

bool ZERNStageAO::InitializeSelf()
{
	if (!ZERNStage::InitializeSelf())
		return false;

	CreateSphereSamples();
	CreateRandomVectors();

	ZERNFilter::GenerateGaussianKernel(HorizontalValues, FilterConstants.KernelSize, 2.0f);
	ZERNFilter::GenerateGaussianKernel(VerticalValues, FilterConstants.KernelSize, 2.0f, false);

	SSAOConstantBuffer = ZEGRConstantBuffer::Create(sizeof(SSAOConstants));
	FilterConstantBuffer = ZEGRConstantBuffer::Create(sizeof(SSAOFilterConstants));

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
	SamplerPointClampDescription.AddressU = ZEGR_TAM_BORDER;
	SamplerPointClampDescription.AddressV = ZEGR_TAM_BORDER;
	SamplerPointClampDescription.BorderColor = ZEVector4::Zero;
	SamplerPointClamp = ZEGRSampler::GetSampler(SamplerPointClampDescription);

	return true;
}

void ZERNStageAO::DeinitializeSelf()
{
	DirtyFlags.RaiseAll();

	OcclusionMap.Release();
	BlurTexture.Release();
	RandomVectorsTexture.Release();

	SSAOConstantBuffer.Release();
	FilterConstantBuffer.Release();

	ScreenCoverVertexShaderPosition.Release();
	ScreenCoverVertexShaderPositionTexCoord.Release();

	SSAOPixelShader.Release();
	FilterPixelShader.Release();

	SSAORenderStateData.Release();
	FilterRenderStateData.Release();

	SamplerPointWrap.Release();
	SamplerPointClamp.Release();

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

void ZERNStageAO::SetFilterKernelValues(const ZEVector4* Values, ZESize KernelSize)
{
	if(memcmp(FilterConstants.KernelValues, Values, KernelSize * sizeof(ZEVector4)) == 0)
		return;

	memcpy(FilterConstants.KernelValues, Values, KernelSize * sizeof(ZEVector4));
	FilterConstants.KernelSize = (ZEUInt)KernelSize;

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

void ZERNStageAO::SetSampleCount(ZERNSSAOSampleCount SampleCount)
{
	if(this->SampleCount == SampleCount)
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

	Context->SetSamplers(ZEGR_ST_PIXEL, 1, 1, SamplerPointClamp.GetPointerToPointer());
	Context->SetTextures(ZEGR_ST_PIXEL, 3, 1, reinterpret_cast<const ZEGRTexture**>(&InputNormal));
	Context->SetTextures(ZEGR_ST_PIXEL, 4, 1, reinterpret_cast<const ZEGRTexture**>(&InputDepth));

	GenerateOcclusionMap(Context);
	ApplyBlur(Context);

	return true;
}

void ZERNStageAO::CleanUp(ZEGRContext* Context)
{	
	ZERNStage::CleanUp(Context);
}

ZERNStageAO::ZERNStageAO()
{
	DirtyFlags.RaiseAll();

	Width = 0;
	Height = 0;

	SampleCount = ZERN_AOSC_MEDIUM;

	Constants.SampleCount = ConvertSampleCount(SampleCount);
	Constants.OcclusionRadius = 0.2f;
	Constants.MinDepthBias = 0.15f;
	Constants.Intensity = 64.0f;
	Constants.DownScale = 1.0f;

	FilterConstants.KernelSize = 11;
}
