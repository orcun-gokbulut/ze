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
#include "ZEGraphics/ZELight.h"
#include "ZEGraphics/ZERenderer.h"
#include "ZEDrawParameters.h"
#include "ZEMath/ZEViewVolume.h"

void ZESceneCuller::DebugDrawEntity(ZEEntity* Entity, ZEDrawParameters* DrawParameters)
{
	if (DebugDrawElements & ZE_VDE_ENTITY_LOCAL_BOUNDING_BOX)
		DebugDraw.DrawOrientedBoundingBox(Entity->GetBoundingBox(), Entity->GetWorldTransform(), DrawParameters->Renderer, ZEVector4(1.0f, 1.0f, 1.0f, 1.0f));

	if (DebugDrawElements & ZE_VDE_ENTITY_WORLD_BOUNDING_BOX)
		DebugDraw.DrawAxisAlignedBoundingBox(Entity->GetWorldBoundingBox(), DrawParameters->Renderer, ZEVector4(0.75f, 0.75f, 1.0f, 1.0f));
}

void ZESceneCuller::DebugDrawLight(ZELight* Light, ZEDrawParameters* DrawParameters)
{
	if (DebugDrawElements & ZE_VDE_LIGHT_RANGE)
		DebugDraw.DrawBoundingSphere(ZEBSphere(Light->GetWorldPosition(), Light->GetRange()), DrawParameters->Renderer, ZEVector4(0.25f, 0.25f, 1.0f, 1.0f));
}

void ZESceneCuller::CullEntity(ZEEntity* Entity, ZEDrawParameters* DrawParameters)
{
	if (FrameStatisticsEnabled)
		Statistics.TotalEntityCount++;

	if (!Entity->GetVisible())
		return;

	ZEUInt32 EntityDrawFlags = Entity->GetDrawFlags();

	if ((EntityDrawFlags & ZE_DF_DRAW) == ZE_DF_DRAW)
	{
		if (FrameStatisticsEnabled)
			Statistics.DrawableEntityCount++;

		if ((EntityDrawFlags & ZE_DF_CULL) == ZE_DF_CULL)
		{
			if (DrawParameters->ViewVolume->CullTest(Entity->GetWorldBoundingBox()))
			{
				if (FrameStatisticsEnabled)
					Statistics.CulledEntityCount++;
			}
			else
			{
				if (FrameStatisticsEnabled)
					Statistics.DrawedEntityCount++;

				Entity->Draw(DrawParameters);

				if (CurrentRendererType == ZE_RT_FRAME)
					DebugDrawEntity(Entity, DrawParameters);
			}
		}
		else
		{
			if (FrameStatisticsEnabled)
				Statistics.DrawedEntityCount++;

			Entity->Draw(DrawParameters);

			if (CurrentRendererType == ZE_RT_FRAME)
				DebugDrawEntity(Entity, DrawParameters);
		}
	}

	if ((EntityDrawFlags & ZE_DF_LIGHT_SOURCE) == ZE_DF_LIGHT_SOURCE)
	{
		if (CurrentRendererType == ZE_RT_FRAME)
		{
			Entity->Draw(DrawParameters);
			DebugDrawLight((ZELight*)Entity, DrawParameters);
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
}

const ZECullStatistics& ZESceneCuller::GetStatistics()
{
	return Statistics;
}

void ZESceneCuller::SetDebugDrawElements(ZEUInt32 Elements)
{
	this->DebugDrawElements = Elements;
}

ZEUInt32 ZESceneCuller::SetDebugDrawElements()
{
	return this->DebugDrawElements;
}

void ZESceneCuller::CullScene(ZEScene* Scene, ZEDrawParameters* DrawParameters)
{
	if (DebugDrawElements == ZE_VDE_NONE)
		DebugDraw.Deinitialize();
	else
		DebugDraw.Initialize();

	CurrentRendererType = DrawParameters->Renderer->GetRendererType();

	if (CurrentRendererType == ZE_RT_FRAME)
		FrameStatisticsEnabled = true;
	else
		FrameStatisticsEnabled = false;

	if (FrameStatisticsEnabled)
		memset(&Statistics, 0, sizeof(ZECullStatistics));

	if (CurrentRendererType == ZE_RT_FRAME)
		DebugDraw.Clean();

	CullEntities(Scene, DrawParameters);
	
	DebugDraw.Draw(DrawParameters->Renderer);
}

ZESceneCuller::ZESceneCuller()
{
	memset(&Statistics, 0, sizeof(ZECullStatistics));
	DebugDrawElements = ZE_VDE_NONE;
	FrameStatisticsEnabled = true;
	CurrentRendererType = ZE_RT_FRAME;
}

ZESceneCuller::~ZESceneCuller()
{
	DebugDraw.Deinitialize();
}
