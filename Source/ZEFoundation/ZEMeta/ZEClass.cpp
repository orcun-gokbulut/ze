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
#include "ZEDS/ZEString.h"
#include "ZEDS/ZEFormat.h"

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
		for (ZESSize I = LowIndex; I < (ZESSize)DataSize; I++)
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

ZEMTDeclarationType ZEClass::GetDeclarationType() const
{
	return ZEMT_DT_CLASS;
}

const char* ZEClass::GetName() const
{
	return "ZEObject";
}

const ZEGUID& ZEClass::GetGUID() const
{
	return ZEGUID::Zero;
}

const ZEMTAttribute* ZEClass::GetAttributes() const
{
	return NULL;
}

ZESize ZEClass::GetAttributeCount() const
{
	return 0;
}

ZEClass* ZEClass::GetParentClass() const
{
	return NULL;
}

ZEMTClassFlags ZEClass::GetFlags() const
{
	return ZEMT_CF_CREATABLE | ZEMT_CF_DESTROYABLE | ZEMT_CF_ASSIGNABLE | ZEMT_CF_COPYABLE | ZEMT_CF_CLONEABLE | ZEMT_CF_CONSTRUCTIBLE | ZEMT_CF_DECONSTRUCTIBLE;
}

bool ZEClass::IsAbstract() const
{
	return (GetFlags() & ZEMT_CF_ABSTRACT) != 0;
}

bool ZEClass::IsFundamental() const
{
	return (GetFlags() & ZEMT_CF_FUNDAMENTAL) != 0;
}

bool ZEClass::IsCreatable() const
{
	return (GetFlags() & ZEMT_CF_CREATABLE) != 0;
}

bool ZEClass::IsDestroyable() const
{
	return (GetFlags() & ZEMT_CF_DESTROYABLE) != 0;
}

bool ZEClass::IsAssignable() const
{
	return (GetFlags() & ZEMT_CF_ASSIGNABLE) != 0;
}

bool ZEClass::IsCopyable() const
{
	return (GetFlags() & ZEMT_CF_COPYABLE) != 0;
}

bool ZEClass::IsCloneable() const
{
	return (GetFlags() & ZEMT_CF_CLONEABLE) != 0;
}

bool ZEClass::IsConstructible() const
{
	return (GetFlags() & ZEMT_CF_CONSTRUCTIBLE) != 0;
}

bool ZEClass::IsDeconstructible() const
{
	return (GetFlags() & ZEMT_CF_DECONSTRUCTIBLE) != 0;
}

const ZEMTProperty* ZEClass::GetProperties() const
{
	return NULL;
}

ZESize ZEClass::GetPropertyCount() const
{
	return 0;
}

const ZEMTProperty* ZEClass::GetPropertyDescription(ZESize PropertyId) const
{
	if (PropertyId >= GetPropertyCount())
		return NULL;

	return &GetProperties()[PropertyId];
}

const ZEMTProperty* ZEClass::GetPropertyDescription(const ZEString& PropertyName) const
{
	ZESize PropertyId = GetPropertyId(PropertyName);
	if (PropertyId == (ZESize)-1)
		return NULL;

	return &GetProperties()[PropertyId];
}

const ZEMTMethod* ZEClass::GetMethods() const
{
	return NULL;
}

ZESize ZEClass::GetMethodCount() const
{
	return 0;
}

bool ZEClass::SetProperty(ZEObject* Object, ZESize PropertyId, const ZEVariant& Value) const
{
	return false;
}

bool ZEClass::SetProperty(ZEObject* Object, const ZEString& PropertyName, const ZEVariant& Value) const
{
	ZESize PropertyId = GetPropertyId(PropertyName);
	if (PropertyId == -1)
		return false;

	return SetProperty(Object, PropertyId, Value);
}

bool ZEClass::SetPropertyConst(const ZEObject* Object, ZESize PropertyId, const ZEVariant& Value) const
{
	return false;
}

bool ZEClass::SetPropertyConst(const ZEObject* Object, const ZEString& PropertyName, const ZEVariant& Value) const
{
	ZESize PropertyId = GetPropertyId(PropertyName);
	if (PropertyId == -1)
		return false;

	return SetPropertyConst(Object, PropertyId, Value);
}

bool ZEClass::GetProperty(ZEObject* Object, ZESize PropertyId, ZEVariant& Value) const
{
	return false;
}

