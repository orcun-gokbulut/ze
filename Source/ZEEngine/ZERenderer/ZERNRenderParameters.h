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
#include "ZEDS/ZEFlags.h"
#include "ZEExport.ZEEngine.h"

class ZEGRContext;
ZEMT_FORWARD_DECLARE(ZEScene);
ZEMT_FORWARD_DECLARE(ZERNRenderer);
ZEMT_FORWARD_DECLARE(ZERNStage);
ZEMT_FORWARD_DECLARE(ZERNCommand);
ZEMT_FORWARD_DECLARE(ZERNView);
ZEMT_FORWARD_DECLARE(ZETimeParameters);

ZE_EXPORT_ZEENGINE ZE_ENUM(ZERNRenderType)
{
	ZERN_RT_COLOR,
	ZERN_RT_SHADOW,
	ZERN_RT_ENVIRONMENT,
	ZERN_RT_OTHER
};

ZE_EXPORT_ZEENGINE ZE_ENUM(ZERNRenderFlag)
{
	ZERN_RF_NONE				= 0x00,
	ZERN_RF_STERIO				= 0x10,
	ZERN_RF_STERIO_LEFT			= 0x11,
	ZERN_RF_STERIO_RIGHT		= 0x12,
	ZERN_RF_STERIO_FIRST_PASS	= 0x11,
	ZERN_RF_STERIO_SECOND_PASS	= 0x12,
	ZERN_RF_CUBE				= 0x20,
	ZERN_RF_CUBE_NX				= 0x21,
	ZERN_RF_CUBE_PX				= 0x22,
	ZERN_RF_CUBE_NY				= 0x23,
	ZERN_RF_CUBE_PY				= 0x24,
	ZERN_RF_CUBE_NZ				= 0x25,
	ZERN_RF_CUBE_PZ				= 0x26
};
typedef ZEFlags ZERNRenderFlags;

class ZE_EXPORT_ZEENGINE ZERNPreRenderParameters : public ZEObject
{
	ZE_OBJECT
	public:
		ZERNRenderer*				Renderer;
		const ZERNView*				View;
		ZERNRenderType				Type;
		ZERNRenderFlags				Flags;
		const ZETimeParameters*		TimeParameters;

		void						UpdateTime();

									ZERNPreRenderParameters();
};

class ZE_EXPORT_ZEENGINE ZERNRenderParameters : public ZEObject
{
	ZE_OBJECT
	public:
		ZEGRContext*				Context;
		ZERNRenderType				Type;
		ZERNRenderFlags				Flags;
		ZERNRenderer*				Renderer;
		const ZERNView*				View;
		const ZERNStage*			Stage;
		const ZERNCommand*			Command;
		const ZETimeParameters*		TimeParameters;

		void						UpdateTime();

									ZERNRenderParameters();
};
