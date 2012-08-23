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
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stdarg.h>

#ifdef ZE_PLATFORM_WINDOWS
    #include <mbstring.h>
	#include <windows.h>

#elif defined ZE_PLATFORM_UNIX

#include <iconv.h>
#include <wctype.h>
#include <errno.h>

#endif

#ifdef ZE_PLATFORM_COMPILER_GCC
	#define _snprintf snprintf
#endif

#include "ZEError.h"
#include "ZEEndian.h"
#include "ZEFormatArgument.h"
#include "ZEStringWriter.h"


ZEUInt ZECharacter::GetByteLength(const char* MultiByteCharacter)
{
	char LengthByte = MultiByteCharacter[0];
	ZEUInt Length = 0;

	if(!(LengthByte & (1 << 7)))
		return 1;

	for(ZEInt I = 7; I > 3; I--)
	{
		if(LengthByte & (1 << I))
			Length++;
	}

	return Length;
}

void ZECharacter::SetValue(char Character)
{
	this->Characters[0] = Character;
	Size = 1;

	if (Owner != NULL)
		Owner->SetCharacter(Position, this->Characters);
}

void ZECharacter::SetValue(const char* MultiByteCharacter)
{
	Size = ZECharacter::GetByteLength(MultiByteCharacter);

	for(ZEUInt I = 0; I < Size; I++)
	{
		Characters[I] = *MultiByteCharacter;
		MultiByteCharacter++;
	}

	if (Owner != NULL)
		Owner->SetCharacter(Position, this->Characters);
}

void ZECharacter::SetValue(const wchar_t* WideCharacter)
{
	#ifdef ZE_PLATFORM_WINDOWS

	int RequiredSize = WideCharToMultiByte(CP_UTF8, 0, WideCharacter, -1, 0, 0, 0, 0) - 1;

	if(RequiredSize > 0)
	{
		WideCharToMultiByte(CP_UTF8, 0, WideCharacter, -1, &Characters[0], RequiredSize, 0, 0);
		Size = RequiredSize;
	}

	#elif defined ZE_PLATFORM_UNIX

	ZESize WideCharacterByteCount = wcslen(WideCharacter) * sizeof(wchar_t);
	char* InputBuffer = (char*)WideCharacter;
	char* OutputBuffer = &Characters[0];

	if(WideCharacterByteCount > 0)
	{
		ZESize CharacterByteCount = sizeof(Characters);
		iconv_t ConversionDefinition = iconv_open("UTF-8", "WCHAR_T");
		iconv(ConversionDefinition, &InputBuffer, &WideCharacterByteCount, &OutputBuffer, &CharacterByteCount);
		iconv_close(ConversionDefinition);
		Size = sizeof(Characters) - CharacterByteCount;
	}

	#endif

	if (Owner != NULL)
		Owner->SetCharacter(Position, this->Characters);
}

void ZECharacter::SetValue(ZEInt Character)
{
	ZEInt ConvertedCharacter = ZEEndian::Big(Character);

	char* Temp = (char*)&ConvertedCharacter;

	ZEInt I = 0;
	ZEInt J = 0;
	Size = 0;

	while(I < sizeof(ZEInt))
	{
		if(*Temp != NULL)
		{
			this->Characters[J] = *Temp;
			Size++;
			J++;
		}

		Temp++;
		I++;
	}
}

void ZECharacter::SetOwner(ZEString* Owner, ZESize Index)
{
	this->Owner = Owner;
	this->Position = Index;
}

const char* ZECharacter::GetValue() const
{
	return Characters;
}

ZEUInt ZECharacter::GetSize() const
{
	return Size;
}

bool ZECharacter::Equals(const ZECharacter& Character) const
{
	if (this->Size == Character.Size)
	{
		bool Result = true;

		for (ZEUInt I = 0; I < Size; I++)
		{
			if (this->Characters[I] != Character.Characters[I])
				Result = false;
		}

		return Result;
	}

	return false;
}

bool ZECharacter::Equals(const char& Character) const
{
	if (Size == 1)
	{
		if (this->Characters[0] == Character)
			return true;
	}

	return false;
}

bool ZECharacter::Equals(const char* MultiByteCharacter) const
{
	ZEUInt MultiByteLength = ZECharacter::GetByteLength(MultiByteCharacter);

	if (Size == MultiByteLength)
	{
		bool Result = true;
		for (ZEUInt I = 0; I < Size; I++)
		{
			if (Characters[I] != *MultiByteCharacter)
				Result = false;

			MultiByteCharacter++;
		}

		return Result;
	}

 	return false;
}

bool ZECharacter::Equals(const wchar_t* WideCharacter) const
{
	ZECharacter Temp(WideCharacter);

	return Equals(Temp);
}

ZECharacter ZECharacter::Upper() const
{
	wchar_t WideBuffer[2] = {0};

	#ifdef ZE_PLATFORM_WINDOWS

	MultiByteToWideChar(CP_UTF8, 0, Characters, Size, WideBuffer, 2);

	#elif defined ZE_PLATFORM_UNIX

	char* OutputBuffer = (char*)WideBuffer;
	ZESize WideCharacterSize = sizeof(wchar_t);

	char* InputBuffer = &Characters[0];
	ZESize CharacterSize = Size;

	iconv_t ConversionDefinition = iconv_open("WCHAR_T", "UTF-8");
	iconv(ConversionDefinition, &InputBuffer, &CharacterSize, &OutputBuffer, &WideCharacterSize);
	iconv_close(ConversionDefinition);

	#endif

	if(WideBuffer[1] != NULL)
		return ZECharacter(this->GetValue());

	ZEInt IntValForWideChar = *WideBuffer;
	ZEInt UpperCaseWideChar = towupper(IntValForWideChar);

	return ZECharacter((wchar_t*)&UpperCaseWideChar);
}

void ZECharacter::UpperSelf()
{
	wchar_t WideBuffer[2] = {0};

	#ifdef ZE_PLATFORM_WINDOWS

	MultiByteToWideChar(CP_UTF8, 0, Characters, Size, WideBuffer, 2);

	#elif defined ZE_PLATFORM_UNIX

	char* OutputBuffer = (char*)WideBuffer;
	ZESize WideCharacterSize = 4;

	char* InputBuffer = &Characters[0];
	ZESize CharacterSize = Size;

	iconv_t ConversionDefinition = iconv_open("WCHAR_T", "UTF-8");
	iconv(ConversionDefinition, &InputBuffer, &CharacterSize, &OutputBuffer, &WideCharacterSize);
	iconv_close(ConversionDefinition);

	#endif

	if(WideBuffer[1] != NULL)
		return;

	ZEInt IntValForWideChar = *WideBuffer;
	ZEInt UpperCaseWideChar = towupper(IntValForWideChar);

	SetValue((wchar_t*)&UpperCaseWideChar);
}

ZECharacter ZECharacter::Lower() const
{
	wchar_t WideBuffer[2] = {0};

	#ifdef ZE_PLATFORM_WINDOWS

	MultiByteToWideChar(CP_UTF8, 0, Characters, Size, WideBuffer, 2);

	#elif defined ZE_PLATFORM_UNIX

	char* OutputBuffer = (char*)WideBuffer;
	ZESize WideCharacterSize = 4;

	char* InputBuffer = &Characters[0];
	ZESize CharacterSize = Size;

	iconv_t ConversionDefinition = iconv_open("WCHAR_T", "UTF-8");
	iconv(ConversionDefinition, &InputBuffer, &CharacterSize, &OutputBuffer, &WideCharacterSize);
	iconv_close(ConversionDefinition);

	#endif

	if(WideBuffer[1] != NULL)
		return ZECharacter(this->GetValue());

	ZEInt IntValForWideChar = *WideBuffer;
	ZEInt LowerCaseWideChar = towlower(IntValForWideChar);

	return ZECharacter((wchar_t*)&LowerCaseWideChar);
}

