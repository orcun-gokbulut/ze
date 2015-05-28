//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETRLayer.cpp
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

#include "ZETRLayer.h"
#include "ZETRLevel.h"

#include "ZEDS\ZEFormat.h"
#include "ZEML\ZEMLReader.h"
#include "ZEError.h"

bool ZETRLayer::LoadHeader()
{
	ZEString HeaderFile = ZEFormat::Format("{0}/Header.zeLayer", Path);
	ZEMLReader Reader;
	if (!Reader.Open(HeaderFile))
		return false;

	ZEMLReaderNode Node = Reader.GetRootNode();
	if (Node.GetName() != "ZETRLayer")
		return false;

	Name = Node.ReadString("Name");
	BlockSize = Node.ReadUInt32("BlockSize", 0);
	PixelType = (ZETRPixelType)Node.ReadUInt8("Type", 0);
	StartX = Node.ReadInt64("StartX", 0);
	StartY = Node.ReadInt64("StartY", 0);
	EndX = Node.ReadInt64("EndX", 0);
	EndY = Node.ReadInt64("EndY", 0);
	MinLevel = Node.ReadInt32("MinLevel", 0);
	MaxLevel = Node.ReadInt32("MaxLevel", 0);

	return true;
}

bool ZETRLayer::InitializeSelf()
{
	if (!LoadHeader())
	{
		zeError("Cannot load terrain layer header file. (Path: \"%s\")", ZEFormat::Format("{0}/Header.zeLayer", Path).ToCString());
		return false;
	}
	
	Cache.SetPath(Path);
	if (!Cache.Initialize())
	{
		zeError("Cannot initialize terrain layer block cache. (Path: \"%s\")", ZEFormat::Format("{0}/Header.zeLayer", Path).ToCString());
		return false;
	}

	Levels.SetCount(MaxLevel - MinLevel);
	for (ZESize I = MinLevel; I < MaxLevel; I++)
	{
		ZETRLevel*& CurrentLevel = Levels[I];
		CurrentLevel = new ZETRLevel();
		CurrentLevel->Layer = this;
		CurrentLevel->SetLevel(I);

		if (!CurrentLevel->Initialize())
			return false;
	}

	return true;
}

void ZETRLayer::DeinitializeSelf()
{
	Cache.Clean();
}

ZETerrain* ZETRLayer::GetTerrain()
{
	return Terrain;
}

ZETRBlockCache* ZETRLayer::GetBlockCache()
{
	return &Cache;
}

const ZEArray<ZETRLevel*>& ZETRLayer::GetLevels()
{
	return Levels;
}

ZETRLevel* ZETRLayer::GetLevel(ZEInt Level)
{
	for (ZESize I = 0; I < Levels.GetCount(); I++)
		if (Levels[I]->GetLevel() == Level)
			return Levels[0];

	return NULL;
}

void ZETRLayer::SetName(const ZEString& Name)
{
	this->Name = Name;
}

const ZEString& ZETRLayer::GetName()
{
	return Name;
}

void ZETRLayer::SetEnabled(bool Enabled)
{
	this->Enabled = Enabled;
}

bool ZETRLayer::GetEnabled()
{
	return Enabled;
}

void ZETRLayer::SetViewPosition(const ZEVector3& Position)
{
	ViewPosition = Position;
}

const ZEVector3& ZETRLayer::GetViewPosition()
{
	return ViewPosition;
}

ZEInt64 ZETRLayer::GetStartX()
{
	return StartX;
}

ZEInt64 ZETRLayer::GetStartY()
{
	return StartY;
}

ZEInt64 ZETRLayer::GetEndX()
{
	return EndX;
}

ZEInt64 ZETRLayer::GetEndY()
{
	return EndY;
}

ZEInt ZETRLayer::GetMinLevel()
{
	return MinLevel;
}

ZEInt ZETRLayer::GetMaxLevel()
{
	return MaxLevel;
}

ZETRPixelType ZETRLayer::GetPixelType()
{
	return PixelType;
}

ZESize ZETRLayer::GetBlockSize()
{
	return BlockSize;
}

void ZETRLayer::SetPath(const ZEString& Path)
{
	if (this->Path == Path)
		return;

	this->Path = Path;
	Reinitialize();
}

const ZEString& ZETRLayer::GetPath()
{
	return Path;
}

void ZETRLayer::Process()
{
	for (ZEInt I = 0; I < Levels.GetCount(); I++)
		Levels[I]->Process();
}

ZETRLayer::ZETRLayer()
{
	Terrain = NULL;
	StartX = 0;
	StartY = 0;
	EndX = 0;
	EndY = 0;
	MinLevel = 0;
	MaxLevel = 0;
	PixelType = ZETR_PT_NONE;
}

ZETRLayer::~ZETRLayer()
{

}
