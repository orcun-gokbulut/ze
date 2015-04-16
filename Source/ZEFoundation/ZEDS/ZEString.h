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
#include "ZEMeta\ZEBuiltIn.h"

class ZEString;

class ZECharacter
{
	friend class ZEString;

	private:
		char					Characters[4];
		ZEUInt					Size;

		ZEString*				Owner;
		ZESize					Position;

	public:
		void					SetValue(char Character);
		void					SetValue(const char* MultiByteCharacter);
		void					SetValue(const wchar_t* WideCharacter);
		void					SetValue(ZEInt Character);

		void					SetOwner(ZEString* Owner, ZESize Index);

		const char*				GetValue() const;
		ZEUInt					GetSize() const;

		bool					Equals(const ZECharacter& Character) const;
		bool					Equals(const char& Character) const;
		bool					Equals(const char* MultiByteCharacter) const;
		bool					Equals(const wchar_t* WideCharacter) const;

		ZECharacter				Upper() const;
		void					UpperSelf();
		ZECharacter				Lower() const;
		void					LowerSelf();

		static ZECharacter 		FromChar(const char& Value);
		static ZECharacter		FromMBChar(const char* Value);
		static ZECharacter		FromWChar(const wchar_t* Value);

		ZECharacter&			operator=(const ZECharacter& Character);
		ZECharacter&			operator=(const char& Character);
		ZECharacter&			operator=(const char* MultiByteCharacter);
		ZECharacter&			operator=(const wchar_t* WideCharacter);

		bool					operator!=(const ZECharacter& Character) const;
		bool					operator!=(const char& Character) const;
		bool					operator!=(const char* MultiByteCharacter) const;
		bool					operator!=(const wchar_t* WideCharacter) const;

		bool					operator==(const ZECharacter& Character) const;
		bool					operator==(const char& Character) const;
		bool					operator==(const char* MultiByteCharacter) const;
		bool					operator==(const wchar_t* WideCharacter) const;

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

bool operator==(const char& Character1, const ZECharacter& Character2);
bool operator==(const char* MultiByteCharacter, const ZECharacter& Character);
bool operator==(const wchar_t* WideCharacter, const ZECharacter& Character);
bool operator!=(const char& Character1, const ZECharacter& Character2);
bool operator!=(const char* MultiByteCharacter, const ZECharacter& Character);
bool operator!=(const wchar_t* WideCharacter, const ZECharacter& Character);

class ZEString
{
	ZE_BUILTIN_OBJECT

	friend class ZEStringWriter;
	private:
		char*								Buffer;
		ZESmartAllocator<char>				Allocator;

		mutable bool						BufferChanged;

		mutable wchar_t*					WBuffer;
		mutable ZEAllocatorBase<wchar_t>	WAllocator;

	public:
		void						SetBuffer(void* Buffer, ZESize Size);

		void						SetValue(void* Buffer, ZESize Size);
		void						SetValue(const ZEString& String);
		void						SetValue(const char* String);
		void						SetValue(const wchar_t* String);
		void						SetValue(const std::string& String);
		void						SetValue(const std::wstring& String);
		void						SetValue(wchar_t Character);
		void						SetValue(const ZECharacter& Character);

		void						SetValue(ZEInt8 Value, const char* Format = NULL);
		void						SetValue(ZEInt16 Value, const char* Format = NULL);
		void						SetValue(ZEInt32 Value, const char* Format = NULL);
		void						SetValue(ZEInt64 Value, const char* Format = NULL);
		void						SetValue(ZEUInt8 Value, const char* Format = NULL);
		void						SetValue(ZEUInt16 Value, const char* Format = NULL);
		void						SetValue(ZEUInt32 Value, const char* Format = NULL);
		void						SetValue(ZEUInt64 Value, const char* Format = NULL);
		void						SetValue(float Value, const char* Format = NULL);
		void						SetValue(double Value, const char* Format = NULL);
		void						SetValue(bool Value, const char* Format = NULL);

		const char*					GetValue() const;

		ZECharacter					GetCharacter(ZESize Position) const;
		void						SetCharacter(ZESize Position, ZECharacter Value);

