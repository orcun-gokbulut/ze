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
#include "ZEGRGraphicsModule.h"

#include <memory.h>

ZEList2<ZEGRSampler> ZEGRSampler::SamplerUniqueList;

void ZEGRSamplerDescription::SetToDefault()
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

ZEGRSamplerDescription::ZEGRSamplerDescription()
{
	SetToDefault();
}

bool ZEGRSampler::Initialize(const ZEGRSamplerDescription& SamplerDescription)
{
	return true;
}

void ZEGRSampler::Deinitialize()
{

}

ZEGRSampler::ZEGRSampler()
{
}

ZEGRSampler::~ZEGRSampler()
{
	ZELink<ZEGRSampler>* Link = SamplerUniqueList.Find(this);
	if(Link != NULL)
		SamplerUniqueList.Remove(Link);
}

const ZEGRSamplerDescription& ZEGRSampler::GetDescription() const
{
	return Description;
}

ZESharedPointer<ZEGRSampler> ZEGRSampler::GetSampler(const ZEGRSamplerDescription& SamplerDescription)
{
	ze_for_each(Entry, SamplerUniqueList)
	{
		if(memcmp(&Entry->Description, &SamplerDescription, sizeof(ZEGRSamplerDescription)) == 0)
			return &(*Entry);
	}

	ZEGRSampler* Sampler = ZEGRGraphicsModule::GetInstance()->CreateSamplerState();
	if(!Sampler->Initialize(SamplerDescription))
	{
		delete Sampler;
		return NULL;
	}

	SamplerUniqueList.AddEnd(new ZELink<ZEGRSampler>(Sampler));

	return Sampler;
}
