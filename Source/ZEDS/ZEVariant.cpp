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
#include "ZEDSDefinitions.h"
#include "..\ZETypes.h"
#include <string.h>
#pragma warning(push)
#pragma warning(disable:4482 4996)

void ZEVariant::SetType(ZEVariantType NewType)
{
	if (Type == ZE_VRT_STRING && Value.String != NULL)
	{
		delete[] Value.String;
		Value.String = NULL;
	}
	else if (Type == ZE_VRT_MATRIX3X3 || Type == ZE_VRT_MATRIX4X4)
		delete Value.Pointer;

	switch(NewType)
	{
		case ZE_VRT_STRING:
			Value.String = NULL;
			break;
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
			Type = ZE_VRT_UNDEFINED;
			Value.String = NULL;
			break;
		case ZE_VRT_NULL:
			SetNull();
			break;
		case ZE_VRT_STRING:
			SetString(NewValue.GetString());
			break;
		case ZE_VRT_INTEGER:
			SetInteger(NewValue.GetInteger());
			break;
		case ZE_VRT_BOOLEAN:
			SetBoolean(NewValue.GetBoolean());
			break;
		case ZE_VRT_FLOAT:
			SetFloat(NewValue.GetFloat());
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
		case ZE_VRT_QUATERNION:
			SetQuaternion(NewValue.GetQuaternion());
			break;
		case ZE_VRT_MATRIX3X3:
			SetMatrix3x3(NewValue.GetMatrix3x3());
			break;
		case ZE_VRT_MATRIX4X4:
			SetMatrix4x4(NewValue.GetMatrix4x4());
			break;
		case ZE_VRT_POINTER:
			SetPointer(NewValue.GetPointer());
			break;
		default:
			zedsAssert(true, "ZEVariant::SetVariant operation failed. Error in variant type.");
	}
}

size_t ZEVariant::SizeOf() const
{
	switch(Type)
	{
		case ZE_VRT_UNDEFINED:
			return 0;
		case ZE_VRT_NULL:
			return 0;
		case ZE_VRT_STRING:
			return strlen(Value.String) + 1;
		case ZE_VRT_INTEGER:
			return sizeof(int);
		case ZE_VRT_BOOLEAN:
			return sizeof(bool);
		case ZE_VRT_FLOAT:
			return sizeof(float);
		case ZE_VRT_VECTOR2:
			return sizeof(ZEVector2);
		case ZE_VRT_VECTOR3:
			return sizeof(ZEVector3);
		case ZE_VRT_VECTOR4:
			return sizeof(ZEVector4);
		case ZE_VRT_QUATERNION:
			return sizeof(ZEQuaternion);
		case ZE_VRT_MATRIX3X3:
			return sizeof(ZEMatrix3x3);
		case ZE_VRT_MATRIX4X4:
			return sizeof(ZEMatrix4x4);
		case ZE_VRT_POINTER:
			return sizeof(void*);
		default:
			zedsAssert(true, "ZEVariant::SetVariant operation failed. Error in variant type.");
			return 0;
	}
}

