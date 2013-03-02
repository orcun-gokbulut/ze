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
#include "ZERenderer.h"
#include "ZEDS/ZEArray.h"
#include "ZERenderStage.h"
#include "ZERenderCommand.h"
#include "ZERenderStageShadow.h"
#include "ZERenderStageForward.h"
#include "ZERenderStageLighting.h"
#include "ZERenderStageGeometry.h"
#include "ZERenderStageParticle.h"
#include "ZERenderStageTransparent.h"
#include "ZERenderStagePostProcess.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZEGraphicsDevice.h"
#include "ZEGraphics/ZEGraphicsEventTracer.h"

/************************************************************************/
/*                        ZECommandBuffer                               */
/************************************************************************/
ZECommandBufferEntry::ZECommandBufferEntry()
{
	EntryType = ZE_CBET_NONE;
	DataSize = 0;
	Data = NULL;
}

ZECommandBufferEntry::~ZECommandBufferEntry()
{

}

__forceinline void ZECommandBuffer::Clear()
{
	EndOfBuffer = 0;
	CommandList.Clear(true);
}

__forceinline bool ZECommandBuffer::BufferEmpty() const
{
	return CommandList.GetCount() == 0;
}

__forceinline ZESize ZECommandBuffer::GetCommandCount() const
{
	return CommandList.GetCount();
}

__forceinline const ZECommandBufferEntry* ZECommandBuffer::GetEntry(ZESize Index)
{
	ZESize Count = CommandList.GetCount();
	if (Count == 0 || Index >= Count)
		return NULL;

	return &CommandList[Index];
}

