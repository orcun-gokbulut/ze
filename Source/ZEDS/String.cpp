//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - String.cpp
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

#include "String.h"
#include "Definitions.h"

#include <string.h>
#include <memory.h>
#include <mbstring.h> 
#include <wchar.h>
#include <stdlib.h>
#include <string>

#ifdef ZEDEBUG_CHECKMEMORY
#include <crtdbg.h>
#endif

bool ZEString::IsEmpty() const
{
	return (Buffer == NULL || (Buffer[0] == L'\0')); 
}

size_t ZEString::GetSize() const
{
	return Allocator.GetSize();
}

void ZEString::Compact()
{
	if (Buffer != NULL)
		Allocator.Reallocate(&Buffer, (wcslen(Buffer) + 1) * sizeof(wchar_t));
}

void ZEString::Clear()
{
	Allocator.Deallocate(&Buffer);
}

void ZEString::SetValue(wchar_t Character)
{
	wchar_t* OldBuffer = Buffer;
	if (Allocator.Allocate(&Buffer, sizeof(wchar_t) * 2))
		delete[] OldBuffer;
	
	Buffer[0] = Character;
	Buffer[1] = L'\0';
}


void ZEString::SetValue(const wchar_t* String)
{
	ZEDS_ASSERT(String == NULL, "Null wchar_t array pointer are not valid parameters.");
	
	size_t StringSize = wcslen(String);
	if (StringSize == 0)
	{
		Clear();
		return;
	}

	size_t Size = ((StringSize) + 1) * sizeof(wchar_t);
	
	wchar_t* OldBuffer = Buffer;
	if (Allocator.Allocate(&Buffer, Size))
		delete[] OldBuffer;

	memcpy(Buffer, String, Size);

	#ifdef ZEDEBUG_CHECKMEMORY
		ZEDS_ASSERT(!_CrtCheckMemory(), "Heap problem");
	#endif
}

void ZEString::SetValue(const char* String)
{
	ZEDS_ASSERT(String == NULL, "Null char array pointer are not valid parameters.");
	
	size_t StringSize = strlen(String);
	if (StringSize == 0)
		Clear();

	size_t Size = (StringSize + 1) * sizeof(wchar_t);
	wchar_t* OldBuffer = Buffer;

	if (Allocator.Allocate(&Buffer, Size))
		delete[] OldBuffer;

	mbstowcs(Buffer, String, Size);

	#ifdef ZEDEBUG_CHECKMEMORY
		ZEDS_ASSERT(!_CrtCheckMemory(), "Heap problem");
	#endif
}

void ZEString::SetNumbericValue(char Value, unsigned int Base)
{
	wchar_t Buffer[10];
	_itow(Value, Buffer, Base);
	SetValue(Buffer);
}

void ZEString::SetNumbericValue(unsigned char Value, unsigned int Base)
{
	wchar_t Buffer[10];
	_ultow(Value, Buffer, Base);
	SetValue(Buffer);
}

void ZEString::SetNumbericValue(short Value, unsigned int Base)
{
	wchar_t Buffer[35];
	_ultow(Value, Buffer, Base);
	SetValue(Buffer);
}

void ZEString::SetNumbericValue(unsigned short Value, unsigned int Base)
{
	wchar_t Buffer[35];
	_ultow(Value, Buffer, Base);
	SetValue(Buffer);
}

void ZEString::SetNumbericValue(int Value, unsigned int Base)
{
	wchar_t Buffer[35];
	_itow(Value, Buffer, Base);
	SetValue(Buffer);
}

void ZEString::SetNumbericValue(unsigned int Value, unsigned int Base)
{
	wchar_t Buffer[35];
	_ultow(Value, Buffer, Base);
	SetValue(Buffer);
}

void ZEString::SetNumbericValue(long Value, unsigned int Base)
{
	wchar_t Buffer[35];
	_ltow(Value, Buffer, Base);
	SetValue(Buffer);
}

void ZEString::SetNumbericValue(unsigned long Value, unsigned int Base)
{
	wchar_t Buffer[35];
	_ultow(Value, Buffer, Base);
	SetValue(Buffer);

}

void ZEString::SetNumbericValue(float Value, unsigned int NumberOfDigits)
{
	char Buffer[_CVTBUFSIZE];
	gcvt(Value, NumberOfDigits, Buffer);
	SetValue(Buffer);
}

void ZEString::SetNumbericValue(double Value, unsigned int NumberOfDigits)
{
	char Buffer[_CVTBUFSIZE];
	gcvt(Value, NumberOfDigits, Buffer);
	SetValue(Buffer);
}

