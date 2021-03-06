//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEString.h
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

#pragma once
#ifndef __ZE_STRING_H__
#define __ZE_STRING_H__

#include <string>
#include "ZETypes.h"
#include "ZEAllocator.h"
#include "ZEMeta\ZEMTFundamental.h"

class ZEString;

class ZECharacter
{
	friend class ZEString;
	private:
		char								Characters[4];
		ZEUInt								Size;

		ZEString*							Owner;
		ZESize								Position;

	public:
		void								SetValue(char Character);
		void								SetValue(const char* MultiByteCharacter);
		void								SetValue(const wchar_t* WideCharacter);
		void								SetValue(ZEInt Character);

		void								SetOwner(ZEString* Owner, ZESize Index);

		const char*							GetValue() const;
		ZEUInt								GetSize() const;

		bool								Equals(const ZECharacter& Character) const;
		bool								Equals(const char& Character) const;
		bool								Equals(const char* MultiByteCharacter) const;
		bool								Equals(const wchar_t* WideCharacter) const;

		ZECharacter							Upper() const;
		void								UpperSelf();
		ZECharacter							Lower() const;
		void								LowerSelf();

		static ZECharacter 					FromChar(const char& Value);
		static ZECharacter					FromMBChar(const char* Value);
		static ZECharacter					FromWChar(const wchar_t* Value);

		ZECharacter&						operator=(const ZECharacter& Character);
		ZECharacter&						operator=(const char& Character);
		ZECharacter&						operator=(const char* MultiByteCharacter);
		ZECharacter&						operator=(const wchar_t* WideCharacter);

		bool								operator!=(const ZECharacter& Character) const;
		bool								operator!=(const char& Character) const;
		bool								operator!=(const char* MultiByteCharacter) const;
		bool								operator!=(const wchar_t* WideCharacter) const;

		bool								operator==(const ZECharacter& Character) const;
		bool								operator==(const char& Character) const;
		bool								operator==(const char* MultiByteCharacter) const;
		bool								operator==(const wchar_t* WideCharacter) const;

											operator char() const;
											operator const char*() const;
											operator wchar_t() const;

											ZECharacter();
											ZECharacter(const ZECharacter& Character);
											ZECharacter(char Character);
											ZECharacter(const char* MultiByteCharacter);
											ZECharacter(const wchar_t* WideCharacter);
											~ZECharacter();
};

class ZEString
{
	ZE_FUNDAMENTAL
	friend class ZEStringWriter;
	private:
		char*								Buffer;
		ZESmartAllocator<char>				Allocator;

		mutable bool						BufferChanged;

		mutable wchar_t*					WBuffer;
		mutable ZEAllocatorBase<wchar_t>	WAllocator;

	public:
		static const ZEString				Empty;

		void								SetBuffer(const void* Buffer, ZESize Size);

		void								SetValue(const void* Buffer, ZESize Size);
		void								SetValue(const ZEString& String);
		void								SetValue(const char* String);
		void								SetValue(const wchar_t* String);
		void								SetValue(const std::string& String);
		void								SetValue(const std::wstring& String);
		void								SetValue(wchar_t Character);
		void								SetValue(const ZECharacter& Character);

		void								SetValue(ZEInt8 Value, const char* Format = NULL);
		void								SetValue(ZEInt16 Value, const char* Format = NULL);
		void								SetValue(ZEInt32 Value, const char* Format = NULL);
		void								SetValue(ZEInt64 Value, const char* Format = NULL);
		void								SetValue(ZEUInt8 Value, const char* Format = NULL);
		void								SetValue(ZEUInt16 Value, const char* Format = NULL);
		void								SetValue(ZEUInt32 Value, const char* Format = NULL);
		void								SetValue(ZEUInt64 Value, const char* Format = NULL);
		void								SetValue(float Value, const char* Format = NULL);
		void								SetValue(double Value, const char* Format = NULL);
		void								SetValue(bool Value, const char* Format = NULL);

		const char*							GetValue() const;

		ZECharacter							GetCharacter(ZESize Position) const;
		void								SetCharacter(ZESize Position, ZECharacter Value);

		bool								IsEmpty() const;
		ZESize								GetBufferSize() const;
		ZESize								GetLength() const;

		void								SetSize(ZESize Size);
		ZESize								GetSize() const;

		void								Compact();
		void								Clear();

		void								Append(const ZEString& String);
		void								Append(const char* String);
		void								Append(const char* String, ZESize Size);
		void								Insert(const ZEString& String);
		void								Insert(const char* String);
		void								Insert(ZESize Position, const ZEString& String);
		void								Insert(ZESize Position, const char* String);

		void								AppendCharacter(const ZECharacter& Character);
		void								InsertCharacter(const ZECharacter& Character);
		void								InsertCharacter(ZESize Position, const ZECharacter& Character);

		void								Remove(ZESize Position, ZESize Count = 1);

		bool								Equals(const ZEString& String) const;
		bool								Equals(const char* String) const;

		bool								EqualsIncase(const ZEString& String) const;
		bool								EqualsIncase(const char* String) const;

