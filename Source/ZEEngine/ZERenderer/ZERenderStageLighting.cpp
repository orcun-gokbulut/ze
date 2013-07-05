//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERenderStageLighting.cpp
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
#include "ZERenderStageGeometry.h"
#include "ZERenderStageLighting.h"
#include "ZEGraphics/ZETexture2D.h"
#include "ZEGraphics/ZERenderTarget.h"
#include "ZEGraphics/ZEGraphicsDevice.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZEShaderMetaTable.h"
#include "ZEGraphics/ZEGraphicsEventTracer.h"


void ZERenderStageLighting::DestroyBuffers()
{
	ZE_DESTROY(RenderTargets.LBuffer1);
	ZE_DESTROY(RenderTargets.LBuffer2);
	ZE_DESTROY(Textures.LBuffer1);
	ZE_DESTROY(Textures.LBuffer2);
}

void ZERenderStageLighting::UpdateBuffers()
{
	ZEUInt Width = zeGraphics->GetScreenWidth();
	ZEUInt Height = zeGraphics->GetScreenHeight();

	if (Textures.LBuffer1 == NULL || Textures.LBuffer1->GetWidth() != Width || Textures.LBuffer1->GetHeight() != Height)
	{
		ZE_DESTROY(RenderTargets.LBuffer1);
		ZE_DESTROY(Textures.LBuffer1);

		Textures.LBuffer1 = ZETexture2D::CreateInstance();
		Textures.LBuffer1->CreateStatic(Width, Height, 1, ZE_TPF_F16_4, true, NULL);
		RenderTargets.LBuffer1 = Textures.LBuffer1->CreateRenderTarget(0);
	}

	if (Textures.LBuffer2 == NULL || Textures.LBuffer2->GetWidth() != Width || Textures.LBuffer2->GetHeight() != Height)
	{
		ZE_DESTROY(RenderTargets.LBuffer2);
		ZE_DESTROY(Textures.LBuffer2);

		Textures.LBuffer2 = ZETexture2D::CreateInstance();
		Textures.LBuffer2->CreateStatic(Width, Height, 1, ZE_TPF_I8_4, true, NULL);
		RenderTargets.LBuffer2 = Textures.LBuffer2->CreateRenderTarget(0);
	}
}

