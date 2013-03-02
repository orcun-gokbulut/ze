//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERenderer.h
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
#ifndef __ZE_RENDERER_H__
#define __ZE_RENDERER_H__

#include "ZETypes.h"
#include "ZEMath/ZEVector.h"
#include "ZEDS/ZEArray.h"

#define ZE_CBET_RENDER_STAGE_SETUP		0x00000004
#define ZE_CBET_RENDERER_SETUP			0x00000002
#define ZE_CBET_RENDER_COMMAND			0x00000001
#define ZE_CBET_NONE					0x00000000
typedef ZEUInt32 ZECommandBufferEntryType;

#define	ZE_COMMAND_BUFFER_SIZE			1024 * 1024	* 1		// 1 MB

class ZECommandBufferEntry
{
	public:
		ZEUInt32					EntryType;
		ZESize						DataSize;
		const void*					Data;

									ZECommandBufferEntry();
									~ZECommandBufferEntry();
};

typedef ZEArray<ZECommandBufferEntry> ZECommandList;

class ZECommandBuffer
{
	private:
		void*						Buffer;
		ZESize						EndOfBuffer;

		ZECommandList				CommandList;

	public:
		void						Clear();

		bool						BufferEmpty() const;
		ZESize						GetCommandCount() const;

		const ZECommandBufferEntry*	GetEntry(ZESize Index);
		bool						AddEntry(const ZECommandBufferEntry& Entry);

									ZECommandBuffer();
									~ZECommandBuffer();
};

/************************************************************************/
/*							 RENDERER SETUP								*/
/************************************************************************/
class ZERenderStage;

class ZERendererConfiguration
{
	public:
		ZESize					Size;
		ZEArray<ZERenderStage*>	StageList;
		
								ZERendererConfiguration(){};
								~ZERendererConfiguration(){};
};


class ZERenderStage;
class ZERenderStageConfiguration;
class ZERenderCommand;

class ZERenderStageShadow;
class ZERenderStageForward;
class ZERenderStageGeometry;
class ZERenderStageLighting;
class ZERenderStageParticle;

#define ZE_MAX_STAGE_COUNT		32

class ZERenderer
{
	protected:
		ZECommandBuffer			CommandBuffer;

		/*
		ZEArray<ZERenderStage*>	StageList;
		ZESize					StageLookupTable[ZE_MAX_STAGE_COUNT];
		bool					StageStatusTable[ZE_MAX_STAGE_COUNT];
		*/

		ZERenderStageShadow*	ShadowStage;
		ZERenderStageForward*	ForwardStage;
		ZERenderStageGeometry*	GeometryStage;
		ZERenderStageLighting*	LightingStage;
		ZERenderStageParticle*	ParticleStage;

		ZEVector2				ShadowMapDimesion;

		bool					ProcessEntry(const ZECommandBufferEntry* Entry, ZERenderStage* Stage);

								ZERenderer();
		virtual					~ZERenderer();

	public:
		virtual void			AddRenderCommand(const ZERenderCommand* Command);
		virtual void			AddRendererConfiguration(const ZERendererConfiguration* Config);
		virtual void			AddRenderStageConfiguration(const ZERenderStageConfiguration* Config);
		
		virtual void			SetRendererConfiguration(const ZERendererConfiguration* Config);

		void					SetShadowMapDimension(ZEVector2 Value);
		ZEVector2				GetShadowMapDimension() const;

		virtual void			Render(float ElaspedTime);

		virtual bool			Initialize();
		virtual void			Deinitialize();

		virtual void			Destroy();

		static ZERenderer*		CreateInstance();
};

#endif
