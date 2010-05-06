//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - MapResource.cpp
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

#include "MapResource.h"
#include "Core/Error.h"
#include "Core/Resource.h"
#include "Core/ResourceFile.h"
#include "Core/ResourceManager.h"
#include "MapFileFormat.h"
#include "Graphics/GraphicsModule.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/Texture2DResource.h"
#include "Graphics/RenderOrder.h"
#include "Graphics/FixedMaterial.h"
#include "Physics/PhysicalStaticMesh.h"
#include "Octree.h"
#include <string.h>

// Reading

const ZETexture2D* ManageMapMaterialTextures(char* FileName, ZESmartArray<ZETexture2DResource*>& TextureResources)
{
	if (strncmp(FileName, "", ZE_MAP_MAX_FILENAME_SIZE) == 0)
		return NULL;

	for (size_t I = 0; I < TextureResources.GetCount(); I++)
		if (strnicmp(TextureResources[I]->GetFileName(), FileName, ZE_MAP_MAX_FILENAME_SIZE) == 0)
			return TextureResources[I]->GetTexture();

	ZETexture2DResource* NewTextureResource = ZETexture2DResource::LoadSharedResource(FileName);
	if (NewTextureResource == NULL)
	{
		zeError("Map Resource", "Can not load texture file. (FileName : \"%s\")", FileName);
		return NULL;
	}
	TextureResources.Add(NewTextureResource);
	return NewTextureResource->GetTexture();
}

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

bool ReadMaterialsFromFile(ZEResourceFile* ResourceFile, ZEArray<ZEMaterial*>& Materials, ZESmartArray<ZETexture2DResource*>& TextureResources)
{
	ZEMapFileMaterialChunk MaterialChunk;
	// Read materials
	for (size_t I = 0; I < Materials.GetCount(); I++)
	{
		// Read Material
		ResourceFile->Read(&MaterialChunk, sizeof(ZEMapFileMaterialChunk), 1);
		
		// Check Chunk Identifier
		if (MaterialChunk.ChunkIdentifier != ZE_MAP_MATERIAL_CHUNK)
		{
			zeError("Map Resource", "Material chunk's id does not match.");
			return false;
		}	
		
		// Fixed Material Hack
		ZEFixedMaterial* CurrentMaterial = ZEFixedMaterial::CreateInstance();
		
		Materials[I] = CurrentMaterial;
		CurrentMaterial->SetDiffuseEnabled(true);
		CurrentMaterial->SetAmbientEnabled(true);
		CurrentMaterial->SetSpecularEnabled(true);
		CurrentMaterial->SetEmmisiveEnabled(true);
		
		CurrentMaterial->SetTwoSided(MaterialChunk.TwoSided);
		CurrentMaterial->SetLightningEnabled(MaterialChunk.LightningEnabled);
		CurrentMaterial->SetWireframe(MaterialChunk.Wireframe);
		CurrentMaterial->SetTransparancyMode(MaterialChunk.Transparant ? ZE_MTM_ADDAPTIVE: ZE_MTM_NOTRANSPARACY);

		CurrentMaterial->SetAmbientColor(MaterialChunk.AmbientColor);
		CurrentMaterial->SetDiffuseColor(MaterialChunk.DiffuseColor);
		CurrentMaterial->SetSpecularColor(MaterialChunk.SpecularColor);
		CurrentMaterial->SetEmmisiveColor(MaterialChunk.EmmisiveColor);
		CurrentMaterial->SetEmmisiveFactor(MaterialChunk.EmmisiveFactor);
		CurrentMaterial->SetSpecularFactor((1.25f - MaterialChunk.SpecularFactor) * 128.0f);
		CurrentMaterial->SetOpacity(MaterialChunk.Transparancy);
		CurrentMaterial->SetReflectionFactor(MaterialChunk.ReflectionFactor);
		CurrentMaterial->SetRefractionFactor(MaterialChunk.RefractionFactor);
		CurrentMaterial->SetDetailMapTiling(MaterialChunk.DetailMapTiling);

		CurrentMaterial->SetDiffuseMap(ManageMapMaterialTextures(MaterialChunk.DiffuseMap, TextureResources));
		
		CurrentMaterial->SetNormalMapEnabled(MaterialChunk.ShaderComponents & ZESHADER_NORMALMAP);
		CurrentMaterial->SetNormalMap(ManageMapMaterialTextures(MaterialChunk.NormalMap, TextureResources));
		CurrentMaterial->SetSpecularMap(ManageMapMaterialTextures(MaterialChunk.SpecularMap, TextureResources));
		CurrentMaterial->SetEmmisiveMap(ManageMapMaterialTextures(MaterialChunk.EmmisiveMap, TextureResources));
		CurrentMaterial->SetOpacityMap(ManageMapMaterialTextures(MaterialChunk.OpacityMap, TextureResources));
		
		CurrentMaterial->SetDetailMapEnabled(MaterialChunk.ShaderComponents & ZESHADER_DETAILNORMALMAP);
		CurrentMaterial->SetDetailDiffuseMap(ManageMapMaterialTextures(MaterialChunk.DetailMap, TextureResources));
		CurrentMaterial->SetDetailNormalMap(ManageMapMaterialTextures(MaterialChunk.DetailNormalMap, TextureResources));
		CurrentMaterial->SetReflectionEnabled(false);
		CurrentMaterial->SetRefractionMap(NULL);//ManageMapMaterialTextures(MaterialChunk.EnvironmentMap, TextureResources);
		CurrentMaterial->SetRefractionEnabled(false);
		CurrentMaterial->SetRefractionMap(NULL);

		CurrentMaterial->SetLightMapEnabled(MaterialChunk.ShaderComponents & ZESHADER_LIGHTMAP);
		CurrentMaterial->SetLightMap(ManageMapMaterialTextures(MaterialChunk.LightMap, TextureResources));

		CurrentMaterial->UpdateMaterial();
	}
	return true;
}

