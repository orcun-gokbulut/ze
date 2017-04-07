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

#include "ZECore/ZECore.h"
#include "ZECore/ZETimeManager.h"
#include "ZEGame/ZEScene.h"
#include "ZEGame/ZEEntity.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRBuffer.h"
#include "ZEGraphics/ZEGRTexture.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"
#include "ZEGraphics/ZEGRSampler.h"
#include "ZERNStageAntiAliasing.h"

ZEHolder<ZEGRBuffer> InstanceVertexBuffer;

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
	if (ViewConstantBuffer == NULL)
		ViewConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, sizeof(ZERNViewConstantBuffer), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);
	
	if (RendererConstantBuffer == NULL)
		RendererConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, sizeof(RendererConstants), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);
	
	ZERNViewConstantBuffer* Buffer;
	if (!ViewConstantBuffer->Map(ZEGR_RMT_WRITE_DISCARD, reinterpret_cast<void**>(&Buffer)))
		return;

	memset(Buffer, 0, sizeof(ZERNViewConstantBuffer));

	Buffer->ViewTransform = View.ViewTransform;
	Buffer->ProjectionTransform = View.ProjectionTransform;
	Buffer->InvViewTransform = View.InvViewTransform;
	Buffer->InvProjectionTransform = View.InvProjectionTransform;
	Buffer->InvViewProjectionTransform = View.InvViewProjectionTransform;
	Buffer->ViewProjectionTransform = View.ViewProjectionTransform;
	Buffer->PrevViewProjectionTransform = PrevViewProjectionTransform;

	PrevViewProjectionTransform = View.ViewProjectionTransform;

	ZETimeManager* TimeManager = ZECore::GetInstance()->GetTimeManager();

	ZERNStageAntiAliasing* StageAA = static_cast<ZERNStageAntiAliasing*>(GetStage(ZERN_STAGE_ANTI_ALIASING));
	if (OutputTexture != NULL && StageAA != NULL && StageAA->GetEnabled() && StageAA->GetTemporalEnabled())
	{
		ZEVector2 Jitters[] = 
		{
			ZEVector2(0.25f, -0.25f),
			ZEVector2(-0.25f, 0.25f)
		};

		ZEMatrix4x4 TranslationMatrix;
		ZEMatrix4x4::CreateTranslation(TranslationMatrix, 
			2.0f * Jitters[TimeManager->GetFrameId() % 2].x / (float)OutputTexture->GetWidth(), 
			2.0f * Jitters[TimeManager->GetFrameId() % 2].y / (float)OutputTexture->GetHeight(), 
			0.0f);
		Buffer->ViewProjectionTransform = TranslationMatrix * Buffer->ViewProjectionTransform;
		Buffer->PrevViewProjectionTransform = TranslationMatrix * Buffer->PrevViewProjectionTransform;
	}

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

	ViewConstantBuffer->Unmap();

	RendererConstants.Time = TimeManager->GetFrameTime();
	RendererConstants.Elapsedtime = TimeManager->GetFrameTimeDelta();
	RendererConstants.FrameId = (ZEUInt32)TimeManager->GetFrameId();
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
		Command->Instances.Sort<CompareCommands>();
		ze_for_each(Stage, Stages)
		{
			if ((Command->StageMask & Stage->GetId()) == 0)
				continue;

			Stage->Commands.AddEnd(Command->GetFreeLink());
		}
	}
}

