//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Variant.cpp
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
//  Orcun's Data Structres Version 2.0 (Zinek Engine)    //
// --------------------------------------------------------- //
//  Copyrights (C) 2005-2007,  Y. Orçun Gökbulut			 //
//  All rights reserved.									 //
///////////////////////////////////////////////////////////////

#include "Variant.h"
#include "Definitions.h"
#include "../Types.h"
#include <string.h>
#pragma warning(push)
#pragma warning(disable:4482 4996)

void ZEVariant::SetType(ZEVariantType NewType)
{
	if (Type == ZEVARIANTTYPE_STRING && Value.String != NULL)
	{
		delete[] Value.String;
		Value.String = NULL;
	}
	else if (Type == ZEVARIANTTYPE_MATRIX3X3 || Type == ZEVARIANTTYPE_MATRIX4X4)
		delete Value.Pointer;

	switch(NewType)
	{
		case ZEVARIANTTYPE_STRING:
			Value.String = NULL;
			break;
		case ZEVARIANTTYPE_MATRIX3X3:
			Value.Matrix3x3 = new ZEMatrix3x3();
			break;
		case ZEVARIANTTYPE_MATRIX4X4:
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
		case ZEVARIANTTYPE_UNDEFINED:
			Type = ZEVARIANTTYPE_UNDEFINED;
			Value.String = NULL;
			break;
		case ZEVARIANTTYPE_NULL:
			SetNull();
			break;
		case ZEVARIANTTYPE_STRING:
			SetString(NewValue.GetString());
			break;
		case ZEVARIANTTYPE_INTEGER:
			SetInteger(NewValue.GetInteger());
			break;
		case ZEVARIANTTYPE_BOOLEAN:
			SetBoolean(NewValue.GetBoolean());
			break;
		case ZEVARIANTTYPE_FLOAT:
			SetFloat(NewValue.GetFloat());
			break;
		case ZEVARIANTTYPE_VECTOR2:
			SetVector2(NewValue.GetVector2());
			break;
		case ZEVARIANTTYPE_VECTOR3:
			SetVector3(NewValue.GetVector3());
			break;
		case ZEVARIANTTYPE_VECTOR4:
			SetVector4(NewValue.GetVector4());
			break;
		case ZEVARIANTTYPE_QUATERNION:
			SetQuaternion(NewValue.GetQuaternion());
			break;
		case ZEVARIANTTYPE_MATRIX3X3:
			SetMatrix3x3(NewValue.GetMatrix3x3());
			break;
		case ZEVARIANTTYPE_MATRIX4X4:
			SetMatrix4x4(NewValue.GetMatrix4x4());
			break;
		case ZEVARIANTTYPE_POINTER:
			SetPointer(NewValue.GetPointer());
			break;
		default:
			ZEDS_ASSERT(true, "ZEVariant::SetVariant operation failed. Error in variant type.");
	}
}

size_t ZEVariant::SizeOf() const
{
	switch(Type)
	{
		case ZEVARIANTTYPE_UNDEFINED:
			return 0;
		case ZEVARIANTTYPE_NULL:
			return 0;
		case ZEVARIANTTYPE_STRING:
			return strlen(Value.String) + 1;
		case ZEVARIANTTYPE_INTEGER:
			return sizeof(int);
		case ZEVARIANTTYPE_BOOLEAN:
			return sizeof(bool);
		case ZEVARIANTTYPE_FLOAT:
			return sizeof(float);
		case ZEVARIANTTYPE_VECTOR2:
			return sizeof(ZEVector2);
		case ZEVARIANTTYPE_VECTOR3:
			return sizeof(ZEVector3);
		case ZEVARIANTTYPE_VECTOR4:
			return sizeof(ZEVector4);
		case ZEVARIANTTYPE_QUATERNION:
			return sizeof(ZEQuaternion);
		case ZEVARIANTTYPE_MATRIX3X3:
			return sizeof(ZEMatrix3x3);
		case ZEVARIANTTYPE_MATRIX4X4:
			return sizeof(ZEMatrix4x4);
		case ZEVARIANTTYPE_POINTER:
			return sizeof(void*);
		default:
			ZEDS_ASSERT(true, "ZEVariant::SetVariant operation failed. Error in variant type.");
			return 0;
	}
}

