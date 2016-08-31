//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECVMain.cpp
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

#include "ZECVModelConverterV2.h"

#include "ZEFile/ZEPathManager.h"
#include "ZECVProcessor.h"
#include "ZECVAsset.h"
#include "ZECVConverter.h"

#include <stdio.h>
#include <stdlib.h>

void Help()
{
	printf(
		" This tool is provided for converting or upgrading files in old format to latest format.\n"
		"\n"
		" Usage:\n"
		"   ZECVConverter [Old Format File] [Destination]\n"
		"   ZECVConverter [Search Directory]\n"
		"\n"
		" Supported Assets:\n");

	ZECVProcessor* Processor = ZECVProcessor::GetInstance();

	ZECVAsset* const* Assets = Processor->GetAssets();
	ZESize AssetCount = Processor->GetAssetCount();
	for (ZESize I = 0; I < AssetCount; I++)
	{
		ZECVAsset* Asset = Assets[I];

		printf("  %s (Versions: ", Asset->GetName());

		ZECVConverter* const* Converters = Asset->GetConverters();
		ZESize ConverterCount = Asset->GetConverterCount();
		for (ZESize N = 0; N < ConverterCount; N++)
		{
			ZECVConverter* Converter = Converters[N];

			if (N != 0)
				printf (", ");

			ZECVVersion SourceVersion = Converter->GetSourceVersion();
			printf("%d.%d", SourceVersion.Major, SourceVersion.Minor);
		}

		ZECVVersion DestinationVersion = Asset->GetCurrentVersion();
		printf(") -> %d.%d\n", DestinationVersion.Major, DestinationVersion.Minor);
	}
}

int main(int argc, char** argv)
{
	printf(" ZECVConverter - Zinek Engine Asset Converter/Updater \n");
	printf("----------------------------------------------------------------------------------- \n");

	ZEPathManager::GetInstance()->SetAccessControl(false);

	const char* Source = argv[1];
	const char* Destination = argv[2];
	if (argc == 3)
	{
		bool Result = ZECVProcessor::GetInstance()->Convert(Source, Destination);
		return Result ? EXIT_SUCCESS : EXIT_FAILURE;
	}
	else if (argc == 2)
	{
		ZECVProcessor::GetInstance()->ConvertDirectory(Source);
		return EXIT_SUCCESS;
	}
	else
	{
		Help();
		return EXIT_FAILURE;
	}
}
