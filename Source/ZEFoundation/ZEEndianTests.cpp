//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEEndianTests.cpp
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

#undef ZE_PLATFORM_ENDIANNESS_BIG
#define ZE_PLATFORM_ENDIANNESS_LITTLE

// #undef ZE_PLATFORM_ENDIANNESS_LITTLE
// #define ZE_PLATFORM_ENDIANNESS_BIG

#include "ZEEndian.h"
#include "ZETest/ZETest.h"
#include "ZETest/ZETestCheck.h"
#include "ZEError.h"

ZETestSuite(ZEEndian)
{
	ZETest("static ZE_FORCE_INLINE ZEEndianness GetEndianness()")
	{
		ZEEndianness Endianness = ZEEndian::GetEndianness();

		//ZE_PLATFORM_ENDIANNESS_LITTLE
		ZETestCheckEqual(Endianness, ZE_ET_LITTLE);

		//ZE_PLATFORM_ENDIANNESS_BIG
		//ZETestCheckEqual(Endianness, ZE_ET_BIG);
	}

	ZETest("static ZE_FORCE_INLINE ZEType Uni(ZEType Value, ZEEndianness Endianness)")
	{
		ZEInt16 Value = 0x0123;

		//ZE_PLATFORM_ENDIANNESS_LITTLE
		ZEInt16 Result = ZEEndian::Uni(Value, ZE_ET_BIG);
		ZETestCheckEqual(Result, 0x2301);

		Result = ZEEndian::Uni(Value, ZE_ET_LITTLE);
		ZETestCheckEqual(Result, 0x0123);

		//ZE_PLATFORM_ENDIANNESS_BIG
// 		ZEInt16 Result = ZEEndian::Uni(Value, ZE_ET_BIG);
// 		ZETestCheckEqual(Result, 0x0123);
// 
// 		Result = ZEEndian::Uni(Value, ZE_ET_LITTLE);
// 		ZETestCheckEqual(Result, 0x2301);
	}

	ZETest("static ZE_FORCE_INLINE ZEUInt16 Little(ZEUInt16 Value)")
	{
		ZEUInt16 Value = 0x0123;

		ZEUInt16 Result = ZEEndian::Little(Value);

		//ZE_PLATFORM_ENDIANNESS_LITTLE
		ZETestCheckEqual(Result, 0x0123);

		//ZE_PLATFORM_ENDIANNESS_BIG
		//ZETestCheckEqual(Result, 0x2301);

		ZETestCase("for negative value")
		{
			ZEUInt16 NegValue = -0x0123;
			ZEUInt16 Res = ZEEndian::Little(NegValue);

			//ZE_PLATFORM_ENDIANNESS_LITTLE
			ZETestCheckEqual(Res, 0xFEDD);

			//ZE_PLATFORM_ENDIANNESS_BIG
			//ZETestCheckEqual(Res, 0xDDFE);
		}
	}

	ZETest("static ZE_FORCE_INLINE ZEUInt32 Little(ZEUInt32 Value)")
	{
		ZEUInt32 Value = 0x01234567;

		ZEUInt32 Result = ZEEndian::Little(Value);

		//ZE_PLATFORM_ENDIANNESS_LITTLE
		ZETestCheckEqual(Result, 0x01234567);

		//ZE_PLATFORM_ENDIANNESS_BIG
		//ZETestCheckEqual(Result, 0x67452301);

		ZETestCase("for negative value")
		{
			ZEUInt32 NegValue = -0x01234567;
			ZEUInt32 Res = ZEEndian::Little(NegValue);

			//ZE_PLATFORM_ENDIANNESS_LITTLE
			ZETestCheckEqual(Res, 0xFEDCBA99);

			//ZE_PLATFORM_ENDIANNESS_BIG
			//ZETestCheckEqual(Res, 0x99BADCFE);
		}
	}

	ZETest("static ZE_FORCE_INLINE ZEUInt64 Little(ZEUInt64 Value)")
	{
		ZEUInt64 Value = 0x0123456789ABCDEF;

		ZEUInt64 Result = ZEEndian::Little(Value);

		//ZE_PLATFORM_ENDIANNESS_LITTLE
		ZETestCheckEqual(Result, 0x0123456789ABCDEF);

		//ZE_PLATFORM_ENDIANNESS_BIG
		//ZETestCheckEqual(Result, 0xEFCDAB8967452301);

		ZETestCase("for negative value")
		{
			ZEUInt64 NegValue = -0x0123456789ABCDEF;
			ZEUInt64 Res = ZEEndian::Little(NegValue);

			//ZE_PLATFORM_ENDIANNESS_LITTLE
			ZETestCheckEqual(Res, 18364758544493064721);

			//ZE_PLATFORM_ENDIANNESS_BIG
			//ZETestCheckEqual(Res, 1239145715825564926);
		}
	}

	ZETest("static ZE_FORCE_INLINE ZEInt16 Little(ZEInt16 Value)")
	{
		ZEInt16 Value = 0x0123;

		ZEInt16 Result = ZEEndian::Little(Value);

		//ZE_PLATFORM_ENDIANNESS_LITTLE
		ZETestCheckEqual(Result, 0x0123);

		//ZE_PLATFORM_ENDIANNESS_BIG
		//ZETestCheckEqual(Result, 0x2301);

		ZETestCase("for negative value")
		{
			ZEInt16 NegValue = -0x0123;
			ZEInt16 Res = ZEEndian::Little(NegValue);

			//ZE_PLATFORM_ENDIANNESS_LITTLE
			ZETestCheckEqual(Res, -0x0123);

			//ZE_PLATFORM_ENDIANNESS_BIG
			//ZETestCheckEqual(Res, -0x2202);
		}
	}

	ZETest("static ZE_FORCE_INLINE ZEInt32 Little(ZEInt32 Value)")
	{
		ZEInt32 Value = 0x01234567;

		ZEInt32 Result = ZEEndian::Little(Value);

		//ZE_PLATFORM_ENDIANNESS_LITTLE
		ZETestCheckEqual(Result, 0x01234567);

		//ZE_PLATFORM_ENDIANNESS_BIG
		//ZETestCheckEqual(Result, 0x67452301);

		ZETestCase("for negative value")
		{
			ZEInt32 NegValue = -0x01234567;
			ZEUInt32 Res = ZEEndian::Little(NegValue);

			//ZE_PLATFORM_ENDIANNESS_LITTLE
			ZETestCheckEqual(Res, -0x01234567);

			//ZE_PLATFORM_ENDIANNESS_BIG
			//ZETestCheckEqual(Res, -0x66452302);
		}
	}

	ZETest("static ZE_FORCE_INLINE ZEInt64 Little(ZEInt64 Value)")
	{
		ZEInt64 Value = 0x0123456789ABCDEF;

		ZEInt64 Result = ZEEndian::Little(Value);

		//ZE_PLATFORM_ENDIANNESS_LITTLE
		ZETestCheckEqual(Result, 0x0123456789ABCDEF);

		//ZE_PLATFORM_ENDIANNESS_BIG
		//ZETestCheckEqual(Result, 0xEFCDAB8967452301);

		ZETestCase("for negative value")
		{
			ZEInt64 NegValue = -0x0123456789ABCDEF;
			ZEInt64 Res = ZEEndian::Little(NegValue);

			//ZE_PLATFORM_ENDIANNESS_LITTLE
			ZETestCheckEqual(Res, -0x0123456789ABCDEF);

			//ZE_PLATFORM_ENDIANNESS_BIG
			//ZETestCheckEqual(Res, 1239145715825564926);
		}
	}

	ZETest("static ZE_FORCE_INLINE ZEUInt16 Big(ZEUInt16 Value)")
	{
		ZEUInt16 Value = 0x0123;

		ZEUInt16 Result = ZEEndian::Big(Value);

		//ZE_PLATFORM_ENDIANNESS_LITTLE
		ZETestCheckEqual(Result, 0x2301);

		//ZE_PLATFORM_ENDIANNESS_BIG
		//ZETestCheckEqual(Result, 0x0123);

		ZETestCase("for negative value")
		{
			ZEUInt16 NegValue = -0x0123;
			ZEUInt16 Res = ZEEndian::Big(NegValue);

			//ZE_PLATFORM_ENDIANNESS_LITTLE
			ZETestCheckEqual(Res, 0xDDFE);

			//ZE_PLATFORM_ENDIANNESS_BIG
			//ZETestCheckEqual(Res, 0xFEDD);
		}
	}

	ZETest("static ZE_FORCE_INLINE ZEUInt32 Big(ZEUInt32 Value)")
	{
		ZEUInt32 Value = 0x01234567;

		ZEUInt32 Result = ZEEndian::Big(Value);

		//ZE_PLATFORM_ENDIANNESS_LITTLE
		ZETestCheckEqual(Result, 0x67452301);

		//ZE_PLATFORM_ENDIANNESS_BIG
		//ZETestCheckEqual(Result, 0x01234567);

		ZETestCase("for negative value")
		{
			ZEUInt32 NegValue = -0x01234567;
			ZEUInt32 Res = ZEEndian::Big(NegValue);

			//ZE_PLATFORM_ENDIANNESS_LITTLE
			ZETestCheckEqual(Res, 0x99BADCFE);

			//ZE_PLATFORM_ENDIANNESS_BIG
			//ZETestCheckEqual(Res, 0xFEDCBA99);
		}
	}

	ZETest("static ZE_FORCE_INLINE ZEUInt64 Big(ZEUInt64 Value)")
	{
		ZEUInt64 Value = 0x0123456789ABCDEF;

		ZEUInt64 Result = ZEEndian::Big(Value);

		//ZE_PLATFORM_ENDIANNESS_LITTLE
		ZETestCheckEqual(Result, 0xEFCDAB8967452301);

		//ZE_PLATFORM_ENDIANNESS_BIG
		//ZETestCheckEqual(Result, 0x0123456789ABCDEF);

		ZETestCase("for negative value")
		{
			ZEUInt64 NegValue = -0x0123456789ABCDEF;
			ZEUInt64 Res = ZEEndian::Big(NegValue);

			//ZE_PLATFORM_ENDIANNESS_LITTLE
			ZETestCheckEqual(Res, 1239145715825564926);

			//ZE_PLATFORM_ENDIANNESS_BIG
			//ZETestCheckEqual(Res, 18364758544493064721);
		}
	}

	ZETest("static ZE_FORCE_INLINE ZEInt16 Big(ZEInt16 Value)")
	{
		ZEInt16 Value = 0x0123;

		ZEInt16 Result = ZEEndian::Big(Value);

		//ZE_PLATFORM_ENDIANNESS_LITTLE
		ZETestCheckEqual(Result, 0x2301);

		//ZE_PLATFORM_ENDIANNESS_BIG
		//ZETestCheckEqual(Result, 0x0123);

		ZETestCase("for negative value")
		{
			ZEInt16 NegValue = -0x0123;
			ZEInt16 Res = ZEEndian::Big(NegValue);

			//ZE_PLATFORM_ENDIANNESS_LITTLE
			ZETestCheckEqual(Res, -0x2202);

			//ZE_PLATFORM_ENDIANNESS_BIG
			//ZETestCheckEqual(Res, -0x0123);
		}
	}

	ZETest("static ZE_FORCE_INLINE ZEInt32 Big(ZEInt32 Value)")
	{
		ZEInt32 Value = 0x01234567;

		ZEInt32 Result = ZEEndian::Big(Value);

		//ZE_PLATFORM_ENDIANNESS_LITTLE
		ZETestCheckEqual(Result, 0x67452301);

		//ZE_PLATFORM_ENDIANNESS_BIG
		//ZETestCheckEqual(Result, 0x01234567);

		ZETestCase("for negative value")
		{
			ZEInt32 NegValue = -0x01234567;
			ZEInt32 Res = ZEEndian::Big(NegValue);

			//ZE_PLATFORM_ENDIANNESS_LITTLE
			ZETestCheckEqual(Res, -0x66452302);

			//ZE_PLATFORM_ENDIANNESS_BIG
			//ZETestCheckEqual(Res, -0x01234567);
		}
	}

	ZETest("static ZE_FORCE_INLINE ZEInt64 Big(ZEInt64 Value)")
	{
		ZEInt64 Value = 0x0123456789ABCDEF;

		ZEInt64 Result = ZEEndian::Big(Value);

		//ZE_PLATFORM_ENDIANNESS_LITTLE
		ZETestCheckEqual(Result, 0xEFCDAB8967452301);

		//ZE_PLATFORM_ENDIANNESS_BIG
		//ZETestCheckEqual(Result, 0x0123456789ABCDEF);

		ZETestCase("for negative value")
		{
			ZEInt64 NegValue = -0x0123456789ABCDEF;
			ZEInt64 Res = ZEEndian::Big(NegValue);

			//ZE_PLATFORM_ENDIANNESS_LITTLE
			ZETestCheckEqual(Res, 1239145715825564926);

			//ZE_PLATFORM_ENDIANNESS_BIG
			//ZETestCheckEqual(Res, -0x0123456789ABCDEF);
		}
	}
}
