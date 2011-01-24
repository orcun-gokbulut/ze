//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEContainer.cpp
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

#include "ZEContainer.h"
#include "ZEClass.h"

void ZEContainer::SetContainerMode(ZEContainerMode Mode)
{
	this->Mode = Mode;
}

ZEContainerMode ZEContainer::GetContainerMode()
{
	return Mode;
}

void ZEContainer::SetBaseClass(ZEClassDescription* Type)
{
	this->Type = Type;
}

ZEClassDescription* ZEContainer::GetBaseClass()
{
	return Type;
}

void ZEContainer::SetAllowDerivedClasses(bool Allow)
{
	AllowDerivedTypes = Allow;
}

bool ZEContainer::GetAllowDerivedClasses()
{
	return AllowDerivedTypes;
}

const ZEArray<ZEClass*>& ZEContainer::GetInstances()
{
	return Instances;
}

bool ZEContainer::AddInstance(ZEClass* Instance)
{
	if (Type == NULL)
	{
		Instances.Add(Instance);
		return true;
	}

	if (AllowDerivedTypes)
	{
		ZEClassDescription* CurrentType = Instance->GetClassDescription();
		while (CurrentType != NULL)
		{
			if (Type == CurrentType)
			{
				Instances.Add(Instance);
				return true;
			}
			CurrentType = CurrentType->GetParent();
		}
	}
	else
	{
		if (Type == Instance->GetClassDescription())
		{
			Instances.Add(Instance);
			return true;
		}
		else
			return false;
	}
}

bool ZEContainer::RemoveInstance(ZEClass* Instance)
{
	Instances.DeleteValue(Instance);
	return true;
}

ZEContainer::ZEContainer()
{
	Mode = ZE_CM_OWNER;
	Type = NULL;
	AllowDerivedTypes = true;
}

ZEContainer::ZEContainer(ZEContainerMode Mode, ZEClassDescription* Type, bool AllowDerived)
{
	this->Mode = Mode;
	this->Type = Type;
	this->AllowDerivedTypes = AllowDerived;
}

ZEContainer::~ZEContainer()
{
	if (Mode == ZE_CM_OWNER)
		for (size_t I = 0; I < Instances.GetCount(); I++)
			delete Instances[I];
	
	Instances.Clear();
}
