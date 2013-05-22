//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEObject.h
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
#ifndef __ZE_OBJECT_H__
#define __ZE_OBJECT_H__

#include "ZEMacro/ZEMacro.h"
#include "ZEClass.h"

#ifdef ZE_META_COMPILER
	#define ZE_META_ATTRIBUTE_INTERNAL(Value) __attribute__((annotate(Value)))
	#define ZE_META_ATTRIBUTE_0(Name)											ZE_META_ATTRIBUTE_INTERNAL(#Name)
	#define ZE_META_ATTRIBUTE_1(Name, Parameter0)								ZE_META_ATTRIBUTE_INTERNAL(#Name "," #Parameter0)
	#define ZE_META_ATTRIBUTE_2(Name, Parameter0, Parameter1)					ZE_META_ATTRIBUTE_INTERNAL(#Name "," #Parameter0 "," #Parameter1)
	#define ZE_META_ATTRIBUTE_3(Name, Parameter0, Parameter1, Parameter2)		ZE_META_ATTRIBUTE_INTERNAL(#Name "," #Parameter0 "," #Parameter1 "," #Parameter2)
	#define ZE_META_CLASS_ATTRIBUTE_0(Name)										class ZE_META_ATTRIBUTE_INTERNAL("ClassAttribute" "," #Name) {};
	#define ZE_META_CLASS_ATTRIBUTE_1(Name, Parameter0)							class ZE_META_ATTRIBUTE_INTERNAL("ClassAttribute" "," #Name "," #Parameter0) {};
	#define ZE_META_CLASS_ATTRIBUTE_2(Name, Parameter0, Parameter1)				class ZE_META_ATTRIBUTE_INTERNAL("ClassAttribute" "," #Name "," #Parameter0 "," #Parameter1) {};
	#define ZE_META_CLASS_ATTRIBUTE_3(Name, Parameter0, Parameter1, Parameter2)	class ZE_META_ATTRIBUTE_INTERNAL("ClassAttribute" "," #Name "," #Parameter0 "," #Parameter1 "," #Parameter2) {};
	#define ZE_META_FORWARD_DECLARE(ClassName, IncludeFile) \
	class ZE_META_ATTRIBUTE_2("ForwardDeclaration", #ClassName, IncludeFile) ClassName;
#else
	#define ZE_META_ATTRIBUTE_0(Name)
	#define ZE_META_ATTRIBUTE_1(Name, Parameter0)
	#define ZE_META_ATTRIBUTE_2(Name, Parameter0, Parameter1)
	#define ZE_META_ATTRIBUTE_3(Name, Parameter0, Parameter1, Parameter2)
	#define ZE_META_CLASS_ATTRIBUTE_0(Name)
	#define ZE_META_CLASS_ATTRIBUTE_1(Name, Parameter0)
	#define ZE_META_CLASS_ATTRIBUTE_2(Name, Parameter0, Parameter1)
	#define ZE_META_CLASS_ATTRIBUTE_3(Name, Parameter0, Parameter1, Parameter2)
	#define ZE_META_FORWARD_DECLARE(ClassName, IncludeFile) class ClassName;
#endif

#define ZE_OBJECT \
	public: \
		virtual	ZEClass*			GetClass() const; \
		static ZEClass*				Class(); \
	private:
		
#define ZE_OBJECT_IMPL(ClassName) \
	ZEClass* ClassName::GetClass() const \
	{ \
		return ClassName::Class(); \
	} \
	ZEClass* ClassName::Class() \
	{ \
		return ClassName##Class::Class(); \
	}

class ZEObject
{
	public:
		virtual	ZEClass*			GetClass() const;
		static ZEClass*				Class();
};

#define ZE_CLASS(ClassName) \
	class ClassName##Class : public ZEClass \
	{ \
		public: \
			virtual ZEClass*				GetParentClass(); \
			virtual const char*				GetName(); \
			virtual ZEGUID					GetGUID(); \
			virtual ZESize					GetSizeOfClass(); \
			virtual ZESize					GetSizeOfScriptBaseClass(); \
			virtual const ZEMetaAttribute*	GetAttributes(); \
			virtual ZESize					GetAttributeCount(); \
			virtual const ZEProperty*		GetProperties(); \
			virtual ZESize					GetPropertyCount(); \
			virtual const ZEMethod*			GetMethods(); \
			virtual ZESize					GetMethodCount(); \
			virtual ZESize					GetPropertyId(ZEString PropertyName); \
			virtual ZESize					GetMethodId(ZEString MethodName, ZESize OverloadIndex = 0); \
			virtual ZESize					GetPropertyOffset(ZESize PropertyId); \
			virtual ZESize					GetPropertyOffset(ZEString PropertyName); \
			virtual bool					SetProperty(ZEObject* Object, ZESize PropertyId, const ZEVariant& Value); \
			bool							SetProperty(ZEObject* Object, ZEString PropertyName, const ZEVariant& Value); \
			virtual bool					GetProperty(ZEObject* Object, ZESize PropertyId, ZEVariant& Value); \
			bool							GetProperty(ZEObject* Object, ZEString PropertyName, ZEVariant& Value); \
			virtual bool					GetPropertyItem(ZEObject* Object, ZESize PropertyId, ZESize Index, ZEVariant& Value); \
			bool							GetPropertyItem(ZEObject* Object, ZEString PropertyName, ZESize Index, ZEVariant& Value); \
			virtual bool					SetPropertyItem(ZEObject* Object, ZESize PropertyId, ZESize Index, ZEVariant& Value); \
			bool							SetPropertyItem(ZEObject* Object, ZEString PropertyName, ZESize Index, ZEVariant& Value); \
			virtual bool					AddItemToProperty(ZEObject* Object, ZESize PropertyId, ZEVariant& Value); \
			bool							AddItemToProperty(ZEObject* Object, ZEString PropertyName, ZEVariant& Value); \
			virtual bool					AddItemToProperty(ZEObject* Object, ZESize PropertyId, ZESize Index, ZEVariant& Value); \
			bool							AddItemToProperty(ZEObject* Object, ZEString PropertyName, ZESize Index, ZEVariant& Value); \
			virtual bool					RemoveItemFromProperty(ZEObject* Object, ZESize PropertyId, ZESize Index); \
			bool							RemoveItemFromProperty(ZEObject* Object, ZEString PropertyName, ZESize Index); \
			virtual bool					GetPropertyItemCount(ZEObject* Object, ZESize PropertyId, ZESize& Count); \
			bool							GetPropertyItemCount(ZEObject* Object, ZEString PropertyName, ZESize& Count); \
			virtual bool					AddEventHandler(ZEObject* Target, ZESize EventId, ZEEventHandlerBase* Handler); \
			bool							AddEventHandler(ZEObject* Target, ZEString EventName, ZEEventHandlerBase* Handler); \
			virtual bool					RemoveEventHandler(ZEObject* Target, ZESize EventId, ZEEventHandlerBase* Handler); \
			bool							RemoveEventHandler(ZEObject* Target, ZEString EventName, ZEEventHandlerBase* Handler); \
			virtual bool					CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference** Parameters, ZESize ParameterCount); \
			virtual bool					CallMethod(ZEObject* Object, ZEString MethodName, ZEVariant& ReturnValue, const ZEReference** Parameters, ZESize ParameterCount); \
			virtual ZEObject*				CreateInstance(); \
			virtual void*					CreateScriptInstance(); \
			virtual void*					GetScriptInstance(); \
			virtual ZEClass*				GetClass() const; \
			static ZEClass*					Class(); \
	};

#endif
