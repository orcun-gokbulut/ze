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

#include "ZEEndian.h"
#include "ZEDS\ZEFormat.h"
#include "ZEFile\ZEFile.h"
#include "ZEPacking.h"

#include <memory.h>
#include <stdint.h>
#include "ZEFile\ZEFileInfo.h"
#include "ZEML\ZEMLWriter.h"
#include "ZEML\ZEMLReader.h"
#include "ZEMath\ZEMath.h"

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

ZEString ZETEBlockDatabase::GetBlockFilePath(ZETEBlock* Block)
{
	return ZEFormat::Format("{0}/L{1}-Y{2}-X{3}.zeBlock", Path, Block->GetLevel(), Block->GetIndexY(), Block->GetIndexX());
}

void ZETEBlockDatabase::SetName(const ZEString& Name)
{
	this->Name = Name;
}

const ZEString& ZETEBlockDatabase::GetName()
{
	return Name;
}

ZETEBlockRegister* ZETEBlockDatabase::GetRegister(ZEUInt64 PositionX, ZEUInt64 PositionY, ZEInt Level)
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

const ZEString& ZETEBlockDatabase::GetPath()
{
	return Path;
}

void ZETEBlockDatabase::SetPixelType(ZETEPixelType Type)
{
	PixelType = Type;
}

ZETEPixelType ZETEBlockDatabase::GetPixelType()
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

ZEInt ZETEBlockDatabase::GetMinLevel()
{
	return MinLevel;
}

ZEInt ZETEBlockDatabase::GetMaxLevel()
{
	return MaxLevel;
}

ZEInt64 ZETEBlockDatabase::GetMinIndexX()
{
	return MinIndexX;
}

ZEInt64 ZETEBlockDatabase::GetMinIndexY()
{
	return MinIndexY;
}

ZEInt64 ZETEBlockDatabase::GetMaxIndexX()
{
	return MaxIndexX;
}

ZEInt64 ZETEBlockDatabase::GetMaxIndexY()
{
	return MaxIndexY;
}

double ZETEBlockDatabase::GetMinPositionX()
{
	return GetMinIndexX() * ZEMath::Power(2, MinLevel) * BlockSize;
}

double ZETEBlockDatabase::GetMinPositionY()
{
	return GetMinIndexY() * ZEMath::Power(2, MinLevel) * BlockSize;
}

double ZETEBlockDatabase::GetMaxPositionX()
{
	return (GetMaxIndexX() + 1) * ZEMath::Power(2, MinLevel) * BlockSize;
}

double ZETEBlockDatabase::GetMaxPositionY()
{
	return (GetMaxIndexY() + 1) * ZEMath::Power(2, MinLevel) * BlockSize;
}

void ZETEBlockDatabase::SetMinLevelLimit(ZEInt Level)
{
	MinLevelLimit = Level;
}

ZEInt ZETEBlockDatabase::GetMinLevelLimit()
{
	return MinLevelLimit;
}

void ZETEBlockDatabase::SetMaxLevelLimit(ZEInt Level)
{
	MaxLevelLimit = Level;
}

ZEInt ZETEBlockDatabase::GetMaxLevelLimit()
{
	return MaxLevelLimit;
}

bool ZETEBlockDatabase::CheckBlock(ZETEBlock* Block)
{
	ZEFileInfo FileInfo(GetBlockFilePath(Block));
	return FileInfo.IsExists();
}

