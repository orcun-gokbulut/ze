//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEClass.h
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

#include "ZEMTDeclaration.h"

#include "ZEMTAttribute.h"
#include "ZEMTProperty.h"
#include "ZEMTMethod.h"
#include "ZETypes.h"
#include "ZEGUID.h"
#include "ZEEnumerator.h"
#include "ZEDS/ZEFlags.h"

#define ZEMT_CLASS_DEFINITION(ClassName) \
	class ClassName##Class : public ZEClass \
	{ \
		public: \
			virtual const char*				GetName() const; \
			virtual const ZEGUID&			GetGUID() const; \
			virtual const ZEMTAttribute*	GetAttributes() const; \
			virtual ZESize					GetAttributeCount() const; \
			virtual ZEClass*				GetParentClass() const; \
			virtual ZEMTClassFlags			GetFlags() const; \
			virtual const ZEMTProperty*		GetProperties() const; \
			virtual ZESize					GetPropertyCount() const; \
			virtual const ZEMTMethod*		GetMethods() const; \
			virtual ZESize					GetMethodCount() const; \
			virtual ZESize					GetPropertyId(const ZEString& PropertyName) const; \
			virtual ZESize					GetMethodId(const ZEString& MethodName, ZESize OverloadIndex = 0) const; \
			virtual bool					SetProperty(ZEObject* Object, ZESize PropertyId, const ZEVariant& Value) const; \
			virtual bool					SetPropertyConst(const ZEObject* Object, ZESize PropertyId, const ZEVariant& Value) const; \
			virtual bool					GetProperty(ZEObject* Object, ZESize PropertyId, ZEVariant& Value) const; \
			virtual bool					GetPropertyConst(const ZEObject* Object, ZESize PropertyId, ZEVariant& Value) const; \
			virtual bool					CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference** Parameters, ZESize ParameterCount) const; \
			virtual bool					CallMethodConst(const ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference** Parameters, ZESize ParameterCount) const; \
			virtual bool					AddEventDelegate(ZEObject* Object, ZESize EventId, ZEEventDelegateBase* Delegate) const; \
			virtual bool					RemoveEventDelegate(ZEObject* Object, ZESize EventId, ZEEventDelegateBase* Delegate) const; \
			virtual ZESize					GetSizeOfObject() const; \
			virtual ZEObject*				CreateInstance() const; \
			virtual bool					Destroy(ZEObject* Object) const; \
			virtual ZEObject*				DynamicCast(ZEObject* Object) const; \
			virtual ZEObject*				Clone(ZEObject* Object) const; \
			virtual bool					Construct(ZEObject* Object) const; \
			virtual bool					Deconstruct(ZEObject* Object) const; \
			virtual bool					Assign(ZEObject* Object, ZEObject* Source) const; \
			virtual ZEObject*				CreateScriptInstance() const; \
			virtual ZESize					GetSizeOfScriptObject() const; \
	};

class ZEObject;
class ZEEventDelegateBase;
class ZEVariant;
class ZEReference;
class ZEScriptObject;
class ZEMLReaderNode;
class ZEMLWriterNode;
class ZEReferenceCounted;

enum ZEMTClassFlag
{
	ZEMT_CF_NONE				= 0x0000,
	ZEMT_CF_FUNDAMENTAL			= 0x0001,
	ZEMT_CF_ABSTRACT			= 0x0002,
	ZEMT_CF_SCRIPT				= 0x0004, 
	ZEMT_CF_CREATABLE			= 0x0008, 
	ZEMT_CF_DESTROYABLE			= 0x0010,
	ZEMT_CF_ASSIGNABLE			= 0x0020,
	ZEMT_CF_COPYABLE			= 0x0040,
	ZEMT_CF_CLONEABLE			= 0x0040,
	ZEMT_CF_CONSTRUCTIBLE		= 0x0080,
	ZEMT_CF_DECONSTRUCTIBLE		= 0x0100
};
typedef ZEInt ZEMTClassFlags;


#define ZEMT_CF_VALUE_OBJECT (ZEMT_CF_CONSTRUCT | ZEMT_CF_DECONSTRUCT)

struct ZEClassSortedData
{
	ZESize			Hash;
	ZESize			ID;
	const char*		Name;
};

class ZEClass : public ZEMTDeclaration
{
	protected:
		static ZESSize					Search(ZEClassSortedData* Data, ZESize DataSize, const ZEString& Name);

