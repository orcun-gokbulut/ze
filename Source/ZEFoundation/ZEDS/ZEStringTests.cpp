//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEStringTests.cpp
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

#include "ZEDS/ZEString.h"
#include <string>
#include "ZETest/ZETest.h"

ZE_TEST(ZEString)
{
	ZE_TEST_ITEM("void ZEString::Append(const ZEString & String)")
	{
		ZEString StringA = "Lorem";
		ZEString StringB = " Ipsum";
		ZEUInt ExpectedSize = 12;
		ZEUInt ExpectedLength = 11;

		StringA.Append(StringB);

		ZE_TEST_CHECK_ERROR(StringA == "Lorem Ipsum");
		ZE_TEST_CHECK_ERROR(StringA.GetSize() == ExpectedSize);
		ZE_TEST_CHECK_ERROR(StringA.GetLength() == ExpectedLength);

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			ZEString StringC = "Işık Ilık";
			ZEString StringD = " Süt İç";

			StringC.Append(StringD);

			ZE_TEST_CHECK_ERROR(StringC == "Işık Ilık Süt İç");
			ZE_TEST_CHECK_ERROR(StringC.GetSize() ==  23);
			ZE_TEST_CHECK_ERROR(StringC.GetLength() == 16);
		}
	}
	ZE_TEST_ITEM("void ZEString::Append(const char * String)")
	{
		ZEString StringA = "Lorem";
		const char* StringB = " Ipsum";
		ZEUInt ExpectedSize = 12;
		ZEUInt ExpectedLength = 11;

		StringA.Append(StringB);

		ZE_TEST_CHECK_ERROR(StringA == "Lorem Ipsum");
		ZE_TEST_CHECK_ERROR(StringA.GetSize() == ExpectedSize);
		ZE_TEST_CHECK_ERROR(StringA.GetLength() == ExpectedLength);

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			ZEString StringC = "Işık Ilık";
			const char* StringD = " Süt İç";

			StringC.Append(StringD);

			ZE_TEST_CHECK_ERROR(StringC == "Işık Ilık Süt İç");
			ZE_TEST_CHECK_ERROR(StringC.GetSize() == 23);
			ZE_TEST_CHECK_ERROR(StringC.GetLength() == 16);
		}
	}

	ZE_TEST_ITEM("void ZEString::Clear()")
	{
		ZEString String = "Lorem Ipsum";

		String.Clear();

		ZE_TEST_CHECK_ERROR(String.GetLength() == 0);
		ZE_TEST_CHECK_ERROR(String.GetSize() == 0);
	}

	ZE_TEST_ITEM("void ZEString::Compact()")
	{
		ZEString String("ZE");
		ZE_TEST_CHECK_ERROR(String.GetSize() == 3);

		String.SetSize(7);
		ZE_TEST_CHECK_ERROR(String == "ZE");
		ZE_TEST_CHECK_ERROR(String.GetSize() == 7);

		String.Compact();
		ZE_TEST_CHECK_ERROR(String == "ZE");
		ZE_TEST_CHECK_ERROR(String.GetSize() == 3);
	}

	ZE_TEST_ITEM("void ZEString::CopyFrom(const ZEString & String)")
	{
		ZE_TEST_CASE("Copying a string to a previously non-empty ZEString")
		{
			ZEString StringA = "Lorem";
			ZEString StringB = "Ipsum";
			ZEUInt ExpectedSize = 5;
			ZEUInt ExpectedLength = 6;

			StringA.CopyFrom(StringB);

			ZE_TEST_CHECK_ERROR(StringA == "Ipsum");
			ZE_TEST_CHECK_ERROR(StringA.GetLength() == 5);
			ZE_TEST_CHECK_ERROR(StringA.GetSize() == 6);
		}

		ZE_TEST_CASE("Copying a string to a previously empty ZEString")
		{
			ZEString StringA = "Lorem Ipsum";
			ZEString StringB;
			ZEUInt ExpectedSize = 12;
			ZEUInt ExpectedLength = 11;

			StringB.CopyFrom(StringA);

			ZE_TEST_CHECK_ERROR(StringB == "Lorem Ipsum");
			ZE_TEST_CHECK_ERROR(StringB.GetLength() == ExpectedLength);
			ZE_TEST_CHECK_ERROR(StringB.GetSize() == ExpectedSize);
		}

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			ZEString StringC = "Işık Ilık";
			ZEString StringD = "Süt İç";

			StringC.CopyFrom(StringD);

			ZE_TEST_CHECK_ERROR(StringC == "Süt İç");
			ZE_TEST_CHECK_ERROR(StringC.GetSize() == 10);
			ZE_TEST_CHECK_ERROR(StringC.GetLength() == 6);
		}
	}

	ZE_TEST_ITEM("void ZEString::CopyTo(ZEString & String) const")
		{
			ZE_TEST_CASE("Copying a string to a previously empty ZEString")
			{
				ZEString StringA = "Lorem Ipsum";
				ZEString StringB;
				ZEUInt ExpectedSize = 12;
				ZEUInt ExpectedLength = 11;

				StringA.CopyTo(StringB);

				ZE_TEST_CHECK_ERROR(StringB == "Lorem Ipsum");
				ZE_TEST_CHECK_ERROR(StringB.GetLength() == ExpectedLength);
				ZE_TEST_CHECK_ERROR(StringB.GetSize() == ExpectedSize);
			}

			ZE_TEST_CASE("Copying a string to a previously non-empty ZEString")
			{
				ZEString StringA = "Lorem";
				ZEString StringB = "Ipsum Dolor";
				ZEUInt ExpectedSize = 6;
				ZEUInt ExpectedLength = 5;

				StringA.CopyTo(StringB);

				ZE_TEST_CHECK_ERROR(StringB == "Lorem");
				ZE_TEST_CHECK_ERROR(StringB.GetLength() == ExpectedLength);
				ZE_TEST_CHECK_ERROR(StringB.GetSize() == ExpectedSize);
			}

			ZE_TEST_CASE("UTF-8 encoding compatibility test")
			{
				ZEString StringC = "Işık Ilık";
				ZEString StringD = "Süt İç";

				StringC.CopyTo(StringD);

				ZE_TEST_CHECK_ERROR(StringD == "Işık Ilık");
				ZE_TEST_CHECK_ERROR(StringD.GetSize() == 13);
				ZE_TEST_CHECK_ERROR(StringD.GetLength() == 9);
			}
		}

	ZE_TEST_ITEM("bool ZEString::Equals(const ZEString & String) const")
	{
		const ZEString StringA = "Lorem Ipsum";
		const ZEString StringB = "Lorem Ipsum";

		ZE_TEST_CHECK_ERROR(StringA.Equals(StringB));

		ZE_TEST_CASE("Two ZEStrings are not equal")
		{
			const ZEString StringA = "Lorem Ipsum";
			const ZEString StringB = "Dolor Sit";
			ZE_TEST_CHECK_ERROR(!(StringA.Equals(StringB)));
		}

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			ZEString StringC = "Işık Ilık";
			ZEString StringD = "Süt İç";
			ZEString StringE = "Işık Ilık";

			ZE_TEST_CHECK_ERROR(StringC.Equals(StringE));
			ZE_TEST_CHECK_ERROR(!(StringC.Equals(StringD)));
		}
	}

	ZE_TEST_ITEM("bool ZEString::Equals(const char * String) const")
	{
		const ZEString StringA = "Lorem Ipsum";
		const char* StringB = "Lorem Ipsum";

		ZE_TEST_CHECK_ERROR(StringA.Equals(StringB));
		ZE_TEST_CHECK_ERROR(StringA.Equals("Lorem Ipsum"));

		ZE_TEST_CASE("Two ZEStrings are not equal")
		{
			const ZEString StringA = "Lorem Ipsum";
			const char* StringB = "Dolor Sit";
			ZE_TEST_CHECK_ERROR(!(StringA.Equals(StringB)));
		}

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			ZEString StringC = "Işık Ilık";
			const char* StringD = "Süt İç";
			const char* StringE = "Işık Ilık";

			ZE_TEST_CHECK_ERROR(StringC.Equals(StringE));
			ZE_TEST_CHECK_ERROR(!(StringC.Equals(StringD)));
		}
	}

	ZE_TEST_ITEM("ZEString ZEString::FromBool(bool Value, const char* Format)")
	{
		bool Value = true;

		ZEString String = ZEString::FromBool(Value);

		ZE_TEST_CHECK_ERROR(String == "true");

		Value = 0;

		String = ZEString::FromBool(Value);
		ZE_TEST_CHECK_ERROR(String == "false");

		ZE_TEST_CASE("for Format != NULL")
		{
			Value = 1;

			String = ZEString::FromBool(Value, "false");
			ZE_TEST_CHECK_ERROR(String == "false");

			String = ZEString::FromBool(Value, "true");
			ZE_TEST_CHECK_ERROR(String == "true");
		}
	}

	ZE_TEST_ITEM("ZEString ZEString::FromChar(char Value)")
	{
		char C = 'a';

		ZEString StringA = ZEString::FromChar(C);

		ZE_TEST_CHECK_ERROR(StringA == "a");
	}

	ZE_TEST_ITEM("ZEString ZEString::FromCString(const char* Value)")
	{
		const char* String = "Lorem Ipsum";

		ZEString StringA = ZEString::FromCString(String);

		ZE_TEST_CHECK_ERROR(StringA == "Lorem Ipsum");

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			const char* StringB = "Işık Ilık Süt İç";

			ZEString StringC = ZEString::FromCString(StringB);

			ZE_TEST_CHECK_ERROR(StringC == "Işık Ilık Süt İç");
		}
	}

	ZE_TEST_ITEM("ZEString ZEString::FromDouble(double Value, const char* Format)")
	{
		double Value = 845756.88781;

		ZEString String = ZEString::FromDouble(Value);

		ZE_TEST_CHECK_ERROR(String == "845756.875000");

		ZE_TEST_CASE("for Format != NULL")
		{
			String = ZEString::FromDouble(Value, ".");
			ZE_TEST_CHECK_ERROR(String == "845757");
		}
	}

	ZE_TEST_ITEM("ZEString ZEString::FromFloat(float Value, const char* Format)")
	{
		float Value = 845756.88781f;

		ZEString String = ZEString::FromFloat(Value);

		ZE_TEST_CHECK_ERROR(String == "845756.875000");

		ZE_TEST_CASE("for Format != NULL")
		{
			String = ZEString::FromFloat(Value, "12.1");
			ZE_TEST_CHECK_ERROR(String == "    845756.9");
		}
	}

	ZE_TEST_ITEM("ZEString ZEString::FromInt16(ZEInt16 Value, const char* Format)")
	{
		ZEInt16 Value = -150;

		ZEString String = ZEString::FromInt16(Value);

		ZE_TEST_CHECK_ERROR(String == "-150");

		ZE_TEST_CASE("for Format != NULL")
		{
			String = ZEString::FromInt16(Value, "d:.6");
			ZE_TEST_CHECK_ERROR(String == "-000150");
		}
	}

	ZE_TEST_ITEM("ZEString ZEString::FromInt32(ZEInt32 Value, const char* Format)")
	{
		ZEInt32 Value = 845756;

		ZEString String = ZEString::FromInt32(Value);

		ZE_TEST_CHECK_ERROR(String == "845756");

		ZE_TEST_CASE("for Format != NULL")
		{
			String = ZEString::FromInt32(Value, "x:08");
			ZE_TEST_CHECK_ERROR(String == "000ce7bc");
		}
	}

	ZE_TEST_ITEM("ZEString ZEString::FromInt64(ZEInt64 Value, const char* Format)")
	{
		ZEInt64 Value = -845756;

		ZEString String = ZEString::FromInt64(Value);

		ZE_TEST_CHECK_ERROR(String == "-845756");

		ZE_TEST_CASE("for Format != NULL")
		{
			String = ZEString::FromInt64(Value, "d:-11.8");
			ZE_TEST_CHECK_ERROR(String == "-00845756  ");
		}
	}

	ZE_TEST_ITEM("ZEString ZEString::FromInt8(ZEInt8 Value, const char* Format)")
	{
		ZEInt8 Value = -20;

		ZEString String = ZEString::FromInt8(Value);

		ZE_TEST_CHECK_ERROR(String == "-20");

		ZE_TEST_CASE("for Format != NULL")
		{
			String = ZEString::FromInt8(Value, "d:6.3");
			ZE_TEST_CHECK_ERROR(String == "  -020");
		}
	}

	ZE_TEST_ITEM("ZEString ZEString::FromStdString(const std::string& Value)")
	{
		std::string Example = "Lorem Ipsum";

		ZEString StringA = ZEString::FromStdString(Example);

		ZE_TEST_CHECK_ERROR(StringA == "Lorem Ipsum");

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			std::string Example2 = "Işık Ilık Süt İç";

			ZEString StringB = ZEString::FromStdString(Example2);

			ZE_TEST_CHECK_ERROR(StringB == "Işık Ilık Süt İç");
		}
	}

	ZE_TEST_ITEM("ZEString ZEString::FromUInt16(ZEUInt16 Value, const char* Format)")
	{
		ZEUInt16 Value = 150;

		ZEString String = ZEString::FromUInt16(Value);

		ZE_TEST_CHECK_ERROR(String == "150");

		ZE_TEST_CASE("for Format != NULL")
		{
			String = ZEString::FromUInt16(Value, "x:04");
			ZE_TEST_CHECK_ERROR(String == "0096");
		}
	}

	ZE_TEST_ITEM("ZEString ZEString::FromUInt32(ZEUInt32 Value, const char* Format)")
	{
		ZEUInt32 Value = 845756;

		ZEString String = ZEString::FromUInt32(Value);

		ZE_TEST_CHECK_ERROR(String == "845756");

		ZE_TEST_CASE("for Format != NULL")
		{
			String = ZEString::FromUInt32(Value, "X:-10.8");
			ZE_TEST_CHECK_ERROR(String == "000CE7BC  ");
		}
	}

	ZE_TEST_ITEM("ZEString ZEString::FromUInt64(ZEUInt64 Value, const char* Format)")
	{
		ZEUInt64 Value = 845756.88781;

		ZEString String = ZEString::FromUInt64(Value);

		ZE_TEST_CHECK_ERROR(String == "845756");

		ZE_TEST_CASE("for Format != NULL")
		{
			String = ZEString::FromUInt64(Value, "d:8.7");
			ZE_TEST_CHECK_ERROR(String == " 0845756");
		}
	}

	ZE_TEST_ITEM("ZEString ZEString::FromUInt8(ZEUInt8 Value, const char* Format)")
	{
		ZEUInt8 Value = 122;

		ZEString String = ZEString::FromUInt8(Value);

		ZE_TEST_CHECK_ERROR(String == "122");

		ZE_TEST_CASE("for Format != NULL")
		{
			String = ZEString::FromUInt8(Value, "c:-4");
			ZE_TEST_CHECK_ERROR(String == "z   ");
		}
	}

	ZE_TEST_ITEM("ZEString ZEString::FromWChar(wchar_t Value)")
	{
		wchar_t WideCharacter = L'a';

		ZEString StringA = ZEString::FromWChar(WideCharacter);

		ZE_TEST_CHECK_ERROR(StringA == L"a");
	}

	ZE_TEST_ITEM("ZEString ZEString::FromWCString(const wchar_t* Value)")
	{
		wchar_t Example[] = {8364, 0};

		ZEString StringA = ZEString::FromWCString(Example);

		ZE_TEST_CHECK_ERROR(StringA == "€");
	}

	ZE_TEST_ITEM("ZEString ZEString::FromWStdString(const std::wstring& Value)")
	{
		wchar_t CString[] = {8364, 0};

		ZEString StringA = ZEString::FromWStdString(std::wstring(CString));

		ZE_TEST_CHECK_ERROR(StringA == "€");
	}

	ZE_TEST_ITEM("char ZEString::GetCharacter(ZESize Position) const")
	{
		ZEString StringA = "Lorem Ipsum";

		char Result1 = StringA.GetCharacter(4);

		ZE_TEST_CHECK_ERROR(Result1 == 'm');

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			ZEString StringB = "Işık Ilık Süt İç";

			ZECharacter Result2 = StringB.GetCharacter(3);
			ZECharacter Result3 = StringB.GetCharacter(11);

			ZE_TEST_CHECK_ERROR(Result2 == "k");
			ZE_TEST_CHECK_ERROR(Result3 == "ü");
		}
	}

	ZE_TEST_ITEM("ZESize ZEString::GetLength() const")
	{
		ZEString String = "Lorem Ipsum";
		ZEUInt ExpectedLength = 11;

		ZE_TEST_CHECK_ERROR(String.GetLength() == ExpectedLength);

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			ZEString StringB = "Işık Ilık Süt İç";

			ZE_TEST_CHECK_ERROR(StringB.GetLength() == 16);
		}
	}

	ZE_TEST_ITEM("ZESize ZEString::GetSize() const")
	{
		ZEString String = "Lorem Ipsum Dolor Sit Amet";
		ZEUInt ExpectedSize = 27;

		ZE_TEST_CHECK_ERROR(String.GetSize() == ExpectedSize);

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			ZEString StringB = "Işık Ilık Süt İç";

			ZE_TEST_CHECK_ERROR(StringB.GetSize() == 23);
		}
	}

	ZE_TEST_ITEM("const char* ZEString::GetValue() const")
	{
		ZEString String = "Lorem";
		const char* Value = String.GetValue();

		ZE_TEST_CHECK_ERROR(strcmp(Value, "Lorem") == 0);

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			ZEString StringB = "Işık";
			const char* Value2 = StringB.GetValue();

			ZE_TEST_CHECK_ERROR(strcmp(Value2, "Işık") == 0);
		}
	}

	ZE_TEST_ITEM("void ZEString::Insert(const ZEString & String);")
	{
		ZEString StringA = "Ipsum";
		ZEString StringB = "Lorem ";

		StringA.Insert(StringB);
		ZE_TEST_CHECK_ERROR(StringA == "Lorem Ipsum");

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			ZEString StringC = "Süt İç";
			ZEString StringD = "Işık Ilık ";

			StringC.Insert(StringD);

			ZE_TEST_CHECK_ERROR(StringC == "Işık Ilık Süt İç");
		}
	}

	ZE_TEST_ITEM("void ZEString::Insert(const char * String);")
	{
		ZEString StringA = "Ipsum";
		const char* StringB = "Lorem ";

		StringA.Insert(StringB);
		ZE_TEST_CHECK_ERROR(StringA == "Lorem Ipsum");

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			ZEString StringC = "Süt İç";
			const char* StringD = "Işık Ilık ";

			StringC.Insert(StringD);

			ZE_TEST_CHECK_ERROR(StringC == "Işık Ilık Süt İç");
		}
	}

	ZE_TEST_ITEM("void ZEString::Insert(ZESize Position, const ZEString & String)")
	{
		ZE_TEST_CASE("Inserting a ZEString inside another ZEString")
		{
			ZEString StringA = "Lorem Sit Amet";
			ZEString StringB = " Ipsum Dolor";
			ZEUInt ExpectedLength = 26;

			StringA.Insert(5, StringB);

			ZE_TEST_CHECK_ERROR(StringA == "Lorem Ipsum Dolor Sit Amet");
			ZE_TEST_CHECK_ERROR(StringA.GetLength() == ExpectedLength);
		}

		ZE_TEST_CASE("Inserting an empty ZEString inside another ZEString")
		{
			ZEString StringC = "Lorem";
			ZEString StringD = "";
			ZEUInt ExpectedLength = 5;

			StringC.Insert(3, StringD);

			ZE_TEST_CHECK_ERROR(StringC == "Lorem");
			ZE_TEST_CHECK_ERROR(StringC.GetLength() == ExpectedLength);
		}

		ZE_TEST_CASE("Inserting a ZESTring towards the end of the current ZEString")
		{
			ZEString StringE = "Lorem ";
			ZEUInt ExpectedLength = 11;

			StringE.Insert(6, "Ipsum");

			ZE_TEST_CHECK_ERROR(StringE == "Lorem Ipsum");
			ZE_TEST_CHECK_ERROR(StringE.GetLength() == ExpectedLength);
		}

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			ZEString StringC = "Işık Süt İç";
			ZEString StringD = "Ilık ";

			StringC.Insert(5, StringD);

			ZE_TEST_CHECK_ERROR(StringC == "Işık Ilık Süt İç");
		}
	}

	ZE_TEST_ITEM("void ZEString::Insert(ZESize Position, const char * String)")
	{
		ZEString StringA = "Lorem ";
		const char* StringB = "Ipsum";
		ZEUInt ExpectedLength = 11;
		ZESize Position = 6;

		StringA.Insert(Position, StringB);

		ZE_TEST_CHECK_ERROR(StringA == "Lorem Ipsum");
		ZE_TEST_CHECK_ERROR(StringA.GetLength() == ExpectedLength);

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			ZEString StringC = "Işık Süt İç";
			const char* StringD = "Ilık ";

			StringC.Insert(5, StringD);

			ZE_TEST_CHECK_ERROR(StringC == "Işık Ilık Süt İç");
		}
	}

	ZE_TEST_ITEM("bool ZEString::IsEmpty() const")
	{
		ZE_TEST_CASE("ZEString is empty")
		{
			ZEString StringA;
			ZEString StringB = "Lorem";
			StringB.Clear();

			ZE_TEST_CHECK_ERROR(StringA.IsEmpty());
			ZE_TEST_CHECK_ERROR(StringB.IsEmpty());
		}

		ZE_TEST_CASE("ZEString is not empty")
		{
			ZEString StringC = "Lorem Ipsum";

			ZE_TEST_CHECK_ERROR(!(StringC.IsEmpty()));
		}

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			ZEString StringD = "Işık Ilık Süt İç";
			StringD.Clear();

			ZE_TEST_CHECK_ERROR(StringD.IsEmpty());
		}
	}

	ZE_TEST_ITEM("ZEString ZEString::Left(ZESize Count) const")
	{
		ZEString String;

		ZE_TEST_CASE("ZEString is empty")
		{
			ZE_TEST_CHECK_ERROR(String.Left(0) == "");
		}

		ZE_TEST_CASE("ZEString has a value")
		{
			String = "1234Test5678";

			ZE_TEST_CHECK_ERROR(String.Left(0) == "");
			ZE_TEST_CHECK_ERROR(String.Left(1) == "1");
			ZE_TEST_CHECK_ERROR(String.Left(4) == "1234");
			ZE_TEST_CHECK_ERROR(String.Left(12) == "1234Test5678");
		}

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			String = "Işık Ilık Süt İç";

			ZE_TEST_CHECK_ERROR(String.Left(0) == "");
			ZE_TEST_CHECK_ERROR(String.Left(1) == "I");
			ZE_TEST_CHECK_ERROR(String.Left(4) == "Işık");
			ZE_TEST_CHECK_ERROR(String.Left(13) == "Işık Ilık Süt");
		}
	}

	ZE_TEST_ITEM("ZEString ZEString::Lower() const")
	{
		ZEString StringA;
		ZEString StringB;
		ZEString StringC;

		StringA = "lorem";
		StringB = "IPSUM";
		StringC = "DoLoR";

		ZE_TEST_CHECK_ERROR(StringA.Lower() == "lorem");
		ZE_TEST_CHECK_ERROR(StringB.Lower() == "ipsum");
		ZE_TEST_CHECK_ERROR(StringC.Lower() == "dolor");

		StringA = "";

		ZE_TEST_CHECK_ERROR(StringA.Lower() == "");

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			//NO NON-ASCII Character Support at the moment.
			ZE_TEST_CHECK_ERROR(false);
		}
	}

	ZE_TEST_ITEM("void ZEString::LowerSelf()")
	{
		ZEString StringA = "lorem";
		ZEString StringB = "IPSUM";
		ZEString StringC = "DoLoR";

		StringA.LowerSelf();
		StringB.LowerSelf();
		StringC.LowerSelf();

		ZE_TEST_CHECK_ERROR(StringA == "lorem");
		ZE_TEST_CHECK_ERROR(StringB == "ipsum");
		ZE_TEST_CHECK_ERROR(StringC == "dolor");

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			//NO NON-ASCII Character Support at the moment.
			ZE_TEST_CHECK_ERROR(false);
		}
	}

	ZE_TEST_ITEM("ZEString ZEString::Middle(ZESize Position, ZESize Count) const")
	{
		ZEString String;

		ZE_TEST_CASE("ZEString is empty")
		{
			ZE_TEST_CHECK_ERROR(String.Middle(0, 0) == "");
		}

		ZE_TEST_CASE("ZEString has a value")
		{
			String = "1234Test5678";

			ZE_TEST_CHECK_ERROR(String.Middle(0, 0) == "");
			ZE_TEST_CHECK_ERROR(String.Middle(2, 8) == "34Test56");
			ZE_TEST_CHECK_ERROR(String.Middle(8, 4) == "5678");
			ZE_TEST_CHECK_ERROR(String.Middle(0, 12) == "1234Test5678");
		}

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			String = "Işık Ilık Süt İç";

			ZE_TEST_CHECK_ERROR(String.Middle(0, 0) == "");
			ZE_TEST_CHECK_ERROR(String.Middle(2, 8) == "ık Ilık ");
			ZE_TEST_CHECK_ERROR(String.Middle(10, 6) == "Süt İç");
			ZE_TEST_CHECK_ERROR(String.Middle(0, 13) == "Işık Ilık Süt");
		}
	}

	ZE_TEST_ITEM("ZEString::operator const char*() const")
	{
		ZEString StringA = "Lorem Ipsum";
		const char* StringB = StringA;

		ZE_TEST_CHECK_ERROR(strcmp(StringB, StringA.GetValue()) == 0);

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			ZEString StringC = "Işık Ilık Süt İç";
			const char* StringD = StringC;

			ZE_TEST_CHECK_ERROR(strcmp(StringD, StringC.GetValue()) == 0);
		}

	}

	ZE_TEST_ITEM("ZEString::operator std::string() const")
	{
		ZEString StringA = "Lorem Ipsum";

		std::string StringB = StringA;

		ZE_TEST_CHECK_ERROR(strcmp(StringB.c_str(), StringA.GetValue()) == 0);

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			ZEString StringC = "Işık Ilık Süt İç";
			std::string StringD = StringC;

			ZE_TEST_CHECK_ERROR(strcmp(StringD.c_str(), StringC.GetValue()) == 0);
		}
	}

	ZE_TEST_ITEM("bool ZEString::operator!=(const ZEString & String) const")
	{
		ZE_TEST_CASE("ZEStrings are not equal")
		{
			const ZEString StringA = "Lorem";
			const ZEString StringB = "Ipsum";

			ZE_TEST_CHECK_ERROR(StringA != StringB);
		}

		ZE_TEST_CASE("ZEStrings are equal")
		{
			const ZEString StringC = "Lorem";
			const ZEString StringD = "Lorem";

			ZE_TEST_CHECK_ERROR(!(StringC != StringD));
		}

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			ZEString StringE = "Işık Ilık";
			ZEString StringF = " Süt İç";
			ZEString StringG = "Işık Ilık";

			ZE_TEST_CHECK_ERROR(StringE != StringF);
			ZE_TEST_CHECK_ERROR(!(StringE != StringG));
		}
	}

	ZE_TEST_ITEM("bool ZEString::operator!=(const char * String) const")
	{
		ZE_TEST_CASE("Strings are not equal")
		{
			const ZEString StringA = "Lorem";
			const char* StringB = "Ipsum";

			ZE_TEST_CHECK_ERROR(StringA != StringB);
		}

		ZE_TEST_CASE("Strings are equal")
		{
			const ZEString StringC = "Lorem";
			const char* StringD = "Lorem";

			ZE_TEST_CHECK_ERROR(!(StringC != StringD));
		}

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			ZEString StringE = "Işık Ilık";
			const char* StringF = " Süt İç";
			const char* StringG = "Işık Ilık";

			ZE_TEST_CHECK_ERROR(StringE != StringF);
			ZE_TEST_CHECK_ERROR(!(StringE != StringG));
		}
	}

	ZE_TEST_ITEM("bool ZEString::operator!=(const wchar_t* String) const")
	{
		ZE_TEST_CASE("Strings are not equal")
		{
			const ZEString StringA = "A";
			const wchar_t* StringB = L"B";

			ZE_TEST_CHECK_ERROR(StringA != StringB);
		}

		ZE_TEST_CASE("Strings are equal")
		{
			const ZEString StringC = L"A";
			const wchar_t* StringD = L"A";

			ZE_TEST_CHECK_ERROR(!(StringC != StringD));
		}

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			ZEString StringE = "Işık Ilık Süt İç";
			wchar_t StringF[] = {8364, 0};
			const wchar_t* StringG = StringE.ToWCString();

			ZE_TEST_CHECK_ERROR(StringE != StringF);
			ZE_TEST_CHECK_ERROR(!(StringE != StringG));
		}
	}

	ZE_TEST_ITEM("bool ZEString::operator!=(const std::wstring& String) const")
	{
		ZE_TEST_CASE("Strings are not equal")
		{
			const ZEString StringA = "A";
			std::wstring StringB = L"B";

			ZE_TEST_CHECK_ERROR(StringA != StringB);
		}

		ZE_TEST_CASE("Strings are equal")
		{
			const ZEString StringC = L"A";
			std::wstring StringD = L"A";

			ZE_TEST_CHECK_ERROR(!(StringC != StringD));
		}

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			ZEString StringE = "Işık Ilık Süt İç";
			wchar_t Temp[] = {8364, 0};
			std::wstring StringF = std::wstring(Temp);
			std::wstring StringG = StringE.ToWCString();

			ZE_TEST_CHECK_ERROR(StringE != StringF);
			ZE_TEST_CHECK_ERROR(!(StringE != StringG));
		}
	}

	ZE_TEST_ITEM("char & ZEString::operator[](ZEInt Index)")
	{
		ZEString StringA = "Lorem";

		ZE_TEST_CHECK_ERROR(StringA[0] == 'L');

		ZE_TEST_CASE("Changing a character of ZEString by index operator");
		{
			StringA[4] = 'M';

			ZE_TEST_CHECK_ERROR(StringA == "LoreM");
		}

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			StringA = "Işık Ilık Süt İç";

			ZE_TEST_CHECK_ERROR(StringA[1] == "ş");

			StringA[1] = "€";

			ZE_TEST_CHECK_ERROR(StringA[1] == "€");
		}
	}

	ZE_TEST_ITEM("const char & ZEString::operator[](ZEInt Index) const")
	{
		const ZEString StringA = "Lorem";

		const char C = StringA[2];

		ZE_TEST_CHECK_ERROR(StringA[2] == C);
		ZE_TEST_CHECK_ERROR(C == 'r');
	}

	ZE_TEST_ITEM("ZEString ZEString::operator+(const ZEString & String)")
	{
		ZEString StringA = "Lorem ";
		ZEString StringB = "Ipsum";

		ZEString Result = StringA + StringB;

		ZE_TEST_CHECK_EQUAL(Result, "Lorem Ipsum");

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			StringA = "Işık Ilık ";
			StringB = "Süt İç";

			ZEString Result = StringA + StringB;

			ZE_TEST_CHECK_ERROR(Result == "Işık Ilık Süt İç");
		}
	}

	ZE_TEST_ITEM("ZEString ZEString::operator+(const char * String)")
	{
		ZEString StringA = "Lorem";
		const char* StringB = " Ipsum";

		ZEString Result = StringA + StringB;

		ZE_TEST_CHECK_ERROR(Result == "Lorem Ipsum");

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			StringA = "Işık Ilık ";
			StringB = "Süt İç";

			ZEString Result = StringA + StringB;

			ZE_TEST_CHECK_ERROR(Result == "Işık Ilık Süt İç");
		}
	}

	ZE_TEST_ITEM("ZEString ZEString::operator+(const std::string& String)")
	{
		ZEString StringA = "Lorem";
		std::string StringB = " Ipsum";

		ZEString Result = StringA + StringB;

		ZE_TEST_CHECK_ERROR(Result == "Lorem Ipsum");

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			StringA = "Işık Ilık ";
			StringB = "Süt İç";

			ZEString Result = StringA + StringB;

			ZE_TEST_CHECK_ERROR(Result == "Işık Ilık Süt İç");
		}
	}

	ZE_TEST_ITEM("ZEString ZEString::operator+(const wchar_t* String)")
	{
		ZEString StringA = "Lorem";
		const wchar_t* StringB = L"a";

		ZEString Result = StringA + StringB;

		ZE_TEST_CHECK_ERROR(Result == "Lorema");

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			StringA = "₭";
			wchar_t StringC[] = {8364, 0};

			ZEString Result = StringA + StringC;

			ZE_TEST_CHECK_ERROR(Result == "₭€");
		}
	}

	ZE_TEST_ITEM("ZEString ZEString::operator+(const std::wstring& String)")
	{
		ZEString StringA = "Lorem";
		std::wstring StringB = L"a";

		ZEString Result = StringA + StringB;

		ZE_TEST_CHECK_ERROR(Result == "Lorema");

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			StringA = "₭";
			wchar_t Temp[] = {8364, 0};
			std::wstring StringC = std::wstring(Temp);

			ZEString Result = StringA + StringC;

			ZE_TEST_CHECK_ERROR(Result == "₭€");
		}
	}

	ZE_TEST_ITEM("ZEString ZEString::operator+(ZEInt8 Value)")
	{
		ZEString String = "€ ";
		ZEInt8 Value = -120;

		ZEString Result = String + Value;

		ZE_TEST_CHECK_ERROR(Result == "€ -120");
	}

	ZE_TEST_ITEM("ZEString ZEString::operator+(ZEInt16 Value)")
	{
		ZEString String = "€ ";
		ZEInt16 Value = -6320;

		ZEString Result = String + Value;

		ZE_TEST_CHECK_ERROR(Result == "€ -6320");
	}

	ZE_TEST_ITEM("ZEString ZEString::operator+(ZEInt32 Value)")
	{
		ZEString String = "€ ";
		ZEInt32 Value = -987654;

		ZEString Result = String + Value;

		ZE_TEST_CHECK_ERROR(Result == "€ -987654");
	}

	ZE_TEST_ITEM("ZEString ZEString::operator+(ZEInt64 Value)")
	{
		ZEString String = "€ ";
		ZEInt64 Value = -987654321;

		ZEString Result = String + Value;

		ZE_TEST_CHECK_ERROR(Result == "€ -987654321");
	}

	ZE_TEST_ITEM("ZEString ZEString::operator+(ZEUInt8 Value)")
	{
		ZEString String = "€ ";
		ZEUInt8 Value = 120;

		ZEString Result = String + Value;

		ZE_TEST_CHECK_ERROR(Result == "€ 120");
	}

	ZE_TEST_ITEM("ZEString ZEString::operator+(ZEUInt16 Value)")
	{
		ZEString String = "€ ";
		ZEUInt16 Value = 6320;

		ZEString Result = String + Value;

		ZE_TEST_CHECK_ERROR(Result == "€ 6320");
	}

	ZE_TEST_ITEM("ZEString ZEString::operator+(ZEUInt32 Value)")
	{
		ZEString String = "€ ";
		ZEInt32 Value = 987654;

		ZEString Result = String + Value;

		ZE_TEST_CHECK_ERROR(Result == "€ 987654");
	}

	ZE_TEST_ITEM("ZEString ZEString::operator+(ZEUInt64 Value)")
	{
		ZEString String = "€ ";
		ZEInt64 Value = -987654321;

		ZEString Result = String + Value;

		ZE_TEST_CHECK_ERROR(Result == "€ -987654321");
	}

	ZE_TEST_ITEM("ZEString ZEString::operator+(float Value)")
	{
		ZEString String = "€ ";
		float Value = 98765.8564f;

		ZEString Result = String + Value;

		ZE_TEST_CHECK_ERROR(Result == "€ 98765.859375");
	}

	ZE_TEST_ITEM("ZEString ZEString::operator+(double Value)")
	{
		ZEString String = "€ ";
		double Value = 98765.8564;

		ZEString Result = String + Value;

		ZE_TEST_CHECK_ERROR(Result == "€ 98765.859375");
	}

	ZE_TEST_ITEM("ZEString & ZEString::operator+=(const ZEString & String)")
	{
		ZEString StringA = "Lorem ";
		ZEString StringB = "Ipsum";

		StringA += StringB;
		ZE_TEST_CHECK_EQUAL(StringA, "Lorem Ipsum");

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			StringA = "Işık Ilık ";
			StringB = "Süt İç";

			StringA += StringB;

			ZE_TEST_CHECK_ERROR(StringA == "Işık Ilık Süt İç");
		}
	}

	ZE_TEST_ITEM("ZEString & ZEString::operator+=(const char * String)")
	{
		ZEString StringA = "Lorem ";
		const char* StringB = "Ipsum";

		StringA += StringB;
		ZE_TEST_CHECK_ERROR(StringA == "Lorem Ipsum");

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			StringA = "Işık Ilık ";
			StringB = "Süt İç";

			StringA += StringB;

			ZE_TEST_CHECK_ERROR(StringA == "Işık Ilık Süt İç");
		}
	}

	ZE_TEST_ITEM("ZEString& ZEString::operator+=(const std::string& String)")
	{
		ZEString StringA = "Lorem ";
		std::string StringB = "Ipsum";

		StringA += StringB;
		ZE_TEST_CHECK_ERROR(StringA == "Lorem Ipsum");

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			StringA = "Işık Ilık ";
			StringB = "Süt İç";

			StringA += StringB;

			ZE_TEST_CHECK_ERROR(StringA == "Işık Ilık Süt İç");
		}
	}

	ZE_TEST_ITEM("ZEString& ZEString::operator+=(const wchar_t* String)")
	{
		ZEString StringA = "Lorem";
		const wchar_t* StringB = L"a";

		StringA += StringB;

		ZE_TEST_CHECK_ERROR(StringA == "Lorema");

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			StringA = "₭";
			wchar_t StringC[] = {8364, 0};

			StringA += StringC;

			ZE_TEST_CHECK_ERROR(StringA == "₭€");
		}
	}

	ZE_TEST_ITEM("ZEString& ZEString::operator+=(const std::wstring& String)")
	{
		ZEString StringA = "Lorem";
		std::wstring StringB = L"a";

		StringA += StringB;

		ZE_TEST_CHECK_ERROR(StringA == "Lorema");

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			StringA = "₭";
			wchar_t Temp[] = {8364, 0};
			std::wstring StringC = std::wstring(Temp);

			StringA += StringC;

			ZE_TEST_CHECK_ERROR(StringA == "₭€");
		}
	}

	ZE_TEST_ITEM("ZEString& ZEString::operator+=(ZEInt8 Value)")
	{
		ZEString String = "€ ";
		ZEInt8 Value = -120;

		String += Value;

		ZE_TEST_CHECK_ERROR(String == "€ -120");
	}

	ZE_TEST_ITEM("ZEString& ZEString::operator+=(ZEInt16 Value)")
	{
		ZEString String = "€ ";
		ZEInt16 Value = -6320;

		String += Value;

		ZE_TEST_CHECK_ERROR(String == "€ -6320");
	}

	ZE_TEST_ITEM("ZEString& ZEString::operator+=(ZEInt32 Value)")
	{
		ZEString String = "€ ";
		ZEInt32 Value = -987654;

		String += Value;

		ZE_TEST_CHECK_ERROR(String == "€ -987654");
	}

	ZE_TEST_ITEM("ZEString& ZEString::operator+=(ZEInt64 Value)")
	{
		ZEString String = "€ ";
		ZEInt64 Value = -987654321;

		String += Value;

		ZE_TEST_CHECK_ERROR(String == "€ -987654321");
	}

	ZE_TEST_ITEM("ZEString& ZEString::operator+=(ZEUInt8 Value)")
	{
		ZEString String = "€ ";
		ZEUInt8 Value = 120;

		String += Value;

		ZE_TEST_CHECK_ERROR(String == "€ 120");
	}

	ZE_TEST_ITEM("ZEString& ZEString::operator+=(ZEUInt16 Value)")
	{
		ZEString String = "€ ";
		ZEUInt16 Value = 6320;

		String += Value;

		ZE_TEST_CHECK_ERROR(String == "€ 6320");
	}

	ZE_TEST_ITEM("ZEString& ZEString::operator+=(ZEUInt32 Value)")
	{
		ZEString String = "€ ";
		ZEInt32 Value = 987654;

		String += Value;

		ZE_TEST_CHECK_ERROR(String == "€ 987654");
	}

	ZE_TEST_ITEM("ZEString& ZEString::operator+=(ZEUInt64 Value)")
	{
		ZEString String = "€ ";
		ZEInt64 Value = 987654321;

		String += Value;

		ZE_TEST_CHECK_ERROR(String == "€ 987654321");
	}

	ZE_TEST_ITEM("ZEString& ZEString::operator+=(float Value)")
	{
		ZEString String = "€ ";
		float Value = 98765.8564f;

		String += Value;

		ZE_TEST_CHECK_ERROR(String == "€ 98765.859375");
	}

	ZE_TEST_ITEM("ZEString& ZEString::operator+=(double Value)")
	{
		ZEString String = "€ ";
		float Value = 98765.8564f;

		String += Value;

		ZE_TEST_CHECK_ERROR(String == "€ 98765.859375");
	}

	ZE_TEST_ITEM("ZEString & ZEString::operator=(const ZEString & String)")
	{
		ZEString StringA = "Lorem";
		ZEString StringB = "Ipsum";

		StringA = StringB;

		ZE_TEST_CHECK_ERROR(StringA == "Ipsum");

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			StringA = "Işık Ilık";
			StringB = "Süt İç";

			StringA = StringB;

			ZE_TEST_CHECK_ERROR(StringA == "Süt İç");
		}
	}

	ZE_TEST_ITEM("ZEString & ZEString::operator=(const char * String)")
	{
		ZEString StringA = "Lorem";
		const char* StringB = "Ipsum";

		StringA = StringB;

		ZE_TEST_CHECK_ERROR(StringA == "Ipsum");

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			StringA = "Işık Ilık";
			StringB = "Süt İç";

			StringA = StringB;

			ZE_TEST_CHECK_ERROR(StringA == "Süt İç");
		}
	}

	ZE_TEST_ITEM("ZEString& ZEString::operator=(const std::string& String)")
	{
		ZEString StringA = "Lorem";
		std::string StringB = "Ipsum";

		StringA = StringB;

		ZE_TEST_CHECK_ERROR(StringA == "Ipsum");

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			StringA = "Işık Ilık";
			StringB = "Süt İç";

			StringA = StringB;

			ZE_TEST_CHECK_ERROR(StringA == "Süt İç");
		}
	}

	ZE_TEST_ITEM("ZEString& ZEString::operator=(const wchar_t* String)")
	{
		ZEString StringA = "Lorem";
		const wchar_t* StringB = L"a";

		StringA = StringB;

		ZE_TEST_CHECK_ERROR(StringA == "a");

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			StringA = "₭";
			wchar_t StringC[] = {8364, 0};

			StringA = StringC;

			ZE_TEST_CHECK_ERROR(StringA == "€");
		}
	}

	ZE_TEST_ITEM("ZEString& ZEString::operator=(const std::wstring& String)")
	{
		ZEString StringA = "Lorem";
		std::wstring StringB = L"a";

		StringA = StringB;

		ZE_TEST_CHECK_ERROR(StringA == "a");

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			StringA = "₭";
			wchar_t Temp[] = {8364, 0};
			std::wstring StringC = std::wstring(Temp);

			StringA = StringC;

			ZE_TEST_CHECK_ERROR(StringA == "€");
		}
	}

	ZE_TEST_ITEM("ZEString& ZEString::operator=(ZEInt8 Value)")
	{
		ZEInt8 Value = -20;

		ZEString String = Value;

		ZE_TEST_CHECK_ERROR(String == "-20");
	}

	ZE_TEST_ITEM("ZEString& ZEString::operator=(ZEInt16 Value)")
	{
		ZEInt32 Value = -8765;

		ZEString String = Value;

		ZE_TEST_CHECK_ERROR(String == "-8765");
	}

	ZE_TEST_ITEM("ZEString& ZEString::operator=(ZEInt32 Value)")
	{
		ZEInt32 Value = -87654321;

		ZEString String = Value;

		ZE_TEST_CHECK_ERROR(String == "-87654321");
	}

	ZE_TEST_ITEM("ZEString& ZEString::operator=(ZEInt64 Value)")
	{
		ZEInt64 Value = -87654321;

		ZEString String = Value;

		ZE_TEST_CHECK_ERROR(String == "-87654321");
	}

	ZE_TEST_ITEM("ZEString& ZEString::operator=(ZEUInt8 Value)")
	{
		ZEUInt8 Value = 20;

		ZEString String = Value;

		ZE_TEST_CHECK_ERROR(String == "20");
	}

	ZE_TEST_ITEM("ZEString& ZEString::operator=(ZEUInt16 Value)")
	{
		ZEUInt16 Value = 8765;

		ZEString String = Value;

		ZE_TEST_CHECK_ERROR(String == "8765");
	}

	ZE_TEST_ITEM("ZEString& ZEString::operator=(ZEUInt32 Value)")
	{
		ZEUInt32 Value = 87654321;

		ZEString String = Value;

		ZE_TEST_CHECK_ERROR(String == "87654321");
	}

	ZE_TEST_ITEM("ZEString& ZEString::operator=(ZEUInt64 Value)")
	{
		ZEUInt64 Value = 87654321;

		ZEString String = Value;

		ZE_TEST_CHECK_ERROR(String == "87654321");
	}

	ZE_TEST_ITEM("ZEString& ZEString::operator=(float Value)")
	{
		float Value = 400.5525252f;

		ZEString String = Value;

		ZE_TEST_CHECK_ERROR(String == "400.552521");
	}

	ZE_TEST_ITEM("ZEString& ZEString::operator=(double Value)")
	{
		double Value = 400.5525252;

		ZEString String = Value;

		ZE_TEST_CHECK_ERROR(String == "400.552521");
	}

	ZE_TEST_ITEM("bool ZEString::operator==(const ZEString & String) const")
	{
		ZE_TEST_CASE("ZEStrings are not equal")
		{
			ZEString StringA = "Lorem";
			ZEString StringB = "Ipsum";

			ZE_TEST_CHECK_ERROR(!(StringA == StringB));
		}
		ZE_TEST_CASE("ZEStrings are equal")
		{
			ZEString StringC = "Lorem Ipsum";
			ZEString StringD = "Lorem Ipsum";

			ZE_TEST_CHECK_ERROR(StringC == StringD);
		}

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			ZEString StringE = "Işık Ilık";
			ZEString StringF = "Süt İç";
			ZEString StringG = "Işık Ilık";

			ZE_TEST_CHECK_ERROR(StringE == StringG);
			ZE_TEST_CHECK_ERROR(!(StringE == StringF));
		}

		ZE_TEST_CASE("Strings are not equal (Wide Character)")
		{
			ZEString StringA = "Lorem";
			const wchar_t* StringB = L"Ipsum";

			ZE_TEST_CHECK_ERROR(!(StringA == StringB));
		}

		ZE_TEST_CASE("Strings are equal (Wide Character)")
		{
			ZEString StringA = "Lorem Ipsum";
			const wchar_t* StringB = L"Lorem Ipsum";

			ZE_TEST_CHECK_ERROR(StringA == StringB);
		}

		ZE_TEST_CASE("UTF-8 encoding compatibility test (Wide Character)")
		{
			ZEString StringE = "€";
			wchar_t StringF[] = {8365, 0};
			wchar_t StringG[] = {8364, 0};

			ZE_TEST_CHECK_ERROR(StringE == StringG);
			ZE_TEST_CHECK_ERROR(!(StringE == StringF));
		}

		ZE_TEST_CASE("Strings are not equal (Wide Standard String)")
		{
			ZEString StringA = "Lorem";
			std::wstring StringB = L"Ipsum";

			ZE_TEST_CHECK_ERROR(!(StringA == StringB));
		}

		ZE_TEST_CASE("Strings are equal (Wide Standard String)")
		{
			ZEString StringA = "Lorem Ipsum";
			std::wstring StringB = L"Lorem Ipsum";

			ZE_TEST_CHECK_ERROR(StringA == StringB);
		}

		ZE_TEST_CASE("UTF-8 encoding compatibility test (Wide Standard String)")
		{
			ZEString StringE = "€";
			wchar_t Temp1[] = {8365, 0};
			std::wstring StringF = std::wstring(Temp1);
			wchar_t Temp2[] = {8364, 0};
			std::wstring StringG = std::wstring(Temp2);

			ZE_TEST_CHECK_ERROR(StringE == StringG);
			ZE_TEST_CHECK_ERROR(!(StringE == StringF));
		}
	}

	ZE_TEST_ITEM("bool ZEString::operator==(const char * String) const")
	{
		ZE_TEST_CASE("Strings are not equal")
		{
			ZEString StringA = "Lorem";
			const char* StringB = "Ipsum";

			ZE_TEST_CHECK_ERROR(!(StringA == StringB));
		}

		ZE_TEST_CASE("Strings are equal")
		{
			ZEString StringA = "Lorem Ipsum";
			const char* StringB = "Lorem Ipsum";

			ZE_TEST_CHECK_ERROR(StringA == StringB);
		}

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			ZEString StringE = "Işık Ilık";
			const char* StringF = "Süt İç";
			const char* StringG = "Işık Ilık";

			ZE_TEST_CHECK_ERROR(StringE == StringG);
			ZE_TEST_CHECK_ERROR(!(StringE == StringF));
		}
    }

	ZE_TEST_ITEM("bool ZEString::operator==(const std::string& String) const")
	{
		ZE_TEST_CASE("Strings are not equal")
		{
			ZEString StringA = "Lorem";
			std::string StringB = "Ipsum";

			ZE_TEST_CHECK_ERROR(!(StringA == StringB));
		}

		ZE_TEST_CASE("Strings are equal")
		{
			ZEString StringA = "Lorem Ipsum";
			std::string StringB = "Lorem Ipsum";

			ZE_TEST_CHECK_ERROR(StringA == StringB);
		}

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			ZEString StringE = "Işık Ilık";
			std::string StringF = "Süt İç";
			std::string StringG = "Işık Ilık";

			ZE_TEST_CHECK_ERROR(StringE == StringG);
			ZE_TEST_CHECK_ERROR(!(StringE == StringF));
		}
	}

	ZE_TEST_ITEM("void ZEString::Remove(ZESize Position, ZEUInt Count = 1)")
	{
		ZEString String = "1234567890";
		ZESize Position = 3;
		ZEUInt Count = 5;

		String.Remove(Position, Count);
		ZE_TEST_CHECK_ERROR(String == "12390");
		ZE_TEST_CHECK_ERROR(String.GetLength() == 5);

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			String = "Işık Ilık Süt İç";
			Position = 9;
			Count = 7;

			String.Remove(Position, Count);
			ZE_TEST_CHECK_ERROR(String == "Işık Ilık");
			ZE_TEST_CHECK_ERROR(String.GetLength() == 9);
		}
	}

	ZE_TEST_ITEM("ZEString ZEString::Right(ZESize Count) const")
	{
		ZEString String;

		ZE_TEST_CASE("ZEString is empty")
		{
			ZE_TEST_CHECK_ERROR(String.Right(0) == "");
		}
		
		ZE_TEST_CASE("ZEString has a value")
		{
			String = "1234Test5678";

			ZE_TEST_CHECK_ERROR(String.Right(0) == "");
			ZE_TEST_CHECK_ERROR(String.Right(1) == "8");
			ZE_TEST_CHECK_ERROR(String.Right(4) == "5678");
			ZE_TEST_CHECK_ERROR(String.Right(12) == "1234Test5678");
		}

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			String = "Işık Ilık Süt İç";

			ZE_TEST_CHECK_ERROR(String.Right(0) == "");
			ZE_TEST_CHECK_ERROR(String.Right(1) == "ç");
			ZE_TEST_CHECK_ERROR(String.Right(6) == "Süt İç");
			ZE_TEST_CHECK_ERROR(String.Right(11) == "Ilık Süt İç");
		}
	}

	ZE_TEST_ITEM("void ZEString::SetBuffer(void* Buffer, ZESize Size)")
	{
		void* Buffer = new unsigned char[1];

		ZEString String("String");
		ZE_TEST_CHECK_ERROR(String.GetSize() == 7);

		String.SetBuffer(Buffer, 1);
		ZE_TEST_CHECK_ERROR(String == "");
		ZE_TEST_CHECK_ERROR(String.GetSize() == 1);
	}

	ZE_TEST_ITEM("void ZEString::SetCharacter(ZESize Position, char Value)")
	{
		ZEString String = "0123456789";
		char Value = 'X';
		ZESize Position = 5;

		String.SetCharacter(Position, Value);
		ZE_TEST_CHECK_ERROR(String == "01234X6789");

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			String = "Işık Ilık Süt İç";
			const char* Value2 = "€";

			String.SetCharacter(15, Value2);
			ZE_TEST_CHECK_ERROR(String[15] == "€");
			ZE_TEST_CHECK_ERROR(String == "Işık Ilık Süt İ€");
		}
	}

	ZE_TEST_ITEM("void ZEString::SetValue(const char* String)")
	{
		ZEString StringA;
		const char* StringB = "Lorem";

		StringA.SetValue(StringB);

		ZE_TEST_CHECK_ERROR(StringA == "Lorem");
	}

	ZE_TEST_ITEM("void ZEString::SetValue(const wchar_t* String)")
	{
		ZEString StringA;
		wchar_t StringB[] = {8364, 0};

		StringA.SetValue(StringB);

		ZE_TEST_CHECK_ERROR(StringA == "€");
	}

	ZE_TEST_ITEM("void ZEString::SetValue(const ZEString& String)")
	{
		ZEString StringA("Test");
		ZE_TEST_CHECK_ERROR(StringA.GetSize() == 5);
		ZEString StringB("String");
		ZE_TEST_CHECK_ERROR(StringB.GetSize() == 7);

		StringA.SetValue(StringB);
		ZE_TEST_CHECK_ERROR(StringA == "String");
		ZE_TEST_CHECK_ERROR(StringA.GetSize() == 7);
	}

	ZE_TEST_ITEM("void ZEString::SetValue(const ZECharacter& Character)")
	{
		ZEString String;
		const char* Temp = "€";
		ZECharacter Character = "€";

		String.SetValue(Character);

		ZE_TEST_CHECK_ERROR(String == "€");
	}

	ZE_TEST_ITEM("void ZEString::SetValue(const std::string& String)")
	{
		ZEString StringA;
		std::string StringB = "Lorem";

		StringA.SetValue(StringB);

		ZE_TEST_CHECK_ERROR(StringA == "Lorem");
	}

	ZE_TEST_ITEM("void ZEString::SetValue(const std::wstring& String)")
	{
		ZEString StringA;
		wchar_t Temp[] = {8364, 0};
		std::wstring StringB = std::wstring(Temp);

		StringA.SetValue(StringB);

		ZE_TEST_CHECK_ERROR(StringA == "€");
	}

	ZE_TEST_ITEM("void ZEString::SetValue(wchar_t Character)")
	{
		ZEString StringA;
		wchar_t Character = 8364;

		StringA.SetValue(Character);

		ZE_TEST_CHECK_ERROR(StringA == "€");
	}

	ZE_TEST_ITEM("void ZEString::SetValue(float Value, const char* Format)")
	{
		float Value = 400.5525252f;
		ZEString String;

		String.SetValue(Value);
		ZE_TEST_CHECK_ERROR(String == "400.552521");

		ZE_TEST_CASE("for Format != NULL")
		{
			String.SetValue(Value, "-8.1");
			ZE_TEST_CHECK_ERROR(String == "400.6   ");
		}
	}

	ZE_TEST_ITEM("void ZEString::SetValue(double Value, const char* Format)")
	{
		double Value = 400.5525252;
		ZEString String;

		String.SetValue(Value);
		ZE_TEST_CHECK_ERROR(String == "400.552521");

		ZE_TEST_CASE("for Format != NULL")
		{
			String.SetValue(Value, "10.3");
			ZE_TEST_CHECK_ERROR(String == "   400.553");
		}
	}

	ZE_TEST_ITEM("void ZEString::SetValue(ZEInt8 Value, const char* Format)")
	{
		ZEInt8 Value = -20;

		ZEString String;
		String.SetValue(Value);

		ZE_TEST_CHECK_ERROR(String == "-20");

		ZE_TEST_CASE("for Format != NULL")
		{
			String.SetValue(Value, "x");
			ZE_TEST_CHECK_ERROR(String == "ffffffec");
		}
	}

	ZE_TEST_ITEM("void ZEString::SetValue(ZEInt16 Value, const char* Format)")
	{
		ZEInt16 Value = -8765;

		ZEString String;
		String.SetValue(Value);

		ZE_TEST_CHECK_ERROR(String == "-8765");

		ZE_TEST_CASE("for Format != NULL")
		{
			String.SetValue(Value, "d:8");
			ZE_TEST_CHECK_ERROR(String == "   -8765");
		}
	}

	ZE_TEST_ITEM("void ZEString::SetValue(ZEInt32 Value, const char* Format)")
	{
		ZEInt32 Value = -87654321;

		ZEString String;
		String.SetValue(Value);

		ZE_TEST_CHECK_ERROR(String == "-87654321");

		ZE_TEST_CASE("for Format != NULL")
		{
			String.SetValue(Value, ":");
			ZE_TEST_CHECK_ERROR(String == "-87654321");
		}
	}

	ZE_TEST_ITEM("void ZEString::SetValue(ZEInt64 Value, const char* Format)")
	{
		ZEInt64 Value = -87654321;

		ZEString String;
		String.SetValue(Value);

		ZE_TEST_CHECK_ERROR(String == "-87654321");

		ZE_TEST_CASE("for Format != NULL")
		{
			String.SetValue(Value, "d:.12");
			ZE_TEST_CHECK_ERROR(String == "-000087654321");
		}
	}

	ZE_TEST_ITEM("void ZEString::SetValue(ZEUInt8 Value, const char* Format)")
	{
		ZEUInt8 Value = 90;

		ZEString String;
		String.SetValue(Value);

		ZE_TEST_CHECK_ERROR(String == "90");

		ZE_TEST_CASE("for Format != NULL")
		{
			String.SetValue(Value, "c:4");
			ZE_TEST_CHECK_ERROR(String == "   Z");
		}
	}

	ZE_TEST_ITEM("void ZEString::SetValue(ZEUInt16 Value, const char* Format)")
	{
		ZEUInt16 Value = 8765;

		ZEString String;
		String.SetValue(Value);

		ZE_TEST_CHECK_ERROR(String == "8765");

		ZE_TEST_CASE("for Format != NULL")
		{
			String.SetValue(Value, "X:-6");
			ZE_TEST_CHECK_ERROR(String == "223D  ");
		}
	}

	ZE_TEST_ITEM("void ZEString::SetValue(ZEUInt32 Value, const char* Format)")
	{
		ZEUInt32 Value = 987654;

		ZEString String;
		String.SetValue(Value);

		ZE_TEST_CHECK_ERROR(String == "987654");

		ZE_TEST_CASE("for Format != NULL")
		{
			String.SetValue(Value, "d:.8");
			ZE_TEST_CHECK_ERROR(String == "00987654");
		}
	}

	ZE_TEST_ITEM("void ZEString::SetValue(ZEUInt64 Value, const char* Format)")
	{
		ZEUInt64 Value = 987654321;

		ZEString String;
		String.SetValue(Value);

		ZE_TEST_CHECK_ERROR(String == "987654321");

		ZE_TEST_CASE("for Format != NULL")
		{
			String.SetValue(Value, "X:-10");
			ZE_TEST_CHECK_ERROR(String == "3ADE68B1  ");
		}
	}

	ZE_TEST_ITEM("void ZEString::SetValue(bool Value, const char* Format)")
	{
		ZE_TEST_CASE("ZEString value is set to True")
		{
			ZEString StringA;

			bool Value = 1;

			StringA.SetValue(Value);

			ZE_TEST_CHECK_ERROR(StringA == "true");
		}
		ZE_TEST_CASE("ZEString value is set to False")
		{
			ZEString StringB;

			bool Value = 0;

			StringB.SetValue(Value);

			ZE_TEST_CHECK_ERROR(StringB == "false");
		}

		ZE_TEST_CASE("for Format != NULL")
		{
			ZEString StringC;

			bool Value = 1;

			StringC.SetValue(Value, "false");
			ZE_TEST_CHECK_ERROR(StringC == "false");
		}
	}

	ZE_TEST_ITEM("void ZEString::SetValue(void* Value, ZESize Size)")
	{
		void* Value = "Value";
		ZEString String("String");
		ZE_TEST_CHECK_ERROR(String.GetSize() == 7);

		String.SetValue(Value, 0);
		ZE_TEST_CHECK_ERROR(String == "");
		ZE_TEST_CHECK_ERROR(String.GetSize() == 1);

		String.SetValue(Value, 1);
		ZE_TEST_CHECK_ERROR(String == "V");
		ZE_TEST_CHECK_ERROR(String.GetSize() == 2);

		String.SetValue(Value, 5);
		ZE_TEST_CHECK_ERROR(String == "Value");
		ZE_TEST_CHECK_ERROR(String.GetSize() == 6);
	}

	ZE_TEST_ITEM("ZEString ZEString::SubString(ZESize StartPosition, ZESize EndPosition) const")
	{
		ZEString String;

		ZE_TEST_CASE("ZEString is empty")
		{
			ZE_TEST_CHECK_ERROR(String.SubString(0, 0) == "");
		}

		ZE_TEST_CASE("ZEString has a value")
		{
			String = "1234Test5678";
			ZE_TEST_CHECK_ERROR(String.SubString(0, 0) == "1");
			ZE_TEST_CHECK_ERROR(String.SubString(0, 1) == "12");
			ZE_TEST_CHECK_ERROR(String.SubString(0, 3) == "1234");

			ZE_TEST_CHECK_ERROR(String.SubString(4, 4) == "T");
			ZE_TEST_CHECK_ERROR(String.SubString(4, 5) == "Te");
			ZE_TEST_CHECK_ERROR(String.SubString(4, 7) == "Test");

			ZE_TEST_CHECK_ERROR(String.SubString(8, 8) == "5");
			ZE_TEST_CHECK_ERROR(String.SubString(8, 9) == "56");
			ZE_TEST_CHECK_ERROR(String.SubString(8, 11) == "5678");
		}

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			String = "Işık Ilık Süt İç";

			ZE_TEST_CHECK_ERROR(String.SubString(0, 0) == "I");
			ZE_TEST_CHECK_ERROR(String.SubString(0, 1) == "Iş");
			ZE_TEST_CHECK_ERROR(String.SubString(0, 3) == "Işık");

			ZE_TEST_CHECK_ERROR(String.SubString(5, 5) == "I");
			ZE_TEST_CHECK_ERROR(String.SubString(5, 8) == "Ilık");
			ZE_TEST_CHECK_ERROR(String.SubString(5, 12) == "Ilık Süt");

			ZE_TEST_CHECK_ERROR(String.SubString(9, 12) == " Süt");
			ZE_TEST_CHECK_ERROR(String.SubString(10, 13) == "Süt ");
			ZE_TEST_CHECK_ERROR(String.SubString(10, 15) == "Süt İç");
		}
	}

	ZE_TEST_ITEM("const char* ZEString::ToCString() const")
	{
		ZEString StringA = "Lorem Ipsum";

		const char* StringB = StringA.ToCString();

		ZE_TEST_CHECK_ERROR(StringA == StringB);

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			StringA = "Işık Ilık Süt İç";

			StringB = StringA.ToCString();

			ZE_TEST_CHECK_ERROR(StringA == StringB);
		}
	}

	ZE_TEST_ITEM("std::string ZEString::ToStdString() const")
	{
		ZEString StringA = "Lorem Ipsum";

		std::string StringB = StringA.ToStdString();

		ZE_TEST_CHECK_ERROR(StringA == StringB);

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			StringA = "Işık Ilık Süt İç";

			StringB = StringA.ToStdString();

			ZE_TEST_CHECK_ERROR(StringA == StringB);
		}
	}

	ZE_TEST_ITEM("const wchar_t* ZEString::ToWCString()")
	{
		ZEString StringA = "Lorem Ipsum";

		const wchar_t* StringB = StringA.ToWCString();

		ZE_TEST_CHECK_ERROR(StringA == StringB);

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			StringA = "Işık Ilık Süt İç";

			StringB = StringA.ToWCString();

			ZE_TEST_CHECK_ERROR(StringA == StringB);
		}
	}

	ZE_TEST_ITEM("std::string ZEString::ToStdString() const")
	{
		ZEString StringA = "Lorem Ipsum";

		std::wstring StringB = StringA.ToWStdString();

		ZE_TEST_CHECK_ERROR(StringA == StringB);

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			StringA = "Işık Ilık Süt İç";

			StringB = StringA.ToWStdString();

			ZE_TEST_CHECK_ERROR(StringA == StringB);
		}
	}

	ZE_TEST_ITEM("float ZEString::ToFloat()")
	{
		ZEString String = "-255.046";

		float Value = String.ToFloat();

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Value, -255.04601f);
	}

	ZE_TEST_ITEM("double ZEString::ToDouble() const")
	{
		ZEString String = "-255.046";

		double Value = String.ToDouble();

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Value, -255.04601f);
	}

	ZE_TEST_ITEM("ZEInt8 ZEString::ToInt8() const")
	{
		ZEString StringA = "120";
		ZEString StringB = "-120.043";

		ZEInt8 ValueA = StringA.ToInt8();
		ZEInt8 ValueB = StringB.ToInt8();

		ZE_TEST_CHECK_EQUAL(ValueA, 120);
		ZE_TEST_CHECK_EQUAL(ValueB, -120);
	}

	ZE_TEST_ITEM("ZEInt16 ZEString::ToInt16() const")
	{
		ZEString StringA = "255";
		ZEString StringB = "-255.043";

		ZEInt16 ValueA = StringA.ToInt16();
		ZEInt16 ValueB = StringB.ToInt16();

		ZE_TEST_CHECK_EQUAL(ValueA, 255);
		ZE_TEST_CHECK_EQUAL(ValueB, -255);
	}

	ZE_TEST_ITEM("ZEInt32 ZEString::ToInt32() const")
	{
		ZEString StringA = "255";
		ZEString StringB = "-255.043";

		ZEInt32 ValueA = StringA.ToInt32();
		ZEInt32 ValueB = StringB.ToInt32();

		ZE_TEST_CHECK_EQUAL(ValueA, 255);
		ZE_TEST_CHECK_EQUAL(ValueB, -255);
	}

	ZE_TEST_ITEM("ZEInt64 ZEString::ToInt64() const")
	{
		ZEString StringA = "255";
		ZEString StringB = "-255.043";

		ZEInt64 ValueA = StringA.ToInt64();
		ZEInt64 ValueB = StringB.ToInt64();

		ZE_TEST_CHECK_EQUAL(ValueA, 255);
		ZE_TEST_CHECK_EQUAL(ValueB, -255);
	}

	ZE_TEST_ITEM("ZEUInt8 ZEString::ToUInt8() const")
	{
		ZEString String = "123";
		ZEUInt8 ExpectedValue = 123;

		ZEUInt8 Value = String.ToUInt8();

		ZE_TEST_CHECK_EQUAL(Value, ExpectedValue);
	}

	ZE_TEST_ITEM("ZEUInt16 ZEString::ToUInt16() const")
	{
		ZEString String = "123";
		ZEUInt16 ExpectedValue = 123;

		ZEUInt16 Value = String.ToUInt16();

		ZE_TEST_CHECK_EQUAL(Value, ExpectedValue);
	}

	ZE_TEST_ITEM("ZEUInt32 ZEString::ToUInt32() const")
	{
		ZEString String = "123456";
		ZEUInt32 ExpectedValue = 123456;

		ZEUInt32 Value = String.ToUInt32();

		ZE_TEST_CHECK_EQUAL(Value, ExpectedValue);
	}

	ZE_TEST_ITEM("ZEUInt64 ZEString::ToUInt64() const")
	{
		ZEString String = "123456";
		ZEUInt64 ExpectedValue = 123456;

		ZEUInt64 Value = String.ToUInt64();

		ZE_TEST_CHECK_EQUAL(Value, ExpectedValue);
	}

	ZE_TEST_ITEM("ZEString ZEString::Trim() const")
	{
		ZEString String;
		ZE_TEST_CHECK_ERROR(String.Trim() == "");

		String = "    ";
		ZE_TEST_CHECK_ERROR(String.Trim() == "");

		String = "Lorem Ipsum";
		ZE_TEST_CHECK_ERROR(String.Trim() == "Lorem Ipsum");

		String = "   Lorem Ipsum   ";
		ZE_TEST_CHECK_ERROR(String.Trim() == "Lorem Ipsum");

		String = "\t\tLorem Ipsum\t\t";
		ZE_TEST_CHECK_ERROR(String.Trim() == "Lorem Ipsum");

		String = " \t \tLorem Ipsum \t \t";
		ZE_TEST_CHECK_ERROR(String.Trim() == "Lorem Ipsum");

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			String = "Işık Ilık Süt İç";
			ZE_TEST_CHECK_ERROR(String.Trim() == "Işık Ilık Süt İç");

			String = "   Işık Ilık Süt İç   ";
			ZE_TEST_CHECK_ERROR(String.Trim() == "Işık Ilık Süt İç");

			String = "\t\tIşık Ilık Süt İç\t\t";
			ZE_TEST_CHECK_ERROR(String.Trim() == "Işık Ilık Süt İç");

			String = " \t \tIşık Ilık Süt İç \t \t";
			ZE_TEST_CHECK_ERROR(String.Trim() == "Işık Ilık Süt İç");
		}
	}

	ZE_TEST_ITEM("void ZEString::TrimSelf()")
	{
		ZEString String;

		String.TrimSelf();
		ZE_TEST_CHECK_ERROR(String == "");

		String = "    ";
		String.TrimSelf();
		ZE_TEST_CHECK_ERROR(String == "");

		String = "Lorem Ipsum";
		String.TrimSelf();
		ZE_TEST_CHECK_ERROR(String == "Lorem Ipsum");

		String = "   Lorem Ipsum   ";
		String.TrimSelf();
		ZE_TEST_CHECK_ERROR(String == "Lorem Ipsum");

		String = "\t\tLorem Ipsum\t\t";
		String.TrimSelf();
		ZE_TEST_CHECK_ERROR(String == "Lorem Ipsum");

		String = " \t \tLorem Ipsum \t \t";
		String.TrimSelf();
		ZE_TEST_CHECK_ERROR(String == "Lorem Ipsum");

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			String = "Işık Ilık Süt İç";
			String.TrimSelf();
			ZE_TEST_CHECK_ERROR(String == "Işık Ilık Süt İç");

			String = "   Işık Ilık Süt İç   ";
			String.TrimSelf();
			ZE_TEST_CHECK_ERROR(String == "Işık Ilık Süt İç");

			String = "\t\tIşık Ilık Süt İç\t\t";
			String.TrimSelf();
			ZE_TEST_CHECK_ERROR(String == "Işık Ilık Süt İç");

			String = " \t \tIşık Ilık Süt İç \t \t";
			String.TrimSelf();
			ZE_TEST_CHECK_ERROR(String == "Işık Ilık Süt İç");
		}
	}

	ZE_TEST_ITEM("ZEString ZEString::TrimLeft() const")
	{
		ZEString String;
		ZE_TEST_CHECK_ERROR(String.TrimLeft() == "");

		String = "    ";
		ZE_TEST_CHECK_ERROR(String.TrimLeft() == "");

		String = "Lorem Ipsum";
		ZE_TEST_CHECK_ERROR(String.TrimLeft() == "Lorem Ipsum");

		String = "   Lorem Ipsum   ";
		ZE_TEST_CHECK_ERROR(String.TrimLeft() == "Lorem Ipsum   ");

		String = "\t\tLorem Ipsum\t\t";
		ZE_TEST_CHECK_ERROR(String.TrimLeft() == "Lorem Ipsum\t\t");

		String = " \t \tLorem Ipsum \t \t";
		ZE_TEST_CHECK_ERROR(String.TrimLeft() == "Lorem Ipsum \t \t");

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			String = "Işık Ilık Süt İç";
			ZE_TEST_CHECK_ERROR(String.TrimLeft() == "Işık Ilık Süt İç");

			String = "   Işık Ilık Süt İç   ";
			ZE_TEST_CHECK_ERROR(String.TrimLeft() == "Işık Ilık Süt İç   ");

			String = "\t\tIşık Ilık Süt İç\t\t";
			ZE_TEST_CHECK_ERROR(String.TrimLeft() == "Işık Ilık Süt İç\t\t");

			String = " \t \tIşık Ilık Süt İç \t \t";
			ZE_TEST_CHECK_ERROR(String.TrimLeft() == "Işık Ilık Süt İç \t \t");
		}
	}

	ZE_TEST_ITEM("void ZEString::TrimLeftSelf()")
	{
		ZEString String;

		String.TrimLeftSelf();
		ZE_TEST_CHECK_ERROR(String == "");

		String = "    ";
		String.TrimLeftSelf();
		ZE_TEST_CHECK_ERROR(String == "");

		String = "Lorem Ipsum";
		String.TrimLeftSelf();
		ZE_TEST_CHECK_ERROR(String == "Lorem Ipsum");

		String = "   Lorem Ipsum   ";
		String.TrimLeftSelf();
		ZE_TEST_CHECK_ERROR(String == "Lorem Ipsum   ");

		String = "\t\tLorem Ipsum\t\t";
		String.TrimLeftSelf();
		ZE_TEST_CHECK_ERROR(String == "Lorem Ipsum\t\t");

		String = " \t \tLorem Ipsum \t \t";
		String.TrimLeftSelf();
		ZE_TEST_CHECK_ERROR(String == "Lorem Ipsum \t \t");

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			String = "Işık Ilık Süt İç";
			String.TrimLeftSelf();
			ZE_TEST_CHECK_ERROR(String == "Işık Ilık Süt İç");

			String = "   Işık Ilık Süt İç   ";
			String.TrimLeftSelf();
			ZE_TEST_CHECK_ERROR(String == "Işık Ilık Süt İç   ");

			String = "\t\tIşık Ilık Süt İç\t\t";
			String.TrimLeftSelf();
			ZE_TEST_CHECK_ERROR(String == "Işık Ilık Süt İç\t\t");

			String = " \t \tIşık Ilık Süt İç \t \t";
			String.TrimLeftSelf();
			ZE_TEST_CHECK_ERROR(String == "Işık Ilık Süt İç \t \t");
		}
	}

	ZE_TEST_ITEM("ZEString ZEString::TrimRight() const")
	{
		ZEString String;
		ZE_TEST_CHECK_ERROR(String.TrimRight() == "");

		String = "    ";
		ZE_TEST_CHECK_ERROR(String.TrimRight() == "");

		String = "Lorem Ipsum";
		ZE_TEST_CHECK_ERROR(String.TrimRight() == "Lorem Ipsum");

		String = "   Lorem Ipsum   ";
		ZE_TEST_CHECK_ERROR(String.TrimRight() == "   Lorem Ipsum");

		String = "\t\tLorem Ipsum\t\t";
		ZE_TEST_CHECK_ERROR(String.TrimRight() == "\t\tLorem Ipsum");

		String = " \t \tLorem Ipsum \t \t";
		ZE_TEST_CHECK_ERROR(String.TrimRight() == " \t \tLorem Ipsum");

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			String = "Işık Ilık Süt İç";
			ZE_TEST_CHECK_ERROR(String.TrimRight() == "Işık Ilık Süt İç");

			String = "   Işık Ilık Süt İç   ";
			ZE_TEST_CHECK_ERROR(String.TrimRight() == "   Işık Ilık Süt İç");

			String = "\t\tIşık Ilık Süt İç\t\t";
			ZE_TEST_CHECK_ERROR(String.TrimRight() == "\t\tIşık Ilık Süt İç");

			String = " \t \tIşık Ilık Süt İç \t \t";
			ZE_TEST_CHECK_ERROR(String.TrimRight() == " \t \tIşık Ilık Süt İç");
		}
	}

	ZE_TEST_ITEM("void ZEString::TrimRightSelf()")
	{
		ZEString String;

		String.TrimRightSelf();
		ZE_TEST_CHECK_ERROR(String == "");

		String = "    ";
		String.TrimRightSelf();
		ZE_TEST_CHECK_ERROR(String == "");

		String = "Lorem Ipsum";
		String.TrimRightSelf();
		ZE_TEST_CHECK_ERROR(String == "Lorem Ipsum");

		String = "   Lorem Ipsum   ";
		String.TrimRightSelf();
		ZE_TEST_CHECK_ERROR(String == "   Lorem Ipsum");

		String = "\t\tLorem Ipsum\t\t";
		String.TrimRightSelf();
		ZE_TEST_CHECK_ERROR(String == "\t\tLorem Ipsum");

		String = " \t \tLorem Ipsum \t \t";
		String.TrimRightSelf();
		ZE_TEST_CHECK_ERROR(String == " \t \tLorem Ipsum");

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			String = "Işık Ilık Süt İç";
			String.TrimRightSelf();
			ZE_TEST_CHECK_ERROR(String == "Işık Ilık Süt İç");

			String = "   Işık Ilık Süt İç   ";
			String.TrimRightSelf();
			ZE_TEST_CHECK_ERROR(String == "   Işık Ilık Süt İç");

			String = "\t\tIşık Ilık Süt İç\t\t";
			String.TrimRightSelf();
			ZE_TEST_CHECK_ERROR(String == "\t\tIşık Ilık Süt İç");

			String = " \t \tIşık Ilık Süt İç \t \t";
			String.TrimRightSelf();
			ZE_TEST_CHECK_ERROR(String == " \t \tIşık Ilık Süt İç");
		}
	}

	ZE_TEST_ITEM(" ZEString ZEString::Upper() const")
	{
		ZEString StringA;
		ZEString StringB;
		ZEString StringC;
		ZEString StringD;

		StringA = "Lorem";
		StringB = "IPSUM";
		StringC = "DoLoR";
		StringD = "";

		ZE_TEST_CHECK_ERROR(StringA.Upper() == "LOREM");
		ZE_TEST_CHECK_ERROR(StringB.Upper() == "IPSUM");
		ZE_TEST_CHECK_ERROR(StringC.Upper() == "DOLOR");
		ZE_TEST_CHECK_ERROR(StringD.Upper() == "");

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			//NO NON-ASCII Character Support at the moment.
			ZE_TEST_CHECK_ERROR(false);
		}
	}

	ZE_TEST_ITEM("void ZEString::UpperSelf()")
	{
		ZEString StringA = "lorem";
		ZEString StringB = "IPSUM";
		ZEString StringC = "DoLoR";

		StringA.UpperSelf();
		StringB.UpperSelf();
		StringC.UpperSelf();

		ZE_TEST_CHECK_ERROR(StringA == "LOREM");
		ZE_TEST_CHECK_ERROR(StringB == "IPSUM");
		ZE_TEST_CHECK_ERROR(StringC == "DOLOR");

		ZE_TEST_CASE("UTF-8 encoding compatibility test")
		{
			//NO NON-ASCII Character Support at the moment.
			ZE_TEST_CHECK_ERROR(false);
		}
	}

	ZE_TEST_ITEM("ZEString::ZEString(const char * String)")
	{
		const char* StringA = "Lorem Ipsum";

		ZEString StringB(StringA);

		ZE_TEST_CHECK_ERROR(StringB == "Lorem Ipsum");
	}

	ZE_TEST_ITEM("ZEString::ZEString(const wchar_t* String)")
	{
		const wchar_t* StringA = L"Lorem Ipsum";

		ZEString StringB(StringA);

		ZE_TEST_CHECK_ERROR(StringB == L"Lorem Ipsum");
	}

	ZE_TEST_ITEM("ZEString::ZEString(const std::string& String)")
	{
		std::string StringA = "Lorem Ipsum";

		ZEString StringB(StringA);

		ZE_TEST_CHECK_ERROR(StringB == "Lorem Ipsum");
	}

	ZE_TEST_ITEM("ZEString::ZEString(const std::wstring& String)")
	{
		std::wstring StringA = L"Lorem Ipsum";

		ZEString StringB(StringA);

		ZE_TEST_CHECK_ERROR(StringB == L"Lorem Ipsum");
	}

	ZE_TEST_ITEM("ZEString::ZEString(const ZEString & String)")
	{
		ZEString StringA = "Lorem Ipsum";

		ZEString StringB(StringA);

		ZE_TEST_CHECK_ERROR(StringB == "Lorem Ipsum");
	}

	ZE_TEST_ITEM("ZEString::ZEString(ZEInt8 Value)")
	{
		ZEInt8 Value = -20;

		ZEString StringB(Value);

		ZE_TEST_CHECK_ERROR(StringB == "-20");
	}

	ZE_TEST_ITEM("ZEString::ZEString(ZEInt16 Value)")
	{
		ZEInt16 Value = -8765;

		ZEString StringB(Value);

		ZE_TEST_CHECK_ERROR(StringB == "-8765");
	}

	ZE_TEST_ITEM("ZEString::ZEString(ZEInt32 Value)")
	{
		ZEInt32 Value = -87654;

		ZEString StringB(Value);

		ZE_TEST_CHECK_ERROR(StringB == "-87654");
	}

	ZE_TEST_ITEM("ZEString::ZEString(ZEInt64 Value)")
	{
		ZEInt64 Value = -87654321;

		ZEString StringB(Value);

		ZE_TEST_CHECK_ERROR(StringB == "-87654321");
	}

	ZE_TEST_ITEM("ZEString::ZEString(ZEUInt8 Value)")
	{
		ZEUInt8 Value = 20;

		ZEString StringB(Value);

		ZE_TEST_CHECK_ERROR(StringB == "20");
	}

	ZE_TEST_ITEM("ZEString::ZEString(ZEUInt16 Value)")
	{
		ZEUInt16 Value = 8765;

		ZEString StringB(Value);

		ZE_TEST_CHECK_ERROR(StringB == "8765");
	}

	ZE_TEST_ITEM("ZEString::ZEString(ZEUInt32 Value)")
	{
		ZEUInt32 Value = 87654;

		ZEString StringB(Value);

		ZE_TEST_CHECK_ERROR(StringB == "87654");
	}

	ZE_TEST_ITEM("ZEString::ZEString(ZEUInt64 Value)")
	{
		ZEUInt64 Value = 87654321;

		ZEString StringB(Value);

		ZE_TEST_CHECK_ERROR(StringB == "87654321");
	}

	ZE_TEST_ITEM("ZEString::ZEString(float Value)")
	{
		float Value = 400.5525252f;

		ZEString String(Value);

		ZE_TEST_CHECK_ERROR(String == "400.552521");
	}

	ZE_TEST_ITEM("ZEString::ZEString(double Value)")
	{
		double Value = 400.5525252;

		ZEString String(Value);

		ZE_TEST_CHECK_ERROR(String == "400.552521");
	}

	ZE_TEST_ITEM("ZEString operator+(const char* String1, const ZEString& String2)")
	{
		ZEString StringA = "Ipsum";
		const char* StringB = "Lorem ";
		ZEString Result;

		Result = StringB + StringA;

		ZE_TEST_CHECK_ERROR(Result == "Lorem Ipsum");
	}

	ZE_TEST_ITEM("ZEString operator+(const wchar_t* String1, const ZEString& String2)")
	{
		ZEString StringA = "Ipsum";
		const wchar_t* StringB = L"Lorem ";
		ZEString Result;

		Result = StringB + StringA;

		ZE_TEST_CHECK_ERROR(Result == "Lorem Ipsum");
	}

	ZE_TEST_ITEM("ZEString operator+(ZEInt8 Value, const ZEString& String);")
	{
		ZEString StringA = " €";
		ZEInt8 StringB = -20;
		ZEString Result;

		Result = StringB + StringA;

		ZE_TEST_CHECK_ERROR(Result == "-20 €");
	}

	ZE_TEST_ITEM("ZEString operator+(ZEInt16 Value, const ZEString& String);")
	{
		ZEString StringA = " €";
		ZEInt16 StringB = -8765;
		ZEString Result;

		Result = StringB + StringA;

		ZE_TEST_CHECK_ERROR(Result == "-8765 €");
	}

	ZE_TEST_ITEM("ZEString operator+(ZEInt32 Value, const ZEString& String);")
	{
		ZEString StringA = " €";
		ZEInt32 StringB = -87654;
		ZEString Result;

		Result = StringB + StringA;

		ZE_TEST_CHECK_ERROR(Result == "-87654 €");
	}

	ZE_TEST_ITEM("ZEString operator+(ZEInt64 Value, const ZEString& String);")
	{
		ZEString StringA = " €";
		ZEInt64 StringB = -8765432;
		ZEString Result;

		Result = StringB + StringA;

		ZE_TEST_CHECK_ERROR(Result == "-8765432 €");
	}

	ZE_TEST_ITEM("ZEString operator+(ZEUInt8 Value, const ZEString& String);")
	{
		ZEString StringA = " €";
		ZEUInt8 StringB = 20;
		ZEString Result;

		Result = StringB + StringA;

		ZE_TEST_CHECK_ERROR(Result == "20 €");
	}

	ZE_TEST_ITEM("ZEString operator+(ZEUInt16 Value, const ZEString& String);")
	{
		ZEString StringA = " €";
		ZEUInt16 StringB = 8765;
		ZEString Result;

		Result = StringB + StringA;

		ZE_TEST_CHECK_ERROR(Result == "8765 €");
	}

	ZE_TEST_ITEM("ZEString operator+(ZEUInt32 Value, const ZEString& String);")
	{
		ZEString StringA = " €";
		ZEUInt32 StringB = 87654;
		ZEString Result;

		Result = StringB + StringA;

		ZE_TEST_CHECK_ERROR(Result == "87654 €");
	}

	ZE_TEST_ITEM("ZEString operator+(ZEUInt64 Value, const ZEString& String);")
	{
		ZEString StringA = " €";
		ZEUInt64 StringB = 8765432;
		ZEString Result;

		Result = StringB + StringA;

		ZE_TEST_CHECK_ERROR(Result == "8765432 €");
	}

	ZE_TEST_ITEM("ZEString operator+(float Value, const ZEString& String);")
	{
		ZEString StringA = " €";
		float StringB = 400.5525252f;
		ZEString Result;

		Result = StringB + StringA;

		ZE_TEST_CHECK_ERROR(Result == "400.552521 €");
	}

	ZE_TEST_ITEM("ZEString operator+(double Value, const ZEString& String);")
	{
		ZEString StringA = " €";
		double StringB = 400.5525252;
		ZEString Result;

		Result = StringB + StringA;

		ZE_TEST_CHECK_ERROR(Result == "400.552521 €");
	}

	ZE_TEST_ITEM("bool operator==(const char* String1, const ZEString& String2)")
	{
		ZE_TEST_CASE("Strings are not equal")
		{
			const ZEString StringA = "Lorem";
			const char* StringB = "Ipsum";

			ZE_TEST_CHECK_ERROR(!(StringB == StringA));
		}

		ZE_TEST_CASE("Strings are equal")
		{
			const ZEString StringA = "Lorem Ipsum";
			const char* StringB = "Lorem Ipsum";

			ZE_TEST_CHECK_ERROR(StringB == StringA);
		}
	}

	ZE_TEST_ITEM("bool operator==(const wchar_t* String1, const ZEString& String2)")
	{
		ZE_TEST_CASE("Strings are not equal")
		{
			const ZEString StringA = "Lorem";
			const wchar_t* StringB = L"Ipsum";

			ZE_TEST_CHECK_ERROR(!(StringB == StringA));
		}

		ZE_TEST_CASE("Strings are equal")
		{
			const ZEString StringA = "Lorem Ipsum";
			const wchar_t* StringB = L"Lorem Ipsum";

			ZE_TEST_CHECK_ERROR(StringB == StringA);
		}
	}

	ZE_TEST_ITEM("bool operator!=(const char* String1, const ZEString& String2)")
	{
		ZE_TEST_CASE("Strings are not equal")
		{
			const ZEString StringA = "Lorem";
			const char* StringB = "Ipsum";

			ZE_TEST_CHECK_ERROR(StringB != StringA);
		}

		ZE_TEST_CASE("Strings are equal")
		{
			const ZEString StringC = "Lorem";
			const char* StringD = "Lorem";

			ZE_TEST_CHECK_ERROR(!(StringD != StringC));
		}
	}

	ZE_TEST_ITEM("bool operator!=(const wchar_t* String1, const ZEString& String2)")
	{
		ZE_TEST_CASE("Strings are not equal")
		{
			const ZEString StringA = "Lorem";
			const wchar_t* StringB = L"Ipsum";

			ZE_TEST_CHECK_ERROR(StringB != StringA);
		}

		ZE_TEST_CASE("Strings are equal")
		{
			const ZEString StringC = "Lorem";
			const wchar_t* StringD = L"Lorem";

			ZE_TEST_CHECK_ERROR(!(StringD != StringC));
		}
	}

	ZE_TEST_ITEM("bool operator!=(const wchar_t* String1, const ZEString& String2)")
	{
		ZE_TEST_CASE("Strings are not equal")
		{
			const ZEString StringA = "Lorem";
			const wchar_t* StringB = L"Ipsum";

			ZE_TEST_CHECK_ERROR(StringB != StringA);
		}

		ZE_TEST_CASE("Strings are equal")
		{
			const ZEString StringC = "Lorem";
			const wchar_t* StringD = L"Lorem";

			ZE_TEST_CHECK_ERROR(!(StringD != StringC));
		}
	}
}
