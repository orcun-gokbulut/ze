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

#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEList2.h"
#include "ZERNView.h"
#include "ZERNStageID.h"
#include "ZEGraphics/ZEGRHolder.h"

class ZEScene;
class ZERNStage;
class ZERNCommand;
class ZEGRContext;
class ZEGRRenderTarget;

class ZERNStageQueue
{
	public:
		ZERNStage*						Stage;
		ZEList2<ZERNCommand>			Commands;
};

class ZERNRenderer : public ZEInitializable
{
	private:
		ZEGRContext*					Context;
		ZEScene*						Scene;
		ZERNView						View;
		ZEGRRenderTarget*				OutputRenderTarget;
		ZEGRHolder<ZEGRConstantBuffer>	ViewConstantBuffer;
		ZEGRHolder<ZEGRConstantBuffer>	RendererConstantBuffer;
		ZEArray<ZERNStageQueue>			StageQueues;

		struct RendererConstants
		{
			float Elapsedtime;
			float Reserved[3];
		}Constants;

		void							Cull();
		void							SortStageQueues();
		void							RenderStage(ZERNStageQueue* Queue);
		void							RenderStages();

		void							UpdateViewConstantBuffer();

		virtual bool					InitializeSelf();
		virtual void					DeinitializeSelf();

	public:
		void							SetContext(ZEGRContext* Context);
		ZEGRContext*					GetContext();

		void							SetView(const ZERNView& View);
		const ZERNView&					GetView();

		void							SetScene(ZEScene* Scene);
		ZEScene*						GetScene();

		void							SetOutputRenderTarget(ZEGRRenderTarget* Output);
		ZEGRRenderTarget*				GetOutputRenderTarget();

		ZEArray<ZERNStage*>				GetStages();
		ZERNStage*						GetStage(ZERNStageID Id);
		void							AddStage(ZERNStage* Stage);
		void							RemoveStage(ZERNStage* Stage);
		void							CleanStages();

		void							AddCommand(ZERNCommand* Command);
		void							RemoveCommand(ZERNCommand* Command);
		void							CleanCommands();
		bool							ContainsCommand(ZERNCommand* Command);

		void							Render(float ElapsedTime);

										ZERNRenderer();
		virtual							~ZERNRenderer();
};