void ZECharacter::LowerSelf()
{
	wchar_t WideBuffer[2] = {0};

	#ifdef ZE_PLATFORM_WINDOWS

	MultiByteToWideChar(CP_UTF8, 0, Characters, Size, WideBuffer, 2);

	#elif defined ZE_PLATFORM_UNIX

	char* OutputBuffer = (char*)WideBuffer;
	ZESize WideCharacterSize = 4;

	char* InputBuffer = &Characters[0];
	ZESize CharacterSize = Size;

	iconv_t ConversionDefinition = iconv_open("WCHAR_T", "UTF-8");
	iconv(ConversionDefinition, &InputBuffer, &CharacterSize, &OutputBuffer, &WideCharacterSize);
	iconv_close(ConversionDefinition);

	#endif

	if(WideBuffer[1] != NULL)
		return;

	ZEInt IntValForWideChar = *WideBuffer;
	ZEInt LowerCaseWideChar = towlower(IntValForWideChar);

	SetValue((wchar_t*)&LowerCaseWideChar);
}

ZECharacter ZECharacter::FromChar(const char& Value)
{
	return Value;
}

ZECharacter ZECharacter::FromMBChar(const char* Value)
{
	return Value;
}

ZECharacter ZECharacter::FromWChar(const wchar_t* Value)
{
	return Value;
}

ZECharacter& ZECharacter::operator=(const ZECharacter& Character)
{
	SetValue(Character.GetValue());
	return *this;
}

ZECharacter& ZECharacter::operator=(const char& Character)
{
	SetValue(Character);
	return *this;
}

ZECharacter& ZECharacter::operator=(const char* MultiByteCharacter)
{
	SetValue(MultiByteCharacter);
	return *this;
}

ZECharacter& ZECharacter::operator=(const wchar_t* WideCharacter)
{
	SetValue(WideCharacter);
	return *this;
}

bool ZECharacter::operator!=(const ZECharacter& Character) const
{
	return !Equals(Character);
}

bool ZECharacter::operator!=(const char& Character) const
{
	return !Equals(Character);
}

bool ZECharacter::operator!=(const char* MultiByteCharacter) const
{
	return !Equals(MultiByteCharacter);
}

bool ZECharacter::operator!=(const wchar_t* WideCharacter) const
{
	return !Equals(WideCharacter);
}

bool ZECharacter::operator==(const ZECharacter& Character) const
{
	return Equals(Character);
}

bool ZECharacter::operator==(const char& Character) const
{
	return Equals(Character);
}

bool ZECharacter::operator==(const char* MultiByteCharacter) const
{
	return Equals(MultiByteCharacter);
}

bool ZECharacter::operator==(const wchar_t* WideCharacter) const
{
	return Equals(WideCharacter);
}

ZECharacter::operator char() const
{
	return Characters[0];
}

ZECharacter::operator const char*() const
{
	return &Characters[0];
}

ZECharacter::operator wchar_t() const
{
	wchar_t Temp;

	#ifdef ZE_PLATFORM_WINDOWS

	MultiByteToWideChar(CP_UTF8, 0, Characters, Size, &Temp, 1);

	#elif defined ZE_PLATFORM_UNIX

	char* OutputBuffer = (char*)&Temp;
	ZESize WideCharacterSize = 4;

	char* InputBuffer = &Characters[0];
	ZESize CharacterSize = Size;

	iconv_t ConversionDefinition = iconv_open("WCHAR_T", "UTF-8");
	iconv(ConversionDefinition, &InputBuffer, &CharacterSize, &OutputBuffer, &WideCharacterSize);
	iconv_close(ConversionDefinition);

	#endif

	return Temp;
}

ZECharacter::ZECharacter()
{
	Size = 0;
	Owner = NULL;
	Position = 0;
}

ZECharacter::ZECharacter(const ZECharacter& Character)
{
	Size = 0;
	Owner = NULL;
	Position = 0;

	SetValue(Character.GetValue());
	Owner = Character.Owner;
	Position = Character.Position;
}

ZECharacter::ZECharacter(char Character)
{
	Size = 0;
	Owner = NULL;
	Position = 0;

	SetValue(Character);
}

ZECharacter::ZECharacter(const char* MultiByteCharacter)
{
	Size = 0;
	Owner = NULL;
	Position = 0;

	SetValue(MultiByteCharacter);
}

ZECharacter::ZECharacter(const wchar_t* WideCharacter)
{
	Size = 0;
	Owner = NULL;
	Position = 0;

	SetValue(WideCharacter);
}

ZECharacter::~ZECharacter()
{
	Owner = NULL;
}

bool operator==(const char& Character1, const ZECharacter& Character2)
{
	return Character2.Equals(Character1);
}

bool operator==(const char* MultiByteCharacter, const ZECharacter& Character)
{
	return Character.Equals(MultiByteCharacter);
}

bool operator==(const wchar_t* WideCharacter, const ZECharacter& Character)
{
	return Character.Equals(WideCharacter);
}

bool operator!=(const char& Character1, const ZECharacter& Character2)
{
	return !Character2.Equals(Character1);
}

bool operator!=(const char* MultiByteCharacter, const ZECharacter& Character)
{
	return !Character.Equals(MultiByteCharacter);
}

bool operator!=(const wchar_t* WideCharacter, const ZECharacter& Character)
{
	return !Character.Equals(WideCharacter);
}



#ifdef ZE_PLATFORM_UNIX

ZESize CalculateRequiredSize(const char* String)
{
	char TempBuffer[512];
	char* InputBuffer = (char*)String;
	ZESize InputSize = strlen(String);
	char* TempOutput = &TempBuffer[0];
	ZESize TempSize = 512;

	ZESize ConvertedCharacterCount;
	ZESize ResultSize = 0;


	iconv_t ConversionDescriptor = iconv_open("WCHAR_T", "UTF-8");

	while(InputSize > 0)
	{
		ConvertedCharacterCount = iconv(ConversionDescriptor, &InputBuffer, &InputSize, &TempOutput, &TempSize);

		if (ConvertedCharacterCount == (ZESize)-1)
		{
			if(errno != E2BIG)
				break;

			ResultSize += 512 - TempSize;
			TempOutput = &TempBuffer[0];
		}
	}

	iconv_close(ConversionDescriptor);

	ResultSize += 512 - TempSize;
	ResultSize /= sizeof(wchar_t);

	return ResultSize;
}


ZESize CalculateRequiredSizeW(const wchar_t* WideString)
{
	char TempBuffer[512];
	char* InputString = (char*)WideString;
	ZESize InputSize = wcslen(WideString) * sizeof(wchar_t);
	char* TempOutput = &TempBuffer[0];
	ZESize TempSize = 512;

	ZESize ConvertedCharacterCount;
	ZESize ResultSize = 0;


	iconv_t ConversionDescriptor = iconv_open("UTF-8", "WCHAR_T");

	while (InputSize > 0)
	{
		ConvertedCharacterCount = iconv(ConversionDescriptor, &InputString, &InputSize, &TempOutput, &TempSize);

		if (ConvertedCharacterCount == (ZESize)-1)
		{
			if (errno != E2BIG)
				break;

			ResultSize += 512 - TempSize;
			TempOutput = &TempBuffer[0];
		}
	}

	iconv_close(ConversionDescriptor);

	ResultSize += 512 - TempSize;

	return ResultSize;
}

#endif


