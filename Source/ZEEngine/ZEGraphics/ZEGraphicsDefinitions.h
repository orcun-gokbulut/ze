//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGraphicsDefinitions.cpp
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

#include "ZEDS/ZEFlags.h"

//#define ZE_GRAPHIC_LOG_ENABLE
//#define ZE_GRAPHICS_DEVICE_DEBUG_LAYER_ENABLED
//#define ZE_GRAPHICS_CONTENT_PROTECTION

#define ZEGR_MAX_SHADER_VARIABLE_NAME		32

#define ZEGR_MAX_RENDER_TARGET_SLOT			8
#define ZEGR_MAX_VERTEX_LAYOUT_ELEMENT		16
#define ZEGR_MAX_VERTEX_BUFFER_SLOT			16
#define ZEGR_MAX_SAMPLER_SLOT				16
#define ZEGR_MAX_TEXTURE_SLOT				32
#define ZEGR_MAX_CONSTANT_BUFFER_SLOT		14
#define ZEGR_MAX_VIEWPORT_SLOT				16
#define ZEGR_MAX_SCISSOR_SLOT				16

#define	ZEGR_STATE_POOL_CACHE_CAPACITY		128

#define	ZEGR_MAX_SHADOW_COPY_COUNT			5
#define	ZEGR_RENDER_BUFFER_COUNT			2

#define ZEGR_CM_NONE			0x0
#define	ZEGR_CM_RED				0x1
#define	ZEGR_CM_GREEN			0x2
#define	ZEGR_CM_BLUE			0x4
#define	ZEGR_CM_ALPHA			0x8
#define ZEGR_CM_ALL				ZEGR_CM_RED | ZEGR_CM_GREEN | ZEGR_CM_BLUE | ZEGR_CM_ALPHA
typedef ZEFlagsBase<ZEUInt8>	ZEGRColorMask;
