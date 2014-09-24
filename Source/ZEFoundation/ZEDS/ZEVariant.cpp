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
#include "ZEReference.h"
#include "ZEError.h"

#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEMath/ZEMatrix.h"

#include "ZEMeta/ZEObject.h"

#pragma warning(push)
#pragma warning(disable:4482 4996 4244)

template<typename ZEReturnType>
inline ZEReturnType ZEVariant::ConvertIntegerValue() const
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
inline ZEReturnType& ZEVariant::ConvertRef() const
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
inline const ZEReturnType& ZEVariant::ConvertConstRef() const
{
	if (ValueType.Type != Type)
		zeCriticalError("Variant type mismatch. Can not convert reference type to different reference type.");

	if (ValueType.TypeQualifier == ZE_TQ_VALUE)
		zeCriticalError("Variant is a value. Can not convert value to const reference.");

	return *(const ZEReturnType*)Value.Pointer;
}

void ZEVariant::SetType(const ZEType& NewType)
{
	if (ValueType == NewType)
		return;

	if (ValueType.TypeQualifier == ZE_TQ_VALUE && Value.Pointer != NULL)
	{
		if (NewType.ContainerType != ZE_CT_NONE)
			Deleter(Value.Pointer);
		else if (ValueType.Type == ZE_TT_OBJECT)
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

const ZEType& ZEVariant::GetType() const
{
	return ValueType;
}

void ZEVariant::SetUndefined()
{
	ValueType = ZEType();
}

void ZEVariant::SetVariant(const ZEVariant& Variant)
{
	SetType(Variant.ValueType);

	if (ValueType.TypeQualifier == ZE_TQ_VALUE)
	{
		if (ValueType.ContainerType != ZE_CT_NONE || ValueType.Type == ZE_TT_OBJECT)
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

void ZEVariant::SetReference(const ZEReference& Reference)
{
	SetType(Reference.GetType());
	Value.Pointer = Reference.Value.Pointer;
}

void ZEVariant::SetInt8(ZEInt8 Value)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_8;
	SetType(Type);

	this->Value.Int32 = Value;
}

void ZEVariant::SetInt8Ref(ZEInt8& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_8;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	Value.Pointer = &Reference;
}

void ZEVariant::SetInt8ConstRef(const ZEInt8& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_8;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	Value.Pointer = const_cast<ZEInt8*>(&Reference);
}

void ZEVariant::SetInt16(ZEInt16 Value)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_16;
	SetType(Type);

	this->Value.Int32 = Value;
}

void ZEVariant::SetInt16Ref(ZEInt16& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_16;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	Value.Pointer = &Reference;
}

void ZEVariant::SetInt16ConstRef(const ZEInt16& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_16;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	Value.Pointer = const_cast<ZEInt16*>(&Reference);
}

void ZEVariant::SetInt32(ZEInt32 Value)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_32;
	SetType(Type);

	this->Value.Int32 = Value;
}

void ZEVariant::SetInt32Ref(ZEInt32& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_32;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	Value.Pointer = &Reference;
}

void ZEVariant::SetInt32ConstRef(const ZEInt32& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_32;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	Value.Pointer = const_cast<ZEInt32*>(&Reference);
}

void ZEVariant::SetInt64(ZEInt64 Value)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_64;
	SetType(Type);

	this->Value.Int64 = Value;
}

void ZEVariant::SetInt64Ref(ZEInt64& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_64;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	Value.Pointer = &Reference;
}

void ZEVariant::SetInt64ConstRef(const ZEInt64& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_64;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	Value.Pointer = const_cast<ZEInt64*>(&Reference);
}

void ZEVariant::SetUInt8(ZEUInt8 Value)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_8;
	SetType(Type);

	this->Value.Int32 = Value;
}

void ZEVariant::SetUInt8Ref(ZEUInt8& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_8;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	Value.Pointer = &Reference;
}

void ZEVariant::SetUInt8ConstRef(const ZEUInt8& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_8;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	Value.Pointer = const_cast<ZEUInt8*>(&Reference);
}

void ZEVariant::SetUInt16(ZEUInt16 Value)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_16;
	SetType(Type);

	this->Value.Int32 = Value;
}

