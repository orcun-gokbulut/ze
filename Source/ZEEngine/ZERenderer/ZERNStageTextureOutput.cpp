//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageTextureOutput.cpp
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

#include "ZERNStageTextureOutput.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZERNStageID.h"
#include "ZERNRenderer.h"
#include "ZEGraphics/ZEGROutput.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZERNStageGBuffer.h"

bool ZERNStageTextureOutput::InitializeSelf()
{
	ZERNStage::InitializeSelf();

	ZEGRShaderCompileOptions Options;
	Options.Model = ZEGR_SM_5_0;
	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNTextureRendering.hlsl";

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNScreenCover_VertexShader_PositionTexcoords";
	VertexShader = ZEGRShader::Compile(Options);
	zeCheckError(VertexShader == NULL, false, "Cannot set vertex shader.");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNTextureRendering_PixelShader_Main";
	PixelShader = ZEGRShader::Compile(Options);
	zeCheckError(PixelShader == NULL, false, "Cannot set pixel shader.");

	ZEGRRenderState RenderState;
	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);
	RenderState.SetShader(ZEGR_ST_VERTEX, VertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, PixelShader);

	RenderStateData= RenderState.Compile();
	zeCheckError(RenderStateData == NULL, false, "Cannot set render state");

	OutputRenderTarget = NULL;
	AutoSplit = true;

	return true;
}

void ZERNStageTextureOutput::DeinitializeSelf()
{
	VertexShader.Release();
	PixelShader.Release();
	RenderStateData.Release();
}

void ZERNStageTextureOutput::SetInputs(ZEGRTexture2D** Inputs, ZESize Count)
{
	zeDebugCheck(Count > ZEGR_MAX_VIEWPORT_SLOT, "Count is too much");
	
	InputTextures.Resize(Count);
	memcpy(&InputTextures[0], &Inputs[0], sizeof(ZEGRTexture2D*) * Count);
}

ZEGRTexture2D*const* ZERNStageTextureOutput::GetInputs()
{
	return &InputTextures[0];
}

void ZERNStageTextureOutput::SetOutput(ZEGRRenderTarget* Output)
{
	OutputRenderTarget = Output;
}

const ZEGRRenderTarget* ZERNStageTextureOutput::GetOutput() const
{
	return OutputRenderTarget;
}

void ZERNStageTextureOutput::SetViewports(ZEGRViewport* Viewports, ZESize Count)
{
	zeDebugCheck(Count > ZEGR_MAX_VIEWPORT_SLOT, "Count is too much");

	memcpy(this->Viewports, Viewports, sizeof(ZEGRViewport) * Count);
}

const ZEGRViewport* ZERNStageTextureOutput::GetViewport() const
{
	return Viewports;
}

ZEInt ZERNStageTextureOutput::GetId()
{
	return ZERN_STAGE_TEXTURE_OUTPUT;
}

const ZEString& ZERNStageTextureOutput::GetName()
{
	static ZEString Name = "Texture Output";
	return Name;
}

bool ZERNStageTextureOutput::Setup(ZERNRenderer* Renderer, ZEGRContext* Context, ZEList2<ZERNCommand>& Commands)
{
	ZEGROutput* Output = Renderer->GetOutput();
	if(Output == NULL)
		return false;

	ZEGRRenderTarget* RenderTarget = Output->GetRenderTarget();
	if(RenderTarget == NULL)
		return false;

	OutputRenderTarget = RenderTarget;

	if(InputTextures.GetCount() == 0)
	{
		ZERNStageGBuffer* StageGBuffer = (ZERNStageGBuffer*)Renderer->GetStage(ZERN_STAGE_GBUFFER);
		if(StageGBuffer == NULL)
			return false;

		InputTextures.Add(StageGBuffer->GetAccumulationMap());
	}

	Context->SetRenderState(RenderStateData);
	Context->SetRenderTargets(1, &OutputRenderTarget, NULL);
	Context->SetVertexBuffers(0, 0, NULL);

	ZEUInt Count = (ZEUInt)InputTextures.GetCount();
	for(ZEUInt I = 0; I < Count; ++I)
	{
		Context->SetTexture(ZEGR_ST_PIXEL, 0, InputTextures[I]);

		if(AutoSplit)
		{
			ZEUInt Width = Count == 1 ? OutputRenderTarget->GetWidth() : OutputRenderTarget->GetWidth() / 2;
			ZEUInt Height = Count <= 2 ? OutputRenderTarget->GetHeight() : OutputRenderTarget->GetHeight() / 2;

			Viewports[I].SetX((float)((I & 1) * Width));
			Viewports[I].SetY((float)((I >> 1) * Height));
			Viewports[I].SetWidth((float)Width);
			Viewports[I].SetHeight((float)Height);
			Viewports[I].SetMinDepth(0.0f);
			Viewports[I].SetMaxDepth(1.0f);
		}

		Context->SetViewports(1, &Viewports[I]);

		Context->Draw(3, 0);
	}

	return true;
}

void ZERNStageTextureOutput::CleanUp(ZERNRenderer* Renderer, ZEGRContext* Context)
{
	InputTextures.Clear();

	Context->SetRenderTargets(0, NULL, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 0, NULL);
}

void ZERNStageTextureOutput::SetAutoSplit(bool AutoSplit)
{
	this->AutoSplit = AutoSplit;
}

bool ZERNStageTextureOutput::IsAutoSplit() const
{
	return AutoSplit;
}
