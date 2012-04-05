//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEObject.cpp
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

#include "ZEObject.h"
#include <string.h>
#include "ZEError.h"
#include "ZEAnimation.h"
#include "ZECore/ZEConsole.h"
#include "ZESerialization/ZESerializer.h"
#include "ZESerialization/ZEUnserializer.h"

#define ZE_CLSF_CLASS_CHUNKID ((ZEUInt32)'CLAS')

struct ZEObjectFileChunk
{
	ZEUInt32		Header;
	char		ClassType[ZE_MAX_NAME_SIZE];
	ZEUInt32		PropertyCount;
};

bool ZEObjectDescription::CheckParent(ZEObjectDescription* Parent, ZEObjectDescription* Children)
{
	ZEObjectDescription* Current = Children;
	
	while (Current != NULL)
	{
		if (Current == Parent)
			return true;

		Current = Current->GetParent();
	}
	return false;
}

bool ZEObject::SetProperty(const char* PropertyName, const ZEVariant& Value)
{
	ZEInt PropertyId = GetPropertyId(PropertyName);

    if (PropertyId != -1)
	{
        return SetProperty(PropertyId, Value);
	}
	else
	{
		ZEUInt Hash = 0;
		ZESize	NameLength = strlen(PropertyName);

		for (ZESize I = 0; I < NameLength ; I++)
		{
			Hash += PropertyName[I];
		}

		for (ZESize I = 0; I < CustomProperties.GetCount(); I++)
		{
			if (CustomProperties[I].Hash == Hash && Value.GetType() == CustomProperties[I].Type)
			{
				CustomProperties[I].Value = Value;
				return true;
			}
		}
	}

	return false;
}

bool ZEObject::GetProperty(const char* PropertyName, ZEVariant& Value) const
{
	ZEInt PropertyId = GetPropertyId(PropertyName);

    if (PropertyId != -1)
        return GetProperty(PropertyId, Value);
	
	else
	{
		ZEUInt Hash = 0;
		ZESize	NameLength = strlen(PropertyName);

		for (ZESize I = 0; I < NameLength ; I++)
		{
			Hash += PropertyName[I];
		}

		for (ZESize I = 0; I < CustomProperties.GetCount(); I++)
		{
			if (CustomProperties[I].Hash == Hash)
			{
				Value = CustomProperties[I].Value;
				return true;
			}
		}
	}

    return false;
}

bool ZEObject::AddCustomProperty(ZERunTimeProperty Property)
{
	for (ZESize I = 0; I < CustomProperties.GetCount(); I++)
	{
		if (GetProperty(Property.Name, ZEVariant()))
		{
			zeLog("Property Already exists, Property name : %s", Property.Name);
			return false;
		}		
	}

	Property.Hash = 0;
	ZESize NameLength = strlen(Property.Name);

	for(ZESize I = 0; I < NameLength; I++)
		Property.Hash += Property.Name[I];

	CustomProperties.Add(Property);

	return true;
}

bool ZEObject::RemoveCustomProperty(const char* PropertyName)
{
	ZEUInt Hash = 0;
	ZESize	NameLength = strlen(PropertyName);

	for (ZESize I = 0; I < NameLength ; I++)
	{
		Hash += PropertyName[I];
	}

	for (ZESize I = 0; I < CustomProperties.GetCount(); I++)
	{
		if (CustomProperties[I].Hash == Hash)
		{
			CustomProperties.DeleteAt(I);
			return true;
		}
	}

	return false;
}

const ZEArray<ZERunTimeProperty>* ZEObject::GetCustomProperties() const
{
	return &CustomProperties;
}

bool ZEObject::AddToContainer(const char* ContainerName, ZEObject* Item)
{
	ZEInt ContainerId = GetContainerId(ContainerName);

	if (ContainerId != -1)
		return AddToContainer(ContainerId, Item);

	return false;
}

bool ZEObject::RemoveFromContainer(const char* ContainerName, ZEObject* Item)
{
	ZEInt ContainerId = GetContainerId(ContainerName);

	if (ContainerId != -1)
		return RemoveFromContainer(ContainerId, Item);

	return false;
}

const ZEObject** ZEObject::GetContainerItems(const char* ContainerName) const
{
	ZEInt ContainerId = GetContainerId(ContainerName);

	if (ContainerId != -1)
		return GetContainerItems(ContainerId);

	return NULL;
}

ZESize ZEObject::GetContainerItemCount(const char* ContainerName) const 
{
	ZEInt ContainerId = GetContainerId(ContainerName);

	if (ContainerId != -1)
		return GetContainerItemCount(ContainerId);

	return 0;
}

