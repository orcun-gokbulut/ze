//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEValue.cpp
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

///////////////////////////////////////////////////////////////
//  Orcun's Data Structres Version 2.0 (Zinek Engine)		 //
// --------------------------------------------------------- //
//  Copyrights (C) 2005-2007,  Y. Orçun GÖKBULUT			 //
//  All rights reserved.									 //
///////////////////////////////////////////////////////////////

#include "ZEValue.h"
#include "ZETypes.h"
#include "ZEError.h"
#include "ZESerialization/ZESerializer.h"
#include "ZESerialization/ZEUnserializer.h"

#include <string.h>

#pragma warning(push)
#pragma warning(disable:4482 4996)

void ZEValue::SetType(ZEValueType NewType)
{
	if (this->Type == NewType)
		return;

	switch (Type)
	{
		case ZE_VRT_STRING:
			Value.String.Clear();
			break;
		
		case ZE_VRT_MATRIX3X3:
			delete Value.Matrix3x3;
			Value.Matrix3x3 = NULL;
			break;

		case ZE_VRT_MATRIX3X3D:
			delete Value.Matrix3x3d;
			Value.Matrix3x3d = NULL;
			break;

		case ZE_VRT_MATRIX4X4:
			delete Value.Matrix4x4;
			Value.Matrix4x4 = NULL;
			break;

		case ZE_VRT_MATRIX4X4D:
			delete Value.Matrix4x4d;
			Value.Matrix4x4d = NULL;
			break;
	}

	switch(NewType)
	{
		case ZE_VRT_MATRIX3X3:
			Value.Matrix3x3 = new ZEMatrix3x3();
			break;

		case ZE_VRT_MATRIX3X3D:
			Value.Matrix3x3d = new ZEMatrix3x3d();
			break;

		case ZE_VRT_MATRIX4X4:
			Value.Matrix4x4 = new ZEMatrix4x4();
			break;

		case ZE_VRT_MATRIX4X4D:
			Value.Matrix4x4d = new ZEMatrix4x4d();
			break;
	}

	Type = NewType;
}

ZEValueType ZEValue::GetType() const
{
	return Type;
}

ZEValue::ZEValueValue ZEValue::GetValue()	const
{
	return Value;
}

ZEValue::ZEValueValue& ZEValue::ImplicitAcesss()
{
	return Value;
}

void ZEValue::SetValue(const ZEValue& NewValue)
{
	switch(NewValue.Type)
	{
		case ZE_VRT_UNDEFINED:
			SetNull();
			Type = ZE_VRT_UNDEFINED;
			break;

		case ZE_VRT_NULL:
			SetNull();
			break;

		case ZE_VRT_FLOAT:
			SetFloat(NewValue.GetFloat());
			break;

		case ZE_VRT_DOUBLE:
			SetDouble(NewValue.GetDouble());
			break;

		case ZE_VRT_INTEGER_8:
			SetInt8(NewValue.GetInt8());
			break;

		case ZE_VRT_INTEGER_16:
			SetInt16(NewValue.GetInt16());
			break;

		case ZE_VRT_INTEGER_32:
			SetInt32(NewValue.GetInt32());
			break;

		case ZE_VRT_INTEGER_64:
			SetInt64(NewValue.GetInt64());
			break;

		case ZE_VRT_UNSIGNED_INTEGER_8:
			SetUInt8(NewValue.GetUInt8());
			break;

		case ZE_VRT_UNSIGNED_INTEGER_16:
			SetUInt16(NewValue.GetUInt16());
			break;

		case ZE_VRT_UNSIGNED_INTEGER_32:
			SetUInt32(NewValue.GetUInt32());
			break;

		case ZE_VRT_UNSIGNED_INTEGER_64:
			SetUInt64(NewValue.GetUInt64());
			break;

		case ZE_VRT_BOOLEAN:
			SetBoolean(NewValue.GetBoolean());
			break;

		case ZE_VRT_STRING:
			SetString(NewValue.GetString());
			break;

		case ZE_VRT_QUATERNION:
			SetQuaternion(NewValue.GetQuaternion());
			break;

		case ZE_VRT_VECTOR2:
			SetVector2(NewValue.GetVector2());
			break;

		case ZE_VRT_VECTOR2D:
			SetVector2d(NewValue.GetVector2d());
			break;

		case ZE_VRT_VECTOR3:
			SetVector3(NewValue.GetVector3());
			break;

		case ZE_VRT_VECTOR3D:
			SetVector3d(NewValue.GetVector3d());
			break;

		case ZE_VRT_VECTOR4:
			SetVector4(NewValue.GetVector4());
			break;

		case ZE_VRT_VECTOR4D:
			SetVector4d(NewValue.GetVector4d());
			break;

		case ZE_VRT_MATRIX3X3:
			SetMatrix3x3(NewValue.GetMatrix3x3());
			break;

		case ZE_VRT_MATRIX3X3D:
			SetMatrix3x3d(NewValue.GetMatrix3x3d());
			break;

		case ZE_VRT_MATRIX4X4:
			SetMatrix4x4(NewValue.GetMatrix4x4());
			break;

		case ZE_VRT_MATRIX4X4D:
			SetMatrix4x4d(NewValue.GetMatrix4x4d());
			break;

		case ZE_VRT_CLASS:
			SetClass(NewValue.GetClass());
			break;

		default:
			zeDebugCheck(true, "ZEValue::SetValue operation failed. Error in Value type.");
	}
}

ZESize ZEValue::SizeOf() const
{
	switch(Type)
	{
		case ZE_VRT_UNDEFINED:
			return 0;

		case ZE_VRT_NULL:
			return 0;

		case ZE_VRT_FLOAT:
			return sizeof(float);

		case ZE_VRT_DOUBLE:
			return sizeof(double);

		case ZE_VRT_INTEGER_8:
			return sizeof(ZEInt8);

		case ZE_VRT_INTEGER_16:
			return sizeof(ZEInt16);

		case ZE_VRT_INTEGER_32:
			return sizeof(ZEInt32);

		case ZE_VRT_INTEGER_64:
			return sizeof(ZEInt64);

		case ZE_VRT_UNSIGNED_INTEGER_8:
			return sizeof(ZEUInt8);

		case ZE_VRT_UNSIGNED_INTEGER_16:
			return sizeof(ZEUInt16);

		case ZE_VRT_UNSIGNED_INTEGER_32:
			return sizeof(ZEUInt32);

		case ZE_VRT_UNSIGNED_INTEGER_64:
			return sizeof(ZEUInt64);

		case ZE_VRT_BOOLEAN:
			return sizeof(bool);

		case ZE_VRT_STRING:
			return this->Value.String.GetSize();

		case ZE_VRT_QUATERNION:
			return sizeof(ZEQuaternion);

		case ZE_VRT_VECTOR2:
			return sizeof(ZEVector2);

		case ZE_VRT_VECTOR2D:
			return sizeof(ZEVector2d);

		case ZE_VRT_VECTOR3:
			return sizeof(ZEVector3);

		case ZE_VRT_VECTOR3D:
			return sizeof(ZEVector3d);

		case ZE_VRT_VECTOR4:
			return sizeof(ZEVector4);

		case ZE_VRT_VECTOR4D:
			return sizeof(ZEVector4d);

		case ZE_VRT_MATRIX3X3:
			return sizeof(ZEMatrix3x3);

		case ZE_VRT_MATRIX3X3D:
			return sizeof(ZEMatrix3x3d);

		case ZE_VRT_MATRIX4X4:
			return sizeof(ZEMatrix4x4);

		case ZE_VRT_MATRIX4X4D:
			return sizeof(ZEMatrix4x4d);

		case ZE_VRT_CLASS:
			return sizeof(void*);

		default:
			zeDebugCheck(true, "ZEValue::SizeOf is wrong type.");
			return 0;
	}
}