bool ZEClass::GetProperty(ZEObject* Object, const ZEString& PropertyName, ZEVariant& Value) const
{
	ZESize PropertyId = GetPropertyId(PropertyName);
	if (PropertyId == -1)
		return false;

	return GetProperty(Object, PropertyId, Value);
}

bool ZEClass::GetPropertyConst(const ZEObject* Object, ZESize PropertyId, ZEVariant& Value) const
{
	return false;
}

bool ZEClass::GetPropertyConst(const ZEObject* Object, const ZEString& PropertyName, ZEVariant& Value) const
{
	ZESize PropertyId = GetPropertyId(PropertyName);
	if (PropertyId == -1)
		return false;

	return GetPropertyConst(Object, PropertyId, Value);
}

bool ZEClass::AddEventDelegate(ZEObject* Target, ZESize EventId, ZEEventDelegateBase* Delegate) const
{
	return false;
}

bool ZEClass::AddEventDelegate(ZEObject* Object, const ZEString& EventName, ZEEventDelegateBase* Delegate) const
{
	ZESize MethodId = GetPropertyId(EventName);
	if (MethodId == -1)
		return false;

	return AddEventDelegate(Object, MethodId, Delegate);
}

bool ZEClass::RemoveEventDelegate(ZEObject* Object, ZESize EventId, ZEEventDelegateBase* Delegate) const
{
	return false;
}

bool ZEClass::RemoveEventDelegate(ZEObject* Object, const ZEString& EventName, ZEEventDelegateBase* Delegate) const
{
	ZESize MethodId = GetPropertyId(EventName);
	if (MethodId == -1)
		return false;

	return RemoveEventDelegate(Object, MethodId, Delegate);
}

bool ZEClass::CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference** Parameters, ZESize ParameterCount) const
{
	return false;
}

bool ZEClass::CallMethod(ZEObject* Object, const ZEString& MethodName, ZEVariant& ReturnValue, const ZEReference** Parameters, ZESize ParameterCount) const
{
	ZESize MethodId = GetPropertyId(MethodName);
	if (MethodId == -1)
		return false;

	return CallMethod(Object, MethodId, ReturnValue, Parameters, ParameterCount);
}

bool ZEClass::CallMethodConst(const ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference** Parameters, ZESize ParameterCount) const
{
	return false;
}

ZESize ZEClass::GetPropertyId(const ZEString& PropertyName) const
{
	return -1;
}

ZESize ZEClass::GetMethodId(const ZEString& MethodName, ZESize OverloadIndex) const
{
	return -1;
}

ZESize ZEClass::GetSizeOfObject() const
{
	return sizeof(ZEObject);
}

ZEObject* ZEClass::CreateInstance() const
{
	return new ZEObject();
}

bool ZEClass::Destroy(ZEObject* Object) const
{
	delete Object;
	return true;
}

bool ZEClass::Construct(ZEObject* Object) const
{
	new(Object) ZEObject();
	return false;
}

bool ZEClass::CopyConstruct(ZEObject* DestinationObject, const ZEObject* SourceObject) const
{
	if (SourceObject->GetClass() != ZEObject::Class())
		return false;

	new (DestinationObject) ZEObject(*SourceObject);
	return true;
}

bool ZEClass::Deconstruct(ZEObject* Object) const
{
	Object->~ZEObject();
	return true;
}

bool ZEClass::Assign(ZEObject* DestinationObject, const ZEObject* SourceObject) const
{
	if (SourceObject->GetClass() != ZEObject::Class())
		return false;

	*DestinationObject = *SourceObject;
	return true;
}

ZEObject* ZEClass::Clone(const ZEObject* Object) const
{
	if (Object->GetClass() != ZEObject::Class())
		return NULL;

	return new ZEObject(*Object);
}

ZEObject* ZEClass::DynamicCast(ZEObject* Object) const
{
	return Object;
}

ZEObject* ZEClass::CreateScriptInstance() const
{
	return NULL;
}

ZESize ZEClass::GetSizeOfScriptObject() const
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

bool ZEClass::IsDerivedFrom(ZEClass* ParentClass, ZEObject* Object)
{
	if (Object == NULL)
		return false;

	return IsDerivedFrom(ParentClass, Object->GetClass());
}

