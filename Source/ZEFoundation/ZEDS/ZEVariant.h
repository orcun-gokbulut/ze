//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEVariant.h
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

#pragma once
#ifndef __ZE_VARIANT_H__
#define __ZE_VARIANT_H__

#pragma warning(push)
#pragma warning(disable:4482)

#include "ZETypes.h"
#include "ZEString.h"
#include "ZEArray.h"
#include "ZEList.h"
#include "ZEValue.h"
#include "ZEMeta/ZEType.h"
#include "ZEMeta/ZETypeGenerator.h"

class ZEVector2;
class ZEVector2d;
class ZEVector3;
class ZEVector3d;
class ZEVector4;
class ZEVector4d;
class ZEQuaternion;
class ZEMatrix3x3;
class ZEMatrix3x3d;
class ZEMatrix4x4;
class ZEMatrix4x4d;
class ZEClass;
class ZEObject;
class ZEReference;

class ZEVariant
{
	public:
		struct ZEVariantValue
		{
			union
			{
				// Value Types
				float					Float;
				double					Double;
				ZEInt32					Int32;
				ZEInt64					Int64;
				bool					Boolean;
				float					Vectors[4];
				double					DoubleVectors[4];

				void*					Pointer;
			};
			ZEString					String;
		}Value;

		void*							(*Cloner)(const void*);
		void							(*Deleter)(void*);

		ZEType							ValueType;
		void							SetType(const ZEType& NewType);

		template<typename ZETypeInstance>
		static void*					ClonerTemplate(const void* Instance);
		template<typename ZETypeInstance>
		static void						DeleterTemplate(void* Instance);

		template<typename ZEReturnType>
		ZEReturnType					ConvertValue() const;
		template <typename ZEReturnType, ZETypeType Type>
		ZEReturnType&					ConvertRef() const;
		template <typename ZEReturnType, ZETypeType Type>
		const ZEReturnType&				ConvertConstRef() const;

	public:
		const ZEType&					GetType() const;

		// Setters
		////////////////////////////////////////////////

		void							SetUndefined();
		void							SetVariant(const ZEVariant& Value);
		void							SetReference(const ZEReference& Value);

		void							SetInt8(ZEInt8 Value);
		void							SetInt8Ref(ZEInt8& Reference);
		void							SetInt8ConstRef(const ZEInt8& Reference);

		void							SetInt16(ZEInt16 Value);
		void							SetInt16Ref(ZEInt16& Reference);
		void							SetInt16ConstRef(const ZEInt16& Reference);

		void							SetInt32(ZEInt32 Value);
		void							SetInt32Ref(ZEInt32& Reference);
		void							SetInt32ConstRef(const ZEInt32& Reference);

		void							SetInt64(ZEInt64 Value);
		void							SetInt64Ref(ZEInt64& Reference);
		void							SetInt64ConstRef(const ZEInt64& Reference);

		void							SetUInt8(ZEUInt8 Value);
		void							SetUInt8Ref(ZEUInt8& Reference);
		void							SetUInt8ConstRef(const ZEUInt8& Reference);

		void							SetUInt16(ZEUInt16 Value);
		void							SetUInt16Ref(ZEUInt16& Reference);
		void							SetUInt16ConstRef(const ZEUInt16& Reference);

		void							SetUInt32(ZEUInt32 Value);
		void							SetUInt32Ref(ZEUInt32& Reference);
		void							SetUInt32ConstRef(const ZEUInt32& Reference);

		void							SetUInt64(ZEUInt64 Value);
		void							SetUInt64Ref(ZEUInt64& Reference);
		void							SetUInt64ConstRef(const ZEUInt64& Reference);

		void							SetFloat(float Value);
		void							SetFloatRef(float& Value);
		void							SetFloatConstRef(const float& Value);

		void							SetDouble(double Value);
		void							SetDoubleRef(double& Value);
		void							SetDoubleConstRef(const double & Value);

		void							SetBool(bool Value);
		void							SetBoolRef(bool& Value);
		void							SetBoolConstRef(const bool & Value);

		void							SetEnum(ZEInt32 Value);

