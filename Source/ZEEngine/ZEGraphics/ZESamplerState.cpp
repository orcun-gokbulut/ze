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

#include "ZEMath\ZEMath.h"

#include <memory.h>

void ZEGRSamplerState::SetMinFilter(ZEGRTextureFilterMode FilterMode)
{
	if (StateData.MinFilter != FilterMode)
	{
		StateData.MinFilter = FilterMode;
		MarkDirty();
	}
}

ZEGRTextureFilterMode ZEGRSamplerState::GetMinFilter() const
{
	return StateData.MinFilter;
}

void ZEGRSamplerState::SetMagFilter(ZEGRTextureFilterMode FilterMode)
{
	if (StateData.MagFilter != FilterMode)
	{
		StateData.MagFilter = FilterMode;
		MarkDirty();
	}
}

ZEGRTextureFilterMode	ZEGRSamplerState::GetMagFilter() const
{
	return StateData.MagFilter;
}

void ZEGRSamplerState::SetMipFilter(ZEGRTextureFilterMode FilterMode)
{
	if (StateData.MipFilter != FilterMode)
	{
		StateData.MipFilter = FilterMode;
		MarkDirty();
	}
}

ZEGRTextureFilterMode ZEGRSamplerState::GetMipFilter() const
{
	return StateData.MipFilter;
}

void ZEGRSamplerState::SetAddressU(ZEGRTextureAddressMode AdressMode)
{
	if (StateData.AddressU != AdressMode)
	{
		StateData.AddressU = AdressMode;
		MarkDirty();
	}
}

ZEGRTextureAddressMode ZEGRSamplerState::GetAddressU() const
{
	return StateData.AddressU;
}

void ZEGRSamplerState::SetAddressV(ZEGRTextureAddressMode AdressMode)
{
	if (StateData.AddressV != AdressMode)
	{
		StateData.AddressV = AdressMode;
		MarkDirty();
	}
}

ZEGRTextureAddressMode ZEGRSamplerState::GetAddressV() const
{
	return StateData.AddressV;
}

void ZEGRSamplerState::SetAddressW(ZEGRTextureAddressMode AdressMode)
{
	if (StateData.AddressW != AdressMode)
	{
		StateData.AddressW = AdressMode;
		MarkDirty();
	}
}

ZEGRTextureAddressMode ZEGRSamplerState::GetAddressW() const
{
	return StateData.AddressW;
}

void ZEGRSamplerState::SetMipLODBias(float LODBias)
{
	if (ZEMath::Abs(StateData.MipLODBias - LODBias) > ZE_ZERO_THRESHOLD)
	{
		StateData.MipLODBias = LODBias;
		MarkDirty();
	}
}

float ZEGRSamplerState::GetMipLODBias() const
{
	return StateData.MipLODBias;
}

void ZEGRSamplerState::SetMaxAnisotrophy(unsigned int AnisotrophyLevel)
{
	if (StateData.MaxAnisotropy != AnisotrophyLevel)
	{
		StateData.MaxAnisotropy = AnisotrophyLevel;
		MarkDirty();
	}
}

unsigned int ZEGRSamplerState::GetMaxAnisotrophy() const
{
	return StateData.MaxAnisotropy;
}

void ZEGRSamplerState::SetBorderColor(const ZEVector4 &Color)
{
	if (StateData.BorderColor != Color)
	{
		StateData.BorderColor = Color;
		MarkDirty();
	}
}

ZEVector4 ZEGRSamplerState::GetBorderColor() const
{
	return StateData.BorderColor;
}

void ZEGRSamplerState::SetMinLOD(float LOD)
{
	if (ZEMath::Abs(StateData.MinLOD - LOD) > ZE_ZERO_THRESHOLD)
	{
		StateData.MinLOD = LOD;
		MarkDirty();
	}
}

float ZEGRSamplerState::GetMinLOD() const
{
	return StateData.MinLOD;
}

void ZEGRSamplerState::SetMaxLOD(float LOD)
{
	if (ZEMath::Abs(StateData.MaxLOD - LOD) > ZE_ZERO_THRESHOLD)
	{
		StateData.MaxLOD = LOD;
		MarkDirty();
	}
}

float ZEGRSamplerState::GetMaxLOD() const
{
	return StateData.MaxLOD;
}

void ZEGRSamplerState::SetToDefault()
{
	memset(&StateData, 0, sizeof(ZESamplerStateData));
	StateData.MinFilter = ZEGR_TFM_POINT;
	StateData.MagFilter = ZEGR_TFM_POINT;
	StateData.MipFilter = ZEGR_TFM_POINT;
	StateData.AddressU = ZEGR_TAM_CLAMP;
	StateData.AddressV = ZEGR_TAM_CLAMP;
	StateData.AddressW = ZEGR_TAM_CLAMP;
	StateData.MinLOD = ZE_FLOAT_MIN;
	StateData.MaxLOD = ZE_FLOAT_MAX;
	StateData.MaxAnisotropy = 8;
	StateData.MipLODBias = 0.0f;
	StateData.BorderColor = ZEVector4::Zero;
}

ZEGRSamplerState::ZEGRSamplerState()
{
	SetToDefault();
}

ZEGRSamplerState::~ZEGRSamplerState()
{
	
}
