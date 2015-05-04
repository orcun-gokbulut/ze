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

#include "ZESceneConverter.h"

#include "ZEFile/ZEPathInfo.h"
#include "ZEDS/ZEFormat.h"
#include "ZEFile/ZEPathManager.h"

#include <stdio.h>
#include <stdlib.h>

static bool Operation(const char* Path, ZEPathOperationElement Element)
{
	ZEPathInfo PathInfo(Path);

	if (PathInfo.GetExtension().Upper() == ".ZESCENE")
	{
		ZEString Destination = ZEFormat::Format("{0}/{1}.new.ZEScene", 
			PathInfo.GetParentDirectory(),
			PathInfo.GetName());

		printf("Converting ZEScene file:\n");
		printf(" Source File: %s.\n", Path);
		printf(" Destination File: %s.\n", Destination.ToCString());

		return ZESceneConverter::Convert(Path, Destination);
	}

	return true;
}

int main(int argc, char** argv)
{
	printf(" Zinek Engine Scene Converter\n");
	printf("----------------------------------------------------------------------------------- \n");

	if (argc == 2 && argc == 3)
	{
		printf(" This tool is provided for converting old format zeScene to new ZEML based zeScene\n");
		printf(" format.\n");
		printf("\n");
		printf(" Usage:\n");
		printf("   ZESceneConverter [Old Format Scene File] [ZEML Based Scene File]\n");
		printf("   ZESceneConverter [Search Directory]\n");

		return EXIT_FAILURE;
	}

	ZEPathManager::GetInstance()->SetAccessControl(false);

	const char* Source = argv[1];
	const char* Destination = argv[2];
	if (argc == 3)
		return ZESceneConverter::Convert(Source, Destination) ? EXIT_SUCCESS : EXIT_FAILURE;
	else if (argc == 2)
		ZEPathInfo(Source).Operate(Source, ZEPathOperationFunction::Create<Operation>(), ZE_POE_FILE, true);

	return EXIT_SUCCESS;
}