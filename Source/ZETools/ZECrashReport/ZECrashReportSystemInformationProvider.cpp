//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECrashReportSystemInformationProvider.cpp
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

#include "ZECrashReportSystemInformationProvider.h"
#include "ZECIM.h"

ZECrashReportProviderType ZECrashReportSystemInformationProvider::GetProviderType()
{
	return ZE_CRPT_SYSTEM_INFORMATION;
}

const char* ZECrashReportSystemInformationProvider::GetName()
{
	return "System Information";
}

ZESize ZECrashReportSystemInformationProvider::GetSize()
{
	return DataSize;
}

bool ZECrashReportSystemInformationProvider::GetData(void* Output, ZESize Offset, ZESize Size)
{
	memcpy(Output, Data.GetValue() + Offset, Size);
	return true;
}

bool ZECrashReportSystemInformationProvider::Generate()
{	
	if (!ZECIM::Initialize())
		return false;
	Data += "<ZECrashReport>\n";
	Data += "<SystemInformation>\n";

	Data += "<Processor>\n";
	if(!ZECIM::ExecuteQuery(Data, "ROOT\\CIMV2", "WQL", "SELECT * FROM Win32_Processor"))
		return false;
	Data += "</Processor>\n";

	Data += "<VideoController>\n";
	if(!ZECIM::ExecuteQuery(Data, "ROOT\\CIMV2", "WQL", "SELECT * FROM Win32_VideoController"))
		return false;
	Data += "</VideoController>\n";

	Data += "<SoundDevice>\n";
	if(!ZECIM::ExecuteQuery(Data, "ROOT\\CIMV2", "WQL", "SELECT * FROM Win32_SoundDevice"))
		return false;
	Data += "</SoundDevice>\n";

	Data += "<OperatingSystem>\n";
	if(!ZECIM::ExecuteQuery(Data, "ROOT\\CIMV2", "WQL", "SELECT * FROM Win32_OperatingSystem"))
		return false;
	Data += "</OperatingSystem>\n";

	Data += "<BaseBoard>\n";
	if(!ZECIM::ExecuteQuery(Data, "ROOT\\CIMV2", "WQL", "SELECT * FROM Win32_BaseBoard"))
		return false;
	Data += "</BaseBoard>\n";

	Data += "<DesktopMonitor>\n";
	if(!ZECIM::ExecuteQuery(Data, "ROOT\\CIMV2", "WQL", "SELECT * FROM Win32_DesktopMonitor"))
		return false;
	Data += "</DesktopMonitor>\n";

	Data += "<Keyboard>\n";
	if(!ZECIM::ExecuteQuery(Data, "ROOT\\CIMV2", "WQL", "SELECT * FROM Win32_Keyboard"))
		return false;
	Data += "</Keyboard>\n";

	Data += "<PointingDevice>\n";
	if(!ZECIM::ExecuteQuery(Data, "ROOT\\CIMV2", "WQL", "SELECT * FROM Win32_PointingDevice"))
		return false;
	Data += "</PointingDevice>\n";

	Data += "<LogicalDisk>\n";
	if(!ZECIM::ExecuteQuery(Data, "ROOT\\CIMV2", "WQL", "SELECT * FROM Win32_LogicalDisk"))
		return false;
	Data += "</LogicalDisk>\n";
	
	Data += "<DiskDrive>\n";
	if(!ZECIM::ExecuteQuery(Data, "ROOT\\CIMV2", "WQL", "SELECT * FROM Win32_DiskDrive"))
		return false;
	Data += "</DiskDrive>\n";

	/*Data += "<PnPSignedDriver>\n";
	if(!ZECIM::ExecuteQuery(Data, "ROOT\\CIMV2", "WQL", "SELECT * FROM Win32_PnPSignedDriver"))
		return false;
	Data += "</PnPSignedDriver>\n";*/
	
	Data += "</SystemInformation>\n";
	Data += "</ZECrashReport>\n";

	DataSize = Data.GetLength() + 1;

	ZECIM::DeInitialize();
	return true;
}

ZECrashReportSystemInformationProvider::ZECrashReportSystemInformationProvider()
{
	DataSize = 0;
}

