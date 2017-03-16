//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETestManager.cpp
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


#include "ZETestManager.h"
#include "ZEError.h"
#include "ZELogSession.h"

#include "ZETest.h"
#include "ZETestItem.h"
#include "ZETerminalUtils.h"

#ifndef NULL
#define NULL 0
#endif

void ZETestManager::ReportProblem(const ZETestProblem& Problem)
{
	if (MSBuildOutputEnabled)
	{
		const char* TypeString;
		if (Problem.Type == ZE_TPT_ERROR || Problem.Type == ZE_TPT_CRITICAL_ERROR)
			TypeString = "error";
		else
			TypeString = "warning";

		printf("%s(%d): %s ZE0001: Test item failed. Test: \"%s\". Test Item: \"%s\". Problem description: %s.\r\n", 
			TypeString, Problem.FileName, Problem.Line, Problem.Item->GetName().ToCString(), 
			Problem.Description.ToCString());
	}

	if (NormalOutputEnabled)
	{
		ZETerminalUtils::Reset();

		switch(Problem.Type)
		{
			case ZE_TPT_CRITICAL_ERROR:
				ZETerminalUtils::SetBold(true);
				ZETerminalUtils::SetForgroundColor(ZE_TC_RED);
				printf("Critical Error");
				break;

			case ZE_TPT_ERROR:
				ZETerminalUtils::SetBold(true);
				ZETerminalUtils::SetForgroundColor(ZE_TC_RED);
				printf("Error");
				break;

			case ZE_TPT_WARNING:
				ZETerminalUtils::SetForgroundColor(ZE_TC_YELLOW);
				printf("Warning");
				break;

			default:
				break;
		}

		ZETerminalUtils::Reset();
		printf(": %s\n", Problem.Description.ToCString());
	}
}

void ZETestManager::SetNormalOutputEnabled(bool Enabled)
{
	NormalOutputEnabled = Enabled;
}

bool ZETestManager::GetNormalOutputEnabled() const
{
	return NormalOutputEnabled;
}

void ZETestManager::SetMSBuildOutputEnabled(bool Enabled)
{
	MSBuildOutputEnabled = Enabled;
}

bool ZETestManager::GetMSBuildOutputEnabled() const
{
	return MSBuildOutputEnabled;
}

const ZEArray<ZETest*>& ZETestManager::GetTests() const
{
	return Tests;
}

void ZETestManager::RegisterTest(ZETest* Test)
{
	zeCheckError(Test == NULL, ZE_VOID, "Cannot register test. Test is NULL.");
	zeCheckError(Tests.Exists(Test), ZE_VOID, "Cannot register test. Test is already registered.");

	Tests.Add(Test);
}

void ZETestManager::UngregisterTest(ZETest* Test)
{
	zeCheckError(Test == NULL, ZE_VOID, "Cannot unregister test. Test is NULL.");
	zeCheckError(!Tests.Exists(Test), ZE_VOID, "Cannot unregister test. Test is not registered.");

	Tests.Add(Test);
}

bool ZETestManager::RunTests()
{
	ZELogSession LogSession;
	LogSession.SetMinimumLevel(ZE_LOG_ERROR);
	LogSession.BeginSession();

	ZEError::GetInstance()->SetBreakOnDebugCheckEnabled(false);
	ZEError::GetInstance()->SetBreakOnErrorEnabled(false);
	ZEError::GetInstance()->SetBreakOnWarningEnabled(false);

	bool Result = true;
	for (ZESize I = 0; I < Tests.GetCount(); I++)
	{
		Tests[I]->Run();
		if (Tests[I]->GetResult() != ZE_TR_PASSED)
			Result = false;

		if (NormalOutputEnabled && I + 1 != Tests.GetCount())
			printf("\n\n");
	}

	LogSession.EndSession();

	ZEError::GetInstance()->SetBreakOnDebugCheckEnabled(true);
	ZEError::GetInstance()->SetBreakOnErrorEnabled(true);
	ZEError::GetInstance()->SetBreakOnWarningEnabled(true);

	return Result;
}

ZETestManager::ZETestManager()
{
	NormalOutputEnabled = true;
	MSBuildOutputEnabled = false;
}

ZETestManager* ZETestManager::GetInstance()
{
	static ZETestManager Manager;
	return &Manager;
}
