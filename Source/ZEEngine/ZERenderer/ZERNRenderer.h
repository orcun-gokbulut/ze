//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNRenderer.h
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

#pragma once

#include "ZEInitializable.h"
#include "ZERNView.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEFlags.h"
#include "ZEDS/ZEList2.h"

#define ZERN_MAX_COMMAND_STAGE 5

typedef ZEFlags ZERNRenderFlags;
#define ZERN_RF_NONE				0x0
#define ZERN_RF_DRAW				0x1
#define ZERN_RF_CULL				0x2
#define ZERN_RF_PROBE				0x4

typedef ZEUInt32 ZERNStageMask, ZERNStageID;
#define ZERN_S_STAGE_ID(Id) (1<<ID)
#define ZERN_S_DEFERRED_GPASS			ZERN_S_STAGE_ID(0)
#define ZERN_S_FORWARD					ZERN_S_STAGE_ID(1)
#define ZERN_S_FORWARD_TRANSPARANT		ZERN_S_STAGE_ID(2)
#define ZERN_S_POST_HDR					ZERN_S_STAGE_ID(3)
#define ZERN_S_POST_EFFECT				ZERN_S_STAGE_ID(4)
#define ZERN_S_2D						ZERN_S_STAGE_ID(5)
#define ZERN_S_SHADOW_OMNI				ZERN_S_STAGE_ID(6)
#define ZERN_S_SHADOW_DIRECTIONAL		ZERN_S_STAGE_ID(7)
#define ZERN_S_SHADOW_PROJECTIVE		ZERN_S_STAGE_ID(8)

class ZERNRenderer;
class ZERNStage;
class ZERNCommand;
class ZEEntity;
class ZEGRContext;
class ZEScene;
class ZECamera;

enum ZERNDrawType
{
	ZERN_DT_CULL,
	ZERN_DT_DRAW_NORMAL,
	ZERN_DT_DRAW_OMNI,
	ZERN_DT_DRAW_PROJECTIVE,
	ZERN_DT_DRAW_ORTOGONAL
};

class ZERNRenderParameters
{
	public:
		ZESize						FrameId;
		float						ElapsedTime;
		float						Time;
		ZEScene*					Scene;
		ZERNRenderer*				Renderer;
		ZERNDrawType				Type;
		ZERNStageID					StageID;
		ZERNStage*					Stage;
		ZERNCommand*				Command;
};

class ZERNCommand
{
	public:
		ZEInt							Priority;
		float							Order;
		ZEUInt							StageMask;
		ZEEntity*						Entity;
		void*							ExtraParameters;

		ZELink<ZERNCommand>				StageQueueLinks[ZERN_MAX_COMMAND_STAGE];

										ZERNCommand();
};

class ZERNStageQueue
{
	public:
		ZERNStage*					Stage;
		ZEList2<ZERNCommand>		Commands;
};

class ZERNRenderer : public ZEInitializable
{
	private:
		ZEGRContext*				Device;
		ZEScene*					Scene;
		ZERNView					View;
		ZEArray<ZERNStageQueue>		StageQueues;
		ZEList2<ZERNCommand>		Commands;
		
		void						Cull();
		void						SortStageQueues();
		void						RenderStage(ZERNStageQueue* Queue);
		void						RenderStages();

		virtual bool				InitializeSelf();
		virtual void				DeinitializeSelf();

	public:
		void						SetDevice(ZEGRContext* Device);
		ZEGRContext*				GetDevice();

		void						SetView(const ZERNView& View);
		const ZERNView&				GetView();

		void						SetScene(ZEScene* Scene);
		ZEScene*					GetScene();

		void						AddCommand(ZERNCommand* Command);
		void						RemoveCommand(ZERNCommand* Command);
		void						CleanCommands();

		void						AddStage(ZERNStage* Stage);
		void						RemoveStage(ZERNStage* Stage);

		void						Render();

		void						Clear();

									ZERNRenderer();
									~ZERNRenderer();
};
