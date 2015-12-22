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
#include "ZERNShaderSlots.h"
#include "ZEGame\ZEEntity.h"
#include "ZEGame\ZEScene.h"
#include "ZEGraphics\ZEGRShader.h"
#include "ZEGraphics\ZEGRRenderState.h"
#include "ZEGraphics\ZEGRGraphicsModule.h"
#include "ZEGraphics\ZEGRContext.h"
#include "ZEGraphics\ZEGRConstantBuffer.h"
#include "ZEGraphics\ZEGRTexture2D.h"
#include "ZEGraphics\ZEGRDepthStencilBuffer.h"
#include "ZEGraphics\ZEGRRenderTarget.h"

namespace
{
	ZEInt CompareCommands(const ZERNCommand* A, const ZERNCommand* B)
	{
		if (A->Priority < B->Priority)
			return -1;
		else if (A->Priority > B->Priority)
			return 1;
		else
			return (int)(A->Order - B->Order);
	}
};

void ZERNRenderer::UpdateViewConstantBuffer()
{
	ZERNViewConstantBuffer* Buffer;
	if (!ViewConstantBuffer->Lock((void**)&Buffer))
		return;

	memset(Buffer, 0, sizeof(ZERNViewConstantBuffer));

	Buffer->ViewTransform = View.ViewTransform;
	Buffer->ProjectionTransform = View.ProjectionTransform;
	Buffer->ViewProjectionTransform = View.ViewProjectionTransform;
	Buffer->InvViewTransform = View.InvViewTransform;
	Buffer->InvProjectionTransform = View.InvProjectionTransform;			
	Buffer->InvViewProjectionTransform = View.InvViewProjectionTransform;

	Buffer->Width = View.Width;
	Buffer->Height = View.Height;
	Buffer->VerticalFOV = View.VerticalFOV;
	Buffer->HorizontalFOV = View.HorizontalFOV;
	Buffer->AspectRatio = View.AspectRatio;
	Buffer->NearZ = View.NearZ;
	Buffer->FarZ = View.FarZ;

	Buffer->Position = View.Position;
	Buffer->RotationQuaternion = View.Rotation;
	ZEQuaternion::ConvertToEulerAngles(
		Buffer->RotationEuler.x,
		Buffer->RotationEuler.y,
		Buffer->RotationEuler.z,
		View.Rotation);
	Buffer->RightVector = View.U;
	Buffer->UpVector = View.V;
	Buffer->FrontVector = View.N;

	ViewConstantBuffer->Unlock();
}

void ZERNRenderer::Cull()
{
	ZERNCullParameters CullParameters;
	CullParameters.Renderer = this;
	CullParameters.View = &View;

	ZESceneCuller Culler;
	Culler.SetScene(Scene);
	Culler.SetCullParameters(CullParameters);
	Culler.Cull();
}

void ZERNRenderer::SortStageQueues()
{
	ZESize Count = StageQueues.GetCount();
	for (ZESize I = 0; I < Count; I++)
		StageQueues[I].Commands.Sort<CompareCommands>();
}

void ZERNRenderer::RenderStages()
{
	if(Context == NULL)
		Context = ZEGRGraphicsModule::GetInstance()->GetMainContext();

	ZERNRenderParameters Parameters;
	Parameters.FrameId = 0;
	Parameters.ElapsedTime = Constants.Elapsedtime;
	Parameters.Time = 0;
	Parameters.Scene = Scene;
	Parameters.Context = Context;
	Parameters.View = &View;
	Parameters.Renderer = this;
	Parameters.Type = ZERN_DT_NORMAL;

	UpdateViewConstantBuffer();

	Context->SetConstantBuffer(ZEGR_ST_ALL, ZERN_SHADER_CONSTANT_VIEW, ViewConstantBuffer);
	Context->SetConstantBuffer(ZEGR_ST_ALL, ZERN_SHADER_CONSTANT_RENDERER, RendererConstantBuffer);

	Context->ClearRenderTarget(OutputRenderTarget, ZEVector4::Zero);

	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, OutputRenderTarget->GetWidth(), OutputRenderTarget->GetHeight()));
	Context->SetRenderTargets(1, &OutputRenderTarget, NULL);

	ZESize Count = StageQueues.GetCount();
	for (ZESize I = 0; I < Count; I++)
	{
		ZERNStageQueue* Queue = &StageQueues[I];
		ZERNStage* Stage = Queue->Stage;

		if (!Stage->GetEnable() || !Stage->Setup(this, Context, Queue->Commands))
			continue;

		Parameters.Stage = Stage;
		ZELink<ZERNCommand>* Link = Queue->Commands.GetFirst();
		while (Link != NULL)
		{
			ZERNCommand* Command = Link->GetItem();
			Parameters.Command = Command;
			Command->Execute(&Parameters);
			Link = Link->GetNext();
		}

		Stage->CleanUp(this, Context);
	}

	CleanCommands();

	Context->SetRenderTargets(0, NULL, NULL);
	Context->SetConstantBuffer(ZEGR_ST_ALL, ZERN_SHADER_CONSTANT_VIEW, NULL);
	Context->SetConstantBuffer(ZEGR_ST_ALL, ZERN_SHADER_CONSTANT_RENDERER, NULL);
}

