//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEReference.cpp
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

#include "ZEReference.h"

#include "ZEError.h"
#include "ZEMeta/ZEObject.h"

#pragma warning(push)
#pragma warning(disable:4482 4996 4244)

template <typename ZEReturnType, ZETypeType Type>
inline ZEReturnType& ZEReference::ConvertRef() const
{
	if (ValueType.Type != Type)
		zeCriticalError("Variant type mismatch. Can not convert reference type to different reference type.");

	if (ValueType.TypeQualifier == ZE_TQ_REFERENCE)
		return *(ZEReturnType*)Value.Pointer;
	else
		zeCriticalError("Variant is const reference. Can not convert const reference to reference.");
}

template <typename ZEReturnType, ZETypeType Type>
inline const ZEReturnType& ZEReference::ConvertConstRef() const
{
	if (ValueType.Type != Type)
		zeCriticalError("Variant type mismatch. Can not convert reference type to different reference type.");

	return *(const ZEReturnType*)Value.Pointer;
}

void ZEReference::SetType(const ZEType& Type)
{
	ValueType = Type;
}

ZEType ZEReference::GetType() const
{
	return ValueType;
}

void ZEReference::SetReference(const ZEReference& Reference)
{
	ValueType = Reference.ValueType;
	Value.Pointer = Reference.Value.Pointer;
}

void ZEReference::SetInt8Ref(ZEInt8& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_8;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	Value.Pointer = &Reference;
}

void ZEReference::SetInt8ConstRef(const ZEInt8& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_8;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	Value.Pointer = const_cast<ZEInt8*>(&Reference);
}

void ZEReference::SetInt16Ref(ZEInt16& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_16;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	Value.Pointer = &Reference;
}

void ZEReference::SetInt16ConstRef(const ZEInt16& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_16;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	Value.Pointer = const_cast<ZEInt16*>(&Reference);
}

void ZEReference::SetInt32Ref(ZEInt32& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_32;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	Value.Pointer = &Reference;
}

void ZEReference::SetInt32ConstRef(const ZEInt32& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_32;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	Value.Pointer = const_cast<ZEInt32*>(&Reference);
}

void ZEReference::SetInt64Ref(ZEInt64& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_64;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	Value.Pointer = &Reference;
}

void ZEReference::SetInt64ConstRef(const ZEInt64& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_64;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	Value.Pointer = const_cast<ZEInt64*>(&Reference);
}

void ZEReference::SetUInt8Ref(ZEUInt8& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_8;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	Value.Pointer = &Reference;
}

void ZEReference::SetUInt8ConstRef(const ZEUInt8& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_8;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	Value.Pointer = const_cast<ZEUInt8*>(&Reference);
}

void ZEReference::SetUInt16Ref(ZEUInt16& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_16;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	Value.Pointer = &Reference;
}

void ZEReference::SetUInt16ConstRef(const ZEUInt16& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_16;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	Value.Pointer = const_cast<ZEUInt16*>(&Reference);
}

void ZEReference::SetUInt32Ref(ZEUInt32& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_32;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	Value.Pointer = &Reference;
}

void ZEReference::SetUInt32ConstRef(const ZEUInt32& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_32;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	Value.Pointer = const_cast<ZEUInt32*>(&Reference);
}

void ZEReference::SetUInt64Ref(ZEUInt64& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_64;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	Value.Pointer = &Reference;
}

void ZEReference::SetUInt64ConstRef(const ZEUInt64& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_64;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	Value.Pointer = const_cast<ZEUInt64*>(&Reference);
}

