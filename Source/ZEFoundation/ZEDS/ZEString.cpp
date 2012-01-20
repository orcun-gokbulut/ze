//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEString.cpp
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

#include "ZEString.h"

#include <string.h>
#include <memory.h>
#include <mbstring.h> 
#include <wchar.h>
#include <stdlib.h>
#include <string>
#include <stdarg.h>

#include "ZEError.h"

bool ZEString::IsEmpty() const
{
	return (Buffer == NULL || (Buffer[0] == '\0')); 
}

size_t ZEString::GetSize() const
{
	return Allocator.GetSize();
}

void ZEString::Compact()
{
	if (Buffer != NULL)
		Allocator.Reallocate(&Buffer, (strlen(Buffer) + 1) * sizeof(char));
}

void ZEString::Clear()
{
	Allocator.Deallocate(&Buffer);
	Buffer = NULL;
}

void ZEString::SetValue(const char* String)
{
	zeAssert(String == NULL, "Null char array pointer are not valid parameters.");
	
	if (String == NULL)
	{
		Clear();
		return;
	}

	size_t StringSize = strlen(String);
	if (StringSize == 0)
	{
		Clear();
		return;
	}

	size_t Size = ((StringSize) + 1) * sizeof(char);
	
	char* OldBuffer = Buffer;
	if (Allocator.Allocate(&Buffer, Size))
		delete[] OldBuffer;

	memcpy(Buffer, String, Size);

	ZEDebugCheckMemory();
}

void ZEString::SetValue(ZEInt Value, ZEUInt Base)
{
	char Buffer[35];
	ltoa(Value, Buffer, Base);
	SetValue(Buffer);
}

void ZEString::SetValue(ZEUInt Value, ZEUInt Base)
{
	char Buffer[35];
	ultoa(Value, Buffer, Base);
	SetValue(Buffer);
}

void ZEString::SetValue(float Value, ZEUInt Digits)
{
	char Buffer[_CVTBUFSIZE];
	gcvt(Value, Digits, Buffer);
	SetValue(Buffer);
}

void ZEString::SetValue(bool Value, const char* TrueText, const char* FalseText)
{
	SetValue(Value ? TrueText : FalseText);
}

const char* ZEString::GetValue() const
{
	if (Buffer == NULL)
		return "";
	else
		return Buffer;
}

char ZEString::GetCharacter(size_t Position) const
{
	zeAssert(Position > strlen(Buffer), "Position parameter value exceed length of the string.");
	return Buffer[Position];
}

void ZEString::SetCharacter(size_t Position, char Value)
{
	zeAssert(Position > strlen(Buffer), "Position parameter value exceed length of the string.");
	Buffer[Position] = Value;
}

size_t ZEString::GetLength() const
{
	if (Buffer != NULL)
		return strlen(Buffer);
	else
		return 0;
}

void ZEString::Append(const char* String)
{
	zeAssert(String == NULL, "String parameter is ilvalid.");
	
	if (Buffer == NULL)
		SetValue(String);
	else
	{
		size_t Length = strlen(Buffer);
		size_t StringLength = strlen(String);
		Allocator.Reallocate(&Buffer, (Length + StringLength + 1) * sizeof(char));
		memcpy(Buffer + Length, String, (StringLength + 1) * sizeof(char));
	}

	ZEDebugCheckMemory();
}

void ZEString::Append(const ZEString& String)
{
	Append(String.GetValue());
}

void ZEString::Insert(const char* String)
{
	zeAssert(String == NULL, "String parameter is ilvalid.");
	
	if (Buffer == NULL)
		SetValue(String);
	else
	{
		size_t StringLength = strlen(String);
		if (StringLength == 0)
			return;

		size_t Length = strlen(Buffer);

		char* OldBuffer = Buffer;
		bool Changed = Allocator.Allocate(&Buffer, (Length + StringLength + 1) * sizeof(char));

		memcpy(Buffer + StringLength, OldBuffer, (Length + 1) * sizeof(char));
		memcpy(Buffer, String, StringLength * sizeof(char));

		if (Changed)
			delete[] OldBuffer;
	}

	ZEDebugCheckMemory();
}

void ZEString::Insert(const ZEString& String)
{
	Insert(String.GetValue());
}

void ZEString::Insert(size_t Position, const ZEString& String)
{
	Insert(Position, String.GetValue());
}

