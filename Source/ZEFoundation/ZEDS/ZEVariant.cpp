//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEVariant.cpp
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

#include "ZEVariant.h"
#include "ZETypes.h"
#include "ZEError.h"
#include "ZESerialization/ZESerializer.h"
#include "ZESerialization/ZEUnserializer.h"

#include <string.h>

#pragma warning(push)
#pragma warning(disable:4482 4996)

void ZEVariant::SetType(ZEVariantType NewType)
{

	if (this->Type != NewType && Value.Pointer != NULL)
	{
		Value.Pointer = NULL;
		if (Type == ZE_VRT_STRING)
		{
			Value.String.Clear();
		}
		else if (Type == ZE_VRT_MATRIX3X3)
		{
			delete Value.Matrix3x3;
			Value.Matrix3x3 = NULL;
		}
		else if (Type == ZE_VRT_MATRIX4X4)
		{
			delete Value.Matrix4x4;
			Value.Matrix3x3 = NULL;
		}
	}

	switch(NewType)
	{
		case ZE_VRT_MATRIX3X3:
			Value.Matrix3x3 = new ZEMatrix3x3();
			break;
		case ZE_VRT_MATRIX4X4:
			Value.Matrix4x4 = new ZEMatrix4x4();
			break;
	}

	Type = NewType;
}

ZEVariantType ZEVariant::GetType() const
{
	return Type;
}

ZEVariant::ZEVariantValue ZEVariant::GetValue()	const
{
	return Value;
}

ZEVariant::ZEVariantValue& ZEVariant::ImplicitAcesss()
{
	return Value;
}

void ZEVariant::SetVariant(const ZEVariant& NewValue)
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
		case ZE_VRT_VECTOR3:
			SetVector3(NewValue.GetVector3());
			break;
		case ZE_VRT_VECTOR4:
			SetVector4(NewValue.GetVector4());
			break;
		case ZE_VRT_MATRIX3X3:
			SetMatrix3x3(NewValue.GetMatrix3x3());
			break;
		case ZE_VRT_MATRIX4X4:
			SetMatrix4x4(NewValue.GetMatrix4x4());
			break;
		case ZE_VRT_CLASS:
			SetClass(NewValue.GetClass());
			break;
		default:
			zeDebugCheck(true, "ZEVariant::SetVariant operation failed. Error in variant type.");
	}
}

ZESize ZEVariant::SizeOf() const
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
		case ZE_VRT_VECTOR3:
			return sizeof(ZEVector3);
		case ZE_VRT_VECTOR4:
			return sizeof(ZEVector4);
		case ZE_VRT_MATRIX3X3:
			return sizeof(ZEMatrix3x3);
		case ZE_VRT_MATRIX4X4:
			return sizeof(ZEMatrix4x4);
		case ZE_VRT_CLASS:
			return sizeof(void*);
		default:
			zeDebugCheck(true, "ZEVariant::SizeOf is wrong type.");
			return 0;
	}
}

bool ZEVariant::Serialize(ZESerializer* Serializer)
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
		case ZE_VRT_VECTOR3:
			Serializer->Write(&Value.Vectors, sizeof(ZEVector3), 1);
			break;
		case ZE_VRT_VECTOR4:
			Serializer->Write(&Value.Vectors, sizeof(ZEVector4), 1);
			break;
		case ZE_VRT_MATRIX3X3:
			Serializer->Write(Value.Matrix3x3, sizeof(ZEMatrix3x3), 1);
			break;
		case ZE_VRT_MATRIX4X4:
			Serializer->Write(Value.Matrix4x4, sizeof(ZEMatrix4x4), 1);
			break;
		case ZE_VRT_CLASS:
			zeDebugCheck(true, "Can not unserialize pointer type.");
			return false;
		default:
			zeDebugCheck(true, "Wrong variant type.");
			return false;
	}
	return true;
}

