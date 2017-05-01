//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECRProviderFile.cpp
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

#include "ZECRProviderFile.h"

#include "ZEPlatform.h"
#include "ZEFile/ZEFileInfo.h"

#ifdef ZE_PLATFORM_WINDOWS
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#elif defined(ZE_PLATFORM_UNIX)
	#include <unistd.h>
#endif

#include <stdio.h>

ZECRDataProviderType ZECRProviderFile::GetProviderType()
{
	return Binary ? ZECR_DPT_BINARY : ZECR_DPT_TEXT;
}

const char*	ZECRProviderFile::GetName()
{
	return Name;
}

const char* ZECRProviderFile::GetExtension()
{
	return FileExtension;
}

void ZECRProviderFile::SetName(const char* Name)
{
	this->Name = Name;
}

const char*	ZECRProviderFile::GetFileName()
{
	return FileName;
}

void ZECRProviderFile::SetFileName(const char* FileName)
{
	this->FileName = FileName;
	FileExtension = ZEFileInfo(FileName).GetExtension();
}

void ZECRProviderFile::SetDeleteOnExit(bool Delete)
{
	this->DeleteOnExit = Delete;
}

bool ZECRProviderFile::GetDeleteOnExit()
{
	return DeleteOnExit;
}

void ZECRProviderFile::SetBinary(bool Binary)
{
	this->Binary = Binary;
}

bool ZECRProviderFile::GetBinary()
{
	return Binary;
}

ZESize ZECRProviderFile::GetSize()
{
	return Size;
}

bool ZECRProviderFile::GetData(void* Output, ZESize Offset, ZESize Size)
{
	if (File == NULL)
		return false;
	
	fseek((FILE*)File, Offset, SEEK_SET);
	if (fread(Output, 1, Size, (FILE*)File) != Size)
		return false;
	
	return true;
}

bool ZECRProviderFile::Generate()
{
	File = fopen(FileName, "rb");
	
	if (File == NULL)
		return false;

	fseek((FILE*)File, 0, SEEK_END);
	Size = ftell((FILE*)File);

	return true;
}

void ZECRProviderFile::CleanUp()
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

ZECRProviderFile::ZECRProviderFile()
{
	DeleteOnExit = true;
	Binary = false;
}
