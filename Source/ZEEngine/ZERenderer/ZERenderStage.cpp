//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERenderStage.cpp
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

#include "ZEError.h"
#include "ZECamera.h"
#include "ZEMaterial.h"
#include "ZERenderStage.h"
#include "ZEGame/ZEScene.h"
#include "ZERenderCommand.h"
#include "ZEGraphics/ZEIndexBuffer.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEgraphics/ZEGraphicsDevice.h"
#include "ZEGraphics/ZEConstantBuffer.h"
#include "ZECore/ZECore.h"
#include "ZECore/ZERealTimeClock.h"

struct ZEPipelineConstants
{
	float			Time0toN;
	float			SinTime0toN;
	float			CosTime0toN;
	float			TanTime0toN;
	
	float			Time0to1;
	float			SinTime0to1;
	float			CosTime0to1;
	float			TanTime0to1;
	
	float			Fov;
	ZEVector2		NearFarZ;
	float			AspectRatio;
	
	ZEVector3		CameraWorldPos;
	float			ElapsedTime;
	ZEVector3		CameraWorldUp;
	float			Dummy1;
	ZEVector3		CameraWorldFront;
	float			Dummy2;
	ZEVector3		CameraWorldRight;
	float			Dummy3;

	ZEVector2		ViewportWidthHeight;
	ZEVector2		InvViewportWidthHeight;
	/*
	ZEMatrix4x4		ViewMatrix;
	ZEMatrix4x4		ProjMatrix;
	ZEMatrix4x4		InvViewMatrix;
	ZEMatrix4x4		InvProjMatrix;
	ZEMatrix4x4		ViewProjMatrix;
	ZEMatrix4x4		InvViewProjMatrix;
	*/
};

// NOTE: this method assumes primitive type is triangle list only !
void ZERenderStage::PumpStreams(ZERenderCommand* RenderCommand)
{
	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	Device->SetVertexLayout(RenderCommand->VertexLayout);
	Device->SetVertexBufferArray(RenderCommand->VertexBuffers);

	if ((RenderCommand->Type & ZE_RCT_INDEXED) && (RenderCommand->Type & ZE_RCT_INSTANCED))
	{
		ZERenderCommandIndexed* IndexedCommand = (ZERenderCommandIndexed*)RenderCommand;
		ZERenderCommandInstanced* InstancedCommand = (ZERenderCommandInstanced*)RenderCommand;
		
		Device->SetIndexBuffer(IndexedCommand->IndexBuffer);
		Device->DrawIndexedInstanced(RenderCommand->PrimitiveType, IndexedCommand->IndexCount, InstancedCommand->InstanceCount, 
										IndexedCommand->FirstIndex, IndexedCommand->BaseVertex, InstancedCommand->FirstInstance);
	}
	else if (RenderCommand->Type & ZE_RCT_INDEXED)
	{
		ZERenderCommandIndexed* IndexedCommand = (ZERenderCommandIndexed*)RenderCommand;

		Device->SetIndexBuffer(IndexedCommand->IndexBuffer);
		Device->DrawIndexed(RenderCommand->PrimitiveType, IndexedCommand->IndexCount, IndexedCommand->FirstIndex, IndexedCommand->BaseVertex);
	}
	else if (RenderCommand->Type & ZE_RCT_INSTANCED)
	{
		ZERenderCommandInstanced* InstancedCommand = (ZERenderCommandInstanced*)RenderCommand;

		Device->DrawInstanced(RenderCommand->PrimitiveType, RenderCommand->PrimitiveCount * 3, RenderCommand->FirstVertex,
								InstancedCommand->InstanceCount, InstancedCommand->FirstInstance);
	}
	else
	{
		Device->Draw(RenderCommand->PrimitiveType, RenderCommand->PrimitiveCount * 3, RenderCommand->FirstVertex);
	}
}

void ZERenderStage::ResetStageDefaults()
{
	DefaultStates.SetToDefault();

	DefaultStates.VertexShaderBuffers[0] = StageBuffer;
	DefaultStates.GeometryShaderBuffers[0] = StageBuffer;
	DefaultStates.PixelShaderBuffers[0] = StageBuffer;
}

void ZERenderStage::CommitStageDefaults()
{
	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	Device->SetVertexShaderBuffer(13, StageBuffer);
	Device->SetGeometryShaderBuffer(13, StageBuffer);
	Device->SetPixelShaderBuffer(13, StageBuffer);
}

