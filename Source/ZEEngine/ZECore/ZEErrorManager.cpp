//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEErrorManager.cpp
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

#include "ZEErrorManager.h"

#include "ZECore.h"
#include "ZELogSession.h"
#include "ZETimeStamp.h"
#include "ZEError.h"
#include "ZEDS/ZEFormat.h"

#include "ZEConsole.h"
#include "ZEConsoleWindow.h"
#include "ZEOptionManager.h"

#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <crtdbg.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static ZEOptionSection ErrorOptions; 

void ZEErrorManager::OnOptionChanged(ZEOption* Option)
{
	ZELogSession* RootSession = ZELog::GetInstance()->GetRootSession();

	if (Option->GetName() == "MinimumLevel")
	{
		ZEString Value = Option->GetValue().GetString().Lower().Trim();
		if (Value == "critical" || Value == "critical error" || Value == "criticalerror")
			RootSession->SetMinimumLevel(ZE_LOG_CRITICAL_ERROR);
		else if (Value == "error")
			RootSession->SetMinimumLevel(ZE_LOG_ERROR);
		else if (Value == "warning")
			RootSession->SetMinimumLevel(ZE_LOG_WARNING);
		else if (Value == "info")
			RootSession->SetMinimumLevel(ZE_LOG_INFO);
		else if (Value == "debug")
			RootSession->SetMinimumLevel(ZE_LOG_DEBUG);
	}
	else if (Option->GetName() == "LogFileEnabled")
	{
		if (Option->GetValue().GetBoolean())
		{		
			char ComputerName[256];
			DWORD Size = sizeof(ComputerName);
			GetComputerNameA(ComputerName, &Size);

			ZEString TimeStamp = ZETimeStamp::Now().ToString("%Y%m%d%H%M");
			ZEString LogFile = ZEOptionManager::GetInstance()->GetOption("Error", "LogFilePath")->GetValue().GetString();
			ZEString Header = ZELogSession::GenerateLogFileHeader();

			RootSession->OpenLogFile(ZEFormat::Format(LogFile, TimeStamp, ComputerName), Header, true);
		}
		else
		{
			RootSession->CloseLogFile();
		}
	}
	else if (Option->GetName() == "LogFilePath")
	{

	}
	else if (Option->GetName() == "LogFileMinimumLevel")
	{
		ZEString Value = Option->GetValue().GetString().Lower().Trim();
		if (Value == "critical" || Value == "critical error" || Value == "criticalerror")
			RootSession->SetLogFileMinLevel(ZE_LOG_CRITICAL_ERROR);
		else if (Value == "error")
			RootSession->SetLogFileMinLevel(ZE_LOG_ERROR);
		else if (Value == "warning")
			RootSession->SetLogFileMinLevel(ZE_LOG_WARNING);
		else if (Value == "info")
			RootSession->SetLogFileMinLevel(ZE_LOG_INFO);
		else if (Value == "debug")
			RootSession->SetLogFileMinLevel(ZE_LOG_DEBUG);
	}
}

void ZEErrorManager::ErrorCallback(ZEErrorType Level)
{
	if (Level == ZE_ET_ERROR)
		ZEConsole::GetInstance()->ShowConsole();

	if (Level == ZE_ET_CRITICAL_ERROR)
	{
		zeCore->Terminate();
		ZEConsoleWindow ConsoleWindow;
		ZECore::GetInstance()->GetConsole()->SetConsoleInterface(&ConsoleWindow);
		ConsoleWindow.TerminationState();
		ZECore::GetInstance()->GetConsole()->SetConsoleInterface(NULL);
		exit(0);
	}
}

ZEErrorManager* ZEErrorManager::GetInstance()
{
	return ZECore::GetInstance()->GetError();
}

ZEErrorManager::ZEErrorManager()
{
	ErrorOptions.SetName("Error");
	ErrorOptions.AddOption(new ZEOption("MinimumLevel", "Info", ZE_OA_NORMAL));
	ErrorOptions.AddOption(new ZEOption("LogFileEnabled", true, ZE_OA_NORMAL));
	ErrorOptions.AddOption(new ZEOption("LogFilePath", "#S:/Logs/ZinekEngine-{0}.log", ZE_OA_NORMAL));
	ErrorOptions.AddOption(new ZEOption("LogFileMinimumLevel", "Warning", ZE_OA_NORMAL));
	ErrorOptions.SetOnChanged(ZEOptionsChangedEvent::Create<ZEErrorManager, &ZEErrorManager::OnOptionChanged>(this));


	ZEOptionManager::GetInstance()->RegisterSection(&ErrorOptions);
	ZEError::GetInstance()->SetCallback(ZEErrorCallback::Create<&ZEErrorManager::ErrorCallback>());
}

ZEErrorManager::~ZEErrorManager()
{
	ZEOptionManager::GetInstance()->UnregisterSection(&ErrorOptions);
}
