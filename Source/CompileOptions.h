//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - CompileOptions.h
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
#ifndef __ZE_COMPILE_OPTIONS_H__
#define __ZE_COMPILE_OPTIONS_H__

#ifndef ZE_ZINEK_ENGINE
#define ZE_ZINEK_ENGINE
#endif

#define ZE_MAKE_VERSION(Major, Minor, Build) ((unsigned int)(Major) << 24 | (unsigned int)(Minor)<<16 | (Build))
#define ZE_VERSION_MAJORNUMBER 0
#define ZE_VERSION_MINORNUMBER 5
#define ZE_VERSION_NUMBER ZE_MAKE_VERSION(ZE_VERSION_MAJORNUMBER, ZE_VERSION_MINORNUMBER, 0)
#define ZE_VERSION_STRING "0.5"

//#define ZE_DEBUG_ENABLED
#define ZESTATISTICS_ENABLED

#define ZE_PLATFORM_X86
//#define ZE_PLATFORM_X64

#define ZE_PLATFORM_WINDOWS
//#define ZE_PLATFORM_WINDOWS_9X
//#define ZE_PLATFORM_WINDOES_2000
#define ZE_PLATFORM_WINDOWS_XP
#define ZE_PLATFORM_WINDOWS_VISTA
#define ZE_PLATFOMR_WINDOWS_7

//#define ZE_PLATFORM_LINUX
//#define ZE_PLATFORM_MACOSX
//#define ZE_PLATFORM_PS2
//#define ZE_PLATFORM_PS3
//#define ZE_PLATFORM_XBOX360
//#define ZE_PLATFORM_WII

#define ZE_PLATFORM_LITTLE_ENDIAN
//#define ZE_PLATFORM_BIG_ENDIAN

#define ZE_PLATFORM_MMX_ENABLED
#define ZE_PLATFORM_SSE_ENABLED
#define ZE_PLATFORM_SSE2_ENABLED
//#define ZE_PLATFORM_SSE3_ENABLED
//#define ZE_PLATFORM_SSE4_ENABLED

#endif
