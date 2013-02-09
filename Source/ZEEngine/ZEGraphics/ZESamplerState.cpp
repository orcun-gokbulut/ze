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

#include "ZEMath/ZEMath.h"
#include "ZESamplerState.h"
#include "ZEGraphicsModule.h"
#include "ZEDS/ZEHashGenerator.h"

void ZESamplerState::UpdateHash()
{
	if (Dirty)
	{
		Hash = 0;
		Dirty = false;
		ZEHashGenerator::Hash(Hash, &StateData, sizeof(ZESamplerStateData));
	}
}

void ZESamplerState::SetMinFilter(ZETextureFilterMode FilterMode)
{
	if (StateData.MinFilter != FilterMode)
	{
		StateData.MinFilter = FilterMode;
		Dirty = true;
	}
}

ZETextureFilterMode ZESamplerState::GetMinFilter() const
{
	return StateData.MinFilter;
}

void ZESamplerState::SetMagFilter(ZETextureFilterMode FilterMode)
{
	if (StateData.MagFilter != FilterMode)
	{
		StateData.MagFilter = FilterMode;
		Dirty = true;
	}
}

ZETextureFilterMode	ZESamplerState::GetMagFilter() const
{
	return StateData.MagFilter;
}

void ZESamplerState::SetMipFilter(ZETextureFilterMode FilterMode)
{
	if (StateData.MipFilter != FilterMode)
	{
		StateData.MipFilter = FilterMode;
		Dirty = true;
	}
}

ZETextureFilterMode ZESamplerState::GetMipFilter() const
{
	return StateData.MipFilter;
}

void ZESamplerState::SetAddressU(ZETextureAddressMode AdressMode)
{
	if (StateData.AddressU != AdressMode)
	{
		StateData.AddressU = AdressMode;
		Dirty = true;
	}
}

ZETextureAddressMode ZESamplerState::GetAddressU() const
{
	return StateData.AddressU;
}

void ZESamplerState::SetAddressV(ZETextureAddressMode AdressMode)
{
	if (StateData.AddressV != AdressMode)
	{
		StateData.AddressV = AdressMode;
		Dirty = true;
	}
}

ZETextureAddressMode ZESamplerState::GetAddressV() const
{
	return StateData.AddressV;
}

void ZESamplerState::SetAddressW(ZETextureAddressMode AdressMode)
{
	if (StateData.AddressW != AdressMode)
	{
		StateData.AddressW = AdressMode;
		Dirty = true;
	}
}

ZETextureAddressMode ZESamplerState::GetAddressW() const
{
	return StateData.AddressW;
}

void ZESamplerState::SetMipLODBias(float LODBias)
{
	if (ZEMath::Abs(StateData.MipLODBias - LODBias) > ZE_ZERO_THRESHOLD)
	{
		StateData.MipLODBias = LODBias;
		Dirty = true;
	}
}

float ZESamplerState::GetMipLODBias() const
{
	return StateData.MipLODBias;
}

void ZESamplerState::SetMaxAnisotrophy(unsigned int AnisotrophyLevel)
{
	if (StateData.MaxAnisotropy != AnisotrophyLevel)
	{
		StateData.MaxAnisotropy = AnisotrophyLevel;
		Dirty = true;
	}
}

unsigned int ZESamplerState::GetMaxAnisotrophy() const
{
	return StateData.MaxAnisotropy;
}

void ZESamplerState::SetBorderColor(const ZEVector4 &Color)
{
	if (StateData.BorderColor != Color)
	{
		StateData.BorderColor = Color;
		Dirty = true;
	}
}

ZEVector4 ZESamplerState::GetBorderColor() const
{
	return StateData.BorderColor;
}

void ZESamplerState::SetMinLOD(float LOD)
{
	if (ZEMath::Abs(StateData.MinLOD - LOD) > ZE_ZERO_THRESHOLD)
	{
		StateData.MinLOD = LOD;
		Dirty = true;
	}
}

float ZESamplerState::GetMinLOD() const
{
	return StateData.MinLOD;
}

void ZESamplerState::SetMaxLOD(float LOD)
{
	if (ZEMath::Abs(StateData.MaxLOD - LOD) > ZE_ZERO_THRESHOLD)
	{
		StateData.MaxLOD = LOD;
		Dirty = true;
	}
}

float ZESamplerState::GetMaxLOD() const
{
	return StateData.MaxLOD;
}

void ZESamplerState::SetToDefault()
{
	Hash = 0;
	Dirty  = false;

	StateData.MinFilter = ZE_TFM_POINT;
	StateData.MagFilter = ZE_TFM_POINT;
	StateData.MipFilter = ZE_TFM_POINT;
	StateData.AddressU = ZE_TAM_CLAMP;
	StateData.AddressV = ZE_TAM_CLAMP;
	StateData.AddressW = ZE_TAM_CLAMP;
	StateData.MaxAnisotropy = zeGraphics->GetAnisotropicFilter();
	StateData.BorderColor = ZEVector4::Zero;
	StateData.MipLODBias = -0.5f;
	StateData.MinLOD = ZE_FLOAT_MIN;
	StateData.MaxLOD = ZE_FLOAT_MAX;

	UpdateHash();
}

const ZESamplerState& ZESamplerState::operator=(const ZESamplerState& State)
{
	Hash = State.Hash;
	Dirty = State.Dirty;
	memcpy(&StateData, &State.StateData, sizeof(ZESamplerStateData));
	return *this;
}

bool ZESamplerState::operator==(const ZESamplerState& State)
{
	return memcmp(&StateData, &State.StateData, sizeof(ZESamplerStateData)) == 0 ? true : false;
}

bool ZESamplerState::operator!=(const ZESamplerState& State)
{
	return !operator==(State);
}

ZESamplerState::ZESamplerState()
{
	memset(&StateData, 0, sizeof(ZESamplerStateData));
	SetToDefault();
}

ZESamplerState::~ZESamplerState()
{

}
