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

#include "ZEMath/ZEMath.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNRenderer.h"
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
	Options.Definitions.Add(ZEGRShaderDefinition("SAMPLE_COUNT", ZEString(ZEGRGraphicsModule::SAMPLE_COUNT)));

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

	Constants.Density = ZEMath::Power(Density, 8.0f);
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

ZEEntityResult ZEATFog::LoadInternal()
{
	ZE_ENTITY_LOAD_CHAIN(ZEEntity);

	ConstantBuffer = ZEGRConstantBuffer::CreateResource(sizeof(Constants));
	zeCheckError(ConstantBuffer == NULL, ZE_ER_FAILED_CLEANUP, "Cannot create constant buffer.");

	if (!Update())
		return ZE_ER_FAILED_CLEANUP;

	return ZE_ER_DONE;
}

ZEEntityResult ZEATFog::UnloadInternal()
{
	ScreenCoverVertexShader.Release();
	PixelShader.Release();
	RenderStateData.Release();
	ConstantBuffer.Release();

	ZE_ENTITY_UNLOAD_CHAIN(ZEEntity);
	return ZE_ER_DONE;
}

ZEATFog::ZEATFog()
{
	DirtyFlags.RaiseAll();

	Command.Entity = this;
	Command.Priority = 5;
	Command.StageMask = ZERN_STAGE_POST_EFFECT;

	Constants.Density = 0.5f;
	Constants.StartDistance = 0.0f;
	Constants.Color = ZEVector3(0.5f);

	SetEntityFlags(ZE_EF_RENDERABLE);
}

ZEATFog::~ZEATFog()
{

}

void ZEATFog::SetDensity(float Density)
{
	if (this->Density == Density)
		return;

	this->Density = Density;

	DirtyFlags.RaiseFlags(ZEAT_FDF_CONSTANT_BUFFERS);
}

float ZEATFog::GetDensity() const
{
	return Density;
}

void ZEATFog::SetStartDistance(float StartDistance)
{
	if (Constants.StartDistance == StartDistance)
		return;

	Constants.StartDistance = StartDistance;

	DirtyFlags.RaiseFlags(ZEAT_FDF_CONSTANT_BUFFERS);
}

float ZEATFog::GetStartDistance() const
{
	return Constants.StartDistance;
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

bool ZEATFog::PreRender(const ZERNPreRenderParameters* Parameters)
{
	if (!ZEEntity::PreRender(Parameters))
		return false;

	Parameters->Renderer->AddCommand(&Command);

	return true;
}

void ZEATFog::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	if (!Update())
		return;

	ZEGRContext* Context = Parameters->Context;
	const ZERNStage* Stage = Parameters->Stage;

	const ZEGRRenderTarget* RenderTarget = Stage->GetProvidedInput(ZERN_SO_COLOR);
	//const ZEGRTexture* DepthMap = Stage->GetOutput(ZERN_SO_DEPTH);

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 9, ConstantBuffer);
	Context->SetRenderState(RenderStateData);
	Context->SetRenderTargets(1, &RenderTarget, NULL);
	//Context->SetTextures(ZEGR_ST_PIXEL, 4, 1, &DepthMap);
	
	Context->Draw(3, 0);
}

ZEATFog* ZEATFog::CreateInstance()
{
	return new ZEATFog();
}