	public:
		virtual ZEMTDeclarationType		GetDeclarationType() const;
		virtual const char*				GetName() const;
		virtual const ZEGUID&			GetGUID() const;
		virtual const ZEMTAttribute*	GetAttributes() const;
		virtual ZESize					GetAttributeCount() const;
		virtual ZEClass*				GetParentClass() const;

		virtual ZEMTClassFlags			GetFlags() const;
		bool							IsFundamental() const;
		bool							IsAbstract() const;
		bool							IsScript() const;
		bool							IsCreatable() const;
		bool							IsDestroyable() const;
		bool							IsAssignable() const;
		bool							IsCopyable() const;
		bool							IsCloneable() const;
		bool							IsConstructible() const;
		bool							IsDeconstructible() const;

		virtual const ZEMTProperty*		GetProperties() const;
		virtual ZESize					GetPropertyCount() const;
		const ZEMTProperty*				GetPropertyDescription(ZESize PropertyId) const;
		const ZEMTProperty*				GetPropertyDescription(const ZEString& PropertyName) const;

		virtual const ZEMTMethod*		GetMethods() const;
		virtual ZESize					GetMethodCount() const;

		virtual ZESize					GetPropertyId(const ZEString& PropertyName) const;
		virtual ZESize					GetMethodId(const ZEString& MethodName, ZESize OverloadIndex = 0) const;

		virtual bool					SetProperty(ZEObject* Object, ZESize PropertyId, const ZEVariant& Value) const;
		bool							SetProperty(ZEObject* Object, const ZEString& PropertyName, const ZEVariant& Value) const;

		virtual bool					SetPropertyConst(const ZEObject* Object, ZESize PropertyId, const ZEVariant& Value) const;
		bool							SetPropertyConst(const ZEObject* Object, const ZEString& PropertyName, const ZEVariant& Value) const;

		virtual bool					GetProperty(ZEObject* Object, ZESize PropertyId, ZEVariant& Value) const;
		bool							GetProperty(ZEObject* Object, const ZEString& PropertyName, ZEVariant& Value) const;

		virtual bool					GetPropertyConst(const ZEObject* Object, ZESize PropertyId, ZEVariant& Value) const;
		bool							GetPropertyConst(const ZEObject* Object, const ZEString& PropertyName, ZEVariant& Value) const;

