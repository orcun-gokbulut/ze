//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRVertexElementType.h
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
#include "ZEPacking.h"

ZEPackStruct(
struct ZEInt8_2
{
	union
	{
		struct
		{
			ZEInt8			x;
			ZEInt8			y;
		};
		ZEInt8				M[2];
		ZEUInt16			Packed;
	};
});

ZEPackStruct(
struct ZEInt8_4
{
	union
	{
		struct
		{
			ZEInt8			x;
			ZEInt8			y;
			ZEInt8			z;
			ZEInt8			w;
		};
		ZEInt8				M[4];
		ZEUInt32			Packed;
	};
});

ZEPackStruct(
struct ZEUInt8_2
{
	union
	{
		struct
		{
			ZEUInt8			x;
			ZEUInt8			y;
		};
		ZEUInt8				M[2];
		ZEUInt16			Packed;
	};
});

ZEPackStruct(
struct ZEUInt8_4
{
	union
	{
		struct
		{
			ZEUInt8			x;
			ZEUInt8			y;
			ZEUInt8			z;
			ZEUInt8			w;
		};
		ZEUInt8				M[4];
		ZEUInt32			Packed;
	};
});

ZEPackStruct(
struct ZEInt16_2
{
	union 
	{
		struct
		{
			ZEInt16			x;
			ZEInt16			y;
		};
		ZEInt16				M[2];
		ZEUInt32			Packed;
	};
});

ZEPackStruct(
struct ZEInt16_4
{
	union
	{
		struct
		{
			ZEInt16			x;
			ZEInt16			y;
			ZEInt16			z;
			ZEInt16			w;
		};
		ZEInt16				M[4];
		ZEUInt64			Packed;

	};
});

ZEPackStruct(
struct ZEUInt16_2
{
	union
	{
		struct
		{
			ZEUInt16		x;
			ZEUInt16		y;
		};
		ZEUInt16			M[2];
		ZEUInt32			Packed;
	};
});

ZEPackStruct(
struct ZEUInt16_4
{
	union 
	{
		struct
		{
			ZEUInt16		x;
			ZEUInt16		y;
			ZEUInt16		z;
			ZEUInt16		w;
		};
		ZEUInt16			M[4];
		ZEUInt64			Packed;
	};
});

ZEPackStruct(
struct ZEInt32_2
{
	union 
	{
		struct
		{
			ZEInt32			x;
			ZEInt32			y;
		};
		ZEInt32				M[2];
		ZEUInt64			Packed;
	};
});

ZEPackStruct(
struct ZEInt32_3
{
	union
	{
		struct
		{
			ZEInt32			x;
			ZEInt32			y;
			ZEInt32			z;
		};
		ZEInt32				M[3];
	};
});

ZEPackStruct(
struct ZEInt32_4
{
	union 
	{
		struct
		{
			ZEInt32			x;
			ZEInt32			y;
			ZEInt32			z;
			ZEInt32			w;
		};
		ZEInt32				M[4];
	};
});

ZEPackStruct(
struct ZEUInt32_2
{
	union
	{
		struct
		{
			ZEUInt32		x;
			ZEUInt32		y;
		};
		ZEUInt32			M[2];
	};
});

ZEPackStruct(
struct ZEUInt32_3
{
	union
	{
		struct
		{
			ZEUInt32		x;
			ZEUInt32		y;
			ZEUInt32		z;
		};
		ZEUInt32			M[3];
	};
});

ZEPackStruct(
struct ZEUInt32_4
{
	union
	{
		struct
		{
			ZEUInt32		x;
			ZEUInt32		y;
			ZEUInt32		z;
			ZEUInt32		w;
		};
		ZEUInt32			M[4];
	};
});