bool ZERNRenderer::InitializeSelf()
{
	ZESize Count = StageQueues.GetCount();
	for (ZESize I = 0; I < Count; I++)
		if (StageQueues[I].Stage != NULL)
			StageQueues[I].Stage->Initialize();

	ViewConstantBuffer = ZEGRConstantBuffer::Create(sizeof(ZERNViewConstantBuffer));
	RendererConstantBuffer = ZEGRConstantBuffer::Create(sizeof(RendererConstants));

	return true;
}

void ZERNRenderer::DeinitializeSelf()
{
	CleanCommands();
	CleanStages();

	ViewConstantBuffer.Release();
	RendererConstantBuffer.Release();
}

void ZERNRenderer::SetContext(ZEGRContext* Context)
{
	this->Context = Context;
}

ZEGRContext* ZERNRenderer::GetContext()
{
	return Context;
}

void ZERNRenderer::SetView(const ZERNView& View)
{
	this->View = View;
}

const ZERNView& ZERNRenderer::GetView()
{
	return View;
}

void ZERNRenderer::SetScene(ZEScene* Scene)
{
	this->Scene = Scene;
}

ZEScene* ZERNRenderer::GetScene()
{
	return Scene;
}

void ZERNRenderer::SetOutputRenderTarget(ZEGRRenderTarget* OutputRenderTarget)
{
	this->OutputRenderTarget = OutputRenderTarget;
}

ZEGRRenderTarget* ZERNRenderer::GetOutputRenderTarget()
{
	return OutputRenderTarget;
}

ZEArray<ZERNStage*> ZERNRenderer::GetStages()
{
	ZEArray<ZERNStage*> Stages;
	ZESize Count = StageQueues.GetCount();
	Stages.SetCount(Count);

	for (ZESize I = 0; I < Count; I++)
		Stages[I] = StageQueues[I].Stage;

	return Stages;
}

ZERNStage* ZERNRenderer::GetStage(ZERNStageID Id)
{
	ZESize Count = StageQueues.GetCount();
	for (ZESize I = 0; I < Count; I++)
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
	if(Stage == NULL)
		return;

	ZESize Count = StageQueues.GetCount();
	for (ZESize I = 0; I < Count; I++)
	{
		if (StageQueues[I].Stage == Stage)
		{
			StageQueues[I].Stage->Deinitialize();

			StageQueues.Remove(I);
			return;
		}
	}
}

void ZERNRenderer::CleanStages()
{
	ZESize Count = StageQueues.GetCount();
	for (ZESize I = 0; I < Count; I++)
	{
		if (StageQueues[I].Stage != NULL)
			StageQueues[I].Stage->Deinitialize();
	}

	StageQueues.Clear();
}

void ZERNRenderer::AddCommand(ZERNCommand* Command)
{


	ZESize Count = StageQueues.GetCount();
	for (ZESize I = 0; I < Count; I++)
	{
		if ((Command->StageMask & StageQueues[I].Stage->GetId()) != 0)
		{
			for (ZESize J = 0; J < ZERN_MAX_COMMAND_STAGE; J++)
			{
				ZELink<ZERNCommand>* LinkCommand = &Command->StageQueueLinks[J];
				if (!LinkCommand->GetInUse())
				{
					//Hacked by Ceyhun
					char* Chp = (char*)LinkCommand;
					ZEUInt64* Val = (ZEUInt64*)(Chp + 8);
					*Val = (ZEUInt64)Command;

					StageQueues[I].Commands.AddEnd(LinkCommand);
					break;
				}
			}
		}
	}
}

void ZERNRenderer::RemoveCommand(ZERNCommand* Command)
{
	ZESize Count = StageQueues.GetCount();
	for (ZESize I = 0; I < Count; I++)
	{
		for (ZESize N = 0; N < ZERN_MAX_COMMAND_STAGE; N++)
		{
			if (StageQueues[I].Commands.Exists(&Command->StageQueueLinks[N]))
			{
				StageQueues[I].Commands.Remove(&Command->StageQueueLinks[N]);
			}
		}
	}
}

void ZERNRenderer::CleanCommands()
{
	ZESize Count = StageQueues.GetCount();
	for (ZESize I = 0; I < Count; I++)
		StageQueues[I].Commands.Clean();
}

bool ZERNRenderer::ContainsCommand(ZERNCommand* Command)
{
	ZESize Count = StageQueues.GetCount();
	for (ZESize I = 0; I < Count; I++)
	{
		for (ZESize N = 0; N < ZERN_MAX_COMMAND_STAGE; N++)
		{
			if (StageQueues[I].Commands.Exists(&Command->StageQueueLinks[N]))
				return true;
		}
	}

	return false;
}

void ZERNRenderer::Render(float ElapsedTime)
{
	Constants.Elapsedtime = ElapsedTime;
	RendererConstantBuffer->SetData(&Constants);

	Cull();
	SortStageQueues();
	RenderStages();
}

ZERNRenderer::ZERNRenderer()
{
	Context = NULL;
	Scene = NULL;
	OutputRenderTarget = NULL;
}

ZERNRenderer::~ZERNRenderer()
{
	Deinitialize();
}