void ZEValue::Clear()
{
	if (Type == ZE_VRT_MATRIX3X3)
	{
		memset(Value.Matrix3x3, 0, sizeof(ZEMatrix3x3));
	}
	else if (Type == ZE_VRT_MATRIX3X3D)
	{
		memset(Value.Matrix3x3d, 0, sizeof(ZEMatrix3x3d));
	}
	else if (Type == ZE_VRT_MATRIX4X4)
	{
		memset(Value.Matrix4x4, 0, sizeof(ZEMatrix4x4));
	}
	else if (Type == ZE_VRT_MATRIX4X4D)
	{
		memset(Value.Matrix4x4d, 0, sizeof(ZEMatrix4x4d));
	}
	else if (Type == ZE_VRT_STRING)
	{
		Value.String.Clear();
	}
	else
	{
		memset(&Value, 0, sizeof(Value));
	}
}

bool ZEValue::Serialize(ZESerializer* Serializer)
{

	ZEUInt32 _Type = Type;
	ZEUInt32 StringSize;
	Serializer->Write(&_Type, sizeof(ZEUInt32), 1);

	switch(Type)
	{
		case ZE_VRT_UNDEFINED:
		case ZE_VRT_NULL:
			break;

		case ZE_VRT_FLOAT:
			Serializer->Write(&Value.Float, sizeof(float), 1);
			break;

		case ZE_VRT_DOUBLE:
			Serializer->Write(&Value.Double, sizeof(double), 1);
			break;

		case ZE_VRT_INTEGER_8:
			Serializer->Write(&Value.Int32, sizeof(ZEInt8), 1);
			break;

		case ZE_VRT_INTEGER_16:
			Serializer->Write(&Value.Int32, sizeof(ZEInt16), 1);
			break;

		case ZE_VRT_INTEGER_32:
			Serializer->Write(&Value.Int32, sizeof(ZEInt32), 1);
			break;

		case ZE_VRT_INTEGER_64:
			Serializer->Write(&Value.Int64, sizeof(ZEInt64), 1);
			break;

		case ZE_VRT_UNSIGNED_INTEGER_8:
			Serializer->Write(&Value.Int32, sizeof(ZEUInt8), 1);
			break;

		case ZE_VRT_UNSIGNED_INTEGER_16:
			Serializer->Write(&Value.Int32, sizeof(ZEUInt16), 1);
			break;

		case ZE_VRT_UNSIGNED_INTEGER_32:
			Serializer->Write(&Value.Int32, sizeof(ZEUInt32), 1);
			break;

		case ZE_VRT_UNSIGNED_INTEGER_64:
			Serializer->Write(&Value.Int64, sizeof(ZEUInt64), 1);
			break;

		case ZE_VRT_BOOLEAN:
			Serializer->Write(&Value.Boolean, sizeof(bool), 1);
			break;

		case ZE_VRT_STRING:
			StringSize = (ZEUInt32)this->Value.String.GetSize();
			Serializer->Write(&StringSize, sizeof(ZEUInt32), 1);
			Serializer->Write(Value.String.GetValue(), sizeof(char), StringSize);
			break;

		case ZE_VRT_QUATERNION:
			Serializer->Write(&Value.Vectors, sizeof(ZEQuaternion), 1);
			break;

		case ZE_VRT_VECTOR2:
			Serializer->Write(&Value.Vectors, sizeof(ZEVector2), 1);
			break;

		case ZE_VRT_VECTOR2D:
			Serializer->Write(&Value.DoubleVectors, sizeof(ZEVector2d), 1);
			break;

		case ZE_VRT_VECTOR3:
			Serializer->Write(&Value.Vectors, sizeof(ZEVector3), 1);
			break;

		case ZE_VRT_VECTOR3D:
			Serializer->Write(&Value.DoubleVectors, sizeof(ZEVector3d), 1);
			break;

		case ZE_VRT_VECTOR4:
			Serializer->Write(&Value.Vectors, sizeof(ZEVector4), 1);
			break;

		case ZE_VRT_VECTOR4D:
			Serializer->Write(&Value.DoubleVectors, sizeof(ZEVector4d), 1);
			break;

		case ZE_VRT_MATRIX3X3:
			Serializer->Write(Value.Matrix3x3, sizeof(ZEMatrix3x3), 1);
			break;

		case ZE_VRT_MATRIX3X3D:
			Serializer->Write(Value.Matrix3x3d, sizeof(ZEMatrix3x3d), 1);
			break;

		case ZE_VRT_MATRIX4X4:
			Serializer->Write(Value.Matrix4x4, sizeof(ZEMatrix4x4), 1);
			break;

		case ZE_VRT_MATRIX4X4D:
			Serializer->Write(Value.Matrix4x4d, sizeof(ZEMatrix4x4d), 1);
			break;

		case ZE_VRT_CLASS:
			zeDebugCheck(true, "Can not unserialize pointer type.");
			return false;

		default:
			zeDebugCheck(true, "Wrong Value type.");
			return false;
	}
	return true;
}