		void							SetVector2(const ZEVector2& Value);
		void							SetVector2Ref(ZEVector2& Value);
		void							SetVector2ConstRef(const ZEVector2& Value);

		void							SetVector2d(const ZEVector2d& Value);
		void							SetVector2dRef(ZEVector2d& Value);
		void							SetVector2dConstRef(const ZEVector2d& Value);

		void							SetVector3(const ZEVector3& Value);
		void							SetVector3Ref(ZEVector3& Value);
		void							SetVector3ConstRef(const ZEVector3& Value);

		void							SetVector3d(const ZEVector3d& Value);
		void							SetVector3dRef(ZEVector3d& Value);
		void							SetVector3dConstRef(const ZEVector3d& Value);

		void							SetVector4(const ZEVector4& Value);
		void							SetVector4Ref(ZEVector4& Value);
		void							SetVector4ConstRef(const ZEVector4& Value);

		void							SetVector4d(const ZEVector4d& Value);
		void							SetVector4dRef(ZEVector4d& Value);
		void							SetVector4dConstRef(const ZEVector4d& Value);
		
		void							SetQuaternion(const ZEQuaternion& Value);
		void							SetQuaternionRef(ZEQuaternion& Value);
		void							SetQuaternionConstRef(const ZEQuaternion& Value);

		void							SetMatrix3x3(const ZEMatrix3x3& Value);
		void							SetMatrix3x3Ref(ZEMatrix3x3& Value);
		void							SetMatrix3x3ConstRef(const ZEMatrix3x3& Value);

		void							SetMatrix3x3d(const ZEMatrix3x3d& Value);
		void							SetMatrix3x3dRef(ZEMatrix3x3d& Value);
		void							SetMatrix3x3dConstRef(const ZEMatrix3x3d& Value);

		void							SetMatrix4x4(const ZEMatrix4x4& Value);
		void							SetMatrix4x4Ref(ZEMatrix4x4& Value);
		void							SetMatrix4x4ConstRef(const ZEMatrix4x4& Value);

		void							SetMatrix4x4d(const ZEMatrix4x4d& Value);
		void							SetMatrix4x4dRef(ZEMatrix4x4d& Value);
		void							SetMatrix4x4dConstRef(const ZEMatrix4x4d& Value);

		void							SetString(const char* Value);
		void							SetString(const ZEString& Value);
		void							SetStringRef(ZEString& Value);
		void							SetStringConstRef(const ZEString& Value);

		template<typename ZEObjectType>
		void							SetObject(const ZEObjectType& Object);
		template<typename ZEObjectType>
		void							SetObjectRef(ZEObjectType& Object);
		template<typename ZEObjectType>
		void							SetObjectConstRef(const ZEObjectType& Object);

		void							SetObjectPtr(ZEObject* Object);
		void							SetObjectPtrRef(ZEObject*& Object);
		void							SetObjectPtrConstRef(const ZEObject*& Object);

		template<typename ZEItemType>
		void							SetArray(const ZEArray<ZEItemType>& Array);
		template<typename ZEItemType>
		void							SetArrayRef(ZEArray<ZEItemType>& Array);
		template<typename ZEItemType>
		void							SetArrayRefConst(const ZEArray<ZEItemType>& Array);

		void							SetClass(ZEClass* Object);
		void							SetClassRef(ZEClass*& Object);
		void							SetClassConstRef(const ZEClass*& Object);

		void							SetValue(const ZEValue& Value);

		// Getters
		////////////////////////////////////////////////
		ZEInt8							GetInt8() const;
		ZEInt8&							GetInt8Ref() const;
		const ZEInt8&					GetInt8ConstRef() const;

		ZEInt16							GetInt16() const;
		ZEInt16&						GetInt16Ref() const;
		const ZEInt16&					GetInt16ConstRef() const;

		ZEInt32							GetInt32() const;
		ZEInt32&						GetInt32Ref() const;
		const ZEInt32&					GetInt32ConstRef() const;

		ZEInt64							GetInt64() const;
		ZEInt64&						GetInt64Ref() const;
		const ZEInt64&					GetInt64ConstRef() const;

		ZEUInt8							GetUInt8() const;
		ZEUInt8&						GetUInt8Ref() const;
		const ZEUInt8&					GetUInt8ConstRef() const;

