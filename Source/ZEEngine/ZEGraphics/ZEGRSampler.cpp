//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRSampler.cpp
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

#include "ZEGRSampler.h"

#include "ZEMath/ZEMath.h"
#include "ZEPointer/ZEPointer.h"

#include "ZEGRGraphicsModule.h"

ZEList2<ZEGRSampler> ZEGRSampler::SamplerCache;
ZELock ZEGRSampler::SamplerCacheLock;

ZEGRSamplerDescription::ZEGRSamplerDescription()
{
	MinFilter = ZEGR_TFM_LINEAR;
	MagFilter = ZEGR_TFM_LINEAR;
	MipFilter = ZEGR_TFM_LINEAR;
	AddressU = ZEGR_TAM_CLAMP;
	AddressV = ZEGR_TAM_CLAMP;
	AddressW = ZEGR_TAM_CLAMP;
	MinLOD = ZE_FLOAT_MIN;
	MaxLOD = ZE_FLOAT_MAX;
	MaxAnisotropy = 1;
	MipMapLODBias = 0.0f;
	BorderColor = ZEVector4::One;
	ComparisonFunction = ZEGR_CF_NEVER;
}

ZEGRSampler::ZEGRSampler() : SamplerCacheLink(this)
{

}

ZEGRSampler::~ZEGRSampler()
{
	SamplerCacheLock.Lock();
	SamplerCache.Remove(&SamplerCacheLink);
	SamplerCacheLock.Unlock();
}

const ZEGRSamplerDescription& ZEGRSampler::GetDescription() const
{
	return Description;
}

ZEHolder<ZEGRSampler> ZEGRSampler::GetSampler(const ZEGRSamplerDescription& SamplerDescription)
{
	ze_for_each(Entry, SamplerCache)
	{
		if (Entry->Description.AddressU == SamplerDescription.AddressU &&
			Entry->Description.AddressV == SamplerDescription.AddressV &&
			Entry->Description.AddressW == SamplerDescription.AddressW &&
			Entry->Description.MinFilter == SamplerDescription.MinFilter &&
			Entry->Description.MagFilter == SamplerDescription.MagFilter &&
			Entry->Description.MipFilter == SamplerDescription.MipFilter &&
			Entry->Description.ComparisonFunction == SamplerDescription.ComparisonFunction &&
			Entry->Description.MaxAnisotropy == SamplerDescription.MaxAnisotropy &&
			Entry->Description.MinLOD == SamplerDescription.MinLOD &&
			Entry->Description.MaxLOD == SamplerDescription.MaxLOD &&
			Entry->Description.MipMapLODBias == SamplerDescription.MipMapLODBias &&
			Entry->Description.BorderColor == SamplerDescription.BorderColor)
		{
			return &(*Entry);
		}
	}

	ZEGRSampler* Sampler = ZEGRGraphicsModule::GetInstance()->CreateSamplerState();
	Sampler->Description = SamplerDescription;

	if (!Sampler->Initialize())
	{
		delete Sampler;
		return NULL;
	}

	SamplerCacheLock.Lock();
	SamplerCache.AddEnd(&Sampler->SamplerCacheLink);
	SamplerCacheLock.Unlock();

	return Sampler;
}

ZEHolder<ZEGRSampler> ZEGRSampler::GetDefaultSampler()
{
	static ZEHolder<ZEGRSampler> DefaultSampler;
	if (DefaultSampler.IsNull())
	{
		ZEGRSamplerDescription DefaultDescription;
		DefaultSampler = ZEGRSampler::GetSampler(DefaultDescription);
	}

	return DefaultSampler;
}
