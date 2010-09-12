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
#include <string.h>

void ZEClassProvider::SetBaseClassType(ZEClassDescription* ClassType)
{
	BaseClassType = ClassType;
}

ZEClassDescription* ZEClassProvider::GetBaseClassType()
{
	return BaseClassType;
}

const ZEArray<ZEClassDescription*>& ZEClassProvider::GetClasses()
{
	return Classes;
}

bool ZEClassProvider::RegisterClass(ZEClassDescription* Description)
{
	zeAssert(Description == NULL, "Description can not be NULL.");

	if (Classes.Exists(Description))
	{
		zeError("Class Provider", "Can not add class type to provider. Class type is already exists.");
		return false;
	}


	if (BaseClassType != NULL && !ZEClassDescription::CheckParent(BaseClassType, Description))
	{
		zeError("Class Provider", "Can not add class type to provider. Class type is not derived from base class.");
		return false;
	}

	Classes.Add(Description);
	return true;
}

void ZEClassProvider::UnregisterClass(ZEClassDescription* Description)
{
	zeAssert(Description == NULL, "Description can not be NULL.");
	zeAssert(!Classes.Exists(Description), "Can not remove class type from provider. Class type is not exists. "
		"Class Type : \"%s\".", Description->GetName());

	Classes.DeleteValue(Description);
}

ZEClass* ZEClassProvider::CreateInstance(size_t Index) const
{
	return Classes[Index]->CreateInstance();
}

ZEClass* ZEClassProvider::CreateInstance(const char* Name) const
{
	for(size_t I = 0; I < Classes.GetCount(); I++)
		if (strcmp(Classes[I]->GetName(), Name) == 0)
			return Classes[I]->CreateInstance();

	zeError("Class Provider", "Can not create instance of a class. Class does not exists. Class Name : \"%s\".", Name);
	return NULL;
}

ZEClassProvider::ZEClassProvider()
{
	BaseClassType = NULL;
}

ZEClassProvider::~ZEClassProvider()
{

}
