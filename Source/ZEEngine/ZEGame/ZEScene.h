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

#include "ZEInitializable.h"
#include "ZEDestroyable.h"

#include "ZETypes.h"
#include "ZEDS/ZEArray.h"
#include "ZEPointer/ZEHolder.h"
#include "ZESpatial/ZEOctree.h"
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
class ZERNRenderer;
class ZEGRBuffer;
class ZERNPreRenderParameters;

class ZEScene : public ZEObject, public ZEInitializable, public ZEDestroyable
{
	ZE_OBJECT
	friend class ZEEntity;
	friend class ZERNRenderer;
	private:
		ZEFlags									SceneDirtyFlags;
		ZEUInt									LastEntityId;
		ZESmartArray<ZEEntity*>					Entities;
		ZEPhysicalWorld*						PhysicalWorld;
		ZECamera*								ActiveCamera;
		ZEListener*								ActiveListener;
		bool									Enabled;
		float									AmbientFactor;
		ZEVector3								AmbientColor;
		ZELock									SceneLock;
		bool									SpatialDatabase;

		ZEList2<ZEEntity>						TickList;
		ZEList2<ZEEntity>						RenderList;
		ZEOctree<ZEEntity*>						RenderListOctree;

		struct
		{
			ZEVector3							AmbientColor;
			float								Reserved0;
		} Constants;

		ZEHolder<ZEGRBuffer>					ConstantBuffer;

		void									AddToTickList(ZEEntity* Entity);
		void									RemoveFromTickList(ZEEntity* Entity);

		void									AddToRenderList(ZEEntity* Entity);
		void									RemoveFromRenderList(ZEEntity* Entity);

		void									UpdateOctree(ZEEntity* Entity);

		void									TickEntity(ZEEntity* Entity, float ElapsedTime);
		bool									PreRenderEntity(ZEEntity* Entity, const ZERNPreRenderParameters* Parameters);
		void									RayCastEntity(ZEEntity* Entity, ZERayCastReport& Report, const ZERayCastParameters& Parameters);
		
		ZEEntity*								GetEntityInternal(ZEEntity* ParentEntity, const ZEString& Name);
		ZEArray<ZEEntity*>						GetEntitiesInternal(ZEEntity* ParentEntity, ZEClass* Class);

		bool									InitializeInternal();
		bool									DeinitializeInternal();

		void									UpdateConstantBuffer();

												ZEScene();
		virtual									~ZEScene();

	public:
		const ZEGRBuffer*						GetConstantBuffer();
		ZEPhysicalWorld*						GetPhysicalWorld();

		void									SetEnabled(bool Enabled);
		bool									GetEnabled() const;

		void									SetAmbientFactor(float Factor);
		float									GetAmbientFactor() const;

		void									SetAmbientColor(ZEVector3 Color);
		const ZEVector3&						GetAmbientColor() const;

		void									SetSpatialDatabase(bool Enabled);
		bool									GetSpatialDatabase();

		ZEUInt									GetLoadingPercentage();

		const ZESmartArray<ZEEntity*>&			GetEntities();
		ZEArray<ZEEntity*>						GetEntities(ZEClass* Class, bool Recursive = false);
		ZEArray<ZEEntity*>						GetEntities(const ZEString& Name, bool Recursive = false);
		ZEEntity*								GetEntity(ZEClass* Class, bool Recursive = false);
		ZEEntity*								GetEntity(const ZEString& Name, bool Recursive = false);
		void									AddEntity(ZEEntity* Entity);
		void									RemoveEntity(ZEEntity* Entity);
		void									ClearEntities();

		bool									IsLoaded();
		bool									IsInitialized();

		void									Tick(float ElapsedTime);
		void									PreRender(const ZERNPreRenderParameters* Parameters);
		void									RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters);

		bool									Serialize(const ZEString& FileName);
		bool									Unserialize(const ZEString& FileName);

		void									LockScene();
		void									UnlockScene();

		static ZEScene*							CreateInstance();
}
ZE_META_ATTRIBUTE(ZEDEditor.ObjectWrapper.Icon, "#R:/ZEDEditor/Icons/ZEDObjectWrapper/ZEScene.png")
ZE_META_ATTRIBUTE(ZEDEditor.ObjectWrapper3D.Pickable, false)
ZE_META_ATTRIBUTE(ZEDEditor.ObjectWrapper3D.BoundingBoxVisible, false)
ZE_META_ATTRIBUTE(ZEDEditor.ObjectWrapper3D.NameplateVisible, false);