void ZERenderStageLighting::ResetStates()
{
	// Reset parent states
	ZERenderStage::ResetStates();

	// Vertex buffer
	// DefaultStates.VertexBuffers[0] = VertexBuffer;

	// Render targets
	DefaultStates.RenderTargets[0] = RenderTargets.LBuffer1;
	DefaultStates.RenderTargets[1] = RenderTargets.LBuffer2;
	DefaultStates.DepthStencilBuffer = zeGraphics->GetDepthBuffer();

	// Use default viewport and scissor rectangles
	ZESize ScreenCount = zeGraphics->GetScreenCount();
	for (ZESize I = 0; I < ScreenCount; ++I)
	{
		DefaultStates.ViewPorts[I] = zeGraphics->GetViewport(I);
		DefaultStates.ScissorRects[I] = zeGraphics->GetScissorRectangle(I);
	}

	// Rasterizer state
	DefaultStates.RasterizerState.SetCullDirection(ZE_CD_CLOCKWISE);
	DefaultStates.RasterizerState.SetFillMode(ZE_FM_SOLID);

	// Sampler
	DefaultStates.PixelShaderSamplers[3].SetAddressU(ZE_TAM_CLAMP);
	DefaultStates.PixelShaderSamplers[3].SetAddressV(ZE_TAM_CLAMP);
	DefaultStates.PixelShaderSamplers[3].SetMinFilter(ZE_TFM_POINT);
	DefaultStates.PixelShaderSamplers[3].SetMagFilter(ZE_TFM_POINT);
	DefaultStates.PixelShaderSamplers[3].SetMipFilter(ZE_TFM_POINT);

	// Blend state
	DefaultStates.BlendState.SetBlendEnable(0, true);
	DefaultStates.BlendState.SetBlendEnable(1, true);
	DefaultStates.BlendState.SetComponentWriteMask(0, ZE_CM_ALL);
	DefaultStates.BlendState.SetComponentWriteMask(1, ZE_CM_ALL);
	DefaultStates.BlendState.SetBlendEquation(ZE_BE_ADD);
	DefaultStates.BlendState.SetSourceBlendOption(ZE_BO_ONE);
	DefaultStates.BlendState.SetDestinationBlendOption(ZE_BO_ONE);
	DefaultStates.BlendState.SetSourceBlendAlphaOption(ZE_BO_ONE);
	DefaultStates.BlendState.SetDestinationBlendAlphaOption(ZE_BO_ONE);

	// Depth stencil state
	DefaultStates.DepthStencilState.SetZTestEnable(true);
	DefaultStates.DepthStencilState.SetZFunction(ZE_CF_GREATER);
	DefaultStates.DepthStencilState.SetZWriteEnable(false);
	DefaultStates.DepthStencilState.SetStencilTestEnable(false);

	// Point Sampler State
	DefaultStates.PixelShaderSamplers[0].SetAddressU(ZE_TAM_CLAMP);
	DefaultStates.PixelShaderSamplers[0].SetAddressV(ZE_TAM_CLAMP);
	DefaultStates.PixelShaderSamplers[0].SetMagFilter(ZE_TFM_POINT);
	DefaultStates.PixelShaderSamplers[0].SetMinFilter(ZE_TFM_POINT);
	DefaultStates.PixelShaderSamplers[0].SetMipFilter(ZE_TFM_POINT);

	if (GBufferInput == NULL)
	{
		zeWarning("Input stage not set");
		return;
	}

	// Textures
	DefaultStates.PixelShaderTextures[0] = GBufferInput->GetGBuffer1();
	DefaultStates.PixelShaderTextures[1] = GBufferInput->GetGBuffer2();
	DefaultStates.PixelShaderTextures[2] = GBufferInput->GetGBuffer3();
}

void ZERenderStageLighting::CommitStates()
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

	Device->SetPixelShaderSampler(0, DefaultStates.PixelShaderSamplers[0]);
	Device->SetPixelShaderTexture(0, DefaultStates.PixelShaderTextures[0]);
	Device->SetPixelShaderTexture(1, DefaultStates.PixelShaderTextures[1]);
	Device->SetPixelShaderTexture(2, DefaultStates.PixelShaderTextures[2]);

	// Commit parent states
	ZERenderStage::CommitStates();
}

const ZETexture2D* ZERenderStageLighting::GetLBuffer1() const
{
	return Textures.LBuffer1;
}

const ZETexture2D* ZERenderStageLighting::GetLBuffer2() const
{
	return Textures.LBuffer2;
}
		
void ZERenderStageLighting::SetInputGeometryStage(const ZERenderStageGeometry* Stage)
{
	zeDebugCheck(Stage == NULL, "Null pointer.");
	
	GBufferInput = Stage;
}

const ZERenderStageGeometry* ZERenderStageLighting::GetInputGeometryStage() const
{
	return GBufferInput;
}

void ZERenderStageLighting::Setup()
{
	UpdateBuffers();
	ResetStates();
	CommitStates();

	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	Device->ClearRenderTarget(RenderTargets.LBuffer1, ZEVector4::Zero);
	Device->ClearRenderTarget(RenderTargets.LBuffer2, ZEVector4::Zero);
}

void ZERenderStageLighting::Process(const ZERenderCommand* RenderCommand)
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

ZERenderStageType ZERenderStageLighting::GetDependencies() const
{
	return ZE_RST_SHADOW | ZE_RST_GEOMETRY;
}

ZERenderStageType ZERenderStageLighting::GetStageType() const
{
	return ZE_RST_LIGHTING;
}

ZERenderStageLighting::ZERenderStageLighting()
{
	GBufferInput = NULL;
	
	Textures.LBuffer1 = NULL;
	Textures.LBuffer2 = NULL;
	RenderTargets.LBuffer1 = NULL;
	RenderTargets.LBuffer2 = NULL;
}

ZERenderStageLighting::~ZERenderStageLighting()
{
	DestroyBuffers();
}
