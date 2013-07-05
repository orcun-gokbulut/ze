//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERenderer.cpp
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

#include "ZECamera.h"
#include "ZEMaterial.h"
#include "ZERenderer.h"
#include "ZEDS/ZEArray.h"
#include "ZERenderStage.h"
#include "ZEMath/ZEMath.h"
#include "ZECore/ZECore.h"
#include "ZEGame/ZEScene.h"
#include "ZEMath/ZEAngle.h"
#include "ZERenderCommand.h"
#include "ZERenderStageShadow.h"
#include "ZERenderStageLighting.h"
#include "ZERenderStageGeometry.h"
#include "ZERenderStageParticle.h"
#include "ZECore/ZERealTimeClock.h"
#include "ZERenderStageTransparent.h"
#include "ZERenderStagePostProcess.h"
#include "ZERenderStageAccumulation.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZEGraphicsDevice.h"
#include "ZEGraphics/ZEConstantBuffer.h"
#include "ZEGraphics/ZEGraphicsEventTracer.h"

/************************************************************************/
/*                        ZECommandBuffer                               */
/************************************************************************/
inline void ZECommandBuffer::Clear()
{
	EndOfBuffer = 0;
	CommandList.Clear(true);
}

void ZECommandBuffer::SortCommands(ZESize StartIndex, ZESize EndIndex)
{
	zeCriticalError("Not implemented yet.");
	// Sort draw commands between the frame start and frame end
}

void ZECommandBuffer::SortFrames()
{
	zeCriticalError("Not implemented yet.");
	// Find frames in the command buffer and send frame start-end to sort commands
}

inline bool ZECommandBuffer::IsEmpty() const
{
	return CommandList.GetCount() == 0;
}

inline ZESize ZECommandBuffer::GetCount() const
{
	return CommandList.GetCount();
}

inline bool ZECommandBuffer::AddCommand(const ZERenderCommand* Entry)
{
	zeDebugCheck(Entry == NULL, "NULL Pointer.");

	ZESize RemainingSize = ZE_COMMAND_BUFFER_SIZE - EndOfBuffer;
	if (Entry->Size > RemainingSize)
		return false;

	void* Target = (ZEUInt8*)Buffer + EndOfBuffer + 1;

	memcpy(Target, Entry, Entry->Size);
	EndOfBuffer += Entry->Size;
	CommandList.Add((ZERenderCommand*)Target);

	return true;
}

inline const ZERenderCommand* ZECommandBuffer::GetCommand(ZESize Index) const
{
	zeDebugCheck(Index >= CommandList.GetCount(), "Index out of range");

	ZESize Count = CommandList.GetCount();
	if (Count == 0 || Index >= Count)
		return NULL;

	return CommandList[Index];
}

ZECommandBuffer::ZECommandBuffer()
{
	EndOfBuffer = 0;
	Buffer = (void*)new ZEUInt8[ZE_COMMAND_BUFFER_SIZE];
}

ZECommandBuffer::~ZECommandBuffer()
{
	if (Buffer)
	{
		delete [] ((ZEUInt8*)Buffer);
		Buffer = NULL;
	}
}

/**************************************************************************/
/*								ZECommandBucket                           */
/**************************************************************************/

void ZECommandBucket::Clear()
{
	Parameters.Clear();
	Commands.Clear();
}

ZECommandBucket::ZECommandBucket()
{

}

ZECommandBucket::~ZECommandBucket()
{
	Clear();
}

/************************************************************************/
/*								ZERenderer                              */
/************************************************************************/

__declspec(align(16))
struct ZEShaderTimeConstants
{
	float			Time0toN;
	float			SinTime0toN;
	float			CosTime0toN;
	float			TanTime0toN;
	
	float			Time0to1;
	float			SinTime0to1;
	float			CosTime0to1;
	float			TanTime0to1;

	float			ElapsedTime;
};

__declspec(align(16))
struct ZEShaderCameraConstants
{
	ZEVector2		ViewPortDimension;
	ZEVector2		InvViewPortDimension;

