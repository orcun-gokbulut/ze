//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFlags.h
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
#include "ZEError.h"

template <typename Type = ZEUInt>
class ZEFlagsBase
{
	public:
		Type Value;

		inline void RaiseBit(ZESize Index)
		{
			Value |= (0x1 << Index);
		}

		inline void UnraiseBit(ZESize Index)
		{
			Value &= ~(0x1 << Index);
		}

		inline void SetBit(ZESize Index, bool Raise)
		{
			if (Raise)
				RaiseBit(Index);
			else
				UnraiseBit(Index);
		}

		inline bool GetBit(ZESize Index) const
		{
			zeDebugCheck(Index > 31, "ZEFlags::GetBit(ZESize Index) index can't be greater than 31.");

			return (Value & (0x1 << Index)) != 0;
		}

		inline void SetFlags(ZEInt Flags, bool Raise)
		{
			if (Raise)
				RaiseFlags(Flags);
			else
				UnraiseFlags(Flags);
		}

		inline void RaiseFlags(ZEInt Flags)
		{
			Value |= Flags;
		}

		inline void UnraiseFlags(Type Flags)
		{
			Value &= ~Flags;

		}

		inline void RaiseAll()
		{
			Value = 0xFFFFFFFF;
		}

		inline void UnraiseAll()
		{
			Value = 0;
		}

		inline bool GetFlags(ZEInt Flags) const
		{
			return (Value & Flags) == Flags;
		}

		inline bool operator[](ZESize Index) const
		{
			return GetBit(Index);
		}

		inline operator Type() const
		{
			return Value;
		}

		inline ZEFlagsBase<Type>& operator&=(const ZEFlagsBase<Type>& Other)
		{
			this->Value &= Other.Value;
			return *this;
		}

		inline ZEFlagsBase<Type>& operator|=(const ZEFlagsBase<Type>& Other)
		{
			this->Value |= Other.Value;
			return *this;
		}

		inline ZEFlagsBase<Type>& operator^=(const ZEFlagsBase<Type>& Other)
		{
			this->Value ^= Other.Value;
			return *this;
		}

		inline ZEFlagsBase<Type>& operator=(const ZEFlagsBase<Type>& Other)
		{
			this->Value = Other.Value;
			return *this;
		}

		inline ZEFlagsBase()
		{
			Value = 0;
		}

		inline ZEFlagsBase(Type Value)
		{
			this->Value = Value;
		}

		inline ZEFlagsBase(const ZEFlagsBase<Type>& Value)
		{
			this->Value = Value;
		}
};

typedef ZEFlagsBase<ZEUInt> ZEFlags;

