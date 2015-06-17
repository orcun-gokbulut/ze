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

#pragma once

#include "ZEGRState.h"

#include "ZEMath/ZEVector.h"

enum ZEGRTextureAddressMode
{
	ZEGR_TAM_WRAP			    = 1,
	ZEGR_TAM_MIRROR			    = 2,
	ZEGR_TAM_CLAMP			    = 3,
	ZEGR_TAM_BORDER				= 4
};

enum ZEGRTextureFilterMode
{
	ZEGR_TFM_POINT				= 1,
	ZEGR_TFM_LINEAR				= 2,
	ZEGR_TFM_ANISOTROPY			= 3
};

class ZEGRSamplerState : public ZEGRState
{
	private:
		struct ZESamplerStateData
		{
			ZEGRTextureAddressMode	AddressU : 4;
			ZEGRTextureAddressMode	AddressV : 4;
			ZEGRTextureAddressMode	AddressW : 4;
			ZEGRTextureFilterMode	MinFilter : 4;
			ZEGRTextureFilterMode	MagFilter : 4;
			ZEGRTextureFilterMode	MipFilter : 4;
			ZEUInt32				MaxAnisotropy : 6;

			float					MinLOD;
			float					MaxLOD;
			float					MipLODBias;
			ZEVector4				BorderColor;
		
		} StateData;

	public:
		void						SetMinFilter(ZEGRTextureFilterMode FilterMode);
		ZEGRTextureFilterMode		GetMinFilter() const;
		
		void						SetMagFilter(ZEGRTextureFilterMode FilterMode);
		ZEGRTextureFilterMode		GetMagFilter() const;
		
		void						SetMipFilter(ZEGRTextureFilterMode FilterMode);
		ZEGRTextureFilterMode		GetMipFilter() const;
		
		void						SetAddressU(ZEGRTextureAddressMode AdressMode);
		ZEGRTextureAddressMode		GetAddressU() const;
		
		void						SetAddressV(ZEGRTextureAddressMode AdressMode);
		ZEGRTextureAddressMode		GetAddressV() const;
		
		void						SetAddressW(ZEGRTextureAddressMode AdressMode);
		ZEGRTextureAddressMode		GetAddressW() const;
		
		void						SetMipLODBias(float LODBias);
		float						GetMipLODBias() const;

		void						SetMaxAnisotrophy(ZEUInt AnisotrophyLevel);
		ZEUInt						GetMaxAnisotrophy() const;
		
		void						SetBorderColor(const ZEVector4& Color);
		ZEVector4					GetBorderColor() const;
		
		void						SetMinLOD(float LOD);
		float						GetMinLOD() const;

		void						SetMaxLOD(float LOD);
		float						GetMaxLOD() const;

		void						SetToDefault();

									ZEGRSamplerState();
		virtual						~ZEGRSamplerState();
};