bool ZEValue::Unserialize(ZEUnserializer* Unserializer)
{
	ZEUInt32 Type;
	ZEUInt32 StringSize;
	Unserializer->Read(&Type, sizeof(ZEUInt32), 1);

	switch(Type)
	{
		case ZE_VRT_UNDEFINED:
			SetType(ZE_VRT_UNDEFINED);
			break;

		case ZE_VRT_NULL:
			SetType(ZE_VRT_NULL);
			break;

		case ZE_VRT_FLOAT:
			SetType(ZE_VRT_FLOAT);
			Unserializer->Read(&Value.Float, sizeof(float), 1);
			break;

		case ZE_VRT_DOUBLE:
			SetType(ZE_VRT_DOUBLE);
			Unserializer->Read(&Value.Double, sizeof(double), 1);
			break;

		case ZE_VRT_INTEGER_8:
			SetType(ZE_VRT_INTEGER_8);
			Unserializer->Read(&Value.Int32, sizeof(ZEInt8), 1);
			break;

		case ZE_VRT_INTEGER_16:
			SetType(ZE_VRT_INTEGER_16);
			Unserializer->Read(&Value.Int32, sizeof(ZEInt16), 1);
			break;

		case ZE_VRT_INTEGER_32:
			SetType(ZE_VRT_INTEGER_32);
			Unserializer->Read(&Value.Int32, sizeof(ZEInt32), 1);
			break;

		case ZE_VRT_INTEGER_64:
			SetType(ZE_VRT_INTEGER_64);
			Unserializer->Read(&Value.Int64, sizeof(ZEInt64), 1);
			break;

		case ZE_VRT_UNSIGNED_INTEGER_8:
			SetType(ZE_VRT_UNSIGNED_INTEGER_8);
			Unserializer->Read(&Value.Int32, sizeof(ZEUInt8), 1);
			break;

		case ZE_VRT_UNSIGNED_INTEGER_16:
			SetType(ZE_VRT_UNSIGNED_INTEGER_16);
			Unserializer->Read(&Value.Int32, sizeof(ZEUInt16), 1);
			break;

		case ZE_VRT_UNSIGNED_INTEGER_32:
			SetType(ZE_VRT_UNSIGNED_INTEGER_32);
			Unserializer->Read(&Value.Int32, sizeof(ZEUInt32), 1);
			break;

		case ZE_VRT_UNSIGNED_INTEGER_64:
			SetType(ZE_VRT_UNSIGNED_INTEGER_64);
			Unserializer->Read(&Value.Int64, sizeof(ZEUInt64), 1);
			break;

		case ZE_VRT_BOOLEAN:
			SetType(ZE_VRT_BOOLEAN);
			Unserializer->Read(&Value.Boolean, sizeof(bool), 1);
			break;

		case ZE_VRT_STRING:
		{
			SetType(ZE_VRT_STRING);
			Unserializer->Read(&StringSize, sizeof(ZEUInt32), 1);
			char* Buffer = new char[StringSize];
			Unserializer->Read(Buffer, sizeof(char), StringSize);
			Buffer[StringSize - 1] = '\0';
			Value.String.SetBuffer(Buffer, StringSize);
			break;
		}
		case ZE_VRT_QUATERNION:
			SetType(ZE_VRT_QUATERNION);
			Unserializer->Read(&Value.Vectors, sizeof(ZEQuaternion), 1);
			break;

		case ZE_VRT_VECTOR2:
			SetType(ZE_VRT_VECTOR2);
			Unserializer->Read(&Value.Vectors, sizeof(ZEVector2), 1);
			break;

		case ZE_VRT_VECTOR2D:
			SetType(ZE_VRT_VECTOR2D);
			Unserializer->Read(&Value.DoubleVectors, sizeof(ZEVector2d), 1);
			break;

		case ZE_VRT_VECTOR3:
			SetType(ZE_VRT_VECTOR3);
			Unserializer->Read(&Value.Vectors, sizeof(ZEVector3), 1);
			break;

		case ZE_VRT_VECTOR3D:
			SetType(ZE_VRT_VECTOR3D);
			Unserializer->Read(&Value.DoubleVectors, sizeof(ZEVector3d), 1);
			break;

		case ZE_VRT_VECTOR4:
			SetType(ZE_VRT_VECTOR4);
			Unserializer->Read(&Value.Vectors, sizeof(ZEVector4), 1);
			break;

		case ZE_VRT_VECTOR4D:
			SetType(ZE_VRT_VECTOR4D);
			Unserializer->Read(&Value.DoubleVectors, sizeof(ZEVector4d), 1);
			break;

		case ZE_VRT_MATRIX3X3:
			SetType(ZE_VRT_MATRIX3X3);
			Unserializer->Read(Value.Matrix3x3, sizeof(ZEMatrix3x3), 1);
			break;

		case ZE_VRT_MATRIX3X3D:
			SetType(ZE_VRT_MATRIX3X3D);
			Unserializer->Read(Value.Matrix3x3d, sizeof(ZEMatrix3x3d), 1);
			break;

		case ZE_VRT_MATRIX4X4:
			SetType(ZE_VRT_MATRIX4X4);
			Unserializer->Read(Value.Matrix4x4, sizeof(ZEMatrix4x4), 1);
			break;

		case ZE_VRT_MATRIX4X4D:
			SetType(ZE_VRT_MATRIX4X4D);
			Unserializer->Read(Value.Matrix4x4d, sizeof(ZEMatrix4x4d), 1);
			break;

		case ZE_VRT_CLASS:
			SetType(ZE_VRT_UNDEFINED);
			zeDebugCheck(true, "Can not unserialize pointer type.");
			return false;

		default:
			SetType(ZE_VRT_UNDEFINED);
			zeDebugCheck(true, "Wrong Value type.");
			return false;
	}
	return true;
}

bool ZEValue::IsInteger() const
{
	return Type == ZE_VRT_INTEGER_8 ||
		Type == ZE_VRT_INTEGER_16 ||
		Type == ZE_VRT_INTEGER_32 ||
		Type == ZE_VRT_INTEGER_64 ||
		Type == ZE_VRT_UNSIGNED_INTEGER_8 ||
		Type == ZE_VRT_UNSIGNED_INTEGER_16 ||
		Type == ZE_VRT_UNSIGNED_INTEGER_32 ||
		Type == ZE_VRT_UNSIGNED_INTEGER_64;
}
bool ZEValue::IsFloatingPoint() const
{
	return ZE_VRT_FLOAT || ZE_VRT_DOUBLE;
}

bool ZEValue::IsUnsignedInteger() const
{
	return Type == ZE_VRT_UNSIGNED_INTEGER_8 ||
		Type == ZE_VRT_UNSIGNED_INTEGER_16 ||
		Type == ZE_VRT_UNSIGNED_INTEGER_32 ||
		Type == ZE_VRT_UNSIGNED_INTEGER_64;
}

bool ZEValue::IsSignedInteger() const
{
	return Type == ZE_VRT_INTEGER_8 ||
		Type == ZE_VRT_INTEGER_16 ||
		Type == ZE_VRT_INTEGER_32 ||
		Type == ZE_VRT_INTEGER_64;
}

bool ZEValue::IsVector() const
{
	return Type == ZE_VRT_VECTOR2 || Type == ZE_VRT_VECTOR3 || Type == ZE_VRT_VECTOR4;
}

bool ZEValue::IsVectord() const
{
	return Type == ZE_VRT_VECTOR2D || Type == ZE_VRT_VECTOR3D || Type == ZE_VRT_VECTOR4D;
}

bool ZEValue::IsMatrix() const
{
	return Type == ZE_VRT_MATRIX3X3 || ZE_VRT_MATRIX4X4;
}

bool ZEValue::IsMatrixd() const
{
	return Type == ZE_VRT_MATRIX3X3D || ZE_VRT_MATRIX4X4D;
}

ZEInt64 ZEValue::GetIntegerRangeMin() const
{
	if (IsUnsignedInteger())
		return 0;

	if (Type == ZE_VRT_INTEGER_8)
		return -128;
	else if (Type == ZE_VRT_INTEGER_16)
		return -32768;
	else if (Type == ZE_VRT_INTEGER_32)
		return INT_MIN;
	else if (Type == ZE_VRT_INTEGER_64)
		return -9223372036854775808ll;

	return 0;
}

ZEUInt64 ZEValue::GetIntegerRangeMax() const
{
	if (Type == ZE_VRT_INTEGER_8)
		return 127;
	else if (Type == ZE_VRT_INTEGER_16)
		return 32767;
	else if (Type == ZE_VRT_INTEGER_32)
		return  2147483647;
	else if (Type == ZE_VRT_INTEGER_64)
		return 9223372036854775807;
	else if (Type == ZE_VRT_UNSIGNED_INTEGER_8)
		return 255;
	else if (Type == ZE_VRT_UNSIGNED_INTEGER_16)
		return 65535;
	else if (Type == ZE_VRT_UNSIGNED_INTEGER_32)
		return  4294967295;
	else if (Type == ZE_VRT_UNSIGNED_INTEGER_64)
		return 18446744073709551615;

	return 0;
}

