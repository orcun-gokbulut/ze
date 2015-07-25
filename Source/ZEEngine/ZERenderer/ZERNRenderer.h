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
#include "ZEDS/ZEList.h"

typedef ZEFlags ZERNRenderFlags;
#define ZERN_RF_NONE				0x0
#define ZERN_RF_DRAW				0x1
#define ZERN_RF_CULL				0x2
#define ZERN_RF_PROBE				0x4

typedef ZEFlags ZERNRenderPasses;
#define ZERN_RP_DEFERRED_GPASS		0x0001
#define ZERN_RP_FORWARD				0x0002
#define ZERN_RP_FORWARD_TRANSPARANT	0x0004
#define ZERN_RP_POST_HDR			0x0008
#define ZERN_RP_POST_EFFECT			0x0010
#define ZERN_RP_2D					0x0020
#define ZERN_RP_SHADOW_OMNI			0x0040
#define ZERN_RP_SHADOW_DIRECTIONAL	0x0080
#define ZERN_RP_SHADOW_PROJECTIVE	0x0080

class ZEEntity;
class ZEGRContext;
class ZEScene;
class ZECamera;
class ZERNStage;

class ZERNRenderInfo : public ZEListItem
{
	public:
		ZERNRenderFlags				Flags;
		ZERNRenderPasses			Passes;
		ZEUInt						Priority;
		float						Order;
		ZEEntity*					Entity;

									ZERNRenderInfo();
};


// Cull Entities
// Order Entities
// Fill Stages

class ZE

class ZERNRenderer : public ZEInitializable
{
	private:
		ZEGRContext*				Device;
		ZEScene*					Scene;
		ZEArray<ZERNStage*>			Stages;
		ZERNView					View;
		ZEList<ZERNRenderInfo>		RenderInfos;			

		virtual bool				InitializeSelf();
		virtual void				DeinitializeSelf();

	public:
		void						SetDevice(ZEGRContext* Device);
		ZEGRContext*				GetDevice();

		void						SetView(const ZERNView& View);
		const ZERNView&				GetView();

		void						SetScene(ZEScene* Scene);
		ZEScene*					GetScene();

		void						AddStage(ZERNStage* Stage);
		void						RemoveStage(ZERNStage* Stage);

		void						Render(float ElaspedTime = 0);

		void						Clear();

									ZERNRenderer();
									~ZERNRenderer();
};