bool ZEVariant::Serialize(ZESerializer* Serializer)
{
	ZEDWORD _Type = Type;
	ZEDWORD StringSize;
	switch(Type)
	{
		case ZE_VRT_UNDEFINED:
		case ZE_VRT_NULL:
			Serializer->Write(&_Type, sizeof(ZEDWORD), 1);
			break;
		case ZE_VRT_STRING:
			Serializer->Write(&_Type, sizeof(ZEDWORD), 1);
			StringSize = strlen(Value.String) + 1;
			Serializer->Write(&StringSize, sizeof(ZEDWORD), 1);
			Serializer->Write(Value.String, sizeof(char), StringSize);
			break;
		case ZE_VRT_INTEGER:
			Serializer->Write(&_Type, sizeof(ZEDWORD), 1);
			Serializer->Write(&Value.Integer, sizeof(int), 1);
			break;
		case ZE_VRT_BOOLEAN:
			Serializer->Write(&_Type, sizeof(ZEDWORD), 1);
			Serializer->Write(&Value.Boolean, sizeof(bool), 1);
			break;
		case ZE_VRT_FLOAT:
			Serializer->Write(&_Type, sizeof(ZEDWORD), 1);
			Serializer->Write(&Value.Float, sizeof(float), 1);
			break;
		case ZE_VRT_VECTOR2:
			Serializer->Write(&_Type, sizeof(ZEDWORD), 1);
			Serializer->Write(&Value.Vectors, sizeof(ZEVector2), 1);
			break;
		case ZE_VRT_VECTOR3:
			Serializer->Write(&_Type, sizeof(ZEDWORD), 1);
			Serializer->Write(&Value.Vectors, sizeof(ZEVector3), 1);
			break;
		case ZE_VRT_VECTOR4:
			Serializer->Write(&_Type, sizeof(ZEDWORD), 1);
			Serializer->Write(&Value.Vectors, sizeof(ZEVector4), 1);
			break;
		case ZE_VRT_QUATERNION:
			Serializer->Write(&_Type, sizeof(ZEDWORD), 1);
			Serializer->Write(&Value.Vectors, sizeof(ZEQuaternion), 1);
			break;
		case ZE_VRT_MATRIX3X3:
			Serializer->Write(&_Type, sizeof(ZEDWORD), 1);
			Serializer->Write(Value.Matrix3x3, sizeof(ZEMatrix3x3), 1);
			break;
		case ZE_VRT_MATRIX4X4:
			Serializer->Write(&_Type, sizeof(ZEDWORD), 1);
			Serializer->Write(Value.Matrix4x4, sizeof(ZEMatrix4x4), 1);
			break;
		case ZE_VRT_POINTER:
			zedsAssert(true, "Can not serialize pointer type.");
			return false;
		default:
			zedsAssert(true, "Wrong variant type.");
			return false;
	}
	return true;
}

bool ZEVariant::Unserialize(ZEUnserializer* Unserializer)
{
	ZEDWORD Type;
	ZEDWORD StringSize;
	Unserializer->Read(&Type, sizeof(ZEDWORD), 1);

	switch(Type)
	{
		case ZE_VRT_UNDEFINED:
			SetType(ZE_VRT_UNDEFINED);
			break;
		case ZE_VRT_NULL:
			SetType(ZE_VRT_NULL);
			break;
		case ZE_VRT_STRING:
			SetType(ZE_VRT_STRING);
			Unserializer->Read(&StringSize, sizeof(ZEDWORD), 1);
			Value.String = new char[StringSize];
			Unserializer->Read(Value.String, sizeof(char), StringSize);
			break;
		case ZE_VRT_INTEGER:
			SetType(ZE_VRT_INTEGER);
			Unserializer->Read(&Value.Integer, sizeof(int), 1);
			break;
		case ZE_VRT_BOOLEAN:
			SetType(ZE_VRT_BOOLEAN);
			Unserializer->Read(&Value.Boolean, sizeof(bool), 1);
			break;
		case ZE_VRT_FLOAT:
			SetType(ZE_VRT_FLOAT);
			Unserializer->Read(&Value.Float, sizeof(float), 1);
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
		case ZE_VRT_QUATERNION:
			SetType(ZE_VRT_QUATERNION);
			Unserializer->Read(&Value.Vectors, sizeof(ZEQuaternion), 1);
			break;
		case ZE_VRT_MATRIX3X3:
			SetType(ZE_VRT_MATRIX3X3);
			Unserializer->Read(Value.Matrix3x3, sizeof(ZEMatrix3x3), 1);
			break;
		case ZE_VRT_MATRIX4X4:
			SetType(ZE_VRT_MATRIX4X4);
			Unserializer->Read(Value.Matrix4x4, sizeof(ZEMatrix4x4), 1);
			break;
		case ZE_VRT_POINTER:
			SetType(ZE_VRT_UNDEFINED);
			zedsAssert(true, "Can not unserialize pointer type.");
			return false;
		default:
			SetType(ZE_VRT_UNDEFINED);
			zedsAssert(true, "Wrong variant type.");
			return false;
	}
	return true;
}


void ZEVariant::SetString(const char *NewValue)
{
	SetType(ZE_VRT_STRING);
	Value.String = new char[strlen(NewValue) + 1];
	strcpy(Value.String, NewValue);
}