bool ZEVariant::Serialize(ZESerializer* Serializer)
{
	ZEDWORD _Type = Type;
	ZEDWORD StringSize;
	switch(Type)
	{
		case ZEVARIANTTYPE_UNDEFINED:
		case ZEVARIANTTYPE_NULL:
			Serializer->Write(&_Type, sizeof(ZEDWORD), 1);
			break;
		case ZEVARIANTTYPE_STRING:
			Serializer->Write(&_Type, sizeof(ZEDWORD), 1);
			StringSize = strlen(Value.String) + 1;
			Serializer->Write(&StringSize, sizeof(ZEDWORD), 1);
			Serializer->Write(Value.String, sizeof(char), StringSize);
			break;
		case ZEVARIANTTYPE_INTEGER:
			Serializer->Write(&_Type, sizeof(ZEDWORD), 1);
			Serializer->Write(&Value.Integer, sizeof(int), 1);
			break;
		case ZEVARIANTTYPE_BOOLEAN:
			Serializer->Write(&_Type, sizeof(ZEDWORD), 1);
			Serializer->Write(&Value.Boolean, sizeof(bool), 1);
			break;
		case ZEVARIANTTYPE_FLOAT:
			Serializer->Write(&_Type, sizeof(ZEDWORD), 1);
			Serializer->Write(&Value.Float, sizeof(float), 1);
			break;
		case ZEVARIANTTYPE_VECTOR2:
			Serializer->Write(&_Type, sizeof(ZEDWORD), 1);
			Serializer->Write(&Value.Vectors, sizeof(ZEVector2), 1);
			break;
		case ZEVARIANTTYPE_VECTOR3:
			Serializer->Write(&_Type, sizeof(ZEDWORD), 1);
			Serializer->Write(&Value.Vectors, sizeof(ZEVector3), 1);
			break;
		case ZEVARIANTTYPE_VECTOR4:
			Serializer->Write(&_Type, sizeof(ZEDWORD), 1);
			Serializer->Write(&Value.Vectors, sizeof(ZEVector4), 1);
			break;
		case ZEVARIANTTYPE_QUATERNION:
			Serializer->Write(&_Type, sizeof(ZEDWORD), 1);
			Serializer->Write(&Value.Vectors, sizeof(ZEQuaternion), 1);
			break;
		case ZEVARIANTTYPE_MATRIX3X3:
			Serializer->Write(&_Type, sizeof(ZEDWORD), 1);
			Serializer->Write(Value.Matrix3x3, sizeof(ZEMatrix3x3), 1);
			break;
		case ZEVARIANTTYPE_MATRIX4X4:
			Serializer->Write(&_Type, sizeof(ZEDWORD), 1);
			Serializer->Write(Value.Matrix4x4, sizeof(ZEMatrix4x4), 1);
			break;
		case ZEVARIANTTYPE_POINTER:
			ZEDS_ASSERT(true, "Can not serialize pointer type.");
			return false;
		default:
			ZEDS_ASSERT(true, "Wrong variant type.");
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
		case ZEVARIANTTYPE_UNDEFINED:
			SetType(ZEVARIANTTYPE_UNDEFINED);
			break;
		case ZEVARIANTTYPE_NULL:
			SetType(ZEVARIANTTYPE_NULL);
			break;
		case ZEVARIANTTYPE_STRING:
			SetType(ZEVARIANTTYPE_STRING);
			Unserializer->Read(&StringSize, sizeof(ZEDWORD), 1);
			Value.String = new char[StringSize];
			Unserializer->Read(Value.String, sizeof(char), StringSize);
			break;
		case ZEVARIANTTYPE_INTEGER:
			SetType(ZEVARIANTTYPE_INTEGER);
			Unserializer->Read(&Value.Integer, sizeof(int), 1);
			break;
		case ZEVARIANTTYPE_BOOLEAN:
			SetType(ZEVARIANTTYPE_BOOLEAN);
			Unserializer->Read(&Value.Boolean, sizeof(bool), 1);
			break;
		case ZEVARIANTTYPE_FLOAT:
			SetType(ZEVARIANTTYPE_FLOAT);
			Unserializer->Read(&Value.Float, sizeof(float), 1);
			break;
		case ZEVARIANTTYPE_VECTOR2:
			SetType(ZEVARIANTTYPE_VECTOR2);
			Unserializer->Read(&Value.Vectors, sizeof(ZEVector2), 1);
			break;
		case ZEVARIANTTYPE_VECTOR3:
			SetType(ZEVARIANTTYPE_VECTOR3);
			Unserializer->Read(&Value.Vectors, sizeof(ZEVector3), 1);
			break;
		case ZEVARIANTTYPE_VECTOR4:
			SetType(ZEVARIANTTYPE_VECTOR4);
			Unserializer->Read(&Value.Vectors, sizeof(ZEVector4), 1);
			break;
		case ZEVARIANTTYPE_QUATERNION:
			SetType(ZEVARIANTTYPE_QUATERNION);
			Unserializer->Read(&Value.Vectors, sizeof(ZEQuaternion), 1);
			break;
		case ZEVARIANTTYPE_MATRIX3X3:
			SetType(ZEVARIANTTYPE_MATRIX3X3);
			Unserializer->Read(Value.Matrix3x3, sizeof(ZEMatrix3x3), 1);
			break;
		case ZEVARIANTTYPE_MATRIX4X4:
			SetType(ZEVARIANTTYPE_MATRIX4X4);
			Unserializer->Read(Value.Matrix4x4, sizeof(ZEMatrix4x4), 1);
			break;
		case ZEVARIANTTYPE_POINTER:
			SetType(ZEVARIANTTYPE_UNDEFINED);
			ZEDS_ASSERT(true, "Can not unserialize pointer type.");
			return false;
		default:
			SetType(ZEVARIANTTYPE_UNDEFINED);
			ZEDS_ASSERT(true, "Wrong variant type.");
			return false;
	}
	return true;
}