void ZEValue::SetString(const char* Value)
{
	SetType(ZE_VRT_STRING);
	this->Value.String.SetValue(Value);
}

void ZEValue::SetString(const ZEString& Value)
{
	SetType(ZE_VRT_STRING);
	this->Value.String.SetValue(Value);
}

void ZEValue::SetInt8(ZEInt8 Value)
{
	Type = ZE_VRT_INTEGER_8;
	this->Value.Int32 = (ZEInt32)Value;
}

void ZEValue::SetInt16(ZEInt16 Value)
{
	Type = ZE_VRT_INTEGER_16;
	this->Value.Int32 = (ZEInt32)Value;
}

void ZEValue::SetInt32(ZEInt32 Value)
{
	Type = ZE_VRT_INTEGER_32;
	this->Value.Int32 = Value;
}

void ZEValue::SetInt64(ZEInt64 Value)
{
	Type = ZE_VRT_INTEGER_64;
	this->Value.Int64 = Value;
}

void ZEValue::SetUInt8(ZEUInt8 Value)
{
	Type = ZE_VRT_UNSIGNED_INTEGER_8;
	this->Value.Int32 = Value;
}

void ZEValue::SetUInt16(ZEUInt16 Value)
{
	Type = ZE_VRT_UNSIGNED_INTEGER_16;
	this->Value.Int32 = Value;
}

void ZEValue::SetUInt32(ZEUInt32 Value)
{
	Type = ZE_VRT_UNSIGNED_INTEGER_32;
	this->Value.Int32 = Value;
}

void ZEValue::SetUInt64(ZEUInt64 Value)
{
	Type = ZE_VRT_UNSIGNED_INTEGER_64;
	this->Value.Int64 = Value;
}

void ZEValue::SetFloat(float Value)
{
	Type = ZE_VRT_FLOAT;
	this->Value.Float = Value;
}

void ZEValue::SetDouble(double Value)
{
	Type = ZE_VRT_DOUBLE;
	this->Value.Double = Value;
}

void ZEValue::SetBoolean(bool Value)
{
	SetType(ZE_VRT_BOOLEAN);
	this->Value.Boolean = Value;
}

void ZEValue::SetVector2(const ZEVector2& Value)
{
	SetType(ZE_VRT_VECTOR2);
	(*(ZEVector2*)(&this->Value.Vectors)) = Value;
}

void ZEValue::SetVector2d(const ZEVector2d& Value)
{
	SetType(ZE_VRT_VECTOR2D);
	(*(ZEVector2d*)(&this->Value.DoubleVectors)) = Value;
}

void ZEValue::SetVector3(const ZEVector3& Value)
{
	SetType(ZE_VRT_VECTOR3);
  	(*(ZEVector3*)(&this->Value.Vectors)) = Value;
}

void ZEValue::SetVector3d(const ZEVector3d& Value)
{
	SetType(ZE_VRT_VECTOR3D);
	(*(ZEVector3d*)(&this->Value.DoubleVectors)) = Value;
}

void ZEValue::SetVector4(const ZEVector4& Value)
{
	SetType(ZE_VRT_VECTOR4);
  	(*(ZEVector4*)(&this->Value.Vectors)) = Value;
}

void ZEValue::SetVector4d(const ZEVector4d& Value)
{
	SetType(ZE_VRT_VECTOR4D);
	(*(ZEVector4d*)(&this->Value.DoubleVectors)) = Value;
}

void ZEValue::SetQuaternion(const ZEQuaternion& Value)
{
	SetType(ZE_VRT_QUATERNION);
	(*(ZEQuaternion*)(&this->Value.Vectors)) = Value;
}

void ZEValue::SetMatrix3x3(const ZEMatrix3x3& Value)
{
	SetType(ZE_VRT_MATRIX3X3);
	*this->Value.Matrix3x3 = Value;
}

void ZEValue::SetMatrix3x3d(const ZEMatrix3x3d& Value)
{
	SetType(ZE_VRT_MATRIX3X3D);
	*this->Value.Matrix3x3d = Value;
}

void ZEValue::SetMatrix4x4(const ZEMatrix4x4& Value)
{
	SetType(ZE_VRT_MATRIX4X4);
	*this->Value.Matrix4x4 = Value;
}

void ZEValue::SetMatrix4x4d(const ZEMatrix4x4d& Value)
{
	SetType(ZE_VRT_MATRIX4X4D);
	*this->Value.Matrix4x4d = Value;
}

void ZEValue::SetClass(ZEObject* Value)
{
	SetType(ZE_VRT_CLASS);
	this->Value.Pointer = Value;
}

void ZEValue::SetNull()
{
	SetType(ZE_VRT_NULL);
}

const ZEString& ZEValue::GetString() const
{
	zeDebugCheck(this->Type != ZE_VRT_STRING, "ZEValue::GetString operation failed. Value type mismatched.");
	return Value.String;
}

ZEInt8 ZEValue::GetInt8() const
{
	zeDebugCheck(this->Type != ZE_VRT_INTEGER_8, "ZEValue::GetInt8 operation failed. Value type mismatched.");
	return (ZEInt8)Value.Int32;
}

ZEInt16 ZEValue::GetInt16() const
{
	zeDebugCheck(this->Type != ZE_VRT_INTEGER_16, "ZEValue::GetInt16 operation failed. Value type mismatched.");
	return (ZEInt16)Value.Int32;
}

ZEInt32 ZEValue::GetInt32() const
{
	zeDebugCheck(this->Type != ZE_VRT_INTEGER_32, "ZEValue::GetInt32 operation failed. Value type mismatched.");
	return Value.Int32;
}

ZEInt64 ZEValue::GetInt64() const
{
	zeDebugCheck(this->Type != ZE_VRT_INTEGER_64, "ZEValue::GetInt64 operation failed. Value type mismatched.");
	return Value.Int64;
}

ZEUInt8 ZEValue::GetUInt8() const
{
	zeDebugCheck(this->Type != ZE_VRT_UNSIGNED_INTEGER_8, "ZEValue::GetUInt8 operation failed. Value type mismatched.");
	return (ZEUInt8)Value.Int32;
}

ZEUInt16 ZEValue::GetUInt16() const
{
	zeDebugCheck(this->Type != ZE_VRT_UNSIGNED_INTEGER_16, "ZEValue::GetUInt16 operation failed. Value type mismatched.");
	return (ZEUInt16)Value.Int32;
}

ZEUInt32 ZEValue::GetUInt32() const
{
	zeDebugCheck(this->Type != ZE_VRT_UNSIGNED_INTEGER_32, "ZEValue::GetUInt32 operation failed. Value type mismatched.");
	return Value.Int32;
}

ZEUInt64 ZEValue::GetUInt64() const
{
	zeDebugCheck(this->Type != ZE_VRT_UNSIGNED_INTEGER_64, "ZEValue::GetUInt64 operation failed. Value type mismatched.");
	return Value.Int64;
}

float ZEValue::GetFloat() const
{
	zeDebugCheck(this->Type != ZE_VRT_FLOAT, "ZEValue::GetFloat operation failed. Value type mismatched.");
	return Value.Float;
}

double ZEValue::GetDouble() const
{
	zeDebugCheck(this->Type != ZE_VRT_DOUBLE, "ZEValue::GetDouble operation failed. Value type mismatched.");
	return Value.Double;
}

