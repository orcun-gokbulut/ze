//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNLightScattering.cpp
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

#include "ZERNLightScattering.h"
#include "ZEGraphics/ZEGRShaderCompileOptions.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"

void ZERNLightScattering::SetInputTexture(ZEGRTexture2D* InputTexture)
{
	this->InputTexture = InputTexture;
}

const ZEGRTexture2D* ZERNLightScattering::GetInputTexture() const
{
	return InputTexture;
}

void ZERNLightScattering::SetDepthTexture(ZEGRTexture2D* DepthTexture)
{
	this->DepthTexture = DepthTexture;
}

const ZEGRTexture2D* ZERNLightScattering::GetDepthTexture() const
{
	return DepthTexture;
}

void ZERNLightScattering::SetOutputRenderTarget(ZEGRRenderTarget* OutputRenderTarget)
{
	this->OutputRenderTarget = OutputRenderTarget;
}

const ZEGRRenderTarget* ZERNLightScattering::GetOutputRenderTarget() const
{
	return OutputRenderTarget;
}

void ZERNLightScattering::SetLightDirection(const ZEVector3& LightDirection)
{
	Constants.LightDirection = LightDirection;
}

const ZEVector3& ZERNLightScattering::GetLightDirection() const
{
	return Constants.LightDirection;
}

void ZERNLightScattering::SetLightIntensity(float LightIntensity)
{
	Constants.LightIntensity = LightIntensity;
}

float ZERNLightScattering::GetLightIntensity() const
{
	return Constants.LightIntensity;
}

void ZERNLightScattering::SetLightColor(const ZEVector3& LightColor)
{
	Constants.LightColor = LightColor;
}

const ZEVector3& ZERNLightScattering::GetLightColor() const
{
	return Constants.LightColor;
}

void ZERNLightScattering::SetMieScatteringStrengh(float MieScatteringStrength)
{
	Constants.MieScatteringStrength = MieScatteringStrength;
}

float ZERNLightScattering::GetMieScatteringStrengh() const
{
	return Constants.MieScatteringStrength;
}

void ZERNLightScattering::CreateShaders()
{
	ZEGRShaderCompileOptions Options;
	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNLightScattering.hlsl";
	Options.Model = ZEGR_SM_5_0;

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNScreenCover_VertexShader_Position";
	VertexShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNLightScattering_PixelShader_Main";
	PixelShader = ZEGRShader::Compile(Options);
}

void ZERNLightScattering::CreateRenderState()
{
	ZEGRRenderState RenderState;
	
	ZEGRDepthStencilState DepthStencilState;
	DepthStencilState.SetDepthTestEnable(false);
	DepthStencilState.SetDepthWriteEnable(false);

	RenderState.SetDepthStencilState(DepthStencilState);

	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);
	RenderState.SetShader(ZEGR_ST_VERTEX, VertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, PixelShader);

	RenderStateData = RenderState.Compile();
	zeCheckError(RenderStateData == NULL, , "Cannot set render state.");
}

bool ZERNLightScattering::InitializeSelf()
{
	CreateShaders();
	CreateRenderState();

	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(LightScatteringConstants));

	return true;
}

void ZERNLightScattering::DeinitializeSelf()
{
	InputTexture = NULL;
	DepthTexture = NULL;
	OutputRenderTarget = NULL;

	VertexShader.Release();
	PixelShader.Release();
	RenderStateData.Release();
}

void ZERNLightScattering::Process(ZEGRContext* Context)
{
	zeDebugCheck(InputTexture == NULL, "Input texture cannot be null");
	//zeDebugCheck(DepthTexture == NULL, "Depth texture cannot be null");
	zeDebugCheck(OutputRenderTarget == NULL, "Output render target cannot be null");

	ConstantBuffer->SetData(&Constants);

	Context->SetTexture(ZEGR_ST_PIXEL, 0, InputTexture);
	//Context->SetTexture(ZEGR_ST_PIXEL, 1, DepthTexture);
	Context->SetRenderTargets(1, &OutputRenderTarget, NULL);
	Context->SetRenderState(RenderStateData);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, ConstantBuffer);
	Context->SetVertexBuffers(0, 0, NULL);

	Context->Draw(3, 0);

	Context->SetTexture(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 1, NULL);
	Context->SetRenderTargets(0, NULL, NULL);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, NULL);
}

ZERNLightScattering::ZERNLightScattering()
{
	InputTexture = NULL;
	DepthTexture = NULL;
	OutputRenderTarget = NULL;
}

ZERNLightScattering::~ZERNLightScattering()
{
	InputTexture = NULL;
	DepthTexture = NULL;
	OutputRenderTarget = NULL;
}

