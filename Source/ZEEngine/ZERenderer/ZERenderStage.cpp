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

#include "ZEView.h"
#include "ZEError.h"
#include "ZEMaterial.h"
#include "ZERenderStage.h"
#include "ZERenderCommand.h"
#include "ZEGraphics/ZEIndexBuffer.h"
#include "ZEGraphics/ZEConstantBuffer.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEgraphics/ZEGraphicsDevice.h"
#include "ZEGame/ZEDrawParameters.h"

/******************************************************************/
/*							ZERenderStageData					  */
/******************************************************************/
void ZERenderStageData::Create(const ZEView* View)
{
	zeDebugCheck(View == NULL, "Null pointer");
	
	Fov = View->GetFov();
	FarZ = View->GetFarZ();
	NearZ = View->GetNearZ();
	AspectRatio = View->GetAspectRatio();
	
	ViewDimension = View->GetViewDimension();
	
	WorldPos = View->GetWorldPosition();
	WorldUp = View->GetWorldUp();
	WorldFront = View->GetWorldFront();
	WorldRight = View->GetWorldRight();
	
	ViewTransform = View->GetViewTransform();
	ProjectionTransform = View->GetProjectionTransform();
}

ZERenderStageData::ZERenderStageData()
{
	Fov = 0.0f;
	FarZ = 0.0f;
	NearZ = 0.0f;
	AspectRatio = 0.0f;

	ViewDimension = ZEVector2::Zero;

	WorldPos = ZEVector3::Zero;
	WorldUp = ZEVector3::Zero;
	WorldFront = ZEVector3::Zero;
	WorldRight = ZEVector3::Zero;

	ViewTransform = ZEMatrix4x4::Identity;
	ProjectionTransform = ZEMatrix4x4::Identity;
}

ZERenderStageData::~ZERenderStageData()
{

}

/******************************************************************/
/*							ZERenderStage						  */
/******************************************************************/
static ZEUInt GetVertexCount(ZEPrimitiveType Type, ZEUInt32 Count)
{
	ZEUInt VertexCount = 0;
	switch (Type)
	{
		default:
		case ZE_PT_NONE:
			zeWarning("Unknown primitive type in render command.");
			break;
		case ZE_PT_POINT_LIST:
			VertexCount = Count;
			break;
		case ZE_PT_LINE_LIST:
			VertexCount = Count * 2;
			break;
		case ZE_PT_TRIANGLE_LIST:
			VertexCount = Count * 3;
			break;
		case ZE_PT_TRIANGLE_STRIPT:
			VertexCount = Count + 2;
			break;
	}
	return VertexCount;
}

void ZERenderStage::PumpStreams(const ZERenderCommand* RenderCommand)
{
	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	switch (RenderCommand->Type)
	{
		default:
		case ZE_RCT_NONE:
		{
			zeWarning("Unknown render command type, possibly corrupted.");
			break;
		}
		case ZE_RCT_DEFAULT:
		{
			ZERenderCommandDefault* Command = (ZERenderCommandDefault*)RenderCommand;
			Device->SetVertexBufferArray(Command->VertexBuffers);
			Device->SetVertexLayout(Command->VertexLayout);

			Device->Draw(Command->PrimitiveType,
						GetVertexCount(Command->PrimitiveType, Command->PrimitiveCount),
						Command->FirstVertex);

			break;
		}
		case ZE_RCT_INDEXED:
		{
			ZERenderCommandIndexed* Command = (ZERenderCommandIndexed*)RenderCommand;
			Device->SetVertexBufferArray(Command->VertexBuffers);
			Device->SetVertexLayout(Command->VertexLayout);
			Device->SetIndexBuffer(Command->IndexBuffer);

			Device->DrawIndexed(Command->PrimitiveType, 
								Command->IndexCount, 
								Command->FirstIndex, 
								Command->BaseVertex);

			break;
		}	
		case ZE_RCT_INSTANCED:
		{
			ZERenderCommandInstanced* Command = (ZERenderCommandInstanced*)RenderCommand;
			Device->SetVertexBufferArray(Command->VertexBuffers);
			Device->SetVertexLayout(Command->VertexLayout);

			Device->DrawInstanced(	Command->PrimitiveType,
									GetVertexCount(Command->PrimitiveType, Command->PrimitiveCount),
									Command->FirstVertex,
									Command->InstanceCount, 
									Command->FirstInstance);

			break;
		}	
		case ZE_RCT_INDEXED_INSTANCED:
		{
			ZERenderCommandIndexedInstanced* Command = (ZERenderCommandIndexedInstanced*)RenderCommand;
			Device->SetVertexBufferArray(Command->VertexBuffers);
			Device->SetVertexLayout(Command->VertexLayout);
			Device->SetIndexBuffer(Command->IndexBuffer);

			Device->DrawIndexedInstanced(Command->PrimitiveType, 
										Command->IndexCount, 
										Command->InstanceCount, 
										Command->FirstIndex, 
										Command->BaseVertex, 
										Command->FirstInstance);

			break;
		}	
	}
}

void ZERenderStage::SetData(const ZERenderStageData* Data)
{
	zeDebugCheck(Data == NULL, "Null pointer.");

	StageData = Data;
}

const ZERenderStageData* ZERenderStage::GetData() const
{
	return StageData;
}

const ZEGraphicsDeviceState* ZERenderStage::GetDefaultStates() const
{
	return &DefaultStates;
}