bool ZEValue::GetBoolean() const
{
	zeDebugCheck(this->Type != ZE_VRT_BOOLEAN, "ZEValue::GetBoolean operation failed. Value type mismatched.");
	return Value.Boolean;
}

const ZEVector2& ZEValue::GetVector2() const
{
	zeDebugCheck(this->Type != ZE_VRT_VECTOR2, "ZEValue::GetVector2 operation failed. Value type mismatched.");
	return *((ZEVector2*)&Value.Vectors);
}

const ZEVector2d& ZEValue::GetVector2d() const
{
	zeDebugCheck(this->Type != ZE_VRT_VECTOR2D, "ZEValue::GetVector2d operation failed. Value type mismatched.");
	return *((ZEVector2d*)&Value.DoubleVectors);
}

const ZEVector3& ZEValue::GetVector3() const
{
	zeDebugCheck(this->Type != ZE_VRT_VECTOR3, "ZEValue::GetVector3 operation failed. Value type mismatched.");
	return *((ZEVector3*)&Value.Vectors);
}

const ZEVector3d& ZEValue::GetVector3d() const
{
	zeDebugCheck(this->Type != ZE_VRT_VECTOR3D, "ZEValue::GetVector3d operation failed. Value type mismatched.");
	return *((ZEVector3d*)&Value.DoubleVectors);
}

const ZEVector4& ZEValue::GetVector4() const
{
	zeDebugCheck(this->Type != ZE_VRT_VECTOR4, "ZEValue::GetVector4 operation failed. Value type mismatched.");
	return *((ZEVector4*)&Value.Vectors);
}

const ZEVector4d& ZEValue::GetVector4d() const
{
	zeDebugCheck(this->Type != ZE_VRT_VECTOR4D, "ZEValue::GetVector4d operation failed. Value type mismatched.");
	return *((ZEVector4d*)&Value.DoubleVectors);
}

const ZEQuaternion& ZEValue::GetQuaternion() const
{
	zeDebugCheck(this->Type != ZE_VRT_QUATERNION, "ZEValue::Quaternion operation failed. Value type mismatched.");
	return *((ZEQuaternion*)&Value.Vectors);
}

const ZEMatrix3x3& ZEValue::GetMatrix3x3() const
{
	zeDebugCheck(this->Type != ZE_VRT_MATRIX3X3, "ZEValue::GetMatrix3x3 operation failed. Value type mismatched.");
	return *Value.Matrix3x3;
}

const ZEMatrix3x3d& ZEValue::GetMatrix3x3d() const
{
	zeDebugCheck(this->Type != ZE_VRT_MATRIX3X3D, "ZEValue::GetMatrix3x3d operation failed. Value type mismatched.");
	return *Value.Matrix3x3d;
}

const ZEMatrix4x4& ZEValue::GetMatrix4x4() const
{
	zeDebugCheck(this->Type != ZE_VRT_MATRIX4X4, "ZEValue::GetMatrix4x4 operation failed. Value type mismatched.");
	return *Value.Matrix4x4;
}

const ZEMatrix4x4d& ZEValue::GetMatrix4x4d() const
{
	zeDebugCheck(this->Type != ZE_VRT_MATRIX4X4D, "ZEValue::GetMatrix4x4d operation failed. Value type mismatched.");
	return *Value.Matrix4x4d;
}

ZEObject* ZEValue::GetClass() const
{
	zeDebugCheck(this->Type != ZE_VRT_CLASS, "ZEValue::GetClass operation failed. Value type mismatched.");
	return Value.Pointer;
}

bool ZEValue::IsNull()	const
{
	return (Type == ZE_VRT_NULL);
}

ZEValue& ZEValue::operator=(const ZEValue& NewValue)
{
	SetValue(NewValue);
	return *this;
}

ZEValue& ZEValue::operator=(const char* NewValue)
{
	SetString(NewValue);
	return *this;
}

ZEValue& ZEValue::operator=(const ZEString& NewValue)
{
	SetString(NewValue.ToCString());
	return *this;
}

ZEValue& ZEValue::operator=(ZEInt8 NewValue)
{
	SetInt8(NewValue);
	return *this;
}

ZEValue& ZEValue::operator=(ZEInt16 NewValue)
{
	SetInt16(NewValue);
	return *this;
}

ZEValue& ZEValue::operator=(ZEInt32 NewValue)
{
	SetInt32(NewValue);
	return *this;
}

ZEValue& ZEValue::operator=(ZEInt64 NewValue)
{
	SetInt64(NewValue);
	return *this;
}

ZEValue& ZEValue::operator=(ZEUInt8 NewValue)
{
	SetUInt8(NewValue);
	return *this;
}

ZEValue& ZEValue::operator=(ZEUInt16 NewValue)
{
	SetUInt16(NewValue);
	return *this;
}

ZEValue& ZEValue::operator=(ZEUInt32 NewValue)
{
	SetUInt32(NewValue);
	return *this;
}

ZEValue& ZEValue::operator=(ZEUInt64 NewValue)
{
	SetUInt64(NewValue);
	return *this;
}

ZEValue& ZEValue::operator=(float NewValue)
{
	SetFloat(NewValue);
	return *this;
}

ZEValue& ZEValue::operator=(double NewValue)
{
	SetDouble(NewValue);
	return *this;
}

ZEValue& ZEValue::operator=(bool NewValue)
{
	SetBoolean(NewValue);
	return *this;
}

ZEValue& ZEValue::operator=(const ZEVector2& Vector)
{
	SetVector2(Vector);
	return *this;
}

ZEValue& ZEValue::operator=(const ZEVector2d& Vector)
{
	SetVector2d(Vector);
	return *this;
}

ZEValue& ZEValue::operator=(const ZEVector3& Vector)
{
	SetVector3(Vector);
	return *this;
}

ZEValue& ZEValue::operator=(const ZEVector3d& Vector)
{
	SetVector3d(Vector);
	return *this;
}

ZEValue& ZEValue::operator=(const ZEVector4& Vector)
{
	SetVector4(Vector);
	return *this;
}

ZEValue& ZEValue::operator=(const ZEVector4d& Vector)
{
	SetVector4d(Vector);
	return *this;
}

ZEValue& ZEValue::operator=(const ZEQuaternion& Quaternion)
{
	SetQuaternion(Quaternion);
	return *this;
}

ZEValue& ZEValue::operator=(const ZEMatrix3x3& Matrix)
{
	SetMatrix3x3(Matrix);
	return *this;
}

ZEValue& ZEValue::operator=(const ZEMatrix3x3d& Matrix)
{
	SetMatrix3x3d(Matrix);
	return *this;
}

ZEValue& ZEValue::operator=(const ZEMatrix4x4& Matrix)
{
	SetMatrix4x4(Matrix);
	return *this;
}

ZEValue& ZEValue::operator=(const ZEMatrix4x4d& Matrix)
{
	SetMatrix4x4d(Matrix);
	return *this;
}

ZEValue& ZEValue::operator=(ZEObject* NewValue)
{
	SetClass(NewValue);
	return *this;
}

ZEValue::operator const char*()
{
	zeDebugCheck(this->Type != ZE_VRT_STRING, "String conversion operation failed. Value type mismatched.");
	return this->Value.String;
}

