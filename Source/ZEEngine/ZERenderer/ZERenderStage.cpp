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

struct ZEPipelineConstants
{
	float			Time_0_N;
	float			Sin_Time_0_N;
	float			Cos_Time_0_N;
	float			Tan_Time_0_N;
	float			Time_0_1;
	float			Sin_Time_0_1;
	float			Cos_Time_0_1;
	float			Tan_Time_0_1;
	float			Fov;
	ZEVector2		Near_Far_Z;
	float			Aspect_Ratio;
	ZEVector3		Camera_Pos;
	float			Dummy0;
	ZEVector3		Camera_World_Up;
	float			Dummy1;
	ZEVector3		Camera_World_Front;
	float			Dummy2;
	ZEVector3		Camera_World_Right;
	float			Dummy3;
	ZEVector2		Viewport_Width_Height;
	ZEVector2		Inv_Viewport_Width_Height;
	ZEMatrix4x4		View_Matrix;
	ZEMatrix4x4		Proj_Matrix;
	ZEMatrix4x4		Inv_View_Matrix;
	ZEMatrix4x4		Inv_Proj_Matrix;
	ZEMatrix4x4		View_Proj_Matrix;
	ZEMatrix4x4		Inv_View_Proj_Matrix;
};

// NOTE: this method assumes primitive type is triangle list only !
void ZERenderStage::PumpStreams(ZERenderCommand* RenderCommand)
{
	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	Device->SetIndexBuffer(RenderCommand->IndexBuffer);
	Device->SetVertexLayout(RenderCommand->VertexLayout);
	Device->SetVertexBufferArray(RenderCommand->VertexBuffers);

	if ((RenderCommand->Flags & ZE_ROF_INDEXED) && (RenderCommand->Flags & ZE_ROF_INSTANCED))
	{
		ZEUInt InstanceCount = (ZEUInt)RenderCommand->Instances.GetCount();
		ZEUInt IndexCount = (ZEUInt)RenderCommand->IndexBuffer->GetIndexCount();
		
		Device->DrawIndexedInstanced(RenderCommand->PrimitiveType, IndexCount, InstanceCount, 
			RenderCommand->FirstIndex, RenderCommand->BaseVertex, RenderCommand->FirstInstance);
	}
	else if (RenderCommand->Flags & ZE_ROF_INDEXED)
	{
		ZEUInt IndexCount = (ZEUInt)RenderCommand->IndexBuffer->GetIndexCount();

		Device->DrawIndexed(RenderCommand->PrimitiveType, IndexCount, 
			RenderCommand->FirstIndex, RenderCommand->BaseVertex);
	}
	else if (RenderCommand->Flags & ZE_ROF_INSTANCED)
	{
		Device->DrawInstanced(RenderCommand->PrimitiveType, RenderCommand->PrimitiveCount * 3, 
			RenderCommand->FirstVertex, (ZEUInt)RenderCommand->Instances.GetCount(), RenderCommand->FirstInstance);
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

	Device->SetVertexShaderBuffer(5, StageBuffer);
	Device->SetGeometryShaderBuffer(5, StageBuffer);
	Device->SetPixelShaderBuffer(5, StageBuffer);
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
					BufferData->Time_0_1 = -1.0f;
					break;
				case ZE_PC_SIN_TIME_0_1:
					// TODO: Temp value used
					BufferData->Sin_Time_0_1 = -1.0f;
					break;
				case ZE_PC_COS_TIME_0_1:
					// TODO: Temp value used
					BufferData->Cos_Time_0_1 = -1.0f;
					break;
				case ZE_PC_TAN_TIME_0_1:
					// TODO: Temp value used
					BufferData->Tan_Time_0_1 = -1.0f;
					break;
				case ZE_PC_TIME_0_N:
					// TODO: Temp value used
					BufferData->Time_0_N = -1.0f;
					break;
				case ZE_PC_SIN_TIME_0_N:
					// TODO: Temp value used
					BufferData->Sin_Time_0_N = -1.0f;
					break;
				case ZE_PC_COS_TIME_0_N:
					// TODO: Temp value used
					BufferData->Cos_Time_0_N = -1.0f;
					break;
				case ZE_PC_TAN_TIME_0_N:
					// TODO: Temp value used
					BufferData->Tan_Time_0_N = -1.0f;
					break;
				case ZE_PC_FOV:
					BufferData->Fov = Camera->GetFOV();
					break;
				case ZE_PC_NEAR_FAR_Z:
					BufferData->Near_Far_Z = ZEVector2(Camera->GetNearZ(), Camera->GetFarZ());
					break;
				case ZE_PC_ACPECT_RATIO:
					BufferData->Aspect_Ratio = Camera->GetAspectRatio();
					break;
				case ZE_PC_CAMERA_POS:
					BufferData->Camera_Pos = Camera->GetWorldPosition();
					break;
				case ZE_PC_CAMERA_WORLD_UP:
					BufferData->Camera_World_Up = Camera->GetWorldUp();
					break;
				case ZE_PC_CAMERA_WORLD_FRONT:
					BufferData->Camera_World_Front = Camera->GetWorldFront();
					break;
				case ZE_PC_CAMERA_WORLD_RIGHT:
					BufferData->Camera_World_Right = Camera->GetWorldRight();
					break;
				case ZE_PC_VIEWPORT_WIDTH_HEIGHT:
					// TODO: Multiple view-ports should be supported
					BufferData->Viewport_Width_Height = ZEVector2((float)zeGraphics->GetScreenWidth(), (float)zeGraphics->GetScreenHeight());
					break;
				case ZE_PC_INV_VIEWPORT_WIDTH_HEIGHT:
					// TODO: Multiple view-ports should be supported
					BufferData->Inv_Viewport_Width_Height = ZEVector2(1.0f / (float)zeGraphics->GetScreenWidth(), 1.0f / (float)zeGraphics->GetScreenHeight());
					break;
				case ZE_PC_VIEW_MATRIX:
					BufferData->View_Matrix = Camera->GetViewTransform();
					break;
				case ZE_PC_PROJ_MATRIX:
					BufferData->Proj_Matrix = Camera->GetProjectionTransform();
					break;
				case ZE_PC_INV_VIEW_MATRIX:
					ZEMatrix4x4::Inverse(BufferData->Proj_Matrix, Camera->GetViewTransform());
					break;
				case ZE_PC_INV_PROJ_MATRIX:
					ZEMatrix4x4::Inverse(BufferData->Inv_Proj_Matrix, Camera->GetProjectionTransform());
					break;
				case ZE_PC_VIEW_PROJ_MATRIX:
					BufferData->View_Proj_Matrix = Camera->GetViewProjectionTransform();
					break;
				case ZE_PC_INV_VIEW_PROJ_MATRIX:
					ZEMatrix4x4::Inverse(BufferData->Inv_Proj_Matrix, Camera->GetViewProjectionTransform());
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
