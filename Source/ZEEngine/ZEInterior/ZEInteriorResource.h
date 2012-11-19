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
#ifndef __ZE_INTERIOR_RESOURCE_H__
#define __ZE_INTERIOR_RESOURCE_H__

#include "ZETypes.h"
#include "ZEDS/ZEArray.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEMath/ZERectangle3D.h"
#include "ZEMath/ZEAABBox.h"
#include "ZEDefinitions.h"
#include "ZECore/ZEResource.h"
#include "ZEGraphics/ZEVertexTypes.h"

class ZEMaterial;
class ZETexture2D;
class ZETexture2DResource;
struct ZEInteriorDoorResource;
class ZEMLSerialReader;

struct ZEInteriorPolygon
{
	ZEInteriorVertex				Vertices[3];
	ZEMaterial*				Material;
	ZEUInt32				LastIteration;
};

struct ZEInteriorPhysicalMeshPolygon
{
	ZEUInt32				Indices[3];
};

struct ZEInteriorPhysicalMeshResource
{
	bool											PhysicalMeshEnabled;
	ZEArray<ZEVector3>								Vertices;
	ZEArray<ZEInteriorPhysicalMeshPolygon>			Polygons;
};

struct ZEInteriorRoomResource
{
	char											Name[ZE_MAX_NAME_SIZE];
	ZEAABBox										BoundingBox;
	ZEVector3										Position;
	ZEQuaternion									Rotation;
	ZEVector3										Scale;
	ZEArray<ZEInteriorDoorResource*>				Doors;
	ZEArray<ZESize>									DoorIds;
	ZEArray<ZEInteriorPolygon>						Polygons;
	ZEInteriorPhysicalMeshResource					PhysicalMesh;
	bool											HasPhysicalMesh;
};

struct ZEInteriorDoorResource
{
	char											Name[ZE_MAX_NAME_SIZE];
	bool											IsOpen;
	float											Width;
	float											Length;
	ZEVector3										Position;
	ZEQuaternion									Rotation;
	ZEVector3										Scale;
	ZEInteriorRoomResource*							Rooms[2];
	ZEUInt											RoomIds[2];
	
};

class ZEFile;

class ZEInteriorResource : public ZEResource
{
	private:
		ZEArray<ZETexture2DResource*>				TextureResources;
		ZEArray<ZEMaterial*>						Materials;
		ZEArray<ZEInteriorDoorResource>				Doors;
		ZEArray<ZEInteriorRoomResource>				Rooms;

		bool										ReadDoors(ZEMLSerialReader* Reader);
		bool										ReadRooms(ZEMLSerialReader* Reader);
		bool										ReadMaterials(ZEMLSerialReader* Reader);

		const ZETexture2D*							ManageInteriorMaterialTextures(const ZEString& FileName);
		bool  										ReadInteriorFromFile(ZEFile* ResourceFile);

		virtual										~ZEInteriorResource();

	public:
		const char*									GetResourceType() const;

		const ZEArray<ZETexture2DResource*>&		GetTextures();
		const ZEArray<ZEMaterial*>&					GetMaterials();
		const ZEArray<ZEInteriorRoomResource>&		GetRooms();
		const ZEArray<ZEInteriorDoorResource>&		GetDoors();

		static ZEInteriorResource*					LoadResource(const ZEString& FileName);
		static ZEInteriorResource*					LoadSharedResource(const ZEString& FileName);
		static void									CacheResource(const ZEString& FileName);
};
#endif
