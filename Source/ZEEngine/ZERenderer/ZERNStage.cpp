//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStage.cpp
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

#include "ZERNStage.h"

#include "ZERNRenderer.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRRenderTarget.h"

const ZEArray<ZERNStageResource>& ZERNStage::GetInputResources() const
{
	return InputResources;
}

const ZEArray<ZERNStageResource>& ZERNStage::GetOutputResources() const
{
	return OutputResources;
}

void ZERNStage::AddInputResource(ZEHolder<const ZEGRResource>* ResourcePointer, const ZEString& Name, ZERNStageResourceUsageType Usage, ZEFlags CreationFlags)
{
	ZERNStageResource StageResource;
	StageResource.Resource = ResourcePointer;
	StageResource.Name = Name;
	StageResource.Usage = Usage;
	StageResource.CreationFlags = CreationFlags;

	InputResources.AddByRef(StageResource);

	if (Renderer != NULL)
		Renderer->MarkDirtyPipeline();
}

void ZERNStage::AddOutputResource(ZEHolder<const ZEGRResource>* ResourcePointer, const ZEString& Name, ZERNStageResourceUsageType Usage, ZEFlags CreationFlags)
{
	ZERNStageResource StageResource;
	StageResource.Resource = ResourcePointer;
	StageResource.Name = Name;
	StageResource.Usage = Usage;
	StageResource.CreationFlags = CreationFlags;

	OutputResources.AddByRef(StageResource);

	if (Renderer != NULL)
		Renderer->MarkDirtyPipeline();
}

void ZERNStage::RemoveInputResource(const ZEString& Name)
{
	ze_for_each(InputResource, InputResources)
	{
		if (InputResource->Name == Name)
		{
			InputResource->Resource->Release();
			InputResources.Remove(InputResource.GetIndex());
			break;
		}
	}

	if (Renderer != NULL)
		Renderer->MarkDirtyPipeline();
}

void ZERNStage::RemoveOutputResource(const ZEString& Name)
{
	ze_for_each(OutputResource, OutputResources)
	{
		if (OutputResource->Name == Name)
		{
			OutputResource->Resource->Release();
			OutputResources.Remove(OutputResource.GetIndex());
			break;
		}
	}

	if (Renderer != NULL)
		Renderer->MarkDirtyPipeline();
}

void ZERNStage::CreateOutput(const ZEString& Name)
{

}

ZERNRenderer* ZERNStage::GetRenderer() const
{
	return Renderer;
}

void ZERNStage::SetEnabled(bool Enabled)
{
	if (this->Enabled == Enabled)
		return;

	this->Enabled = Enabled;

	if (Renderer != NULL)
		Renderer->MarkDirtyPipeline();
}

bool ZERNStage::GetEnabled() const
{
	return Enabled;
}

const ZEList2<ZERNCommand>& ZERNStage::GetCommands() const
{
	return Commands;
}

const ZEGRResource* ZERNStage::GetInput(const ZEString& ResourceName) const
{
	ze_for_each(InputResource, GetInputResources())
	{
		if (InputResource->Name == ResourceName)
			return (*InputResource->Resource);
	}

	return NULL;
}

const ZEGRResource* ZERNStage::GetOutput(const ZEString& ResourceName) const
{
	ze_for_each(OutputResource, GetOutputResources())
	{
		if (OutputResource->Name == ResourceName)
			return (*OutputResource->Resource);
	}

	return NULL;
}

void ZERNStage::Resized(ZEUInt Width, ZEUInt Height)
{

}

bool ZERNStage::Setup(ZEGRContext* Context)
{
	zeCheckError(Renderer == NULL, false, "Stage process failed. Owner renderer of the stage cannot be NULL. Stage: \"%s\"", GetName().ToCString());
	zeCheckError(Context == NULL, false, "Stage process failed. Parameter context cannot be NULL. Stage: \"%s\"", GetName().ToCString());

	return true;
}

void ZERNStage::CleanUp(ZEGRContext* Context)
{

}

ZERNStage::ZERNStage() : Link(this)
{
	Renderer = NULL;
	Enabled = true;
}

ZEGRRenderState ZERNStage::GetRenderState()
{
	return ZEGRRenderState::Default;
}
