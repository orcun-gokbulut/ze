//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDAsset.h
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

#include "ZEMeta/ZEObject.h"

#include "ZETimeStamp.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEString.h"
#include "ZEDS/ZELink.h"
#include "ZEDAssetMeta.h"
#include "ZEDAssetDependency.h"

class ZEDAsset;
class ZEDAssetType;
class ZEDAssetDirectory;
class ZEDAssetCategory;
class ZEDAssetManager;
class ZEDThumbnailWidget;
class ZEDPreviewWidget;
class ZEDEditor;

struct ZEDAssetDependencyItem
{
	ZEDAsset*							AssetName;
	ZEDAssetDependency					Dependency;
};

class ZEDAsset : public ZEObject
{
	ZE_OBJECT
	friend class ZEDAssetCategory;
	friend class ZEDAssetDirectory;
	friend class ZEDAssetManager;
	private:
		ZEDAssetManager*								Manager;

		ZEDAssetDirectory*								Directory;
		ZELink<ZEDAsset>								DirectoryLink;

		ZEDAssetCategory*								Category;
		ZELink<ZEDAsset>								CategoryLink;

		ZEDAssetType*									Type;
		ZELink<ZEDAsset>								TypeLink;

		ZEString										Name;
		ZEString										CategoryPath;
		ZEArray<ZEString>								Tags;
		ZESize											FileSize;
		ZETimeStamp										ModificationTime;
	
	protected:
														ZEDAsset();
		virtual											~ZEDAsset();

	public:
		ZEDAssetManager*								GetManager() const;
		ZEDAssetDirectory*								GetDirectory() const;
		ZEDAssetCategory*								GetCategory() const;
		ZEString										GetCategoryPath() const;

		void											SetName(const ZEString& Name);
		const ZEString&									GetName() const;
		
		ZEString										GetPath() const;
		ZEDAssetType*									GetType() const;

		const ZEArray<ZEString>&						GetTags() const;

		ZESize											GetFileSize() const;
		ZETimeStamp										GetModificationTime() const;
		
		virtual ZEVariant								GetAssetProperty(const ZEString& PropertyName);
		virtual ZEDThumbnailWidget*						CreateThumbnailWidget();
		virtual ZEDPreviewWidget*						CreatePreviewWidget();
		virtual ZEDEditor*								CreateEditor() const;
		virtual bool									ReadMetaData(ZEDAssetMetaData& MetaData) const;
		virtual bool									WriteMetaData(const ZEDAssetMetaData& MetaData) const;

		virtual ZEArray<ZEDAssetDependencyItem>			ReadDependencies() const;

};
