//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMCGeneratorClass.cpp
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

void ZEMCGenerator::GenerateClass(ZEMCClass* CurrentClass)
{
	GenerateForwardDeclarations(CurrentClass);
	GenerateZEMetaMacros(CurrentClass);
	GenerateGetParentClass(CurrentClass);
	GenerateGetName(CurrentClass);
	GenerateGetGUID(CurrentClass);
	GenerateGetSizeOfClass(CurrentClass);
	GenerateGetSizeOfScriptingClass(CurrentClass);
	GenerateCreateInstance(CurrentClass);
	GenerateCreateScriptingInstance(CurrentClass);
	GenerateGetAttributes(CurrentClass);
	GenerateGetAttributeCount(CurrentClass);

	GenerateProperties(CurrentClass);
	GenerateMethods(CurrentClass);
	GenerateScriptingObject(CurrentClass);
}

void ZEMCGenerator::GenerateCastedObject(ZEMCClass* CurrentClass)
{
	WriteToFile(
		"\t%s* CastedObject = ZEClass::Cast<%s>(Object);\n"
		"\tif (CastedObject == NULL)\n"
		"\t\treturn false;\n\n",
		CurrentClass->Name.ToCString(),
		CurrentClass->Name.ToCString());
}

void ZEMCGenerator::GenerateZEMetaMacros(ZEMCClass* CurrentClass)
{
	WriteToFile("ZE_CLASS_IMPLEMENTATION(%s);\n", CurrentClass->Name.ToCString());
	WriteToFile("ZE_OBJECT_IMPLEMENTATION(%s);\n\n", CurrentClass->Name.ToCString());
}

void ZEMCGenerator::GenerateForwardDeclarations(ZEMCClass* CurrentClass)
{
	for(ZESize I = 0; I < Context->ForwardDeclarations.GetCount(); I++)
	{
		if(Context->ForwardDeclarations[I]->HeaderFileDeclaredIn == CurrentClass->Name)
			WriteToFile("#include \"%s\"\n", 
			Context->ForwardDeclarations[I]->HeaderName.ToCString());
	}

	WriteToFile("\n");
}

void ZEMCGenerator::GenerateGetParentClass(ZEMCClass* CurrentClass)
{
	WriteToFile(
		"ZEClass* %sClass::GetParentClass()\n"
		"{\n"
		"\treturn %s::Class();\n"
		"}\n\n",
		CurrentClass->Name.ToCString(), 
		CurrentClass->BaseClass->Name.ToCString());
}

void ZEMCGenerator::GenerateGetName(ZEMCClass* CurrentClass)
{
	WriteToFile(
		"const char* %sClass::GetName()\n"
		"{\n"
		"\treturn \"%s\";\n"
		"}\n\n",
		CurrentClass->Name.ToCString(), 
		CurrentClass->Name.ToCString());
}

void ZEMCGenerator::GenerateGetGUID(ZEMCClass* CurrentClass)
{
	WriteToFile(
		"ZEGUID %sClass::GetGUID()\n"
		"{\n"
		"\treturn ZEGUID();\n"
		"}\n\n", 
		CurrentClass->Name.ToCString());
}

void ZEMCGenerator::GenerateGetSizeOfClass(ZEMCClass* CurrentClass)
{
	WriteToFile(
		"ZESize %sClass::GetSizeOfClass()\n"
		"{\n"
		"\treturn sizeof(%s);\n"
		"}\n\n", 
		CurrentClass->Name.ToCString(), 
		CurrentClass->Name.ToCString());
}

void ZEMCGenerator::GenerateGetAttributes(ZEMCClass* CurrentClass)
{
	WriteToFile(
		"const ZEMetaAttribute* %sClass::GetAttributes()\n"
		"{\n", 
		CurrentClass->Name.ToCString());

	// Attribute Parameters
	for(ZESize I = 0; I < CurrentClass->Attributes.GetCount(); I++)
	{
		ZEMCAttribute* CurrentAttribute = CurrentClass->Attributes[I];
		WriteToFile(
			"\tstatic const char* Attribute%dParameters[%d] = {", 
			I, CurrentAttribute->Parameters.GetCount());

		for(ZESize J = 0; J < CurrentAttribute->Parameters.GetCount(); J++)
		{
			WriteToFile("\"%s\"%s", 
				CurrentAttribute->Parameters[J].ToCString(), 
				J != CurrentAttribute->Parameters.GetCount() - 1 ? ", " : "");
		}

		WriteToFile("};\n\n");
	}

	// Attribute List
	if (CurrentClass->Attributes.GetCount() > 0)
	{
		if(CurrentClass->Attributes.GetCount() > 0)
		{
			WriteToFile(
				"\tstatic ZEMetaAttribute Attributes[%d] =\n"
				"\t{\n", CurrentClass->Attributes.GetCount());
		}

		for(ZESize I = 0; I < CurrentClass->Attributes.GetCount(); I++)
		{
			ZEMCAttribute* currentAttribute = CurrentClass->Attributes[I];
			WriteToFile(
				"\t\t{\"%s\", Attribute%dParameters, %d}%s\n",
				currentAttribute->Name.ToCString(), I, 
				currentAttribute->Parameters.GetCount(),
				(I != CurrentClass->Attributes.GetCount() - 1 ? "," : ""));
		}

		WriteToFile(
			"\t};\n\n"
			"\treturn Attributes;\n"
			"}\n\n");
	}
	else
	{
		WriteToFile("\treturn NULL;\n");
	}

	WriteToFile("}\n\n");
}

void ZEMCGenerator::GenerateGetAttributeCount(ZEMCClass* CurrentClass)
{
	WriteToFile( 
		"ZESize %sClass::GetAttributeCount()\n"
		"{\n"
		"\treturn %d;\n"
		"}\n\n",
		CurrentClass->Name.ToCString(), 
		CurrentClass->Attributes.GetCount());
}

void ZEMCGenerator::GenerateCreateInstance(ZEMCClass* CurrentClass)
{
	WriteToFile(
		"ZEObject* %sClass::CreateInstance()\n"
		"{\n", 
		CurrentClass->Name.ToCString());

	if (CurrentClass->HasCreateInstanceMethod && !CurrentClass->IsAbstract) 
	{
		WriteToFile("\treturn %s::CreateInstance();\n", CurrentClass->Name.ToCString());
	}
	else if (CurrentClass->HasPublicDefaultConstructor && !CurrentClass->IsAbstract)
	{
		WriteToFile("\treturn new %s();\n", CurrentClass->Name.ToCString());
	}
	else
	{
		WriteToFile("\treturn NULL;\n");
	}

	WriteToFile("}\n\n");
}
