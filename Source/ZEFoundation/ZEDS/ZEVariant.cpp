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
// -------------------------------------------------------- //
//  Copyrights (C) 2005-2007,  Y. Orçun GÖKBULUT			 //
//  All rights reserved.									 //
///////////////////////////////////////////////////////////////

#include "ZEVariant.h"
#include "ZEReference.h"
#include "ZEError.h"

#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEVectord.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEMatrixd.h"

#include "ZEMeta/ZEObject.h"
#include "ZEMeta/ZEMTType.h"
#include "ZEPointer/ZEHolder.h"

#pragma warning(push)
#pragma warning(disable:4800)

void ZEVariant::CheckConversionType(ZEMTBaseType TargetType) const
{
	if (ValueType.IsCollection())
		zeError("Variant type mismatch. Cannot convert between collection types and primitive types.");

	if (ValueType.GetBaseType() != TargetType)
		zeError("Variant type mismatch. Can not convert reference type to a different reference type.");
}

void ZEVariant::CheckConversionMutable() const
{
	if (ValueType.GetBaseQualifier() == ZEMT_TQ_CONST_REFERENCE)
		zeError("Variant type mismatch. Cannot convert const reference type to non-const reference type.");
	else if (ValueType.GetBaseQualifier() == ZEMT_TQ_CONST_VALUE)
		zeError("Variant type mismatch. Cannot convert const value type to non-const reference type.");
}

template<typename ZEReturnType>
inline ZEReturnType ZEVariant::ConvertValuePrimitive() const
{
	if (ValueType.IsCollection())
		zeError("Variant type mismatch. Cannot convert collection type to non-collection type.");

	if (!ValueType.IsReference())
	{
		switch(ValueType.GetBaseType())
		{
			case ZEMT_BT_INTEGER_8:
			case ZEMT_BT_INTEGER_16:
			case ZEMT_BT_INTEGER_32:
			case ZEMT_BT_UNSIGNED_INTEGER_8:
			case ZEMT_BT_UNSIGNED_INTEGER_16:
			case ZEMT_BT_UNSIGNED_INTEGER_32:
				return (ZEReturnType)Value.Int32;
			case ZEMT_BT_INTEGER_64:
			case ZEMT_BT_UNSIGNED_INTEGER_64:
				return (ZEReturnType)Value.Int64;
			case ZEMT_BT_FLOAT:
				return (ZEReturnType)Value.Float;
			case ZEMT_BT_DOUBLE:
				return (ZEReturnType)Value.Double;
			case ZEMT_BT_BOOLEAN:
				return (ZEReturnType)Value.Boolean;

			default:
			case ZEMT_BT_UNDEFINED:
			case ZEMT_BT_VOID:
				zeError("Variant type mismatch. Cannot convert non-primitive value type to primitive value type.");
		}
	}
	else
	{
		switch(ValueType.GetBaseType())
		{
			case ZEMT_BT_INTEGER_8:
				return (ZEReturnType)*(const ZEInt8*)Value.Pointer;
			case ZEMT_BT_INTEGER_16:
				return (ZEReturnType)*(const ZEInt16*)Value.Pointer;
			case ZEMT_BT_INTEGER_32:
				return (ZEReturnType)*(const ZEInt32*)Value.Pointer;
			case ZEMT_BT_INTEGER_64:
				return (ZEReturnType)*(const ZEInt64*)Value.Pointer;
			case ZEMT_BT_UNSIGNED_INTEGER_8:
				return (ZEReturnType)*(const ZEUInt8*)Value.Pointer;
			case ZEMT_BT_UNSIGNED_INTEGER_16:
				return (ZEReturnType)*(const ZEUInt16*)Value.Pointer;
			case ZEMT_BT_UNSIGNED_INTEGER_32:
				return (ZEReturnType)*(const ZEUInt32*)Value.Pointer;
			case ZEMT_BT_UNSIGNED_INTEGER_64:
				return (ZEReturnType)*(const ZEInt64*)Value.Pointer;
			case ZEMT_BT_FLOAT:
				return (ZEReturnType)*(const float*)Value.Pointer;
			case ZEMT_BT_DOUBLE:
				return (ZEReturnType)*(const double*)Value.Pointer;
			case ZEMT_BT_BOOLEAN:
				return (ZEReturnType)*(const bool*)Value.Pointer;
			default:
				zeError("Variant type mismatch. Cannot convert non-primitive reference type to primitive value type.");
		}
	}

	return 0;
}

template <typename ZEReturnType, ZEMTBaseType Type>
inline ZEReturnType& ZEVariant::ConvertValue(const void* ValuePointer) const
{
	CheckConversionType(Type);
	
	if (ValueType.IsReference())
		return *static_cast<ZEReturnType*>(const_cast<void*>(Value.Pointer));
	else
		return *static_cast<ZEReturnType*>(const_cast<void*>(ValuePointer)); 
}

template <typename ZEReturnType, ZEMTBaseType Type>
inline const ZEReturnType& ZEVariant::ConvertConstValue(const void* ValuePointer) const
{
	CheckConversionType(Type);

	if (ValueType.IsReference())
		return *static_cast<const ZEReturnType*>(Value.Pointer);
	else
		return *static_cast<const ZEReturnType*>(ValuePointer);
}

template <typename ZEReturnType, ZEMTBaseType Type>
inline ZEReturnType& ZEVariant::ConvertRef(const void* ValuePointer) const
{
	CheckConversionType(Type);
	CheckConversionMutable();

	if (ValueType.IsReference())
		return *static_cast<ZEReturnType*>(const_cast<void*>(Value.Pointer));
	else 
		return *static_cast<ZEReturnType*>(const_cast<void*>(ValuePointer));
}

template <typename ZEReturnType, ZEMTBaseType Type>
inline const ZEReturnType& ZEVariant::ConvertConstRef(const void* ValuePointer) const
{
	CheckConversionType(Type);

	if (ValueType.IsReference())
		return *static_cast<ZEReturnType*>(Value.Pointer);
	else
		return *static_cast<ZEReturnType*>(const_cast<void*>(ValuePointer));
}