		bool						IsEmpty() const;
		ZESize						GetBufferSize() const;
		ZESize						GetLength() const;

		void						SetSize(ZESize Size);
		ZESize						GetSize() const;

		void						Compact();
		void						Clear();

		void						Append(const ZEString& String);
		void						Append(const char* String);
		void						Insert(const ZEString& String);
		void						Insert(const char* String);
		void						Insert(ZESize Position, const ZEString& String);
		void						Insert(ZESize Position, const char* String);

		void						AppendCharacter(const ZECharacter& Character);
		void						InsertCharacter(const ZECharacter& Character);
		void						InsertCharacter(ZESize Position, const ZECharacter& Character);

		void						Remove(ZESize Position, ZESize Count = 1);

		bool						Equals(const ZEString& String) const;
		bool						Equals(const char* String) const;

		void						CopyTo(ZEString& String) const;
		void						CopyFrom(const ZEString& String);

		ZEString					Left(ZESize Count) const;
		ZEString					Right(ZESize Count) const;
		ZEString					Middle(ZESize Position, ZESize Count) const;
		ZEString					SubString(ZESize StartPosition, ZESize EndPosition) const;

		ZEString					TrimLeft() const;
		void						TrimLeftSelf();
		ZEString					TrimRight() const;
		void						TrimRightSelf();
		ZEString					Trim() const;
		void						TrimSelf();

		ZESize						Hash() const;

		ZEString					Lower() const;
		void						LowerSelf();
		ZEString					Upper() const;
		void						UpperSelf();

		const char*					ToCString() const;
		std::string					ToStdString() const;
		const wchar_t*				ToWCString() const;
		std::wstring				ToWStdString() const;
		ZEInt8						ToInt8() const;
		ZEInt16						ToInt16() const;
		ZEInt32						ToInt32() const;
		ZEInt64						ToInt64() const;
		ZEUInt8						ToUInt8() const;
		ZEUInt16					ToUInt16() const;
		ZEUInt32					ToUInt32() const;
		ZEUInt64					ToUInt64() const;
		float						ToFloat() const;
		double						ToDouble() const;

		static ZEString 			FromChar(char Value);
		static ZEString				FromWChar(wchar_t Value);
		static ZEString 			FromCString(const char* Value);
		static ZEString 			FromStdString(const std::string& Value);
		static ZEString				FromWCString(const wchar_t* Value);
		static ZEString				FromWStdString(const std::wstring& Value);
		static ZEString 			FromInt8(ZEInt8 Value, const char* Format = NULL);
		static ZEString 			FromInt16(ZEInt16 Value, const char* Format = NULL);
		static ZEString 			FromInt32(ZEInt32 Value, const char* Format = NULL);
		static ZEString 			FromInt64(ZEInt64 Value, const char* Format = NULL);
		static ZEString 			FromUInt8(ZEUInt8 Value, const char* Format = NULL);
		static ZEString 			FromUInt16(ZEUInt16 Value, const char* Format = NULL);
		static ZEString 			FromUInt32(ZEUInt32 Value, const char* Format = NULL);
		static ZEString 			FromUInt64(ZEUInt64 Value, const char* Format = NULL);
		static ZEString 			FromFloat(float Value, const char* Format = NULL);
		static ZEString 			FromDouble(double Value, const char* Format = NULL);
		static ZEString 			FromBool(bool Value, const char* Format = NULL);

		ZEString&					operator=(const ZEString& String);
		ZEString&					operator=(const char* String);
		ZEString&					operator=(const wchar_t* String);
		ZEString&					operator=(const std::string& String);
		ZEString&					operator=(const std::wstring& String);
		ZEString&					operator=(ZEInt8 Value);
		ZEString&					operator=(ZEInt16 Value);
		ZEString&					operator=(ZEInt32 Value);
		ZEString&					operator=(ZEInt64 Value);
		ZEString&					operator=(ZEUInt8 Value);
		ZEString&					operator=(ZEUInt16 Value);
		ZEString&					operator=(ZEUInt32 Value);
		ZEString&					operator=(ZEUInt64 Value);
		ZEString&					operator=(float Value);
		ZEString&					operator=(double Value);
		ZEString&					operator=(bool Value);

