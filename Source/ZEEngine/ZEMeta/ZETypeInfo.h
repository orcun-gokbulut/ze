//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETypeInfo.h
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

#include "ZETypes.h"

template<typename ZEType = int>
class ZEInteger
{
	private:
		ZEType Value;

	public:
		// Arithmetic		
		ZEInteger				operator+(const ZEInteger& Other) { return Value + Other.Value; }
		ZEInteger				operator-(const ZEInteger& Other) { return Value - Other.Value; }
		ZEInteger				operator*(const ZEInteger& Other) { return Value * Other.Value; }
		ZEInteger				operator/(const ZEInteger& Other) { return Value / Other.Value; }
		ZEInteger				operator%(const ZEInteger& Other) { return Value % Other.Value; }
		ZEInteger&				operator++() {Value++; return *this; }
		ZEInteger&				operator--() {Value--; return *this; }
		ZEInteger				operator-() { return -Value; }
		ZEInteger				operator+() { return -Value; }

		// Binary
		ZEInteger				operator~() { return ~Value; }
		ZEInteger				operator>>(const ZEInteger& Other) { return Value >> Other.Value; };
		ZEInteger				operator<<(const ZEInteger& Other) { return Value << Other.Value; };
		ZEInteger				operator&(const ZEInteger& Other) { return ~Value & Other.Value; };
		ZEInteger				operator|(const ZEInteger& Other) { return ~Value | Other.Value; };
		ZEInteger				operator^(const ZEInteger& Other) { return ~Value ^ Other.Value; };

		// Comparison
		bool					operator==(const ZEInteger& Other) { Value == return Other.value; };
		bool					operator!=(const ZEInteger& Other) { Value != return Other.value; };
		bool					operator<(const ZEInteger& Other) { Value < return Other.value; };
		bool					operator>(const ZEInteger& Other) { Value > return Other.value; };
		bool					operator<=(const ZEInteger& Other) { Value <= return Other.value; };
		bool					operator>=(const ZEInteger& Other) { Value >= return Other.value; };

		ZEType operator			ZEType() { return Value; }

								ZEInteger() {}
								ZEInteger(const ZEInteger& InitialValue) {Value = InitialValue;}
};

class ZEEnumurator : public ZEInteger<>
{
	public:
		virtual ZEClass*	GetClass();
		static ZEClass*		Class();
};

#define ze_enum const ZEInt32

class ZEBla : public ZEEnumurator
{
	ze_enum Deneme = 1;
	ze_enum Deneme = 3;
	ze_enum BlaBla = 3;
};

enum ZETypeInfo
{
	ZE_VRT_UNDEFINED = 0,
	ZE_VRT_NULL,
	ZE_VRT_FLOAT,
	ZE_VRT_DOUBLE,
	ZE_VRT_INTEGER_8,
	ZE_VRT_INTEGER_16,
	ZE_VRT_INTEGER_32,
	ZE_VRT_INTEGER_64,
	ZE_VRT_UNSIGNED_INTEGER_8,
	ZE_VRT_UNSIGNED_INTEGER_16,
	ZE_VRT_UNSIGNED_INTEGER_32,
	ZE_VRT_UNSIGNED_INTEGER_64,
	ZE_VRT_BOOLEAN,
	ZE_VRT_STRING,
	ZE_VRT_QUATERNION,
	ZE_VRT_VECTOR2,
	ZE_VRT_VECTOR3,
	ZE_VRT_VECTOR4,
	ZE_VRT_MATRIX3X3,
	ZE_VRT_MATRIX4X4,
	ZE_VRT_OBJECT,
	ZE_VRT_CLASS
};

enum ZECanonicalType
{
	ZE_CT_VALUE,
	ZE_CT_CONST_VALUE,
	ZE_CT_REFERENCE,
	ZE_CT_CONST_REFERENCE,
	ZE_CT_POINTER,
	ZE_CT_CONST_POINTER
};

class ZEClass;
struct ZETypeInfo
{
	ZEVariantType			Type;
	ZEClass*				BaseClass;
	ZECanonicalType			CanonicalType;

	static bool				Compare(const ZEPropertySignature& A, const ZEPropertySignature& B);
	ZEString				GetName();

	ZEPropertySignature();
};