void ZERNRenderer::RenderStages()
{
	if (Context == NULL)
		Context = ZEGRGraphicsModule::GetInstance()->GetMainContext();

	ZERNRenderParameters Parameters;
	Parameters.UpdateTime();
	Parameters.Context = Context;
	Parameters.View = &View;
	Parameters.Renderer = this;
	Parameters.Type = (GetStage(ZERN_STAGE_SHADOW_MAP_GENERATION) != NULL) ? ZERN_RT_SHADOW : ZERN_RT_COLOR;

	UpdateConstantBuffers();

	ZEGRSampler* Samplers[] = {SamplerLinearClamp, SamplerLinearWrap, SamplerLinearBorderZero, SamplerPointClamp, SamplerPointWrap, SamplerComparisonLinearPointClamp};
	Context->SetSamplers(ZEGR_ST_PIXEL, 10, 6, Samplers);

	ZEGRBuffer* PrevConstantBuffers[3] = {};
	Context->GetConstantBuffers(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_RENDERER, 3, PrevConstantBuffers);

	ZEGRBuffer* ConstantBuffers[] = {RendererConstantBuffer, ViewConstantBuffer};
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
		{
			Stage->CleanUp(Context);
			continue;
		}

		Parameters.Stage = Stage.GetPointer();
		ZEInt LastSceneIndex = -1;
		ze_for_each(Command, Stage->Commands)
		{
			if (Command->SceneIndex != LastSceneIndex)
			{
				Context->SetConstantBuffer(ZEGR_ST_ALL, ZERN_SHADER_CONSTANT_SCENE, Scenes[Command->SceneIndex]->GetConstantBuffer());
				LastSceneIndex = Command->SceneIndex;
			}

			Parameters.Command = Command.GetPointer();
			Command->Execute(&Parameters);
		}

		Stage->CleanUp(Context);
	}

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
	
	if (::InstanceVertexBuffer == NULL)
		this->InstanceVertexBuffer = ::InstanceVertexBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_VERTEX_BUFFER, sizeof(ZERNInstanceData) * 8192, sizeof(ZERNInstanceData), ZEGR_RU_DYNAMIC, ZEGR_RBF_VERTEX_BUFFER);
	else
		this->InstanceVertexBuffer = ::InstanceVertexBuffer;

	CreatePredefinedSamplers();

	return true;
}

