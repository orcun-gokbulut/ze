//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMain.cpp
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

#include <stdio.h>
#include "ZEMetaGenerator.h"
#include "ZEMetaProcessor.h"
#include "ZEMetaCompilerOptions.h"

static void Error(const char* Text)
{
	printf("Error: ");
	printf(Text);
	printf("\nPlease use ZEMetaCompiler -h command to get help.");
 	exit(EXIT_FAILURE);
}

static void ShowHelp()
{
	printf(
		"Description:\n"
		"  ZEMetaCompiler parses given header file and automatically generates necessary \n"
		"  meta code for the types in the header files. If you have and ZEObject derived \n"
		"  class you should generate its support code by using this tool.\n"
		"\n"
		"  Note: You can use ZEMetaCompile flag in ze_add_source CMake command which \n"
		"        automatically uses this tool internally.\n"
		"\n"
		"Usage:\n"
		"   ZEMetaCompiler [InputFileName] [Parameters]\n"
		"\n"
		"Parameters:\n"
		"  [InputFileName] : The cpp header file that you want to process.\n"
		"  -h : Shows this help text."
		"  -I  [Include Directory] : Adds new include directory.\n"
		"  -D  [Definition] : Adds new preprocessor definition.\n"
		"  -D  [Definition]=[Value] : Adds new preprocessor definition with value.\n"
		"  -o  [OutputFileName] : Generated output. Default: [HeaderFileName].ZEMeta.cpp\n"
		"  -c  [ClassCollectionOutputFileName] : File name of the class collection file that the output will be appended to.\n"
		"  -cs [ClassCollectionName] : Writes the beginning of class collection files.\n"
		"  -ca [ClassCollectionName] : Append output into class collection.\n"
		"  -ce [ClassCollectionName] : Writes ending of class collection files.\n"
		"  -v : Verbose mode which outputs compilation errors. Default : Not set.\n"
		"\n"
		"Example:\n"
		"  ZEMetaCompiler ZEEntity.h -I . -I ..\\ZEFoundation -D ZE_PLATFORM_WINDOWS -D ZE_VERSION_MAJOR=0 \n"
		"\n"
		"\n"
		"CMake Example:\n"
		"  ze_add_source(ZEEntity.h Headers Sources ZEMC)\n"
		"\n");
}

static void ParseParameters(int Argc, const char** Argv, ZEMetaCompilerOptions& Options)
{
	Options.BinaryPath = Argv[0];
	Options.Verbose = false;
	for (int I = 1; I < Argc; I++)
	{
		if (strncmp(Argv[I], "-h", 2) == 0)
		{
			ShowHelp();
			exit(EXIT_SUCCESS);
		}
		else if (strncmp(Argv[I], "-I", 2) == 0)
		{
			if (I + 1 >= Argc)
				Error("Empty parameter value.");

			I++;
			Options.IncludeDirectories.Add(Argv[I]);
		}
		else if (strncmp(Argv[I], "-D", 2) == 0)
		{
			if (I + 1 >= Argc)
				Error("Empty parameter value.");

			I++;
			Options.Definitions.Add(Argv[I]);
		}
		else if (strncmp(Argv[I], "-o", 2) == 0)
		{
			if (!Options.OutputFileName.IsEmpty())
				Error("Only one output file can be given.");

			if (I + 1 >= Argc)
				Error("Empty parameter value.");
			
			I++;
			Options.OutputFileName = Argv[I];
		}
		else if (strncmp(Argv[I], "-c", 2) == 0)
		{
			if (strncmp(Argv[I], "-cs", 3) == 0)
			{
				if(Options.ClassCollectionOutputFileName.IsEmpty())
					Error("Class collection output file is not given.");

				Options.ClassCollectionWriteMode = ZE_CCWM_START;

				if (I + 1 >= Argc)
					Error("Empty parameter value.");

				I++;
				Options.ClassCollectionName = Argv[I];

				continue;
			}
			else if (strncmp(Argv[I], "-ca", 3) == 0)
			{
				if(Options.ClassCollectionOutputFileName.IsEmpty())
					Error("Class collection output file is not given.");

				Options.ClassCollectionWriteMode = ZE_CCWM_APPEND;

				if (I + 1 >= Argc)
					Error("Empty parameter value.");

				I++;
				Options.ClassCollectionName = Argv[I];

				continue;
			}
			else if (strncmp(Argv[I], "-ce", 3) == 0)
			{
				if(Options.ClassCollectionOutputFileName.IsEmpty())
					Error("Class collection output file is not given.");

				Options.ClassCollectionWriteMode = ZE_CCWM_END;

				if (I + 1 >= Argc)
					Error("Empty parameter value.");

				I++;
				Options.ClassCollectionName = Argv[I];

				continue;
			}

			if (I + 1 >= Argc)
				Error("Empty parameter value.");

			I++;
			Options.ClassCollectionOutputFileName = Argv[I];
		}
		else if (strncmp(Argv[I], "-v", 2) == 0)
		{
			Options.Verbose = true;
		}
		else
		{
			if (Options.InputFileName != NULL)
				Error("Only one input file can be given.");

			Options.InputFileName = Argv[I];
		}
	}

	if (Options.InputFileName == NULL)
	{
		zeError("Missing input header file name.");
		ShowHelp();
	}

	if (Options.OutputFileName.IsEmpty())
	{
		Options.OutputFileName.Append(Options.InputFileName);
		Options.OutputFileName.Append(".ZEMeta.cpp");
	}
}
int main(int Argc, const char** Argv)
{
	printf(
		"ZEMetaCompiler - Version : 0.3.7\n"
		"Copyright (C) 2013, Zinek Code House. All rights reserved.\n\n");

	if (Argc == 1)
		Error("Command line arguments are missing.");

	ZEMetaCompilerOptions Options;
	ParseParameters(Argc, Argv, Options);

	ZEMetaData MetaData;
	bool Result = ZEMetaProcessor::Process(&MetaData, Options);
	if (!Result)
		Error("Processing failed.");

	Result = ZEMetaGenerator::Generate(Options, &MetaData);
	if (!Result)
		Error("Generation failed.");

	return EXIT_SUCCESS;
}
