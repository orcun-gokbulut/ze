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

#include "ZEAllocator.h"
#include <string>

#include "ZETypes.h"

class ZEString
{
	private:
		char*						Buffer;
		ZEAllocatorBase<char>		Allocator;

	public:			
		void						SetValue(const char* String);
		void						SetValue(ZEInt Value, ZEUInt Base = 10);
		void						SetValue(ZEUInt Value, ZEUInt Base = 10);
		void						SetValue(float Value, ZEUInt Digits);
		void						SetValue(bool Value, const char* TrueText = "True", const char* FalseText = "False");

		const char*					GetValue() const;

		char						GetCharacter(size_t Position) const;
		void						SetCharacter(size_t Position, char Value);

		bool						IsEmpty() const;
		size_t						GetLength() const;
		size_t						GetSize() const;

		void						Compact();
		void						Clear();

		void						Append(const ZEString& String);
		void						Append(const char* String);
		void						Insert(const ZEString& String);
		void						Insert(const char* String);
		void						Insert(size_t Position, const ZEString& String);
		void						Insert(size_t Position, const char* String);
		void						Remove(size_t Position, unsigned int Count = 1);

		size_t						Contains(const char* String) const;	
		size_t						Contains(const ZEString& String) const;
		void						Replace(const char* StringToReplace, const char* Replace);
		void						Replace(const ZEString& Find, const ZEString& Replace);
		void						Delete(const char* StringToRemove);
		void						Delete(const ZEString& String);
		
		bool						Equals(const ZEString& String) const;
		bool						Equals(const char* String) const;

		void						CopyTo(ZEString& String) const;
		void						CopyFrom(const ZEString& String);

		ZEString					Left(size_t Count) const;
		ZEString					Right(size_t Count) const;
		ZEString					Middle(size_t Position, size_t Count) const;
		ZEString					SubString(size_t StartPosition, size_t EndPosition) const;

		ZEString					TrimLeft() const;
		ZEString					TrimRight() const;
		ZEString					Trim() const;

		ZEString					Lower() const;
		ZEString					Upper() const;

		ZEInt						ToInt() const;
		ZEUInt						ToUInt() const;
		float						ToFloat() const;
		const char*					ToCString() const;
		std::string					ToStdString() const;

		static ZEString 			FromChar(char Value);
		static ZEString 			FromInt(ZEInt Value, ZEUInt Base = 10);
		static ZEString 			FromUInt(ZEUInt Value, ZEUInt Base = 10);
		static ZEString 			FromFloat(float Value, ZEUInt Digits);
		static ZEString 			FromBool(bool Value, const char* TrueText = "True", const char* FalseText = "False");
		static ZEString 			FromCString(const char* Value);
		static ZEString 			FromStdString(const std::string& Value);

		ZEString&					operator=(const ZEString& String);
		ZEString&					operator=(const char* String);

		ZEString					operator+(const ZEString& String);
		ZEString					operator+(const char* String);

		ZEString&					operator+=(const ZEString& String);
		ZEString&					operator+=(const char* String);

		char&						operator[](int Index);
		const char&					operator[](int Index) const;

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
									~ZEString();


};

ZEString operator+(const char* String1, const ZEString& String2);
bool operator==(const char* String1, const ZEString& String2);
bool operator!=(const char* String1, const ZEString& String2);

#endif





