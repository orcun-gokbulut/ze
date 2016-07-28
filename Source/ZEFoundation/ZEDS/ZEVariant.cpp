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
#include "ZEMath/ZEVectord.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEMatrixd.h"

#include "ZEMeta/ZEObject.h"
#include "ZEMeta/ZEType.h"

#pragma warning(push)
#pragma warning(disable:4482 4996 4244 4800)

template<typename ZEReturnType>
inline ZEReturnType ZEVariant::ConvertValue() const
{
	if (ValueType.TypeQualifier ==  ZE_TQ_VALUE)
	{
		switch(ValueType.Type)
		{
			default:
			case ZE_TT_UNDEFINED:
			case ZE_TT_VOID:
				return (ZEReturnType)0;
			case ZE_TT_INTEGER_8:
			case ZE_TT_INTEGER_16:
			case ZE_TT_INTEGER_32:
			case ZE_TT_UNSIGNED_INTEGER_8:
			case ZE_TT_UNSIGNED_INTEGER_16:
			case ZE_TT_UNSIGNED_INTEGER_32:
				return (ZEReturnType)Value.Int32;
			case ZE_TT_INTEGER_64:
			case ZE_TT_UNSIGNED_INTEGER_64:
				return (ZEReturnType)Value.Int64;
			case ZE_TT_FLOAT:
				return Value.Float;
			case ZE_TT_DOUBLE:
				return Value.Double;
			case ZE_TT_BOOLEAN:
				return (ZEReturnType)Value.Boolean;
		}
	}
	else
	{
		switch(ValueType.Type)
		{
			case ZE_TT_INTEGER_8:
				return (ZEReturnType)*(const ZEInt8*)Value.Pointer;
			case ZE_TT_INTEGER_16:
				return (ZEReturnType)*(const ZEInt16*)Value.Pointer;
			case ZE_TT_INTEGER_32:
				return (ZEReturnType)*(const ZEInt32*)Value.Pointer;
			case ZE_TT_INTEGER_64:
				return (ZEReturnType)*(const ZEInt64*)Value.Pointer;
			case ZE_TT_UNSIGNED_INTEGER_8:
				return (ZEReturnType)*(const ZEUInt8*)Value.Pointer;
			case ZE_TT_UNSIGNED_INTEGER_16:
				return (ZEReturnType)*(const ZEUInt16*)Value.Pointer;
			case ZE_TT_UNSIGNED_INTEGER_32:
				return (ZEReturnType)*(const ZEUInt32*)Value.Pointer;
			case ZE_TT_UNSIGNED_INTEGER_64:
				return (ZEReturnType)*(const ZEInt64*)Value.Pointer;
			case ZE_TT_FLOAT:
				return (ZEReturnType)*(const float*)Value.Pointer;
			case ZE_TT_DOUBLE:
				return (ZEReturnType)*(const double*)Value.Pointer;
			case ZE_TT_BOOLEAN:
				return (ZEReturnType)*(const bool*)Value.Pointer;
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

	if (ValueType.TypeQualifier == ZE_TQ_VALUE)
		zeCriticalError("Variant is a value. Can not convert value to reference.");
	else if (ValueType.TypeQualifier == ZE_TQ_CONST_REFERENCE)
		zeCriticalError("Variant is const reference. Can not convert const reference to reference.");

	return *(ZEReturnType*)Value.Pointer;
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
		if (ValueType.Type == ZE_TT_OBJECT)
			Deleter(Value.Pointer);
		else if (ValueType.Type == ZE_TT_STRING)
			Value.String.Clear();
		else if (ValueType.Type == ZE_TT_MATRIX3X3)
			delete (ZEMatrix3x3*)Value.Pointer;
		else if (ValueType.Type == ZE_TT_MATRIX3X3D)
			delete (ZEMatrix3x3d*)Value.Pointer;
		else if (ValueType.Type == ZE_TT_MATRIX4X4)
			delete (ZEMatrix4x4*)Value.Pointer;
		else if (ValueType.Type == ZE_TT_MATRIX4X4D)
			delete (ZEMatrix4x4d*)Value.Pointer;

		Value.Pointer = NULL;
	}

	Cloner = NULL;
	Deleter = NULL;
	ValueType = NewType;

	if (ValueType.TypeQualifier == ZE_TQ_VALUE && Value.Pointer == NULL)
	{
		if (ValueType.Type == ZE_TT_MATRIX3X3)
			Value.Pointer = new ZEMatrix3x3();
		else if (ValueType.Type == ZE_TT_MATRIX3X3D)
			Value.Pointer =  new ZEMatrix3x3d();
		else if (ValueType.Type == ZE_TT_MATRIX4X4)
			Value.Pointer =  new ZEMatrix4x4();
		else if (ValueType.Type == ZE_TT_MATRIX4X4D)
			Value.Pointer = new ZEMatrix4x4d();
	}
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
		if (ValueType.Type == ZE_TT_OBJECT)
			Value.Pointer = Variant.Cloner(Variant.Value.Pointer);
		else if (ValueType.Type == ZE_TT_MATRIX3X3)
			Value.Pointer = new ZEMatrix3x3(*(const ZEMatrix3x3*)Variant.Value.Pointer);
		else if (ValueType.Type == ZE_TT_MATRIX3X3D)
			Value.Pointer = new ZEMatrix3x3d(*(const ZEMatrix3x3d*)Variant.Value.Pointer);
		else if (ValueType.Type == ZE_TT_MATRIX4X4)
			Value.Pointer = new ZEMatrix4x4(*(const ZEMatrix4x4*)Variant.Value.Pointer);
		else if (ValueType.Type == ZE_TT_MATRIX4X4D)
			Value.Pointer = new ZEMatrix4x4d(*(const ZEMatrix4x4d*)Variant.Value.Pointer);
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

void ZEVariant::SetVector2d(const ZEVector2d& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR2D;
	SetType(Type);

	memcpy(this->Value.DoubleVectors, &Value, sizeof(ZEVector2d));
}

void ZEVariant::SetVector2dRef(ZEVector2d& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR2D;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZEVariant::SetVector2dConstRef(const ZEVector2d& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR2D;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<ZEVector2d*>(&Value);
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

void ZEVariant::SetVector3d(const ZEVector3d& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR3D;
	SetType(Type);

	memcpy(this->Value.DoubleVectors, &Value, sizeof(ZEVector3d));
}

void ZEVariant::SetVector3dRef(ZEVector3d& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR3D;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZEVariant::SetVector3dConstRef(const ZEVector3d& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR3D;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<ZEVector3d*>(&Value);
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

void ZEVariant::SetVector4d(const ZEVector4d& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR4D;
	SetType(Type);

	memcpy(this->Value.DoubleVectors, &Value, sizeof(ZEVector4d));
}

void ZEVariant::SetVector4dRef(ZEVector4d& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR4D;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZEVariant::SetVector4dConstRef(const ZEVector4d& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR4D;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<ZEVector4d*>(&Value);
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

	*(ZEMatrix3x3*)this->Value.Pointer = Value;
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

void ZEVariant::SetMatrix3x3d(const ZEMatrix3x3d& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_MATRIX3X3D;
	SetType(Type);

	*(ZEMatrix3x3d*)this->Value.Pointer = Value;
}

void ZEVariant::SetMatrix3x3dRef(ZEMatrix3x3d& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_MATRIX3X3D;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZEVariant::SetMatrix3x3dConstRef(const ZEMatrix3x3d& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_MATRIX3X3D;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<ZEMatrix3x3d*>(&Value);
}

void ZEVariant::SetMatrix4x4(const ZEMatrix4x4& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_MATRIX4X4;
	SetType(Type);

	*(ZEMatrix4x4*)this->Value.Pointer = Value;
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

void ZEVariant::SetMatrix4x4d(const ZEMatrix4x4d& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_MATRIX4X4D;
	SetType(Type);

	*(ZEMatrix4x4d*)this->Value.Pointer = Value;
}

void ZEVariant::SetMatrix4x4dRef(ZEMatrix4x4d& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_MATRIX4X4D;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	SetType(Type);

	this->Value.Pointer = &Value;
}

void ZEVariant::SetMatrix4x4dConstRef(const ZEMatrix4x4d& Value)
{
	ZEType Type;
	Type.Type = ZE_TT_MATRIX4X4D;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	SetType(Type);

	this->Value.Pointer = const_cast<ZEMatrix4x4d*>(&Value);
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
	return (GetType().Type == ZE_TT_UNDEFINED);
}

ZEInt8 ZEVariant::GetInt8() const
{
	return ConvertValue<ZEInt8>();
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
	return ConvertValue<ZEInt16>();
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
	return ConvertValue<ZEInt32>();
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
	return ConvertValue<ZEInt64>();
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
	return ConvertValue<ZEUInt8>();
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
	return ConvertValue<ZEUInt16>();
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
	return ConvertValue<ZEUInt32>();
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
	return ConvertValue<ZEUInt64>();
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
	return ConvertValue<float>();
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
	return ConvertValue<double>();
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
	return ConvertValue<bool>();
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
	return ConvertValue<ZEInt32>();
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

const ZEVector2d& ZEVariant::GetVector2d() const
{
	if (ValueType.Type != ZE_TT_VECTOR2D)
		zeCriticalError("Variant type mismatch. Can not convert non-numerical non-scaler types.");

	if (ValueType.TypeQualifier == ZE_TQ_VALUE)
		return *(ZEVector2d*)&Value.DoubleVectors;
	else
		return *(ZEVector2d*)&Value.Pointer;
}

ZEVector2d& ZEVariant::GetVector2dRef() const
{
	return ConvertRef<ZEVector2d, ZE_TT_VECTOR2D>();
}

const ZEVector2d& ZEVariant::GetVector2dConstRef() const
{
	return ConvertConstRef<ZEVector2d, ZE_TT_VECTOR2D>();
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

const ZEVector3d& ZEVariant::GetVector3d() const
{
	if (ValueType.Type != ZE_TT_VECTOR3D)
		zeCriticalError("Variant type mismatch. Can not convert non-numerical non-scaler types.");

	if (ValueType.TypeQualifier == ZE_TQ_VALUE)
		return *(ZEVector3d*)&Value.DoubleVectors;
	else
		return *(ZEVector3d*)&Value.Pointer;
}

ZEVector3d& ZEVariant::GetVector3dRef() const
{
	return ConvertRef<ZEVector3d, ZE_TT_VECTOR3D>();
}

const ZEVector3d& ZEVariant::GetVector3dConstRef() const
{
	return ConvertConstRef<ZEVector3d, ZE_TT_VECTOR3D>();
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

const ZEVector4d& ZEVariant::GetVector4d() const
{
	if (ValueType.Type != ZE_TT_VECTOR4D)
		zeCriticalError("Variant type mismatch. Can not convert non-numerical non-scaler types.");

	if (ValueType.TypeQualifier == ZE_TQ_VALUE)
		return *(ZEVector4d*)&Value.DoubleVectors;
	else
		return *(ZEVector4d*)&Value.Pointer;
}

ZEVector4d& ZEVariant::GetVector4dRef() const
{
	return ConvertRef<ZEVector4d, ZE_TT_VECTOR4D>();
}

const ZEVector4d& ZEVariant::GetVector4dConstRef() const
{
	return ConvertConstRef<ZEVector4d, ZE_TT_VECTOR4D>();
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

const ZEMatrix3x3d& ZEVariant::GetMatrix3x3d() const
{
	if (ValueType.Type != ZE_TT_MATRIX3X3D)
		zeCriticalError("Variant type mismatch. Can not convert non-numerical non-scaler types.");

	return *(ZEMatrix3x3d*)Value.Pointer;
}

ZEMatrix3x3d& ZEVariant::GetMatrix3x3dRef() const
{
	return ConvertRef<ZEMatrix3x3d, ZE_TT_MATRIX3X3D>();
}

const ZEMatrix3x3d& ZEVariant::GetMatrix3x3dConstRef() const
{
	return ConvertConstRef<ZEMatrix3x3d, ZE_TT_MATRIX3X3D>();
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

const ZEMatrix4x4d& ZEVariant::GetMatrix4x4d() const
{
	if (ValueType.Type != ZE_TT_MATRIX4X4D)
		zeCriticalError("Variant type mismatch. Can not convert non-numerical non-scaler types.");

	return *(ZEMatrix4x4d*)Value.Pointer;
}

ZEMatrix4x4d& ZEVariant::GetMatrix4x4dRef() const
{
	return ConvertRef<ZEMatrix4x4d, ZE_TT_MATRIX4X4D>();
}

const ZEMatrix4x4d& ZEVariant::GetMatrix4x4dConstRef() const
{
	return ConvertConstRef<ZEMatrix4x4d, ZE_TT_MATRIX4X4D>();
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

ZEValue ZEVariant::GetValue()
{
	switch (ValueType.Type)
	{
		default:
			zeCriticalError("Unknown variant type.");
		case ZE_TT_OBJECT:	
		case ZE_TT_UNDEFINED:
		case ZE_TT_VOID:
		case ZE_TT_CLASS:
			return ZEValue();

		case ZE_TT_INTEGER_8:
			return ZEValue(GetInt8());

		case ZE_TT_INTEGER_16:
			return ZEValue(GetInt16());

		case ZE_TT_INTEGER_32:
			return ZEValue(GetInt32());

		case ZE_TT_INTEGER_64:
			return ZEValue(GetInt64());

		case ZE_TT_UNSIGNED_INTEGER_8:
			return ZEValue(GetUInt8());

		case ZE_TT_UNSIGNED_INTEGER_16:
			return ZEValue(GetUInt16());

		case ZE_TT_UNSIGNED_INTEGER_32:
			return ZEValue(GetUInt32());

		case ZE_TT_UNSIGNED_INTEGER_64:
			return ZEValue(GetUInt64());

		case ZE_TT_FLOAT:
			return ZEValue(GetFloat());

		case ZE_TT_DOUBLE:
			return ZEValue(GetDouble());

		case ZE_TT_BOOLEAN:
			return ZEValue(GetBool());

		case ZE_TT_STRING:
			return ZEValue(GetString());

		case ZE_TT_QUATERNION:
			return ZEValue(GetQuaternion());

		case ZE_TT_VECTOR2:
			return ZEValue(GetVector2());

		case ZE_TT_VECTOR2D:
			return ZEValue(GetVector2d());

		case ZE_TT_VECTOR3:
			return ZEValue(GetVector3());

		case ZE_TT_VECTOR3D:
			return ZEValue(GetVector3d());

		case ZE_TT_VECTOR4:
			return ZEValue(GetVector4());

		case ZE_TT_VECTOR4D:
			return ZEValue(GetVector4d());

		case ZE_TT_MATRIX3X3:
			return ZEValue(GetMatrix3x3());

		case ZE_TT_MATRIX3X3D:
			return ZEValue(GetMatrix3x3d());

		case ZE_TT_MATRIX4X4:
			return ZEValue(GetMatrix4x4());

		case ZE_TT_MATRIX4X4D:
			return ZEValue(GetMatrix4x4d());

		case ZE_TT_ENUMERATOR:
			return ZEValue(GetInt32());
	}
}

ZEVariant::ZEVariant()
{
	Value.Pointer = NULL;
	Deleter = NULL;
	Cloner = NULL;
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
	SetType(ZEType());
}

#pragma warning(pop)
