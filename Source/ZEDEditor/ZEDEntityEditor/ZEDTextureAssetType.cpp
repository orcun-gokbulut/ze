//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDTextureAssetType.cpp
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

#include "ZEDTextureAssetType.h"
#include "ZEDTextureAsset.h"

ZEDTextureAssetType::ZEDTextureAssetType()
{

}

const char* ZEDTextureAssetType::GetName() const
{
	return "Texture";
}

const char* const* ZEDTextureAssetType::GetExtensions() const
{
	static const char* Extensions[9] =
	{
		".ZETexture",
		".bmp",
		".tga",
		".png",
		".tiff",
		".tif",
		".jpg",
		".jpeg",
		".dds"
	};

	return Extensions;
}

ZESize ZEDTextureAssetType::GetExtensionCount() const
{
	return 9;
}

const ZEVector3& ZEDTextureAssetType::GetColor() const 
{
	static ZEVector3 Color(0.247f, 0.318f, 0.027f);
	return Color;
}

const char* ZEDTextureAssetType::GetIconPath(const ZEString& Extension) const
{
	if (Extension.EqualsIncase(".png"))
		return "#R:/ZEDEditor/Icons/ZEDAsset/ZEDTextureAsset-PNG.png";
	else if (Extension.EqualsIncase(".jpg") || Extension.EqualsIncase(".jpeg"))
		return "#R:/ZEDEditor/Icons/ZEDAsset/ZEDTextureAsset-JPEG.png";
	else if (Extension.EqualsIncase(".tif") || Extension.EqualsIncase(".tiff"))
		return "#R:/ZEDEditor/Icons/ZEDAsset/ZEDTextureAsset-TIFF.png";
	else
		return "#R:/ZEDEditor/Icons/ZEDAsset/ZEDTextureAsset-General.png";
}


const char* const* ZEDTextureAssetType::GetCustomProperties() const
{
	static const char* Properties[4] = 
	{
		"Dimensions"
		"Width"
		"Height"
		"Format"
	};

	return Properties;
}

ZESize ZEDTextureAssetType::GetCustomPropertyCount() const 
{
	return 4;
}

ZEDAssetTypeCapabilities ZEDTextureAssetType::GetCapabilities() const
{
	return ZED_ATC_THUMBNAIL;
}

ZEDAsset* ZEDTextureAssetType::Wrap(const ZEString& Path) const 
{
	ZEDTextureAsset* Asset = new ZEDTextureAsset();
	Asset->SetType(const_cast<ZEDTextureAssetType*>(this));
	return Asset;
}
