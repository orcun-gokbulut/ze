//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELensFlare.cpp
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

#include "ZELensFlare.h"
#include "ZEGame/ZEScene.h"
#include "ZEGraphics/ZECamera.h"
#include "ZEGame/ZEEntityProvider.h"
#include "ZEGame/ZEDrawParameters.h"
#include "ZEGraphics/ZESkyBoxMaterial.h"
#include "ZEGraphics/ZEDirectionalLight.h"
#include "ZETexture/ZETextureCubeResource.h"


#include <string.h>
#include "ZEWeather.h"

ZE_META_REGISTER_CLASS(ZEEntityProvider, ZELensFlare);

ZEDrawFlags ZELensFlare::GetDrawFlags() const
{
	return ZE_DF_DRAW;
}

bool ZELensFlare::InitializeSelf()
{
	if (!ZEEntity::InitializeSelf())
		return false;

	
	return true;
}

bool ZELensFlare::DeinitializeSelf()
{

	return ZEEntity::DeinitializeSelf();
}

void ZELensFlare::Draw(ZEDrawParameters* DrawParameters)
{
	if (DrawParameters->Pass == ZE_RP_SHADOW_MAP)
		return;

	// Find Sun

	ZEArray<ZEEntity*> Entities = ZEScene::GetInstance()->GetEntities(ZEWeather::Description());
	if (Entities.GetSize() == 0)
		return;

	ZEWeather* Weather = static_cast<ZEWeather*>(Entities[0]);
	ZEVector3 SunDirection = Weather->GetSunDirection();

	ZECamera* Camera = ZEScene::GetInstance()->GetActiveCamera();
	
	ZEVector4 SunDirectionScreen;
	ZEMatrix4x4::Transform(SunDirectionScreen, Camera->GetViewProjectionTransform(), ZEVector4(SunDirection, 1.0f));

	SunDirectionScreen.x /= SunDirectionScreen.w;
	SunDirectionScreen.y /= SunDirectionScreen.w;

	float Length = SunDirectionScreen.Length();
	



}

void ZELensFlare::Tick(float Time)
{

}

ZELensFlare::ZELensFlare()
{

}

ZELensFlare::~ZELensFlare()
{

}

ZELensFlare* ZELensFlare::CreateInstance()
{
	return new ZELensFlare();
}

ZEEntityRunAt ZELensFlareDescription::GetRunAt() const
{
	return ZE_ERA_BOTH;
}