bool ZEObject::CallMethod(const char* MethodName, const ZEVariant* Parameters, ZESize ParameterCount, ZEVariant& ReturnValue)
{
	ZEInt MethodId = GetMethodId(MethodName);

	if (MethodId != -1)
		return CallMethod(MethodId, Parameters, ParameterCount, ReturnValue);

	return false;
}

bool ZEObject::CallMethod(ZEInt MethodId, const ZEArray<ZEVariant>& Parameters, ZEVariant& ReturnValue)
{
	return CallMethod(MethodId, Parameters.GetConstCArray(), Parameters.GetCount(), ReturnValue);
}

bool ZEObject::CallMethod(const char* MethodName, const ZEArray<ZEVariant>& Parameters, ZEVariant& ReturnValue)
{
	ZEInt MethodId = GetMethodId(MethodName);

	if (MethodId != -1)
		return CallMethod(MethodId, Parameters.GetConstCArray(), Parameters.GetCount(), ReturnValue);

	return false;
}


bool ZEObject::Serialize(ZESerializer* Serializer)
{
	ZEVariant Value;
	ZEObjectDescription* CurrDesc = GetDescription();
 
	ZEObjectFileChunk ClassChunk;
	ClassChunk.Header = ZE_CLSF_CLASS_CHUNKID;
	strncpy(ClassChunk.ClassType, CurrDesc->GetName(), ZE_MAX_NAME_SIZE);
	ClassChunk.PropertyCount = CurrDesc->GetPropertyCount() + CurrDesc->GetPropertyOffset();
	Serializer->Write(&ClassChunk, sizeof(ZEObjectFileChunk), 1);

	while (CurrDesc != NULL)
	{
		ZESize PropertyCount = CurrDesc->GetPropertyCount();
		if (PropertyCount == 0)
		{
			CurrDesc = CurrDesc->GetParent();
			continue;
		}	

		ZESize PropertyOffset = CurrDesc->GetPropertyOffset();
		const ZEPropertyDescription* Properties = CurrDesc->GetProperties();

		for (ZESize I = 0; I < PropertyCount; I++)
		{
			char NameBuffer[ZE_MAX_NAME_SIZE];
			memset(NameBuffer, 0, ZE_MAX_NAME_SIZE);
			strcpy(NameBuffer, Properties[I].Name);

			Serializer->Write((void*)NameBuffer, sizeof(char), ZE_MAX_NAME_SIZE);
			if (Properties[I].Access == (ZE_PA_READ | ZE_PA_WRITE))
				if (!GetProperty(PropertyOffset + I, Value))
				{
					zeError("Class does not have specified property. (Class Type Name : \"%s\", Property Name : \"%s\")", GetDescription()->GetName(), Properties[I].Name);
					return false;
				}

			Value.Serialize(Serializer);
		}

		CurrDesc = CurrDesc->GetParent();
	}

	return true;
}

bool ZEObject::Unserialize(ZEUnserializer* Unserializer)
{
	ZEObjectFileChunk ClassChunk;
	Unserializer->Read(&ClassChunk, sizeof(ZEObjectFileChunk), 1);
	
	if (ClassChunk.Header != ZE_CLSF_CLASS_CHUNKID)
	{
		zeError("Corrupted file. Class chunk id does not have matches. (Class Type Name : \"%s\")", GetDescription()->GetName());
		return false;
	}

	if (strncmp(ClassChunk.ClassType, GetDescription()->GetName(), ZE_MAX_NAME_SIZE) != 0)
	{
		zeError("Class type does not matches. (Expected Class Type Name : \"%s\" Given Class Type Name : \"%s\")", GetDescription()->GetName(), ClassChunk.ClassType);
		return false;
	}

	for (ZESize I = 0; I < ClassChunk.PropertyCount; I++)
	{
		char PropertyName[ZE_MAX_NAME_SIZE];
		Unserializer->Read(PropertyName, sizeof(char), ZE_MAX_NAME_SIZE);

		ZEVariant Value;
		Value.Unserialize(Unserializer);

		if (!SetProperty(PropertyName, Value))
		{
			zeError("Class does not have specified property. (Class Type Name : \"%s\", Property Name : \"%s\")", GetDescription()->GetName(), PropertyName);
			return false;
		}
	}

	return true;
}

void ZEObject::SetAnimationController(ZEAnimationController* AnimationController)
{
	this->AnimationController = AnimationController;
	AnimationController->SetOwner(this);
}

ZEAnimationController* ZEObject::GetAnimationController()
{
	return AnimationController;
}
												
ZEObject::ZEObject()
{
	AnimationController = NULL;
	CustomProperties.SetCount(0);
}

ZEObject::~ZEObject()
{
}


