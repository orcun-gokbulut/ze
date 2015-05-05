//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETEBlockDatabase.cpp
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

#include "ZETEBlockDatabase.h"

#include "ZETEBlock.h"

#include <memory.h>
#include "ZEDS\ZEFormat.h"
#include "ZEEndian.h"
#include "ZEFile\ZEFile.h"
#include "ZEPacking.h"

ZEPackStruct
(
	struct ZETEBlockRegisterData
	{
		ZELittleEndian<ZEUInt64> PositionX;
		ZELittleEndian<ZEUInt64> PositionY;
		ZELittleEndian<ZEUInt> Level;
		ZELittleEndian<ZEUInt64> SubBlocks[4];
	}
);

ZETEBlockRegister::ZETEBlockRegister()
{
	PositionX = 0;
	PositionY = 0;
	Level = 0;
	SubBlocks[0] = NULL;
	SubBlocks[2] = NULL;
	SubBlocks[3] = NULL;
	SubBlocks[4] = NULL;
	Cache = NULL;
}

ZETEBlockRegister::~ZETEBlockRegister()
{
	if (Cache != NULL)
	{
		delete Cache;
		Cache = NULL;
	}
}

ZETEBlockRegister* ZETEBlockDatabase::GetRegister(ZEUInt64 PositionX, ZEUInt64 PositionY, ZEUInt Level)
{
	for (ZESize I = 0; I < Registers.GetCount(); I++)
	{
		if (Registers[I].Level == Level && 
			Registers[I].PositionX == PositionX && 
			Registers[I].PositionY == PositionY)
		{
			return &Registers[I];
		}
	}
	return NULL;
}

void ZETEBlockDatabase::SetPath(const ZEString& Path)
{
	this->Path = Path;
}

const ZEString& ZETEBlockDatabase::GetPath(const ZEString& Path)
{
	return Path;
}

void ZETEBlockDatabase::SetBlockType(ZETEPixelType Type)
{
	PixelType = Type;
}

ZETEPixelType ZETEBlockDatabase::GetBlockType()
{
	return PixelType;
}

void ZETEBlockDatabase::SetBlockSize(ZESize BlockSize)
{
	this->BlockSize = BlockSize;
}

ZESize ZETEBlockDatabase::GetBlockSize()
{
	return BlockSize;
}

bool ZETEBlockDatabase::LoadBlock(ZETEBlock* Block)
{
	ZETEBlockRegister* Register = GetRegister(Block->GetPositionX(), Block->GetPositionY(), Block->GetLevel());
	if (Register != NULL)
	{
		Block->Clean();
		return false;
	}
	else
	{
		ZEString BlockPath = ZEFormat::Format("{0}/Y{1}-X{2}-Z{3}.zeBlock", Path, Block->GetPositionY(), Block->GetPositionX(), Block->GetLevel());
		if (Block->Load(BlockPath))
		{
			return true;
		}
		else
		{
			Block->Clean();
			return false;
		}
	}
}

bool ZETEBlockDatabase::StoreBlock(ZETEBlock* Block)
{
	ZEString BlockPath = ZEFormat::Format("{0}/Y{1}-X{2}-Z{3}.zeBlock", Path, Block->GetPositionY(), Block->GetPositionX(), Block->GetLevel());
	if (!Block->Save(BlockPath))
		return false;

	static ZELock Lock;
	Lock.Lock();
	ZETEBlockRegister* Register = GetRegister(Block->GetPositionX(), Block->GetPositionY(), Block->GetLevel());
	if (Register == NULL)
	{
		ZETEBlockRegister NewRegister;
		NewRegister.PositionX = Block->GetPositionX();
		NewRegister.PositionY = Block->GetPositionY();
		NewRegister.Level = Block->GetLevel();

		Registers.Add(NewRegister);
	}
	Lock.Unlock();

	return true;
}

bool ZETEBlockDatabase::SaveDatabase()
{
	ZEFile File;
	if (!File.Open(ZEFormat::Format("{0}/Index.zeBlockDatabase", Path), ZE_FOM_WRITE, ZE_FCM_OVERWRITE))
		return false;
	
	ZELittleEndian<ZEUInt64> ElementCount = Registers.GetCount();
	File.Write(&ElementCount, sizeof(ZEUInt64), 1);
	for (ZESize I = 0; I < Registers.GetCount(); I++)
	{
		ZETEBlockRegisterData Data;
		Data.PositionX = Registers[I].PositionX;
		Data.PositionY = Registers[I].PositionY;
		Data.Level = Registers[I].Level;
		
		ZESize* SubBlocks = Registers[I].SubBlocks;
		Data.SubBlocks[0] = SubBlocks[0];
		Data.SubBlocks[1] = SubBlocks[1];
		Data.SubBlocks[2] = SubBlocks[2];
		Data.SubBlocks[3] = SubBlocks[3];

		if (!File.Write(&Data, sizeof(ZETEBlockRegisterData), 1) == 1)
			return false;
	}

	File.Close();

	return true;
}

bool ZETEBlockDatabase::LoadDatabase()
{
	ZEFile File;
	if (!File.Open(ZEFormat::Format("{0}/Index.zeBlockDatabase", Path), ZE_FOM_READ, ZE_FCM_NONE))
		return false;

	ZELittleEndian<ZEUInt64> ElementCount = 0;
	if (File.Read(&ElementCount, sizeof(ZEUInt64), 1) != 1)
		return false;

	Registers.SetCount(ElementCount);

	for (ZESize I = 0; I < Registers.GetCount(); I++)
	{
		ZETEBlockRegisterData Data;
		if (File.Read(&Data, sizeof(ZETEBlockRegisterData), 1) != 1)
			return false;

		Registers[I].PositionX = Data.PositionX;
		Registers[I].PositionY = Data.PositionY;
		Registers[I].Level = Data.Level;

		ZESize* SubBlocks = Registers[I].SubBlocks;
		SubBlocks[0] = Data.SubBlocks[0];
		SubBlocks[1] = Data.SubBlocks[1];
		SubBlocks[2] = Data.SubBlocks[2];
		SubBlocks[3] = Data.SubBlocks[3];
	}

	File.Close();

	return true;
}

ZETEBlockDatabase::ZETEBlockDatabase()
{
	BlockSize = 512;
	PixelType = ZETE_PT_NONE;
}
