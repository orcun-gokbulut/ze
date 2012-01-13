//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETMapFile.h
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
#ifndef __ZET_MAP_FILE_H__
#define __ZET_MAP_FILE_H__

#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEVariant.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZERectangle3D.h"
#include "ZEMath/ZEAABBox.h"
#include "ZETypes.h"
#include "ZETDefinitions.h"

#define ZE_MPFL_MAX_NAME_SIZE				ZE_MAX_NAME_SIZE
#define ZE_MPFL_MAX_FILENAME_SIZE			ZE_MAX_FILE_NAME_SIZE

#define ZEMAP_FILEPART_GEOMETRY				0x01
#define ZEMAP_FILEPART_ENTITIES				0x02
#define ZEMAP_FILEPART_MATERIALS			0x04
#define ZEMAP_FILEPART_ALL					(ZEMAP_FILEPART_GEOMETRY | ZEMAP_FILEPART_ENTITIES | ZEMAP_FILEPART_MATERIALS)	

#define ZESHADER_SKINTRANSFORM				1
#define ZESHADER_DIFFUSEMAP					2
#define ZESHADER_NORMALMAP					4
#define ZESHADER_SPECULARMAP				8
#define ZESHADER_EMMISIVEMAP				16
#define ZESHADER_OCAPASITYMAP				32
#define ZESHADER_DETAILDIFFUSEMAP			64
#define ZESHADER_DETAILNORMALMAP			128
#define ZESHADER_REFLECTION					256
#define ZESHADER_REFRACTION					512
#define ZESHADER_LIGHTMAP					1024
#define ZESHADER_DISTORTIONMAP				2048

struct ZEMapFileMaterial
{
	char									Shader[ZE_MPFL_MAX_FILENAME_SIZE];
	ZEUInt32									ShaderComponents;

	bool									TwoSided;
	bool									LightningEnabled;
	bool									Wireframe;
	bool									Transparant;

	ZEVector3								AmbientColor;
	ZEVector3								DiffuseColor;
	ZEVector3								SpecularColor;
	ZEVector3								EmmisiveColor;
	float									EmmisiveFactor;
	float									SpecularFactor;
	float									Opasity;
	float									ReflectionFactor;
	float									RefractionFactor;
	ZEVector2								DetailMapTiling;

	char									DiffuseMap[ZE_MPFL_MAX_FILENAME_SIZE];
	char									NormalMap[ZE_MPFL_MAX_FILENAME_SIZE];
	char									SpecularMap[ZE_MPFL_MAX_FILENAME_SIZE];
	char									EmmisiveMap[ZE_MPFL_MAX_FILENAME_SIZE];
	char									OpasityMap[ZE_MPFL_MAX_FILENAME_SIZE];
	char									DetailMap[ZE_MPFL_MAX_FILENAME_SIZE];
	char									DetailNormalMap[ZE_MPFL_MAX_FILENAME_SIZE];
	char									ReflectionMap[ZE_MPFL_MAX_FILENAME_SIZE];
	char									RefractionMap[ZE_MPFL_MAX_FILENAME_SIZE];
	char									LightMap[ZE_MPFL_MAX_FILENAME_SIZE];
};

struct ZEMapFileVertex
{
	ZEVector3								Position;
	ZEVector3								Normal;
	ZEVector3								Tangent;
	ZEVector3								Binormal;
	ZEVector2								Texcoord;
};

struct ZEMapFilePolygon
{	
	ZEUInt32									Material;
	ZEMapFileVertex							Vertices[3];
};

struct ZEMapFilePhysicalMeshPolygon
{
	ZEUInt32									Indices[3];
};

struct ZEMapFilePhysicalMesh
{
	ZEArray<ZEVector3>						Vertices;
	ZEArray<ZEMapFilePhysicalMeshPolygon>	Polygons;
};

struct ZEMapFileDoor
{
	char									Name[ZE_MPFL_MAX_NAME_SIZE];
	ZERectangle3D							Rectangle;
	ZEUInt32									PortalIds[2];
	bool									IsOpen;
};

class ZEMapFileOctree;
class ZEMapFilePortal
{
public:
	char									Name[ZE_MPFL_MAX_NAME_SIZE];
	ZEAABBox							BoundingBox;
	bool									GenerateOctree;
	unsigned int							MaxOctreeDepth;

	ZEMapFileOctree*						Octree;
	ZEArray<ZEMapFilePolygon>				Polygons;
	bool									PhysicalMeshEnabled;
	ZEMapFilePhysicalMesh					PhysicalMesh;

											ZEMapFilePortal();
											~ZEMapFilePortal();
};

class ZEMapFile
{
	public:
		ZEAABBox						MapBoundingBox;

		ZEArray<ZEMapFileMaterial>			Materials;
		ZEArray<ZEMapFilePortal>			Portals;
		ZEArray<ZEMapFileDoor>				Doors;

		bool								WriteToFile(const char* File, int Chunks = ZEMAP_FILEPART_ALL);
		bool								ReadFromFile(const char* File, int Chunks = ZEMAP_FILEPART_ALL);

		void								Clear();

		bool								Validate();

											ZEMapFile();
											~ZEMapFile();
};

#endif
