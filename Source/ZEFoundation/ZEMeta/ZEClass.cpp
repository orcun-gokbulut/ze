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
#include "ZEDS/ZEReference.h"
#include "ZEML/ZEMLWriter.h"
#include "ZEML/ZEMLReader.h"

ZESSize ZEClass::Search(ZEClassSortedData* Data, ZESize DataSize, const ZEString& Name)
{
	ZESize Hash = Name.Hash();
	ZESSize LowIndex = 0, HighIndex = DataSize, MiddleIndex;

	while (LowIndex < HighIndex)
	{
		MiddleIndex = (LowIndex + HighIndex) / 2;
		if (Data[MiddleIndex].Hash < Hash)
			LowIndex  = MiddleIndex + 1;
		else
			HighIndex = MiddleIndex;
	}

	if (LowIndex == HighIndex)
	{
		for (ZESSize I = LowIndex; I < DataSize; I++)
		{
			if (Data[LowIndex].Hash != Hash)
				break;

			if (Name == Data[LowIndex].Name)
				return Data[LowIndex].ID;
		}

		for (ZESSize I = LowIndex - 1; I >= 0; I--)
		{
			if (Data[LowIndex].Hash != Hash)
				break;

			if (Name == Data[LowIndex].Name)
				return Data[LowIndex].ID;
		}
	}

	return -1;
}

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

ZEClassFlags ZEClass::GetFlags()
{
	return ZE_CF_CREATE_INSTANCE | ZE_CF_DESTROY | ZE_CF_ASSIGN | ZE_CF_CLONE | ZE_CF_CONSTRUCT | ZE_CF_DECONSTRUCT;
}

const ZEAttribute* ZEClass::GetAttributes()
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

const ZEProperty* ZEClass::GetPropertyDescription(ZESize PropertyId)
{
	if (PropertyId >= GetPropertyCount())
		return NULL;

	return &GetProperties()[PropertyId];
}

