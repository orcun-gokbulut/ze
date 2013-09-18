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
#include "ZEGraphics/ZEGraphicsWindow.h"
#include "ZEGraphics/ZEShaderMetaTable.h"
#include "ZEGraphics/ZEGraphicsEventTracer.h"


void ZERenderStageLighting::DestroyBuffers()
{
	ZE_DESTROY(RenderTargets.LightSpecular);
	ZE_DESTROY(Textures.LightSpecular);
}

void ZERenderStageLighting::UpdateBuffers()
{
	ZETexture2D* Texture = NULL;
	ZEInt Width, Height;
	zeGraphics->GetWindow()->GetSize(Width, Height);

	Texture = Textures.LightSpecular;
	if (Texture == NULL || Texture->GetWidth() != Width || Texture->GetHeight() != Height)
	{
		ZE_DESTROY(RenderTargets.LightSpecular);
		ZE_DESTROY(Textures.LightSpecular);

		Textures.LightSpecular = ZETexture2D::CreateInstance();
		Textures.LightSpecular->CreateStatic(Width, Height, 1, ZE_TPF_F16_4, true, NULL);
		RenderTargets.LightSpecular = Textures.LightSpecular->CreateRenderTarget(0);
	}
}

bool ZERenderStageLighting::ResetStates(const ZEMaterial* Material)
{
	// Reset to default states
	// ----------------------------------------------------------
	if (!ZERenderStage::ResetStates(Material))
		return false;

	// Render targets
	DefaultStates.RenderTargets[0] = RenderTargets.LightSpecular;
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
	
	// Rasterizer state
	DefaultStates.RasterizerState.SetFrontIsCounterClockwise(false);
	DefaultStates.RasterizerState.SetCullDirection(ZE_CD_CLOCKWISE);
	DefaultStates.RasterizerState.SetFillMode(ZE_FM_SOLID);

	// Blend state
	DefaultStates.BlendState.SetBlendEnable(0, true);
	DefaultStates.BlendState.SetComponentWriteMask(0, ZE_CM_ALL);
	DefaultStates.BlendState.SetBlendEquation(ZE_BE_ADD);
	DefaultStates.BlendState.SetBlendAlphaEquation(ZE_BE_ADD);
	DefaultStates.BlendState.SetSourceBlendOption(ZE_BO_ONE);
	DefaultStates.BlendState.SetSourceBlendAlphaOption(ZE_BO_ONE);
	DefaultStates.BlendState.SetDestinationBlendOption(ZE_BO_ONE);
	DefaultStates.BlendState.SetDestinationBlendAlphaOption(ZE_BO_ONE);

	// Depth stencil state
	DefaultStates.DepthStencilState.SetZTestEnable(true);
	DefaultStates.DepthStencilState.SetZFunction(ZE_CF_GREATER_EQUAL);
	DefaultStates.DepthStencilState.SetZWriteEnable(false);
	DefaultStates.DepthStencilState.SetStencilTestEnable(false);

	// Samplers
	DefaultStates.PixelShaderSamplers[0].SetAddressU(ZE_TAM_CLAMP);
	DefaultStates.PixelShaderSamplers[0].SetAddressV(ZE_TAM_CLAMP);
	DefaultStates.PixelShaderSamplers[0].SetAddressW(ZE_TAM_CLAMP);
	DefaultStates.PixelShaderSamplers[0].SetMinFilter(ZE_TFM_POINT);
	DefaultStates.PixelShaderSamplers[0].SetMagFilter(ZE_TFM_POINT);
	DefaultStates.PixelShaderSamplers[0].SetMipFilter(ZE_TFM_POINT);

	DefaultStates.PixelShaderSamplers[1].SetAddressU(ZE_TAM_CLAMP);
	DefaultStates.PixelShaderSamplers[1].SetAddressV(ZE_TAM_CLAMP);
	DefaultStates.PixelShaderSamplers[1].SetAddressW(ZE_TAM_CLAMP);
	DefaultStates.PixelShaderSamplers[1].SetMinFilter(ZE_TFM_POINT);
	DefaultStates.PixelShaderSamplers[1].SetMagFilter(ZE_TFM_POINT);
	DefaultStates.PixelShaderSamplers[1].SetMipFilter(ZE_TFM_POINT);

	DefaultStates.PixelShaderSamplers[2].SetAddressU(ZE_TAM_CLAMP);
	DefaultStates.PixelShaderSamplers[2].SetAddressV(ZE_TAM_CLAMP);
	DefaultStates.PixelShaderSamplers[2].SetAddressW(ZE_TAM_CLAMP);
	DefaultStates.PixelShaderSamplers[2].SetMinFilter(ZE_TFM_POINT);
	DefaultStates.PixelShaderSamplers[2].SetMagFilter(ZE_TFM_POINT);
	DefaultStates.PixelShaderSamplers[2].SetMipFilter(ZE_TFM_POINT);

	// Textures
	DefaultStates.PixelShaderTextures[0] = InputStageGeometry->GetTextureDepth();
	DefaultStates.PixelShaderTextures[1] = InputStageGeometry->GetTextureNormalSpecular();
	DefaultStates.PixelShaderTextures[2] = InputStageGeometry->GetTextureSSScatteringVelocity();

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

	Device->SetPixelShaderSampler(0, DefaultStates.PixelShaderSamplers[0]);
	Device->SetPixelShaderSampler(1, DefaultStates.PixelShaderSamplers[1]);
	Device->SetPixelShaderSampler(2, DefaultStates.PixelShaderSamplers[2]);
	
	Device->SetPixelShaderTexture(0, DefaultStates.PixelShaderTextures[0]);
	Device->SetPixelShaderTexture(1, DefaultStates.PixelShaderTextures[1]);
	Device->SetPixelShaderTexture(2, DefaultStates.PixelShaderTextures[2]);

	return true;
}

const ZETexture2D* ZERenderStageLighting::GetTextureLightSpecular() const
{
	return Textures.LightSpecular;
}

const ZERenderTarget* ZERenderStageLighting::GetRenderTargetLightSpecular() const
{
	return RenderTargets.LightSpecular;
}

void ZERenderStageLighting::SetInputStageGeometry(const ZERenderStageGeometry* Stage)
{
	zeDebugCheck(Stage == NULL, "Null pointer.");
	
	InputStageGeometry = Stage;
}

const ZERenderStageGeometry* ZERenderStageLighting::GetInputStageGeometry() const
{
	return InputStageGeometry;
}

bool ZERenderStageLighting::Setup()
{
	if (!ZERenderStage::Setup())
		return false;

	UpdateBuffers();

	ZEGraphicsDevice* Device = zeGraphics->GetDevice();
	Device->ClearRenderTarget(RenderTargets.LightSpecular, ZEVector4::Zero);

	return true;
}

bool ZERenderStageLighting::Process(const ZERenderCommand* RenderCommand)
{
	if (!ZERenderStage::Process(RenderCommand))
		return false;

	if (InputStageGeometry == NULL)
	{
		zeWarning("Null input stage");
		return false;
	}

	ZEUInt PassId = 0;
	bool Continue = false;

	do
	{
		Continue = RenderCommand->Material->SetupPass(PassId++, this, RenderCommand);
	
	} while (Continue);

	PumpStreams(RenderCommand);

	return true;
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
	InputStageGeometry = NULL;
	
	Textures.LightSpecular = NULL;
	RenderTargets.LightSpecular = NULL;
}

ZERenderStageLighting::~ZERenderStageLighting()
{
	DestroyBuffers();
}
