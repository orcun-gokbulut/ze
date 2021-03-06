//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMCContext.cpp
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

#include "ZEMCContext.h"

bool ZEMCType::operator==(const ZEMCType& Other) const
{
	return Equal(*this, Other);
}

bool ZEMCType::operator!=(const ZEMCType& Other) const
{
	return !Equal(*this, Other);
}

bool ZEMCType::Equal(const ZEMCType& A, const ZEMCType& B)
{
	if (A.BaseType != B.BaseType || A.TypeQualifier != B.TypeQualifier || 
		A.CollectionType != B.CollectionType || A.CollectionQualifier != B.CollectionQualifier)
	{
		return false;
	}

	if (A.BaseType == ZEMC_BT_OBJECT || A.BaseType == ZEMC_BT_OBJECT_PTR || A.BaseType == ZEMC_BT_OBJECT_HOLDER) 
	{
		if (A.Class != B.Class)
			return false;
	}

	if (A.BaseType == ZEMC_BT_ENUMERATOR || A.BaseType == ZEMC_BT_FLAGS) 
	{
		if (A.Enumerator != B.Enumerator)
			return false;
	}

	return true;
}

bool ZEMCType::Check()
{
	if (BaseType == ZEMC_BT_OBJECT || BaseType == ZEMC_BT_OBJECT_PTR || BaseType == ZEMC_BT_OBJECT_HOLDER)
	{
		if (Class == NULL)
			return false;
	}

	if (BaseType == ZEMC_BT_ENUMERATOR || BaseType == ZEMC_BT_FLAGS)
	{
		if (Enumerator == NULL)
			return false;
	}

	if (CollectionType == ZEMC_CT_ARRAY)
	{
		if (TypeQualifier == ZEMC_TQ_REFERENCE || TypeQualifier == ZEMC_TQ_CONST_REFERENCE)
			return false;
	}
	else
	{
		if (CollectionType != ZEMC_CT_NONE)
			return false;
	}

	return true;
}

ZEMCType::ZEMCType()
{
	BaseType = ZEMC_BT_UNDEFINED;
	TypeQualifier = ZEMC_TQ_VALUE;
	CollectionType = ZEMC_CT_NONE;
	CollectionQualifier = ZEMC_TQ_VALUE;
	Class = NULL;
	Enumerator = NULL;
}

ZEMCType::~ZEMCType()
{

}

ZEMCForwardDeclaration::ZEMCForwardDeclaration()
{
	Type = ZEMC_DT_NONE;
}

ZEMCForwardDeclaration::~ZEMCForwardDeclaration()
{

}

ZEMCEnumeratorItem::ZEMCEnumeratorItem()
{

}

ZEMCEnumeratorItem::~ZEMCEnumeratorItem()
{

}

ZEMCAttribute::ZEMCAttribute()
{
	Owner = NULL;
}

ZEMCAttribute::~ZEMCAttribute()
{
}

ZEMCEnumerator::ZEMCEnumerator()
{
	ZEUInt32 Hash = 0;
	ZEMCClass* BaseClass = NULL;
}

ZEMCEnumerator::~ZEMCEnumerator()
{
}

void ZEMCDeclaration::NormalizeAttributeStack()
{
	Attributes.Clear();
	for (ZESize I = 0; I < AttributeStack.GetCount(); I++)
	{
		if (AttributeStack[I].Name.IsEmpty() ||
			AttributeStack[I].Name[0] == "*" ||
			AttributeStack[I].Name[0] == "@" ||
			AttributeStack[I].Name[0] == "~")
		{
			continue;
		}

		bool Found = false;
		for (ZESize N = 0; N < Attributes.GetCount(); N++)
		{
			if (Attributes[N].Name == AttributeStack[I].Name)
			{
				Attributes[N] = AttributeStack[I];
				Found = true;
				break;
			}
		}

		if (!Found)
			Attributes.Add(AttributeStack[I]);
	}
}

ZEMCAttribute* ZEMCDeclaration::GetAttribute(const char* Name)
{
	for (ZESize I = 0; I < Attributes.GetCount(); I++)
	{
		if (Attributes[I].Name == Name)
			return &Attributes[I];
	}

	return NULL;
}

