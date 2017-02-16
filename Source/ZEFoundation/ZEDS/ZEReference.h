//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEReference.h
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
#ifndef __ZE_REFERENCE_H__
#define __ZE_REFERENCE_H__

#pragma warning(push)
#pragma warning(disable:4482)

#include "ZETypes.h"
#include "ZEDS/ZEString.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEList.h"
#include "ZEMeta/ZEMTType.h"


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
class ZEObject;
class ZEString;

#define ZE_TEMPLATE template<typename ZEType>

class ZEReference
{
	friend class ZEVariant;
	private:
		ZEMTType							ValueType;
		struct
		{
			void*						Pointer;
		} Value;

		template <typename ZEReturnType, ZEMTTypeType Type>
		ZEReturnType&					ConvertRef() const;
		template <typename ZEReturnType, ZEMTTypeType Type>
		const ZEReturnType&				ConvertConstRef() const;

		void							SetType(const ZEMTType& Type);
	public:
		ZEMTType							GetType() const;
		bool							IsNull() const;
		bool							IsConst() const;

		void							SetNull();
		void							SetReference(const ZEReference& Value);

		void							SetInt8Ref(ZEInt8& Value);
		void							SetInt8ConstRef(const ZEInt8& Value);
		void							SetInt16Ref(ZEInt16& Value);
		void							SetInt16ConstRef(const ZEInt16& Value);
		void							SetInt32Ref(ZEInt32& Value);
		void							SetInt32ConstRef(const ZEInt32& Value);
		void							SetInt64Ref(ZEInt64& Value);
		void							SetInt64ConstRef(const ZEInt64& Value);

		void							SetUInt8Ref(ZEUInt8& Value);
		void							SetUInt8ConstRef(const ZEUInt8& Value);
		void							SetUInt16Ref(ZEUInt16& Value);
		void							SetUInt16ConstRef(const ZEUInt16& Value);
		void							SetUInt32Ref(ZEUInt32& Value);
		void							SetUInt32ConstRef(const ZEUInt32& Value);
		void							SetUInt64Ref(ZEUInt64& Value);
		void							SetUInt64ConstRef(const ZEUInt64& Value);
		
		void							SetBooleanRef(bool& Value);
		void							SetBoolConstRef(const bool& Value);

		void							SetFloatRef(float& Value);
		void							SetFloatConstRef(const float& Value);
		void							SetDoubleRef(double& Value);
		void							SetDoubleConstRef(const double& Value);

		void							SetVector2Ref(ZEVector2& Reference);
		void							SetVector2ConstRef(const ZEVector2& Reference);
		void							SetVector2dRef(ZEVector2d& Reference);
		void							SetVector2dConstRef(const ZEVector2d& Reference);
		void							SetVector3Ref(ZEVector3& Reference);
		void							SetVector3ConstRef(const ZEVector3& Reference);
		void							SetVector3dRef(ZEVector3d& Reference);
		void							SetVector3dConstRef(const ZEVector3d& Reference);
		void							SetVector4Ref(ZEVector4& Reference);
		void							SetVector4ConstRef(const ZEVector4& Reference);
		void							SetVector4dRef(ZEVector4d& Reference);
		void							SetVector4dConstRef(const ZEVector4d& Reference);
		void							SetQuaternionRef(ZEQuaternion& Reference);
		void							SetQuaternionConstRef(const ZEQuaternion& Reference);

		void							SetMatrix3x3Ref(ZEMatrix3x3& Reference);
		void							SetMatrix3x3ConstRef(const ZEMatrix3x3& Reference);
		void							SetMatrix3x3dRef(ZEMatrix3x3d& Reference);
		void							SetMatrix3x3dConstRef(const ZEMatrix3x3d& Reference);
		void							SetMatrix4x4Ref(ZEMatrix4x4& Reference);
		void							SetMatrix4x4ConstRef(const ZEMatrix4x4& Reference);
		void							SetMatrix4x4dRef(ZEMatrix4x4d& Reference);
		void							SetMatrix4x4dConstRef(const ZEMatrix4x4d& Reference);

