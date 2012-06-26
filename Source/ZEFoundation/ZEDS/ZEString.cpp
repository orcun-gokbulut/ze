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
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stdarg.h>

#ifdef ZE_PLATFORM_COMPILER_GCC
	#define _snprintf snprintf
#endif
#include "ZEError.h"

bool ZEString::IsEmpty() const
{
	return (Buffer == NULL || (Buffer[0] == '\0')); 
}

ZESize ZEString::GetSize() const
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
	zeDebugCheck(String == NULL, "Null char array pointer are not valid parameters.");
	
	if (String == NULL)
	{
		Clear();
		return;
	}

	ZESize StringSize = strlen(String);
	if (StringSize == 0)
	{
		Clear();
		return;
	}

	ZESize Size = ((StringSize) + 1) * sizeof(char);
	
	char* OldBuffer = Buffer;
	if (Allocator.Allocate(&Buffer, Size))
		delete[] OldBuffer;

	memcpy(Buffer, String, Size);

	ZEDebugCheckMemory();
}

void ZEString::SetValue(wchar_t Character)
{
	char Temp[4];
	ZESize Size = wctomb(Temp, Character) + 1;

	Allocator.Allocate(&Buffer, Size);

	memcpy(Buffer, Temp, Size - 1);
	Temp[Size - 1] = '\0';
}

void ZEString::SetValue(const wchar_t* String)
{
	ZESize Size = wcstombs(NULL, String, (size_t)-1);
	Allocator.Allocate(&Buffer, Size);
	wcstombs(Buffer, String, (size_t)-1);
}

void ZEString::SetValue(ZEInt8 Value)
{
	char Buffer[35];
	_snprintf(Buffer, 35, "%d", Value);
	SetValue(Buffer);
}

void ZEString::SetValue(ZEInt16 Value)
{
	char Buffer[35];
	_snprintf(Buffer, 35, "%d", Value);
	SetValue(Buffer);
}

void ZEString::SetValue(ZEInt32 Value)
{
	char Buffer[35];
	_snprintf(Buffer, 35, "%d", Value);
	SetValue(Buffer);
}

void ZEString::SetValue(ZEInt64 Value)
{
	char Buffer[67];
	_snprintf(Buffer, 67, "%lld", Value);
	SetValue(Buffer);
}

void ZEString::SetValue(ZEUInt8 Value)
{
	char Buffer[35];
	_snprintf(Buffer, 35, "%u", Value);
	SetValue(Buffer);
}

void ZEString::SetValue(ZEUInt16 Value)
{
	char Buffer[35];
	_snprintf(Buffer, 35, "%u", Value);
	SetValue(Buffer);
}

void ZEString::SetValue(ZEUInt32 Value)
{
	char Buffer[35];
	_snprintf(Buffer, 35, "%u", Value);
	SetValue(Buffer);
}

void ZEString::SetValue(ZEUInt64 Value)
{
	char Buffer[67];
	_snprintf(Buffer, 67, "%llu", Value);
	SetValue(Buffer);
}

void ZEString::SetValue(float Value, ZEUInt Digits)
{
	char Format[100];
	_snprintf(Format, 100, "%%.%df", Digits);
	char Buffer[100];
	_snprintf(Buffer, 100, Format, Value);
	SetValue(Buffer);
}

void ZEString::SetValue(float Value)
{
	char Format[100];
	_snprintf(Format, 100, "%f");
	char Buffer[100];
	_snprintf(Buffer, 100, Format, Value);
	SetValue(Buffer);
}

void ZEString::SetValue(double Value, ZEUInt Digits)
{
	char Format[100];
	_snprintf(Format, 100, "%%.%df", Digits);
	char Buffer[100];
	_snprintf(Buffer, 100, Format, Value);
	SetValue(Buffer);
}

