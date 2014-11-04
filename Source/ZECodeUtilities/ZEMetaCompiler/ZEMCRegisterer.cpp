//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMCRegisterer.cpp
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

#include "ZEMCRegisterer.h"

void ZEMCRegisterer::SetOptions(ZEMCOptions* options)
{
	Options = options;
}

void ZEMCRegisterer::ParseCSVLine(ZEArray<ZEString>& Output, const char* Input)
{
	Output.Clear();

	ZEString CurrentValue;

	const char* CurrentChar = Input;
	while(true)
	{
		if (*CurrentChar == '\n' || *CurrentChar == '\0' || *CurrentChar == ';')
		{
			if (!CurrentValue.IsEmpty())
				Output.Add(CurrentValue);
			break;
		}
		else if (*CurrentChar == ',')
		{
			Output.Add(CurrentValue);
			CurrentValue.Clear();
			CurrentChar++;
			continue;
		}

		CurrentValue.AppendCharacter(ZECharacter(*CurrentChar));
		CurrentChar++;
	}
}

void ZEMCRegisterer::LoadDeclarations()
{
	for (ZESize I = 0; I < Options->DeclarationsFileNames.GetCount(); I++)
	{
		FILE* RegisterFile;
		RegisterFile = fopen(Options->DeclarationsFileNames[I].ToCString(), "r");

		if(RegisterFile == NULL)
		{
			zeError("Cannot open declarations file \"%s\" for reading.", Options->DeclarationsFileNames[I].ToCString());
			return;
		}

		char Buffer[4096];
		if (fgets(Buffer, 4096, RegisterFile) == NULL)
			continue;

		ZEArray<ZEString> Values;
		ParseCSVLine(Values, Buffer);
		if (Values.GetCount() == 0)
			continue;
		
		if (!Context.Includes.Exists(Values[0]))
			Context.Includes.Add(Values[0]);

		while(!feof(RegisterFile))
		{

			if (fgets(Buffer, 4096, RegisterFile) == NULL)
				continue;

			ParseCSVLine(Values, Buffer);
			if (Values.GetCount() < 2)
				continue;

			if (Values[0] == "Class")
				Context.Classes.Add(Values[1]);
			else if (Values[0] == "Enum")
				Context.Enumerators.Add(Values[1]);
		}
	}
}

void ZEMCRegisterer::GenerateRegisterHeader(FILE* File)
{
	FILE* RegisterHeaderFile;
	RegisterHeaderFile = fopen(Options->RegisterHeaderFileName.ToCString(), "w");
	if (RegisterHeaderFile == NULL)
	{
		zeError("Cannot open register header file \"%s\" for writing.\n", Options->RegisterHeaderFileName.ToCString());
		return;
	}

	fprintf(RegisterHeaderFile,
		"#pragma once\n"
		"#include \"ZEMeta/ZERegistar.h\"\n\n"
		"class %s : public ZERegistar\n"
		"{\n"
		"\tpublic:\n"
		"\t\tvirtual ZEClass**\t\t\t\t\tGetClasses();\n"
		"\t\tvirtual ZESize\t\t\t\t\t\tGetClassCount();\n\n"
		"\t\tvirtual ZEEnumerator**\t\t\t\tGetEnumerators();\n"
		"\t\tvirtual ZESize\t\t\t\t\t\tGetEnumeratorCount();\n\n"
		"\t\tstatic %s*\t\t\tGetInstance();\n"
		"};\n\n", 
		Context.RegisterName.ToCString(), 
		Context.RegisterName.ToCString());

	fclose(RegisterHeaderFile);
}

void ZEMCRegisterer::GenerateIncludes(FILE* File)
{
	fprintf(File, "#include \"%s\"\n", Options->RegisterHeaderFileName.ToCString());

	for(ZESize I = 0; I < Context.Includes.GetCount(); I++)
		fprintf(File, "#include \"%s\"\n", Context.Includes[I].ToCString());
}

