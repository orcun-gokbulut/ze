//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERenderStageForward.cpp
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

#include "ZEMaterial.h"
#include "ZERenderCommand.h"
#include "ZERenderStageForward.h"
#include "ZERenderStageGeometry.h"
#include "ZERenderStageLighting.h"
#include "ZEGraphics/ZETexture2D.h"
#include "ZEGraphics/ZERenderTarget.h"
#include "ZEGraphics/ZEGraphicsDevice.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZEGraphicsEventTracer.h"

void ZERenderStageForward::ResetStageDefaults()
{
	// Requested pipeline constants
	StageConstants.SetCount(2);
	StageConstants.Add(ZE_PC_CAMERA_POS);
	StageConstants.Add(ZE_PC_INV_VIEWPORT_WIDTH_HEIGHT);

	// Reset parent states
	ZERenderStage::ResetStageDefaults();

	// Outputs
	DefaultStates.ScreenWriteEnable = true;
	//DefaultStates.RenderTargets[0] = RenderTargets.ABuffer;
	DefaultStates.RenderTargets[0] = zeGraphics->GetFrameBuffer();
	DefaultStates.DepthStencilBuffer = zeGraphics->GetDepthBuffer();

	// Use default viewport and scissor rectangles
	ZESize ScreenCount = zeGraphics->GetScreenCount();
	for (ZESize I = 0; I < ScreenCount; ++I)
	{
		DefaultStates.ViewPorts[I] = zeGraphics->GetViewport(I);
		DefaultStates.ScissorRectangles[I] = zeGraphics->GetScissorRectangle(I);
	}

	// Blend state
	DefaultStates.BlendState.SetBlendEnable(0, false);
	DefaultStates.BlendState.SetColorChannelMask(0, ZE_CCM_ALL);

	// Point sampler state
	DefaultStates.PixelShaderSamplers[0].SetAddressU(ZE_TAM_CLAMP);
	DefaultStates.PixelShaderSamplers[0].SetAddressV(ZE_TAM_CLAMP);
	DefaultStates.PixelShaderSamplers[0].SetMagFilter(ZE_TFM_POINT);
	DefaultStates.PixelShaderSamplers[0].SetMinFilter(ZE_TFM_POINT);
	DefaultStates.PixelShaderSamplers[0].SetMipFilter(ZE_TFM_POINT);
	
	// Depth stencil state
	DefaultStates.DepthStencilState.SetZFunction(ZE_CF_LESS_EQUAL);
	DefaultStates.DepthStencilState.SetZTestEnable(true);
	DefaultStates.DepthStencilState.SetZWriteEnable(false);

	// Rasterizer state
	DefaultStates.RasterizerState.SetCullDirection(ZE_CD_COUNTER_CLOCKWISE);
	DefaultStates.RasterizerState.SetFillMode(ZE_FM_SOLID);

	if (GBufferInput == NULL || LightBufferInput == NULL)
	{
		zeError("Missing input stage.");
		return;
	}

	// Textures
	DefaultStates.PixelShaderTextures[0] = GBufferInput->GetGBuffer1();
	DefaultStates.PixelShaderTextures[1] = GBufferInput->GetGBuffer2();
	DefaultStates.PixelShaderTextures[2] = GBufferInput->GetGBuffer3();
	DefaultStates.PixelShaderTextures[3] = LightBufferInput->GetLBuffer1();
	DefaultStates.PixelShaderTextures[4] = LightBufferInput->GetLBuffer2();
}

void ZERenderStageForward::CommitStageDefaults()
{
// 	static ZEUInt PrevFrameId = 0;
// 	static ZEUInt CommitCount = 0;
// 
// 	ZEUInt CurFrameId = zeCore->GetFrameId();
// 
// 	if (CurFrameId > PrevFrameId)
// 	{
// 		CommitCount = 0;
// 		PrevFrameId = CurFrameId;
// 	}
// 	
// 	CommitCount++;

	// Commit parent States
	ZERenderStage::CommitStageDefaults();

	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	Device->SetRenderTargetScreen(DefaultStates.RenderTargets[0]);
	Device->SetDepthStencilBuffer(DefaultStates.DepthStencilBuffer);

	ZESize ScreenCount = zeGraphics->GetScreenCount();
	for (ZESize I = 0; I < ScreenCount; ++I)
	{
		Device->SetViewport(I, DefaultStates.ViewPorts[I]);
		Device->SetScissorRectangle(I, DefaultStates.ScissorRectangles[I]);
	}

	Device->SetPixelShaderTexture(0, DefaultStates.PixelShaderTextures[0]);
	Device->SetPixelShaderTexture(1, DefaultStates.PixelShaderTextures[1]);
	Device->SetPixelShaderTexture(2, DefaultStates.PixelShaderTextures[2]);
	Device->SetPixelShaderTexture(3, DefaultStates.PixelShaderTextures[3]);
	Device->SetPixelShaderTexture(4, DefaultStates.PixelShaderTextures[4]);

	Device->SetPixelShaderSampler(0, DefaultStates.PixelShaderSamplers[0]);

	Device->SetBlendState(DefaultStates.BlendState);
	Device->SetRasterizerState(DefaultStates.RasterizerState);
	Device->SetDepthStencilState(DefaultStates.DepthStencilState);
}

