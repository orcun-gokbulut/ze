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
#ifndef __ZE_CLASS_H__
#define __ZE_CLASS_H__

#include "ZEAttribute.h"
#include "ZEProperty.h"
#include "ZEMethod.h"
#include "ZEEnumerator.h"
#include "ZETypes.h"
#include "ZEGUID.h"
#include "ZEDS\ZEFlags.h"

#define ZE_CLASS_DEFINITION(ClassName) \
	class ClassName##Class : public ZEClass \
	{ \
		public: \
			virtual ZEClass*				GetParentClass(); \
			virtual const char*				GetName(); \
			virtual ZEGUID					GetGUID(); \
			virtual ZEClassFlags			GetFlags(); \
			virtual const ZEAttribute*		GetAttributes(); \
			virtual ZESize					GetAttributeCount(); \
			virtual const ZEProperty*		GetProperties(); \
			virtual ZESize					GetPropertyCount(); \
			virtual const ZEMethod*			GetMethods(); \
			virtual ZESize					GetMethodCount(); \
			virtual ZESize					GetPropertyId(const ZEString& PropertyName); \
			virtual ZESize					GetMethodId(const ZEString& MethodName, ZESize OverloadIndex = 0); \
			virtual bool					SetProperty(ZEObject* Object, ZESize PropertyId, const ZEVariant& Value); \
			virtual bool					GetProperty(ZEObject* Object, ZESize PropertyId, ZEVariant& Value); \
			virtual bool					GetPropertyItem(ZEObject* Object, ZESize PropertyId, ZESize Index, ZEVariant& Value); \
			virtual bool					SetPropertyItem(ZEObject* Object, ZESize PropertyId, ZESize Index, ZEVariant& Value); \
			virtual bool					AddItemToProperty(ZEObject* Object, ZESize PropertyId, ZESize Index, ZEVariant& Value); \
			virtual bool					RemoveItemFromProperty(ZEObject* Object, ZESize PropertyId, ZESize Index); \
			virtual bool					GetPropertyItemCount(ZEObject* Object, ZESize PropertyId, ZESize& Count); \
			virtual bool					AddEventHandler(ZEObject* Object, ZESize EventId, ZEEventHandlerBase* Handler); \
			virtual bool					RemoveEventHandler(ZEObject* Object, ZESize EventId, ZEEventHandlerBase* Handler); \
			virtual bool					CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference** Parameters, ZESize ParameterCount); \
			virtual ZESize					GetSizeOfObject(); \
			virtual ZEObject*				CreateInstance(); \
			virtual bool					Destroy(ZEObject* Object); \
			virtual ZEObject*				DynamicCast(ZEObject* Object); \
			virtual ZEObject*				Clone(ZEObject* Object); \
			virtual bool					Construct(ZEObject* Object); \
			virtual bool					Deconstruct(ZEObject* Object); \
			virtual bool					Assign(ZEObject* Object, ZEObject* Source); \
			virtual ZEObject*				CreateScriptInstance(); \
			virtual ZESize					GetSizeOfScriptObject(); \
	};

class ZEObject;
class ZEEventHandlerBase;
class ZEVariant;
class ZEReference;
class ZEScriptObject;

typedef ZEUInt ZEClassFlags;
#define ZE_CF_NONE						0
#define ZE_CF_BUILTIN					1
#define ZE_CF_ABSTRACT					2
#define ZE_CF_SCRIPT					4 
#define ZE_CF_CREATE_INSTANCE			8 
#define ZE_CF_DESTROY					16
#define ZE_CF_ASSIGN					32
#define ZE_CF_CLONE						64
#define ZE_CF_CONSTRUCT					128
#define ZE_CF_DECONSTRUCT				256

#define ZE_CF_VALUE_OBJECT				(ZE_CF_CONSTRUCT | ZE_CF_DECONSTRUCT)

struct ZEClassSortedData
{
	ZESize			Hash;
	ZESize			ID;
	const char*		Name;
};

class ZEClass
{
	protected:
		static ZESSize					Search(ZEClassSortedData* Data, ZESize DataSize, const ZEString& Name);