bool SortVertices(ZEStaticVertexBuffer** VertexBuffer, ZEArray<ZERenderOrder>& RenderOrders, ZEArray<ZEMapFilePolygonChunk>& Polygons, ZEArray<ZEMaterial*>& Materials)
{
	if (*VertexBuffer == NULL)
		*VertexBuffer = zeGraphics->CreateStaticVertexBuffer();

	if (!(*VertexBuffer)->Create(Polygons.GetCount() * 3 * sizeof(ZEMapVertex)))
		return false;

	ZEMapVertex* Buffer = (ZEMapVertex*)(*VertexBuffer)->Lock();
	
	size_t VertexIndex = 0;
	int RenderOrderIndex = -1;
	for (size_t I = 0; I < Polygons.GetCount(); I++)
	{
		if (Polygons[I].Material != 0xFFFFFFFF)
		{
			size_t MaterialId = Polygons[I].Material;
			ZERenderOrder* RenderOrder = RenderOrders.Add();
			RenderOrder->SetZero();
			RenderOrder->Flags = ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM | ZE_ROF_ENABLE_Z_CULLING;
			RenderOrder->Material = Materials[Polygons[I].Material];
			RenderOrder->PrimitiveType = ZE_ROPT_TRIANGLE;
			RenderOrder->VertexBufferOffset = sizeof(ZEMapVertex) * VertexIndex;
			RenderOrder->VertexBuffer = *VertexBuffer;
			RenderOrder->VertexDeclaration = ZEMapVertex::GetVertexDeclaration();
			ZEMatrix4x4::CreateIdentity(RenderOrder->WorldMatrix);
			size_t PrimitiveCount = 0;
			for (size_t N = I; N < Polygons.GetCount(); N++)
				if (Polygons[N].Material == MaterialId)
				{
					memcpy(Buffer + VertexIndex, Polygons[N].Vertices, sizeof(ZEMapVertex) * 3);
					Polygons[N].Material = 0xFFFFFFFF;
					VertexIndex += 3;
					PrimitiveCount++;
				}
			RenderOrder->PrimitiveCount = PrimitiveCount;
		}
	}
	(*VertexBuffer)->Unlock();

	return true;
}

