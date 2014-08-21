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
#include "ZEGraphics/ZEGraphicsWindow.h"
#include "ZEGraphics/ZEGraphicsEventTracer.h"

/************************************************************************/
/*                        ZECommandBuffer                               */
/************************************************************************/
void ZECommandBuffer::Clear()
{
	EndOfBuffer = 0;
	CommandList.Clear(true);
}

void ZECommandBuffer::SortCommands()
{
	zeCriticalError("Not implemented yet.");
	// Sort commands that are in commadlist
}

bool ZECommandBuffer::IsEmpty() const
{
	return CommandList.GetCount() == 0;
}

ZESize ZECommandBuffer::GetCount() const
{
	return CommandList.GetCount();
}

bool ZECommandBuffer::AddCommand(const ZERenderCommand* Entry)
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

const ZERenderCommand* ZECommandBuffer::GetCommand(ZESize Index) const
{
	zeDebugCheck(Index >= CommandList.GetCount(), "Index out of range");

	return CommandList.GetCount() == 0 ? NULL : CommandList[Index];
}

ZECommandBuffer::ZECommandBuffer()
{
	EndOfBuffer = 0;
	Buffer = (void*)new ZEUInt8[ZE_COMMAND_BUFFER_SIZE];
}

ZECommandBuffer::~ZECommandBuffer()
{
	if (Buffer !=  NULL)
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
	TargetStages = 0;

	if (StageData != NULL)
	{
		delete StageData;
		StageData = NULL;
	}

	CommandBuffer.Clear();
}

bool ZECommandBucket::AddRenderCommand(const ZERenderCommand* Command)
{
	zeDebugCheck(Command == NULL, "Null pointer");

	return CommandBuffer.AddCommand(Command);
}

ZECommandBucket::ZECommandBucket()
{
	TargetStages = 0;
	StageData = NULL;
}

ZECommandBucket::~ZECommandBucket()
{
	Clear();
}

/************************************************************************/
/*								ZERenderer                              */
/************************************************************************/
void ZERenderer::ProcessStage(ZERenderStage* Stage)
{	
	ZERenderStageType StageType = Stage->GetStageType();
	ZEListIterator<ZECommandBucket> Iterator = BucketList.GetIterator();
	
	while (!Iterator.IsEnd())
	{
		ZECommandBucket* Bucket = Iterator.GetItem();

		// Skip if bucket is not for this stage
		if (!Bucket->TargetStages.GetFlags(StageType))
		{
			Iterator.MoveNext();
			continue;
		}
			
		// Setup stage
		Stage->SetData(Bucket->StageData);
		Stage->Setup();
		Stage->ResetStates(NULL);

		// Process all commands in bucket with given stage
		ZESize CommandCount = Bucket->CommandBuffer.GetCount();
		for (ZESize J = 0; J < CommandCount; ++J)
		{
			const ZERenderCommand* Command = Bucket->CommandBuffer.GetCommand(J);
			Stage->Process(Command);
		}

		// Move to next bucket
		Iterator.MoveNext();

		// Remove stage from target stages
		Bucket->TargetStages.UnraiseFlags(StageType);

		// If there are no more target stages
		if (Bucket->TargetStages == 0)
		{
			ZECommandBucket* OldBucket = BucketList.Dequeue();
			OldBucket->Clear();

			EmptyBuckets.Append(OldBucket);
		}
	}
}

ZECommandBucket* ZERenderer::CreateCommandBucket(ZERenderStageType TargetStages, ZERenderStageData* StageData)
{
	zeDebugCheck(StageData == NULL, "NULL Pointer");
	zeDebugCheck(TargetStages == 0, "There must be at least one target stage");

	ZECommandBucket* NewBucket = NULL;

	if (EmptyBuckets.GetCount() == 0)
	{
		NewBucket = new ZECommandBucket();
	}
	else
	{
		NewBucket = EmptyBuckets.Dequeue();
	}
	
	NewBucket->StageData = StageData;
	NewBucket->TargetStages = TargetStages;
	BucketList.Append(NewBucket);

	return NewBucket;
}

// Move this struct to renderer!
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
	float			Reserved[3];
};

