//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECrashReportFileProvider.cpp
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

#include "ZECrashReportFileProvider.h"

#include <stdio.h>

#ifdef ZE_PLATFORM_WINDOWS
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#elif defined(ZE_PLATFORM_UNIX)
	#include <unistd.h>
#endif

ZECrashReportProviderType ZECrashReportFileProvider::GetProviderType()
{
	return Binary ? ZE_CRPT_BINARY : ZE_CRPT_TEXT;
}

const char*	ZECrashReportFileProvider::GetName()
{
	return Name;
}

void ZECrashReportFileProvider::SetName(const char* Name)
{
	this->Name = Name;
}

const char*	ZECrashReportFileProvider::GetFileName()
{
	return FileName;
}

void ZECrashReportFileProvider::SetFileName(const char* FileName)
{
	this->FileName = FileName;
}

void ZECrashReportFileProvider::SetDeleteOnExit(bool Delete)
{
	this->DeleteOnExit = Delete;
}

bool ZECrashReportFileProvider::GetDeleteOnExit()
{
	return DeleteOnExit;
}

void ZECrashReportFileProvider::SetBinary(bool Binary)
{
	this->Binary = Binary;
}

bool ZECrashReportFileProvider::GetBinary()
{
	return Binary;
}

ZESize ZECrashReportFileProvider::GetSize()
{
	return Size;
}

bool ZECrashReportFileProvider::GetData(void* Output, ZESize Offset, ZESize Size)
{
	if (File == NULL)
		return false;
	
	fseek((FILE*)File, Offset, SEEK_SET);
	if (fread(Output, 1, Size, (FILE*)File) != Size)
		return false;
	
	return true;
}

bool ZECrashReportFileProvider::Generate()
{
	File = fopen(FileName, "rb");
	
	if (File == NULL)
		return false;

	fseek((FILE*)File, 0, SEEK_END);
	Size = ftell((FILE*)File);

	return true;
}

void ZECrashReportFileProvider::CleanUp()
{
	if (File != NULL)
		fclose((FILE*)File);

	if (DeleteOnExit)
	{
		#ifdef ZE_PLATFORM_WINDOWS
				DeleteFile(FileName);
		#elif defined(ZE_PLATFORM_UNIX)
				unlink(FileName);
		#endif
	}
}

ZECrashReportFileProvider::ZECrashReportFileProvider()
{
	DeleteOnExit = true;
	Binary = false;
}
