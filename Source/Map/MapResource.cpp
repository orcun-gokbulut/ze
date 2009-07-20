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

// Reading

const ZETexture* ManageMapMaterialTextures(char* Filename, ZESmartArray<ZETextureResource*>& TextureResources)
{
	if (strncmp(Filename, "", ZE_MAP_MAX_FILENAME_SIZE) == 0)
		return NULL;

	for (size_t I = 0; I < TextureResources.GetCount(); I++)
		if (strnicmp(TextureResources[I]->GetFileName(), Filename, ZE_MAP_MAX_FILENAME_SIZE) == 0)
			return TextureResources[I]->GetTexture();

	ZETextureResource* NewTextureResource = ZETextureResource::LoadSharedResource(Filename);
	if (NewTextureResource == NULL)
	{
		zeError("Map Resource", "Can not load texture file. (Filename : \"%s\")", Filename);
		return NULL;
	}
	TextureResources.Add(NewTextureResource);
	return NewTextureResource->GetTexture();
}

bool ReadMaterialsFromFile(ZEResourceFile* ResourceFile, ZEArray<ZEDefaultMaterial>& Materials, ZESmartArray<ZETextureResource*>& TextureResources)
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
		ZEDefaultMaterial* CurrentMaterial = &Materials[I];

		CurrentMaterial->SetShaderComponents(MaterialChunk.ShaderComponents);
		
		CurrentMaterial->TwoSided = MaterialChunk.TwoSided;
		CurrentMaterial->LightningEnabled = MaterialChunk.LightningEnabled;
		CurrentMaterial->Wireframe = MaterialChunk.Wireframe;
		CurrentMaterial->TransparancyMode = (MaterialChunk.Transparant ? ZE_TM_ADDAPTIVE: ZE_TM_NOTRANSPARACY);

		CurrentMaterial->AmbientColor = MaterialChunk.AmbientColor;
		CurrentMaterial->DiffuseColor = MaterialChunk.DiffuseColor;
		CurrentMaterial->SpecularColor = MaterialChunk.SpecularColor;
		CurrentMaterial->EmmisiveColor = MaterialChunk.EmmisiveColor;
		CurrentMaterial->EmmisiveFactor = MaterialChunk.EmmisiveFactor;
		CurrentMaterial->SpecularFactor = (1.25f - MaterialChunk.SpecularFactor) * 128.0f;
		CurrentMaterial->Opasity = MaterialChunk.Transparancy;
		CurrentMaterial->ReflectionFactor = MaterialChunk.ReflectionFactor;
		CurrentMaterial->RefractionFactor = MaterialChunk.RefractionFactor;
		CurrentMaterial->DetailMapTiling = MaterialChunk.DetailMapTiling;

		CurrentMaterial->DiffuseMap = ManageMapMaterialTextures(MaterialChunk.DiffuseMap, TextureResources);
		CurrentMaterial->NormalMap = ManageMapMaterialTextures(MaterialChunk.NormalMap, TextureResources);
		CurrentMaterial->SpecularMap = ManageMapMaterialTextures(MaterialChunk.SpecularMap, TextureResources);
		CurrentMaterial->EmmisiveMap = ManageMapMaterialTextures(MaterialChunk.EmmisiveMap, TextureResources);
		CurrentMaterial->OpacityMap = ManageMapMaterialTextures(MaterialChunk.OpacityMap, TextureResources);
		CurrentMaterial->DetailMap = ManageMapMaterialTextures(MaterialChunk.DetailMap, TextureResources);
		CurrentMaterial->DetailNormalMap = ManageMapMaterialTextures(MaterialChunk.DetailNormalMap, TextureResources);
		CurrentMaterial->EnvironmentMap = NULL;//ManageMapMaterialTextures(MaterialChunk.EnvironmentMap, TextureResources);
		CurrentMaterial->LightMap = ManageMapMaterialTextures(MaterialChunk.LightMap, TextureResources);
	}
	return true;
}

bool SortVertices(ZEStaticVertexBuffer** VertexBuffer, ZEArray<ZERenderList>& RenderLists, ZEArray<ZEMapFilePolygonChunk>& Polygons, ZEArray<ZEDefaultMaterial>& Materials)
{
	if (*VertexBuffer == NULL)
		*VertexBuffer = zeGraphics->CreateStaticVertexBuffer();

	if (!(*VertexBuffer)->Create(Polygons.GetCount() * 3 * sizeof(ZEMapVertex)))
		return false;

	ZEMapVertex* Buffer = (ZEMapVertex*)(*VertexBuffer)->Lock();
	
	size_t VertexIndex = 0;
	int RenderListIndex = -1;
	for (size_t I = 0; I < Polygons.GetCount(); I++)
	{
		if (Polygons[I].Material != 0xFFFFFFFF)
		{
			size_t MaterialId = Polygons[I].Material;
			ZERenderList* RenderList = RenderLists.Add();
			RenderList->SetZero();
			RenderList->Flags = ZE_RLF_ENABLE_VIEWPROJECTION_TRANSFORM | ZE_RLF_ENABLE_ZCULLING;
			RenderList->Material = &Materials[Polygons[I].Material];
			RenderList->PrimitiveType = ZE_RLPT_TRIANGLE;
			RenderList->VertexBufferOffset = sizeof(ZEMapVertex) * VertexIndex;
			RenderList->VertexBuffer = *VertexBuffer;
			RenderList->VertexType = ZE_VT_MAPVERTEX;
			ZEMatrix4x4::CreateIdentity(RenderList->WorldMatrix);
			size_t PrimitiveCount = 0;
			for (size_t N = I; N < Polygons.GetCount(); N++)
				if (Polygons[N].Material == MaterialId)
				{
					memcpy(Buffer + VertexIndex, Polygons[N].Vertices, sizeof(ZEMapVertex) * 3);
					Polygons[N].Material = 0xFFFFFFFF;
					VertexIndex += 3;
					PrimitiveCount++;
				}
			RenderList->PrimitiveCount = PrimitiveCount;
		}
	}
	(*VertexBuffer)->Unlock();

	return true;
}