void ZEVariant::SetUInt16Ref(ZEUInt16& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_16;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	Value.Pointer = &Reference;
}

void ZEVariant::SetUInt16ConstRef(const ZEUInt16& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_16;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	Value.Pointer = const_cast<ZEUInt16*>(&Reference);
}

void ZEVariant::SetUInt32(ZEUInt32 Value)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_32;
	SetType(Type);

	this->Value.Int32 = Value;
}

void ZEVariant::SetUInt32Ref(ZEUInt32& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_32;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	Value.Pointer = &Reference;
}

void ZEVariant::SetUInt32ConstRef(const ZEUInt32& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_32;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	Value.Pointer = const_cast<ZEUInt32*>(&Reference);
}

void ZEVariant::SetUInt64(ZEUInt64 Value)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_64;
	SetType(Type);

	this->Value.Int64 = Value;
}

void ZEVariant::SetUInt64Ref(ZEUInt64& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_64;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	Value.Pointer = &Reference;
}

void ZEVariant::SetUInt64ConstRef(const ZEUInt64& Reference)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_64;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	Value.Pointer = const_cast<ZEUInt64*>(&Reference);
}

void ZEVariant::SetFloat(float Value)
{
	ZEType Type;
	Type.Type = ZE_TT_FLOAT;
	Type.TypeQualifier = ZE_TQ_VALUE;
	SetType(Type);

	this->Value.Float = Value;
}

