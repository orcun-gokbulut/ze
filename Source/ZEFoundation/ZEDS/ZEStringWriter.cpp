//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEStringWriter.cpp
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

#include "ZEStringWriter.h"

void ZEStringWriter::Append(const char* String)
{
	if (String == NULL)
		return;

	while(*String != '\0')
	{
		if (Size + 1 > Allocator.GetSize())
			Allocator.Reallocate(&Buffer, Size + 1);
		Buffer[Size] = *String;
		Size++;
		String++;
	}
}

void ZEStringWriter::Append(char Character)
{
	if (Size + 1 > Allocator.GetSize())
		Allocator.Reallocate(&Buffer, Size + 1);
	Buffer[Size] = Character;
	Size++;
}

void ZEStringWriter::AppendUTF8(char* Character)
{
	ZESize CharacterSize = 0;
	if ((*Character & 0xF0) == 0xF0)
		CharacterSize = 4;
	else if ((*Character & 0xE0) == 0xE0)
		CharacterSize = 3;
	else if ((*Character & 0xC0) == 0xC0)
		CharacterSize = 2;
	else
		CharacterSize = 1;

	if (Size + CharacterSize > Allocator.GetSize())
		Allocator.Reallocate(&Buffer, Size + CharacterSize);

	for (ZESize I = 0; I < CharacterSize; I++)
	{
		Buffer[Size] = Character[I];
		Size++;
	}
}

void ZEStringWriter::AppendEnd()
{
	Append('\0');
}

void ZEStringWriter::Clear()
{
	Allocator.Deallocate(&Buffer);
	Size = 0;
}

void ZEStringWriter::Output(ZEString& String)
{
	String.Buffer = Buffer;
	String.Allocator.Size = Size;
	
	Buffer = NULL;
	Size = 0;

	Allocator.Reallocate(&Buffer, Size);
}

ZEStringWriter::ZEStringWriter()
{
	Buffer = NULL;
	Size = 0;
}

ZEStringWriter::~ZEStringWriter()
{
	Clear();
}
