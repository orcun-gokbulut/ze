//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageID.h
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

#include "ZETypes.h"

typedef ZEUInt32 ZERNStageMask;
typedef ZEUInt64 ZERNStageID;

// Normal
#define ZERN_STAGE_ID(ID)						(1<<ID)
#define ZERN_STAGE_GBUFFER						ZERN_STAGE_ID(1)
#define ZERN_STAGE_LIGHTING						ZERN_STAGE_ID(2)
#define ZERN_STAGE_AO							ZERN_STAGE_ID(3)
#define ZERN_STAGE_FORWARD						ZERN_STAGE_ID(4)
#define ZERN_STAGE_FORWARD_TRANSPARENT			ZERN_STAGE_ID(5)
#define ZERN_STAGE_PRE_HDR_EFFECT				ZERN_STAGE_ID(6)
#define ZERN_STAGE_HDR							ZERN_STAGE_ID(7)
#define ZERN_STAGE_POST_EFFECT					ZERN_STAGE_ID(8)
#define ZERN_STAGE_2D							ZERN_STAGE_ID(9)
#define ZERN_STAGE_POST_2D_EFFECT				ZERN_STAGE_ID(10)
#define ZERN_STAGE_ANTI_ALIASING				ZERN_STAGE_ID(11)
#define ZERN_STAGE_SHADOWING					ZERN_STAGE_ID(12)
#define ZERN_STAGE_SHADOW_MAP_GENERATION		ZERN_STAGE_ID(13)
#define ZERN_STAGE_TEXTURE_OUTPUT				ZERN_STAGE_ID(14)
#define ZERN_STAGE_PRE_EFFECT					ZERN_STAGE_ID(15)
#define ZERN_STAGE_PARTICLE_RENDERING			ZERN_STAGE_ID(16)
#define ZERN_STAGE_OUTPUT						ZERN_STAGE_ID(17)
#define ZERN_STAGE_DEBUG						ZERN_STAGE_ID(18)
#define ZERN_STAGE_INPUT						ZERN_STAGE_ID(19)
#define ZERN_STAGE_RESOLVING					ZERN_STAGE_ID(20)
#define ZERN_STAGE_RENDER_DEPTH					ZERN_STAGE_ID(21)
#define ZERN_STAGE_ATMOSPHERE					ZERN_STAGE_ID(22)