		ZEString					operator+(const ZEString& String) const;
		ZEString					operator+(const char* String) const;
		ZEString					operator+(const wchar_t* String) const;
		ZEString					operator+(const std::string& String) const;
		ZEString					operator+(const std::wstring& String) const;
		ZEString					operator+(ZEInt8 Value) const;
		ZEString					operator+(ZEInt16 Value) const;
		ZEString					operator+(ZEInt32 Value) const;
		ZEString					operator+(ZEInt64 Value) const;
		ZEString					operator+(ZEUInt8 Value) const;
		ZEString					operator+(ZEUInt16 Value) const;
		ZEString					operator+(ZEUInt32 Value) const;
		ZEString					operator+(ZEUInt64 Value) const;	
		ZEString					operator+(float Value) const;
		ZEString					operator+(double Value) const;
		ZEString					operator+(bool Value) const;

		ZEString&					operator+=(const ZEString& String);
		ZEString&					operator+=(const char* String);
		ZEString&					operator+=(const wchar_t* String);
		ZEString&					operator+=(const std::string& String);
		ZEString&					operator+=(const std::wstring& String);
		ZEString&					operator+=(ZEInt8 Value);
		ZEString&					operator+=(ZEInt16 Value);
		ZEString&					operator+=(ZEInt32 Value);
		ZEString&					operator+=(ZEInt64 Value);
		ZEString&					operator+=(ZEUInt8 Value);
		ZEString&					operator+=(ZEUInt16 Value);
		ZEString&					operator+=(ZEUInt32 Value);
		ZEString&					operator+=(ZEUInt64 Value);
		ZEString&					operator+=(float Value);
		ZEString&					operator+=(double Value);
		ZEString&					operator+=(bool Value);

		ZECharacter					operator[](ZESSize Index);
		const ZECharacter			operator[](ZESSize Index) const;

		bool						operator!=(const ZEString& String) const;
		bool						operator!=(const char* String) const;
		bool						operator!=(const std::string& String) const;

		bool						operator==(const ZEString& String) const;
		bool						operator==(const char* String) const;
		bool						operator==(const std::string& String) const;

									operator std::string() const;
									operator const char*() const;

									ZEString();

									ZEString(const char* String);
									ZEString(const wchar_t* String);
									ZEString(const ZEString& String);
									ZEString(const std::string& String);
									ZEString(const std::wstring& String);
									ZEString(wchar_t Character);
									ZEString(const ZECharacter& Character);
									ZEString(ZEInt8 Value);
									ZEString(ZEInt16 Value);
									ZEString(ZEInt32 Value);
									ZEString(ZEInt64 Value);
									ZEString(ZEUInt8 Value);
									ZEString(ZEUInt16 Value);
									ZEString(ZEUInt32 Value);
									ZEString(ZEUInt64 Value);
									ZEString(float Value);
									ZEString(double Value);
									ZEString(bool Value);
									~ZEString();
};


ZEString operator+(const char* String1, const ZEString& String2);
ZEString operator+(const wchar_t* String1, const ZEString& String2);
ZEString operator+(ZEInt8 Value, const ZEString& String);
ZEString operator+(ZEInt16 Value, const ZEString& String);
ZEString operator+(ZEInt32 Value, const ZEString& String);
ZEString operator+(ZEInt64 Value, const ZEString& String);
ZEString operator+(ZEUInt8 Value, const ZEString& String);
ZEString operator+(ZEUInt16 Value, const ZEString& String);
ZEString operator+(ZEUInt32 Value, const ZEString& String);
ZEString operator+(ZEUInt64 Value, const ZEString& String);
ZEString operator+(float Value, const ZEString& String);
ZEString operator+(double Value, const ZEString& String);
ZEString operator+(bool Value, const ZEString& String);

bool operator==(const char* String1, const ZEString& String2);
bool operator==(const wchar_t* String1, const ZEString& String2);
bool operator!=(const char* String1, const ZEString& String2);
bool operator!=(const wchar_t* String1, const ZEString& String2);

#endif
