//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageMultiplexer.cpp
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

#include "ZERNStageMultiplexer.h"

#include "ZERNRenderer.h"
#include "ZERNStageID.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGROutput.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZERNStageDisplay.h"

bool ZERNStageMultiplexer::UpdateInputOutputs()
{
	OutputRenderTarget = GetNextProvidedInput(ZERN_SO_COLOR);
	if (OutputRenderTarget == NULL)
	{
		ZEGRRenderTarget* RenderTarget = GetRenderer()->GetOutputRenderTarget();
		if (RenderTarget == NULL)
			return false;

		ZEUInt Width = RenderTarget->GetWidth();
		ZEUInt Height = RenderTarget->GetHeight();

		// No Provided Output - Create Own Buffer
		if (OutputTexture == NULL || 
			OutputTexture->GetWidth() != Width || OutputTexture->GetHeight() !=  Height)
		{
			OutputTexture.Release();
			OutputTexture = ZEGRTexture2D::CreateInstance(Width, Height, 1, ZEGR_TF_R8G8B8A8_UNORM_SRGB);
		}

		OutputRenderTarget = OutputTexture->GetRenderTarget();
	}
	else
	{
		// Output is provided - Release own output texture and use provided one.
		OutputTexture.Release();
	}

	return true;
}

void ZERNStageMultiplexer::DrawSingle(ZEGRContext* Context)
{
	if (Displays.GetCount() == 0)
		return;

	if (Mode != ZERN_SMM_CUSTOM)
	{
		ZEGRViewport Viewport;
		Viewport.SetX(0.0f);
		Viewport.SetY(0.0f);
		Viewport.SetWidth(OutputRenderTarget->GetWidth());
		Viewport.SetHeight(OutputRenderTarget->GetHeight());

		Displays[0]->Setup(Context, Viewport);
	}
	else
	{
		Displays[0]->Setup(Context);
	}
}

void ZERNStageMultiplexer::DrawVertical2(ZEGRContext* Context)
{
	ZEUInt ViewportWidth = OutputRenderTarget->GetWidth() / 2;
	ZEUInt ViewportHeight = OutputRenderTarget->GetHeight();

	if (Displays.GetCount() > 0)
	{
		if (Mode != ZERN_SMM_CUSTOM)
		{
			ZEGRViewport Viewport;
			Viewport.SetX(0.0f);
			Viewport.SetY(0.0f);
			Viewport.SetWidth(ViewportWidth);
			Viewport.SetHeight(ViewportHeight);

			Displays[0]->Setup(Context, Viewport);
		}
		else
		{
			Displays[0]->Setup(Context);
		}
	}

	if (Displays.GetCount() > 1)
	{
		if (Mode != ZERN_SMM_CUSTOM)
		{
			ZEGRViewport Viewport;
			Viewport.SetX(ViewportWidth);
			Viewport.SetY(0.0f);
			Viewport.SetWidth(ViewportWidth);
			Viewport.SetHeight(ViewportHeight);

			Displays[1]->Setup(Context, Viewport);
		}
		else
		{
			Displays[1]->Setup(Context);
		}
	}
}

void ZERNStageMultiplexer::DrawHorizontal2(ZEGRContext* Context)
{
	ZESize ViewportWidth = OutputRenderTarget->GetWidth();
	ZESize ViewportHeight = OutputRenderTarget->GetHeight() / 2;

	if (Displays.GetCount() > 0)
	{
		if (Mode != ZERN_SMM_CUSTOM)
		{
			ZEGRViewport Viewport;
			Viewport.SetX(0.0f);
			Viewport.SetY(0.0f);
			Viewport.SetWidth(ViewportWidth);
			Viewport.SetHeight(ViewportHeight);

			Displays[0]->Setup(Context, Viewport);
		}
		else
		{
			Displays[0]->Setup(Context);
		}
	}

	if (Displays.GetCount() > 1)
	{
		if (Mode != ZERN_SMM_CUSTOM)
		{
			ZEGRViewport Viewport;
			Viewport.SetX(0.0f);
			Viewport.SetY(ViewportHeight);
			Viewport.SetWidth(ViewportWidth);
			Viewport.SetHeight(ViewportHeight);

			Displays[1]->Setup(Context, Viewport);
		}
		else
		{
			Displays[1]->Setup(Context);
		}
	}
}

void ZERNStageMultiplexer::Draw2x2(ZEGRContext* Context)
{
	ZESize ViewportWidth = OutputRenderTarget->GetWidth() / 2;
	ZESize ViewportHeight = OutputRenderTarget->GetHeight() / 2;

	if (Displays.GetCount() > 0)
	{
		if (Mode != ZERN_SMM_CUSTOM)
		{
			ZEGRViewport Viewport;
			Viewport.SetX(0.0f);
			Viewport.SetY(0.0f);
			Viewport.SetWidth(ViewportWidth);
			Viewport.SetHeight(ViewportHeight);

			Displays[0]->Setup(Context, Viewport);
		}
		else
		{
			Displays[0]->Setup(Context);
		}
	}

	if (Displays.GetCount() > 1)
	{
		if (Mode != ZERN_SMM_CUSTOM)
		{
			ZEGRViewport Viewport;
			Viewport.SetX(ViewportWidth);
			Viewport.SetY(0.0f);
			Viewport.SetWidth(ViewportWidth);
			Viewport.SetHeight(ViewportHeight);

			Displays[1]->Setup(Context, Viewport);

		}
		else
		{
			Displays[1]->Setup(Context);
		}
	}

	if (Displays.GetCount() > 2)
	{
		if (Mode != ZERN_SMM_CUSTOM)
		{
			ZEGRViewport Viewport;
			Viewport.SetX(0.0f);
			Viewport.SetY(ViewportHeight);
			Viewport.SetWidth(ViewportWidth);
			Viewport.SetHeight(ViewportHeight);

			Displays[2]->Setup(Context, Viewport);
		}
		else
		{
			Displays[2]->Setup(Context);
		}
	}

	if (Displays.GetCount() > 3)
	{
		if (Mode != ZERN_SMM_CUSTOM)
		{
			ZEGRViewport Viewport;
			Viewport.SetX(ViewportWidth);
			Viewport.SetY(ViewportHeight);
			Viewport.SetWidth(ViewportWidth);
			Viewport.SetHeight(ViewportHeight);

			Displays[3]->Setup(Context, Viewport);
		}
		else
		{
			Displays[3]->Setup(Context);
		}
	}
}