void ZEVariant::SetInteger(int Value)
{
	Type = ZE_VRT_INTEGER;
	this->Value.Integer = Value;
}

void ZEVariant::SetFloat(float Value)
{
	Type = ZE_VRT_FLOAT;
	this->Value.Float = Value;
}

void ZEVariant::SetBoolean(bool Value)
{
	SetType(ZE_VRT_BOOLEAN);
	this->Value.Boolean = Value;
}

void ZEVariant::SetVector2(const ZEVector2& Vector)
{
	SetType(ZE_VRT_VECTOR2);
	(*(ZEVector2*)(&this->Value.Vectors)) = Vector;
}

void ZEVariant::SetVector3(const ZEVector3& Vector)
{
	SetType(ZE_VRT_VECTOR3);
  	(*(ZEVector3*)(&this->Value.Vectors)) = Vector;
}

void ZEVariant::SetVector4(const ZEVector4& Vector)
{
	SetType(ZE_VRT_VECTOR4);
  	(*(ZEVector4*)(&this->Value.Vectors)) = Vector;
}

void ZEVariant::SetQuaternion(const ZEQuaternion& Quaternion)
{
	SetType(ZE_VRT_QUATERNION);
	(*(ZEQuaternion*)(&this->Value.Vectors)) = Quaternion;
}

void ZEVariant::SetMatrix3x3(const ZEMatrix3x3& Matrix)
{
	SetType(ZE_VRT_MATRIX3X3);
	*this->Value.Matrix3x3 = Matrix;

}

void ZEVariant::SetMatrix4x4(const ZEMatrix4x4& Matrix)
{
	SetType(ZE_VRT_MATRIX3X3);
	*this->Value.Matrix4x4 = Matrix;
}

void ZEVariant::SetPointer(void* Pointer)
{
	SetType(ZE_VRT_POINTER);
	Value.Pointer = Pointer;
}

void ZEVariant::SetNull()
{
	SetType(ZE_VRT_NULL);
}

char* ZEVariant::GetString() const
{
	zedsAssert(this->Type != ZE_VRT_STRING, "ZEVariant::GetString operation failed. Variant type mismatched.");
	return Value.String;
}

int ZEVariant::GetInteger() const
{
	zedsAssert(this->Type != ZE_VRT_INTEGER, "ZEVariant::GetInteger operation failed. Variant type mismatched.");
	return Value.Integer;
}

float ZEVariant::GetFloat() const
{
	zedsAssert(this->Type != ZE_VRT_FLOAT, "ZEVariant::GetFloat operation failed. Variant type mismatched.");
	return Value.Float;
}

bool ZEVariant::GetBoolean() const
{
	zedsAssert(this->Type != ZE_VRT_BOOLEAN, "ZEVariant::GetBoolean operation failed. Variant type mismatched.");
	return Value.Boolean;
}
ZEVector2& ZEVariant::GetVector2() const
{
	zedsAssert(this->Type != ZE_VRT_VECTOR2, "ZEVariant::GetVector2 operation failed. Variant type mismatched.");
	return *((ZEVector2*)&Value.Vectors);
}

ZEVector3& ZEVariant::GetVector3() const
{
	zedsAssert(this->Type != ZE_VRT_VECTOR3, "ZEVariant::GetVector3 operation failed. Variant type mismatched.");
	return *((ZEVector3*)&Value.Vectors);
}

ZEVector4& ZEVariant::GetVector4() const
{
	zedsAssert(this->Type != ZE_VRT_VECTOR4, "ZEVariant::GetVector4 operation failed. Variant type mismatched.");
	return *((ZEVector4*)&Value.Vectors);
}

ZEQuaternion& ZEVariant::GetQuaternion() const
{
	zedsAssert(this->Type != ZE_VRT_QUATERNION, "ZEVariant::Quaternion operation failed. Variant type mismatched.");
	return *((ZEQuaternion*)&Value.Vectors);
}

ZEMatrix3x3& ZEVariant::GetMatrix3x3() const
{
	zedsAssert(this->Type != ZE_VRT_MATRIX3X3, "ZEVariant::GetMatrix3x3 operation failed. Variant type mismatched.");
	return *Value.Matrix3x3;
}

