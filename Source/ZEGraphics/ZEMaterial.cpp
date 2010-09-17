//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMaterial.cpp
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

#include "ZEMaterial.h"
#include "ZECore\ZECore.h"
#include "ZEGraphicsModule.h"
#include "ZEMeta\ZEAnimation.h"
#include <memory.h>

ZEMaterial::ZEMaterial()
{
}

ZEMaterial::~ZEMaterial()
{
}

bool ZEMaterial::SetupPreLightning() const
{
	return false;
}

size_t ZEMaterial::DoPreLightningPass() const
{
	return 0;
}

bool ZEMaterial::SetupLightning() const
{
	return false;
}

bool ZEMaterial::SetupPointLightPass(bool Shadowed) const
{
	return false;
}

size_t ZEMaterial::DoPointLightPass(ZEPointLight** Lights, size_t Count) const
{
	return 0;
}

bool ZEMaterial::SetupDirectionalLightPass(bool Shadowed) const
{
	return false;
}

size_t ZEMaterial::DoDirectionalLightPass(ZEDirectionalLight** Lights, size_t Count) const
{
	return 0;
}

bool ZEMaterial::SetupProjectiveLightPass(bool Shadowed) const
{
	return false;
}

size_t ZEMaterial::DoProjectiveLightPass(ZEProjectiveLight** Lights, size_t Count) const
{
	return 0;
}

bool ZEMaterial::SetupOmniProjectiveLightPass(bool Shadowed) const
{
	return false;
}

size_t ZEMaterial::DoOmniProjectivePass(ZEOmniProjectiveLight** Lights, size_t Count) const
{
	return 0;
}

bool ZEMaterial::SetupCustomPass(unsigned int CustomPassId) const
{
	return false;
}

bool ZEMaterial::DoCustomPass(unsigned int CustomPassId, void* CustomData) const
{
	return false;
}

bool ZEMaterial::SetupShadowPass() const
{
	return false;
}

size_t ZEMaterial::DoShadowPass() const
{
	return 0;
}

void ZEMaterial::EndOfPasses() const
{

}

void ZEMaterial::UpdateMaterial()
{

}

void ZEMaterial::AdvanceAnimation(float TimeElapsed)
{
	if (GetAnimationController() != NULL)
		GetAnimationController()->AdvanceAnimation(TimeElapsed);
}

void ZEMaterial::Destroy()
{
	delete this;
}

#include "ZEMaterial.h.zpp"
