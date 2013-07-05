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
#include "ZEGraphics/ZETexture2D.h"
#include "ZERenderStageAccumulation.h"
#include "ZEGraphics/ZERenderTarget.h"
#include "ZEGraphics/ZEGraphicsDevice.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZEShaderMetaTable.h"
#include "ZEGraphics/ZEGraphicsEventTracer.h"

void ZERenderStageParticle::ResetStates()
{
	// Reset parent states
	ZERenderStage::ResetStates();
	
	// Outputs
	DefaultStates.RenderTargets[0] = ABufferInput->GetOutputAccumulationRenderTarget();
	DefaultStates.DepthStencilBuffer = zeGraphics->GetDepthBuffer();

	// Use default viewport and scissor rectangles
	ZESize ScreenCount = zeGraphics->GetScreenCount();
	for (ZESize I = 0; I < ScreenCount; ++I)
	{
		DefaultStates.ViewPorts[I] = zeGraphics->GetViewport(I);
		DefaultStates.ScissorRects[I] = zeGraphics->GetScissorRectangle(I);
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

void ZERenderStageParticle::CommitStates()
{
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
		Device->SetScissorRectangle(I, DefaultStates.ScissorRects[I]);
	}

	// Commit parent States
	ZERenderStage::CommitStates();
}

void ZERenderStageParticle::UpdateBuffers()
{

}

void ZERenderStageParticle::DestroyBuffers()
{

}

ZERenderStageType ZERenderStageParticle::GetDependencies() const
{
	return ZE_RST_ACCUMULATION;
}

ZERenderStageType ZERenderStageParticle::GetStageType() const
{
	return ZE_RST_PARTICLE;
}

void ZERenderStageParticle::SetInputAccumulationStage(const ZERenderStageAccumulation* Input)
{
	zeDebugCheck(Input == NULL, "Null pointer.");

	ABufferInput = Input;
}

const ZERenderStageAccumulation* ZERenderStageParticle::GetInputAccumulationStage() const
{
	return ABufferInput;
}
		
void ZERenderStageParticle::Setup()
{
	UpdateBuffers();
	ResetStates();
	CommitStates();
}

void ZERenderStageParticle::Process(const ZERenderCommand* RenderCommand)
{
	zeDebugCheck(RenderCommand->Material == NULL, "Null Pointer.");

	bool Done = false;
	ZEUInt PassId = 0;
	while (!Done)
	{
		Done = RenderCommand->Material->SetupPass(PassId++, this, RenderCommand);
	}

	PumpStreams(RenderCommand);
}

ZERenderStageParticle::ZERenderStageParticle()
{
	ABufferInput = NULL;
	LastMaterial = 0;
}

ZERenderStageParticle::~ZERenderStageParticle()
{

}
