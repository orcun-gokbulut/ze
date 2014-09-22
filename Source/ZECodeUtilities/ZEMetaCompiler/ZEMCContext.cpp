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
	if (A.BaseType != B.BaseType && A.TypeQualifier != B.TypeQualifier)
		return false;

	if (A.ContainerType == B.ContainerType)
		return false;

	if (A.BaseType == ZEMC_BT_UNDEFINED)
		return false;
	else if ((A.BaseType == ZEMC_BT_OBJECT || A.BaseType == ZEMC_BT_OBJECT_PTR) &&  A.Class != B.Class)
		return false;
	else if (A.BaseType == ZEMC_BT_ENUMERATOR && A.Enumurator != B.Enumurator)
		return false;

	return true;
}

ZEMCType::ZEMCType()
{
	BaseType = ZEMC_BT_UNDEFINED;
	TypeQualifier = ZEMC_TQ_VALUE;
	ContainerType = ZEMC_CT_NONE;
	Class = NULL;
	Enumurator = NULL;
}

ZEMCType::~ZEMCType()
{

}

ZEMCForwardDeclaration::ZEMCForwardDeclaration()
{

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

}

ZEMCAttribute::~ZEMCAttribute()
{
	for (ZESize I = 0; I < Parameters.GetCount(); I++)
		delete Parameters[I];

	Parameters.Clear();
}

ZEMCEnumerator::ZEMCEnumerator()
{
	ZEUInt32 Hash = 0;
	ZEMCClass* BaseClass = NULL;
}

ZEMCEnumerator::~ZEMCEnumerator()
{
	for (ZESize I = 0; I < Items.GetCount(); I++)
		delete Items[I];

	Items.Clear();
}

ZEMCDeclaration::ZEMCDeclaration()
{
	Hash = 0;
}

ZEMCDeclaration::~ZEMCDeclaration()
{
	for (ZESize I = 0; I < Attributes.GetCount(); I++)
		delete Attributes[I];

	Attributes.Clear();
}

ZEMCProperty::ZEMCProperty()
{
	ID = 0;
	HasSetterGetter = false;
	IsStatic = false;
	IsContainer = false;
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

	IsVirtual = false;
	IsPure = false;
	IsStatic = false;
	IsEvent = false;
	IsConst = false;

	IsOperator = false;
	OperatorType = ZEMC_MOT_NOT_OPERATOR;
}

ZEMCMethod::~ZEMCMethod()
{
	for (ZESize I = 0; I < Parameters.GetCount(); I++)
		delete Parameters[I];

	Parameters.Clear();
}

ZEMCClass::ZEMCClass()
{
	BaseClass = NULL;
	HasScriptBase = false;
	HasPublicConstructor = false;
	HasPublicCopyConstructor = false;
	HasCreateInstanceMethod = false;
	IsAbstract = false;
	IsBuiltInClass = false;
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
	TargetDeclarations.Clear();

	for (ZESize I = 0; I < Declarations.GetCount(); I++)
		delete Declarations[I];

	Declarations.Clear();

	for (ZESize I = 0; I < Enumurators.GetCount(); I++)
		delete Enumurators[I];
	
	Enumurators.Clear();

	for (ZESize I = 0; I < ForwardDeclarations.GetCount(); I++)
		delete ForwardDeclarations[I];

	ForwardDeclarations.Clear();
}