void ZEString::Insert(size_t Position, const char* String)
{
	zeAssert(String == NULL, "Inserting string is not valid.");
	zeAssert(Buffer == NULL && Position != 0, "A string can only be inserted in to empty string at position 0.");

	size_t StringLength = strlen(String);
	if (StringLength == 0)
		return;

	if (Buffer == NULL && Position == 0)
	{
		SetValue(String);
		return;
	}

	size_t Length = strlen(Buffer);
	zeAssert(Position > Length, "Position parameter is more than string length.");
	Length += StringLength;

	char* OldBuffer = Buffer;
	// Check weather allocator allocated new memory location (returned true) or uses same memory location (returned false).
	if (Allocator.Allocate(&Buffer, Length * sizeof(char) + 1))
	{

		memcpy(Buffer, OldBuffer, Position * sizeof(char));
		memcpy(Buffer + Position, String, StringLength * sizeof(char));
		memcpy(Buffer + Position + StringLength, OldBuffer + Position, (Length - Position - StringLength + 1) * sizeof(char));

		// If allocator allocated new memory location we should delete old memory location.
		delete[] OldBuffer;
	}
	else
	{
		memcpy(Buffer + Position, String, StringLength * sizeof(char));
		memcpy(Buffer + Position + StringLength, Buffer + Position, (Length - Position - StringLength + 1) * sizeof(char));
	}

	ZEDebugCheckMemory();
}

void ZEString::Remove(size_t Position, size_t Count)
{

	if (Buffer == NULL && Position == 0 && Count == 0)
		return;

	size_t Length = strlen(Buffer);
	zeAssert(Position > Length, "Position parameter is more than string length.");
	zeAssert(Position + Count > Length, "Remove operation range (Position + Count) exceeds length of the string.");

	if (Length - Count == 0)
	{
		Clear();
		return;
	}

	char* OldBuffer = Buffer;
	if (Allocator.Allocate(&Buffer, (Length - Count + 1) * sizeof(char)))
	{
		memcpy(Buffer, OldBuffer, Position * sizeof(char));
		memcpy(Buffer + Position, OldBuffer + Position + Count, (Length - Position - Count + 1) * sizeof(char));
		delete[] OldBuffer;
	}
	else
	{
		memcpy(Buffer + Position, Buffer + Position + Count, (Length - Position - Count + 1) * sizeof(char));	
	}

	ZEDebugCheckMemory();
}

bool ZEString::Equals(const ZEString& String) const
{
	return strcmp(String.GetValue(), GetValue()) == 0;
}

bool ZEString::Equals(const char* String) const
{
	return strcmp(GetValue(), String) == 0;
}

void ZEString::CopyTo(ZEString& String) const
{
	String.CopyFrom(*this);
}

void ZEString::CopyFrom(const ZEString& String)
{
	if (String.GetLength() == 0)
		Clear();
	else
		SetValue(String.GetValue());
}

ZEString ZEString::Left(size_t Count) const
{
	if (Count == 0)
		return ZEString();

	zeAssert(Buffer == NULL, "Buffer is empty.");
	zeAssert(Count >  strlen(Buffer), "Position is bigger than string length.");

	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (Count + 1) * sizeof(char));
	memcpy(Temp.Buffer, Buffer, Count * sizeof(char)); 
	Temp.Buffer[Count] = L'\0';

	ZEDebugCheckMemory();

	return Temp;
}

ZEString ZEString::Right(size_t Count) const
{
	if (Count == 0)
		return ZEString();

	zeAssert(Buffer == NULL, "Buffer is empty.");

	size_t Length = strlen(Buffer);
	zeAssert(Count > strlen(Buffer), "Position is bigger than string length.");

	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (Count + 1) * sizeof(char));
	memcpy(Temp.Buffer, Buffer + Length - Count, (Count + 1) * sizeof(char)); 

	ZEDebugCheckMemory();

	return Temp;
}

ZEString ZEString::Middle(size_t Position, size_t Count) const
{
	if (Count == 0)
		return ZEString();

	zeAssert(Buffer == NULL, "Buffer is empty.");
	zeAssert(Position + Count > strlen(Buffer), "Sub string range (Position and count) is exceed string length.");

	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (Count + 1) * sizeof(char));
	memcpy(Temp.Buffer, Buffer + Position, Count * sizeof(char));
	Temp.Buffer[Count] = L'\0';

	ZEDebugCheckMemory();

	return Temp;
}