bool ZEVariant::Unserialize(ZEUnserializer* Unserializer)
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
		case ZE_VRT_VECTOR3:
			SetType(ZE_VRT_VECTOR3);
			Unserializer->Read(&Value.Vectors, sizeof(ZEVector3), 1);
			break;
		case ZE_VRT_VECTOR4:
			SetType(ZE_VRT_VECTOR4);
			Unserializer->Read(&Value.Vectors, sizeof(ZEVector4), 1);
			break;
		case ZE_VRT_MATRIX3X3:
			SetType(ZE_VRT_MATRIX3X3);
			Unserializer->Read(Value.Matrix3x3, sizeof(ZEMatrix3x3), 1);
			break;
		case ZE_VRT_MATRIX4X4:
			SetType(ZE_VRT_MATRIX4X4);
			Unserializer->Read(Value.Matrix4x4, sizeof(ZEMatrix4x4), 1);
			break;
		case ZE_VRT_CLASS:
			SetType(ZE_VRT_UNDEFINED);
			zeDebugCheck(true, "Can not unserialize pointer type.");
			return false;
		default:
			SetType(ZE_VRT_UNDEFINED);
			zeDebugCheck(true, "Wrong variant type.");
			return false;
	}
	return true;
}


void ZEVariant::SetString(const char* Value)
{
	SetType(ZE_VRT_STRING);
	this->Value.String.SetValue(Value);
}

void ZEVariant::SetString(const ZEString& Value)
{
	this->Value.String.SetValue(Value);
}

void ZEVariant::SetInt8(ZEInt8 Value)
{
	Type = ZE_VRT_INTEGER_8;
	this->Value.Int32 = (ZEInt32)Value;
}

void ZEVariant::SetInt16(ZEInt16 Value)
{
	Type = ZE_VRT_INTEGER_16;
	this->Value.Int32 = (ZEInt32)Value;
}

void ZEVariant::SetInt32(ZEInt32 Value)
{
	Type = ZE_VRT_INTEGER_32;
	this->Value.Int32 = Value;
}

void ZEVariant::SetInt64(ZEInt64 Value)
{
	Type = ZE_VRT_INTEGER_64;
	this->Value.Int64 = Value;
}

void ZEVariant::SetUInt8(ZEUInt8 Value)
{
	Type = ZE_VRT_UNSIGNED_INTEGER_8;
	this->Value.Int32 = Value;
}

void ZEVariant::SetUInt16(ZEUInt16 Value)
{
	Type = ZE_VRT_UNSIGNED_INTEGER_16;
	this->Value.Int32 = Value;
}

void ZEVariant::SetUInt32(ZEUInt32 Value)
{
	Type = ZE_VRT_UNSIGNED_INTEGER_32;
	this->Value.Int32 = Value;
}

void ZEVariant::SetUInt64(ZEUInt64 Value)
{
	Type = ZE_VRT_UNSIGNED_INTEGER_64;
	this->Value.Int64 = Value;
}

void ZEVariant::SetFloat(float Value)
{
	Type = ZE_VRT_FLOAT;
	this->Value.Float = Value;
}

void ZEVariant::SetDouble(double Value)
{
	Type = ZE_VRT_DOUBLE;
	this->Value.Double = Value;
}

void ZEVariant::SetBoolean(bool Value)
{
	SetType(ZE_VRT_BOOLEAN);
	this->Value.Boolean = Value;
}

void ZEVariant::SetVector2(const ZEVector2& Value)
{
	SetType(ZE_VRT_VECTOR2);
	(*(ZEVector2*)(&this->Value.Vectors)) = Value;
}

void ZEVariant::SetVector3(const ZEVector3& Value)
{
	SetType(ZE_VRT_VECTOR3);
  	(*(ZEVector3*)(&this->Value.Vectors)) = Value;
}

void ZEVariant::SetVector4(const ZEVector4& Value)
{
	SetType(ZE_VRT_VECTOR4);
  	(*(ZEVector4*)(&this->Value.Vectors)) = Value;
}

void ZEVariant::SetQuaternion(const ZEQuaternion& Value)
{
	SetType(ZE_VRT_QUATERNION);
	(*(ZEQuaternion*)(&this->Value.Vectors)) = Value;
}

