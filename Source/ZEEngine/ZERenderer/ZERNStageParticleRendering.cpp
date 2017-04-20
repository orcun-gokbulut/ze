//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageParticleRendering.cpp
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

#include "ZERNStageParticleRendering.h"

#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRBuffer.h"
#include "ZEGraphics/ZEGRTexture.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZERNCommand.h"
#include "ZERNStageLighting.h"
#include "ZEParticle/ZEParticleEmitter.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZERNMaterial.h"

#define ZERN_SPDF_OUTPUT		1
#define ZERN_SPDF_SHADERS		2
#define ZERN_SPDF_RENDER_STATES	4

bool ZERNStageParticleRendering::InitializeInternal()
{
	if (!ZERNStage::InitializeInternal())
		return false;

	EmittersConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, 1000 * sizeof(ZERNShading_ParticleEmitter), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);
	ParticlePoolBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_BUFFER, 1000000 * sizeof(ZEVector4), sizeof(ZEVector4), ZEGR_RU_DYNAMIC, ZEGR_RBF_SHADER_RESOURCE, ZEGR_TF_R32G32B32A32_FLOAT);

	return true;
}

bool ZERNStageParticleRendering::DeinitializeInternal()
{
	DirtyFlags.RaiseAll();

	ScreenCoverVertexShader.Release();
	TiledPixelShader.Release();
	TiledCullingComputeShader.Release();
	
	TiledRenderStateData.Release();
	TiledCullingRenderStateData.Release();

	EmittersConstantBuffer.Release();
	ParticlePoolBuffer.Release();
	TileParticleStructuredBuffer.Release();

	AccumulationTexture = NULL;
	DepthTexture = NULL;
	FogConstantBuffer = NULL;
	TileLightStructuredBuffer = NULL;
	LightsConstantBuffer = NULL;

	return ZERNStage::DeinitializeInternal();
}

void ZERNStageParticleRendering::CreateOutput(const ZEString& Name)
{
	ZEUInt Width = GetRenderer()->GetOutputTexture()->GetWidth();
	ZEUInt Height = GetRenderer()->GetOutputTexture()->GetHeight();

	if (Name == "ColorTexture")
	{
		if (DirtyFlags.GetFlags(ZERN_SPDF_OUTPUT) || AccumulationTexture == GetRenderer()->GetOutputTexture())
		{
			AccumulationTexture = ZEGRTexture::CreateResource(ZEGR_TT_2D, Width, Height, 1, ZEGR_TF_R11G11B10_FLOAT, ZEGR_RU_STATIC, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_RENDER_TARGET, 1, ZEGRGraphicsModule::SAMPLE_COUNT).GetPointer();
			DirtyFlags.UnraiseFlags(ZERN_SPDF_OUTPUT);
		}
	}
}

bool ZERNStageParticleRendering::UpdateShaders()
{
	if (!DirtyFlags.GetFlags(ZERN_SPDF_SHADERS))
		return true;

	ZEGRShaderCompileOptions Options;
	Options.Model = ZEGR_SM_5_0;
	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNParticleMaterial.hlsl";
	Options.Definitions.Add(ZEGRShaderDefinition("SAMPLE_COUNT", ZEString::FromInt32(ZEGRGraphicsModule::SAMPLE_COUNT)));

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNScreenCover_VertexShader_Position";
	ScreenCoverVertexShader = ZEGRShader::Compile(Options);
	zeCheckError(ScreenCoverVertexShader == NULL, false, "Cannot set vertex shader.");

	Options.Definitions.Add(ZEGRShaderDefinition("ZERN_SHADING_TRANSPARENT"));
	Options.Definitions.Add(ZEGRShaderDefinition("ZERN_TILED_PARTICLE"));
	Options.Definitions.Add(ZEGRShaderDefinition("ZERN_PM_DIFFUSE_MAP"));

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNParticleMaterial_Tiled_PixelShader";
	TiledPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(TiledPixelShader == NULL, false, "Cannot set pixel shader");

	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNTiledDeferredShadingCompute.hlsl";
	Options.Type = ZEGR_ST_COMPUTE;
	Options.EntryPoint = "ZERNTiledDeferredShadingCompute_ComputeShader_Main";

	TiledCullingComputeShader = ZEGRShader::Compile(Options);
	zeCheckError(TiledCullingComputeShader == NULL, false, "Cannot set compute shader.");

	DirtyFlags.UnraiseFlags(ZERN_SPDF_SHADERS);
	DirtyFlags.RaiseFlags(ZERN_SPDF_RENDER_STATES);
	
	return true;
}

