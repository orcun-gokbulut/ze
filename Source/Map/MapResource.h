//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - MapResource.h
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
#ifndef __ZE_MAP_RESOURCE_H__
#define __ZE_MAP_RESOURCE_H__

#include "ZEDS/Array.h"
#include "ZEMath/Vector.h"
#include "ZEMath/Rectangle3D.h"
#include "Definitions.h"
#include "Core/Resource.h"
#include "Graphics/Material.h"
#include "Graphics/VertexTypes.h"
#include "Graphics/IndexedPolygon.h"
#include "Graphics/TextureResource.h"
#include "Octree.h"
#include "GameInterface/Entity.h"

struct ZEMapPolygon
{
	ZEMapVertex									Vertices[3];
	ZEMaterial*									Material;
	unsigned int								LastIteration;
};

class ZEMapPortal;
class ZEMapResource;

class ZEMapPortalDoor
{
	public:
		char									Name[ZE_MAX_NAME_SIZE];
		ZERectangle3D							Rectangle;
		ZEMapPortal*							DestinationPortal;
		bool									IsOpen;
};

struct ZEMapPhysicalMeshPolygon
{
	ZEDWORD										Indices[3];
};

struct ZEMapPhysicalMesh
{
	ZEArray<ZEVector3>							Vertices;
	ZEArray<ZEMapPhysicalMeshPolygon>			Polygons;
};


class ZEMapPortal
{
	public:
		char									Name[ZE_MAX_NAME_SIZE];
		ZEAABoundingBox							BoundingBox;
		ZEArray<ZEMapPortalDoor>				Doors;
		ZEVertexBuffer*							VertexBuffer;
		ZEArray<ZERenderOrder>					RenderOrders;
		ZEArray<ZEMapPolygon>					Polygons;
				
		bool									HasOctree;
		ZEOctree*								Octree;
	
		bool									HasPhysicalMesh;
		ZEMapPhysicalMesh						PhysicalMesh;

												ZEMapPortal();
												~ZEMapPortal();
};

class ZEMapResource : public ZEResource
{
	public:
		ZESmartArray<ZETexture2DResource*>		TextureResources;
		ZEArray<ZEMaterial*>					Materials;
		ZEArray<ZEMapPortalDoor>				PortalDoors;
		ZEArray<ZEMapPortal>					Portals;
		ZEArray<ZEEntityData>					Entities;

		const char*								GetResourceType() const;

		static ZEMapResource*					LoadResource(const char* FileName);
		static const ZEMapResource*				LoadSharedResource(const char* FileName);
		static void								CacheResource(const char* FileName);

												~ZEMapResource();
};
#endif
