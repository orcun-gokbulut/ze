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
#include "ZEFile/ZEFile.h"
#include "ZEArray.h"

ZETestSuite(ZECharacter)
{
	ZETest("void ZECharacter::SetValue(char Character)")
	{
		ZECharacter Character;

		char SingleByteChar = 'A';

		Character.SetValue(SingleByteChar);

		ZETestCheck(Character == 'A');
	}
	
	ZETest("void ZECharacter::SetValue(const char* MultiByteCharacter)")
	{
		ZECharacter Character;

		char MultiByteCharacters[] = "€";

		Character.SetValue(&MultiByteCharacters[0]);

		ZETestCheck(Character == MultiByteCharacters);
	}

	ZETest("void ZECharacter::SetValue(const wchar_t* WideCharacter)")
	{
		char MultiByteCharacters[] = "€";
		wchar_t WideCharacters[2] = {8364, 0};	
		
		ZECharacter Character;
		Character.SetValue(WideCharacters);

		ZETestCheck(Character == MultiByteCharacters);
	}

	ZETest("void ZECharacter::SetValue(ZEInt Character)")
	{
		char MultiByteCharacters[] = "€";
		int MultiByteInt = '€';

		ZECharacter Character;
		Character.SetValue(MultiByteInt);

		ZETestCheck(Character == MultiByteCharacters);
	}

	ZETest("void ZECharacter::SetOwner(ZEString* Owner, ZESize Index)")
	{
		//I dont know if this will exist or not.. Decide after string tests.
	}

	ZETest("const char* ZECharacter::GetValue() const")
	{
		char MultiByteCharacters[] = "€";

		ZECharacter Character;
		Character.SetValue(MultiByteCharacters);

		ZETestCheck(strncmp(Character.GetValue(), MultiByteCharacters, Character.GetSize()) == 0);
	}

	ZETest("ZESize ZECharacter::GetSize() const")
	{
		char MultiByteCharacters[] = "€";

		ZECharacter Character;
		Character.SetValue(MultiByteCharacters);

		ZESize SizeWithoutNull = strlen(MultiByteCharacters);

		ZETestCheck(Character.GetSize() == SizeWithoutNull);
	}

	ZETest("bool ZECharacter::Equals(const ZECharacter& Character) const")
	{
		char MultiByteCharactersForEuro[] = "€";

		ZECharacter CharacterEuro;
		CharacterEuro.SetValue(MultiByteCharactersForEuro);

		char MultiByteCharactersForEuro2[] = "€";

		ZECharacter CharacterEuro2;
		CharacterEuro2.SetValue(MultiByteCharactersForEuro2);

		ZETestCheck(CharacterEuro.Equals(CharacterEuro2));

		ZETestCase("False Condition")
		{
			char MultiByteCharactersForKip[] = "₭";

			ZECharacter CharacterKip;
			CharacterKip.SetValue(MultiByteCharactersForKip);

			ZETestCheck(!CharacterEuro.Equals(CharacterKip));
		}
	}

	ZETest("bool ZECharacter::Equals(const char& Character) const")
	{
		ZECharacter Character;

		char SingleByteChar = 'A';

		Character.SetValue(SingleByteChar);

		ZETestCheck(Character.Equals('A'));

		ZETestCase("False Condition")
		{
			ZETestCheck(!Character.Equals('B'));
		}
	}

	ZETest("bool ZECharacter::Equals(const char* MultiByteCharacter) const")
	{
		char MultiByteCharacters[] = "€";

		ZECharacter Character;
		Character.SetValue(MultiByteCharacters);

		ZETestCheck(Character.Equals("€"));
		ZETestCheck(Character.Equals("€a"));

		ZETestCase("False Condition")
		{
			ZETestCheck(!Character.Equals("₭"));
		}
	}

	ZETest("bool ZECharacter::Equals(const wchar_t* WideCharacter) const")
	{
		char MultiByteCharacters[] = "€";

		ZECharacter Character;
		Character.SetValue(MultiByteCharacters);

		wchar_t WideCharacter[2] = {8364, 0};

		ZETestCheck(Character.Equals(WideCharacter));

		ZETestCase("False Condition")
		{
			char AnotherMultiByteCharacter[] = "₭";

			wchar_t AnotherWideCharacter[2] = {8365, 0};

			ZETestCheck(!Character.Equals(AnotherWideCharacter));
		}
	}

	ZETest("ZECharacter ZECharacter::Upper() const")
	{
		ZECharacter Character;
		Character.SetValue("a");

		ZECharacter UpperCaseCharacter = Character.Upper();

		ZETestCheck(UpperCaseCharacter == "A");
	}

	ZETest("void ZECharacter::UpperSelf()")
	{
		ZECharacter Character;
		Character.SetValue("a");

		Character.UpperSelf();

		ZETestCheck(Character == "A");
	}

	ZETest("ZECharacter ZECharacter::Lower() const")
	{
		ZECharacter Character;
		Character.SetValue("A");

		ZECharacter LowerCaseCharacter = Character.Lower();

		ZETestCheck(LowerCaseCharacter == "a");
	}

	ZETest("void ZECharacter::LowerSelf()")
	{
		ZECharacter Character;
		Character.SetValue("A");

		Character.LowerSelf();

		ZETestCheck(Character == "a");
	}

	ZETest("ZECharacter ZECharacter::FromChar(const char& Value)")
	{
		char SingleByteCharacter = 'A';
		ZECharacter Character = ZECharacter::FromChar(SingleByteCharacter);

		ZETestCheck(Character == 'A');
	}

	ZETest("ZECharacter ZECharacter::FromMBChar(const char* Value)")
	{
		char MultiByteCharacter[] = "€";
		ZECharacter Character = ZECharacter::FromMBChar(MultiByteCharacter);

		ZETestCheck(Character == "€");
	}

	ZETest("ZECharacter ZECharacter::FromWChar(const wchar_t* Value)")
	{
		char MultiByteCharacter[] = "€";

		wchar_t WideCharacter[2] = {8364, 0};

		ZECharacter Character = ZECharacter::FromWChar(WideCharacter);

		ZETestCheck(Character == "€");
	}

	ZETest("ZECharacter& ZECharacter::operator=(const ZECharacter& Character)")
	{
		ZECharacter TempCharacter("€");

		ZETestCheck(TempCharacter == "€");

		ZECharacter ResultCharacter("₭");

		TempCharacter = ResultCharacter;

		ZETestCheck(TempCharacter == "₭");
	}

	ZETest("ZECharacter& ZECharacter::operator=(const char& Character)")
	{
		ZECharacter Character('A');

		ZETestCheck(Character == 'A');

		Character = 'B';

		ZETestCheck(Character == 'B');
	}

	ZETest("ZECharacter& ZECharacter::operator=(const char* MultiByteCharacter)")
	{
		ZECharacter Character("€");

		ZETestCheck(Character == "€");

		char MultiByteCharacter[] = "A";

		Character = MultiByteCharacter;

		ZETestCheck(Character == "A");
	}

	ZETest("ZECharacter& ZECharacter::operator=(const wchar_t* WideCharacter)")
	{
		ZECharacter Character("€");

		ZETestCheck(Character == "€");

		wchar_t WideCharacter[2] = {8365, 0};

		Character = WideCharacter;

		ZETestCheck(Character == "₭");
	}

	ZETest("bool ZECharacter::operator!=(const ZECharacter& Character) const")
	{
		ZECharacter CharacterEuro("€");

		ZECharacter CharacterKip("₭");

		ZETestCheck((CharacterEuro != CharacterKip));

		ZETestCase("False Condition")
		{
			ZECharacter CharacterEuro2("€");

			ZETestCheck(!(CharacterEuro != CharacterEuro2));
		}
	}

	ZETest("bool ZECharacter::operator!=(const char& Character) const")
	{
		ZECharacter Character('A');

		ZETestCheck(Character != 'B');

		ZETestCase("False Condition")
		{
			ZETestCheck(!(Character != 'A'));
		}
	}

	ZETest("bool ZECharacter::operator!=(const char* MultiByteCharacter) const")
	{
		ZECharacter Character("€");

		ZETestCheck(Character != "₭");

		ZETestCase("False Condition")
		{
			ZETestCheck(!(Character != "€"));
			ZETestCheck(!(Character != "€a"));
		}
	}

	ZETest("bool ZECharacter::operator!=(const wchar_t* WideCharacter) const")
	{
		char MultiByteCharacter[] = "€";
		ZECharacter Character(MultiByteCharacter);

		wchar_t AnotherWideCharacter[2] = {8365, 0};

		ZETestCheck(Character != AnotherWideCharacter);

		ZETestCase("False Condition")
		{
			wchar_t WideCharacter[2] = {8364, 0};

			ZETestCheck(!(Character != WideCharacter));
		}
	}

	ZETest("bool ZECharacter::operator==(const ZECharacter& Character) const")
	{
		ZECharacter CharacterEuro("€");
		
		ZECharacter CharacterEuro2("€");

		ZETestCheck(CharacterEuro == CharacterEuro2);

		ZETestCase("False Condition")
		{
			ZECharacter CharacterKip("₭");

			ZETestCheck(!(CharacterEuro == CharacterKip));
		}
	}

	ZETest("bool ZECharacter::operator==(const char& Character) const")
	{
		ZECharacter Character('A');

		ZETestCheck(Character == 'A');

		ZETestCase("False Condition")
		{
			ZETestCheck(!(Character == 'B'));
		}
	}

	ZETest("bool ZECharacter::operator==(const char* MultiByteCharacter) const")
	{
		ZECharacter Character("€");

		ZETestCheck(Character == "€");
		ZETestCheck(Character == "€a");

		ZETestCase("False Condition")
		{
			ZETestCheck(!(Character == "₭"));
		}
	}

	ZETest("bool ZECharacter::operator==(const wchar_t* WideCharacter) const")
	{
		char MultiByteCharacter[] = "€";

		ZECharacter Character(MultiByteCharacter);

		wchar_t WideCharacter[2] = {8364, 0};

		ZETestCheck(Character == WideCharacter);

		ZETestCase("False Condition")
		{
			wchar_t AnotherWideCharacter[2] = {8365, 0};

			ZETestCheck(!(Character == AnotherWideCharacter));
		}
	}

	ZETest("ZECharacter::operator char() const")
	{
		ZECharacter Character('A');

		char Result = Character;

		ZETestCheck(Result == 'A');
	}

	ZETest("ZECharacter::operator const char*() const")
	{
		ZECharacter Character("€");

		const char* Result = Character;

		ZETestCheck(strncmp(Result, "€", Character.GetSize()) == 0);
	}

	ZETest("ZECharacter::operator wchar_t() const")
	{
		ZECharacter Character("€");

		wchar_t Temp = Character;

		ZEString Result(Temp);

		ZETestCheck(strcmp(Result, "€") == 0);
	}
}
