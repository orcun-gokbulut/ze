//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDAssetCategory.cpp
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

#include "ZEDAssetCategory.h"

#include "ZEDS/ZEFormat.h"

ZEDAssetCategory::ZEDAssetCategory()
{
	Manager = NULL;
	ParentCategory = NULL;
	Persistent = false;
}

ZEDAssetCategory::~ZEDAssetCategory()
{
	while (Assets.GetCount() != 0)
		Assets.Remove(Assets.GetFirst());

	while (SubCatagories.GetCount() != 0)
		delete SubCatagories.GetFirst()->GetItem();
}

ZEDAssetManager* ZEDAssetCategory::GetManager() const
{
	return Manager;
}

ZEDAssetCategory* ZEDAssetCategory::GetParentCategory() const
{
	return ParentCategory;
}

ZEString ZEDAssetCategory::GetPath() const
{
	if (GetParentCategory() == NULL)
		return Name;
	else
		return ZEFormat::Format("{0}/{1}", GetParentCategory()->GetName(), Name);
}

void ZEDAssetCategory::SetName(const ZEString& Name)
{
	this->Name = Name;
}

const ZEString& ZEDAssetCategory::GetName() const
{
	return Name;
}

void ZEDAssetCategory::SetPersistent(bool Persistent)
{
	this->Persistent = Persistent;
}

bool ZEDAssetCategory::GetPersistent()
{
	return Persistent;
}

const ZEList2<ZEDAsset>& ZEDAssetCategory::GetAssets() const
{
	return Assets;
}

const ZEList2<ZEDAssetCategory>& ZEDAssetCategory::GetSubCatagories() const
{
	return SubCatagories;
}
