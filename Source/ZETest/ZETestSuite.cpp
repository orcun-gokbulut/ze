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

	printf("Running Test Suite \"%s\".\n", GetName());

	bool CurrentResult = true;
	for (ZESize I = 0; I < TotalTestCount; I++)
	{
		printf("  Test #%d/%d - %s.\n", I + 1, TotalTestCount, Tests[I]->GetName());

		if (!Tests[I]->RunTest())
		{
			printf("  Test #%d/%d  - \"%s\"FAILED !!! Elapsed time : %f ms. \n", I + 1, TotalTestCount, Tests[I]->GetName(), Tests[I]->GetEleapsedTime());
			CurrentResult = false;
		}
		else
		{
			PassedTestCount++;
			printf("  Test #%d/%d - \"%s\" passed. Elapsed time : %f ms. \n", I + 1, TotalTestCount, Tests[I]->GetName(), Tests[I]->GetEleapsedTime());
		}

		ElapsedTime += Tests[I]->GetEleapsedTime();
	}

	Result = CurrentResult ? ZE_TR_PASSED : ZE_TR_FAILED;

	if (CurrentResult)
		printf("Test suite \"%s\" has passed. Total Time : %f ms \n", GetName(), ElapsedTime);
	else
		printf("Test suite \"%s\" has FAILED !!! Elapsed time : %f, Passed test count : %d, Failed test count : %d.\n", GetName(), ElapsedTime, PassedTestCount, TotalTestCount - PassedTestCount);

	return CurrentResult;
}

void ZETestSuiteItem::Reset()
{
	Result = ZE_TR_NOT_RUN;
	ElapsedTime = 0;
	PassedTestCount = 0;
	for (ZESize I = 0; I < TotalTestCount; I++)
		Tests[I]->Reset();
}

ZETestResult ZETestSuiteItem::GetResult()
{
	return Result;
}

float ZETestSuiteItem::GetElapsedTime()
{
	return ElapsedTime;
}

ZEInt ZETestSuiteItem::GetTotalTestCount()
{
	return TotalTestCount;
}

ZEInt ZETestSuiteItem::GetFailedTestCount()
{
	if (Result == ZE_TR_NOT_RUN)
		return 0;

	return TotalTestCount - PassedTestCount;
}

ZEInt ZETestSuiteItem::GetPassedTestCount()
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