static bool CheckSpecialType(const ZEMTType& Type)
{
	if (Type.IsCollection())
	{
		return !Type.IsReference();
	}
	else
	{
		if (Type.IsReference())
			return false;

		return Type.GetBaseType() == ZEMT_BT_OBJECT || 
			Type.GetBaseType() == ZEMT_BT_MATRIX3X3 ||
			Type.GetBaseType() == ZEMT_BT_MATRIX3X3D ||
			Type.GetBaseType() == ZEMT_BT_MATRIX4X4 ||
			Type.GetBaseType() == ZEMT_BT_MATRIX4X4D;
	}
}

void ZEVariant::SetType(const ZEMTType& NewType)
{
	if (ValueType == NewType)
		return;

	bool SpecialType = CheckSpecialType(NewType);
	if (SpecialType && Value.Pointer != NULL)
		ValueType.DestroyInstance(Value.Pointer);

	ValueType = NewType;

	if (SpecialType)
		Value.Pointer = ValueType.CreateInstance();
}

const ZEMTType& ZEVariant::GetType() const
{
	return ValueType;
}

void ZEVariant::SetUndefined()
{
	ValueType = ZEMTType();
}

void ZEVariant::SetVariant(const ZEVariant& Variant)
{
	SetType(Variant.ValueType);

	if (CheckSpecialType(ValueType))
		ValueType.AssignInstance(Value.Pointer, Variant.Value.Pointer);
	else
		ValueType.AssignInstance(&Value, &Variant.Value);
}

void ZEVariant::SetReference(const ZEReference& Reference)
{
	SetType(Reference.GetType());
	Value.Pointer = Reference.Value.Pointer;
}

