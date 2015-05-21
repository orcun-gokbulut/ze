//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETRBlock.cpp
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

#include "ZETRBlock.h"

#include "ZEError.h"
#include "ZEPacking.h"
#include "ZEEndian.h"
#include "ZEFile\ZEFile.h"
#include "ZEMath\ZEMath.h"
#include "ZEFile\ZEFileInfo.h"

ZEPackStruct(
	struct ZETRBlockFileHeader
	{
		ZELittleEndian<ZEUInt64>	PositionX;
		ZELittleEndian<ZEUInt64>	PositionY;
		ZEInt8						Level;
		ZELittleEndian<ZEUInt32>	Size;
		ZEUInt8						Type;
	}
);

void ZETRBlock::Clean()
{
	if (Data != NULL)
	{
		delete[] Data;
		Data = NULL;
	}

	Status = ZETR_BRS_NONE;
	BlockSquence = 0;

	PositionX = 0;
	PositionY = 0;
	Level = 0;
	Size = 0;

	DataSize = 0;
	PixelType = ZETR_PT_NONE;
}

ZETRBlockStatus ZETRBlock::GetStatus()
{
	return Status;
}

ZEUInt64 ZETRBlock::GetBlockSequence()
{
	return BlockSquence;
}

ZEInt64 ZETRBlock::GetPositionX()
{
	return PositionX;
}

ZEInt64 ZETRBlock::GetPositionY()
{
	return PositionY;
}

ZEInt64 ZETRBlock::GetEndX()
{
	return PositionX + Size * (ZEInt64)ZEMath::Power(2, (float)Level);
}

ZEInt64 ZETRBlock::GetEndY()
{
	return PositionY + Size * (ZEInt64)ZEMath::Power(2, (float)Level);
}

ZEInt ZETRBlock::GetLevel()
{
	return Level;
}

ZESize ZETRBlock::GetSize()
{
	return Size;
}

ZETRPixelType ZETRBlock::GetPixelType()
{
	return PixelType;
}

ZESize ZETRBlock::GetPixelSize()
{
	switch (PixelType)
	{
		default:
		case ZETR_PT_NONE:
			return 0;
		case ZETR_PT_ELEVATION:
			return 4;
		case ZETR_PT_COLOR:
			return 4;
		case ZETR_PT_GRAYSCALE:
			return 1;
	}
}

void* ZETRBlock::GetData()
{
	return (ZEUInt8*)Data;
}

ZESize ZETRBlock::GetPitch()
{
	return Size * GetPixelSize();
}

ZESize ZETRBlock::GetDataSize()
{
	return DataSize;
}

bool ZETRBlock::Load(const ZEString& FileName)
{
	ZEFileInfo FileInfo(FileName);
	if (!FileInfo.IsExists())
	{
		Status = ZETR_BRS_NOT_AVAILABLE;
		return false;
	}

	ZEFile File;
	if (!File.Open(FileName, ZE_FOM_READ, ZE_FCM_NONE))
	{
		Status = ZETR_BRS_NOT_AVAILABLE;
		return false;
	}

	if (!Load(&File))
		return false;

	File.Close();

	return true;
}

bool ZETRBlock::Load(ZEFile* File)
{
	if (Status != ZETR_BRS_NONE)
		return false;

	if (File == NULL)
		return false;

	Status = ZETR_BRS_LOADING;

	Clean();

	ZETRBlockFileHeader Header = {0};
	if (!File->Read(&Header, sizeof(ZETRBlockFileHeader), 1) == 1)
	{
		Status = ZETR_BRS_NOT_AVAILABLE;
		return false;
	}

	if (Header.Type == ZETR_PT_NONE || Header.Size == 0)
	{
		Status = ZETR_BRS_NOT_AVAILABLE;
		return false;
	}

	PositionX = Header.PositionX;
	PositionY = Header.PositionY;
	Level = Header.Level;
	PixelType = (ZETRPixelType)Header.Type;
	Size = Header.Size;

	if (PixelType == ZETR_PT_NONE || Size == 0)
	{
		Clean();
		Status = ZETR_BRS_NOT_AVAILABLE;
		return false;
	}

	DataSize = Size * Size * GetPixelSize();
	Data = new ZEBYTE[DataSize];

	if (File->Read(GetData(), DataSize, 1) != 1)
	{
		Clean();
		Status = ZETR_BRS_NOT_AVAILABLE;
		return false;
	}

	Status = ZETR_BRS_AVAILABLE;
	return true;
}

ZETRBlock::ZETRBlock()
{
	Data = NULL;
	Clean();
}

ZETRBlock::~ZETRBlock()
{
	Clean();
}


ZETRBlock* ZETRBlock::Create(ZEUInt64 BlockSquence, ZEInt64 PositionX, ZEInt64 PositionY, ZEInt Level)
{
	ZETRBlock* NewBlock = new ZETRBlock();
	NewBlock->PositionX = PositionX;
	NewBlock->PositionY = PositionY;
	NewBlock->Level = Level;
	NewBlock->BlockSquence = BlockSquence;

	return NewBlock;
}