void ZEString::SetValue(double Value)
{
	char Format[100];
	_snprintf(Format, 100, "%f");
	char Buffer[100];
	_snprintf(Buffer, 100, Format, Value);
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

char ZEString::GetCharacter(ZESize Position) const
{
	zeDebugCheck(Position > strlen(Buffer), "Position parameter value exceed length of the string.");
	return Buffer[Position];
}

void ZEString::SetCharacter(ZESize Position, char Value)
{
	zeDebugCheck(Position > strlen(Buffer), "Position parameter value exceed length of the string.");
	Buffer[Position] = Value;
}

ZESize ZEString::GetLength() const
{
	if (Buffer != NULL)
		return strlen(Buffer);
	else
		return 0;
}

void ZEString::Append(const char* String)
{
	zeDebugCheck(String == NULL, "String parameter is ilvalid.");
	
	if (Buffer == NULL)
		SetValue(String);
	else
	{
		ZESize Length = strlen(Buffer);
		ZESize StringLength = strlen(String);
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
	zeDebugCheck(String == NULL, "String parameter is ilvalid.");
	
	if (Buffer == NULL)
		SetValue(String);
	else
	{
		ZESize StringLength = strlen(String);
		if (StringLength == 0)
			return;

		ZESize Length = strlen(Buffer);

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

void ZEString::Insert(ZESize Position, const ZEString& String)
{
	Insert(Position, String.GetValue());
}

void ZEString::Insert(ZESize Position, const char* String)
{
	zeDebugCheck(String == NULL, "Inserting string is not valid.");
	zeDebugCheck(Buffer == NULL && Position != 0, "A string can only be inserted in to empty string at position 0.");

	ZESize StringLength = strlen(String);
	if (StringLength == 0)
		return;

	if (Buffer == NULL && Position == 0)
	{
		SetValue(String);
		return;
	}

	ZESize Length = strlen(Buffer);
	zeDebugCheck(Position > Length, "Position parameter is more than string length.");
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

void ZEString::Remove(ZESize Position, ZESize Count)
{

	if (Buffer == NULL && Position == 0 && Count == 0)
		return;

	ZESize Length = strlen(Buffer);
	zeDebugCheck(Position > Length, "Position parameter is more than string length.");
	zeDebugCheck(Position + Count > Length, "Remove operation range (Position + Count) exceeds length of the string.");

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

ZEString ZEString::Left(ZESize Count) const
{
	if (Count == 0)
		return ZEString();

	zeDebugCheck(Buffer == NULL, "Buffer is empty.");
	zeDebugCheck(Count >  strlen(Buffer), "Position is bigger than string length.");

	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (Count + 1) * sizeof(char));
	memcpy(Temp.Buffer, Buffer, Count * sizeof(char)); 
	Temp.Buffer[Count] = L'\0';

	ZEDebugCheckMemory();

	return Temp;
}

ZEString ZEString::Right(ZESize Count) const
{
	if (Count == 0)
		return ZEString();

	zeDebugCheck(Buffer == NULL, "Buffer is empty.");

	ZESize Length = strlen(Buffer);
	zeDebugCheck(Count > strlen(Buffer), "Position is bigger than string length.");

	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (Count + 1) * sizeof(char));
	memcpy(Temp.Buffer, Buffer + Length - Count, (Count + 1) * sizeof(char)); 

	ZEDebugCheckMemory();

	return Temp;
}

ZEString ZEString::Middle(ZESize Position, ZESize Count) const
{
	if (Count == 0)
		return ZEString();

	zeDebugCheck(Buffer == NULL, "Buffer is empty.");
	zeDebugCheck(Position + Count > strlen(Buffer), "Sub string range (Position and count) is exceed string length.");

	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (Count + 1) * sizeof(char));
	memcpy(Temp.Buffer, Buffer + Position, Count * sizeof(char));
	Temp.Buffer[Count] = L'\0';

	ZEDebugCheckMemory();

	return Temp;
}

ZEString ZEString::SubString(ZESize StartPosition, ZESize EndPosition) const
{
	if (Buffer == NULL && StartPosition == 0 && EndPosition == 0)
		return ZEString();

	zeDebugCheck(Buffer == NULL, "Buffer is empty.");
	zeDebugCheck(StartPosition > strlen(Buffer) || EndPosition > strlen(Buffer), "Sub string range (Position and count) is exceed string length.");
	zeDebugCheck(EndPosition < StartPosition, "EndPosition parameter can not be smaller than StartPosition parameter.");
	
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

	ZESize Length = strlen(Buffer);

	char* Cursor = Buffer;
	while(*Cursor != L'\0' && (*Cursor == L' ' || *Cursor == L'\t'))
		Cursor++;

	ZESize Count = Cursor - Buffer;
	if (Count == Length)
		return ZEString();
	
	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (Length - Count + 1) * sizeof(char));
	memcpy(Temp.Buffer, Buffer + Count, (Length - Count + 1) * sizeof(char)); 

	ZEDebugCheckMemory();

	return Temp;
}

void ZEString::TrimLeftSelf()
{
	if (Buffer == NULL)
		return;

	ZESize Length = strlen(Buffer);

	char* Cursor = Buffer;
	while(*Cursor != L'\0' && (*Cursor == L' ' || *Cursor == L'\t'))
		Cursor++;
	 
	ZESize Count = Cursor - Buffer;
	if (Count == Length)
		return;

	memmove(Buffer, Buffer + Count, (Length - Count + 1) * sizeof(char));
	Allocator.Reallocate(&Buffer, (Length - Count + 1) * sizeof(char));
	Buffer[Length - Count] = L'\0';

	ZEDebugCheckMemory();
}

ZEString ZEString::TrimRight() const
{
	if (Buffer == NULL)
		return ZEString();

	ZESize Length = strlen(Buffer);
	char* Cursor = Buffer + (Length - 1);

	while(*Cursor != L'\0' && (*Cursor == L' ' || *Cursor == L'\t'))
		Cursor--; 
	
	ZESize Count = Length - ((Cursor + 1) - Buffer);
	if (Count == Length)
		return ZEString();

	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (Length - Count + 1) * sizeof(char));
	memcpy(Temp.Buffer, Buffer, (Length - Count) * sizeof(char)); 
	Temp.Buffer[Length - Count] = L'\0';

	ZEDebugCheckMemory();

	return Temp;
}

void ZEString::TrimRightSelf()
{
	if (Buffer == NULL)
		return;

	ZESize Length = strlen(Buffer);
	char* Cursor = Buffer + (Length - 1);

	while(*Cursor != L'\0' && (*Cursor == L' ' || *Cursor == L'\t'))
		Cursor--; 

	ZESize Count = Length - ((Cursor + 1) - Buffer);
	if (Count == Length)
		return;

	memmove(Buffer, Buffer, (Length - Count + 1) * sizeof(char));
	Allocator.Reallocate(&Buffer, (Length - Count + 1) * sizeof(char));
	Buffer[Length - Count] = L'\0';

	ZEDebugCheckMemory();
}

ZEString ZEString::Trim() const
{
	if (Buffer == NULL)
		return ZEString();

	ZESize Length = strlen(Buffer);

	char* Cursor = Buffer;
	while(*Cursor != L'\0' && (*Cursor == L' ' || *Cursor == L'\t'))
		Cursor++;

	ZESize LeftCount = Cursor - Buffer;
	
	if (LeftCount == Length)
		return ZEString();

	Cursor = Buffer + (Length - 1);
	while(*Cursor != L'\0' && (*Cursor == L' ' || *Cursor == L'\t'))
		Cursor--; 
	
	ZESize RightCount = Length - ((Cursor + 1) - Buffer);

	if (RightCount + LeftCount == Length)
		return ZEString();

	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (Length - LeftCount - RightCount + 1) * sizeof(char));
	memcpy(Temp.Buffer, Buffer + LeftCount, (Length - LeftCount - RightCount) * sizeof(char)); 
	Temp.Buffer[Length - LeftCount - RightCount] = L'\0';

	ZEDebugCheckMemory();

	return Temp;
}