void ZEVariant::SetMatrix3x3(const ZEMatrix3x3& Value)
{
	SetType(ZE_VRT_MATRIX3X3);
	*this->Value.Matrix3x3 = Value;

}

void ZEVariant::SetMatrix4x4(const ZEMatrix4x4& Value)
{
	SetType(ZE_VRT_MATRIX4X4);
	*this->Value.Matrix4x4 = Value;
}

void ZEVariant::SetClass(ZEObject* Value)
{
	SetType(ZE_VRT_CLASS);
	this->Value.Pointer = Value;
}

void ZEVariant::SetNull()
{
	SetType(ZE_VRT_NULL);
}

const ZEString& ZEVariant::GetString() const
{
	zeDebugCheck(this->Type != ZE_VRT_STRING, "ZEVariant::GetString operation failed. Variant type mismatched.");
	return Value.String;
}

ZEInt8 ZEVariant::GetInt8() const
{
	zeDebugCheck(this->Type != ZE_VRT_INTEGER_8, "ZEVariant::GetInt8 operation failed. Variant type mismatched.");
	return (ZEInt8)Value.Int32;
}

ZEInt16 ZEVariant::GetInt16() const
{
	zeDebugCheck(this->Type != ZE_VRT_INTEGER_16, "ZEVariant::GetInt16 operation failed. Variant type mismatched.");
	return (ZEInt16)Value.Int32;
}

ZEInt32 ZEVariant::GetInt32() const
{
	zeDebugCheck(this->Type != ZE_VRT_INTEGER_32, "ZEVariant::GetInt32 operation failed. Variant type mismatched.");
	return Value.Int32;
}

ZEInt64 ZEVariant::GetInt64() const
{
	zeDebugCheck(this->Type != ZE_VRT_INTEGER_64, "ZEVariant::GetInt64 operation failed. Variant type mismatched.");
	return Value.Int64;
}

ZEUInt8 ZEVariant::GetUInt8() const
{
	zeDebugCheck(this->Type != ZE_VRT_UNSIGNED_INTEGER_8, "ZEVariant::GetUInt8 operation failed. Variant type mismatched.");
	return (ZEUInt8)Value.Int32;
}

ZEUInt16 ZEVariant::GetUInt16() const
{
	zeDebugCheck(this->Type != ZE_VRT_UNSIGNED_INTEGER_16, "ZEVariant::GetUInt16 operation failed. Variant type mismatched.");
	return (ZEUInt16)Value.Int32;
}

ZEUInt32 ZEVariant::GetUInt32() const
{
	zeDebugCheck(this->Type != ZE_VRT_UNSIGNED_INTEGER_32, "ZEVariant::GetUInt32 operation failed. Variant type mismatched.");
	return Value.Int32;
}

ZEUInt64 ZEVariant::GetUInt64() const
{
	zeDebugCheck(this->Type != ZE_VRT_UNSIGNED_INTEGER_64, "ZEVariant::GetUInt64 operation failed. Variant type mismatched.");
	return Value.Int64;
}

float ZEVariant::GetFloat() const
{
	zeDebugCheck(this->Type != ZE_VRT_FLOAT, "ZEVariant::GetFloat operation failed. Variant type mismatched.");
	return Value.Float;
}

double ZEVariant::GetDouble() const
{
	zeDebugCheck(this->Type != ZE_VRT_DOUBLE, "ZEVariant::GetDouble operation failed. Variant type mismatched.");
	return Value.Double;
}

bool ZEVariant::GetBoolean() const
{
	zeDebugCheck(this->Type != ZE_VRT_BOOLEAN, "ZEVariant::GetBoolean operation failed. Variant type mismatched.");
	return Value.Boolean;
}
ZEVector2& ZEVariant::GetVector2() const
{
	zeDebugCheck(this->Type != ZE_VRT_VECTOR2, "ZEVariant::GetVector2 operation failed. Variant type mismatched.");
	return *((ZEVector2*)&Value.Vectors);
}