void ZEReference::SetFloatRef(float& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_FLOAT;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZEReference::SetFloatConstRef(const float& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_FLOAT;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<float*>(&Value);
}

void ZEReference::SetDoubleRef(double& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_DOUBLE;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZEReference::SetDoubleConstRef(const double& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_DOUBLE;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<double*>(&Value);
}

void ZEReference::SetBooleanRef(bool& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_BOOLEAN;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZEReference::SetBoolConstRef(const bool& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_BOOLEAN;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<bool*>(&Value);
}

void ZEReference::SetVector2Ref(ZEVector2& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR2;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZEReference::SetVector2ConstRef(const ZEVector2& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR2;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<ZEVector2*>(&Value);
}

void ZEReference::SetVector3Ref(ZEVector3& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR3;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZEReference::SetVector3ConstRef(const ZEVector3& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR3;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<ZEVector3*>(&Value);
}

void ZEReference::SetVector4Ref(ZEVector4& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR4;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZEReference::SetVector4ConstRef(const ZEVector4& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR4;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<ZEVector4*>(&Value);
}

void ZEReference::SetQuaternionRef(ZEQuaternion& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_QUATERNION;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZEReference::SetQuaternionConstRef(const ZEQuaternion& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_QUATERNION;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<ZEQuaternion*>(&Value);
}

void ZEReference::SetMatrix3x3Ref(ZEMatrix3x3& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_MATRIX3X3;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZEReference::SetMatrix3x3ConstRef(const ZEMatrix3x3& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_MATRIX3X3;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<ZEMatrix3x3*>(&Value);
}

void ZEReference::SetMatrix4x4Ref(ZEMatrix4x4& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_MATRIX4X4;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZEReference::SetMatrix4x4ConstRef(const ZEMatrix4x4& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_MATRIX4X4;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<ZEMatrix4x4*>(&Value);
}


void ZEReference::SetStringRef(ZEString& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_STRING;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZEReference::SetStringConstRef(const ZEString& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_STRING;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<ZEString*>(&Value);
}

void ZEReference::SetEnumRef(const ZEInt32& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_32;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	Value.Pointer = const_cast<ZEInt32*>(&Reference);
}

void ZEReference::SetObjectPtrRef(ZEObject*& Object)
{
	ZEType Type;
	Type.Type = ZE_TT_OBJECT_PTR;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	Type.Class = Object->GetClass();
	SetType(Type);

	Value.Pointer = &Object;
}

void ZEReference::SetObjectPtrConstRef(const ZEObject*& Object)
{
	ZEType Type;
	Type.Type = ZE_TT_OBJECT_PTR;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	Type.Class = Object->GetClass();
	SetType(Type);

	Value.Pointer = const_cast<ZEObject**>(&Object);
}

void ZEReference::SetClassRef(ZEClass*& Class)
{
	ZEType Type;
	Type.Type = ZE_TT_CLASS;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	Value.Pointer = &Class;
}

void ZEReference::SetClassConstRef(const ZEClass*& Class)
{
	ZEType Type;
	Type.Type = ZE_TT_CLASS;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	Value.Pointer = const_cast<ZEClass**>(&Class);
}

ZEInt8& ZEReference::GetInt8Ref() const
{
	return ConvertRef<ZEInt8, ZE_TT_INTEGER_8>();
}

const ZEInt8& ZEReference::GetInt8ConstRef() const
{
	return ConvertConstRef<ZEInt8, ZE_TT_INTEGER_8>();
}

ZEInt16& ZEReference::GetInt16Ref() const
{
	return ConvertRef<ZEInt16, ZE_TT_INTEGER_16>();
}

const ZEInt16& ZEReference::GetInt16ConstRef() const
{
	return ConvertConstRef<ZEInt16, ZE_TT_INTEGER_16>();
}

ZEInt32& ZEReference::GetInt32Ref() const
{
	return ConvertRef<ZEInt32, ZE_TT_INTEGER_32>();
}

const ZEInt32& ZEReference::GetInt32ConstRef() const
{
	return ConvertConstRef<ZEInt32, ZE_TT_INTEGER_32>();
}

ZEInt64& ZEReference::GetInt64Ref() const
{
	return ConvertRef<ZEInt64, ZE_TT_INTEGER_64>();
}

const ZEInt64& ZEReference::GetInt64ConstRef() const
{
	return ConvertConstRef<ZEInt64, ZE_TT_INTEGER_64>();
}

ZEUInt8& ZEReference::GetUInt8Ref() const
{
	return ConvertRef<ZEUInt8, ZE_TT_UNSIGNED_INTEGER_8>();
}

const ZEUInt8& ZEReference::GetUInt8ConstRef() const
{
	return ConvertConstRef<ZEUInt8, ZE_TT_UNSIGNED_INTEGER_8>();
}

ZEUInt16& ZEReference::GetUInt16Ref() const
{
	return ConvertRef<ZEUInt16, ZE_TT_UNSIGNED_INTEGER_16>();
}

const ZEUInt16& ZEReference::GetUInt16ConstRef() const
{
	return ConvertConstRef<ZEUInt16, ZE_TT_UNSIGNED_INTEGER_16>();
}

ZEUInt32& ZEReference::GetUInt32Ref() const
{
	return ConvertRef<ZEUInt32, ZE_TT_UNSIGNED_INTEGER_32>();
}

const ZEUInt32& ZEReference::GetUInt32ConstRef() const
{
	return ConvertConstRef<ZEUInt32, ZE_TT_UNSIGNED_INTEGER_32>();
}

ZEUInt64& ZEReference::GetUInt64Ref() const
{
	return ConvertRef<ZEUInt64, ZE_TT_UNSIGNED_INTEGER_64>();
}

const ZEUInt64& ZEReference::GetUInt64ConstRef() const
{
	return ConvertConstRef<ZEUInt64, ZE_TT_UNSIGNED_INTEGER_64>();
}

float& ZEReference::GetFloatRef() const
{
	return ConvertRef<float, ZE_TT_FLOAT>();
}

const float& ZEReference::GetFloatConstRef() const
{
	return ConvertConstRef<float, ZE_TT_FLOAT>();
}

double& ZEReference::GetDoubleRef() const
{
	return ConvertRef<double, ZE_TT_DOUBLE>();
}

const double& ZEReference::GetDoubleConstRef() const
{
	return ConvertConstRef<double, ZE_TT_DOUBLE>();
}

bool& ZEReference::GetBoolRef() const
{
	return ConvertRef<bool, ZE_TT_BOOLEAN>();
}

const bool& ZEReference::GetBoolConstRef() const
{
	return ConvertConstRef<bool, ZE_TT_BOOLEAN>();
}

ZEVector2& ZEReference::GetVector2Ref() const
{
	return ConvertRef<ZEVector2, ZE_TT_VECTOR2>();
}

const ZEVector2& ZEReference::GetVector2ConstRef() const
{
	return ConvertConstRef<ZEVector2, ZE_TT_VECTOR2>();
}

ZEVector3& ZEReference::GetVector3Ref() const
{
	return ConvertRef<ZEVector3, ZE_TT_VECTOR3>();
}

const ZEVector3& ZEReference::GetVector3ConstRef() const
{
	return ConvertConstRef<ZEVector3, ZE_TT_VECTOR3>();
}

ZEVector4& ZEReference::GetVector4Ref() const
{
	return ConvertRef<ZEVector4, ZE_TT_VECTOR4>();
}

const ZEVector4& ZEReference::GetVector4ConstRef() const
{
	return ConvertConstRef<ZEVector4, ZE_TT_VECTOR4>();
}

ZEQuaternion& ZEReference::GetQuaternionRef() const
{
	return ConvertRef<ZEQuaternion, ZE_TT_QUATERNION>();
}

const ZEQuaternion& ZEReference::GetQuaternionConstRef() const
{
	return ConvertConstRef<ZEQuaternion, ZE_TT_QUATERNION>();
}

ZEMatrix3x3& ZEReference::GetMatrix3x3Ref() const
{
	return ConvertRef<ZEMatrix3x3, ZE_TT_MATRIX3X3>();
}

const ZEMatrix3x3& ZEReference::GetMatrix3x3ConstRef() const
{
	return ConvertConstRef<ZEMatrix3x3, ZE_TT_MATRIX3X3>();
}

ZEMatrix4x4& ZEReference::GetMatrix4x4Ref() const
{
	return ConvertRef<ZEMatrix4x4, ZE_TT_MATRIX4X4>();
}

const ZEMatrix4x4& ZEReference::GetMatrix4x4ConstRef() const
{
	return ConvertConstRef<ZEMatrix4x4, ZE_TT_MATRIX4X4>();
}

ZEString& ZEReference::GetStringRef() const
{
	return ConvertRef<ZEString, ZE_TT_STRING>();
}

const ZEString& ZEReference::GetStringConstRef() const
{
	return ConvertConstRef<ZEString, ZE_TT_STRING>();
}

ZEObject*& ZEReference::GetObjectPtrRef() const
{
	return ConvertRef<ZEObject*, ZE_TT_OBJECT_PTR>();
}

ZEObject*const& ZEReference::GetObjectPtrConstRef() const
{

	if (ValueType.Type != ZE_TT_OBJECT_PTR)
		zeCriticalError("Variant type mismatch. Can not convert reference type to different reference type.");

	return (ZEObject* const)Value.Pointer;
}

ZEClass*& ZEReference::GetClassRef() const
{
	return ConvertRef<ZEClass*, ZE_TT_CLASS>();
}

ZEClass* const& ZEReference::GetClassConstRef() const
{
	if (ValueType.Type != ZE_TT_CLASS)
		zeCriticalError("Variant type mismatch. Can not convert reference type to different reference type.");

	return (ZEClass* const)Value.Pointer;
}

ZEReference& ZEReference::operator=(const ZEReference& Value)
{
	SetReference(Value);
	return *this;
}

ZEReference& ZEReference::operator=(ZEInt8& Value)
{
	SetInt8Ref(Value);
	return *this;
}

ZEReference& ZEReference::operator=(ZEInt16& Value)
{
	SetInt16Ref(Value);
	return *this;
}

ZEReference& ZEReference::operator=(ZEInt32& Value)
{
	SetInt32Ref(Value);
	return *this;
}

ZEReference& ZEReference::operator=(ZEInt64& Value)
{
	SetInt64Ref(Value);
	return *this;
}

ZEReference& ZEReference::operator=(ZEUInt8& Value)
{
	SetUInt8Ref(Value);
	return *this;
}

ZEReference& ZEReference::operator=(ZEUInt16& Value)
{
	SetUInt16Ref(Value);
	return *this;
}

ZEReference& ZEReference::operator=(ZEUInt32& Value)
{
	SetUInt32Ref(Value);
	return *this;
}

ZEReference& ZEReference::operator=(ZEUInt64& Value)
{
	SetUInt64Ref(Value);
	return *this;
}

ZEReference& ZEReference::operator=(float& Value)
{
	SetFloatRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(double& Value)
{
	SetDoubleRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(bool& Value)
{
	SetBooleanRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(ZEVector2& Value)
{
	SetVector2Ref(Value);
	return *this;
}

ZEReference& ZEReference::operator=(ZEVector3& Value)
{
	SetVector3Ref(Value);
	return *this;
}

ZEReference& ZEReference::operator=(ZEVector4& Value)
{
	SetVector4Ref(Value);
	return *this;
}

ZEReference& ZEReference::operator=(ZEQuaternion& Value)
{
	SetQuaternionRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(ZEMatrix3x3& Value)
{
	SetMatrix3x3Ref(Value);
	return *this;
}

ZEReference& ZEReference::operator=(ZEMatrix4x4& Value)
{
	SetMatrix4x4Ref(Value);
	return *this;
}

ZEReference& ZEReference::operator=(ZEObject*& Value)
{
	SetObjectPtrRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(ZEString& Value)
{
	SetStringRef(Value);
	return *this;
}


ZEReference& ZEReference::operator=(const ZEInt8& Value)
{
	SetInt8ConstRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(const ZEInt16& Value)
{
	SetInt16ConstRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(const ZEInt32& Value)
{
	SetInt32ConstRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(const ZEInt64& Value)
{
	SetInt64ConstRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(const ZEUInt8& Value)
{
	SetUInt8ConstRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(const ZEUInt16& Value)
{
	SetUInt16ConstRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(const ZEUInt32& Value)
{
	SetUInt32ConstRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(const ZEUInt64& Value)
{
	SetUInt64ConstRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(const float& Value)
{
	SetFloatConstRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(const double& Value)
{
	SetDoubleConstRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(const bool& Value)
{
	SetBoolConstRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(const ZEVector2& Value)
{
	SetVector2ConstRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(const ZEVector3& Value)
{
	SetVector3ConstRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(const ZEVector4& Value)
{
	SetVector4ConstRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(const ZEQuaternion& Value)
{
	SetQuaternionConstRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(const ZEMatrix3x3& Value)
{
	SetMatrix3x3ConstRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(const ZEMatrix4x4& Value)
{
	SetMatrix4x4ConstRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(const ZEObject*& Value)
{
	SetObjectPtrConstRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(const ZEString& Value)
{
	SetStringConstRef(Value.ToCString());
	return *this;
}

ZEReference::operator ZEInt8&()
{
	return GetInt8Ref();
}

ZEReference::operator ZEInt16&()
{
	return GetInt16Ref();
}

ZEReference::operator ZEInt32&()
{
	return GetInt32Ref();
}

ZEReference::operator ZEInt64&()
{
	return GetInt64Ref();
}

ZEReference::operator ZEUInt8&()
{
	return GetUInt8Ref();
}

ZEReference::operator ZEUInt16&()
{
	return GetUInt16Ref();
}

ZEReference::operator ZEUInt32&()
{
	return GetUInt32Ref();
}

ZEReference::operator ZEUInt64&()
{
	return GetUInt64Ref();
}

ZEReference::operator float&()
{
	return GetFloatRef();
}

ZEReference::operator double&()
{
	return GetDoubleRef();
}

ZEReference::operator bool&()
{
	return GetBoolRef();
}

ZEReference::operator ZEVector2&()
{
	return GetVector2Ref();
}

ZEReference::operator ZEVector3&()
{
	return GetVector3Ref();
}

ZEReference::operator ZEVector4&()
{
	return GetVector4Ref();
}

ZEReference::operator ZEQuaternion&()
{
	return GetQuaternionRef();
}

ZEReference::operator ZEMatrix3x3&()
{
	return GetMatrix3x3Ref();
}

ZEReference::operator ZEMatrix4x4&()
{
	return GetMatrix4x4Ref();
}

ZEReference::operator ZEObject*&()
{
	return GetObjectPtrRef();
}

ZEReference::operator const ZEInt8&()
{
	return GetInt8ConstRef();
}

ZEReference::operator const ZEInt16&()
{
	return GetInt16ConstRef();
}

ZEReference::operator const ZEInt32&()
{
	return GetInt32ConstRef();
}

ZEReference::operator const ZEInt64&()
{
	return GetInt64ConstRef();
}

ZEReference::operator const ZEUInt8&()
{
	return GetUInt8ConstRef();
}

ZEReference::operator const ZEUInt16&()
{
	return GetUInt16ConstRef();
}

ZEReference::operator const ZEUInt32&()
{
	return GetUInt32ConstRef();
}

ZEReference::operator const ZEUInt64&()
{
	return GetUInt64ConstRef();
}

ZEReference::operator const float&()
{
	return GetFloatConstRef();
}

ZEReference::operator const double&()
{
	return GetDoubleConstRef();
}

ZEReference::operator const bool&()
{
	return GetBoolConstRef();
}

ZEReference::operator const ZEVector2&()
{
	return GetVector2ConstRef();
}

ZEReference::operator const ZEVector3&()
{
	return GetVector3ConstRef();
}

ZEReference::operator const ZEVector4&()
{
	return GetVector4ConstRef();
}

ZEReference::operator const ZEQuaternion&()
{
	return GetQuaternionConstRef();
}

ZEReference::operator const ZEMatrix3x3&()
{
	return GetMatrix3x3ConstRef();
}

ZEReference::operator const ZEMatrix4x4&()
{
	return GetMatrix4x4ConstRef();
}

ZEReference::operator ZEObject*const&()
{
	return GetObjectPtrConstRef();
}

ZEReference::ZEReference()
{
	ValueType.Type = ZE_TT_UNDEFINED;
	ValueType.TypeQualifier = ZE_TQ_REFERENCE;
	Value.Pointer = NULL;
}

ZEReference::ZEReference(const ZEReference& Value)
{
	SetReference(Value);
}

ZEReference::ZEReference(ZEInt8& Value)
{
	SetInt8Ref(Value);
}

ZEReference::ZEReference(ZEInt16& Value)
{
	SetInt16Ref(Value);
}

ZEReference::ZEReference(ZEInt32& Value)
{
	SetInt32Ref(Value);
}

ZEReference::ZEReference(ZEInt64& Value)
{
	SetInt64Ref(Value);
}

ZEReference::ZEReference(ZEUInt8& Value)
{
	SetUInt8Ref(Value);
}

ZEReference::ZEReference(ZEUInt16& Value)
{
	SetUInt16Ref(Value);
}

ZEReference::ZEReference(ZEUInt32& Value)
{
	SetUInt32Ref(Value);
}

ZEReference::ZEReference(ZEUInt64& Value)
{
	SetUInt64Ref(Value);
}

ZEReference::ZEReference(float& Value)
{
	SetFloatRef(Value);
}

ZEReference::ZEReference(double& Value)
{
	SetDoubleRef(Value);
}

ZEReference::ZEReference(bool& Value)
{
	SetBooleanRef(Value);
}

ZEReference::ZEReference(ZEVector2& Value)
{
	SetVector2Ref(Value);
}

ZEReference::ZEReference(ZEVector3& Value)
{
	SetVector3Ref(Value);
}

ZEReference::ZEReference(ZEVector4& Value)
{
	SetVector4Ref(Value);
}

ZEReference::ZEReference(ZEQuaternion& Value)
{
	SetQuaternionRef(Value);
}

ZEReference::ZEReference(ZEMatrix3x3& Value)
{
	SetMatrix3x3Ref(Value);
}

ZEReference::ZEReference(ZEMatrix4x4& Value)
{
	SetMatrix4x4Ref(Value);
}

ZEReference::ZEReference(ZEObject*& Value)
{
	SetObjectPtrRef(Value);
}

ZEReference::ZEReference(ZEString& Value)
{
	SetStringRef(Value);
}

ZEReference::ZEReference(const ZEInt8& Value)
{
	SetInt8ConstRef(Value);
}

ZEReference::ZEReference(const ZEInt16& Value)
{
	SetInt16ConstRef(Value);
}

ZEReference::ZEReference(const ZEInt32& Value)
{
	SetInt32ConstRef(Value);
}

ZEReference::ZEReference(const ZEInt64& Value)
{
	SetInt64ConstRef(Value);
}

ZEReference::ZEReference(const ZEUInt8& Value)
{
	SetUInt8ConstRef(Value);
}

ZEReference::ZEReference(const ZEUInt16& Value)
{
	SetUInt16ConstRef(Value);
}

ZEReference::ZEReference(const ZEUInt32& Value)
{
	SetUInt32ConstRef(Value);
}

ZEReference::ZEReference(const ZEUInt64& Value)
{
	SetUInt64ConstRef(Value);
}

ZEReference::ZEReference(const float& Value)
{
	SetFloatConstRef(Value);
}

ZEReference::ZEReference(const double& Value)
{
	SetDoubleConstRef(Value);
}

ZEReference::ZEReference(const bool& Value)
{
	SetBoolConstRef(Value);
}

ZEReference::ZEReference(const ZEVector2& Value)
{
	SetVector2ConstRef(Value);
}

ZEReference::ZEReference(const ZEVector3& Value)
{
	SetVector3ConstRef(Value);
}

ZEReference::ZEReference(const ZEVector4& Value)
{
	SetVector4ConstRef(Value);
}

ZEReference::ZEReference(const ZEQuaternion& Value)
{
	SetQuaternionConstRef(Value);
}

ZEReference::ZEReference(const ZEMatrix3x3& Value)
{
	SetMatrix3x3ConstRef(Value);
}

ZEReference::ZEReference(const ZEMatrix4x4& Value)
{
	SetMatrix4x4ConstRef(Value);
}

ZEReference::ZEReference(const ZEObject*& Value)
{
	SetObjectPtrConstRef(Value);
}

ZEReference::ZEReference(const ZEString& Value)
{
	SetStringConstRef(Value);
}

ZEReference::~ZEReference()
{

}


#pragma warning(pop)
