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
#include "ZETestSuite.h"
#include "ZETestItem.h"
#include "ZEError.h"

#include <stdio.h>

#ifndef NULL
#define NULL 0
#endif

#include <string.h>
#include "ZELogSession.h"

void ZETestManager::SetVisualStudioOutput(bool Enabled)
{
	VisualStudioOutput = Enabled;
}
bool ZETestManager::GetVisualStudioOutput()
{
	return VisualStudioOutput;
}

void ZETestManager::RegisterTestSuite(ZETestSuiteItem* Suite)
{
	TestSuites[TestSuiteCount] = Suite;
	TestSuiteCount++;
}

void ZETestManager::SetPackageName(const char* Name)
{
	strncpy(PackageName, Name, 255);
}

const char* ZETestManager::GetPackageName()
{
	return PackageName;
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
	for (ZESize I = 0; I < TestSuiteCount; I++)
	{
		try
		{
			if (!TestSuites[I]->RunTests())
				Result = false;
		}
		catch(...)
		{
			printf("%s : error T0004: Exception occurred. Package Name : \"%s\". \r\n", GetPackageName(), GetPackageName());

			Result = false;
			break;
		}
	}
	
	if (!Result)
	{
		printf("%s : error T0003: Test package failed. Package Name : \"%s\". \r\n", GetPackageName(), GetPackageName());
	}

	LogSession.EndSession();

	ZEError::GetInstance()->SetBreakOnDebugCheckEnabled(true);
	ZEError::GetInstance()->SetBreakOnErrorEnabled(true);
	ZEError::GetInstance()->SetBreakOnWarningEnabled(true);

	return Result;
}

void ZETestManager::ReportProblem(ZETestSuiteItem* Suite, ZETestItem* Test, ZETestProblemType Type, const char* ProblemText, const char* File, ZEInt Line)
{
	const char* TypeString;

	switch(Type)
	{
		default:
		case ZE_TPT_ERROR:
			TypeString = "error";
			break;

		case ZE_TPT_WARNING:
			TypeString = "warning";
			break;

		case ZE_TPT_NOTICE:
			TypeString = "info";
			break;
	}

	if (Type == ZE_TPT_ERROR)
	{
		printf("  Test failed. Suite : \"%s\", Test : \"%s\", Case : \"%s\". %s.\r\n", Suite->GetName(), Test->GetName(), Test->GetCurrentCase(), ProblemText);
		if (VisualStudioOutput)
			printf("  %s(%d) : warning T0001: Test failed. Suite : \"%s\", Test : \"%s\", Case : \"%s\". %s.\r\n", File, Line, Suite->GetName(), Test->GetName(), Test->GetCurrentCase(), ProblemText);
	}
	else
	{
		printf("  Test gave warning. Suite : \"%s\", Test : \"%s\", Case : \"%s\". %s.\r\n", Suite->GetName(), Test->GetName(), Test->GetCurrentCase(), ProblemText);
		if (VisualStudioOutput)
			printf("  %s(%d) : warning T0002: Test gave warning. Suite : \"%s\", Test : \"%s\", Case : \"%s\". %s.\r\n", File, Line, Suite->GetName(), Test->GetName(), Test->GetCurrentCase(), ProblemText);
	}
}

ZETestManager* ZETestManager::GetInstance()
{
	static ZETestManager* Instance = NULL;
	if (Instance == NULL)
		Instance = new ZETestManager();

	return Instance;
}

ZETestManager::ZETestManager()
{
	VisualStudioOutput = false;
	TestSuiteCount = 0;
	PackageName[0] = '\0';
}
