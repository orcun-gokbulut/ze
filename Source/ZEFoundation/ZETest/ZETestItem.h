//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETestItem.h
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

#pragma once
#include "ZETypes.h"
#include "ZETimeCounter.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEString.h"
#include "ZEExport.ZEFoundation.h"

#define ZE_TEST_ITEM_INTERNAL(Name, Identifier) \
	class Identifier : public ZETestItem \
	{ \
		public: \
			virtual void Implementation() override; \
			Identifier(); \
	}; \
	static Identifier ZE_MACRO_CONCAT(Identifier, Instance); \
	Identifier::Identifier() \
	{ \
		SetName(Name); \
		TestInstance.AddItem(this); \
	} \
	void Identifier::Implementation()
#define ZE_TEST_ITEM(Name) ZE_TEST_ITEM_INTERNAL(Name, ZE_MACRO_CONCAT(ZETestItem, __COUNTER__))

#define ZE_TEST_CASE(Name)

class ZETestSuiteItem;

enum ZETestResult
{
	ZE_TR_NOT_RUN,
	ZE_TR_PASSED,
	ZE_TR_FAILED
};

enum ZETestProblemType
{
	ZE_TPT_CRITICAL_ERROR,
	ZE_TPT_ERROR,
	ZE_TPT_WARNING,
};

class ZETestItem;

class ZETestProblem
{
	public:
		ZETestItem*						Item;
		ZETestProblemType				Type;
		ZEString						Description;
		ZEString						FileName;
		ZEUInt							Line;
};


class ZE_EXPORT_ZEFOUNDATION ZETestItem
{
	friend class ZETest;
	private:
		ZETest*							Test;
		ZEString						Name;
		ZETestResult					Result;
		ZETimeCounter					TimeCounter;
		ZEArray<ZETestProblem>			Problems;
		bool							ExpectingError;
		ZEErrorType						ExpectingErrorType;
		ZEString						ExpectingErrorModuleName;
		bool							ExpectedErrorOccured;

		bool							SuppressingLogging;
		ZELogType						SuppressingLoggingPreviousLevel;
		

		ZEErrorCallback					ErrorCallbackChain;
		void							ErrorCallback(ZEErrorType Type);

	protected:
		void							ReportProblem(ZETestProblemType Type, const ZEString& Description, const char* File, ZEInt Line);
		
		void							ResumeTimeCounter();
		void							PauseTimeCounter();

		void							StartSuppressingLogging();
		void							StopSuppressingLogging();

		void							StartExpectingError(ZEErrorType Type, const char* ModuleName);
		bool							DidExpectedErrorOccured();
		bool							StopExpectingError();

		virtual void					Implementation() = 0;

	public:
		ZETest*							GetTest() const;

		void							SetName(const ZEString& Name);
		const ZEString&					GetName() const;

		ZETestResult					GetResult() const;
		double							GetElapsedTime() const;
		const ZEArray<ZETestProblem>&	GetProblems() const;

		void							Run();
		void							Reset();

										ZETestItem();
		virtual							~ZETestItem();
};