void ZEString::TrimSelf()
{
	if (Buffer == NULL)
		return;

	ZESize Length = strlen(Buffer);

	char* Cursor = Buffer;
	while(*Cursor != L'\0' && (*Cursor == L' ' || *Cursor == L'\t'))
		Cursor++;

	ZESize LeftCount = Cursor - Buffer;

	if (LeftCount == Length)
		return;

	Cursor = Buffer + (Length - 1);
	while(*Cursor != L'\0' && (*Cursor == L' ' || *Cursor == L'\t'))
		Cursor--; 

	ZESize RightCount = Length - ((Cursor + 1) - Buffer);

	if (RightCount + LeftCount == Length)
		return;

	memmove(Buffer, Buffer + LeftCount, (Length - LeftCount - RightCount) * sizeof(char));
	Allocator.Reallocate(&Buffer, (Length - LeftCount - RightCount + 1) * sizeof(char));
	Buffer[Length - LeftCount - RightCount] = L'\0';

	ZEDebugCheckMemory();
}

ZEString ZEString::Lower() const
{
	if (Buffer == NULL)
		return ZEString();

	ZESize Length = strlen(Buffer);
	
	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (Length + 1) * sizeof(char));

	for (ZESize I = 0; I < Length; I++)
		Temp.Buffer[I] = tolower(Buffer[I]);

	Temp.Buffer[Length] = '\0';
	
	return Temp;
}

