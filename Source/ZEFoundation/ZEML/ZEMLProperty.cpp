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

ZEMLElementType ZEMLProperty::GetType()
{
	return ZEML_ET_PROPERTY;
}

ZEMLElement* ZEMLProperty::Clone()
{
	ZEMLProperty* CloneProperty = new ZEMLProperty(GetName());
	CloneProperty->SetValue(GetValue());
	CloneProperty->SetUserData(GetUserData());
	
	return CloneProperty;
}

ZEMLValueType ZEMLProperty::GetValueType()
{
	return (ZEMLValueType)ZEMLUtils::ConvertValueType(Value.GetType());
}

bool ZEMLProperty::SetValue(const ZEValue& Value)
{
	ZEMLValueType ValueType = ZEMLUtils::ConvertValueType(Value.GetType());
	if (ValueType == ZEML_VT_UNDEFINED)
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

void ZEMLProperty::SetFloat(float Value)
{
	SetValue(ZEValue(Value));
}

void ZEMLProperty::SetDouble(double Value)
{
	SetValue(ZEValue(Value));
}

void ZEMLProperty::SetInt8(ZEInt8 Value)
{
	SetValue(ZEValue(Value));
}

void ZEMLProperty::SetInt16(ZEInt16 Value)
{
	SetValue(ZEValue(Value));
}

void ZEMLProperty::SetInt32(ZEInt32 Value)
{
	SetValue(ZEValue(Value));
}

void ZEMLProperty::SetInt64(ZEInt64 Value)
{
	SetValue(ZEValue(Value));
}

void ZEMLProperty::SetUInt8(ZEUInt8 Value)
{
	SetValue(ZEValue(Value));
}

void ZEMLProperty::SetUInt16(ZEUInt16 Value)
{
	SetValue(ZEValue(Value));
}

void ZEMLProperty::SetUInt32(ZEUInt32 Value)
{
	SetValue(ZEValue(Value));
}

void ZEMLProperty::SetUInt64(ZEUInt64 Value)
{
	SetValue(ZEValue(Value));
}

void ZEMLProperty::SetBool(bool Value)
{
	SetValue(ZEValue(Value));
}

void ZEMLProperty::SetString(const char* Value)
{
	SetValue(ZEValue(Value));
}

void ZEMLProperty::SetQuaternion(const ZEQuaternion& Value)
{
	SetValue(ZEValue(Value));
}

void ZEMLProperty::SetVector2(const ZEVector2& Value)
{
	SetValue(ZEValue(Value));
}

void ZEMLProperty::SetVector2d(const ZEVector2d& Value)
{
	SetValue(ZEValue(Value));
}

void ZEMLProperty::SetVector3(const ZEVector3& Value)
{
	SetValue(ZEValue(Value));
}

void ZEMLProperty::SetVector3d(const ZEVector3d& Value)
{
	SetValue(ZEValue(Value));
}

void ZEMLProperty::SetVector4(const ZEVector4& Value)
{
	SetValue(ZEValue(Value));
}

void ZEMLProperty::SetVector4d(const ZEVector4d& Value)
{
	SetValue(ZEValue(Value));
}

void ZEMLProperty::SetMatrix3x3(const ZEMatrix3x3& Value)
{
	SetValue(ZEValue(Value));
}

void ZEMLProperty::SetMatrix3x3d(const ZEMatrix3x3d& Value)
{
	SetValue(ZEValue(Value));
}

void ZEMLProperty::SetMatrix4x4(const ZEMatrix4x4& Value)
{
	SetValue(ZEValue(Value));
}

void ZEMLProperty::SetMatrix4x4d(const ZEMatrix4x4d& Value)
{
	SetValue(ZEValue(Value));
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
	SetName(Name);
	SetValue(Value);
}
