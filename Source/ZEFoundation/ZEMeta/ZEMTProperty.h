//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMTProperty.h
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

#pragma once

#include "ZETypes.h"
#include "ZEMTType.h"
#include "ZEDS/ZEFlags.h"

class ZEClass;
struct ZEMTAttribute;

#define ZEMT_PROPERTY_SETTER(PropertyName) ZEMT_ATTRIBUTE(ZEMC.PropertyAccessor, Setter, #PropertyName)
#define ZEMT_PROPERTY_GETTER(PropertyName) ZEMT_ATTRIBUTE(ZEMC.PropertyAccessor, Getter, #PropertyName)
#define ZEMT_PROPERTY_COUNTER(PropertyName) ZEMT_ATTRIBUTE(ZEMC.PropertyAccessor, Counter, #PropertyName)
#define ZEMT_PROPERTY_ADDER(PropertyName) ZEMT_ATTRIBUTE(ZEMC.PropertyAccessor, Adder, #PropertyName)
#define ZEMT_PROPERTY_REMOVER(PropertyName) ZEMT_ATTRIBUTE(ZEMC.PropertyAccessor, Remover, #PropertyName)
#define ZEMT_PROPERTY_SETTER_ITEM(PropertyName) ZEMT_ATTRIBUTE(ZEMC.PropertyAccessor, SetterItem, #PropertyName)
#define ZEMT_PROPERTY_GETTER_ITEM(PropertyName) ZEMT_ATTRIBUTE(ZEMC.PropertyAccessor, GetterItem, #PropertyName)

enum ZEMTPropertyAccess
{
	ZEMT_PA_NONE			= 0x00,
	ZEMT_PA_READ			= 0x01,
	ZEMT_PA_WRITE			= 0x02,
	ZEMT_PA_READ_WRITE		= 0x03
};

enum ZEMTPropertyFlag
{
	ZEMT_PF_NONE			= 0x00,
	ZEMT_PF_GENERATED		= 0x01,
	ZEMT_PF_STATIC			= 0x02,
	ZEMT_PF_MUTABLE			= 0x04
};
typedef ZEFlags ZEMTPropertyFlags;

struct ZEMTProperty
{
	ZESize								ID;
	ZEClass*							MemberOf;

	const char*							Name;
	ZEUInt32							Hash;
	void*								OffsetOrAddress;
	ZEMTType							Type;

	ZEMTPropertyAccess					Access;
	ZEMTPropertyFlags					Flags;

	ZEMTAttribute*						Attributes;
	ZESize								AttributeCount;

	bool								IsGenerated() const;
	bool								IsStatic() const;
	bool								IsMutable() const;

	const ZEMTAttribute*				GetAttribute(const char* Name) const;
	const char*							GetAttributeValue(const char* AttributeName, ZESize Index = 0, const char* DefaultValue = "") const;

	bool								CheckAttribute(const char* Name) const;
	bool								CheckAttributeHasValue(const char* Name, const char* Value) const;
};
