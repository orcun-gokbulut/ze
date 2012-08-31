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

#include "ZECore.h"
#include "ZEError.h"
#include "ZEConsole.h"
#include "ZEErrorManager.h"
#include "ZEConsoleWindow.h"
#include "ZEOptionManager.h"

#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <crtdbg.h>

static ZEOptionSection ErrorOptions; 

bool ZEErrorManager::OptionCallback_General(ZEOption* Option, ZETypedVariant* Value)
{
	if (Option->GetName() == "FileLogging")
		ZELog::GetInstance()->SetLogFileEnabled(Option->GetValue().GetBoolean());
	else if (Option->GetName() == "LogFile")
		ZELog::GetInstance()->SetLogFileName(Option->GetValue().GetString());

	return true;
}

void ZEErrorManager::ErrorCallback(ZEErrorType Level)
{
	if (Level == ZE_ET_ERROR)
		ZEConsole::GetInstance()->ShowConsole();

	if (Level == ZE_ET_CRITICAL_ERROR)
	{
		zeCore->SetCoreState(ZE_CS_CRITICAL_ERROR);
		ZEConsoleWindow ConsoleWindow;
		ZECore::GetInstance()->GetConsole()->SetConsoleInterface(&ConsoleWindow);
		ConsoleWindow.TermiantionState();
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
	ZELog::GetInstance()->SetLogFileEnabled(false);
	ErrorOptions.SetName("Error");
	ErrorOptions.AddOption(new ZEOption("LogFileEnabled", false, ZE_OA_NORMAL));
	ErrorOptions.AddOption(new ZEOption("LogFileName", "error.log", ZE_OA_NORMAL));
	ZEOptionManager::GetInstance()->RegisterSection(&ErrorOptions);
	ZEError::GetInstance()->SetCallback(&ZEErrorManager::ErrorCallback);
}

ZEErrorManager::~ZEErrorManager()
{
	ZEOptionManager::GetInstance()->UnregisterSection(&ErrorOptions);
}