ZESize ZEString::GetBytePosition(const char* String, ZESize CharacterPosition)
{
	ZESize CharacterSize = 0;
	ZESize CharacterCount = 0;
	ZESize ByteCount = 0;
	const char* TempBuffer = String;

	while (TempBuffer != NULL)
	{
		if (CharacterCount == CharacterPosition)
			break;

		CharacterSize = ZECharacter::GetByteLength(TempBuffer);
		TempBuffer = TempBuffer + CharacterSize;
		ByteCount += CharacterSize;
		CharacterCount++;
	}

	return ByteCount;
}

char* ZEString::IncrementByCharacter(const char* Position)
{
	ZESize CharacterSize = ZECharacter::GetByteLength(Position);

	return (char*)(Position + CharacterSize);
}

char* ZEString::DecrementByCharacter(const char* Start, const char* Position)
{
	const char* PrevLeadByte = Position;

	while (Start != PrevLeadByte)
	{
		PrevLeadByte = PrevLeadByte - 1;

		if(*PrevLeadByte & 1 << 7)
		{
			if(*PrevLeadByte & 1 << 6)
				return (char*)PrevLeadByte;
		}
		else
		{
			return (char*)PrevLeadByte;
		}
	}

	return (char*)PrevLeadByte;
}

bool ZEString::IsEmpty() const
{
	return (Buffer == NULL || (Buffer[0] == '\0'));
}

void ZEString::SetSize(ZESize Size)
{
	Allocator.Reallocate(&Buffer, Size);
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

void ZEString::SetBuffer(void* Buffer, ZESize Size)
{
	BufferChanged = true;

	Clear();
	this->Buffer = (char*)Buffer;
	this->Buffer[Size - 1] = '\0';
	this->Allocator.Size = Size;
}

void ZEString::SetValue(void* Value, ZESize Size)
{
	BufferChanged = true;
	this->Allocator.Reallocate(&Buffer, Size + 1);
	memcpy(Buffer, Value, Size);
	Buffer[Size] = '\0';
}

void ZEString::SetValue(const ZEString& String)
{
	SetValue(String.Buffer);
}

void ZEString::SetValue(const char* String)
{
	BufferChanged = true;

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
	BufferChanged = true;

	#ifdef ZE_PLATFORM_WINDOWS

	ZESize Size = WideCharToMultiByte(CP_UTF8, 0, &Character, 1, 0, 0, 0, 0);

	char* OldBuffer = Buffer;
	if (Allocator.Allocate(&Buffer, Size + 1))
		delete[] OldBuffer;

	WideCharToMultiByte(CP_UTF8, 0, &Character, 1, Buffer, Size + 1, 0, 0);

	Buffer[Size] = '\0';

	#elif defined ZE_PLATFORM_UNIX

	wchar_t TempBuffer[2] = {Character, 0};
	char* InputBuffer = (char*)(&TempBuffer[0]);
	ZESize WideCharacterSize = sizeof(wchar_t);

	ZESize RequiredSize = CalculateRequiredSizeW(TempBuffer);

	char* OldBuffer = Buffer;
	if (Allocator.Allocate(&Buffer, RequiredSize + 1))
		delete[] OldBuffer;

    char* OutputBuffer = Buffer;
	ZESize OutputSize = RequiredSize;
	iconv_t ConversionDefinition = iconv_open("UTF-8", "WCHAR_T");
	iconv(ConversionDefinition, &InputBuffer, &WideCharacterSize, &OutputBuffer, &OutputSize);
	iconv_close(ConversionDefinition);

	Buffer[RequiredSize] = '\0';

	#endif
}

void ZEString::SetValue(const wchar_t* String)
{
	BufferChanged = true;

	#ifdef ZE_PLATFORM_WINDOWS

	ZESize Size = WideCharToMultiByte(CP_UTF8, 0, String, -1, 0, 0, 0, 0);

	char* OldBuffer = Buffer;
	if (Allocator.Allocate(&Buffer, Size))
		delete[] OldBuffer;

	WideCharToMultiByte(CP_UTF8, 0, String, -1, Buffer, Size, 0, 0);

	#elif defined ZE_PLATFORM_UNIX

	char* InputBuffer = (char*)String;
	ZESize WideCharacterSize = wcslen(String) * sizeof(wchar_t);

	ZESize RequiredSize = CalculateRequiredSizeW(String);

	char* OldBuffer = Buffer;
	if (Allocator.Allocate(&Buffer, RequiredSize + 1))
		delete[] OldBuffer;

    char* OutputBuffer = Buffer;
	ZESize OutputSize = RequiredSize;
	iconv_t ConversionDefinition = iconv_open("UTF-8", "WCHAR_T");
	iconv(ConversionDefinition, &InputBuffer, &WideCharacterSize, &OutputBuffer, &OutputSize);
	iconv_close(ConversionDefinition);

	Buffer[RequiredSize] = '\0';

	#endif
}

void ZEString::SetValue(const std::string& String)
{
	SetValue(String.c_str());
}

void ZEString::SetValue(const std::wstring& String)
{
	SetValue(String.c_str());
}

void ZEString::SetValue(const ZECharacter& Character)
{
	BufferChanged = true;
	ZESize Size = Character.GetSize() * sizeof(char);

	char* OldBuffer = Buffer;
	if (Allocator.Allocate(&Buffer, Size + 1))
		delete[] OldBuffer;

	memcpy(Buffer, Character.GetValue(), Size);

	Buffer[Size] = '\0';

	ZEDebugCheckMemory();
}

void ZEString::SetValue(ZEInt8 Value, const char* Format)
{
	ZEStringWriter Writer;
	ZEFormatArgument::Format(Writer, (ZEInt64)Value, Format);
	Writer.Output(*this);
}

void ZEString::SetValue(ZEInt16 Value, const char* Format)
{
	ZEStringWriter Writer;
	ZEFormatArgument::Format(Writer, (ZEInt64)Value, Format);
	Writer.Output(*this);
}

void ZEString::SetValue(ZEInt32 Value, const char* Format)
{
	ZEStringWriter Writer;
	ZEFormatArgument::Format(Writer, (ZEInt64)Value, Format);
	Writer.Output(*this);
}

void ZEString::SetValue(ZEInt64 Value, const char* Format)
{
	ZEStringWriter Writer;
	ZEFormatArgument::Format(Writer, (ZEInt64)Value, Format);
	Writer.Output(*this);
}

void ZEString::SetValue(ZEUInt8 Value, const char* Format)
{
	ZEStringWriter Writer;
	ZEFormatArgument::Format(Writer, (ZEUInt64)Value, Format);
	Writer.Output(*this);
}

void ZEString::SetValue(ZEUInt16 Value, const char* Format)
{
	ZEStringWriter Writer;
	ZEFormatArgument::Format(Writer, (ZEUInt64)Value, Format);
	Writer.Output(*this);
}

void ZEString::SetValue(ZEUInt32 Value, const char* Format)
{
	ZEStringWriter Writer;
	ZEFormatArgument::Format(Writer, (ZEUInt64)Value, Format);
	Writer.Output(*this);
}

void ZEString::SetValue(ZEUInt64 Value, const char* Format)
{
	ZEStringWriter Writer;
	ZEFormatArgument::Format(Writer, (ZEUInt64)Value, Format);
	Writer.Output(*this);
}

void ZEString::SetValue(float Value, ZEUInt Digits)
{
	char Format[100];
	_snprintf(Format, 100, "%%.%df", Digits);
	char Buffer[100];
	_snprintf(Buffer, 100, Format, Value);
	SetValue(Buffer);
}

void ZEString::SetValue(float Value, const char* Format)
{
	ZEStringWriter Writer;
	ZEFormatArgument::Format(Writer, Value, Format);
	Writer.Output(*this);
}

void ZEString::SetValue(double Value, ZEUInt Digits)
{
	char Format[100];
	_snprintf(Format, 100, "%%.%df", Digits);
	char Buffer[100];
	_snprintf(Buffer, 100, Format, Value);
	SetValue(Buffer);
}

void ZEString::SetValue(double Value, const char* Format)
{
	ZEStringWriter Writer;
	ZEFormatArgument::Format(Writer, (ZEUInt64)Value, Format);
	Writer.Output(*this);
}

void ZEString::SetValue(bool Value, const char* Format)
{
	ZEStringWriter Writer;
	ZEFormatArgument::Format(Writer, (ZEUInt64)Value, Format);
	Writer.Output(*this);
}

const char* ZEString::GetValue() const
{
	if (Buffer == NULL)
		return "";
	else
		return Buffer;
}

ZECharacter ZEString::GetCharacter(ZESize Position) const
{
	zeDebugCheck(Position > this->GetLength(), "Position parameter value exceed length of the string.");

	ZESize BytePosition = ZEString::GetBytePosition(Buffer, Position);

	return Buffer + BytePosition;
}

void ZEString::SetCharacter(ZESize Position, ZECharacter Value)
{
	zeDebugCheck(Position > this->GetLength(), "Position parameter value exceed length of the string.");

	Remove(Position);
	Insert(Position, ZEString(Value));
}

ZESize ZEString::GetLength() const
{
	if (Buffer != NULL)
	{
		ZESize CharacterSize = 0;
		ZESize StringLength = 0;
		const char* TempBuffer = Buffer;

		while (*TempBuffer != NULL)
		{
			CharacterSize = ZECharacter::GetByteLength(TempBuffer);
			TempBuffer = TempBuffer + CharacterSize;
			StringLength++;
		}

		return StringLength;
	}
	else
		return 0;

}

void ZEString::Append(const char* String)
{
	zeDebugCheck(String == NULL, "String parameter is invalid.");

	if (Buffer == NULL)
		SetValue(String);
	else
	{
		BufferChanged = true;
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

		BufferChanged = true;

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
	zeDebugCheck(String == NULL, "String parameter is invalid.");
	zeDebugCheck(Buffer == NULL && Position != 0, "A string can only be inserted in to empty string at position 0.");

	ZESize StringByteLength = strlen(String);
	if (StringByteLength == 0)
		return;

	if (Buffer == NULL && Position == 0)
	{
		SetValue(String);
		return;
	}

	zeDebugCheck(Position > this->GetLength(), "Position parameter is more than string length.");

	BufferChanged = true;

	ZESize BufferByteLength = strlen(Buffer);
	BufferByteLength += StringByteLength;
	ZESize BytePosition = ZEString::GetBytePosition(Buffer, Position);

	char* OldBuffer = Buffer;
	// Check whether allocator allocated new memory location (returned true) or uses same memory location (returned false).
	if (Allocator.Allocate(&Buffer, BufferByteLength * sizeof(char) + 1))
	{

		memcpy(Buffer, OldBuffer, BytePosition * sizeof(char));
		memcpy(Buffer + BytePosition, String, StringByteLength * sizeof(char));
		memcpy(Buffer + BytePosition + StringByteLength, OldBuffer + BytePosition, (BufferByteLength - BytePosition - StringByteLength + 1) * sizeof(char));

		// If allocator allocated new memory location we should delete old memory location.
		delete[] OldBuffer;
	}
	else
	{
		memcpy(Buffer + BytePosition, String, StringByteLength * sizeof(char));
		memcpy(Buffer + BytePosition + StringByteLength, Buffer + BytePosition, (BufferByteLength - BytePosition - StringByteLength + 1) * sizeof(char));
	}

	ZEDebugCheckMemory();
}

void ZEString::Remove(ZESize Position, ZESize Count)
{

	if (Buffer == NULL && Position == 0 && Count == 0)
		return;

	ZESize ByteLength = strlen(Buffer);
	ZESize BytePosition = ZEString::GetBytePosition(Buffer, Position);
	ZESize ByteCount = ZEString::GetBytePosition(Buffer + BytePosition, Count);
	/*ZESize ByteCount = _mbsnbcnt((unsigned char*)Buffer, Position + Count) - BytePosition;*/


	zeDebugCheck(BytePosition > ByteLength, "Position parameter is more than string length.");
	zeDebugCheck(BytePosition + ByteCount > ByteLength, "Remove operation range (Position + Count) exceeds length of the string.");

	if (ByteLength - ByteCount == 0)
	{
		Clear();
		return;
	}

	BufferChanged = true;

	char* OldBuffer = Buffer;
	if (Allocator.Allocate(&Buffer, (ByteLength - ByteCount + 1) * sizeof(char)))
	{
		memcpy(Buffer, OldBuffer, BytePosition * sizeof(char));
		memcpy(Buffer + BytePosition, OldBuffer + BytePosition + ByteCount, (ByteLength - BytePosition - ByteCount + 1) * sizeof(char));
		delete[] OldBuffer;
	}
	else
	{
		memcpy(Buffer + BytePosition, Buffer + BytePosition + ByteCount, (ByteLength - BytePosition - ByteCount + 1) * sizeof(char));
	}

	ZEDebugCheckMemory();
}

bool ZEString::Equals(const ZEString& String) const
{
	return Equals(String.Buffer);
}

bool ZEString::Equals(const char* String) const
{
 	if (this->Buffer == String) // NULL Check
 		return true;

 	if (String == NULL)
		return false;

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

	ZESize ByteCount = ZEString::GetBytePosition(Buffer, Count);

	zeDebugCheck(Buffer == NULL, "Buffer is empty.");
	zeDebugCheck(ByteCount >  strlen(Buffer), "Position is bigger than string length.");

	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (ByteCount + 1) * sizeof(char));
	memcpy(Temp.Buffer, Buffer, ByteCount * sizeof(char));
	Temp.Buffer[ByteCount] = '\0';

	ZEDebugCheckMemory();

	return Temp;
}

ZEString ZEString::Right(ZESize Count) const
{
	if (Count == 0)
		return ZEString();

	zeDebugCheck(Buffer == NULL, "Buffer is empty.");

	ZESize ByteLength = strlen(Buffer);

	char* CountBuffer = Buffer + ByteLength;

	for (ZEUInt I = 0; I < Count; I++)
		CountBuffer = ZEString::DecrementByCharacter(Buffer, CountBuffer);

	ZESize ByteCount = ZEString::GetBytePosition(CountBuffer, Count);

	zeDebugCheck(ByteCount > ByteLength, "Position is bigger than string length.");

	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (ByteCount + 1) * sizeof(char));
	memcpy(Temp.Buffer, Buffer + ByteLength - ByteCount, (ByteCount + 1) * sizeof(char));

	ZEDebugCheckMemory();

	return Temp;
}

