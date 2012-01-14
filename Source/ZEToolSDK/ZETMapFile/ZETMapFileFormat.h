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
#include "ZETypes.h"
#include "ZEDS/ZEVariant.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEAABBox.h"
#include "ZEMath/ZERectangle3D.h"

#define ZE_MPFL_MAX_NAME_SIZE				ZE_MAX_NAME_SIZE
#define ZE_MPFL_MAX_FILENAME_SIZE			ZE_MAX_FILE_NAME_SIZE

#define ZEFILE_MAKEVERSION(Major, Minor) ((ZEUInt32)((((ZEUInt32)(Major)) << 16) + (ZEUInt32)(Minor)))
#define ZEFILE_GETMINORVERSION(x) ((ZEUInt32)((x) & 0x0000FFFF))
#define ZEFILE_GETMAJORVERSION(x) ((ZEUInt32)(((x) & 0xFFFF0000) >> 16))

#define ZE_MPFL_VERSION ZEFILE_MAKEVERSION(0, 5)

#define ZE_MPFL_HEADER ((ZEUInt32)((ZEUInt32)'ZEMF' + (ZEUInt32)'MAP '))
#define ZE_MPFL_MATERIAL_CHUNK ((ZEUInt32)(ZE_MPFL_HEADER + (ZEUInt32)'MTRL'))
#define ZE_MPFL_PORTAL_CHUNK ((ZEUInt32)(ZE_MPFL_HEADER + (ZEUInt32)'PRTL'))
#define ZE_MPFL_PORTAL_DOOR_CHUNK ((ZEUInt32)(ZE_MPFL_PORTAL_CHUNK + (ZEUInt32)'DOOR'))
#define ZE_MPFL_BRUSHE_CHUNK ((ZEUInt32)(ZE_MPFL_PORTAL_CHUNK + (ZEUInt32)'BRSH'))
#define ZE_MPFL_POLYGONS_CHUNK ((ZEUInt32)(ZE_MPFL_PORTAL_CHUNK + (ZEUInt32)'PLYG'))
#define ZE_MPFL_OCTREE_CHUNK ((ZEUInt32)(ZE_MPFL_PORTAL_CHUNK + (ZEUInt32)'OCTR' + (ZEUInt32)'NODE'))
#define ZE_MPFL_OCTREE_POLYGONIDS_CHUNK ((ZEUInt32)(ZE_MPFL_OCTREE_CHUNK + (ZEUInt32)'PLID'))
#define ZE_MPFL_ENTITY_CHUNK ((ZEUInt32)(ZE_MPFL_HEADER + (ZEUInt32)'ENTY'))
#define ZE_MPFL_ENTITY_ATTRIBUTE_CHUNK ((ZEUInt32)(ZE_MPFL_ENTITY_CHUNK + (ZEUInt32)'ATTR'))
#define ZE_MPFL_PHYSICAL_MESH_CHUNK ((ZEUInt32)(ZE_MPFL_PORTAL_CHUNK + (ZEUInt32)'MESH'))
#define ZE_MPFL_PHYSICAL_MESH_VERTICES_CHUNK ((ZEUInt32)(ZE_MPFL_PHYSICAL_MESH_CHUNK + (ZEUInt32)'VERT'))
#define ZE_MPFL_PHYSICAL_MESH_POLYGONS_CHUNK ((ZEUInt32)(ZE_MPFL_PHYSICAL_MESH_CHUNK + (ZEUInt32)'INDX'))

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
	ZEUInt32							Material;
	ZEMapFileVertexChunk			Vertices[3];
};

struct ZEMapFilePhysicalMeshPolygonChunk
{
	ZEUInt32							Indices[3];
};

struct ZEMapFilePhysicalMeshChunk
{
	ZEUInt32							ChunkIdentifier;
	ZEUInt32							VertexCount;
	ZEUInt32							PolygonCount;
};

struct ZEMapFileOctreeChunk
{
	ZEUInt32							ChunkIdentifier;
	ZEAABBox					BoundingBox;
	ZEUInt32							Depth;
	bool							SubSpaces[8];
	bool							IsLeaf;
	ZEUInt32							PolygonCount;
};

struct ZEMapFileDoorChunk
{
	ZEUInt32							ChunkIdentifier;
	char							Name[ZE_MPFL_MAX_NAME_SIZE];
	ZERectangle3D					Rectangle;
	ZEUInt32							PortalIds[2];
	bool							IsOpen;
};

struct ZEMapFilePortalChunk
{
	ZEUInt32							ChunkIdentifier;
	char							Name[ZE_MPFL_MAX_NAME_SIZE];
	ZEAABBox					BoundingBox;
	ZEUInt32							PolygonCount;
	bool							HasOctree;
	bool							HasPhysicalMesh;
};

struct ZEMapFileHeader
{
	ZEUInt32							Header;
	ZEUInt32							Version;

	ZEUInt32							MaterialCount;
	ZEUInt32							PortalCount;
	ZEUInt32							DoorCount;

};

#endif