void ZEVariant::SetString(const char *NewValue)
{
	SetType(ZEVARIANTTYPE_STRING);
	Value.String = new char[strlen(NewValue) + 1];
	strcpy(Value.String, NewValue);
}

void ZEVariant::SetInteger(int Value)
{
	Type = ZEVARIANTTYPE_INTEGER;
	this->Value.Integer = Value;
}

void ZEVariant::SetFloat(float Value)
{
	Type = ZEVARIANTTYPE_FLOAT;
	this->Value.Float = Value;
}

void ZEVariant::SetBoolean(bool Value)
{
	SetType(ZEVARIANTTYPE_BOOLEAN);
	this->Value.Boolean = Value;
}

void ZEVariant::SetVector2(const ZEVector2& Vector)
{
	SetType(ZEVARIANTTYPE_VECTOR2);
	(*(ZEVector2*)(&this->Value.Vectors)) = Vector;
}

void ZEVariant::SetVector3(const ZEVector3& Vector)
{
	SetType(ZEVARIANTTYPE_VECTOR3);
  	(*(ZEVector3*)(&this->Value.Vectors)) = Vector;
}

void ZEVariant::SetVector4(const ZEVector4& Vector)
{
	SetType(ZEVARIANTTYPE_VECTOR4);
  	(*(ZEVector4*)(&this->Value.Vectors)) = Vector;
}

void ZEVariant::SetQuaternion(const ZEQuaternion& Quaternion)
{
	SetType(ZEVARIANTTYPE_QUATERNION);
	(*(ZEQuaternion*)(&this->Value.Vectors)) = Quaternion;
}

void ZEVariant::SetMatrix3x3(const ZEMatrix3x3& Matrix)
{
	SetType(ZEVARIANTTYPE_MATRIX3X3);
	*this->Value.Matrix3x3 = Matrix;

}

void ZEVariant::SetMatrix4x4(const ZEMatrix4x4& Matrix)
{
	SetType(ZEVARIANTTYPE_MATRIX3X3);
	*this->Value.Matrix4x4 = Matrix;
}

void ZEVariant::SetPointer(void* Pointer)
{
	SetType(ZEVARIANTTYPE_POINTER);
	Value.Pointer = Pointer;
}

void ZEVariant::SetNull()
{
	SetType(ZEVARIANTTYPE_NULL);
}

char* ZEVariant::GetString() const
{
	ZEDS_ASSERT(this->Type != ZEVARIANTTYPE_STRING, "ZEVariant::GetString operation failed. Variant type mismatched.");
	return Value.String;
}

int ZEVariant::GetInteger() const
{
	ZEDS_ASSERT(this->Type != ZEVARIANTTYPE_INTEGER, "ZEVariant::GetInteger operation failed. Variant type mismatched.");
	return Value.Integer;
}

float ZEVariant::GetFloat() const
{
	ZEDS_ASSERT(this->Type != ZEVARIANTTYPE_FLOAT, "ZEVariant::GetFloat operation failed. Variant type mismatched.");
	return Value.Float;
}

bool ZEVariant::GetBoolean() const
{
	ZEDS_ASSERT(this->Type != ZEVARIANTTYPE_BOOLEAN, "ZEVariant::GetBoolean operation failed. Variant type mismatched.");
	return Value.Boolean;
}
ZEVector2& ZEVariant::GetVector2() const
{
	ZEDS_ASSERT(this->Type != ZEVARIANTTYPE_VECTOR2, "ZEVariant::GetVector2 operation failed. Variant type mismatched.");
	return *((ZEVector2*)&Value.Vectors);
}

ZEVector3& ZEVariant::GetVector3() const
{
	ZEDS_ASSERT(this->Type != ZEVARIANTTYPE_VECTOR3, "ZEVariant::GetVector3 operation failed. Variant type mismatched.");
	return *((ZEVector3*)&Value.Vectors);
}

