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

bool ZEClass::SetProperty(ZEObject* Object, ZESize PropertyId, const ZENewVariant& Value)
{
	return false;
}

bool ZEClass::GetProperty(ZEObject* Object, ZESize PropertyId, ZENewVariant& Value)
{
	return false;
}

bool ZEClass::GetPropertyItem(ZEObject* Object, ZESize PropertyId, ZESize Index, ZENewVariant& Value)
{
	return false;
}

bool ZEClass::SetPropertyItem(ZEObject* Object, ZESize PropertyId, ZESize Index, ZENewVariant& Value)
{
	return false;
}

bool ZEClass::AddItemToProperty(ZEObject* Object, ZESize PropertyId, ZENewVariant& Value)
{
	return false;
}

bool ZEClass::AddItemToProperty(ZEObject* Object, ZESize PropertyId, ZESize Index, ZENewVariant& Value)
{
	return false;
}

bool ZEClass::RemoveItemFromProperty(ZEObject* Object, ZESize PropertyId, ZESize Index)
{
	return false;
}

bool ZEClass::GetPropertyItemCount(ZEObject* Object, ZESize PropertyId, ZESize& Count)
{
	return false;
}

bool ZEClass::AddEventHandler(ZEObject* Target, ZESize EventId, ZEEventHandlerBase* Handler)
{
	return false;
}

bool ZEClass::RemoveEventHandler(ZEObject* Target, ZESize EventId, ZEEventHandlerBase* Handler)
{
	return false;
}

bool ZEClass::CallMethod(ZEObject* Object, ZESize MethodId, ZENewVariant& ReturnValue, const ZEReference** Parameters, ZESize ParameterCount)
{
	return false;
}

ZESSize ZEClass::GetPropertyId(const char* PropertyName)
{
	return -1;
}

ZESSize	ZEClass::GetMethodId(const char* MethodName, ZESize OverloadIndex)
{
	return -1;
}

ZEObject* ZEClass::CreateInstance()
{
	return NULL;
}
