//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZENewVariant.cpp
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

#include "ZENewVariant.h"
#include "ZEError.h"

#include "ZEReference.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEMath/ZEMatrix.h"

#include "ZEMeta/ZEObject.h"

#pragma warning(push)
#pragma warning(disable:4482 4996 4244)

template<typename ZEReturnType>
inline ZEReturnType ZENewVariant::ConvertIntegerValue() const
{
	if (ValueType.TypeQualifier ==  ZE_TQ_VALUE)
	{
		if (ValueType.Type < 2)
			return 0;
		if (ValueType.Type < ZE_TT_UNSIGNED_INTEGER_64)
			return Value.Int32;
		else if (ValueType.Type < ZE_TT_FLOAT)
			return Value.Int64;
		else if (ValueType.Type == ZE_TT_FLOAT)
			return Value.Float;
		else if (ValueType.Type == ZE_TT_DOUBLE)
			return Value.Double;
		else if (ValueType.Type == ZE_TT_BOOLEAN)
			return Value.Boolean;
		else
			zeCriticalError("Variant type mismatch. Can not convert non-numerical non-scaler types.");
	}
	else
	{
		switch(ValueType.Type)
		{
			case ZE_TT_INTEGER_8:
				return *(const ZEInt8*)Value.Pointer;
			case ZE_TT_INTEGER_16:
				return *(const ZEInt16*)Value.Pointer;
			case ZE_TT_INTEGER_32:
				return *(const ZEInt32*)Value.Pointer;
			case ZE_TT_INTEGER_64:
				return *(const ZEInt64*)Value.Pointer;
			case ZE_TT_UNSIGNED_INTEGER_8:
				return *(const ZEUInt8*)Value.Pointer;
			case ZE_TT_UNSIGNED_INTEGER_16:
				return *(const ZEUInt16*)Value.Pointer;
			case ZE_TT_UNSIGNED_INTEGER_32:
				return *(const ZEUInt32*)Value.Pointer;
			case ZE_TT_UNSIGNED_INTEGER_64:
				return *(const ZEInt64*)Value.Pointer;
			case ZE_TT_FLOAT:
				return *(const float*)Value.Pointer;
			case ZE_TT_DOUBLE:
				return *(const double*)Value.Pointer;
			case ZE_TT_BOOLEAN:
				return *(const bool*)Value.Pointer;
			default:
				zeCriticalError("Variant type mismatch. Can not convert non-numerical non-scaler types.");
		}
	}
	return 0;
}

template <typename ZEReturnType, ZETypeType Type>
inline ZEReturnType& ZENewVariant::ConvertRef() const
{
	if (ValueType.Type != Type)
		zeCriticalError("Variant type mismatch. Can not convert reference type to different reference type.");

	if (ValueType.TypeQualifier == ZE_TQ_REFERENCE)
		return *(ZEReturnType*)Value.Pointer;
	else if (ValueType.TypeQualifier == ZE_TQ_VALUE)
		zeCriticalError("Variant is a value. Can not convert value to reference.");
	else
		zeCriticalError("Variant is const reference. Can not convert const reference to reference.");
}

template <typename ZEReturnType, ZETypeType Type>
inline const ZEReturnType& ZENewVariant::ConvertConstRef() const
{
	if (ValueType.Type != Type)
		zeCriticalError("Variant type mismatch. Can not convert reference type to different reference type.");

	if (ValueType.TypeQualifier == ZE_TQ_VALUE)
		zeCriticalError("Variant is a value. Can not convert value to const reference.");

	return *(const ZEReturnType*)Value.Pointer;
}

void ZENewVariant::SetType(const ZEType& NewType)
{
	if (ValueType == NewType)
		return;

	if (ValueType.TypeQualifier == ZE_TQ_VALUE && Value.Pointer != NULL)
	{
		if (ValueType.Type == ZE_TT_OBJECT || ValueType.Type == ZE_TT_ARRAY)
			Deleter(Value.Pointer);
		else if (ValueType.Type == ZE_TT_STRING)
			Value.String.Clear();
		else if (ValueType.Type == ZE_TT_MATRIX3X3)
			delete (ZEMatrix3x3*)Value.Pointer;
		else if (ValueType.Type == ZE_TT_MATRIX4X4)
			delete (ZEMatrix4x4*)Value.Pointer;

		Value.Pointer = NULL;
	}

	Cloner = NULL;
	Deleter = NULL;
	ValueType = NewType;
}

