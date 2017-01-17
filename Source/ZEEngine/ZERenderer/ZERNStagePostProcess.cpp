//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStagePostProcess.cpp
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

#include "ZERNStagePostProcess.h"

#include "ZEGraphics/ZEGRTexture.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZERNRenderer.h"
#include "ZERNStageID.h"
#include "ZERNFilter.h"

#define ZERN_SPDF_OUTPUT	1

bool ZERNStagePostProcess::InitializeInternal()
{
	if (!ZERNStage::InitializeInternal())
		return false;

	ze_for_each(Filter, Filters)
		Filter->Initialize();

	return true;
}

bool ZERNStagePostProcess::DeinitializeInternal()
{
	DirtyFlags.RaiseAll();

	InputTexture = NULL;
	OutputTexture = NULL;

	while (Filters.GetFirst() != NULL)
	{
		ZERNFilter* Filter = Filters.GetFirst()->GetItem();
		RemoveFilter(Filter);
		delete Filter;
	}

	return ZERNStage::DeinitializeInternal();
}

void ZERNStagePostProcess::CreateOutput(const ZEString& Name)
{
	if (Name == "ColorTexture")
	{
		if (DirtyFlags.GetFlags(ZERN_SPDF_OUTPUT) || OutputTexture == GetRenderer()->GetOutputTexture())
		{
			OutputTexture = ZEGRTexture::CreateResource(ZEGR_TT_2D, InputTexture->GetWidth(), InputTexture->GetHeight(), 1, InputTexture->GetFormat()).GetPointer();
			DirtyFlags.UnraiseFlags(ZERN_SPDF_OUTPUT);
		}
	}
}

ZEInt ZERNStagePostProcess::GetId() const
{
	return ZERN_STAGE_POST_EFFECT;
}

const ZEString& ZERNStagePostProcess::GetName() const
{
	static const ZEString Name = "Stage post process";
	return Name;
}

const ZEList2<ZERNFilter>& ZERNStagePostProcess::GetFilters() const
{
	return Filters;
}

ZEArray<ZERNFilter*> ZERNStagePostProcess::GetFilters(ZEClass* Class) const
{
	ZEArray<ZERNFilter*> Output;

	ze_for_each(Filter, Filters)
	{
		if (ZEClass::IsDerivedFrom(Class, Filter->GetClass()))
			Output.Add(Filter.GetPointer());
	}

	return Output;
}

ZERNFilter* ZERNStagePostProcess::GetFilter(ZESize Index) const
{
	if (Index >= Filters.GetCount())
		return NULL;

	return Filters.GetItem(Index);
}

void ZERNStagePostProcess::AddFilter(ZERNFilter* Filter)
{
	zeCheckError(Filter == NULL, ZE_VOID, "Filter cannot be null.");
	//zeCheckError(Stage->GetRenderer() != NULL, ZE_VOID, "Stage is already added to a renderer.");

	Filters.AddEnd(&Filter->Link);
	//Stage->Renderer = this;

	if (IsInitialized())
		Filter->Initialize();
}

void ZERNStagePostProcess::RemoveFilter(ZERNFilter* Filter)
{
	zeCheckError(Filter == NULL, ZE_VOID, "Filter cannot be null.");
	//zeCheckError(Stage->GetRenderer() != this, ZE_VOID, "Stage doesn't belong to this renderer.");

	Filter->Deinitialize();
	//Stage->Renderer = NULL;
	Filters.Remove(&Filter->Link);
}

void ZERNStagePostProcess::Resized(ZEUInt Width, ZEUInt Height)
{
	if (Filters.GetCount() > 0)
	{
		TempTextures.SetCount(Filters.GetCount() - 1);
		ze_for_each(Texture, TempTextures)
			Texture.GetItem() = ZEGRTexture::CreateResource(ZEGR_TT_2D, Width, Height, 1, ZEGR_TF_R8G8B8A8_UNORM_SRGB);
	}

	DirtyFlags.RaiseFlags(ZERN_SPDF_OUTPUT);
}

bool ZERNStagePostProcess::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	if (Filters.GetCount() == 0)
		return false;

	const ZEGRRenderTarget* RenderTarget = OutputTexture->GetRenderTarget();

	Context->SetRenderTargets(1, &RenderTarget, NULL);
	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, (float)OutputTexture->GetWidth(), (float)OutputTexture->GetHeight()));

	if (Filters.GetCount() == 1)
	{
		Filters[0]->SetInputTexture(InputTexture);
		Filters[0]->SetOutputTexture(OutputTexture);
		Filters[0]->Apply(Context);
	}
	else
	{
		for (ZEUInt I = 0; I < (Filters.GetCount() - 1); I++)
		{
			if (I > 0)
				Filters[I]->SetInputTexture(TempTextures[I - 1]);
			else
				Filters[I]->SetInputTexture(InputTexture);

			Filters[I]->SetOutputTexture(TempTextures[I]);
			Filters[I]->Apply(Context);
		}

		Filters[(Filters.GetCount() - 1)]->SetInputTexture(TempTextures.GetLastItem());
		Filters[(Filters.GetCount() - 1)]->SetOutputTexture(OutputTexture);
		Filters[(Filters.GetCount() - 1)]->Apply(Context);
	}

	return true;
}

void ZERNStagePostProcess::CleanUp(ZEGRContext* Context)
{
	ZERNStage::CleanUp(Context);
}

ZERNStagePostProcess::ZERNStagePostProcess()
{
	DirtyFlags.RaiseAll();

	AddInputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&InputTexture), "ColorTexture", ZERN_SRUT_READ, ZERN_SRCF_GET_FROM_PREV | ZERN_SRCF_REQUIRED);

	AddOutputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&OutputTexture), "ColorTexture", ZERN_SRUT_WRITE, ZERN_SRCF_CREATE_OWN | ZERN_SRCF_GET_OUTPUT);
}

ZERNStagePostProcess::~ZERNStagePostProcess()
{
	Deinitialize();
}

ZEGRRenderState ZERNStagePostProcess::GetRenderState()
{
	static ZEGRRenderState RenderState;
	static bool Initialized = false;

	if (!Initialized)
	{
		Initialized = true;

		ZEGRDepthStencilState DepthStencilStateNoTestWrite;
		DepthStencilStateNoTestWrite.SetDepthTestEnable(false);
		DepthStencilStateNoTestWrite.SetDepthWriteEnable(false);

		RenderState.SetDepthStencilState(DepthStencilStateNoTestWrite);

		RenderState.SetDepthStencilFormat(ZEGR_TF_NONE);
		RenderState.SetRenderTargetFormat(0, ZEGR_TF_R8G8B8A8_UNORM_SRGB);
	}

	return RenderState;
}
