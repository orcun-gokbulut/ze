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

#include "ZETestManager.h"
#include "ZELog.h"
#include "ZELogSession.h"

void ZETestItem::ErrorCallback(ZEErrorType Type)
{
	if (ExpectingError && ExpectingErrorType == Type)
	{
		ExpectedErrorOccured = true;
		return;
	}
	else
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
		}

		char Buffer[4096];
		sprintf(Buffer, " ZEError event has been caught. Error type : %s.", ZEError::GetErrorTypeString(Type));

		ReportProblem(ProblemType, Buffer, NULL, 0);

		ErrorCallbackChain.CheckAndCall(Type);
	}
}

void ZETestItem::SetName(const ZEString& Name)
{
	this->Name = Name;
}

void ZETestItem::ReportProblem(ZETestProblemType Type, const ZEString& Description, const char* FileName, ZEInt Line)
{
	if (Type == ZE_TPT_CRITICAL_ERROR || Type == ZE_TPT_ERROR)
		Result = ZE_TR_FAILED;

	ZETestProblem* Problem = Problems.Add();
	Problem->Item = this;
	Problem->Type = Type;
	Problem->Description = Description;
	Problem->FileName = FileName;
	Problem->Line = Line;
}

void ZETestItem::ResumeTimeCounter()
{
	TimeCounter.Start();
}

void ZETestItem::PauseTimeCounter()
{
	TimeCounter.Pause();
}

void ZETestItem::StartSuppressingLogging()
{
	SuppressingLogging = true;
	SuppressingLoggingPreviousLevel = ZELog::GetInstance()->GetCurrentSession()->GetMinimumLevel();
	ZELog::GetInstance()->GetCurrentSession()->SetMinimumLevel(ZE_LOG_CRITICAL_ERROR);
}

void ZETestItem::StopSuppressingLogging()
{
	zeDebugCheck(!SuppressingLogging, "Logging is not suppressed.");
	SuppressingLogging = false;
	ZELog::GetInstance()->GetCurrentSession()->SetMinimumLevel(SuppressingLoggingPreviousLevel);
}

void ZETestItem::StartExpectingError(ZEErrorType Type, const char* ModuleName)
{
	zeDebugCheck(ExpectingError, "Another error has already being expected.");
	ExpectingError = true;
	ExpectingErrorType = Type;
	ExpectingErrorModuleName = ModuleName;
	ExpectedErrorOccured = false;
}

bool ZETestItem::DidExpectedErrorOccured()
{
	zeDebugCheck(!ExpectingError, "Error hasn't being expected.");
	return ExpectedErrorOccured;
}

bool ZETestItem::StopExpectingError()
{
	zeDebugCheck(!ExpectingError, "Error hasn't being expected.");
	bool Temp = ExpectingError;
	ExpectingError = false;
	ExpectingErrorType = ZE_ET_NONE;
	ExpectingErrorModuleName = ZEString::Empty;
	ExpectedErrorOccured = false;
	return Temp;
}

ZETest* ZETestItem::GetTest() const
{
	return Test;
}

const ZEString& ZETestItem::GetName() const
{
	return Name;
}

ZETestResult ZETestItem::GetResult() const
{
	return Result;
}

double ZETestItem::GetElapsedTime() const
{
	return TimeCounter.GetTimeMilliseconds();
}

const ZEArray<ZETestProblem>& ZETestItem::GetProblems() const
{
	return Problems;
}

void ZETestItem::Run()
{
	Reset();

	#ifdef __COVERAGESCANNER__ 
	char Buffer[1024];
	sprintf(Buffer, "%s::%s", Test->GetName(), GetName());
	__coveragescanner_clear();
	__coveragescanner_testname(Buffer);
	#endif

	ErrorCallbackChain = ZEError::GetInstance()->GetCallback();
	ZEError::GetInstance()->SetCallback(ZEErrorCallback::Create<ZETestItem, &ZETestItem::ErrorCallback>(this));
	
	TimeCounter.Start();
	try
	{
		Implementation();
	}
	catch(...)
	{
		PauseTimeCounter();
		ReportProblem(ZE_TPT_ERROR, "Unhandled exception has occurred.", "", 0);
	}

	TimeCounter.Stop();
	ZEError::GetInstance()->SetCallback(ErrorCallbackChain);

	zeDebugCheck(SuppressingLogging, "Log is still supressed. StopSuppressingLog is not called.");
	zeDebugCheck(ExpectingError, "Error is still expecting. StopExpectingError is not called.");
	#ifdef __COVERAGESCANNER__  
	__coveragescanner_teststate(Result == ZE_TR_PASSED ? "PASSED" : "FAILED");
	__coveragescanner_save();
	__coveragescanner_testname("");
	#endif

	if (Result != ZE_TR_FAILED)
		Result = ZE_TR_PASSED;
}

void ZETestItem::Reset()
{
	Problems.Clear();
	TimeCounter.Reset();
	Result = ZE_TR_NOT_RUN;
	ExpectingError = false;
	ExpectingErrorType = ZE_ET_NONE;
	ExpectingErrorModuleName = "";
	ExpectedErrorOccured = false;
	SuppressingLogging = false;
	SuppressingLoggingPreviousLevel = ZE_LOG_ERROR;
}

ZETestItem::ZETestItem()
{
	Test = NULL;
	Reset();
}

ZETestItem::~ZETestItem()
{

}