ZEString ZEString::Middle(ZESize Position, ZESize Count) const
{
	if (Count == 0)
		return ZEString();

	zeDebugCheck(Buffer == NULL, "Buffer is empty.");

	ZESize BytePosition = ZEString::GetBytePosition(Buffer, Position);
	ZESize ByteCount = ZEString::GetBytePosition(Buffer + BytePosition, Count);

	zeDebugCheck(BytePosition + ByteCount > strlen(Buffer), "Sub string range (Position and count) is exceed string length.");

	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (ByteCount + 1) * sizeof(char));
	memcpy(Temp.Buffer, Buffer + BytePosition, ByteCount * sizeof(char));
	Temp.Buffer[ByteCount] = '\0';

	ZEDebugCheckMemory();

	return Temp;
}

ZEString ZEString::SubString(ZESize StartPosition, ZESize EndPosition) const
{
	if (Buffer == NULL && StartPosition == 0 && EndPosition == 0)
		return ZEString();

	zeDebugCheck(Buffer == NULL, "Buffer is empty.");

	ZESize StartBytePosition = ZEString::GetBytePosition(Buffer, StartPosition);
	ZESize EndBytePosition = ZEString::GetBytePosition(Buffer, EndPosition);
	ZESize EndByteLength = ZECharacter::GetByteLength(Buffer + EndBytePosition);
	EndBytePosition += EndByteLength - 1;

	zeDebugCheck(StartBytePosition > strlen(Buffer) || EndBytePosition > strlen(Buffer), "Sub string range (Position and count) is exceed string length.");
	zeDebugCheck(EndBytePosition < StartBytePosition, "EndPosition parameter can not be smaller than StartPosition parameter.");

	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (EndBytePosition - StartBytePosition + 2) * sizeof(char));
	memcpy(Temp.Buffer, Buffer + StartBytePosition, (EndBytePosition - StartBytePosition + 1) * sizeof(char));
	Temp.Buffer[EndBytePosition - StartBytePosition + 1] = '\0';

	ZEDebugCheckMemory();

	return Temp;
}

