//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECharacterTests.cpp
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
#include <String>
#include "ZETest/ZETest.h"
#include <mbstring.h>
#include "ZEArray.h"

ZE_TEST(ZECharacter)
{
	ZE_TEST_ITEM("void ZECharacter::SetValue(char Character)")
	{
		ZECharacter Character;

		char SingleByteChar = 'A';

		Character.SetValue(SingleByteChar);

		ZE_TEST_CHECK_ERROR(Character == 'A');
	}
	
	ZE_TEST_ITEM("void ZECharacter::SetValue(const char* MultiByteCharacter)")
	{
		ZECharacter Character;

		char MultiByteCharacters[] = "€";

		Character.SetValue(&MultiByteCharacters[0]);

		ZE_TEST_CHECK_ERROR(Character == MultiByteCharacters);
	}

	ZE_TEST_ITEM("void ZECharacter::SetValue(const wchar_t* WideCharacter)")
	{
		char MultiByteCharacters[] = "€";
		wchar_t WideCharacters[2] = {8364, 0};	
		
		ZECharacter Character;
		Character.SetValue(WideCharacters);

		ZE_TEST_CHECK_ERROR(Character == MultiByteCharacters);
	}

	ZE_TEST_ITEM("void ZECharacter::SetValue(ZEInt Character)")
	{
		char MultiByteCharacters[] = "€";
		int MultiByteInt = '€';

		ZECharacter Character;
		Character.SetValue(MultiByteInt);

		ZE_TEST_CHECK_ERROR(Character == MultiByteCharacters);
	}

	ZE_TEST_ITEM("void ZECharacter::SetOwner(ZEString* Owner, ZESize Index)")
	{
		ZECharacter Character('p');
		ZEString String("Lorem Ipsum");

		Character.SetOwner(&String, 7);
		Character.SetValue('k');

		ZE_TEST_CHECK_ERROR(String == "Lorem Iksum");
	}

	ZE_TEST_ITEM("const char* ZECharacter::GetValue() const")
	{
		char MultiByteCharacters[] = "€";

		ZECharacter Character;
		Character.SetValue(MultiByteCharacters);

		ZE_TEST_CHECK_ERROR(strncmp(Character.GetValue(), MultiByteCharacters, Character.GetSize()) == 0);
	}

	ZE_TEST_ITEM("ZESize ZECharacter::GetSize() const")
	{
		char MultiByteCharacters[] = "€";

		ZECharacter Character;
		Character.SetValue(MultiByteCharacters);

		ZESize SizeWithoutNull = strlen(MultiByteCharacters);

		ZE_TEST_CHECK_ERROR(Character.GetSize() == SizeWithoutNull);
	}

	ZE_TEST_ITEM("bool ZECharacter::Equals(const ZECharacter& Character) const")
	{
		char MultiByteCharactersForEuro[] = "€";

		ZECharacter CharacterEuro;
		CharacterEuro.SetValue(MultiByteCharactersForEuro);

		char MultiByteCharactersForEuro2[] = "€";

		ZECharacter CharacterEuro2;
		CharacterEuro2.SetValue(MultiByteCharactersForEuro2);

		ZE_TEST_CHECK_ERROR(CharacterEuro.Equals(CharacterEuro2));

		ZE_TEST_CASE("False Condition")
		{
			char MultiByteCharactersForKip[] = "₭";

			ZECharacter CharacterKip;
			CharacterKip.SetValue(MultiByteCharactersForKip);

			ZE_TEST_CHECK_ERROR(!CharacterEuro.Equals(CharacterKip));
		}
	}

	ZE_TEST_ITEM("bool ZECharacter::Equals(const char& Character) const")
	{
		ZECharacter Character;

		char SingleByteChar = 'A';

		Character.SetValue(SingleByteChar);

		ZE_TEST_CHECK_ERROR(Character.Equals('A'));

		ZE_TEST_CASE("False Condition")
		{
			ZE_TEST_CHECK_ERROR(!Character.Equals('B'));
		}
	}

	ZE_TEST_ITEM("bool ZECharacter::Equals(const char* MultiByteCharacter) const")
	{
		char MultiByteCharacters[] = "€";

		ZECharacter Character;
		Character.SetValue(MultiByteCharacters);

		ZE_TEST_CHECK_ERROR(Character.Equals("€"));
		ZE_TEST_CHECK_ERROR(Character.Equals("€a"));

		ZE_TEST_CASE("False Condition")
		{
			ZE_TEST_CHECK_ERROR(!Character.Equals("₭"));
		}
	}

	ZE_TEST_ITEM("bool ZECharacter::Equals(const wchar_t* WideCharacter) const")
	{
		char MultiByteCharacters[] = "€";

		ZECharacter Character;
		Character.SetValue(MultiByteCharacters);

		wchar_t WideCharacter[2] = {8364, 0};

		ZE_TEST_CHECK_ERROR(Character.Equals(WideCharacter));

		ZE_TEST_CASE("False Condition")
		{
			char AnotherMultiByteCharacter[] = "₭";

			wchar_t AnotherWideCharacter[2] = {8365, 0};

			ZE_TEST_CHECK_ERROR(!Character.Equals(AnotherWideCharacter));
		}
	}

	ZE_TEST_ITEM("ZECharacter ZECharacter::Upper() const")
	{
		ZECharacter Character;
		Character.SetValue("a");

		ZECharacter UpperCaseCharacter = Character.Upper();

		ZE_TEST_CHECK_ERROR(UpperCaseCharacter == "A");
	}

	ZE_TEST_ITEM("void ZECharacter::UpperSelf()")
	{
		ZECharacter Character;
		Character.SetValue("a");

		Character.UpperSelf();

		ZE_TEST_CHECK_ERROR(Character == "A");
	}

	ZE_TEST_ITEM("ZECharacter ZECharacter::Lower() const")
	{
		ZECharacter Character;
		Character.SetValue("A");

		ZECharacter LowerCaseCharacter = Character.Lower();

		ZE_TEST_CHECK_ERROR(LowerCaseCharacter == "a");
	}

	ZE_TEST_ITEM("void ZECharacter::LowerSelf()")
	{
		ZECharacter Character;
		Character.SetValue("A");

		Character.LowerSelf();

		ZE_TEST_CHECK_ERROR(Character == "a");
	}

	ZE_TEST_ITEM("ZECharacter ZECharacter::FromChar(const char& Value)")
	{
		char SingleByteCharacter = 'A';
		ZECharacter Character = ZECharacter::FromChar(SingleByteCharacter);

		ZE_TEST_CHECK_ERROR(Character == 'A');
	}

	ZE_TEST_ITEM("ZECharacter ZECharacter::FromMBChar(const char* Value)")
	{
		char MultiByteCharacter[] = "€";
		ZECharacter Character = ZECharacter::FromMBChar(MultiByteCharacter);

		ZE_TEST_CHECK_ERROR(Character == "€");
	}

	ZE_TEST_ITEM("ZECharacter ZECharacter::FromWChar(const wchar_t* Value)")
	{
		char MultiByteCharacter[] = "€";

		wchar_t WideCharacter[2] = {8364, 0};

		ZECharacter Character = ZECharacter::FromWChar(WideCharacter);

		ZE_TEST_CHECK_ERROR(Character == "€");
	}

	ZE_TEST_ITEM("ZECharacter& ZECharacter::operator=(const ZECharacter& Character)")
	{
		ZECharacter TempCharacter("€");

		ZE_TEST_CHECK_ERROR(TempCharacter == "€");

		ZECharacter ResultCharacter("₭");

		TempCharacter = ResultCharacter;

		ZE_TEST_CHECK_ERROR(TempCharacter == "₭");
	}

	ZE_TEST_ITEM("ZECharacter& ZECharacter::operator=(const char& Character)")
	{
		ZECharacter Character('A');

		ZE_TEST_CHECK_ERROR(Character == 'A');

		Character = 'B';

		ZE_TEST_CHECK_ERROR(Character == 'B');
	}

	ZE_TEST_ITEM("ZECharacter& ZECharacter::operator=(const char* MultiByteCharacter)")
	{
		ZECharacter Character("€");

		ZE_TEST_CHECK_ERROR(Character == "€");

		char MultiByteCharacter[] = "A";

		Character = MultiByteCharacter;

		ZE_TEST_CHECK_ERROR(Character == "A");
	}

	ZE_TEST_ITEM("ZECharacter& ZECharacter::operator=(const wchar_t* WideCharacter)")
	{
		ZECharacter Character("€");

		ZE_TEST_CHECK_ERROR(Character == "€");

		wchar_t WideCharacter[2] = {8365, 0};

		Character = WideCharacter;

		ZE_TEST_CHECK_ERROR(Character == "₭");
	}

	ZE_TEST_ITEM("bool ZECharacter::operator!=(const ZECharacter& Character) const")
	{
		ZECharacter CharacterEuro("€");

		ZECharacter CharacterKip("₭");

		ZE_TEST_CHECK_ERROR((CharacterEuro != CharacterKip));

		ZE_TEST_CASE("False Condition")
		{
			ZECharacter CharacterEuro2("€");

			ZE_TEST_CHECK_ERROR(!(CharacterEuro != CharacterEuro2));
		}
	}

	ZE_TEST_ITEM("bool ZECharacter::operator!=(const char& Character) const")
	{
		ZECharacter Character('A');

		ZE_TEST_CHECK_ERROR(Character != 'B');

		ZE_TEST_CASE("False Condition")
		{
			ZE_TEST_CHECK_ERROR(!(Character != 'A'));
		}
	}

	ZE_TEST_ITEM("bool ZECharacter::operator!=(const char* MultiByteCharacter) const")
	{
		ZECharacter Character("€");

		ZE_TEST_CHECK_ERROR(Character != "₭");

		ZE_TEST_CASE("False Condition")
		{
			ZE_TEST_CHECK_ERROR(!(Character != "€"));
			ZE_TEST_CHECK_ERROR(!(Character != "€a"));
		}
	}

	ZE_TEST_ITEM("bool ZECharacter::operator!=(const wchar_t* WideCharacter) const")
	{
		char MultiByteCharacter[] = "€";
		ZECharacter Character(MultiByteCharacter);

		wchar_t AnotherWideCharacter[2] = {8365, 0};

		ZE_TEST_CHECK_ERROR(Character != AnotherWideCharacter);

		ZE_TEST_CASE("False Condition")
		{
			wchar_t WideCharacter[2] = {8364, 0};

			ZE_TEST_CHECK_ERROR(!(Character != WideCharacter));
		}
	}

	ZE_TEST_ITEM("bool ZECharacter::operator==(const ZECharacter& Character) const")
	{
		ZECharacter CharacterEuro("€");
		
		ZECharacter CharacterEuro2("€");

		ZE_TEST_CHECK_ERROR(CharacterEuro == CharacterEuro2);

		ZE_TEST_CASE("False Condition")
		{
			ZECharacter CharacterKip("₭");

			ZE_TEST_CHECK_ERROR(!(CharacterEuro == CharacterKip));
		}
	}

	ZE_TEST_ITEM("bool ZECharacter::operator==(const char& Character) const")
	{
		ZECharacter Character('A');

		ZE_TEST_CHECK_ERROR(Character == 'A');

		ZE_TEST_CASE("False Condition")
		{
			ZE_TEST_CHECK_ERROR(!(Character == 'B'));
		}
	}

	ZE_TEST_ITEM("bool ZECharacter::operator==(const char* MultiByteCharacter) const")
	{
		ZECharacter Character("€");

		ZE_TEST_CHECK_ERROR(Character == "€");
		ZE_TEST_CHECK_ERROR(Character == "€a");

		ZE_TEST_CASE("False Condition")
		{
			ZE_TEST_CHECK_ERROR(!(Character == "₭"));
		}
	}

	ZE_TEST_ITEM("bool ZECharacter::operator==(const wchar_t* WideCharacter) const")
	{
		char MultiByteCharacter[] = "€";

		ZECharacter Character(MultiByteCharacter);

		wchar_t WideCharacter[2] = {8364, 0};

		ZE_TEST_CHECK_ERROR(Character == WideCharacter);

		ZE_TEST_CASE("False Condition")
		{
			wchar_t AnotherWideCharacter[2] = {8365, 0};

			ZE_TEST_CHECK_ERROR(!(Character == AnotherWideCharacter));
		}
	}

	ZE_TEST_ITEM("ZECharacter::operator char() const")
	{
		ZECharacter Character('A');

		char Result = Character;

		ZE_TEST_CHECK_ERROR(Result == 'A');
	}

	ZE_TEST_ITEM("ZECharacter::operator const char*() const")
	{
		ZECharacter Character("€");

		const char* Result = Character;

		ZE_TEST_CHECK_ERROR(strncmp(Result, "€", Character.GetSize()) == 0);
	}

	ZE_TEST_ITEM("ZECharacter::operator wchar_t() const")
	{
		ZECharacter Character("€");

		wchar_t Temp = Character;

		ZEString Result(Temp);

		ZE_TEST_CHECK_ERROR(strcmp(Result, "€") == 0);
	}
}
