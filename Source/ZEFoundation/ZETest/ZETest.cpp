//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETest.cpp
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

#include "ZETest.h"

#include "ZEError.h"
#include "ZETerminalUtils.h"
#include "ZETestManager.h"

void ZETest::SetName(const ZEString& Name)
{
	this->Name = Name;
}
const ZEString& ZETest::GetName() const
{
	return Name;
}

const ZEArray<ZETestItem*>& ZETest::GetItems() const
{
	return Items;
}

ZETestResult ZETest::GetResult() const
{
	for (ZESize I = 0; I < Items.GetCount(); I++)
	{
		if (Items[I]->GetResult() == ZE_TR_FAILED)
			return ZE_TR_FAILED;
		else if (Items[I]->GetResult() == ZE_TR_NOT_RUN)
			return ZE_TR_NOT_RUN;
	}

	return ZE_TR_PASSED;
}

ZESize ZETest::GetPassedItemCount() const
{
	ZESize Count = 0;
	for (ZESize I = 0; I < Items.GetCount(); I++)
	{
		if (Items[I]->GetResult() == ZE_TR_PASSED)
			Count++;
	}
	return Count;
}

ZESize ZETest::GetFailedItemCount() const
{
	ZESize Count = 0;
	for (ZESize I = 0; I < Items.GetCount(); I++)
	{
		if (Items[I]->GetResult() == ZE_TR_FAILED)
			Count++;
	}
	return Count;
}

double ZETest::GetElapsedTime() const
{
	double ElapsedTime = 0;
	for (ZESize I = 0; I < Items.GetCount(); I++)
		ElapsedTime += Items[I]->GetElapsedTime();
	return ElapsedTime;
}

ZEArray<ZETestProblem> ZETest::GetProblems() const
{
	ZEArray<ZETestProblem> Problems;
	for (ZESize I = 0; I < Items.GetCount(); I++)
		Problems.AddMultiple(Items[I]->GetProblems());

	return Problems;
}

void ZETest::AddItem(ZETestItem* Item)
{
	zeCheckError(Item == NULL, ZE_VOID, "Cannot add test item. Item is NULL.");
	zeCheckError(Item->Test != NULL, ZE_VOID, "Cannot add test item. Item is already registered to a test.");
	
	Item->Test = this;
	Items.Add(Item);
}

void ZETest::RemoveItem(ZETestItem* Item)
{
	zeCheckError(Item == NULL, ZE_VOID, "Cannot remove test item. Item is NULL.");
	zeCheckError(Item->Test != this, ZE_VOID, "Cannot remove test item. Item is not registered to this test.");

	Item->Test = NULL;
	Items.RemoveValue(Item);
}

void ZETest::Run()
{
	Reset();

	bool Verbose = ZETestManager::GetInstance()->GetNormalOutputEnabled();
	if (!Verbose)
	{
		for (ZESize I = 0; I < Items.GetCount(); I++)
			Items[I]->Run();
	}
	else
	{
		ZETerminalUtils::Reset();
		printf(
			"Running Test %s.\n"
			"--------------------------------------------------------------------\n", 
			Name.ToCString());

		for (ZESize I = 0; I < Items.GetCount(); I++)
		{
			ZETestItem* TestItem = Items[I];

			ZETerminalUtils::Reset();
			printf("Test ");
			ZETerminalUtils::SetBold(true);
			printf("#%u/%u", I + 1, Items.GetCount());
			ZETerminalUtils::Reset();
			printf(" - %s - ", Items[I]->GetName().ToCString());

			TestItem->Run();

			ZETerminalUtils::Reset();
			ZETerminalUtils::SetBold(true);
			if (TestItem->GetResult() == ZE_TR_PASSED)
			{
				ZETerminalUtils::SetForgroundColor(ZE_TC_GREEN);
				printf("PASSED");
			}
			else
			{
				ZETerminalUtils::SetForgroundColor(ZE_TC_RED);
				printf("FAILED");
			}

			ZETerminalUtils::Reset();
			printf(" (%f ms)\n", TestItem->GetElapsedTime());


			for (ZESize I = 0; I < TestItem->GetProblems().GetCount(); I++)
				ZETestManager::GetInstance()->ReportProblem(TestItem->GetProblems()[I]);
		}

		ZETerminalUtils::Reset();
		printf("\nTest suite \"%s\" has ", GetName().ToCString());

		ZETerminalUtils::SetBold(true);
		if (GetResult() == ZE_TR_PASSED)
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
			" Total Test Item Count: %u\n"
			" Passed Test Item Count: %u\n"
			" Failed Test Item Count: %u\n"
			" Elapsed Time: %f ms\n",
			Items.GetCount(),
			GetPassedItemCount(),
			GetFailedItemCount(),
			GetElapsedTime());
	}
}

void ZETest::Reset()
{
	for (ZESize I = 0; I < Items.GetCount(); I++)
		Items[I]->Reset();
}

ZETest::ZETest()
{

}

ZETest::~ZETest()
{

}