bool ZERenderer::PreRender()
{
	if (!zeGraphics->GetEnabled())
	{
		zeWarning("zeGraphics disabled.");
		return false;
	}

	const ZEArray<ZEGraphicsWindow*>& Windows = zeGraphics->GetWindows();
	for (ZESize I = 0; I < Windows.GetCount(); ++I)
	{
		if (!Windows[I]->Update())
			return false;
	}

	float ElapsedTime = (float)zeCore->GetRealTimeClock()->GetFrameDeltaTime() / 1000000.0f;
	float Time0ToN = (float)zeCore->GetRealTimeClock()->GetCurrentTime() / 1000000.0f;
	float Time0To1 = Time0ToN - ZEMath::Floor(Time0ToN);

	ZEShaderTimeConstants* Constants = NULL;
	TimerConstants->Lock((void**)&Constants);

		Constants->ElapsedTime = ElapsedTime;
		
		Constants->Time0to1	= Time0To1;
		Constants->SinTime0to1 = ZEAngle::Sin(Time0To1);
		Constants->CosTime0to1 = ZEAngle::Cos(Time0To1);
		Constants->TanTime0to1 = ZEAngle::Tan(Time0To1);

		Constants->Time0toN	= Time0ToN;
		Constants->SinTime0toN = ZEAngle::Sin(Time0ToN);
		Constants->CosTime0toN = ZEAngle::Cos(Time0ToN);
		Constants->TanTime0toN = ZEAngle::Tan(Time0ToN);

	TimerConstants->Unlock();

	ZEGraphicsDevice* Device = zeGraphics->GetDevice();
	Device->SetVertexShaderBuffer(12, TimerConstants);
	Device->SetGeometryShaderBuffer(12, TimerConstants);
	Device->SetPixelShaderBuffer(12, TimerConstants);

	ZEListIterator<ZECommandBucket> Iterator = BucketList.GetIterator();
	
	while (!Iterator.IsEnd())
	{
		ZECommandBucket* Bucket = Iterator.GetItem();
		Bucket->CommandBuffer.SortCommands();
	}

	return true;
}

bool ZERenderer::PostRender()
{
	const ZEArray<ZEGraphicsWindow*>& Windows = zeGraphics->GetWindows();
	for (ZESize I = 0; I < Windows.GetCount(); ++I)
	{
		if (!Windows[I]->Present())
			return false;
	}

	return true;
}

bool ZERenderer::Render(float ElaspedTime)
{
	ZEGraphicsEventTracer* Tracer = ZEGraphicsEventTracer::GetInstance();

	//zeLog("<=====================Shadow Stage===========================>");

	Tracer->StartEvent("Shadow Stage");
	ProcessStage(ShadowStage);
	Tracer->EndEvent();	

	//zeLog("<=====================Geometry Stage===========================>");

	Tracer->StartEvent("Geometry Stage");
	ProcessStage(GeometryStage);
	Tracer->EndEvent();
	
	//zeLog("<=====================Lighting Stage===========================>");

	Tracer->StartEvent("Lighting Stage");
	ProcessStage(LightingStage);
	Tracer->EndEvent();
	
	//zeLog("<=====================Accumulation Stage===========================>");

	Tracer->StartEvent("Accumulation Stage");
	ProcessStage(AccumulationStage);
	Tracer->EndEvent();

	//zeLog("<=====================Particle Stage===========================>");

	Tracer->StartEvent("Particle Stage");
	ProcessStage(ParticleStage);
	Tracer->EndEvent();
	
	return true;
}

bool ZERenderer::Initialize() 
{
	if (ShadowStage == NULL)
	{
		ShadowStage	= new ZERenderStageShadow();
	}

	if (GeometryStage == NULL)
	{
		GeometryStage = new ZERenderStageGeometry();
	}
	
	if (LightingStage == NULL)
	{
		LightingStage = new ZERenderStageLighting();
		LightingStage->SetInputStageGeometry(GeometryStage);
	}
	
	if (AccumulationStage == NULL)
	{
		AccumulationStage = new ZERenderStageAccumulation();
		AccumulationStage->SetInputStageGeometry(GeometryStage);
		AccumulationStage->SetInputStageLighting(LightingStage);
	}
	
	if (ParticleStage == NULL)
	{
		ParticleStage = new ZERenderStageParticle();
		ParticleStage->SetInputStageAccumulation(AccumulationStage);
	}	

	if (TimerConstants == NULL)
	{
		TimerConstants = ZEConstantBuffer::CreateInstance();
		TimerConstants->Create(sizeof(ZEShaderTimeConstants));
		TimerConstants->SetZero();
	}

	return true;
}

void ZERenderer::Deinitialize()
{
	ZE_DESTROY(ShadowStage);
	ZE_DESTROY(AccumulationStage);
	ZE_DESTROY(GeometryStage);
	ZE_DESTROY(LightingStage);
	ZE_DESTROY(ParticleStage);

	ZE_DESTROY(TimerConstants);

	ZESize BucketCount = EmptyBuckets.GetCount();
	for (ZESize I = 0; I < BucketCount; ++I)
	{
		ZECommandBucket* Bucket = EmptyBuckets.Dequeue();
		
		Bucket->Clear();
		delete Bucket;
	}

	BucketCount = BucketList.GetCount();
	for (ZESize I = 0; I < BucketCount; ++I)
	{
		ZECommandBucket* Bucket = BucketList.Dequeue();
		
		Bucket->Clear();
		delete Bucket;
	}
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

	TimerConstants = NULL;
}

ZERenderer::~ZERenderer()
{
	Deinitialize();
}
