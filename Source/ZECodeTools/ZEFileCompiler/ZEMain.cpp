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

#include "ZEFileCompiler.h"
#include "ZEFileCompilerOptions.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
		"  ZEFileCompiler transfer binary files into ZEData based classes so that it can be\n"
		"  embedded into executables. This tool can generate both header and source files. \n"
		"\n"
		"  Note    : You can use ZEFileCompile flag in ze_add_source CMake command which \n"
		"		     automatically uses this tool internally.\n"
		"  Note    : If you want everything in single cpp file don't use -H parameter.\n"
		"  Warning : Parameters are case sensitive !\n"
		"\n"
		"Usage:\n"
		"   ZEFileCompiler [InputFileName] -C [ClassName] -S [SourceFile] -H [Header File]\n"
		"\n"
		"Parameters:\n"
		"  [InputFileName] : Input file name.\n"
		"  -h : Shows this help text."
		"  -C [ClassNameName] : Class name of the C++ class that is going to be generated.\n"
		"  -S [SourceFile] : C/C++ source file name that is going to be generated.\n"
		"  -H [HeaderFileName] : C/C++ header file name that is going to be generated. (Optional)\n"
		"  -v : Verbose mode which outputs compilation errors. Default : Not set.\n"
		"\n"
		"Command Line Example:\n"
		"  ZEMetaCompiler ZEIconResource -I ApplicaionIcon.bmp -S ZEIconResource.h -H ZEIconResource.h \n\n"
		"\n"
		"CMake Command Example:\n"
		"  ze_add_source(ApplicationIcon.bmp Sources ZEFileCompiler)\n"
		"\n");
}

#include <windows.h>
static void ParseParameters(int Argc, const char** Argv, ZEFileCompilerOptions& Options)
{
	MessageBox(NULL, "Attach", "ZEFC", MB_OK);
	Options.Verbose = false;
	for (int I = 1; I < Argc; I++)
	{
		if (strncmp(Argv[I], "-h", 2) == 0)
		{
			ShowHelp();
			exit(EXIT_SUCCESS);
		}
		else if (strncmp(Argv[I], "-C", 2) == 0)
		{
			if (!Options.SourceFileName.IsEmpty())
				Error("Only one class name can be given.");

			if (I + 1 >= Argc)
				Error("Empty parameter value.");

			I++;
			Options.ClassName = Argv[I];
		}
		else if (strncmp(Argv[I], "-S", 2) == 0)
		{
			if (!Options.SourceFileName.IsEmpty())
				Error("Only one source file name can be given.");

			if (I + 1 >= Argc)
				Error("Empty parameter value.");

			I++;
			Options.SourceFileName = Argv[I];
		}
		else if (strncmp(Argv[I], "-H", 2) == 0)
		{
			if (!Options.HeaderFileName.IsEmpty())
				Error("Only one header file name can be given.");

			if (I + 1 >= Argc)
				Error("Empty parameter value.");

			I++;
			Options.HeaderFileName = Argv[I];
		}
		else if (strncmp(Argv[I], "-v", 2) == 0)
		{
			Options.Verbose = true;
		}
		else
		{
			if (!Options.InputFileName.IsEmpty())
				Error("Only one input file can be given.");

			Options.InputFileName = Argv[I];
		}
	}

	if (Options.InputFileName.IsEmpty())
	{
		Error("Missing input header file name.");
		ShowHelp();
	}

	if (Options.ClassName.IsEmpty())
	{
		Error("Missing class name (-C) parameter.");
		ShowHelp();
	}

	if (Options.SourceFileName.IsEmpty() && Options.HeaderFileName.IsEmpty())
	{
		Error("Missing source file name (-S) and header file name (-H) parameters. You should give at last one of the parameters.");
		ShowHelp();
	}
	else if (Options.SourceFileName.IsEmpty() && Options.HeaderFileName.IsEmpty())
	{
		Error("Missing header file name (-H) parameter.");
		ShowHelp();
	}
}

int main(int Argc, const char** Argv)
{
	printf(
		"ZEFileCompiler - Version : 0.1\n"
		"Copyright (C) 2012, Zinek Code House. All rights reserved.\n\n");

	if (Argc == 1)
		Error("Command line arguments are missing.");

	ZEFileCompilerOptions Options;
	ParseParameters(Argc, Argv, Options);

	bool Result = ZEFileCompiler::Compile(Options);
	if (!Result)
		Error("Generation failed.");

	return EXIT_SUCCESS;
}
