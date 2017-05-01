//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECRProviderSystemInformation.cpp
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

#include "ZECRProviderSystemInformation.h"
#include "ZECRCIM.h"

ZECRDataProviderType ZECRProviderSystemInformation::GetProviderType()
{
	return ZECR_DPT_TEXT;
}

const char* ZECRProviderSystemInformation::GetName()
{
	return "System Information";
}

const char* ZECRProviderSystemInformation::GetExtension()
{
	return ".xml"
}

ZESize ZECRProviderSystemInformation::GetSize()
{
	return DataSize;
}

bool ZECRProviderSystemInformation::GetData(void* Output, ZESize Offset, ZESize Size)
{
	memcpy(Output, Data.GetValue() + Offset, Size);
	return true;
}

bool ZECRProviderSystemInformation::Generate()
{	
	if (!ZECRCIM::Initialize())
		return false;
	Data += "<ZECrashReport>\n";
	Data += "<SystemInformation>\n";

	Data += "<Processor>\n";
	if(!ZECRCIM::ExecuteQuery(Data, "ROOT\\CIMV2", "WQL", "SELECT * FROM Win32_Processor"))
		return false;
	Data += "</Processor>\n";

	Data += "<VideoController>\n";
	if(!ZECRCIM::ExecuteQuery(Data, "ROOT\\CIMV2", "WQL", "SELECT * FROM Win32_VideoController"))
		return false;
	Data += "</VideoController>\n";

	Data += "<SoundDevice>\n";
	if(!ZECRCIM::ExecuteQuery(Data, "ROOT\\CIMV2", "WQL", "SELECT * FROM Win32_SoundDevice"))
		return false;
	Data += "</SoundDevice>\n";

	Data += "<OperatingSystem>\n";
	if(!ZECRCIM::ExecuteQuery(Data, "ROOT\\CIMV2", "WQL", "SELECT * FROM Win32_OperatingSystem"))
		return false;
	Data += "</OperatingSystem>\n";

	Data += "<BaseBoard>\n";
	if(!ZECRCIM::ExecuteQuery(Data, "ROOT\\CIMV2", "WQL", "SELECT * FROM Win32_BaseBoard"))
		return false;
	Data += "</BaseBoard>\n";

	Data += "<DesktopMonitor>\n";
	if(!ZECRCIM::ExecuteQuery(Data, "ROOT\\CIMV2", "WQL", "SELECT * FROM Win32_DesktopMonitor"))
		return false;
	Data += "</DesktopMonitor>\n";

	Data += "<Keyboard>\n";
	if(!ZECRCIM::ExecuteQuery(Data, "ROOT\\CIMV2", "WQL", "SELECT * FROM Win32_Keyboard"))
		return false;
	Data += "</Keyboard>\n";

	Data += "<PointingDevice>\n";
	if(!ZECRCIM::ExecuteQuery(Data, "ROOT\\CIMV2", "WQL", "SELECT * FROM Win32_PointingDevice"))
		return false;
	Data += "</PointingDevice>\n";

	Data += "<LogicalDisk>\n";
	if(!ZECRCIM::ExecuteQuery(Data, "ROOT\\CIMV2", "WQL", "SELECT * FROM Win32_LogicalDisk"))
		return false;
	Data += "</LogicalDisk>\n";
	
	Data += "<DiskDrive>\n";
	if(!ZECRCIM::ExecuteQuery(Data, "ROOT\\CIMV2", "WQL", "SELECT * FROM Win32_DiskDrive"))
		return false;
	Data += "</DiskDrive>\n";

	/*Data += "<PnPSignedDriver>\n";
	if(!ZECIM::ExecuteQuery(Data, "ROOT\\CIMV2", "WQL", "SELECT * FROM Win32_PnPSignedDriver"))
		return false;
	Data += "</PnPSignedDriver>\n";*/
	
	Data += "</SystemInformation>\n";
	Data += "</ZECrashReport>\n";

	DataSize = Data.GetLength() + 1;

	ZECRCIM::DeInitialize();
	return true;
}

ZECRProviderSystemInformation::ZECRProviderSystemInformation()
{
	DataSize = 0;
}

