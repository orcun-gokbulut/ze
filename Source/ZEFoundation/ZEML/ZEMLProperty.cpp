//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLProperty.cpp
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

#include "ZEMLProperty.h"
#include "ZEError.h"

ZEMLElementType1 ZEMLProperty::GetType()
{
	return ZEML_ET_PROPERTY;
}

ZESize ZEMLProperty::GetSize()
{
	return Value.SizeOf();
}

bool ZEMLProperty::SetValue(const ZEValue& Value)
{
	ZEMLElementType ValueType = ZEMLUtils::ConvertType(Value.GetType());
	if (ValueType == ZEML_ET_UNDEFINED)
	{
		zeError("Cannot set ZEMLProperty value. Unsupported ZEValue type.");
		return false;
	}

	this->Value = Value;

	return true;
}

const ZEValue& ZEMLProperty::GetValue() const
{
	return Value;
}

ZEMLValueType ZEMLProperty::GetValueType()
{
	return (ZEMLValueType)ZEMLUtils::ConvertType(Value.GetType());
}

ZEMLProperty::ZEMLProperty()
{
}

ZEMLProperty::ZEMLProperty(const char* Name)
{
	SetName(Name);
}

ZEMLProperty::ZEMLProperty(const char* Name ,const ZEValue& Value)
{
	SetName(NULL);
	SetValue(Value);
}