	public:
		virtual ZEClass*				GetParentClass();

		virtual const char*				GetName();
		virtual ZEGUID					GetGUID();
		virtual ZEClassFlags			GetFlags();

		virtual const ZEAttribute*		GetAttributes();
		virtual ZESize					GetAttributeCount();

		virtual const ZEProperty*		GetProperties();
		virtual ZESize					GetPropertyCount();

		virtual const ZEMethod*			GetMethods();
		virtual ZESize					GetMethodCount();

		virtual ZESize					GetPropertyId(const ZEString& PropertyName);
		virtual ZESize					GetMethodId(const ZEString& MethodName, ZESize OverloadIndex = 0);

		virtual bool					SetProperty(ZEObject* Object, ZESize PropertyId, const ZEVariant& Value);
		bool							SetProperty(ZEObject* Object, const ZEString& PropertyName, const ZEVariant& Value);

		virtual bool					GetProperty(ZEObject* Object, ZESize PropertyId, ZEVariant& Value);
		bool							GetProperty(ZEObject* Object, const ZEString& PropertyName, ZEVariant& Value);

		virtual bool					SetPropertyItem(ZEObject* Object, ZESize PropertyId, ZESize Index, ZEVariant& Value);
		bool							SetPropertyItem(ZEObject* Object, const ZEString& PropertyName, ZESize Index, ZEVariant& Value);

		virtual bool					GetPropertyItem(ZEObject* Object, ZESize PropertyId, ZESize Index, ZEVariant& Value);
		bool							GetPropertyItem(ZEObject* Object, const ZEString& PropertyName, ZESize Index, ZEVariant& Value);

		bool							AddItemToProperty(ZEObject* Object, ZESize PropertyId, ZEVariant& Value);
		bool							AddItemToProperty(ZEObject* Object, const ZEString& PropertyName, ZEVariant& Value);

		virtual bool					AddItemToProperty(ZEObject* Object, ZESize PropertyId, ZESize Index, ZEVariant& Value);
		bool							AddItemToProperty(ZEObject* Object, const ZEString& PropertyName, ZESize Index, ZEVariant& Value);

		virtual bool					RemoveItemFromProperty(ZEObject* Object, ZESize PropertyId, ZESize Index);
		bool							RemoveItemFromProperty(ZEObject* Object, const ZEString& PropertyName, ZESize Index);

		virtual bool					GetPropertyItemCount(ZEObject* Object, ZESize PropertyId, ZESize& Count);
		bool							GetPropertyItemCount(ZEObject* Object, const ZEString& PropertyName, ZESize& Count);

		virtual bool					AddEventHandler(ZEObject* Target, ZESize EventId, ZEEventHandlerBase* Handler);
		bool							AddEventHandler(ZEObject* Target, const ZEString& EventName, ZEEventHandlerBase* Handler);

		virtual bool					RemoveEventHandler(ZEObject* Target, ZESize EventId, ZEEventHandlerBase* Handler);
		bool							RemoveEventHandler(ZEObject* Target, const ZEString& EventName, ZEEventHandlerBase* Handler);