bool ReadOctreeNodeFromFile(ZEResourceFile* ResourceFile, ZEOctree* Octree, ZEArray<ZEDefaultMaterial>& Materials)
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
		if (!SortVertices((ZEStaticVertexBuffer**)&Octree->VertexBuffer, Octree->RenderLists, MapPolygons, Materials))
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

bool ReadOctreeFromFile(ZEResourceFile* ResourceFile, ZEOctree** Octree, ZEArray<ZEDefaultMaterial>& Materials)
{
	// Create octree
	*Octree = new ZEOctree();
	(*Octree)->Depth = 0;

	// Load it
	if (!ReadOctreeNodeFromFile(ResourceFile, *Octree, Materials))
		return false;

	return true;
}

bool ReadEntitiesFromFile(ZEResourceFile* ResourceFile, ZEArray<ZEEntityData>& Entities)
{
	ZEDWORD ChunkIdentifier = ZE_MAP_ENTITY_CHUNK;
	
	for (size_t I = 0; I < Entities.GetCount(); I++)
	{
		// Read EntityData header
		ZEEntityDataChunk FileEntityData;
		ZEEntityData* EntityData = &Entities[I];

		ResourceFile->Read(&FileEntityData, sizeof(ZEEntityDataChunk), 1);
		if (FileEntityData.ChunkIndentifier != ZE_MAP_ENTITY_CHUNK)
		{
			zeError("Map Resource", "Entity Data chunk's id does not match.");
			return false;

		}
		strncpy(EntityData->EntityType, FileEntityData.EntityType, ZE_MAP_MAX_NAME_SIZE);
		EntityData->Attributes.SetCount(FileEntityData.AttributeCount);


		// Read Attributes
		for (size_t N = 0; N < FileEntityData.AttributeCount; N++)
		{
			ZEEntityAttributeChunk FileAttribute;
			ZEEntityAttribute* Attribute = &EntityData->Attributes[N];

			ResourceFile->Read(&FileAttribute, sizeof(ZEEntityAttributeChunk), 1);

			// Check chunk identifier for corrupution
			if (FileAttribute.ChunkIndentifier = ZE_MAP_ENTITY_ATTRIBUTE_CHUNK)
			{
				zeError("Map Resource", "Entity Data chunk's id does not match.");
				return false;
			}

//			strncpy(Attribute->Name, FileAttribute.Name, ZE_MAP_MAX_NAME_SIZE);
			Attribute->Type = FileAttribute.Type;
			Attribute->Value.Unserialize(ResourceFile);
		}
	}
	return true;
}

bool ReadPhysicalMeshFromFile(ZEResourceFile* ResourceFile, ZEMapPhysicalMesh& PhysicalMesh)
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

	PhysicalMesh.Polygons.SetCount(FilePhysicalMesh.PolygonCount);
	PhysicalMesh.Vertices.SetCount(FilePhysicalMesh.VertexCount);

	ZEDWORD ChunkIdentifier = 0;

	// Check physical mesh vertices chunk identifier
	ResourceFile->Read(&ChunkIdentifier, sizeof(ZEDWORD), 1);
	if (ChunkIdentifier != ZE_MAP_PHYSICAL_MESH_VERTICES_CHUNK)
	{
		zeError("Map Resource", "Physical mesh vertices chunk's id does not match.");
		return false;
	}

	// Read physical mesh vertices
	ResourceFile->Read(PhysicalMesh.Vertices.GetCArray(), sizeof(ZEVector3), PhysicalMesh.Vertices.GetCount());

	// Check physical mesh polygons chunk identifier
	ResourceFile->Read(&ChunkIdentifier, sizeof(ZEDWORD), 1);
	if (ChunkIdentifier != ZE_MAP_PHYSICAL_MESH_POLYGONS_CHUNK)
	{
		zeError("Map Resource", "Physical mesh polygons chunk's id does not match.");
		return false;
	}

	// Read physical mesh polygons
	ResourceFile->Read(PhysicalMesh.Polygons.GetCArray(), sizeof(ZEMapFilePhysicalMeshPolygonChunk), PhysicalMesh.Polygons.GetCount());

	return true;
}