void ZERNStageMultiplexer::DrawCustom(ZEGRContext* Context)
{
	ze_for_each(Input, Displays)
		Input->Setup(Context);
}

bool ZERNStageMultiplexer::InitializeSelf()
{
	if (!ZERNStage::InitializeSelf())
		return false;

	ZEGRShaderCompileOptions Options;
	Options.Model = ZEGR_SM_5_0;
	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNStageMultiplexer.hlsl";

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNScreenCover_VertexShader_PositionTexcoords";
	ZEHolder<ZEGRShader> VertexShader = ZEGRShader::Compile(Options);
	zeCheckError(VertexShader == NULL, false, "Cannot set vertex shader.");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNStageMultiplexer_PixelShader";
	ZEHolder<ZEGRShader> PixelShader = ZEGRShader::Compile(Options);
	zeCheckError(PixelShader == NULL, false, "Cannot set pixel shader.");

	ZEGRRenderState RenderState;
	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);
	RenderState.SetShader(ZEGR_ST_VERTEX, VertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, PixelShader);

	RenderStateData = RenderState.Compile();
	zeCheckError(RenderStateData == NULL, false, "Cannot set render state");

	ze_for_each(Display, Displays)
		Display->Initialize();

	return true;
}

void ZERNStageMultiplexer::DeinitializeSelf()
{
	OutputTexture.Release();
	RenderStateData.Release();

	ze_for_each(Display, Displays)
		Display->Deinitialize();

	OutputRenderTarget = NULL;

	ZERNStage::Deinitialize();
}

ZEInt ZERNStageMultiplexer::GetId() const
{
	return ZERN_STAGE_TEXTURE_OUTPUT;
}

const ZEString& ZERNStageMultiplexer::GetName() const
{
	static const ZEString Name = "Texture Output";
	return Name;
}

void ZERNStageMultiplexer::SetMode(ZERNStageMultiplexerMode Mode)
{
	this->Mode = Mode;
}

ZERNStageMultiplexerMode ZERNStageMultiplexer::GetMode()
{
	return Mode;
}

const ZEList2<ZERNStageDisplay>& ZERNStageMultiplexer::GetInputs()
{
	return Displays;
}

void ZERNStageMultiplexer::AddInput(ZERNStageDisplay* Input)
{
	zeCheckError(Input->Owner != NULL, ZE_VOID, "Multiplexer input is already registered with a stage.");
	
	Displays.AddEnd(&Input->StageLink);

	Input->Owner = this;
	if (IsInitialized())
		Input->Initialize();
}

void ZERNStageMultiplexer::RemoveInput(ZERNStageDisplay* Input)
{
	zeCheckError(Input->Owner != this, ZE_VOID, "Multiplexer input is not associated with a stage.");

	Input->Deinitialize();
	Input->Owner = NULL;

	Displays.Remove(&Input->StageLink);
}

const ZEGRRenderTarget* ZERNStageMultiplexer::GetProvidedInput(ZERNStageBuffer Input) const
{
	if (GetEnabled() && Input == ZERN_SO_COLOR)
		return NULL;
	else
		return ZERNStage::GetProvidedInput(Input);
}

const ZEGRTexture2D* ZERNStageMultiplexer::GetOutput(ZERNStageBuffer Output) const
{
	if (GetEnabled() && Output == ZERN_SO_COLOR)
		return OutputTexture;
	else
		return ZERNStage::GetOutput(Output);
}

bool ZERNStageMultiplexer::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	if (!UpdateInputOutputs())
		return false;

	if (OutputRenderTarget == NULL)
		return false;

	Context->SetRenderState(RenderStateData);
	Context->SetRenderTargets(1, &OutputRenderTarget, NULL);

	switch (Mode)
	{
		case ZERN_SMM_SINGLE:
			DrawSingle(Context);
			break;

		case ZERN_SMM_VERTICAL:
			DrawVertical2(Context);
			break;

		case ZERN_SMM_HORIZONTAL:
			DrawHorizontal2(Context);
			break;

		case ZERN_SMM_2x2:
			Draw2x2(Context);
			break;

		case ZERN_SMM_CUSTOM:
			DrawCustom(Context);
			break;

		default:
			break;
	}

	return true;
}

void ZERNStageMultiplexer::CleanUp(ZEGRContext* Context)
{
	ZERNStage::CleanUp(Context);
}

ZERNStageMultiplexer::ZERNStageMultiplexer()
{
	Mode = ZERN_SMM_SINGLE;
	OutputRenderTarget = NULL;
}

ZERNStageMultiplexer::~ZERNStageMultiplexer()
{
	while(Displays.GetFirst() != NULL)
	{
		ZERNStageDisplay* Input = Displays.GetFirst()->GetItem();
		Displays.Remove(&Input->StageLink);
		delete Input;
	}
}
