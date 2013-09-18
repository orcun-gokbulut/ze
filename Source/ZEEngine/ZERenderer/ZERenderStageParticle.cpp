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
#include "ZEGraphics/ZEGraphicsWindow.h"
#include "ZEGraphics/ZEShaderMetaTable.h"
#include "ZEGraphics/ZEGraphicsEventTracer.h"

bool ZERenderStageParticle::ResetStates(const ZEMaterial* Material)
{
	// Reset to default states
	// ---------------------------------------------------------
	if (!ZERenderStage::ResetStates(Material))
		return false;

	// Outputs
	DefaultStates.RenderTargets[0] = InputStageAccumulation->GetRenderTargetAccumulation();
	DefaultStates.DepthStencilBuffer = zeGraphics->GetWindow()->GetDepthBuffer();

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
	DefaultStates.ScissorRects[0].StateData.Bottom = Height;
	
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

	// Commit default states
	// ----------------------------------------------------------
	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	Device->SetRenderTargetArray(DefaultStates.RenderTargets);
	Device->SetDepthStencilBuffer(DefaultStates.DepthStencilBuffer);

	Device->SetBlendState(DefaultStates.BlendState);
	Device->SetDepthStencilState(DefaultStates.DepthStencilState);

	Device->SetRasterizerState(DefaultStates.RasterizerState);

	Device->SetViewport(0, DefaultStates.ViewPorts[0]);
	Device->SetScissorRectangle(0, DefaultStates.ScissorRects[0]);


	return true;
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

void ZERenderStageParticle::SetInputStageAccumulation(const ZERenderStageAccumulation* Input)
{
	zeDebugCheck(Input == NULL, "Null pointer.");

	InputStageAccumulation = Input;
}

const ZERenderStageAccumulation* ZERenderStageParticle::GetInputStageAccumulation() const
{
	return InputStageAccumulation;
}
		
bool ZERenderStageParticle::Setup()
{
	if (!ZERenderStage::Setup())
		return false;

	UpdateBuffers();

	return true;
}

bool ZERenderStageParticle::Process(const ZERenderCommand* RenderCommand)
{
	zeDebugCheck(RenderCommand == NULL, "Null pointer");

	if (!ZERenderStage::Process(RenderCommand))
		return false;	

	if (InputStageAccumulation == NULL)
	{
		zeWarning("Null input stage");
		return false;
	}

	bool Done = false;
	ZEUInt PassId = 0;
	
	do
	{
		Done = RenderCommand->Material->SetupPass(PassId++, this, RenderCommand);
	
	} while (!Done);

	PumpStreams(RenderCommand);

	return true;
}

ZERenderStageParticle::ZERenderStageParticle()
{
	InputStageAccumulation = NULL;
}

ZERenderStageParticle::~ZERenderStageParticle()
{

}
