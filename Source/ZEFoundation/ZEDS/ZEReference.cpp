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
#include "ZEPointer/ZEHolder.h"

template <typename ZEReturnType, ZEMTBaseType Type>
inline ZEReturnType& ZEReference::ConvertRef() const
{
	if (ValueType.GetCollectionType() != ZEMT_CT_NONE)
		zeCriticalError("Variant type mismatch. Can not convert collection type to non-collection type.");

	if (ValueType.GetBaseType() != Type)
		zeCriticalError("Variant type mismatch. Can not convert reference type to different reference type.");

	if (ValueType.GetBaseQualifier() != ZEMT_TQ_REFERENCE)
		zeCriticalError("Variant is const reference. Can not convert const reference to reference.");

	return *(ZEReturnType*)Value.Pointer;		
}

template <typename ZEReturnType, ZEMTBaseType Type>
inline const ZEReturnType& ZEReference::ConvertConstRef() const
{
	if (ValueType.GetCollectionType() != ZEMT_CT_NONE)
		zeCriticalError("Variant type mismatch. Can not convert collection type to non-collection type.");

	if (ValueType.GetBaseType() != Type)
		zeCriticalError("Variant type mismatch. Can not convert reference type to different reference type.");

	return *(const ZEReturnType*)Value.Pointer;
}

void ZEReference::SetType(const ZEMTType& Type)
{
	ValueType = Type;
}

