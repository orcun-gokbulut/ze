//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEVariantImp.h
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

template <typename ZEReturnType, ZEMTBaseType Type>
inline ZEReturnType ZEVariant::ConvertValue(const void* ValuePointer) const
{
	if (ValueType.ContainerType != ZEMT_CT_NONE)
		zeCriticalError("Variant type mismatch. Cannot convert container type to non-container type.");

	if (ValueType.Type != Type)
		zeCriticalError("Variant type mismatch. Cannot convert non-primitive value type to a different value type.");

	if (ValueType.TypeQualifier == ZEMT_TQ_REFERENCE)
		return *static_cast<ZEReturnType*>(const_cast<void*>(Value.Pointer));
	else if (ValueType.TypeQualifier == ZEMT_TQ_VALUE)
		return *static_cast<ZEReturnType*>(const_cast<void*>(ValuePointer));
	else if (ValueType.TypeQualifier == ZEMT_TQ_CONST_REFERENCE)
		zeCriticalError("Variant type mismatch. Cannot convert const value type to non-const reference type.");
	else if (ValueType.TypeQualifier == ZEMT_TQ_CONST_VALUE)
		zeCriticalError("Variatn type mismatch. Cannot convert const value type to non-const reference type.");
	else
		zeCriticalError("Variant internal error. Unknown type qualifier.");
}

template <typename ZEReturnType, ZEMTBaseType Type>
inline const ZEReturnType ZEVariant::ConvertConstValue(const void* ValuePointer) const
{
	if (ValueType.ContainerType != ZEMT_CT_NONE)
		zeCriticalError("Variant type mismatch. Cannot convert container type to non-container type.");

	if (ValueType.Type != Type)
		zeCriticalError("Variant type mismatch. Can not convert reference type to a different reference type.");

	if (ValueType.TypeQualifier == ZEMT_TQ_VALUE || ValueType.TypeQualifier == ZEMT_TQ_CONST_VALUE)
		return *static_cast<const ZEReturnType*>(ValuePointer);
	else if (ValueType.TypeQualifier == ZEMT_TQ_REFERENCE || ValueType.TypeQualifier == ZEMT_TQ_CONST_REFERENCE)
		return *static_cast<const ZEReturnType*>(Value.Pointer);
	else
		zeCriticalError("Variant internal error. Unknown type qualifier.");
}

template <typename ZEReturnType, ZEMTBaseType Type>
inline ZEReturnType& ZEVariant::ConvertRef(const void* ValuePointer) const
{
	if (ValueType.ContainerType != ZEMT_CT_NONE)
		zeCriticalError("Variant type mismatch. Cannot convert container type to non-container reference type.");

	if (ValueType.Type != Type)
		zeCriticalError("Variant type mismatch. Can not convert reference type to a different reference type.");

	if (ValueType.TypeQualifier == ZEMT_TQ_REFERENCE)
		return *static_cast<ZEReturnType*>(const_cast<void*>(Value.Pointer));
	else if (ValueType.TypeQualifier == ZEMT_TQ_VALUE)
		return *static_cast<ZEReturnType*>(const_cast<void*>(ValuePointer));
	else if (ValueType.TypeQualifier == ZEMT_TQ_CONST_REFERENCE)
		zeCriticalError("Variant type mismatch. Cannot convert const reference type to non-const reference type.");
	else if (ValueType.TypeQualifier == ZEMT_TQ_CONST_VALUE)
		zeCriticalError("Variatn type mismatch. Cannot convert const value type to non-const reference type.");
	else
		zeCriticalError("Variant internal error. Unknown type qualifier.");
}

template <typename ZEReturnType, ZEMTBaseType Type>
inline ZEReturnType& ZEVariant::ConvertConstRef(const void* ValuePointer) const
{
	if (ValueType.ContainerType != ZEMT_CT_NONE)
		zeCriticalError("Variant type mismatch. Cannot convert container type to non-container type.");

	if (ValueType.Type != Type)
		zeCriticalError("Variant type mismatch. Can not convert reference type to a different reference type.");

	if (ValueType.TypeQualifier == ZEMT_TQ_VALUE || ValueType.TypeQualifier == ZEMT_TQ_CONST_VALUE)
		return *(ZEReturnType*)(ValuePointer);
	else if (ValueType.TypeQualifier == ZEMT_TQ_REFERENCE || ValueType.TypeQualifier == ZEMT_TQ_CONST_REFERENCE)
		return *static_cast<ZEReturnType*>(Value.Pointer);
	else
		zeCriticalError("Variant internal error. Unknown type qualifier.");
}

