//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEScene.h
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

#include "ZEDS/ZEArray.h"
#include "ZESceneDebugDraw.h"
#include "ZESceneCuller.h"
#include "ZETypes.h"
#include "ZEDrawParameters.h"
#include "ZEMeta/ZEObject.h"
#include "ZERayCast.h"

ZE_META_FORWARD_DECLARE(ZEEntity,			"ZEEntity.h")
ZE_META_FORWARD_DECLARE(ZECamera,			"ZEGraphics/ZECamera.h")
ZE_META_FORWARD_DECLARE(ZEFrameRenderer,	"ZEGraphics/ZEFrameRenderer.h")
ZE_META_FORWARD_DECLARE(ZEShadowRenderer,	"ZEGraphics/ZEShadowRenderer.h")
ZE_META_FORWARD_DECLARE(ZEListener,			"ZESound/ZEListener.h")
ZE_META_FORWARD_DECLARE(ZEInterior,			"ZEInterior/ZEInterior.h")
ZE_META_FORWARD_DECLARE(ZEInteriorResource,	"ZEInterior/ZEInteriorResource.h")
ZE_META_FORWARD_DECLARE(ZEPostProcessor,	"ZEPostProcessor/ZEPostProcessor.h")

class ZEPhysicalWorld;

#define ZE_RCF_ENTITY							1
#define ZE_RCF_COMPONENT						2
#define ZE_RCF_MAP								4
#define ZE_RCF_POSITON							8
#define ZE_RCF_NORMAL							16

/*#define zeScene ZEScene::GetInstance()*/

class ZEScene : public ZEObject
{
	ZE_OBJECT

	private:
		bool									Initialized;

		ZEUInt									LastEntityId;

		ZESceneCuller							Culler;

		ZESmartArray<ZEEntity*>					Entities;

		ZEPhysicalWorld*						PhysicalWorld;

		ZEDrawParameters						FrameDrawParameters;

		ZEFrameRenderer*						Renderer;
		ZEShadowRenderer*						ShadowRenderer;
		ZEPostProcessor*						PostProcessor;
		ZECamera*								ActiveCamera;
		ZEListener*								ActiveListener;

		bool									Enabled;
		float									AmbientFactor;
		ZEVector3								AmbientColor;

		void									Tick(ZEEntity* Entity, float ElapsedTime);

	protected:
												ZEScene();
		virtual									~ZEScene();

	public:
		virtual void							AddEntity(ZEEntity* Entity);
		virtual void							RemoveEntity(ZEEntity* Entity);

		const ZESmartArray<ZEEntity*>&			GetEntities();
		ZEArray<ZEEntity*>						GetEntities(ZEClass* Class);

		void									ClearEntities();

		ZERenderer*								GetRenderer();
		ZERenderer*								GetShadowRenderer();
		ZEPhysicalWorld*						GetPhysicalWorld();

		void									SetActiveCamera(ZECamera* Camera);
		ZECamera*								GetActiveCamera();

		void									SetActiveListener(ZEListener* Listener);
		ZEListener*								GetActiveListener();

		void									SetEnabled(bool Enabled);
		bool									GetEnabled() const;

		ZESceneCuller&							GetSceneCuller();
		const ZESceneStatistics&				GetStatistics() const;

		virtual bool							Save(const ZEString& FileName);
		virtual bool							Load(const ZEString& FileName);

		virtual bool							Initialize();
		virtual void							Deinitialize();
		virtual void							Destroy();

		virtual void							Tick(float ElapsedTime);
		virtual void							Render(float ElapsedTime);
		virtual bool							RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters);

		void									SetAmbientFactor(float Factor);
		float									GetAmbientFactor() const;

		void									SetAmbientColor(ZEVector3 Color);
		const ZEVector3&						GetAmbientColor() const;

		static ZEScene*							CreateInstance();
};

#endif
