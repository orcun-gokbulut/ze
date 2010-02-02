//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Class.cpp
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

#include "Class.h"
#include <string.h>
#include "Core/Error.h"
#include "Animation.h"

#define ZE_CLSF_CLASS_CHUNKID ((ZEDWORD)'CLAS')

struct ZEClassFileChunk
{
	ZEDWORD		Header;
	char		ClassType[ZE_MAX_NAME_SIZE];
	ZEDWORD		PropertyCount;
};


bool ZEClass::Serialize(ZESerializer* Serializer)
{
	ZEVariant Value;
	ZEClassDescription* CurrDesc = GetClassDescription();
 
	ZEClassFileChunk ClassChunk;
	ClassChunk.Header = ZE_CLSF_CLASS_CHUNKID;
	strncpy(ClassChunk.ClassType, CurrDesc->GetName(), ZE_MAX_NAME_SIZE);
	ClassChunk.PropertyCount = CurrDesc->GetPropertyCount() + CurrDesc->GetPropertyOffset();
	Serializer->Write(&ClassChunk, sizeof(ZEClassFileChunk), 1);

	while (CurrDesc != NULL)
	{
		size_t PropertyCount = CurrDesc->GetPropertyCount();
		if (PropertyCount == 0)
		{
			CurrDesc = CurrDesc->GetParent();
			continue;
		}	

		size_t PropertyOffset = CurrDesc->GetPropertyOffset();
		const ZEPropertyDescription* Properties = CurrDesc->GetProperties();

		for (size_t I = 0; I < PropertyCount; I++)
		{
			Serializer->Write((void*)Properties[I].Name, sizeof(char), ZE_MAX_NAME_SIZE);
			if (Properties[I].Access == ZE_PA_READWRITE)
				if (!GetProperty(PropertyOffset + I, Value))
				{
					zeError("Class Serialize", "Class does not have specified property. (Class Type Name : \"%s\", Property Name : \"%s\")", GetClassDescription()->GetName(), Properties[I].Name);
					return false;
				}

			Value.Serialize(Serializer);
		}

		CurrDesc = CurrDesc->GetParent();
	}

	return true;
}

bool ZEClass::Unserialize(ZEUnserializer* Unserializer)
{
	ZEClassFileChunk ClassChunk;
	Unserializer->Read(&ClassChunk, sizeof(ZEClassFileChunk), 1);
	
	if (ClassChunk.Header != ZE_CLSF_CLASS_CHUNKID)
	{
		zeError("Class Unserialize", "Corrupted file. Class chunk id does not have matches. (Class Type Name : \"%s\")", GetClassDescription()->GetName());
		return false;
	}

	if (strncmp(ClassChunk.ClassType, GetClassDescription()->GetName(), ZE_MAX_NAME_SIZE) != 0)
	{
		zeError("Class Unserialize", "Class type does not matches. (Expected Class Type Name : \"%s\" Given Class Type Name : \"%s\")", GetClassDescription()->GetName(), ClassChunk.ClassType);
		return false;
	}

	for (size_t I = 0; I < ClassChunk.PropertyCount; I++)
	{
		char PropertyName[ZE_MAX_NAME_SIZE];
		Unserializer->Read(PropertyName, sizeof(char), ZE_MAX_NAME_SIZE);

		ZEVariant Value;
		Value.Unserialize(Unserializer);

		if (!SetProperty(PropertyName, Value))
		{
			zeError("Class Unserialize", "Class does not have specified property. (Class Type Name : \"%s\", Property Name : \"%s\")", GetClassDescription()->GetName(), PropertyName);
			return false;
		}
	}

	return true;
}

void ZEClass::SetAnimationController(ZEAnimationController* AnimationController)
{
	this->AnimationController = AnimationController;
	AnimationController->SetOwner(this);
}

ZEAnimationController* ZEClass::GetAnimationController()
{
	return AnimationController;
}
												
ZEClass::ZEClass()
{
	AnimationController = NULL;
}
