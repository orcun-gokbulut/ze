//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECrashReportApplicationInformationProvider.cpp
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

#include "ZECrashReportApplicationInformationProvider.h"
#include "ZECIM.h"
#include "ZEDS/ZEFormat.h"
#include <memory.h>

#include <Windows.h>

ZECrashReportProviderType ZECrashReportApplicationInformationProvider::GetProviderType()
{
	return ZE_CRPT_TEXT;
}

const char* ZECrashReportApplicationInformationProvider::GetName()
{
	return "Application Information";
}

ZESize ZECrashReportApplicationInformationProvider::GetSize()
{
	return DataSize;
}

bool ZECrashReportApplicationInformationProvider::GetData(void* Output, ZESize Offset, ZESize Size)
{
	memcpy(Output, Data.GetValue() + Offset, Size);
	return true;
}

void ZECrashReportApplicationInformationProvider::SetVersion(const ZEVersion& Version)
{
	this->Version = Version;
}

const ZEVersion& ZECrashReportApplicationInformationProvider::GetVersion()
{
	return Version;
}

void ZECrashReportApplicationInformationProvider::SetProcessId(ZEUInt32 ProcessId)
{
	this->ProcessId = ProcessId;
}

ZEUInt32 ZECrashReportApplicationInformationProvider::GetProcessId()
{
	return ProcessId;
}

bool ZECrashReportApplicationInformationProvider::Generate()
{
	if (!ZECIM::Initialize())
		return false;

	Data += "<ZECrashReport>\n";
	Data += "<ApplicationInformation>\n";
	Data += "<Version>\n";

	Data += ZEFormat::Format("<{0}>{1}</{0}>\n", "Major", Version.Major);
	Data += ZEFormat::Format("<{0}>{1}</{0}>\n", "Minor", Version.Minor);
	Data += ZEFormat::Format("<{0}>{1}</{0}>\n", "Internal", Version.Internal);
	Data += ZEFormat::Format("<{0}>{1}</{0}>\n", "Revision", Version.Revision);
	Data += ZEFormat::Format("<{0}>{1}</{0}>\n", "Branch", Version.Branch);
	Data += ZEFormat::Format("<{0}>{1}</{0}>\n", "Platform", Version.Platform);
	Data += ZEFormat::Format("<{0}>{1}</{0}>\n", "Architecture", Version.Architecture);
	Data += "</Version>\n";
	Data += "<Process>\n";

	if(!ZECIM::ExecuteQuery(Data, "ROOT\\CIMV2", "WQL", ZEFormat::Format( "SELECT * FROM Win32_Process WHERE ProcessId = '{0}'", ProcessId)))
		return false;
	
	ZECIM::DeInitialize();

	Data += "</Process>\n";	
	Data += "</ApplicationInformation>\n";
	Data += "</ZECrashReport>\n";

	DataSize = Data.GetSize();

	return true;
}

ZECrashReportApplicationInformationProvider::ZECrashReportApplicationInformationProvider()
{
	DataSize = 0;
	ProcessId = 0;
}
