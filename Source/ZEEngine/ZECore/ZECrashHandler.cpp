//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECrashHandler.cpp
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

#include "ZECrashHandler.h"

#include "ZECore.h"
#include "ZEApplicationModule.h"
#include "ZELogSession.h"
#include "ZETools/ZECrashReport/ZECRReportParameters.h"
#include "ZEFile/ZEPathManager.h"

void ZECrashHandler::GenerateParameters(ZECRReportParameters& Parameters)
{
	ZELogSession* Session = ZELog::GetInstance()->GetRootSession();
	if (Session == NULL)
		Parameters.LogFilePath[0] = '\0';
	else
		strncpy(Parameters.LogFilePath, Session->GetLogFileName().ToCString(), 1024);

	ZEApplicationModule* Application = ZECore::GetInstance()->GetApplicationModule();
	if (Application == NULL)
	{
		Parameters.ApplicationName[0] = '\0';
		Parameters.ApplicationVersion = ZEVersion();		
		Parameters.LicenseProductName[0] = '\0';
		Parameters.LicenseLicenseeName[0] = '\0';
		Parameters.LicenseSerialKey[0] = '\0';
		Parameters.LicenseVersion = 0;
	}
	else
	{
		strncpy(Parameters.ApplicationName, Application->GetName(), 1024);
		Parameters.ApplicationVersion = Application->GetVersion();
		ZELCLicense License = Application->GetLicense();
		strncpy(Parameters.LicenseProductName, License.GetProductName().ToCString(), 1024);
		strncpy(Parameters.LicenseLicenseeName, License.GetLicenseeName().ToCString(), 1024);
		strncpy(Parameters.LicenseSerialKey, License.GetSerialKey().ToCString(), 1024);
		Parameters.LicenseVersion = License.GetLicenseVersion();
	}

	strncpy(Parameters.EnginePath, ZEPathManager::GetInstance()->GetEnginePath(), 1024);
	strncpy(Parameters.ResourcePath, ZEPathManager::GetInstance()->GetResourcePath(), 1024);
	strncpy(Parameters.StoragePath, ZEPathManager::GetInstance()->GetStoragePath(), 1024);
	strncpy(Parameters.UserStoragePath, ZEPathManager::GetInstance()->GetUserStoragePath(), 1024);
	strncpy(Parameters.SystemStoragePath, ZEPathManager::GetInstance()->GetSystemStoragePath(), 1024);
}

void ZECrashHandler::SetExecuteCrashReporter(bool Enabled)
{
	ExecuteCrashReporter = Enabled;
}

bool ZECrashHandler::GetExecuteCrashReporter() const
{
	return ExecuteCrashReporter;
}

ZECrashHandler* ZECrashHandler::CreateInstance()
{
	return new ZECrashHandler();
}
