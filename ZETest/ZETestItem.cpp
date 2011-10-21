//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETestItem.cpp
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

#include "ZETestItem.h"
#include "ZETestSuite.h"
#include "ZETestManager.h"
#include "ZETestTimer.h"
#include "ZEError.h"

#include <stdio.h>
#include <string.h>

static ZETestItem* CurrentTestItem = NULL;
static void ZEErrorManagerErrorCallback(const char* Module, ZEErrorType Type, const char* ErrorText)
{
	if (CurrentTestItem != NULL)
	{
		ZETestProblemType ProblemType;

		switch(Type)
		{
			default:
			case ZE_ET_CRITICAL_ERROR:
			case ZE_ET_ERROR:
				ProblemType = ZE_TPT_ERROR;
				break;

			case ZE_ET_WARNING:
				ProblemType = ZE_TPT_WARNING;
				break;

			case ZE_ET_NOTICE:
			case ZE_ET_LOG:
				ProblemType = ZE_TPT_NOTICE;
				break;
		}

		char Buffer[4096];
		sprintf(Buffer, "ZEError::RaiseError redirected. Error type : %s. Error text : \"%s\".", ZEError::GetErrorTypeString(Type), ErrorText);

		CurrentTestItem->ReportProblem(ProblemType, Buffer, CurrentTestItem->GetName(), 1);
	}
}

static void ZEErrorManagerAssertCallback(ZEAssertType Type, const char* AssertText, const char* Function, const char* File, int Line)
{
	if (CurrentTestItem != NULL)
	{
		ZETestProblemType ProblemType;

		if (Type == ZE_AT_WARNING_ASSERT)
			ProblemType = ZE_TPT_WARNING;
		else
			ProblemType = ZE_TPT_ERROR;

		char Buffer[4096];
		sprintf(Buffer, "ZEError::RaiseAssert redirected. Assert Type : \"%s\", Assert text : \"%s\"", ZEError::GetAssertTypeString(Type), AssertText);

		CurrentTestItem->ReportProblem(ProblemType, Buffer, File, Line);
	}
}

void ZETestItem::ReportProblem(ZETestProblemType Type, const char* Problem, const char* File, int Line)
{
	if (Type == ZE_TPT_ERROR)
		Result = ZE_TR_FAILED;
	ZETestManager::GetInstance()->ReportProblem(Owner, this, Type, Problem, File, Line);
}

const char* ZETestItem::GetName()
{
	return Name;
}

ZETestSuite* ZETestItem::GetOwner()
{
	return Owner;
}

ZETestResult ZETestItem::GetResult()
{
	return Result;
}

float ZETestItem::GetEleapsedTime()
{
	return ElapsedTime;
}

void ZETestItem::Reset()
{
	Result = ZE_TR_NOT_RUN;
	ElapsedTime = 0;
}

bool ZETestItem::RunTest()
{
	#ifdef __COVERAGESCANNER__ 
	char Buffer[1024];
	sprintf(Buffer, "%s::%s", Owner->GetName(), GetName());
	__coveragescanner_clear();
	__coveragescanner_testname(Buffer);
	#endif

	ZETestTimer Timer;
	try
	{
		Timer.Start();
		TestImpl();
		Timer.Stop();

		ElapsedTime = Timer.GetElapsedTime();

		if (Result == ZE_TR_NOT_RUN)
			Result = ZE_TR_PASSED;
		
		#ifdef __COVERAGESCANNER__  
		__coveragescanner_teststate(Result == ZE_TR_PASSED ? "PASSED" : "FAILED");
		__coveragescanner_save();
		__coveragescanner_testname("");
		#endif

		return (Result == ZE_TR_PASSED ? true : false);
	}
	catch (...)
	{
		Timer.Stop();
		ElapsedTime = Timer.GetElapsedTime();

		Result = ZE_TR_FAILED;
		return false;
	}
}

ZETestItem::ZETestItem(const char* Name, ZETestSuite* Owner)
{
	strncpy(this->Name, Name, 255);
	this->Owner = Owner;
	ElapsedTime = 0;
	Owner->RegisterTest(this);
	this->Result = ZE_TR_NOT_RUN;
}