		ZEUInt16						GetUInt16() const;
		ZEUInt16&						GetUInt16Ref() const;
		const ZEUInt16&					GetUInt16ConstRef() const;

		ZEUInt32						GetUInt32() const;
		ZEUInt32&						GetUInt32Ref() const;
		const ZEUInt32&					GetUInt32ConstRef() const;
	
		ZEUInt64						GetUInt64() const;
		ZEUInt64&						GetUInt64Ref() const;
		const ZEUInt64&					GetUInt64ConstRef() const;

		bool							GetBool() const;
		bool&							GetBoolRef() const;
		const bool&						GetBoolConstRef() const;

		ZEInt32							GetEnum() const ;

		float							GetFloat() const;
		float&							GetFloatRef() const;
		const float&					GetFloatConstRef() const;

		double							GetDouble() const;
		double&							GetDoubleRef() const;
		const double&					GetDoubleConstRef() const;

		const ZEVector2&				GetVector2() const;
		ZEVector2&						GetVector2Ref() const;
		const ZEVector2&				GetVector2ConstRef() const;

		const ZEVector2d&				GetVector2d() const;
		ZEVector2d&						GetVector2dRef() const;
		const ZEVector2d&				GetVector2dConstRef() const;

		const ZEVector3&				GetVector3() const;
		ZEVector3&						GetVector3Ref() const;
		const ZEVector3&				GetVector3ConstRef() const;

		const ZEVector3d&				GetVector3d() const;
		ZEVector3d&						GetVector3dRef() const;
		const ZEVector3d&				GetVector3dConstRef() const;

		const ZEVector4&				GetVector4() const;
		ZEVector4&						GetVector4Ref() const;
		const ZEVector4&				GetVector4ConstRef() const;

		const ZEVector4d&				GetVector4d() const;
		ZEVector4d&						GetVector4dRef() const;
		const ZEVector4d&				GetVector4dConstRef() const;
		
		const ZEQuaternion&				GetQuaternion() const;
		ZEQuaternion&					GetQuaternionRef() const;
		const ZEQuaternion&				GetQuaternionConstRef() const;

		const ZEMatrix3x3&				GetMatrix3x3() const;
		ZEMatrix3x3&					GetMatrix3x3Ref() const;
		const ZEMatrix3x3&				GetMatrix3x3ConstRef() const;

		const ZEMatrix3x3d&				GetMatrix3x3d() const;
		ZEMatrix3x3d&					GetMatrix3x3dRef() const;
		const ZEMatrix3x3d&				GetMatrix3x3dConstRef() const;

		const ZEMatrix4x4&				GetMatrix4x4() const;
		ZEMatrix4x4&					GetMatrix4x4Ref() const;
		const ZEMatrix4x4&				GetMatrix4x4ConstRef() const;

		const ZEMatrix4x4d&				GetMatrix4x4d() const;
		ZEMatrix4x4d&					GetMatrix4x4dRef() const;
		const ZEMatrix4x4d&				GetMatrix4x4dConstRef() const;

		const ZEString&					GetString() const;
		ZEString&						GetStringRef() const;
		const ZEString&					GetStringConstRef() const;

		ZEObject*						GetObjectPtr() const;
		ZEObject*&						GetObjectPtrRef() const;
		ZEObject*const&					GetObjectPtrConstRef() const;

		template<typename ZEObjecType>
		const ZEObjecType&				GetObject() const;
		
		template<typename ZEObjecType>
		ZEObjecType&					GetObjectRef() const;

		template<typename ZEObjecType>
		const ZEObjecType&				GetObjectConstRef() const;		

		template<typename ZEItemType>
		const ZEArray<ZEItemType>&		GetArray() const;
		template<typename ZEItemType>
		ZEArray<ZEItemType>&			GetArrayRef() const;
		template<typename ZEItemType>
		const ZEArray<ZEItemType>&		GetArrayConstRef() const;

		ZEClass*						GetClass() const;
		ZEClass*&						GetClassRef() const;
		ZEClass* const& 				GetClassConstRef() const;

		ZEValue							GetValue();