bool ZETEBlockDatabase::LoadBlock(ZETEBlock* Block)
{
	ZETEBlockRegister* Register = GetRegister(Block->GetPositionX(), Block->GetPositionY(), Block->GetLevel());
	if (Register == NULL)
	{
		Block->Clean();
		return false;
	}
	else
	{
		if (Block->Load(GetBlockFilePath(Block)))
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
	if (Block->GetLevel() > MaxLevelLimit || Block->GetLevel() < MinLevelLimit)
		return false;

	ZEString BlockPath = GetBlockFilePath(Block);

	if (!Block->Save(BlockPath))
		return false;

	static ZELock Lock;
	Lock.Lock();
	{
		ZETEBlockRegister* Register = GetRegister(Block->GetPositionX(), Block->GetPositionY(), Block->GetLevel());
		if (Register == NULL)
		{
			ZETEBlockRegister NewRegister;
			NewRegister.PositionX = Block->GetPositionX();
			NewRegister.PositionY = Block->GetPositionY();
			NewRegister.Level = Block->GetLevel();

			if (MinLevel == INT_MAX)
				MinLevel = Block->GetLevel();

			ZEInt LevelDifference = Block->GetLevel() - MinLevel;

			if (MaxLevel < Block->GetLevel())
				MaxLevel = Block->GetLevel();

			if (MinLevel > Block->GetLevel())
			{
				ZEInt LevelScale = ZEMath::Power(2, LevelDifference);
				MinIndexX *= LevelScale;
				MinIndexY *= LevelScale;
				MaxIndexX *= LevelScale;
				MaxIndexY *= LevelScale;

				LevelDifference = 0;

				MinLevel = Block->GetLevel();
			}

			ZEInt LevelScale = ZEMath::Power(2, LevelDifference);		

			if (MinIndexX > Block->GetIndexX() * LevelScale)
				MinIndexX = Block->GetIndexX() * LevelScale;

			if (MinIndexY > Block->GetIndexY() * LevelScale)
				MinIndexY = Block->GetIndexY() * LevelScale;

			if (MaxIndexX < Block->GetIndexX() * LevelScale)
				MaxIndexX = Block->GetIndexX() * LevelScale;

			if (MaxIndexY < Block->GetIndexY() * LevelScale)
				MaxIndexY = Block->GetIndexY() * LevelScale;

			Registers.Add(NewRegister);
		}
	}
	Lock.Unlock();

	return true;
}

bool ZETEBlockDatabase::RemoveBlock(ZETEBlock* Block)
{
	ZEFileInfo FileInfo(GetBlockFilePath(Block));
	return FileInfo.Delete();
}

bool ZETEBlockDatabase::SaveDatabase()
{
	ZEString HeaderFile = ZEFormat::Format("{0}/Header.zeLayer", Path);

	ZEMLWriter Writer;
	if (!Writer.Open(HeaderFile))
		return false;

	ZEMLWriterNode Node;
	Writer.OpenRootNode("ZETRLayer", Node);
		Node.WriteString("Name", GetName());
		Node.WriteUInt32("BlockSize", (ZEUInt32)GetBlockSize());
		Node.WriteUInt8("Type",	(ZEUInt8)this->GetPixelType());
		Node.WriteInt64("MinIndexX", GetMinIndexX());
		Node.WriteInt64("MaxIndexX", GetMinIndexY());
		Node.WriteInt64("MinIndexY", GetMaxIndexX());
		Node.WriteInt64("MaxIndexY", GetMaxIndexY());
		Node.WriteInt32("MinLevel", GetMinLevel());
		Node.WriteInt32("MaxLevel", GetMaxLevel());
	Node.CloseNode();

	Writer.Close();

	return true;
}

bool ZETEBlockDatabase::LoadDatabase()
{
	ZEString HeaderFile = ZEFormat::Format("{0}/Header.zeLayer", Path);

	ZEMLReader Reader;
	if (!Reader.Open(HeaderFile))
		return false;

	ZEMLReaderNode Node = Reader.GetRootNode();

	if (Node.GetName() != "ZETELayer")
		return false;

	Name = Node.ReadString("Name", "");
	BlockSize = Node.ReadUInt32("BlockSize", 0);
	PixelType = (ZETEPixelType)Node.ReadUInt8("Type", ZETE_PT_NONE);
	MinIndexX = Node.ReadInt64("MinIndexX", 0);
	MinIndexY = Node.ReadInt64("MaxIndexX", 0);
	MaxIndexX = Node.ReadInt64("MinIndexY", 0);
	MaxIndexY = Node.ReadInt64("MaxIndexY", 0);
	MinLevel = Node.ReadInt32("MinLevel", 0);
	MaxLevel = Node.ReadInt32("MaxLevel", 0);

	Reader.Close();

	return true;
}

ZETEBlockDatabase::ZETEBlockDatabase()
{
	BlockSize = 512;
	PixelType = ZETE_PT_NONE;
	MinIndexX = INT64_MAX;
	MinIndexY = INT64_MAX;
	MaxIndexX = INT64_MIN;
	MaxIndexY = INT64_MIN;
	MinLevel = INT_MAX;
	MaxLevel = INT_MIN;
	MinLevelLimit = 0;
	MaxLevelLimit = 22;
}
