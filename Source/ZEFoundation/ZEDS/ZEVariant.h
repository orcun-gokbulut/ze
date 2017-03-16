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

#pragma warning(push)
#pragma warning(disable:4482)

#ifdef GetObject
	#undef GetObject
#endif

#include "ZETypes.h"
#include "ZEString.h"
#include "ZEArray.h"
#include "ZEList.h"
#include "ZEValue.h"
#include "ZEPointer/ZEHolder.h"
#include "ZEMeta/ZEMTType.h"
#include "ZEMeta/ZEMTTypeGenerator.h"

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
class ZEHolderBase;
class ZEReference;

class ZEVariant
{
	friend class ZEMTCollection;
	private:
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
			ZEHolder<const ZEReferenceCounted> Holder;
			ZEString					String;
		} Value;

		ZEMTType						ValueType;

		void							SetType(const ZEMTType& NewType);

		void							CheckConversionType(ZEMTBaseType TargetType) const;
		void							CheckConversionMutable() const;

		template<typename ZEReturnType>
		ZEReturnType					ConvertValuePrimitive() const;
		template<typename ZEReturnType, ZEMTBaseType Type>
		ZEReturnType&					ConvertValue(const void* ValuePointer) const;
		template<typename ZEReturnType, ZEMTBaseType Type>
		const ZEReturnType&				ConvertConstValue(const void* ValuePointer) const;
		template <typename ZEReturnType, ZEMTBaseType Type>
		ZEReturnType&					ConvertRef(const void* ValuePointer) const;
		template <typename ZEReturnType, ZEMTBaseType Type>
		const ZEReturnType&				ConvertConstRef(const void* ValuePointer) const;

	public:
		const ZEMTType&					GetType() const;

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
		void							SetFloatRef(float& Reference);
		void							SetFloatConstRef(const float& Reference);

		void							SetDouble(double Value);
		void							SetDoubleRef(double& Reference);
		void							SetDoubleConstRef(const double& Reference);

		void							SetBool(bool Value);
		void							SetBoolRef(bool& Reference);
		void							SetBoolConstRef(const bool& Reference);

		void							SetVector2(const ZEVector2& Value);
		void							SetVector2Ref(ZEVector2& Reference);
		void							SetVector2ConstRef(const ZEVector2& Reference);

		void							SetVector2d(const ZEVector2d& Value);
		void							SetVector2dRef(ZEVector2d& Reference);
		void							SetVector2dConstRef(const ZEVector2d& Reference);

		void							SetVector3(const ZEVector3& Value);
		void							SetVector3Ref(ZEVector3& Reference);
		void							SetVector3ConstRef(const ZEVector3& Reference);

		void							SetVector3d(const ZEVector3d& Value);
		void							SetVector3dRef(ZEVector3d& Reference);
		void							SetVector3dConstRef(const ZEVector3d& Reference);

		void							SetVector4(const ZEVector4& Value);
		void							SetVector4Ref(ZEVector4& Reference);
		void							SetVector4ConstRef(const ZEVector4& Reference);

		void							SetVector4d(const ZEVector4d& Value);
		void							SetVector4dRef(ZEVector4d& Reference);
		void							SetVector4dConstRef(const ZEVector4d& Reference);
		
		void							SetQuaternion(const ZEQuaternion& Value);
		void							SetQuaternionRef(ZEQuaternion& Reference);
		void							SetQuaternionConstRef(const ZEQuaternion& Reference);

		void							SetMatrix3x3(const ZEMatrix3x3& Value);
		void							SetMatrix3x3Ref(ZEMatrix3x3& Reference);
		void							SetMatrix3x3ConstRef(const ZEMatrix3x3& Reference);

		void							SetMatrix3x3d(const ZEMatrix3x3d& Value);
		void							SetMatrix3x3dRef(ZEMatrix3x3d& Reference);
		void							SetMatrix3x3dConstRef(const ZEMatrix3x3d& Reference);

		void							SetMatrix4x4(const ZEMatrix4x4& Value);
		void							SetMatrix4x4Ref(ZEMatrix4x4& Reference);
		void							SetMatrix4x4ConstRef(const ZEMatrix4x4& Reference);

		void							SetMatrix4x4d(const ZEMatrix4x4d& Value);
		void							SetMatrix4x4dRef(ZEMatrix4x4d& Reference);
		void							SetMatrix4x4dConstRef(const ZEMatrix4x4d& Reference);

		void							SetString(const ZEString& Value);
		void							SetStringRef(ZEString& Reference);
		void							SetStringConstRef(const ZEString& Reference);

		void							SetObject(ZEObject& Object);
		void							SetObjectRef(ZEObject& Reference);
		void							SetObjectConstRef(const ZEObject& Reference);

		void							SetObjectPtr(ZEObject* Object);
		void							SetObjectPtrConst(const ZEObject* Reference);
		void							SetObjectPtrRef(ZEObject*& Reference);
		void							SetObjectPtrConstRef(const ZEObject*& Reference);

		void							SetObjectHolder(ZEHolderBase& Object);
		void							SetObjectHolderRef(ZEHolderBase& Reference);

		void							SetClass(ZEClass* Object);
		void							SetClassRef(ZEClass*& Reference);

		void							SetCollection(const ZEMTCollection& Collection);
		void							SetCollectionRef(ZEMTCollection& Reference);
		void							SetCollectionConstRef(const ZEMTCollection& Reference);

		void							SetValue(const ZEValue& Value);

		// Getters
		////////////////////////////////////////////////
		bool							IsUndefined() const;

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

		ZEInt32							GetEnum() const;

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

		ZEString						GetString() const;
		ZEString&						GetStringRef() const;
		const ZEString&					GetStringConstRef() const;

		const ZEObject&					GetObject() const;
		ZEObject&						GetObjectRef() const;
		const ZEObject&					GetObjectConstRef() const;		

		ZEObject*						GetObjectPtr() const;
		const ZEObject*					GetObjectPtrConst() const;
		ZEObject*&						GetObjectPtrRef() const;
		const ZEObject*&				GetObjectPtrConstRef() const;

		ZEHolderBase&					GetObjectHolder() const;
		ZEHolderBase&					GetObjectHolderRef() const;

		const ZEMTCollection&			GetCollection() const;
		ZEMTCollection&					GetCollectionRef() const;
		const ZEMTCollection&			GetCollectionConstRef() const;

		ZEClass*						GetClass() const;
		ZEClass*&						GetClassRef() const;

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
										ZEVariant(ZEClass* Class);
										~ZEVariant();
};

#pragma warning(pop)
