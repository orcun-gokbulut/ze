//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEStringWriterTests.cpp
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
#include "ZEVariant.h"
#include "ZEFormat.h"
#include "ZEArray.h"

ZEInt CompareFunc(const ZEInt *F, const ZEInt *E)
{
	ZEInt A = *F;
	ZEInt B = *E;

	ZEInt ReturnValue = 0;

	if (A > B)
		ReturnValue = 1;
	else if (A == B)
		ReturnValue = 0;
	else if (A < B)
		ReturnValue = -1;

	return ReturnValue;
};

ZETestSuite(ZEStringWriter)
{
	ZETest("ZEStringWriter::ZEStringWriter()")
	{
		ZEStringWriter Writer;

		ZEString Output;

		Writer.Output(Output);
		ZETestCheck(Output == NULL);
		ZETestCheckEqual(Output.GetSize(), 0);
	}

	ZETest("ZEStringWriter::~ZEStringWriter()")
	{
		ZEStringWriter Writer;
		ZEString Output;

		Writer.Append("test");

		Writer.~ZEStringWriter();

		Writer.Output(Output);
		ZETestCheck(Output == NULL);
		ZETestCheckEqual(Output.GetSize(), 0);
	}

	ZETest("void ZEStringWriter::Append(const char* String)")
	{
		ZEStringWriter Writer;
		ZEString Output;
		const char* String1 = "New ";
		const char* String2 = "Test";
		const char* String3 = " String";
		
		Writer.Append(String1);

		Writer.Append(String2);

		Writer.Append(String3);

		Writer.AppendEnd();

		Writer.Output(Output);
		ZETestCheck(Output == "New Test String");
		ZETestCheck(Output.GetSize() == 16);

		ZETestCase("call Append(const char* String) after AppendEnd()")
		{
			Writer.Append(String1);
			Writer.AppendEnd();

			Writer.Append(String2);
			Writer.AppendEnd();

			Writer.Append(String3);
			Writer.AppendEnd();

			Writer.Output(Output);
			ZETestCheckString(Output, "New ");
			ZETestCheckEqual(Output.GetSize(), 18);
		}

		ZETestCase("append string and caharacter")
		{
			const char* String = "Zine";
			char Character = 'k';

			Writer.Append(String);
			Writer.Append(Character);
			Writer.AppendEnd();

			Writer.Output(Output);
			ZETestCheckString(Output, "Zinek");
			ZETestCheckEqual(Output.GetSize(), 6);
		}

		ZETestCase("append the previous output string and a new string")
		{
			const char* NewString = " Engine";

			Writer.Append(Output);
			Writer.Append(NewString);
			Writer.AppendEnd();

			Writer.Output(Output);
			ZETestCheckString(Output, "Zinek Engine");
			ZETestCheckEqual(Output.GetSize(), 13);
		}

		ZETestCase("UTF-8 encoding compatibility test")
		{
			const char* StringA = "Işık ";
			const char* StringB = "çok";
			const char* StringC = " süt ";
			const char* StringD = "iç.";

			Writer.Append(StringA);
			Writer.Append(StringB);
			Writer.Append(StringC);
			Writer.Append(StringD);
			Writer.AppendEnd();

			Writer.Output(Output);
			ZETestCheckString(Output, "Işık çok süt iç.");
			ZETestCheckEqual(Output.GetSize(), 22);
		}

		ZETestCase("append a formatted string to Writer")
		{
			ZEVariant Arg0;
			ZEVariant Arg1;
			ZEVariant Arg2;
			ZEVariant Arg3;
			ZEVariant Arg4;

			ZEArray<ZEInt> Array;
			Array.Add(602);
			Array.Add(98);
			Array.Add(12);
			Array.Add(126);
			Array.Add(500);
			Array.Sort(&CompareFunc);

			Arg0.SetInt32(Array[0]);
			Arg1.SetInt32(Array[1]);
			Arg2.SetInt32(Array[2]);
			Arg3.SetInt32(Array[3]);
			Arg4.SetInt32(Array[4]);

			Writer.Append("The sorted array elements' hexadecimal equivalents : ");
			Writer.Append(ZEFormat::Format("First item 0x{0:X}, Second item 0x{1:X}, Third item 0x{2:X}, Fourth item 0x{3:X}, Fifth item 0x{4:X}", Arg0, Arg1, Arg2, Arg3, Arg4));
			Writer.AppendEnd();

			Writer.Output(Output);
			ZETestCheckString(Output, "The sorted array elements' hexadecimal equivalents : First item 0xC, Second item 0x62, Third item 0x7E, Fourth item 0x1F4, Fifth item 0x25A");
		}
	}

	ZETest("void ZEStringWriter::Append(char Character)")
	{
		ZEStringWriter Writer;
		ZEString Output;
		char Character1 = 'Z';
		char Character2 = 'E';

		Writer.Append(Character1);
		Writer.Append(Character2);
		Writer.AppendEnd();

		Writer.Output(Output);
		ZETestCheckString(Output, "ZE");
		ZETestCheckEqual(Output.GetSize(), 3);
	}

	ZETest("void ZEStringWriter::AppendUTF8(char* Character)")
	{
		ZEStringWriter Writer;
		ZEString Output;

		ZETestCase("append € and ü")
		{
			char* Character1 = "€";
			char* Character2 = "ü";

			Writer.AppendUTF8(Character1);
			Writer.AppendUTF8(Character2);
			Writer.AppendEnd();

			Writer.Output(Output);
			ZETestCheckString(Output, "€ü");
			ZETestCheckEqual(Output.GetSize(), 6);
		}

		ZETestCase("append euro sign with its hex equivalent")
		{
			char* Character = "\xe2\x82\xac";

			Writer.AppendUTF8(Character);
			Writer.AppendEnd();
			
			Writer.Output(Output);
			ZETestCheckString(Output, "€");
			ZETestCheckEqual(Output.GetSize(), 4);
		}

		ZETestCase("append 1 byte, 2 byte and 3 byte chars respectively")
		{
			char* Character1 = "$";
			char* Character2 = "£";
			char* Character3 = "€";

			Writer.AppendUTF8(Character1);
			Writer.AppendUTF8(Character2);
			Writer.AppendUTF8(Character3);
			Writer.AppendEnd();

			Writer.Output(Output);
			ZETestCheckString(Output, "$£€");
			ZETestCheckEqual(Output.GetSize(), 7);
		}

		ZETestCase("use all append functions")
		{
			Writer.Append("The most common 3 currency units are ");
			Writer.AppendUTF8("$");
			Writer.Append(", ");
			Writer.AppendUTF8("€");
			Writer.Append(" and ");
			Writer.AppendUTF8("£");
			Writer.Append('.');
			Writer.AppendEnd();

			Writer.Output(Output);
			ZETestCheckString(Output, "The most common 3 currency units are $, € and £.");
			ZETestCheckEqual(Output.GetSize(), 52);
		}

		ZETestCase("append a 4 byte character with its hex equivalent")
		{
			char* Character = "\xf0\xa0\x9c\x8e";

			Writer.AppendUTF8(Character);
			Writer.AppendEnd();

			Writer.Output(Output);
			ZETestCheck(Output == Character);
			ZETestCheckEqual(Output.GetSize(), 5);
		}

		ZETestCase("multiple chars")
		{
			char Character1[] = "Işık";
			char* Character2 = "çok";
			char* Character3 = "süt";
			char* Character4 = "iç";

			Writer.AppendUTF8(Character1);
			Writer.AppendEnd();

			Writer.Output(Output);
			ZETestCheckString(Output, "I");
			ZETestCheckEqual(Output.GetSize(), 2);

			Writer.AppendUTF8(Character2);
			Writer.AppendEnd();

			Writer.Output(Output);
			ZETestCheckString(Output, "ç");
			ZETestCheckEqual(Output.GetSize(), 3);

			Writer.AppendUTF8(Character3);
			Writer.AppendEnd();

			Writer.Output(Output);
			ZETestCheckString(Output, "s");
			ZETestCheckEqual(Output.GetSize(), 2);

			Writer.AppendUTF8(Character4);
			Writer.AppendEnd();

			Writer.Output(Output);
			ZETestCheckString(Output, "i");
			ZETestCheckEqual(Output.GetSize(), 2);

			Writer.AppendUTF8(Character1);
			Writer.AppendUTF8(Character2);
			Writer.AppendUTF8(Character3);
			Writer.AppendUTF8(Character4);
			Writer.Append('\0');

			Writer.Output(Output);
			ZETestCheckString(Output, "Içsi");
			ZETestCheckEqual(Output.GetSize(), 6);

			char* Char1 = "I";
			char* Char2 = "ş";
			char* Char3 = "ı";
			char* Char4 = "k";

			Writer.AppendUTF8(Char1);
			Writer.AppendUTF8(Char2);
			Writer.AppendUTF8(Char3);
			Writer.AppendUTF8(Char4);
			Writer.AppendEnd();

			Writer.Output(Output);
			ZETestCheckString(Output, "Işık");
			ZETestCheckEqual(Output.GetSize(), 7);
		}
	}

	ZETest("void ZEStringWriter::AppendEnd()")
	{
		ZEStringWriter Writer;
		ZEString Output;

		Writer.AppendEnd();

		Writer.Output(Output);
		ZETestCheck(Output == "");
		ZETestCheckEqual(Output.GetSize(), 1);

		const char* String = "ZE";

		Writer.Append(String);

		Writer.Output(Output);
		ZETestCheckEqual(Output.GetSize(), 2);

		Writer.Append(String);
		Writer.AppendEnd();

		Writer.Output(Output);
		ZETestCheckString(Output, "ZE");
		ZETestCheckEqual(Output.GetSize(), 3);
	}

	ZETest("void ZEStringWriter::Clear()")
	{
		ZEStringWriter Writer;
		ZEString Output;
		const char* String = "string";

		Writer.Append(String);
		Writer.AppendEnd();

		Writer.Clear();

		Writer.Output(Output);
		ZETestCheck(Output == NULL);
		ZETestCheckEqual(Output.GetSize(), 0);

		ZETestCase("call Clear after append string then append another string")
		{
			Writer.Append(String);

			Writer.Clear();

			Writer.Append("test clear");
			Writer.AppendEnd();

			Writer.Output(Output);
			ZETestCheckString(Output, "test clear");
			ZETestCheckEqual(Output.GetSize(), 11);
		}
	}

	ZETest("void ZEStringWriter::Output(ZEString& String)")
	{
		ZEStringWriter Writer;
		ZEString Output;

		ZETestCase("assign value to Output string first then call ZEStringWriter functions")
		{
			Output = "string";
			ZETestCheckEqual(Output.GetSize(), 7);

			Writer.Output(Output);
			ZETestCheck(Output == "");
			ZETestCheckEqual(Output.GetSize(), 0);

			Output = "string";
			ZETestCheckEqual(Output.GetSize(), 7);
			Writer.AppendEnd();

			Writer.Output(Output);
			ZETestCheck(Output == "");
			ZETestCheckEqual(Output.GetSize(), 1);
		}

		ZETestCase("assign a value to Output string then append that Output string to Writer")
		{
			Output = "string";
			
			Writer.Append(Output);
			Writer.AppendEnd();

			Writer.Output(Output);
			ZETestCheckString(Output, "string");
			ZETestCheckEqual(Output.GetSize(), 7);
		}

		ZETestCase("use same Output string value for different writers")
		{
			ZEStringWriter Writer1;
			ZEStringWriter Writer2;
			
			Writer1.Append("test");
			Writer1.AppendEnd();

			Writer1.Output(Output);
			ZETestCheckString(Output, "test");
			ZETestCheckEqual(Output.GetSize(), 5);

			Writer2.Append(Output);
			Writer2.Append(" string");
			Writer2.AppendEnd();

			Writer2.Output(Output);
			ZETestCheckString(Output, "test string");
			ZETestCheckEqual(Output.GetSize(), 12);
		}
	}
}
