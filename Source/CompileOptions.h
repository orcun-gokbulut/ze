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

#define ZE_ZINEKENGINE

#define ZEMAKEVERSION(Major, Minor, Build) ((unsigned int)(Major) << 24 | (unsigned int)(Minor)<<16 | (Build))
#define ZEVERSION_MAJORNUMBER 0
#define ZEVERSION_MINORNUMBER 5
#define ZEVERSION_NUMBER ZEMAKEVERSION(ZEVERSION_MAJORNUMBER, ZEVERSION_MINORNUMBER, 0)
#define ZEVERSION_STRING "0.5"

//#define ZEDEBUG_ENABLED
#define ZESTATISTICS_ENABLED

#define ZEPLATFORM_X86
//#define ZEPLATFORM_X64

#define ZEPLATFORM_WINDOWS
//#define ZEPLATFORM_LINUX
//#define ZEPLATFORM_MACOSX
//#define ZEPLATFORM_PS2
//#define ZEPLATFORM_PS3
//#define ZEPLATFORM_XBOX360
//#define ZEPLATFORM_WII


#define ZESSE_ENABLED
#define ZESSE2_ENABLED
//#define ZESSE3_ENABLED
//#define ZESSE4_ENABLED

#endif
