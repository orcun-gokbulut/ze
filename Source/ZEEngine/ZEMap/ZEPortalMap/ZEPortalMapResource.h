//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPortalMapResource.h
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
#ifndef __ZE_PORTAL_MAP_RESOURCE_H__
#define __ZE_PORTAL_MAP_RESOURCE_H__

#include "../ZEMapResource.h"
#include "ZEDS/ZEArray.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZERectangle3D.h"
#include "ZEMath/ZEAABBox.h"
#include "ZEDefinitions.h"
#include "ZECore/ZEResource.h"
#include "ZEGraphics/ZEVertexTypes.h"

class ZEMaterial;
class ZETexture2D;
class ZETexture2DResource;
struct ZEPortalMapResourceDoor;

struct ZEPortalMapPolygon
{
	ZEMapVertex		Vertices[3];
	ZEMaterial*		Material;
	ZEUInt	LastIteration;
};

struct ZEPortalMapPhysicalMeshPolygon
{
	ZEUInt32			Indices[3];
};

struct ZEPortalMapResourcePhysicalMesh
{
	ZEArray<ZEVector3>							Vertices;
	ZEArray<ZEPortalMapPhysicalMeshPolygon>		Polygons;
};

struct ZEPortalMapResourcePortal
{
	char										Name[ZE_MAX_NAME_SIZE];
	ZEAABBox								BoundingBox;
	ZEArray<ZEPortalMapResourceDoor*>			Doors;
	ZEArray<ZESize>								DoorIds;
	ZEArray<ZEPortalMapPolygon>					Polygons;
	ZEPortalMapResourcePhysicalMesh				PhysicalMesh;
	bool										HasOctree;
	//ZEPortalMapOctree							Octree;
	bool										HasPhysicalMesh;
};

struct ZEPortalMapResourceDoor
{
	char										Name[ZE_MAX_NAME_SIZE];
	ZERectangle3D								Rectangle;
	ZEPortalMapResourcePortal*					Portals[2];
	ZESize										PortalIds[2];
	bool										IsOpen;
};

class ZEFile;

class ZEPortalMapResource : public ZEMapResource
{
	private:
		ZEArray<ZETexture2DResource*>				TextureResources;
		ZEArray<ZEMaterial*>						Materials;
		ZEArray<ZEPortalMapResourceDoor>			Doors;
		ZEArray<ZEPortalMapResourcePortal>			Portals;

		const ZETexture2D*							ManageMapMaterialTextures(const ZEString& FileName);
		bool  										ReadMaterialsFromFile(ZEFile* ResourceFile);
		bool  										ReadPhysicalMeshFromFile(ZEFile* ResourceFile, ZEPortalMapResourcePortal* Portal);
		bool  										ReadPortalsFromFile(ZEFile* ResourceFile);
		bool  										ReadDoorsFromFile(ZEFile* ResourceFile);
		bool  										ReadMapFromFile(ZEFile* ResourceFile);

		virtual										~ZEPortalMapResource();

	public:
		const char*									GetResourceType() const;

		const ZEArray<ZETexture2DResource*>&		GetTextures();
		const ZEArray<ZEMaterial*>&					GetMaterials();
		const ZEArray<ZEPortalMapResourcePortal>&	GetPortals();
		const ZEArray<ZEPortalMapResourceDoor>&		GetDoors();

		static ZEPortalMapResource*					LoadResource(const ZEString& FileName);
		static ZEPortalMapResource*					LoadSharedResource(const ZEString& FileName);
		static void									CacheResource(const ZEString& FileName);
};
#endif