ZEString ZEString::SubString(size_t StartPosition, size_t EndPosition) const
{
	if (Buffer == NULL && StartPosition == 0 && EndPosition == 0)
		return ZEString();

	zeAssert(Buffer == NULL, "Buffer is empty.");
	zeAssert(StartPosition > strlen(Buffer) || EndPosition > strlen(Buffer), "Sub string range (Position and count) is exceed string length.");
	zeAssert(EndPosition < StartPosition, "EndPosition parameter can not be smaller than StartPosition parameter.");
	
	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (EndPosition - StartPosition + 2) * sizeof(char));
	memcpy(Temp.Buffer, Buffer + StartPosition, (EndPosition - StartPosition + 1) * sizeof(char));
	Temp.Buffer[EndPosition - StartPosition + 1] = '\0';

	ZEDebugCheckMemory();

	return Temp;
}

ZEString ZEString::TrimLeft() const
{
	if (Buffer == NULL)
		return ZEString();

	size_t Length = strlen(Buffer);

	char* Cursor = Buffer;
	while(*Cursor != L'\0' && (*Cursor == L' ' || *Cursor == L'\t'))
		Cursor++;

	size_t Count = Cursor - Buffer;
	if (Count == Length)
		return ZEString();
	
	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (Length - Count + 1) * sizeof(char));
	memcpy(Temp.Buffer, Buffer + Count, (Length - Count + 1) * sizeof(char)); 

	ZEDebugCheckMemory();

	return Temp;
}

ZEString ZEString::TrimRight() const
{
	if (Buffer == NULL)
		return ZEString();

	size_t Length = strlen(Buffer);
	char* Cursor = Buffer + (Length - 1);

	while(*Cursor != L'\0' && (*Cursor == L' ' || *Cursor == L'\t'))
		Cursor--; 
	
	size_t Count = Length - ((Cursor + 1) - Buffer);
	if (Count == Length)
		return ZEString();

	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (Length - Count + 1) * sizeof(char));
	memcpy(Temp.Buffer, Buffer, (Length - Count) * sizeof(char)); 
	Temp.Buffer[Length - Count] = L'\0';

	ZEDebugCheckMemory();

	return Temp;

}

ZEString ZEString::Trim() const
{
	if (Buffer == NULL)
		return ZEString();

	size_t Length = strlen(Buffer);

	char* Cursor = Buffer;
	while(*Cursor != L'\0' && (*Cursor == L' ' || *Cursor == L'\t'))
		Cursor++;

	size_t LeftCount = Cursor - Buffer;
	
	if (LeftCount == Length)
		return ZEString();

	Cursor = Buffer + (Length - 1);
	while(*Cursor != L'\0' && (*Cursor == L' ' || *Cursor == L'\t'))
		Cursor--; 
	
	size_t RightCount = Length - ((Cursor + 1) - Buffer);

	if (RightCount + LeftCount == Length)
		return ZEString();

	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (Length - LeftCount - RightCount + 1) * sizeof(char));
	memcpy(Temp.Buffer, Buffer + LeftCount, (Length - LeftCount - RightCount) * sizeof(char)); 
	Temp.Buffer[Length - LeftCount - RightCount] = L'\0';

	ZEDebugCheckMemory();

	return Temp;
}

ZEString ZEString::Lower() const
{
	if (Buffer == NULL)
		return ZEString();

	size_t Length = strlen(Buffer);
	
	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (Length + 1) * sizeof(char));

	for (size_t I = 0; I < Length; I++)
		Temp.Buffer[I] = tolower(Buffer[I]);

	Temp.Buffer[Length] = '\0';
	
	return Temp;
}

ZEString ZEString::Upper() const
{
	if (Buffer == NULL)
		return ZEString();

	size_t Length = strlen(Buffer);
	
	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (Length + 1) * sizeof(char));

	for (size_t I = 0; I < Length; I++)
		Temp.Buffer[I] = toupper(Buffer[I]);
	
	Temp.Buffer[Length] = '\0';

	return Temp;

}

ZEInt32 ZEString::ToInt() const
{
	return atoi(Buffer);
}

ZEUInt32 ZEString::ToUInt() const
{
	return strtoul(Buffer, NULL, 10);
}

float ZEString::ToFloat() const
{
 	return (float)atof(Buffer);
}

const char* ZEString::ToCString() const
{
	return Buffer;
}

std::string ZEString::ToStdString() const
{
	return std::string(Buffer);
}