__forceinline bool ZECommandBuffer::AddEntry(const ZECommandBufferEntry& Entry)
{
	zeDebugCheck(Entry.DataSize == 0, "Zero Size");
	zeDebugCheck(Entry.Data == NULL, "NULL Pointer.");
	zeDebugCheck(Entry.EntryType == 0, "Unknown entry type");

	ZESize RemainingSize = ZE_COMMAND_BUFFER_SIZE - EndOfBuffer;
	if (Entry.DataSize > RemainingSize)
		return false;

	void* Target = (ZEUInt8*)Buffer + EndOfBuffer + 1;

	memcpy(Target, Entry.Data, Entry.DataSize);
	EndOfBuffer += Entry.DataSize;
	
	ZECommandBufferEntry* Added = CommandList.Add(Entry);
	Added->Data = Target;
	
	return true;
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

/************************************************************************/
/*								ZERenderer                              */
/************************************************************************/
bool ZERenderer::Initialize() 
{
	ShadowStage		= new ZERenderStageShadow();
	ForwardStage	= new ZERenderStageForward();
	GeometryStage	= new ZERenderStageGeometry();
	LightingStage	= new ZERenderStageLighting();
	ParticleStage	= new ZERenderStageParticle();

	LightingStage->SetInputGeometryStage(GeometryStage);
	ForwardStage->SetInputGeometryStage(GeometryStage);
	ForwardStage->SetInputLightingStage(LightingStage);
	ParticleStage->SetInputAccumulationStage(ForwardStage);

	return true;
}

void ZERenderer::Deinitialize()
{
	ZE_DESTROY(ShadowStage);
	ZE_DESTROY(ForwardStage);
	ZE_DESTROY(GeometryStage);
	ZE_DESTROY(LightingStage);
	ZE_DESTROY(ParticleStage);
}

void ZERenderer::Destroy()
{
	delete this;
}

static ZESize GetIndex(ZEUInt32 Value)
{
	if (Value == 0)
		return 0;

	ZESize Index = 0;
	do
	{
		Value >>= 1;
		Index++;
	
	} while (Value);
	
	return Index-1;
}

/*
ZEUInt32				SolveDependency(ZEUInt32 Solved = 0, ZEUInt32 StageIdentifier = 0, ZEUInt32 Dependency = 0xFFFFFFFE);
ZEUInt32 ZERenderer::SolveDependency(ZEUInt32 SolvedDependencies, ZEUInt32 StageIdentifier, ZEUInt32 Dependency)
{
	
	// Check if dependency needs to be solved
	if ((Dependency & ZE_RENDER_STAGE_NONE) == ZE_RENDER_STAGE_NONE)
		return StageIdentifier;

	// Check if dependency already solved
	if ((SolvedDependencies & Dependency) == Dependency)
		return SolvedDependencies;

	// Loop to find the required stages for the Dependency
	ZEUInt32 Solved = 0;
	ZESize StageCount = StageList.GetCount();
	for (ZESize I = 0; I < StageCount && Dependency != 0; I++)
	{
		// if a stage is found that is required for dependency
		ZEUInt32 Identifier = StageList[I]->GetStageIndentifier();
		
		if ((Dependency & Identifier) != Identifier)
			continue;

		// get current stage dependency and solve it
		ZEUInt32 NewDependencies = StageList[I]->GetDependencies();
		
		Solved = SolveDependency(SolvedDependencies, Identifier, NewDependencies);
		if (Dependency & Solved)
		{
			ZESize Index = GetIndex(Identifier);
			StageLookupTable[Index] = I;
		}

		Dependency &= ~Solved;
		SolvedDependencies |= Solved;
	}

	return SolvedDependencies | StageIdentifier;
}
*/

bool ZERenderer::ProcessEntry(const ZECommandBufferEntry* Entry, ZERenderStage* Stage)
{
	bool Result = false;
	switch(Entry->EntryType)
	{
		default:
		case ZE_CBET_NONE:
			break;

		case ZE_CBET_RENDER_COMMAND:
			Stage->Process((ZERenderCommand*)Entry->Data);
			break;

		case ZE_CBET_RENDERER_SETUP:
			SetRendererConfiguration((ZERendererConfiguration*)Entry->Data);
			break;

		case ZE_CBET_RENDER_STAGE_SETUP:
			Stage->SetStageConfiguration((ZERenderStageConfiguration*)Entry->Data);
			break;
	};

	return Result;
}

void ZERenderer::AddRenderCommand(const ZERenderCommand* Command)
{
	zeDebugCheck(Command == NULL, "Null pointer.");

	ZECommandBufferEntry Entry;
	Entry.Data = (const void*)Command;
	Entry.DataSize = Command->Size;
	Entry.EntryType = ZE_CBET_RENDER_COMMAND;

	CommandBuffer.AddEntry(Entry);
}

void ZERenderer::AddRendererConfiguration(const ZERendererConfiguration* Config)
{
	zeDebugCheck(Config == NULL, "Null pointer.");

	ZECommandBufferEntry Entry;
	Entry.Data = (const void*)Config;
	Entry.DataSize = Config->Size;
	Entry.EntryType = ZE_CBET_RENDERER_SETUP;

	CommandBuffer.AddEntry(Entry);
}

void ZERenderer::AddRenderStageConfiguration(const ZERenderStageConfiguration* Config)
{
	zeDebugCheck(Config == NULL, "Null pointer.");

	ZECommandBufferEntry Entry;
	Entry.Data = (const void*)Config;
	Entry.DataSize = Config->Size;
	Entry.EntryType = ZE_CBET_RENDER_STAGE_SETUP;

	CommandBuffer.AddEntry(Entry);
}

void ZERenderer::SetRendererConfiguration(const ZERendererConfiguration* Config)
{

}

void ZERenderer::SetShadowMapDimension(ZEVector2 Value)
{
	ShadowMapDimesion = Value;
}

ZEVector2 ZERenderer::GetShadowMapDimension() const
{
	return ShadowMapDimesion;
}

void ZERenderer::Render(float ElaspedTime)
{
	if (!zeGraphics->GetEnabled())
	{
		zeWarning("zeGraphics disabled.");
		return;
	}

	ZESize CommandCount = CommandBuffer.GetCommandCount();
	ZEGraphicsEventTracer* Tracer = ZEGraphicsEventTracer::GetInstance();
	/************************************************************************/
	/*                         ShadowStage                                  */
	/************************************************************************/
// 	Tracer->StartEvent("ShadowStage");
// 	for (ZESize I = 0; I < CommandCount; ++I)
// 	{
// 		const ZECommandBufferEntry* Entry = CommandBuffer.GetEntry(I);
// 		ProcessEntry(Entry, ShadowStage);
// 	}
// 	Tracer->EndEvent();
	/************************************************************************/
	/*                         GeometryStage                                */
	/************************************************************************/
	Tracer->StartEvent("GeometryStage");
	GeometryStage->Setup();
	for (ZESize I = 0; I < CommandCount; ++I)
	{
		const ZECommandBufferEntry* Entry = CommandBuffer.GetEntry(I);
		ProcessEntry(Entry, GeometryStage);
	}
	Tracer->EndEvent();
	/************************************************************************/
	/*                         LightingStage                                */
	/************************************************************************/
	Tracer->StartEvent("LightingStage");
	LightingStage->Setup();
	for (ZESize I = 0; I < CommandCount; ++I)
	{
		const ZECommandBufferEntry* Entry = CommandBuffer.GetEntry(I);
		ProcessEntry(Entry, LightingStage);
	}
	Tracer->EndEvent();
	/************************************************************************/
	/*                         ForwardStage                                 */
	/************************************************************************/
	Tracer->StartEvent("ForwardStage");
	ForwardStage->Setup();
	for (ZESize I = 0; I < CommandCount; ++I)
	{
		const ZECommandBufferEntry* Entry = CommandBuffer.GetEntry(I);
		ProcessEntry(Entry, ForwardStage);
	}
	Tracer->EndEvent();
	/************************************************************************/
	/*                         ParticleStage                                */
	/************************************************************************/
	Tracer->StartEvent("ParticleStage");
	for (ZESize I = 0; I < CommandCount; ++I)
	{
		const ZECommandBufferEntry* Entry = CommandBuffer.GetEntry(I);
		ParticleStage->Setup();
		ProcessEntry(Entry, ParticleStage);
	}
	Tracer->EndEvent();

	zeGraphics->GetDevice()->Present();

	CommandBuffer.Clear();
}

ZERenderer* ZERenderer::CreateInstance()
{
	return new ZERenderer();
}

ZERenderer::ZERenderer()
{
	ShadowMapDimesion = ZEVector2(1024.0f, 1024.0f);

// 	memset(StageStatusTable, 0, sizeof(bool) * ZE_MAX_STAGE_COUNT);
// 	memset(StageLookupTable, 0, sizeof(ZESize) * ZE_MAX_STAGE_COUNT);
}

ZERenderer::~ZERenderer()
{
	Deinitialize();
}