void ZEString::LowerSelf()
{
	if (Buffer == NULL)
		return;

	ZESize Length = strlen(Buffer);

	for (ZESize I = 0; I < Length; I++)
		Buffer[I] = tolower(Buffer[I]);
}

ZEString ZEString::Upper() const
{
	if (Buffer == NULL)
		return ZEString();

	ZESize Length = strlen(Buffer);
	
	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (Length + 1) * sizeof(char));

	for (ZESize I = 0; I < Length; I++)
		Temp.Buffer[I] = toupper(Buffer[I]);
	
	Temp.Buffer[Length] = '\0';

	return Temp;

}

void ZEString::UpperSelf()
{
	if (Buffer == NULL)
		return;

	ZESize Length = strlen(Buffer);

	for (ZESize I = 0; I < Length; I++)
		Buffer[I] = toupper(Buffer[I]);
}

ZEInt8 ZEString::ToInt8() const
{
	return (ZEInt8)atoi(Buffer);
}

ZEInt16 ZEString::ToInt16() const
{
	return (ZEInt16)atoi(Buffer);
}

ZEInt32 ZEString::ToInt32() const
{
	return (ZEInt32)atoi(Buffer);
}

ZEInt64 ZEString::ToInt64() const
{
	return (ZEInt64)_atoi64(Buffer);
}

ZEUInt8 ZEString::ToUInt8() const
{
	return (ZEUInt8)strtoul(Buffer, NULL, 10);
}

ZEUInt16 ZEString::ToUInt16() const
{
	return (ZEUInt16)strtoul(Buffer, NULL, 10);
}

ZEUInt32 ZEString::ToUInt32() const
{
	return (ZEUInt32)strtoul(Buffer, NULL, 10);
}

ZEUInt64 ZEString::ToUInt64() const
{
	return (ZEUInt64)_strtoui64(Buffer, NULL, 10);
}

float ZEString::ToFloat() const
{
 	return (float)atof(Buffer);
}

