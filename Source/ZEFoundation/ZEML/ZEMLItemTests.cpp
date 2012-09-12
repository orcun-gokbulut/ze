//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLItemTests.cpp
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

#include "ZEMLItem.h"
#include "ZETest/ZETest.h"
#include "ZETest/ZETestCheck.h"
#include "ZEError.h"
#include "ZEMLNode.h"



class ZEMLTestItem : public ZEMLItem
{
	public:

		virtual void		ReadFromFile(ZEFile* File, bool DeferredDataReading)
		{
			
		}

		virtual void		WriteToFile(ZEFile* File)
		{
		
		}

		virtual ZEUInt64	GetTotalSize()
		{
			return GetDataSize();
		}

		ZEMLTestItem()
		{
			Type = (ZEUInt8)ZEML_IT_UNDEFINED;
			DataSize = 0;
			Parent = NULL;
		}

		void SetTypeTest(ZEMLItemType Type)
		{
			ZEMLItem::SetType(Type);
		}
};

ZETestSuite(ZEMLItem)
{
	ZETest("void ZEMLItem::SetType(ZEMLItemType Type)")
	{
		ZEMLTestItem Item;

		ZEMLItemType Type = Item.GetType();
		ZETestCheckEqual(Type, ZEML_IT_UNDEFINED);

		Item.SetTypeTest(ZEML_IT_BOOLEAN);
		Type = Item.GetType();
		ZETestCheckEqual(Type, ZEML_IT_BOOLEAN);
	}

	ZETest("ZEMLItemType ZEMItem::GetType() const")
	{
		ZEMLTestItem Item;

		ZEMLItemType Type = Item.GetType();
		ZETestCheckEqual(Type, ZEML_IT_UNDEFINED);

		ZEMLNode Node;
		ZEInt8 Value = 'z';
		Node.AddProperty("Prop", Value);
		ZEMLItem* MLItem = (ZEMLItem*)Node.GetProperties()[0];

		Type = MLItem->GetType();
		ZETestCheckEqual(Type, ZEML_IT_INT8);
	}

	ZETest("void ZEMLItem::SetName(const ZEString& Name)")
	{
		ZEMLTestItem Item;

		Item.SetName("MLItem");
		ZETestCheckString(Item.GetName(), "MLItem");

		ZEMLNode Node;
		Node.AddProperty();
		ZEMLItem* MLItem = (ZEMLItem*)Node.GetProperties()[0];

		MLItem->SetName("MLItem");
		ZETestCheckString(MLItem->GetName(), "MLItem");
	}

	ZETest("const ZEString& ZEMLItem::GetName() const")
	{
		ZEMLTestItem Item;

		Item.SetName("MLTestItem");

		const ZEString ItemName = Item.GetName();
		ZETestCheckString(ItemName, "MLTestItem");

		ZEMLNode Node;
		Node.AddProperty();
		ZEMLItem* MLItem = (ZEMLItem*)Node.GetProperties()[0];
		MLItem->SetName("MLItem");

		const ZEString Name = MLItem->GetName();
		ZETestCheckString(Name, "MLItem");
	}

	ZETest("ZEUInt64 ZEMLItem::GetDataSize()")
	{
		ZEMLTestItem Item;
		Item.SetTypeTest(ZEML_IT_BOOLEAN);

		ZEUInt64 DataSize = Item.GetDataSize();
		ZETestCheckEqual(DataSize, 0);

		ZEMLNode Node;
		Node.AddProperty("Prop", "Test");
		ZEMLItem* MLItem = (ZEMLItem*)Node.GetProperties()[0];

		DataSize = MLItem->GetDataSize();
		ZETestCheckEqual(DataSize, 5);
	}
}

