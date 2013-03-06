//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMetaData.cpp
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

#include "ZEMetaData.h"

bool ZEMetaType::operator==(const ZEMetaType& Other) const
{
	return Equal(*this, Other);
}

bool ZEMetaType::operator!=(const ZEMetaType& Other) const
{
	return !Equal(*this, Other);
}

bool ZEMetaType::Equal(const ZEMetaType& A, const ZEMetaType& B)
{
	if (A.Type != B.Type || A.TypeQualifier != B.TypeQualifier)
		return false;
	else if (A.Type == ZE_MTT_OBJECT && A.ClassData != B.ClassData)
		return false;
	else if (A.Type == ZE_MTT_LIST || A.Type == ZE_MTT_ARRAY)
	{
		if (A.SubType != B.SubType || A.SubTypeQualifier != B.SubTypeQualifier)
			return false;
		if (A.SubType == ZE_MTT_OBJECT && A.ClassData != B.ClassData)
			return false;
	}

	return true;
}

ZEMetaType::ZEMetaType()
{
	Type = ZE_MTT_UNDEFINED;
	TypeQualifier = ZE_MTQ_VALUE;
	SubType = ZE_MTT_UNDEFINED;
	SubTypeQualifier = ZE_MTQ_VALUE;
	ClassData = new ZEClassData();
	ClassData->BaseClass = new ZEClassData();
}

ZEMetaType::ZEMetaType(ZEMetaTypeType Type, ZEMetaTypeQualifier TypeQualifier, ZEMetaTypeType SubType, ZEMetaTypeQualifier SubTypeQualifier, ZEString ClassName, ZEClassData* Class)
{
	this->Type = Type;
	this->TypeQualifier = TypeQualifier;
	this->SubType = SubType;
	this->SubTypeQualifier = SubTypeQualifier;
	this->ClassData = Class;
	this->ClassData->Name = ClassName;
}

ZEMetaType::ZEMetaType(ZEMetaTypeType Type, ZEMetaTypeQualifier TypeQualifier, ZEString ClassName, ZEClassData* ClassData)
{
	this->Type = Type;
	this->TypeQualifier = TypeQualifier;
	this->SubType = ZE_MTT_UNDEFINED;
	this->SubTypeQualifier = ZE_MTQ_VALUE;
	this->ClassData = ClassData;
	this->ClassData->Name = ClassName;
}

ZEEnumData::~ZEEnumData()
{
	ZEArray<ZEEnumParameterData*>::Iterator EnumParameterIterator = Parameters.GetIterator();
	while(!EnumParameterIterator.IsEnd())
	{
		delete EnumParameterIterator.GetItem();
		EnumParameterIterator.MoveNext();
	}
	Parameters.Clear();
}

ZETypeData::~ZETypeData()
{
	ZEArray<ZEAttributeData*>::Iterator AttributeIterator = Attributes.GetIterator();
	while(!AttributeIterator.IsEnd())
	{
		delete AttributeIterator.GetItem();
		AttributeIterator.MoveNext();
	}
	Attributes.Clear();
}

ZEEventData::~ZEEventData()
{
	ZEArray<ZEEventParameterData*>::Iterator ParameterIterator = Parameters.GetIterator();
	while(!ParameterIterator.IsEnd())
	{
		delete ParameterIterator.GetItem();
		ParameterIterator.MoveNext();
	}
	Parameters.Clear();
}

ZEMethodData::~ZEMethodData()
{
	ZEArray<ZEMethodParameterData*>::Iterator ParameterIterator = Parameters.GetIterator();
	while(!ParameterIterator.IsEnd())
	{
		delete ParameterIterator.GetItem();
		ParameterIterator.MoveNext();
	}
	Parameters.Clear();
}

ZEClassData::~ZEClassData()
{
	ZEArray<ZEPropertyData*>::Iterator PropertyIterator = Properties.GetIterator();
	while(!PropertyIterator.IsEnd())
	{
		delete PropertyIterator.GetItem();
		PropertyIterator.MoveNext();
	}
	Properties.Clear();

	ZEArray<ZEMethodData*>::Iterator MethodIterator = Methods.GetIterator();
	while(!MethodIterator.IsEnd())
	{
		delete MethodIterator.GetItem();
		MethodIterator.MoveNext();
	}
	Methods.Clear();
}

ZEMetaData::~ZEMetaData()
{
	ZEArray<ZETypeData*>::Iterator Iterator = TargetTypes.GetIterator();

	while(!Iterator.IsEnd())
	{
		delete Iterator.GetItem();
		Iterator.MoveNext();
	}

	TargetTypes.Clear();
	Types.Clear();
}