void ZEMCRegisterer::GenerateRegisterGetClasses(FILE* File)
{
	fprintf(File,
		"\n"
		"ZEClass** %s::GetClasses()\n"
		"{\n", Context.RegisterName.ToCString());

	if (Context.Classes.GetCount() == 0)
	{
		fprintf(File, "\treturn NULL;\n");
	}
	else
	{
		fprintf(File, "\tstatic ZEClass* Classes[%d] = \n"
			"\t{\n", Context.Classes.GetCount());

		for(ZESize I = 0; I < Context.Classes.GetCount(); I++)
			fprintf(File, "\t\t%s::Class()%s\n", Context.Classes[I].ToCString(), I < Context.Classes.GetCount() - 1 ? "," : "");

		fprintf(File,
			"\t};\n\n"
			"\treturn Classes;\n");
	}

	fprintf(File, "};\n\n");
}

void ZEMCRegisterer::GenerateRegisterGetClassCount(FILE* File)
{
	fprintf(File,
		"ZESize %s::GetClassCount()\n"
		"{\n"
		"\treturn %d;\n"
		"}\n\n", Context.RegisterName.ToCString(), Context.Classes.GetCount());
}

void ZEMCRegisterer::GenereteRegisterGetEnumerators(FILE* File)
{
	fprintf(File,
		"ZEEnumerator** %s::GetEnumerators()\n"
		"{\n", Context.RegisterName.ToCString());

	if (Context.Enumerators.GetCount() == 0)
	{
		fprintf(File, "\treturn NULL;\n");
	}
	else
	{
		fprintf(File, "\tstatic ZEEnumerator* Enumerators[%d] = \n"
			"\t{\n", Context.Enumerators.GetCount());

		for(ZESize I = 0; I < Context.Enumerators.GetCount(); I++)
			fprintf(File, "\t\t%s_Declaration()%s\n", Context.Enumerators[I].ToCString(), I < Context.Enumerators.GetCount() - 1 ? "," : "");

		fprintf(File,
			"\t};\n\n"
			"\treturn Enumerators;\n");
	}

	fprintf(File, "};\n\n");
}

void ZEMCRegisterer::GenerateRegisterGetEnumeratorCount(FILE* File)
{
	fprintf(File,
		"ZESize %s::GetEnumeratorCount()\n"
		"{\n"
		"\treturn %d;\n"
		"}\n\n", Context.RegisterName.ToCString(), Context.Classes.GetCount());
}

void ZEMCRegisterer::GenerateDeclarationsFile(ZEMCContext* Context)
{
	FILE* File;
	File = fopen(Options->DeclarationsFileName.ToCString(), "w");
	if(File == NULL)
	{
		zeError("Cannot open declarations file \"%s\" for writing.", Options->DeclarationsFileName.ToCString());
		return;
	}

	fprintf(File, "%s\n", Options->InputFileName.ToCString());
	for (ZESize I = 0; I < Context->TargetClasses.GetCount(); I++)
	{
		if (!Context->TargetClasses[I]->IsBuiltInClass)
			fprintf(File, "Class,%s;\n", Context->TargetClasses[I]->Name.ToCString());
	}

	for (ZESize I = 0; I < Context->TargetEnumerators.GetCount(); I++)
		fprintf(File, "Enum,%s;\n", Context->Enumerators[I]->Name.ToCString());

	fclose(File);
}

void ZEMCRegisterer::GenerateGetInstance(FILE* File)
{
	fprintf(File,
		"%s* %s::GetInstance()\n"
		"{\n"
		"\tstatic %s Register;\n"
		"\treturn &Register;\n"
		"}\n\n", Context.RegisterName.ToCString(), Context.RegisterName.ToCString(), Context.RegisterName.ToCString());
}

void ZEMCRegisterer::GenerateRegister()
{
	Context.RegisterName = Options->RegisterName;
	Context.Classes.Clear();
	Context.Enumerators.Clear();
	Context.Includes.Clear();

	LoadDeclarations();

	FILE* File;
	File = fopen(Options->RegisterSourceFileName.ToCString(), "w");
	if (File == NULL)
	{
		zeError("Cannot open register source file \"%s\" for writing.\n", Options->RegisterSourceFileName.ToCString());
		return;
	}

	GenerateRegisterHeader(File);
	GenerateIncludes(File);
	GenerateRegisterGetClasses(File);
	GenerateRegisterGetClassCount(File);
	GenereteRegisterGetEnumerators(File);
	GenerateRegisterGetEnumeratorCount(File);
	GenerateGetInstance(File);

	fclose(File);
}