void ZEString::SetBooleanValue(bool Value)
{
	SetValue(Value ? L"true" : L"false");
}


const wchar_t* ZEString::GetValue() const
{
	if (Buffer == NULL)
		return L"";
	else
		return Buffer;
}

wchar_t ZEString::GetCharacter(size_t Position) const
{
	ZEDS_ASSERT(Position > wcslen(Buffer), "Position parameter value exceed length of the string.");
	return Buffer[Position];
}

void ZEString::SetCharacter(size_t Position, wchar_t Value)
{
	ZEDS_ASSERT(Position > wcslen(Buffer), "Position parameter value exceed length of the string.");
	Buffer[Position] = Value;
}

size_t ZEString::GetLength() const
{
	if (Buffer != NULL)
		return wcslen(Buffer);
	else
		return 0;
}

void ZEString::Append(const wchar_t* String)
{
	ZEDS_ASSERT(String == NULL, "String parameter is ilvalid.");
	
	if (Buffer == NULL)
		SetValue(String);
	else
	{
		size_t Length = wcslen(Buffer);
		size_t StringLength = wcslen(String);
		Allocator.Reallocate(&Buffer, (Length + StringLength + 1) * sizeof(wchar_t));
		memcpy(Buffer + Length, String, (StringLength + 1) * sizeof(wchar_t));
	}

	#ifdef ZEDEBUG_CHECKMEMORY
		ZEDS_ASSERT(!_CrtCheckMemory(), "Heap problem");
	#endif
}

void ZEString::Append(wchar_t Character)
{
	if (Buffer == NULL)
		SetValue(Character);
	else
	{
		size_t Length = wcslen(Buffer);
		Allocator.Reallocate(&Buffer, (Length + 2) * sizeof(wchar_t));
		Buffer[Length] = Character;
		Buffer[Length + 1] = L'\0';
	}
}


void ZEString::Append(const ZEString& String)
{
	Append(String.GetValue());
}

void ZEString::Insert(const wchar_t* String)
{
	ZEDS_ASSERT(String == NULL, "String parameter is ilvalid.");
	
	if (Buffer == NULL)
		SetValue(String);
	else
	{
		size_t StringLength = wcslen(String);
		if (StringLength == 0)
			return;

		size_t Length = wcslen(Buffer);

		wchar_t* OldBuffer = Buffer;
		bool Changed = Allocator.Allocate(&Buffer, (Length + StringLength + 1) * sizeof(wchar_t));

		memcpy(Buffer + StringLength, OldBuffer, (Length + 1) * sizeof(wchar_t));
		memcpy(Buffer, String, StringLength * sizeof(wchar_t));

		if (Changed)
			delete[] OldBuffer;
	}

	#ifdef ZEDEBUG_CHECKMEMORY
		ZEDS_ASSERT(!_CrtCheckMemory(), "Heap problem");
	#endif
}

void ZEString::Insert(const ZEString& String)
{
	Insert(String.GetValue());
}

void ZEString::Insert(size_t Position, const ZEString& String)
{
	Insert(Position, String.GetValue());
}

void ZEString::Insert(size_t Position, const wchar_t* String)
{
	ZEDS_ASSERT(String == NULL, "Inserting string is not valid.");
	ZEDS_ASSERT(Buffer == NULL && Position != 0, "A string can only be inserted in to empty string at position 0.");

	size_t StringLength = wcslen(String);
	if (StringLength == 0)
		return;

	if (Buffer == NULL && Position == 0)
	{
		SetValue(String);
		return;
	}

	size_t Length = wcslen(Buffer);
	ZEDS_ASSERT(Position > Length, "Position parameter is more than string length.");
	Length += StringLength;

	wchar_t* OldBuffer = Buffer;
	// Check weather allocator allocated new memory location (returned true) or uses same memory location (returned false).
	if (Allocator.Allocate(&Buffer, Length * sizeof(wchar_t) + 1))
	{

		memcpy(Buffer, OldBuffer, Position * sizeof(wchar_t));
		memcpy(Buffer + Position, String, StringLength * sizeof(wchar_t));
		memcpy(Buffer + Position + StringLength, OldBuffer + Position, (Length - Position - StringLength + 1) * sizeof(wchar_t));

		// If allocator allocated new memory location we should delete old memory location.
		delete[] OldBuffer;
	}
	else
	{
		memcpy(Buffer + Position, String, StringLength * sizeof(wchar_t));
		memcpy(Buffer + Position + StringLength, Buffer + Position, (Length - Position - StringLength + 1) * sizeof(wchar_t));
	}

	#ifdef ZEDEBUG_CHECKMEMORY
		ZEDS_ASSERT(!_CrtCheckMemory(), "Heap problem");
	#endif
}

