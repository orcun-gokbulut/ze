//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFormatTests.cpp
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

#include <string>
#include "ZETest/ZETest.h"
#include "ZEFormat.h"
#include "ZEMath/ZEAngle.h"

ZETestSuite(ZEFormat)
{
	ZETest("ZEString ZEFormat::Format(const char* Format)")
	{
		ZEString Result;
		Result = ZEFormat::Format("test format string");
		ZETestCheck(Result == "test format string");

		ZETestCase("empty string")
		{
			Result = ZEFormat::Format("");
			ZETestCheck(Result == NULL);
		}

		ZETestCase("{test}")
		{
			Result = ZEFormat::Format("{test}");
			ZETestCheck(Result == NULL);
			//error: Formatting error. Argument index must be integer.
		}

		ZETestCase("{0}")
		{
			Result = ZEFormat::Format("{0}");
			ZETestCheck(Result == NULL);
			//error: Formatting error. Argument index is out of bounds.
		}

		ZETestCase("{}")
		{
			Result = ZEFormat::Format("{}");
			ZETestCheck(Result == NULL);
			//error: Formatting error. Argument index must be integer.
		}
	}

	ZETest("ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0)")
	{
		ZEVariant Arg0;
		ZEString Result;

		ZETestCase("for ZE_VRT_UNDEFINED typed argument")
		{
			Result = ZEFormat::Format("{0:}", Arg0);
			ZETestCheck(Result == "false");
		}

		ZETestCase("for ZE_VRT_NULL typed argument")
		{
			Arg0.SetNull();
			Result = ZEFormat::Format("{0:}", Arg0);
			ZETestCheck(Result == "false");
		}

		ZETestCase("for an invalid format item")
		{
			Arg0.SetInt16(46);
			Result = ZEFormat::Format("{0:+5}", Arg0);
			ZETestCheck(Result == "false");
		}

		ZETestCase("multiple format items for Arg0")
		{
			Arg0.SetInt8('Z');
			Result = ZEFormat::Format("Character Value : {0:c}, Uppercase Hexadecimal Value : {0:X}, Lowercase Hexadecimal Value : {0:x}, Decimal Value : {0:d}", Arg0);
			ZETestCheck(Result == "Character Value : Z, Uppercase Hexadecimal Value : 5A, Lowercase Hexadecimal Value : 5a, Decimal Value : 90");
		}

		ZETestCase("nested opening and closing braces")
		{
			Arg0.SetInt16(176);
			Result = ZEFormat::Format("{0:x:04}", Arg0);
			ZETestCheck(Result == "00b0");

			Result = ZEFormat::Format("{{0:x:04}}", Arg0);
			ZETestCheck(Result == "{0:x:04}");

			Result = ZEFormat::Format("{{{0:x:04}}}", Arg0);
			ZETestCheck(Result == "{00b0}");
		}
	}

	ZETest("ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1)")
	{
		ZEVariant Arg0;
		ZEVariant Arg1;
		ZEString Result;

		Arg0.SetInt8(90);
		Arg1.SetInt8(69);

		Result = ZEFormat::Format("{0:c}{1:c}", Arg0, Arg1);
		ZETestCheck(Result == "ZE");

		ZETestCase("change the order of format items in Format string")
		{
			Result = ZEFormat::Format("{1:c}{0:c}", Arg0, Arg1);
			ZETestCheck(Result == "EZ");
		}

		ZETestCase("Arg1 not referenced by a format item")
		{
			Result = ZEFormat::Format("0x{0:X}", Arg0, Arg1);
			ZETestCheck(Result == "0x5A");
		}

		ZETestCase("invalid format for Arg1")
		{
			Result = ZEFormat::Format("{0:d:5}-{1:5d}", Arg0, Arg1);
			ZETestCheck(Result == "false");
		}
	}

	ZETest("ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2)")
	{
		ZEVariant Arg0;
		ZEVariant Arg1;
		ZEVariant Arg2;
		ZEString Result;

		Arg0.SetInt16(31678);
		Arg1.SetFloat(7.8f);
		Arg2.SetVector2(ZEVector2(3.7, -2.8));

		Result = ZEFormat::Format("", Arg0, Arg1, Arg2);
		ZETestCheck(Result == NULL);

		Result = ZEFormat::Format("{0:X}|{1:-+6.1}|{2:5.}", Arg0, Arg1, Arg2);
		ZETestCheck(Result == "7BBE|+7.8  |<    4,    -3>");
	}

	ZETest("ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3)")
	{
		ZEVariant Arg0;
		ZEVariant Arg1;
		ZEVariant Arg2;
		ZEVariant Arg3;
		ZEString Result;

		Arg0.SetQuaternion(ZEQuaternion(ZE_PI_4, ZEVector3::UnitX));
		Arg1.SetInt8('z');
		Arg2.SetUInt16(13);
		Arg3.SetBoolean(true);

		Result = ZEFormat::Format("{0:5.2}|{1:d}|{2:x:04}|{3:false}", Arg0, Arg1, Arg2, Arg3);
		ZETestCheck(Result == "< 0.38,  0.00,  0.00,  0.92>|122|000d|false");
	}

	ZETest("ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4)")
	{
		ZEVariant Arg0;
		ZEVariant Arg1;
		ZEVariant Arg2;
		ZEVariant Arg3;
		ZEVariant Arg4;
		ZEString Result;

		Arg0.SetUInt32(108);
		Arg1.SetInt32(108);
		Arg2.SetInt16(108);
		Arg3.SetInt8(108);
		Arg4.SetInt64(108);

		Result = ZEFormat::Format("{0:d.5}|{1::+}|{2:x}|{3:X:5}|{4::+.2}", Arg0, Arg1, Arg2, Arg3, Arg4);
		ZETestCheck(Result == "108.5|+108|6c|   6C|+108");
	}

	ZETest("ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5)")
	{
		ZEVariant Arg0;
		ZEVariant Arg1;
		ZEVariant Arg2;
		ZEVariant Arg3;
		ZEVariant Arg4;
		ZEVariant Arg5;
		Arg0.SetBoolean(false);
		Arg1.SetDouble(6.15);
		Arg2.SetFloat(3.2718f);
		Arg3.SetInt32(60);
		Arg4.SetInt8('Z');
		Arg5.SetString("Test");

		ZEString Result = ZEFormat::Format("{0:true} - {1:.1} - {2:+.3} - 0x{3:X} - {4:c} - {5:new}", Arg0, Arg1, Arg2, Arg3, Arg4, Arg5);
		ZETestCheck(Result == "false - 6.2 - +3.272 - 0x3C - Z - Test");
	}

	ZETest("ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6)")
	{
		ZEVariant Arg0;
		ZEVariant Arg1;
		ZEVariant Arg2;
		ZEVariant Arg3;
		ZEVariant Arg4;
		ZEVariant Arg5;
		ZEVariant Arg6;
		ZEString Result;

		Arg0.SetBoolean(false);
		Arg1.SetDouble(6.15);
		Arg2.SetFloat(3.2718f);
		Arg3.SetInt32(60);
		Arg4.SetInt8('Z');
		Arg5.SetString("Test");
		Arg6.SetQuaternion(ZEQuaternion(ZE_PI_4, ZEVector3::UnitX));

		Result = ZEFormat::Format("{0:true} - {1:-5.1} - {2:+.3} - 0x{3:X:08} - {4:c} - {5:} - {6::}", Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6);
		ZETestCheck(Result == "false - 6.2   - +3.272 - 0x0000003C - Z - Test - <0.383, 0.000, 0.000, 0.924>");
	}

	ZETest("ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7)")
	{
		ZEVariant Arg0;
		ZEVariant Arg1;
		ZEVariant Arg2;
		ZEVariant Arg3;
		ZEVariant Arg4;
		ZEVariant Arg5;
		ZEVariant Arg6;
		ZEVariant Arg7;

		Arg0.SetBoolean(true);
		Arg1.SetQuaternion(ZEQuaternion(ZE_PI_4, ZEVector3::UnitX));
		Arg2.SetFloat(5.87f);
		Arg3.SetInt8(122);
		Arg4.SetInt32(3500);
		Arg5.SetVector2(ZEVector2(3.7f, -2.8f));
		Arg6.SetString("String");
		Arg7.SetDouble(6.1);

		ZEString Result = ZEFormat::Format("{0:false}|{1:-+5.1}|{2:-8}|0x{3:X}|{4:d:6}|{5:+.}|{6:New}|{7:3.1}", Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7);
		ZETestCheck(Result == "false|<+0.4 , +0.0 , +0.0 , +0.9 >|5.870000|0x7A|  3500|<+4, -3>|String|6.1") ;
	}

	ZETest("ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8)")
	{
		ZEVariant Arg0;
		ZEVariant Arg1;
		ZEVariant Arg2;
		ZEVariant Arg3;
		ZEVariant Arg4;
		ZEVariant Arg5;
		ZEVariant Arg6;
		ZEVariant Arg7;
		ZEVariant Arg8;
		ZEString Result;

		Arg0.SetUInt64(987654321);
		Arg1.SetBoolean(true);
		Arg2.SetQuaternion(ZEQuaternion(ZE_PI_4, ZEVector3::UnitX));
		Arg3.SetFloat(5.87f);
		Arg4.SetInt8(122);
		Arg5.SetInt32(3500);
		Arg6.SetVector2(ZEVector2(3.7f, -2.8f));
		Arg7.SetString("String");
		Arg8.SetDouble(6.1);

		Result = ZEFormat::Format("0x{0:x:012}|{1:false}|{2:.}|{3:5.1}|{4:c:4}|{5:d:8}|{6:+5.2}|{7:new}|{8:-5.1}", Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8);
		ZETestCheck(Result == "0x00003ade68b1|false|<0, 0, 0, 1>|  5.9|   z|    3500|<+3.70, -2.80>|String|6.1  ");
	}

	ZETest("ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9)")
	{
		ZEVariant Arg0;
		ZEVariant Arg1;
		ZEVariant Arg2;
		ZEVariant Arg3;
		ZEVariant Arg4;
		ZEVariant Arg5;
		ZEVariant Arg6;
		ZEVariant Arg7;
		ZEVariant Arg8;
		ZEVariant Arg9;
		ZEString Result;

		Arg0.SetInt8(122);
		Arg1.SetInt16(576);
		Arg2.SetInt32(678905);
		Arg3.SetInt64(-780315724);
		Arg4.SetUInt8(90);
		Arg5.SetUInt16(43000);
		Arg6.SetUInt32(4294967011);
		Arg7.SetUInt64(987654321);
		Arg8.SetDouble(7.3);
		Arg9.SetFloat(5.86f);

		Result = ZEFormat::Format("{0:c}|{1:d:+}|0x{2:x:08}|{3:d:12}|{4:d:-6}|{5:d:-06}|0x{6:X}|{7:d:+-11}|{8:.3}|{9:+-9.4}", Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9);
		ZETestCheck(Result == "z|+576|0x000a5bf9|  -780315724|90    |43000 |0xFFFFFEE3|+987654321 |7.300|+5.8600  ");
	}

	ZETest("ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10)")
	{
		ZEVariant Arg0;
		ZEVariant Arg1;
		ZEVariant Arg2;
		ZEVariant Arg3;
		ZEVariant Arg4;
		ZEVariant Arg5;
		ZEVariant Arg6;
		ZEVariant Arg7;
		ZEVariant Arg8;
		ZEVariant Arg9;
		ZEVariant Arg10;
		ZEString Result;

		Arg0.SetInt8(122);
		Arg1.SetInt16(576);
		Arg2.SetInt32(678905);
		Arg3.SetInt64(-780315724);
		Arg4.SetUInt8(90);
		Arg5.SetUInt16(43000);
		Arg6.SetUInt32(4294967011);
		Arg7.SetUInt64(987654321);
		Arg8.SetDouble(7.3);
		Arg9.SetFloat(5.86f);
		Arg10.SetBoolean(false);

		Result = ZEFormat::Format("{10:true}|{9:+-9.4}|{8:.3}|{7:d:+-11}|0x{6:X}|{5:d:-06}|{4:d:-6}|{3:d:12}|0x{2:x:08}|{1:d:+}|{0:c}", Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10);
		ZETestCheck(Result == "false|+5.8600  |7.300|+987654321 |0xFFFFFEE3|43000 |90    |  -780315724|0x000a5bf9|+576|z");
	}

	ZETest("ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11)")
	{
		ZEVariant Arg0;
		ZEVariant Arg1;
		ZEVariant Arg2;
		ZEString Result;

		Arg0.SetFloat(2.36971f);
		Arg1.SetInt32(179);
		Arg2.SetVector3(ZEVector3(5.2f, 1.7f, -7.5f));

		Result = ZEFormat::Format("{0:#10}|{1:#-10}|{2:   .2}|{3:x:-6}|{4:d:6}|{5:d:+-6}|0x{6:X:08}|{7:+5.}|{8:-3.}|{9:#+-7.2}|{10:+.}|{11:5.1}", Arg0, Arg0, Arg0, Arg1, Arg1, Arg1, Arg1, Arg2, Arg2, Arg2, Arg2, Arg2);
		ZETestCheck(Result == "  2.369710|2.369710  | 2.37|b3    |   179|+179  |0x000000B3|<   +5,    +2,    -8>|<5  , 2  , -8 >|<+5.20  , +1.70  , -7.50  >|<+5, +2, -8>|<  5.2,   1.7,  -7.5>");
	}

	ZETest("ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12)")
	{
		ZEVariant Arg0;
		ZEVariant Arg1;
		ZEVariant Arg2;
		ZEVariant Arg3;
		ZEVariant Arg4;
		ZEVariant Arg5;
		ZEVariant Arg6;
		ZEVariant Arg7;
		ZEVariant Arg8;
		ZEVariant Arg9;
		ZEVariant Arg10;
		ZEVariant Arg11;
		ZEVariant Arg12;
		ZEString Result;

		Arg0.SetVector2(ZEVector2(4.2f, -7.7f));
		Arg1.SetString("String");
		Arg2.SetQuaternion(ZEQuaternion(ZE_PI_4, ZEVector3::UnitX));
		Arg3.SetInt8(122);
		Arg4.SetInt16(576);
		Arg5.SetInt32(678905);
		Arg6.SetInt64(-780315724);
		Arg7.SetUInt8(90);
		Arg8.SetUInt16(43000);
		Arg9.SetUInt32(4294967011);
		Arg10.SetUInt64(987654321);
		Arg11.SetDouble(7.3);
		Arg12.SetFloat(5.86f);

		Result = ZEFormat::Format("{0::}|{1:new}|{2:.}|{3:c}|{4:d:+}|0x{5:x:08}|{6:d:12}|{7:d:-6}|{8:d:-06}|0x{9:X}|{10:d:+-11}|{11:.3}|{12:+-9.4}", Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12);
		ZETestCheck(Result == "<4.200, -7.700>|String|<0, 0, 0, 1>|z|+576|0x000a5bf9|  -780315724|90    |43000 |0xFFFFFEE3|+987654321 |7.300|+5.8600  ");
	}

	ZETest("ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12)")
	{
		ZEVariant Arg0;
		ZEVariant Arg1;
		ZEVariant Arg2;
		ZEVariant Arg3;
		ZEVariant Arg4;
		ZEVariant Arg5;
		ZEVariant Arg6;
		ZEVariant Arg7;
		ZEVariant Arg8;
		ZEVariant Arg9;
		ZEVariant Arg10;
		ZEVariant Arg11;
		ZEVariant Arg12;
		ZEVariant Arg13;
		ZEString Result;

		Arg0.SetVector2(ZEVector2(4.2f, -7.7f));
		Arg1.SetString("String");
		Arg2.SetQuaternion(ZEQuaternion(ZE_PI_4, ZEVector3::UnitX));
		Arg3.SetInt8(122);
		Arg4.SetInt16(576);
		Arg5.SetInt32(678905);
		Arg6.SetInt64(-780315724);
		Arg7.SetUInt8(90);
		Arg8.SetUInt16(43000);
		Arg9.SetUInt32(4294967011);
		Arg10.SetUInt64(987654321);
		Arg11.SetDouble(7.3);
		Arg12.SetFloat(5.86f);
		Arg13.SetVector3(ZEVector3::One);

		Result = ZEFormat::Format("{0::}|{1:new}|{2:.}|{3:c}|{4:d:+}|0x{5:x:08}|{6:d:12}|{7:d:-6}|{8:d:-06}|0x{9:X}|{10:d:+-11}|{11:.3}|{12:+-9.4}|{13:+6.1}", Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13);
		ZETestCheck(Result == "<4.200, -7.700>|String|<0, 0, 0, 1>|z|+576|0x000a5bf9|  -780315724|90    |43000 |0xFFFFFEE3|+987654321 |7.300|+5.8600  |<  +1.0,   +1.0,   +1.0>");
	}

	ZETest("ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12, const ZEVariant& Arg13)")
	{
		ZEVariant Arg0;
		ZEVariant Arg1;
		ZEVariant Arg2;
		ZEVariant Arg3;
		ZEVariant Arg4;
		ZEVariant Arg5;
		ZEVariant Arg6;
		ZEVariant Arg7;
		ZEVariant Arg8;
		ZEVariant Arg9;
		ZEVariant Arg10;
		ZEVariant Arg11;
		ZEVariant Arg12;
		ZEVariant Arg13;
		ZEString Result;

		Arg0.SetVector2(ZEVector2(4.2f, -7.7f));
		Arg1.SetString("String");
		Arg2.SetQuaternion(ZEQuaternion(ZE_PI_4, ZEVector3::UnitX));
		Arg3.SetInt8(122);
		Arg4.SetInt16(576);
		Arg5.SetInt32(678905);
		Arg6.SetInt64(-780315724);
		Arg7.SetUInt8(90);
		Arg8.SetUInt16(43000);
		Arg9.SetUInt32(4294967011);
		Arg10.SetUInt64(987654321);
		Arg11.SetDouble(7.3);
		Arg12.SetFloat(5.86f);
		Arg13.SetVector3(ZEVector3::One);

		Result = ZEFormat::Format("{0::}|{1:new}|{2:.}|{3:c}|{4:d:+}|0x{5:x:08}|{6:d:12}|{7:d:-6}|{8:d:-06}|0x{9:X}|{10:d:+-11}|{11:.3}|{12:+-9.4}|{13:+6.1}", Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13);
		ZETestCheck(Result == "<4.200, -7.700>|String|<0, 0, 0, 1>|z|+576|0x000a5bf9|  -780315724|90    |43000 |0xFFFFFEE3|+987654321 |7.300|+5.8600  |<  +1.0,   +1.0,   +1.0>");
	}

	ZETest("ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12, const ZEVariant& Arg13, const ZEVariant& Arg14)")
	{
		ZEVariant Arg0;
		ZEVariant Arg1;
		ZEVariant Arg2;
		ZEVariant Arg3;
		ZEVariant Arg4;
		ZEVariant Arg5;
		ZEVariant Arg6;
		ZEVariant Arg7;
		ZEVariant Arg8;
		ZEVariant Arg9;
		ZEVariant Arg10;
		ZEVariant Arg11;
		ZEVariant Arg12;
		ZEVariant Arg13;
		ZEVariant Arg14;
		ZEString Result;

		Arg0.SetVector2(ZEVector2(4.2f, -7.7f));
		Arg1.SetString("String");
		Arg2.SetQuaternion(ZEQuaternion(ZE_PI_4, ZEVector3::UnitX));
		Arg3.SetInt8(122);
		Arg4.SetInt16(576);
		Arg5.SetInt32(678905);
		Arg6.SetInt64(-780315724);
		Arg7.SetUInt8(90);
		Arg8.SetUInt16(43000);
		Arg9.SetUInt32(4294967011);
		Arg10.SetUInt64(987654321);
		Arg11.SetDouble(7.3);
		Arg12.SetFloat(5.86f);
		Arg13.SetVector3(ZEVector3::One);
		Arg14.SetVector4(ZEVector4(4.5f, 3.6f, 2.7f, -1.8f));

		Result = ZEFormat::Format("{0::}|{1:new}|{2:.}|{3:c}|{4:d:+}|0x{5:x:08}|{6:d:12}|{7:d:-6}|{8:d:-06}|0x{9:X}|{10:d:+-11}|{11:.3}|{12:+-9.4}|{13:+6.1}|{14:+.}", Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14);
		ZETestCheck(Result == "<4.200, -7.700>|String|<0, 0, 0, 1>|z|+576|0x000a5bf9|  -780315724|90    |43000 |0xFFFFFEE3|+987654321 |7.300|+5.8600  |<  +1.0,   +1.0,   +1.0>|<+5, +4, +3, -2>");
	}

	ZETest("ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12, const ZEVariant& Arg13, const ZEVariant& Arg14, const ZEVariant& Arg15)")
	{
		ZEVariant Arg;
		ZEString Result;

		Arg.SetInt8('Z');

		Result = ZEFormat::Format("{0:c}|{1:x}|{2:X}|{3:d}|{4:c:4}|{5:x:5}|{6:X:-6}|{7:d:7}|{8:d:-7}|{9:d:+-7}|{10:d:.7}|{11:d:+.7}|{12:c:-7}|{13:d:-.7}|{14}|{15::}", Arg, Arg, Arg, Arg, Arg, Arg, Arg, Arg, Arg, Arg, Arg, Arg, Arg, Arg, Arg, Arg);
		ZETestCheck(Result =="Z|5a|5A|90|   Z|   5a|5A    |     90|90     |+90    |0000090|+0000090|Z      |0000090|90|90");
	}

	ZETest("ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12, const ZEVariant& Arg13, const ZEVariant& Arg14, const ZEVariant& Arg15, const ZEVariant& Arg16)")
	{
		ZEVariant Arg0;
		ZEVariant Arg1;
		ZEVariant Arg2;
		ZEVariant Arg3;
		ZEVariant Arg4;
		ZEVariant Arg5;
		ZEVariant Arg6;
		ZEVariant Arg7;
		ZEVariant Arg8;
		ZEVariant Arg9;
		ZEVariant Arg10;
		ZEVariant Arg11;
		ZEVariant Arg12;
		ZEVariant Arg13;
		ZEVariant Arg14;
		ZEVariant Arg15;
		ZEVariant Arg16;
		ZEString Result;

		Arg0.SetBoolean(true);
		Arg1.SetDouble(120.2);
		Arg2.SetFloat(510.3f);
		Arg3.SetInt16(-30);
		Arg4.SetInt32(520);
		Arg5.SetInt64(1552879);
		Arg6.SetInt8(110);
		Arg7.SetMatrix3x3(ZEMatrix3x3(34.0f, 21.0f, -13.0f,
			8.0f, -5.0f, 3.0f,
			2.0f, 1.0f, -1.0f));
		Arg8.SetMatrix4x4(ZEMatrix4x4(-35.0f, 23.0f, 16.0f, 12.0f,
			10.0f, 9.0f, 9.0f, -9.0f, 
			10.0f, 1.0f, -2.0f, 3.0f,
			4.0f, -5.0f, 6.0f, 7.0f));
		Arg9.SetQuaternion(ZEQuaternion(ZE_PI_8, ZEVector3::UnitZ));
		Arg10.SetString("ZE");
		Arg11.SetUInt16(890);
		Arg12.SetUInt32(93500);
		Arg13.SetUInt64(9876543210);
		Arg14.SetUInt8(78);
		Arg15.SetVector2(ZEVector2(6.7896f, 5.23752f));
		Arg16.SetVector3(ZEVector3(9.3f, 9.8f, -7.3f));

		Result = ZEFormat::Format("{0:false}|{1:5.}|{2::}|{3:}|{4:d:5.}|{5:d:11.}|{6:c}|{7:.2}|{8:.}|{9:#+}|{10:str}|{11:d:+-8.}|0x{12:x:8}|0x{13:X}|{14:c:-3}|{15:+.3}|{16:5.0}", Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14, Arg15, Arg16);
		ZETestCheck(Result == "false|  120|510.299988|-30|  520|    1552879|n|<34.00, 21.00, -13.00, 8.00, -5.00, 3.00, 2.00, 1.00, -1.00>|<-35, 23, 16, 12, 10, 9, 9, -9, 10, 1, -2, 3, 4, -5, 6, 7>|<+0.000000, +0.000000, +0.195090, +0.980785>|ZE|890     |0x   16d3c|0x24CB016EA|N  |<+6.790, +5.238>|<    9,    10,    -7>");
	}

	ZETest("ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12, const ZEVariant& Arg13, const ZEVariant& Arg14, const ZEVariant& Arg15, const ZEVariant& Arg16, const ZEVariant& Arg17)")
	{
		ZEVariant Arg0;
		ZEVariant Arg1;
		ZEVariant Arg2;
		ZEVariant Arg3;
		ZEVariant Arg4;
		ZEVariant Arg5;
		ZEVariant Arg6;
		ZEVariant Arg7;
		ZEVariant Arg8;
		ZEVariant Arg9;
		ZEVariant Arg10;
		ZEVariant Arg11;
		ZEVariant Arg12;
		ZEVariant Arg13;
		ZEVariant Arg14;
		ZEVariant Arg15;
		ZEVariant Arg16;
		ZEVariant Arg17;
		ZEString Result;

		Arg0.SetBoolean(true);
		Arg1.SetDouble(120.2);
		Arg2.SetFloat(510.3f);
		Arg3.SetInt16(-30);
		Arg4.SetInt32(520);
		Arg5.SetInt64(1552879);
		Arg6.SetInt8(110);
		Arg7.SetMatrix3x3(ZEMatrix3x3(34.0f, 21.0f, -13.0f,
			8.0f, -5.0f, 3.0f,
			2.0f, 1.0f, -1.0f));
		Arg8.SetMatrix4x4(ZEMatrix4x4(-35.0f, 23.0f, 16.0f, 12.0f,
			10.0f, 9.0f, 9.0f, -9.0f, 
			10.0f, 1.0f, -2.0f, 3.0f,
			4.0f, -5.0f, 6.0f, 7.0f));
		Arg9.SetQuaternion(ZEQuaternion(ZE_PI_8, ZEVector3::UnitZ));
		Arg10.SetString("ZE");
		Arg11.SetUInt16(890);
		Arg12.SetUInt32(93500);
		Arg13.SetUInt64(9876543210);
		Arg14.SetUInt8(78);
		Arg15.SetVector2(ZEVector2(6.7896f, 5.23752f));
		Arg16.SetVector3(ZEVector3(9.3f, 9.8f, -7.3f));
		Arg17.SetVector4(ZEVector4(2.36f, 3.67f, 1.84f, 0.01f));

		Result = ZEFormat::Format("{0:false}|{1:5.}|{2::}|{3:}|{4:d:5.}|{5:d:11.}|{6:c}|{7:.2}|{8:.}|{9:#+}|{10:str}|{11:d:+-8.}|0x{12:x:8}|0x{13:X}|{14:c:-3}|{15:+.3}|{16:5.0}|{17:+.}", Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14, Arg15, Arg16, Arg17);
		ZETestCheck(Result == "false|  120|510.299988|-30|  520|    1552879|n|<34.00, 21.00, -13.00, 8.00, -5.00, 3.00, 2.00, 1.00, -1.00>|<-35, 23, 16, 12, 10, 9, 9, -9, 10, 1, -2, 3, 4, -5, 6, 7>|<+0.000000, +0.000000, +0.195090, +0.980785>|ZE|890     |0x   16d3c|0x24CB016EA|N  |<+6.790, +5.238>|<    9,    10,    -7>|<+2, +4, +2, +0>");
	}

	ZETest("ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12, const ZEVariant& Arg13, const ZEVariant& Arg14, const ZEVariant& Arg15, const ZEVariant& Arg16, const ZEVariant& Arg17, const ZEVariant& Arg18)")
	{
		ZEVariant Arg0;
		ZEVariant Arg1;
		ZEVariant Arg2;
		ZEVariant Arg3;
		ZEVariant Arg4;
		ZEString Result;

		Arg0.SetInt8(122);		
		Arg1.SetFloat(5.1873f);		
		Arg2.SetBoolean(true);		
		Arg3.SetVector2(ZEVector2(6.7896f, 5.23752f));		
		Arg4.SetInt32(500);	

		Result = ZEFormat::Format("{0:c:5}|{1:c:.5}|{2:d:+-5}|{3:d:+-5.2}|{4:x:04}|{5:.2}|{6:-+10}|{7:+4.}|{8: .1}|{9: #+.1}|{10:false}|{11:true}|{12:1}|{13:-5.2}|{14:+.09}|{15:3.}|{16:x}|{{17:x}}|{{{18:x}}}", Arg0, Arg0, Arg0, Arg0, Arg0, Arg1, Arg1, Arg1, Arg1, Arg1, Arg2, Arg2, Arg2, Arg3, Arg3, Arg3, Arg4, Arg4, Arg4);
		ZETestCheck(Result == "    z|z|+122 |+122 |007a|5.19|+5.187300 |  +5| 5.2|+5.2|false|true|1|<6.79 , 5.24 >|<+6.789599895, +5.237520218>|<  7,   5>|1f4|{17:x}|{1f4}");
	}

	ZETest("ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12, const ZEVariant& Arg13, const ZEVariant& Arg14, const ZEVariant& Arg15, const ZEVariant& Arg16, const ZEVariant& Arg17, const ZEVariant& Arg18, const ZEVariant& Arg19)")
	{
		ZEVariant Arg0;
		ZEVariant Arg1;
		ZEVariant Arg2;
		ZEVariant Arg3;
		ZEVariant Arg4;
		ZEString Result;

		Arg0.SetInt8(122);		
		Arg1.SetFloat(5.1873f);		
		Arg2.SetBoolean(true);		
		Arg3.SetVector2(ZEVector2(6.7896f, 5.23752f));		
		Arg4.SetInt32(500);	

		Result = ZEFormat::Format("{0:c:5}|{1:c:.5}|{2:d:+-5}|{3:d:+-5.2}|{4:x:04}|{5:X:4}|{6:.2}|{7:-+10}|{8:+4.}|{9: .1}|{10: #+.1}|{11:false}|{12:true}|{13:1}|{14:-5.2}|{15:+.09}|{16:3.}|{17:x}|{{18:x}}|{{{19:x}}}", Arg0, Arg0, Arg0, Arg0, Arg0, Arg0, Arg1, Arg1, Arg1, Arg1, Arg1, Arg2, Arg2, Arg2, Arg3, Arg3, Arg3, Arg4, Arg4, Arg4);
		ZETestCheck(Result == "    z|z|+122 |+122 |007a|  7A|5.19|+5.187300 |  +5| 5.2|+5.2|false|true|1|<6.79 , 5.24 >|<+6.789599895, +5.237520218>|<  7,   5>|1f4|{18:x}|{1f4}");
	}

	ZETest("ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12, const ZEVariant& Arg13, const ZEVariant& Arg14, const ZEVariant& Arg15, const ZEVariant& Arg16, const ZEVariant& Arg17, const ZEVariant& Arg18, const ZEVariant& Arg19, const ZEVariant& Arg20)")
	{
		ZEVariant Arg0;
		ZEVariant Arg1;
		ZEVariant Arg2;
		ZEVariant Arg3;
		ZEVariant Arg4;
		ZEVariant Arg5;
		ZEVariant Arg6;
		ZEVariant Arg7;
		ZEVariant Arg8;
		ZEVariant Arg9;
		ZEVariant Arg10;
		ZEVariant Arg11;
		ZEVariant Arg12;
		ZEVariant Arg13;
		ZEVariant Arg14;
		ZEVariant Arg15;
		ZEVariant Arg16;
		ZEVariant Arg17;
		ZEVariant Arg18;
		ZEVariant Arg19;
		ZEVariant Arg20;
		ZEString Result;

		Arg0.SetBoolean(true);
		Arg1.SetBoolean(false);
		Arg2.SetDouble(120.2);
		Arg3.SetFloat(510.3f);
		Arg4.SetInt16(-30);
		Arg5.SetInt16(30);
		Arg6.SetInt32(520);
		Arg7.SetInt64(1552879);
		Arg8.SetInt8(110);
		Arg9.SetInt8(-28);
		Arg10.SetMatrix3x3(ZEMatrix3x3(34.0f, 21.0f, -13.0f,
			8.0f, -5.0f, 3.0f,
			2.0f, 1.0f, -1.0f));
		Arg11.SetMatrix4x4(ZEMatrix4x4(-35.0f, 23.0f, 16.0f, 12.0f,
			10.0f, 9.0f, 9.0f, -9.0f, 
			10.0f, 1.0f, -2.0f, 3.0f,
			4.0f, -5.0f, 6.0f, 7.0f));
		Arg12.SetQuaternion(ZEQuaternion(ZE_PI_8, ZEVector3::UnitZ));
		Arg13.SetString("ZE");
		Arg14.SetUInt16(890);
		Arg15.SetUInt32(93500);
		Arg16.SetUInt64(9876543210);
		Arg17.SetUInt8(78);
		Arg18.SetVector2(ZEVector2(6.7896f, 5.23752f));
		Arg19.SetVector3(ZEVector3(9.3f, 9.8f, -7.3f));
		Arg20.SetVector4(ZEVector4(3.5f, 7.6f, 2.4f, 8.1f));

		Result = ZEFormat::Format("{0:false}|{1:true}|{2:5.}|{3::}|{4:x}|{5:x}|{6:d:5.}|{7:d:11}|{8:d:+}|{9:d:+}|{10:.2}|{11:.}|{12:+.1}|{13:new}|{14:d:+-8.}|0x{15:x:8}|0x{16:X}|{17:c:-3}|{18:+.3}|{19:5.0}|{20:+-5.}", Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14, Arg15, Arg16, Arg17, Arg18, Arg19, Arg20);
		ZETestCheck(Result == "false|false|  120|510.299988|ffffffe2|1e|  520|    1552879|+110|-28|<34.00, 21.00, -13.00, 8.00, -5.00, 3.00, 2.00, 1.00, -1.00>|<-35, 23, 16, 12, 10, 9, 9, -9, 10, 1, -2, 3, 4, -5, 6, 7>|<+0.0, +0.0, +0.2, +1.0>|ZE|890     |0x   16d3c|0x24CB016EA|N  |<+6.790, +5.238>|<    9,    10,    -7>|<+4   , +8   , +2   , +8   >");
	}

	ZETest("ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12, const ZEVariant& Arg13, const ZEVariant& Arg14, const ZEVariant& Arg15, const ZEVariant& Arg16, const ZEVariant& Arg17, const ZEVariant& Arg18, const ZEVariant& Arg19, const ZEVariant& Arg20, const ZEVariant& Arg21)")
	{
		ZEVariant Arg0;
		ZEVariant Arg1;
		ZEVariant Arg2;
		ZEVariant Arg3;
		ZEVariant Arg4;
		ZEVariant Arg5;
		ZEVariant Arg6;
		ZEVariant Arg7;
		ZEVariant Arg8;
		ZEVariant Arg9;
		ZEVariant Arg10;
		ZEVariant Arg11;
		ZEVariant Arg12;
		ZEVariant Arg13;
		ZEVariant Arg14;
		ZEVariant Arg15;
		ZEVariant Arg16;
		ZEVariant Arg17;
		ZEVariant Arg18;
		ZEVariant Arg19;
		ZEVariant Arg20;
		ZEVariant Arg21;
		ZEString Result;

		Arg0.SetBoolean(true);
		Arg1.SetBoolean(false);
		Arg2.SetDouble(120.2);
		Arg3.SetFloat(510.3f);
		Arg4.SetInt16(-30);
		Arg5.SetInt16(30);
		Arg6.SetInt32(520);
		Arg7.SetInt64(1552879);
		Arg8.SetInt8(110);
		Arg9.SetInt8(-28);
		Arg10.SetMatrix3x3(ZEMatrix3x3(34.0f, 21.0f, -13.0f,
			8.0f, -5.0f, 3.0f,
			2.0f, 1.0f, -1.0f));
		Arg11.SetMatrix4x4(ZEMatrix4x4(-35.0f, 23.0f, 16.0f, 12.0f,
			10.0f, 9.0f, 9.0f, -9.0f, 
			10.0f, 1.0f, -2.0f, 3.0f,
			4.0f, -5.0f, 6.0f, 7.0f));
		Arg12.SetQuaternion(ZEQuaternion(ZE_PI_8, ZEVector3::UnitZ));
		Arg13.SetString("ZE");
		Arg14.SetUInt16(890);
		Arg15.SetUInt32(93500);
		Arg16.SetUInt64(9876543210);
		Arg17.SetUInt8(78);
		Arg18.SetVector2(ZEVector2(6.7896f, 5.23752f));
		Arg19.SetVector3(ZEVector3(9.3f, 9.8f, -7.3f));
		Arg20.SetVector4(ZEVector4(3.5f, 7.6f, 2.4f, 8.1f));
		Arg21.SetUInt16(58);

		Result = ZEFormat::Format("{0:false}|{1:true}|{2:5.}|{3::}|{4:x}|{5:x}|{6:d:5.}|{7:d:11}|{8:d:+}|{9:d:+}|{10:.2}|{11:.}|{12:+.1}|{13:new}|{14:d:+-8.}|0x{15:x:8}|0x{16:X}|{17:c:-3}|{18:+.3}|{19:5.0}|{20:+-5.}|{{{21:x}}}", Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14, Arg15, Arg16, Arg17, Arg18, Arg19, Arg20, Arg21);
		ZETestCheck(Result == "false|false|  120|510.299988|ffffffe2|1e|  520|    1552879|+110|-28|<34.00, 21.00, -13.00, 8.00, -5.00, 3.00, 2.00, 1.00, -1.00>|<-35, 23, 16, 12, 10, 9, 9, -9, 10, 1, -2, 3, 4, -5, 6, 7>|<+0.0, +0.0, +0.2, +1.0>|ZE|890     |0x   16d3c|0x24CB016EA|N  |<+6.790, +5.238>|<    9,    10,    -7>|<+4   , +8   , +2   , +8   >|{3a}");
	}

	ZETest("ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12, const ZEVariant& Arg13, const ZEVariant& Arg14, const ZEVariant& Arg15, const ZEVariant& Arg16, const ZEVariant& Arg17, const ZEVariant& Arg18, const ZEVariant& Arg19, const ZEVariant& Arg20, const ZEVariant& Arg21, const ZEVariant& Arg22)")
	{
		ZEVariant Arg0;
		ZEVariant Arg1;
		ZEVariant Arg2;
		ZEVariant Arg3;
		ZEVariant Arg4;
		ZEVariant Arg5;
		ZEVariant Arg6;
		ZEVariant Arg7;
		ZEVariant Arg8;
		ZEVariant Arg9;
		ZEVariant Arg10;
		ZEVariant Arg11;
		ZEVariant Arg12;
		ZEVariant Arg13;
		ZEVariant Arg14;
		ZEVariant Arg15;
		ZEVariant Arg16;
		ZEVariant Arg17;
		ZEVariant Arg18;
		ZEVariant Arg19;
		ZEVariant Arg20;
		ZEVariant Arg21;
		ZEVariant Arg22;
		ZEString Result;

		Arg0.SetBoolean(true);
		Arg1.SetBoolean(false);
		Arg2.SetDouble(120.2);
		Arg3.SetFloat(510.3f);
		Arg4.SetInt16(-30);
		Arg5.SetInt16(30);
		Arg6.SetInt32(520);
		Arg7.SetInt64(1552879);
		Arg8.SetInt8(110);
		Arg9.SetInt8(-28);
		Arg10.SetMatrix3x3(ZEMatrix3x3(34.0f, 21.0f, -13.0f,
			8.0f, -5.0f, 3.0f,
			2.0f, 1.0f, -1.0f));
		Arg11.SetMatrix4x4(ZEMatrix4x4(-35.0f, 23.0f, 16.0f, 12.0f,
			10.0f, 9.0f, 9.0f, -9.0f, 
			10.0f, 1.0f, -2.0f, 3.0f,
			4.0f, -5.0f, 6.0f, 7.0f));
		Arg12.SetQuaternion(ZEQuaternion(ZE_PI_8, ZEVector3::UnitZ));
		Arg13.SetString("ZE");
		Arg14.SetUInt16(890);
		Arg15.SetUInt32(93500);
		Arg16.SetUInt64(9876543210);
		Arg17.SetUInt8(78);
		Arg18.SetVector2(ZEVector2(6.7896f, 5.23752f));
		Arg19.SetVector3(ZEVector3(9.3f, 9.8f, -7.3f));
		Arg20.SetVector4(ZEVector4(3.5f, 7.6f, 2.4f, 8.1f));
		Arg21.SetUInt16(58);
		Arg22.SetUInt32(486);

		Result = ZEFormat::Format("{{22:x}}|{{{21:x}}}|{20:+-5.}|{19:5.0}|{18:+.3}|{17:c:-3}|0x{16:X}|0x{15:x:8}|{14:d:+-8.}|{13:new}|{12:+.1}|{11:.}|{10:.2}|{9:d:+}|{8:d:+}|{7:d:11}|{6:d:5.}|{5:x}|{4:x}|{3::}|{2:5.}|{1:true}|{0:false}", Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14, Arg15, Arg16, Arg17, Arg18, Arg19, Arg20, Arg21, Arg22);
		ZETestCheck(Result == "{22:x}|{3a}|<+4   , +8   , +2   , +8   >|<    9,    10,    -7>|<+6.790, +5.238>|N  |0x24CB016EA|0x   16d3c|890     |ZE|<+0.0, +0.0, +0.2, +1.0>|<-35, 23, 16, 12, 10, 9, 9, -9, 10, 1, -2, 3, 4, -5, 6, 7>|<34.00, 21.00, -13.00, 8.00, -5.00, 3.00, 2.00, 1.00, -1.00>|-28|+110|    1552879|  520|1e|ffffffe2|510.299988|  120|false|false");
	}

	ZETest("ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12, const ZEVariant& Arg13, const ZEVariant& Arg14, const ZEVariant& Arg15, const ZEVariant& Arg16, const ZEVariant& Arg17, const ZEVariant& Arg18, const ZEVariant& Arg19, const ZEVariant& Arg20, const ZEVariant& Arg21, const ZEVariant& Arg22, const ZEVariant& Arg23)")
	{
		ZEVariant Arg0;
		ZEVariant Arg1;
		ZEVariant Arg2;
		ZEVariant Arg3;
		ZEVariant Arg4;
		ZEVariant Arg5;
		ZEVariant Arg6;
		ZEVariant Arg7;
		ZEVariant Arg8;
		ZEVariant Arg9;
		ZEVariant Arg10;
		ZEVariant Arg11;
		ZEVariant Arg12;
		ZEVariant Arg13;
		ZEVariant Arg14;
		ZEVariant Arg15;
		ZEVariant Arg16;
		ZEVariant Arg17;
		ZEVariant Arg18;
		ZEVariant Arg19;
		ZEVariant Arg20;
		ZEVariant Arg21;
		ZEVariant Arg22;
		ZEVariant Arg23;
		ZEString Result;

		Arg0.SetBoolean(true);
		Arg1.SetBoolean(false);
		Arg2.SetDouble(120.2);
		Arg3.SetFloat(510.3f);
		Arg4.SetInt16(-30);
		Arg5.SetInt16(30);
		Arg6.SetInt32(520);
		Arg7.SetInt64(1552879);
		Arg8.SetInt8(110);
		Arg9.SetInt8(-28);
		Arg10.SetMatrix3x3(ZEMatrix3x3(34.0f, 21.0f, -13.0f,
			8.0f, -5.0f, 3.0f,
			2.0f, 1.0f, -1.0f));
		Arg11.SetMatrix4x4(ZEMatrix4x4(-35.0f, 23.0f, 16.0f, 12.0f,
			10.0f, 9.0f, 9.0f, -9.0f, 
			10.0f, 1.0f, -2.0f, 3.0f,
			4.0f, -5.0f, 6.0f, 7.0f));
		Arg12.SetQuaternion(ZEQuaternion(ZE_PI_8, ZEVector3::UnitZ));
		Arg13.SetString("ZE");
		Arg14.SetUInt16(890);
		Arg15.SetUInt32(93500);
		Arg16.SetUInt64(9876543210);
		Arg17.SetUInt8(78);
		Arg18.SetVector2(ZEVector2(6.7896f, 5.23752f));
		Arg19.SetVector3(ZEVector3(9.3f, 9.8f, -7.3f));
		Arg20.SetVector4(ZEVector4(3.5f, 7.6f, 2.4f, 8.1f));
		Arg21.SetUInt16(58);
		Arg22.SetUInt32(486);
		Arg23.SetUInt64(0);

		Result = ZEFormat::Format("{0:false}|{1:true}|{2:5.}|{3::}|{4:x}|{5:x}|{6:d:5.}|{7:d:11}|{8:d:+}|{9:d:+}|{10:.2}|{11:.}|{12:+.1}|{13:new}|{14:d:+-8.}|0x{15:x:8}|0x{16:X}|{17:c:-3}|{18:+.3}|{19:5.0}|{20:+-5.}|{{{21:x}}}|{{22:x}}|{23:x}", Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14, Arg15, Arg16, Arg17, Arg18, Arg19, Arg20, Arg21, Arg22, Arg23);
		ZETestCheck(Result == "false|false|  120|510.299988|ffffffe2|1e|  520|    1552879|+110|-28|<34.00, 21.00, -13.00, 8.00, -5.00, 3.00, 2.00, 1.00, -1.00>|<-35, 23, 16, 12, 10, 9, 9, -9, 10, 1, -2, 3, 4, -5, 6, 7>|<+0.0, +0.0, +0.2, +1.0>|ZE|890     |0x   16d3c|0x24CB016EA|N  |<+6.790, +5.238>|<    9,    10,    -7>|<+4   , +8   , +2   , +8   >|{3a}|{22:x}|0");
	}

	ZETest("ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12, const ZEVariant& Arg13, const ZEVariant& Arg14, const ZEVariant& Arg15, const ZEVariant& Arg16, const ZEVariant& Arg17, const ZEVariant& Arg18, const ZEVariant& Arg19, const ZEVariant& Arg20, const ZEVariant& Arg21, const ZEVariant& Arg22, const ZEVariant& Arg23, const ZEVariant& Arg24)")
	{
		ZEVariant Arg0;
		ZEVariant Arg1;
		ZEVariant Arg2;
		ZEVariant Arg3;
		ZEVariant Arg4;
		ZEVariant Arg5;
		ZEVariant Arg6;
		ZEVariant Arg7;
		ZEVariant Arg8;
		ZEVariant Arg9;
		ZEVariant Arg10;
		ZEVariant Arg11;
		ZEVariant Arg12;
		ZEVariant Arg13;
		ZEVariant Arg14;
		ZEVariant Arg15;
		ZEVariant Arg16;
		ZEVariant Arg17;
		ZEVariant Arg18;
		ZEVariant Arg19;
		ZEVariant Arg20;
		ZEVariant Arg21;
		ZEVariant Arg22;
		ZEVariant Arg23;
		ZEVariant Arg24;
		ZEString Result;

		Arg0.SetBoolean(true);
		Arg1.SetBoolean(false);
		Arg2.SetDouble(120.2);
		Arg3.SetFloat(510.3f);
		Arg4.SetInt16(-30);
		Arg5.SetInt16(30);
		Arg6.SetInt32(520);
		Arg7.SetInt64(1552879);
		Arg8.SetInt8(110);
		Arg9.SetInt8(-28);
		Arg10.SetMatrix3x3(ZEMatrix3x3(34.0f, 21.0f, -13.0f,
			8.0f, -5.0f, 3.0f,
			2.0f, 1.0f, -1.0f));
		Arg11.SetMatrix4x4(ZEMatrix4x4(-35.0f, 23.0f, 16.0f, 12.0f,
			10.0f, 9.0f, 9.0f, -9.0f, 
			10.0f, 1.0f, -2.0f, 3.0f,
			4.0f, -5.0f, 6.0f, 7.0f));
		Arg12.SetQuaternion(ZEQuaternion(ZE_PI_8, ZEVector3::UnitZ));
		Arg13.SetString("ZE");
		Arg14.SetUInt16(890);
		Arg15.SetUInt32(93500);
		Arg16.SetUInt64(9876543210);
		Arg17.SetUInt8(78);
		Arg18.SetVector2(ZEVector2(6.7896f, 5.23752f));
		Arg19.SetVector3(ZEVector3(9.3f, 9.8f, -7.3f));
		Arg20.SetVector4(ZEVector4(3.5f, 7.6f, 2.4f, 8.1f));
		Arg21.SetUInt16(58);
		Arg22.SetUInt32(486);
		Arg23.SetUInt64(0);
		Arg24.SetUInt64(8562071);

		Result = ZEFormat::Format("{0:false}|{1:true}|{2:5.}|{3::}|{4:x}|{5:x}|{6:d:5.}|{7:d:11}|{8:d:+}|{9:d:+}|{10:.2}|{11:.}|{12:+.1}|{13:new}|{14:d:+-8.}|0x{15:x:8}|0x{16:X}|{17:c:-3}|{18:+.3}|{19:5.0}|{20:+-5.}|{{{21:x}}}|{{22:x}}|{23:x}|{24:d:010}", Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14, Arg15, Arg16, Arg17, Arg18, Arg19, Arg20, Arg21, Arg22, Arg23, Arg24);
		ZETestCheck(Result == "false|false|  120|510.299988|ffffffe2|1e|  520|    1552879|+110|-28|<34.00, 21.00, -13.00, 8.00, -5.00, 3.00, 2.00, 1.00, -1.00>|<-35, 23, 16, 12, 10, 9, 9, -9, 10, 1, -2, 3, 4, -5, 6, 7>|<+0.0, +0.0, +0.2, +1.0>|ZE|890     |0x   16d3c|0x24CB016EA|N  |<+6.790, +5.238>|<    9,    10,    -7>|<+4   , +8   , +2   , +8   >|{3a}|{22:x}|0|0008562071");
	}
}