void ZERenderStageForward::UpdateBuffers()
{
	// Update parent buffers
	ZERenderStage::UpdateBuffers();

	ZEUInt Width = zeGraphics->GetScreenWidth();
	ZEUInt Height = zeGraphics->GetScreenHeight();

	if (Textures.ABuffer == NULL || Textures.ABuffer->GetWidth() != Width || Textures.ABuffer->GetHeight() != Height)
	{
		ZE_DESTROY(RenderTargets.ABuffer);
		ZE_DESTROY(Textures.ABuffer);

		Textures.ABuffer = ZETexture2D::CreateInstance();
		Textures.ABuffer->CreateStatic(Width, Height, 1, ZE_TPF_F16_4, true, NULL);
		RenderTargets.ABuffer = Textures.ABuffer->CreateRenderTarget(0);
	}
}

void ZERenderStageForward::DestroyBuffers()
{
	ZE_DESTROY(RenderTargets.ABuffer);
	ZE_DESTROY(Textures.ABuffer);
}

const ZETexture2D* ZERenderStageForward::GetABuffer() const
{
	return Textures.ABuffer;
}

void ZERenderStageForward::SetGBufferInput(const ZERenderStageGeometry* Input)
{
	zeDebugCheck(Input == NULL, "Null pointer");

	GBufferInput = Input;
}

const ZERenderStageGeometry* ZERenderStageForward::GetGBufferInput() const
{
	return GBufferInput;
}

void ZERenderStageForward::SetLBufferInput(const ZERenderStageLighting* Input)
{
	zeDebugCheck(Input == NULL, "Null pointer");

	LightBufferInput = Input;
}

const ZERenderStageLighting* ZERenderStageForward::GetLBufferInput() const
{
	return LightBufferInput;
}

void ZERenderStageForward::Setup()
{
	UpdateBuffers();
	ResetStageDefaults();
	CommitStageDefaults();

	LastMaterial = -1;

	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	Device->ClearRenderTarget(RenderTargets.ABuffer, ZEVector4(0.0f, 1.0f, 0.0f, 1.0f));
	Device->ClearRenderTarget(zeGraphics->GetFrameBuffer(), ZEVector4(1.0f, 1.0f, 0.0f, 1.0f));
}

void ZERenderStageForward::Process(ZERenderCommand* RenderCommand)
{
	ZEMaterial* Material = RenderCommand->Material;

	zeDebugCheck(RenderCommand->Material == NULL, "Cannot process null material");

	if (!(Material->GetMaterialFlags() & ZE_MTF_FORWARD_PASS))
	{
		return;
	}

// 	ZEUInt32 Hash = Material->GetHash();
// 	if (LastMaterial != Hash)
// 	{
// 		// Return to defaults if material is changed
// 		CommitStageDefaults();
// 		LastMaterial = Hash;
// 	}

	zeGraphics->GetEventTracer()->StartEvent("Object Pass");

	bool Done = false;
	ZEUInt PassId = 0;
	while (!Done)
	{
		Done = RenderCommand->Material->SetupPass(PassId++, this, RenderCommand);
	}

	PumpStreams(RenderCommand);

	zeGraphics->GetEventTracer()->EndEvent();
}

ZEUInt32 ZERenderStageForward::GetStageFlags() const
{
	return ZE_RENDER_STAGE_FORWARD;
}

ZERenderStageForward::ZERenderStageForward()
{	
	LastMaterial = -1;

	Textures.ABuffer = NULL;
	RenderTargets.ABuffer = NULL;

	GBufferInput = NULL;
	LightBufferInput = NULL;
}

ZERenderStageForward::~ZERenderStageForward()
{
	DestroyBuffers();
}