	float			NearZ;
	float			FarZ;
	float			Fov;
	float			AspectRatio;
	
	ZEVector3		ViewWorldPos;
	ZEVector3		ViewWorldUp;
	ZEVector3		ViewWorldFront;
	ZEVector3		ViewWorldRight;
	
	ZEMatrix4x4		ViewProjection;
	ZEMatrix4x4		Projection;
	ZEMatrix4x4		View;
	
	ZEMatrix4x4		InvViewProjection;
	ZEMatrix4x4		InvProjection;
	ZEMatrix4x4		InvView;
};

/*
void ZERenderer::UpdateConstants(bool ForceUpdate)
{
	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	// Time Constants
	// -----------------------------------------------------------------
	if (ForceUpdate || UpdateTimeConstants)
	{	
		float ElapsedTime = (float)zeCore->GetRealTimeClock()->GetFrameDeltaTime() / 1000000.0f;
		float Time0ToN = (float)zeCore->GetRealTimeClock()->GetCurrentTime() / 1000000.0f;
		float Time0To1 = Time0ToN - ZEMath::Floor(Time0ToN);

		ZEShaderTimeConstants* TimeBuffer = NULL;
		TimeConstants->Lock((void**)&TimeBuffer);
	
			TimeBuffer->ElapsedTime = ElapsedTime;
			TimeBuffer->Time0to1 = Time0To1;
			TimeBuffer->SinTime0to1 = ZEAngle::Sin(Time0To1);
			TimeBuffer->CosTime0to1 = ZEAngle::Cos(Time0To1);
			TimeBuffer->TanTime0to1 = ZEAngle::Tan(Time0To1);
			TimeBuffer->Time0toN = Time0ToN;
			TimeBuffer->SinTime0toN = ZEAngle::Sin(Time0ToN);
			TimeBuffer->CosTime0toN = ZEAngle::Cos(Time0ToN);
			TimeBuffer->TanTime0toN = ZEAngle::Tan(Time0ToN);

		TimeConstants->Unlock();
		UpdateTimeConstants = false;
	}

	// Camera Constants
	// -----------------------------------------------------------------
	if (ForceUpdate || UpdateCameraConstants)
	{
		ZEVector2 WidthHeight = zeGraphics->GetScreenSize();

		ZEShaderCameraConstants* CameraBuffer = NULL;
		CameraConstants->Lock((void**)&CameraBuffer);

			CameraBuffer->ViewPortDimension = WidthHeight;
			CameraBuffer->InvViewPortDimension = ZEVector2::One / WidthHeight;

			CameraBuffer->NearZ = Camera->GetNearZ();
			CameraBuffer->FarZ = Camera->GetFarZ();
			CameraBuffer->Fov = Camera->GetFOV();
			CameraBuffer->AspectRatio = Camera->GetAspectRatio();
		
			CameraBuffer->ViewWorldPos = Camera->GetWorldPosition();
			CameraBuffer->ViewWorldRight = Camera->GetWorldRight();
			CameraBuffer->ViewWorldUp = Camera->GetWorldUp();
			CameraBuffer->ViewWorldFront = Camera->GetWorldFront();
		
			CameraBuffer->View = Camera->GetViewTransform();
			CameraBuffer->Projection = Camera->GetProjectionTransform();
			CameraBuffer->ViewProjection = Camera->GetViewProjectionTransform();
		
			CameraBuffer->InvView = Camera->GetViewTransform().Inverse();
			CameraBuffer->InvProjection = Camera->GetProjectionTransform().Inverse();
			CameraBuffer->InvViewProjection = Camera->GetViewProjectionTransform().Inverse();

		CameraConstants->Unlock();
		UpdateCameraConstants = false;
	}

	Device->SetVertexShaderBuffer(12, TimeConstants);
	Device->SetGeometryShaderBuffer(12, TimeConstants);
	Device->SetPixelShaderBuffer(12, TimeConstants);

	Device->SetVertexShaderBuffer(13, CameraConstants);
	Device->SetGeometryShaderBuffer(13, CameraConstants);
	Device->SetPixelShaderBuffer(13, CameraConstants);
}
*/

