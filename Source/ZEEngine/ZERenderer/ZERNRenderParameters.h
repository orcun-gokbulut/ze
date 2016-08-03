//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNRenderParameters.h
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

typedef ZEFlags ZERNRenderFlags;
#define ZERN_RF_NONE				0x0
#define ZERN_RF_DRAW				0x1
#define ZERN_RF_CULL				0x2
#define ZERN_RF_PROBE				0x4

class ZEGRContext;
ZE_META_FORWARD_DECLARE(ZEScene, "ZEGame/ZEScene.h");
ZE_META_FORWARD_DECLARE(ZERNRenderer, "ZERNRenderer.h");
ZE_META_FORWARD_DECLARE(ZERNStage, "ZERNStage.h");
ZE_META_FORWARD_DECLARE(ZERNCommand, "ZERNCommand.h");
ZE_META_FORWARD_DECLARE(ZERNView, "ZERNView.h");

ZE_ENUM(ZERNPreRenderType)
{
	ZERN_PRT_COLOR,
	ZERN_PRT_SHADOW,
	ZERN_PRT_OTHER
};

class ZERNPreRenderParameters : public ZEObject
{
	ZE_OBJECT
	public:
		ZEUInt64					FrameId;
		float						ElapsedTime;
		float						Time;
		ZERNRenderer*				Renderer;
		const ZERNView*				View;
		ZERNPreRenderType			Type;

		void						UpdateTime();

									ZERNPreRenderParameters();
};

class ZERNRenderParameters : public ZEObject
{
	ZE_OBJECT
	public:
		ZEUInt64					FrameId;
		float						ElapsedTime;
		float						Time;
		ZEGRContext*				Context;
		ZERNRenderer*				Renderer;
		const ZERNView*				View;
		const ZERNStage*			Stage;
		const ZERNCommand*			Command;

		void						UpdateTime();

									ZERNRenderParameters();
};
