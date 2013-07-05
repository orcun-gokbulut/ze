//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESceneCuller.cpp
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

#include "ZESceneCuller.h"
#include "ZEScene.h"
#include "ZEEntity.h"
#include "ZERenderer/ZELight.h"
#include "ZERenderer/ZERenderer.h"
#include "ZEDrawParameters.h"
#include "ZEMath/ZEViewVolume.h"


void ZESceneCuller::CullEntity(ZEEntity* Entity, ZEDrawParameters* DrawParameters)
{
	ZEUInt32 EntityDrawFlags = Entity->GetDrawFlags();

	if (DrawParameters->Pass == ZE_RP_COLOR)
	{
		Statistics.TotalEntityCount++;

		if ((EntityDrawFlags & ZE_DF_LIGHT_SOURCE) == ZE_DF_LIGHT_SOURCE)
			Statistics.TotalLightCount++;
	}

	if (!Entity->GetVisible())
		return;

	if ((EntityDrawFlags & ZE_DF_DRAW) == ZE_DF_DRAW)
	{
		if (DrawParameters->Pass == ZE_RP_COLOR)
			Statistics.DrawableEntityCount++;

		if ((EntityDrawFlags & ZE_DF_CULL) == ZE_DF_CULL)
		{
			if (DrawParameters->ViewVolume->CullTest(Entity->GetWorldBoundingBox()))
			{
				if (DrawParameters->Pass == ZE_RP_COLOR)
					Statistics.CulledEntityCount++;
			}
			else
			{
				if (DrawParameters->Pass == ZE_RP_COLOR)
					Statistics.DrawedEntityCount++;

				Entity->Draw(DrawParameters);
			}
		}
		else
		{
			if (DrawParameters->Pass == ZE_RP_COLOR)
				Statistics.DrawedEntityCount++;

			Entity->Draw(DrawParameters);
		}
	}

	const ZEArray<ZEEntity*>& Components = Entity->GetComponents();
	for (ZESize I = 0; I < Components.GetCount(); I++)
		CullEntity(Components[I], DrawParameters);

	const ZEArray<ZEEntity*>& ChildEntities = Entity->GetChildEntities();
	for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
		CullEntity(ChildEntities[I], DrawParameters);

	return;
}

void ZESceneCuller::CullEntities(ZEScene* Scene, ZEDrawParameters* DrawParameters)
{
	const ZESmartArray<ZEEntity*>& Entities = Scene->GetEntities();

	for (ZESize I = 0; I < Entities.GetCount(); I++)
		CullEntity(Entities[I], DrawParameters);

	if (DrawParameters->Pass == ZE_RP_COLOR)
	{
		//Statistics.DrawedLightCount = (ZEUInt32)DrawParameters->Lights.GetCount();
		Statistics.CulledLightCount = Statistics.TotalLightCount - Statistics.DrawedLightCount;
	}

}

void ZESceneCuller::CullScene(ZEScene* Scene, ZEDrawParameters* DrawParameters)
{
	if (DrawParameters->Pass == ZE_RP_COLOR)
		memset(&Statistics, 0, sizeof(ZESceneStatistics));

	CullEntities(Scene, DrawParameters);

	if (DrawParameters->Pass == ZE_RP_COLOR)
		DrawParameters->Statistics.SceneStatistics = Statistics;
}

const ZESceneStatistics& ZESceneCuller::GetStatistics() const
{
	return Statistics;
}

ZESceneCuller::ZESceneCuller()
{
	memset(&Statistics, 0, sizeof(ZESceneStatistics));
}

ZESceneCuller::~ZESceneCuller()
{
	
}
