//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEITIntegrityGenerator.cpp
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

#include "ZEITIntegrityGenerator.h"
#include "ZEML\ZEMLReader.h"
#include "ZEFile\ZEDirectoryInfo.h"
#include "ZEML\ZEMLWriter.h"
#include "ZEDS\ZEFormat.h"
#include "ZEML\ZEMLFormat.h"
#include <omp.h>

const ZEArray<ZEITIntegrityRecord> ZEITIntegrityGenerator::GetRecords() const
{
	return Records;
}

void ZEITIntegrityGenerator::AddRecord(const ZEITIntegrityRecord& Record)
{
	Lock.Lock();
	Records.Add(Record);
	Lock.Unlock();
}

void ZEITIntegrityGenerator::RemoveRecord(const ZEITIntegrityRecord& Record)
{
	Lock.Lock();
	for (ZESize I = 0; I < Records.GetCount(); I++)
	{
		if (Records[I].GetPath() == Record.GetPath())
		{
			Records.Remove(I);
			break;
		}
	}
	Lock.Unlock();
}

void ZEITIntegrityGenerator::ScanDirectory(const ZEString& Path, bool Recursive)
{
	zeLog("Scanning Directory: %s", Path.ToCString());

	ZEDirectoryInfo Info(Path);
	ZEArray<ZEString> Files = Info.GetFiles();
	
	int Count = omp_get_max_threads();
	omp_set_num_threads(Count);

	#pragma omp parallel for
	for (ZESSize I = 0; I < Files.GetCount(); I++) 
	{
		ZEITIntegrityRecord Record;
		Record.SetPath(ZEFormat::Format("{0}/{1}", Path, Files[I]));
		Record.SetRequired(true);
		Record.SetType(ZEIT_RT_FILE);
		zeLog("Generating Record: %s, Thread ID: %d", Record.GetPath().ToCString(), omp_get_thread_num());
		Record.Generate();
		AddRecord(Record);
	}

	if (!Recursive)
		return;

	ZEArray<ZEString> SubDirectories = Info.GetSubDirectories();
	for (ZESize I = 0; I < SubDirectories.GetCount(); I++) 
		ScanDirectory(ZEFormat::Format("{0}/{1}", Path, SubDirectories[I]), true);
}


void ZEITIntegrityGenerator::SetIntegrityFileName(const ZEString& FileName)
{
	IntegrityFile = FileName;
}

const ZEString& ZEITIntegrityGenerator::GetIntegrityFileName() const
{
	return IntegrityFile;
}

bool ZEITIntegrityGenerator::Load()
{
	ZEMLReader Reader;
	if (!Reader.Open(IntegrityFile))
	{
		zeError("Cannot read Integrity file. File Name: \"%s\".", IntegrityFile.ToCString());
		return false;
	}

	ZEMLReaderNode RootNode = Reader.GetRootNode();
	if (RootNode.GetName() != "ZEITIntegrity")
	{
		zeError("Unknown Integrity file format. File Name: \"%s\".", IntegrityFile.ToCString());
		return false;
	}

	ZEMLReaderNode RecordsNode = RootNode.GetNode("Records");

	ZESize RecordCount = RecordsNode.GetNodeCount("Record");
	Records.SetCount(RecordCount);
	for (ZESize I = 0; I < RecordCount; I++)
	{
		ZEMLReaderNode RecordNode = RootNode.GetNode("Record", I);
		Records[I].Load(&RecordNode);
	}

	return true;
}

bool ZEITIntegrityGenerator::Save()
{
	ZEMLWriter Writer;
	Writer.SetFormat(ZEMLFormat::GetDefaultTextFormat()->CreateInstance());
	if (!Writer.Open(IntegrityFile))
	{
		zeError("Cannot writer Integrity file. File Name: \"%s\".", IntegrityFile.ToCString());
		return false;
	}

	ZEMLWriterNode RootNode;
	Writer.OpenRootNode("ZEIntegrity", RootNode);

	ZEMLWriterNode RecordsNode;
	RootNode.OpenNode("Records", RecordsNode);

	for (ZESize I = 0; I < Records.GetCount(); I++)
		Records[I].Save(&RecordsNode);

	RecordsNode.CloseNode();
	RootNode.CloseNode();
	Writer.Close();

	return true;
}
