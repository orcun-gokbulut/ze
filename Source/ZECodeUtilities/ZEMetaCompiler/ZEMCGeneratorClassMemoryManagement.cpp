//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMCGeneratorClassMemoryManagement.cpp
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

void ZEMCGenerator::GenerateClassGetSizeOfObject(ZEMCClass* CurrentClass)
{
	WriteToFile(
		"ZESize %sClass::GetSizeOfObject() const\n"
		"{\n"
		"\treturn sizeof(%s);\n"
		"}\n\n", 
		CurrentClass->Name.ToCString(), 
		CurrentClass->Name.ToCString());
}

void ZEMCGenerator::GenerateClassCreateInstance(ZEMCClass* CurrentClass)
{
	WriteToFile(
		"ZEObject* %sClass::CreateInstance() const\n"
		"{\n", 
		CurrentClass->Name.ToCString());

	if (CurrentClass->HasCreateInstanceMethod && !CurrentClass->IsAbstract) 
	{
		if (CurrentClass->IsFundamental)
			WriteToFile("\treturn reinterpret_cast<ZEObject*>(%s::CreateInstance());\n", CurrentClass->Name.ToCString());
		else
			WriteToFile("\treturn %s::CreateInstance();\n", CurrentClass->Name.ToCString());
	}
	else if (CurrentClass->HasPublicDefaultConstructor && !CurrentClass->IsAbstract)
	{
		if (CurrentClass->IsFundamental)
			WriteToFile("\treturn reinterpret_cast<ZEObject*>(new %s());\n", CurrentClass->Name.ToCString());
		else
			WriteToFile("\treturn new %s();\n", CurrentClass->Name.ToCString());
	}
	else
	{
		WriteToFile("\treturn NULL;\n");
	}

	WriteToFile("}\n\n");
}

void ZEMCGenerator::GenerateClassClone(ZEMCClass* CurrentClass)
{
	WriteToFile( 
		"ZEObject* %sClass::Clone(const ZEObject* Object) const\n"
		"{\n",
		CurrentClass->Name.ToCString());

	if (CurrentClass->HasPublicCopyConstructor && !CurrentClass->IsAbstract)
	{
		GenerateCastedObjectConst(CurrentClass);

		if (CurrentClass->IsFundamental)
			WriteToFile("\treturn reinterpret_cast<ZEObject*>(new %s(*CastedObject));\n", CurrentClass->Name.ToCString());
		else
			WriteToFile("\treturn new %s(*CastedObject);\n", CurrentClass->Name.ToCString());
	}
	else
	{
		WriteToFile("\treturn NULL;\n");
	}

	WriteToFile("}\n\n");
}

void ZEMCGenerator::GenerateClassDestroy(ZEMCClass* CurrentClass)
{
	WriteToFile( 
		"bool %sClass::Destroy(ZEObject* Object) const\n"
		"{\n",
		CurrentClass->Name.ToCString());

	if (CurrentClass->HasPublicDestroyMethod)
	{
		GenerateClassTypeCheck(CurrentClass);

		WriteToFile("\t((%s*)Object)->Destroy();\n",
			CurrentClass->Name.ToCString());
		WriteToFile("\treturn true;\n");
	}
	else if (CurrentClass->HasPublicDestroyMethod)
	{
		GenerateClassTypeCheck(CurrentClass);

		WriteToFile("\tdelete (%s*)Object;\n",
			CurrentClass->Name.ToCString());
		WriteToFile("\treturn true;\n");
	}
	else
	{
		WriteToFile("\treturn false;\n");
	}

	WriteToFile("}\n\n");
}

void ZEMCGenerator::GenerateClassConstruct(ZEMCClass* CurrentClass)
{
	WriteToFile( 
		"bool %sClass::Construct(ZEObject* Object) const\n"
		"{\n",
		CurrentClass->Name.ToCString());
	
	if (CurrentClass->HasPublicDefaultConstructor && !CurrentClass->IsAbstract)
	{
		WriteToFile("\tnew(Object) %s();\n", 
			CurrentClass->Name.ToCString());

		WriteToFile("\treturn true;\n");
	}
	else
	{
		WriteToFile("\treturn false;\n");
	}

	WriteToFile("}\n\n");
}

void ZEMCGenerator::GenerateClassCopyConstruct(ZEMCClass* CurrentClass)
{
	WriteToFile( 
		"bool %sClass::CopyConstruct(ZEObject* DestinationObject, const ZEObject* SourceObject) const\n"
		"{\n",
		CurrentClass->Name.ToCString());

	if (CurrentClass->HasPublicCopyConstructor && !CurrentClass->IsAbstract)
	{
		GenerateCastedObject(CurrentClass, "DestinationObject", "CastedDestinationObject", true);
		GenerateCastedObjectConst(CurrentClass, "SourceObject", "CastedSourceObject");

		WriteToFile("\tnew(CastedDestinationObject) %s(*CastedSourceObject);\n", CurrentClass->Name.ToCString());
		WriteToFile("\treturn true;\n");
	}
	else
	{
		WriteToFile("\treturn false;\n");
	}

	WriteToFile("}\n\n");
}

void ZEMCGenerator::GenerateClassDeconstruct(ZEMCClass* CurrentClass)
{
	WriteToFile( 
		"bool %sClass::Deconstruct(ZEObject* Object) const\n"
		"{\n",
		CurrentClass->Name.ToCString());

	if (CurrentClass->HasPublicDestructor)
	{
		GenerateCastedObject(CurrentClass);
		WriteToFile("\tCastedObject->~%s();\n", 
			CurrentClass->Name.ToCString());
		WriteToFile("\treturn false;\n");
	}
	else
	{
		WriteToFile("\treturn false;\n");
	}

	WriteToFile("}\n\n");
}


void ZEMCGenerator::GenerateClassAssign(ZEMCClass* CurrentClass)
{
	WriteToFile( 
		"bool %sClass::Assign(ZEObject* DestinationObject, const ZEObject* SourceObject) const\n"
		"{\n",
		CurrentClass->Name.ToCString());

	if (CurrentClass->HasPublicAssignmentOperator)
	{
		GenerateCastedObject(CurrentClass, "DestinationObject", "CastedDestinationObject", true);
		GenerateCastedObjectConst(CurrentClass, "SourceObject", "CastedSourceObject");

		WriteToFile("\t*CastedDestinationObject = *CastedSourceObject;\n");
		WriteToFile("\treturn true;\n");
	}
	else
	{
		WriteToFile("\treturn false;\n");
	}

	WriteToFile("}\n\n");
}


void ZEMCGenerator::GenerateClassDynamicCast(ZEMCClass* CurrentClass)
{
	WriteToFile( 
		"ZEObject* %sClass::DynamicCast(ZEObject* Object) const\n"
		"{\n",
		CurrentClass->Name.ToCString());

	WriteToFile("\tif (Object == NULL)\n"
		"\t\treturn NULL;\n\n");

	GenerateCastedObject(CurrentClass);

	if (CurrentClass->IsFundamental)
		WriteToFile("\treturn reinterpret_cast<ZEObject*>(CastedObject);\n");
	else 
		WriteToFile("\treturn CastedObject;\n");

	WriteToFile("}\n\n");
}