bool ZERNStageParticleRendering::UpdateRenderStates()
{
	if (!DirtyFlags.GetFlags(ZERN_SPDF_RENDER_STATES))
		return true;

	ZEGRBlendState BlendStateAlphaBlended;
	BlendStateAlphaBlended.SetBlendEnable(true);
	ZEGRBlendRenderTarget BlendRenderTargetAlphaBlended = BlendStateAlphaBlended.GetRenderTarget(0);
	BlendRenderTargetAlphaBlended.SetSource(ZEGR_BO_SRC_ALPHA);
	BlendRenderTargetAlphaBlended.SetDestination(ZEGR_BO_INV_SRC_ALPHA);
	BlendRenderTargetAlphaBlended.SetOperation(ZEGR_BE_ADD);
	BlendRenderTargetAlphaBlended.SetBlendEnable(true);
	BlendStateAlphaBlended.SetRenderTargetBlend(0, BlendRenderTargetAlphaBlended);

	ZEGRDepthStencilState DepthStencilStateNoTestWrite;
	DepthStencilStateNoTestWrite.SetDepthTestEnable(false);
	DepthStencilStateNoTestWrite.SetDepthWriteEnable(false);

	ZEGRRenderState RenderState;
	RenderState.SetBlendState(BlendStateAlphaBlended);
	RenderState.SetDepthStencilState(DepthStencilStateNoTestWrite);
	RenderState.SetShader(ZEGR_ST_VERTEX, ScreenCoverVertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, TiledPixelShader);

	TiledRenderStateData = RenderState.Compile();
	zeCheckError(TiledRenderStateData == NULL, false, "Cannot set render state");
	
	ZEGRComputeRenderState ComputeRenderState;
	ComputeRenderState.SetComputeShader(TiledCullingComputeShader);

	TiledCullingRenderStateData = ComputeRenderState.Compile();
	zeCheckError(TiledCullingRenderStateData == NULL, false, "Cannot set render state");

	DirtyFlags.UnraiseFlags(ZERN_SPDF_RENDER_STATES);
	
	return true;
}

bool ZERNStageParticleRendering::UpdateBuffers()
{
	if (GetCommands().GetCount() == 0)
		return false;

	ze_for_each(Command, GetCommands())
	{
		if (Command->GetClass() != ZERNCommandParticleEmitter::Class())
			continue;
		
		const ZEUInt ConstantStride = (sizeof(ZERNShading_ParticleEmitter) + 255) & (~255);
		const ZEUInt ConstantCount = ConstantStride / 16;
		ZEUInt FirstConstant = Emitters.GetSize() / 16;

		ZERNCommandParticleEmitter* CommandParticleEmitter = static_cast<ZERNCommandParticleEmitter*>(Command.GetPointer());
		MaterialInstanced Instance = {CommandParticleEmitter->Material, CommandParticleEmitter->ParticleCount, FirstConstant, ConstantCount};
		
		ZERNShading_ParticleEmitter Emitter;
		Emitter.Axis = ((ZEUInt)CommandParticleEmitter->Axis.x << 16) | ((ZEUInt)CommandParticleEmitter->Axis.y << 8) | ((ZEUInt)CommandParticleEmitter->Axis.z);
		Emitter.Flags = CommandParticleEmitter->Flags;
		Emitter.ParticleTexCoords = CommandParticleEmitter->ParticleTexCoords;
		Emitter.ParticleColor = CommandParticleEmitter->ParticleColor;
		Emitter.ParticleSize = CommandParticleEmitter->ParticleSize;
		Emitter.ParticleRotation = CommandParticleEmitter->ParticleRotation;
		Emitter.ParticleCount = CommandParticleEmitter->ParticleCount;
		Emitter.StartOffset = ParticlePool.GetSize() / sizeof(ZEVector4);

		if (CommandParticleEmitter->Flags.GetFlags(ZE_PEF_TILED_RENDERING))
			TiledInstances.AddByRef(Instance);
		else
			Instances.AddByRef(Instance);

		ZEBYTE* EmitterChunk = Emitters.AddMultiple(ConstantStride);
		memcpy(EmitterChunk, &Emitter, sizeof(Emitter));

		ZESize ParticlePoolStride = (CommandParticleEmitter->DataSize + 65535) & (~65535);
		ZEBYTE* ParticleChunk = ParticlePool.AddMultiple(ParticlePoolStride);
		memcpy(ParticleChunk, CommandParticleEmitter->Data, CommandParticleEmitter->DataSize);
		
		delete [] CommandParticleEmitter->Data;
	}
	
	EmittersConstantBuffer->SetData(Emitters.GetConstCArray(), Emitters.GetSize());
	ParticlePoolBuffer->SetData(ParticlePool.GetConstCArray(), ParticlePool.GetSize());

	return true;
}

