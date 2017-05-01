//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECRPackager.cpp
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

#include "ZECRPackager.h"

#include "ZECRProvider.h"
#include "ZEFile\ZEFile.h"
#include "ZECompression\ZECompressorZLIB.h"
#include "ZEML\ZEMLWriter.h"
#include "ZETimeStamp.h"

void ZECRPackager::SetOutputFileName(const char* FileName)
{
	OutputFileName = FileName;
}

const char* ZECRPackager::GetOutputFileName()
{
	return OutputFileName;
}

void ZECRPackager::SetCrashReport(ZECRCrashReport* CrashReport)
{
	this->CrashReport = CrashReport;
}

ZECRCrashReport* ZECRPackager::GetCrashReport()
{
	return CrashReport;
}

bool ZECRPackager::Pack()
{
	ZEMLWriter Writer;
	ZEMLWriterNode RootNode;
	Writer.OpenRootNode("ZECRCrashReport", RootNode);

	RootNode.WriteUInt32("VersionMajor", 1);
	RootNode.WriteUInt32("VersionMinor", 0);
	RootNode.WriteString("ComputerName", "");

	ZETimeStamp TimeStamp = ZETimeStamp::Now();
	ZEMLWriterNode TimeStampNode;
	RootNode.OpenNode("TimeStamp", TimeStampNode);
	TimeStampNode.WriteInt16("Year", TimeStamp.GetYear());
	TimeStampNode.WriteInt16("Month", TimeStamp.GetMonth());
	TimeStampNode.WriteInt16("Day", TimeStamp.GetDay());
	TimeStampNode.WriteInt16("Hour", TimeStamp.GetHour());
	TimeStampNode.WriteInt16("Minute", TimeStamp.GetMinute());
	TimeStampNode.WriteInt16("Second", TimeStamp.GetSecond());
	TimeStampNode.CloseNode();

	ZEMLWriterNode ProvidersNode;
	RootNode.OpenNode("Providers", ProvidersNode);
	for (ZESize I = 0; I < CrashReport->GetProviders().GetCount(); I++)
	{
		ZECRProvider* Provider = CrashReport->GetProviders()[I];
		ZEMLWriterNode ProviderNode;
		ProvidersNode.OpenNode("Provider", ProvidersNode);
		
		ProviderNode.WriteString("Name", Provider->GetName());
		ProviderNode.WriteString("Extension", Provider->GetExtension());

		ZEArray<ZEBYTE> Data;
		Data.SetCount(Provider->GetSize());
		Provider->GetData(Data.GetCArray(), 0, Data.GetSize());
		
		ProviderNode.WriteData("Data",  Data.GetCArray(), Data.GetCount());

		ProviderNode.CloseNode();
	}
	ProvidersNode.CloseNode();
	RootNode.CloseNode();

	return true;
}
