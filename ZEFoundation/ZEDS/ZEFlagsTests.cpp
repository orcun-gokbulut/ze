//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFlagsTests.cpp
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


#include "ZEDS/ZEFlags.h"
#include "ZETest.h"

ZETestSuite(ZEFlags)
{
	ZETest("inline bool GetBit(size_t Index) const")
	{
		size_t Index = 0;
		
		ZEFlags Flag;
		Flag.SetBit(Index, true);
		
		bool Bit = Flag.GetBit(Index);
		ZETestCheck(Bit == true);

		Bit = Flag.GetBit(3);
		ZETestCheck(Bit == false);

	}
	ZETest("inline bool GetFlags(int Flags) const")
	{
		ZEFlags Value = 1;
		int Flags = 1;

		bool Result = Value.GetFlags(Flags);
		ZETestCheck(Result == true);

		ZETestCase("for false")
		{
			Value = 4;
			Flags = 5;

			Result = Value.GetFlags(Flags);
			ZETestCheck(Result == false);
		}
	}
	ZETest("inline ZEFlagsBase<Type> operator&(const Type& Other) const")
	{
		ZEFlagsBase<int> Flags = 4;
		const int Other = 4;

		ZEFlagsBase<int> Result = Flags & Other;
		ZETestCheckEqual(Result, 4);

		ZETestCase("for different values")
		{
			ZEFlagsBase<int> Flags = 3;
			const int Other = 1;

			ZEFlagsBase<int> Result = Flags & Other;
			ZETestCheckEqual(Result, 1);
		}

		ZETestCase("for different values")
		{
			ZEFlagsBase<int> Flags = 1;
			const int Other = 2;

			ZEFlagsBase<int> Result = Flags & Other;
			ZETestCheckEqual(Result, 0);
		}
	}
	ZETest("inline ZEFlagsBase<Type> operator&(const ZEFlagsBase<Type>& Other) const")
	{
		ZEFlagsBase<int> Flags = 1;
		ZEFlagsBase<int> Other = 2;

		ZEFlagsBase<int> Result = Flags & Other;
		ZETestCheckEqual(Result, 0);

		ZETestCase("for different values")
		{
			Flags = 3;
			Other = 1;

			Result = Flags & Other;
			ZETestCheckEqual(Result, 1);
		}
	}
	ZETest("inline ZEFlagsBase<Type>& operator&=(const Type& Other)")
	{
		ZEFlagsBase<int> Flags = 4;
		const int Other = 4;

		Flags &= Other;
		ZETestCheckEqual(Flags, 4);

		ZETestCase("for different values")
		{
			ZEFlagsBase<int> Flags = 1;
			const int Other = 2;

			Flags &= Other;
			ZETestCheckEqual(Flags, 0);
		}
	}
	ZETest("inline ZEFlagsBase<Type>& operator&=(const ZEFlagsBase<Type>& Other)")
	{
		ZEFlagsBase<int> Flags = 3;
		ZEFlagsBase<int> Other = 3;

		Flags &= Other;
		ZETestCheckEqual(Flags, 3);

		ZETestCase("for different values")
		{
			ZEFlagsBase<int> Flags = 3;
			ZEFlagsBase<int> Other = 1;

			Flags &= Other;
			ZETestCheckEqual(Flags, 1);
		}
	}
	ZETest("inline bool operator[](size_t Index) const")
	{
		ZEFlags Flags = 12;

		bool Value0 = Flags[0];
		bool Value1 = Flags[1];
		bool Value2 = Flags[2];
		bool Value3 = Flags[3];
		ZETestCheck(Value0 == false);
		ZETestCheck(Value1 == false);
		ZETestCheck(Value2 == true);
		ZETestCheck(Value3 == true);

	}
	ZETest("inline ZEFlagsBase<Type> operator^(const Type& Other) const")
	{
		ZEFlagsBase<int> Flags = 4;
		const int Other = 4;

		ZEFlagsBase<int> Result = Flags ^ Other;
		ZETestCheckEqual(Result, 0);

		ZETestCase("for different values")
		{
			ZEFlagsBase<int> Flags = 7;
			const int Other = 11;

			ZEFlagsBase<int> Result = Flags ^ Other;
			ZETestCheckEqual(Result, 12);
		}
	}
	ZETest("inline ZEFlagsBase<Type> operator^(const ZEFlagsBase<Type>& Other) const")
	{
		ZEFlagsBase<int> Flags = 4;
		ZEFlagsBase<int> Other = 4;

		ZEFlagsBase<int> Result = Flags ^ Other;
		ZETestCheckEqual(Result, 0);

		ZETestCase("for different values")
		{
			Flags = 1;
			Other = 5;

			Result = Flags ^ Other;
			ZETestCheckEqual(Result, 4);
		}
	}
	ZETest("inline ZEFlagsBase<Type>& operator^=(const Type& Other)")
	{
		ZEFlagsBase<int> Flags = 4;
		const int Other = 4;

		Flags ^= Other;
		ZETestCheckEqual(Flags, 0);

		ZETestCase("for different values")
		{
			ZEFlagsBase<int> Flags = 7;
			const int Other = 11;

			Flags ^= Other;
			ZETestCheckEqual(Flags, 12);
		}
	}
	ZETest("inline ZEFlagsBase<Type>& operator^=(const ZEFlagsBase<Type>& Other)")
	{
		ZEFlagsBase<int> Flags = 4;
		ZEFlagsBase<int> Other = 4;

		Flags ^= Other;
		ZETestCheckEqual(Flags, 0);

		ZETestCase("for different values")
		{
			Flags = 1;
			Other = 5;

			Flags ^= Other;
			ZETestCheckEqual(Flags, 4);
		}
	}
	ZETest("inline ZEFlagsBase<Type> operator|(const Type& Other) const")
	{
		ZEFlagsBase<int> Flags = 4;
		const int Other = 4;

		ZEFlagsBase<int> Result = Flags | Other;
		ZETestCheckEqual(Result, 4);

		ZETestCase("for different values")
		{
			ZEFlagsBase<int> Flags = 3;
			const int Other = 5;

			ZEFlagsBase<int> Result = Flags | Other;
			ZETestCheckEqual(Result, 7);
		}
	}
	ZETest("inline ZEFlagsBase<Type> operator|(const ZEFlagsBase<Type>& Other) const")
	{
		ZEFlagsBase<int> Flags = 1;
		ZEFlagsBase<int> Other = 1;

		ZEFlagsBase<int> Result = Flags | Other;
		ZETestCheckEqual(Result, 1);

		ZETestCase("for different values")
		{
			Flags = 1;
			Other = 8;

			Result = Flags | Other;
			ZETestCheckEqual(Result, 9);
		}
	}
	ZETest("inline ZEFlagsBase<Type>& operator|=(const Type& Other)")
	{
		ZEFlagsBase<int> Flags = 4;
		const int Other = 4;

		Flags |= Other;
		ZETestCheckEqual(Flags, 4);

		ZETestCase("for different values")
		{
			ZEFlagsBase<int> Flags = 3;
			const int Other = 5;

			Flags |= Other;
			ZETestCheckEqual(Flags, 7);
		}
	}
	ZETest("inline ZEFlagsBase<Type>& operator|=(const ZEFlagsBase<Type>& Other)")
	{
		ZEFlagsBase<int> Flags = 1;
		ZEFlagsBase<int> Other = 1;

		Flags |= Other;
		ZETestCheckEqual(Flags, 1);

		ZETestCase("for different values")
		{
			Flags = 1;
			Other = 8;

			Flags |= Other;
			ZETestCheckEqual(Flags, 9);
		}
	}
	ZETest("inline ZEFlagsBase<Type>& operator~() const")
	{
		ZEFlagsBase<int> Flags = 2;

		ZEFlagsBase<int> Result = ~ Flags.Value;
		ZETestCheckEqual(Result, -3);

		ZETestCase("for 0")
		{
			Flags = 0;

			Result = ~ Flags.Value;
			ZETestCheckEqual(Result, -1);
		}
	}
	ZETest("inline ZEFlagsBase<Type>& operator=(const Type& Other)")
	{
		ZEFlagsBase<int> Flags = 1;
		const int Other = 3;

		Flags = Other;
		ZETestCheckEqual(Flags.Value, 3);
	}
	ZETest("inline ZEFlagsBase<Type>& operator=(const ZEFlagsBase<Type>& Other)")
	{
		ZEFlagsBase<int> Flags = 2;
		ZEFlagsBase<int> Other = 4;

		Flags = Other;
		ZETestCheckEqual(Flags.Value, 4);
	}
	ZETest("inline operator Type() const")
	{
		ZEFlagsBase<int> Flags = 5;
		int Value;

		Value = (int)Flags;
		ZETestCheckEqual(Flags.Value, 5);
		ZETestCheckEqual(Value, 5);
	}
	ZETest("inline void RaiseBit(size_t Index)")
	{
		int Value = 0;
		size_t Index = 4;
		ZEFlags Flag;

		Flag.SetFlags(Value, true);
		ZETestCheckEqual(Flag.Value, 0);

		Flag.RaiseBit(Index);
		ZETestCheckEqual(Flag.Value, 16);

		ZETestCase("for Flag Value different 0")
		{
			Flag = 1;
			Index = 3;

			Flag.RaiseBit(Index);
			ZETestCheckEqual(Flag.Value, 9);
		}
	}
	ZETest("inline void RaiseFlags(int Flags)")
	{
		ZEFlags Value = 4;
		int Flags = 3;

		Value.RaiseFlags(Flags);
		ZETestCheckEqual(Value, 7);
	}
	ZETest("inline void SetBit(size_t Index, bool Raise)")
	{
		ZEFlags Flag = 0;
		size_t Index = 4;
		bool Raise = true;

		Flag.SetBit(Index, Raise);
		ZETestCheckEqual(Flag.Value, 16);

		ZETestCase("for Flag Value different from 0")
		{
			Flag = 2;
			Index = 4;
			Raise = true;

			Flag.SetBit(Index, Raise);
			ZETestCheckEqual(Flag.Value, 18);
		}

		ZETestCase("for Raise false")
		{
			Flag = 2;
			Index = 4;
			Raise = false;

			Flag.SetBit(Index, Raise);
			ZETestCheckEqual(Flag.Value, 2);
		}
	}
	ZETest("inline void SetFlags(int Flags, bool Raise)")
	{
		ZEFlags Value = 0;
		int Flags = 3;
		bool Raise = true;

		Value.SetFlags(Flags, Raise);
		ZETestCheckEqual(Value, 3);

		ZETestCase("for Raise false")
		{
			Value = 0;
			Flags = 3;
			Raise = false;

			Value.SetFlags(Flags, Raise);
			ZETestCheckEqual(Value, 0);
		}
	}
	ZETest("inline void UnraiseBit(size_t Index)")
	{
		ZEFlags Flag = 18;
		size_t Index = 4;

		Flag.UnraiseBit(Index);
		ZETestCheckEqual(Flag.Value, 2);

		ZETestCase("for Flag's Index Bit = 0")
		{
			Flag = 2;
			Index = 4;

			Flag.UnraiseBit(Index);
			ZETestCheckEqual(Flag.Value, 2);
		}
	}
	ZETest("inline void UnraiseFlags(Type Flags)")
	{
		ZEFlags Value = 7;
		int Flags = 4;

		Value.UnraiseFlags(Flags);
		ZETestCheckEqual(Value, 3);
	}
	ZETest("inline ZEFlagsBase()")
	{
		ZEFlagsBase<int> Flags;
		ZETestCheckEqual(Flags.Value, 0);
	}
	ZETest("inline ZEFlagsBase(const Type& Value)")
	{
		const int Value = 5;

		ZEFlagsBase<int> Flags(Value);
		ZETestCheckEqual(Flags.Value, 5);
	}
	ZETest("inline ZEFlagsBase(const ZEFlagsBase<Type>& Value)")
	{
		ZEFlagsBase<int> Value = 3;

		ZEFlagsBase<int> Flags(Value);
		ZETestCheckEqual(Flags.Value, 3);
	}
}
