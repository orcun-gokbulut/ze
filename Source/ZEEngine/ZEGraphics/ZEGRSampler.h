//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRSampler.h
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

#include "ZEMeta/ZEEnumerator.h"
#include "ZEMeta/ZEObject.h"

#include "ZEInitializable.h"
#include "ZEDS/ZELink.h"
#include "ZEDS/ZEList2.h"
#include "ZEMath/ZEVector.h"
#include "ZEPointer/ZEReferenceCounted.h"
#include "ZEPointer/ZEHolder.h"

#include "ZEGRState.h"


ZE_ENUM_TYPED(ZEGRTextureAddressing, ZEUInt8)
{
	ZEGR_TAM_WRAP						= 0,
	ZEGR_TAM_MIRROR						= 1,
	ZEGR_TAM_CLAMP						= 2,
	ZEGR_TAM_BORDER						= 3
};

ZE_ENUM_TYPED(ZEGRTextureFilter, ZEUInt8)
{
	ZEGR_TFM_POINT						= 0,
	ZEGR_TFM_LINEAR						= 1,
	ZEGR_TFM_ANISOTROPIC				= 2
};

struct ZEGRSamplerDescription
{
	ZEGRTextureAddressing					AddressU;
	ZEGRTextureAddressing					AddressV;
	ZEGRTextureAddressing					AddressW;
	ZEGRTextureFilter						MinFilter;
	ZEGRTextureFilter						MagFilter;
	ZEGRTextureFilter						MipFilter;
	ZEGRComparisonFunction					ComparisonFunction;
	ZEUInt8									MaxAnisotropy;

	float									MinLOD;
	float									MaxLOD;
	float									MipMapLODBias;
	ZEVector4								BorderColor;

											ZEGRSamplerDescription();
};

class ZEGRSampler : public ZEObject
{
	ZE_OBJECT
	private:
		static ZEList2<ZEGRSampler>			SamplerCache;
		static ZELock						SamplerCacheLock;
		ZELink<ZEGRSampler>					SamplerCacheLink;

	protected:
		ZEGRSamplerDescription				Description;

		virtual bool						Initialize() = 0;
		virtual void						Deinitialize() = 0;

											ZEGRSampler();
		virtual								~ZEGRSampler();

	public:
		const ZEGRSamplerDescription&		GetDescription() const;

		static ZEHolder<ZEGRSampler>		GetSampler(const ZEGRSamplerDescription& SamplerDescription);
		static ZEHolder<ZEGRSampler>		GetDefaultSampler();
};