bool ZERNStageParticleRendering::Update()
{
	if (!UpdateShaders())
		return false;

	if (!UpdateRenderStates())
		return false;

	if (!UpdateBuffers())
		return false;

	return true;
}

ZEInt ZERNStageParticleRendering::GetId() const
{
	return ZERN_STAGE_PARTICLE_RENDERING;
}

const ZEString& ZERNStageParticleRendering::GetName() const
{
	static const ZEString Name = "Stage particle rendering";
	return Name;
}

void ZERNStageParticleRendering::Resized(ZEUInt Width, ZEUInt Height)
{
	Viewport.SetWidth((float)Width);
	Viewport.SetHeight((float)Height);

	ZEUInt TileCountX = (Width + TILE_PARTICLE_DIMENSION - 1) / TILE_PARTICLE_DIMENSION;
	ZEUInt TileCountY = (Height + TILE_PARTICLE_DIMENSION - 1) / TILE_PARTICLE_DIMENSION;

	ZESize Size = TileCountX * TileCountY * TILE_PARTICLE_TOTAL_COUNT * sizeof(ZEUInt);
	TileParticleStructuredBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_STRUCTURED_BUFFER, Size, sizeof(ZEUInt), ZEGR_RU_STATIC, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_UNORDERED_ACCESS);

	DirtyFlags.RaiseFlags(ZERN_SPDF_OUTPUT);
}

