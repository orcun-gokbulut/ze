//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFormatArgumentTests.cpp
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
#include "ZEStringWriter.h"
#include "ZEFormatArgument.h"
#include "ZEMath/ZEAngle.h"

ZE_TEST(ZEFormatArgument)
{
	ZE_TEST_ITEM("bool ZEFormatArgument::Format(ZEStringWriter& Output, ZEInt Argument, const char* ArgumentFormat)")
	{
		ZEInt Argument = -124;
		ZEString String;
		ZEStringWriter Output;

		bool Result = ZEFormatArgument::Format(Output, Argument, "d:-6");
		ZE_TEST_CHECK_ERROR(Result);
		Output.AppendEnd();
		Output.Output(String);
		ZE_TEST_CHECK_ERROR(String == "-124  ");
	}

	ZE_TEST_ITEM("bool ZEFormatArgument::Format(ZEStringWriter& Output, ZEUInt Argument, const char* ArgumentFormat)")
	{
		ZEUInt Argument = 48567;
		ZEString String;
		ZEStringWriter Output;

		bool Result = ZEFormatArgument::Format(Output, Argument, "d:+06");
		ZE_TEST_CHECK_ERROR(Result);
		Output.AppendEnd();
		Output.Output(String);
		ZE_TEST_CHECK_ERROR(String == "048567");
	}

	ZE_TEST_ITEM("bool ZEFormatArgument::Format(ZEStringWriter& Output, ZEInt64 Argument, const char* ArgumentFormat)")
	{
		ZEInt64 Argument = 987654321;
		ZEString String;
		ZEStringWriter Output;

		bool Result = ZEFormatArgument::Format(Output, Argument, "x:016");
		ZE_TEST_CHECK_ERROR(Result);
		Output.AppendEnd();
		Output.Output(String);
		ZE_TEST_CHECK_ERROR(String == "000000003ade68b1");
	}

	ZE_TEST_ITEM("bool ZEFormatArgument::Format(ZEStringWriter& Output, ZEUInt64 Argument, const char* ArgumentFormat)")
	{
		ZEUInt64 Argument = 26578319;
		ZEString String;
		ZEStringWriter Output;

		bool Result = ZEFormatArgument::Format(Output, Argument, "X:-16");
		ZE_TEST_CHECK_ERROR(Result);
		Output.AppendEnd();
		Output.Output(String);
		ZE_TEST_CHECK_ERROR(String == "1958D8F         ");
	}

	ZE_TEST_ITEM("bool ZEFormatArgument::Format(ZEStringWriter& Output, float Argument, const char* ArgumentFormat)")
	{
		float ArgumentPos = 10.236;
		float ArgumentNeg = -10.236;
		ZEString String;
		ZEStringWriter Output;

		bool Result = ZEFormatArgument::Format(Output, ArgumentPos, "5.");
		ZE_TEST_CHECK_ERROR(Result);
		Output.AppendEnd();
		Output.Output(String);
		ZE_TEST_CHECK_ERROR(String == "   10");

		Result = ZEFormatArgument::Format(Output, ArgumentNeg, "5.");
		ZE_TEST_CHECK_ERROR(Result);
		Output.AppendEnd();
		Output.Output(String);
		ZE_TEST_CHECK_ERROR(String == "  -10");
	}

	ZE_TEST_ITEM("bool ZEFormatArgument::Format(ZEStringWriter& Output, bool Argument, const char* ArgumentFormat)")
	{
		bool Argument = true;
		ZEString String;
		ZEStringWriter Output;

		bool Result = ZEFormatArgument::Format(Output, Argument, "false");
		ZE_TEST_CHECK_ERROR(Result);
		Output.AppendEnd();
		Output.Output(String);
		ZE_TEST_CHECK_ERROR(String == "false");
	}

	ZE_TEST_ITEM("bool ZEFormatArgument::Format(ZEStringWriter& Output, const ZEVector2& Argument, const char* ArgumentFormat)")
	{
		ZEVector2 Argument(3.8f, -5.7f);
		ZEString String;
		ZEStringWriter Output;

		bool Result = ZEFormatArgument::Format(Output, Argument, ".");
		ZE_TEST_CHECK_ERROR(Result);
		Output.AppendEnd();
		Output.Output(String);
		ZE_TEST_CHECK_ERROR(String == "<4, -6>");

		ZE_TEST_CASE("ArgumentFormat ':' ")
		{
			Result = ZEFormatArgument::Format(Output, Argument, ":");
			ZE_TEST_CHECK_ERROR(Result);
			Output.AppendEnd();
			Output.Output(String);
			ZE_TEST_CHECK_ERROR(String == "<3.800, -5.700>");
		}
	}

	ZE_TEST_ITEM("bool ZEFormatArgument::Format(ZEStringWriter& Output, const ZEVector3& Argument, const char* ArgumentFormat)")
	{
		ZEVector3 Argument(2.1f, 6.9f, -1.6f);
		ZEString String;
		ZEStringWriter Output;

		bool Result = ZEFormatArgument::Format(Output, Argument, "+7.2");
		ZE_TEST_CHECK_ERROR(Result);
		Output.AppendEnd();
		Output.Output(String);
		ZE_TEST_CHECK_ERROR(String == "<  +2.10,   +6.90,   -1.60>");
	}

	ZE_TEST_ITEM("bool ZEFormatArgument::Format(ZEStringWriter& Output, const ZEVector4& Argument, const char* ArgumentFormat)")
	{
		ZEVector4 Argument(-1.3f, 3.6f, 3.0f, -5.7f);
		ZEString String;
		ZEStringWriter Output;

		bool Result = ZEFormatArgument::Format(Output, Argument, "-5.");
		ZE_TEST_CHECK_ERROR(Result);
		Output.AppendEnd();
		Output.Output(String);
		ZE_TEST_CHECK_ERROR(String == "<-1   , 4    , 3    , -6   >");
	}

	ZE_TEST_ITEM("bool ZEFormatArgument::Format(ZEStringWriter& Output, const ZEQuaternion& Argument, const char* ArgumentFormat)")
	{
		ZEQuaternion Argument(ZE_PI_4, ZEVector3::UnitX);
		ZEString String;
		ZEStringWriter Output;

		bool Result = ZEFormatArgument::Format(Output, Argument, "+-06.1");
		ZE_TEST_CHECK_ERROR(Result);
		Output.AppendEnd();
		Output.Output(String);
		ZE_TEST_CHECK_ERROR(String == "<+0.4  , +0.0  , +0.0  , +0.9  >");
	}

	ZE_TEST_ITEM("bool ZEFormatArgument::Format(ZEStringWriter& Output, const ZEMatrix3x3& Argument, const char* ArgumentFormat)")
	{
		ZEMatrix3x3 Argument(34.0f, 21.0f, -13.0f,
			8.0f, -5.0f, 3.0f,
			2.0f, 1.0f, -1.0f);
		ZEString String;
		ZEStringWriter Output;

		bool Result = ZEFormatArgument::Format(Output, Argument, "08.1");
		ZE_TEST_CHECK_ERROR(Result);
		Output.AppendEnd();
		Output.Output(String);
		ZE_TEST_CHECK_ERROR(String == "<000034.0, 000021.0, -00013.0, 000008.0, -00005.0, 000003.0, 000002.0, 000001.0, -00001.0>");
	}

	ZE_TEST_ITEM("bool ZEFormatArgument::Format(ZEStringWriter& Output, const ZEMatrix4x4& Argument, const char* ArgumentFormat)")
	{
		ZEMatrix4x4 Argument(-35.0f, 23.0f, 16.0f, 12.0f,
			10.0f, 9.0f, 9.0f, -9.0f, 
			10.0f, 1.0f, -2.0f, 3.0f,
			4.0f, -5.0f, 6.0f, 7.0f);
		ZEString String;
		ZEStringWriter Output;

		bool Result = ZEFormatArgument::Format(Output, Argument, "+05.");
		ZE_TEST_CHECK_ERROR(Result);
		Output.AppendEnd();
		Output.Output(String);
		ZE_TEST_CHECK_ERROR(String == "<-0035, +0023, +0016, +0012, +0010, +0009, +0009, -0009, +0010, +0001, -0002, +0003, +0004, -0005, +0006, +0007>");
	}

	ZE_TEST_ITEM("bool ZEFormatArgument::Format(ZEStringWriter& Output, const ZEString& Argument, const char* ArgumentFormat)")
	{
		ZEString Argument("String");
		ZEString String;
		ZEStringWriter Output;

		bool Result = ZEFormatArgument::Format(Output, Argument, "test");
		ZE_TEST_CHECK_ERROR(Result);
		Output.AppendEnd();
		Output.Output(String);
		ZE_TEST_CHECK_ERROR(String == "String");
	}

	ZE_TEST_ITEM("bool ZEFormatArgument::Format(ZEStringWriter& Output, ZEObject* Argument, const char* ArgumentFormat)")
	{

	}

	ZE_TEST_ITEM("bool ZEFormatArgument::FormatVariant(ZEStringWriter& Output, const ZEVariant& Argument, const char* ArgumentFormat)")
	{
		ZEStringWriter Output;
		ZEVariant Argument;
		ZEString String;
		bool Result;

		ZE_TEST_CASE("for ZE_VRT_FLOAT")
		{
			Argument.SetFloat(3.7f);
			
			Result = ZEFormatArgument::FormatVariant(Output, Argument, "-+7.3");
			ZE_TEST_CHECK_ERROR(Result);
			Output.AppendEnd();
			Output.Output(String);
			ZE_TEST_CHECK_ERROR(String == "+3.700 ");
		}

		ZE_TEST_CASE("for ZE_VRT_DOUBLE")
		{
			Argument.SetDouble(3.7);

			Result = ZEFormatArgument::FormatVariant(Output, Argument, "+.");
			ZE_TEST_CHECK_ERROR(Result);
			Output.AppendEnd();
			Output.Output(String);
			ZE_TEST_CHECK_ERROR(String == "+4");
		}

		ZE_TEST_CASE("Invalid ArgumentFormat for ZE_VRT_INTEGER_64")
		{
			Argument.SetInt64(6824610);

			Result = ZEFormatArgument::FormatVariant(Output, Argument, "10");
			ZE_TEST_CHECK_ERROR(!Result);
			Output.AppendEnd();
			Output.Output(String);
			ZE_TEST_CHECK_ERROR(String == "");
		}

		ZE_TEST_CASE("Invalid ArgumentFormat for ZE_VRT_VECTOR2")
		{
			Argument.SetVector2(ZEVector2(5.82, 11.3));

			Result = ZEFormatArgument::FormatVariant(Output, Argument, "05d");
			ZE_TEST_CHECK_ERROR(!Result);
			Output.AppendEnd();
			Output.Output(String);
			ZE_TEST_CHECK_ERROR(String == "");
		}
	}
}
