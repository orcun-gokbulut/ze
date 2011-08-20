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
/*
class ZEChar
{
	private:
		unsigned short				Character;

	public:
		bool						IsNull();
		bool						IsLetter();
		bool						IsLowerCaseLetter();
		bool						IsUpperCaseLetter();
		bool						IsDigit();
		bool						IsSpace();

		void						SetInteger(int Value);
		void						SetASCII(char Value);
		void						SetUTF8(unsigned char Value);
		void						SetUTF16(unsigned short Value);
		void						SetWChar(char Value);
		
		char						GetAASCII()
		unsigned char				GetUTF8();
		unsigned short				GetUTF16();
		char						GetWChar();		
		int							GetInteger();
		
		ZEChar						ToUpper();
		ZEChar						ToLower();
		
		bool						operator==(ZEChar Value);
		bool						operator==(char Value);
		bool						operator==(char Value);
		bool						operator==(unsigned char Value);

		bool						operator>(ZEChar Value);
		bool						operator>(char Value);
		bool						operator>(char Value);
		bool						operator>(unsigned char Value);

		bool						operator<(ZEChar Value);
		bool						operator<(char Value);
		bool						operator<(char Value);
		bool						operator<(unsigned char Value);

		bool						operator==(ZEChar Value);
		bool						operator==(char Value);
		bool						operator==(char Value);

		ZEChar						operator=(char Value);
		ZEChar						operator=(char Value);
		ZEChar						operator=(unsigned char Value);
		ZEChar						operator=(ZEChar Value);

		boo
}*/

// Internally UTF8
class ZEString
{
	public:
		ZEAllocatorBase<char>		Allocator;
		char*						Buffer;

	protected:
		void						SetSize();

	public:			
		void						SetValue(const char* String);
		const char*					GetValue() const;

		void						SetNumbericValue(char Value, unsigned int Base = 10);
		void						SetNumbericValue(unsigned char Value, unsigned int Base = 10);
		void						SetNumbericValue(short Value, unsigned int Base = 10);
		void						SetNumbericValue(unsigned short Value, unsigned int Base = 10);
		void						SetNumbericValue(int Value, unsigned int Base = 10);
		void						SetNumbericValue(unsigned int Value, unsigned int Base = 10);
		void						SetNumbericValue(long Value, unsigned int Base = 10);
		void						SetNumbericValue(unsigned long Value, unsigned int Base = 10);
		void						SetNumbericValue(float Value, unsigned int NumberOfDigits = 6);
		void						SetNumbericValue(double Value, unsigned int NumberOfDigits = 6);
		void						SetBooleanValue(bool Value);

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
		void						Insert(size_t Position, char Character);
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

		ZEString					ToLower() const;
		ZEString					ToUpper() const;

		int							ToInteger();
		unsigned int				ToUnsignedInteger();
		float						ToFloat();
		double						ToDouble();

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

									ZEString();					
									ZEString(const char* String);
									ZEString(const ZEString& String);
									ZEString(std::string& String);
									~ZEString();


};

#endif





