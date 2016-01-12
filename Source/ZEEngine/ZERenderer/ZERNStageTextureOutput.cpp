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

#include "ZERNRenderer.h"
#include "ZERNStageID.h"
#include "ZERNStageGBuffer.h"
#include "ZERNStageHDR.h"
#include "ZERNStagePostProcess.h"

#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGROutput.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"

bool ZERNStageTextureOutput::InitializeSelf()
{
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

	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(Constants));

	return true;
}

void ZERNStageTextureOutput::DeinitializeSelf()
{
	VertexShader.Release();
	PixelShader.Release();
	RenderStateData.Release();
	ConstantBuffer.Release();
}

void ZERNStageTextureOutput::SetInputs(ZEGRTexture** Inputs, ZESize Count)
{
	zeDebugCheck(Count > ZEGR_MAX_VIEWPORT_SLOT, "Count is too much");
	
	InputTextures.Resize(Count);
	memcpy(&InputTextures[0], &Inputs[0], sizeof(ZEGRTexture*) * Count);
}

ZEGRTexture** ZERNStageTextureOutput::GetInputs()
{
	return &InputTextures[0];
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

void ZERNStageTextureOutput::SetAutoSplit(bool AutoSplit)
{
	this->AutoSplit = AutoSplit;
}

bool ZERNStageTextureOutput::IsAutoSplit() const
{
	return AutoSplit;
}

ZERNStageTextureOutput::ZERNStageTextureOutput()
{
	OutputRenderTarget = NULL;
	AutoSplit = true;
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
	ZERNStageHDR* StageHDR = (ZERNStageHDR*)Renderer->GetStage(ZERN_STAGE_HDR);
	if(StageHDR != NULL)
		return false;

	ZEGRRenderTarget* RenderTarget = Renderer->GetOutputRenderTarget();
	if(RenderTarget == NULL)
		return false;

	OutputRenderTarget = RenderTarget;

	if(InputTextures.GetCount() == 0)
	{
		ZERNStagePostProcess* StagePostProcess = (ZERNStagePostProcess*)Renderer->GetStage(ZERN_STAGE_POST_EFFECT);
		if(StagePostProcess != NULL)
		{
			InputTextures.Add(StagePostProcess->GetOutputTexture());
		}
		else
		{		
			ZERNStageGBuffer* StageGBuffer = (ZERNStageGBuffer*)Renderer->GetStage(ZERN_STAGE_GBUFFER);
			if(StageGBuffer == NULL)
				return false;

			InputTextures.Add(StageGBuffer->GetAccumulationMap());
		}
	}

	Context->SetRenderState(RenderStateData);
	Context->SetRenderTargets(1, &OutputRenderTarget, NULL);
	Context->SetVertexBuffers(0, 0, NULL);

	ZEUInt Count = (ZEUInt)InputTextures.GetCount();
	for(ZEUInt I = 0; I < Count; I++)
	{
		ZEGRTexture* Texture = InputTextures[I];
		Context->SetTexture(ZEGR_ST_PIXEL, 5, Texture);
		if(Texture->GetTextureType() == ZEGR_TT_2D)
		{
			ZEGRTexture2D* Texture2D = static_cast<ZEGRTexture2D*>(Texture);
			if(Texture2D->GetArrayCount() > 1)
			{
				Constants.TextureIndex = I;
				ConstantBuffer->SetData(&Constants);
				Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, ConstantBuffer);
			}
			else
			{
				Constants.TextureIndex = 0;
				ConstantBuffer->SetData(&Constants);
				Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, ConstantBuffer);
			}
		}

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
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, NULL);
}