										ZEVariant();
										explicit ZEVariant(const ZEValue& Value);
										explicit ZEVariant(const ZEReference& Reference);
										ZEVariant(const ZEVariant& Value);
										ZEVariant(const char* Value);
										ZEVariant(const ZEString& Value);
										ZEVariant(ZEInt8 Value);
										ZEVariant(ZEInt16 Value);
										ZEVariant(ZEInt32 Value);
										ZEVariant(ZEInt64 Value);
										ZEVariant(ZEUInt8 Value);
										ZEVariant(ZEUInt16 Value);
										ZEVariant(ZEUInt32 Value);
										ZEVariant(ZEUInt64 Value);
										ZEVariant(float Value);
										ZEVariant(double Value);
										ZEVariant(bool Value);
										ZEVariant(const ZEVector2& Value);
										ZEVariant(const ZEVector2d& Value);
										ZEVariant(const ZEVector3& Value);
										ZEVariant(const ZEVector3d& Value);
										ZEVariant(const ZEVector4& Value);
										ZEVariant(const ZEVector4d& Value);
										ZEVariant(const ZEQuaternion& Value);
										ZEVariant(const ZEMatrix3x3& Value);
										ZEVariant(const ZEMatrix3x3d& Value);
										ZEVariant(const ZEMatrix4x4& Value);
										ZEVariant(const ZEMatrix4x4d& Value);
										
										template<typename ZEObjectType>
										ZEVariant(const ZEObjectType& Value);
										ZEVariant(ZEObject* Value);

										template<typename ZEArrayType>
										ZEVariant(const ZEArray<ZEArrayType>& Value);

										ZEVariant(ZEClass* Class);
										~ZEVariant();
};

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
	ZEType Type = ZETypeGenerator<ZEItemType>::GetType();
	if (Type.Type == ZE_TT_UNDEFINED)
		return;

	if (Type.ContainerType == ZE_CT_NONE)
		zeCriticalError("Array to arrays are not supported.");

	if (Type.TypeQualifier != ZE_TQ_VALUE)
		zeCriticalError("Only value or object pointer type arrays supported.");

	Type.TypeQualifier = ZE_TQ_VALUE;
	Type.ContainerType = ZE_CT_ARRAY;

	SetType(Type);
	Cloner = &ClonerTemplate<ZEArray<ZEItemType> >;
	Deleter = &DeleterTemplate<ZEArray<ZEItemType> >;
	Value.Pointer = new ZEArray<ZEItemType>(Array);
}

template<typename ZEItemType>
void ZEVariant::SetArrayRef(ZEArray<ZEItemType>& Array)
{
	ZEType Type = ZETypeGenerator<ZEItemType>::GetType();
	if (Type.Type == ZE_TT_UNDEFINED)
		return;

	if (Type.ContainerType == ZE_CT_NONE)
		zeCriticalError("Array to arrays are not supported.");

	if (Type.TypeQualifier != ZE_TQ_VALUE)
		zeCriticalError("Only value or object pointer type arrays supported.");

	Type.TypeQualifier = ZE_TQ_REFERENCE;
	Type.ContainerType = ZE_CT_ARRAY;

	SetType(Type);
	Value.Pointer = &Array;
}

template<typename ZEItemType>
void ZEVariant::SetArrayRefConst(const ZEArray<ZEItemType>& Array)
{
	ZEType Type = ZETypeGenerator<ZEItemType>::GetType();
	
	if (Type.Type == ZE_TT_UNDEFINED)
		return;
	
	if (Type.ContainerType == ZE_CT_NONE)
		zeCriticalError("Array to arrays are not supported.");

	if (Type.TypeQualifier != ZE_TQ_VALUE)
		zeCriticalError("Only value or object pointer type arrays supported.");

	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	Type.ContainerType = ZE_CT_ARRAY;
	
	SetType(Type);
	Value.Pointer = const_cast<ZEArray<ZEItemType>*>(&Array);
}

template<typename ZEObjectType>
void ZEVariant::SetObject(const ZEObjectType& Object)
{
	ZEType Type;
	Type.Type = ZE_TT_OBJECT;
	Type.TypeQualifier = ZE_TQ_VALUE;
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
	ZEType Type;
	Type.Type = ZE_TT_OBJECT;
	Type.TypeQualifier = ZE_TQ_REFERENCE;
	Type.Class = Object.GetClass();
	SetType(Type);

	Value.Pointer = &Object;
}