const char* ZEMCDeclaration::GetAttributeValue(const char* Name, ZESize Index, const char* DefaultValue)
{
	ZEMCAttribute* Attribute = GetAttribute(Name);

	if (Attribute == NULL)
		return NULL;

	if (Index >= Attribute->Values.GetCount())
		return false;

	return Attribute->Values[Index].ToCString();
}


bool ZEMCDeclaration::CheckAttribute(const char* Name)
{
	return GetAttribute(Name) != NULL;
}

bool ZEMCDeclaration::CheckAttributeValue(const char* Name, const char* TestValue, ZESize Index, const char* DefaultValue)
{
	if (TestValue == NULL)
		return false;

	ZEMCAttribute* Attribute = GetAttribute(Name);
	if (Attribute == NULL)
	{
		if (DefaultValue == NULL)
			return false;

		return (strcmp(TestValue, DefaultValue) == 0);
	}

	if (Index >= Attribute->Values.GetCount())
	{
		if (DefaultValue == NULL)
			return false;

		return (strcmp(TestValue, DefaultValue) == 0);
	}

	return (strcmp(Attribute->Values[Index], TestValue) == 0);
}

bool ZEMCDeclaration::CheckAttributeHasValue(const char* Name, const char* Value)
{
	ZEMCAttribute* Attribute = GetAttribute(Name);

	if (Attribute == NULL)
		return NULL;

	for (ZESize I = 0; Attribute->Values.GetCount(); I++)
	{
		if (Attribute->Values[I] == Value)
			return true;
	}

	return false;
}

ZEMCDeclaration::ZEMCDeclaration()
{
	Hash = 0;
}

ZEMCDeclaration::~ZEMCDeclaration()
{

}

ZEMCProperty::ZEMCProperty()
{
	ID = 0;
	Class = NULL;
	Access = ZEMC_PA_NONE;
	HasAccessors = false;
	IsStatic = false;
	IsContainer = false;
	Getter = NULL;
	Setter = NULL;
	Adder = NULL;
	Remover = NULL;
	Counter = NULL;
}

ZEMCProperty::~ZEMCProperty()
{

}

ZEMCMethodParameter::ZEMCMethodParameter()
{

}

ZEMCMethodParameter::~ZEMCMethodParameter()
{

}

ZEMCMethod::ZEMCMethod()
{
	ID = 0;
	Class = NULL;
	IsVirtual = false;
	IsPure = false;
	IsStatic = false;
	IsEvent = false;
	IsConst = false;
	IsConstructor = false;

	IsOperator = false;
	OperatorType = ZEMC_OT_NONE;
}

ZEMCMethod::~ZEMCMethod()
{

}

ZEMCClass::ZEMCClass()
{
	BaseClass = NULL;
	HasScriptBase = false;
	HasPublicDefaultConstructor = true;
	HasPublicDestructor = false;
	HasPublicCopyConstructor = true;
	HasCreateInstanceMethod = false;
	HasPublicAssignmentOperator = true;
	HasPublicDestroyMethod = false;
	IsAbstract = false;
	IsFundamental = false;
	IsForwardDeclared = false;
}

ZEMCClass::~ZEMCClass()
{
	for (ZESize I = 0; I < Properties.GetCount(); I++)
		delete Properties[I];
	Properties.Clear();

	for (ZESize I = 0; I < Methods.GetCount(); I++)
		delete Methods[I];
	Methods.Clear();
}

ZEMCContext::ZEMCContext()
{

}

ZEMCContext::~ZEMCContext()
{
	for (ZESize I = 0; I < Classes.GetCount(); I++)
		delete Classes[I];
	Classes.Clear();

	for (ZESize I = 0; I < Enumerators.GetCount(); I++)
		delete Enumerators[I];
	Enumerators.Clear();

	for (ZESize I = 0; I < ForwardDeclarations.GetCount(); I++)
		delete ForwardDeclarations[I];
	ForwardDeclarations.Clear();

	for (ZESize I = 0; I < Includes.GetCount(); I++)
		delete Includes[I];
	Includes.Clear();

	TargetClasses.Clear();
	TargetEnumerators.Clear();
}
