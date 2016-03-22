//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELightPoint.cpp
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

#include "ZELightPoint.h"

#include "ZEGame/ZEScene.h"
#include "ZEGame/ZEEntityProvider.h"
#include "ZEGraphics/ZEGRTexture2D.h"

#define ZE_LDF_VIEW_TRANSFORM			1
#define ZE_LDF_PROJECTION_TRANSFORM		2
#define ZE_LDF_SHADOW_MAP				4
#define ZE_LDF_VIEW_VOLUME				8

ZELightType ZELightPoint::GetLightType() const
{
	return ZE_LT_POINT;
}

ZESize ZELightPoint::GetViewCount() const
{
	return 1;
}

const ZEViewVolume& ZELightPoint::GetViewVolume(ZESize Index) const
{
	if (DirtyFlags.GetFlags(ZE_LDF_VIEW_VOLUME))
	{
		ViewVolume.Create(GetWorldPosition(), GetRange(), 0.0f);
		DirtyFlags.UnraiseFlags(ZE_LDF_VIEW_VOLUME);
	}
	
	return ViewVolume;
}

ZEGRTexture* ZELightPoint::GetShadowMap(ZESize Index) const
{
	return NULL;
}

const ZEMatrix4x4& ZELightPoint::GetViewTransform(ZESize Index) const
{
	return ViewTransform;
}

const ZEMatrix4x4& ZELightPoint::GetProjectionTransform(ZESize Index) const
{
	return ProjectionTransform;
}

ZELightPoint::ZELightPoint()
{
	Command.Entity = this;
	Command.Priority = 2;
	Command.StageMask = ZERN_STAGE_DEBUG;
}

ZELightPoint::~ZELightPoint()
{

}

ZELightPoint* ZELightPoint::CreateInstance()
{
	return new ZELightPoint();
}
