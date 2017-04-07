//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEInteriorResource.h
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

#include "ZECore/ZEResource.h"

#include "ZETypes.h"
#include "ZEDS/ZEArray.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEMath/ZERectangle3D.h"
#include "ZEMath/ZEAABBox.h"
#include "ZESpatial/ZEOctree.h"
#include "ZEGraphics/ZEGRVertexLayout.h"

class ZERNMaterial;
class ZEMLReaderNode;
class ZEGRTexture;
class ZETexture2DResource;
class ZEGRVertexLayout;
struct ZEInteriorResourceDoor;

enum ZEInteriorResourceHelperOwnerType
{
	ZE_IRHOT_INTERIOR		= 0,
	ZE_IRHOT_ROOM			= 1
};

struct ZEInteriorVertex
{
	private:
		static ZEGRVertexLayout			VertexLayout;

	public:
		ZEVector3						Position;
		ZEVector3						Normal;
		ZEVector3						Tangent;
		ZEVector3						Binormal;
		ZEVector2						Texcoord;

		static ZEGRVertexLayout*		GetVertexLayout();
};

struct ZEInteriorPolygon
{
	ZEInteriorVertex					Vertices[3];
	const ZERNMaterial*					Material;
	ZEUInt32							LastIteration;
};

struct ZEInteriorPhysicalMeshPolygon
{
	ZEUInt32							Indices[3];
};

struct ZEInteriorResourcePhysicalMesh
{
	bool											PhysicalMeshEnabled;
	ZEArray<ZEVector3>								Vertices;
	ZEArray<ZEInteriorPhysicalMeshPolygon>			Polygons;
	ZEString										UserDefinedProperties;
};

struct ZEInteriorResourceRoom
{
	ZEString										Name;
	ZEAABBox										BoundingBox;
	ZEVector3										Position;
	ZEQuaternion									Rotation;
	ZEVector3										Scale;
	ZEArray<ZEInteriorResourceDoor*>				Doors;
	ZEArray<ZESize>									DoorIds;
	ZEArray<ZEInteriorPolygon>						Polygons;
	ZEInteriorResourcePhysicalMesh					PhysicalMesh;
	bool											HasPhysicalMesh;
	ZEString										UserDefinedProperties;
	ZEOctree<ZESize>								Octree;
	bool											HasOctree;
};

struct ZEInteriorResourceDoor
{
	ZEString										Name;
	bool											IsOpen;
	float											Width;
	float											Length;
	ZEVector3										Position;
	ZEQuaternion									Rotation;
	ZEVector3										Scale;
	ZEInteriorResourceRoom*							Rooms[2];
	ZEInt32											RoomIds[2];
	ZEString										UserDefinedProperties;
	
};

struct ZEInteriorResourceHelper
{
	ZEString										Name;
	ZEInteriorResourceHelperOwnerType				OwnerType;
	ZEInt32											OwnerIndex;
	ZEInteriorResourceRoom*							OwnerRoom;
	ZEVector3										Position;
	ZEQuaternion									Rotation;
	ZEVector3										Scale;
	ZEString										UserDefinedProperties;
};

class ZEFile;

class ZEInteriorResource : public ZEResource
{
	private:
		ZEArray<ZEHolder<ZEGRTexture>>					TextureResources;
		ZEArray<ZEHolder<const ZERNMaterial>>			Materials;
		ZEArray<ZEInteriorResourceDoor>					Doors;
		ZEArray<ZEInteriorResourceRoom>					Rooms;
		ZEArray<ZEInteriorResourceHelper>				Helpers;

		bool											ReadDoors(ZEMLReaderNode* DoorsNode);
		bool											ReadRooms(ZEMLReaderNode* RoomsNode);
		bool											ReadHelpers(ZEMLReaderNode* HelpersNode);
		bool											ReadMaterials(ZEMLReaderNode* MaterialsNode);

		const ZEGRTexture*								ManageInteriorMaterialTextures(const ZEString& FileName);
		bool  											ReadInteriorFromFile(ZEFile* ResourceFile);

		virtual											~ZEInteriorResource();

	public:
		const char*										GetResourceType() const;

		const ZEArray<ZEHolder<ZEGRTexture>>&			GetTextures() const;
		const ZEArray<ZEHolder<const ZERNMaterial>>&	GetMaterials() const;
		const ZEArray<ZEInteriorResourceRoom>&			GetRooms() const;
		const ZEArray<ZEInteriorResourceDoor>&			GetDoors() const;
		const ZEArray<ZEInteriorResourceHelper>&		GetHelpers() const;

		static ZEInteriorResource*						LoadResource(const ZEString& FileName);
		static ZEInteriorResource*						LoadSharedResource(const ZEString& FileName);
		static void										CacheResource(const ZEString& FileName);
};
