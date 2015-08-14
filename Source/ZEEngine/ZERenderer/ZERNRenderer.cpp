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

#include "ZERNCuller.h"

#include "ZERNRenderer.h"
#include "ZERNStage.h"
#include "ZEGame\ZEEntity.h"
#include "ZEGame\ZEScene.h"
#include "ZERNCommand.h"

static inline ZEInt CompareCommands(const ZERNCommand* A, const ZERNCommand* B)
{
	if (A->Priority < B->Priority)
		return -1;
	else if (A->Priority > B->Priority)
		return 1;
	else
		return (int)(A->Order - B->Order);
}

void ZERNRenderer::Cull()
{
	ZESceneCuller Culler;
	ZERNCullParameters CullParameters;
	CullParameters.Renderer = this;
	CullParameters.View = &View;

	Culler.SetScene(Scene);
	Culler.Cull();
}

void ZERNRenderer::SortStageQueues()
{
	for (ZESize I = 0; I < StageQueues.GetCount(); I++)
		StageQueues[I].Commands.Sort<CompareCommands>();
}

void ZERNRenderer::RenderStages()
{
	for (ZESize I = 0; I < StageQueues.GetCount(); I++)
	{
		ZERNStageQueue* Queue = &StageQueues[I];
		ZELink<ZERNCommand>* Link = StageQueues[I].Commands.GetFirst();
		Queue->Stage->Setup(Device);
		while (Link != NULL)
		{
			ZERNCommand* Command = Link->GetItem();
			Command->Execute(NULL);
		}
		Queue->Stage->CleanUp();
	}
}

bool ZERNRenderer::InitializeSelf()
{
	for (ZESize I = 0; I < StageQueues.GetCount(); I++)
		if (!StageQueues[I].Stage != NULL)
			StageQueues[I].Stage->Initialize();

	return true;
}

void ZERNRenderer::DeinitializeSelf()
{
	for (ZESize I = 0; I < StageQueues.GetCount(); I++)
		if (StageQueues[I].Stage != NULL)
			StageQueues[I].Stage->Deinitialize();
}

void ZERNRenderer::SetScene(ZEScene* Scene)
{
	this->Scene = Scene;
}

ZEScene* ZERNRenderer::GetScene()
{
	return Scene;
}

void ZERNRenderer::AddStage(ZERNStage* Stage)
{
	ZERNStageQueue Queue;
	Queue.Stage = Stage;
	StageQueues.Add(Queue);

	if (IsInitialized())
		Stage->Initialize();
}

void ZERNRenderer::RemoveStage(ZERNStage* Stage)
{
	for (ZESize I = 0; I < StageQueues.GetCount(); I++)
	{
		if (StageQueues[I].Stage == Stage)
		{
			if (StageQueues[I].Stage != NULL)
				StageQueues[I].Stage->Deinitialize();

			StageQueues.Remove(I);
		}
	}
}

void ZERNRenderer::AddCommand(ZERNCommand* Command)
{
	ZELink<ZERNCommand>* EmptyLink = NULL;
	for (ZESize I = 0; I < ZERN_MAX_COMMAND_STAGE; I++)
		EmptyLink = &Command->StageQueueLinks[I];

	if (EmptyLink == NULL)
		return;

	for (ZESize I = 0; I < StageQueues.GetCount(); I++)
		if ((Command->StageMask & StageQueues[I].Stage->GetId()) != 0)
			StageQueues[I].Commands.AddEnd(EmptyLink);
}

void ZERNRenderer::RemoveCommand(ZERNCommand* Command)
{
	for (ZESize I = 0; I < StageQueues.GetCount(); I++)
	{
		for (ZESize N = 0; N < ZERN_MAX_COMMAND_STAGE; I++)
		{
			if (StageQueues[I].Commands.Exists(&Command->StageQueueLinks[N]))
				StageQueues[I].Commands.Remove(&Command->StageQueueLinks[N]);
			break;
		}
	}
}

void ZERNRenderer::CleanCommands()
{
	for (ZESize I = 0; I < StageQueues.GetCount(); I++)
		StageQueues[I].Commands.Clean();
}

void ZERNRenderer::Render()
{
	Cull();
	SortStageQueues();
	RenderStages();
}

ZERNRenderer::ZERNRenderer()
{
	Device = NULL;
	Scene = NULL;
}

ZERNRenderer::~ZERNRenderer()
{

}
