//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMTDeclaration.cpp
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

#include "ZEMTDeclaration.h"

#include "ZEGUID.h"
#include "ZEAttribute.h"

#include <string.h>


const ZEGUID& ZEMTDeclaration::GetGUID() const
{
	return ZEGUID::Zero;
}

const ZEAttribute* ZEMTDeclaration::GetAttribute(const char* Name) const
{
	ZESize AttributeCount = GetAttributeCount();
	const ZEAttribute* Attributes = GetAttributes();

	for (ZESize I = 0; I < AttributeCount; I++)
	{
		if (strcmp(Attributes[I].Name, Name) == 0)
			return &Attributes[I];
	}

	return NULL;
}

const char* ZEMTDeclaration::GetAttributeValue(const char* Name, ZESize Index, const char* DefaultValue) const
{
	const ZEAttribute* Attribute = GetAttribute(Name);
	if (Attribute == NULL)
		return DefaultValue;

	if (Attribute->ValueCount < Index)
		return DefaultValue;

	return Attribute->Values[Index];
}

bool ZEMTDeclaration::CheckAttribute(const char* Name) const
{
	const ZEAttribute* Attribute = GetAttribute(Name);
	if (Attribute != NULL)
		return true;

	return false;
}

bool ZEMTDeclaration::CheckAttributeHasValue(const char* Name, const char* Value) const
{
	const ZEAttribute* Attribute = GetAttribute(Name);
	if (Attribute == NULL)
		return false;

	for (ZESize I = 0; I < Attribute->ValueCount; I++)
	{
		if (strcmp(Attribute->Values[I], Value) != 0)
			return true;
	}

	return false;
}