bool ReadOctreeNodeFromFile(ZEResourceFile* ResourceFile, ZEOctree* Octree, ZEArray<ZEMaterial*>& Materials)
{
	ZEMapFileOctreeChunk	FileOctree;
	
	// Read octree header
	ResourceFile->Read(&FileOctree, sizeof(ZEMapFileOctreeChunk), 1);

	// Check chunk id
	if (FileOctree.ChunkIdentifier != ZE_MAP_OCTREE_CHUNK)
	{
		zeError("Map Resource", "Octree chunk's id does not match.");
		return false;
	}

	// Map octree attributes
	Octree->BoundingBox = FileOctree.BoundingBox;
	Octree->IsLeaf = FileOctree.IsLeaf;
	
	if (FileOctree.PolygonCount != 0)
	{
		// if node is leaf node then read polygon ids.
		ZEDWORD ChunkIndentifier;
		ResourceFile->Read(&ChunkIndentifier, sizeof(ZEDWORD), 1);

		// Check chunk indentifier
		if (ChunkIndentifier != ZE_MAP_OCTREE_POLYGONIDS_CHUNK)
		{
			zeError("Map Resource", "Octree Polygons' chunk id does not match.");
			return false;
		}

		// Read ids of polygons that is included in this octree node
		ZEArray<ZEMapFilePolygonChunk> MapPolygons;
		MapPolygons.SetCount(FileOctree.PolygonCount);
		ResourceFile->Read(MapPolygons.GetCArray(), sizeof(ZEMapFilePolygonChunk), MapPolygons.GetCount());
		if (!SortVertices((ZEStaticVertexBuffer**)&Octree->VertexBuffer, Octree->RenderOrders, MapPolygons, Materials))
			return false;

	}

	if(!FileOctree.IsLeaf)
	{
		// if node is not leaf recursively create and load sub nodes
		for(int I = 0; I < 8; I++)
			if(FileOctree.SubSpaces[I])
			{
				// if sub note is available create it and load it
				Octree->SubTrees[I] = new ZEOctree();
				Octree->SubTrees[I]->Depth = Octree->Depth + 1;
				if (!ReadOctreeNodeFromFile(ResourceFile, Octree->SubTrees[I], Materials))
					return false;
			}
			else
				Octree->SubTrees[I] = NULL;
	}

	return true;
}

bool ReadOctreeFromFile(ZEResourceFile* ResourceFile, ZEOctree** Octree, ZEArray<ZEMaterial*>& Materials)
{
	// Create octree
	*Octree = new ZEOctree();
	(*Octree)->Depth = 0;

	// Load it
	if (!ReadOctreeNodeFromFile(ResourceFile, *Octree, Materials))
		return false;

	return true;
}

bool ReadPhysicalMeshFromFile(ZEResourceFile* ResourceFile, ZEMapPortal* Portal)
{
	// Read physical mesh header
	ZEMapFilePhysicalMeshChunk FilePhysicalMesh;
	ResourceFile->Read(&FilePhysicalMesh, sizeof(ZEMapFilePhysicalMeshChunk), 1);

	// Check physical mesh chunk identifier
	if (FilePhysicalMesh.ChunkIdentifier != ZE_MAP_PHYSICAL_MESH_CHUNK)
	{
		zeError("Map Resource", "Physical mesh chunk's id does not match.");
		return false;
	}

	ZEArray<ZEVector3> Vertices;
	ZEArray<ZEMapFilePhysicalMeshPolygonChunk> Polygons;
	Polygons.SetCount(FilePhysicalMesh.PolygonCount);
	Vertices.SetCount(FilePhysicalMesh.VertexCount);

	ZEDWORD ChunkIdentifier = 0;

	// Check physical mesh vertices chunk identifier
	ResourceFile->Read(&ChunkIdentifier, sizeof(ZEDWORD), 1);
	if (ChunkIdentifier != ZE_MAP_PHYSICAL_MESH_VERTICES_CHUNK)
	{
		zeError("Map Resource", "Physical mesh vertices chunk's id does not match.");
		return false;
	}

	// Read physical mesh vertices
	ResourceFile->Read(Vertices.GetCArray(), sizeof(ZEVector3), Vertices.GetCount());

	// Check physical mesh polygons chunk identifier
	ResourceFile->Read(&ChunkIdentifier, sizeof(ZEDWORD), 1);
	if (ChunkIdentifier != ZE_MAP_PHYSICAL_MESH_POLYGONS_CHUNK)
	{
		zeError("Map Resource", "Physical mesh polygons chunk's id does not match.");
		return false;
	}
	// Read physical mesh polygons
	ResourceFile->Read(Polygons.GetCArray(), sizeof(ZEMapFilePhysicalMeshPolygonChunk), Polygons.GetCount());

	if (Polygons.GetCount() == 0 || Vertices.GetCount() == 0)
	{
		Portal->PhysicalMesh = NULL;
		return true;
	}

	// Create Physical Mesh
	Portal->PhysicalMesh = ZEPhysicalStaticMesh::CreateInstance();
	if (Portal->PhysicalMesh == NULL)
	{
		zeError("Map Resource", "Can not create physical static mesh");
		return false;
	}

	// Convert polygons to ZEPhysicalTriangle
	ZEArray<ZEPhysicalTriangle> PhysicalTriangles;
	PhysicalTriangles.SetCount(Polygons.GetCount());
	for (size_t I = 0; I < PhysicalTriangles.GetCount(); I++)
	{
		PhysicalTriangles[I].MaterialIndex = 0;
		PhysicalTriangles[I].Indices[0] = Polygons[I].Indices[0];
		PhysicalTriangles[I].Indices[1] = Polygons[I].Indices[1];
		PhysicalTriangles[I].Indices[2] = Polygons[I].Indices[2];
	}

	// Feed data to physical mesh
	if (!Portal->PhysicalMesh->SetData(Vertices.GetCArray(), Vertices.GetCount(), PhysicalTriangles.GetCArray(), PhysicalTriangles.GetCount(), NULL, 0))
	{
		zeError("Map Resource", "Can not set physical static mesh polygons.");
		return false;
	}

	return true;
}