ZEMTType ZEReference::GetType() const
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
	SetType(ZEMTType(ZEMT_BT_INTEGER_8, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = &Reference;
}

void ZEReference::SetInt8ConstRef(const ZEInt8& Reference)
{
	SetType(ZEMTType(ZEMT_BT_INTEGER_8, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = const_cast<ZEInt8*>(&Reference);
}

void ZEReference::SetInt16Ref(ZEInt16& Reference)
{
	SetType(ZEMTType(ZEMT_BT_INTEGER_16, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = &Reference;
}

void ZEReference::SetInt16ConstRef(const ZEInt16& Reference)
{
	SetType(ZEMTType(ZEMT_BT_INTEGER_16, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = const_cast<ZEInt16*>(&Reference);
}

void ZEReference::SetInt32Ref(ZEInt32& Reference)
{

	SetType(ZEMTType(ZEMT_BT_INTEGER_32, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = &Reference;
}

void ZEReference::SetInt32ConstRef(const ZEInt32& Reference)
{
	SetType(ZEMTType(ZEMT_BT_INTEGER_32, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = const_cast<ZEInt32*>(&Reference);
}

void ZEReference::SetInt64Ref(ZEInt64& Reference)
{
	SetType(ZEMTType(ZEMT_BT_INTEGER_64, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = &Reference;
}

void ZEReference::SetInt64ConstRef(const ZEInt64& Reference)
{
	SetType(ZEMTType(ZEMT_BT_INTEGER_64, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = const_cast<ZEInt64*>(&Reference);
}

void ZEReference::SetUInt8Ref(ZEUInt8& Reference)
{
	SetType(ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_8, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = &Reference;
}

void ZEReference::SetUInt8ConstRef(const ZEUInt8& Reference)
{
	SetType(ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_8, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = const_cast<ZEUInt8*>(&Reference);
}

void ZEReference::SetUInt16Ref(ZEUInt16& Reference)
{
	SetType(ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_16, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = &Reference;
}

void ZEReference::SetUInt16ConstRef(const ZEUInt16& Reference)
{
	SetType(ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_16, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = const_cast<ZEUInt16*>(&Reference);
}

void ZEReference::SetUInt32Ref(ZEUInt32& Reference)
{
	SetType(ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_32, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = &Reference;
}

void ZEReference::SetUInt32ConstRef(const ZEUInt32& Reference)
{
	SetType(ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_32, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = const_cast<ZEUInt32*>(&Reference);
}

void ZEReference::SetUInt64Ref(ZEUInt64& Reference)
{
	SetType(ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_64, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = &Reference;
}

void ZEReference::SetUInt64ConstRef(const ZEUInt64& Reference)
{
	SetType(ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_64, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = const_cast<ZEUInt64*>(&Reference);
}

void ZEReference::SetFloatRef(float& Value)
{
	SetType(ZEMTType(ZEMT_BT_FLOAT, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = &Value;
}

void ZEReference::SetFloatConstRef(const float& Value)
{
	SetType(ZEMTType(ZEMT_BT_FLOAT, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = const_cast<float*>(&Value);
}

void ZEReference::SetDoubleRef(double& Value)
{
	SetType(ZEMTType(ZEMT_BT_DOUBLE, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = &Value;
}

void ZEReference::SetDoubleConstRef(const double& Value)
{
	SetType(ZEMTType(ZEMT_BT_DOUBLE, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = const_cast<double*>(&Value);
}

void ZEReference::SetBooleanRef(bool& Value)
{
	SetType(ZEMTType(ZEMT_BT_BOOLEAN, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = &Value;
}

void ZEReference::SetBoolConstRef(const bool& Value)
{
	SetType(ZEMTType(ZEMT_BT_BOOLEAN, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = const_cast<bool*>(&Value);
}

void ZEReference::SetVector2Ref(ZEVector2& Value)
{
	SetType(ZEMTType(ZEMT_BT_VECTOR2, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = &Value;
}

void ZEReference::SetVector2ConstRef(const ZEVector2& Value)
{
	SetType(ZEMTType(ZEMT_BT_VECTOR2, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = const_cast<ZEVector2*>(&Value);
}

void ZEReference::SetVector2dRef(ZEVector2d& Value)
{
	SetType(ZEMTType(ZEMT_BT_VECTOR2D, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = &Value;
}

void ZEReference::SetVector2dConstRef(const ZEVector2d& Value)
{
	SetType(ZEMTType(ZEMT_BT_VECTOR2D, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = const_cast<ZEVector2d*>(&Value);
}

void ZEReference::SetVector3Ref(ZEVector3& Value)
{
	SetType(ZEMTType(ZEMT_BT_VECTOR3, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = &Value;
}

void ZEReference::SetVector3ConstRef(const ZEVector3& Value)
{
	SetType(ZEMTType(ZEMT_BT_VECTOR3, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = const_cast<ZEVector3*>(&Value);
}

void ZEReference::SetVector3dRef(ZEVector3d& Value)
{
	SetType(ZEMTType(ZEMT_BT_VECTOR3D, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = &Value;
}

void ZEReference::SetVector3dConstRef(const ZEVector3d& Value)
{
	SetType(ZEMTType(ZEMT_BT_VECTOR3D, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = const_cast<ZEVector3d*>(&Value);
}

void ZEReference::SetVector4Ref(ZEVector4& Value)
{
	SetType(ZEMTType(ZEMT_BT_VECTOR4, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = &Value;
}

void ZEReference::SetVector4ConstRef(const ZEVector4& Value)
{
	SetType(ZEMTType(ZEMT_BT_VECTOR4, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = const_cast<ZEVector4*>(&Value);
}

void ZEReference::SetVector4dRef(ZEVector4d& Value)
{
	SetType(ZEMTType(ZEMT_BT_VECTOR4D, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = &Value;
}

void ZEReference::SetVector4dConstRef(const ZEVector4d& Value)
{
	SetType(ZEMTType(ZEMT_BT_VECTOR4D, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = const_cast<ZEVector4d*>(&Value);
}

void ZEReference::SetQuaternionRef(ZEQuaternion& Value)
{
	SetType(ZEMTType(ZEMT_BT_QUATERNION, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = &Value;
}

void ZEReference::SetQuaternionConstRef(const ZEQuaternion& Value)
{
	SetType(ZEMTType(ZEMT_BT_QUATERNION, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = const_cast<ZEQuaternion*>(&Value);
}

void ZEReference::SetMatrix3x3Ref(ZEMatrix3x3& Value)
{
	SetType(ZEMTType(ZEMT_BT_MATRIX3X3, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = &Value;
}

void ZEReference::SetMatrix3x3ConstRef(const ZEMatrix3x3& Value)
{
	SetType(ZEMTType(ZEMT_BT_MATRIX3X3, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = const_cast<ZEMatrix3x3*>(&Value);
}

void ZEReference::SetMatrix3x3dRef(ZEMatrix3x3d& Value)
{
	SetType(ZEMTType(ZEMT_BT_MATRIX3X3D, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = &Value;
}

void ZEReference::SetMatrix3x3dConstRef(const ZEMatrix3x3d& Value)
{
	SetType(ZEMTType(ZEMT_BT_MATRIX3X3D, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = const_cast<ZEMatrix3x3d*>(&Value);
}

void ZEReference::SetMatrix4x4Ref(ZEMatrix4x4& Value)
{
	SetType(ZEMTType(ZEMT_BT_MATRIX4X4, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = &Value;
}

void ZEReference::SetMatrix4x4ConstRef(const ZEMatrix4x4& Value)
{
	SetType(ZEMTType(ZEMT_BT_MATRIX4X4, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = const_cast<ZEMatrix4x4*>(&Value);
}

void ZEReference::SetMatrix4x4dRef(ZEMatrix4x4d& Value)
{
	SetType(ZEMTType(ZEMT_BT_MATRIX4X4D, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = &Value;
}

void ZEReference::SetMatrix4x4dConstRef(const ZEMatrix4x4d& Value)
{
	SetType(ZEMTType(ZEMT_BT_MATRIX4X4D, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = const_cast<ZEMatrix4x4d*>(&Value);
}
void ZEReference::SetStringRef(ZEString& Value)
{
	SetType(ZEMTType(ZEMT_BT_STRING, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = &Value;
}

void ZEReference::SetStringConstRef(const ZEString& Value)
{
	SetType(ZEMTType(ZEMT_BT_STRING, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	this->Value.Pointer = const_cast<ZEString*>(&Value);
}

void ZEReference::SetObjectRef(ZEObject& Object)
{
	SetType(ZEMTType(ZEMT_BT_OBJECT, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, Object.GetClass()));
	Value.Pointer = &Object;
}

void ZEReference::SetObjectConstRef(const ZEObject& Object)
{
	SetType(ZEMTType(ZEMT_BT_OBJECT, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, Object.GetClass()));
	Value.Pointer = const_cast<ZEObject*>(&Object);
}

void ZEReference::SetObjectPtrRef(ZEObject*& Object)
{
	SetType(ZEMTType(ZEMT_BT_OBJECT_PTR, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, Object->GetClass()));
	Value.Pointer = &Object;
}

void ZEReference::SetObjectPtrConstRef(const ZEObject*& Object)
{
	SetType(ZEMTType(ZEMT_BT_OBJECT_PTR, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, Object->GetClass()));
	Value.Pointer = const_cast<ZEObject**>(&Object);
}

void ZEReference::SetObjectHolderRef(ZEHolderBase& Holder)
{
	SetType(ZEMTType(ZEMT_BT_OBJECT_HOLDER, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, Holder.GetObjectPtrConst() == NULL ? ZEObject::Class() : Holder.GetObjectPtrConst()->GetClass()));
	Value.Pointer = &Holder;
	
}
void ZEReference::SetObjectHolderConstRef(const ZEHolderBase& Holder)
{
	SetType(ZEMTType(ZEMT_BT_OBJECT_HOLDER, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, Holder.GetObjectPtrConst() == NULL ? ZEObject::Class() : Holder.GetObjectPtrConst()->GetClass()));
	Value.Pointer = const_cast<ZEHolderBase*>(&Holder);
}

void ZEReference::SetClassRef(ZEClass*& Class)
{
	SetType(ZEMTType(ZEMT_BT_CLASS, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = &Class;
}

void ZEReference::SetClassConstRef(const ZEClass*& Class)
{
	SetType(ZEMTType(ZEMT_BT_CLASS, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL));
	Value.Pointer = &Class;
}

void ZEReference::SetCollectionRef(ZEMTCollection& Collection)
{
	ZEMTType Type = Collection.GetType();
	Type.SetCollectionQualifier(ZEMT_TQ_REFERENCE);
	SetType(Type);

	Value.Pointer = &Collection;
}

void ZEReference::SetCollectionConstRef(const ZEMTCollection& Collection)
{
	ZEMTType Type = Collection.GetType();
	Type.SetCollectionQualifier(ZEMT_TQ_CONST_REFERENCE);
	SetType(Type);

	Value.Pointer = const_cast<ZEMTCollection*>(&Collection);
}

ZEInt8& ZEReference::GetInt8Ref() const
{
	return ConvertRef<ZEInt8, ZEMT_BT_INTEGER_8>();
}

const ZEInt8& ZEReference::GetInt8ConstRef() const
{
	return ConvertConstRef<ZEInt8, ZEMT_BT_INTEGER_8>();
}

ZEInt16& ZEReference::GetInt16Ref() const
{
	return ConvertRef<ZEInt16, ZEMT_BT_INTEGER_16>();
}

const ZEInt16& ZEReference::GetInt16ConstRef() const
{
	return ConvertConstRef<ZEInt16, ZEMT_BT_INTEGER_16>();
}

ZEInt32& ZEReference::GetInt32Ref() const
{
	return ConvertRef<ZEInt32, ZEMT_BT_INTEGER_32>();
}

const ZEInt32& ZEReference::GetInt32ConstRef() const
{
	return ConvertConstRef<ZEInt32, ZEMT_BT_INTEGER_32>();
}

ZEInt64& ZEReference::GetInt64Ref() const
{
	return ConvertRef<ZEInt64, ZEMT_BT_INTEGER_64>();
}

const ZEInt64& ZEReference::GetInt64ConstRef() const
{
	return ConvertConstRef<ZEInt64, ZEMT_BT_INTEGER_64>();
}

ZEUInt8& ZEReference::GetUInt8Ref() const
{
	return ConvertRef<ZEUInt8, ZEMT_BT_UNSIGNED_INTEGER_8>();
}

const ZEUInt8& ZEReference::GetUInt8ConstRef() const
{
	return ConvertConstRef<ZEUInt8, ZEMT_BT_UNSIGNED_INTEGER_8>();
}

ZEUInt16& ZEReference::GetUInt16Ref() const
{
	return ConvertRef<ZEUInt16, ZEMT_BT_UNSIGNED_INTEGER_16>();
}

const ZEUInt16& ZEReference::GetUInt16ConstRef() const
{
	return ConvertConstRef<ZEUInt16, ZEMT_BT_UNSIGNED_INTEGER_16>();
}

ZEUInt32& ZEReference::GetUInt32Ref() const
{
	return ConvertRef<ZEUInt32, ZEMT_BT_UNSIGNED_INTEGER_32>();
}

const ZEUInt32& ZEReference::GetUInt32ConstRef() const
{
	return ConvertConstRef<ZEUInt32, ZEMT_BT_UNSIGNED_INTEGER_32>();
}

ZEUInt64& ZEReference::GetUInt64Ref() const
{
	return ConvertRef<ZEUInt64, ZEMT_BT_UNSIGNED_INTEGER_64>();
}

const ZEUInt64& ZEReference::GetUInt64ConstRef() const
{
	return ConvertConstRef<ZEUInt64, ZEMT_BT_UNSIGNED_INTEGER_64>();
}

float& ZEReference::GetFloatRef() const
{
	return ConvertRef<float, ZEMT_BT_FLOAT>();
}

const float& ZEReference::GetFloatConstRef() const
{
	return ConvertConstRef<float, ZEMT_BT_FLOAT>();
}

double& ZEReference::GetDoubleRef() const
{
	return ConvertRef<double, ZEMT_BT_DOUBLE>();
}

const double& ZEReference::GetDoubleConstRef() const
{
	return ConvertConstRef<double, ZEMT_BT_DOUBLE>();
}

bool& ZEReference::GetBoolRef() const
{
	return ConvertRef<bool, ZEMT_BT_BOOLEAN>();
}

const bool& ZEReference::GetBoolConstRef() const
{
	return ConvertConstRef<bool, ZEMT_BT_BOOLEAN>();
}

ZEString& ZEReference::GetStringRef() const
{
	return ConvertRef<ZEString, ZEMT_BT_STRING>();
}

const ZEString& ZEReference::GetStringConstRef() const
{
	return ConvertConstRef<ZEString, ZEMT_BT_STRING>();
}

ZEVector2& ZEReference::GetVector2Ref() const
{
	return ConvertRef<ZEVector2, ZEMT_BT_VECTOR2>();
}

const ZEVector2& ZEReference::GetVector2ConstRef() const
{
	return ConvertConstRef<ZEVector2, ZEMT_BT_VECTOR2>();
}

ZEVector2d& ZEReference::GetVector2dRef() const
{
	return ConvertRef<ZEVector2d, ZEMT_BT_VECTOR2D>();
}

const ZEVector2d& ZEReference::GetVector2dConstRef() const
{
	return ConvertConstRef<ZEVector2d, ZEMT_BT_VECTOR2D>();
}

ZEVector3& ZEReference::GetVector3Ref() const
{
	return ConvertRef<ZEVector3, ZEMT_BT_VECTOR3>();
}

const ZEVector3& ZEReference::GetVector3ConstRef() const
{
	return ConvertConstRef<ZEVector3, ZEMT_BT_VECTOR3>();
}

ZEVector3d& ZEReference::GetVector3dRef() const
{
	return ConvertRef<ZEVector3d, ZEMT_BT_VECTOR3D>();
}

const ZEVector3d& ZEReference::GetVector3dConstRef() const
{
	return ConvertConstRef<ZEVector3d, ZEMT_BT_VECTOR3D>();
}

ZEVector4& ZEReference::GetVector4Ref() const
{
	return ConvertRef<ZEVector4, ZEMT_BT_VECTOR4>();
}

const ZEVector4& ZEReference::GetVector4ConstRef() const
{
	return ConvertConstRef<ZEVector4, ZEMT_BT_VECTOR4>();
}

ZEVector4d& ZEReference::GetVector4dRef() const
{
	return ConvertRef<ZEVector4d, ZEMT_BT_VECTOR4D>();
}

const ZEVector4d& ZEReference::GetVector4dConstRef() const
{
	return ConvertConstRef<ZEVector4d, ZEMT_BT_VECTOR4D>();
}

ZEQuaternion& ZEReference::GetQuaternionRef() const
{
	return ConvertRef<ZEQuaternion, ZEMT_BT_QUATERNION>();
}

const ZEQuaternion& ZEReference::GetQuaternionConstRef() const
{
	return ConvertConstRef<ZEQuaternion, ZEMT_BT_QUATERNION>();
}

ZEMatrix3x3& ZEReference::GetMatrix3x3Ref() const
{
	return ConvertRef<ZEMatrix3x3, ZEMT_BT_MATRIX3X3>();
}

const ZEMatrix3x3& ZEReference::GetMatrix3x3ConstRef() const
{
	return ConvertConstRef<ZEMatrix3x3, ZEMT_BT_MATRIX3X3>();
}

ZEMatrix3x3d& ZEReference::GetMatrix3x3dRef() const
{
	return ConvertRef<ZEMatrix3x3d, ZEMT_BT_MATRIX3X3D>();
}

const ZEMatrix3x3d& ZEReference::GetMatrix3x3dConstRef() const
{
	return ConvertConstRef<ZEMatrix3x3d, ZEMT_BT_MATRIX3X3D>();
}

ZEMatrix4x4& ZEReference::GetMatrix4x4Ref() const
{
	return ConvertRef<ZEMatrix4x4, ZEMT_BT_MATRIX4X4>();
}

const ZEMatrix4x4& ZEReference::GetMatrix4x4ConstRef() const
{
	return ConvertConstRef<ZEMatrix4x4, ZEMT_BT_MATRIX4X4>();
}

ZEMatrix4x4d& ZEReference::GetMatrix4x4dRef() const
{
	return ConvertRef<ZEMatrix4x4d, ZEMT_BT_MATRIX4X4D>();
}

const ZEMatrix4x4d& ZEReference::GetMatrix4x4dConstRef() const
{
	return ConvertConstRef<ZEMatrix4x4d, ZEMT_BT_MATRIX4X4D>();
}

ZEObject& ZEReference::GetObjectRef() const
{
	return ConvertRef<ZEObject, ZEMT_BT_OBJECT_PTR>();
}

const ZEObject& ZEReference::GetObjectConstRef() const
{
	return ConvertConstRef<ZEObject, ZEMT_BT_OBJECT_PTR>();
}

ZEObject*& ZEReference::GetObjectPtrRef() const
{
	return ConvertRef<ZEObject*, ZEMT_BT_OBJECT_PTR>();
}

const ZEObject*& ZEReference::GetObjectPtrConstRef() const
{
	if (ValueType.GetBaseType() != ZEMT_BT_OBJECT_PTR)
		zeCriticalError("Variant type mismatch. Can not convert reference type to different reference type.");

	return *static_cast<const ZEObject**>(Value.Pointer);
}

ZEHolderBase& ZEReference::GetObjectHolderRef() const
{
	return ConvertRef<ZEHolderBase, ZEMT_BT_OBJECT_HOLDER>();
}

const ZEHolderBase& ZEReference::GetObjectHolderConstRef() const
{
	return ConvertConstRef<ZEHolderBase, ZEMT_BT_OBJECT_HOLDER>();
}

ZEMTCollection& ZEReference::GetCollectionRef() const
{
	if (ValueType.GetCollectionType() != ZEMT_CT_NONE)
		zeCriticalError("Variant type mismatch. Can not convert non-collection type to collection type.");

	return *static_cast<ZEMTCollection*>(Value.Pointer);
}

const ZEMTCollection& ZEReference::GetCollectionConstRef() const
{
	if (ValueType.GetCollectionType() != ZEMT_CT_NONE)
		zeCriticalError("Variant type mismatch. Can not convert non-collection type to collection type.");

	return *static_cast<const ZEMTCollection*>(Value.Pointer);
}

ZEClass*& ZEReference::GetClassRef() const
{
	if (ValueType.GetBaseType() != ZEMT_BT_CLASS)
		zeCriticalError("Variant type mismatch. Can not convert reference type to different reference type.");

	return *static_cast<ZEClass**>(Value.Pointer);
}

const ZEClass*& ZEReference::GetClassConstRef() const
{
	return *static_cast<const ZEClass**>(Value.Pointer);
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

ZEReference& ZEReference::operator=(ZEVector2d& Value)
{
	SetVector2dRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(ZEVector3& Value)
{
	SetVector3Ref(Value);
	return *this;
}

ZEReference& ZEReference::operator=(ZEVector3d& Value)
{
	SetVector3dRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(ZEVector4& Value)
{
	SetVector4Ref(Value);
	return *this;
}

ZEReference& ZEReference::operator=(ZEVector4d& Value)
{
	SetVector4dRef(Value);
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

ZEReference& ZEReference::operator=(ZEMatrix3x3d& Value)
{
	SetMatrix3x3dRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(ZEMatrix4x4& Value)
{
	SetMatrix4x4Ref(Value);
	return *this;
}

ZEReference& ZEReference::operator=(ZEMatrix4x4d& Value)
{
	SetMatrix4x4dRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(ZEString& Value)
{
	SetStringRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(ZEObject& Value)
{
	SetObjectRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(ZEObject*& Value)
{
	SetObjectPtrRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(ZEHolderBase& Value)
{
	SetObjectHolderRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(ZEMTCollection& Value)
{
	SetCollectionRef(Value);
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

ZEReference& ZEReference::operator=(const ZEString& Value)
{
	SetStringConstRef(Value.ToCString());
	return *this;
}

ZEReference& ZEReference::operator=(const ZEVector2& Value)
{
	SetVector2ConstRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(const ZEVector2d& Value)
{
	SetVector2dConstRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(const ZEVector3& Value)
{
	SetVector3ConstRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(const ZEVector3d& Value)
{
	SetVector3dConstRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(const ZEVector4& Value)
{
	SetVector4ConstRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(const ZEVector4d& Value)
{
	SetVector4dConstRef(Value);
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

ZEReference& ZEReference::operator=(const ZEMatrix3x3d& Value)
{
	SetMatrix3x3dConstRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(const ZEMatrix4x4& Value)
{
	SetMatrix4x4ConstRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(const ZEMatrix4x4d& Value)
{
	SetMatrix4x4dConstRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(const ZEObject& Value)
{
	SetObjectConstRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(const ZEObject*& Value)
{
	SetObjectPtrConstRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(const ZEHolderBase& Value)
{
	SetObjectHolderConstRef(Value);
	return *this;
}

ZEReference& ZEReference::operator=(const ZEMTCollection& Value)
{
	SetCollectionConstRef(Value);
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

ZEReference::operator ZEVector2d&()
{
	return GetVector2dRef();
}

ZEReference::operator ZEVector3&()
{
	return GetVector3Ref();
}

ZEReference::operator ZEVector3d&()
{
	return GetVector3dRef();
}

ZEReference::operator ZEVector4&()
{
	return GetVector4Ref();
}

ZEReference::operator ZEVector4d&()
{
	return GetVector4dRef();
}

ZEReference::operator ZEQuaternion&()
{
	return GetQuaternionRef();
}

ZEReference::operator ZEMatrix3x3&()
{
	return GetMatrix3x3Ref();
}

ZEReference::operator ZEMatrix3x3d&()
{
	return GetMatrix3x3dRef();
}

ZEReference::operator ZEMatrix4x4&()
{
	return GetMatrix4x4Ref();
}

ZEReference::operator ZEMatrix4x4d&()
{
	return GetMatrix4x4dRef();
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

ZEReference::operator const ZEVector2d&()
{
	return GetVector2dConstRef();
}

ZEReference::operator const ZEVector3&()
{
	return GetVector3ConstRef();
}

ZEReference::operator const ZEVector3d&()
{
	return GetVector3dConstRef();
}

ZEReference::operator const ZEVector4&()
{
	return GetVector4ConstRef();
}

ZEReference::operator const ZEVector4d&()
{
	return GetVector4dConstRef();
}

ZEReference::operator const ZEQuaternion&()
{
	return GetQuaternionConstRef();
}

ZEReference::operator const ZEMatrix3x3&()
{
	return GetMatrix3x3ConstRef();
}

ZEReference::operator const ZEMatrix3x3d&()
{
	return GetMatrix3x3dConstRef();
}

ZEReference::operator const ZEMatrix4x4&()
{
	return GetMatrix4x4ConstRef();
}

ZEReference::operator const ZEMatrix4x4d&()
{
	return GetMatrix4x4dConstRef();
}

ZEReference::operator const ZEObject*&()
{
	return GetObjectPtrConstRef();
}

ZEReference::ZEReference()
{
	Value.Pointer = NULL;
}

ZEReference::ZEReference(const ZEReference& Value)
{
	new (this) ZEReference();
	SetReference(Value);
}

ZEReference::ZEReference(ZEInt8& Value)
{
	new (this) ZEReference();
	SetInt8Ref(Value);
}

ZEReference::ZEReference(ZEInt16& Value)
{
	new (this) ZEReference();
	SetInt16Ref(Value);
}

ZEReference::ZEReference(ZEInt32& Value)
{
	new (this) ZEReference();
	SetInt32Ref(Value);
}

ZEReference::ZEReference(ZEInt64& Value)
{
	new (this) ZEReference();
	SetInt64Ref(Value);
}

ZEReference::ZEReference(ZEUInt8& Value)
{
	new (this) ZEReference();
	SetUInt8Ref(Value);
}

ZEReference::ZEReference(ZEUInt16& Value)
{
	new (this) ZEReference();
	SetUInt16Ref(Value);
}

ZEReference::ZEReference(ZEUInt32& Value)
{
	new (this) ZEReference();
	SetUInt32Ref(Value);
}

ZEReference::ZEReference(ZEUInt64& Value)
{
	new (this) ZEReference();
	SetUInt64Ref(Value);
}

ZEReference::ZEReference(float& Value)
{
	new (this) ZEReference();
	SetFloatRef(Value);
}

ZEReference::ZEReference(double& Value)
{
	new (this) ZEReference();
	SetDoubleRef(Value);
}

ZEReference::ZEReference(bool& Value)
{
	new (this) ZEReference();
	SetBooleanRef(Value);
}

ZEReference::ZEReference(ZEString& Value)
{
	new (this) ZEReference();
	SetStringRef(Value);
}

ZEReference::ZEReference(ZEVector2& Value)
{
	new (this) ZEReference();
	SetVector2Ref(Value);
}

ZEReference::ZEReference(ZEVector2d& Value)
{
	new (this) ZEReference();
	SetVector2dRef(Value);
}

ZEReference::ZEReference(ZEVector3& Value)
{
	new (this) ZEReference();
	SetVector3Ref(Value);
}

ZEReference::ZEReference(ZEVector3d& Value)
{
	new (this) ZEReference();
	SetVector3dRef(Value);
}

ZEReference::ZEReference(ZEVector4& Value)
{
	new (this) ZEReference();
	SetVector4Ref(Value);
}

ZEReference::ZEReference(ZEVector4d& Value)
{
	new (this) ZEReference();
	SetVector4dRef(Value);
}

ZEReference::ZEReference(ZEQuaternion& Value)
{
	new (this) ZEReference();
	SetQuaternionRef(Value);
}

ZEReference::ZEReference(ZEMatrix3x3& Value)
{
	new (this) ZEReference();
	SetMatrix3x3Ref(Value);
}

ZEReference::ZEReference(ZEMatrix3x3d& Value)
{
	new (this) ZEReference();
	SetMatrix3x3dRef(Value);
}

ZEReference::ZEReference(ZEMatrix4x4& Value)
{
	new (this) ZEReference();
	SetMatrix4x4Ref(Value);
}

ZEReference::ZEReference(ZEMatrix4x4d& Value)
{
	new (this) ZEReference();
	SetMatrix4x4dRef(Value);
}

ZEReference::ZEReference(ZEObject& Value)
{
	new (this) ZEReference();
	SetObjectRef(Value);
}

ZEReference::ZEReference(ZEObject*& Value)
{
	new (this) ZEReference();
	SetObjectPtrRef(Value);
}

ZEReference::ZEReference(ZEHolderBase& Value)
{
	new (this) ZEReference();
	SetObjectHolderRef(Value);
}

ZEReference::ZEReference(ZEMTCollection& Value)
{
	new (this) ZEReference();
	SetCollectionRef(Value);
}

ZEReference::ZEReference(const ZEInt8& Value)
{
	new (this) ZEReference();
	SetInt8ConstRef(Value);
}

ZEReference::ZEReference(const ZEInt16& Value)
{
	new (this) ZEReference();
	SetInt16ConstRef(Value);
}

ZEReference::ZEReference(const ZEInt32& Value)
{
	new (this) ZEReference();
	SetInt32ConstRef(Value);
}

ZEReference::ZEReference(const ZEInt64& Value)
{
	new (this) ZEReference();
	SetInt64ConstRef(Value);
}

ZEReference::ZEReference(const ZEUInt8& Value)
{
	new (this) ZEReference();
	SetUInt8ConstRef(Value);
}

ZEReference::ZEReference(const ZEUInt16& Value)
{
	new (this) ZEReference();
	SetUInt16ConstRef(Value);
}

ZEReference::ZEReference(const ZEUInt32& Value)
{
	new (this) ZEReference();
	SetUInt32ConstRef(Value);
}

ZEReference::ZEReference(const ZEUInt64& Value)
{
	new (this) ZEReference();
	SetUInt64ConstRef(Value);
}

ZEReference::ZEReference(const float& Value)
{
	new (this) ZEReference();
	SetFloatConstRef(Value);
}

ZEReference::ZEReference(const double& Value)
{
	new (this) ZEReference();
	SetDoubleConstRef(Value);
}

ZEReference::ZEReference(const bool& Value)
{
	new (this) ZEReference();
	SetBoolConstRef(Value);
}

ZEReference::ZEReference(const ZEString& Value)
{
	new (this) ZEReference();
	SetStringConstRef(Value);
}

ZEReference::ZEReference(const ZEVector2& Value)
{
	new (this) ZEReference();
	SetVector2ConstRef(Value);
}

ZEReference::ZEReference(const ZEVector2d& Value)
{
	new (this) ZEReference();
	SetVector2dConstRef(Value);
}

ZEReference::ZEReference(const ZEVector3& Value)
{
	new (this) ZEReference();
	SetVector3ConstRef(Value);
}

ZEReference::ZEReference(const ZEVector3d& Value)
{
	new (this) ZEReference();
	SetVector3dConstRef(Value);
}

ZEReference::ZEReference(const ZEVector4& Value)
{
	new (this) ZEReference();
	SetVector4ConstRef(Value);
}

ZEReference::ZEReference(const ZEVector4d& Value)
{
	new (this) ZEReference();
	SetVector4dConstRef(Value);
}

ZEReference::ZEReference(const ZEQuaternion& Value)
{
	new (this) ZEReference();
	SetQuaternionConstRef(Value);
}

ZEReference::ZEReference(const ZEMatrix3x3& Value)
{
	new (this) ZEReference();
	SetMatrix3x3ConstRef(Value);
}

ZEReference::ZEReference(const ZEMatrix3x3d& Value)
{
	new (this) ZEReference();
	SetMatrix3x3dConstRef(Value);
}

ZEReference::ZEReference(const ZEMatrix4x4& Value)
{
	new (this) ZEReference();
	SetMatrix4x4ConstRef(Value);
}

ZEReference::ZEReference(const ZEMatrix4x4d& Value)
{
	new (this) ZEReference();
	SetMatrix4x4dConstRef(Value);
}

ZEReference::ZEReference(const ZEObject& Value)
{
	new (this) ZEReference();
	SetObjectConstRef(Value);
}

ZEReference::ZEReference(const ZEObject*& Value)
{
	new (this) ZEReference();
	SetObjectPtrConstRef(Value);
}

ZEReference::ZEReference(const ZEHolderBase& Value)
{
	new (this) ZEReference();
	SetObjectHolderConstRef(Value);
}

ZEReference::ZEReference(const ZEMTCollection& Value)
{
	new (this) ZEReference();
	SetCollectionConstRef(Value);
}

ZEReference::~ZEReference()
{

}
