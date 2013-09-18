//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERenderStageShadow.cpp
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

#include "ZELight.h"
#include "ZERenderer.h"
#include "ZEMaterial.h"
#include "ZEGame/ZEScene.h"
#include "ZERenderCommand.h"
#include "ZEGraphics/ZEShader.h"
#include "ZERenderStageShadow.h"
#include "ZEGraphics/ZEGraphicsDevice.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZEGraphicsWindow.h"
#include "ZEGraphics/ZEShaderCompiler.h"
#include "ZEGraphics/ZEDepthStencilBuffer.h"
#include "ZEGraphics/ZEGraphicsDeviceState.h"
#include "ZEGraphics/ZEGraphicsEventTracer.h"

ZERenderStageShadowData::ZERenderStageShadowData()
{
	RenderTarget = NULL;
	DepthBuffer = NULL;
}

ZERenderStageShadowData::~ZERenderStageShadowData()
{
	
}

bool ZERenderStageShadow::ResetStates(const ZEMaterial* Material)
{
	// Reset to default states
	// ---------------------------------------------------------
	if (!ZERenderStage::ResetStates(Material))
		return false;

	ZERenderStageShadowData* ShadowStageData = (ZERenderStageShadowData*)StageData;

	// Render targets
	DefaultStates.DepthStencilBuffer = ShadowStageData->DepthBuffer;
	DefaultStates.RenderTargets[0] = ShadowStageData->RenderTarget;
	
	ZEInt Width, Height;
	zeGraphics->GetWindow()->GetSize(Width, Height);

	// Use default viewport and scissor rectangles
	DefaultStates.ViewPorts[0].StateData.TopLeftX = 0.0f;
	DefaultStates.ViewPorts[0].StateData.TopLeftY = 0.0f;
	DefaultStates.ViewPorts[0].StateData.Width = (float)Width;
	DefaultStates.ViewPorts[0].StateData.Height = (float)Height;
	DefaultStates.ViewPorts[0].StateData.MinDepth = 0.0f;
	DefaultStates.ViewPorts[0].StateData.MaxDepth = 1.0f;

	DefaultStates.ScissorRects[0].StateData.Left = 0;
	DefaultStates.ScissorRects[0].StateData.Top = 0;
	DefaultStates.ScissorRects[0].StateData.Right = Width;
	DefaultStates.ScissorRects[0].StateData.Bottom = Width;
	
	// Rasterizer state
	DefaultStates.RasterizerState.SetFillMode(ZE_FM_SOLID);
	DefaultStates.RasterizerState.SetCullDirection(ZE_CD_COUNTER_CLOCKWISE);

	// Blend state
	DefaultStates.BlendState.SetComponentWriteMask(0, ZE_CM_RED);

	// Depth stencil state
	DefaultStates.DepthStencilState.SetZTestEnable(true);
	DefaultStates.DepthStencilState.SetZFunction(ZE_CF_LESS_EQUAL);
	DefaultStates.DepthStencilState.SetZWriteEnable(true);

	// Commit default states
	// ----------------------------------------------------------
	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	Device->SetDepthStencilBuffer(DefaultStates.DepthStencilBuffer);
	Device->SetRenderTargetArray(DefaultStates.RenderTargets);

	Device->SetBlendState(DefaultStates.BlendState);
	Device->SetDepthStencilState(DefaultStates.DepthStencilState);
	Device->SetRasterizerState(DefaultStates.RasterizerState);

	Device->SetViewport(0, DefaultStates.ViewPorts[0]);
	Device->SetScissorRectangle(0, DefaultStates.ScissorRects[0]);

	return true;
}

ZERenderStageType ZERenderStageShadow::GetStageType() const
{
	return ZE_RST_SHADOW;
}

ZERenderStageType ZERenderStageShadow::GetDependencies() const
{
	return ZE_RST_NONE;
}

bool ZERenderStageShadow::Setup()
{
	if (!ZERenderStage::Setup())
		return false;

	ZERenderStageShadowData* ShadowStageData = (ZERenderStageShadowData*)StageData;

	ZEGraphicsDevice* Device = zeGraphics->GetDevice();
	Device->ClearRenderTarget(ShadowStageData->RenderTarget, ZEVector4::Zero);
	Device->ClearDepthStencilBuffer(ShadowStageData->DepthBuffer, true, true, 1.0f, 0);

	return true;
}

bool ZERenderStageShadow::Process(const ZERenderCommand* RenderCommand)
{
	if (!ZERenderStage::Process(RenderCommand))
		return false;	

	ZEUInt PassId = 0;
	bool Continue = false;

	do
	{
		Continue = RenderCommand->Material->SetupPass(PassId++, this, RenderCommand);
	
	} while (Continue);

	PumpStreams(RenderCommand);

	return true;
}

ZERenderStageShadow::ZERenderStageShadow()
{

}

ZERenderStageShadow::~ZERenderStageShadow()
{

}