void ZEString::Insert(size_t Position, wchar_t Character)
{
	if (Buffer == NULL && Position == 0)
	{
		SetValue(Character);
		return;
	}

	size_t Length = wcslen(Buffer);
	ZEDS_ASSERT(Position > Length, "Position parameter is more than string length.");

	// Check weather allocator allocated new memory location (returned true) or uses same memory location (returned false).
	wchar_t* OldBuffer = Buffer;
	if (Allocator.Allocate(&Buffer, (Length + 2) * sizeof(wchar_t)))
	{
		memcpy(Buffer, OldBuffer, Position * sizeof(wchar_t));
		Buffer[Position] = Character;
		memcpy(Buffer + Position + 1, OldBuffer + Position, (Length - Position) * sizeof(wchar_t));

		// If allocator allocated new memory location we should delete old memory location.
		delete[] OldBuffer;
	}
	else
	{
		Buffer[Position] = Character;
		memcpy(Buffer + Position + 1, Buffer + Position, (Length - Position) * sizeof(wchar_t));
	}

	#ifdef ZEDEBUG_CHECKMEMORY
		ZEDS_ASSERT(!_CrtCheckMemory(), "Heap problem");
	#endif
}

void ZEString::Remove(size_t Position, size_t Count)
{

	if (Buffer == NULL && Position == 0 && Count == 0)
		return;

	size_t Length = wcslen(Buffer);
	ZEDS_ASSERT(Position > Length, "Position parameter is more than string length.");
	ZEDS_ASSERT(Position + Count > Length, "Remove operation range (Position + Count) exceeds length of the string.");

	if (Length - Count == 0)
	{
		Clear();
		return;
	}

	wchar_t* OldBuffer = Buffer;
	if (Allocator.Allocate(&Buffer, (Length - Count + 1) * sizeof(wchar_t)))
	{
		memcpy(Buffer, OldBuffer, Position * sizeof(wchar_t));
		memcpy(Buffer + Position, OldBuffer + Position + Count, (Length - Position - Count + 1) * sizeof(wchar_t));
		delete[] OldBuffer;
	}
	else
	{
		memcpy(Buffer + Position, Buffer + Position + Count, (Length - Position - Count + 1) * sizeof(wchar_t));	
	}

	#ifdef ZEDEBUG_CHECKMEMORY
		ZEDS_ASSERT(!_CrtCheckMemory(), "Heap problem");
	#endif
}
		
bool ZEString::Equals(const ZEString& String) const
{
	return wcscmp(String.GetValue(), GetValue()) == 0;
}

bool ZEString::Equals(const wchar_t* String) const
{
	return wcscmp(GetValue(), String) == 0;
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

	ZEDS_ASSERT(Buffer == NULL, "Buffer is empty.");
	ZEDS_ASSERT(Count >  wcslen(Buffer), "Position is bigger than string length.");

	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (Count + 1) * sizeof(wchar_t));
	memcpy(Temp.Buffer, Buffer, Count * sizeof(wchar_t)); 
	Temp.Buffer[Count] = L'\0';

	#ifdef ZEDEBUG_CHECKMEMORY
		ZEDS_ASSERT(!_CrtCheckMemory(), "Heap problem");
	#endif

	return Temp;
}

ZEString ZEString::Right(size_t Count) const
{
	if (Count == 0)
		return ZEString();

	ZEDS_ASSERT(Buffer == NULL, "Buffer is empty.");

	size_t Length = wcslen(Buffer);
	ZEDS_ASSERT(Count > wcslen(Buffer), "Position is bigger than string length.");

	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (Count + 1) * sizeof(wchar_t));
	memcpy(Temp.Buffer, Buffer + Length - Count, (Count + 1) * sizeof(wchar_t)); 

	#ifdef ZEDEBUG_CHECKMEMORY
		ZEDS_ASSERT(!_CrtCheckMemory(), "Heap problem");
	#endif

	return Temp;
}

