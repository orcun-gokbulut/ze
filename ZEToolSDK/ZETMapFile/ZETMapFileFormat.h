//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETMapFileFormat.h
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
#ifndef __ZET_MAP_FILE_FORMAT_H__
#define __ZET_MAP_FILE_FORMAT_H__

#include "ZETDefinitions.h"
#include "ZETTypes.h"
#include "ZEDS/ZEVariant.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEAABBox.h"
#include "ZEMath/ZERectangle3D.h"

#define ZE_MPFL_MAX_NAME_SIZE				ZE_MAX_NAME_SIZE
#define ZE_MPFL_MAX_FILENAME_SIZE			ZE_MAX_FILE_NAME_SIZE

#define ZEFILE_MAKEVERSION(Major, Minor) ((ZEDWORD)((((ZEDWORD)(Major)) << 16) + (ZEDWORD)(Minor)))
#define ZEFILE_GETMINORVERSION(x) ((ZEDWORD)((x) & 0x0000FFFF))
#define ZEFILE_GETMAJORVERSION(x) ((ZEDWORD)(((x) & 0xFFFF0000) >> 16))

#define ZE_MPFL_VERSION ZEFILE_MAKEVERSION(0, 5)

#define ZE_MPFL_HEADER ((ZEDWORD)((ZEDWORD)'ZEMF' + (ZEDWORD)'MAP '))
#define ZE_MPFL_MATERIAL_CHUNK ((ZEDWORD)(ZE_MPFL_HEADER + (ZEDWORD)'MTRL'))
#define ZE_MPFL_PORTAL_CHUNK ((ZEDWORD)(ZE_MPFL_HEADER + (ZEDWORD)'PRTL'))
#define ZE_MPFL_PORTAL_DOOR_CHUNK ((ZEDWORD)(ZE_MPFL_PORTAL_CHUNK + (ZEDWORD)'DOOR'))
#define ZE_MPFL_BRUSHE_CHUNK ((ZEDWORD)(ZE_MPFL_PORTAL_CHUNK + (ZEDWORD)'BRSH'))
#define ZE_MPFL_POLYGONS_CHUNK ((ZEDWORD)(ZE_MPFL_PORTAL_CHUNK + (ZEDWORD)'PLYG'))
#define ZE_MPFL_OCTREE_CHUNK ((ZEDWORD)(ZE_MPFL_PORTAL_CHUNK + (ZEDWORD)'OCTR' + (ZEDWORD)'NODE'))
#define ZE_MPFL_OCTREE_POLYGONIDS_CHUNK ((ZEDWORD)(ZE_MPFL_OCTREE_CHUNK + (ZEDWORD)'PLID'))
#define ZE_MPFL_ENTITY_CHUNK ((ZEDWORD)(ZE_MPFL_HEADER + (ZEDWORD)'ENTY'))
#define ZE_MPFL_ENTITY_ATTRIBUTE_CHUNK ((ZEDWORD)(ZE_MPFL_ENTITY_CHUNK + (ZEDWORD)'ATTR'))
#define ZE_MPFL_PHYSICAL_MESH_CHUNK ((ZEDWORD)(ZE_MPFL_PORTAL_CHUNK + (ZEDWORD)'MESH'))
#define ZE_MPFL_PHYSICAL_MESH_VERTICES_CHUNK ((ZEDWORD)(ZE_MPFL_PHYSICAL_MESH_CHUNK + (ZEDWORD)'VERT'))
#define ZE_MPFL_PHYSICAL_MESH_POLYGONS_CHUNK ((ZEDWORD)(ZE_MPFL_PHYSICAL_MESH_CHUNK + (ZEDWORD)'INDX'))

struct ZEMapFileVertexChunk
{
	ZEVector3						Position;
	ZEVector3						Normal;
	ZEVector3						Tangent;
	ZEVector3						Binormal;
	ZEVector2						Texcoord;
};

struct ZEMapFilePolygonChunk
{	
	ZEDWORD							Material;
	ZEMapFileVertexChunk			Vertices[3];
};

struct ZEMapFilePhysicalMeshPolygonChunk
{
	ZEDWORD							Indices[3];
};

struct ZEMapFilePhysicalMeshChunk
{
	ZEDWORD							ChunkIdentifier;
	ZEDWORD							VertexCount;
	ZEDWORD							PolygonCount;
};

struct ZEMapFileOctreeChunk
{
	ZEDWORD							ChunkIdentifier;
	ZEAABBox					BoundingBox;
	ZEDWORD							Depth;
	bool							SubSpaces[8];
	bool							IsLeaf;
	ZEDWORD							PolygonCount;
};

struct ZEMapFileDoorChunk
{
	ZEDWORD							ChunkIdentifier;
	char							Name[ZE_MPFL_MAX_NAME_SIZE];
	ZERectangle3D					Rectangle;
	ZEDWORD							PortalIds[2];
	bool							IsOpen;
};

struct ZEMapFilePortalChunk
{
	ZEDWORD							ChunkIdentifier;
	char							Name[ZE_MPFL_MAX_NAME_SIZE];
	ZEAABBox					BoundingBox;
	ZEDWORD							PolygonCount;
	bool							HasOctree;
	bool							HasPhysicalMesh;
};

struct ZEMapFileHeader
{
	ZEDWORD							Header;
	ZEDWORD							Version;

	ZEDWORD							MaterialCount;
	ZEDWORD							PortalCount;
	ZEDWORD							DoorCount;

};

#endif