template<typename ZETypeInstance>
void* ZEVariant::ClonerTemplate(const void* Instance)
{
	return new ZETypeInstance(*(const ZETypeInstance*)Instance);
}

template<typename ZETypeInstance>
void ZEVariant::DeleterTemplate(void* Instance)
{
	delete (ZETypeInstance*)Instance;
}

template<typename ZEItemType>
void ZEVariant::SetArray(const ZEArray<ZEItemType>& Array)
{
	ZEMTType Type = ZEMTTypeGenerator<ZEItemType>::GetType();
	if (Type.Type == ZEMT_BT_UNDEFINED)
		return;

	if (Type.TypeQualifier != ZEMT_TQ_VALUE)
		zeCriticalError("Only value or object pointer type arrays supported.");

	Type.TypeQualifier = ZEMT_TQ_VALUE;
	Type.ContainerType = ZEMT_CT_ARRAY;

	SetType(Type);
	Cloner = &ClonerTemplate<ZEArray<ZEItemType> >;
	Deleter = &DeleterTemplate<ZEArray<ZEItemType> >;
	Value.Pointer = new ZEArray<ZEItemType>(Array);
}

template<typename ZEItemType>
void ZEVariant::SetArray(const ZEArray<ZEItemType>& Array, ZEClass* ObjectPtrClass)
{
	ZEMTType Type;
	Type.Type = ZEMT_BT_OBJECT_PTR;
	Type.TypeQualifier = ZEMT_TQ_VALUE;
	Type.ContainerType = ZEMT_CT_ARRAY;
	Type.Class = ObjectPtrClass;

	SetType(Type);
	Cloner = &ClonerTemplate<ZEArray<ZEItemType> >;
	Deleter = &DeleterTemplate<ZEArray<ZEItemType> >;
	Value.Pointer = new ZEArray<ZEItemType>(Array);
}

template<typename ZEItemType>
void ZEVariant::SetArrayRef(ZEArray<ZEItemType>& Array)
{
	ZEMTType Type = ZEMTTypeGenerator<ZEItemType>::GetType();
	if (Type.Type == ZEMT_BT_UNDEFINED)
		return;

	if (Type.ContainerType == ZEMT_CT_NONE)
		zeCriticalError("Array to arrays are not supported.");

	if (Type.TypeQualifier != ZEMT_TQ_VALUE)
		zeCriticalError("Only value or object pointer type arrays supported.");

	Type.TypeQualifier = ZEMT_TQ_REFERENCE;
	Type.ContainerType = ZEMT_CT_ARRAY;

	SetType(Type);
	Value.Pointer = &Array;
}

template<typename ZEItemType>
void ZEVariant::SetArrayRef(ZEArray<ZEItemType>& Array, ZEClass* ObjectClass)
{
	ZEMTType Type;
	Type.Type = ZEMT_BT_OBJECT_PTR;
	Type.TypeQualifier = ZEMT_TQ_REFERENCE;
	Type.ContainerType = ZEMT_CT_ARRAY;
	Type.Class = ObjectClass;

	SetType(Type);
	Value.Pointer = &Array;
}

template<typename ZEItemType>
void ZEVariant::SetArrayConstRef(const ZEArray<ZEItemType>& Array)
{
	ZEMTType Type = ZEMTTypeGenerator<ZEItemType>::GetType();
	
	if (Type.Type == ZEMT_BT_UNDEFINED)
		return;
	
	if (Type.ContainerType == ZEMT_CT_NONE)
		zeCriticalError("Array to arrays are not supported.");

	if (Type.TypeQualifier != ZEMT_TQ_VALUE)
		zeCriticalError("Only value or object pointer type arrays supported.");

	Type.TypeQualifier = ZEMT_TQ_CONST_REFERENCE;
	Type.ContainerType = ZEMT_CT_ARRAY;
	
	SetType(Type);
	Value.Pointer = const_cast<ZEArray<ZEItemType>*>(&Array);
}

template<typename ZEItemType>
void ZEVariant::SetArrayConstRef(const ZEArray<ZEItemType>& Array, ZEClass* ObjectClass)
{
	ZEMTType Type;
	Type.Type = ZEMT_BT_OBJECT_PTR;
	Type.TypeQualifier = ZEMT_TQ_CONST_REFERENCE;
	Type.ContainerType = ZEMT_CT_ARRAY;
	Type.Class = ObjectClass;

	SetType(Type);
	Value.Pointer = const_cast<ZEArray<ZEItemType>*>(&Array);
}

