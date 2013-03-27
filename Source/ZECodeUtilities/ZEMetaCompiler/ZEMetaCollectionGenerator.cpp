//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMetaCollectionGenerator.cpp
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

#include "ZEMetaCollectionGenerator.h"
#include "ZEFile/ZEFileInfo.h"

struct ZERegisteredClass
{
	ZEString RegisteredClassName;
	ZEString IncludeDirectory;
};

void ZEMetaCollectionGenerator::Generate(const ZEMetaCompilerOptions& Options)
{
	ZEArray<ZERegisteredClass> RegisteredClasses;

	for(ZESize I = 0; I < Options.RegisterFiles.GetCount(); I++)
	{
		FILE* RegisterFile;
		RegisterFile = fopen(Options.RegisterFiles[I].ToCString(), "r");

		if(RegisterFile == NULL)
			zeError("Error! Register file is not found!");

		fseek(RegisterFile, 0, SEEK_END);
		unsigned int FileLength = ftell(RegisterFile);
		fseek(RegisterFile, 0, SEEK_SET);

		ZEArray<ZEInt8> Buffer;
		Buffer.SetCount(FileLength);
		ZESize BytesRead = fread(Buffer.GetCArray(), 1, FileLength, RegisterFile);

		if (BytesRead != FileLength)
			zeError("Error! Can not read register file!");

		fclose(RegisterFile);

		ZEString Context = (char*)Buffer.GetConstCArray();
		ZESize Index = 0;

		RegisteredClasses.Add();
		for(ZESize J = 0; J < Context.GetLength(); J++)
		{
			if(Context[J] == ',')
			{
				RegisteredClasses.GetLastItem().RegisteredClassName = Context.SubString(Index, J - 1);
				Index = J + 1;
				continue;
			}

			if(Context[J] == ';')
			{
				bool IsDuplicateIncludeFileFound = false;
				ZEString IncludeDirectory = Context.SubString(Index, J - 1);
				for(ZESize I = 0 ; I < RegisteredClasses.GetCount(); I++)
				{
					if(RegisteredClasses[I].IncludeDirectory == IncludeDirectory)
					{
						IsDuplicateIncludeFileFound = true;
						break;
					}
				}

				if(!IsDuplicateIncludeFileFound)
					RegisteredClasses.GetLastItem().IncludeDirectory = Context.SubString(Index, J - 1);

				Index = J + 1;
				RegisteredClasses.Add();
				continue;
			}
		}
		RegisteredClasses.Pop();
	}

	ZEString ClassCollectionName = Options.ClassCollectionName;
	ZEString ClassCollectionHeader = Options.ClassCollectionHeaderFile;

	FILE* ClassCollectionHeaderFile;
	ClassCollectionHeaderFile = fopen(ClassCollectionHeader.ToCString(), "w");

	fprintf(ClassCollectionHeaderFile,
		"#pragma once\n"
		"#include \"ZEMeta/ZEMetaRegister.h\"\n\n"
		"class %s : public ZEMetaRegister\n"
		"{\n"
		"\tpublic:\n"
		"\t\tvirtual ZEClass**\t\t\t\t\t\tGetClasses();\n"
		"\t\tvirtual ZESize\t\t\t\t\t\t\tGetClassCount();\n\n"
		"\t\tstatic %s*\t\t\tGetInstance();\n"
		"};\n\n", 
		ClassCollectionName.ToCString(), 
		ClassCollectionName.ToCString(), 
		ClassCollectionName.ToCString(), 
		ClassCollectionName.ToCString());

	fclose(ClassCollectionHeaderFile);

	ZEString ClassCollectionCpp = Options.ClassCollectionSourceFile;

	FILE* ClassCollectionSourceFile;
	ClassCollectionSourceFile = fopen(ClassCollectionCpp.ToCString(), "w");

	fprintf(ClassCollectionSourceFile, "#include \"%s\"\n", ClassCollectionHeader.ToCString());

	bool IsBuiltInTypeFound = false;
	for(ZESize I = 0; I < RegisteredClasses.GetCount(); I++)
	{
		if(!RegisteredClasses[I].IncludeDirectory.IsEmpty())
		{
			//Is registered class a builtin math class type?
			if(RegisteredClasses[I].IncludeDirectory == "ZEVector.h" || RegisteredClasses[I].IncludeDirectory == "ZEMatrix.h" || RegisteredClasses[I].IncludeDirectory == "ZEQuaternion.h")
			{
				fprintf(ClassCollectionSourceFile, "#include \"ZEMath/%s\"\n", RegisteredClasses[I].IncludeDirectory.ToCString());
				IsBuiltInTypeFound = true;
			}
			else
				fprintf(ClassCollectionSourceFile, "#include \"%s\"\n", RegisteredClasses[I].IncludeDirectory.ToCString());
		}
	}

	if(IsBuiltInTypeFound)
			fprintf(ClassCollectionSourceFile, "#include \"ZEPrimitiveTypes.h\"\n");

	fprintf(ClassCollectionSourceFile,
		"\n"
		"ZEClass** %s::GetClasses()\n"
		"{\n"
		"\tstatic ZEClass* Classes[%d] = \n"
		"\t{\n", ClassCollectionName.ToCString(), RegisteredClasses.GetCount());

	for(ZESize I = 0; I < RegisteredClasses.GetCount(); I++)
	{
		if(!RegisteredClasses[I].RegisteredClassName.IsEmpty())
		{
			//Is registered class a builtin class type?
			if(IsBuiltInTypeFound)
				fprintf(ClassCollectionSourceFile, "\t\t%sClass::Class()%s\n", RegisteredClasses[I].RegisteredClassName.ToCString(), I < RegisteredClasses.GetCount() - 1 ? "," : "");
			else
				fprintf(ClassCollectionSourceFile, "\t\t%s::Class()%s\n", RegisteredClasses[I].RegisteredClassName.ToCString(), I < RegisteredClasses.GetCount() - 1 ? "," : "");
		}
	}

	fprintf(ClassCollectionSourceFile,
		"\t};\n\n"
		"\treturn Classes;\n"
		"};\n\n");

	fprintf(ClassCollectionSourceFile,
		"ZESize %s::GetClassCount()\n"
		"{\n"
		"\treturn %d;\n"
		"}\n\n", ClassCollectionName.ToCString(), RegisteredClasses.GetCount());

	fprintf(ClassCollectionSourceFile,
		"%s* %s::GetInstance()\n"
		"{\n"
		"\tstatic %s Collection;\n"
		"\treturn &Collection;\n"
		"}\n\n", ClassCollectionName.ToCString(), ClassCollectionName.ToCString(), ClassCollectionName.ToCString());

	fclose(ClassCollectionSourceFile);
}

