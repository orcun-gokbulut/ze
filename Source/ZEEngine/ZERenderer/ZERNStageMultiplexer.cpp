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

#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRBuffer.h"
#include "ZEGraphics/ZEGRTexture.h"
#include "ZEGraphics/ZEGROutput.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZERNRenderer.h"
#include "ZERNStageID.h"
#include "ZERNStageDisplay.h"

#define ZERN_SMDF_OUTPUT	1

void ZERNStageMultiplexer::DrawSingle(ZEGRContext* Context)
{
	Displays[0]->SetViewport(ZEGRViewport(0.0f, 0.0f, (float)OutputRenderTarget->GetWidth(), (float)OutputRenderTarget->GetHeight()));
	Displays[0]->Draw(Context);
}

void ZERNStageMultiplexer::DrawVertical2(ZEGRContext* Context)
{
	float ViewportWidth = (float)OutputRenderTarget->GetWidth() / 2.0f;
	float ViewportHeight = (float)OutputRenderTarget->GetHeight();

	if (Displays.GetCount() > 0)
	{
		Displays[0]->SetViewport(ZEGRViewport(0.0f, 0.0f, ViewportWidth, ViewportHeight));
		Displays[0]->Draw(Context);
	}

	if (Displays.GetCount() > 1)
	{
		Displays[1]->SetViewport(ZEGRViewport(ViewportWidth, 0.0f, ViewportWidth, ViewportHeight));
		Displays[1]->Draw(Context);
	}
}

void ZERNStageMultiplexer::DrawHorizontal2(ZEGRContext* Context)
{
	float ViewportWidth = (float)OutputRenderTarget->GetWidth();
	float ViewportHeight = (float)OutputRenderTarget->GetHeight() / 2.0f;

	if (Displays.GetCount() > 0)
	{
		Displays[0]->SetViewport(ZEGRViewport(0.0f, 0.0f, ViewportWidth, ViewportHeight));
		Displays[0]->Draw(Context);
	}

	if (Displays.GetCount() > 1)
	{
		Displays[1]->SetViewport(ZEGRViewport(0.0f, ViewportHeight, ViewportWidth, ViewportHeight));
		Displays[1]->Draw(Context);
	}
}

void ZERNStageMultiplexer::Draw2x2(ZEGRContext* Context)
{
	float ViewportWidth = (float)OutputRenderTarget->GetWidth() / 2.0f;
	float ViewportHeight = (float)OutputRenderTarget->GetHeight() / 2.0f;

	if (Displays.GetCount() > 0)
	{
		Displays[0]->SetViewport(ZEGRViewport(0.0f, 0.0f, ViewportWidth, ViewportHeight));
		Displays[0]->Draw(Context);
	}

	if (Displays.GetCount() > 1)
	{
		Displays[1]->SetViewport(ZEGRViewport(ViewportWidth, 0.0f, ViewportWidth, ViewportHeight));
		Displays[1]->Draw(Context);
	}

	if (Displays.GetCount() > 2)
	{
		Displays[2]->SetViewport(ZEGRViewport(0.0f, ViewportHeight, ViewportWidth, ViewportHeight));
		Displays[2]->Draw(Context);
	}

	if (Displays.GetCount() > 3)
	{
		Displays[3]->SetViewport(ZEGRViewport(ViewportWidth, ViewportHeight, ViewportWidth, ViewportHeight));
		Displays[3]->Draw(Context);
	}
}

void ZERNStageMultiplexer::DrawCustom(ZEGRContext* Context)
{
	ze_for_each(Input, Displays)
		Input->Draw(Context);
}

bool ZERNStageMultiplexer::InitializeInternal()
{
	if (!ZERNStage::InitializeInternal())
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

bool ZERNStageMultiplexer::DeinitializeInternal()
{
	DirtyFlags.RaiseAll();

	ze_for_each(Display, Displays)
		Display->Deinitialize();

	RenderStateData.Release();

	OutputTexture = NULL;
	OutputRenderTarget = NULL;

	return ZERNStage::DeinitializeInternal();
}

void ZERNStageMultiplexer::CreateOutput(const ZEString& Name)
{
	ZEUInt Width = GetRenderer()->GetOutputTexture()->GetWidth();
	ZEUInt Height = GetRenderer()->GetOutputTexture()->GetHeight();

	if (Name == "ColorTexture")
	{
		if (DirtyFlags.GetFlags(ZERN_SMDF_OUTPUT) || OutputTexture == GetRenderer()->GetOutputTexture())
		{
			OutputTexture = ZEGRTexture::CreateResource(ZEGR_TT_2D, Width, Height, 1, ZEGR_TF_R8G8B8A8_UNORM_SRGB);
			DirtyFlags.UnraiseFlags(ZERN_SMDF_OUTPUT);
		}
	}
}

ZEInt ZERNStageMultiplexer::GetId() const
{
	return ZERN_STAGE_TEXTURE_OUTPUT;
}

const ZEString& ZERNStageMultiplexer::GetName() const
{
	static const ZEString Name = "Stage Multiplexer";
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

	Input->Owner = this;
	Displays.AddEnd(&Input->Link);
	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&Input->InputTexture), Input->GetInput(), ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV);

	if (IsInitialized())
		Input->Initialize();
}

void ZERNStageMultiplexer::RemoveInput(ZERNStageDisplay* Input)
{
	zeCheckError(Input->Owner != this, ZE_VOID, "Multiplexer input is not associated with a stage.");

	Input->Deinitialize();
	Input->Owner = NULL;
	Displays.Remove(&Input->Link);
	RemoveInputResource(Input->GetInput());
}

void ZERNStageMultiplexer::Resized(ZEUInt Width, ZEUInt Height)
{
	DirtyFlags.RaiseFlags(ZERN_SMDF_OUTPUT);
}

bool ZERNStageMultiplexer::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	if (Displays.GetCount() == 0)
		return false;

	OutputRenderTarget = OutputTexture->GetRenderTarget();
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
	DirtyFlags.RaiseAll();

	Mode = ZERN_SMM_SINGLE;

	OutputRenderTarget = NULL;

	AddOutputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&OutputTexture), "ColorTexture", ZERN_SRUT_WRITE, ZERN_SRCF_CREATE_OWN | ZERN_SRCF_GET_OUTPUT);
}

ZERNStageMultiplexer::~ZERNStageMultiplexer()
{
	Deinitialize();

	while (Displays.GetFirst() != NULL)
	{
		ZERNStageDisplay* Input = Displays.GetFirst()->GetItem();
		Displays.Remove(&Input->Link);
		delete Input;
	}
}