template<typename ZEObjectType>
void ZEVariant::SetObject(const ZEObjectType& Object)
{
	ZEMTType Type;
	Type.Type = ZEMT_BT_OBJECT;
	Type.TypeQualifier = ZEMT_TQ_VALUE;
	Type.Class = Object.GetClass();
	SetType(Type);

	Cloner = ClonerTemplate<ZEObjectType>;
	Deleter = DeleterTemplate<ZEObjectType>;

	if (Value.Pointer != NULL)
		Deleter(Value.Pointer);

	Value.Pointer = new ZEObjectType(Object);
}

template<typename ZEObjectType>
void ZEVariant::SetObjectRef(ZEObjectType& Object)
{
	ZEMTType Type;
	Type.Type = ZEMT_BT_OBJECT;
	Type.TypeQualifier = ZEMT_TQ_REFERENCE;
	Type.Class = Object.GetClass();
	SetType(Type);

	Value.Pointer = &Object;
}

template<typename ZEObjectType>
void ZEVariant::SetObjectConstRef(const ZEObjectType& Object)
{
	ZEMTType Type;
	Type.Type = ZEMT_BT_OBJECT;
	Type.TypeQualifier = ZEMT_TQ_CONST_REFERENCE;
	Type.Class = Object.GetClass();
	SetType(Type);

	Value.Pointer = const_cast<ZEObjectType*>(&Object);
}

template<typename ZEItemType>
const ZEArray<ZEItemType>& ZEVariant::GetArray() const
{
	ZEMTType Type = ZEMTTypeGenerator<ZEItemType>::GetType();
	if (ValueType.Type != ZE_TT_ARRAY || ValueType.SubType != Type.SubType)
		zeCriticalError("Variant type mismatch. Can not convert non-numerical non-scaler types.");

	return *(const ZEArray<ZEItemType>*)Value.Pointer;
}

template<typename ZEItemType>
ZEArray<ZEItemType>& ZEVariant::GetArrayRef() const
{
	ZEMTType Type = ZEMTTypeGenerator<ZEItemType>::GetType();
	if (ValueType.Type != ZE_TT_ARRAY || ValueType.SubTypeType != Type.Type)
		zeCriticalError("Variant type mismatch. Can not convert reference type to different reference type.");

	if (ValueType.SubTypeQualifier == ZEMT_TQ_REFERENCE)
		return *(ZEReturnType*)Value.Pointer;
	else if (ValueType.SubTypeQualifier == ZEMT_TQ_CONST_REFERENCE)
		zeCriticalError("Variant is const reference. Can not convert const reference to reference.");

	return *(ZEArray<ZEItemType>*)Value.Pointer;
}

template<typename ZEItemType>
const ZEArray<ZEItemType>& ZEVariant::GetArrayConstRef() const
{
	ZEMTType Type = ZEMTTypeGenerator<ZEItemType>::GetType();

	if (ValueType.Type = ZE_TT_ARRAY || ValueType.SubTypeType != Type.Type)
		zeCriticalError("Variant type mismatch. Can not convert reference type to different reference type.");

	return *(const ZEArray<ZEItemType>*)Value.Pointer;
}

template<typename ZEObjectType>
const ZEObjectType& ZEVariant::GetObject() const
{
	return ConvertValue<ZEObjectType, ZEMT_BT_OBJECT>(Value.Pointer);
}

template<typename ZEObjectType>
ZEObjectType& ZEVariant::GetObjectRef() const
{
	return ConvertRef<ZEObjectType, ZEMT_BT_OBJECT>(Value.Pointer);
}

template<typename ZEObjectType>
const ZEObjectType& ZEVariant::GetObjectConstRef() const
{
	return ConvertConstRef<const ZEObjectType, ZEMT_BT_OBJECT>(Value.Pointer);
}

template<typename ZEObjectType>
ZEVariant::ZEVariant(const ZEObjectType& Value)
{
	SetObject(Value);
}

template<typename ZEObjectType>
ZEVariant::ZEVariant(ZEObjectType* Value)
{
	SetObjectPtr(Value);
}

template<typename ZEObjectType>
ZEVariant::ZEVariant(const ZEObjectType* Value)
{
	SetObjectPtrConstRef(Value);
}


template<typename ZEArrayType>
ZEVariant::ZEVariant(const ZEArray<ZEArrayType>& Value)
{
	SetArray(Value);
}
