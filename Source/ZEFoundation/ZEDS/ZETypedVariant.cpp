//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETypedVariant.cpp
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

///////////////////////////////////////////////////////////////
//  Orcun's Data Structres Version 2.0 (Zinek Engine)        //
// --------------------------------------------------------- //
//  Copyrights (C) 2005-2007,  Y. Orçun GÖKBULUT			 //
//  All rights reserved.									 //
///////////////////////////////////////////////////////////////

#include "ZEtypedvariant.h"
#include <string.h>
#include "ZEError.h"
#include <string.h>
#pragma warning(push)
#pragma warning(disable:4482 4996)

void ZETypedVariant::SetString(char *NewValue)
{
	zeDebugCheck(Type != ZE_VRT_STRING && Type != ZE_VRT_UNDEFINED, "ZETypedVariant::SetString operation failed. Types does not match.");
	if(Type == ZE_VRT_STRING || Type == ZE_VRT_MATRIX3X3 || Type == ZE_VRT_MATRIX4X4)
	{
		if (Value.String != NULL || Type == ZE_VRT_UNDEFINED)
			delete[] Value.String;
		Value.String = new char[strlen(NewValue) + 1];
		strcpy(Value.String, NewValue);
		return;
	}

	if (Type == ZE_VRT_UNDEFINED)
	{
		SetType(ZE_VRT_STRING);
		SetString(NewValue);
	}
}

void ZETypedVariant::SetInteger(ZEInt NewValue)
{
	zeDebugCheck(Type != ZE_VRT_INTEGER_32 && Type != ZE_VRT_UNDEFINED, "ZETypedVariant::SetInteger operation failed. Types does not match.");
	if (Type == ZE_VRT_INTEGER_32)
	{
		Value.Int32 = NewValue;
		return;
	}

	if (Type == ZE_VRT_UNDEFINED)
	{
		SetType(ZE_VRT_INTEGER_32);
		Value.Int32 = NewValue;
	}

}

void ZETypedVariant::SetFloat(float NewValue)
{
	zeDebugCheck(Type != ZE_VRT_FLOAT && Type != ZE_VRT_UNDEFINED, "ZETypedVariant::SetFloat operation failed. Types does not match.");
	if(Type == ZE_VRT_FLOAT)
	{
		Value.Float = NewValue;
		return;
	}

	if (Type == ZE_VRT_UNDEFINED)
	{
		SetType(ZE_VRT_FLOAT);
		Value.Float = NewValue;
	}
}

void ZETypedVariant::SetBoolean(bool NewValue)
{
	zeDebugCheck(Type != ZE_VRT_BOOLEAN && Type != ZE_VRT_UNDEFINED, "ZETypedVariant::SetBoolean operation failed. Types does not match.");
	if(Type == ZE_VRT_BOOLEAN)
	{
		Value.Boolean = NewValue;
		return;
	}

	if (Type == ZE_VRT_UNDEFINED)
	{
		SetType(ZE_VRT_BOOLEAN);
		Value.Boolean = NewValue;

	}
}

void ZETypedVariant::SetVariant(const ZEVariant &NewValue)
{
	zeDebugCheck(Type != NewValue.GetType() && Type != ZE_VRT_UNDEFINED, "ZETypedVariant::SetVariant operation failed. Types does not match.");
	if(Type == NewValue.GetType())
	{
		if(Type == ZE_VRT_STRING)
			SetString(NewValue.GetString());
		else
			Value = NewValue.GetValue();
		return;
	}

	if (Type == ZE_VRT_UNDEFINED)
	{
		SetType(NewValue.GetType());
		Value.Int32 = NewValue.GetValue().Int32;
	}
}

void ZETypedVariant::SetTypedVariant(const ZETypedVariant &NewValue)
{
	zeDebugCheck(Type != NewValue.GetType() && Type != ZE_VRT_UNDEFINED, "ZETypedVariant::SetTypedVariant operation failed. Types does not match.");
	if(Type == NewValue.GetType())
	{
		if(Type == ZE_VRT_STRING)
			SetString(NewValue.Value.String);
		else
			Value.Int32 = NewValue.Value.Int32;
		return;
	}

	if (Type == ZE_VRT_UNDEFINED)
	{
		SetType(NewValue.GetType());
		Value.Int32 = NewValue.GetValue().Int32;
	}
}

void ZETypedVariant::operator=(char* NewValue)
{
	SetString(NewValue);
}

void ZETypedVariant::operator=(ZEInt NewValue)
{
	SetInteger(NewValue);
}

void ZETypedVariant::operator=(float NewValue)
{
	SetFloat(NewValue);
}

void ZETypedVariant::operator=(bool NewValue)
{
	SetBoolean(NewValue);
}

void ZETypedVariant::operator=(ZEVariant& NewValue)
{
	SetVariant(NewValue);
}

void ZETypedVariant::operator=(ZETypedVariant& NewValue)
{
	SetTypedVariant(NewValue);
}

ZETypedVariant::ZETypedVariant()
{
	Type = ZE_VRT_UNDEFINED;
	Value.String = NULL;
}

ZETypedVariant::ZETypedVariant(ZEVariant& InitialValue)
{
	SetType(InitialValue.GetType());
	SetVariant(InitialValue);
}

ZETypedVariant::ZETypedVariant(ZETypedVariant& InitialValue)
{
	SetType(InitialValue.GetType());
	SetVariant(InitialValue);
}

ZETypedVariant::ZETypedVariant(char* InitialValue)
{
	SetType(ZE_VRT_STRING);
	SetString(InitialValue);
}
	
ZETypedVariant::ZETypedVariant(ZEInt InitialValue)
{
	SetType(ZE_VRT_INTEGER_32);
	SetInteger(InitialValue);
}

ZETypedVariant::ZETypedVariant(float InitialValue)
{
	SetType(ZE_VRT_FLOAT);
	SetFloat(InitialValue);
}

ZETypedVariant::ZETypedVariant(bool InitialValue)
{
	SetType(ZE_VRT_BOOLEAN);
	SetBoolean(InitialValue);
}

ZETypedVariant::~ZETypedVariant()
{
	SetType(ZE_VRT_UNDEFINED);
}
#pragma warning(pop)