		virtual bool					CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference** Parameters, ZESize ParameterCount) const;
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference** Parameters, ZESize ParameterCount) const;
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue) const;
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0) const;
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1) const;
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2) const;
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3) const;
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4) const;
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5) const;
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6) const;
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7) const;
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8) const;
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9) const;
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10) const;
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11) const;
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12) const;
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13) const;
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14) const;
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15) const;
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16) const;
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16, const ZEReference& Parameters17) const;
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16, const ZEReference& Parameters17, const ZEReference& Parameters18) const;
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16, const ZEReference& Parameters17, const ZEReference& Parameters18, const ZEReference& Parameters19) const;
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16, const ZEReference& Parameters17, const ZEReference& Parameters18, const ZEReference& Parameters19, const ZEReference& Parameters20) const;
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16, const ZEReference& Parameters17, const ZEReference& Parameters18, const ZEReference& Parameters19, const ZEReference& Parameters20, const ZEReference& Parameters21) const;
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16, const ZEReference& Parameters17, const ZEReference& Parameters18, const ZEReference& Parameters19, const ZEReference& Parameters20, const ZEReference& Parameters21, const ZEReference& Parameters22) const;
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16, const ZEReference& Parameters17, const ZEReference& Parameters18, const ZEReference& Parameters19, const ZEReference& Parameters20, const ZEReference& Parameters21, const ZEReference& Parameters22, const ZEReference& Parameters23) const;
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16, const ZEReference& Parameters17, const ZEReference& Parameters18, const ZEReference& Parameters19, const ZEReference& Parameters20, const ZEReference& Parameters21, const ZEReference& Parameters22, const ZEReference& Parameters23, const ZEReference& Parameters24) const;

		virtual bool					CallMethodConst(const ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference** Parameters, ZESize ParameterCount) const;
		bool							CallMethodConst(const ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference** Parameters, ZESize ParameterCount) const;
		bool							CallMethodConst(const ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue) const;
		bool							CallMethodConst(const ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0) const;
		bool							CallMethodConst(const ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1) const;
		bool							CallMethodConst(const ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2) const;
		bool							CallMethodConst(const ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3) const;
		bool							CallMethodConst(const ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4) const;
		bool							CallMethodConst(const ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5) const;
		bool							CallMethodConst(const ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6) const;
		bool							CallMethodConst(const ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7) const;
		bool							CallMethodConst(const ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8) const;
		bool							CallMethodConst(const ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9) const;
		bool							CallMethodConst(const ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10) const;
		bool							CallMethodConst(const ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11) const;
		bool							CallMethodConst(const ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12) const;
		bool							CallMethodConst(const ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13) const;
		bool							CallMethodConst(const ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14) const;
		bool							CallMethodConst(const ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15) const;
		bool							CallMethodConst(const ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16) const;
		bool							CallMethodConst(const ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16, const ZEReference& Parameters17) const;
		bool							CallMethodConst(const ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16, const ZEReference& Parameters17, const ZEReference& Parameters18) const;
		bool							CallMethodConst(const ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16, const ZEReference& Parameters17, const ZEReference& Parameters18, const ZEReference& Parameters19) const;
		bool							CallMethodConst(const ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16, const ZEReference& Parameters17, const ZEReference& Parameters18, const ZEReference& Parameters19, const ZEReference& Parameters20) const;
		bool							CallMethodConst(const ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16, const ZEReference& Parameters17, const ZEReference& Parameters18, const ZEReference& Parameters19, const ZEReference& Parameters20, const ZEReference& Parameters21) const;
		bool							CallMethodConst(const ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16, const ZEReference& Parameters17, const ZEReference& Parameters18, const ZEReference& Parameters19, const ZEReference& Parameters20, const ZEReference& Parameters21, const ZEReference& Parameters22) const;
		bool							CallMethodConst(const ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16, const ZEReference& Parameters17, const ZEReference& Parameters18, const ZEReference& Parameters19, const ZEReference& Parameters20, const ZEReference& Parameters21, const ZEReference& Parameters22, const ZEReference& Parameters23) const;
		bool							CallMethodConst(const ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16, const ZEReference& Parameters17, const ZEReference& Parameters18, const ZEReference& Parameters19, const ZEReference& Parameters20, const ZEReference& Parameters21, const ZEReference& Parameters22, const ZEReference& Parameters23, const ZEReference& Parameters24) const;

		virtual bool					AddEventDelegate(ZEObject* Target, ZESize EventId, ZEEventDelegateBase* Delegate) const;
		bool							AddEventDelegate(ZEObject* Target, const ZEString& EventName, ZEEventDelegateBase* Delegate) const;
		virtual bool					RemoveEventDelegate(ZEObject* Target, ZESize EventId, ZEEventDelegateBase* Delegate) const;
		bool							RemoveEventDelegate(ZEObject* Target, const ZEString& EventName, ZEEventDelegateBase* Delegate) const;

		// Life Cycle Management
		virtual ZESize					GetSizeOfObject() const;

		virtual ZEObject*				CreateInstance() const;
		virtual ZEObject*				Clone(ZEObject* Object) const;
		virtual bool					Destroy(ZEObject* Object) const;
		virtual bool					Construct(ZEObject* Object) const;
		virtual bool					Deconstruct(ZEObject* Object) const;
		virtual bool					Assign(ZEObject* Destination, const ZEObject* Source) const;
		virtual bool					Copy(ZEObject* Destination, const ZEObject* Source) const;
		virtual bool					Equals(ZEObject* A, const ZEObject* B) const;
		virtual bool					NotEquals(ZEObject* A, const ZEObject* B) const;
		virtual ZEObject*				DynamicCast(ZEObject* Object) const;

		// Scripting
		virtual ZEObject*				CreateScriptInstance() const;
		virtual ZESize					GetSizeOfScriptObject() const;

		// Serialization & Unserialization
		bool							Serialize(ZEObject* Object, ZEMLWriterNode& ObjectNode) const;
		bool							SerializeProperties(ZEObject* Object, ZEMLWriterNode& PropertiesNode) const;
		
		bool							Unserialize(ZEObject* Object, const ZEMLReaderNode& ObjectNode) const;
		bool							UnserializeProperties(ZEObject* Object, const ZEMLReaderNode& PropertiesNode) const;

		static bool						IsDerivedFrom(ZEClass* ParentClass, ZEClass* Class);
		static bool						IsDerivedFrom(ZEClass* ParentClass, ZEObject* Object);
		template<typename ZECastedObjectType>
		static ZECastedObjectType*		Cast(ZEObject* Object);
};

template<typename ZECastedObjectType>
ZECastedObjectType* ZEClass::Cast(ZEObject* Object)
{
	if (ZEClass::IsDerivedFrom(ZECastedObjectType::Class(), Object))
		return static_cast<ZECastedObjectType*>(Object);
	
	return NULL;
}