const ZEProperty* ZEClass::GetPropertyDescription(const ZEString& PropertyName)
{
	ZESize PropertyId = GetPropertyId(PropertyName);
	if (PropertyId == (ZESize)-1)
		return NULL;

	return &GetProperties()[PropertyId];
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

bool ZEClass::SetProperty(ZEObject* Object, const ZEString& PropertyName, const ZEVariant& Value)
{
	ZESize PropertyId = GetPropertyId(PropertyName);
	if (PropertyId == -1)
		return false;

	return SetProperty(Object, PropertyId, Value);
}

bool ZEClass::GetProperty(ZEObject* Object, ZESize PropertyId, ZEVariant& Value)
{
	return false;
}

bool ZEClass::GetProperty(ZEObject* Object, const ZEString& PropertyName, ZEVariant& Value)
{
	ZESize PropertyId = GetPropertyId(PropertyName);
	if (PropertyId == -1)
		return false;

	return GetProperty(Object, PropertyId, Value);
}

bool ZEClass::GetPropertyItem(ZEObject* Object, ZESize PropertyId, ZESize Index, ZEVariant& Value)
{
	return false;
}

bool ZEClass::GetPropertyItem(ZEObject* Object, const ZEString& PropertyName, ZESize Index, ZEVariant& Value)
{
	ZESize PropertyId = GetPropertyId(PropertyName);
	if (PropertyId == -1)
		return false;

	return GetPropertyItem(Object, PropertyId, Index, Value);
}

bool ZEClass::SetPropertyItem(ZEObject* Object, ZESize PropertyId, ZESize Index, ZEVariant& Value)
{
	return false;
}

bool ZEClass::SetPropertyItem(ZEObject* Object, const ZEString& PropertyName, ZESize Index, ZEVariant& Value)
{
	ZESize PropertyId = GetPropertyId(PropertyName);
	if (PropertyId == -1)
		return false;

	return SetPropertyItem(Object, PropertyId, Index, Value);
}

bool ZEClass::AddItemToProperty(ZEObject* Object, ZESize PropertyId, ZEVariant& Value)
{
	ZESize Count;
	bool Result = GetPropertyItemCount(Object, PropertyId, Count);
	if (!Result)
		return false;

	return AddItemToProperty(Object, PropertyId, Count, Value);
}

bool ZEClass::AddItemToProperty(ZEObject* Object, const ZEString& PropertyName, ZEVariant& Value)
{
	ZESize PropertyId = GetPropertyId(PropertyName);
	if (PropertyId == -1)
		return false;

	ZESize Count;
	bool Result = GetPropertyItemCount(Object, PropertyId, Count);
	if (!Result)
		return false;

	return AddItemToProperty(Object, PropertyId, Count, Value);
}

bool ZEClass::AddItemToProperty(ZEObject* Object, ZESize PropertyId, ZESize Index, ZEVariant& Value)
{
	return false;
}

bool ZEClass::AddItemToProperty(ZEObject* Object, const ZEString& PropertyName, ZESize Index, ZEVariant& Value)
{
	ZESize PropertyId = GetPropertyId(PropertyName);
	if (PropertyId == -1)
		return false;

	return AddItemToProperty(Object, PropertyId, Index, Value);
}

bool ZEClass::RemoveItemFromProperty(ZEObject* Object, ZESize PropertyId, ZESize Index)
{
	return false;
}

bool ZEClass::RemoveItemFromProperty(ZEObject* Object, const ZEString& PropertyName, ZESize Index)
{
	ZESize PropertyId = GetPropertyId(PropertyName);
	if (PropertyId == -1)
		return false;

	return RemoveItemFromProperty(Object, PropertyId, Index);
}

bool ZEClass::GetPropertyItemCount(ZEObject* Object, ZESize PropertyId, ZESize& Count)
{
	return false;
}

bool ZEClass::GetPropertyItemCount(ZEObject* Object, const ZEString& PropertyName, ZESize& Count)
{
	ZESize PropertyId = GetPropertyId(PropertyName);
	if (PropertyId == -1)
		return false;

	return GetPropertyItemCount(Object, PropertyId, Count);
}

bool ZEClass::AddEventHandler(ZEObject* Target, ZESize EventId, ZEEventHandlerBase* Handler)
{
	return false;
}

bool ZEClass::AddEventHandler(ZEObject* Object, const ZEString& EventName, ZEEventHandlerBase* Handler)
{
	ZESize MethodId = GetPropertyId(EventName);
	if (MethodId == -1)
		return false;

	return AddEventHandler(Object, MethodId, Handler);
}

bool ZEClass::RemoveEventHandler(ZEObject* Object, ZESize EventId, ZEEventHandlerBase* Handler)
{
	return false;
}

bool ZEClass::RemoveEventHandler(ZEObject* Object, const ZEString& EventName, ZEEventHandlerBase* Handler)
{
	ZESize MethodId = GetPropertyId(EventName);
	if (MethodId == -1)
		return false;

	return RemoveEventHandler(Object, MethodId, Handler);
}

bool ZEClass::CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference** Parameters, ZESize ParameterCount)
{
	return false;
}

bool ZEClass::CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference** Parameters, ZESize ParameterCount)
{
	ZESize MethodId = GetPropertyId(MethodName);
	if (MethodId == -1)
		return false;

	return CallMethod(Object, MethodId, ReturnValue, Parameters, ParameterCount);
}

ZESize ZEClass::GetPropertyId(const ZEString& PropertyName)
{
	return -1;
}

ZESize ZEClass::GetMethodId(const ZEString& MethodName, ZESize OverloadIndex)
{
	return -1;
}

ZESize ZEClass::GetSizeOfObject()
{
	return sizeof(ZEObject);
}

ZEObject* ZEClass::CreateInstance()
{
	return new ZEObject();
}

bool ZEClass::Destroy(ZEObject* Object)
{
	delete Object;
	return true;
}

ZEObject* ZEClass::DynamicCast(ZEObject* Object)
{
	return Object;
}

ZEObject* ZEClass::Clone(ZEObject* Object)
{
	ZEObject* NewObject = new ZEObject();
	NewObject = Object;
	return NewObject;
}

bool ZEClass::Construct(ZEObject* Object)
{
	new(Object) ZEObject();
	return false;
}

bool ZEClass::Deconstruct(ZEObject* Object)
{
	Object->~ZEObject();
	return true;
}