bool ZEClass::Serialize(ZEObject* Object, ZEMLWriterNode& ObjectNode) const
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

	return true;
}

bool ZEClass::SerializeProperties(ZEObject* Object, ZEMLWriterNode& PropertiesNode) const
{
	for (ZESize I = 0; I < GetPropertyCount(); I++)
	{
		const ZEMTProperty* Property = GetProperties() + I;
		if (Property->Access != ZEMT_PA_READ_WRITE)
			continue;

		const ZEMTProperty* PropertyDescription = GetPropertyDescription(I);
		if (PropertyDescription == NULL)
			continue;

		ZEVariant Value;
		if (!GetProperty(Object, Property->ID, Value))
			continue;

		if (PropertyDescription->Type.GetBaseType() == ZEMT_BT_ENUMERATOR)
		{
			ZEMTEnumerator* Enumurator = PropertyDescription->Type.GetEnumerator();
			ZESize EnumeratorItemCount = Enumurator->GetItemCount();
			const ZEMTEnumeratorItem* EnumeratorItems = Enumurator->GetItems();
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

bool ZEClass::Unserialize(ZEObject* Object, const ZEMLReaderNode& ObjectNode) const
{
	if (ObjectNode.ReadString("Class") != Object->GetClass()->GetName())
	{
		zeWarning("Cannot find Class node in object node. Class: \"%s\", ZEML Path: \"%s\", .", 
			Object->GetClass()->GetName(),
			ObjectNode.GetPath().ToCString());

		return false;
	}

	ZEMLReaderNode Properties = ObjectNode.GetNode("Properties");
	if (!Properties.IsValid())
	{
		zeWarning("Cannot find Properties node in object node. Class: \"%s\", ZEML Path: \"%s\".", 
			Object->GetClass()->GetName(),
			ObjectNode.GetPath().ToCString());
		return false;
	}

	return UnserializeProperties(Object, Properties);
}

bool ZEClass::UnserializeProperties(ZEObject* Object, const ZEMLReaderNode& PropertiesNode) const
{
	const ZEArray<ZEMLFormatElement>& Elements = PropertiesNode.GetElements();
	for (ZESize I = 0; I < Elements.GetCount(); I++)
	{
		const ZEMLFormatElement* Element = &Elements[I];

		if (Element->ElementType != ZEML_ET_PROPERTY)
			continue;

		const ZEMTProperty* PropertyDescription = GetPropertyDescription(Element->Name);
		if (PropertyDescription == NULL)
		{
			zeWarning("Property not found. Class Name: \"%s\", Property Name: \"%s\", ZEML Path: \"%s\".", 
				Object->GetClass()->GetName(), 
				Element->Name.ToCString(),
				ZEFormat::Format("{0}/{1}", PropertiesNode.GetPath(), Element->Name).ToCString());
			continue;
		}

		// Handle Enumerations
		if (Element->ValueType == ZEML_VT_STRING && PropertyDescription->Type.GetBaseType() == ZEMT_BT_ENUMERATOR)
		{
			ZEMTEnumerator* Enumerator = PropertyDescription->Type.GetEnumerator();
			ZESize EnumeratorItemCount = Enumerator->GetItemCount();
			const ZEMTEnumeratorItem* EnumeratorItems = Enumerator->GetItems();

			for (ZESize N = 0; N < EnumeratorItemCount; N++)
			{
				if (EnumeratorItems[N].Name != Element->Value.GetString())
					continue;

				if (!SetProperty(Object, PropertyDescription->ID, EnumeratorItems[N].Value))
				{
					zeWarning("SetProperty failed. Class Name: \"%s\", Property Name: \"%s\", ZEML Path: \"%s\".", 
						Object->GetClass()->GetName(), 
						Element->Name,
						ZEFormat::Format("{0}/{1}", PropertiesNode.GetPath(), Element->Name).ToCString());
				}
				break;
			}
		}
		else
		{
			if (!SetProperty(Object, PropertyDescription->ID, ZEVariant(Element->Value)))
			{
				zeWarning("SetProperty failed. Class Name: \"%s\", Property Name: \"%s\", ZEML Path: \"%s\".", 
					Object->GetClass()->GetName(),
					Element->Name.ToCString(),
					ZEFormat::Format("{0}/{1}", PropertiesNode.GetPath(), Element->Name).ToCString());
				continue;
			}
		}
	}

	return true;
}