ZEVector3& ZEVariant::GetVector3() const
{
	zeDebugCheck(this->Type != ZE_VRT_VECTOR3, "ZEVariant::GetVector3 operation failed. Variant type mismatched.");
	return *((ZEVector3*)&Value.Vectors);
}

ZEVector4& ZEVariant::GetVector4() const
{
	zeDebugCheck(this->Type != ZE_VRT_VECTOR4, "ZEVariant::GetVector4 operation failed. Variant type mismatched.");
	return *((ZEVector4*)&Value.Vectors);
}

ZEQuaternion& ZEVariant::GetQuaternion() const
{
	zeDebugCheck(this->Type != ZE_VRT_QUATERNION, "ZEVariant::Quaternion operation failed. Variant type mismatched.");
	return *((ZEQuaternion*)&Value.Vectors);
}

ZEMatrix3x3& ZEVariant::GetMatrix3x3() const
{
	zeDebugCheck(this->Type != ZE_VRT_MATRIX3X3, "ZEVariant::GetMatrix3x3 operation failed. Variant type mismatched.");
	return *Value.Matrix3x3;
}

ZEMatrix4x4& ZEVariant::GetMatrix4x4() const
{
	zeDebugCheck(this->Type != ZE_VRT_MATRIX4X4, "ZEVariant::GetMatrix4x4 operation failed. Variant type mismatched.");
	return *Value.Matrix4x4;
}

ZEObject* ZEVariant::GetClass() const
{
	zeDebugCheck(this->Type != ZE_VRT_CLASS, "ZEVariant::GetClass operation failed. Variant type mismatched.");
	return Value.Pointer;
}

bool ZEVariant::IsNull()	const
{
	return (Type == ZE_VRT_NULL);
}

void ZEVariant::operator=(const char* NewValue)
{
	SetString(NewValue);
}

void ZEVariant::operator=(ZEInt8 NewValue)
{
	SetInt8(NewValue);
}

void ZEVariant::operator=(ZEInt16 NewValue)
{
	SetInt16(NewValue);
}

void ZEVariant::operator=(ZEInt32 NewValue)
{
	SetInt32(NewValue);
}

void ZEVariant::operator=(ZEInt64 NewValue)
{
	SetInt64(NewValue);
}

void ZEVariant::operator=(ZEUInt8 NewValue)
{
	SetUInt8(NewValue);
}

void ZEVariant::operator=(ZEUInt16 NewValue)
{
	SetUInt16(NewValue);
}

void ZEVariant::operator=(ZEUInt32 NewValue)
{
	SetUInt32(NewValue);
}

void ZEVariant::operator=(ZEUInt64 NewValue)
{
	SetUInt64(NewValue);
}

void ZEVariant::operator=(float NewValue)
{
	SetFloat(NewValue);
}

void ZEVariant::operator=(double NewValue)
{
	SetDouble(NewValue);
}

void ZEVariant::operator=(bool NewValue)
{
	SetBoolean(NewValue);
}

void ZEVariant::operator=(const ZEVariant& NewValue)
{
	SetVariant(NewValue);
}

void ZEVariant::operator= (const ZEVector2& Vector)
{
	SetVector2(Vector);
}

void ZEVariant::operator= (const ZEVector3& Vector)
{
	SetVector3(Vector);
}

void ZEVariant::operator= (const ZEVector4& Vector)
{
	SetVector4(Vector);
}

void ZEVariant::operator= (const ZEQuaternion& Quaternion)
{
	SetQuaternion(Quaternion);
}

void ZEVariant::operator= (const ZEMatrix3x3& Matrix)
{
	SetMatrix3x3(Matrix);
}

void ZEVariant::operator= (const ZEMatrix4x4& Matrix)
{
	SetMatrix4x4(Matrix);
}

ZEVariant::operator const char*()
{
	zeDebugCheck(this->Type != ZE_VRT_STRING, "String conversion operation failed. Variant type mismatched.");
	return this->Value.String;
}

