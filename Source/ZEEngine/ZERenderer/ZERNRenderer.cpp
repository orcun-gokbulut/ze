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
#include "ZERNCommand.h"
#include "ZERNRenderParameters.h"
#include "ZERNView.h"
#include "ZERNShaderSlots.h"
#include "ZEGame\ZEEntity.h"
#include "ZEGame\ZEScene.h"
#include "ZEGraphics\ZEGRGraphicsModule.h"
#include "ZEGraphics\ZEGRContext.h"
#include "ZEGraphics\ZEGRConstantBuffer.h"

static inline ZEInt CompareCommands(const ZERNCommand* A, const ZERNCommand* B)
{
	if (A->Priority < B->Priority)
		return -1;
	else if (A->Priority > B->Priority)
		return 1;
	else
		return (int)(A->Order - B->Order);
}

void ZERNRenderer::UpdateViewConstantBuffer()
{
	ZERNViewConstantBuffer* Buffer;
	if (!ViewConstantBuffer->Lock((void**)&Buffer))
		return;

	memset(Buffer, 0, sizeof(ZERNViewConstantBuffer));

	Buffer->ViewTransform = View.ViewTransform;
	Buffer->ProjectionTransform = View.ProjectionTransform;
	Buffer->ViewProjectionTransform = View.ProjectionTransform;
	Buffer->InvViewTransform = View.InvViewTransform;
	Buffer->InvProjectionTransform = View.InvProjectionTransform;			
	Buffer->InvViewProjectionTransform = View.InvProjectionTransform;

	Buffer->Width = View.Width;
	Buffer->Height = View.Height;
	Buffer->VerticalFOV = View.VerticalFOV;
	Buffer->HorizontalFOV = View.HorizontalFOV;
	Buffer->AspectRatio = View.AspectRatio;
	Buffer->NearZ = View.NearZ;
	Buffer->FarZ = View.FarZ;
	Buffer->Reserved0 = 0.0f;

	Buffer->Position = View.Position;
	Buffer->RotationQuaternion = View.Rotation;
	ZEQuaternion::ConvertToEulerAngles(
		Buffer->RotationEuler.x,
		Buffer->RotationEuler.y,
		Buffer->RotationEuler.z, 
		View.Rotation);
	Buffer->U = View.U;
	Buffer->V = View.V;
	Buffer->N = View.N;

	ViewConstantBuffer->Unlock();
}

void ZERNRenderer::Cull()
{
	ZESceneCuller Culler;
	ZERNCullParameters CullParameters;
	CullParameters.Renderer = this;
	CullParameters.View = &View;

	Culler.SetScene(Scene);
	Culler.SetCullParameters(CullParameters);
	Culler.Cull();
}

void ZERNRenderer::SortStageQueues()
{
	for (ZESize I = 0; I < StageQueues.GetCount(); I++)
		StageQueues[I].Commands.Sort<CompareCommands>();
}

void ZERNRenderer::RenderStages()
{
	ZEGRContext* Context = ZEGRGraphicsModule::GetInstance()->GetMainContext();

	ZERNRenderParameters Parameters;
	Parameters.FrameId = 0;
	Parameters.ElapsedTime = 0;
	Parameters.Time = 0;
	Parameters.Scene = Scene;
	Parameters.Context = Context;
	Parameters.View = &View;
	Parameters.Renderer = this;
	Parameters.Type = ZERN_DT_NORMAL;
	Parameters.Command;

	UpdateViewConstantBuffer();

	//Context->SetConstantBuffer(ZEGR_ST_ALL, 0, GetConstantBuffer());
	//Context->SetConstantBuffer(ZEGR_ST_ALL, ZERN_SHADER_CONSTANT_VIEW, Scene->GetConstantBuffer());
	Context->SetConstantBuffer(ZEGR_ST_ALL, ZERN_SHADER_CONSTANT_VIEW, ViewConstantBuffer);

	for (ZESize I = 0; I < StageQueues.GetCount(); I++)
	{
		ZERNStageQueue* Queue = &StageQueues[I];
		ZELink<ZERNCommand>* Link = StageQueues[I].Commands.GetFirst();

		Parameters.StageID =  Queue->Stage->GetId();
		Parameters.Stage = Queue->Stage;
	
		if (!Queue->Stage->Setup(this, Context, StageQueues[I].Commands))
			continue;

		while (Link != NULL)
		{
			ZERNCommand* Command = Link->GetItem();
			Parameters.Command = Command;
			Command->Execute(&Parameters);
			Link = Link->GetNext();
		}

		Queue->Stage->CleanUp(this, Context);
	}
}

bool ZERNRenderer::InitializeSelf()
{
	for (ZESize I = 0; I < StageQueues.GetCount(); I++)
		if (!StageQueues[I].Stage != NULL)
			StageQueues[I].Stage->Initialize();

	ViewConstantBuffer = ZEGRConstantBuffer::Create(sizeof(ZERNViewConstantBuffer));
	return true;
}

void ZERNRenderer::DeinitializeSelf()
{
	for (ZESize I = 0; I < StageQueues.GetCount(); I++)
		if (StageQueues[I].Stage != NULL)
			StageQueues[I].Stage->Deinitialize();
	
	ViewConstantBuffer.Release();
}

void ZERNRenderer::SetDevice(ZEGRContext* Device)
{
	this->Device = Device;
}

ZEGRContext* ZERNRenderer::GetDevice()
{
	return Device;
}

void ZERNRenderer::SetView(const ZERNView& View)
{
	this->View = View;
}

const ZERNView& ZERNRenderer::GetView()
{
	return View;
}

void ZERNRenderer::SetOutput(ZEGROutput* Output)
{
	this->Output = Output;
}

ZEGROutput* ZERNRenderer::GetOutput()
{
	return Output;
}

void ZERNRenderer::SetScene(ZEScene* Scene)
{
	this->Scene = Scene;
}

ZEScene* ZERNRenderer::GetScene()
{
	return Scene;
}

ZEArray<ZERNStage*> ZERNRenderer::GetStages()
{
	ZEArray<ZERNStage*> Stages;
	Stages.SetCount(StageQueues.GetCount());

	for (ZESize I = 0; I < Stages.GetCount(); I++)
		Stages[I] = StageQueues[I].Stage;

	return Stages;
}

ZERNStage* ZERNRenderer::GetStage(ZERNStageID Id)
{
	for (ZESize I = 0; I < StageQueues.GetCount(); I++)
		if (StageQueues[I].Stage->GetId() == Id)
			return StageQueues[I].Stage;

	return NULL;
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
	{
		if (!Command->StageQueueLinks[I].GetInUse())
		{
			EmptyLink = &Command->StageQueueLinks[I];
			break;
		}
	}

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
