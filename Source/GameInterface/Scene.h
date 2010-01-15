//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Scene.h
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
#ifndef __ZE_SCENE_H__
#define __ZE_SCENE_H__

#include "ZEDS/Array.h"
#include "Entity.h"
#include "Map/Map.h"
#include "Portal.h"
#include "Graphics/FixedMaterial.h"

#define ZE_RCF_ENTITY								1
#define ZE_RCF_COMPONENT							2
#define ZE_RCF_MAP									4
#define ZE_RCF_POSITON								8
#define ZE_RCF_NORMAL								16

#define ZE_VDE_NONE									0
#define ZE_VDE_ENTITY_ORIENTED_BOUNDINGBOX			1
#define ZE_VDE_ENTITY_AXISALIGNED_BOUNDINGBOX		2
#define ZE_VDE_ENTITY_BOUNDINGSPHERE				8
#define ZE_VDE_ENTITY_DEBUGDRAW						16
#define ZE_VDE_COMPONENT_ORIENTED_BOUNDINGBOX		32
#define ZE_VDE_COMPONENT_AXISALIGNED_BOUNDINGBOX	64
#define ZE_VDE_COMPONENT_BOUNDINGSPHERE				128
#define ZE_VDE_COMPONENT_DEBUGDRAW					256
#define ZE_VDE_LIGHT_RANGE							512
#define ZE_VDE_ALL									1023

#define zeScene ZEScene::GetInstance()

class ZESceneBridge
{
	virtual void*								GetScene() = 0;
	virtual void*								OnEntityChanged(ZEEntity* Entity) = 0;
};

class ZEViewVolume;
class ZEBoundingBox;
class ZECamera;
class ZEListener;
class ZEShadowRenderer;
class ZEPostProcessor;
class ZEFixedMaterial;
class ZEEntity;
class ZEScene
{
	private:
		ZEDWORD									VisualDebugElements;

		void									OptionsChanged();

		unsigned int							LastEntityId;

		ZEPortalMap								Environment;
		ZESmartArray<ZEEntity*>					Entities;
	
		ZERenderer*								Renderer;
		ZEPostProcessor*						PostProcessor;
		ZEShadowRenderer*						ShadowRenderer;
		ZECamera*								ActiveCamera;
		ZEListener*								ActiveListener;

		ZEFixedMaterial*						EntityOrientedBoundingBoxMaterial;
		ZEFixedMaterial*						EntityAxisAlignedBoundingBoxMaterial;
		ZEFixedMaterial*						EntityBoundingSphereMaterial;
		ZEFixedMaterial*						ComponentOrientedBoundingBoxMaterial;
		ZEFixedMaterial*						ComponentAxisAlignedBoundingBoxMaterial;
		ZEFixedMaterial*						ComponentBoundingSphereMaterial;
		ZEFixedMaterial*						LightRangeMaterial;

		ZERenderOrder							BoundingBoxRenderOrder;
		ZERenderOrder							BoundingSphereRenderOrder;
		
		void									DrawOrientedBoundingBox(const ZEAABoundingBox& BoundingBox, const ZEMatrix4x4& Transform, ZERenderer* Renderer, ZEMaterial* Material);
		void									DrawAxisAlignedBoundingBox(const ZEAABoundingBox& BoundingBox, ZERenderer* Renderer, ZEMaterial* Material);
		void									DrawBoundingSphere(const ZEBoundingSphere& BoundingSphere, ZERenderer* Renderer, ZEMaterial* Material);

	public:
		void									SetVisualDebugElements(ZEDWORD VisualDebugElements);
		ZEDWORD									GetVisualDebugElements();

		bool									Initialize();
		bool									Deinitialize();
		void									Reset();
		void									Destroy();

		void									AddEntity(ZEEntity* Entity);
		void									RemoveEntity(ZEEntity* Entity);
		const ZESmartArray<ZEEntity*>&			GetEntities();

		void									SetActiveCamera(ZECamera* Camera);
		ZECamera*								GetActiveCamera();

		void									SetActiveListener(ZEListener* Listener);
		ZEListener*								GetActiveListener();

		void									Tick(float ElapsedTime);
		void									Render(float ElapsedTime);

		virtual ZEEntity*						CastRay(const ZERay& Ray, float Range);
		virtual bool							CastRay(const ZERay& Ray, float Range, ZEEntity** IntersectedEntity, ZEVector3& Position, ZEVector3& Normal);

		virtual void							CullScene(ZERenderer* Renderer, const ZEViewVolume& ViewVolume, bool LightsEnabled = true);
		bool									LoadEnvironment(const char* FileName);

		bool									Save(const char* FileName);
		bool									Load(const char* FileName);

												ZEScene();
		virtual									~ZEScene();

		static ZEScene*							GetInstance();
};

class ZEPortalScene : public ZEScene
{
};

#endif
