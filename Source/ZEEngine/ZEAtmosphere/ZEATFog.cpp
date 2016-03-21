//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEATFog.cpp
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

#include "ZEATFog.h"

#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZERNCuller.h"
#include "ZERenderer/ZERNStagePostProcess.h"

#define	ZEAT_FDF_SHADERS			1
#define ZEAT_FDF_RENDER_STATES		2
#define ZEAT_FDF_CONSTANT_BUFFERS	4

bool ZEATFog::UpdateShaders()
{
	if (!DirtyFlags.GetFlags(ZEAT_FDF_SHADERS))
		return true;

	ZEGRShaderCompileOptions Options;
	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNFog.hlsl";
	Options.Model = ZEGR_SM_5_0;

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNScreenCover_VertexShader_Position";
	ScreenCoverVertexShader = ZEGRShader::Compile(Options);
	zeCheckError(ScreenCoverVertexShader == NULL, false, "Can not compile vertex shader");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNFog_PixelShader_Main";
	PixelShader = ZEGRShader::Compile(Options);
	zeCheckError(PixelShader == NULL, false, "Can not compile pixel shader");

	DirtyFlags.UnraiseFlags(ZEAT_FDF_SHADERS);
	DirtyFlags.RaiseFlags(ZEAT_FDF_RENDER_STATES);

	return true;
}

bool ZEATFog::UpdateRenderStates()
{
	if (!DirtyFlags.GetFlags(ZEAT_FDF_RENDER_STATES))
		return true;

	ZEGRRenderState RenderState = ZERNStagePostProcess::GetRenderState();
	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);

	ZEGRBlendState BlendStateAlphaBlend;
	BlendStateAlphaBlend.SetBlendEnable(true);
	ZEGRBlendRenderTarget BlendRenderTargetAlphaBlend = BlendStateAlphaBlend.GetRenderTarget(0);
	BlendRenderTargetAlphaBlend.SetSource(ZEGR_BO_INV_SRC_ALPHA);
	BlendRenderTargetAlphaBlend.SetDestination(ZEGR_BO_SRC_ALPHA);
	BlendRenderTargetAlphaBlend.SetOperation(ZEGR_BE_ADD);
	BlendRenderTargetAlphaBlend.SetBlendEnable(true);
	BlendStateAlphaBlend.SetRenderTargetBlend(0, BlendRenderTargetAlphaBlend);

	RenderState.SetBlendState(BlendStateAlphaBlend);

	RenderState.SetShader(ZEGR_ST_VERTEX, ScreenCoverVertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, PixelShader);

	RenderStateData = RenderState.Compile();
	zeCheckError(RenderStateData == NULL, false, "Can not compiel render state");

	DirtyFlags.UnraiseFlags(ZEAT_FDF_RENDER_STATES);

	return true;
}

bool ZEATFog::UpdateConstantBuffers()
{
	if (!DirtyFlags.GetFlags(ZEAT_FDF_CONSTANT_BUFFERS))
		return true;

	ConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZEAT_FDF_CONSTANT_BUFFERS);

	return true;
}

bool ZEATFog::Update()
{
	if (!UpdateShaders())
		return false;

	if (!UpdateRenderStates())
		return false;

	if (!UpdateConstantBuffers())
		return false;

	return true;
}

bool ZEATFog::InitializeSelf()
{
	if (!ZEEntity::InitializeSelf())
		return false;

	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(Constants));

	return true;
}

bool ZEATFog::DeinitializeSelf()
{
	ScreenCoverVertexShader.Release();
	PixelShader.Release();
	RenderStateData.Release();
	ConstantBuffer.Release();

	return ZEEntity::DeinitializeSelf();
}

ZEATFog::ZEATFog()
{
	DirtyFlags.RaiseAll();

	Command.Entity = this;
	Command.Priority = 4;
	Command.StageMask = ZERN_STAGE_POST_EFFECT;

	Constants.Density = 0.5f;
	Constants.Range = 1000.0f;
	Constants.Color = ZEVector3(0.15f);
}

ZEATFog::~ZEATFog()
{

}

ZEDrawFlags ZEATFog::GetDrawFlags() const
{
	return ZE_DF_DRAW;
}

void ZEATFog::SetDensity(float Density)
{
	if (Constants.Density == Density)
		return;

	Constants.Density = Density;

	DirtyFlags.RaiseFlags(ZEAT_FDF_CONSTANT_BUFFERS);
}

float ZEATFog::GetDensity() const
{
	return Constants.Density;
}

void ZEATFog::SetRange(float Range)
{
	if (Constants.Range == Range)
		return;

	Constants.Range = Range;

	DirtyFlags.RaiseFlags(ZEAT_FDF_CONSTANT_BUFFERS);
}

float ZEATFog::GetRange() const
{
	return Constants.Range;
}

void ZEATFog::SetColor(const ZEVector3& Color)
{
	if (Constants.Color == Color)
		return;

	Constants.Color = Color;

	DirtyFlags.RaiseFlags(ZEAT_FDF_CONSTANT_BUFFERS);
}

const ZEVector3& ZEATFog::GetColor() const
{
	return Constants.Color;
}

bool ZEATFog::PreRender(const ZERNCullParameters* CullParameters)
{
	CullParameters->Renderer->AddCommand(&Command);

	return true;
}

void ZEATFog::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	if (!Update())
		return;

	ZEGRContext* Context = Parameters->Context;
	ZERNStage* Stage = Parameters->Stage;

	const ZEGRRenderTarget* RenderTarget = Stage->GetProvidedInput(ZERN_SO_COLOR);

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, ConstantBuffer);
	Context->SetRenderState(RenderStateData);
	Context->SetRenderTargets(1, &RenderTarget, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 0, Stage->GetOutput(ZERN_SO_DEPTH));
	Context->SetVertexBuffers(0, 0, NULL);
	
	Context->Draw(3, 0);

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, NULL);
	Context->SetRenderTargets(0, NULL, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 0, NULL);
}

ZEATFog* ZEATFog::CreateInstance()
{
	return new ZEATFog();
}
