//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMetaClassCollectionGenerator.cpp
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

#include "ZEMetaClassCollectionGenerator.h"
#include "ZEFile/ZEFileInfo.h"

struct ZERegisteredClass
{
	ZEString RegisteredClassName;
	ZEString IncludeDirectory;
};

void ZEMetaClassCollectionGenerator::Generate(const ZEMetaCompilerOptions& Options)
{
	ZEArray<ZERegisteredClass> RegisteredClasses;
	RegisteredClasses.SetCount(Options.RegisterFiles.GetCount());

	for(ZESize I = 0; I < Options.RegisterFiles.GetCount(); I++)
	{
		FILE* File;
		File = fopen(Options.RegisterFiles[I].ToCString(), "r");

		if(File == NULL)
			zeError("Error! Register file is not found!");

		fseek(File, 0, SEEK_END);
		unsigned int FileLength = ftell(File);
		fseek(File, 0, SEEK_SET);

		ZEArray<ZEInt8> Buffer;
		Buffer.SetCount(FileLength);
		ZESize BytesRead = fread(Buffer.GetCArray(), 1, FileLength, File);

		if (BytesRead != FileLength)
			zeError("Error! Can not read register file!");

		fclose(File);

		ZEString Context = (char*)Buffer.GetConstCArray();
		ZESize Index = 0;

		for(ZESize J = 0; J < Context.GetLength(); J++)
		{
			if(Context[J] == ',')
			{
				RegisteredClasses[I].RegisteredClassName = Context.SubString(Index, J - 1);
				Index = J + 1;
				continue;
			}

			if(Context[J] == ';')
			{
				RegisteredClasses[I].IncludeDirectory = Context.SubString(Index, J - 1);
				Index = J + 1;
				continue;
			}
		}
	}

	ZEString ClassCollectionName = Options.ClassCollectionName;
	ZEString ClassCollectionHeader = Options.ClassCollectionHeaderFile;

	FILE* File;
	File = fopen(ClassCollectionHeader.ToCString(), "w");

	fprintf(File,
		"#pragma once\n"
		"#include \"ZEMeta/ZEClassCollection.h\"\n\n"
		"class %sClassCollection : public ZEClassCollection\n"
		"{\n"
		"\tprivate:\n"
		"\t\t\t\t\t\t\t\t\t\t\t\t%sClassCollection();\n"
		"\t\tvirtual\t\t\t\t\t\t\t\t\t~%sClassCollection();\n\n"
		"\tpublic:\n"
		"\t\tvirtual ZEClass**\t\t\t\t\t\tGetClasses();\n"
		"\t\tvirtual ZESize\t\t\t\t\t\t\tGetClassCount();\n\n"
		"\t\tstatic %sClassCollection*\t\t\tGetInstance();\n"
		"};\n\n", 
		ClassCollectionName.ToCString(), 
		ClassCollectionName.ToCString(), 
		ClassCollectionName.ToCString(), 
		ClassCollectionName.ToCString());

	fclose(File);

	ZEString ClassCollectionCpp = Options.ClassCollectionSourceFile;

	File = fopen(ClassCollectionCpp.ToCString(), "w");

	fprintf(File, "#include \"%s\"\n", ClassCollectionHeader.ToCString());

	for(ZESize I = 0; I < RegisteredClasses.GetCount(); I++)
		fprintf(File, "#include \"%s\"\n", RegisteredClasses[I].IncludeDirectory.ToCString());

	fprintf(File,
		"\n"
		"ZEClass** %sClassCollection::GetClasses()\n"
		"{\n"
		"\tstatic ZEClass* Classes[%d] = \n"
		"\t{\n", ClassCollectionName.ToCString(), RegisteredClasses.GetCount());

	for(ZESize I = 0; I < RegisteredClasses.GetCount(); I++)
		fprintf(File, "\t\t%s::Class()%s\n", RegisteredClasses[I].RegisteredClassName.ToCString(), I < RegisteredClasses.GetCount() - 1 ? "," : "");

	fprintf(File,
		"\t}\n\n"
		"\treturn Classes;\n"
		"};\n\n");

	fprintf(File,
		"ZESize %sClassCollection::GetClassCount()\n"
		"{\n"
		"\treturn %d;\n"
		"}\n\n", ClassCollectionName.ToCString(), RegisteredClasses.GetCount());

	fprintf(File,
		"%sClassCollection::%sClassCollection()\n"
		"{\n\n"
		"}\n\n", ClassCollectionName.ToCString(), ClassCollectionName.ToCString());

	fprintf(File,
		"%sClassCollection::~%sClassCollection()\n"
		"{\n\n"
		"}\n\n", ClassCollectionName.ToCString(), ClassCollectionName.ToCString());

	fprintf(File,
		"%sClassCollection* %sClassCollection::GetInstance()\n"
		"{\n"
		"\tstatic %sClassCollection Collection;\n"
		"\treturn &Collection;\n"
		"}\n\n", ClassCollectionName.ToCString(), ClassCollectionName.ToCString(), ClassCollectionName.ToCString());

}

