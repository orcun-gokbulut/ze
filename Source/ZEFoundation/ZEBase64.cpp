//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEBase64.cpp
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

#include "ZEBase64.h"
#include <string>

ZEString ZEBase64::Base64Characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

ZEBase64::ZEBase64()
{
	
}

ZEBase64::~ZEBase64()
{

}

ZEString ZEBase64::Encode(unsigned char const* Input, ZESize InputLength)
{
	ZEString Result;
	const std::string BaseCharacters = Base64Characters.ToStdString();
	ZEUInt64 I = 0;
	ZEUInt64 J = 0;

	unsigned char CharacterArray3[3];
	unsigned char CharacterArray4[4];

	while(InputLength--)
	{
		CharacterArray3[I++] = *(Input++);
		if(I == 3)
		{
			CharacterArray4[0] = (CharacterArray3[0] & 0xfc) >> 2;
			CharacterArray4[1] = ((CharacterArray3[0] & 0x03) << 4) + ((CharacterArray3[1] & 0xf0) >> 4);
			CharacterArray4[2] = ((CharacterArray3[1] & 0x0f) << 2) + ((CharacterArray3[2] & 0xc0) >> 6);
			CharacterArray4[3] = CharacterArray3[2] & 0x3f;

			for(I = 0; I < 4; I++)
				Result += ZEString::FromChar(BaseCharacters[CharacterArray4[I]]);
			
			I = 0;
		}
	}

	if(I)
	{
		for(J = I; J < 3; J++)
			CharacterArray3[J] = '\0';

		CharacterArray4[0] = (CharacterArray3[0] & 0xfc) >> 2;
		CharacterArray4[1] = ((CharacterArray3[0] & 0x03) << 4) + ((CharacterArray3[1] & 0xf0) >> 4);
		CharacterArray4[2] = ((CharacterArray3[1] & 0x0f) << 2) + ((CharacterArray3[2] & 0xc0) >> 6);
		CharacterArray4[3] = CharacterArray3[2] & 0x3f;

		for (J = 0; J < I + 1; J++)
			Result += ZEString::FromChar(BaseCharacters[CharacterArray4[J]]);

		while((I++ < 3))
			Result += "=";
	}

	return Result;
}

ZEString ZEBase64::Decode(std::string const& Input)
{
	ZEString Result;
	const std::string BaseCharacters = Base64Characters.ToStdString();
	int InputLength = Input.size();
	ZESize I = 0;
	ZESize J = 0;
	int Index = 0;

	unsigned char* CharacterArray3 = new unsigned char[3];
	unsigned char* CharacterArray4 = new unsigned char[4];

	while(InputLength-- && (Input[Index] != '=') && isBase64(Input[Index]))
	{
		CharacterArray4[I++] = Input[Index];
		Index++;
		
		if(I == 4)
		{
			for(I = 0; I < 4; I++)
				CharacterArray4[I] = BaseCharacters.find(CharacterArray4[I]);

			CharacterArray3[0] = (CharacterArray4[0] << 2) + ((CharacterArray4[1] & 0x30) >> 4);
			CharacterArray3[1] = ((CharacterArray4[1] & 0xf) << 4) + ((CharacterArray4[2] & 0x3c) >> 2);
			CharacterArray3[2] = ((CharacterArray4[2] & 0x3) << 6) + CharacterArray4[3];

			for (I = 0; I < 3; I++)
				Result += ZEString::FromChar(CharacterArray3[I]);

			I = 0;
		}
	}

	if(I)
	{
		for(J = I; J < 4; J++)
			CharacterArray4[J] = 0;

		for(J = 0; J < 4; J++)
			CharacterArray4[J] = BaseCharacters.find(CharacterArray4[J]);

		CharacterArray3[0] = (CharacterArray4[0] << 2) + ((CharacterArray4[1] & 0x30) >> 4);
		CharacterArray3[1] = ((CharacterArray4[1] & 0xf) << 4) + ((CharacterArray4[2] & 0x3c) >> 2);
		CharacterArray3[2] = ((CharacterArray4[2] & 0x3) << 6) + CharacterArray4[3];
		
		for(J = 0; J < I - 1; J++)
			Result += ZEString::FromChar(CharacterArray3[J]);
	}

	return Result;
}

bool ZEBase64::isBase64(unsigned char Character)
{
	return (isalnum(Character) || (Character == '+') || (Character == '/'));
}

