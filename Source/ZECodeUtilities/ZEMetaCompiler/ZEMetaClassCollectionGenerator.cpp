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

static void IncludeHeader(FILE* File, const char* IncludedFile)
{
	fprintf(File, "#include \"%s\"\n", IncludedFile);
}

static void CreateClassInterface(FILE* File, const char* LibraryName)
{
	fprintf(File,
		"\n"
		"class %sClassCollection : public ZEClassCollection\n"
		"{\n"
		"\tprivate:\n"
		"\t\t\t\t\t\t\t\t\t\t\t\t%sClassCollection();\n"
		"\t\tvirtual\t\t\t\t\t\t\t\t\t~%sClassCollection();\n\n"
		"\tpublic:\n"
		"\t\tvirtual ZEClass*\t\t\t\t\t\tGetClasses();\n"
		"\t\tstatic %sClassCollection*\t\tGetInstance();\n"
		"};\n\n", LibraryName, LibraryName, LibraryName, LibraryName);
}

void CreateClassImplementation(FILE* File, const char* HeaderFile, const char* LibraryName)
{
	fprintf(File,
		"#include \"%s\"\n"
		"#include \"%sClassCollectionIncludes.h\"\n\n"
		"ZEClass* %sClassCollection::GetClasses()\n"
		"{\n"
		"\tstatic ZEClass* Classes[] = \n"
		"\t{\n", HeaderFile, LibraryName, LibraryName);
}

void AppendClassToImplementation(FILE* File, const char* ClassName)
{
	fprintf(File, "\t\t%s::Class(),\n", ClassName);
}

void FinishClassImplementation(FILE* File)
{
	fprintf(File,
		"\t\tNULL\n"
		"\t}\n\n"
		"\treturn Classes;\n"
		"}\n\n");
}

void CreateClassDependencies(FILE* File, const char* LibraryName)
{
	fprintf(File,
		"%sClassCollection::%sClassCollection()\n"
		"{\n\n"
		"}\n\n", LibraryName, LibraryName);

	fprintf(File,
		"%sClassCollection::~%sClassCollection()\n"
		"{\n\n"
		"}\n\n", LibraryName, LibraryName);

	fprintf(File,
		"%sClassCollection* %sClassCollection::GetInstance()\n"
		"{\n"
		"\tstatic %sClassCollection Collection;\n"
		"\treturn &Collection;\n"
		"}\n\n", LibraryName, LibraryName, LibraryName);
}

bool ZEMetaClassCollectionGenerator::Generate(const ZEMetaCompilerOptions& Options, ZEMetaData* Data)
{
	if(Options.ClassCollectionWriteMode == ZE_CCWM_START)
	{
		ZEString HeaderFile = Options.ClassCollectionOutputFileName.ToCString();
		HeaderFile.Append("ClassCollection.h");

		FILE* File;
		File = fopen(HeaderFile.ToCString(), "w");

		fprintf(File, "#pragma once\n");
		fprintf(File, "#include \"ZEMeta/ZEClassCollection.h\"\n");
		fclose(File);

		ZEString IncludeFiles = Options.ClassCollectionOutputFileName.ToCString();
		IncludeFiles.Append("ClassCollectionIncludes.h");

		File = fopen(IncludeFiles.ToCString(), "w");
		IncludeHeader(File, ZEFileInfo::GetFileName(Options.InputFileName).ToCString());
		fclose(File);

		ZEString CppFile = Options.ClassCollectionOutputFileName.ToCString();
		CppFile.Append("ClassCollection.cpp");

		File = fopen(CppFile.ToCString(), "w");

		ZEString InterfaceInclude = Options.ClassCollectionOutputFileName;
		InterfaceInclude.Append("ClassCollection.h");

		CreateClassImplementation(File, InterfaceInclude.ToCString(), Options.ClassCollectionName.ToCString());
		AppendClassToImplementation(File, Data->HeaderTypes.GetLastItem()->Name.ToCString());

		fclose(File);

		return true;
	}
	else if(Options.ClassCollectionWriteMode == ZE_CCWM_APPEND)
	{
		ZEString IncludeFiles = Options.ClassCollectionOutputFileName.ToCString();
		IncludeFiles.Append("ClassCollectionIncludes.h");

		FILE* File;
		File = fopen(IncludeFiles.ToCString(), "a");
		IncludeHeader(File, ZEFileInfo::GetFileName(Options.InputFileName).ToCString());
		fclose(File);

		ZEString CppFile = Options.ClassCollectionOutputFileName.ToCString();
		CppFile.Append("ClassCollection.cpp");

		File = fopen(CppFile.ToCString(), "a");

		AppendClassToImplementation(File, Data->HeaderTypes.GetLastItem()->Name.ToCString());

		fclose(File);

		return true;
	}
	else if(Options.ClassCollectionWriteMode == ZE_CCWM_END)
	{
		ZEString HeaderFile = Options.ClassCollectionOutputFileName.ToCString();
		HeaderFile.Append("ClassCollection.h");

		FILE* File;
		File = fopen(HeaderFile.ToCString(), "a");
		CreateClassInterface(File, Options.ClassCollectionName.ToCString());
		fclose(File);

		ZEString IncludeFiles = Options.ClassCollectionOutputFileName.ToCString();
		IncludeFiles.Append("ClassCollectionIncludes.h");

		File = fopen(IncludeFiles.ToCString(), "a");
		IncludeHeader(File, ZEFileInfo::GetFileName(Options.InputFileName).ToCString());
		fclose(File);

		ZEString CppFile = Options.ClassCollectionOutputFileName.ToCString();
		CppFile.Append("ClassCollection.cpp");

		File = fopen(CppFile.ToCString(), "a");

		AppendClassToImplementation(File, Data->HeaderTypes.GetLastItem()->Name.ToCString());
		FinishClassImplementation(File);
		CreateClassDependencies(File, Options.ClassCollectionName.ToCString());

		fclose(File);

		return true;
	}
	
	return false;
}
