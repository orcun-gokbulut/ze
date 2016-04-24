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

#include "ZEMeta/ZEObject.h"
#include "ZEInitializable.h"

#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEList2.h"
#include "ZEPointer/ZEHolder.h"
#include "ZERNView.h"
#include "ZERNStageID.h"

ZE_META_FORWARD_DECLARE(ZEScene, "ZEGame/ZEScene.h");
ZE_META_FORWARD_DECLARE(ZERNStage, "ZERNStage.h");
class ZERNCommand;
class ZEGRContext;
class ZEGRRenderTarget;

class ZERNRenderer : public ZEObject, public ZEInitializable
{
	ZE_OBJECT
	private:
		ZEGRContext*					Context;
		ZERNView						View;
		ZEGRRenderTarget*				OutputRenderTarget;
		ZEHolder<ZEGRConstantBuffer>	ViewConstantBuffer;
		ZEHolder<ZEGRConstantBuffer>	RendererConstantBuffer;
		ZEHolder<ZEGRConstantBuffer>	SceneConstantBuffer;
		ZEList2<ZERNStage>				Stages;

		struct RendererConstants
		{
			float						Elapsedtime;
			float						Time;
			ZEUInt32					FrameId;
			ZEUInt32					Reserved0;
			ZEVector2					OutputSize;
			ZEVector2					InvOutputSize;
			ZEMatrix3x3Shader			ScreenTransform;
		} RendererConstants;

		void							SortStageCommands();
		void							RenderStage(ZERNStage* Queue);
		void							RenderStages();

		void							UpdateConstantBuffers();

		virtual bool					InitializeSelf();
		virtual void					DeinitializeSelf();

	public:
		void							SetContext(ZEGRContext* Context);
		ZEGRContext*					GetContext();

		void							SetView(const ZERNView& View);
		const ZERNView&					GetView();

		void							SetOutputRenderTarget(ZEGRRenderTarget* Output);
		ZEGRRenderTarget*				GetOutputRenderTarget();

		const ZEList2<ZERNStage>&		GetStages();
		ZERNStage*						GetStage(ZERNStageID Id);
		ZERNStage*						GetStage(ZEClass* Class);
		void							AddStage(ZERNStage* Stage);
		void							RemoveStage(ZERNStage* Stage);
		void							CleanStages();

		void							PreRenderScene(ZEScene* Scene);

		void							AddCommand(ZERNCommand* Command);
		void							RemoveCommand(ZERNCommand* Command);
		void							CleanCommands();
		bool							ContainsCommand(ZERNCommand* Command);

		void							Render(float ElapsedTime);

										ZERNRenderer();
		virtual							~ZERNRenderer();
};