ZEString ZEString::TrimLeft() const
{
	if (Buffer == NULL)
		return ZEString();

	ZESize ByteLength = strlen(Buffer);

	char* Cursor = Buffer;
	while(*Cursor != '\0' && (*Cursor == ' ' || *Cursor == '\t'))
		Cursor = ZEString::IncrementByCharacter(Cursor);

	ZESize ByteCount = Cursor - Buffer;
	if (ByteCount == ByteLength)
		return ZEString();

	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (ByteLength - ByteCount + 1) * sizeof(char));
	memcpy(Temp.Buffer, Buffer + ByteCount, (ByteLength - ByteCount + 1) * sizeof(char));

	ZEDebugCheckMemory();

	return Temp;
}

void ZEString::TrimLeftSelf()
{
	if (Buffer == NULL)
		return;

	BufferChanged = true;

	ZESize ByteLength = strlen(Buffer);

	char* Cursor = Buffer;
	while(*Cursor != '\0' && (*Cursor == ' ' || *Cursor == '\t'))
		Cursor = ZEString::IncrementByCharacter(Cursor);

	ZESize ByteCount = Cursor - Buffer;

	if (ByteCount == ByteLength)
	{
		Clear();
		return;
	}

	memmove(Buffer, Buffer + ByteCount, (ByteLength - ByteCount + 1) * sizeof(char));
	Allocator.Reallocate(&Buffer, (ByteLength - ByteCount + 1) * sizeof(char));
	Buffer[ByteLength - ByteCount] = '\0';

	ZEDebugCheckMemory();
}

ZEString ZEString::TrimRight() const
{
	if (Buffer == NULL)
		return ZEString();

	ZESize ByteLength = strlen(Buffer);
	char* Cursor = Buffer + (ByteLength - 1);

	while(Cursor != Buffer && (*Cursor == ' ' || *Cursor == '\t'))
		Cursor = ZEString::DecrementByCharacter(Buffer, Cursor);

	ZESize CursorSize = ZECharacter::GetByteLength(Cursor);
	Cursor += CursorSize - 1;

	if(Cursor == Buffer && (*Cursor == ' ' || *Cursor == '\t'))
		return ZEString();

	ZESize ByteCount = (ByteLength) - ((Cursor + 1) - Buffer);

	if (ByteCount == ByteLength)
		return ZEString();

	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (ByteLength - ByteCount + 1) * sizeof(char));
	memcpy(Temp.Buffer, Buffer, (ByteLength - ByteCount) * sizeof(char));
	Temp.Buffer[ByteLength - ByteCount] = '\0';

	ZEDebugCheckMemory();

	return Temp;
}

void ZEString::TrimRightSelf()
{
	if (Buffer == NULL)
		return;

	BufferChanged = true;

	ZESize ByteLength = strlen(Buffer);
	char* Cursor = Buffer + (ByteLength - 1);

	while(Cursor != Buffer && (*Cursor == ' ' || *Cursor == '\t'))
		Cursor = ZEString::DecrementByCharacter(Buffer, Cursor);

	ZESize CursorSize = ZECharacter::GetByteLength(Cursor);
	Cursor += CursorSize - 1;

	if(Cursor == Buffer && (*Cursor == ' ' || *Cursor == '\t'))
	{
		Clear();
		return;
	}

	ZESize ByteCount = ByteLength - ((Cursor + 1) - Buffer);

	if (ByteCount == ByteLength)
	{
		Clear();
		return;
	}

	memmove(Buffer, Buffer, (ByteLength - ByteCount + 1) * sizeof(char));
	Allocator.Reallocate(&Buffer, (ByteLength - ByteCount + 1) * sizeof(char));
	Buffer[ByteLength - ByteCount] = '\0';

	ZEDebugCheckMemory();
}

ZEString ZEString::Trim() const
{
	if (Buffer == NULL)
		return ZEString();

	ZESize ByteLength = strlen(Buffer);

	char* Cursor = Buffer;
	while(*Cursor != '\0' && (*Cursor == ' ' || *Cursor == '\t'))
		Cursor = ZEString::IncrementByCharacter(Cursor);

	ZESize LeftByteCount = Cursor - Buffer;

	if (LeftByteCount == ByteLength)
		return ZEString();

	Cursor = Buffer + (ByteLength - 1);
	while(Cursor != Buffer && (*Cursor == ' ' || *Cursor == '\t'))
		Cursor = ZEString::DecrementByCharacter(Buffer, Cursor);

	ZESize CursorSize = ZECharacter::GetByteLength(Cursor);
	Cursor += CursorSize - 1;

	if(Cursor == Buffer && (*Cursor == ' ' || *Cursor == '\t'))
		return ZEString();

	ZESize RightByteCount = ByteLength - ((Cursor + 1) - Buffer);

	if (RightByteCount + LeftByteCount == ByteLength)
		return ZEString();

	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (ByteLength - LeftByteCount - RightByteCount + 1) * sizeof(char));
	memcpy(Temp.Buffer, Buffer + LeftByteCount, (ByteLength - LeftByteCount - RightByteCount) * sizeof(char));
	Temp.Buffer[ByteLength - LeftByteCount - RightByteCount] = '\0';

	ZEDebugCheckMemory();

	return Temp;
}

void ZEString::TrimSelf()
{
	if (Buffer == NULL)
		return;

	BufferChanged = true;

	ZESize ByteLength = strlen(Buffer);

	char* Cursor = Buffer;
	while(*Cursor != '\0' && (*Cursor == ' ' || *Cursor == '\t'))
		Cursor = ZEString::IncrementByCharacter(Cursor);

	ZESize LeftByteCount = Cursor - Buffer;

	if (LeftByteCount == ByteLength)
	{
		Clear();
		return;
	}

	Cursor = Buffer + (ByteLength - 1);
	while(Cursor != Buffer && (*Cursor == ' ' || *Cursor == '\t'))
		Cursor = ZEString::DecrementByCharacter(Buffer, Cursor);

	ZESize CursorSize = ZECharacter::GetByteLength(Cursor);
	Cursor += CursorSize - 1;

	if(Cursor == Buffer && (*Cursor == ' ' || *Cursor == '\t'))
	{
		Clear();
		return;
	}

	ZESize RightByteCount = ByteLength - ((Cursor + 1) - Buffer);

	if (RightByteCount + LeftByteCount == ByteLength)
		return;

	memmove(Buffer, Buffer + LeftByteCount, (ByteLength - LeftByteCount - RightByteCount) * sizeof(char));
	Allocator.Reallocate(&Buffer, (ByteLength - LeftByteCount - RightByteCount + 1) * sizeof(char));
	Buffer[ByteLength - LeftByteCount - RightByteCount] = '\0';

	ZEDebugCheckMemory();
}

