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

enum ZEDAssetCrawlerReplayType
{
	ZED_ACRT_ADD,
	ZED_ACRT_DELETE
};

class ZEDAssetCrawlerRecord
{
	bool Delete;
	ZEDAsset* Asset;
};

class ZEDAssetManager : public ZEDComponent
{
	ZE_OBJECT
	private:
		ZEArray<ZEDAssetType*>					AssetTypes;
		ZEDAssetDirectory*						DirectoryRoot;
		ZEDAssetCategory*						CategoryRoot;
		ZEThread								CrawlerThread;
		void*									MonitorThreadHandle;
		ZEThread								MonitorThread;
		ZEArrayMT<ZEString>						CrawlLocations;
		ZESmartArrayMT<ZEDAssetCrawlerRecord>	CrawlerRecords;

		ZEString								ResourcePath;

		void									Crawl();
		void									CrawlerFunction(ZEThread* Thread, void* ExtraParameters);
		void									MonitorFunction(ZEThread* Thread, void* ExtraParameters);

		void									AddCrawlLocation(const ZEString& Location);

		ZEDAsset*								CreateAsset(const ZEString& Path);
		void									RemoveAsset(ZEDAsset* Asset);

		ZEDAssetDirectory*						CreateDirectory(const ZEString& DirectoryPath);
		void									RemoveDirectory(ZEDAssetDirectory* Directory);

		ZEDAssetCategory*						CreateCategory(const ZEString& CategoryPath);
		void									RemoveCategory(ZEDAssetCategory* Category);

		void									SetAssetCategory(ZEDAsset* Asset, ZEDAssetCategory* Category);

		void									RegisterAssetTypes();

		virtual bool							InitializeInternal();
		virtual bool							DeinitializeInternal();

		void									Clear();

		virtual void							TickEvent(const ZEDTickEvent* Event) override;

												ZEDAssetManager();
		virtual									~ZEDAssetManager();

	public:
		void									SetResourcePath(const ZEString& ResourceDirectory);
		const ZEString&							GetResourcePath();

		const ZEArray<const ZEDAssetType*>&		GetAssetTypes();
		ZEDAssetType*							GetAssetType(const ZEString& Path);
		void									RegisterAssetType(ZEDAssetType* AssetType);
		void									UnregisterAssetType(ZEDAssetType* AssetType);

		ZEDAssetDirectory*						GetDirectoryRoot();
		ZEDAssetCategory*						GetCategoryRoot();

		ZEDAsset*								GetAsset(const ZEString& AssetPath);
		ZEDAssetDirectory*						GetDirectory(const ZEString& DirectoryPath);
		ZEDAssetCategory*						GetCategory(const ZEString& CategoryPath);

		ZEDAsset*								ScanFile(const ZEString& FilePath);
		ZEDAssetDirectory*						ScanDirectory(const ZEString& DirectoryPath, bool Recursive = false);

		void									UpdatePath(const ZEString& FilePath); // Parallel

		virtual void							Process();

		static ZEDAssetManager*					CreateInstance();
};