		void								CopyTo(ZEString& String) const;
		void								CopyFrom(const ZEString& String);

		ZEString							Left(ZESize Count) const;
		ZEString							Right(ZESize Count) const;
		ZEString							Middle(ZESize Position, ZESize Count) const;
		ZEString							SubString(ZESize StartPosition, ZESize EndPosition) const;

		ZEString							TrimLeft() const;
		void								TrimLeftSelf();
		ZEString							TrimRight() const;
		void								TrimRightSelf();
		ZEString							Trim() const;
		void								TrimSelf();

		ZEUInt								Hash() const;

		ZEString							Lower() const;
		void								LowerSelf();
		ZEString							Upper() const;
		void								UpperSelf();

		const char*							ToCString() const;
		std::string							ToStdString() const;
		const wchar_t*						ToWCString() const;
		std::wstring						ToWStdString() const;
		ZEInt8								ToInt8() const;
		ZEInt16								ToInt16() const;
		ZEInt32								ToInt32() const;
		ZEInt64								ToInt64() const;
		ZEUInt8								ToUInt8(int Base = 10) const;
		ZEUInt16							ToUInt16(int Base = 10) const;
		ZEUInt32							ToUInt32(int Base = 10) const;
		ZEUInt64							ToUInt64(int Base = 10) const;
		float								ToFloat() const;
		double								ToDouble() const;

		static ZEString 					FromChar(char Value);
		static ZEString						FromWChar(wchar_t Value);
		static ZEString 					FromCString(const char* Value);
		static ZEString 					FromStdString(const std::string& Value);
		static ZEString						FromWCString(const wchar_t* Value);
		static ZEString						FromWStdString(const std::wstring& Value);
		static ZEString 					FromInt8(ZEInt8 Value, const char* Format = NULL);
		static ZEString 					FromInt16(ZEInt16 Value, const char* Format = NULL);
		static ZEString 					FromInt32(ZEInt32 Value, const char* Format = NULL);
		static ZEString 					FromInt64(ZEInt64 Value, const char* Format = NULL);
		static ZEString 					FromUInt8(ZEUInt8 Value, const char* Format = NULL);
		static ZEString 					FromUInt16(ZEUInt16 Value, const char* Format = NULL);
		static ZEString 					FromUInt32(ZEUInt32 Value, const char* Format = NULL);
		static ZEString 					FromUInt64(ZEUInt64 Value, const char* Format = NULL);
		static ZEString 					FromFloat(float Value, const char* Format = NULL);
		static ZEString 					FromDouble(double Value, const char* Format = NULL);
		static ZEString 					FromBool(bool Value, const char* Format = NULL);

		ZEString&							operator=(const ZEString& String);
		ZEString&							operator=(const char* String);
		ZEString&							operator=(const wchar_t* String);
		ZEString&							operator=(const std::string& String);
		ZEString&							operator=(const std::wstring& String);

		ZEString							operator+(const ZEString& String) const;
		ZEString							operator+(const char* String) const;
		ZEString							operator+(const wchar_t* String) const;
		ZEString							operator+(const std::string& String) const;
		ZEString							operator+(const std::wstring& String) const;

		ZEString&							operator+=(const ZEString& String);
		ZEString&							operator+=(const char* String);
		ZEString&							operator+=(const wchar_t* String);
		ZEString&							operator+=(const std::string& String);
		ZEString&							operator+=(const std::wstring& String);

		ZECharacter							operator[](ZESSize Index);
		const ZECharacter					operator[](ZESSize Index) const;

		bool								operator!=(const ZEString& String) const;
		bool								operator!=(const char* String) const;
		bool								operator!=(const std::string& String) const;

		bool								operator==(const ZEString& String) const;
		bool								operator==(const char* String) const;
		bool								operator==(const std::string& String) const;

											operator std::string() const;
											operator const char*() const;
											operator const wchar_t*() const;

											ZEString();

											ZEString(const char* String);
											ZEString(const char* Start, const char* End);
											ZEString(const wchar_t* String);
											ZEString(const void* Buffer, ZESize Length);
											ZEString(const ZEString& String);
											ZEString(const std::string& String);
											ZEString(const std::wstring& String);
											ZEString(const ZECharacter& Character);
											~ZEString();
};


bool operator==(const char& Character1, const ZECharacter& Character2);
bool operator==(const char* MultiByteCharacter, const ZECharacter& Character);
bool operator==(const wchar_t* WideCharacter, const ZECharacter& Character);
bool operator!=(const char& Character1, const ZECharacter& Character2);
bool operator!=(const char* MultiByteCharacter, const ZECharacter& Character);
bool operator!=(const wchar_t* WideCharacter, const ZECharacter& Character);

ZEString operator+(const char* String1, const ZEString& String2);
ZEString operator+(const wchar_t* String1, const ZEString& String2);

bool operator==(const char* String1, const ZEString& String2);
bool operator==(const wchar_t* String1, const ZEString& String2);
bool operator!=(const char* String1, const ZEString& String2);
bool operator!=(const wchar_t* String1, const ZEString& String2);

#endif
