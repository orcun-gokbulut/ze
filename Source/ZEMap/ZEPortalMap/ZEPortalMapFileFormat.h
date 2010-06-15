//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - PortalMapFileFormat.h
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
#ifndef __ZE_PORTAL_MAP_FILE_FORMAT_H__
#define __ZE_PORTAL_MAP_FILE_FORMAT_H__

#include "Definitions.h"
#include "Types.h"
#include "ZEMath/Vector.h"
#include "ZEMath/AABoundingBox.h"
#include "ZEMath/Rectangle3D.h"

#define ZE_MAP_MAX_NAME_SIZE ZE_MAX_NAME_SIZE
#define ZE_MAP_MAX_FILENAME_SIZE ZE_MAX_FILE_NAME_SIZE

#define ZE_FILE_FORMAT_MAKEVERSION(Major, Minor) ((ZEDWORD)((((ZEDWORD)(Major)) << 16) + (ZEDWORD)(Minor)))
#define ZE_FILE_FORMAT_GETMINORVERSION(x) ((ZEDWORD)((x) & 0x0000FFFF))
#define ZE_FILE_FORMAT_GETMAJORVERSION(x) ((ZEDWORD)(((x) & 0xFFFF0000) >> 16))

#define ZE_MAP_VERSION ZE_FILE_FORMAT_MAKEVERSION(0, 5)

#define ZE_MAP_HEADER ((ZEDWORD)((ZEDWORD)'ZEMF' + (ZEDWORD)'MAP '))
#define ZE_MAP_MATERIAL_CHUNK ((ZEDWORD)(ZE_MAP_HEADER + (ZEDWORD)'MTRL'))
#define ZE_MAP_PORTAL_CHUNK ((ZEDWORD)(ZE_MAP_HEADER + (ZEDWORD)'PRTL'))
#define ZE_MAP_PORTAL_DOOR_CHUNK ((ZEDWORD)(ZE_MAP_PORTAL_CHUNK + (ZEDWORD)'DOOR'))
#define ZE_MAP_BRUSHE_CHUNK ((ZEDWORD)(ZE_MAP_PORTAL_CHUNK + (ZEDWORD)'BRSH'))
#define ZE_MAP_POLYGONS_CHUNK ((ZEDWORD)(ZE_MAP_PORTAL_CHUNK + (ZEDWORD)'PLYG'))
#define ZE_MAP_OCTREE_CHUNK ((ZEDWORD)(ZE_MAP_PORTAL_CHUNK + (ZEDWORD)'OCTR' + (ZEDWORD)'NODE'))
#define ZE_MAP_OCTREE_POLYGONIDS_CHUNK ((ZEDWORD)(ZE_MAP_OCTREE_CHUNK + (ZEDWORD)'PLID'))
#define ZE_MAP_ENTITY_CHUNK ((ZEDWORD)(ZE_MAP_HEADER + (ZEDWORD)'ENTY'))
#define ZE_MAP_ENTITY_ATTRIBUTE_CHUNK ((ZEDWORD)(ZE_MAP_ENTITY_CHUNK + (ZEDWORD)'ATTR'))
#define ZE_MAP_PHYSICAL_MESH_CHUNK ((ZEDWORD)(ZE_MAP_PORTAL_CHUNK + (ZEDWORD)'MESH'))
#define ZE_MAP_PHYSICAL_MESH_VERTICES_CHUNK ((ZEDWORD)(ZE_MAP_PHYSICAL_MESH_CHUNK + (ZEDWORD)'VERT'))
#define ZE_MAP_PHYSICAL_MESH_POLYGONS_CHUNK ((ZEDWORD)(ZE_MAP_PHYSICAL_MESH_CHUNK + (ZEDWORD)'INDX'))

struct ZEMapFileMaterialChunk
{
	ZEDWORD							ChunkIdentifier;

	char							Shader[ZE_MAP_MAX_FILENAME_SIZE];
	ZEDWORD							ShaderComponents;

	bool							TwoSided;
	bool							LightningEnabled;
	bool							Wireframe;
	bool							Transparant;

	ZEVector3						AmbientColor;
	ZEVector3						DiffuseColor;
	ZEVector3						SpecularColor;
	ZEVector3						EmmisiveColor;
	float							EmmisiveFactor;
	float							SpecularFactor;
	float							Transparancy;
	float							ReflectionFactor;
	float							RefractionFactor;
	ZEVector2						DetailMapTiling;

	char							DiffuseMap[ZE_MAP_MAX_FILENAME_SIZE];
	char							NormalMap[ZE_MAP_MAX_FILENAME_SIZE];
	char							SpecularMap[ZE_MAP_MAX_FILENAME_SIZE];
	char							EmmisiveMap[ZE_MAP_MAX_FILENAME_SIZE];
	char							OpacityMap[ZE_MAP_MAX_FILENAME_SIZE];
	char							DetailMap[ZE_MAP_MAX_FILENAME_SIZE];
	char							DetailNormalMap[ZE_MAP_MAX_FILENAME_SIZE];
	char							ReflectionMap[ZE_MAP_MAX_FILENAME_SIZE];
	char							RefractionMap[ZE_MAP_MAX_FILENAME_SIZE];
	char							LightMap[ZE_MAP_MAX_FILENAME_SIZE];
};

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

struct ZEMapFileOctreeChunk
{
	ZEDWORD							ChunkIdentifier;
	ZEAABoundingBox					BoundingBox;
	ZEDWORD							Depth;
	bool							SubSpaces[8];
	bool							IsLeaf;
	ZEDWORD							PolygonCount;
};

struct ZEMapFileDoorChunk
{
	ZEDWORD							ChunkIdentifier;
	char							Name[ZE_MAP_MAX_NAME_SIZE];
	ZERectangle3D					Rectangle;
	ZEDWORD							PortalIds[2];
	bool							IsOpen;
};

struct ZEMapFilePortalChunk
{
	ZEDWORD							ChunkIdentifier;
	char							Name[ZE_MAP_MAX_NAME_SIZE];
	ZEAABoundingBox					BoundingBox;
	ZEDWORD							PolygonCount;
	bool							HasOctree;
	bool							HasPhysicalMesh;
};

struct ZEMapFilePhysicalMeshChunk
{
	ZEDWORD							ChunkIdentifier;
	ZEDWORD							VertexCount;
	ZEDWORD							PolygonCount;
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