ZEString ZEString::FromChar(char Value)
{
	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, 2 * sizeof(char));
	Temp.Buffer[0] = Value;
	Temp.Buffer[1] = '\0';

	return Temp;
}

ZEString ZEString::FromInt(ZEInt Value, ZEUInt Base)
{
	char Buffer[33];
	ltoa(Value, Buffer, Base);
	
	size_t Size = strlen(Buffer) + 1;
	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, Size * sizeof(char));
	memcpy(Temp.Buffer, Buffer, Size);

	return Temp;
}

ZEString ZEString::FromUInt(ZEUInt Value, ZEUInt Base)
{
	char Buffer[33];
	ultoa(Value, Buffer, Base);
	
	size_t Size = strlen(Buffer) + 1;
	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, Size * sizeof(char));
	memcpy(Temp.Buffer, Buffer, Size);

	return Temp;
}

ZEString ZEString::FromFloat(float Value, ZEUInt Digits)
{
	char Buffer[_CVTBUFSIZE];
	gcvt(Value, Digits, Buffer);
	
	ZEString Temp;
	size_t Size = (strlen(Buffer) + 1) * sizeof(char);
	Temp.Allocator.Allocate(&Temp.Buffer, Size);
	memcpy(Temp.Buffer, Buffer, Size);
	
	return Temp;
}

ZEString ZEString::FromBool(bool Value, const char* TrueText, const char* FalseText)
{
	return Value ? TrueText : FalseText;
}

ZEString ZEString::FromCString(const char* Value)
{
	return Value;
}

ZEString ZEString::FromStdString(const std::string& Value)
{
	return Value.c_str();
}

ZEString ZEString::Format(const char* Format, ...)
{
	va_list List;
	va_start(List, Format);
	
	int Length = _vscprintf(Format, List);
	
	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (Length + 1) * sizeof(char));
	vsprintf(Temp.Buffer, Format, List);
	
	va_end(List);
	
	return Temp;
}

ZEString& ZEString::operator=(const ZEString& String)
{
	CopyFrom(String);
	return *this;
}

ZEString& ZEString::operator=(const char* String)
{
	SetValue(String);
	return *this;
}

ZEString ZEString::operator+(const ZEString& String)
{
	ZEString Temp(*this);
	Temp.Append(String);
	return Temp;
}

ZEString ZEString::operator+(const char* String)
{
	ZEString Temp(*this);
	Temp.Append(String);
	return Temp;
}

ZEString& ZEString::operator+=(const ZEString& String)
{
	Append(String);
	return *this;
}

ZEString& ZEString::operator+=(const char* String)
{
	Append(String);
	return *this;
}

const char& ZEString::operator[](ptrdiff_t Index) const
{
	zeAssert(Buffer == NULL, "Empty string can not be indexed.");
	zeAssert(Index < 0, "Index parameter is negative.");
	zeAssert(Index > strlen(Buffer), "Index parameter value exceed length of the string.");
	return Buffer[Index];
}

char& ZEString::operator[](ptrdiff_t Index)
{
	zeAssert(Buffer == NULL, "Empty string can not be indexed.");
	zeAssert(Index < 0, "Index parameter is negative.");
	zeAssert(Index > strlen(Buffer), "Index parameter value exceed length of the string.");
	return Buffer[Index];
}


bool ZEString::operator!=(const ZEString& String) const
{
	return !Equals(String);
}

bool ZEString::operator!=(const char* String) const
{
	return !Equals(String);
}

bool ZEString::operator==(const ZEString& String) const
{
	return Equals(String);
}

bool ZEString::operator==(const char* String) const
{
	return Equals(String);
}

ZEString::operator std::string() const
{
	return std::string(Buffer);
}

ZEString::operator const char*() const
{
	return Buffer;
}

ZEString::ZEString()
{
	Buffer = NULL;
}

ZEString::ZEString(const char* String)
{
	Buffer = NULL;
	SetValue(String);
}

ZEString::ZEString(const ZEString& String)
{
	Buffer = NULL;
	CopyFrom(String);
}

ZEString::~ZEString()
{
	Clear();

	ZEDebugCheckMemory();
}

ZEString operator+(const char* String1, const ZEString& String2)
{
	ZEString Temp(String1);
	Temp.Append(String2);
	return Temp;
}

bool operator==(const char* String1, const ZEString& String2)
{
	return String2.Equals(String1);
}

bool operator!=(const char* String1, const ZEString& String2)
{
	return !String2.Equals(String1);
}