bool ZERNRenderer::DeinitializeInternal()
{
	CleanCommands();
	CleanStages();

	ViewConstantBuffer.Release();
	RendererConstantBuffer.Release();
	InstanceVertexBuffer.Release();

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

void ZERNRenderer::SetOutputTexture(const ZEGRTexture* OutputTexture)
{
	if (this->OutputTexture == NULL || 
		this->OutputTexture != OutputTexture ||
		this->OutputTexture->GetWidth() != OutputTexture->GetWidth() || 
		this->OutputTexture->GetHeight() != OutputTexture->GetHeight())
		Resized = true;

	this->OutputTexture = OutputTexture;
}

const ZEGRTexture* ZERNRenderer::GetOutputTexture() const
{
	return OutputTexture;
}

void ZERNRenderer::SetMSAASampleCount(ZEUInt SampleCount)
{
	if (IsInitialized())
		return;

	this->MSAASampleCount = SampleCount;
	ZEGRGraphicsModule::SAMPLE_COUNT = SampleCount;
}

ZEUInt ZERNRenderer::GetMSAASampleCount() const
{
	return MSAASampleCount;
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
			if (InputResource->Usage == ZERN_SRUT_NONE)
				continue;

			*InputResource->Resource = NULL;

			if (InputResource->CreationFlags.GetFlags(ZERN_SRCF_GET_FROM_PREV))
			{
				for (auto PrevStage = Stages.GetIterator(Stage->Link.GetPrev()); PrevStage.IsValid(); --PrevStage)
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

					if ((*InputResource->Resource) == NULL)
					{
						ze_for_each(PrevStageInputResource, PrevStage->InputResources)
						{
							if (PrevStageInputResource->Name == InputResource->Name)
							{
								*InputResource->Resource = *PrevStageInputResource->Resource;
								break;
							}
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
				for (auto PrevStage = Stages.GetIterator(Stage->Link.GetPrev()); PrevStage.IsValid(); --PrevStage)
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
				if (OutputResource->CreationFlags.GetFlags(ZERN_SRCF_REQUIRED))
				{
					Stage->CreateOutput(OutputResource->Name);
					Valid = true;
				}
				else
				{
					for (auto NextStage = Stages.GetIterator(Stage->Link.GetNext()); NextStage.IsValid(); ++NextStage)
					{
						if (!NextStage->GetEnabled())
							continue;

						ze_for_each(NextStageInputResource, NextStage->InputResources)
						{
							if (NextStageInputResource->Name == OutputResource->Name)
							{
								if (NextStageInputResource->CreationFlags.GetFlags(ZERN_SRCF_GET_FROM_PREV))
								{
									Stage->CreateOutput(OutputResource->Name);
									Valid = true;

									break;
								}
							}
						}

						if (!Valid)
						{
							ze_for_each(NextStageOutputResource, NextStage->OutputResources)
							{
								if (NextStageOutputResource->Name == OutputResource->Name)
								{
									if (NextStageOutputResource->Usage == ZERN_SRUT_READ_WRITE && NextStageOutputResource->CreationFlags.GetFlags(ZERN_SRCF_GET_FROM_PREV))
									{
										Stage->CreateOutput(OutputResource->Name);
										Valid = true;
										
										break;
									}
								}
							}
						}

						if (Valid)
							break;
					}
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

void ZERNRenderer::BeginScene(ZEScene* Scene)
{
	Scenes.Enqueue(Scene);
	CurrentSceneIndex++;
	Scene->RenderList.LockRead();
}

void ZERNRenderer::EndScene()
{
	CurrentSceneIndex--;
	zeDebugCheck(CurrentSceneIndex < -1, "EndScene called befor BeginScene.");
}

void ZERNRenderer::AddCommand(ZERNCommand* Command)
{
	if (!IsInitialized())
		return;

	Command->SceneIndex = CurrentSceneIndex;
	if (Command->InstanceTag != NULL)
	{
		bool Found = false;
		ZELink<ZERNCommand>* InstancedCommand;
		CommandListInstanced.LockRead();
		{
			InstancedCommand = CommandListInstanced.GetFirst();
			while (InstancedCommand != NULL)
			{
				ZERNCommand* CurrentCommand = InstancedCommand->GetItem();

				if (CurrentCommand->InstanceTag == NULL ||
					CurrentCommand->InstanceTag->Hash != Command->InstanceTag->Hash ||
					CurrentCommand->InstanceTag->GetClass() != Command->InstanceTag->GetClass() ||
					!CurrentCommand->InstanceTag->Check(Command->InstanceTag))
				{
					InstancedCommand = InstancedCommand->GetNext();
					continue;
				}

				Found = true;
				break;
			}
		}
		CommandListInstanced.UnlockRead();

		if (Found)
		{
			if (InstancedCommand != CommandListInstanced.GetFirst())
			{
				CommandListInstanced.LockWrite();
				{
					CommandListInstanced.Remove(InstancedCommand);
					CommandListInstanced.AddBegin(InstancedCommand);
				}
				CommandListInstanced.UnlockWrite();
			}

			InstancedCommand->GetItem()->Instances.AddEnd(Command->GetFreeLink());
		}
		else
		{
			CommandListInstanced.AddEnd(Command->GetFreeLink());
		}
	}
	else
	{
		CommandList.AddEnd(Command->GetFreeLink());
	}
}

void ZERNRenderer::CleanCommands()
{
	ze_for_each(Command, CommandList)
		Command->Instances.Clear();

	ze_for_each(Stage, Stages)
		Stage->Commands.Clear();

	CommandList.Clear();
}

void ZERNRenderer::BeginNestedRenderer()
{
	ze_for_each(Command, CommandList)
		Command->PushInstances();
}

void ZERNRenderer::EndNestedRenderer()
{
	ze_for_each(Command, CommandList)
		Command->PopInstances();
}

void ZERNRenderer::Render()
{
	if (!IsInitialized())
		return;

	Context->BeginEvent("PopulateStageCommands");
	PopulateStageCommands();
	Context->EndEvent();
	Context->BeginEvent("RenderStages");
	RenderStages();
	Context->EndEvent();
	Context->BeginEvent("CleanCommands");
	CleanCommands();
	Context->EndEvent();

	for (ZESize I = 0; I < Scenes.GetCount(); I++)
		Scenes[I]->RenderList.UnlockRead();

	Scenes.Clear();
}

ZERNRenderer::ZERNRenderer()
{
	Context = NULL;

	CurrentSceneIndex = -1;
	DirtyPipeline = false;
	Resized = false;

	memset(&RendererConstants, 0, sizeof(RendererConstants));
}

ZERNRenderer::~ZERNRenderer()
{
	Deinitialize();

	CleanCommands();
	CleanStages();
}