bool ReadPortalsFromFile(ZEResourceFile* ResourceFile, ZEArray<ZEMapPortal>& Portals, ZEArray<ZEDefaultMaterial>& Materials, ZEArray<ZEMapPortalDoor>& PortalDoors)
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
		Portal->Brushes.SetCount(FilePortal.BrushCount);
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
				Portal->Polygons[I].Material = &Materials[MapPolygons[I].Material];
				Portal->Polygons[I].LastIteration = 0;
				Portal->Polygons[I].Vertices[0] = *(ZEMapVertex*)&MapPolygons[I].Vertices[0];
				Portal->Polygons[I].Vertices[1] = *(ZEMapVertex*)&MapPolygons[I].Vertices[1];
				Portal->Polygons[I].Vertices[2] = *(ZEMapVertex*)&MapPolygons[I].Vertices[2];
			}
			
			if (!SortVertices((ZEStaticVertexBuffer**)&Portal->VertexBuffer, Portal->RenderLists, MapPolygons, Materials))
				return false;
		}

		// Read Physical Mesh
		if (FilePortal.HasPhysicalMesh)
			if (!ReadPhysicalMeshFromFile(ResourceFile, Portal->PhysicalMesh))
				return false;

		// Read Brushes
		if (!ReadEntitiesFromFile(ResourceFile, Portal->Brushes))
			return false;
	}
	return true;
}

bool ReadMapFromFile(ZEResourceFile* ResourceFile, ZEMapResource* Map)
{
	ZEMapFileHeader TempHeader;
	ResourceFile->Read(&TempHeader, sizeof(ZEMapFileHeader), 1);

	if(TempHeader.Header!= ZE_MAP_HEADER)
	{
		zeError("Map Resource", "Unknown ZEMap file format. (Filename : \"%s\")", ResourceFile->GetFilename());
		return false;
	}
	
	if(TempHeader.Version != ZE_MAP_VERSION)
	{	
		zeError("Map Resource", "ZEMap file version mismatched. (Filename : \"%s\")", ResourceFile->GetFilename());
		return false;
	}

	Map->Portals.SetCount(TempHeader.PortalCount);
	Map->Materials.SetCount(TempHeader.MaterialCount);
	Map->Entities.SetCount(TempHeader.EntityCount);

	if (!ReadMaterialsFromFile(ResourceFile, Map->Materials, Map->TextureResources))
	{
		zeError("Map Resource", "File is corrupted. Can not read materials from file. (Filename : \"%s\")", ResourceFile->GetFilename());
		return false;
	}

	if (!ReadPortalsFromFile(ResourceFile, Map->Portals, Map->Materials, Map->PortalDoors))
	{
		zeError("Map Resource", "File is corrupted. Can not read portals from file. (Filename : \"%s\")", ResourceFile->GetFilename());
		return false;
	}

	if (!ReadEntitiesFromFile(ResourceFile, Map->Entities))
	{
		zeError("Map Resource", "File is corrupted. Can not read entities from file. (Filename : \"%s\")", ResourceFile->GetFilename());
		return false;
	}

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

const ZEMapResource* ZEMapResource::LoadSharedResource(const char* Filename)
{
	// Try to get instance of shared ZEMap file from resource manager
	ZEMapResource* Resource = (ZEMapResource*)zeResources->GetResource(Filename);
	
	if (Resource != NULL)
		return Resource;
	else
	{
		// If there is no shared instance of ZEMap file create and load new instance
		Resource = LoadResource(Filename);
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

void ZEMapResource::CacheResource(const char* Filename)
{
	// Try to get instance of shared ZEMap file from resource manager
	ZEMapResource* Resource = (ZEMapResource*)zeResources->GetResource(Filename);
	if (Resource != NULL)
		Resource->Cached = true;
	else
	{
		// If there is no shared instance of ZEMap file create and load new instance
		Resource = LoadResource (Filename);
		if (Resource != NULL)
		{
			// Flag as cached and add it to ZEResourceManager
			Resource->Cached = true;
			Resource->ReferenceCount = 0;
			zeResources->AddResource(Resource);
		}
	}
}

ZEMapResource* ZEMapResource::LoadResource(const char* Filename)
{
	ZEDWORD ChunkId;

	// Open ZEMap file
	ZEResourceFile ResourceFile;
	if (ResourceFile.Open(Filename))
	{
		// Create ZEMapResource
		ZEMapResource* MapResource = new ZEMapResource();
		MapResource->SetFilename(Filename);
		MapResource->Cached = false;
		MapResource->ReferenceCount = 0;

		if (!ReadMapFromFile(&ResourceFile, MapResource))
		{
			zeError("Map Resource", "Can not load map resource. (Filename : \"%s\")", Filename);
			ResourceFile.Close();
			delete MapResource;
			return NULL;
		}
		
		ResourceFile.Close();
		return MapResource;
	}
	else
	{
		zeError("Map Resource", "Map file does not exists. Filename : \"%s\"", Filename);
		return NULL;
	}
}

ZEMapResource::~ZEMapResource()
{
	for (size_t I = 0; I < TextureResources.GetCount(); I++)
		TextureResources[I]->Release();
	TextureResources.Clear();
}
