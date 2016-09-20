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
#include "ZERNCommand.h"
#include "ZERNInstanceTag.h"
#include "ZERNShaderSlots.h"
#include "ZERNRenderParameters.h"

#include "ZECore\ZECore.h"
#include "ZEGame\ZEScene.h"
#include "ZEGame\ZEEntity.h"
#include "ZEGraphics\ZEGRShader.h"
#include "ZEGraphics\ZEGRContext.h"
#include "ZEGraphics\ZEGRViewport.h"
#include "ZEGraphics\ZEGRTexture2D.h"
#include "ZEGraphics\ZEGRRenderState.h"
#include "ZEGraphics\ZEGRRenderTarget.h"
#include "ZEGraphics\ZEGRGraphicsModule.h"
#include "ZEGraphics\ZEGRConstantBuffer.h"
#include "ZEGraphics\ZEGRDepthStencilBuffer.h"
#include "ZEGraphics\ZEGRSampler.h"

ZEInt CompareCommands(const ZERNCommand* A, const ZERNCommand* B)
{
	if (A->SceneIndex < B->SceneIndex)
	{
		return -1;
	}
	else if (A->SceneIndex > B->SceneIndex)
	{
		return 1;
	}
	else
	{
		if (A->Priority < B->Priority)
			return -1;
		else if (A->Priority > B->Priority)
			return 1;
		else
			return (ZEInt)(A->Order - B->Order);
	}
}

void ZERNRenderer::CreatePredefinedSamplers()
{
	SamplerLinearClamp = ZEGRSampler::GetDefaultSampler();

	ZEGRSamplerDescription SamplerLinearWrapDescription;
	SamplerLinearWrapDescription.AddressU = ZEGR_TAM_WRAP;
	SamplerLinearWrapDescription.AddressV = ZEGR_TAM_WRAP;
	SamplerLinearWrapDescription.AddressW = ZEGR_TAM_WRAP;
	SamplerLinearWrap = ZEGRSampler::GetSampler(SamplerLinearWrapDescription);

	ZEGRSamplerDescription SamplerLinearBorderZeroDescription;
	SamplerLinearBorderZeroDescription.AddressU = ZEGR_TAM_BORDER;
	SamplerLinearBorderZeroDescription.AddressV = ZEGR_TAM_BORDER;
	SamplerLinearBorderZeroDescription.AddressW = ZEGR_TAM_BORDER;
	SamplerLinearBorderZeroDescription.BorderColor = ZEVector4::Zero;
	SamplerLinearBorderZero = ZEGRSampler::GetSampler(SamplerLinearBorderZeroDescription);

	ZEGRSamplerDescription SamplerPointClampDescription;
	SamplerPointClampDescription.MinFilter = ZEGR_TFM_POINT;
	SamplerPointClampDescription.MagFilter = ZEGR_TFM_POINT;
	SamplerPointClampDescription.MipFilter = ZEGR_TFM_POINT;
	SamplerPointClamp = ZEGRSampler::GetSampler(SamplerPointClampDescription);

	ZEGRSamplerDescription SamplerPointWrapDescription;
	SamplerPointWrapDescription.MinFilter = ZEGR_TFM_POINT;
	SamplerPointWrapDescription.MagFilter = ZEGR_TFM_POINT;
	SamplerPointWrapDescription.MipFilter = ZEGR_TFM_POINT;
	SamplerPointWrapDescription.AddressU = ZEGR_TAM_WRAP;
	SamplerPointWrapDescription.AddressV = ZEGR_TAM_WRAP;
	SamplerPointWrapDescription.AddressW = ZEGR_TAM_WRAP;
	SamplerPointWrap = ZEGRSampler::GetSampler(SamplerPointWrapDescription);

	ZEGRSamplerDescription SamplerComparisonLinearPointClampDescription;
	SamplerComparisonLinearPointClampDescription.MinFilter = ZEGR_TFM_LINEAR;
	SamplerComparisonLinearPointClampDescription.MagFilter = ZEGR_TFM_LINEAR;
	SamplerComparisonLinearPointClampDescription.MipFilter = ZEGR_TFM_POINT;
	SamplerComparisonLinearPointClampDescription.AddressU = ZEGR_TAM_CLAMP;
	SamplerComparisonLinearPointClampDescription.AddressV = ZEGR_TAM_CLAMP;
	SamplerComparisonLinearPointClampDescription.AddressW = ZEGR_TAM_CLAMP;
	SamplerComparisonLinearPointClampDescription.ComparisonFunction = ZEGR_CF_GREATER;
	SamplerComparisonLinearPointClamp = ZEGRSampler::GetSampler(SamplerComparisonLinearPointClampDescription);
}