ZEVariant::operator ZEInt8()
{
	zeDebugCheck(this->Type != ZE_VRT_FLOAT && this->Type != ZE_VRT_DOUBLE
				&& this->Type != ZE_VRT_UNSIGNED_INTEGER_8 && this->Type != ZE_VRT_UNSIGNED_INTEGER_16 
				&& this->Type != ZE_VRT_UNSIGNED_INTEGER_32 && this->Type != ZE_VRT_UNSIGNED_INTEGER_64
				&& this->Type != ZE_VRT_INTEGER_8 && this->Type != ZE_VRT_INTEGER_16 
				&& this->Type != ZE_VRT_INTEGER_32 && this->Type != ZE_VRT_INTEGER_64 
				, "Integer conversion operation failed. Variant type mismatched.");

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

ZEVariant::operator ZEInt16()
{
	zeDebugCheck(this->Type != ZE_VRT_FLOAT && this->Type != ZE_VRT_DOUBLE
		&& this->Type != ZE_VRT_UNSIGNED_INTEGER_8 && this->Type != ZE_VRT_UNSIGNED_INTEGER_16 
		&& this->Type != ZE_VRT_UNSIGNED_INTEGER_32 && this->Type != ZE_VRT_UNSIGNED_INTEGER_64
		&& this->Type != ZE_VRT_INTEGER_8 && this->Type != ZE_VRT_INTEGER_16 
		&& this->Type != ZE_VRT_INTEGER_32 && this->Type != ZE_VRT_INTEGER_64 
		, "Integer conversion operation failed. Variant type mismatched.");

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

ZEVariant::operator ZEInt32()
{
	zeDebugCheck(this->Type != ZE_VRT_FLOAT && this->Type != ZE_VRT_DOUBLE
		&& this->Type != ZE_VRT_UNSIGNED_INTEGER_8 && this->Type != ZE_VRT_UNSIGNED_INTEGER_16 
		&& this->Type != ZE_VRT_UNSIGNED_INTEGER_32 && this->Type != ZE_VRT_UNSIGNED_INTEGER_64
		&& this->Type != ZE_VRT_INTEGER_8 && this->Type != ZE_VRT_INTEGER_16 
		&& this->Type != ZE_VRT_INTEGER_32 && this->Type != ZE_VRT_INTEGER_64 
		, "Integer conversion operation failed. Variant type mismatched.");

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

ZEVariant::operator ZEInt64()
{
	zeDebugCheck(this->Type != ZE_VRT_FLOAT && this->Type != ZE_VRT_DOUBLE
		&& this->Type != ZE_VRT_UNSIGNED_INTEGER_8 && this->Type != ZE_VRT_UNSIGNED_INTEGER_16 
		&& this->Type != ZE_VRT_UNSIGNED_INTEGER_32 && this->Type != ZE_VRT_UNSIGNED_INTEGER_64
		&& this->Type != ZE_VRT_INTEGER_8 && this->Type != ZE_VRT_INTEGER_16 
		&& this->Type != ZE_VRT_INTEGER_32 && this->Type != ZE_VRT_INTEGER_64 
		, "Integer conversion operation failed. Variant type mismatched.");

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

ZEVariant::operator ZEUInt8()
{
	zeDebugCheck(this->Type != ZE_VRT_FLOAT && this->Type != ZE_VRT_DOUBLE
		&& this->Type != ZE_VRT_UNSIGNED_INTEGER_8 && this->Type != ZE_VRT_UNSIGNED_INTEGER_16 
		&& this->Type != ZE_VRT_UNSIGNED_INTEGER_32 && this->Type != ZE_VRT_UNSIGNED_INTEGER_64
		&& this->Type != ZE_VRT_INTEGER_8 && this->Type != ZE_VRT_INTEGER_16 
		&& this->Type != ZE_VRT_INTEGER_32 && this->Type != ZE_VRT_INTEGER_64 
		, "Integer conversion operation failed. Variant type mismatched.");

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

ZEVariant::operator ZEUInt16()
{
	zeDebugCheck(this->Type != ZE_VRT_FLOAT && this->Type != ZE_VRT_DOUBLE
		&& this->Type != ZE_VRT_UNSIGNED_INTEGER_8 && this->Type != ZE_VRT_UNSIGNED_INTEGER_16 
		&& this->Type != ZE_VRT_UNSIGNED_INTEGER_32 && this->Type != ZE_VRT_UNSIGNED_INTEGER_64
		&& this->Type != ZE_VRT_INTEGER_8 && this->Type != ZE_VRT_INTEGER_16 
		&& this->Type != ZE_VRT_INTEGER_32 && this->Type != ZE_VRT_INTEGER_64 
		, "Integer conversion operation failed. Variant type mismatched.");

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

ZEVariant::operator ZEUInt32()
{
	zeDebugCheck(this->Type != ZE_VRT_FLOAT && this->Type != ZE_VRT_DOUBLE
		&& this->Type != ZE_VRT_UNSIGNED_INTEGER_8 && this->Type != ZE_VRT_UNSIGNED_INTEGER_16 
		&& this->Type != ZE_VRT_UNSIGNED_INTEGER_32 && this->Type != ZE_VRT_UNSIGNED_INTEGER_64
		&& this->Type != ZE_VRT_INTEGER_8 && this->Type != ZE_VRT_INTEGER_16 
		&& this->Type != ZE_VRT_INTEGER_32 && this->Type != ZE_VRT_INTEGER_64 
		, "Integer conversion operation failed. Variant type mismatched.");

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

ZEVariant::operator ZEUInt64()
{
	zeDebugCheck(this->Type != ZE_VRT_FLOAT && this->Type != ZE_VRT_DOUBLE
		&& this->Type != ZE_VRT_UNSIGNED_INTEGER_8 && this->Type != ZE_VRT_UNSIGNED_INTEGER_16 
		&& this->Type != ZE_VRT_UNSIGNED_INTEGER_32 && this->Type != ZE_VRT_UNSIGNED_INTEGER_64
		&& this->Type != ZE_VRT_INTEGER_8 && this->Type != ZE_VRT_INTEGER_16 
		&& this->Type != ZE_VRT_INTEGER_32 && this->Type != ZE_VRT_INTEGER_64 
		, "Integer conversion operation failed. Variant type mismatched.");

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

ZEVariant::operator float()
{
	zeDebugCheck(this->Type != ZE_VRT_FLOAT && this->Type != ZE_VRT_DOUBLE
		&& this->Type != ZE_VRT_UNSIGNED_INTEGER_8 && this->Type != ZE_VRT_UNSIGNED_INTEGER_16 
		&& this->Type != ZE_VRT_UNSIGNED_INTEGER_32 && this->Type != ZE_VRT_UNSIGNED_INTEGER_64
		&& this->Type != ZE_VRT_INTEGER_8 && this->Type != ZE_VRT_INTEGER_16 
		&& this->Type != ZE_VRT_INTEGER_32 && this->Type != ZE_VRT_INTEGER_64 
		, "Integer conversion operation failed. Variant type mismatched.");

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

ZEVariant::operator double()
{
	{
		zeDebugCheck(this->Type != ZE_VRT_FLOAT && this->Type != ZE_VRT_DOUBLE
			&& this->Type != ZE_VRT_UNSIGNED_INTEGER_8 && this->Type != ZE_VRT_UNSIGNED_INTEGER_16 
			&& this->Type != ZE_VRT_UNSIGNED_INTEGER_32 && this->Type != ZE_VRT_UNSIGNED_INTEGER_64
			&& this->Type != ZE_VRT_INTEGER_8 && this->Type != ZE_VRT_INTEGER_16 
			&& this->Type != ZE_VRT_INTEGER_32 && this->Type != ZE_VRT_INTEGER_64 
			, "Integer conversion operation failed. Variant type mismatched.");

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

ZEVariant::operator bool()
{
	zeDebugCheck(this->Type != ZE_VRT_BOOLEAN, "Boolean conversion operation failed. Variant type mismatched.");
	return Value.Boolean;
}

ZEVariant::operator ZEVector2()
{
	zeDebugCheck(this->Type != ZE_VRT_VECTOR2, "ZEVector2 conversion operation failed. Variant type mismatched.");
	return *((ZEVector2*)(&Value.Vectors));
}

ZEVariant::operator ZEVector3()
{
	zeDebugCheck(this->Type != ZE_VRT_VECTOR3, "ZEVector3 conversion operation failed. Variant type mismatched.");
	return *((ZEVector3*)(&Value.Vectors));
}

ZEVariant::operator ZEVector4()
{
	zeDebugCheck(this->Type != ZE_VRT_VECTOR4, "ZEVector4 conversion operation failed. Variant type mismatched.");
	return *((ZEVector4*)(&Value.Vectors));
}

ZEVariant::operator ZEQuaternion()
{
	zeDebugCheck(this->Type != ZE_VRT_QUATERNION, "ZEQuaternion conversion operation failed. Variant type mismatched.");
	return *((ZEQuaternion*)(&Value.Vectors));
}


ZEVariant::operator ZEMatrix3x3()
{
	zeDebugCheck(this->Type != ZE_VRT_MATRIX3X3, "ZEMatrix3x3 conversion operation failed. Variant type mismatched.");
	return *Value.Matrix3x3;
}

ZEVariant::operator ZEMatrix4x4()
{
	zeDebugCheck(this->Type != ZE_VRT_MATRIX4X4, "ZEMatrix3x3 conversion operation failed. Variant type mismatched.");
	return *Value.Matrix4x4;

}

ZEVariant::operator ZEObject*()
{
	zeDebugCheck(this->Type != ZE_VRT_CLASS, "Pointer conversion operation failed. Variant type mismatched.");
	return Value.Pointer;
}

ZEVariant::ZEVariant()
{
	Type = ZE_VRT_UNDEFINED;
	Value.Pointer = NULL;
}

ZEVariant::ZEVariant(const ZEVariant& Value)
{
	SetVariant(Value);
}

ZEVariant::ZEVariant(const char* Value)
{
	SetString(Value);
}

ZEVariant::ZEVariant(const ZEString& String)
{
	SetString(String);
}

ZEVariant::ZEVariant(ZEInt8 Value)
{
	SetInt8(Value);
}

ZEVariant::ZEVariant(ZEInt16 Value)
{
	SetInt16(Value);
}

ZEVariant::ZEVariant(ZEInt32 Value)
{
	SetInt32(Value);
}

ZEVariant::ZEVariant(ZEInt64 Value)
{
	SetInt64(Value);
}

ZEVariant::ZEVariant(ZEUInt8 Value)
{
	SetUInt8(Value);
}

ZEVariant::ZEVariant(ZEUInt16 Value)
{
	SetUInt16(Value);
}

ZEVariant::ZEVariant(ZEUInt32 Value)
{
	SetUInt32(Value);
}

ZEVariant::ZEVariant(ZEUInt64 Value)
{
	SetUInt64(Value);
}

ZEVariant::ZEVariant(float Value)
{
	SetFloat(Value);
}

ZEVariant::ZEVariant(double Value)
{
	SetDouble(Value);
}

ZEVariant::ZEVariant(bool Value)
{
	SetBoolean(Value);
}

ZEVariant::ZEVariant(const ZEVector2& Vector)
{
	SetVector2(Vector);
}

ZEVariant::ZEVariant(const ZEVector3& Vector)
{
	SetVector3(Vector);
}

ZEVariant::ZEVariant(const ZEVector4& Vector)
{
	SetVector4(Vector);
}

ZEVariant::ZEVariant(const ZEQuaternion& Quaternion)
{
	SetQuaternion(Quaternion);
}

ZEVariant::ZEVariant(const ZEMatrix3x3& Matrix)
{
	SetMatrix3x3(Matrix);
}

ZEVariant::ZEVariant(const ZEMatrix4x4& Matrix)
{
	SetMatrix4x4(Matrix);
}

ZEVariant::ZEVariant(ZEObject* Class)
{
	SetClass(Class);
}

ZEVariant::~ZEVariant()
{
	SetType(ZE_VRT_UNDEFINED);
}

#pragma warning(pop)