double ZEString::ToDouble() const
{
	return atof(Buffer);
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

ZEString ZEString::FromWChar(wchar_t Value)
{
	ZEString Temp;
	Temp.SetValue(Value);
	return Temp;
}

ZEString ZEString::FromInt8(ZEInt8 Value)
{
	ZEString Temp(Value);
	return Temp;
}

ZEString ZEString::FromInt16(ZEInt16 Value)
{
	ZEString Temp(Value);
	return Temp;
}

ZEString ZEString::FromInt32(ZEInt32 Value)
{
	ZEString Temp(Value);
	return Temp;
}

ZEString ZEString::FromInt64(ZEInt64 Value)
{
	ZEString Temp(Value);
	return Temp;
}

ZEString ZEString::FromUInt8(ZEUInt8 Value)
{
	ZEString Temp(Value);
	return Temp;
}

ZEString ZEString::FromUInt16(ZEUInt16 Value)
{
	ZEString Temp(Value);
	return Temp;
}

ZEString ZEString::FromUInt32(ZEUInt32 Value)
{
	ZEString Temp(Value);
	return Temp;
}

ZEString ZEString::FromUInt64(ZEUInt64 Value)
{
	ZEString Temp(Value);
	return Temp;
}

ZEString ZEString::FromFloat(float Value, ZEUInt Digits)
{
	ZEString Temp(Value, Digits);
	return Temp;
}

ZEString ZEString::FromFloat(float Value)
{
	ZEString Temp(Value);
	return Temp;
}

ZEString ZEString::FromDouble(double Value, ZEUInt Digits)
{
	ZEString Temp(Value, Digits);
	return Temp;
}

ZEString ZEString::FromDouble(double Value)
{
	ZEString Temp(Value);
	return Temp;
}

ZEString ZEString::FromBool(bool Value, const char* TrueText, const char* FalseText)
{
	return Value ? TrueText : FalseText;
}

ZEString ZEString::FromWString(const wchar_t* Value)
{
	ZEString Temp;
	Temp.SetValue(Value);
	return Temp;
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
	
	ZESize Length = _vscprintf(Format, List);
	
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

ZEString& ZEString::operator=(ZEInt8 Value)
{
	SetValue(Value);
	return *this;
}

ZEString& ZEString::operator=(ZEInt16 Value)
{
	SetValue(Value);
	return *this;
}

ZEString& ZEString::operator=(ZEInt32 Value)
{
	SetValue(Value);
	return *this;
}

ZEString& ZEString::operator=(ZEInt64 Value)
{
	SetValue(Value);
	return *this;
}

ZEString& ZEString::operator=(ZEUInt8 Value)
{
	SetValue(Value);
	return *this;
}

ZEString& ZEString::operator=(ZEUInt16 Value)
{
	SetValue(Value);
	return *this;
}

ZEString& ZEString::operator=(ZEUInt32 Value)
{
	SetValue(Value);
	return *this;
}

ZEString& ZEString::operator=(ZEUInt64 Value)
{
	SetValue(Value);
	return *this;
}

ZEString& ZEString::operator=(float Value)
{
	SetValue(Value);
	return *this;
}

ZEString& ZEString::operator=(double Value)
{
	SetValue(Value);
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

ZEString ZEString::operator+(ZEInt8 Value)
{
	return *this + ZEString(Value);
}

ZEString ZEString::operator+(ZEInt16 Value)
{
	return *this + ZEString(Value);
}

ZEString ZEString::operator+(ZEInt32 Value)
{
	return *this + ZEString(Value);
}

ZEString ZEString::operator+(ZEInt64 Value)
{
	return *this + ZEString(Value);
}

ZEString ZEString::operator+(ZEUInt8 Value)
{
	return *this + ZEString(Value);
}

ZEString ZEString::operator+(ZEUInt16 Value)
{
	return *this + ZEString(Value);
}

ZEString ZEString::operator+(ZEUInt32 Value)
{
	return *this + ZEString(Value);
}

ZEString ZEString::operator+(ZEUInt64 Value)
{
	return *this + ZEString(Value);
}

ZEString ZEString::operator+(float Value)
{
	return *this + ZEString(Value);
}

ZEString ZEString::operator+(double Value)
{
	return *this + ZEString(Value);
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

ZEString& ZEString::operator+=(ZEInt8 Value)
{
	Append(ZEString(Value));
	return *this;
}

ZEString& ZEString::operator+=(ZEInt16 Value)
{
	Append(ZEString(Value));
	return *this;
}

ZEString& ZEString::operator+=(ZEInt32 Value)
{
	Append(ZEString(Value));
	return *this;
}

ZEString& ZEString::operator+=(ZEInt64 Value)
{
	Append(ZEString(Value));
	return *this;
}

ZEString& ZEString::operator+=(ZEUInt8 Value)
{
	Append(ZEString(Value));
	return *this;
}

ZEString& ZEString::operator+=(ZEUInt16 Value)
{
	Append(ZEString(Value));
	return *this;
}

ZEString& ZEString::operator+=(ZEUInt32 Value)
{
	Append(ZEString(Value));
	return *this;
}

ZEString& ZEString::operator+=(ZEUInt64 Value)
{
	Append(ZEString(Value));
	return *this;
}

ZEString& ZEString::operator+=(float Value)
{
	Append(ZEString(Value));
	return *this;
}

ZEString& ZEString::operator+=(double Value)
{
	Append(ZEString(Value));
	return *this;
}

const char& ZEString::operator[](ZESSize Index) const
{
	zeDebugCheck(Buffer == NULL, "Empty string can not be indexed.");
	zeDebugCheck(Index < 0, "Index parameter is negative.");
	zeDebugCheck((ZESize)Index > strlen(Buffer), "Index parameter value exceed length of the string.");
	return Buffer[Index];
}

char& ZEString::operator[](ZESSize Index)
{
	zeDebugCheck(Buffer == NULL, "Empty string can not be indexed.");
	zeDebugCheck(Index < 0, "Index parameter is negative.");
	zeDebugCheck((ZESize)Index > strlen(Buffer), "Index parameter value exceed length of the string.");
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

ZEString::ZEString(ZEInt8 Value)
{
	Buffer = NULL;
	SetValue(Value);
}

ZEString::ZEString(ZEInt16 Value)
{
	Buffer = NULL;
	SetValue(Value);
}

ZEString::ZEString(ZEInt32 Value)
{
	Buffer = NULL;
	SetValue(Value);
}

ZEString::ZEString(ZEInt64 Value)
{
	Buffer = NULL;
	SetValue(Value);
}

ZEString::ZEString(ZEUInt8 Value)
{
	Buffer = NULL;
	SetValue(Value);
}

ZEString::ZEString(ZEUInt16 Value)
{
	Buffer = NULL;
	SetValue(Value);
}

ZEString::ZEString(ZEUInt32 Value)
{
	Buffer = NULL;
	SetValue(Value);
}

ZEString::ZEString(ZEUInt64 Value)
{
	Buffer = NULL;
	SetValue(Value);
}

ZEString::ZEString(float Value, ZEUInt Digits)
{
	Buffer = NULL;
	SetValue(Value, Digits);
}

ZEString::ZEString(float Value)
{
	Buffer = NULL;
	SetValue(Value);
}

ZEString::ZEString(double Value, ZEUInt Digits)
{
	Buffer = NULL;
	SetValue(Value, Digits);
}

ZEString::ZEString(double Value)
{
	Buffer = NULL;
	SetValue(Value);
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

ZEString operator+(ZEInt8 Value, const ZEString& String)
{
	ZEString Temp(Value);
	Temp.Append(String);
	return Temp;
}

ZEString operator+(ZEInt16 Value, const ZEString& String)
{
	ZEString Temp(Value);
	Temp.Append(String);
	return Temp;
}

ZEString operator+(ZEInt32 Value, const ZEString& String)
{
	ZEString Temp(Value);
	Temp.Append(String);
	return Temp;
}

ZEString operator+(ZEInt64 Value, const ZEString& String)
{
	ZEString Temp(Value);
	Temp.Append(String);
	return Temp;
}

ZEString operator+(ZEUInt8 Value, const ZEString& String)
{
	ZEString Temp(Value);
	Temp.Append(String);
	return Temp;
}

ZEString operator+(ZEUInt16 Value, const ZEString& String)
{
	ZEString Temp(Value);
	Temp.Append(String);
	return Temp;
}

ZEString operator+(ZEUInt32 Value, const ZEString& String)
{
	ZEString Temp(Value);
	Temp.Append(String);
	return Temp;
}

ZEString operator+(ZEUInt64 Value, const ZEString& String)
{
	ZEString Temp(Value);
	Temp.Append(String);
	return Temp;
}

ZEString operator+(float Value, const ZEString& String)
{
	ZEString Temp(Value);
	Temp.Append(String);
	return Temp;
}

ZEString operator+(double Value, const ZEString& String)
{
	ZEString Temp(Value);
	Temp.Append(String);
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