ZEString ZEString::Lower() const
{
	if (Buffer == NULL)
		return ZEString();

	ZESize ByteLength = strlen(Buffer);

	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (ByteLength + 1) * sizeof(char));
	Temp = Buffer;

	#ifdef ZE_PLATFORM_WINDOWS

	ZESize RequiredSizeforWide = MultiByteToWideChar(CP_UTF8, 0, Temp, -1, 0, 0);
	wchar_t* WideTemp = new wchar_t[RequiredSizeforWide];
	MultiByteToWideChar(CP_UTF8, 0, Temp, -1, WideTemp, RequiredSizeforWide);
	WideTemp = _wcslwr(WideTemp);

	ZESize RequiredSizeforResult = WideCharToMultiByte(CP_UTF8, 0, WideTemp, -1, 0, 0, 0, 0);
	Temp.Allocator.Reallocate(&Temp.Buffer, RequiredSizeforResult * sizeof(char));
	WideCharToMultiByte(CP_UTF8, 0, WideTemp, -1, Temp.Buffer, RequiredSizeforResult, 0, 0);
	delete[] WideTemp;

	#elif defined ZE_PLATFORM_UNIX

	ZESize RequiredSizeforWideArray = CalculateRequiredSize(Temp);
	ZESize RequiredSizeforWide = RequiredSizeforWideArray * sizeof(wchar_t);
	wchar_t* WideTemp = new wchar_t[RequiredSizeforWideArray + 1];
	char* WideBuffer = (char*)WideTemp;
    char* ResultBuffer = Temp.Buffer;

	ZESize InputSize = ByteLength;
	ZESize OutputSize = RequiredSizeforWide;
	iconv_t ConversionDescriptor = iconv_open("WCHAR_T", "UTF-8");
	iconv(ConversionDescriptor, &ResultBuffer, &InputSize, &WideBuffer, &OutputSize);
	iconv_close(ConversionDescriptor);

	WideTemp[RequiredSizeforWideArray] = L'\0';

	for (ZEUInt I = 0; I < RequiredSizeforWideArray; I++)
        WideTemp[I] = towlower(WideTemp[I]);

	ZESize RequiredSizeforResult = CalculateRequiredSizeW(WideTemp);
	Temp.Allocator.Reallocate(&Temp.Buffer, (RequiredSizeforResult + 1) * sizeof(char));

	InputSize = RequiredSizeforWide;
    WideBuffer = (char*)WideTemp;
	OutputSize = RequiredSizeforResult;
	ResultBuffer = Temp.Buffer;

	ConversionDescriptor = iconv_open("UTF-8", "WCHAR_T");
	iconv(ConversionDescriptor, &WideBuffer, &InputSize, &ResultBuffer, &OutputSize);
	iconv_close(ConversionDescriptor);

	Temp.Buffer[RequiredSizeforResult] = '\0';
	delete[] WideTemp;

	#endif

	return Temp;
}

void ZEString::LowerSelf()
{
	if (Buffer == NULL)
		return;

	BufferChanged = true;

	ZESize ByteLength = strlen(Buffer);

	#ifdef ZE_PLATFORM_WINDOWS

	ZESize RequiredSizeforWide = MultiByteToWideChar(CP_UTF8, 0, Buffer, -1, 0, 0);
	wchar_t* WideTemp = new wchar_t[RequiredSizeforWide];
	MultiByteToWideChar(CP_UTF8, 0, Buffer, -1, WideTemp, RequiredSizeforWide);
	WideTemp = _wcslwr(WideTemp);

	ZESize RequiredSizeforResult = WideCharToMultiByte(CP_UTF8, 0, WideTemp, -1, 0, 0, 0, 0);
	Allocator.Reallocate(&Buffer, RequiredSizeforResult * sizeof(char));
	WideCharToMultiByte(CP_UTF8, 0, WideTemp, -1, Buffer, RequiredSizeforResult, 0, 0);
	delete[] WideTemp;

	#elif defined ZE_PLATFORM_UNIX

	ZESize RequiredSizeforWideArray = CalculateRequiredSize(Buffer);
	ZESize RequiredSizeforWide = RequiredSizeforWideArray * sizeof(wchar_t);
	wchar_t* WideTemp = new wchar_t[RequiredSizeforWideArray + 1];
	char* WideBuffer = (char*)WideTemp;
	char* ResultBuffer = Buffer;


	ZESize InputSize = ByteLength;
	ZESize OutputSize = RequiredSizeforWide;
	iconv_t ConversionDescriptor = iconv_open("WCHAR_T", "UTF-8");
	iconv(ConversionDescriptor, &ResultBuffer, &InputSize, &WideBuffer, &OutputSize);
	iconv_close(ConversionDescriptor);

	for (ZEUInt I = 0; I < RequiredSizeforWideArray; I++)
		WideTemp[I] = towlower(WideTemp[I]);

	ZESize RequiredSizeforResult = CalculateRequiredSizeW(WideTemp);
	Allocator.Reallocate(&Buffer, (RequiredSizeforResult + 1) * sizeof(char));

	InputSize = RequiredSizeforWide;
	WideBuffer = (char*)WideTemp;
	OutputSize = RequiredSizeforResult;
    ResultBuffer = Buffer;

	ConversionDescriptor = iconv_open("UTF-8", "WCHAR_T");
	iconv(ConversionDescriptor, &WideBuffer, &InputSize, &ResultBuffer, &OutputSize);
	iconv_close(ConversionDescriptor);

	Buffer[RequiredSizeforResult] = '\0';
	delete[] WideTemp;

	#endif

}

ZEString ZEString::Upper() const
{
	if (Buffer == NULL)
		return ZEString();

	ZESize ByteLength = strlen(Buffer);

	ZEString Temp;
	Temp.Allocator.Allocate(&Temp.Buffer, (ByteLength + 1) * sizeof(char));
	Temp = Buffer;

	#ifdef ZE_PLATFORM_WINDOWS

	ZESize RequiredSizeforWide = MultiByteToWideChar(CP_UTF8, 0, Temp, -1, 0, 0);
	wchar_t* WideTemp = new wchar_t[RequiredSizeforWide];
	MultiByteToWideChar(CP_UTF8, 0, Temp, -1, WideTemp, RequiredSizeforWide);
	WideTemp = _wcsupr(WideTemp);

	ZESize RequiredSizeforResult = WideCharToMultiByte(CP_UTF8, 0, WideTemp, -1, 0, 0, 0, 0);
	Temp.Allocator.Reallocate(&Temp.Buffer, RequiredSizeforResult * sizeof(char));
	WideCharToMultiByte(CP_UTF8, 0, WideTemp, -1, Temp.Buffer, RequiredSizeforResult, 0, 0);
	delete[] WideTemp;

	#elif defined ZE_PLATFORM_UNIX

	ZESize RequiredSizeforWideArray = CalculateRequiredSize(Temp);
	ZESize RequiredSizeforWide = RequiredSizeforWideArray * sizeof(wchar_t);
	wchar_t* WideTemp = new wchar_t[RequiredSizeforWideArray + 1];
	char* WideBuffer = (char*)WideTemp;
    char* ResultBuffer = Temp.Buffer;


	ZESize InputSize = ByteLength;
	ZESize OutputSize = RequiredSizeforWide;
	iconv_t ConversionDescriptor = iconv_open("WCHAR_T", "UTF-8");
	iconv(ConversionDescriptor, &ResultBuffer, &InputSize, &WideBuffer, &OutputSize);
	iconv_close(ConversionDescriptor);

	WideTemp[RequiredSizeforWideArray] = L'\0';

	for (ZEUInt I = 0; I < RequiredSizeforWideArray; I++)
        WideTemp[I] = towupper(WideTemp[I]);

	ZESize RequiredSizeforResult = CalculateRequiredSizeW(WideTemp);
	Temp.Allocator.Reallocate(&Temp.Buffer, (RequiredSizeforResult + 1) * sizeof(char));

	InputSize = RequiredSizeforWide;
    WideBuffer = (char*)WideTemp;
	OutputSize = RequiredSizeforResult;
	ResultBuffer = Temp.Buffer;

	ConversionDescriptor = iconv_open("UTF-8", "WCHAR_T");
	iconv(ConversionDescriptor, &WideBuffer, &InputSize, &ResultBuffer, &OutputSize);
	iconv_close(ConversionDescriptor);

	Temp.Buffer[RequiredSizeforResult] = '\0';
	delete[] WideTemp;

	#endif

	return Temp;
}