void ZERenderStage::UpdateBuffers()
{
	ZECamera* Camera = zeScene->GetActiveCamera();
	if (Camera == NULL)
	{
		zeError("Cannot update buffer without camera.");
		return;
	}

	if (StageBuffer == NULL)
	{
		StageBuffer = ZEConstantBuffer::CreateInstance();
		StageBuffer->Create(sizeof(ZEPipelineConstants));
	}
	
	if (StageConstants.GetCount() != 0)
	{
		ZEPipelineConstants* BufferData = NULL;
		StageBuffer->Lock((void**)&BufferData);

		for (ZESize I = 0; I < StageConstants.GetCount(); ++I)
		{
			switch (StageConstants[I])
			{
				case ZE_PC_TIME_0_1:
					// TODO: Temp value used
					BufferData->Time0to1 = -1.0f;
					break;
				case ZE_PC_SIN_TIME_0_1:
					// TODO: Temp value used
					BufferData->SinTime0to1 = -1.0f;
					break;
				case ZE_PC_COS_TIME_0_1:
					// TODO: Temp value used
					BufferData->CosTime0to1 = -1.0f;
					break;
				case ZE_PC_TAN_TIME_0_1:
					// TODO: Temp value used
					BufferData->TanTime0to1 = -1.0f;
					break;
				case ZE_PC_TIME_0_N:
					// TODO: Temp value used
					BufferData->Time0toN = -1.0f;
					break;
				case ZE_PC_SIN_TIME_0_N:
					// TODO: Temp value used
					BufferData->SinTime0toN = -1.0f;
					break;
				case ZE_PC_COS_TIME_0_N:
					// TODO: Temp value used
					BufferData->CosTime0toN = -1.0f;
					break;
				case ZE_PC_TAN_TIME_0_N:
					// TODO: Temp value used
					BufferData->TanTime0toN = -1.0f;
					break;
				case ZE_PC_FOV:
					BufferData->Fov = Camera->GetFOV();
					break;
				case ZE_PC_NEAR_FAR_Z:
					BufferData->NearFarZ = ZEVector2(Camera->GetNearZ(), Camera->GetFarZ());
					break;
				case ZE_PC_ACPECT_RATIO:
					BufferData->AspectRatio = Camera->GetAspectRatio();
					break;
				case ZE_PC_CAMERA_WORLD_POS:
					BufferData->CameraWorldPos = Camera->GetWorldPosition();
					break;
				case ZE_PC_ELAPSED_TIME:
					BufferData->ElapsedTime = (float)zeCore->GetRealTimeClock()->GetFrameDeltaTime() / 1000000.0f;
					break;

				case ZE_PC_CAMERA_WORLD_UP:
					BufferData->CameraWorldUp = Camera->GetWorldUp();
					break;
				case ZE_PC_CAMERA_WORLD_FRONT:
					BufferData->CameraWorldFront = Camera->GetWorldFront();
					break;
				case ZE_PC_CAMERA_WORLD_RIGHT:
					BufferData->CameraWorldRight = Camera->GetWorldRight();
					break;
				case ZE_PC_VIEWPORT_WIDTH_HEIGHT:
					// TODO: Multiple view-ports should be supported
					BufferData->ViewportWidthHeight = ZEVector2((float)zeGraphics->GetScreenWidth(), (float)zeGraphics->GetScreenHeight());
					break;
				case ZE_PC_INV_VIEWPORT_WIDTH_HEIGHT:
					// TODO: Multiple view-ports should be supported
					BufferData->InvViewportWidthHeight = ZEVector2(1.0f / (float)zeGraphics->GetScreenWidth(), 1.0f / (float)zeGraphics->GetScreenHeight());
					break;
			}
		}

		StageBuffer->Unlock();
	}
}

void ZERenderStage::DestroyBuffers()
{
	ZE_DESTROY(StageBuffer);
}

const ZEGraphicsDeviceState* ZERenderStage::GetDefaultStates() const
{
	return &DefaultStates;
}

void ZERenderStage::Destroy()
{
	delete this;
}

ZERenderStage::ZERenderStage()
{
	StageBuffer = NULL;
}

ZERenderStage::~ZERenderStage()
{
	DestroyBuffers();
}
