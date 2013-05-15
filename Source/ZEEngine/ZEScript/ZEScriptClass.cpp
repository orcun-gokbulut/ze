//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEScriptClass.cpp
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

/*
#include "ZEScriptClass.h"

void ZEScriptClass::SetParentClass(ZEClass* ParentClass)
{
	this->ParentClass = ParentClass;
}

void ZEScriptClass::SetName(const char* Name)
{
	this->Name = Name;
}

void ZEScriptClass::SetGUID(const ZEGUID& GUID)
{

}

void ZEScriptClass::AddAttribute(const char* AttributeName)
{

}

void ZEScriptClass::RemoveAttribute(ZEMetaAttribute* Attribute)
{

}

void ZEScriptClass::AddProperty(ZEProperty* Property)
{

}

void ZEScriptClass::RemovePropery(ZEProperty* Property)
{

}

void ZEScriptClass::AddMethod(ZEMethod* Method)
{

}

void ZEScriptClass::RemoveMethod(ZEMethod* Method)
{

}

ZEClass* ZEScriptClass::GetParentClass()
{
	return ParentClass;
}

const char*	 ZEScriptClass::GetName()
{
	return Name;
}

ZEGUID ZEScriptClass::GetGUID()
{
	return GUID;
}

ZESize ZEScriptClass::GetSizeOfClass()
{
	return 0;
}

const ZEMetaAttribute* ZEScriptClass::GetAttributes()
{
	return Attributes.GetConstCArray();
}

ZESize ZEScriptClass::GetAttributeCount()
{
	return Attributes.GetCount();
}

const ZEProperty* ZEScriptClass::GetProperties()
{
	return Properties.GetConstCArray();
}

ZESize ZEScriptClass::GetPropertyCount()
{
	return Properties.GetCount();
}

const ZEMethod* ZEScriptClass::GetMethods()
{
	return Methods.GetConstCArray();
}

ZESize ZEScriptClass::GetMethodCount()
{
	return Methods.GetCount();
}

ZESize ZEScriptClass::GetPropertyId(ZEString PropertyName)
{
	// Binary Search
}

ZESize ZEScriptClass::GetMethodId(ZEString MethodName, ZESize OverloadIndex = 0)
{
	// Binary Search
}

ZESize ZEScriptClass::GetPropertyOffset(ZESize PropertyId)
{
	return -1;
}

ZESize ZEScriptClass::GetPropertyOffset(ZEString PropertyName)
{
	return -1;
}

bool ZEScriptClass::SetProperty(ZEObject* Object, ZESize PropertyId, const ZEVariant& Value)
{
	if (ParentClass->GetPropertyCount() < PropertyId)
		return ParentClass->SetProperty(Object, PropertyId, Value);

	// Dynamic Properties
}

bool ZEScriptClass::GetProperty(ZEObject* Object, ZESize PropertyId, ZEVariant& Value);
bool ZEScriptClass::GetPropertyItem(ZEObject* Object, ZESize PropertyId, ZESize Index, ZEVariant& Value);
bool ZEScriptClass::SetPropertyItem(ZEObject* Object, ZESize PropertyId, ZESize Index, ZEVariant& Value);
bool ZEScriptClass::AddItemToProperty(ZEObject* Object, ZESize PropertyId, ZEVariant& Value);
bool ZEScriptClass::AddItemToProperty(ZEObject* Object, ZESize PropertyId, ZESize Index, ZEVariant& Value);
bool ZEScriptClass::RemoveItemFromProperty(ZEObject* Object, ZESize PropertyId, ZESize Index);
bool ZEScriptClass::GetPropertyItemCount(ZEObject* Object, ZESize PropertyId, ZESize& Count);
bool ZEScriptClass::AddEventHandler(ZEObject* Target, ZESize EventId, ZEEventHandlerBase* Handler);
bool ZEScriptClass::RemoveEventHandler(ZEObject* Target, ZESize EventId, ZEEventHandlerBase* Handler);
bool ZEScriptClass::CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference** Parameters, ZESize ParameterCount);
bool ZEScriptClass::CallMethod(ZEObject* Object, ZEString MethodName, ZEVariant& ReturnValue, const ZEReference** Parameters, ZESize ParameterCount);*/
