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

const char* ZETestSuite::GetName()
{
	return Name;
}

void ZETestSuite::RegisterTest(ZETestItem* Test)
{
	Tests[TestCount] = Test;
	TestCount++;
}

bool ZETestSuite::RunTests()
{
	Reset();

	bool CurrentResult = true;
	for (size_t I = 0; I < TestCount; I++)
	{
		printf("    Test #%d - %s.\n", I, Tests[I]->GetName());

		if (!Tests[I]->RunTest())
		{
			printf("    FAILED. \n", I, Tests[I]->GetName());
			CurrentResult = false;
		}
		else
		{
			printf("    PASSED. \n", I, Tests[I]->GetName());
		}
	}

	Result = CurrentResult ? ZE_TR_PASSED : ZE_TR_FAILED;

	return CurrentResult;
}

void ZETestSuite::Reset()
{
	Result = ZE_TR_NOT_RUN;
	for (size_t I = 0; I < TestCount; I++)
		Tests[I]->Reset();
}

ZETestResult ZETestSuite::GetResult()
{
	return Result;
}

ZETestSuite::ZETestSuite(const char* Name)
{
	Result = ZE_TR_NOT_RUN;
	strncpy(this->Name, Name, 255);
}

ZETestSuiteRegister::ZETestSuiteRegister(ZETestSuite* Suite)
{
	ZETestManager::GetInstance()->RegisterTestSuite(Suite);
}
