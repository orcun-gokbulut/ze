//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMCDriver.cpp
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

#include "ZEMCOptions.h"

#include "ZEMCParser.h"
#include "ZEMCGenerator.h"
#include "ZEMCRegisterer.h"
#include "ZETimeCounter.h"

#include <stdio.h>

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
		"  -r  [RegisterFileName] : On meta generation mode, Creates a register file with given name. On class collection generation mode, inputs given register file. \n"
		"  -msvc : Generates Microsoft Visual Studio like error messages. (Default: Not set.)\n"
		"  -q : Quiet mode, which only outputs errors.\n"
		"\n"
		"Example(s) (Meta Generation):\n"
		"  ZEMetaCompiler ZEEntity.h -I . -I ..\\ZEFoundation -D ZE_PLATFORM_WINDOWS -D ZE_VERSION_MAJOR=0 -o ZEEntity.ZEMeta.cpp -r ZEEntity.ZEMeta.Register\n"
		"  ZEMetaCompiler ZEScene.h -I . -I ..\\ZEFoundation -D ZE_PLATFORM_WINDOWS -D ZE_VERSION_MAJOR=0 -o ZEScene.ZEMeta.cpp -r ZEScene.ZEMeta.Register\n"
		"\n"
		"ZEBuild(CMake) Example:\n"
		"  ze_add_source(ZEEntity.h Headers Sources ZEMC)\n"
		"\n");
}

static void ParseParameters(int Argc, const char** Argv, ZEMCOptions& Options)
{
	Options.Argc= Argc;
	Options.Argv = Argv;
	Options.BinaryPath = Argv[0];
	Options.Quiet = false;
	Options.Benchmark = false;

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
			if (strcmp(Argv[I], "") != 0 && strcmp(Argv[I], "\"\"") != 0)
				Options.IncludeDirectories.Add(Argv[I]);
		}
		else if (strncmp(Argv[I], "-D", 2) == 0)
		{
			if (I + 1 >= Argc)
				Error("Empty parameter value.");

			I++;
			if (strcmp(Argv[I], "") != 0 && strcmp(Argv[I], "\"\"") != 0)
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
		else if (strncmp(Argv[I], "-r", 2) == 0)
		{
			if (I + 1 >= Argc)
				Error("Empty parameter value.");

			I++;
			Options.RegisterFileName = Argv[I];
		}
		else if (strncmp(Argv[I], "-q", 2) == 0)
		{
			Options.Quiet = true;
		}
		else if (strncmp(Argv[I], "-msvc", 5) == 0)
		{
			Options.MSVC = true;
		}
		else if (strncmp(Argv[I], "-benchmark", 5) == 0)
		{
			Options.Benchmark = true;
		}
		else
		{
			if (Options.InputFileName != NULL)
				Error("Only one input file can be given.");
			Options.InputFileName = Argv[I];
		}
	}

	if (Options.InputFileName == "")
	{
		zeError("Missing input header file name.");
		ShowHelp();
	}

	if (Options.OutputFileName == "")
	{
		zeError("Missing output file name.");
		ShowHelp();
	}
}

int main(int Argc, const char** Argv)
{
	if (Argc == 1)
		Error("Command line arguments are missing.");

	ZEMCOptions Options;
	ParseParameters(Argc, Argv, Options);

	if (!Options.Quiet)
	{
		printf("ZEMetaCompiler - Version : 0.4.0\n"
			"Copyright (C) 2014, Zinek Code House. All rights reserved.\n\n");
	}

	if (Options.Benchmark)
		printf("Compilation has started.\n");

	ZETimeCounter Counter;
	Counter.Start();
		
	ZEMCContext Context;
	ZEMCParser Parser;
	Parser.SetMetaContext(&Context);
	Parser.SetOptions(&Options);
	if (!Parser.Parse())
		Error("Cannot parse input file.");
	if (Options.Benchmark)
		printf("Parser is done. It took %I64u microseconds.\n", Counter.GetTimeMicroseconds());
		
	ZEMCGenerator Generator;
	Generator.SetOptions(&Options);
	Generator.SetMetaContext(&Context);
	if (!Generator.Generate())
		Error("Cannot generate ZEMeta file.");
	if (Options.Benchmark)
		printf("Generator is done. It took %I64u microseconds.\n", Counter.GetTimeMicroseconds());

	ZEMCRegisterer Registerer;
	Registerer.SetOptions(&Options);
	Registerer.GenerateRegisterFile(&Context);
	if (Options.Benchmark)
		printf("Registerer is done. It took %I64u microseconds.\n", Counter.GetTimeMicroseconds());

	Counter.Stop();

	if (Options.Benchmark)
		printf("Compilation is done.\n");

	return EXIT_SUCCESS;
}
