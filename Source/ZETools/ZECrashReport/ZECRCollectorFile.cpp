//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECRCollectorFile.cpp
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

#include "ZECRCollectorFile.h"

#include "ZEPlatform.h"
#include "ZEFile/ZEFileInfo.h"
#include "ZEML/ZEMLReader.h"

#ifdef ZE_PLATFORM_WINDOWS
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#elif defined(ZE_PLATFORM_UNIX)
	#include <unistd.h>
#endif

#include <stdio.h>
#include "ZEML/ZEMLWriter.h"

ZECRDataProviderType ZECRCollectorFile::GetCollectorType()
{
	return Binary ? ZECR_DPT_BINARY : ZECR_DPT_TEXT;
}

ZEFile& ZECRCollectorFile::GetFile()
{
	return File;
}

const char*	ZECRCollectorFile::GetName()
{
	return Name;
}

const char* ZECRCollectorFile::GetExtension()
{
	return FileExtension;
}

void ZECRCollectorFile::SetName(const char* Name)
{
	this->Name = Name;
}

const char*	ZECRCollectorFile::GetFileName()
{
	return FileName;
}

void ZECRCollectorFile::SetFileName(const char* FileName)
{
	this->FileName = FileName;
	FileExtension = ZEFileInfo(FileName).GetExtension();
}

void ZECRCollectorFile::SetDeleteOnExit(bool Delete)
{
	this->DeleteOnExit = Delete;
}

bool ZECRCollectorFile::GetDeleteOnExit()
{
	return DeleteOnExit;
}

void ZECRCollectorFile::SetBinary(bool Binary)
{
	this->Binary = Binary;
}

bool ZECRCollectorFile::GetBinary()
{
	return Binary;
}

bool ZECRCollectorFile::Generate(ZEMLWriterNode* CollectorNode, const ZECRReportParameters* Parameters)
{
	ZEFile File;
	if (!File.Open(FileName, ZE_FOM_READ, ZE_FCM_NONE))
		return false;

	ZEArray<ZEBYTE> FileData;
	FileData.SetCount(File.GetSize());
	if (File.Read(FileData.GetCArray(), FileData.GetSize(), 1) != 1)
		return false;
	
	File.Close();

	if (!ZECRCollector::Generate(CollectorNode, Parameters))
		return false;

	if (!CollectorNode->WriteData("Data", FileData.GetConstCArray(), FileData.GetCount()))
		return false;

	if (DeleteOnExit)
	{
		ZEFileInfo FileInfo(FileName);
		FileInfo.Delete();
	}

	return true;
}

void ZECRCollectorFile::LoadConfiguration(const ZEMLReaderNode& ConfigurationNode)
{
	ZECRCollector::LoadConfiguration(ConfigurationNode);

	Name = ConfigurationNode.ReadString("Name");
	FileName = ConfigurationNode.ReadString("FileName");
	DeleteOnExit = ConfigurationNode.ReadBoolean("DeleteOnExit");
}

ZECRCollectorFile::ZECRCollectorFile()
{
	Size = 0;
	DeleteOnExit = true;
	Binary = false;
}
