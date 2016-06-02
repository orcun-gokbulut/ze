//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEEndian.h
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

#ifndef __ZE_ENDIAN_H__
#define __ZE_ENDIAN_H__

#include "ZECommon.h"
#include "ZETypes.h"
#include "ZESwap.h"
#include "ZEPlatform.h"

#ifdef ZE_PLATFORM_COMPILER_MSVC
#include <intrin.h>
#endif

enum ZEEndianness
{
	ZE_ET_LITTLE,
	ZE_ET_BIG
};

class ZEEndian
{
	public:
		static ZE_INLINE ZEEndianness GetEndianness()
		{
			#ifdef ZE_PLATFORM_ENDIANNESS_LITTLE
				return ZE_ET_LITTLE;
			#endif

			#ifdef ZE_PLATFORM_ENDIANNESS_BIG
				return ZE_ET_BIG;
			#endif
		}

		static ZE_INLINE ZEUInt8 Little(ZEUInt8 Value)
		{
			#ifdef ZE_PLATFORM_ENDIANNESS_LITTLE
				return Value;
			#endif

			#ifdef ZE_PLATFORM_ENDIANNESS_BIG
				return Value;
			#endif
		}

		static ZE_INLINE ZEUInt16 Little(ZEUInt16 Value)
		{
			#ifdef ZE_PLATFORM_ENDIANNESS_LITTLE
				return Value;
			#endif

			#ifdef ZE_PLATFORM_ENDIANNESS_BIG
				return ZESwap::Swap(Value);
			#endif
		}

		static ZE_INLINE ZEUInt32 Little(ZEUInt32 Value)
		{
			#ifdef ZE_PLATFORM_ENDIANNESS_LITTLE
				return Value;
			#endif

			#ifdef ZE_PLATFORM_ENDIANNESS_BIG
				return ZESwap::Swap(Value);
			#endif
		}

		static ZE_INLINE ZEUInt64 Little(ZEUInt64 Value)
		{
			#ifdef ZE_PLATFORM_ENDIANNESS_LITTLE
				return Value;
			#endif

			#ifdef ZE_PLATFORM_ENDIANNESS_BIG
				return ZESwap::Swap(Value);
			#endif
		}

		static ZE_INLINE ZEInt8 Little(ZEInt8 Value)
		{
			#ifdef ZE_PLATFORM_ENDIANNESS_LITTLE
				return Value;
			#endif

			#ifdef ZE_PLATFORM_ENDIANNESS_BIG
				return Value;
			#endif
		}

		static ZE_INLINE ZEInt16 Little(ZEInt16 Value)
		{
			#ifdef ZE_PLATFORM_ENDIANNESS_LITTLE
				return Value;
			#endif

			#ifdef ZE_PLATFORM_ENDIANNESS_BIG
				return ZESwap::Swap((ZEUInt16)Value);
			#endif
		}

		static ZE_INLINE ZEInt32 Little(ZEInt32 Value)
		{
			#ifdef ZE_PLATFORM_ENDIANNESS_LITTLE
				return Value;
			#endif

			#ifdef ZE_PLATFORM_ENDIANNESS_BIG
				return ZESwap::Swap((ZEUInt32)Value);
			#endif
		}

		static ZE_INLINE ZEInt64 Little(ZEInt64 Value)
		{
			#ifdef ZE_PLATFORM_ENDIANNESS_LITTLE
				return Value;
			#endif

			#ifdef ZE_PLATFORM_ENDIANNESS_BIG
				return ZESwap::Swap((ZEUInt64)Value);
			#endif
		}

		static ZE_INLINE float Little(float Value)
		{
			#ifdef ZE_PLATFORM_ENDIANNESS_LITTLE
				return Value;
			#endif

			#ifdef ZE_PLATFORM_ENDIANNESS_BIG
				ZEUInt32 Temp = *(ZEUInt32*)&Value;
				Temp = ZESwap::Swap((ZEUInt32)Temp);
				return *(float*)&Temp;
			#endif
		}

		static ZE_INLINE double Little(double Value)
		{
			#ifdef ZE_PLATFORM_ENDIANNESS_LITTLE
				return Value;
			#endif

			#ifdef ZE_PLATFORM_ENDIANNESS_BIG
				ZEUInt64 Temp = *(ZEUInt64*)&Value;
				Temp = ZESwap::Swap((ZEUInt64)Temp);
				return *(double*)&Temp;
			#endif
		}

