//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPathTokenizer.cpp
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

#include "ZEPathTokenizer.h"

#include <string.h>

char* ZEPathTokenizer::Trim(char* Data)
{
	char* Cursor = Data;

	char* Start = NULL;
	while(*Cursor != '\0')
	{
		if (*Cursor != ' ' && *Cursor != '\t')
		{
			Start = Cursor;
			break;
		}
		Cursor++;
	}

	if (Start == NULL)
	{
		*Data = '\0';
		return Data;
	}

	char* LastNonSpace = NULL;
	while(*Cursor != '\0')
	{
		if (*Cursor != ' ' && *Cursor != '\t')
			LastNonSpace = Cursor;

		Cursor++;
	}

	LastNonSpace[1] = '\0';

	return Start;
}

const char* ZEPathTokenizer::GetToken(ZESize Index)
{
	return TokenList[Index];
}

void ZEPathTokenizer::SetToken(ZESize Index, const char* String)
{
	TokenList[Index] = String;
}

ZESize ZEPathTokenizer::GetTokenCount()
{
	return TokenCount;
}

const char* ZEPathTokenizer::GetOutput()
{
	return Buffer;
}

void ZEPathTokenizer::Squish()
{
	ZESize OldTokenCount = TokenCount;
	for (ZESize I = 0; I < OldTokenCount; I++)
	{
		if (TokenList[I] != NULL)
			continue;

		bool Found = false;
		for (ZESize N = I + 1; N < OldTokenCount; N++)
		{
			if (TokenList[N] == NULL)
					continue;

			TokenList[I] = TokenList[N];
			TokenList[N] = NULL;
			Found = true;
			break;
		}

		if (!Found)
		{
			TokenCount = I;
			return;
		}
	}
}

void ZEPathTokenizer::Tokenize(const char* Path)
{
	strcpy_s(Buffer, 4095, Path);

	TokenCount = 0;
	char* Context = NULL;
	TokenList[TokenCount] = strtok_s(Buffer, "\\/", &Context);
	while(TokenList[TokenCount] != NULL)
	{
		TokenList[TokenCount] = Trim((char*)TokenList[TokenCount]);
		TokenCount++;
		TokenList[TokenCount] = strtok_s(NULL, "\\/", &Context);
	}
}

void ZEPathTokenizer::Combine()
{
	char Temp[ZE_PATH_TOKENIZER_BUFFER_SIZE];
	Temp[0] = '\0';

	for (ZESize I = 0; I < TokenCount; I++)
	{
		if (TokenList[I] == NULL)
			continue;

		strcat_s(Temp, ZE_PATH_TOKENIZER_BUFFER_SIZE, TokenList[I]);

		if (I != TokenCount - 1)
		{
			#ifdef ZE_PLATFORM_WINDOWS
				strcat_s(Temp, ZE_PATH_TOKENIZER_BUFFER_SIZE, "\\");
			#else
				strcat_s(Temp, ZE_PATH_TOKENIZER_BUFFER_SIZE, "/");
			#endif
		}
	}

	strcpy(Buffer, Temp);
}