ZEVector4& ZEVariant::GetVector4() const
{
	ZEDS_ASSERT(this->Type != ZEVARIANTTYPE_VECTOR4, "ZEVariant::GetVector4 operation failed. Variant type mismatched.");
	return *((ZEVector4*)&Value.Vectors);
}

ZEQuaternion& ZEVariant::GetQuaternion() const
{
	ZEDS_ASSERT(this->Type != ZEVARIANTTYPE_QUATERNION, "ZEVariant::Quaternion operation failed. Variant type mismatched.");
	return *((ZEQuaternion*)&Value.Vectors);
}

ZEMatrix3x3& ZEVariant::GetMatrix3x3() const
{
	ZEDS_ASSERT(this->Type != ZEVARIANTTYPE_MATRIX3X3, "ZEVariant::GetMatrix3x3 operation failed. Variant type mismatched.");
	return *Value.Matrix3x3;
}

ZEMatrix4x4& ZEVariant::GetMatrix4x4() const
{
	ZEDS_ASSERT(this->Type != ZEVARIANTTYPE_MATRIX4X4, "ZEVariant::GetMatrix4x4 operation failed. Variant type mismatched.");
	return *Value.Matrix4x4;
}

void* ZEVariant::GetPointer() const
{
	ZEDS_ASSERT(this->Type != ZEVARIANTTYPE_POINTER, "ZEVariant::GetPointer operation failed. Variant type mismatched.");
	return Value.Pointer;
}

bool ZEVariant::IsNull()	const
{
	return (Type == ZEVARIANTTYPE_NULL);
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
	ZEDS_ASSERT(this->Type != ZEVARIANTTYPE_STRING, "String conversion operation failed. Variant type mismatched.");
	return Value.String;
}

ZEVariant::operator int()
{
	ZEDS_ASSERT(this->Type != ZEVARIANTTYPE_INTEGER || this->Type != ZEVARIANTTYPE_FLOAT, "Integer conversion operation failed. Variant type mismatched.");
	if (this->Type == ZEVARIANTTYPE_FLOAT)
		return (int)Value.Float;
	else
		return Value.Integer;
}

ZEVariant::operator float()
{
	ZEDS_ASSERT(this->Type != ZEVARIANTTYPE_INTEGER || this->Type != ZEVARIANTTYPE_FLOAT, "Float conversion operation failed. Variant type mismatched.");
	if (this->Type == ZEVARIANTTYPE_INTEGER)
		return (float)Value.Integer;
	else
		return Value.Float;
}

ZEVariant::operator bool()
{
	ZEDS_ASSERT(this->Type != ZEVARIANTTYPE_BOOLEAN, "Boolean conversion operation failed. Variant type mismatched.");
	return Value.Boolean;
}

ZEVariant::operator ZEVector2()
{
	ZEDS_ASSERT(this->Type != ZEVARIANTTYPE_VECTOR2, "ZEVector2 conversion operation failed. Variant type mismatched.");
	return *((ZEVector2*)(&Value.Vectors));
}

ZEVariant::operator ZEVector3()
{
	ZEDS_ASSERT(this->Type != ZEVARIANTTYPE_VECTOR3, "ZEVector3 conversion operation failed. Variant type mismatched.");
	return *((ZEVector3*)(&Value.Vectors));
}

ZEVariant::operator ZEVector4()
{
	ZEDS_ASSERT(this->Type != ZEVARIANTTYPE_VECTOR4, "ZEVector4 conversion operation failed. Variant type mismatched.");
	return *((ZEVector4*)(&Value.Vectors));
}

ZEVariant::operator ZEQuaternion()
{
	ZEDS_ASSERT(this->Type != ZEVARIANTTYPE_QUATERNION, "ZEQuaternion conversion operation failed. Variant type mismatched.");
	return *((ZEQuaternion*)(&Value.Vectors));
}


ZEVariant::operator ZEMatrix3x3()
{
	ZEDS_ASSERT(this->Type != ZEVARIANTTYPE_MATRIX3X3, "ZEMatrix3x3 conversion operation failed. Variant type mismatched.");
	return *Value.Matrix3x3;
}

ZEVariant::operator ZEMatrix4x4()
{
	ZEDS_ASSERT(this->Type != ZEVARIANTTYPE_MATRIX4X4, "ZEMatrix3x3 conversion operation failed. Variant type mismatched.");
	return *Value.Matrix4x4;

}

ZEVariant::operator void*()
{
	ZEDS_ASSERT(this->Type != ZEVARIANTTYPE_POINTER, "Pointer conversion operation failed. Variant type mismatched.");
	return Value.Pointer;
}

ZEVariant::ZEVariant()
{
	Type = ZEVARIANTTYPE_UNDEFINED;
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
	SetType(ZEVARIANTTYPE_UNDEFINED);
}

#pragma warning(pop)