void ZEString::UpperSelf()
{
	if (Buffer == NULL)
		return;

	BufferChanged = true;

	ZESize ByteLength = strlen(Buffer);

	#ifdef ZE_PLATFORM_WINDOWS

	ZESize RequiredSizeforWide = MultiByteToWideChar(CP_UTF8, 0, Buffer, -1, 0, 0);
	wchar_t* WideTemp = new wchar_t[RequiredSizeforWide];
	MultiByteToWideChar(CP_UTF8, 0, Buffer, -1, WideTemp, RequiredSizeforWide);
	WideTemp = _wcsupr(WideTemp);

	ZESize RequiredSizeforResult = WideCharToMultiByte(CP_UTF8, 0, WideTemp, -1, 0, 0, 0, 0);
	Allocator.Reallocate(&Buffer, RequiredSizeforResult * sizeof(char));
	WideCharToMultiByte(CP_UTF8, 0, WideTemp, -1, Buffer, RequiredSizeforResult, 0, 0);
	delete[] WideTemp;

	#elif defined ZE_PLATFORM_UNIX

	ZESize RequiredSizeforWideArray = CalculateRequiredSize(Buffer);
	ZESize RequiredSizeforWide = RequiredSizeforWideArray * sizeof(wchar_t);
	wchar_t* WideTemp = new wchar_t[RequiredSizeforWideArray + 1];
	char* WideBuffer = (char*)WideTemp;
	char* ResultBuffer = Buffer;


	ZESize InputSize = ByteLength;
	ZESize OutputSize = RequiredSizeforWide;
	iconv_t ConversionDescriptor = iconv_open("WCHAR_T", "UTF-8");
	iconv(ConversionDescriptor, &ResultBuffer, &InputSize, &WideBuffer, &OutputSize);
	iconv_close(ConversionDescriptor);

	for (ZEUInt I = 0; I < RequiredSizeforWideArray; I++)
		WideTemp[I] = towupper(WideTemp[I]);

	ZESize RequiredSizeforResult = CalculateRequiredSizeW(WideTemp);
	Allocator.Reallocate(&Buffer, (RequiredSizeforResult + 1) * sizeof(char));

	InputSize = RequiredSizeforWide;
	WideBuffer = (char*)WideTemp;
	OutputSize = RequiredSizeforResult;
    ResultBuffer = Buffer;

	ConversionDescriptor = iconv_open("UTF-8", "WCHAR_T");
	iconv(ConversionDescriptor, &WideBuffer, &InputSize, &ResultBuffer, &OutputSize);
	iconv_close(ConversionDescriptor);

	Buffer[RequiredSizeforResult] = '\0';
	delete[] WideTemp;

	#endif
}

const char* ZEString::ToCString() const
{
	return Buffer;
}

const wchar_t* ZEString::ToWCString()
{
	if (BufferChanged)
	{
		#ifdef ZE_PLATFORM_WINDOWS

		ZESize RequiredSize = MultiByteToWideChar(CP_UTF8, 0, Buffer, -1, 0, 0);
		WAllocator.Reallocate(&WBuffer, RequiredSize);
		MultiByteToWideChar(CP_UTF8, 0, Buffer, -1, WBuffer, RequiredSize);

		#elif defined ZE_PLATFORM_UNIX

		ZESize RequiredSize = CalculateRequiredSize(Buffer);
		WAllocator.Reallocate(&WBuffer, (RequiredSize + 1) * sizeof(wchar_t));
		char* TempWideBuffer = (char*)WBuffer;
        char* CurrentBuffer = (char*)Buffer;

		ZESize InputSize = strlen(Buffer);
		ZESize OutputSize = RequiredSize * sizeof(wchar_t);
		iconv_t ConversionDescriptor = iconv_open("WCHAR_T", "UTF-8");
		iconv(ConversionDescriptor, &CurrentBuffer, &InputSize, &TempWideBuffer, &OutputSize);
		iconv_close(ConversionDescriptor);
		WBuffer[RequiredSize] = L'\0';

		#endif

		BufferChanged = false;
	}

	return WBuffer;

}

std::wstring ZEString::ToWStdString()
{
	return std::wstring(this->ToWCString());
}

std::string ZEString::ToStdString() const
{
	return std::string(Buffer);
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
    #ifdef ZE_PLATFORM_COMPILER_MSVC
        return (ZEInt64)_atoi64(Buffer);
    #else
        return (ZEInt64)atoll(Buffer);
    #endif
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
	#ifdef ZE_PLATFORM_COMPILER_MSVC
		return (ZEUInt64)_strtoui64(Buffer, NULL, 10);
	#else
		return strtoull(Buffer, NULL, 10);
	#endif
}

float ZEString::ToFloat() const
{
 	return (float)atof(Buffer);
}