ZEValue::operator ZEInt8()
{
	zeDebugCheck(this->Type != ZE_VRT_FLOAT && this->Type != ZE_VRT_DOUBLE
				&& this->Type != ZE_VRT_UNSIGNED_INTEGER_8 && this->Type != ZE_VRT_UNSIGNED_INTEGER_16 
				&& this->Type != ZE_VRT_UNSIGNED_INTEGER_32 && this->Type != ZE_VRT_UNSIGNED_INTEGER_64
				&& this->Type != ZE_VRT_INTEGER_8 && this->Type != ZE_VRT_INTEGER_16 
				&& this->Type != ZE_VRT_INTEGER_32 && this->Type != ZE_VRT_INTEGER_64 
				, "Integer conversion operation failed. Value type mismatched.");

	switch(Type)
	{
		case ZE_VRT_FLOAT:
			return (ZEInt8)Value.Float;
			break;
		case ZE_VRT_DOUBLE:
			return (ZEInt8)Value.Double;
			break;
		case ZE_VRT_INTEGER_8:
		case ZE_VRT_INTEGER_16:
		case ZE_VRT_INTEGER_32:
		case ZE_VRT_UNSIGNED_INTEGER_8:
		case ZE_VRT_UNSIGNED_INTEGER_16:
		case ZE_VRT_UNSIGNED_INTEGER_32:
			return (ZEInt8)Value.Int32;
			break;
		case ZE_VRT_UNSIGNED_INTEGER_64:
		case ZE_VRT_INTEGER_64:
			return (ZEInt8)Value.Int64;
			break;
		default:
			return 0;
	}
}

ZEValue::operator ZEInt16()
{
	zeDebugCheck(this->Type != ZE_VRT_FLOAT && this->Type != ZE_VRT_DOUBLE
		&& this->Type != ZE_VRT_UNSIGNED_INTEGER_8 && this->Type != ZE_VRT_UNSIGNED_INTEGER_16 
		&& this->Type != ZE_VRT_UNSIGNED_INTEGER_32 && this->Type != ZE_VRT_UNSIGNED_INTEGER_64
		&& this->Type != ZE_VRT_INTEGER_8 && this->Type != ZE_VRT_INTEGER_16 
		&& this->Type != ZE_VRT_INTEGER_32 && this->Type != ZE_VRT_INTEGER_64 
		, "Integer conversion operation failed. Value type mismatched.");

	switch(Type)
	{
		case ZE_VRT_FLOAT:
			return (ZEInt16)Value.Float;
			break;
		case ZE_VRT_DOUBLE:
			return (ZEInt16)Value.Double;
			break;
		case ZE_VRT_INTEGER_8:
		case ZE_VRT_INTEGER_16:
		case ZE_VRT_INTEGER_32:
		case ZE_VRT_UNSIGNED_INTEGER_8:
		case ZE_VRT_UNSIGNED_INTEGER_16:
		case ZE_VRT_UNSIGNED_INTEGER_32:
			return (ZEInt16)Value.Int32;
			break;
		case ZE_VRT_UNSIGNED_INTEGER_64:
		case ZE_VRT_INTEGER_64:
			return (ZEInt16)Value.Int64;
			break;
		default:
			return 0;
	}
}

ZEValue::operator ZEInt32()
{
	zeDebugCheck(this->Type != ZE_VRT_FLOAT && this->Type != ZE_VRT_DOUBLE
		&& this->Type != ZE_VRT_UNSIGNED_INTEGER_8 && this->Type != ZE_VRT_UNSIGNED_INTEGER_16 
		&& this->Type != ZE_VRT_UNSIGNED_INTEGER_32 && this->Type != ZE_VRT_UNSIGNED_INTEGER_64
		&& this->Type != ZE_VRT_INTEGER_8 && this->Type != ZE_VRT_INTEGER_16 
		&& this->Type != ZE_VRT_INTEGER_32 && this->Type != ZE_VRT_INTEGER_64 
		, "Integer conversion operation failed. Value type mismatched.");

	switch(Type)
	{
		case ZE_VRT_FLOAT:
			return (ZEInt32)Value.Float;
			break;
		case ZE_VRT_DOUBLE:
			return (ZEInt32)Value.Double;
			break;
		case ZE_VRT_INTEGER_8:
		case ZE_VRT_INTEGER_16:
		case ZE_VRT_INTEGER_32:
		case ZE_VRT_UNSIGNED_INTEGER_8:
		case ZE_VRT_UNSIGNED_INTEGER_16:
		case ZE_VRT_UNSIGNED_INTEGER_32:
			return Value.Int32;
			break;
		case ZE_VRT_UNSIGNED_INTEGER_64:
		case ZE_VRT_INTEGER_64:
			return (ZEInt32)Value.Int64;
			break;
		default:
			return 0;
	}
}

ZEValue::operator ZEInt64()
{
	zeDebugCheck(this->Type != ZE_VRT_FLOAT && this->Type != ZE_VRT_DOUBLE
		&& this->Type != ZE_VRT_UNSIGNED_INTEGER_8 && this->Type != ZE_VRT_UNSIGNED_INTEGER_16 
		&& this->Type != ZE_VRT_UNSIGNED_INTEGER_32 && this->Type != ZE_VRT_UNSIGNED_INTEGER_64
		&& this->Type != ZE_VRT_INTEGER_8 && this->Type != ZE_VRT_INTEGER_16 
		&& this->Type != ZE_VRT_INTEGER_32 && this->Type != ZE_VRT_INTEGER_64 
		, "Integer conversion operation failed. Value type mismatched.");

	switch(Type)
	{
		case ZE_VRT_FLOAT:
			return (ZEInt64)Value.Float;
			break;
		case ZE_VRT_DOUBLE:
			return (ZEInt64)Value.Double;
			break;
		case ZE_VRT_INTEGER_8:
		case ZE_VRT_INTEGER_16:
		case ZE_VRT_INTEGER_32:
		case ZE_VRT_UNSIGNED_INTEGER_8:
		case ZE_VRT_UNSIGNED_INTEGER_16:
		case ZE_VRT_UNSIGNED_INTEGER_32:
			return (ZEInt64)Value.Int32;
			break;
		case ZE_VRT_UNSIGNED_INTEGER_64:
		case ZE_VRT_INTEGER_64:
			return Value.Int64;
			break;
		default:
			return 0;
	}
}

ZEValue::operator ZEUInt8()
{
	zeDebugCheck(this->Type != ZE_VRT_FLOAT && this->Type != ZE_VRT_DOUBLE
		&& this->Type != ZE_VRT_UNSIGNED_INTEGER_8 && this->Type != ZE_VRT_UNSIGNED_INTEGER_16 
		&& this->Type != ZE_VRT_UNSIGNED_INTEGER_32 && this->Type != ZE_VRT_UNSIGNED_INTEGER_64
		&& this->Type != ZE_VRT_INTEGER_8 && this->Type != ZE_VRT_INTEGER_16 
		&& this->Type != ZE_VRT_INTEGER_32 && this->Type != ZE_VRT_INTEGER_64 
		, "Integer conversion operation failed. Value type mismatched.");

	switch(Type)
	{
		case ZE_VRT_FLOAT:
			return (ZEUInt8)Value.Float;
			break;
		case ZE_VRT_DOUBLE:
			return (ZEUInt8)Value.Double;
			break;
		case ZE_VRT_INTEGER_8:
		case ZE_VRT_INTEGER_16:
		case ZE_VRT_INTEGER_32:
		case ZE_VRT_UNSIGNED_INTEGER_8:
		case ZE_VRT_UNSIGNED_INTEGER_16:
		case ZE_VRT_UNSIGNED_INTEGER_32:
			return (ZEUInt8)Value.Int32;
			break;
		case ZE_VRT_UNSIGNED_INTEGER_64:
		case ZE_VRT_INTEGER_64:
			return (ZEUInt8)Value.Int64;
			break;
		default:
			return 0;
	}
}

