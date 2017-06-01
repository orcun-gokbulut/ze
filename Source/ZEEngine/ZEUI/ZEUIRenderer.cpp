//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUIRenderer.cpp
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

#include "ZEUIRenderer.h"

#include "ZEMath/ZEMath.h"

#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRSampler.h"
#include "ZEGraphics/ZEGRBuffer.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRVertexLayout.h"
#include "ZEGraphics/ZEGRTexture.h"
#include "ZERenderer/ZERNStageID.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNStage2D.h"


ZEInt CompareRectangle(const ZEUIRectangle* A, const ZEUIRectangle* B)
{
	if(A->ZOrder > B->ZOrder)
	{
		return 1;
	}
	else if(A->ZOrder < B->ZOrder)
	{
		return -1;
	}
	else
	{
		if (A->Texture == B->Texture)
			return 0;
		else if (A->Texture.GetPointer() > B->Texture.GetPointer())
			return 1;
		else
			return -1;
	}
}

ZEUIRendererBatch::ZEUIRendererBatch()
{
	Offset = 0;
	Count = 0;
	Texture = NULL;
	CustomCommand = NULL;
}

void ZEUIRenderer::UpdateBatches()
{
	Rectangles.Sort<CompareRectangle>();

	ZESize VertexBufferSize = Rectangles.GetCount() * 6 * sizeof(ZEUIVertex);
	if (VertexBuffer == NULL ||
		VertexBuffer->GetSize() < VertexBufferSize)
	{
		VertexBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_VERTEX_BUFFER, Rectangles.GetCount() * 6 * sizeof(ZEUIVertex), sizeof(ZEUIVertex), ZEGR_RU_DYNAMIC, ZEGR_RBF_VERTEX_BUFFER);
	}

	Batches.Clear();

	ZEUIVertex* Vertices;
	VertexBuffer->Map(ZEGR_RMT_WRITE_DISCARD, reinterpret_cast<void**>(&Vertices));
	{
		ZEUIRendererBatch* LastBatch = NULL;
		ze_for_each(Rectangle, Rectangles)
		{
			if (LastBatch == NULL || Rectangle->Texture != LastBatch->Texture)
			{
				// New Batch
				ZEUIRendererBatch Batch;
				if (LastBatch != NULL)
					Batch.Offset = LastBatch->Offset + LastBatch->Count;
				else
					Batch.Offset = 0;

				Batch.Count = 0;
				Batch.Texture = Rectangle->Texture;
				Batches.Add(Batch);
				LastBatch = &Batches.GetLastItem();
			}
		
			Rectangle->ConvertToVertices(Vertices);
			Vertices += 6;
			LastBatch->Count += 6;
		}
	}
	VertexBuffer->Unmap();
}