double ZEString::ToDouble() const
{
	return atof(Buffer);
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

ZEString ZEString::FromWCString(const wchar_t* Value)
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

ZEString ZEString::FromWStdString(const std::wstring& Value)
{
	return Value.c_str();
}

ZEString ZEString::FromInt8(ZEInt8 Value, const char* Format)
{
	ZEString Output;
	Output.SetValue(Value, Format);
	return Output;
}

ZEString ZEString::FromInt16(ZEInt16 Value, const char* Format)
{
	ZEString Output;
	Output.SetValue(Value, Format);
	return Output;
}

ZEString ZEString::FromInt32(ZEInt32 Value, const char* Format)
{
	ZEString Output;
	Output.SetValue(Value, Format);
	return Output;
}

ZEString ZEString::FromInt64(ZEInt64 Value, const char* Format)
{
	ZEString Output;
	Output.SetValue(Value, Format);
	return Output;
}

ZEString ZEString::FromUInt8(ZEUInt8 Value, const char* Format)
{
	ZEString Output;
	Output.SetValue(Value, Format);
	return Output;
}

ZEString ZEString::FromUInt16(ZEUInt16 Value, const char* Format)
{
	ZEString Output;
	Output.SetValue(Value, Format);
	return Output;
}

ZEString ZEString::FromUInt32(ZEUInt32 Value, const char* Format)
{
	ZEString Output;
	Output.SetValue(Value, Format);
	return Output;
}

ZEString ZEString::FromUInt64(ZEUInt64 Value, const char* Format)
{
	ZEString Output;
	Output.SetValue(Value, Format);
	return Output;
}

ZEString ZEString::FromFloat(float Value, ZEUInt Digits)
{
	ZEString Output;
	Output.SetValue(Value, Digits);
	return Output;
}

ZEString ZEString::FromFloat(float Value, const char* Format)
{
	ZEString Output;
	Output.SetValue(Value, Format);
	return Output;
}

ZEString ZEString::FromDouble(double Value, ZEUInt Digits)
{
	ZEString Output;
	Output.SetValue(Value, Digits);
	return Output;
}

ZEString ZEString::FromDouble(double Value, const char* Format)
{
	ZEString Output;
	Output.SetValue(Value, Format);
	return Output;
}

ZEString ZEString::FromBool(bool Value, const char* TrueText, const char* FalseText)
{
	return Value ? TrueText : FalseText;
}

ZEString ZEString::FromBool(bool Value, const char* Format)
{
	ZEString Output;
	Output.SetValue(Value, Format);
	return Output;
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

ZEString& ZEString::operator=(const wchar_t* String)
{
	SetValue(String);
	return *this;
}

ZEString& ZEString::operator=(const std::string& String)
{
	SetValue(String);
	return *this;
}

ZEString& ZEString::operator=(const std::wstring& String)
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

ZEString& ZEString::operator=(bool Value)
{
	SetValue(Value);
	return *this;
}

ZEString ZEString::operator+(const ZEString& String) const
{
	ZEString Temp(*this);
	Temp.Append(String);
	return Temp;
}

ZEString ZEString::operator+(const char* String) const
{
	ZEString Temp(*this);
	Temp.Append(String);
	return Temp;
}

ZEString ZEString::operator+(const wchar_t* String) const
{
	ZEString Result(*this);
	ZEString Temp(String);

	Result.Append(Temp);
	return Result;
}

ZEString ZEString::operator+(const std::string& String) const
{
	ZEString Temp(*this);
	Temp.Append(String.c_str());
	return Temp;
}

ZEString ZEString::operator+(const std::wstring& String) const
{
	ZEString Result(*this);
	ZEString Temp(String);

	Result.Append(Temp);
	return Result;
}

ZEString ZEString::operator+(ZEInt8 Value) const
{
	return *this + ZEString(Value);
}

ZEString ZEString::operator+(ZEInt16 Value) const
{
	return *this + ZEString(Value);
}

ZEString ZEString::operator+(ZEInt32 Value) const
{
	return *this + ZEString(Value);
}

ZEString ZEString::operator+(ZEInt64 Value) const
{
	return *this + ZEString(Value);
}

ZEString ZEString::operator+(ZEUInt8 Value) const
{
	return *this + ZEString(Value);
}

ZEString ZEString::operator+(ZEUInt16 Value) const
{
	return *this + ZEString(Value);
}

ZEString ZEString::operator+(ZEUInt32 Value) const
{
	return *this + ZEString(Value);
}

ZEString ZEString::operator+(ZEUInt64 Value) const
{
	return *this + ZEString(Value);
}

ZEString ZEString::operator+(float Value) const
{
	return *this + ZEString(Value);
}

ZEString ZEString::operator+(double Value) const
{
	return *this + ZEString(Value);
}

ZEString ZEString::operator+(bool Value) const
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

ZEString& ZEString::operator+=(const wchar_t* String)
{
	ZEString Temp(String);
	Append(Temp);
	return *this;
}

ZEString& ZEString::operator+=(const std::string& String)
{
	Append(String.c_str());
	return *this;
}

ZEString& ZEString::operator+=(const std::wstring& String)
{
	ZEString Temp(String);
	Append(Temp);
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

ZEString& ZEString::operator+=(bool Value)
{
	Append(ZEString(Value));
	return *this;
}

const ZECharacter ZEString::operator[](ZESSize Index) const
{
	zeDebugCheck(Buffer == NULL, "Empty string can not be indexed.");
	zeDebugCheck(Index < 0, "Index parameter is negative.");
	zeDebugCheck((ZESize)Index > this->GetLength(), "Index parameter value exceed length of the string.");

	return GetCharacter(Index);
}

ZECharacter ZEString::operator[](ZESSize Index)
{
	zeDebugCheck(Buffer == NULL, "Empty string can not be indexed.");
	zeDebugCheck(Index < 0, "Index parameter is negative.");
	zeDebugCheck((ZESize)Index > this->GetLength(), "Index parameter value exceed length of the string.");

	ZECharacter Temp = GetCharacter(Index);
	Temp.SetOwner(this, Index);

	return Temp;
}

bool ZEString::operator!=(const ZEString& String) const
{
	return !Equals(String);
}

bool ZEString::operator!=(const char* String) const
{
	return !Equals(String);
}

bool ZEString::operator!=(const std::string& String) const
{
	return !Equals(String.c_str());
}

bool ZEString::operator==(const ZEString& String) const
{
	return Equals(String);
}

bool ZEString::operator==(const char* String) const
{
	return Equals(String);
}

bool ZEString::operator==(const std::string& String) const
{
	return Equals(String.c_str());
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
	WBuffer = NULL;
}

ZEString::ZEString(const char* String)
{
	Buffer = NULL;
	WBuffer = NULL;
	SetValue(String);
}

ZEString::ZEString(const wchar_t* String)
{
	Buffer = NULL;
	WBuffer = NULL;
	SetValue(String);
}

ZEString::ZEString(const ZEString& String)
{
	Buffer = NULL;
	WBuffer = NULL;
	CopyFrom(String);
}

ZEString::ZEString(const std::string& String)
{
	Buffer = NULL;
	WBuffer = NULL;
	SetValue(String.c_str());
}

ZEString::ZEString(const std::wstring& String)
{
	Buffer = NULL;
	WBuffer = NULL;
	SetValue(String.c_str());
}

ZEString::ZEString(wchar_t Character)
{
	Buffer = NULL;
	WBuffer = NULL;
	SetValue(Character);
}

ZEString::ZEString(const ZECharacter& Character)
{
	Buffer = NULL;
	WBuffer = NULL;
	SetValue(Character);
}

ZEString::ZEString(ZEInt8 Value)
{
	Buffer = NULL;
	WBuffer = NULL;
	SetValue(Value);
}

ZEString::ZEString(ZEInt16 Value)
{
	Buffer = NULL;
	WBuffer = NULL;
	SetValue(Value);
}

ZEString::ZEString(ZEInt32 Value)
{
	Buffer = NULL;
	WBuffer = NULL;
	SetValue(Value);
}

ZEString::ZEString(ZEInt64 Value)
{
	Buffer = NULL;
	WBuffer = NULL;
	SetValue(Value);
}

ZEString::ZEString(ZEUInt8 Value)
{
	Buffer = NULL;
	WBuffer = NULL;
	SetValue(Value);
}

ZEString::ZEString(ZEUInt16 Value)
{
	Buffer = NULL;
	WBuffer = NULL;
	SetValue(Value);
}

ZEString::ZEString(ZEUInt32 Value)
{
	Buffer = NULL;
	WBuffer = NULL;
	SetValue(Value);
}

ZEString::ZEString(ZEUInt64 Value)
{
	Buffer = NULL;
	WBuffer = NULL;
	SetValue(Value);
}

ZEString::ZEString(float Value, ZEUInt Digits)
{
	Buffer = NULL;
	WBuffer = NULL;
	SetValue(Value, Digits);
}

ZEString::ZEString(float Value)
{
	Buffer = NULL;
	WBuffer = NULL;
	SetValue(Value);
}

ZEString::ZEString(double Value, ZEUInt Digits)
{
	Buffer = NULL;
	WBuffer = NULL;
	SetValue(Value, Digits);
}

ZEString::ZEString(double Value)
{
	Buffer = NULL;
	WBuffer = NULL;
	SetValue(Value);
}

ZEString::ZEString(bool Value)
{
	Buffer = NULL;
	WBuffer = NULL;
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

ZEString operator+(const wchar_t* String1, const ZEString& String2)
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

ZEString operator+(bool Value, const ZEString& String)
{
	ZEString Temp(Value);
	Temp.Append(String);
	return Temp;
}

bool operator==(const char* String1, const ZEString& String2)
{
	return String2.Equals(String1);
}

bool operator==(const wchar_t* String1, const ZEString& String2)
{
	ZEString Temp(String1);
	return String2.Equals(Temp);
}

bool operator!=(const char* String1, const ZEString& String2)
{
	return !String2.Equals(String1);
}

bool operator!=(const wchar_t* String1, const ZEString& String2)
{
	ZEString Temp(String1);
	return !String2.Equals(Temp);
}