		void							SetStringRef(ZEString& Value);
		void							SetStringConstRef(const ZEString& Value);

		void							SetEnumRef(const ZEInt32& Reference);

		template<typename ZEObjectType>
		void							SetObjectRef(ZEObjectType& Object);
		template<typename ZEObjectType>
		void							SetObjectConstRef(const ZEObjectType& Object);

		void							SetObjectPtrRef(ZEObject*& Object);
		void							SetObjectPtrConstRef(const ZEObject*& Object);

		void							SetClassRef(ZEClass*& Class);
		void							SetClassConstRef(const ZEClass*& Class);

		template<typename ZEItemType>
		void							SetArrayRef(ZEArray<ZEItemType>& Array);
		template<typename ZEItemType>
		void							SetArrayConstRef(const ZEArray<ZEItemType>& Array);

		ZEInt8&							GetInt8Ref() const;
		const ZEInt8&					GetInt8ConstRef() const;
		ZEInt16&						GetInt16Ref() const;
		const ZEInt16&					GetInt16ConstRef() const;
		ZEInt32&						GetInt32Ref() const;
		const ZEInt32&					GetInt32ConstRef() const;
		ZEInt64&						GetInt64Ref() const;
		const ZEInt64&					GetInt64ConstRef() const;

		ZEUInt8&						GetUInt8Ref() const;
		const ZEUInt8&					GetUInt8ConstRef() const;
		ZEUInt16&						GetUInt16Ref() const;
		const ZEUInt16&					GetUInt16ConstRef() const;
		ZEUInt32&						GetUInt32Ref() const;
		const ZEUInt32&					GetUInt32ConstRef() const;
		ZEUInt64&						GetUInt64Ref() const;
		const ZEUInt64&					GetUInt64ConstRef() const;
		
		float&							GetFloatRef() const;
		const float&					GetFloatConstRef() const;

		double&							GetDoubleRef() const;
		const double&					GetDoubleConstRef() const;

		bool&							GetBoolRef() const;
		const bool&						GetBoolConstRef() const;

		ZEVector2&						GetVector2Ref() const;
		const ZEVector2&				GetVector2ConstRef() const;
		ZEVector2d&						GetVector2dRef() const;
		const ZEVector2d&				GetVector2dConstRef() const;
		ZEVector3&						GetVector3Ref() const;
		const ZEVector3&				GetVector3ConstRef() const;
		ZEVector3d&						GetVector3dRef() const;
		const ZEVector3d&				GetVector3dConstRef() const;
		ZEVector4&						GetVector4Ref() const;
		const ZEVector4&				GetVector4ConstRef() const;
		ZEVector4d&						GetVector4dRef() const;
		const ZEVector4d&				GetVector4dConstRef() const;
		ZEQuaternion&					GetQuaternionRef() const;
		const ZEQuaternion&				GetQuaternionConstRef() const;

		ZEMatrix3x3&					GetMatrix3x3Ref() const;
		const ZEMatrix3x3&				GetMatrix3x3ConstRef() const;
		ZEMatrix3x3d&					GetMatrix3x3dRef() const;
		const ZEMatrix3x3d&				GetMatrix3x3dConstRef() const;
		ZEMatrix4x4&					GetMatrix4x4Ref() const;
		const ZEMatrix4x4&				GetMatrix4x4ConstRef() const;
		ZEMatrix4x4d&					GetMatrix4x4dRef() const;
		const ZEMatrix4x4d&				GetMatrix4x4dConstRef() const;

		ZEString&						GetStringRef() const;
		const ZEString&					GetStringConstRef() const;

		ZEObject*&						GetObjectPtrRef() const;
		ZEObject*const&					GetObjectPtrConstRef() const;

