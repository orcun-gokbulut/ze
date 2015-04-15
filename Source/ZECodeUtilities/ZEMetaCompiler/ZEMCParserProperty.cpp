//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMCParserProperty.cpp
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

#include "ZEMCParser.h"
#include "ZEMCOptions.h"
#include "ZEPointer\ZEPointer.h"

void ZEMCParser::ProcessProperty(ZEMCClass* ClassData, DeclaratorDecl* PropertyDeclaration)
{
	if (PropertyDeclaration->getAccess() != AccessSpecifier::AS_public)
		return;

	std::string Name = PropertyDeclaration->getNameAsString();
	ZEMCType PropertyType;
	if (!ProcessType(PropertyType, PropertyDeclaration->getType()))
		return;

	// Only value qualified types can be container
	if (PropertyType.TypeQualifier != ZEMC_TQ_VALUE && PropertyType.TypeQualifier != ZEMC_TQ_CONST_VALUE)
		return;

	ZEPointer<ZEMCProperty> PropertyData = new ZEMCProperty();
	PropertyData->Name = PropertyDeclaration->getNameAsString();
	PropertyData->Hash = PropertyData->Name.Hash();
	PropertyData->IsStatic = isa<VarDecl>(PropertyDeclaration);
	PropertyData->IsContainer = PropertyType.ContainerType != ZEMC_CT_NONE;
	PropertyData->Type = PropertyType;
	PropertyData->Access = PropertyType.TypeQualifier == ZEMC_TQ_VALUE ? ZEMC_PA_READ_WRITE : ZEMC_PA_READ;

	ParseAttributes(PropertyData, PropertyDeclaration);

	ClassData->Properties.Add(PropertyData.Transfer());
}

#define GET_ATTRIBUTE(Attributes, Name) 0
#define CHECK_ATTRIBUTE(Attributes, Name) false

static void WipeAccessor(ZEArray<ZEMCAccessor>& Accessors, ZEString Name)
{
	for (ZESize N = 0; N < Accessors.GetCount(); N++)
	{
		if (Accessors[N].Name == Name)
		{
			Accessors.Remove(N);
			N--;
		}
	}
}

void ZEMCParser::FilterPropertyAccessors(ZEMCClass* ClassData, ZEArray<ZEMCAccessor>& Accessors)
{
	// Wipe Faulty Ones
	for (ZESize I = 0; I < Accessors.GetCount(); I++)
	{
		for (ZESize N = 0; N < Accessors.GetCount(); N++)
		{
			if (N == I)
				continue;

			if (Accessors[I].Name != Accessors[N].Name)
				continue;

			if 	(Accessors[I].Type == Accessors[N].Type || Accessors[I].PropertyType != Accessors[N].PropertyType)
			{
				WipeAccessor(Accessors, Accessors[I].Name);
				I = 0;
				break;
			}
		}
	}

	// Wipe Already Property
	for (ZESize I = 0; I < ClassData->Properties.GetCount(); I++)
	{
		for (ZESize N = 0; N < Accessors.GetCount(); N++)
		{
			if (ClassData->Properties[I]->Name == Accessors[N].Name)
			{
				WipeAccessor(Accessors, Accessors[N].Name);
				break;
			}
		}
	}
}

void ZEMCParser::ProcessPropertyAccessor(ZEArray<ZEMCAccessor>& Accessors, ZEMCMethod* MethodData)
{
	if (MethodData->Name.GetLength() <= 3)
		return;

	if (MethodData->CheckAttribute("NotAccessor"))
		return;

	if (MethodData->Name.Left(3) == "Set")
	{
		if (MethodData->Parameters.GetCount() != 1)
			return;

		ZEMCAccessor Accessor;
		Accessor.Name = MethodData->Name.Right(MethodData->Name.GetLength() - 3);
		Accessor.Type = ZEMC_AT_SETTER;
		Accessor.PropertyType = MethodData->Parameters[0].Type;
		Accessor.PropertyType.TypeQualifier = ZEMC_TQ_VALUE;
		Accessor.Method = MethodData;
		Accessors.Add(Accessor);

	}
	else if (MethodData->Name.Left(3) == "Get")
	{
		if (MethodData->Parameters.GetSize() != 0)
			return;
		
		if (MethodData->ReturnValue.BaseType == ZEMC_BT_VOID)
			return;

		ZEMCAccessor Accessor;
		Accessor.Name = MethodData->Name.Right(MethodData->Name.GetLength() - 3);
		Accessor.Type = ZEMC_AT_GETTER;
		Accessor.PropertyType = MethodData->ReturnValue;
		Accessor.PropertyType.TypeQualifier = ZEMC_TQ_VALUE;
		Accessor.Method = MethodData;
		Accessors.Add(Accessor);
	}
}

void ZEMCParser::ProcessPropertyAccessors(ZEMCClass* ClassData)
{
	ZEArray<ZEMCAccessor> Accessors;
	const ZEArray<ZEMCMethod*>& Methods = ClassData->Methods;
	for (ZESize I = 0; I < Methods.GetCount(); I++)
		ProcessPropertyAccessor(Accessors, Methods[I]);

	FilterPropertyAccessors(ClassData, Accessors);

	for (ZESize I = 0; I < Accessors.GetCount(); I++)
	{
		if (Accessors[I].PropertyType.ContainerType != ZEMC_CT_NONE)
			continue;

		ZEMCAccessor& AccessorData = Accessors[I];
		ZEMCProperty* PropertyData = NULL;
		for (ZESize N = 0; N < ClassData->Properties.GetCount(); N++)
		{
			if (ClassData->Properties[N]->Name == AccessorData.Name)
			{
				PropertyData = ClassData->Properties[N];
				break;
			}
		}

		if (PropertyData == NULL)
		{
			PropertyData = new ZEMCProperty();
			PropertyData->Name = AccessorData.Name;
			PropertyData->Type = AccessorData.PropertyType;
			PropertyData->IsContainer = AccessorData.PropertyType.ContainerType != ZEMC_CT_NONE;
			PropertyData->Hash = PropertyData->Name.Hash();
			ClassData->Properties.Add(PropertyData);
		}

		switch (Accessors[I].Type)
		{
			case ZEMC_AT_SETTER:
				PropertyData->HasAccessors = true;
				PropertyData->Setter = AccessorData.Method;
				PropertyData->Access = (ZEMCPropertyAccess)(PropertyData->Access | ZEMC_PA_WRITE);
				break;

			case ZEMC_AT_GETTER:
				PropertyData->HasAccessors = true;
				PropertyData->Getter = AccessorData.Method;
				PropertyData->Access = (ZEMCPropertyAccess)(PropertyData->Access | ZEMC_PA_READ);
				break;

			default:
				break;
		}
	}


}