bool ZEUIRenderer::InitializeInternal()
{
	if (!ZEInitializable::InitializeInternal())
		return false;

	ZEGRRenderState RenderState = ZERNStage2D::GetRenderState();

	// Layout
	ZEGRVertexElement ElementArray[] = 
	{
		{ZEGR_VES_POSITION,	0, ZEGR_VET_FLOAT_2, 0, 0,  ZEGR_VU_PER_VERTEX, 0},
		{ZEGR_VES_TEXCOORD,	0, ZEGR_VET_FLOAT_2, 0, 8,  ZEGR_VU_PER_VERTEX, 0},
		{ZEGR_VES_TEXCOORD,	1, ZEGR_VET_FLOAT_4, 0, 16, ZEGR_VU_PER_VERTEX, 0},
	};


	ZEGRVertexLayout VertexLayout;
	VertexLayout.SetElements(ElementArray, 3);
	RenderState.SetVertexLayout(VertexLayout);

	// Shaders
	ZEGRShaderCompileOptions Options;
	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZEUIRenderer.hlsl";
	Options.Model = ZEGR_SM_5_0;
	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZEUIRenderer_VertexShader";
	RenderState.SetShader(ZEGR_ST_VERTEX, ZEGRShader::Compile(Options));
	zeCheckError(RenderState.GetShader(ZEGR_ST_VERTEX) == NULL, false, "Cannot compile vertex shader.");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZEUIRenderer_PixelShader";
	RenderState.SetShader(ZEGR_ST_PIXEL, ZEGRShader::Compile(Options));
	zeCheckError(RenderState.GetShader(ZEGR_ST_PIXEL) == NULL, false, "Cannot compile pixel shader.");

	RenderStateData = RenderState.Compile();
	zeCheckError(RenderStateData == NULL, false, "Cannot compile render state.");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZEUIRenderer_PixelShaderNonTextured";
	RenderState.SetShader(ZEGR_ST_PIXEL, ZEGRShader::Compile(Options));
	zeCheckError(RenderState.GetShader(ZEGR_ST_PIXEL) == NULL, false, "Cannot compile pixel shader.");

	RenderStateDataNonTextured = RenderState.Compile(); 
	zeCheckError(RenderStateData == NULL, false, "Cannot compile render state.");

	// Sampler
	ZEGRSamplerDescription SamplerDescription;
	SamplerDescription.AddressU = ZEGR_TAM_BORDER;
	SamplerDescription.AddressV = ZEGR_TAM_BORDER;
	SamplerDescription.BorderColor = ZEVector4::Zero;
	Sampler = ZEGRSampler::GetSampler(SamplerDescription);

	return true;
}

bool ZEUIRenderer::DeinitializeInternal()
{
	VertexBuffer.Release();
	RenderStateData.Release();
	Sampler.Release();

	return ZEInitializable::DeinitializeInternal();
}

ZEUIRenderer::ZEUIRenderer()
{

}

ZEUIRenderer::~ZEUIRenderer()
{
	Deinitialize();
}

void ZEUIRenderer::AddRectangle(ZEUIRectangle* Rectangle)
{
	Rectangles.Add(*Rectangle);
}

void ZEUIRenderer::AddCustomCommand(ZEUInt ZOrder, ZERNCommand* Command)
{

}

void ZEUIRenderer::Clean()
{
	Rectangles.Clear();
}

void ZEUIRenderer::Setup(ZERNRenderer* Renderer)
{
	if (Rectangles.GetCount() == 0)
		return;

	Command.Priority = ZE_INT_MAX;
	Command.Order = ZE_FLOAT_MAX;
	Command.Callback = ZEDelegateMethod(ZERNCommandCallback, ZEUIRenderer, Render, this);
	Command.StageMask = ZERN_STAGE_2D;

	UpdateBatches();

	//Renderer->AddCommand(&Command);
	Command.Reset();
	Renderer->GetCommandList()->AddCommand(&Command);
}

void ZEUIRenderer::Render(const ZERNRenderParameters* RenderParameters, const ZERNCommand* Command)
{
	if (Rectangles.GetCount() == 0)
		return;

	ZEGRContext* Context = RenderParameters->Context;
	Context->SetRenderState(RenderStateData);
	Context->SetSampler(ZEGR_ST_PIXEL, 0, Sampler);
	Context->SetVertexBuffer(0, VertexBuffer);

	bool LastTextured = false;
	ze_for_each(Batch, Batches)
	{
		if (Batch->Texture != NULL && Batch->Texture->IsLoaded())
		{
			if (!LastTextured)
				Context->SetRenderState(RenderStateData);

			Context->SetTexture(ZEGR_ST_PIXEL, 0, Batch->Texture);
			LastTextured = true;
		}
		else
		{
			Context->SetRenderState(RenderStateDataNonTextured);
			Context->SetTexture(ZEGR_ST_PIXEL, 0, NULL);
			LastTextured = false;
		}

		Context->Draw(Batch->Count, Batch->Offset);
	}

}

void ZEUIRenderer::Destroy()
{
	delete this;
}

ZEUIRenderer* ZEUIRenderer::CreateInstance()
{
	return new ZEUIRenderer();
}
