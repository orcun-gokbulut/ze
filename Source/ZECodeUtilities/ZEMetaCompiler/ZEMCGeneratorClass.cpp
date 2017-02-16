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
	WriteToFile("\n\n");
	WriteToFile("// %s\n", CurrentClass->Name.ToCString());
	WriteToFile("////////////////////////////////////////////////////////////////////////////////////////\n\n");

	GenerateClassMacros(CurrentClass);

	GenerateDeclarationGetName(CurrentClass);
	GenerateDeclarationGetGUID(CurrentClass);
	GenerateDeclarationGetAttributes(CurrentClass);
	GenerateDeclarationGetAttributeCount(CurrentClass);

	GenerateClassGetParentClass(CurrentClass);
	GenerateClassGetFlags(CurrentClass);

	GenerateClassGetSizeOfObject(CurrentClass);
	GenerateClassCreateInstance(CurrentClass);
	GenerateClassDestroy(CurrentClass);
	GenerateClassDynamicCast(CurrentClass);
	GenerateClassClone(CurrentClass);
	GenerateClassConstruct(CurrentClass);
	GenerateClassDeconstruct(CurrentClass);
	GenerateClassAssign(CurrentClass);

	GenerateClassGetSizeOfScriptObject(CurrentClass);
	GenerateClassCreateScriptInstance(CurrentClass);

	GenerateClassProperties(CurrentClass);
	GenerateClassMethods(CurrentClass);
	GenerateClassScriptObject(CurrentClass);
}

void ZEMCGenerator::GenerateCastedObject(ZEMCClass* CurrentClass, const char* SourceName, const char* DestinationName)
{
	if (!CurrentClass->IsFundamental)
	{
		WriteToFile(
			"\t%s* %s = ZEClass::Cast<%s>(%s);\n"
			"\tif (%s == NULL)\n"
			"\t\treturn false;\n\n",
			CurrentClass->Name.ToCString(),
			DestinationName,
			CurrentClass->Name.ToCString(),
			SourceName,
			DestinationName);
	}
	else
	{
		WriteToFile(
			"\t%s* %s = reinterpret_cast<%s*>(%s);\n",
			CurrentClass->Name.ToCString(),
			DestinationName,
			CurrentClass->Name.ToCString(),
			SourceName);
	}
}

void ZEMCGenerator::GenerateClassMacros(ZEMCClass* CurrentClass)
{
	WriteToFile("ZEMT_CLASS_DEFINITION(%s);\n", CurrentClass->Name.ToCString());
	if (CurrentClass->IsFundamental)
		WriteToFile("ZEMT_FUNDAMENTAL_IMPLEMENTATION(%s);\n\n", CurrentClass->Name.ToCString());
	else
		WriteToFile("ZEMT_OBJECT_IMPLEMENTATION(%s);\n\n", CurrentClass->Name.ToCString());
}

void ZEMCGenerator::GenerateClassGetParentClass(ZEMCClass* CurrentClass)
{
	WriteToFile(
		"ZEClass* %sClass::GetParentClass()\n"
		"{\n", CurrentClass->Name.ToCString());

	if (CurrentClass->BaseClass == NULL)
		WriteToFile("\treturn NULL;\n");
	else
		WriteToFile("\treturn %s::Class();\n", CurrentClass->BaseClass->Name.ToCString());

	WriteToFile("}\n\n");
}

void ZEMCGenerator::GenerateClassGetFlags(ZEMCClass* CurrentClass)
{
	WriteToFile(
		"ZEMTClassFlags %sClass::GetFlags()\n"
		"{\n", 
		CurrentClass->Name.ToCString());

	WriteToFile("\treturn ZEMT_CF_NONE");

	if (CurrentClass->IsFundamental)
		WriteToFile(" | ZEMT_CF_FUNDAMENTAL");
	
	if (CurrentClass->IsAbstract)
		WriteToFile(" | ZEMT_CF_ABSTRACT");
	
	if (false)
		WriteToFile(" | ZEMT_CF_SCRIPT");

	if (!CurrentClass->IsAbstract)
	{
		if (CurrentClass->HasPublicDefaultConstructor || CurrentClass->HasCreateInstanceMethod)
			WriteToFile(" | ZEMT_CF_CREATE_INSTANCE");

		if (CurrentClass->HasPublicDestroyMethod || CurrentClass->HasPublicDestructor)
			WriteToFile(" | ZEMT_CF_DESTROY");

		if (CurrentClass->HasPublicAssignmentOperator)
			WriteToFile(" | ZEMT_CF_ASSIGN");

		if (CurrentClass->HasPublicCopyConstructor)
			WriteToFile(" | ZEMT_CF_CLONE");

		if (CurrentClass->HasPublicDefaultConstructor)
			WriteToFile(" | ZEMT_CF_CONSTRUCT");

		if (CurrentClass->HasPublicDestructor)
			WriteToFile(" | ZEMT_CF_DECONSTRUCT");
	}

	WriteToFile(";\n");
	WriteToFile("}\n\n");
}
