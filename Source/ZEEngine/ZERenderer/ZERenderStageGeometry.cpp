//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERenderStageGeometry.cpp
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
#include "ZERenderStageGeometry.h"
#include "ZEGraphics/ZETexture2D.h"
#include "ZEGraphics/ZERenderTarget.h"
#include "ZEGraphics/ZEGraphicsDevice.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZEGraphicsEventTracer.h"

void ZERenderStageGeometry::ResetStates()
{
	// Reset parent states
	ZERenderStage::ResetStates();

	// Render targets
	DefaultStates.RenderTargets[0] = RenderTargets.GBuffer1;
	DefaultStates.RenderTargets[1] = RenderTargets.GBuffer2;
	DefaultStates.RenderTargets[2] = RenderTargets.GBuffer3;
	DefaultStates.DepthStencilBuffer = zeGraphics->GetDepthBuffer();

	// Use default viewport and scissor rectangles
	ZESize ScreenCount = zeGraphics->GetScreenCount();
	for (ZESize I = 0; I < ScreenCount; ++I)
	{
		DefaultStates.ViewPorts[I] = zeGraphics->GetViewport(I);
		DefaultStates.ScissorRects[I] = zeGraphics->GetScissorRectangle(I);
	}

	// Blend state
	DefaultStates.BlendState.SetComponentWriteMask(0, ZE_CM_RED);   // Output to RT0 is float
	DefaultStates.BlendState.SetComponentWriteMask(1, ZE_CM_ALL);   // Output to RT1 is float4
	DefaultStates.BlendState.SetComponentWriteMask(2, ZE_CM_ALL);   // Output to RT2 is float4

	// Depth Stencil state
	DefaultStates.DepthStencilState.SetStencilTestEnable(false);
	DefaultStates.DepthStencilState.SetZFunction(ZE_CF_LESS_EQUAL);
	DefaultStates.DepthStencilState.SetZTestEnable(true);
	DefaultStates.DepthStencilState.SetZWriteEnable(true);

	// Rasterizer state
	DefaultStates.RasterizerState.SetCullDirection(ZE_CD_COUNTER_CLOCKWISE);
	DefaultStates.RasterizerState.SetFillMode(ZE_FM_SOLID);
}

void ZERenderStageGeometry::CommitStates()
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

void ZERenderStageGeometry::UpdateBuffers()
{
	ZEUInt Width = zeGraphics->GetScreenWidth();
	ZEUInt Height = zeGraphics->GetScreenHeight();

	if (Textures.GBuffer1 == NULL || Textures.GBuffer1->GetWidth() != Width || Textures.GBuffer1->GetHeight() != Height)
	{
		ZE_DESTROY(RenderTargets.GBuffer1);
		ZE_DESTROY(Textures.GBuffer1);

		Textures.GBuffer1 = ZETexture2D::CreateInstance();
		Textures.GBuffer1->CreateStatic(Width, Height, 1, ZE_TPF_F32, true, NULL);
		RenderTargets.GBuffer1 = Textures.GBuffer1->CreateRenderTarget(0);
	}
	
	if (Textures.GBuffer2 == NULL || Textures.GBuffer2->GetWidth() != Width || Textures.GBuffer2->GetHeight() != Height)
	{
		ZE_DESTROY(RenderTargets.GBuffer2);
		ZE_DESTROY(Textures.GBuffer2);

		Textures.GBuffer2 = ZETexture2D::CreateInstance();
		Textures.GBuffer2->CreateStatic(Width, Height, 1, ZE_TPF_F16_4, true, NULL);
		RenderTargets.GBuffer2 = Textures.GBuffer2->CreateRenderTarget(0);
	}

	if (Textures.GBuffer3 == NULL || Textures.GBuffer3->GetWidth() != Width || Textures.GBuffer3->GetHeight() != Height)
	{
		ZE_DESTROY(RenderTargets.GBuffer3);
		ZE_DESTROY(Textures.GBuffer3);

		Textures.GBuffer3 = ZETexture2D::CreateInstance();
		Textures.GBuffer3->CreateStatic(Width, Height, 1, ZE_TPF_I8_4, true, NULL);
		RenderTargets.GBuffer3 = Textures.GBuffer3->CreateRenderTarget(0);
	}
}

void ZERenderStageGeometry::DestroyBuffers()
{
	ZE_DESTROY(RenderTargets.GBuffer1);
	ZE_DESTROY(Textures.GBuffer1);
	
	ZE_DESTROY(RenderTargets.GBuffer2);
	ZE_DESTROY(Textures.GBuffer2);

	ZE_DESTROY(RenderTargets.GBuffer3);
	ZE_DESTROY(Textures.GBuffer3);
}

const ZETexture2D* ZERenderStageGeometry::GetGBuffer1() const
{
	return Textures.GBuffer1;
}

const ZETexture2D* ZERenderStageGeometry::GetGBuffer2() const
{
	return Textures.GBuffer2;
}

const ZETexture2D* ZERenderStageGeometry::GetGBuffer3() const
{
	return Textures.GBuffer3;
}

void ZERenderStageGeometry::Setup()
{
	UpdateBuffers();
	ResetStates();
	CommitStates();

	LastMaterial = -1;

	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	// Clear targets
	Device->ClearRenderTarget(RenderTargets.GBuffer1, ZEVector4::Zero);
	Device->ClearRenderTarget(RenderTargets.GBuffer2, ZEVector4::Zero);
	Device->ClearRenderTarget(RenderTargets.GBuffer3, ZEVector4::Zero);
	Device->ClearDepthStencilBuffer(zeGraphics->GetDepthBuffer(), true, true, 1.0f, 0);
}

void ZERenderStageGeometry::Process(const ZERenderCommand* RenderCommand)
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

ZERenderStageType ZERenderStageGeometry::GetDependencies() const
{
	return ZE_RST_NONE;
}

ZERenderStageType ZERenderStageGeometry::GetStageType() const
{
	return ZE_RST_GEOMETRY;
}

ZERenderStageGeometry::ZERenderStageGeometry()
{	
	LastMaterial = -1;

	Textures.GBuffer1 = NULL;
	RenderTargets.GBuffer1 = NULL;

	Textures.GBuffer2 = NULL;
	RenderTargets.GBuffer2 = NULL;

	Textures.GBuffer3 = NULL;
	RenderTargets.GBuffer3 = NULL;
}

ZERenderStageGeometry::~ZERenderStageGeometry()
{
	DestroyBuffers();
}
