//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEInteriorRoom.h
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

#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEFlags.h"
#include "ZEPointer/ZEHolder.h"
#include "ZESpatial/ZEOctree.h"
#include "ZEGame/ZERayCast.h"
#include "ZEGraphics/ZEGRCanvas.h"
#include "ZERenderer/ZERNCommand.h"

ZEMT_FORWARD_DECLARE(ZEInterior);
ZEMT_FORWARD_DECLARE(ZEInteriorDoor);

struct ZEInteriorResourceRoom;
class ZEPhysicalMesh;
class ZEGRBuffer;
class ZEGRBuffer;
class ZERNMaterial;
class ZERNPreRenderParameters;
class ZERNRenderParameters;

typedef ZEFlags ZEInteriorRoomDirtyFlags;

struct ZEInteriorRoomDraw
{
	ZEUInt										VertexOffset;
	ZEUInt										VertexCount;
	ZEHolder<const ZERNMaterial>				Material;
	ZERNCommand									RenderCommand;
};

class ZEInteriorRoom : public ZEObject
{
	ZE_OBJECT
	friend class ZEInteriorDoor;
	friend class ZEInterior;
	private:
		ZEInterior*								Owner;
		const ZEInteriorResourceRoom*			Resource;
		ZEPhysicalMesh*							PhysicalMesh;
		ZEArray<ZEInteriorDoor*>				Doors;
		ZESmartArray<ZEInteriorRoomDraw>		Draws;

		ZEHolder<ZEGRBuffer>					VertexBuffer;
		ZEHolder<ZEGRBuffer>					ConstantBuffer;

		bool									CullPass;
		bool									IsDrawn;
		bool									IsPersistentDraw;

		mutable ZEInteriorRoomDirtyFlags		DirtyFlags;

		ZEAABBox								BoundingBox;
		mutable ZEAABBox						WorldBoundingBox;
		
		mutable ZEMatrix4x4						LocalTransform;
		mutable ZEMatrix4x4						WorldTransform;
		mutable ZEMatrix4x4						InvWorldTransform;

		ZEVector3								Position;
		ZEQuaternion							Rotation;
		ZEVector3								Scale;

		void									UpdateConstantBuffer();

		void									RayCastOctreePoligons(const ZEOctree<ZESize>& Octree, ZERayCastReport& Report, ZERayCastHelper& Helper);
		void									RayCastOctree(const ZEOctree<ZESize>& Octree, ZERayCastReport& Report, ZERayCastHelper& Helper);

		void									ParentTransformChanged();

												ZEInteriorRoom();
												~ZEInteriorRoom();

	public:
		ZEInterior*								GetOwner();
		const ZEString&							GetName();

		const ZEArray<ZEInteriorDoor*>&			GetDoors();
		ZEPhysicalMesh*							GetPhysicalMesh();
		ZESize									GetPolygonCount();

		const ZEAABBox&							GetBoundingBox() const;
		const ZEAABBox&							GetWorldBoundingBox() const;

		const ZEMatrix4x4&						GetTransform() const;
		const ZEMatrix4x4&						GetWorldTransform() const;
		const ZEMatrix4x4&						GetInvWorldTransform() const;

		void									SetPosition(const ZEVector3& NewPosition);
		const ZEVector3&						GetPosition() const;

		void									SetRotation(const ZEQuaternion& NewRotation);
		const ZEQuaternion&						GetRotation() const;

		void									SetScale(const ZEVector3& NewScale);
		const ZEVector3&						GetScale() const;

		void									SetPersistentDraw(bool Enabled);

		bool									Initialize(ZEInterior* Owner, ZEInteriorResourceRoom* Resource);
		void									Deinitialize();

		void									PreRender(const ZERNPreRenderParameters* Parameters);
		void									Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command);

		void									RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters);

		static ZEInteriorRoom*					CreateInstance();
};