void ZERNRenderer::UpdateConstantBuffers()
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

	Buffer->Width = View.Viewport.GetWidth();
	Buffer->Height = View.Viewport.GetHeight();
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

	Buffer->ShadowDistance = View.ShadowDistance;
	Buffer->ShadowFadeDistance = View.ShadowFadeDistance;

	ViewConstantBuffer->Unlock();

	RendererConstants.Time = ZECore::GetInstance()->GetRuningTime();
	RendererConstants.Elapsedtime = ZECore::GetInstance()->GetElapsedTime();
	RendererConstants.FrameId = (ZEUInt32)ZECore::GetInstance()->GetFrameId();
	if (OutputTexture != NULL)
	{
		RendererConstants.OutputSize = ZEVector2((float)OutputTexture->GetWidth(), (float)OutputTexture->GetHeight());
		RendererConstants.InvOutputSize = ZEVector2::One / RendererConstants.OutputSize;
	}
	else
	{
		RendererConstants.OutputSize = ZEVector2::Zero;
		RendererConstants.InvOutputSize = ZEVector2::Zero;
	}

	ZEMatrix3x3 ScreenTransform;
	ZEMatrix3x3::Create(ScreenTransform,
		2.0f / RendererConstants.OutputSize.x, 0.0f, -1.0f,
		0.0f, -2.0f / RendererConstants.OutputSize.y, 1.0f,
		0.0f, 0.0f, 1.0f);
	RendererConstants.ScreenTransform = ScreenTransform.ToMatrix3x3Shader();
	RendererConstantBuffer->SetData(&RendererConstants);
}

void ZERNRenderer::PopulateStageCommands()
{
	CommandList.MergeEnd(CommandListInstanced);
	CommandList.Sort<CompareCommands>();

	ze_for_each(Command, CommandList)
	{
		ze_for_each(Stage, Stages)
		{
			if ((Command->StageMask & Stage->GetId()) == 0)
				continue;

			Stage->Commands.AddEnd(Command->GetFreeLink());
		}
	}

	CommandList.Clear();
}

void ZERNRenderer::RenderStages()
{
	if (Context == NULL)
		Context = ZEGRGraphicsModule::GetInstance()->GetMainContext();

	ZERNRenderParameters Parameters;
	Parameters.FrameId = 0;
	Parameters.ElapsedTime = RendererConstants.Elapsedtime;
	Parameters.Time = 0;
	Parameters.Context = Context;
	Parameters.View = &View;
	Parameters.Renderer = this;

	UpdateConstantBuffers();

	ZEGRSampler* Samplers[] = {SamplerLinearClamp, SamplerLinearWrap, SamplerLinearBorderZero, SamplerPointClamp, SamplerPointWrap, SamplerComparisonLinearPointClamp};
	Context->SetSamplers(ZEGR_ST_PIXEL, 10, 6, Samplers);

	ZEGRConstantBuffer* PrevRendererConstantBuffer;
	ZEGRConstantBuffer* PrevViewConstantBuffer;
	ZEGRConstantBuffer* PrevSceneConstantBuffer;
	Context->GetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_RENDERER, &PrevRendererConstantBuffer);
	Context->GetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_VIEW, &PrevViewConstantBuffer);
	Context->GetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_SCENE, &PrevSceneConstantBuffer);

	ZEGRConstantBuffer* ConstantBuffers[] = {RendererConstantBuffer, ViewConstantBuffer};
	Context->SetConstantBuffers(ZEGR_ST_ALL, ZERN_SHADER_CONSTANT_RENDERER, 2, ConstantBuffers);

	if (OutputTexture != NULL && OutputTexture->GetResourceBindFlags().GetFlags(ZEGR_RBF_RENDER_TARGET))
		Context->ClearRenderTarget(OutputTexture->GetRenderTarget(), ZEVector4::Zero);

	if (Resized)
	{
		ze_for_each(Stage, Stages)
			Stage->Resized(OutputTexture->GetWidth(), OutputTexture->GetHeight());
	}

	if (DirtyPipeline || Resized)
	{
		DirtyPipeline = false;
		Resized = false;

		BindStages();
	}

	ze_for_each(Stage, Stages)
	{
		if (!Stage->GetEnabled())
			continue;

		if (!Stage->Setup(Context))
			continue;

		Parameters.Stage = Stage.GetPointer();
		ZEInt LastSceneIndex = -1;
		ze_for_each(Command, Stage->Commands)
		{
			if (Command->SceneIndex != LastSceneIndex)
			{
				Context->SetConstantBuffer(ZEGR_ST_ALL, ZERN_SHADER_CONSTANT_SCENE, SceneConstants[Command->SceneIndex]);
				LastSceneIndex = Command->SceneIndex;
			}

			Parameters.Command = Command.GetPointer();
			Command->Execute(&Parameters);
		}

		Stage->CleanUp(Context);
	}

	ZEGRConstantBuffer* PrevConstantBuffers[] = {PrevRendererConstantBuffer, PrevViewConstantBuffer, PrevSceneConstantBuffer};
	Context->SetConstantBuffers(ZEGR_ST_ALL, ZERN_SHADER_CONSTANT_RENDERER, 3, PrevConstantBuffers);
}

