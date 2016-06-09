//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEProvider.cpp
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

#include "ZEProvider.h"

#include "ZEClass.h"
#include "ZEEnumerator.h"
#include "ZEML\ZEMLReader.h"
#include "ZEObject.h"

bool ZEProvider::RegisterClass(ZEClass* Class)
{
	if (Class == NULL)
		return false;

	if (Classes.Exists(Class))
		return false;

	RegisterClass(Class->GetParentClass());

	Classes.Add(Class);

	return true;
}

void ZEProvider::UnregisterClass(ZEClass* Class)
{
	Classes.RemoveValue(Class);
}

bool ZEProvider::RegisterEnumerator(ZEEnumerator* Enumerator)
{
	if (Enumerator == NULL)
		return false;

	if (Enumerators.Exists(Enumerator))
		return false;

	Enumerators.Add(Enumerator);

	return true;
}

void ZEProvider::UnregisterEnumerator(ZEEnumerator* Enumerator)
{
	Enumerators.RemoveValue(Enumerator);
}

const ZEArray<ZEClass*>& ZEProvider::GetClasses()
{
	return Classes;
}

ZEClass* ZEProvider::GetClass(const char* ClassName)
{
	for(ZESize I = 0; I < Classes.GetCount(); I++)
	{
		if(strcmp(Classes[I]->GetName(), ClassName) == 0)
			return Classes[I];
	}

	return NULL;
}

ZEArray<ZEClass*> ZEProvider::GetClass(ZEClass* ParentClass)
{
	ZEArray<ZEClass*> DerivedClasses;
	for (ZESize I = 0; I < Classes.GetCount(); I++)
	{
		if (ZEClass::IsDerivedFrom(ParentClass, Classes[I]))
			DerivedClasses.Add(Classes[I]);
	}

	return DerivedClasses;
}

ZEEnumerator* ZEProvider::GetEnumerator(const char* EnumeratorName)
{
	for(ZESize I = 0; I < Enumerators.GetCount(); I++)
	{
		if(Enumerators[I]->GetName() == EnumeratorName)
			return Enumerators[I];
	}

	return NULL;
}

ZEObject* ZEProvider::CreateInstance(const char* ClassName)
{
	ZEClass* Class = GetClass(ClassName);
	if (Class == NULL)
	{
		zeError("Cannot create object instace because class is not available. Class Name: \"%s\".", ClassName);
		return NULL;
	}

	return Class->CreateInstance();
}

ZEObject* ZEProvider::CreateDerivedInstance(ZEClass* BaseClass, const char* ClassName)
{
	ZEClass* Class = GetClass(ClassName);
	if (Class == NULL)
	{
		zeError("Cannot create object instace because base class is not available. Class Name: \"%s\".", ClassName);
		return NULL;
	}

	if (!ZEClass::IsDerivedFrom(BaseClass, Class))
	{
		zeError("Cannot create object instace because class is not derived from base class. Base Class Name: \"%s\", Class Name: \"%s\".", 
			BaseClass->GetName(), 
			ClassName);
		return NULL;
	}

	return Class->CreateInstance();
}

ZEObject* ZEProvider::CreateDerivedInstance(ZEClass* BaseClass, ZEMLReaderNode& ObjectNode)
{
	ZEObject* Object = CreateDerivedInstance(BaseClass, ObjectNode.ReadString("Class"));
	if (Object == NULL)
		return NULL;

	if (!Object->GetClass()->Unserialize(Object, ObjectNode))
	{
		zeError("Failed to unserialize class.");
		return false;
	}

	return Object;
}

ZEProvider* ZEProvider::GetInstance()
{
	static ZEProvider Provider;
	return &Provider;
}
