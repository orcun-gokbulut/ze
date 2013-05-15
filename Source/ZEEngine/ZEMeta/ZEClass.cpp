//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEClass.cpp
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

#include "ZEClass.h"
#include "ZEObject.h"
#include "ZEDS/ZEVariant.h"
#include "ZEReference.h"

ZEClass* ZEClass::GetParentClass()
{
	return NULL;
}

const char* ZEClass::GetName()
{
	return "";
}

ZEGUID ZEClass::GetGUID()
{
	return ZEGUID(0, 0, 0, 0);
}

ZESize ZEClass::GetSizeOfClass()
{
	return 0;
}

const ZEMetaAttribute* ZEClass::GetAttributes()
{
	return NULL;
}

ZESize ZEClass::GetAttributeCount()
{
	return 0;
}

const ZEProperty* ZEClass::GetProperties()
{
	return NULL;
}
ZESize ZEClass::GetPropertyCount()
{
	return 0;
}

const ZEMethod* ZEClass::GetMethods()
{
	return NULL;
}

ZESize ZEClass::GetMethodCount()
{
	return 0;
}

bool ZEClass::SetProperty(ZEObject* Object, ZESize PropertyId, const ZEVariant& Value)
{
	return false;
}

bool ZEClass::SetProperty(ZEObject* Object, ZEString PropertyName, const ZEVariant& Value)
{
	return false;
}

bool ZEClass::GetProperty(ZEObject* Object, ZESize PropertyId, ZEVariant& Value)
{
	return false;
}

bool ZEClass::GetProperty(ZEObject* Object, ZEString PropertyName, ZEVariant& Value)
{
	return false;
}

ZESize ZEClass::GetPropertyOffset(ZESize PropertyId)
{
	return 0;
}

ZESize ZEClass::GetPropertyOffset(ZEString PropertyName)
{
	return 0;
}

bool ZEClass::GetPropertyItem(ZEObject* Object, ZESize PropertyId, ZESize Index, ZEVariant& Value)
{
	return false;
}

bool ZEClass::GetPropertyItem(ZEObject* Object, ZEString PropertyName, ZESize Index, ZEVariant& Value)
{
	return false;
}

bool ZEClass::SetPropertyItem(ZEObject* Object, ZESize PropertyId, ZESize Index, ZEVariant& Value)
{
	return false;
}

bool ZEClass::SetPropertyItem(ZEObject* Object, ZEString PropertyName, ZESize Index, ZEVariant& Value)
{
	return false;
}

bool ZEClass::AddItemToProperty(ZEObject* Object, ZESize PropertyId, ZEVariant& Value)
{
	return false;
}

bool ZEClass::AddItemToProperty(ZEObject* Object, ZEString PropertyName, ZEVariant& Value)
{
	return false;
}

bool ZEClass::AddItemToProperty(ZEObject* Object, ZESize PropertyId, ZESize Index, ZEVariant& Value)
{
	return false;
}

bool ZEClass::AddItemToProperty(ZEObject* Object, ZEString PropertyName, ZESize Index, ZEVariant& Value)
{
	return false;
}

bool ZEClass::RemoveItemFromProperty(ZEObject* Object, ZESize PropertyId, ZESize Index)
{
	return false;
}

bool ZEClass::RemoveItemFromProperty(ZEObject* Object, ZEString PropertyName, ZESize Index)
{
	return false;
}

bool ZEClass::GetPropertyItemCount(ZEObject* Object, ZESize PropertyId, ZESize& Count)
{
	return false;
}

bool ZEClass::GetPropertyItemCount(ZEObject* Object, ZEString PropertyName, ZESize& Count)
{
	return false;
}

bool ZEClass::AddEventHandler(ZEObject* Target, ZESize EventId, ZEEventHandlerBase* Handler)
{
	return false;
}

bool ZEClass::AddEventHandler(ZEObject* Target, ZEString EventName, ZEEventHandlerBase* Handler)
{
	return false;
}

bool ZEClass::RemoveEventHandler(ZEObject* Target, ZESize EventId, ZEEventHandlerBase* Handler)
{
	return false;
}

bool ZEClass::RemoveEventHandler(ZEObject* Target, ZEString EventName, ZEEventHandlerBase* Handler)
{
	return false;
}

bool ZEClass::CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference** Parameters, ZESize ParameterCount)
{
	return false;
}

bool ZEClass::CallMethod(ZEObject* Object, ZEString MethodName, ZEVariant& ReturnValue, const ZEReference** Parameters, ZESize ParameterCount)
{
	return false;
}

ZESize ZEClass::GetPropertyId(ZEString PropertyName)
{
	return -1;
}

ZESize	ZEClass::GetMethodId(ZEString MethodName, ZESize OverloadIndex)
{
	return -1;
}

ZEObject* ZEClass::CreateInstance()
{
	return NULL;
}

bool ZEClass::IsDerivedFrom(ZEClass* ParentClass, ZEClass* Class)
{
	ZEClass* CurrentClass = Class;
	while(CurrentClass->GetParentClass() != NULL)
	{
		if(ParentClass == CurrentClass->GetParentClass())
			return true;

		CurrentClass = CurrentClass->GetParentClass();
	}

	return false;
}

bool ZEClass::IsDerivedFrom(ZEClass* ParentClass, ZEObject* Object)
{
	ZEClass* CurrentClass = Object->GetClass();
	while(CurrentClass->GetParentClass() != NULL)
	{
		if(ParentClass == CurrentClass->GetParentClass())
			return true;

		CurrentClass = CurrentClass->GetParentClass();
	}

	return false;
}

bool ZEClass::IsSame(ZEClass* ClassA, ZEClass* ClassB)
{
	return ClassA == ClassB ? true : false;
}

bool ZEClass::IsSame(ZEClass* Class, ZEObject* Object)
{
	return Class == Object->GetClass() ? true : false;
}

