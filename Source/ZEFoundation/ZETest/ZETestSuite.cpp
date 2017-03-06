//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETestSuite.cpp
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

#include "ZETestSuite.h"
#include "ZETestItem.h"
#include "ZETestManager.h"

#include <string.h>
#include <stdio.h>
#include "ZETerminalUtils.h"



const char* ZETestSuiteItem::GetName()
{
	return Name;
}

void ZETestSuiteItem::RegisterTest(ZETestItem* Test)
{
	Tests[TotalTestCount] = Test;
	TotalTestCount++;
}

bool ZETestSuiteItem::RunTests()
{
	Reset();
	printf("Test Suite - %s\n\n", GetName());

	bool CurrentResult = true;
	for (ZESize I = 0; I < (ZESize)TotalTestCount; I++)
	{
		ZETerminalUtils::Reset();
		printf("Test ");
		ZETerminalUtils::SetBold(true);
		printf("#%u/%u", I + 1, TotalTestCount);
		ZETerminalUtils::Reset();
		printf(" - %s - ", Tests[I]->GetName());

		if (Tests[I]->RunTest())
		{
			PassedTestCount++;
			ZETerminalUtils::Reset();
			ZETerminalUtils::SetForgroundColor(ZE_TC_GREEN);
			ZETerminalUtils::SetBold(true);
			printf("PASSED");
			ZETerminalUtils::Reset();
			printf(" (%f ms)\n", Tests[I]->GetEleapsedTime());

		}
		else
		{
			printf(" Elapsed Time: %f ms\n\n", Tests[I]->GetEleapsedTime());
			CurrentResult = false;
		}

		ElapsedTime += Tests[I]->GetEleapsedTime();
	}

	Result = CurrentResult ? ZE_TR_PASSED : ZE_TR_FAILED;

	ZETerminalUtils::Reset();
	printf("\n\nTest suite \"%s\" has ", GetName());
	ZETerminalUtils::SetBold(true);
	if (CurrentResult)
	{
		ZETerminalUtils::SetForgroundColor(ZE_TC_GREEN);
		printf("PASSED\n");
	}
	else
	{
		ZETerminalUtils::SetForgroundColor(ZE_TC_RED);
		printf("FAILED\n");
	}

	ZETerminalUtils::Reset();
	printf(
		" Total Test Count: %u\n"
		" Passed Test Count: %u\n"
		" Failed Test Count: %u\n"
		" Elapsed Time: %f ms\n",
		TotalTestCount,
		PassedTestCount,
		TotalTestCount - PassedTestCount,
		ElapsedTime);

	return CurrentResult;
}

void ZETestSuiteItem::Reset()
{
	Result = ZE_TR_NOT_RUN;
	ElapsedTime = 0;
	PassedTestCount = 0;
	for (ZESize I = 0; I < (ZESize)TotalTestCount; I++)
		Tests[I]->Reset();
}

ZETestResult ZETestSuiteItem::GetResult()
{
	return Result;
}

double ZETestSuiteItem::GetElapsedTime()
{
	return ElapsedTime;
}

ZEUInt ZETestSuiteItem::GetTotalTestCount()
{
	return TotalTestCount;
}

ZEUInt ZETestSuiteItem::GetFailedTestCount()
{
	if (Result == ZE_TR_NOT_RUN)
		return 0;

	return TotalTestCount - PassedTestCount;
}

ZEUInt ZETestSuiteItem::GetPassedTestCount()
{
	return PassedTestCount;
}

ZETestSuiteItem::ZETestSuiteItem(const char* Name)
{
	Result = ZE_TR_NOT_RUN;
	ElapsedTime = 0;
	PassedTestCount = 0;
	strncpy(this->Name, Name, 255);
}

ZETestSuiteItemRegister::ZETestSuiteItemRegister(ZETestSuiteItem* Suite)
{
	ZETestManager::GetInstance()->RegisterTestSuite(Suite);
}
