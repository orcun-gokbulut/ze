//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPortalMapFileFormat.h
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

#include "ZEDefinitions.h"
#include "ZETypes.h"
#include "ZEFile/ZEFileStructure.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEAABBox.h"
#include "ZEMath/ZERectangle3D.h"

#define ZE_MAP_MAX_NAME_SIZE ZE_MAX_NAME_SIZE
#define ZE_MAP_MAX_FILENAME_SIZE ZE_MAX_FILE_NAME_SIZE

#define ZE_FILE_FORMAT_MAKEVERSION(Major, Minor) ((ZEUInt32)((((ZEUInt32)(Major)) << 16) + (ZEUInt32)(Minor)))
#define ZE_FILE_FORMAT_GETMINORVERSION(x) ((ZEUInt32)((x) & 0x0000FFFF))
#define ZE_FILE_FORMAT_GETMAJORVERSION(x) ((ZEUInt32)(((x) & 0xFFFF0000) >> 16))

#define ZE_MAP_VERSION ZE_FILE_FORMAT_MAKEVERSION(0, 5)

#define ZE_MAP_HEADER ((ZEUInt32)((ZEUInt32)'ZEMF' + (ZEUInt32)'MAP '))
#define ZE_MAP_MATERIAL_CHUNK ((ZEUInt32)(ZE_MAP_HEADER + (ZEUInt32)'MTRL'))
#define ZE_MAP_PORTAL_CHUNK ((ZEUInt32)(ZE_MAP_HEADER + (ZEUInt32)'PRTL'))
#define ZE_MAP_PORTAL_DOOR_CHUNK ((ZEUInt32)(ZE_MAP_PORTAL_CHUNK + (ZEUInt32)'DOOR'))
#define ZE_MAP_BRUSHE_CHUNK ((ZEUInt32)(ZE_MAP_PORTAL_CHUNK + (ZEUInt32)'BRSH'))
#define ZE_MAP_POLYGONS_CHUNK ((ZEUInt32)(ZE_MAP_PORTAL_CHUNK + (ZEUInt32)'PLYG'))
#define ZE_MAP_OCTREE_CHUNK ((ZEUInt32)(ZE_MAP_PORTAL_CHUNK + (ZEUInt32)'OCTR' + (ZEUInt32)'NODE'))
#define ZE_MAP_OCTREE_POLYGONIDS_CHUNK ((ZEUInt32)(ZE_MAP_OCTREE_CHUNK + (ZEUInt32)'PLID'))
#define ZE_MAP_ENTITY_CHUNK ((ZEUInt32)(ZE_MAP_HEADER + (ZEUInt32)'ENTY'))
#define ZE_MAP_ENTITY_ATTRIBUTE_CHUNK ((ZEUInt32)(ZE_MAP_ENTITY_CHUNK + (ZEUInt32)'ATTR'))
#define ZE_MAP_PHYSICAL_MESH_CHUNK ((ZEUInt32)(ZE_MAP_PORTAL_CHUNK + (ZEUInt32)'MESH'))
#define ZE_MAP_PHYSICAL_MESH_VERTICES_CHUNK ((ZEUInt32)(ZE_MAP_PHYSICAL_MESH_CHUNK + (ZEUInt32)'VERT'))
#define ZE_MAP_PHYSICAL_MESH_POLYGONS_CHUNK ((ZEUInt32)(ZE_MAP_PHYSICAL_MESH_CHUNK + (ZEUInt32)'INDX'))

ZEPackStruct(
struct  ZEMapFileMaterialChunk
{
	ZEUInt32				ChunkIdentifier;

	char					Shader[ZE_MAP_MAX_FILENAME_SIZE];
	ZEUInt32				ShaderComponents;

	bool					TwoSided;
	bool					LightningEnabled;
	bool					Wireframe;
	bool					Transparant;

	ZEVector3				AmbientColor;
	ZEVector3				DiffuseColor;
	ZEVector3				SpecularColor;
	ZEVector3				EmmisiveColor;
	float					EmmisiveFactor;
	float					SpecularFactor;
	float					Transparancy;
	float					ReflectionFactor;
	float					RefractionFactor;
	ZEVector2				DetailMapTiling;

	char					DiffuseMap[ZE_MAP_MAX_FILENAME_SIZE];
	char					NormalMap[ZE_MAP_MAX_FILENAME_SIZE];
	char					SpecularMap[ZE_MAP_MAX_FILENAME_SIZE];
	char					EmmisiveMap[ZE_MAP_MAX_FILENAME_SIZE];
	char					OpacityMap[ZE_MAP_MAX_FILENAME_SIZE];
	char					DetailMap[ZE_MAP_MAX_FILENAME_SIZE];
	char					DetailNormalMap[ZE_MAP_MAX_FILENAME_SIZE];
	char					ReflectionMap[ZE_MAP_MAX_FILENAME_SIZE];
	char					RefractionMap[ZE_MAP_MAX_FILENAME_SIZE];
	char					LightMap[ZE_MAP_MAX_FILENAME_SIZE];
});

ZEPackStruct(
struct ZEMapFileVertexChunk
{
	ZEVector3				Position;
	ZEVector3				Normal;
	ZEVector3				Tangent;
	ZEVector3				Binormal;
	ZEVector2				Texcoord;
});

ZEPackStruct(
struct  ZEMapFilePolygonChunk
{	
	ZEUInt32				Material;
	ZEMapFileVertexChunk	Vertices[3];
});

ZEPackStruct(
struct  ZEMapFilePhysicalMeshPolygonChunk
{
	ZEUInt32				Indices[3];
});

ZEPackStruct(
struct  ZEMapFileOctreeChunk
{
	ZEUInt32				ChunkIdentifier;
	ZEAABBox				BoundingBox;
	ZEUInt32				Depth;
	bool					SubSpaces[8];
	bool					IsLeaf;
	ZEUInt32				PolygonCount;
});

ZEPackStruct(
struct  ZEMapFileDoorChunk
{
	ZEUInt32				ChunkIdentifier;
	char					Name[ZE_MAP_MAX_NAME_SIZE];
	ZERectangle3D			Rectangle;
	ZEUInt32				PortalIds[2];
	bool					IsOpen;
});

ZEPackStruct(
struct  ZEMapFilePortalChunk
{
	ZEUInt32				ChunkIdentifier;
	char					Name[ZE_MAP_MAX_NAME_SIZE];
	ZEAABBox				BoundingBox;
	ZEUInt32				PolygonCount;
	bool					HasOctree;
	bool					HasPhysicalMesh;
});

ZEPackStruct(
struct  ZEMapFilePhysicalMeshChunk
{
	ZEUInt32				ChunkIdentifier;
	ZEUInt32				VertexCount;
	ZEUInt32				PolygonCount;
});

ZEPackStruct(
struct  ZEMapFileHeader
{
	ZEUInt32				Header;
	ZEUInt32				Version;

	ZEUInt32				MaterialCount;
	ZEUInt32				PortalCount;
	ZEUInt32				DoorCount;

});

#endif




