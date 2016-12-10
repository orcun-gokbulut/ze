//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDAsset.cpp
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

#include "ZEDAsset.h"
#include "ZEFile\ZEFileInfo.h"
#include "ZEDAssetManager.h"

ZEUInt ZEDAsset::GetHash() const
{
	return Hash;
}

void ZEDAsset::SetType(ZEDAssetType* Type)
{
	this->Type = Type;
}

void ZEDAsset::SetPath(const ZEString& Path)
{
	if (this->Path == Path)
		return;

	this->Path = ZEFileInfo(Path).Normalize();
	this->Hash = this->Path.Hash();

	ZEPathInfo PathInfo(Path);
	ModificationTime = PathInfo.GetModificationTime();
}

ZEString ZEDAsset::GetPath() const
{
	return Path;
}

ZEDAssetType* ZEDAsset::GetType() const
{
	return Type;
}

void ZEDAsset::SetCategory(const ZEString& Category)
{
	this->Category = Category;
}

const ZEString& ZEDAsset::GetCategory() const
{
	return Category;
}

void ZEDAsset::SetTags(const ZEArray<ZEString>& Tags)
{
	this->Tags = Tags;
}

const ZEArray<ZEString>& ZEDAsset::GetTags() const
{
	return Tags;
}

ZETimeStamp ZEDAsset::GetModificationTime()
{
	return ModificationTime;
}

ZEDAsset::ZEDAsset()
{
	Type = NULL;
	Hash = 0;
	ModificationTime = 0;
}

ZEDAsset::~ZEDAsset()
{

}
