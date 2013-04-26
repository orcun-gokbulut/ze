//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEIGHardwareActivator.cpp
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

#include "ZEDS/ZEFormat.h"
#include "md5.h"

#include <stdio.h>
#include <intrin.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int main(int argc, char** argv)
{
	printf("Activating this hardware as ZinekIG hardware.");
	char SystemDirectoryPath[256];
	GetSystemDirectory(SystemDirectoryPath, 256);

	char ActivationFile[512];
	sprintf(ActivationFile, "%s\\zinekig.act", SystemDirectoryPath);

	char ActivationCode[256];
	memset(ActivationCode, 0, 256);

	FILE* File = fopen(ActivationFile, "w");
	if (File == NULL)
	{
		printf("Activation failed please run as administrator.");
		return EXIT_FAILURE;
	}

	int Values[4];
	__cpuid(Values, 0);
	ZEString Key = ZEFormat::Format("8961a9-8sd4f32hl%{0:X:08}%RcasT435%{1:X:08}%4fdgd24**45%{2:X:08}%EaBXA313", Values[1], Values[2], Values[3]);
	std::string GeneratedActivationCode = md5(Key.ToCString());
	
	fputs(GeneratedActivationCode.c_str(), File);
	fclose(File);

	printf("Done.");
}