bool ZEClass::Assign(ZEObject* Object, ZEObject* Source)
{
	ZEObject* CastedSource = ZEClass::Cast<ZEObject>(Source);
	if (CastedSource == NULL)
		return false;

	*(ZEObject*)Object = *CastedSource;

	return true;
}

ZEObject* ZEClass::CreateScriptInstance()
{
	return NULL;
}

ZESize ZEClass::GetSizeOfScriptObject()
{
	return 0;
}

bool ZEClass::IsDerivedFrom(ZEClass* ParentClass, ZEClass* Class)
{
	if (ParentClass == NULL)
		return false;

	while(Class != NULL)
	{
		if(ParentClass == Class)
			return true;

		Class = Class->GetParentClass();
	}

	return false;
}

bool ZEClass::Serialize(ZEObject* Object, ZEMLWriterNode& ObjectNode)
{
	if (this != Object->GetClass())
		return false;

	ObjectNode.WriteString("Class", GetName());
	
	ZEMLWriterNode PropertiesNode;
	if (!ObjectNode.OpenNode("Properties", PropertiesNode))
		return false;

	if (!SerializeProperties(Object, PropertiesNode))
		return false;

	PropertiesNode.CloseNode();
}

bool ZEClass::SerializeProperties(ZEObject* Object, ZEMLWriterNode& PropertiesNode)
{
	for (ZESize I = 0; I < GetPropertyCount(); I++)
	{
		const ZEProperty* Property = GetProperties() + I;
		if (Property->Access != ZEMT_PA_READ_WRITE)
			continue;

		const ZEProperty* PropertyDescription = GetPropertyDescription(I);
		if (PropertyDescription == NULL)
			continue;

		ZEVariant Value;
		if (!GetProperty(Object, Property->ID, Value))
			continue;

		if (PropertyDescription->Type.Enumerator != NULL)
		{
			ZESize EnumeratorItemCount = PropertyDescription->Type.Enumerator->GetItemCount();
			const ZEEnumeratorItem* EnumeratorItems = PropertyDescription->Type.Enumerator->GetItems();

			for (ZESize N = 0; N < EnumeratorItemCount; N++)
			{
				if (EnumeratorItems[I].Value != Value.GetInt32())
					break;

				PropertiesNode.WriteValue(Property->Name, Value.GetValue());
				break;
			}
		}
		else
		{
			PropertiesNode.WriteValue(Property->Name, Value.GetValue());
		}
	}

	return true;
}

bool ZEClass::Unserialize(ZEObject* Object, const ZEMLReaderNode& ObjectNode)
{
	if (ObjectNode.ReadString("Class") != Object->GetClass()->GetName())
		return false;

	return UnserializeProperties(Object, ObjectNode.GetNode("Properties"));
}

bool ZEClass::UnserializeProperties(ZEObject* Object, const ZEMLReaderNode& PropertiesNode)
{
	const ZEArray<ZEMLFormatElement>& Elements = PropertiesNode.GetElements();
	for (ZESize I = 0; I < Elements.GetCount(); I++)
	{
		const ZEMLFormatElement* Element = &Elements[I];

		if (Element->ElementType != ZEML_ET_PROPERTY)
			continue;

		const ZEProperty* PropertyDescription = GetPropertyDescription(Element->Name);
		if (PropertyDescription == NULL)
			continue;

		// Handle Enumerations
		if (Element->ValueType == ZEML_VT_STRING && PropertyDescription->Type.Enumerator != NULL)
		{
			ZESize EnumeratorItemCount = PropertyDescription->Type.Enumerator->GetItemCount();
			const ZEEnumeratorItem* EnumeratorItems = PropertyDescription->Type.Enumerator->GetItems();

			for (ZESize N = 0; N < EnumeratorItemCount; N++)
			{
				if (EnumeratorItems[N].Name != Element->Value.GetString())
					continue;

				SetProperty(Object, PropertyDescription->ID, EnumeratorItems[N].Value);
				break;
			}
		}
		else
		{
			SetProperty(Object, PropertyDescription->ID, ZEVariant(Element->Value));
		}
	}

	return true;
}