		ZEClass*&						GetClassRef() const;
		ZEClass* const& 				GetClassConstRef() const;

		template<typename ZEObjecType>
		ZEObjecType&					GetObjectRef() const;
		template<typename ZEObjecType>
		const ZEObjecType&				GetObjectConstRef() const;		

		template<typename ZEItemType>
		ZEArray<ZEItemType>&			GetArrayRef() const;
		template<typename ZEItemType>
		const ZEArray<ZEItemType>&		GetArrayConstRef() const;
		

		ZEReference&					operator=(const ZEReference& Value);

		ZEReference&					operator=(ZEInt8& Value);
		ZEReference&					operator=(ZEInt16& Value);
		ZEReference&					operator=(ZEInt32& Value);
		ZEReference&					operator=(ZEInt64& Value);
		ZEReference&					operator=(ZEUInt8& Value);
		ZEReference&					operator=(ZEUInt16& Value);
		ZEReference&					operator=(ZEUInt32& Value);
		ZEReference&					operator=(ZEUInt64& Value);
		ZEReference&					operator=(float& Value);
		ZEReference&					operator=(double& Value);
		ZEReference&					operator=(bool& Value);
		ZEReference&					operator=(ZEVector2& Value);
		ZEReference&					operator=(ZEVector2d& Value);
		ZEReference&					operator=(ZEVector3& Value);
		ZEReference&					operator=(ZEVector3d& Value);
		ZEReference&					operator=(ZEVector4& Value);
		ZEReference&					operator=(ZEVector4d& Value);
		ZEReference&					operator=(ZEQuaternion& Value);
		ZEReference&					operator=(ZEMatrix3x3& Value);
		ZEReference&					operator=(ZEMatrix3x3d& Value);
		ZEReference&					operator=(ZEMatrix4x4& Value);
		ZEReference&					operator=(ZEMatrix4x4d& Value);
		ZEReference&					operator=(ZEObject*& Value);
		ZEReference&					operator=(ZEString& Value);

		ZEReference&					operator=(const ZEInt8& Value);
		ZEReference&					operator=(const ZEInt16& Value);
		ZEReference&					operator=(const ZEInt32& Value);
		ZEReference&					operator=(const ZEInt64& Value);
		ZEReference&					operator=(const ZEUInt8& Value);
		ZEReference&					operator=(const ZEUInt16& Value);
		ZEReference&					operator=(const ZEUInt32& Value);
		ZEReference&					operator=(const ZEUInt64& Value);
		ZEReference&					operator=(const float& Value);
		ZEReference&					operator=(const double& Value);
		ZEReference&					operator=(const bool& Value);
		ZEReference&					operator=(const ZEVector2& Value);
		ZEReference&					operator=(const ZEVector2d& Value);
		ZEReference&					operator=(const ZEVector3& Value);
		ZEReference&					operator=(const ZEVector3d& Value);
		ZEReference&					operator=(const ZEVector4& Value);
		ZEReference&					operator=(const ZEVector4d& Value);
		ZEReference&					operator=(const ZEQuaternion& Value);
		ZEReference&					operator=(const ZEMatrix3x3& Value);
		ZEReference&					operator=(const ZEMatrix3x3d& Value);
		ZEReference&					operator=(const ZEMatrix4x4& Value);
		ZEReference&					operator=(const ZEMatrix4x4d& Value);
		ZEReference&					operator=(const ZEObject*& Value);
		ZEReference&					operator=(const ZEString& Value);

										operator ZEInt8&();
										operator ZEInt16&();
										operator ZEInt32&();
										operator ZEInt64&();
										operator ZEUInt8&();
										operator ZEUInt16&();
										operator ZEUInt32&();
										operator ZEUInt64&();
										operator float&();
										operator double&();
										operator bool&();
										operator ZEVector2&();
										operator ZEVector2d&();
										operator ZEVector3&();
										operator ZEVector3d&();
										operator ZEVector4&();
										operator ZEVector4d&();
										operator ZEQuaternion&();
										operator ZEMatrix3x3&();
										operator ZEMatrix3x3d&();
										operator ZEMatrix4x4&();
										operator ZEMatrix4x4d&();
										operator ZEObject*&();

