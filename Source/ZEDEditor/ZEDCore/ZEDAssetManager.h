//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDAssetManager.h
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

#include "ZEDComponent.h"

#include "ZEDAsset.h"

#include "ZEDEvent.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEString.h"
#include "ZEThread/ZEThread.h"
#include "ZERegEx/ZERegEx.h"

class ZEDAsset;
class ZEDAssetType;

ZE_ENUM(ZEDAssetEventType)
{
	ZED_AET_ASSET_ADDED,
	ZED_AET_ASSET_REMOVED,
	ZED_AET_ASSET_CHANGED,
	ZED_AET_ASSET_CATEGORY_REMOVED,
	ZED_AET_ASSET_CATEGORY_ADDED,
	ZED_AET_ASSET_CATEGORY_CHANGED
};

class ZEDAssetEvent : public ZEDEvent
{
	public:
		ZEDAssetEventType						Type;
		ZEString								Path;
};

class ZEDAssetCategory
{
	friend class ZEDAssetManager;
	private:
		ZEDAssetCategory*						Parent;
		ZEString								Name;
		ZESize									Count;
		ZEArray<ZEDAsset*>						Assets;
		ZEArray<ZEDAssetCategory*>				SubCatagories;

		ZEString								GetPath();
};

class ZEDAssetTag
{
	private:
		ZEString								Name;
		ZEArray<ZEDAsset*>						Assets;
};

class ZEDAssetManager : public ZEDComponent
{
	ZE_OBJECT
	private:
		ZEArray<ZEDAssetType*>					AssetTypes;
		ZESmartArray<ZEDAsset*>					Assets;
		ZEArray<ZEDAssetCategory*>				Categories;

		ZERegEx									MatchCategoryLast;
		ZERegEx									MatchCategoryFirst;
		ZERegEx									MatchCategorySingle;

		ZEThread								CrawlerThread;
		ZEThread								MonitorThread;
		ZEArray<ZEString>						CrawlLocations;

		ZEString								ResourcePath;

		void									UpdateCachedAsset(const ZEString& Path);

		void									CrawlerFunction(ZEThread* Thread, void* ExtraParameters);
		void									MonitorFunction(ZEThread* Thread, void* ExtraParameters);

		void									AddAssetToCache(ZEDAsset* Asset);
		void									RemoveAssetFromCache(ZEDAsset* Asset);
		void									UpdateAssetInCache(ZESize Index, const ZEDAsset& Asset);

		ZEDAssetCategory*						GetCategory(const ZEString& Category);
		ZEDAssetCategory*						AddCategory(const ZEString& Category);
		void									RemoveCategory(ZEDAssetCategory* Category);
		void									AddAssetToCategory(ZEDAsset* Asset);
		void									RemoveAssetFromCategory(ZEDAsset* Asset);
		void									GetCategoriesInternal(ZESmartArray<ZEString>& Output, ZEDAssetCategory* CurrentPath, const ZEString& ParentPathString, bool Recursive);

		const ZEDAssetTag*						GetTag(const ZEString& Name) const;
		void									AddTag(const ZEString& Name);
		void									RemoveTag(const ZEString& Name);
		void									AddAssetToTags(ZEDAsset* Asset);
		void									RemoveAssetFromTags(ZEDAsset* Asset);

		virtual bool							InitializeInternal();
		virtual bool							DeinitializeInternal();

		void									Clear();

												ZEDAssetManager();
		virtual									~ZEDAssetManager();

	public:
		const ZEArray<const ZEDAssetType*>&		GetAssetTypes();
		void									RegisterAssetType(ZEDAssetType* AssetType);
		void									UnregisterAssetType(ZEDAssetType* AssetType);

		void									SetResourcePath(const ZEString& ResourceDirectory);
		const ZEString&							GetResourcePath();

		ZESmartArray<ZEString>					GetCategories(const ZEString& CategoryPath = ZEString::Empty, bool Recursive = false);
		const ZESmartArray<const ZEDAsset*>&	GetAssetsInCache();
		ZESmartArray<ZEDAsset>					GetAssetsInDirectory(const ZEString& DirectoryPath, bool Recursive = false, bool IncludeNonAsset = false);
		ZESmartArray<ZEDAsset>					GetAssetsInCategory(const ZEString& CategoryPath, bool Recursive = false);
		ZESmartArray<ZEDAsset>					GetAssetsByTags(const ZEArray<ZEString>& Tags);
		
		void									UpdateAssetCache(const ZEString& Path);
		bool									WrapAsset(ZEDAsset* Asset, const ZEString& Path);

		static ZEDAssetManager*					CreateInstance();
};
