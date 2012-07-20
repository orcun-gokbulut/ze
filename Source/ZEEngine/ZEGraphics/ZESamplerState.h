//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESamplerState.h
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

#ifndef __ZE_SAMPLER_STATE_H__
#define __ZE_SAMPLER_STATE_H__

#include "ZEFoundation/ZEMath/ZEVector.h"
#include "ZETexture.h"

enum ZETextureAdressMode
{
	ZE_TAM_CURRENT				= 0,	
	ZE_TAM_WRAP				    = 1,
	ZE_TAM_MIRROR			    = 2,
	ZE_TAM_CLAMP			    = 3,
	ZE_TAM_BORDER				= 4

};

enum ZETextureFilterMode
{
	ZE_TFM_CURRENT				= 0,
	ZE_TFM_NONE					= 1,
	ZE_TFM_POINT				= 2,
	ZE_TFM_LINEAR				= 3,
	ZE_TFM_ANISOTROPY			= 4
};

class ZESamplerState
{
protected:
	ZETextureFilterMode				MinFilter;
	ZETextureFilterMode				MagFilter;
	ZETextureFilterMode				MipFilter;
	ZETextureAdressMode				AddressU;
	ZETextureAdressMode				AddressV;
	ZETextureAdressMode				AddressW;
	ZEUInt							MaxAnisotropy;
	ZEVector4						BorderColor;
	float							MaxLOD;

	bool							Changed;

	// Currently Attached Texture
	ZETexture*						CurrentTexture;

public:
	void							SetMinFilter(ZETextureFilterMode FilterMode);
	ZETextureFilterMode				GetMinFilter() const;
	void							SetMagFilter(ZETextureFilterMode FilterMode);
	ZETextureFilterMode				GetMagFilter() const;
	void							SetMipFilter(ZETextureFilterMode FilterMode);
	ZETextureFilterMode				GetMipFilter() const;
	void							SetAddressU(ZETextureAdressMode AdressMode);
	ZETextureAdressMode				GetAddressU() const;
	void							SetAddressV(ZETextureAdressMode AdressMode);
	ZETextureAdressMode				GetAddressV() const;
	void							SetAddressW(ZETextureAdressMode AdressMode);
	ZETextureAdressMode				GetAddressW() const;
	void							SetMaxAnisotrophy(ZEUInt AnisotrophyLevel);
	ZEUInt							GetMaxAnisotrophy() const;
	void							SetBorderColor(const ZEVector4& Color);
	ZEVector4						GetBorderColor() const;
	void							SetMaxLOD(float LOD);
	float							GetMaxLOD() const;
	void							SetChanged(bool Change);
	bool							GetChanged() const;
	void							SetCurrentTexture(ZETexture* Texture);
	ZETexture*						GetTexture() const;

	const ZESamplerState&			operator=(const ZESamplerState& State);

									ZESamplerState();
	virtual							~ZESamplerState();
};
#endif
