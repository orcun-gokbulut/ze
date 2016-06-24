//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMCGeneratorDeclaration.cpp
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

#include "ZEMCGenerator.h"
#include "ZEMCContext.h"

void ZEMCGenerator::GenerateDeclarationGetName(ZEMCDeclaration* Declaration)
{
	WriteToFile(
		"const char* %s::GetName() const\n"
		"{\n"
		"\treturn \"%s\";\n"
		"}\n\n",
		Declaration->MetaName.ToCString(), 
		Declaration->Name.ToCString());
}

void ZEMCGenerator::GenerateDeclarationGetGUID(ZEMCDeclaration* Declaration)
{
	WriteToFile(
		"const ZEGUID& %s::GetGUID() const\n"
		"{\n"
		"\treturn ZEGUID::Zero;\n"
		"}\n\n", 
		Declaration->MetaName.ToCString());
}

void ZEMCGenerator::GenerateDeclarationGetAttributes(ZEMCDeclaration* Declaration)
{
	WriteToFile(
		"const ZEAttribute* %s::GetAttributes() const\n"
		"{\n", 
		Declaration->MetaName.ToCString());

	// Attribute Parameters
	for(ZESize I = 0; I < Declaration->Attributes.GetCount(); I++)
	{
		ZEMCAttribute* CurrentAttribute = &Declaration->Attributes[I];
		if (CurrentAttribute->Values.GetCount() == 0)
			continue;

		WriteToFile(
			"\tstatic const char* Attribute%dParameters[%d] = {", 
			I, CurrentAttribute->Values.GetCount());

		for(ZESize J = 0; J < CurrentAttribute->Values.GetCount(); J++)
		{
			WriteToFile("\"%s\"%s", 
				CurrentAttribute->Values[J].ToCString(), 
				J != CurrentAttribute->Values.GetCount() - 1 ? ", " : "");
		}

		WriteToFile("};\n");
	}

	WriteToFile("\n");

	// Attribute List
	if (Declaration->Attributes.GetCount() > 0)
	{
		if(Declaration->Attributes.GetCount() > 0)
		{
			WriteToFile(
				"\tstatic ZEAttribute Attributes[%d] =\n"
				"\t{\n", Declaration->Attributes.GetCount());
		}

		for(ZESize I = 0; I < Declaration->Attributes.GetCount(); I++)
		{
			ZEMCAttribute* CurrentAttribute = &Declaration->Attributes[I];
			if (CurrentAttribute->Values.GetCount() > 0)
			{
				WriteToFile(
					"\t\t{\"%s\", Attribute%dParameters, %d}",
					CurrentAttribute->Name.ToCString(), 
					I, 
					CurrentAttribute->Values.GetCount());
			}
			else
			{
				WriteToFile("\t\t{\"%s\", NULL, 0}", CurrentAttribute->Name.ToCString());
			}

			WriteToFile("%s\n", I < Declaration->Attributes.GetCount() - 1 ? "," : "");
		}

		WriteToFile(
			"\t};\n\n"
			"\treturn Attributes;\n");
	}
	else
	{
		WriteToFile("\treturn NULL;\n");
	}

	WriteToFile("}\n\n");
}

void ZEMCGenerator::GenerateDeclarationGetAttributeCount(ZEMCDeclaration* Declaration)
{
	WriteToFile( 
		"ZESize %s::GetAttributeCount() const\n"
		"{\n"
		"\treturn %d;\n"
		"}\n\n",
		Declaration->MetaName.ToCString(), 
		Declaration->Attributes.GetCount());
}