ZEMatrix4x4& ZEVariant::GetMatrix4x4() const
{
	zedsAssert(this->Type != ZE_VRT_MATRIX4X4, "ZEVariant::GetMatrix4x4 operation failed. Variant type mismatched.");
	return *Value.Matrix4x4;
}

void* ZEVariant::GetPointer() const
{
	zedsAssert(this->Type != ZE_VRT_POINTER, "ZEVariant::GetPointer operation failed. Variant type mismatched.");
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
void ZEVariant::operator=(int NewValue)
{
	SetInteger(NewValue);
}

void ZEVariant::operator=(float NewValue)
{
	SetFloat(NewValue);
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
	zedsAssert(this->Type != ZE_VRT_STRING, "String conversion operation failed. Variant type mismatched.");
	return Value.String;
}

ZEVariant::operator int()
{
	zedsAssert(this->Type != ZE_VRT_INTEGER || this->Type != ZE_VRT_FLOAT, "Integer conversion operation failed. Variant type mismatched.");
	if (this->Type == ZE_VRT_FLOAT)
		return (int)Value.Float;
	else
		return Value.Integer;
}

ZEVariant::operator float()
{
	zedsAssert(this->Type != ZE_VRT_INTEGER || this->Type != ZE_VRT_FLOAT, "Float conversion operation failed. Variant type mismatched.");
	if (this->Type == ZE_VRT_INTEGER)
		return (float)Value.Integer;
	else
		return Value.Float;
}

ZEVariant::operator bool()
{
	zedsAssert(this->Type != ZE_VRT_BOOLEAN, "Boolean conversion operation failed. Variant type mismatched.");
	return Value.Boolean;
}

ZEVariant::operator ZEVector2()
{
	zedsAssert(this->Type != ZE_VRT_VECTOR2, "ZEVector2 conversion operation failed. Variant type mismatched.");
	return *((ZEVector2*)(&Value.Vectors));
}

ZEVariant::operator ZEVector3()
{
	zedsAssert(this->Type != ZE_VRT_VECTOR3, "ZEVector3 conversion operation failed. Variant type mismatched.");
	return *((ZEVector3*)(&Value.Vectors));
}

ZEVariant::operator ZEVector4()
{
	zedsAssert(this->Type != ZE_VRT_VECTOR4, "ZEVector4 conversion operation failed. Variant type mismatched.");
	return *((ZEVector4*)(&Value.Vectors));
}

ZEVariant::operator ZEQuaternion()
{
	zedsAssert(this->Type != ZE_VRT_QUATERNION, "ZEQuaternion conversion operation failed. Variant type mismatched.");
	return *((ZEQuaternion*)(&Value.Vectors));
}


ZEVariant::operator ZEMatrix3x3()
{
	zedsAssert(this->Type != ZE_VRT_MATRIX3X3, "ZEMatrix3x3 conversion operation failed. Variant type mismatched.");
	return *Value.Matrix3x3;
}

ZEVariant::operator ZEMatrix4x4()
{
	zedsAssert(this->Type != ZE_VRT_MATRIX4X4, "ZEMatrix3x3 conversion operation failed. Variant type mismatched.");
	return *Value.Matrix4x4;

}

ZEVariant::operator void*()
{
	zedsAssert(this->Type != ZE_VRT_POINTER, "Pointer conversion operation failed. Variant type mismatched.");
	return Value.Pointer;
}

ZEVariant::ZEVariant()
{
	Type = ZE_VRT_UNDEFINED;
}

ZEVariant::ZEVariant(const ZEVariant& InitialValue)
{
	SetVariant(InitialValue);
}

ZEVariant::ZEVariant(const char* InitialValue)
{
	SetString(InitialValue);
}
	
ZEVariant::ZEVariant(int InitialValue)
{
	SetInteger(InitialValue);
}

ZEVariant::ZEVariant(float InitialValue)
{
	SetFloat(InitialValue);
}

ZEVariant::ZEVariant(bool InitialValue)
{
	SetBoolean(InitialValue);
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

ZEVariant::ZEVariant(void* Pointer)
{
	SetPointer(Pointer);
}

ZEVariant::~ZEVariant()
{
	SetType(ZE_VRT_UNDEFINED);
}

#pragma warning(pop)





