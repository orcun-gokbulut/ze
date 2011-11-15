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

#ifndef __ZE_FLAGS_H__
#define __ZE_FLAGS_H__

#include "ZETypes.h"

template <typename Type = ZEUInt>
class ZEFlagsBase
{
	public:
		Type Value;

		inline void RaiseBit(size_t Index)
		{
			Value |= (0x1 << Index);
		}

		inline void UnraiseBit(size_t Index)
		{
			Value &= ~(0x1 << Index);
		}

		inline void SetBit(size_t Index, bool Raise)
		{
			if (Raise)
				RaiseBit(Index);
			else
				UnraiseBit(Index);
		}

		inline bool GetBit(size_t Index) const
		{
			return (Value & (0x1 << Index)) != 0;
		}

		inline void SetFlags(int Flags, bool Raise)
		{
			if (Raise)
				RaiseFlags(Flags);
			else
				UnraiseFlags(Flags);
		}

		inline void RaiseFlags(int Flags)
		{
			Value |= Value;
		}

		inline void UnraiseFlags(Type Flags)
		{
			Value &= ~Flags;

		}

		inline bool GetFlags(int Flags) const
		{
			return (Value & Flags) == Flags;
		}

		inline bool operator[](size_t Index) const
		{
			return GetBit(Index);
		}

		inline operator Type() const
		{
			return Value;
		}

		inline ZEFlagsBase<Type> operator&(const Type& Other) const
		{
			return ZEFlagsBase<Type>(this->Value & Other);		
		}

		inline ZEFlagsBase<Type> operator&(const ZEFlagsBase<Type>& Other) const
		{
			ZEFlagsBase<Type>(this->Value & Other->Value);
			return *this;
		}

		inline ZEFlagsBase<Type>& operator&=(const Type& Other)
		{
			this->Value &= Other;
			return *this;
		}

		inline ZEFlagsBase<Type>& operator&=(const ZEFlagsBase<Type>& Other)
		{
			this->Value &= Other->Value;
			return *this;
		}

		inline ZEFlagsBase<Type> operator|(const Type& Other) const
		{
			return ZEFlagsBase<Type>(this->Value | Other);
		}

		inline ZEFlagsBase<Type> operator|(const ZEFlagsBase<Type>& Other) const
		{
			return ZEFlagsBase<Type>(this->Value | Other->Value);
		}

		inline ZEFlagsBase<Type>& operator|=(const Type& Other)
		{
			this->Value |= Other;
			return *this;
		}

		inline ZEFlagsBase<int>& operator|=(const ZEFlagsBase<Type>& Other)
		{
			this->Value |= Other->Value;
			return *this;
		}

		inline ZEFlagsBase<Type> operator^(const Type& Other) const
		{
			return ZEFlagsBase<Type>(this->Value ^ Other);
		}

		inline ZEFlagsBase<Type> operator^(const ZEFlagsBase<Type>& Other) const
		{
			return ZEFlagsBase<Type>(this->Value ^ Other->Value);
		}

		inline ZEFlagsBase<Type>& operator^=(const Type& Other)
		{
			this->Type ^= Other;
			return *this;
		}

		inline ZEFlagsBase<Type>& operator^=(const ZEFlagsBase<Type>& Other)
		{
			this->Value ^= Other->Value;
			return *this;
		}

		inline ZEFlagsBase<Type>& operator~() const
		{
			return ZEFlagsBase<int>(~this->Value);
		}

		inline ZEFlagsBase<Type>& operator=(const Type& Other)
		{
			this->Value = Other;
			return *this;
		}
		
		inline ZEFlagsBase<Type>& operator=(const ZEFlagsBase<Type>& Other)
		{
			this->Value = Other->Value;
			return *this;
		}

		inline ZEFlagsBase()
		{
			Value = 0;
		}

		inline ZEFlagsBase(const Type& Value)
		{
			this->Value = Value;
		}

		inline ZEFlagsBase(const ZEFlagsBase<Type>& Value)
		{
			this->Value = Value;
		}
};

typedef ZEFlagsBase<> ZEFlags;

#endif