bool ZERNStageParticleRendering::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	if (!Update())
		return false;

	ZERNStageLighting* StageLighting = static_cast<ZERNStageLighting*>(GetRenderer()->GetStage(ZERN_STAGE_LIGHTING));
	if (StageLighting != NULL)
	{
		Context->SetBuffer(ZEGR_ST_PIXEL, 15, TileLightStructuredBuffer);

		const ZEGRBuffer* Buffers[] = {LightsConstantBuffer, LightsConstantBuffer, LightsConstantBuffer, LightsConstantBuffer};
		const ZEUInt Offsets[] = {StageLighting->CommonParamsRange.Offset, StageLighting->PointLightRange.Offset, StageLighting->SpotLightRange.Offset, StageLighting->SpotLightShadowRange.Offset};
		const ZEUInt Counts[] = {StageLighting->CommonParamsRange.Count, StageLighting->PointLightRange.Count, StageLighting->SpotLightRange.Count, StageLighting->SpotLightShadowRange.Count};
	
		Context->SetConstantBuffers(ZEGR_ST_PIXEL, 8, 4, Buffers, Offsets, Counts);
	}
	
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 12, FogConstantBuffer);
	Context->SetBuffer(ZEGR_ST_PIXEL, 17, ParticlePoolBuffer);
	Context->SetTexture(ZEGR_ST_PIXEL, 4, DepthTexture);

	const ZEGRRenderTarget* RenderTarget = AccumulationTexture->GetRenderTarget();
	Context->SetRenderTargets(1, &RenderTarget, DepthTexture->GetDepthStencilBuffer(true));
	Context->SetViewports(1, &Viewport);

	if (Instances.GetCount() > 0)
	{
		ze_for_each(Instance, Instances)
		{
			if (!Instance->Material->SetupMaterial(Context, this))
				continue;

			Context->SetBuffer(ZEGR_ST_VERTEX, 17, ParticlePoolBuffer);
			Context->SetConstantBuffer(ZEGR_ST_VERTEX, 13, EmittersConstantBuffer, &Instance->FirstConstant, &Instance->ConstantCount);
			Context->SetConstantBuffer(ZEGR_ST_PIXEL, 13, EmittersConstantBuffer, &Instance->FirstConstant, &Instance->ConstantCount);
			Context->DrawInstanced(4, 0, Instance->InstanceCount, 0);

			Instance->Material->CleanupMaterial(Context, this);
		}
	}

	if (TiledInstances.GetCount() > 0)
	{
		Context->SetConstantBuffer(ZEGR_ST_COMPUTE, 13, EmittersConstantBuffer);
		Context->SetBuffer(ZEGR_ST_COMPUTE, 17, ParticlePoolBuffer);
		Context->SetTexture(ZEGR_ST_COMPUTE, 4, DepthTexture);
		Context->SetRWBuffer(1, TileParticleStructuredBuffer);

		Context->SetComputeRenderState(TiledCullingRenderStateData);
		ZEUInt TileCountX = (AccumulationTexture->GetWidth() + TILE_PARTICLE_DIMENSION - 1) / TILE_PARTICLE_DIMENSION;
		ZEUInt TileCountY = (AccumulationTexture->GetHeight() + TILE_PARTICLE_DIMENSION - 1) / TILE_PARTICLE_DIMENSION;
		Context->Dispatch(TileCountX, TileCountY, 1);

		Context->SetBuffer(ZEGR_ST_PIXEL, 16, TileParticleStructuredBuffer);
		
		ze_for_each(Instance, TiledInstances)
		{
			if (!Instance->Material->SetupMaterial(Context, this))
				continue;
			
			Context->SetConstantBuffer(ZEGR_ST_PIXEL, 13, EmittersConstantBuffer, &Instance->FirstConstant, &Instance->ConstantCount);
			Context->SetRenderState(TiledRenderStateData);
			Context->Draw(3, 0);

			Instance->Material->CleanupMaterial(Context, this);
		}
	}

	return false;
}

void ZERNStageParticleRendering::CleanUp(ZEGRContext* Context)
{
	Instances.Clear();
	TiledInstances.Clear();
	Emitters.Clear();
	ParticlePool.Clear();

	ZERNStage::CleanUp(Context);
}

ZERNStageParticleRendering::ZERNStageParticleRendering()
{
	DirtyFlags.RaiseAll();

	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&DepthTexture), "DepthTexture", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV);
	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&FogConstantBuffer), "FogConstantBuffer", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV | ZERN_SRCF_REQUIRED);
	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&TileLightStructuredBuffer), "TileLightBuffer", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV | ZERN_SRCF_REQUIRED);
	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&LightsConstantBuffer), "LightsConstantBuffer", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV | ZERN_SRCF_REQUIRED);

	AddOutputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&AccumulationTexture), "ColorTexture", ZERN_SRUT_WRITE, ZERN_SRCF_GET_FROM_PREV | ZERN_SRCF_CREATE_OWN | ZERN_SRCF_GET_OUTPUT);
}

ZERNStageParticleRendering::~ZERNStageParticleRendering()
{
	Deinitialize();
}

ZEGRRenderState ZERNStageParticleRendering::GetRenderState()
{
	static ZEGRRenderState RenderState;
	static bool Initialized = false;

	if(!Initialized)
	{
		Initialized = true;

		ZEGRBlendState BlendStateAlphaBlended;
		BlendStateAlphaBlended.SetBlendEnable(true);
		ZEGRBlendRenderTarget BlendRenderTargetAlphaBlended = BlendStateAlphaBlended.GetRenderTarget(0);
		BlendRenderTargetAlphaBlended.SetSource(ZEGR_BO_SRC_ALPHA);
		BlendRenderTargetAlphaBlended.SetDestination(ZEGR_BO_INV_SRC_ALPHA);
		BlendRenderTargetAlphaBlended.SetOperation(ZEGR_BE_ADD);
		BlendRenderTargetAlphaBlended.SetBlendEnable(true);
		BlendStateAlphaBlended.SetRenderTargetBlend(0, BlendRenderTargetAlphaBlended);
		RenderState.SetBlendState(BlendStateAlphaBlended);

		RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_STRIPT);
	}

	return RenderState;
}