ZEValue::operator ZEUInt16()
{
	zeDebugCheck(this->Type != ZE_VRT_FLOAT && this->Type != ZE_VRT_DOUBLE
		&& this->Type != ZE_VRT_UNSIGNED_INTEGER_8 && this->Type != ZE_VRT_UNSIGNED_INTEGER_16 
		&& this->Type != ZE_VRT_UNSIGNED_INTEGER_32 && this->Type != ZE_VRT_UNSIGNED_INTEGER_64
		&& this->Type != ZE_VRT_INTEGER_8 && this->Type != ZE_VRT_INTEGER_16 
		&& this->Type != ZE_VRT_INTEGER_32 && this->Type != ZE_VRT_INTEGER_64 
		, "Integer conversion operation failed. Value type mismatched.");

	switch(Type)
	{
		case ZE_VRT_FLOAT:
			return (ZEUInt16)Value.Float;
			break;
		case ZE_VRT_DOUBLE:
			return (ZEUInt16)Value.Double;
			break;
		case ZE_VRT_INTEGER_8:
		case ZE_VRT_INTEGER_16:
		case ZE_VRT_INTEGER_32:
		case ZE_VRT_UNSIGNED_INTEGER_8:
		case ZE_VRT_UNSIGNED_INTEGER_16:
		case ZE_VRT_UNSIGNED_INTEGER_32:
			return (ZEUInt16)Value.Int32;
			break;
		case ZE_VRT_UNSIGNED_INTEGER_64:
		case ZE_VRT_INTEGER_64:
			return (ZEUInt16)Value.Int64;
			break;
		default:
			return 0;
	}
}

ZEValue::operator ZEUInt32()
{
	zeDebugCheck(this->Type != ZE_VRT_FLOAT && this->Type != ZE_VRT_DOUBLE
		&& this->Type != ZE_VRT_UNSIGNED_INTEGER_8 && this->Type != ZE_VRT_UNSIGNED_INTEGER_16 
		&& this->Type != ZE_VRT_UNSIGNED_INTEGER_32 && this->Type != ZE_VRT_UNSIGNED_INTEGER_64
		&& this->Type != ZE_VRT_INTEGER_8 && this->Type != ZE_VRT_INTEGER_16 
		&& this->Type != ZE_VRT_INTEGER_32 && this->Type != ZE_VRT_INTEGER_64 
		, "Integer conversion operation failed. Value type mismatched.");

	switch(Type)
	{
		case ZE_VRT_FLOAT:
			return (ZEUInt32)Value.Float;
			break;
		case ZE_VRT_DOUBLE:
			return (ZEUInt32)Value.Double;
			break;
		case ZE_VRT_INTEGER_8:
		case ZE_VRT_INTEGER_16:
		case ZE_VRT_INTEGER_32:
		case ZE_VRT_UNSIGNED_INTEGER_8:
		case ZE_VRT_UNSIGNED_INTEGER_16:
		case ZE_VRT_UNSIGNED_INTEGER_32:
			return (ZEUInt32)Value.Int32;
			break;
		case ZE_VRT_UNSIGNED_INTEGER_64:
		case ZE_VRT_INTEGER_64:
			return (ZEUInt32)Value.Int64;
			break;
		default:
			return 0;
	}
}

ZEValue::operator ZEUInt64()
{
	zeDebugCheck(this->Type != ZE_VRT_FLOAT && this->Type != ZE_VRT_DOUBLE
		&& this->Type != ZE_VRT_UNSIGNED_INTEGER_8 && this->Type != ZE_VRT_UNSIGNED_INTEGER_16 
		&& this->Type != ZE_VRT_UNSIGNED_INTEGER_32 && this->Type != ZE_VRT_UNSIGNED_INTEGER_64
		&& this->Type != ZE_VRT_INTEGER_8 && this->Type != ZE_VRT_INTEGER_16 
		&& this->Type != ZE_VRT_INTEGER_32 && this->Type != ZE_VRT_INTEGER_64 
		, "Integer conversion operation failed. Value type mismatched.");

	switch(Type)
	{
		case ZE_VRT_FLOAT:
			return (ZEUInt64)Value.Float;
			break;
		case ZE_VRT_DOUBLE:
			return (ZEUInt64)Value.Double;
			break;
		case ZE_VRT_INTEGER_8:
		case ZE_VRT_INTEGER_16:
		case ZE_VRT_INTEGER_32:
		case ZE_VRT_UNSIGNED_INTEGER_8:
		case ZE_VRT_UNSIGNED_INTEGER_16:
		case ZE_VRT_UNSIGNED_INTEGER_32:
			return (ZEUInt64)Value.Int32;
			break;
		case ZE_VRT_UNSIGNED_INTEGER_64:
		case ZE_VRT_INTEGER_64:
			return (ZEUInt64)Value.Int64;
			break;
		default:
			return 0;
	}
}

ZEValue::operator float()
{
	zeDebugCheck(this->Type != ZE_VRT_FLOAT && this->Type != ZE_VRT_DOUBLE
		&& this->Type != ZE_VRT_UNSIGNED_INTEGER_8 && this->Type != ZE_VRT_UNSIGNED_INTEGER_16 
		&& this->Type != ZE_VRT_UNSIGNED_INTEGER_32 && this->Type != ZE_VRT_UNSIGNED_INTEGER_64
		&& this->Type != ZE_VRT_INTEGER_8 && this->Type != ZE_VRT_INTEGER_16 
		&& this->Type != ZE_VRT_INTEGER_32 && this->Type != ZE_VRT_INTEGER_64 
		, "Integer conversion operation failed. Value type mismatched.");

	switch(Type)
	{
		case ZE_VRT_FLOAT:
			return Value.Float;
			break;
		case ZE_VRT_DOUBLE:
			return (float)Value.Double;
			break;
		case ZE_VRT_INTEGER_8:
		case ZE_VRT_INTEGER_16:
		case ZE_VRT_INTEGER_32:
		case ZE_VRT_UNSIGNED_INTEGER_8:
		case ZE_VRT_UNSIGNED_INTEGER_16:
		case ZE_VRT_UNSIGNED_INTEGER_32:
			return (float)Value.Int32;
			break;
		case ZE_VRT_UNSIGNED_INTEGER_64:
		case ZE_VRT_INTEGER_64:
			return (float)Value.Int64;
			break;
		default:
			return 0;
	}
}