const ZEType& ZENewVariant::GetType() const
{
	return ValueType;
}

void ZENewVariant::SetUndefined()
{
	ValueType = ZEType();
}

void ZENewVariant::SetVariant(const ZENewVariant& Variant)
{
	SetType(Variant.ValueType);

	if (ValueType.TypeQualifier == ZE_TQ_VALUE)
	{
		if (ValueType.Type == ZE_TT_OBJECT || ValueType.Type == ZE_TT_ARRAY || ValueType.Type == ZE_TT_LIST)
			Value.Pointer = Variant.Cloner(Variant.Value.Pointer);
		else if (ValueType.Type == ZE_TT_MATRIX3X3)
			Value.Pointer = new ZEMatrix4x4(*(const ZEMatrix4x4*)Variant.Value.Pointer);
		else if (ValueType.Type == ZE_TT_MATRIX4X4)
			Value.Pointer = new ZEMatrix4x4(*(const ZEMatrix4x4*)Variant.Value.Pointer);
		else if (ValueType.Type == ZE_TT_STRING)
			Value.String = Variant.Value.String;
		else
			Value.Int64 = Variant.Value.Int64;
	}
	else
		Value.Int64 = Variant.Value.Int64;
}

void ZENewVariant::SetReference(const ZEReference& Reference)
{
	SetType(Reference.GetType());
	Value.Pointer = Reference.Value.Pointer;
}

void ZENewVariant::SetInt8(ZEInt8 Value)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_8;
	SetType(Type);

	this->Value.Int32 = Value;
}

void ZENewVariant::SetInt8Ref(ZEInt8& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_8;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	Value.Pointer = &Reference;
}

void ZENewVariant::SetInt8ConstRef(const ZEInt8& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_8;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	Value.Pointer = const_cast<ZEInt8*>(&Reference);
}

void ZENewVariant::SetInt16(ZEInt16 Value)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_16;
	SetType(Type);

	this->Value.Int32 = Value;
}

void ZENewVariant::SetInt16Ref(ZEInt16& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_16;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	Value.Pointer = &Reference;
}

void ZENewVariant::SetInt16ConstRef(const ZEInt16& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_16;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	Value.Pointer = const_cast<ZEInt16*>(&Reference);
}

void ZENewVariant::SetInt32(ZEInt32 Value)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_32;
	SetType(Type);

	this->Value.Int32 = Value;
}

void ZENewVariant::SetInt32Ref(ZEInt32& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_32;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	Value.Pointer = &Reference;
}

void ZENewVariant::SetInt32ConstRef(const ZEInt32& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_32;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	Value.Pointer = const_cast<ZEInt32*>(&Reference);
}

void ZENewVariant::SetInt64(ZEInt64 Value)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_64;
	SetType(Type);

	this->Value.Int64 = Value;
}

void ZENewVariant::SetInt64Ref(ZEInt64& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_64;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	Value.Pointer = &Reference;
}

void ZENewVariant::SetInt64ConstRef(const ZEInt64& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_64;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	Value.Pointer = const_cast<ZEInt64*>(&Reference);
}

void ZENewVariant::SetUInt8(ZEUInt8 Value)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_8;
	SetType(Type);

	this->Value.Int32 = Value;
}

void ZENewVariant::SetUInt8Ref(ZEUInt8& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_8;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	Value.Pointer = &Reference;
}

void ZENewVariant::SetUInt8ConstRef(const ZEUInt8& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_8;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	Value.Pointer = const_cast<ZEUInt8*>(&Reference);
}

void ZENewVariant::SetUInt16(ZEUInt16 Value)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_16;
	SetType(Type);

	this->Value.Int32 = Value;
}

void ZENewVariant::SetUInt16Ref(ZEUInt16& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_16;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	Value.Pointer = &Reference;
}

void ZENewVariant::SetUInt16ConstRef(const ZEUInt16& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_16;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	Value.Pointer = const_cast<ZEUInt16*>(&Reference);
}

void ZENewVariant::SetUInt32(ZEUInt32 Value)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_32;
	SetType(Type);

	this->Value.Int32 = Value;
}

void ZENewVariant::SetUInt32Ref(ZEUInt32& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_32;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	Value.Pointer = &Reference;
}