		static ZE_INLINE ZEUInt8 Big(ZEUInt8 Value)
		{
			#ifdef ZE_PLATFORM_ENDIANNESS_LITTLE
				return Value;
			#endif

			#ifdef ZE_PLATFORM_ENDIANNESS_BIG
				return Value;
			#endif
		}

		static ZE_INLINE ZEUInt16 Big(ZEUInt16 Value)
		{
			#ifdef ZE_PLATFORM_ENDIANNESS_LITTLE
				return ZESwap::Swap(Value);
			#endif

			#ifdef ZE_PLATFORM_ENDIANNESS_BIG
				return Value;
			#endif
		}

		static ZE_INLINE ZEUInt32 Big(ZEUInt32 Value)
		{
			#ifdef ZE_PLATFORM_ENDIANNESS_LITTLE
				return ZESwap::Swap(Value);
			#endif

			#ifdef ZE_PLATFORM_ENDIANNESS_BIG
				return Value;
			#endif
		}

		static ZE_INLINE ZEUInt64 Big(ZEUInt64 Value)
		{
			#ifdef ZE_PLATFORM_ENDIANNESS_LITTLE
				return ZESwap::Swap(Value);
			#endif

			#ifdef ZE_PLATFORM_ENDIANNESS_BIG
				return Value;
			#endif
		}

		static ZE_INLINE ZEInt8 Big(ZEInt8 Value)
		{
			#ifdef ZE_PLATFORM_ENDIANNESS_LITTLE
				return Value;
			#endif

			#ifdef ZE_PLATFORM_ENDIANNESS_BIG
				return Value;
			#endif
		}

		static ZE_INLINE ZEInt16 Big(ZEInt16 Value)
		{
			#ifdef ZE_PLATFORM_ENDIANNESS_LITTLE
				return ZESwap::Swap((ZEUInt16)Value);
			#endif

			#ifdef ZE_PLATFORM_ENDIANNESS_BIG
				return Value;
			#endif
		}

		static ZE_INLINE ZEInt32 Big(ZEInt32 Value)
		{
			#ifdef ZE_PLATFORM_ENDIANNESS_LITTLE
				return ZESwap::Swap((ZEUInt32)Value);
			#endif

			#ifdef ZE_PLATFORM_ENDIANNESS_BIG
				return Value;
			#endif
		}

		static ZE_INLINE ZEInt64 Big(ZEInt64 Value)
		{
			#ifdef ZE_PLATFORM_ENDIANNESS_LITTLE
				return ZESwap::Swap((ZEUInt64)Value);
			#endif

			#ifdef ZE_PLATFORM_ENDIANNESS_BIG
				return Value;
			#endif
		}

		static ZE_INLINE float Big(float Value)
		{
			#ifdef ZE_PLATFORM_ENDIANNESS_LITTLE
				ZEUInt32 Temp = *(ZEUInt32*)&Value;
				Temp = ZESwap::Swap((ZEUInt32)Temp);
				return *(float*)&Temp;
			#endif

			#ifdef ZE_PLATFORM_ENDIANNESS_BIG
				return Value;
			#endif
		}

		static ZE_INLINE double Big(double Value)
		{
			#ifdef ZE_PLATFORM_ENDIANNESS_LITTLE
				ZEUInt64 Temp = *(ZEUInt64*)&Value;
				Temp = ZESwap::Swap((ZEUInt64)Temp);
				return *(double*)&Temp;
			#endif

			#ifdef ZE_PLATFORM_ENDIANNESS_BIG
				return Value;
			#endif
		}

		template<typename ZEType>
		static ZE_INLINE ZEType Uni(ZEType Value, ZEEndianness Endianness)
		{
			if (Endianness == ZE_ET_BIG)
				return ZEEndian::Big(Value);
			else
				return ZEEndian::Little(Value);
		}
};

template<typename ZEType>
class ZEBigEndian
{
	private:
		ZEType Value;

	public:
		operator ZEType()
		{
			return ZEEndian::Big(Value);
		}

		ZEBigEndian(const ZEType& Value)
		{
			this->Value = ZEEndian::Big(Value);
		}

		ZEBigEndian()
		{

		}
};

template<typename ZEType>
class ZELittleEndian
{
	private:
		ZEType Value;

	public:
		operator ZEType()
		{
			return ZEEndian::Little(Value);
		}

		ZELittleEndian(const ZEType& Value)
		{
			this->Value = ZEEndian::Little(Value);
		}

		ZELittleEndian()
		{

		}
};

#endif