void ZERenderer::ProcessStage(ZERenderStage* Stage)
{
	ZESize BucketCount = Buckets.GetCount();
	ZERenderStageType BucketStage = Stage->GetStageType();
	
	ZEListIteratorConst<ZECommandBucket> Iterator = Buckets.GetConstIterator();

	while (!Iterator.IsEnd())
	{
		const ZECommandBucket* Bucket = Iterator.GetItem();
		ZERenderStageType Stages =  Bucket->Parameters.Stages;

		if (Stage->GetStageType().GetFlags(Stages))
			continue;

		ZESize CommandCount = Bucket->Commands.GetCount();
		for (ZESize J = 0; J < CommandCount; ++J)
		{
			const ZERenderCommand* Command = Bucket->Commands.GetCommand(J);
			Stage->Process(Command);
		}

		Iterator.MoveNext();
	}
}

ZECommandBucket* ZERenderer::CreateCommandBucket(const ZEDrawParameters* Parameters)
{
	ZECommandBucket* NewBucket = new ZECommandBucket();
	NewBucket->Parameters = *Parameters;
	Buckets.Append(NewBucket);

	return NewBucket;
}

bool ZERenderer::PreRender()
{
	if (!zeGraphics->GetEnabled())
	{
		zeWarning("zeGraphics disabled.");
		return false;
	}
}

bool ZERenderer::PostRender()
{
	zeGraphics->GetDevice()->Present();
	Buckets.RemoveAll();
}

bool ZERenderer::Render(float ElaspedTime)
{
	ZEGraphicsEventTracer* Tracer = ZEGraphicsEventTracer::GetInstance();
	
	//	ShadowStage
	//------------------------------------------------
	Tracer->StartEvent("Shadow Stage");
	ShadowStage->Setup();
	ProcessStage(ShadowStage);
	Tracer->EndEvent();	

	//	GeometryStage
	//------------------------------------------------
	Tracer->StartEvent("Geometry Stage");
	GeometryStage->Setup();
	ProcessStage(GeometryStage);
	Tracer->EndEvent();
	
	//	LightingStage
	//------------------------------------------------
	Tracer->StartEvent("Lighting Stage");
	LightingStage->Setup();
	ProcessStage(LightingStage);
	Tracer->EndEvent();

	//	ForwardStage
	//------------------------------------------------
	Tracer->StartEvent("Forward Stage");
	AccumulationStage->Setup();
	ProcessStage(AccumulationStage);
	Tracer->EndEvent();

	//	ParticleStage
	//-----------------------------------------------
	Tracer->StartEvent("Particle Stage");
	ParticleStage->Setup();
	ProcessStage(ParticleStage);
	Tracer->EndEvent();
}

bool ZERenderer::Initialize() 
{
	ShadowStage	= new ZERenderStageShadow();
	AccumulationStage = new ZERenderStageAccumulation();
	GeometryStage = new ZERenderStageGeometry();
	LightingStage = new ZERenderStageLighting();
	ParticleStage = new ZERenderStageParticle();

	LightingStage->SetInputGeometryStage(GeometryStage);
	AccumulationStage->SetInputGeometryStage(GeometryStage);
	AccumulationStage->SetInputLightingStage(LightingStage);
	ParticleStage->SetInputAccumulationStage(AccumulationStage);

	return true;
}

void ZERenderer::Deinitialize()
{
	ZE_DESTROY(ShadowStage);
	ZE_DESTROY(AccumulationStage);
	ZE_DESTROY(GeometryStage);
	ZE_DESTROY(LightingStage);
	ZE_DESTROY(ParticleStage);
}

void ZERenderer::Destroy()
{
	delete this;
}

ZERenderer* ZERenderer::CreateInstance()
{
	return new ZERenderer();
}

ZERenderer::ZERenderer()
{
	ShadowStage = NULL;
	GeometryStage = NULL;
	LightingStage = NULL;
	AccumulationStage = NULL;
	ParticleStage = NULL;
}

ZERenderer::~ZERenderer()
{
	Deinitialize();
}