										operator const ZEInt8&();
										operator const ZEInt16&();
										operator const ZEInt32&();
										operator const ZEInt64&();
										operator const ZEUInt8&();
										operator const ZEUInt16&();
										operator const ZEUInt32&();
										operator const ZEUInt64&();
										operator const float&();
										operator const double&();
										operator const bool&();
										operator const ZEVector2&();
										operator const ZEVector2d&();
										operator const ZEVector3&();
										operator const ZEVector3d&();
										operator const ZEVector4&();
										operator const ZEVector4d&();
										operator const ZEQuaternion&();
										operator const ZEMatrix3x3&();
										operator const ZEMatrix3x3d&();
										operator const ZEMatrix4x4&();
										operator const ZEMatrix4x4d&();
										operator ZEObject* const&();

										ZEReference();
										ZEReference(const ZEReference& Value);

										ZEReference(ZEInt8& Value);
										ZEReference(ZEInt16& Value);
										ZEReference(ZEInt32& Value);
										ZEReference(ZEInt64& Value);
										ZEReference(ZEUInt8& Value);
										ZEReference(ZEUInt16& Value);
										ZEReference(ZEUInt32& Value);
										ZEReference(ZEUInt64& Value);
										ZEReference(float& Value);
										ZEReference(double& Value);
										ZEReference(bool& Value);
										ZEReference(ZEVector2& Value);
										ZEReference(ZEVector2d& Value);
										ZEReference(ZEVector3& Value);
										ZEReference(ZEVector3d& Value);
										ZEReference(ZEVector4& Value);
										ZEReference(ZEVector4d& Value);
										ZEReference(ZEQuaternion& Value);
										ZEReference(ZEMatrix3x3& Value);
										ZEReference(ZEMatrix3x3d& Value);
										ZEReference(ZEMatrix4x4& Value);
										ZEReference(ZEMatrix4x4d& Value);
										ZEReference(ZEObject*& Value);
										ZEReference(ZEString& Value);

										ZEReference(const ZEInt8& Value);
										ZEReference(const ZEInt16& Value);
										ZEReference(const ZEInt32& Value);
										ZEReference(const ZEInt64& Value);
										ZEReference(const ZEUInt8& Value);
										ZEReference(const ZEUInt16& Value);
										explicit ZEReference(const ZEUInt32& Value);
										explicit ZEReference(const ZEUInt64& Value);
										ZEReference(const float& Value);
										ZEReference(const double& Value);
										ZEReference(const bool& Value);
										ZEReference(const ZEVector2& Value);
										ZEReference(const ZEVector2d& Value);
										ZEReference(const ZEVector3& Value);
										ZEReference(const ZEVector3d& Value);
										ZEReference(const ZEVector4& Value);
										ZEReference(const ZEVector4d& Value);
										ZEReference(const ZEQuaternion& Value);
										ZEReference(const ZEMatrix3x3& Value);
										ZEReference(const ZEMatrix3x3d& Value);
										ZEReference(const ZEMatrix4x4& Value);
										ZEReference(const ZEMatrix4x4d& Value);
										ZEReference(const ZEObject*& Value);
										ZEReference(const ZEString& Value);

										~ZEReference();
};


template<typename ZEItemType>
void ZEReference::SetArrayRef(ZEArray<ZEItemType>& Array)
{
	ZEMTType Type = ZEMTTypeGenerator<ZEItemType>::GetType();
	if (Type.Type == ZEMT_TT_UNDEFINED)
		return;

	Type.SubType = Type.Type;
	Type.SubTypeQualifier = Type.TypeQualifier;
	Type.Type = ZE_TT_ARRAY;
	Type.TypeQualifier = ZEMT_TQ_REFERENCE;

	SetType(Type);
	Value.Pointer = &Array;
}