ZEString ZEString::Middle(size_t Position, size_t Count) const
{
	if (Count == 0)
		return ZEString();

	ZEDS_ASSERT(Buffer == NULL, "Buffer is empty.");
	ZEDS_ASSERT(Position + Count > wcslen(Buffer), "Sub string range (Position and count) is exceed string length.");

	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (Count + 1) * sizeof(wchar_t));
	memcpy(Temp.Buffer, Buffer + Position, Count * sizeof(wchar_t));
	Temp.Buffer[Count] = L'\0';

	#ifdef ZEDEBUG_CHECKMEMORY
		ZEDS_ASSERT(!_CrtCheckMemory(), "Heap problem");
	#endif

	return Temp;
}

ZEString ZEString::SubString(size_t StartPosition, size_t EndPosition) const
{
	if (Buffer == NULL && StartPosition == 0 && EndPosition == 0)
		return ZEString();

	ZEDS_ASSERT(Buffer == NULL, "Buffer is empty.");
	ZEDS_ASSERT(StartPosition > wcslen(Buffer) || EndPosition > wcslen(Buffer), "Sub string range (Position and count) is exceed string length.");
	ZEDS_ASSERT(EndPosition < StartPosition, "EndPosition parameter can not be smaller than StartPosition parameter.");
	
	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (EndPosition - StartPosition + 2) * sizeof(wchar_t));
	memcpy(Temp.Buffer, Buffer + StartPosition, (EndPosition - StartPosition + 1) * sizeof(wchar_t));
	Temp.Buffer[EndPosition - StartPosition + 1] = '\0';

	#ifdef ZEDEBUG_CHECKMEMORY
		ZEDS_ASSERT(!_CrtCheckMemory(), "Heap problem");
	#endif

	return Temp;
}

ZEString ZEString::TrimLeft() const
{
	if (Buffer == NULL)
		return ZEString();

	size_t Length = wcslen(Buffer);

	wchar_t* Cursor = Buffer;
	while(*Cursor != L'\0' && (*Cursor == L' ' || *Cursor == L'\t'))
		Cursor++;

	size_t Count = Cursor - Buffer;
	if (Count == Length)
		return ZEString();
	
	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (Length - Count + 1) * sizeof(wchar_t));
	memcpy(Temp.Buffer, Buffer + Count, (Length - Count + 1) * sizeof(wchar_t)); 

	#ifdef ZEDEBUG_CHECKMEMORY
		ZEDS_ASSERT(!_CrtCheckMemory(), "Heap problem");
	#endif

	return Temp;
}

ZEString ZEString::TrimRight() const
{
	if (Buffer == NULL)
		return ZEString();

	size_t Length = wcslen(Buffer);
	wchar_t* Cursor = Buffer + (Length - 1);

	while(*Cursor != L'\0' && (*Cursor == L' ' || *Cursor == L'\t'))
		Cursor--; 
	
	size_t Count = Length - ((Cursor + 1) - Buffer);
	if (Count == Length)
		return ZEString();

	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (Length - Count + 1) * sizeof(wchar_t));
	memcpy(Temp.Buffer, Buffer, (Length - Count) * sizeof(wchar_t)); 
	Temp.Buffer[Length - Count] = L'\0';

	#ifdef ZEDEBUG_CHECKMEMORY
		ZEDS_ASSERT(!_CrtCheckMemory(), "Heap problem");
	#endif

	return Temp;

}

ZEString ZEString::Trim() const
{
	if (Buffer == NULL)
		return ZEString();

	size_t Length = wcslen(Buffer);

	wchar_t* Cursor = Buffer;
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
	Temp.Allocator.Allocate(&Temp.Buffer, (Length - LeftCount - RightCount + 1) * sizeof(wchar_t));
	memcpy(Temp.Buffer, Buffer + LeftCount, (Length - LeftCount - RightCount) * sizeof(wchar_t)); 
	Temp.Buffer[Length - LeftCount - RightCount] = L'\0';

	#ifdef ZEDEBUG_CHECKMEMORY
		ZEDS_ASSERT(!_CrtCheckMemory(), "Heap problem");
	#endif

	return Temp;
}

ZEString ZEString::ToLower() const
{
	if (Buffer == NULL)
		return ZEString();

	size_t Length = wcslen(Buffer);
	
	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (Length + 1) * sizeof(wchar_t));

	for (size_t I = 0; I < Length; I++)
		Temp.Buffer[I] = towlower(Buffer[I]);

	Temp.Buffer[Length] = L'\0';
	
	return Temp;
}

