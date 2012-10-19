//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLSerialReader.h
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
#ifndef	__ZEML_SERIAL_READER_H__
#define __ZEML_SERIAL_READER_H__

#include "ZETypes.h"
#include "ZEDS\ZEVariant.h"
#include "ZEDS\ZEString.h"
#include "ZEMLItem.h"
#include "ZEDS\ZEHashGenerator.h"

typedef ZEInt64 ZEMLSerialPointer;

struct ZEMLSerialListItem
{
	bool					IsFound;
	const char*				Name;
	ZEUInt64				Hash;
	ZEVariant*				Value;
	ZEVariantType			VariantType;
	bool					Mandatory;

	ZEMLSerialPointer*		Pointer;
};

#define ZEML_LIST_PROPERTY(Name, Value, ValueType, Mandatory)	{false, Name, ZEHashGenerator::Hash(Name), &Value, ValueType, Mandatory, NULL}
#define ZEML_LIST_NODE(Name, Pointer, Mandatory)	{false, Name, ZEHashGenerator::Hash(Name), NULL, ZE_VRT_NULL, Mandatory, &Pointer}
#define ZEML_LIST_DATA ZEML_LIST_NODE

class ZEFile;

class ZEMLSerialReader
{
	private:

		ZEFile*				File;
		ZEMLSerialPointer	DataItemDataPointer;
		ZEMLSerialPointer	MaxPointer;

		ZEMLItemType		CurrentItemType;
		ZEVariant			CurrentItemValue;
		ZEString			CurrentItemName;
		ZEUInt64			CurrentItemDataSize;
		ZEUInt64			CurrentItemSubItemCount;

	public:

		bool				Read();
		bool				SkipNodeAndRead();

		void				SeekPointer(ZEMLSerialPointer Pointer);
		ZEMLSerialPointer	GetCurrentPointer();
		void				GoToCurrentPointer();
		
		ZEMLItemType		GetItemType();
		ZEString			GetItemName();
		ZEUInt64			GetSubItemCount();
		ZEVariant			GetItemValue();
		ZEUInt64			GetDataSize();
		bool				GetData(void* Buffer, ZEUInt64 BufferSize, ZEUInt64 Offset = 0);

		bool				ReadPropertyList(ZEMLSerialListItem* List, ZESize ItemCount);

							ZEMLSerialReader(ZEFile* File);
};

#endif
