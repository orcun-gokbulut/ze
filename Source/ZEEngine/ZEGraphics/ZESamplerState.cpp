//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESamplerState.cpp
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
#include "ZESamplerState.h"

void ZESamplerState::SetMinFilter(ZETextureFilterMode FilterMode)
{
	MinFilter = FilterMode;
	Changed = true;
}

ZETextureFilterMode ZESamplerState::GetMinFilter() const
{
	return MinFilter;
}

void ZESamplerState::SetMagFilter(ZETextureFilterMode FilterMode)
{
	MagFilter = FilterMode;
	Changed = true;
}

ZETextureFilterMode	ZESamplerState::GetMagFilter() const
{
	return MagFilter;
}

void ZESamplerState::SetMipFilter(ZETextureFilterMode FilterMode)
{
	MipFilter = FilterMode;
	Changed = true;
}

ZETextureFilterMode ZESamplerState::GetMipFilter() const
{
	return MipFilter;
}

void ZESamplerState::SetAddressU(ZETextureAdressMode AdressMode)
{
	AddressU = AdressMode;
	Changed = true;
}

ZETextureAdressMode	ZESamplerState::GetAddressU() const
{
	return AddressU;
}

void ZESamplerState::SetAddressV(ZETextureAdressMode AdressMode)
{
	AddressV = AdressMode;
	Changed = true;
}

ZETextureAdressMode	ZESamplerState::GetAddressV() const
{
	return AddressV;
}

void ZESamplerState::SetAddressW(ZETextureAdressMode AdressMode)
{
	AddressW = AdressMode;
	Changed = true;
}

ZETextureAdressMode	ZESamplerState::GetAddressW() const
{
	return AddressW;
}

void ZESamplerState::SetMaxAnisotrophy(unsigned int AnisotrophyLevel)
{
	MaxAnisotropy = AnisotrophyLevel;
	Changed = true;
}

unsigned int ZESamplerState::GetMaxAnisotrophy() const
{
	return MaxAnisotropy;
}

void ZESamplerState::SetBorderColor(const ZEVector4 &Color)
{
	BorderColor = Color;
	Changed = true;
}

ZEVector4 ZESamplerState::GetBorderColor() const
{
	return BorderColor;
}

void ZESamplerState::SetMaxLOD(float LOD)
{
	MaxLOD = LOD;
	Changed = true;
}

float ZESamplerState::GetMaxLOD() const
{
	return MaxLOD;
}

void ZESamplerState::SetChanged(bool Change)
{
	Changed = Change;
}

bool ZESamplerState::GetChanged() const
{
	return Changed;
}

void ZESamplerState::SetCurrentTexture(ZETexture* Texture)
{
	CurrentTexture = Texture;
	Changed = true;
}

ZETexture* ZESamplerState::GetTexture() const
{
	return CurrentTexture;
}

const ZESamplerState& ZESamplerState::operator=(const ZESamplerState& State)
{
	AddressU = State.AddressU;
	AddressV = State.AddressV;
	AddressW = State.AddressW;

	MinFilter = State.MinFilter;
	MagFilter = State.MagFilter;
	MipFilter = State.MipFilter;

	MaxLOD = State.MaxLOD;
	MaxAnisotropy = State.MaxAnisotropy;
	BorderColor = State.BorderColor;
	Changed = State.Changed;
	return *this;
}

ZESamplerState::ZESamplerState() :	MinFilter(ZE_TFM_CURRENT),
									MagFilter(ZE_TFM_CURRENT),
									MipFilter(ZE_TFM_CURRENT),
									AddressU(ZE_TAM_CURRENT),
									AddressV(ZE_TAM_CURRENT),
									AddressW(ZE_TAM_CURRENT),
									MaxAnisotropy(1),
									BorderColor(),
									MaxLOD(0),
									Changed(false),
									CurrentTexture(NULL)
{

}

ZESamplerState::~ZESamplerState()
{

}