ZEString ZEString::ToUpper() const
{
	if (Buffer == NULL)
		return ZEString();

	size_t Length = wcslen(Buffer);
	
	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (Length + 1) * sizeof(wchar_t));

	for (size_t I = 0; I < Length; I++)
		Temp.Buffer[I] = towupper(Buffer[I]);
	
	Temp.Buffer[Length] = L'\0';

	return Temp;

}

unsigned int ZEString::ToUnsignedInteger()
{
	return _wtoi(Buffer);
}

int ZEString::ToInteger()
{
	return _wtoi(Buffer);
}

float ZEString::ToFloat()
{
	return _wtof(Buffer);
}

double ZEString::ToDouble()
{
	wchar_t* Temp;
	return wcstod(Buffer, &Temp);
}

ZEString& ZEString::operator=(const ZEString& String)
{
	CopyFrom(String);
	return *this;
}

ZEString& ZEString::operator=(const wchar_t* String)
{
	SetValue(String);
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

ZEString ZEString::operator+(const wchar_t* String)
{
	ZEString Temp(*this);
	Temp.Append(String);
	return Temp;
}

ZEString ZEString::operator+(const char* String)
{
	ZEString Temp(*this);
	Temp.Append(ZEString(String));
	return Temp;
}

ZEString& ZEString::operator+=(const ZEString& String)
{
	Append(String);
	return *this;
}

ZEString& ZEString::operator+=(const wchar_t* String)
{
	Append(String);
	return *this;
}

ZEString& ZEString::operator+=(const char* String)
{
	Append(ZEString(String));
	return *this;
}

const wchar_t ZEString::operator[](size_t Index) const
{
	return GetCharacter(Index);
}

wchar_t& ZEString::operator[](size_t Index)
{
	ZEDS_ASSERT(Index > wcslen(Buffer), "Index parameter value exceed length of the string.");
	return Buffer[Index];
}

bool ZEString::operator!=(const ZEString& String) const
{
	return !Equals(String);
}

bool ZEString::operator!=(const wchar_t* String) const
{
	return !Equals(String);
}

bool ZEString::operator!=(const char* String) const
{
	return !Equals(ZEString(String));
}

bool ZEString::operator==(const ZEString& String) const
{
	return Equals(String);
}

bool ZEString::operator==(const wchar_t* String) const
{
	return Equals(String);
}

bool ZEString::operator==(const char* String) const
{
	return Equals(ZEString(String));
}

ZEString::operator std::string() const
{
	if (CharBuffer != NULL)
		delete[] CharBuffer;

	size_t Length = wcslen(Buffer);
	((ZEString*)this)->CharBuffer = new char[Length * sizeof(char) * 3 + sizeof(char)];
	wcstombs(CharBuffer, Buffer, Length);

	#ifdef ZEDEBUG_CHECKMEMORY
		ZEDS_ASSERT(!_CrtCheckMemory(), "Heap problem");
	#endif

	return std::string(CharBuffer);
}

ZEString::operator std::wstring() const
{
	return std::wstring(Buffer);
}

ZEString::operator const wchar_t*() const
{
	if (Buffer == NULL)
		return L"";
	else
		return Buffer;
}

ZEString::operator const char*() const
{
	if (Buffer == NULL)
		return "";

	if (CharBuffer != NULL)
		delete[] CharBuffer;

	size_t Length = wcslen(Buffer);
	((ZEString*)this)->CharBuffer = new char[(Length + 1) * sizeof(char) * 3];
	wcstombs(CharBuffer, Buffer, Length + 1);

	#ifdef ZEDEBUG_CHECKMEMORY
		ZEDS_ASSERT(!_CrtCheckMemory(), "Heap problem");
	#endif

	return CharBuffer;
}

ZEString::ZEString()
{
	Buffer = NULL;
	CharBuffer = NULL;
}

ZEString::ZEString(const char* String)
{
	Buffer = NULL;
	CharBuffer = NULL;
	SetValue(String);
}

ZEString::ZEString(const wchar_t* String)
{
	Buffer = NULL;
	CharBuffer = NULL;
	SetValue(String);
}

ZEString::ZEString(const ZEString& String)
{
	Buffer = NULL;
	CharBuffer = NULL;
	CopyFrom(String);
}

ZEString::~ZEString()
{
	if (CharBuffer == NULL)
		delete[] CharBuffer;
	
	if (Buffer != NULL)
		Allocator.Deallocate(&Buffer);

	#ifdef ZEDEBUG_CHECKMEMORY
		ZEDS_ASSERT(!_CrtCheckMemory(), "Heap problem");
	#endif

}
