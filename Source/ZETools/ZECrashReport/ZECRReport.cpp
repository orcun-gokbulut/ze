//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECRReport.cpp
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

#include "ZECRReport.h"
#include "ZECRCollector.h"
#include "ZETimeStamp.h"
#include "ZEDS/ZEFormat.h"
#include "ZEFile/ZEDirectoryInfo.h"
#include "ZEFile/ZEFileInfo.h"
#include "ZEML/ZEMLWriter.h"
#include "ZEML/ZEMLReader.h"
#include "ZECRReportParameters.h"

void ZECRReport::GenerateReportFileName(const ZECRReportParameters* Parameters, const ZEGUID& GUID, const ZETimeStamp& TimeStamp)
{
	if (!ReportFileName.IsEmpty())
		return;

	ZEString FileName = ZEFormat::Format(ReportFilePattern, 
		ZEFileInfo(Parameters->Executable).GetName(), 
		GUID.ToString(),
		TimeStamp.ToString("%Y%m%d%H%M%S"),
		"");

	ReportFileName = ZEFormat::Format("{0}/{1}", ReportFileDirectory, FileName);
}

struct ZEReportQuoteEntry
{
	ZEFileInfo FileInfo;
	ZETimeStamp ModificationTime;
};

static int SortByDate(const ZEReportQuoteEntry* A, const ZEReportQuoteEntry* B)
{
	if (A < B)
		return -1;
	else
		return 1;
}

void ZECRReport::ManageReportQuote()
{
	if (ReportFileQuota < 0)
		return;

	ZEDirectoryInfo DirectoryInfo(ReportFileDirectory);
	ZEArray<ZEString> Files = DirectoryInfo.GetFiles();

	ZEArray<ZEReportQuoteEntry> QuoteEntires;
	for (ZESize I = 0; I < Files.GetCount(); I++)
	{
		ZEReportQuoteEntry NewEntry;
		NewEntry.FileInfo.SetPath(ZEFormat::Format("{0}/{1}", DirectoryInfo.GetPath(), Files[I]));
		
		if (NewEntry.FileInfo.GetExtension() != ".ZECRReport")
			continue;

		NewEntry.ModificationTime = NewEntry.FileInfo.GetModificationTime();
		QuoteEntires.Add(NewEntry);
	}

	if (QuoteEntires.GetCount() < ReportFileQuota)
		return;

	QuoteEntires.Sort<SortByDate>();

	for (ZESize I = 0; I < QuoteEntires.GetCount() - ReportFileQuota + 1; I++)
		QuoteEntires[I].FileInfo.Delete();
}


const ZEArray<ZECRCollector*>& ZECRReport::GetCollectors()
{
	return Collectors;
}

ZECRCollector* ZECRReport::GetCollector(const ZEString& Name)
{
	for (ZESize I = 0; I < Collectors.GetCount(); I++)
	{
		if (Name == Collectors[I]->GetName())
			return Collectors[I];
	}

	return NULL;
}

bool ZECRReport::AddCollector(ZECRCollector* Collector)
{
	zeCheckError(Collector == NULL, false, "Cannot remove collector. Collector is NULL.");
	zeCheckError(Collector->Report != NULL, false, "Cannot remove collector. Collector has been already added to a report.");

	Collectors.Add(Collector);

	return true;
}

void ZECRReport::RemoveCollector(ZECRCollector* Collector)
{
	zeCheckError(Collector == NULL, ZE_VOID, "Cannot remove collector. Collector is NULL.");
	zeCheckError(Collector->Report != this, ZE_VOID, "Cannot remove collector. Collector does not belong to this report.");

	Collectors.RemoveValue(Collector);
}

void ZECRReport::SetReportFileDirectory(const ZEString& Directory)
{
	ReportFileDirectory = Directory;
}
const ZEString& ZECRReport::GetReportFileDirectory()
{
	return ReportFileDirectory;
}

void ZECRReport::SetReportFileNamePattern(const ZEString& Pattern)
{
	ReportFilePattern = Pattern;
}
const ZEString& ZECRReport::GetReportFileNamePattern()
{
	return ReportFilePattern;
}

void ZECRReport::SetReportFileName(const ZEString& FileName)
{
	ReportFileName = FileName;
}

const ZEString& ZECRReport::GetReportFileName()
{
	return ReportFileName;
}

bool ZECRReport::Generate(const ZECRReportParameters* Parameters)
{
	ZEString ExecutableName = ZEFileInfo(Parameters->Executable).GetFileName();
	ZEGUID GUID = ZEGUID::Generate();
	ZETimeStamp TimeStamp = ZETimeStamp::Now();

	ManageReportQuote();
	GenerateReportFileName(Parameters, GUID, TimeStamp);

	ZEMLWriter Writer;
	if (!Writer.Open(ReportFileName))
		return false;

	ZEMLWriterNode RootNode;
	Writer.OpenRootNode("ZECRReport", RootNode);

	RootNode.WriteUInt32("VersionMajor", 1);
	RootNode.WriteUInt32("VersionMinor", 0);

	RootNode.WriteString("GUID", GUID.ToString());
	RootNode.WriteString("Executable", ExecutableName);
	RootNode.WriteUInt64("TimeStamp", TimeStamp.ToCTime());
	RootNode.WriteString("ComputerName", "");

	ZEMLWriterNode CollectorsNode;
	RootNode.OpenNode("Collectors", CollectorsNode);
	for (ZESize I = 0; I < Collectors.GetCount(); I++)
	{
		ZECRCollector* Collector =  Collectors[I];
		ZEMLWriterNode CollectorNode;
		CollectorsNode.OpenNode("Collector", CollectorNode);

		CollectorNode.WriteString("Name", Collector->GetName());
		CollectorNode.WriteString("Extension", Collector->GetExtension());
		CollectorNode.WriteUInt8("Type", Collector->GetCollectorType());

		Collector->Generate(&CollectorNode, Parameters);

		CollectorNode.CloseNode();
	}
	CollectorsNode.CloseNode();
	RootNode.CloseNode();

	return true;
}

bool ZECRReport::LoadConfiguration()
{
	return LoadConfiguration("#E:/ZECRReport.ZEConfig");
}

bool ZECRReport::LoadConfiguration(const ZEString& FileName)
{
	ZEMLReader Reader;
	if (!Reader.Open(FileName))
		return false;

	ZEMLReaderNode ReportNode = Reader.GetRootNode();
	if (ReportNode.GetName() != "ZECRReport")
		return false;

	return LoadConfiguration(&ReportNode);
}

bool ZECRReport::LoadConfiguration(ZEMLReaderNode* ReportNode)
{
	return true;
}

ZECRReport::ZECRReport()
{
	ReportFileDirectory = "#S:/CrashReports";
	ReportFilePattern = "{0}-{2}.ZECRReport";
	ReportFileQuota = 5;
}

ZECRReport::~ZECRReport()
{

}