bool ZERenderStage::ResetStates(const ZEMaterial* Material)
{
	bool ForceReset = Material == NULL;

	if (!ForceReset && (LastMaterialHash == Material->GetHash()))
		return false;

	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	DefaultStates.SetToDefault();

	DefaultStates.VertexShaderBuffers[13] = ViewConstants;
	DefaultStates.GeometryShaderBuffers[13] = ViewConstants;
	DefaultStates.PixelShaderBuffers[13] = ViewConstants;

	Device->SetVertexShaderBuffer(13, DefaultStates.VertexShaderBuffers[13]);
	Device->SetGeometryShaderBuffer(13, DefaultStates.GeometryShaderBuffers[13]);
	Device->SetPixelShaderBuffer(13, DefaultStates.PixelShaderBuffers[13]);

	LastMaterialHash = ForceReset ? LastMaterialHash : Material->GetHash();

	return true;
}

bool ZERenderStage::Process(const ZERenderCommand* RenderCommand)
{
	zeDebugCheck(RenderCommand == NULL, "Null Pointer");
	zeDebugCheck(RenderCommand->Material == NULL, "Null Pointer");

	ZERenderStageType StageType = GetStageType();
	ZERenderStageType MaterialStages = RenderCommand->Material->GetEnabledStages();
	
	if (!MaterialStages.GetFlags(StageType))
		return false;

	ResetStates(RenderCommand->Material);

	return true;
}

struct ZEShaderViewConstants
{
	ZEVector2		ViewDimension;
	ZEVector2		InvViewDimension;

	ZEVector3		WorldPos;
	float			Fov;
	ZEVector3		WorldUp;
	float			FarZ;
	ZEVector3		WorldFront;
	float			NearZ;
	ZEVector3		WorldRight;
	float			AspectRatio;

	ZEMatrix4x4		ViewProjection;
	ZEMatrix4x4		Projection;
	ZEMatrix4x4		View;

	ZEMatrix4x4		InvViewProjection;
	ZEMatrix4x4		InvProjection;
	ZEMatrix4x4		InvView;
};

bool ZERenderStage::Setup()
{
	if (StageData == NULL)
	{
		zeWarning("Render stage data not set");
		return false;
	}

	// Set stage constants
	ZEShaderViewConstants* Constants = NULL;
	ViewConstants->Lock((void**)&Constants);

		Constants->Fov = StageData->Fov;
		Constants->FarZ = StageData->FarZ;
		Constants->NearZ = StageData->NearZ;
		Constants->AspectRatio = StageData->AspectRatio;
		
		Constants->ViewDimension = StageData->ViewDimension;
		Constants->InvViewDimension = ZEVector2::One / StageData->ViewDimension;
		
		Constants->WorldPos = StageData->WorldPos;
		Constants->WorldUp = StageData->WorldUp;
		Constants->WorldFront = StageData->WorldFront;
		Constants->WorldRight = StageData->WorldRight;
		
		Constants->View = StageData->ViewTransform;
		Constants->Projection = StageData->ProjectionTransform;
		Constants->ViewProjection = StageData->ProjectionTransform * StageData->ViewTransform;

		ZEMatrix4x4::Inverse(Constants->InvView, Constants->View);
		ZEMatrix4x4::Inverse(Constants->InvProjection, Constants->Projection);
		ZEMatrix4x4::Inverse(Constants->InvViewProjection, Constants->ViewProjection);

	ViewConstants->Unlock();

	ZEGraphicsDevice* Device = zeGraphics->GetDevice();
	Device->SetVertexShaderBuffer(13, ViewConstants);
	Device->SetGeometryShaderBuffer(13, ViewConstants);
	Device->SetPixelShaderBuffer(13, ViewConstants);

	return true;
}

void ZERenderStage::Destroy()
{
	delete this;
}

ZERenderStage::ZERenderStage()
{
	StageData = NULL;

	LastMaterialHash = 0;

	ViewConstants = ZEConstantBuffer::CreateInstance();
	ViewConstants->Create(sizeof(ZEShaderViewConstants));
	ViewConstants->SetZero();
}

ZERenderStage::~ZERenderStage()
{
	ZE_DESTROY(ViewConstants);
}

/*
void ZERenderer::UpdateConstants(bool ForceUpdate)
{
	ZEGraphicsDevice* Device = zeGraphics->GetDevice();
	if (ForceUpdate || UpdateTimeConstants)
	{	
		float ElapsedTime = (float)zeCore->GetRealTimeClock()->GetFrameDeltaTime() / 1000000.0f;
		float Time0ToN = (float)zeCore->GetRealTimeClock()->GetCurrentTime() / 1000000.0f;
		float Time0To1 = Time0ToN - ZEMath::Floor(Time0ToN);

		ZEShaderTimeConstants* TimeBuffer = NULL;
		TimeConstants->Lock((void**)&TimeBuffer);
	
			TimeBuffer->ElapsedTime = ElapsedTime;
			TimeBuffer->Time0to1	= Time0To1;
			TimeBuffer->SinTime0to1 = ZEAngle::Sin(Time0To1);
			TimeBuffer->CosTime0to1 = ZEAngle::Cos(Time0To1);
			TimeBuffer->TanTime0to1 = ZEAngle::Tan(Time0To1);
			TimeBuffer->Time0toN	= Time0ToN;
			TimeBuffer->SinTime0toN = ZEAngle::Sin(Time0ToN);
			TimeBuffer->CosTime0toN = ZEAngle::Cos(Time0ToN);
			TimeBuffer->TanTime0toN = ZEAngle::Tan(Time0ToN);

		TimeConstants->Unlock();
		UpdateTimeConstants = false;
	}

	Device->SetVertexShaderBuffer(12, TimeConstants);
	Device->SetGeometryShaderBuffer(12, TimeConstants);
	Device->SetPixelShaderBuffer(12, TimeConstants);
}
*/