template<typename ZEItemType>
void ZEReference::SetArrayConstRef(const ZEArray<ZEItemType>& Array)
{
	ZEMTType Type = ZEMTTypeGenerator<ZEItemType>::GetType();
	if (Type.Type == ZEMT_TT_UNDEFINED)
		return;

	Type.SubType = Type.Type;
	Type.SubTypeQualifier = Type.TypeQualifier;
	Type.Type = ZE_TT_ARRAY;
	Type.TypeQualifier = ZEMT_TQ_REFERENCE;

	SetType(Type);
	Value.Pointer = const_cast<ZEArray<ZEItemType>*>(&Array);
}

template<typename ZEObjectType>
void ZEReference::SetObjectRef(ZEObjectType& Object)
{
	ZEMTType Type;
	Type.Type = ZEMT_TT_OBJECT;
	Type.Class = (static_cast<ZEObject*>(Object))->GetClass();
	SetType(Type);

	Value.Pointer = &Object;
}

template<typename ZEObjectType>
void ZEReference::SetObjectConstRef(const ZEObjectType& Object)
{
	ZEMTType Type;
	Type.Type = ZEMT_TT_OBJECT;
	Type.Class = (static_cast<ZEObject*>(Object))->GetClass();
	SetType(Type);

	Value.Pointer = const_cast<ZEObjectType*>(&Object);
}

template<typename ZEItemType>
ZEArray<ZEItemType>& ZEReference::GetArrayRef() const
{
	ZEMTType Type = ZEMTTypeGenerator<ZEItemType>::GetType();
	if (ValueType.ContainerType != ZEMT_CT_ARRAY || ValueType.Type != Type.Type)
		zeCriticalError("Variant type mismatch. Can not convert reference type to different reference type.");

	if (ValueType.TypeQualifier == ZEMT_TQ_CONST_REFERENCE)
		zeCriticalError("Variant is const reference. Can not convert const reference to reference.");

	return *(ZEArray<ZEItemType>*)Value.Pointer;
}

template<typename ZEItemType>
const ZEArray<ZEItemType>& ZEReference::GetArrayConstRef() const
{
	ZEMTType Type = ZEMTTypeGenerator<ZEItemType>::GetType();
	if (ValueType.ContainerType != ZEMT_CT_ARRAY || ValueType.Type != Type.Type)
		zeCriticalError("Variant type mismatch. Can not convert reference type to different reference type.");

	return *(const ZEArray<ZEItemType>*)Value.Pointer;
}

template<typename ZEObjectType>
ZEObjectType& ZEReference::GetObjectRef() const
{
	if (ValueType.Type != ZEMT_TT_OBJECT)
		zeCriticalError("Value of the variant is not object.");

	if (!ZEClass::IsDerivedFrom(ZEObjectType::Class(), ((ZEObject*)Value.Pointer)->GetClass()))
		zeCriticalError("Value of the variant is not inherited from Object Type.");

	if (ValueType.TypeQualifier == ZEMT_TQ_CONST_REFERENCE)
		zeCriticalError("Value of the variant is const reference.");

	return *(ZEObjectType*)Value.Pointer;
}

template<typename ZEObjectType>
const ZEObjectType& ZEReference::GetObjectConstRef() const
{
	if (ValueType.Type != ZEMT_TT_OBJECT)
		zeCriticalError("Value of the variant is not object.");

	if (!ZEClass::IsDerivedFrom(ZEObjectType::Class(), ((ZEObjectType*)Value.Pointer)->GetClass()))
		zeCriticalError("Value of the variant is not inherited from Object Type.");

	return *(const ZEObjectType*)Value.Pointer;
}

#pragma warning(pop)
#endif
