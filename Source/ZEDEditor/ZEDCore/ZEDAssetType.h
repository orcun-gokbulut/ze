//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDAssetType.h
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

#include "ZEMeta\ZEObject.h"

#include "ZEDAsset.h"

#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEString.h"
#include "ZEDS/ZEList2.h"
#include "ZEMath/ZEVector.h"

ZE_ENUM(ZEDAssetEditorType)
{
	ZED_AET_NONE,
	ZED_AET_INTERNAL,
	ZED_AET_EXTERNAL
};

ZE_ENUM(ZEDAssetTypeCapability)
{
	ZED_ATC_NONE,
	ZED_ATC_THUMBNAIL,
	ZED_ATC_PREVIEW,
	ZED_ATC_WRAPPER,
	ZED_ATC_EDITOR
};
typedef ZEFlags ZEDAssetTypeCapabilities;

class ZEDEditor;
class ZEDObjectWrapper;
class QWidget;

class ZEDAssetType : public ZEObject
{
	ZE_OBJECT
	friend class ZEDAsset;
	friend class ZEDAssetManager;
	private:
		ZEList2<ZEDAsset>						Assets;

	protected:
												ZEDAssetType();
		virtual									~ZEDAssetType();

	public:
		virtual const char*						GetName() const;
		virtual const char* const*				GetExtensions() const;
		virtual ZESize							GetExtensionCount() const;

		const ZEList2<ZEDAsset>&				GetAssets() const;

		virtual const ZEVector3&				GetColor() const;
		virtual const char*						GetIconPath(const ZEString& Extension) const;
		virtual const char* const*				GetCustomProperties() const;
		virtual ZESize							GetCustomPropertyCount() const;
		virtual ZEDAssetTypeCapabilities		GetCapabilities() const;

		virtual ZEDAsset*						Wrap(const ZEString& Path) const;
};
