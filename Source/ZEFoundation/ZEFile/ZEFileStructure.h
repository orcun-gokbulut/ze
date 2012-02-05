//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFileStructure.h
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
#ifndef	__ZE_FILE_STRUCTURE_H__
#define __ZE_FILE_STRUCTURE_H__

#include "ZETypes.h"
#if defined(ZE_PLATFORM_COMPILER_MSVC)
	#define ZEPackStruct(Declaration) __pragma(pack(push, 4)) Declaration __pragma(pack(pop))
#elif defined(ZE_PLATFORM_COMPILER_GCC)
	#define ZEPackStruct(Declaration) Declaration __attribute__((__packed__))
#endif

class ZEEndian
{
	public:
		static ZEUInt16 ToLittle(ZEUInt16 Value);
		static ZEUInt32 ToLittle(ZEUInt32 Value);
		static ZEUInt64 ToLittle(ZEUInt64 Value);

		static ZEInt16 ToLittle(ZEInt16 Value);
		static ZEInt32 ToLittle(ZEInt32 Value);
		static ZEInt64 ToLittle(ZEInt64 Value);

		static ZEUInt16 ToBig(ZEUInt16 Value);
		static ZEUInt32 ToBig(ZEUInt32 Value);
		static ZEUInt64 ToBig(ZEUInt64 Value);

		static ZEInt16 ToBig(ZEInt16 Value);
		static ZEInt32 ToBig(ZEInt32 Value);
		static ZEInt64 ToBig(ZEInt64 Value);
};

#endif
