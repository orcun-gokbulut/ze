//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEVariant.h
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
#ifndef __ZE_VARIANT_H__
#define __ZE_VARIANT_H__

#pragma warning(push)
#pragma warning(disable:4482)

#include "ZETypes.h"
#include "ZESerialization/ZESerializable.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEDS/ZEString.h"

class ZESerializer;
class ZEUnserializer;

enum ZEVariantType
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
	ZE_VRT_CLASS
};

class ZEMatrix4x4;
class ZEMatrix4x4;
class ZEVector2;
class ZEVector3;
class ZEVector4;
class ZEObject;

class ZEVariant : public ZESerializable
{
	public:
		struct ZEVariantValue
		{
			union
			{
				float			Float;
				double			Double;

				ZEInt32			Int32;
				ZEInt64			Int64;
			
				bool			Boolean;
				ZEObject*		Pointer;
			
				struct
				{
					float		A, B, C, D;
				} Vectors;

				ZEMatrix3x3*	Matrix3x3;
				ZEMatrix4x4*	Matrix4x4;
			};
			ZEString			String;
		};

	protected:
		ZEVariantType			Type;
		ZEVariantValue			Value;

	public:
		void					SetType	(ZEVariantType NewType);
		ZEVariantType			GetType	() const;

		ZEVariantValue			GetValue() const;
		ZEVariantValue&			ImplicitAcesss();

		void					SetString(const char* Value);
		void					SetString(const ZEString& Value);
		void					SetInt8(ZEInt8 Value);
		void					SetInt16(ZEInt16 Value);
		void					SetInt32(ZEInt32 Value);
		void					SetInt64(ZEInt64 Value);
		void					SetUInt8(ZEUInt8 Value);
		void					SetUInt16(ZEUInt16 Value);
		void					SetUInt32(ZEUInt32 Value);
		void					SetUInt64(ZEUInt64 Value);
		void					SetFloat(float Value);
		void					SetDouble(double Value);
		void					SetBoolean(bool Value);
		void					SetVector2(const ZEVector2& Value);
		void					SetVector3(const ZEVector3& Value);
		void					SetVector4(const ZEVector4& Value);
		void					SetQuaternion(const ZEQuaternion& Value);
		void					SetMatrix3x3(const ZEMatrix3x3& Value);
		void					SetMatrix4x4(const ZEMatrix4x4& Value);
		void					SetClass(ZEObject* Value);
		void					SetNull();
		void					SetVariant(const ZEVariant& Value);

		const ZEString&			GetString() const;
		ZEInt8					GetInt8() const;
		ZEInt16					GetInt16() const;
		ZEInt32					GetInt32() const;
		ZEInt64					GetInt64() const;
		ZEUInt8					GetUInt8() const;
		ZEUInt16				GetUInt16() const;
		ZEUInt32				GetUInt32() const;
		ZEUInt64				GetUInt64() const;
		float					GetFloat() const;
		double					GetDouble() const;
		bool					GetBoolean() const;
		ZEVector2&				GetVector2() const;
		ZEVector3&				GetVector3() const;
		ZEVector4&				GetVector4() const;
		ZEQuaternion&			GetQuaternion() const;
		ZEMatrix3x3&			GetMatrix3x3() const;
		ZEMatrix4x4&			GetMatrix4x4() const;
		ZEObject*				GetClass() const;
		
		bool					IsNull() const;
		ZESize					SizeOf() const;

		bool					Serialize(ZESerializer* Serializer);
		bool					Unserialize(ZEUnserializer* Unserializer);

		void					operator= (const ZEVariant& Value);
		void					operator= (const char* Value);
		void					operator= (const ZEString& Value);
		void					operator= (ZEInt8 Value);
		void					operator= (ZEInt16 Value);
		void					operator= (ZEInt32 Value);
		void					operator= (ZEInt64 Value);
		void					operator= (ZEUInt8 Value);
		void					operator= (ZEUInt16 Value);
		void					operator= (ZEUInt32 Value);
		void					operator= (ZEUInt64 Value);
		void					operator= (float Value);
		void					operator= (double Value);
		void					operator= (bool Value);
		void					operator= (const ZEVector2& Value);
		void					operator= (const ZEVector3& Value);
		void					operator= (const ZEVector4& Value);
		void					operator= (const ZEQuaternion& Value);
		void					operator= (const ZEMatrix3x3& Value);
		void					operator= (const ZEMatrix4x4& Value);
		void					operator= (ZEObject* Value);

								operator const char*();
								
								operator ZEInt8();
								operator ZEInt16();
								operator ZEInt32();
								operator ZEInt64();
								operator ZEUInt8();
								operator ZEUInt16();
								operator ZEUInt32();
								operator ZEUInt64();
								operator float();
								operator double();
								operator bool();
								operator ZEVector2();
								operator ZEVector3();
								operator ZEVector4();
								operator ZEQuaternion();
								operator ZEMatrix3x3();
								operator ZEMatrix4x4();
								operator ZEObject*();

								ZEVariant();
								ZEVariant(const ZEVariant& Value);
								ZEVariant(const char* Value);
								ZEVariant(const ZEString& Value);
								ZEVariant(ZEInt8 Value);
								ZEVariant(ZEInt16 Value);
								ZEVariant(ZEInt32 Value);
								ZEVariant(ZEInt64 Value);
								ZEVariant(ZEUInt8 Value);
								ZEVariant(ZEUInt16 Value);
								ZEVariant(ZEUInt32 Value);
								ZEVariant(ZEUInt64 Value);
								ZEVariant(float Value);
								ZEVariant(double Value);
								ZEVariant(bool Value);
								ZEVariant(const ZEVector2& Value);
								ZEVariant(const ZEVector3& Value);
								ZEVariant(const ZEVector4& Value);
								ZEVariant(const ZEQuaternion& Quaternion);
								ZEVariant(const ZEMatrix3x3& Value);
								ZEVariant(const ZEMatrix4x4& Value);
								ZEVariant(ZEObject* Value);
								~ZEVariant();
};


#pragma warning(pop)
#endif