bool ReadPortalsFromFile(ZEResourceFile* ResourceFile, ZEArray<ZEMapPortal>& Portals, ZEArray<ZEMaterial*>& Materials, ZEArray<ZEMapPortalDoor>& PortalDoors)
{
	ZEDWORD ChunkIdentifier;
	ZEMapFilePortalChunk FilePortal;

	for (size_t I = 0; I < Portals.GetCount(); I++)
	{
		ZEMapPortal* Portal = &Portals[I];

		ResourceFile->Read(&FilePortal, sizeof(ZEMapFilePortalChunk), 1);

		if (FilePortal.ChunkIdentifier != ZE_MAP_PORTAL_CHUNK)
		{
			zeError("Map Resource", "Portal chunk's id does not match.");
			return false;
		}

		strncpy(Portal->Name, FilePortal.Name, ZE_MAP_MAX_NAME_SIZE);
		Portal->BoundingBox = FilePortal.BoundingBox;
		Portal->Doors.SetCount(FilePortal.DoorCount);
		Portal->Polygons.SetCount(FilePortal.PolygonCount);
		Portal->HasPhysicalMesh = FilePortal.HasPhysicalMesh;
		Portal->HasOctree = FilePortal.HasOctree;
		Portal->Octree = NULL;

		// Read portal doors
		for (size_t N = 0; N < FilePortal.DoorCount; N++)
		{
			ZEMapPortalDoor* Door = &Portal->Doors[N];
			ZEMapFilePortalDoorChunk FileDoor;

			ResourceFile->Read(&FileDoor, sizeof(ZEMapFilePortalDoorChunk), 1);

			if (FileDoor.ChunkIdentifier != ZE_MAP_PORTAL_DOOR_CHUNK)
			{
				zeError("Map Resource", "Portal chunk's id does not match.");
				return false;
			}

			strncpy(Door->Name, FileDoor.Name, ZE_MAX_NAME_SIZE);
			Door->Rectangle = FileDoor.Rectangle;
			Door->DestinationPortal = &Portals[FileDoor.DestinationPortal];
			Door->IsOpen = FileDoor.IsOpen;
		}

		// Read Octree
		if (FilePortal.HasOctree)
		{
			if (!ReadOctreeFromFile(ResourceFile, &Portal->Octree, Materials))
				return false;
		}
		else
		{
			// Read chunk identifier and check it
			ZEDWORD ChunkIdentifier = 0;
			ResourceFile->Read(&ChunkIdentifier, sizeof(ZEDWORD), 1);
			if (ChunkIdentifier != ZE_MAP_POLYGONS_CHUNK)
			{
				zeError("Map Resource", "Polygons chunk's id does not match.");
				return false;
			}

			// Read polygons from file
			ZEArray<ZEMapFilePolygonChunk> MapPolygons;
			MapPolygons.SetCount(FilePortal.PolygonCount);
			ResourceFile->Read(MapPolygons.GetCArray(), sizeof(ZEMapFilePolygonChunk), MapPolygons.GetCount());
			for (size_t I = 0; I < Portal->Polygons.GetCount(); I++)
			{
				Portal->Polygons[I].Material = Materials[MapPolygons[I].Material];
				Portal->Polygons[I].LastIteration = 0;
				Portal->Polygons[I].Vertices[0] = *(ZEMapVertex*)&MapPolygons[I].Vertices[0];
				Portal->Polygons[I].Vertices[1] = *(ZEMapVertex*)&MapPolygons[I].Vertices[1];
				Portal->Polygons[I].Vertices[2] = *(ZEMapVertex*)&MapPolygons[I].Vertices[2];
			}
			
			if (!SortVertices((ZEStaticVertexBuffer**)&Portal->VertexBuffer, Portal->RenderOrders, MapPolygons, Materials))
				return false;
		}

		// Read Physical Mesh
		if (FilePortal.HasPhysicalMesh)
			if (!ReadPhysicalMeshFromFile(ResourceFile, Portal))
				return false;

	/*	// Read Brushes
		if (!ReadEntitiesFromFile(ResourceFile, Portal->Brushes))
			return false;*/
	}
	return true;
}

