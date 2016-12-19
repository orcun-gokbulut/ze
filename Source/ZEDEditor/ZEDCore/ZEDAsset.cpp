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

#include "ZEDS/ZEFormat.h"
#include "ZEFile/ZEFileInfo.h"
#include "ZEDAssetDirectory.h"
#include "ZEDAssetCategory.h"
#include "ZEDAssetType.h"
#include "ZEDAssetManager.h"

ZEDAsset::ZEDAsset() : DirectoryLink(this), CategoryLink(this), TypeLink(this)
{
	Manager = NULL;
	Directory = NULL;
	Category = NULL;
	Type = NULL;
	FileSize = 0;
	ModificationTime = 0;
}

ZEDAsset::~ZEDAsset()
{
	if (GetDirectory() != NULL)
		GetDirectory()->Assets.Remove(&DirectoryLink);

	if (GetCategory() != NULL)
		GetCategory()->Assets.Remove(&CategoryLink);

	if (GetType() != NULL)
		GetType()->Assets.Remove(&TypeLink);
}

ZEDAssetManager* ZEDAsset::GetManager() const
{
	return Manager;
}

ZEDAssetDirectory* ZEDAsset::GetDirectory() const
{
	return Directory;
}

ZEString ZEDAsset::GetPath() const
{
	if (GetDirectory() != NULL)
		return ZEFormat::Format("{0}/{1}", GetDirectory()->GetPath(), GetName());
	else
		return GetName();
}

ZEDAssetCategory* ZEDAsset::GetCategory() const
{
	return Category;
}

ZEString ZEDAsset::GetCategoryPath() const
{
	return CategoryPath;
}

void ZEDAsset::SetName(const ZEString& Name)
{
	this->Name = Name;
}

const ZEString& ZEDAsset::GetName() const
{
	return Name;
}

ZEDAssetType* ZEDAsset::GetType() const
{
	return Type;
}

ZEString ZEDAsset::GetIconPath()
{
	return ZEString::Empty;
}

void ZEDAsset::SetTags(const ZEArray<ZEString>& Tags)
{
	this->Tags = Tags;
}

const ZEArray<ZEString>& ZEDAsset::GetTags() const
{
	return Tags;
}

ZESize ZEDAsset::GetFileSize() const
{
	return FileSize;
}

ZETimeStamp ZEDAsset::GetModificationTime() const
{
	return ModificationTime;
}

ZEDAssetMetaData ZEDAsset::GetMetaData() const
{
	return ZEDAssetMetaData();
}
