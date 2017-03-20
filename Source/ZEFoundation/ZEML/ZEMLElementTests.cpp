//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLElementTests.cpp
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

#include "ZEMLElement.h"
#include "ZETest/ZETest.h"
#include "ZETest/ZETestCheck.h"
#include "ZEError.h"
#include "ZEMLNode.h"
#include "ZEFile/ZEFile.h"


ZE_TEST(ZEMLElement)
{
	ZE_TEST_ITEM("void ZEMLItem::SetType(ZEMLItemType Type)")
	{

	}

	ZE_TEST_ITEM("ZEMLItemType ZEMItem::GetType() const")
	{
		ZEMLNode Node;
		ZEInt8 Value = 'z';
		Node.AddProperty("Prop", Value);
		ZEMLItem* MLItem = (ZEMLItem*)Node.GetProperties()[0];

		ZEMLItemType Type = MLItem->GetType();
		ZE_TEST_CHECK_EQUAL(Type, ZEML_IT_INT8);
	}

	ZE_TEST_ITEM("void ZEMLItem::SetName(const ZEString& Name)")
	{
		ZEMLNode Node;
		Node.AddProperty();
		ZEMLItem* MLItem = (ZEMLItem*)Node.GetProperties()[0];

		MLItem->SetName("MLItem");
		ZE_TEST_CHECK_STRING_EQUAL(MLItem->GetName(), "MLItem");
	}

	ZE_TEST_ITEM("const ZEString& ZEMLItem::GetName() const")
	{
		ZEMLNode Node;
		Node.AddProperty();
		ZEMLItem* MLItem = (ZEMLItem*)Node.GetProperties()[0];
		MLItem->SetName("MLItem");

		const ZEString Name = MLItem->GetName();
		ZE_TEST_CHECK_STRING_EQUAL(Name, "MLItem");
	}

	ZE_TEST_ITEM("ZEUInt64 ZEMLItem::GetDataSize()")
	{
		ZEMLNode Node;
		Node.AddProperty("Prop", "Test");
		ZEMLItem* MLItem = (ZEMLItem*)Node.GetProperties()[0];

		ZEUInt64 DataSize = MLItem->GetDataSize();
		ZE_TEST_CHECK_EQUAL(DataSize, 5);
	}

	ZE_TEST_ITEM("ZEUInt64 ZEMLItem::GetFilePosition()")
	{
		ZEMLNode Node("Node");
		Node.AddProperty("Prop", "Test");
		ZEMLItem* MLItem1 = Node.GetProperties()[0];
		ZEUInt64 FilePosition = MLItem1->GetFilePosition();
		ZE_TEST_CHECK_EQUAL(FilePosition, 0);

		void* Data = new char[sizeof(unsigned char)];
		Node.AddDataProperty("DataProp", Data, sizeof(unsigned char), false);
		ZEMLItem* MLItem2 = Node.GetProperties()[1];
		FilePosition = MLItem2->GetFilePosition();
		ZE_TEST_CHECK_EQUAL(FilePosition, 0);

		ZEFile* File = new ZEFile();
		File->Open("MLItemTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		bool Written = Node.Write(File);
		ZE_TEST_CHECK_ERROR(Written);

		MLItem1 = Node.GetProperties()[0];
		ZE_TEST_CHECK_EQUAL(MLItem1->GetFilePosition(), 0);
		MLItem2 = Node.GetProperties()[1];
		ZE_TEST_CHECK_EQUAL(MLItem2->GetFilePosition(), 0);

		File->Flush();
		File->Seek(-File->GetSize() * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
		bool Read = Node.Read(File, false);
		ZE_TEST_CHECK_ERROR(Read);

		MLItem1 = Node.GetProperties()[0];
		ZE_TEST_CHECK_EQUAL(MLItem1->GetFilePosition(), 0);
		MLItem2 = Node.GetProperties()[1];
		ZE_TEST_CHECK_EQUAL(MLItem2->GetFilePosition(), 0);
		ZEMLItem* MLItem3 = Node.GetProperties()[2];
		ZE_TEST_CHECK_EQUAL(MLItem3->GetFilePosition(), 24);
		ZEMLItem* MLItem4 = Node.GetProperties()[3];
		ZE_TEST_CHECK_EQUAL(MLItem4->GetFilePosition(), 45);

		File->Close();
		remove("MLItemTests.txt");
		delete Data;
	}
}

