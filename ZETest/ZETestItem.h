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
#ifndef __ZE_TEST_ITEM_H__
#define __ZE_TEST_ITEM_H__

#include "ZETypes.h"

class ZETestSuite;

enum ZETestResult
{
	ZE_TR_NOT_RUN,
	ZE_TR_PASSED,
	ZE_TR_FAILED
};

enum ZETestProblemType
{
	ZE_TPT_ERROR,
	ZE_TPT_WARNING,
	ZE_TPT_NOTICE
};

class ZETestItem
{
	private:
		char					Name[256];
		ZETestSuite*			Owner;
		ZETestResult			Result;
		float					ElapsedTime;

	public:
		const char*				GetName();
		ZETestSuite*			GetOwner();

		void					ReportProblem(ZETestProblemType Type, const char* Problem, const char* File, int Line);

		virtual void			TestImpl() = 0;

		bool					RunTest();
		void					Reset();
		ZETestResult			GetResult();
		float					GetEleapsedTime();

								ZETestItem(const char* Name, ZETestSuite* Owner);
};

#define ZETestItemAdd(Name)\
	class ZETest_##Name : public ZETestItem\
	{\
		public:\
			virtual void TestImpl();\
			ZETest_##Name(const char* Name1, ZETestSuite* Owner1) : ZETestItem(Name1, Owner1) {}\
	} Test_##Name(#Name, &Suite);\
	void ZETest_##Name::TestImpl()

#endif
