//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETypes.h
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

#include <stddef.h>

typedef signed char			ZEInt8;
typedef signed short int	ZEInt16;
typedef signed int			ZEInt32;
typedef signed long long	ZEInt64;
typedef signed int			ZEInt;

typedef unsigned char		ZEUInt8;
typedef unsigned short int	ZEUInt16;
typedef unsigned int		ZEUInt32;
typedef unsigned long long	ZEUInt64;
typedef unsigned int		ZEUInt;

typedef size_t				ZESize;
typedef ptrdiff_t			ZESSize;

typedef ZEUInt8				ZEBYTE;
typedef ZEUInt16			ZEWORD;
typedef ZEUInt32			ZEDWORD;
typedef ZEUInt64			ZEQWORD;

typedef ZEUInt8				ZEBool8;
typedef ZEUInt16			ZEBool16;
typedef ZEUInt32			ZEBool32;
typedef ZEUInt64			ZEBool64;

#define ZE_SIZE_KB 1024
#define ZE_SIZE_MB 1048576
#define ZE_SIZE_GB 1073741824
#define ZE_SIZE_TB 1099511627776
