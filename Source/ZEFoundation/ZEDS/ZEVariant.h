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

class ZESerializer;
class ZEUnserializer;

enum ZEVariantType
{
	ZE_VRT_UNDEFINED = 0,
	ZE_VRT_NULL,
	ZE_VRT_FLOAT,
	ZE_VRT_INTEGER,
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
		union ZEVariantValue
		{
			float			Float;
			ZEInt			Integer;
			bool			Boolean;
			char*			String;
			ZEObject*		Pointer;
			
			struct
			{
				float A, B, C, D;
			} Vectors;

			ZEMatrix3x3*	Matrix3x3;
			ZEMatrix4x4*	Matrix4x4;
		};

	protected:
		ZEVariantType		Type;
		ZEVariantValue		Value;

	public:
		void				SetType	(ZEVariantType NewType);
		ZEVariantType		GetType	() const;

		ZEVariantValue		GetValue() const;
		ZEVariantValue&		ImplicitAcesss();

		void				SetString(const char *NewValue);
		void				SetInteger(ZEInt Value);
		void				SetFloat(float Value);
		void				SetBoolean(bool Value);
		void				SetVector2(const ZEVector2& Vector);
		void				SetVector3(const ZEVector3& Vector);
		void				SetVector4(const ZEVector4& Vector);
		void				SetQuaternion(const ZEQuaternion& Quaternion);
		void				SetMatrix3x3(const ZEMatrix3x3& Matrix);
		void				SetMatrix4x4(const ZEMatrix4x4& Matrix);
		void				SetClass(ZEObject* Pointer);
		void				SetNull();
		void				SetVariant(const ZEVariant& NewValue);

		char*				GetString() const;
		ZEInt				GetInteger() const;
		float				GetFloat() const;
		bool				GetBoolean() const;
		ZEVector2&			GetVector2() const;
		ZEVector3&			GetVector3() const;
		ZEVector4&			GetVector4() const;
		ZEQuaternion&		GetQuaternion() const;
		ZEMatrix3x3&		GetMatrix3x3() const;
		ZEMatrix4x4&		GetMatrix4x4() const;
		ZEObject*			GetClass() const;
		
		bool				IsNull() const;
		ZESize				SizeOf() const;

		bool				Serialize(ZESerializer* Serializer);
		bool				Unserialize(ZEUnserializer* Unserializer);

		void				operator= (const ZEVariant &NewValue);
		void				operator= (const char* NewValue);
		void				operator= (ZEInt NewValue);
		void				operator= (float NewValue);
		void				operator= (bool NewValue);
		void				operator= (const ZEVector2& Vector);
		void				operator= (const ZEVector3& Vector);
		void				operator= (const ZEVector4& Vector);
		void				operator= (const ZEQuaternion& Quaternion);
		void				operator= (const ZEMatrix3x3& Matrix);
		void				operator= (const ZEMatrix4x4& Matrix);
		void				operator= (ZEObject* Pointer);

							operator const char*();
							operator ZEInt();
							operator float();
							operator bool();
							operator ZEVector2();
							operator ZEVector3();
							operator ZEVector4();
							operator ZEQuaternion();
							operator ZEMatrix3x3();
							operator ZEMatrix4x4();
							operator ZEObject*();

							ZEVariant();
							ZEVariant(const ZEVariant &InitialValue);
							ZEVariant(const char* InitialValue);	
							ZEVariant(ZEInt InitialValue);
							ZEVariant(float InitialValue);
							ZEVariant(bool InitialValue);
							ZEVariant(const ZEVector2& Vector);
							ZEVariant(const ZEVector3& Vector);
							ZEVariant(const ZEVector4& Vector);
							ZEVariant(const ZEQuaternion& Quaternion);
							ZEVariant(const ZEMatrix3x3& Matrix);
							ZEVariant(const ZEMatrix4x4& Matrix);
							ZEVariant(ZEObject* Pointer);
							~ZEVariant();
};


#pragma warning(pop)
#endif





