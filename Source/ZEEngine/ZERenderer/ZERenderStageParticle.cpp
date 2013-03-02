//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERenderStageParticle.cpp
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

#include "ZECamera.h"
#include "ZERenderer.h"
#include "ZEMaterial.h"
#include "ZEGame/ZEScene.h"
#include "ZEMath/ZEAngle.h"
#include "ZERenderCommand.h"
#include "ZEGraphics/ZEShader.h"
#include "ZERenderStageParticle.h"
#include "ZERenderStageForward.h"
#include "ZEGraphics/ZETexture2D.h"
#include "ZEGraphics/ZERenderTarget.h"
#include "ZEGraphics/ZEGraphicsDevice.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZEShaderMetaTable.h"
#include "ZEGraphics/ZEGraphicsEventTracer.h"

void ZERenderStageParticle::ResetStageDefaults()
{
	// Requested pipeline constants
	StageConstants.SetCount(5);
	StageConstants.Add(ZE_PC_CAMERA_WORLD_UP);
	StageConstants.Add(ZE_PC_CAMERA_WORLD_FRONT);
	StageConstants.Add(ZE_PC_CAMERA_WORLD_RIGHT);
	StageConstants.Add(ZE_PC_CAMERA_WORLD_POS);
	StageConstants.Add(ZE_PC_INV_VIEWPORT_WIDTH_HEIGHT);

	// Reset parent states
	ZERenderStage::ResetStageDefaults();
	
	// Outputs
	DefaultStates.RenderTargets[0] = ABufferInput->GetOutputAccumulationRenderTarget();
	DefaultStates.DepthStencilBuffer = zeGraphics->GetDepthBuffer();

	// Use default viewport and scissor rectangles
	ZESize ScreenCount = zeGraphics->GetScreenCount();
	for (ZESize I = 0; I < ScreenCount; ++I)
	{
		DefaultStates.ViewPorts[I] = zeGraphics->GetViewport(I);
		DefaultStates.ScissorRectangles[I] = zeGraphics->GetScissorRectangle(I);
	}

	// Blend state: Use default

	// Point sampler state
	DefaultStates.PixelShaderSamplers[0].SetAddressU(ZE_TAM_CLAMP);
	DefaultStates.PixelShaderSamplers[0].SetAddressV(ZE_TAM_CLAMP);
	DefaultStates.PixelShaderSamplers[0].SetMagFilter(ZE_TFM_POINT);
	DefaultStates.PixelShaderSamplers[0].SetMinFilter(ZE_TFM_POINT);
	DefaultStates.PixelShaderSamplers[0].SetMipFilter(ZE_TFM_POINT);
	
	// Depth stencil state
	DefaultStates.DepthStencilState.SetZFunction(ZE_CF_GREATER_EQUAL);
	DefaultStates.DepthStencilState.SetZTestEnable(true);
	DefaultStates.DepthStencilState.SetZWriteEnable(false);

	// Rasterizer state
	DefaultStates.RasterizerState.SetCullDirection(ZE_CD_COUNTER_CLOCKWISE);
	DefaultStates.RasterizerState.SetFillMode(ZE_FM_SOLID);

	if (ABufferInput == NULL)
	{
		zeError("Missing input stage.");
		return;
	}
}

void ZERenderStageParticle::CommitStageDefaults()
{
	// Commit parent States
	ZERenderStage::CommitStageDefaults();

	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	Device->SetRenderTargetArray(DefaultStates.RenderTargets);
	Device->SetDepthStencilBuffer(DefaultStates.DepthStencilBuffer);

	Device->SetBlendState(DefaultStates.BlendState);
	Device->SetDepthStencilState(DefaultStates.DepthStencilState);

	Device->SetRasterizerState(DefaultStates.RasterizerState);

	ZESize ScreenCount = zeGraphics->GetScreenCount();
	for (ZESize I = 0; I < ScreenCount; ++I)
	{
		Device->SetViewport(I, DefaultStates.ViewPorts[I]);
		Device->SetScissorRectangle(I, DefaultStates.ScissorRectangles[I]);
	}
}

void ZERenderStageParticle::UpdateBuffers()
{
	ZERenderStage::UpdateBuffers();
}

void ZERenderStageParticle::DestroyBuffers()
{

}

ZEUInt32 ZERenderStageParticle::GetStageFlags() const
{
	return 0;
}

ZEUInt32 ZERenderStageParticle::GetDependencies() const
{
	return ZE_RENDER_STAGE_FORWARD;
}

ZEUInt32 ZERenderStageParticle::GetStageIndentifier() const
{
	return ZE_RENDER_STAGE_PARTICLE;
}

void ZERenderStageParticle::SetInputAccumulationStage(const ZERenderStageForward* Input)
{
	zeDebugCheck(Input == NULL, "Null pointer.");

	ABufferInput = Input;
}

const ZERenderStageForward* ZERenderStageParticle::GetInputAccumulationStage() const
{
	return ABufferInput;
}
		
void ZERenderStageParticle::Setup()
{
	ResetStageDefaults();
	CommitStageDefaults();
}

void ZERenderStageParticle::Process(ZERenderCommand* RenderCommand)
{
	ZEMaterial* Material = RenderCommand->Material;

	zeDebugCheck(RenderCommand->Material == NULL, "Cannot process null material");

	if (!(Material->GetMaterialFlags() & ZE_MTF_PARTICLE_PASS))
	{
		return;
	}

	zeGraphics->GetEventTracer()->StartEvent("Particle Pass");

	bool Done = false;
	ZEUInt PassId = 0;
	while (!Done)
	{
		Done = RenderCommand->Material->SetupPass(PassId++, this, RenderCommand);
	}

	PumpStreams(RenderCommand);

	zeGraphics->GetEventTracer()->EndEvent();
}

void ZERenderStageParticle::SetStageConfiguration(const ZERenderStageConfiguration* Config)
{
	
}

ZERenderStageParticle::ZERenderStageParticle()
{
	ABufferInput = NULL;
	LastMaterial = 0;
}

ZERenderStageParticle::~ZERenderStageParticle()
{

}
