//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFileCompiler.cpp
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

#include "ZEDS/ZEArray.h"
#include "ZEFileCompiler.h"
#include "ZEError.h"

#include <stdio.h>

bool ZEFileCompiler::Compile(const ZEFileCompilerOptions& Options)
{
	FILE* InputFile = fopen(Options.InputFileName.ToCString(), "rb");
	if (!InputFile)
	{
		zeError("Can not open input file \"%s\".", Options.InputFileName.ToCString());
		return false;
	}

	fseek(InputFile, 0, SEEK_END);
	unsigned int FileSize = ftell(InputFile);
	fseek(InputFile, 0, SEEK_SET);

	if (FileSize > 16 * 1024 * 1024)
	{
		zeError("File size is too big. Allowed max file size is 16 MB.");
		return false;
	}

	FILE* SourceFile = fopen(Options.SourceFileName.ToCString(), "w");
	if (!SourceFile)
	{
		zeError("Can not open output file \"%s\".", Options.SourceFileName.ToCString());
		return false;
	}

	FILE* HeaderFile = NULL;
	if (!Options.HeaderFileName.IsEmpty())
	{
		HeaderFile = fopen(Options.HeaderFileName.ToCString(), "w");
		if (!HeaderFile)
		{
			zeError("Can not open output file \"%s\".", Options.HeaderFileName.ToCString());
			return false;
		}
	}


	if (HeaderFile != NULL)
	{
		fprintf(HeaderFile, 
			"#pragma once\n\n"
			"#include \"ZEData.h\"\n"
			"#include \"ZETypes.h\"\n"
			"\n"
			"class %s : public ZEData\n"
			"{\n"
				"\tpublic:\n"
					"\t\tvoid*\t\tGetData();\n"
					"\t\tZESize\t\tGetSize();\n"
			"};\n", 
			Options.ClassName.ToCString());
		fclose(HeaderFile);

	}
	if (SourceFile != NULL)
	{
		fprintf(SourceFile,	"#include \"%s\"\n"
			"\n"
			"void* %s::GetData()\n"
			"{\n"
				"\tstatic ZEUInt8 Data[] =\n"
				"\t{\n", 
			Options.HeaderFileName.ToCString(), Options.ClassName.ToCString());

		ZEArray<ZEUInt8> Buffer;
		Buffer.SetCount(FileSize);
		ZESize BytesRead = fread(Buffer.GetCArray(), 1, FileSize, InputFile);
		if (BytesRead != FileSize)
		{
			zeError("Error occured while reading content of input file.");
			return false;
		}
		fclose(InputFile);

		ZEUInt8* Current = Buffer.GetCArray();
		BytesRead = 0;
		while(true)
		{
			if (FileSize > BytesRead + 16)
			{
				fprintf(SourceFile, "\t\t0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X,\n",
					Current[0], Current[1], Current[2], Current[3], Current[4], Current[5], Current[6], Current[7],
					Current[8], Current[9], Current[10], Current[11], Current[12], Current[13], Current[14], Current[15]);

				BytesRead += 16;
				Current += 16;
			}
			else
			{
				fprintf(SourceFile, "\t\t");
				for (int I = 0; I < FileSize - BytesRead; I++)
				{
					fprintf(SourceFile,"0x%02X, ", *Current);
					Current++;
				}
				fprintf(SourceFile, "\n");
				break;
			}
		}

		fprintf(SourceFile,	
					"\t\t0x00, 0x00, 0x00, 0x00\n"
				"\t};\n"
				"\n"
				"\treturn Data;\n"
			"}\n"
			"\n"
			"ZESize %s::GetSize()\n"
			"{\n"
				"\treturn %u;\n"
			"}", 
			Options.ClassName.ToCString(), FileSize);

		fclose(SourceFile);
	}
	return true;
};


