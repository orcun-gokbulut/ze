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
#include "ZECompoundEntity.h"
#include "ZEGraphics/ZELight.h"
#include "ZEGraphics/ZERenderer.h"
#include "ZEDrawParameters.h"
#include "ZEGraphics/ZEViewVolume.h"

bool ZESceneCuller::CullLight(ZELight* Light, ZEDrawParameters* DrawParameters)
{
	Statistics.TotalLightCount++;

	if (!Light->GetVisible())
		return false;

	Statistics.VisibleLightCount++;

	if (DrawParameters->ViewVolume->LightCullTest(Light))
	{
		Statistics.CulledLightCount++;
		return false;
	}

	Statistics.DrawedLightCount++;	
	
	return true;
}

void ZESceneCuller::CullLights(ZEScene* Scene, ZEDrawParameters* DrawParameters)
{
	ZESmartArray<ZELight*> VisibleLights;
	ZESmartArray<ZEEntity*> Entities = Scene->GetEntities();

	for (size_t I = 0; I < Entities.GetCount(); I++)
	{
		if (ZEClassDescription::CheckParent(ZELight::ClassDescription(), Entities[I]->GetClassDescription()))
		{
			if (CullLight((ZELight*)Entities[I], DrawParameters))
			{
				DrawParameters->Renderer->AddToLightList((ZELight*)Entities[I]);

				/*if (VisualDebugElements & ZE_VDE_LIGHT_RANGE)
					DebugDraw.DrawBoundingSphere(ZEBoundingSphere(Light->GetWorldPosition(), ((ZELight*)Light)->GetRange()), Renderer, ZEVector4(0.25f, 0.25f, 1.0f, 1.0f));*/
			}
		}


		if (Entities[I]->GetEntityType() == ZE_ET_COMPOUND)
		{
			const ZEArray<ZEComponent*>& Components = ((ZECompoundEntity*)Entities[I])->GetComponents();

			for (size_t N = 0; N < Components.GetCount(); N++)
			{
				if (ZEClassDescription::CheckParent(ZELight::ClassDescription(), Components[N]->GetClassDescription()))
					if (CullLight((ZELight*)Components[N], DrawParameters))
					{
						DrawParameters->Renderer->AddToLightList((ZELight*)Entities[I]);

						/*if (VisualDebugElements & ZE_VDE_LIGHT_RANGE)
						DebugDraw.DrawBoundingSphere(ZEBoundingSphere(Light->GetWorldPosition(), ((ZELight*)Light)->GetRange()), Renderer, ZEVector4(0.25f, 0.25f, 1.0f, 1.0f));*/
					}
			}
		}
	}
}

bool ZESceneCuller::CullEntity(ZEEntity* Entity, ZEDrawParameters* DrawParameters)
{
	Statistics.TotalEntityCount++;

	ZEDWORD EntityDrawFlags = Entity->GetDrawFlags();
	if (EntityDrawFlags & ZE_DF_DRAW == false)
		return false;

	Statistics.DrawableEntityCount++;

	if (EntityDrawFlags & ZE_DF_CULL && DrawParameters->ViewVolume->CullTest(Entity))
	{
		Statistics.CulledEntityCount++;
		return false;
	}

	Statistics.DrawedEntityCount++;

	return true;
}

void ZESceneCuller::CullEntities(ZEScene* Scene, ZEDrawParameters* DrawParameters)
{
	ZESmartArray<ZEEntity*> Entities = Scene->GetEntities();

	for (size_t I = 0; I < Entities.GetCount(); I++)
	{
		if (CullEntity(Entities[I], DrawParameters))
		{
			Entities[I]->Draw(DrawParameters);

			/*if (VisualDebugElements & ZE_VDE_ENTITY_ORIENTED_BOUNDINGBOX)
			DebugDraw.DrawOrientedBoundingBox(Entity->GetLocalBoundingBox(), Components[N]->GetWorldTransform(), Renderer, ZEVector4(1.0f, 1.0f, 1.0f, 1.0f));

			if (VisualDebugElements & ZE_VDE_ENTITY_AXISALIGNED_BOUNDINGBOX)
			DebugDraw.DrawAxisAlignedBoundingBox(Components[N]->GetWorldBoundingBox(), Renderer, ZEVector4(0.5f, 0.5f, 0.5f, 1.0f));*/
		}

		if (Entities[I]->GetEntityType() == ZE_ET_COMPOUND)
		{
			const ZEArray<ZEComponent*> Components = ((ZECompoundEntity*)Entities[I])->GetComponents();

			for (size_t N = 0; N < Components.GetCount(); N++)
				if (CullEntity(Components[N], DrawParameters))
				{
					Components[N]->Draw(DrawParameters);

					/*if (VisualDebugElements & ZE_VDE_ENTITY_ORIENTED_BOUNDINGBOX)
						DebugDraw.DrawOrientedBoundingBox(Entity->GetLocalBoundingBox(), Components[N]->GetWorldTransform(), Renderer, ZEVector4(1.0f, 1.0f, 1.0f, 1.0f));

					if (VisualDebugElements & ZE_VDE_ENTITY_AXISALIGNED_BOUNDINGBOX)
						DebugDraw.DrawAxisAlignedBoundingBox(Components[N]->GetWorldBoundingBox(), Renderer, ZEVector4(0.5f, 0.5f, 0.5f, 1.0f));*/
				}
		}
	}
}

const ZECullStatistics& ZESceneCuller::GetStatistics()
{
	return Statistics;
}

void ZESceneCuller::CullScene(ZEScene* Scene, ZEDrawParameters* DrawParameters)
{
	memset(&Statistics, 0, sizeof(ZECullStatistics));

	if (DrawParameters->Renderer->GetRendererType() == ZE_RT_FRAME)
		CullLights(Scene, DrawParameters);

	CullEntities(Scene, DrawParameters);
}
