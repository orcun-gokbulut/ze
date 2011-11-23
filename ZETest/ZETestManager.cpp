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
	bool Result = true;
	for (size_t I = 0; I < TestSuiteCount; I++)
	{
		if (!TestSuites[I]->RunTests())
			Result = false;
	}
	
	if (!Result)
		printf("error T0003: Test package \"%s\" has failed. \r\n", GetPackageName());

	return Result;
}

void ZETestManager::ReportProblem(ZETestSuiteItem* Suite, ZETestItem* Test, ZETestProblemType Type, const char* ProblemText, const char* File, int Line)
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
		printf("  %s(%d) : info T0001: Test \"%s::%s\" \"%s\" failed. %s \r\n", File, Line, TypeString, Suite->GetName(), Test->GetName(), Test->GetCurrentCase(), ProblemText);
	else
		printf("  %s(%d) : info T0002: Test \"%s::%s\" \"%s\" gives warning. %s \r\n", File, Line, TypeString, Suite->GetName(), Test->GetName(), Test->GetCurrentCase(), ProblemText);
}

ZETestManager* ZETestManager::GetInstance()
{
	static ZETestManager* Instance = NULL;
	if (Instance == NULL)
		Instance = new ZETestManager();

	return Instance;
}