ZEValue::operator double()
{
	{
		zeDebugCheck(this->Type != ZE_VRT_FLOAT && this->Type != ZE_VRT_DOUBLE
			&& this->Type != ZE_VRT_UNSIGNED_INTEGER_8 && this->Type != ZE_VRT_UNSIGNED_INTEGER_16 
			&& this->Type != ZE_VRT_UNSIGNED_INTEGER_32 && this->Type != ZE_VRT_UNSIGNED_INTEGER_64
			&& this->Type != ZE_VRT_INTEGER_8 && this->Type != ZE_VRT_INTEGER_16 
			&& this->Type != ZE_VRT_INTEGER_32 && this->Type != ZE_VRT_INTEGER_64 
			, "Integer conversion operation failed. Value type mismatched.");

		switch(Type)
		{
			case ZE_VRT_FLOAT:
				return (double)Value.Float;
				break;
			case ZE_VRT_DOUBLE:
				return Value.Double;
				break;
			case ZE_VRT_INTEGER_8:
			case ZE_VRT_INTEGER_16:
			case ZE_VRT_INTEGER_32:
			case ZE_VRT_UNSIGNED_INTEGER_8:
			case ZE_VRT_UNSIGNED_INTEGER_16:
			case ZE_VRT_UNSIGNED_INTEGER_32:
				return (double)Value.Int32;
				break;
			case ZE_VRT_UNSIGNED_INTEGER_64:
			case ZE_VRT_INTEGER_64:
				return (double)Value.Int64;
				break;
			default:
				return 0;
		}
	}
}

ZEValue::operator bool()
{
	zeDebugCheck(this->Type != ZE_VRT_BOOLEAN, "Boolean conversion operation failed. Value type mismatched.");
	return Value.Boolean;
}

ZEValue::operator ZEVector2()
{
	zeDebugCheck(this->Type != ZE_VRT_VECTOR2, "ZEVector2 conversion operation failed. Value type mismatched.");
	return *((ZEVector2*)(&Value.Vectors));
}

ZEValue::operator ZEVector2d()
{
	zeDebugCheck(this->Type != ZE_VRT_VECTOR2D, "ZEVector2d conversion operation failed. Value type mismatched.");
	return *((ZEVector2d*)(&Value.DoubleVectors));
}

ZEValue::operator ZEVector3()
{
	zeDebugCheck(this->Type != ZE_VRT_VECTOR3, "ZEVector3 conversion operation failed. Value type mismatched.");
	return *((ZEVector3*)(&Value.Vectors));
}

ZEValue::operator ZEVector3d()
{
	zeDebugCheck(this->Type != ZE_VRT_VECTOR3D, "ZEVector3d conversion operation failed. Value type mismatched.");
	return *((ZEVector3d*)(&Value.DoubleVectors));
}

ZEValue::operator ZEVector4()
{
	zeDebugCheck(this->Type != ZE_VRT_VECTOR4, "ZEVector4 conversion operation failed. Value type mismatched.");
	return *((ZEVector4*)(&Value.Vectors));
}

ZEValue::operator ZEVector4d()
{
	zeDebugCheck(this->Type != ZE_VRT_VECTOR4D, "ZEVector4d conversion operation failed. Value type mismatched.");
	return *((ZEVector4d*)(&Value.DoubleVectors));
}

ZEValue::operator ZEQuaternion()
{
	zeDebugCheck(this->Type != ZE_VRT_QUATERNION, "ZEQuaternion conversion operation failed. Value type mismatched.");
	return *((ZEQuaternion*)(&Value.Vectors));
}


ZEValue::operator ZEMatrix3x3()
{
	zeDebugCheck(this->Type != ZE_VRT_MATRIX3X3, "ZEMatrix3x3 conversion operation failed. Value type mismatched.");
	return *Value.Matrix3x3;
}

ZEValue::operator ZEMatrix3x3d()
{
	zeDebugCheck(this->Type != ZE_VRT_MATRIX3X3D, "ZEMatrix3x3d conversion operation failed. Value type mismatched.");
	return *Value.Matrix3x3d;
}

ZEValue::operator ZEMatrix4x4()
{
	zeDebugCheck(this->Type != ZE_VRT_MATRIX4X4, "ZEMatrix4x4 conversion operation failed. Value type mismatched.");
	return *Value.Matrix4x4;
}

ZEValue::operator ZEMatrix4x4d()
{
	zeDebugCheck(this->Type != ZE_VRT_MATRIX4X4D, "ZEMatrix4x4d conversion operation failed. Value type mismatched.");
	return *Value.Matrix4x4d;
}

ZEValue::operator ZEObject*()
{
	zeDebugCheck(this->Type != ZE_VRT_CLASS, "Pointer conversion operation failed. Value type mismatched.");
	return Value.Pointer;
}

ZEValue::ZEValue()
{
	Type = ZE_VRT_UNDEFINED;
	Value.Pointer = NULL;
}

ZEValue::ZEValue(const ZEValue& Value)
{
	SetValue(Value);
}

ZEValue::ZEValue(const char* Value)
{
	SetString(Value);
}

ZEValue::ZEValue(const ZEString& String)
{
	SetString(String);
}

ZEValue::ZEValue(ZEInt8 Value)
{
	SetInt8(Value);
}

ZEValue::ZEValue(ZEInt16 Value)
{
	SetInt16(Value);
}

ZEValue::ZEValue(ZEInt32 Value)
{
	SetInt32(Value);
}

ZEValue::ZEValue(ZEInt64 Value)
{
	SetInt64(Value);
}

ZEValue::ZEValue(ZEUInt8 Value)
{
	SetUInt8(Value);
}

ZEValue::ZEValue(ZEUInt16 Value)
{
	SetUInt16(Value);
}

ZEValue::ZEValue(ZEUInt32 Value)
{
	SetUInt32(Value);
}

ZEValue::ZEValue(ZEUInt64 Value)
{
	SetUInt64(Value);
}

ZEValue::ZEValue(float Value)
{
	SetFloat(Value);
}

ZEValue::ZEValue(double Value)
{
	SetDouble(Value);
}

ZEValue::ZEValue(bool Value)
{
	SetBoolean(Value);
}

ZEValue::ZEValue(const ZEVector2& Vector)
{
	SetVector2(Vector);
}

ZEValue::ZEValue(const ZEVector2d& Vector)
{
	SetVector2d(Vector);
}

ZEValue::ZEValue(const ZEVector3& Vector)
{
	SetVector3(Vector);
}

ZEValue::ZEValue(const ZEVector3d& Vector)
{
	SetVector3d(Vector);
}

ZEValue::ZEValue(const ZEVector4& Vector)
{
	SetVector4(Vector);
}

ZEValue::ZEValue(const ZEVector4d& Vector)
{
	SetVector4d(Vector);
}

ZEValue::ZEValue(const ZEQuaternion& Quaternion)
{
	SetQuaternion(Quaternion);
}

ZEValue::ZEValue(const ZEMatrix3x3& Matrix)
{
	SetMatrix3x3(Matrix);
}

ZEValue::ZEValue(const ZEMatrix3x3d& Matrix)
{
	SetMatrix3x3d(Matrix);
}

ZEValue::ZEValue(const ZEMatrix4x4& Matrix)
{
	SetMatrix4x4(Matrix);
}

ZEValue::ZEValue(const ZEMatrix4x4d& Matrix)
{
	SetMatrix4x4d(Matrix);
}

ZEValue::ZEValue(ZEObject* Class)
{
	SetClass(Class);
}

ZEValue::~ZEValue()
{
	SetType(ZE_VRT_UNDEFINED);
}

#pragma warning(pop)