bool ZERNRenderer::InitializeInternal()
{
	if (!ZEInitializable::InitializeInternal())
		return false;

	ze_for_each(Stage, Stages)
	{
		if (!Stage->Initialize())
		{
			zeError("Cannot initialize stage. Stage Name: \"%s\"", Stage->GetName().ToCString());
			return false;
		}
	}

	ViewConstantBuffer = ZEGRConstantBuffer::CreateResource(sizeof(ZERNViewConstantBuffer));
	RendererConstantBuffer = ZEGRConstantBuffer::CreateResource(sizeof(RendererConstants));

	CreatePredefinedSamplers();

	return true;
}

bool ZERNRenderer::DeinitializeInternal()
{
	CleanCommands();
	CleanStages();

	ViewConstantBuffer.Release();
	RendererConstantBuffer.Release();

	Context = NULL;
	OutputTexture = NULL;

	return ZEInitializable::DeinitializeInternal();
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

void ZERNRenderer::SetOutputTexture(const ZEGRTexture2D* OutputTexture)
{
	if (this->OutputTexture == NULL || 
		this->OutputTexture->GetWidth() != OutputTexture->GetWidth() || 
		this->OutputTexture->GetHeight() != OutputTexture->GetHeight())
		Resized = true;

	this->OutputTexture = OutputTexture;
}

const ZEGRTexture2D* ZERNRenderer::GetOutputTexture() const
{
	return OutputTexture;
}

const ZEList2<ZERNStage>& ZERNRenderer::GetStages()
{
	return Stages;
}

ZERNStage* ZERNRenderer::GetStage(ZEClass* Class)
{
	ze_for_each(Stage, Stages)
	{
		if (Stage->GetClass() == Class)
			return Stage.GetPointer();
	}

	return NULL;
}

ZERNStage* ZERNRenderer::GetStage(ZERNStageID Id)
{
	ze_for_each(Stage, Stages)
	{
		if (Stage->GetId() == Id)
			return Stage.GetPointer();
	}

	return NULL;
}

void ZERNRenderer::AddStage(ZERNStage* Stage)
{
	zeCheckError(Stage == NULL, ZE_VOID, "Stage cannot be null.");
	zeCheckError(Stage->GetRenderer() != NULL, ZE_VOID, "Stage is already added to a renderer.");

	Stages.AddEnd(&Stage->Link);
	Stage->Renderer = this;

	if (IsInitialized())
		Stage->Initialize();
}

void ZERNRenderer::RemoveStage(ZERNStage* Stage)
{
	zeCheckError(Stage == NULL, ZE_VOID, "Stage cannot be null.");
	zeCheckError(Stage->GetRenderer() != this, ZE_VOID, "Stage doesn't belong to this renderer.");

	Stage->Deinitialize();
	Stage->Renderer = NULL;
	Stages.Remove(&Stage->Link);
}

void ZERNRenderer::CleanStages()
{
	while (Stages.GetFirst() != NULL)
	{
		ZERNStage* Stage = Stages.GetFirst()->GetItem();
		RemoveStage(Stage);
		delete Stage;
	}
}

void ZERNRenderer::MarkDirtyPipeline()
{
	DirtyPipeline = true;
}

void ZERNRenderer::BindStages()
{
	ze_for_each(Stage, Stages)
	{
		if (!Stage->GetEnabled())
			continue;

		ze_for_each(InputResource, Stage->InputResources)
		{
			*InputResource->Resource = NULL;

			if (InputResource->CreationFlags.GetFlags(ZERN_SRCF_GET_FROM_PREV))
			{
				for (auto PrevStage = Stage->Link.GetPrev()->GetIteratorConst(); PrevStage.IsValid(); --PrevStage)
				{
					if (!PrevStage->GetEnabled())
						continue;

					ze_for_each(PrevStageOutputResource, PrevStage->OutputResources)
					{
						if (PrevStageOutputResource->Name == InputResource->Name)
						{
							*InputResource->Resource = *PrevStageOutputResource->Resource;
							break;
						}
					}

					if ((*InputResource->Resource) != NULL)
						break;
				}
			}

			if ((*InputResource->Resource) == NULL && InputResource->CreationFlags.GetFlags(ZERN_SRCF_REQUIRED))
				zeError("Binding input resource failed. Required input resource of a stage cannot be null. Input resource name: %s, Stage: %s", InputResource->Name.ToCString(), Stage->GetName().ToCString());
		}

		ze_for_each(OutputResource, Stage->OutputResources)
		{
			bool Valid = false;

			if (OutputResource->CreationFlags.GetFlags(ZERN_SRCF_GET_FROM_PREV))
			{
				for (auto PrevStage = Stage->Link.GetPrev()->GetIteratorConst(); PrevStage.IsValid(); --PrevStage)
				{
					if (!PrevStage->GetEnabled())
						continue;

					ze_for_each(PrevStageOutputResource, PrevStage->OutputResources)
					{
						if (PrevStageOutputResource->Name == OutputResource->Name)
						{
							if (*PrevStageOutputResource->Resource != NULL)
							{
								*OutputResource->Resource = *PrevStageOutputResource->Resource;
								Valid = true;
							}

							break;
						}
					}

					if (Valid)
						break;
				}
			}

			if (!Valid && OutputResource->CreationFlags.GetFlags(ZERN_SRCF_CREATE_OWN))
			{
				for (auto NextStage = Stage->Link.GetNext()->GetIteratorConst(); NextStage.IsValid(); ++NextStage)
				{
					if (!NextStage->GetEnabled())
						continue;

					ze_for_each(NextStageInputResource, NextStage->InputResources)
					{
						if (NextStageInputResource->Name == OutputResource->Name)
						{
							if (NextStageInputResource->Usage == ZERN_SRUT_READ)
							{
								Stage->CreateOutput(OutputResource->Name);
								Valid = true;
							}

							break;
						}
					}

					if (Valid)
						break;
				}
			}

			if (!Valid && OutputResource->CreationFlags.GetFlags(ZERN_SRCF_GET_OUTPUT))
			{
				*OutputResource->Resource = this->OutputTexture;
				Valid = true;
			}

			if (!Valid && OutputResource->CreationFlags.GetFlags(ZERN_SRCF_REQUIRED))
				zeError("Binding output resource failed. Output resource of a stage cannot be null. Output resource name: %s, Stage: %s", OutputResource->Name.ToCString(), Stage->GetName().ToCString());

		}
	}
}

void ZERNRenderer::StartScene(const ZEGRConstantBuffer* ConstantBuffer)
{
	SceneConstants.Add(ConstantBuffer);
}

void ZERNRenderer::EndScene()
{

}

void ZERNRenderer::AddCommand(ZERNCommand* Command)
{
	if (!IsInitialized())
		return;
	
	Command->InstancesPrevious.Clear();
	Command->InstancesPrevious.MergeBegin(Command->Instances);

	Command->SceneIndex = (ZEInt)SceneConstants.GetCount() - 1;
	zeBreak(Command->SceneIndex < 0 || Command->SceneIndex > 100);

	if (Command->InstanceTag != NULL)
	{
		bool Found = false;
		ze_for_each(CurrentCommand, CommandListInstanced)
		{
			if (CurrentCommand->InstanceTag == NULL ||
				!CurrentCommand->InstanceTag->Check(Command->InstanceTag))
			{
				continue;
			}
			
			CurrentCommand->Instances.AddEnd(Command->GetFreeLink());			
			Found = true;
			break;
		}

		if (!Found)
			CommandListInstanced.AddEnd(Command->GetFreeLink());

	}
	else
	{
		CommandList.AddEnd(Command->GetFreeLink());
	}
}

void ZERNRenderer::CleanCommands()
{
	ze_for_each(Command, CommandList)
		Command->PopInstances();

	CommandList.Clear();
	CommandListInstanced.Clear();
	SceneConstants.Clear();

	ze_for_each(Stage, Stages)
		Stage->Commands.Clear();
}

void ZERNRenderer::Render()
{
	if (!IsInitialized())
		return;

	PopulateStageCommands();
	RenderStages();
	CleanCommands();
}

ZERNRenderer::ZERNRenderer()
{
	Context = NULL;

	DirtyPipeline = false;
	Resized = false;
}

ZERNRenderer::~ZERNRenderer()
{
	CleanCommands();
	CleanStages();
}
