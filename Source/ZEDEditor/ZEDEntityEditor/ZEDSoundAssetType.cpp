//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDSoundAssetType.cpp
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

#include "ZEDSoundAssetType.h"
#include "ZEDSoundAsset.h"

ZEDSoundAssetType::ZEDSoundAssetType()
{

}

const char* ZEDSoundAssetType::GetName() const
{
	return "Sound";
}

const char* const* ZEDSoundAssetType::GetExtensions() const
{
	static const char* Extensions[3] =
	{
		".wav",
		".ogg",
		".mp3",
	};

	return Extensions;
}

ZESize ZEDSoundAssetType::GetExtensionCount() const
{
	return 3;
}

const ZEVector3& ZEDSoundAssetType::GetColor() const 
{
	static ZEVector3 Color(1.0f, 0.756f, 0.027f);
	return Color;
}

const char* ZEDSoundAssetType::GetIconPath(const ZEString& Extension) const
{
	if (Extension.EqualsIncase(".wav") || Extension.EqualsIncase(".wave"))
		return "#R:/ZEDEditor/Icons/ZEDAsset/ZEDSoundAsset-WAV.png";
	else if (Extension.EqualsIncase(".mp3"))
		return "#R:/ZEDEditor/Icons/ZEDAsset/ZEDSoundAsset-MP3.png";
	else if (Extension.EqualsIncase(".ogg"))
		return "#R:/ZEDEditor/Icons/ZEDAsset/ZEDSoundAsset-OGG.png";
	else
		return "#R:/ZEDEditor/Icons/ZEDAsset/ZEDSoundAsset-General.png";
}

const char* const* ZEDSoundAssetType::GetCustomProperties() const
{
	static const char* Properties[4] = 
	{
		"Bitrate",
		"Bit",
		"SampleCount",
		"Format"
	};

	return Properties;
}

ZESize ZEDSoundAssetType::GetCustomPropertyCount() const 
{
	return 4;
}

ZEDAsset* ZEDSoundAssetType::Wrap(const ZEString& Path) const 
{
	ZEDSoundAsset* Asset = new ZEDSoundAsset();
	Asset->SetType(const_cast<ZEDSoundAssetType*>(this));
	return Asset;
}
