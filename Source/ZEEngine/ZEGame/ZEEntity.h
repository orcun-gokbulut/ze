//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEEntity.h
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

#include "ZETypes.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEString.h"
#include "ZEDS/ZEFlags.h"
#include "ZEDefinitions.h"
#include "ZEDS/ZEValue.h"
#include "ZEMath/ZEOBBox.h"
#include "ZEMath/ZEAABBox.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEBSphere.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZERayCast.h"

#include "ZEMeta/ZEObject.h"
#include "ZEMeta/ZEEnumerator.h"
#include "ZEDS/ZELink.h"
#include "ZEDS/ZEList2.h"

struct ZERNDrawParameters;
struct ZERNCullParameters;
class ZERNRenderParameters;
class ZERNCommand;


// ZEDrawFlags
typedef ZEFlags ZEEntityFlags;
#define ZE_EF_TICKABLE		0x01
#define ZE_EF_RENDERABLE	0x02
#define ZE_EF_CULLABLE		0x03


ZE_ENUM(ZEEntityState)
{
	ZE_ES_NOT_INITIALIZED				= 0,
	ZE_ES_INITIALIZING					= 1,
	ZE_ES_INITIALIZED					= 2,
	ZE_ES_DEINITIALIZING				= 3
};

ZE_ENUM(ZEEntityLoadingState)
{
	ZE_ELS_NOT_LOADED				= 0,
	ZE_ELS_LOADING					= 1,
	ZE_ELS_LOADED					= 2,
	ZE_ELS_SEMI_LOADED				= 3,
	ZE_ELS_UNLOADING				= 4
};

class ZEScene;
class ZERSResource;
class ZEMLWriterNode;
class ZEMLReaderNode;

class ZEEntity : public ZEObject
{
	ZE_OBJECT
	friend class ZEScene;
	friend class ZESceneCuller;
	friend class ZEDebugDrawer;
	private: 
		ZEEntity*								Parent;
		ZELink<ZEEntity>						ParentLink;
		ZEScene*								Scene;
		ZEEntityFlags							EntiyFlags;

		ZEString								Name;
		ZEInt									EntityId;
		ZEVector3								Position;
		ZEQuaternion							Rotation;
		ZEVector3								Scale;

		ZEEntityState							State;
		ZEEntityLoadingState					LoadingState;

		bool									Enabled;
		bool									Visible;
		bool									Tickable;
		bool									Renderable;

		mutable ZEFlags							EntityDirtyFlags;

		mutable ZEMatrix4x4						Transform;
		mutable ZEMatrix4x4						InvTransform;
		mutable ZEMatrix4x4						WorldTransform;
		mutable ZEMatrix4x4						InvWorldTransform;

		mutable ZEAABBox						BoundingBox;
		mutable ZEAABBox						WorldBoundingBox;

		ZEList2<ZEEntity>						Components;
		ZEList2<ZEEntity>						ChildEntities;
		
		ZEArray<ZEHolder<const ZERSResource>>	Resources;
	
		void									ParentChanged();
		void									TransformChangedLocal();
		void									TransformChangedWorld();

	protected:
		void									SetTickable(bool Tickable);
		void									SetRenderable(bool Tickable);

		const ZEList2<ZEEntity>&				GetComponents() const;
		virtual bool							AddComponent(ZEEntity* Entity); 
		void									RemoveComponent(ZEEntity* Entity);

		void									RegisterResource(ZEHolder<const ZERSResource> Resource);
		void									UnregisterResource(ZEHolder<const ZERSResource> Resource);

		virtual bool							InitializeSelf();
		virtual void							DeinitializeSelf();

		virtual bool							LoadSelf();
		virtual void							UnloadSelf();

												ZEEntity();
		virtual									~ZEEntity();

	public:
		virtual ZEEntity*						GetParent() const;
		ZEScene*								GetScene() const;
		ZEEntityState							GetInitalizationState() const;

		ZEEntityFlags							GetFlags() const;

		void									SetEntityId(ZEInt EntityId);
		ZEInt									GetEntityId() const;

		void									SetName(ZEString NewName);
		ZEString								GetName() const;

		virtual void							SetPosition(const ZEVector3& NewPosition);
		const ZEVector3&						GetPosition() const;

		virtual void							SetRotation(const ZEQuaternion& NewRotation);
		const ZEQuaternion&						GetRotation() const;

		virtual void							SetScale(const ZEVector3& NewScale);
		const ZEVector3&						GetScale() const;

		virtual void							SetEnabled(bool Enabled);
		bool									GetEnabled() const;

		virtual void							SetVisible(bool Visibility);
		bool									GetVisible() const;

		void									SetBoundingBox(const ZEAABBox& BoundingBox);
		const ZEAABBox&							GetBoundingBox() const;

		const ZEMatrix4x4&						GetTransform() const;
		const ZEMatrix4x4&						GetInvTransform() const;
		const ZEMatrix4x4&						GetWorldTransform() const;
		const ZEMatrix4x4&						GetInvWorldTransform() const;

		ZEVector3								GetFront();
		ZEVector3								GetRight();
		ZEVector3								GetUp();

		void									SetWorldPosition(const ZEVector3& NewPosition);
		const ZEVector3							GetWorldPosition() const;

		void									SetWorldRotation(const ZEQuaternion& NewRotation);
		const ZEQuaternion						GetWorldRotation() const;

		void									SetWorldScale(const ZEVector3& NewScale);
		const ZEVector3							GetWorldScale() const;

		const ZEAABBox&							GetWorldBoundingBox() const;

		ZEVector3								GetWorldFront() const;
		ZEVector3								GetWorldRight() const;
		ZEVector3								GetWorldUp() const;

		const ZEArray<ZEEntity*>&				GetChildEntities() const;
		bool									AddChildEntity(ZEEntity* Entity);
		void									RemoveChildEntity(ZEEntity* Entity);

		bool									IsInitialized();
		bool									Initialize();
		void									Deinitialize();
		
		virtual bool							Load();
		virtual void							Unload();

		virtual bool							Save(ZEMLWriterNode* Serializer);
		virtual bool							Restore(ZEMLReaderNode* Unserializer);

		virtual void							Tick(float Time);

		virtual bool							PreRender(const ZERNCullParameters* CullParameters);
		virtual void							Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command);

		virtual bool							RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters);

		virtual void							Destroy();

};