void ZEVariant::SetInt8(ZEInt8 Value)
{
	SetType(ZEMTType(ZEMT_BT_INTEGER_8, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Int32 = Value;
}

void ZEVariant::SetInt8Ref(ZEInt8& Reference)
{
	SetType(ZEMTType(ZEMT_BT_INTEGER_8, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = &Reference;
}

void ZEVariant::SetInt8ConstRef(const ZEInt8& Reference)
{
	SetType(ZEMTType(ZEMT_BT_INTEGER_8, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = const_cast<ZEInt8*>(&Reference);
}

void ZEVariant::SetInt16(ZEInt16 Value)
{
	SetType(ZEMTType(ZEMT_BT_INTEGER_16, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Int32 = Value;
}

void ZEVariant::SetInt16Ref(ZEInt16& Reference)
{
	SetType(ZEMTType(ZEMT_BT_INTEGER_16, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = &Reference;
}

void ZEVariant::SetInt16ConstRef(const ZEInt16& Reference)
{
	SetType(ZEMTType(ZEMT_BT_INTEGER_16, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = const_cast<ZEInt16*>(&Reference);
}

void ZEVariant::SetInt32(ZEInt32 Value)
{
	SetType(ZEMTType(ZEMT_BT_INTEGER_32, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Int32 = Value;
}

void ZEVariant::SetInt32Ref(ZEInt32& Reference)
{
	SetType(ZEMTType(ZEMT_BT_INTEGER_32, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = &Reference;
}

void ZEVariant::SetInt32ConstRef(const ZEInt32& Reference)
{
	SetType(ZEMTType(ZEMT_BT_INTEGER_32, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = const_cast<ZEInt32*>(&Reference);
}

void ZEVariant::SetInt64(ZEInt64 Value)
{
	SetType(ZEMTType(ZEMT_BT_INTEGER_64, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Int64 = Value;
}

void ZEVariant::SetInt64Ref(ZEInt64& Reference)
{
	SetType(ZEMTType(ZEMT_BT_INTEGER_64, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = &Reference;
}

void ZEVariant::SetInt64ConstRef(const ZEInt64& Reference)
{
	SetType(ZEMTType(ZEMT_BT_INTEGER_64, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = const_cast<ZEInt64*>(&Reference);
}

void ZEVariant::SetUInt8(ZEUInt8 Value)
{
	SetType(ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_8, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Int32 = Value;
}

void ZEVariant::SetUInt8Ref(ZEUInt8& Reference)
{
	SetType(ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_8, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = &Reference;
}

void ZEVariant::SetUInt8ConstRef(const ZEUInt8& Reference)
{
	SetType(ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_8, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = const_cast<ZEUInt8*>(&Reference);
}

void ZEVariant::SetUInt16(ZEUInt16 Value)
{
	SetType(ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_16, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Int32 = Value;
}

void ZEVariant::SetUInt16Ref(ZEUInt16& Reference)
{
	SetType(ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_16, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = &Reference;
}

void ZEVariant::SetUInt16ConstRef(const ZEUInt16& Reference)
{
	SetType(ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_16, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = const_cast<ZEUInt16*>(&Reference);
}

void ZEVariant::SetUInt32(ZEUInt32 Value)
{
	SetType(ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_32, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Int32 = Value;
}

void ZEVariant::SetUInt32Ref(ZEUInt32& Reference)
{
	SetType(ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_32, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = &Reference;
}

void ZEVariant::SetUInt32ConstRef(const ZEUInt32& Reference)
{
	SetType(ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_32, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = const_cast<ZEUInt32*>(&Reference);
}

void ZEVariant::SetUInt64(ZEUInt64 Value)
{
	SetType(ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_64, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Int64 = Value;
}

void ZEVariant::SetUInt64Ref(ZEUInt64& Reference)
{
	SetType(ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_64, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = &Reference;
}

void ZEVariant::SetUInt64ConstRef(const ZEUInt64& Reference)
{
	SetType(ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_64, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = const_cast<ZEUInt64*>(&Reference);
}

void ZEVariant::SetFloat(float Value)
{
	SetType(ZEMTType(ZEMT_BT_FLOAT, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Float = Value;
}

void ZEVariant::SetFloatRef(float& Reference)
{
	SetType(ZEMTType(ZEMT_BT_FLOAT, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = &Reference;
}

void ZEVariant::SetFloatConstRef(const float& Reference)
{
	SetType(ZEMTType(ZEMT_BT_FLOAT, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = const_cast<float*>(&Reference);
}

void ZEVariant::SetDouble(double Value)
{
	SetType(ZEMTType(ZEMT_BT_DOUBLE, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Double = Value;
}

void ZEVariant::SetDoubleRef(double& Reference)
{
	SetType(ZEMTType(ZEMT_BT_DOUBLE, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = &Reference;
}

void ZEVariant::SetDoubleConstRef(const double& Reference)
{
	SetType(ZEMTType(ZEMT_BT_DOUBLE, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = const_cast<double*>(&Reference);
}

void ZEVariant::SetBool(bool Value)
{
	SetType(ZEMTType(ZEMT_BT_BOOLEAN, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Boolean = Value;
}

void ZEVariant::SetBoolRef(bool& Reference)
{
	SetType(ZEMTType(ZEMT_BT_BOOLEAN, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = &Reference;
}

void ZEVariant::SetBoolConstRef(const bool& Reference)
{
	SetType(ZEMTType(ZEMT_BT_BOOLEAN, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = const_cast<bool*>(&Reference);
}

void ZEVariant::SetVector2(const ZEVector2& Value)
{
	SetType(ZEMTType(ZEMT_BT_VECTOR2, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	memcpy(this->Value.Vectors, &Value, sizeof(ZEVector2));
}

void ZEVariant::SetVector2Ref(ZEVector2& Reference)
{
	SetType(ZEMTType(ZEMT_BT_VECTOR2, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = &Reference;
}

void ZEVariant::SetVector2ConstRef(const ZEVector2& Reference)
{
	SetType(ZEMTType(ZEMT_BT_VECTOR2, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = const_cast<ZEVector2*>(&Reference);
}

void ZEVariant::SetVector2d(const ZEVector2d& Value)
{
	SetType(ZEMTType(ZEMT_BT_VECTOR2D, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	memcpy(this->Value.DoubleVectors, &Value, sizeof(ZEVector2d));
}

void ZEVariant::SetVector2dRef(ZEVector2d& Reference)
{
	SetType(ZEMTType(ZEMT_BT_VECTOR2D, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = &Reference;
}

void ZEVariant::SetVector2dConstRef(const ZEVector2d& Reference)
{
	SetType(ZEMTType(ZEMT_BT_VECTOR2D, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = const_cast<ZEVector2d*>(&Reference);
}

void ZEVariant::SetVector3(const ZEVector3& Value)
{
	SetType(ZEMTType(ZEMT_BT_VECTOR3, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	memcpy(this->Value.Vectors, &Value, sizeof(ZEVector3));
}

void ZEVariant::SetVector3Ref(ZEVector3& Reference)
{
	SetType(ZEMTType(ZEMT_BT_VECTOR3, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = &Reference;
}

void ZEVariant::SetVector3ConstRef(const ZEVector3& Reference)
{
	SetType(ZEMTType(ZEMT_BT_VECTOR3, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = const_cast<ZEVector3*>(&Reference);
}

void ZEVariant::SetVector3d(const ZEVector3d& Value)
{
	SetType(ZEMTType(ZEMT_BT_VECTOR3D, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	memcpy(this->Value.DoubleVectors, &Value, sizeof(ZEVector3d));
}

void ZEVariant::SetVector3dRef(ZEVector3d& Reference)
{
	SetType(ZEMTType(ZEMT_BT_VECTOR3D, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = &Reference;
}

void ZEVariant::SetVector3dConstRef(const ZEVector3d& Reference)
{
	SetType(ZEMTType(ZEMT_BT_VECTOR3D, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = const_cast<ZEVector3d*>(&Reference);
}

void ZEVariant::SetVector4(const ZEVector4& Value)
{
	SetType(ZEMTType(ZEMT_BT_VECTOR4, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	memcpy(this->Value.Vectors, &Value, sizeof(ZEVector4));
}

void ZEVariant::SetVector4Ref(ZEVector4& Reference)
{
	SetType(ZEMTType(ZEMT_BT_VECTOR4, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = &Reference;
}

void ZEVariant::SetVector4ConstRef(const ZEVector4& Reference)
{
	SetType(ZEMTType(ZEMT_BT_VECTOR4, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = const_cast<ZEVector4*>(&Reference);
}

void ZEVariant::SetVector4d(const ZEVector4d& Value)
{
	SetType(ZEMTType(ZEMT_BT_VECTOR4D, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	memcpy(this->Value.DoubleVectors, &Value, sizeof(ZEVector4d));
}

void ZEVariant::SetVector4dRef(ZEVector4d& Reference)
{
	SetType(ZEMTType(ZEMT_BT_VECTOR4D, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = &Reference;
}

void ZEVariant::SetVector4dConstRef(const ZEVector4d& Reference)
{
	SetType(ZEMTType(ZEMT_BT_VECTOR4D, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = const_cast<ZEVector4d*>(&Reference);
}

void ZEVariant::SetQuaternion(const ZEQuaternion& Value)
{
	SetType(ZEMTType(ZEMT_BT_QUATERNION, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	memcpy(this->Value.Vectors, &Value, sizeof(ZEQuaternion));
}

void ZEVariant::SetQuaternionRef(ZEQuaternion& Reference)
{
	SetType(ZEMTType(ZEMT_BT_QUATERNION, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = &Reference;
}

void ZEVariant::SetQuaternionConstRef(const ZEQuaternion& Reference)
{
	SetType(ZEMTType(ZEMT_BT_QUATERNION, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = const_cast<ZEQuaternion*>(&Reference);
}

void ZEVariant::SetMatrix3x3(const ZEMatrix3x3& Value)
{
	SetType(ZEMTType(ZEMT_BT_MATRIX3X3, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	*(ZEMatrix3x3*)this->Value.Pointer = Value;
}

void ZEVariant::SetMatrix3x3Ref(ZEMatrix3x3& Reference)
{
	SetType(ZEMTType(ZEMT_BT_MATRIX3X3, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = &Reference;
}

void ZEVariant::SetMatrix3x3ConstRef(const ZEMatrix3x3& Reference)
{
	SetType(ZEMTType(ZEMT_BT_MATRIX3X3, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = const_cast<ZEMatrix3x3*>(&Reference);
}

void ZEVariant::SetMatrix3x3d(const ZEMatrix3x3d& Value)
{
	SetType(ZEMTType(ZEMT_BT_MATRIX3X3D, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	*(ZEMatrix3x3d*)this->Value.Pointer = Value;
}

void ZEVariant::SetMatrix3x3dRef(ZEMatrix3x3d& Reference)
{
	SetType(ZEMTType(ZEMT_BT_MATRIX3X3D, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = &Reference;
}

void ZEVariant::SetMatrix3x3dConstRef(const ZEMatrix3x3d& Reference)
{
	SetType(ZEMTType(ZEMT_BT_MATRIX3X3D, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = const_cast<ZEMatrix3x3d*>(&Reference);
}

void ZEVariant::SetMatrix4x4(const ZEMatrix4x4& Value)
{
	SetType(ZEMTType(ZEMT_BT_MATRIX4X4, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	*(ZEMatrix4x4*)this->Value.Pointer = Value;
}

void ZEVariant::SetMatrix4x4Ref(ZEMatrix4x4& Reference)
{
	SetType(ZEMTType(ZEMT_BT_MATRIX4X4, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = &Reference;
}

void ZEVariant::SetMatrix4x4ConstRef(const ZEMatrix4x4& Reference)
{
	SetType(ZEMTType(ZEMT_BT_MATRIX4X4, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = const_cast<ZEMatrix4x4*>(&Reference);
}

void ZEVariant::SetMatrix4x4d(const ZEMatrix4x4d& Value)
{
	SetType(ZEMTType(ZEMT_BT_MATRIX4X4D, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	*(ZEMatrix4x4d*)this->Value.Pointer = Value;
}

void ZEVariant::SetMatrix4x4dRef(ZEMatrix4x4d& Reference)
{
	SetType(ZEMTType(ZEMT_BT_MATRIX4X4D, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = &Reference;
}

void ZEVariant::SetMatrix4x4dConstRef(const ZEMatrix4x4d& Reference)
{
	SetType(ZEMTType(ZEMT_BT_MATRIX4X4D, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = const_cast<ZEMatrix4x4d*>(&Reference);
}

void ZEVariant::SetString(const ZEString& Value)
{
	SetType(ZEMTType(ZEMT_BT_STRING, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.String = Value;
}

void ZEVariant::SetStringRef(ZEString& Reference)
{
	SetType(ZEMTType(ZEMT_BT_STRING, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = &Reference;
}

void ZEVariant::SetStringConstRef(const ZEString& Reference)
{
	SetType(ZEMTType(ZEMT_BT_STRING, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = const_cast<ZEString*>(&Reference);
}

void ZEVariant::SetObject(ZEObject& Object)
{
	SetType(ZEMTType(ZEMT_BT_OBJECT, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, Object.GetClass()));
	Value.Pointer = Object.GetClass()->Clone(&Object);
}

void ZEVariant::SetObjectRef(ZEObject& Reference)
{
	SetType(ZEMTType(ZEMT_BT_OBJECT, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, Reference.GetClass()));
	Value.Pointer = &Reference;
}

void ZEVariant::SetObjectConstRef(const ZEObject& Reference)
{
	SetType(ZEMTType(ZEMT_BT_OBJECT, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, Reference.GetClass()));
	Value.Pointer = const_cast<ZEObject*>(&Reference);
}

void ZEVariant::SetObjectPtr(ZEObject* Object)
{
	SetType(ZEMTType(ZEMT_BT_OBJECT_PTR, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, Object == NULL ? ZEObject::Class() : Object->GetClass()));
	Value.Pointer = Object;
}

void ZEVariant::SetObjectPtrConst(const ZEObject* Object)
{
	SetType(ZEMTType(ZEMT_BT_OBJECT_PTR, ZEMT_TQ_CONST_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, Object == NULL ? ZEObject::Class() : Object->GetClass()));
	Value.Pointer = const_cast<ZEObject*>(Object);
}

void ZEVariant::SetObjectPtrRef(ZEObject*& Reference)
{
	SetType(ZEMTType(ZEMT_BT_OBJECT_PTR, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, Reference == NULL ? ZEObject::Class() : Reference->GetClass()));
	Value.Pointer = &Reference;
}

void ZEVariant::SetObjectPtrConstRef(const ZEObject*& Reference)
{
	SetType(ZEMTType(ZEMT_BT_OBJECT_PTR, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, Reference == NULL ? ZEObject::Class() : Reference->GetClass()));
	Value.Pointer = const_cast<ZEObject**>(&Reference);
}

void ZEVariant::SetObjectHolder(ZEHolderBase& Object)
{
	SetType(ZEMTType(ZEMT_BT_OBJECT_HOLDER, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, Object.GetObjectPtrConst() == NULL ? ZEObject::Class() : Object.GetObjectPtrConst()->GetClass()));
	Value.Holder = Object.GetObjectPtr();
}

void ZEVariant::SetObjectHolderRef(ZEHolderBase& Reference)
{
	SetType(ZEMTType(ZEMT_BT_OBJECT_HOLDER, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, Reference.GetObjectPtr() == NULL ? ZEObject::Class() : Reference.GetObjectPtr()->GetClass()));
	Value.Pointer = const_cast<ZEHolderBase*>(&Reference);
}

void ZEVariant::SetClass(ZEClass* Class)
{
	SetType(ZEMTType(ZEMT_BT_CLASS, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = Class;
}

void ZEVariant::SetClassRef(ZEClass*& Reference)
{
	SetType(ZEMTType(ZEMT_BT_CLASS, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = &Reference;
}

void ZEVariant::SetCollection(const ZEMTCollection& Collection)
{
	ZEMTType CollectionType = Collection.GetType();
	SetType(CollectionType);
	CollectionType.AssignInstance(Value.Pointer, &Collection);
}

void ZEVariant::SetCollectionRef(ZEMTCollection& Reference)
{
	ZEMTType CollectionType = Reference.GetType();
	CollectionType.SetCollectionQualifier(ZEMT_TQ_REFERENCE);
	SetType(CollectionType);
	Value.Pointer = &Reference;
}

void ZEVariant::SetCollectionConstRef(const ZEMTCollection& Reference)
{
	ZEMTType CollectionType = Reference.GetType();
	CollectionType.SetCollectionQualifier(ZEMT_TQ_CONST_REFERENCE);
	SetType(CollectionType);
	Value.Pointer = const_cast<ZEMTCollection*>(&Reference);
}

void ZEVariant::SetValue(const ZEValue& Value)
{
	switch (Value.GetType())
	{
		case ZE_VRT_UNDEFINED:
		case ZE_VRT_NULL:
		case ZE_VRT_CLASS:
			SetUndefined();
			break;

		case ZE_VRT_INTEGER_8:
			return SetInt8(Value.GetInt8());
			break;

		case ZE_VRT_INTEGER_16:
			SetInt16(Value.GetInt16());
			break;

		case ZE_VRT_INTEGER_32:
			SetInt32(Value.GetInt32());
			break;

		case ZE_VRT_INTEGER_64:
			SetInt64(Value.GetInt64());
			break;

		case ZE_VRT_UNSIGNED_INTEGER_8:
			SetInt8(Value.GetUInt8());
			break;

		case ZE_VRT_UNSIGNED_INTEGER_16:
			SetInt16(Value.GetUInt16());
			break;

		case ZE_VRT_UNSIGNED_INTEGER_32:
			SetInt32(Value.GetUInt32());
			break;

		case ZE_VRT_UNSIGNED_INTEGER_64:
			SetInt64(Value.GetUInt64());
			break;

		case ZE_VRT_FLOAT:
			SetFloat(Value.GetFloat());
			break;

		case ZE_VRT_DOUBLE:
			SetDouble(Value.GetDouble());
			break;

		case ZE_VRT_BOOLEAN:
			SetBool(Value.GetBoolean());
			break;

		case ZE_VRT_STRING:
			SetString(Value.GetString());
			break;

		case ZE_VRT_QUATERNION:
			SetQuaternion(Value.GetQuaternion());
			break;

		case ZE_VRT_VECTOR2:
			SetVector2(Value.GetVector2());
			break;

		case ZE_VRT_VECTOR2D:
			SetVector2d(Value.GetVector2d());
			break;

		case ZE_VRT_VECTOR3:
			SetVector3(Value.GetVector3());
			break;

		case ZE_VRT_VECTOR3D:
			SetVector3d(Value.GetVector3d());
			break;

		case ZE_VRT_VECTOR4:
			SetVector4(Value.GetVector4());
			break;

		case ZE_VRT_VECTOR4D:
			SetVector4d(Value.GetVector4d());
			break;

		case ZE_VRT_MATRIX3X3:
			SetMatrix3x3(Value.GetMatrix3x3());
			break;

		case ZE_VRT_MATRIX3X3D:
			SetMatrix3x3d(Value.GetMatrix3x3d());
			break;

		case ZE_VRT_MATRIX4X4:
			SetMatrix4x4(Value.GetMatrix4x4());
			break;

		case ZE_VRT_MATRIX4X4D:
			SetMatrix4x4d(Value.GetMatrix4x4d());
			break;
	}
}

bool ZEVariant::IsUndefined() const
{
	return (GetType().GetBaseType() == ZEMT_BT_UNDEFINED);
}

ZEInt8 ZEVariant::GetInt8() const
{
	return ConvertValuePrimitive<ZEInt8>();
}

ZEInt8& ZEVariant::GetInt8Ref() const
{
	return ConvertRef<ZEInt8, ZEMT_BT_INTEGER_8>(&Value.Int32);
}

const ZEInt8& ZEVariant::GetInt8ConstRef() const
{
	return ConvertConstRef<const ZEInt8, ZEMT_BT_INTEGER_8>(&Value.Int32);
}

ZEInt16 ZEVariant::GetInt16() const
{
	return ConvertValuePrimitive<ZEInt16>();
}

ZEInt16& ZEVariant::GetInt16Ref() const
{
	return ConvertRef<ZEInt16, ZEMT_BT_INTEGER_16>(&Value.Int32);
}

const ZEInt16& ZEVariant::GetInt16ConstRef() const
{
	return ConvertConstRef<const ZEInt16, ZEMT_BT_INTEGER_16>(&Value.Int32);
}

ZEInt32 ZEVariant::GetInt32() const
{
	return ConvertValuePrimitive<ZEInt32>();
}

ZEInt32& ZEVariant::GetInt32Ref() const
{
	return ConvertRef<ZEInt32, ZEMT_BT_INTEGER_32>(&Value.Int32);
}

const ZEInt32& ZEVariant::GetInt32ConstRef() const
{
	return ConvertConstRef<const ZEInt32, ZEMT_BT_INTEGER_32>(&Value.Int32);
}

ZEInt64 ZEVariant::GetInt64() const
{
	return ConvertValuePrimitive<ZEInt64>();
}

ZEInt64& ZEVariant::GetInt64Ref() const
{
	return ConvertRef<ZEInt64, ZEMT_BT_INTEGER_64>(&Value.Int64);
}

const ZEInt64& ZEVariant::GetInt64ConstRef() const
{
	return ConvertConstRef<const ZEInt64, ZEMT_BT_INTEGER_64>(&Value.Int64);
}

ZEUInt8 ZEVariant::GetUInt8() const
{
	return ConvertValuePrimitive<ZEUInt8>();
}

ZEUInt8& ZEVariant::GetUInt8Ref() const
{
	return ConvertRef<ZEUInt8, ZEMT_BT_UNSIGNED_INTEGER_8>(&Value.Int32);
}

const ZEUInt8& ZEVariant::GetUInt8ConstRef() const
{
	return ConvertConstRef<const ZEUInt8, ZEMT_BT_UNSIGNED_INTEGER_8>(&Value.Int32);
}

ZEUInt16 ZEVariant::GetUInt16() const
{
	return ConvertValuePrimitive<ZEUInt16>();
}

ZEUInt16& ZEVariant::GetUInt16Ref() const
{
	return ConvertRef<ZEUInt16, ZEMT_BT_UNSIGNED_INTEGER_16>(&Value.Int32);
}

const ZEUInt16& ZEVariant::GetUInt16ConstRef() const
{
	return ConvertConstRef<const ZEUInt16, ZEMT_BT_UNSIGNED_INTEGER_16>(&Value.Int32);
}

ZEUInt32 ZEVariant::GetUInt32() const
{
	return ConvertValuePrimitive<ZEUInt32>();
}

ZEUInt32& ZEVariant::GetUInt32Ref() const
{
	return ConvertRef<ZEUInt32, ZEMT_BT_UNSIGNED_INTEGER_32>(&Value.Int32);
}

const ZEUInt32& ZEVariant::GetUInt32ConstRef() const
{
	return ConvertConstRef<const ZEUInt32, ZEMT_BT_UNSIGNED_INTEGER_32>(&Value.Int32);
}

ZEUInt64 ZEVariant::GetUInt64() const
{
	return ConvertValuePrimitive<ZEUInt64>();
}

ZEUInt64& ZEVariant::GetUInt64Ref() const
{
	return ConvertRef<ZEUInt64, ZEMT_BT_UNSIGNED_INTEGER_64>(&Value.Int64);
}

const ZEUInt64& ZEVariant::GetUInt64ConstRef() const
{
	return ConvertConstRef<const ZEUInt64, ZEMT_BT_UNSIGNED_INTEGER_64>(&Value.Int64);
}

float ZEVariant::GetFloat() const
{
	return ConvertValuePrimitive<float>();
}

float& ZEVariant::GetFloatRef() const
{
	return ConvertRef<float, ZEMT_BT_FLOAT>(&Value.Float);
}

const float& ZEVariant::GetFloatConstRef() const
{
	return ConvertConstRef<const float, ZEMT_BT_FLOAT>(&Value.Float);
}

double ZEVariant::GetDouble() const
{
	return ConvertValuePrimitive<double>();
}

double& ZEVariant::GetDoubleRef() const
{
	return ConvertRef<double, ZEMT_BT_DOUBLE>(&Value.Double);
}

const double& ZEVariant::GetDoubleConstRef() const
{
	return ConvertConstRef<const double, ZEMT_BT_DOUBLE>(&Value.Double);
}

bool ZEVariant::GetBool() const
{
	return ConvertValuePrimitive<bool>();
}

bool& ZEVariant::GetBoolRef() const
{
	return ConvertRef<bool, ZEMT_BT_BOOLEAN>(&Value.Boolean);
}

const bool& ZEVariant::GetBoolConstRef() const
{
	return ConvertConstRef<const bool, ZEMT_BT_BOOLEAN>(&Value.Boolean);
}

ZEInt32 ZEVariant::GetEnum() const
{
	return ConvertValuePrimitive<ZEInt32>();
}

const ZEVector2& ZEVariant::GetVector2() const
{
	return ConvertValue<ZEVector2, ZEMT_BT_VECTOR2>(&Value.Vectors);
}

ZEVector2& ZEVariant::GetVector2Ref() const
{
	return ConvertRef<ZEVector2, ZEMT_BT_VECTOR2>(&Value.Vectors);
}

const ZEVector2& ZEVariant::GetVector2ConstRef() const
{
	return ConvertConstRef<const ZEVector2, ZEMT_BT_VECTOR2>(&Value.Vectors);
}

const ZEVector2d& ZEVariant::GetVector2d() const
{
	return ConvertValue<ZEVector2d, ZEMT_BT_VECTOR2D>(&Value.DoubleVectors);
}

ZEVector2d& ZEVariant::GetVector2dRef() const
{
	return ConvertRef<ZEVector2d, ZEMT_BT_VECTOR2D>(&Value.DoubleVectors);
}

const ZEVector2d& ZEVariant::GetVector2dConstRef() const
{
	return ConvertConstRef<const ZEVector2d, ZEMT_BT_VECTOR2D>(&Value.DoubleVectors);
}

const ZEVector3& ZEVariant::GetVector3() const
{
	return ConvertValue<ZEVector3, ZEMT_BT_VECTOR3>(&Value.Vectors);
}

ZEVector3& ZEVariant::GetVector3Ref() const
{
	return ConvertRef<ZEVector3, ZEMT_BT_VECTOR3>(&Value.Vectors);
}

const ZEVector3& ZEVariant::GetVector3ConstRef() const
{
	return ConvertConstRef<const ZEVector3, ZEMT_BT_VECTOR3>(&Value.Vectors);
}

const ZEVector3d& ZEVariant::GetVector3d() const
{
	return ConvertValue<ZEVector3d, ZEMT_BT_VECTOR3D>(&Value.DoubleVectors);
}

ZEVector3d& ZEVariant::GetVector3dRef() const
{
	return ConvertRef<ZEVector3d, ZEMT_BT_VECTOR3D>(&Value.DoubleVectors);
}

const ZEVector3d& ZEVariant::GetVector3dConstRef() const
{
	return ConvertConstRef<const ZEVector3d, ZEMT_BT_VECTOR3D>(&Value.DoubleVectors);
}

const ZEVector4& ZEVariant::GetVector4() const
{
	return ConvertValue<ZEVector4, ZEMT_BT_VECTOR4>(&Value.Vectors);
}

ZEVector4& ZEVariant::GetVector4Ref() const
{
	return ConvertRef<ZEVector4, ZEMT_BT_VECTOR4>(&Value.Vectors);
}

const ZEVector4& ZEVariant::GetVector4ConstRef() const
{
	return ConvertConstRef<const ZEVector4, ZEMT_BT_VECTOR4>(&Value.Vectors);
}

const ZEVector4d& ZEVariant::GetVector4d() const
{
	return ConvertValue<ZEVector4d, ZEMT_BT_VECTOR4D>(&Value.DoubleVectors);
}

ZEVector4d& ZEVariant::GetVector4dRef() const
{
	return ConvertRef<ZEVector4d, ZEMT_BT_VECTOR4D>(&Value.DoubleVectors);
}

const ZEVector4d& ZEVariant::GetVector4dConstRef() const
{
	return ConvertConstRef<const ZEVector4d, ZEMT_BT_VECTOR4D>(&Value.DoubleVectors);
}

const ZEQuaternion& ZEVariant::GetQuaternion() const
{
	return ConvertValue<ZEQuaternion, ZEMT_BT_QUATERNION>(&Value.Vectors);
}

ZEQuaternion& ZEVariant::GetQuaternionRef() const
{
	return ConvertRef<ZEQuaternion, ZEMT_BT_QUATERNION>(&Value.Vectors);
}

const ZEQuaternion& ZEVariant::GetQuaternionConstRef() const
{
	return ConvertConstRef<const ZEQuaternion, ZEMT_BT_QUATERNION>(&Value.Vectors);
}

const ZEMatrix3x3& ZEVariant::GetMatrix3x3() const
{
	return ConvertValue<ZEMatrix3x3, ZEMT_BT_MATRIX3X3>(Value.Pointer);
}

ZEMatrix3x3& ZEVariant::GetMatrix3x3Ref() const
{
	return ConvertRef<ZEMatrix3x3, ZEMT_BT_MATRIX3X3>(Value.Pointer);
}

const ZEMatrix3x3& ZEVariant::GetMatrix3x3ConstRef() const
{
	return ConvertConstRef<const ZEMatrix3x3, ZEMT_BT_MATRIX3X3>(Value.Pointer);
}

const ZEMatrix3x3d& ZEVariant::GetMatrix3x3d() const
{
	return ConvertValue<ZEMatrix3x3d, ZEMT_BT_MATRIX3X3D>(Value.Pointer);
}

ZEMatrix3x3d& ZEVariant::GetMatrix3x3dRef() const
{
	return ConvertRef<ZEMatrix3x3d, ZEMT_BT_MATRIX3X3D>(Value.Pointer);
}

const ZEMatrix3x3d& ZEVariant::GetMatrix3x3dConstRef() const
{
	return ConvertConstRef<const ZEMatrix3x3d, ZEMT_BT_MATRIX3X3D>(Value.Pointer);
}

const ZEMatrix4x4& ZEVariant::GetMatrix4x4() const
{
	return ConvertValue<ZEMatrix4x4, ZEMT_BT_MATRIX4X4>(Value.Pointer);
}

ZEMatrix4x4& ZEVariant::GetMatrix4x4Ref() const
{
	return ConvertRef<ZEMatrix4x4, ZEMT_BT_MATRIX4X4>(Value.Pointer);
}

const ZEMatrix4x4& ZEVariant::GetMatrix4x4ConstRef() const
{
	return ConvertConstRef<const ZEMatrix4x4, ZEMT_BT_MATRIX4X4>(Value.Pointer);
}

const ZEMatrix4x4d& ZEVariant::GetMatrix4x4d() const
{
	return ConvertValue<ZEMatrix4x4d, ZEMT_BT_MATRIX4X4D>(Value.Pointer);
}

ZEMatrix4x4d& ZEVariant::GetMatrix4x4dRef() const
{
	return ConvertRef<ZEMatrix4x4d, ZEMT_BT_MATRIX4X4D>(Value.Pointer);
}

const ZEMatrix4x4d& ZEVariant::GetMatrix4x4dConstRef() const
{
	return ConvertConstRef<const ZEMatrix4x4d, ZEMT_BT_MATRIX4X4D>(Value.Pointer);
}

ZEString ZEVariant::GetString() const
{
	return ConvertValue<ZEString, ZEMT_BT_STRING>(&Value.String);
}

ZEString& ZEVariant::GetStringRef() const
{
	return ConvertRef<ZEString, ZEMT_BT_STRING>(&Value.String);
}

const ZEString& ZEVariant::GetStringConstRef() const
{
	return ConvertConstRef<const ZEString, ZEMT_BT_STRING>(&Value.String);
}

const ZEObject& ZEVariant::GetObject() const
{
	CheckConversionType(ZEMT_BT_OBJECT);
	return *static_cast<ZEObject*>(const_cast<void*>(Value.Pointer));
}

ZEObject& ZEVariant::GetObjectRef() const
{
	return ConvertRef<ZEObject, ZEMT_BT_OBJECT>(Value.Pointer);
}

const ZEObject& ZEVariant::GetObjectConstRef() const
{
	return ConvertConstRef<ZEObject, ZEMT_BT_OBJECT>(Value.Pointer);
}

ZEObject* ZEVariant::GetObjectPtr() const
{
	return ConvertValue<ZEObject*, ZEMT_BT_OBJECT_PTR>(&Value.Pointer);
}

const ZEObject* ZEVariant::GetObjectPtrConst() const
{
	return ConvertConstValue<ZEObject*, ZEMT_BT_OBJECT_PTR>(&Value.Pointer);
}

ZEObject*& ZEVariant::GetObjectPtrRef() const
{
	return ConvertRef<ZEObject*, ZEMT_BT_OBJECT_PTR>(&Value.Pointer);
}

const ZEObject*& ZEVariant::GetObjectPtrConstRef() const
{
	CheckConversionType(ZEMT_BT_OBJECT_PTR);
	if (ValueType.IsReference())
		return *(const ZEObject**)(Value.Pointer);
	else
		return *(const ZEObject**)(&Value.Pointer);
}

ZEHolderBase& ZEVariant::GetObjectHolder() const
{
	return ConvertValue<ZEHolderBase, ZEMT_BT_OBJECT_HOLDER>(&Value.Holder);
}

ZEHolderBase& ZEVariant::GetObjectHolderRef() const
{
	return ConvertRef<ZEHolderBase, ZEMT_BT_OBJECT_HOLDER>(&Value.Holder);
}

ZEClass* ZEVariant::GetClass() const
{
	return ConvertValue<ZEClass*, ZEMT_BT_CLASS>(&Value.Pointer);
}

ZEClass*& ZEVariant::GetClassRef() const
{
	return ConvertRef<ZEClass*, ZEMT_BT_CLASS>(&Value.Pointer);
}

const ZEMTCollection& ZEVariant::GetCollection() const
{
	if (!ValueType.IsCollection())
		zeError("Variant type mismatch. Cannot convert between collection types and primitive types.");

	if (!ValueType.IsReference())
		return *static_cast<const ZEMTCollection*>(Value.Pointer);
	else
		return **static_cast<const ZEMTCollection**>(Value.Pointer);
}

ZEMTCollection& ZEVariant::GetCollectionRef() const
{
	if (!ValueType.IsCollection())
		zeError("Variant type mismatch. Cannot convert between collection types and primitive types.");

	if (ValueType.GetCollectionQualifier() == ZEMT_TQ_CONST_REFERENCE)
		zeError("Variant type mismatch. Cannot convert const reference type to non-const reference type.");
	else if (ValueType.GetCollectionQualifier() == ZEMT_TQ_CONST_VALUE)
		zeError("Variant type mismatch. Cannot convert const value type to non-const reference type.");

	if (!ValueType.IsReference())
		return *static_cast<ZEMTCollection*>(Value.Pointer);
	else
		return **static_cast<ZEMTCollection** const>(Value.Pointer);
}

const ZEMTCollection& ZEVariant::GetCollectionConstRef() const
{
	if (!ValueType.IsCollection())
		zeError("Variant type mismatch. Cannot convert between collection types and primitive types.");

	if (!ValueType.IsReference())
		return *static_cast<ZEMTCollection*>(Value.Pointer);
	else
		return **static_cast<ZEMTCollection** const>(Value.Pointer);
}

ZEValue ZEVariant::GetValue()
{
	if (ValueType.IsCollection())
		return ZEValue();

	switch (ValueType.GetBaseType())
	{
		default:
			zeError("Unknown variant type.");
		
		case ZEMT_BT_UNDEFINED:
		case ZEMT_BT_VOID:
		case ZEMT_BT_CLASS:
		case ZEMT_BT_OBJECT:	
		case ZEMT_BT_OBJECT_PTR:
		case ZEMT_BT_OBJECT_HOLDER:
			return ZEValue();

		case ZEMT_BT_INTEGER_8:
			return ZEValue(GetInt8());

		case ZEMT_BT_INTEGER_16:
			return ZEValue(GetInt16());

		case ZEMT_BT_INTEGER_32:
			return ZEValue(GetInt32());

		case ZEMT_BT_INTEGER_64:
			return ZEValue(GetInt64());

		case ZEMT_BT_UNSIGNED_INTEGER_8:
			return ZEValue(GetUInt8());

		case ZEMT_BT_UNSIGNED_INTEGER_16:
			return ZEValue(GetUInt16());

		case ZEMT_BT_UNSIGNED_INTEGER_32:
			return ZEValue(GetUInt32());

		case ZEMT_BT_UNSIGNED_INTEGER_64:
			return ZEValue(GetUInt64());

		case ZEMT_BT_FLOAT:
			return ZEValue(GetFloat());

		case ZEMT_BT_DOUBLE:
			return ZEValue(GetDouble());

		case ZEMT_BT_BOOLEAN:
			return ZEValue(GetBool());

		case ZEMT_BT_STRING:
			return ZEValue(GetString());

		case ZEMT_BT_QUATERNION:
			return ZEValue(GetQuaternion());

		case ZEMT_BT_VECTOR2:
			return ZEValue(GetVector2());

		case ZEMT_BT_VECTOR2D:
			return ZEValue(GetVector2d());

		case ZEMT_BT_VECTOR3:
			return ZEValue(GetVector3());

		case ZEMT_BT_VECTOR3D:
			return ZEValue(GetVector3d());

		case ZEMT_BT_VECTOR4:
			return ZEValue(GetVector4());

		case ZEMT_BT_VECTOR4D:
			return ZEValue(GetVector4d());

		case ZEMT_BT_MATRIX3X3:
			return ZEValue(GetMatrix3x3());

		case ZEMT_BT_MATRIX3X3D:
			return ZEValue(GetMatrix3x3d());

		case ZEMT_BT_MATRIX4X4:
			return ZEValue(GetMatrix4x4());

		case ZEMT_BT_MATRIX4X4D:
			return ZEValue(GetMatrix4x4d());

		case ZEMT_BT_ENUMERATOR:
			return ZEValue(GetInt32());

		case ZEMT_BT_FLAGS:
			return ZEValue(GetInt32());
	}
}

ZEVariant::ZEVariant()
{
	Value.Pointer = NULL;
}

ZEVariant::ZEVariant(const ZEVariant& Value)
{
	new (this) ZEVariant();
	SetVariant(Value);
}

ZEVariant::ZEVariant(const ZEValue& Value)
{
	new (this) ZEVariant();
	SetValue(Value);
}

ZEVariant::ZEVariant(const ZEReference& Value)
{
	new (this) ZEVariant();
	SetReference(Value);
}

ZEVariant::ZEVariant(const char* Value)
{
	new (this) ZEVariant();
	SetString(Value);
}

ZEVariant::ZEVariant(const ZEString& String)
{
	new (this) ZEVariant();
	SetString(String);
}

ZEVariant::ZEVariant(ZEInt8 Value)
{
	new (this) ZEVariant();
	SetInt32(Value);
}

ZEVariant::ZEVariant(ZEInt16 Value)
{
	new (this) ZEVariant();
	SetInt16(Value);
}

ZEVariant::ZEVariant(ZEInt32 Value)
{
	new (this) ZEVariant();
	SetInt32(Value);
}

ZEVariant::ZEVariant(ZEInt64 Value)
{
	new (this) ZEVariant();
	SetInt64(Value);
}

ZEVariant::ZEVariant(ZEUInt8 Value)
{
	new (this) ZEVariant();
	SetUInt8(Value);
}

ZEVariant::ZEVariant(ZEUInt16 Value)
{
	new (this) ZEVariant();
	SetUInt16(Value);
}

ZEVariant::ZEVariant(ZEUInt32 Value)
{
	new (this) ZEVariant();
	SetUInt32(Value);
}

ZEVariant::ZEVariant(ZEUInt64 Value)
{
	new (this) ZEVariant();
	SetUInt64(Value);
}

ZEVariant::ZEVariant(float Value)
{
	new (this) ZEVariant();
	SetFloat(Value);
}

ZEVariant::ZEVariant(double Value)
{
	new (this) ZEVariant();
	SetDouble(Value);
}

ZEVariant::ZEVariant(bool Value)
{
	new (this) ZEVariant();
	SetBool(Value);
}

ZEVariant::ZEVariant(const ZEVector2& Value)
{
	new (this) ZEVariant();
	SetVector2(Value);
}

ZEVariant::ZEVariant(const ZEVector2d& Value)
{
	new (this) ZEVariant();
	SetVector2d(Value);
}

ZEVariant::ZEVariant(const ZEVector3& Value)
{
	new (this) ZEVariant();
	SetVector3(Value);
}

ZEVariant::ZEVariant(const ZEVector3d& Value)
{
	new (this) ZEVariant();
	SetVector3d(Value);
}

ZEVariant::ZEVariant(const ZEVector4& Value)
{
	new (this) ZEVariant();
	SetVector4(Value);
}

ZEVariant::ZEVariant(const ZEVector4d& Value)
{
	new (this) ZEVariant();
	SetVector4d(Value);
}

ZEVariant::ZEVariant(const ZEQuaternion& Value)
{
	new (this) ZEVariant();
	SetQuaternion(Value);
}

ZEVariant::ZEVariant(const ZEMatrix3x3& Value)
{
	new (this) ZEVariant();
	SetMatrix3x3(Value);
}

ZEVariant::ZEVariant(const ZEMatrix3x3d& Value)
{
	new (this) ZEVariant();
	SetMatrix3x3d(Value);
}

ZEVariant::ZEVariant(const ZEMatrix4x4& Matrix)
{
	new (this) ZEVariant();
	SetMatrix4x4(Matrix);
}

ZEVariant::ZEVariant(const ZEMatrix4x4d& Matrix)
{
	new (this) ZEVariant();
	SetMatrix4x4d(Matrix);
}

ZEVariant::ZEVariant(ZEClass* Class)
{
	new (this) ZEVariant();
	SetClass(Class);
}

ZEVariant::~ZEVariant()
{
	SetType(ZEMTType());
}

#pragma warning(pop)
