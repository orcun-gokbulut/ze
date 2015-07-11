//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNRenderer.cpp
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

#include "ZERNRenderer.h"
#include "ZERNStage.h"

bool ZERNRenderer::InitializeSelf()
{
	for (ZESize I = 0; I < Stages.GetCount(); I++)
		if (!Stages[I]->Initialize())
			return false;

	return true;
}

void ZERNRenderer::DeinitializeSelf()
{
	for (ZESize I = 0; I < Stages.GetCount(); I++)
		Stages[I]->Deinitialize();
}

const ZEArray<ZERNStage*>& ZERNRenderer::GetStages()
{
	return Stages;
}

ZERNStage* ZERNRenderer::GetStage(const char* Name)
{
	for (ZESize I = 0; I < Stages.GetCount(); I++)
		if (Stages[I]->GetName() == Name)
			return Stages[I];

	return NULL;
}

void ZERNRenderer::AddStage(ZERNStage* Stage)
{
	if (Stages.Exists(Stage))
		zeError("Stage is already added.");

	Stages.Add(Stage);
}

void ZERNRenderer::RemoveStage(ZERNStage* Stage)
{
	Stages.RemoveValue(Stage);
}

const ZERendererStatistics& ZERNRenderer::GetStatistics() const
{
	return Statistics;
}

void ZERNRenderer::AddLight(ZELight* Light)
{
	Lights.Add(Light);
}

void ZERNRenderer::ClearLights()
{
	Lights.Clear();
}

void ZERNRenderer::AddCommand(ZERNCommand* Command)
{
	Commands.Add(Command);
}

void ZERNRenderer::ClearCommands()
{
	Commands.Clear();
}

void ZERNRenderer::Render(float ElaspedTime)
{
	for (ZESize I = 0; I < Stages.GetCount(); I++)
	{
		ZERNStage* Stage = Stages[I];
		Stage->Setup(Device);

		for (ZESize N = 0; N < Commands.GetCount(); I++)
			Stage->Render(Commands[I]);

		Stage->CleanUp();
	}
}

void ZERNRenderer::Clear()
{
	ClearLights();
	ClearCommands();
}

ZERNRenderer::ZERNRenderer()
{
	memset(&Statistics, 0, sizeof(ZERendererStatistics));
}

ZERNRenderer::~ZERNRenderer()
{

}