void ZENewVariant::SetUInt32ConstRef(const ZEUInt32& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_32;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	Value.Pointer = const_cast<ZEUInt32*>(&Reference);
}

void ZENewVariant::SetUInt64(ZEUInt64 Value)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_64;
	SetType(Type);

	this->Value.Int64 = Value;
}

void ZENewVariant::SetUInt64Ref(ZEUInt64& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_64;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	Value.Pointer = &Reference;
}

void ZENewVariant::SetUInt64ConstRef(const ZEUInt64& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_64;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	Value.Pointer = const_cast<ZEUInt64*>(&Reference);
}

void ZENewVariant::SetFloat(float Value)
{
	ZEType Type;
	Type.Type = ZE_TT_FLOAT;
	Type.SubTypeQualifier = ZE_TQ_VALUE;
	SetType(Type);

	this->Value.Float = Value;
}

void ZENewVariant::SetFloatRef(float& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_FLOAT;
	Type.SubTypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZENewVariant::SetFloatConstRef(const float& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_FLOAT;
	Type.SubTypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<float*>(&Value);
}

void ZENewVariant::SetDouble(double Value)
{
	ZEType Type;
	Type.Type = ZE_TT_DOUBLE;
	Type.SubTypeQualifier = ZE_TQ_VALUE;
	SetType(Type);

	this->Value.Double = Value;
}

void ZENewVariant::SetDoubleRef(double& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_DOUBLE;
	Type.SubTypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZENewVariant::SetDoubleConstRef(const double& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_DOUBLE;
	Type.SubTypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<double*>(&Value);
}

void ZENewVariant::SetBool(bool Value)
{
	ZEType Type;
	Type.Type = ZE_TT_BOOLEAN;
	Type.SubTypeQualifier = ZE_TQ_VALUE;
	SetType(Type);

	this->Value.Boolean = Value;
}

void ZENewVariant::SetBoolRef(bool& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_BOOLEAN;
	Type.SubTypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZENewVariant::SetBoolConstRef(const bool& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_BOOLEAN;
	Type.SubTypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<bool*>(&Value);
}

void ZENewVariant::SetEnum(ZEInt32 Value)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_32;
	SetType(Type);

	this->Value.Int32 = Value;
}

void ZENewVariant::SetVector2(const ZEVector2& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR2;
	SetType(Type);

	memcpy(this->Value.Vectors, &Value, sizeof(ZEVector2));
}

void ZENewVariant::SetVector2Ref(ZEVector2& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR2;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZENewVariant::SetVector2ConstRef(const ZEVector2& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR2;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<ZEVector2*>(&Value);
}

void ZENewVariant::SetVector3(const ZEVector3& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR3;
	SetType(Type);

	memcpy(this->Value.Vectors, &Value, sizeof(ZEVector3));
}

void ZENewVariant::SetVector3Ref(ZEVector3& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR3;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZENewVariant::SetVector3ConstRef(const ZEVector3& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR3;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<ZEVector3*>(&Value);
}

void ZENewVariant::SetVector4(const ZEVector4& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR4;
	SetType(Type);

	memcpy(this->Value.Vectors, &Value, sizeof(ZEVector4));
}

void ZENewVariant::SetVector4Ref(ZEVector4& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR4;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZENewVariant::SetVector4ConstRef(const ZEVector4& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR4;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<ZEVector4*>(&Value);
}

void ZENewVariant::SetQuaternion(const ZEQuaternion& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_QUATERNION;
	SetType(Type);

	memcpy(this->Value.Vectors, &Value, sizeof(ZEQuaternion));
}

void ZENewVariant::SetQuaternionRef(ZEQuaternion& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_QUATERNION;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZENewVariant::SetQuaternionConstRef(const ZEQuaternion& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_QUATERNION;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<ZEQuaternion*>(&Value);
}

void ZENewVariant::SetMatrix3x3(const ZEMatrix3x3& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_MATRIX3X3;
	SetType(Type);

	this->Value.Pointer = new ZEMatrix3x3(Value);	
}

void ZENewVariant::SetMatrix3x3Ref(ZEMatrix3x3& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_MATRIX3X3;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZENewVariant::SetMatrix3x3ConstRef(const ZEMatrix3x3& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_MATRIX3X3;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<ZEMatrix3x3*>(&Value);
}

void ZENewVariant::SetMatrix4x4(const ZEMatrix4x4& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_MATRIX4X4;
	SetType(Type);

	this->Value.Pointer = new ZEMatrix4x4(Value);	
}

void ZENewVariant::SetMatrix4x4Ref(ZEMatrix4x4& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_MATRIX4X4;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZENewVariant::SetMatrix4x4ConstRef(const ZEMatrix4x4& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_MATRIX4X4;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<ZEMatrix4x4*>(&Value);
}

void ZENewVariant::SetString(const char* Value)
{
	ZEType Type;
	Type.Type = ZE_TT_STRING;
	SetType(Type);

	this->Value.String = Value;
}

void ZENewVariant::SetString(const ZEString& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_STRING;
	SetType(Type);

	this->Value.String = Value;
}

void ZENewVariant::SetStringRef(ZEString& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_STRING;
	Type.SubTypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZENewVariant::SetStringConstRef(const ZEString& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_STRING;
	Type.SubTypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<ZEString*>(&Value);
}

void ZENewVariant::SetObjectPtr(ZEObject* Object)
{
	ZEType Type;
	Type.Type = ZE_TT_OBJECT_PTR;
	Type.TypeQualifier = ZE_TQ_VALUE;
	Type.Class = Object->GetClass();
	SetType(Type);

	Value.Pointer = Object;
}

void ZENewVariant::SetObjectPtrRef(ZEObject*& Object)
{
	ZEType Type;
	Type.Type = ZE_TT_OBJECT_PTR;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	Type.Class = Object->GetClass();
	SetType(Type);

	Value.Pointer = &Object;
}

void ZENewVariant::SetObjectPtrConstRef(const ZEObject*& Object)
{
	ZEType Type;
	Type.Type = ZE_TT_OBJECT_PTR;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	Type.Class = Object->GetClass();
	SetType(Type);

	Value.Pointer = const_cast<ZEObject**>(&Object);
}

void ZENewVariant::SetClass(ZEClass* Class)
{
	ZEType Type;
	Type.Type = ZE_TT_CLASS;
	Type.TypeQualifier = ZE_TQ_VALUE;
	SetType(Type);

	Value.Pointer = Class;

}

void ZENewVariant::SetClassRef(ZEClass*& Class)
{
	ZEType Type;
	Type.Type = ZE_TT_CLASS;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	Value.Pointer = &Class;
}

void ZENewVariant::SetClassConstRef(const ZEClass*& Class)
{
	ZEType Type;
	Type.Type = ZE_TT_CLASS;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	Value.Pointer = const_cast<ZEClass**>(&Class);
}

ZEInt8 ZENewVariant::GetInt8() const
{
	return ConvertIntegerValue<ZEInt8>();
}

ZEInt8& ZENewVariant::GetInt8Ref() const
{
	return ConvertRef<ZEInt8, ZE_TT_INTEGER_8>();
}

const ZEInt8& ZENewVariant::GetInt8ConstRef() const
{
	return ConvertConstRef<ZEInt8, ZE_TT_INTEGER_8>();
}

ZEInt16 ZENewVariant::GetInt16() const
{
	return ConvertIntegerValue<ZEInt16>();
}

ZEInt16& ZENewVariant::GetInt16Ref() const
{
	return ConvertRef<ZEInt16, ZE_TT_INTEGER_16>();
}

const ZEInt16& ZENewVariant::GetInt16ConstRef() const
{
	return ConvertConstRef<ZEInt16, ZE_TT_INTEGER_16>();
}

ZEInt32 ZENewVariant::GetInt32() const
{
	return ConvertIntegerValue<ZEInt32>();
}

ZEInt32& ZENewVariant::GetInt32Ref() const
{
	return ConvertRef<ZEInt32, ZE_TT_INTEGER_32>();
}

const ZEInt32& ZENewVariant::GetInt32ConstRef() const
{
	return ConvertConstRef<ZEInt32, ZE_TT_INTEGER_32>();
}

ZEInt64 ZENewVariant::GetInt64() const
{
	return ConvertIntegerValue<ZEInt64>();
}

ZEInt64& ZENewVariant::GetInt64Ref() const
{
	return ConvertRef<ZEInt64, ZE_TT_INTEGER_64>();
}

const ZEInt64& ZENewVariant::GetInt64ConstRef() const
{
	return ConvertConstRef<ZEInt64, ZE_TT_INTEGER_64>();
}

ZEUInt8 ZENewVariant::GetUInt8() const
{
	return ConvertIntegerValue<ZEUInt8>();
}

ZEUInt8& ZENewVariant::GetUInt8Ref() const
{
	return ConvertRef<ZEUInt8, ZE_TT_UNSIGNED_INTEGER_8>();
}

const ZEUInt8& ZENewVariant::GetUInt8ConstRef() const
{
	return ConvertConstRef<ZEUInt8, ZE_TT_UNSIGNED_INTEGER_8>();
}

ZEUInt16 ZENewVariant::GetUInt16() const
{
	return ConvertIntegerValue<ZEUInt16>();
}

ZEUInt16& ZENewVariant::GetUInt16Ref() const
{
	return ConvertRef<ZEUInt16, ZE_TT_UNSIGNED_INTEGER_16>();
}

const ZEUInt16& ZENewVariant::GetUInt16ConstRef() const
{
	return ConvertConstRef<ZEUInt16, ZE_TT_UNSIGNED_INTEGER_16>();
}

ZEUInt32 ZENewVariant::GetUInt32() const
{
	return ConvertIntegerValue<ZEUInt32>();
}

ZEUInt32& ZENewVariant::GetUInt32Ref() const
{
	return ConvertRef<ZEUInt32, ZE_TT_UNSIGNED_INTEGER_32>();
}

const ZEUInt32& ZENewVariant::GetUInt32ConstRef() const
{
	return ConvertConstRef<ZEUInt32, ZE_TT_UNSIGNED_INTEGER_32>();
}

ZEUInt64 ZENewVariant::GetUInt64() const
{
	return ConvertIntegerValue<ZEUInt64>();
}

ZEUInt64& ZENewVariant::GetUInt64Ref() const
{
	return ConvertRef<ZEUInt64, ZE_TT_UNSIGNED_INTEGER_64>();
}

const ZEUInt64& ZENewVariant::GetUInt64ConstRef() const
{
	return ConvertConstRef<ZEUInt64, ZE_TT_UNSIGNED_INTEGER_64>();
}

float ZENewVariant::GetFloat() const
{
	return ConvertIntegerValue<float>();
}

float& ZENewVariant::GetFloatRef() const
{
	return ConvertRef<float, ZE_TT_FLOAT>();
}

const float& ZENewVariant::GetFloatConstRef() const
{
	return ConvertConstRef<float, ZE_TT_FLOAT>();
}

double ZENewVariant::GetDouble() const
{
	return ConvertIntegerValue<double>();
}

double& ZENewVariant::GetDoubleRef() const
{
	return ConvertRef<double, ZE_TT_DOUBLE>();
}

const double& ZENewVariant::GetDoubleConstRef() const
{
	return ConvertConstRef<double, ZE_TT_DOUBLE>();
}

bool ZENewVariant::GetBool() const
{
	return ConvertIntegerValue<bool>();
}

bool& ZENewVariant::GetBoolRef() const
{
	return ConvertRef<bool, ZE_TT_BOOLEAN>();
}

const bool& ZENewVariant::GetBoolConstRef() const
{
	return ConvertConstRef<bool, ZE_TT_BOOLEAN>();
}

ZEInt32 ZENewVariant::GetEnum() const
{
	return ConvertIntegerValue<ZEInt32>();
}

const ZEVector2& ZENewVariant::GetVector2() const
{
	if (ValueType.Type != ZE_TT_VECTOR2)
		zeCriticalError("Variant type mismatch. Can not convert non-numerical non-scaler types.");

	if (ValueType.TypeQualifier == ZE_TQ_VALUE)
		return *(ZEVector2*)&Value.Vectors;
	else
		return *(ZEVector2*)&Value.Pointer;
}

ZEVector2& ZENewVariant::GetVector2Ref() const
{
	return ConvertRef<ZEVector2, ZE_TT_VECTOR2>();
}

const ZEVector2& ZENewVariant::GetVector2ConstRef() const
{
	return ConvertConstRef<ZEVector2, ZE_TT_VECTOR2>();
}

const ZEVector3& ZENewVariant::GetVector3() const
{
	if (ValueType.Type != ZE_TT_VECTOR3)
		zeCriticalError("Variant type mismatch. Can not convert non-numerical non-scaler types.");

	if (ValueType.TypeQualifier == ZE_TQ_VALUE)
		return *(ZEVector3*)&Value.Vectors;
	else
		return *(ZEVector3*)&Value.Pointer;
}

ZEVector3& ZENewVariant::GetVector3Ref() const
{
	return ConvertRef<ZEVector3, ZE_TT_VECTOR3>();
}

const ZEVector3& ZENewVariant::GetVector3ConstRef() const
{
	return ConvertConstRef<ZEVector3, ZE_TT_VECTOR3>();
}

const ZEVector4& ZENewVariant::GetVector4() const
{
	if (ValueType.Type != ZE_TT_VECTOR4)
		zeCriticalError("Variant type mismatch. Can not convert non-numerical non-scaler types.");

	if (ValueType.TypeQualifier == ZE_TQ_VALUE)
		return *(ZEVector4*)&Value.Vectors;
	else
		return *(ZEVector4*)&Value.Pointer;
}

ZEVector4& ZENewVariant::GetVector4Ref() const
{
	return ConvertRef<ZEVector4, ZE_TT_VECTOR4>();
}

const ZEVector4& ZENewVariant::GetVector4ConstRef() const
{
	return ConvertConstRef<ZEVector4, ZE_TT_VECTOR4>();
}

const ZEQuaternion& ZENewVariant::GetQuaternion() const
{
	if (ValueType.Type != ZE_TT_QUATERNION)
		zeCriticalError("Variant type mismatch. Can not convert non-numerical non-scaler types.");

	if (ValueType.TypeQualifier == ZE_TQ_VALUE)
		return *(ZEQuaternion*)&Value.Vectors;
	else
		return *(ZEQuaternion*)&Value.Pointer;
}

ZEQuaternion& ZENewVariant::GetQuaternionRef() const
{
	return ConvertRef<ZEQuaternion, ZE_TT_QUATERNION>();
}

const ZEQuaternion& ZENewVariant::GetQuaternionConstRef() const
{
	return ConvertConstRef<ZEQuaternion, ZE_TT_QUATERNION>();
}

const ZEMatrix3x3& ZENewVariant::GetMatrix3x3() const
{
	if (ValueType.Type != ZE_TT_MATRIX3X3)
		zeCriticalError("Variant type mismatch. Can not convert non-numerical non-scaler types.");

	return *(ZEMatrix3x3*)Value.Pointer;
}

ZEMatrix3x3& ZENewVariant::GetMatrix3x3Ref() const
{
	return ConvertRef<ZEMatrix3x3, ZE_TT_MATRIX3X3>();
}

const ZEMatrix3x3& ZENewVariant::GetMatrix3x3ConstRef() const
{
	return ConvertConstRef<ZEMatrix3x3, ZE_TT_MATRIX3X3>();
}

const ZEMatrix4x4& ZENewVariant::GetMatrix4x4() const
{
	if (ValueType.Type != ZE_TT_MATRIX4X4)
		zeCriticalError("Variant type mismatch. Can not convert non-numerical non-scaler types.");

	return *(ZEMatrix4x4*)Value.Pointer;
}

ZEMatrix4x4& ZENewVariant::GetMatrix4x4Ref() const
{
	return ConvertRef<ZEMatrix4x4, ZE_TT_MATRIX4X4>();
}

const ZEMatrix4x4& ZENewVariant::GetMatrix4x4ConstRef() const
{
	return ConvertConstRef<ZEMatrix4x4, ZE_TT_MATRIX4X4>();
}

const ZEString& ZENewVariant::GetString() const
{
	// No Conversion
	if (ValueType.Type != ZE_TT_STRING)
		zeCriticalError("Variant type mismatch. Can not convert non-numerical non-scaler types.");

	if (ValueType.TypeQualifier == ZE_TQ_VALUE)
		return Value.String;
	else
		return *(ZEString*)Value.Pointer;
}

ZEString& ZENewVariant::GetStringRef() const
{
	return ConvertRef<ZEString, ZE_TT_STRING>();
}

const ZEString& ZENewVariant::GetStringConstRef() const
{
	return ConvertConstRef<ZEString, ZE_TT_STRING>();
}

ZEObject* ZENewVariant::GetObjectPtr() const
{
	if (ValueType.Type != ZE_TT_OBJECT_PTR)
		zeCriticalError("Variant type mismatch. Can not convert non-numerical non-scaler types.");

	if (ValueType.TypeQualifier == ZE_TQ_VALUE)
		return (ZEObject*)Value.Pointer;
	else
		return *(ZEObject**)Value.Pointer;
}

ZEObject*& ZENewVariant::GetObjectPtrRef() const
{
	return ConvertRef<ZEObject*, ZE_TT_OBJECT_PTR>();
}

ZEObject*const& ZENewVariant::GetObjectPtrConstRef() const
{
		
	if (ValueType.Type != ZE_TT_OBJECT_PTR)
		zeCriticalError("Variant type mismatch. Can not convert reference type to different reference type.");

	if (ValueType.SubTypeQualifier == ZE_TQ_VALUE)
		zeCriticalError("Variant is a value. Can not convert value to const reference.");

	return (ZEObject* const)Value.Pointer;
}

ZEClass* ZENewVariant::GetClass() const
{
	if (ValueType.Type != ZE_TT_CLASS)
		zeCriticalError("Variant type mismatch. Can not convert non-numerical non-scaler types.");

	if (ValueType.TypeQualifier == ZE_TQ_VALUE)
		return (ZEClass*)Value.Pointer;
	else
		return *(ZEClass**)Value.Pointer;
}

ZEClass*& ZENewVariant::GetClassRef() const
{
	return ConvertRef<ZEClass*, ZE_TT_CLASS>();
}

ZEClass* const& ZENewVariant::GetClassConstRef() const
{
	if (ValueType.Type != ZE_TT_CLASS)
		zeCriticalError("Variant type mismatch. Can not convert reference type to different reference type.");

	if (ValueType.SubTypeQualifier == ZE_TQ_VALUE)
		zeCriticalError("Variant is a value. Can not convert value to const reference.");

	return (ZEClass* const)Value.Pointer;
}

ZENewVariant::ZENewVariant()
{
	Value.Pointer = NULL;
}

ZENewVariant::ZENewVariant(const ZENewVariant& Value)
{
	SetVariant(Value);
}

ZENewVariant::ZENewVariant(const ZEReference& Value)
{
	SetReference(Value);
}

ZENewVariant::ZENewVariant(const char* Value)
{
	SetString(Value);
}

ZENewVariant::ZENewVariant(const ZEString& String)
{
	SetString(String);
}

ZENewVariant::ZENewVariant(ZEInt8 Value)
{
	SetInt32(Value);
}

ZENewVariant::ZENewVariant(ZEInt16 Value)
{
	SetInt16(Value);
}

ZENewVariant::ZENewVariant(ZEInt32 Value)
{
	SetInt32(Value);
}

ZENewVariant::ZENewVariant(ZEInt64 Value)
{
	SetInt64(Value);
}

ZENewVariant::ZENewVariant(ZEUInt8 Value)
{
	SetUInt8(Value);
}

ZENewVariant::ZENewVariant(ZEUInt16 Value)
{
	SetUInt16(Value);
}

ZENewVariant::ZENewVariant(ZEUInt32 Value)
{
	SetUInt32(Value);
}

ZENewVariant::ZENewVariant(ZEUInt64 Value)
{
	SetUInt64(Value);
}

ZENewVariant::ZENewVariant(float Value)
{
	SetFloat(Value);
}

ZENewVariant::ZENewVariant(double Value)
{
	SetDouble(Value);
}

ZENewVariant::ZENewVariant(bool Value)
{
	SetBool(Value);
}

ZENewVariant::ZENewVariant(const ZEVector2& Value)
{
	SetVector2(Value);
}

ZENewVariant::ZENewVariant(const ZEVector3& Value)
{
	SetVector3(Value);
}

ZENewVariant::ZENewVariant(const ZEVector4& Value)
{
	SetVector4(Value);
}

ZENewVariant::ZENewVariant(const ZEQuaternion& Value)
{
	SetQuaternion(Value);
}

ZENewVariant::ZENewVariant(const ZEMatrix3x3& Value)
{
	SetMatrix3x3(Value);
}

ZENewVariant::ZENewVariant(const ZEMatrix4x4& Matrix)
{
	SetMatrix4x4(Matrix);
}

ZENewVariant::~ZENewVariant()
{
	SetType(ZEType());
}

#pragma warning(pop)