		virtual bool					CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference** Parameters, ZESize ParameterCount);
		bool							CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue);
		bool							CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference& Parameters0);
		bool							CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1);
		bool							CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2);
		bool							CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3);
		bool							CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4);
		bool							CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5);
		bool							CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6);
		bool							CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7);
		bool							CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8);
		bool							CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9);
		bool							CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10);
		bool							CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11);
		bool							CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12);
		bool							CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13);
		bool							CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14);
		bool							CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15);
		bool							CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16);
		bool							CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16, const ZEReference& Parameters17);
		bool							CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16, const ZEReference& Parameters17, const ZEReference& Parameters18);
		bool							CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16, const ZEReference& Parameters17, const ZEReference& Parameters18, const ZEReference& Parameters19);
		bool							CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16, const ZEReference& Parameters17, const ZEReference& Parameters18, const ZEReference& Parameters19, const ZEReference& Parameters20);
		bool							CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16, const ZEReference& Parameters17, const ZEReference& Parameters18, const ZEReference& Parameters19, const ZEReference& Parameters20, const ZEReference& Parameters21);
		bool							CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16, const ZEReference& Parameters17, const ZEReference& Parameters18, const ZEReference& Parameters19, const ZEReference& Parameters20, const ZEReference& Parameters21, const ZEReference& Parameters22);
		bool							CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16, const ZEReference& Parameters17, const ZEReference& Parameters18, const ZEReference& Parameters19, const ZEReference& Parameters20, const ZEReference& Parameters21, const ZEReference& Parameters22, const ZEReference& Parameters23);
		bool							CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16, const ZEReference& Parameters17, const ZEReference& Parameters18, const ZEReference& Parameters19, const ZEReference& Parameters20, const ZEReference& Parameters21, const ZEReference& Parameters22, const ZEReference& Parameters23, const ZEReference& Parameters24);

		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference** Parameters, ZESize ParameterCount);
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue);
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0);
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1);
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2);
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3);
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4);
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5);
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6);
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7);
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8);
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9);
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10);
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11);
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12);
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13);
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14);
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15);
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16);
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16, const ZEReference& Parameters17);
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16, const ZEReference& Parameters17, const ZEReference& Parameters18);
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16, const ZEReference& Parameters17, const ZEReference& Parameters18, const ZEReference& Parameters19);
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16, const ZEReference& Parameters17, const ZEReference& Parameters18, const ZEReference& Parameters19, const ZEReference& Parameters20);
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16, const ZEReference& Parameters17, const ZEReference& Parameters18, const ZEReference& Parameters19, const ZEReference& Parameters20, const ZEReference& Parameters21);
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16, const ZEReference& Parameters17, const ZEReference& Parameters18, const ZEReference& Parameters19, const ZEReference& Parameters20, const ZEReference& Parameters21, const ZEReference& Parameters22);
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16, const ZEReference& Parameters17, const ZEReference& Parameters18, const ZEReference& Parameters19, const ZEReference& Parameters20, const ZEReference& Parameters21, const ZEReference& Parameters22, const ZEReference& Parameters23);
		bool							CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference& Parameters0, const ZEReference& Parameters1, const ZEReference& Parameters2, const ZEReference& Parameters3, const ZEReference& Parameters4, const ZEReference& Parameters5, const ZEReference& Parameters6, const ZEReference& Parameters7, const ZEReference& Parameters8, const ZEReference& Parameters9, const ZEReference& Parameters10, const ZEReference& Parameters11, const ZEReference& Parameters12, const ZEReference& Parameters13, const ZEReference& Parameters14, const ZEReference& Parameters15, const ZEReference& Parameters16, const ZEReference& Parameters17, const ZEReference& Parameters18, const ZEReference& Parameters19, const ZEReference& Parameters20, const ZEReference& Parameters21, const ZEReference& Parameters22, const ZEReference& Parameters23, const ZEReference& Parameters24);

		// Life Cycle Management
		virtual ZESize					GetSizeOfObject();
		virtual ZEObject*				CreateInstance();
		virtual bool					Destroy(ZEObject* Object);

		virtual ZEObject*				DynamicCast(ZEObject* Object);
		virtual ZEObject*				Clone(ZEObject* Object);
		virtual bool					Construct(ZEObject* Object);
		virtual bool					Deconstruct(ZEObject* Object);
		virtual bool					Assign(ZEObject* Object, ZEObject* Source);

		// Scripting
		virtual ZEObject*				CreateScriptInstance();
		virtual ZESize					GetSizeOfScriptObject();

		static bool						IsDerivedFrom(ZEClass* Parent, ZEClass* Target);
		template<typename ZECastedObjectType>
		static ZECastedObjectType*		Cast(ZEObject* Object);
};

template<typename ZECastedObjectType>
ZECastedObjectType* ZEClass::Cast(ZEObject* Object)
{
	if (ZEClass::IsDerivedFrom(ZECastedObjectType::Class(), Object->GetClass()))
		return static_cast<ZECastedObjectType*>(Object);
	
	return NULL;
}

#endif
