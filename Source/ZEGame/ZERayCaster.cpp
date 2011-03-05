//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERayCaster.cpp
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

#include "ZERayCaster.h"
#include "ZEEntity.h"
#include "ZEScene.h"

ZEEntity* ZERayCaster::CastRay(ZEScene* Scene, const ZERay& Ray, float Range)
{
	float T;
	ZEVector3 Position, Normal;
	return ZERayCaster::CastRay(Scene, Ray, T, Position, Normal, Range);
}

ZEEntity* ZERayCaster::CastRay(ZEScene* Scene, const ZERay& Ray, float& T, float Range)
{
	ZEVector3 Position, Normal;
	return ZERayCaster::CastRay(Scene, Ray, T, Position, Normal, Range);
}

ZEEntity* ZERayCaster::CastRay(ZEScene* Scene, const ZERay& Ray, ZEVector3& Position, float Range)
{
	ZEVector3 Normal;
	return ZERayCaster::CastRay(Scene, Ray, Position, Normal, Range);

}

ZEEntity* ZERayCaster::CastRay(ZEScene* Scene, const ZERay& Ray, ZEVector3& Position, ZEVector3& Normal, float Range)
{
	float MinT, MaxT;
	float CurrentT = Range / Ray.v.Length();

	ZEEntity* IntersectedEntity = NULL;

	const ZESmartArray<ZEEntity*>& Entities = Scene->GetEntities();

	for (size_t I = 0; I < Entities.GetCount(); I++)
	{
		ZEEntity* CurrentEntity = Entities[I];

		ZEDWORD RayCastFlags = CurrentEntity->GetRayCastFlags();
		if (RayCastFlags == ZE_RCF_BOUNDING_BOX)
		{
			if (!ZEAABoundingBox::IntersectionTest(CurrentEntity->GetWorldBoundingBox(), Ray, MinT, MaxT))
				continue;

			if (CurrentT < MinT)
				continue;

			Position = Ray.GetPointOn(CurrentT);
			Normal = -Ray.v;

			IntersectedEntity = CurrentEntity;
			MinT = CurrentT;
		}
		else if (RayCastFlags & ZE_RCF_INTERNAL)
		{
			if (RayCastFlags & ZE_RCF_BOUNDING_BOX)
			{
				if (!ZEAABoundingBox::IntersectionTest(CurrentEntity->GetWorldBoundingBox(), Ray, MinT, MaxT))
					continue;

				if (CurrentT < MaxT)
					continue;
			}

			if (!CurrentEntity->CastRay(Ray, CurrentT, Position, Normal))
				continue;

			if (MinT < CurrentT)
				continue;

			IntersectedEntity = CurrentEntity;
			MinT = CurrentT;
		}
	}

	return IntersectedEntity;
}

