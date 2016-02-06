//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11Sampler.cpp
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

#include "ZED11Sampler.h"

#include "ZEError.h"
#include "ZEGraphics/ZEGRDefinitions.h"

static D3D11_COMPARISON_FUNC Convert(ZEGRComparisonFunction CompFunc)
{
	switch (CompFunc)
	{
	default:
		zeDebugCheck(true, "Wrong ZEGRComparisonFunction value.");

	case ZEGR_CF_NEVER:
		return D3D11_COMPARISON_NEVER;

	case ZEGR_CF_LESS:
		return D3D11_COMPARISON_LESS;

	case ZEGR_CF_EQUAL:
		return D3D11_COMPARISON_EQUAL;

	case ZEGR_CF_LESS_EQUAL:
		return D3D11_COMPARISON_LESS_EQUAL;

	case ZEGR_CF_GREATER:
		return D3D11_COMPARISON_GREATER;

	case ZEGR_CF_NOT_EQUAL:
		return D3D11_COMPARISON_NOT_EQUAL;

	case ZEGR_CF_GREATER_EQUAL:
		return D3D11_COMPARISON_GREATER_EQUAL;

	case ZEGR_CF_ALWAYS:
		return D3D11_COMPARISON_ALWAYS;
	}
}

static D3D11_TEXTURE_ADDRESS_MODE Convert(ZEGRTextureAddressing TextureAddress)
{
	switch (TextureAddress)
	{
	default:
		zeDebugCheck(true, "Wrong ZEGRTextureAddressMode value.");

	case ZEGR_TAM_WRAP:
		return D3D11_TEXTURE_ADDRESS_WRAP;

	case ZEGR_TAM_MIRROR:
		return D3D11_TEXTURE_ADDRESS_MIRROR;

	case ZEGR_TAM_CLAMP:
		return D3D11_TEXTURE_ADDRESS_CLAMP;

	case ZEGR_TAM_BORDER:
		return D3D11_TEXTURE_ADDRESS_BORDER;
	}
}

static D3D11_FILTER_TYPE Convert(ZEGRTextureFilter FilterMode)
{
	switch (FilterMode)
	{
	default:
		zeDebugCheck(true, "Wrong ZEGRTextureFilterMode value.");

	case ZEGR_TFM_POINT:
		return D3D11_FILTER_TYPE_POINT;

	case ZEGR_TFM_LINEAR:
		return D3D11_FILTER_TYPE_LINEAR;
	}
}

static D3D11_FILTER Convert(ZEGRTextureFilter Min, ZEGRTextureFilter Mag, ZEGRTextureFilter Mip, bool Comparison)
{
	if (Min == ZEGR_TFM_ANISOTROPIC || Mag == ZEGR_TFM_ANISOTROPIC || Mip == ZEGR_TFM_ANISOTROPIC)
		return D3D11_FILTER_ANISOTROPIC;

	return (D3D11_FILTER)(	(Comparison ? D3D11_COMPARISON_FILTERING_BIT : 0) |
		(Convert(Min) << D3D11_MIN_FILTER_SHIFT) | 
		(Convert(Mag) << D3D11_MAG_FILTER_SHIFT) | 
		(Convert(Mip) << D3D11_MIP_FILTER_SHIFT));
}

bool ZED11Sampler::Initialize(const ZEGRSamplerDescription& Description)
{
	this->Description = Description;

	D3D11_SAMPLER_DESC SamplerDesc;
	memset(&SamplerDesc, 0, sizeof(D3D11_SAMPLER_DESC));
	SamplerDesc.BorderColor[0] = Description.BorderColor.x;
	SamplerDesc.BorderColor[1] = Description.BorderColor.y;
	SamplerDesc.BorderColor[2] = Description.BorderColor.z;
	SamplerDesc.BorderColor[3] = Description.BorderColor.w;
	SamplerDesc.ComparisonFunc = Convert(Description.ComparisonFunction);
	SamplerDesc.MinLOD = Description.MinLOD;
	SamplerDesc.MaxLOD = Description.MaxLOD;
	SamplerDesc.MipLODBias = Description.MipMapLODBias;
	SamplerDesc.MaxAnisotropy = Description.MaxAnisotropy;
	SamplerDesc.AddressU = Convert(Description.AddressU);
	SamplerDesc.AddressV = Convert(Description.AddressV);
	SamplerDesc.AddressW = Convert(Description.AddressW);
	SamplerDesc.Filter = Convert(Description.MinFilter, Description.MagFilter, Description.MipFilter, Description.ComparisonFunction != ZEGR_CF_NEVER);

	HRESULT Result = GetDevice()->CreateSamplerState(&SamplerDesc, &NativeSampler);
	if (FAILED(Result))
	{
		zeError("D3D11 Sampler state creation failed. ErrorCode: %d.", Result);
		return false;
	}

	return true;
}

void ZED11Sampler::Deinitialize()
{
	ZEGR_RELEASE(NativeSampler);
}

ZED11Sampler::ZED11Sampler()
{
}

ZED11Sampler::~ZED11Sampler()
{
	Deinitialize();
}

ID3D11SamplerState* ZED11Sampler::GetInterface() const
{
	return NativeSampler;
}
