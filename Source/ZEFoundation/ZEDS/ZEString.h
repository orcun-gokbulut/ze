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

#include "ZETypes.h"
#include "ZEAllocator.h"

#include <string>

class ZEString
{
	private:
		char*						Buffer;
		ZEAllocatorBase<char>		Allocator;

	public:			
		void						SetValue(const char* String);
		void						SetValue(const wchar_t* String);
		void						SetValue(wchar_t Character);
		void						SetValue(ZEInt8 Value);
		void						SetValue(ZEInt16 Value);
		void						SetValue(ZEInt32 Value);
		void						SetValue(ZEInt64 Value);
		void						SetValue(ZEUInt8 Value);
		void						SetValue(ZEUInt16 Value);
		void						SetValue(ZEUInt32 Value);
		void						SetValue(ZEUInt64 Value);
		void						SetValue(float Value, ZEUInt Digits);
		void						SetValue(float Value);
		void						SetValue(double Value, ZEUInt Digits);
		void						SetValue(double Value);
		void						SetValue(bool Value, const char* TrueText = "True", const char* FalseText = "False");

		const char*					GetValue() const;

		char						GetCharacter(ZESize Position) const;
		void						SetCharacter(ZESize Position, char Value);

		bool						IsEmpty() const;
		ZESize						GetLength() const;
		ZESize						GetSize() const;

		void						Compact();
		void						Clear();

		void						Append(const ZEString& String);
		void						Append(const char* String);
		void						Insert(const ZEString& String);
		void						Insert(const char* String);
		void						Insert(ZESize Position, const ZEString& String);
		void						Insert(ZESize Position, const char* String);
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

		ZEString					Lower() const;
		void						LowerSelf();
		ZEString					Upper() const;
		void						UpperSelf();

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
		const char*					ToCString() const;
		std::string					ToStdString() const;
		
		static ZEString 			FromChar(char Value);
		static ZEString				FromWChar(wchar_t Value);		
		static ZEString 			FromInt8(ZEInt8 Value);
		static ZEString 			FromInt16(ZEInt16 Value);
		static ZEString 			FromInt32(ZEInt32 Value);
		static ZEString 			FromInt64(ZEInt64 Value);
		static ZEString 			FromUInt8(ZEUInt8 Value);
		static ZEString 			FromUInt16(ZEUInt16 Value);
		static ZEString 			FromUInt32(ZEUInt32 Value);
		static ZEString 			FromUInt64(ZEUInt64 Value);
		static ZEString 			FromFloat(float Value, ZEUInt Digits);
		static ZEString 			FromFloat(float Value);
		static ZEString 			FromDouble(double Value, ZEUInt Digits);
		static ZEString 			FromDouble(double Value);
		static ZEString 			FromBool(bool Value, const char* TrueText = "True", const char* FalseText = "False");
		static ZEString 			FromCString(const char* Value);
		static ZEString				FromWString(const wchar_t* Value);
		static ZEString 			FromStdString(const std::string& Value);

		ZEString&					operator=(const ZEString& String);
		ZEString&					operator=(const char* String);
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

		ZEString					operator+(const ZEString& String);
		ZEString					operator+(const char* String);
		ZEString					operator+(ZEInt8 Value);
		ZEString					operator+(ZEInt16 Value);
		ZEString					operator+(ZEInt32 Value);
		ZEString					operator+(ZEInt64 Value);
		ZEString					operator+(ZEUInt8 Value);
		ZEString					operator+(ZEUInt16 Value);
		ZEString					operator+(ZEUInt32 Value);
		ZEString					operator+(ZEUInt64 Value);	
		ZEString					operator+(float Value);
		ZEString					operator+(double Value);	

		ZEString&					operator+=(const ZEString& String);
		ZEString&					operator+=(const char* String);
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

		char&						operator[](ZESSize Index);
		const char&					operator[](ZESSize Index) const;

		bool						operator!=(const ZEString& String) const;
		bool						operator!=(const char* String) const;

		bool						operator==(const ZEString& String) const;
		bool						operator==(const char* String) const;
	
									operator std::string() const;
									operator const char*() const;

		static ZEString				Format(const char* Format, ...);

									ZEString();					
									ZEString(const char* String);
									ZEString(const ZEString& String);
									ZEString(const std::string& String);
									ZEString(ZEInt8 Value);
									ZEString(ZEInt16 Value);
									ZEString(ZEInt32 Value);
									ZEString(ZEInt64 Value);
									ZEString(ZEUInt8 Value);
									ZEString(ZEUInt16 Value);
									ZEString(ZEUInt32 Value);
									ZEString(ZEUInt64 Value);
									ZEString(float Value, ZEUInt Digists);
									ZEString(float Value);
									ZEString(double Value, ZEUInt Digists);
									ZEString(double Value);
									~ZEString();


};

ZEString operator+(const char* String1, const ZEString& String2);
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

bool operator==(const char* String1, const ZEString& String2);
bool operator!=(const char* String1, const ZEString& String2);

#endif
