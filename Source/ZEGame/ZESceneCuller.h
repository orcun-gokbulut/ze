//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESceneCuller.h
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
#ifndef __ZE_SCENE_CULLER_H__
#define __ZE_SCENE_CULLER_H__

#include "ZEDS/ZEArray.h"
#include "ZESceneDebugDraw.h"

class ZEScene;
class ZEEntity;
class ZELight;
class ZEDrawParameters;

struct ZECullStatistics
{
	size_t								TotalEntityCount;
	size_t								DrawableEntityCount;
	size_t								VisibleEntityCount;
	size_t								CulledEntityCount;
	size_t								DrawedEntityCount;

	size_t								TotalLightCount;
	size_t								VisibleLightCount;
	size_t								CulledLightCount;
	size_t								DrawedLightCount;
};

#define ZE_VDE_NONE									0
#define ZE_VDE_ENTITY_LOCAL_BOUNDING_BOX			1
#define ZE_VDE_ENTITY_WORLD_BOUNDING_BOX			2
#define ZE_VDE_LIGHT_RANGE							4
#define ZE_VDE_ALL									7

class ZESceneCuller
{
	private:
		ZECullStatistics				Statistics;

		ZESceneDebugDraw				DebugDraw;
		ZEDWORD							DebugDrawElements;

		void							DebugDrawEntity(ZEEntity* Entity, ZEDrawParameters* DrawParameters);
		void							DebugDrawLight(ZELight* Light, ZEDrawParameters* DrawParameters);

		bool							CullLight(ZELight* Light, ZEDrawParameters* DrawParameters);
		void							CullLights(ZEScene* Scene, ZEDrawParameters* DrawParameters);

		bool							CullEntity(ZEEntity* Entity, ZEDrawParameters* DrawParameters);
		void							CullEntities(ZEScene* Scene, ZEDrawParameters* DrawParameters);

	public:
		virtual const ZECullStatistics&	GetStatistics();

		void							SetDebugDrawElements(ZEDWORD Elements);
		ZEDWORD							SetDebugDrawElements();

		virtual void					CullScene(ZEScene* Scene, ZEDrawParameters* DrawParameters);

										ZESceneCuller();
										~ZESceneCuller();
};

#endif