template<typename ZEObjectType>
void ZEVariant::SetObjectConstRef(const ZEObjectType& Object)
{
	ZEType Type;
	Type.Type = ZE_TT_OBJECT;
	Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
	Type.Class = Object.GetClass();
	SetType(Type);

	Value.Pointer = const_cast<ZEObjectType*>(&Object);
}

template<typename ZEItemType>
const ZEArray<ZEItemType>& ZEVariant::GetArray() const
{
	ZEType Type = ZETypeGenerator<ZEItemType>::GetType();
	if (ValueType.Type != ZE_TT_ARRAY || ValueType.SubType != Type.SubType)
		zeCriticalError("Variant type mismatch. Can not convert non-numerical non-scaler types.");

	return *(const ZEArray<ZEItemType>*)Value.Pointer;
}

template<typename ZEItemType>
ZEArray<ZEItemType>& ZEVariant::GetArrayRef() const
{
	ZEType Type = ZETypeGenerator<ZEItemType>::GetType();
	if (ValueType.Type = ZE_TT_ARRAY || ValueType.SubTypeType != Type.Type)
		zeCriticalError("Variant type mismatch. Can not convert reference type to different reference type.");

	if (ValueType.SubTypeQualifier == ZE_TQ_REFERENCE)
		return *(ZEReturnType*)Value.Pointer;
	else if (ValueType.SubTypeQualifier == ZE_TQ_CONST_REFERENCE)
		zeCriticalError("Variant is const reference. Can not convert const reference to reference.");

	return *(ZEArray<ZEItemType>*)Value.Pointer;
}

template<typename ZEItemType>
const ZEArray<ZEItemType>& ZEVariant::GetArrayConstRef() const
{
	ZEType Type = ZETypeGenerator<ZEItemType>::GetType();

	if (ValueType.Type = ZE_TT_ARRAY || ValueType.SubTypeType != Type.Type)
		zeCriticalError("Variant type mismatch. Can not convert reference type to different reference type.");

	return *(const ZEArray<ZEItemType>*)Value.Pointer;
}

template<typename ZEObjectType>
const ZEObjectType& ZEVariant::GetObject() const
{
	if (ValueType.Type != ZE_TT_OBJECT)
		zeCriticalError("Value of the variant is not object.");

	if (!ZEClass::IsDerivedFrom(ZEObjectType::Class(), ((ZEObject*)Value.Pointer)->GetClass()))
		zeCriticalError("Value of the variant is not inherited from Object Type.");

	return *(ZEObjectType*)Value.Pointer;
}

template<typename ZEObjectType>
ZEObjectType& ZEVariant::GetObjectRef() const
{
	if (ValueType.Type != ZE_TT_OBJECT)
		zeCriticalError("Value of the variant is not object.");

	if (!ZEClass::IsDerivedFrom(ZEObjectType::Class(), ((ZEObject*)Value.Pointer)->GetClass()))
		zeCriticalError("Value of the variant is not inherited from Object Type.");

	if (ValueType.TypeQualifier == ZE_TQ_CONST_REFERENCE)
		zeCriticalError("Value of the variant is const reference.");

	return *(ZEObjectType*)Value.Pointer;
}

template<typename ZEObjectType>
const ZEObjectType& ZEVariant::GetObjectConstRef() const
{
	if (ValueType.Type != ZE_TT_OBJECT)
		zeCriticalError("Value of the variant is not object.");

	if (!ZEClass::IsDerivedFrom(ZEObjectType::Class(), ((ZEObject*)Value.Pointer)->GetClass()))
		zeCriticalError("Value of the variant is not inherited from Object Type.");

	return *(const ZEObjectType*)Value.Pointer;
}

template<typename ZEObjectType>
ZEVariant::ZEVariant(const ZEObjectType& Value)
{
	SetObject(Value);
}

template<typename ZEArrayType>
ZEVariant::ZEVariant(const ZEArray<ZEArrayType>& Value)
{
	SetArray(Value);
}

#pragma warning(pop)
#endif