bool ReadMapFromFile(ZEResourceFile* ResourceFile, ZEMapResource* Map)
{
	ZEMapFileHeader TempHeader;
	ResourceFile->Read(&TempHeader, sizeof(ZEMapFileHeader), 1);

	if(TempHeader.Header!= ZE_MAP_HEADER)
	{
		zeError("Map Resource", "Unknown ZEMap file format. (FileName : \"%s\")", ResourceFile->GetFileName());
		return false;
	}
	
	if(TempHeader.Version != ZE_MAP_VERSION)
	{	
		zeError("Map Resource", "ZEMap file version mismatched. (FileName : \"%s\")", ResourceFile->GetFileName());
		return false;
	}

	Map->Portals.SetCount(TempHeader.PortalCount);
	Map->Materials.SetCount(TempHeader.MaterialCount);
	/*Map->Entities.SetCount(TempHeader.EntityCount);*/

	if (!ReadMaterialsFromFile(ResourceFile, Map->Materials, Map->TextureResources))
	{
		zeError("Map Resource", "File is corrupted. Can not read materials from file. (FileName : \"%s\")", ResourceFile->GetFileName());
		return false;
	}

	if (!ReadPortalsFromFile(ResourceFile, Map->Portals, Map->Materials, Map->PortalDoors))
	{
		zeError("Map Resource", "File is corrupted. Can not read portals from file. (FileName : \"%s\")", ResourceFile->GetFileName());
		return false;
	}

/*	if (!ReadEntitiesFromFile(ResourceFile, Map->Entities))
	{
		zeError("Map Resource", "File is corrupted. Can not read entities from file. (FileName : \"%s\")", ResourceFile->GetFileName());
		return false;
	}*/

	return true;
}

ZEMapPortal::ZEMapPortal()
{
	VertexBuffer = NULL;
	Octree = NULL;
}

ZEMapPortal::~ZEMapPortal()
{
	if (Octree != NULL)
		delete Octree;
}

const char* ZEMapResource::GetResourceType() const
{
	return "Map Resource";
}

const ZEMapResource* ZEMapResource::LoadSharedResource(const char* FileName)
{
	// Try to get instance of shared ZEMap file from resource manager
	ZEMapResource* Resource = (ZEMapResource*)zeResources->GetResource(FileName);
	
	if (Resource != NULL)
		return Resource;
	else
	{
		// If there is no shared instance of ZEMap file create and load new instance
		Resource = LoadResource(FileName);
		if (Resource != NULL)
		{
			// Flag as shared and add it to ZEResourceManager and return a instance
			Resource->Cached = false;
			Resource->ReferenceCount = 1;
			zeResources->AddResource(Resource);
			return Resource;
		}
		else
			return NULL;
	}
}

void ZEMapResource::CacheResource(const char* FileName)
{
	// Try to get instance of shared ZEMap file from resource manager
	ZEMapResource* Resource = (ZEMapResource*)zeResources->GetResource(FileName);
	if (Resource != NULL)
		Resource->Cached = true;
	else
	{
		// If there is no shared instance of ZEMap file create and load new instance
		Resource = LoadResource (FileName);
		if (Resource != NULL)
		{
			// Flag as cached and add it to ZEResourceManager
			Resource->Cached = true;
			Resource->ReferenceCount = 0;
			zeResources->AddResource(Resource);
		}
	}
}

ZEMapResource* ZEMapResource::LoadResource(const char* FileName)
{
	// Open ZEMap file
	ZEResourceFile ResourceFile;
	if (ResourceFile.Open(FileName))
	{
		// Create ZEMapResource
		ZEMapResource* MapResource = new ZEMapResource();
		MapResource->SetFileName(FileName);
		MapResource->Cached = false;
		MapResource->ReferenceCount = 0;

		if (!ReadMapFromFile(&ResourceFile, MapResource))
		{
			zeError("Map Resource", "Can not load map resource. (FileName : \"%s\")", FileName);
			ResourceFile.Close();
			delete MapResource;
			return NULL;
		}
		
		ResourceFile.Close();
		return MapResource;
	}
	else
	{
		zeError("Map Resource", "Map file does not exists. FileName : \"%s\"", FileName);
		return NULL;
	}
}

ZEMapResource::~ZEMapResource()
{
	for (size_t I = 0; I < TextureResources.GetCount(); I++)
		TextureResources[I]->Release();
	TextureResources.Clear();
}
