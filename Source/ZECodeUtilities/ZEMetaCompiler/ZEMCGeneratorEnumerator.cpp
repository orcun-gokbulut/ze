//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMCGeneratorEnumerator.cpp
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

void ZEMCGenerator::GenerateEnumeratorMacros(ZEMCEnumerator* CurrentEnumerator)
{
	WriteToFile("\n\n");
	WriteToFile("// %s\n", CurrentEnumerator->Name.ToCString());
	WriteToFile("////////////////////////////////////////////////////////////////////////////////////////\n\n");

	WriteToFile("ZEMT_ENUMERATOR_IMPLEMENTATION(%s);\n", CurrentEnumerator->Name.ToCString());
	WriteToFile("ZEMT_ENUMERATOR_DECLARATION(%s);\n\n", CurrentEnumerator->Name.ToCString());
}

void ZEMCGenerator::GenerateEnumeratorGetItems(ZEMCEnumerator* CurrentEnumerator)
{
	WriteToFile("const ZEMTEnumeratorItem* %sEnumerator::GetItems() const\n"
		"{\n", CurrentEnumerator->Name.ToCString());

	if (CurrentEnumerator->Items.GetCount() != 0)
	{
		WriteToFile("\tstatic ZEMTEnumeratorItem Items[%d] =\n"
			"\t{\n", 
			CurrentEnumerator->Items.GetCount());

		for (ZESize I = 0; I < CurrentEnumerator->Items.GetCount(); I++)
		{
			ZEMCEnumeratorItem* CurrentItem = &CurrentEnumerator->Items[I];

			WriteToFile("\t\t{\"%s\", %d}%s\n", 
				CurrentItem->Name.ToCString(), 
				CurrentItem->Value,
				I + 1 != CurrentEnumerator->Items.GetCount() ? ", " : "");
		}

		WriteToFile(
			"\t};\n\n"
			"\treturn Items;\n");
	}
	else
	{
		WriteToFile("\treturn NULL;\n");
	}
	
	WriteToFile("}\n\n");
}

void ZEMCGenerator::GenerateEnumeratorGetItemCount(ZEMCEnumerator* CurrentEnumerator)
{
	WriteToFile("ZESize %sEnumerator::GetItemCount() const\n"
		"{\n"
		"\treturn %d;\n"
		"}\n\n", 
		CurrentEnumerator->Name.ToCString(), CurrentEnumerator->Items.GetCount());
}

void ZEMCGenerator::GenerateEnumerator(ZEMCEnumerator* CurrentEnumerator)
{
	GenerateEnumeratorMacros(CurrentEnumerator);

	GenerateDeclarationGetName(CurrentEnumerator);
	GenerateDeclarationGetGUID(CurrentEnumerator);
	GenerateDeclarationGetAttributes(CurrentEnumerator);
	GenerateDeclarationGetAttributeCount(CurrentEnumerator);

	GenerateEnumeratorGetItems(CurrentEnumerator);
	GenerateEnumeratorGetItemCount(CurrentEnumerator);
}