void ZEVariant::SetFloatRef(float& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_FLOAT;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZEVariant::SetFloatConstRef(const float& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_FLOAT;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<float*>(&Value);
}

void ZEVariant::SetDouble(double Value)
{
	ZEType Type;
	Type.Type = ZE_TT_DOUBLE;
	Type.TypeQualifier = ZE_TQ_VALUE;
	SetType(Type);

	this->Value.Double = Value;
}

void ZEVariant::SetDoubleRef(double& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_DOUBLE;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZEVariant::SetDoubleConstRef(const double& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_DOUBLE;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<double*>(&Value);
}

void ZEVariant::SetBool(bool Value)
{
	ZEType Type;
	Type.Type = ZE_TT_BOOLEAN;
	Type.TypeQualifier = ZE_TQ_VALUE;
	SetType(Type);

	this->Value.Boolean = Value;
}

void ZEVariant::SetBoolRef(bool& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_BOOLEAN;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZEVariant::SetBoolConstRef(const bool& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_BOOLEAN;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<bool*>(&Value);
}

void ZEVariant::SetEnum(ZEInt32 Value)
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_32;
	SetType(Type);

	this->Value.Int32 = Value;
}

void ZEVariant::SetVector2(const ZEVector2& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR2;
	SetType(Type);

	memcpy(this->Value.Vectors, &Value, sizeof(ZEVector2));
}

void ZEVariant::SetVector2Ref(ZEVector2& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR2;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZEVariant::SetVector2ConstRef(const ZEVector2& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR2;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<ZEVector2*>(&Value);
}

void ZEVariant::SetVector3(const ZEVector3& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR3;
	SetType(Type);

	memcpy(this->Value.Vectors, &Value, sizeof(ZEVector3));
}

void ZEVariant::SetVector3Ref(ZEVector3& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR3;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZEVariant::SetVector3ConstRef(const ZEVector3& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR3;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<ZEVector3*>(&Value);
}

void ZEVariant::SetVector4(const ZEVector4& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR4;
	SetType(Type);

	memcpy(this->Value.Vectors, &Value, sizeof(ZEVector4));
}

void ZEVariant::SetVector4Ref(ZEVector4& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR4;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZEVariant::SetVector4ConstRef(const ZEVector4& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR4;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<ZEVector4*>(&Value);
}

void ZEVariant::SetQuaternion(const ZEQuaternion& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_QUATERNION;
	SetType(Type);

	memcpy(this->Value.Vectors, &Value, sizeof(ZEQuaternion));
}

void ZEVariant::SetQuaternionRef(ZEQuaternion& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_QUATERNION;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZEVariant::SetQuaternionConstRef(const ZEQuaternion& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_QUATERNION;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<ZEQuaternion*>(&Value);
}

void ZEVariant::SetMatrix3x3(const ZEMatrix3x3& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_MATRIX3X3;
	SetType(Type);

	this->Value.Pointer = new ZEMatrix3x3(Value);	
}

void ZEVariant::SetMatrix3x3Ref(ZEMatrix3x3& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_MATRIX3X3;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZEVariant::SetMatrix3x3ConstRef(const ZEMatrix3x3& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_MATRIX3X3;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<ZEMatrix3x3*>(&Value);
}

void ZEVariant::SetMatrix4x4(const ZEMatrix4x4& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_MATRIX4X4;
	SetType(Type);

	this->Value.Pointer = new ZEMatrix4x4(Value);	
}

void ZEVariant::SetMatrix4x4Ref(ZEMatrix4x4& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_MATRIX4X4;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZEVariant::SetMatrix4x4ConstRef(const ZEMatrix4x4& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_MATRIX4X4;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<ZEMatrix4x4*>(&Value);
}

void ZEVariant::SetString(const char* Value)
{
	ZEType Type;
	Type.Type = ZE_TT_STRING;
	SetType(Type);

	this->Value.String = Value;
}

void ZEVariant::SetString(const ZEString& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_STRING;
	SetType(Type);

	this->Value.String = Value;
}

void ZEVariant::SetStringRef(ZEString& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_STRING;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZEVariant::SetStringConstRef(const ZEString& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_STRING;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<ZEString*>(&Value);
}

void ZEVariant::SetObjectPtr(ZEObject* Object)
{
	ZEType Type;
	Type.Type = ZE_TT_OBJECT_PTR;
	Type.TypeQualifier = ZE_TQ_VALUE;
	Type.Class = Object->GetClass();
	SetType(Type);

	Value.Pointer = Object;
}

void ZEVariant::SetObjectPtrRef(ZEObject*& Object)
{
	ZEType Type;
	Type.Type = ZE_TT_OBJECT_PTR;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	Type.Class = Object->GetClass();
	SetType(Type);

	Value.Pointer = &Object;
}

void ZEVariant::SetObjectPtrConstRef(const ZEObject*& Object)
{
	ZEType Type;
	Type.Type = ZE_TT_OBJECT_PTR;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	Type.Class = Object->GetClass();
	SetType(Type);

	Value.Pointer = const_cast<ZEObject**>(&Object);
}

void ZEVariant::SetClass(ZEClass* Class)
{
	ZEType Type;
	Type.Type = ZE_TT_CLASS;
	Type.TypeQualifier = ZE_TQ_VALUE;
	SetType(Type);

	Value.Pointer = Class;

}

void ZEVariant::SetClassRef(ZEClass*& Class)
{
	ZEType Type;
	Type.Type = ZE_TT_CLASS;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	Value.Pointer = &Class;
}

void ZEVariant::SetClassConstRef(const ZEClass*& Class)
{
	ZEType Type;
	Type.Type = ZE_TT_CLASS;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	Value.Pointer = const_cast<ZEClass**>(&Class);
}

ZEInt8 ZEVariant::GetInt8() const
{
	return ConvertIntegerValue<ZEInt8>();
}

ZEInt8& ZEVariant::GetInt8Ref() const
{
	return ConvertRef<ZEInt8, ZE_TT_INTEGER_8>();
}

const ZEInt8& ZEVariant::GetInt8ConstRef() const
{
	return ConvertConstRef<ZEInt8, ZE_TT_INTEGER_8>();
}

ZEInt16 ZEVariant::GetInt16() const
{
	return ConvertIntegerValue<ZEInt16>();
}

ZEInt16& ZEVariant::GetInt16Ref() const
{
	return ConvertRef<ZEInt16, ZE_TT_INTEGER_16>();
}

const ZEInt16& ZEVariant::GetInt16ConstRef() const
{
	return ConvertConstRef<ZEInt16, ZE_TT_INTEGER_16>();
}

ZEInt32 ZEVariant::GetInt32() const
{
	return ConvertIntegerValue<ZEInt32>();
}

ZEInt32& ZEVariant::GetInt32Ref() const
{
	return ConvertRef<ZEInt32, ZE_TT_INTEGER_32>();
}

const ZEInt32& ZEVariant::GetInt32ConstRef() const
{
	return ConvertConstRef<ZEInt32, ZE_TT_INTEGER_32>();
}

ZEInt64 ZEVariant::GetInt64() const
{
	return ConvertIntegerValue<ZEInt64>();
}

ZEInt64& ZEVariant::GetInt64Ref() const
{
	return ConvertRef<ZEInt64, ZE_TT_INTEGER_64>();
}

const ZEInt64& ZEVariant::GetInt64ConstRef() const
{
	return ConvertConstRef<ZEInt64, ZE_TT_INTEGER_64>();
}

ZEUInt8 ZEVariant::GetUInt8() const
{
	return ConvertIntegerValue<ZEUInt8>();
}

ZEUInt8& ZEVariant::GetUInt8Ref() const
{
	return ConvertRef<ZEUInt8, ZE_TT_UNSIGNED_INTEGER_8>();
}

const ZEUInt8& ZEVariant::GetUInt8ConstRef() const
{
	return ConvertConstRef<ZEUInt8, ZE_TT_UNSIGNED_INTEGER_8>();
}

ZEUInt16 ZEVariant::GetUInt16() const
{
	return ConvertIntegerValue<ZEUInt16>();
}

ZEUInt16& ZEVariant::GetUInt16Ref() const
{
	return ConvertRef<ZEUInt16, ZE_TT_UNSIGNED_INTEGER_16>();
}

const ZEUInt16& ZEVariant::GetUInt16ConstRef() const
{
	return ConvertConstRef<ZEUInt16, ZE_TT_UNSIGNED_INTEGER_16>();
}

ZEUInt32 ZEVariant::GetUInt32() const
{
	return ConvertIntegerValue<ZEUInt32>();
}

ZEUInt32& ZEVariant::GetUInt32Ref() const
{
	return ConvertRef<ZEUInt32, ZE_TT_UNSIGNED_INTEGER_32>();
}

const ZEUInt32& ZEVariant::GetUInt32ConstRef() const
{
	return ConvertConstRef<ZEUInt32, ZE_TT_UNSIGNED_INTEGER_32>();
}

ZEUInt64 ZEVariant::GetUInt64() const
{
	return ConvertIntegerValue<ZEUInt64>();
}

ZEUInt64& ZEVariant::GetUInt64Ref() const
{
	return ConvertRef<ZEUInt64, ZE_TT_UNSIGNED_INTEGER_64>();
}

const ZEUInt64& ZEVariant::GetUInt64ConstRef() const
{
	return ConvertConstRef<ZEUInt64, ZE_TT_UNSIGNED_INTEGER_64>();
}

float ZEVariant::GetFloat() const
{
	return ConvertIntegerValue<float>();
}

float& ZEVariant::GetFloatRef() const
{
	return ConvertRef<float, ZE_TT_FLOAT>();
}

const float& ZEVariant::GetFloatConstRef() const
{
	return ConvertConstRef<float, ZE_TT_FLOAT>();
}

double ZEVariant::GetDouble() const
{
	return ConvertIntegerValue<double>();
}

double& ZEVariant::GetDoubleRef() const
{
	return ConvertRef<double, ZE_TT_DOUBLE>();
}

const double& ZEVariant::GetDoubleConstRef() const
{
	return ConvertConstRef<double, ZE_TT_DOUBLE>();
}

bool ZEVariant::GetBool() const
{
	return ConvertIntegerValue<bool>();
}

bool& ZEVariant::GetBoolRef() const
{
	return ConvertRef<bool, ZE_TT_BOOLEAN>();
}

const bool& ZEVariant::GetBoolConstRef() const
{
	return ConvertConstRef<bool, ZE_TT_BOOLEAN>();
}

ZEInt32 ZEVariant::GetEnum() const
{
	return ConvertIntegerValue<ZEInt32>();
}

const ZEVector2& ZEVariant::GetVector2() const
{
	if (ValueType.Type != ZE_TT_VECTOR2)
		zeCriticalError("Variant type mismatch. Can not convert non-numerical non-scaler types.");

	if (ValueType.TypeQualifier == ZE_TQ_VALUE)
		return *(ZEVector2*)&Value.Vectors;
	else
		return *(ZEVector2*)&Value.Pointer;
}

ZEVector2& ZEVariant::GetVector2Ref() const
{
	return ConvertRef<ZEVector2, ZE_TT_VECTOR2>();
}

const ZEVector2& ZEVariant::GetVector2ConstRef() const
{
	return ConvertConstRef<ZEVector2, ZE_TT_VECTOR2>();
}

const ZEVector3& ZEVariant::GetVector3() const
{
	if (ValueType.Type != ZE_TT_VECTOR3)
		zeCriticalError("Variant type mismatch. Can not convert non-numerical non-scaler types.");

	if (ValueType.TypeQualifier == ZE_TQ_VALUE)
		return *(ZEVector3*)&Value.Vectors;
	else
		return *(ZEVector3*)&Value.Pointer;
}

ZEVector3& ZEVariant::GetVector3Ref() const
{
	return ConvertRef<ZEVector3, ZE_TT_VECTOR3>();
}

const ZEVector3& ZEVariant::GetVector3ConstRef() const
{
	return ConvertConstRef<ZEVector3, ZE_TT_VECTOR3>();
}

const ZEVector4& ZEVariant::GetVector4() const
{
	if (ValueType.Type != ZE_TT_VECTOR4)
		zeCriticalError("Variant type mismatch. Can not convert non-numerical non-scaler types.");

	if (ValueType.TypeQualifier == ZE_TQ_VALUE)
		return *(ZEVector4*)&Value.Vectors;
	else
		return *(ZEVector4*)&Value.Pointer;
}

ZEVector4& ZEVariant::GetVector4Ref() const
{
	return ConvertRef<ZEVector4, ZE_TT_VECTOR4>();
}

const ZEVector4& ZEVariant::GetVector4ConstRef() const
{
	return ConvertConstRef<ZEVector4, ZE_TT_VECTOR4>();
}

const ZEQuaternion& ZEVariant::GetQuaternion() const
{
	if (ValueType.Type != ZE_TT_QUATERNION)
		zeCriticalError("Variant type mismatch. Can not convert non-numerical non-scaler types.");

	if (ValueType.TypeQualifier == ZE_TQ_VALUE)
		return *(ZEQuaternion*)&Value.Vectors;
	else
		return *(ZEQuaternion*)&Value.Pointer;
}

ZEQuaternion& ZEVariant::GetQuaternionRef() const
{
	return ConvertRef<ZEQuaternion, ZE_TT_QUATERNION>();
}

const ZEQuaternion& ZEVariant::GetQuaternionConstRef() const
{
	return ConvertConstRef<ZEQuaternion, ZE_TT_QUATERNION>();
}

const ZEMatrix3x3& ZEVariant::GetMatrix3x3() const
{
	if (ValueType.Type != ZE_TT_MATRIX3X3)
		zeCriticalError("Variant type mismatch. Can not convert non-numerical non-scaler types.");

	return *(ZEMatrix3x3*)Value.Pointer;
}

ZEMatrix3x3& ZEVariant::GetMatrix3x3Ref() const
{
	return ConvertRef<ZEMatrix3x3, ZE_TT_MATRIX3X3>();
}

const ZEMatrix3x3& ZEVariant::GetMatrix3x3ConstRef() const
{
	return ConvertConstRef<ZEMatrix3x3, ZE_TT_MATRIX3X3>();
}

const ZEMatrix4x4& ZEVariant::GetMatrix4x4() const
{
	if (ValueType.Type != ZE_TT_MATRIX4X4)
		zeCriticalError("Variant type mismatch. Can not convert non-numerical non-scaler types.");

	return *(ZEMatrix4x4*)Value.Pointer;
}

ZEMatrix4x4& ZEVariant::GetMatrix4x4Ref() const
{
	return ConvertRef<ZEMatrix4x4, ZE_TT_MATRIX4X4>();
}

const ZEMatrix4x4& ZEVariant::GetMatrix4x4ConstRef() const
{
	return ConvertConstRef<ZEMatrix4x4, ZE_TT_MATRIX4X4>();
}

const ZEString& ZEVariant::GetString() const
{
	// No Conversion
	if (ValueType.Type != ZE_TT_STRING)
		zeCriticalError("Variant type mismatch. Can not convert non-numerical non-scaler types.");

	if (ValueType.TypeQualifier == ZE_TQ_VALUE)
		return Value.String;
	else
		return *(ZEString*)Value.Pointer;
}

ZEString& ZEVariant::GetStringRef() const
{
	return ConvertRef<ZEString, ZE_TT_STRING>();
}

const ZEString& ZEVariant::GetStringConstRef() const
{
	return ConvertConstRef<ZEString, ZE_TT_STRING>();
}

ZEObject* ZEVariant::GetObjectPtr() const
{
	if (ValueType.Type != ZE_TT_OBJECT_PTR)
		zeCriticalError("Variant type mismatch. Can not convert non-numerical non-scaler types.");

	if (ValueType.TypeQualifier == ZE_TQ_VALUE)
		return (ZEObject*)Value.Pointer;
	else
		return *(ZEObject**)Value.Pointer;
}

ZEObject*& ZEVariant::GetObjectPtrRef() const
{
	return ConvertRef<ZEObject*, ZE_TT_OBJECT_PTR>();
}

ZEObject*const& ZEVariant::GetObjectPtrConstRef() const
{
		
	if (ValueType.Type != ZE_TT_OBJECT_PTR)
		zeCriticalError("Variant type mismatch. Can not convert reference type to different reference type.");

	if (ValueType.TypeQualifier == ZE_TQ_VALUE)
		zeCriticalError("Variant is a value. Can not convert value to const reference.");

	return (ZEObject* const)Value.Pointer;
}

ZEClass* ZEVariant::GetClass() const
{
	if (ValueType.Type != ZE_TT_CLASS)
		zeCriticalError("Variant type mismatch. Can not convert non-numerical non-scaler types.");

	if (ValueType.TypeQualifier == ZE_TQ_VALUE)
		return (ZEClass*)Value.Pointer;
	else
		return *(ZEClass**)Value.Pointer;
}

ZEClass*& ZEVariant::GetClassRef() const
{
	return ConvertRef<ZEClass*, ZE_TT_CLASS>();
}

ZEClass* const& ZEVariant::GetClassConstRef() const
{
	if (ValueType.Type != ZE_TT_CLASS)
		zeCriticalError("Variant type mismatch. Can not convert reference type to different reference type.");

	if (ValueType.TypeQualifier == ZE_TQ_VALUE)
		zeCriticalError("Variant is a value. Can not convert value to const reference.");

	return (ZEClass* const)Value.Pointer;
}

ZEVariant::ZEVariant()
{
	Value.Pointer = NULL;
}

ZEVariant::ZEVariant(const ZEVariant& Value)
{
	SetVariant(Value);
}

ZEVariant::ZEVariant(const ZEReference& Value)
{
	SetReference(Value);
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
	SetInt32(Value);
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
	SetBool(Value);
}

ZEVariant::ZEVariant(const ZEVector2& Value)
{
	SetVector2(Value);
}

ZEVariant::ZEVariant(const ZEVector3& Value)
{
	SetVector3(Value);
}

ZEVariant::ZEVariant(const ZEVector4& Value)
{
	SetVector4(Value);
}

ZEVariant::ZEVariant(const ZEQuaternion& Value)
{
	SetQuaternion(Value);
}

ZEVariant::ZEVariant(const ZEMatrix3x3& Value)
{
	SetMatrix3x3(Value);
}

ZEVariant::ZEVariant(const ZEMatrix4x4& Matrix)
{
	SetMatrix4x4(Matrix);
}

ZEVariant::ZEVariant(ZEClass* Class)
{
	SetClass(Class);
}

ZEVariant::~ZEVariant()
{
	SetType(ZEType());
}

#pragma warning(pop)
