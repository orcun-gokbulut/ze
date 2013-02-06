//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPortalMapResource.cpp
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

#include "ZEPortalMapResource.h"
#include "ZEPortalMapFileFormat.h"

#include "ZEError.h"
#include "ZEFile/ZEFile.h"
#include "ZECore/ZEConsole.h"
#include "ZECore/ZEResource.h"
#include "ZECore/ZEResourceManager.h"
#include "ZETexture/ZETexture2DResource.h"
#include "ZERenderer/ZEFixedMaterial.h"
#include "ZEPhysics/ZEPhysicalMesh.h"


#include <string.h>
#include "ZEFile/ZEFileInfo.h"
#include "ZETexture/ZETextureCubeResource.h"

// Reading
#define ZE_SHADER_SKINTRANSFORM				1
#define ZE_SHADER_BASE_MAP					2
#define ZE_SHADER_NORMAL_MAP				4
#define ZE_SHADER_SPECULAR_MAP				8
#define ZE_SHADER_EMMISIVE_MAP				16
#define ZE_SHADER_OPACITY_MAP				32
#define ZE_SHADER_DETAIL_BASE_MAP			64
#define ZE_SHADER_DETAIL_NORMAL_MAP			128
#define ZE_SHADER_REFLECTION				256
#define ZE_SHADER_REFRACTION				512
#define ZE_SHADER_LIGHT_MAP					1024
#define ZE_SHADER_DISTORTION_MAP			2048

static ZEString ConstructResourcePath(const ZEString& Path)
{
	ZEString NewString = Path;
	ZESize ConstLength = strlen("resources\\") - 1;

	if (Path[0] == '\\' || Path[0] == '/')
		NewString = NewString.SubString(1, Path.GetLength() - 1);

	// If it is guaranteed that there is no "resources\\" string in beginning
	if (NewString.GetLength() - 1 < ConstLength)
	{
		NewString.Insert(0, "resources\\");
		return NewString;
	}
	// Else check if there is "resources\\" in the beginning
	else if (_stricmp("resources\\", Path.SubString(0, ConstLength)) != 0)
	{
		NewString.Insert(0, "resources\\");
		return NewString;
	}

	return NewString;
}

ZETexture2D* ZEPortalMapResource::ManageMapMaterialTextures(const ZEString& FileName)
{
	if (FileName == "")
		return NULL;

	for (ZESize I = 0; I < TextureResources.GetCount(); I++)
		if (TextureResources[I]->GetFileName() == FileName)
			return TextureResources[I]->GetTexture();

	ZETexture2DResource* NewTextureResource = ZETexture2DResource::LoadSharedResource(ZEFileInfo::GetParentDirectory(this->GetFileName()) + "\\" + FileName);
	if (NewTextureResource == NULL)
	{
		zeError("Can not load texture file. (FileName : \"%s\")", FileName.ToCString());
		return NULL;
	}
	TextureResources.Add(NewTextureResource);
	return NewTextureResource->GetTexture();
}

bool ZEPortalMapResource::ReadMaterialsFromFile(ZEFile* ResourceFile)
{
	ZEMapFileMaterialChunk MaterialChunk;
	// Read materials
	for (ZESize I = 0; I < Materials.GetCount(); I++)
	{
		// Read Material
		ResourceFile->Read(&MaterialChunk, sizeof(ZEMapFileMaterialChunk), 1);
		
		// Check Chunk Identifier
		if (MaterialChunk.ChunkIdentifier != ZE_MAP_MATERIAL_CHUNK)
		{
			zeError("Material chunk's id does not match.");
			return false;
		}
		
		// Fixed Material Hack
		ZEFixedMaterial* CurrentMaterial = ZEFixedMaterial::CreateInstance();
		
		Materials[I] = CurrentMaterial;

		CurrentMaterial->SetTwoSided(MaterialChunk.TwoSided);
		CurrentMaterial->SetWireframe(MaterialChunk.Wireframe);
		CurrentMaterial->SetLightningEnabled(MaterialChunk.LightningEnabled);

		//Specular
		// @NOTE: Hack to enable specular mapping
		// @NOTE: Specular factor always comes 0.0f from exporter
		MaterialChunk.SpecularFactor = 0.5f;
		MaterialChunk.SpecularColor = ZEVector3::One;
		CurrentMaterial->SetSpecularEnabled(true);
		CurrentMaterial->SetSpecularColor(MaterialChunk.SpecularColor);
		CurrentMaterial->SetSpecularFactor(MaterialChunk.SpecularFactor);
		CurrentMaterial->SetSpecularShininess(0.5f);
		
		if ((MaterialChunk.ShaderComponents & ZE_SHADER_SPECULAR_MAP) == ZE_SHADER_SPECULAR_MAP)
		{
			CurrentMaterial->SetSpecularMapEnabled(true);
			CurrentMaterial->SetSpecularMap(ManageMapMaterialTextures(MaterialChunk.SpecularMap));
		}
		
		// Emissive
		CurrentMaterial->SetEmissiveEnabled(true);
		CurrentMaterial->SetEmissiveColor(MaterialChunk.EmmisiveColor);
		CurrentMaterial->SetEmissiveFactor(MaterialChunk.EmmisiveFactor);

		if((MaterialChunk.ShaderComponents & ZE_SHADER_EMMISIVE_MAP) == ZE_SHADER_EMMISIVE_MAP)
		{
			CurrentMaterial->SetEmissiveMapEnabled(true);
			CurrentMaterial->SetEmissiveMap(ManageMapMaterialTextures(MaterialChunk.EmmisiveMap));
		}
		
		// Opacity
		CurrentMaterial->SetOpacityEnabled(true);
		CurrentMaterial->SetOpacity(MaterialChunk.Transparancy);
		CurrentMaterial->SetTransparancyMode(MaterialChunk.Transparant ? ZE_MTM_ADDAPTIVE : ZE_MTM_NONE);

		if ((MaterialChunk.ShaderComponents & ZE_SHADER_OPACITY_MAP) == ZE_SHADER_OPACITY_MAP)
		{
			CurrentMaterial->SetOpacityMap(ManageMapMaterialTextures(MaterialChunk.EmmisiveMap));
		}

		// Diffuse
		CurrentMaterial->SetDiffuseEnabled(true);
		CurrentMaterial->SetDiffuseFactor(1.0f);
		CurrentMaterial->SetDiffuseSubSurfaceScatteringFactor(0.0f);
		CurrentMaterial->SetDiffuseColor(MaterialChunk.DiffuseColor);
	
		// Ambient
		// @NOTE: Hack to enable ambient, not all materials comes with proper ambient color and factor
		CurrentMaterial->SetAmbientEnabled(true);
		CurrentMaterial->SetAmbientFactor(0.1f);
		CurrentMaterial->SetAmbientColor(MaterialChunk.AmbientColor);
	
		// Base map
		if ((MaterialChunk.ShaderComponents & ZE_SHADER_BASE_MAP) == ZE_SHADER_BASE_MAP)
		{
			CurrentMaterial->SetBaseMapEnabled(true);
			CurrentMaterial->SetBaseMap(ManageMapMaterialTextures(MaterialChunk.DiffuseMap));
		}
		
		// Detail map
		// @NOTE: These parameters are hard coded since exporter does not support them yet
		CurrentMaterial->SetDetailDistance(1.0f);
		CurrentMaterial->SetDetailFadeDistance(3.0f);
		CurrentMaterial->SetDetailMapTiling(MaterialChunk.DetailMapTiling);

		if ((MaterialChunk.ShaderComponents & ZE_SHADER_DETAIL_BASE_MAP) == ZE_SHADER_DETAIL_BASE_MAP)
		{
			CurrentMaterial->SetDetailBaseMapEnabled(true);
			CurrentMaterial->SetDetailBaseMap(ManageMapMaterialTextures(MaterialChunk.DetailMap));
		}
		if ((MaterialChunk.ShaderComponents & ZE_SHADER_DETAIL_NORMAL_MAP) == ZE_SHADER_DETAIL_NORMAL_MAP)
		{
			CurrentMaterial->SetDetailNormalMapEnabled(true);
			CurrentMaterial->SetDetailNormalMap(ManageMapMaterialTextures(MaterialChunk.DetailNormalMap));
		}

		// NormalMap
		if ((MaterialChunk.ShaderComponents & ZE_SHADER_NORMAL_MAP) == ZE_SHADER_NORMAL_MAP)
		{
			CurrentMaterial->SetNormalMapEnabled(true);
			CurrentMaterial->SetNormalMap(ManageMapMaterialTextures(MaterialChunk.NormalMap));
		}

		// Light Map
		if ((MaterialChunk.ShaderComponents & ZE_SHADER_LIGHT_MAP) == ZE_SHADER_LIGHT_MAP)
		{
			CurrentMaterial->SetLightMapEnabled(true);
			CurrentMaterial->SetLightMap(ManageMapMaterialTextures(MaterialChunk.LightMap));
		}

		// Reflection
		if ((MaterialChunk.ShaderComponents & ZE_SHADER_REFLECTION) == ZE_SHADER_REFLECTION)
		{

			CurrentMaterial->SetReflectionEnabled(true);
			CurrentMaterial->SetReflectionFactor(MaterialChunk.ReflectionFactor);
		}

		// Refraction
		if ((MaterialChunk.ShaderComponents & ZE_SHADER_REFRACTION) == ZE_SHADER_REFRACTION)
		{

			CurrentMaterial->SetRefractionEnabled(true);
			CurrentMaterial->SetRefractionFactor(0.5f);
			CurrentMaterial->SetRefractionIndex(0.5f);
			CurrentMaterial->SetRefractionFactor(MaterialChunk.RefractionFactor);
		}

		CurrentMaterial->UpdateMaterial();

	}
	return true;
}

bool ZEPortalMapResource::ReadPhysicalMeshFromFile(ZEFile* ResourceFile, ZEPortalMapResourcePortal* Portal)
{
	// Read physical mesh header
	ZEMapFilePhysicalMeshChunk FilePhysicalMesh;
	ResourceFile->Read(&FilePhysicalMesh, sizeof(ZEMapFilePhysicalMeshChunk), 1);

	// Check physical mesh chunk identifier
	if (FilePhysicalMesh.ChunkIdentifier != ZE_MAP_PHYSICAL_MESH_CHUNK)
	{
		zeError("Physical mesh chunk's id does not match.");
		return false;
	}

	ZEUInt32 ChunkIdentifier = 0;

	// Check physical mesh vertices chunk identifier
	ResourceFile->Read(&ChunkIdentifier, sizeof(ZEUInt32), 1);
	if (ChunkIdentifier != ZE_MAP_PHYSICAL_MESH_VERTICES_CHUNK)
	{
		zeError("Physical mesh vertices chunk's id does not match.");
		return false;
	}

	// Read physical mesh vertices
	Portal->PhysicalMesh.Vertices.SetCount((ZESize)FilePhysicalMesh.VertexCount);
	ResourceFile->Read(Portal->PhysicalMesh.Vertices.GetCArray(), sizeof(ZEMapFilePhysicalMeshPolygonChunk), Portal->PhysicalMesh.Vertices.GetCount());


	// Check physical mesh polygons chunk identifier
	ResourceFile->Read(&ChunkIdentifier, sizeof(ZEUInt32), 1);
	if (ChunkIdentifier != ZE_MAP_PHYSICAL_MESH_POLYGONS_CHUNK)
	{
		zeError("Physical mesh polygons chunk's id does not match.");
		return false;
	}

	// Read physical mesh polygons
	Portal->PhysicalMesh.Polygons.SetCount((ZESize)FilePhysicalMesh.PolygonCount);
	for (ZESize I = 0; I < (ZESize)FilePhysicalMesh.PolygonCount; I++)
	{
		ZEMapFilePhysicalMeshPolygonChunk Chunk;
		ResourceFile->Read(&Chunk, sizeof(ZEMapFilePhysicalMeshPolygonChunk), 1);
		Portal->PhysicalMesh.Polygons[I].Indices[0] = Chunk.Indices[0];
		Portal->PhysicalMesh.Polygons[I].Indices[1] = Chunk.Indices[1];
		Portal->PhysicalMesh.Polygons[I].Indices[2] = Chunk.Indices[2];
	}

	return true;
}

bool ZEPortalMapResource::ReadPortalsFromFile(ZEFile* ResourceFile)
{
	ZEMapFilePortalChunk FilePortal;

	for (ZESize I = 0; I < Portals.GetCount(); I++)
	{
		ZEPortalMapResourcePortal* Portal = &Portals[I];

		ResourceFile->Read(&FilePortal, sizeof(ZEMapFilePortalChunk), 1);

		if (FilePortal.ChunkIdentifier != ZE_MAP_PORTAL_CHUNK)
		{
			zeError("Portal chunk's id does not match.");
			return false;
		}

		strncpy(Portal->Name, FilePortal.Name, ZE_MAP_MAX_NAME_SIZE);
		Portal->BoundingBox = FilePortal.BoundingBox;
		Portal->Polygons.SetCount((ZESize)FilePortal.PolygonCount);
		Portal->HasPhysicalMesh = FilePortal.HasPhysicalMesh;
		Portal->HasOctree = FilePortal.HasOctree;
//		Portal->Octree = NULL;


		// Read Octree
		if (FilePortal.HasOctree)
		{
			zeError("Octree is not supported.");
			return false;
			/*if (!ReadOctreeFromFile(ResourceFile, &Portal->Octree, Materials))
				return false;*/
		}
		else
		{
			// Read chunk identifier and check it
			ZEUInt32 ChunkIdentifier = 0;
			ResourceFile->Read(&ChunkIdentifier, sizeof(ZEUInt32), 1);
			if (ChunkIdentifier != ZE_MAP_POLYGONS_CHUNK)
			{
				zeError("Polygons chunk's id does not match.");
				return false;
			}

			// Read polygons from file
			ZEArray<ZEMapFilePolygonChunk> MapPolygons;
			MapPolygons.SetCount((ZESize)FilePortal.PolygonCount);
			ResourceFile->Read(MapPolygons.GetCArray(), sizeof(ZEMapFilePolygonChunk), MapPolygons.GetCount());
			for (ZESize I = 0; I < Portal->Polygons.GetCount(); I++)
			{
				Portal->Polygons[I].LastIteration	= 0;
				Portal->Polygons[I].Material			= Materials[(ZESize)MapPolygons[I].Material];
				
				Portal->Polygons[I].Vertices[0]		= *(ZEMapVertex*)&MapPolygons[I].Vertices[0];
				Portal->Polygons[I].Vertices[1]		= *(ZEMapVertex*)&MapPolygons[I].Vertices[1];
				Portal->Polygons[I].Vertices[2]		= *(ZEMapVertex*)&MapPolygons[I].Vertices[2];
			}
		}

		// Read Physical Mesh
		if (FilePortal.HasPhysicalMesh)
			if (!ReadPhysicalMeshFromFile(ResourceFile, Portal))
				return false;
	}
	return true;
}

bool ZEPortalMapResource::ReadDoorsFromFile(ZEFile* ResourceFile)
{ 
	for (ZESize I = 0; I < Doors.GetCount(); I++)
	{
		ZEPortalMapResourceDoor* Door = &Doors[I];
		
		ZEMapFileDoorChunk FileDoor;
		ResourceFile->Read(&FileDoor, sizeof(ZEMapFileDoorChunk), 1);

		if (FileDoor.ChunkIdentifier != ZE_MAP_PORTAL_DOOR_CHUNK)
		{
			zeError("Portal chunk's id does not match.");
			return false;
		}

		strncpy(Door->Name, FileDoor.Name, ZE_MAX_NAME_SIZE);
		Door->Rectangle = FileDoor.Rectangle;
		Door->IsOpen = FileDoor.IsOpen;

		Door->PortalIds[0] = FileDoor.PortalIds[0];
		Door->Portals[0] = &Portals[(ZESize)Door->PortalIds[0]];
		Door->Portals[0]->DoorIds.Add(I);
		Door->Portals[0]->Doors.Add(Door);

		Door->PortalIds[1] = FileDoor.PortalIds[1];
		Door->Portals[1] = &Portals[(ZESize)Door->PortalIds[1]];
		Door->Portals[1]->DoorIds.Add(I);
		Door->Portals[1]->Doors.Add(Door);
	}

	return true;
}

bool ZEPortalMapResource::ReadMapFromFile(ZEFile* ResourceFile)
{
	zeLog("Loading map file \"%s\".", ResourceFile->GetPath().GetValue());

	ZEMapFileHeader TempHeader;
	ResourceFile->Read(&TempHeader, sizeof(ZEMapFileHeader), 1);

	if(TempHeader.Header!= ZE_MAP_HEADER)
	{
		zeError("Unknown ZEMap file format. (FileName : \"%s\")", ResourceFile->GetPath().GetValue());
		return false;
	}
	
	if(TempHeader.Version != ZE_MAP_VERSION)
	{	
		zeError("ZEMap file version mismatched. (FileName : \"%s\")", ResourceFile->GetPath().GetValue());
		return false;
	}

	Portals.SetCount((ZESize)TempHeader.PortalCount);
	Doors.SetCount((ZESize)TempHeader.DoorCount);
	Materials.SetCount((ZESize)TempHeader.MaterialCount);

	if (!ReadMaterialsFromFile(ResourceFile))
	{
		zeError("File is corrupted. Can not read materials from file. (FileName : \"%s\")", ResourceFile->GetPath().GetValue());
		return false;
	}

	if (!ReadPortalsFromFile(ResourceFile))
	{
		zeError("File is corrupted. Can not read portals from file. (FileName : \"%s\")", ResourceFile->GetPath().GetValue());
		return false;
	}

	if (!ReadDoorsFromFile(ResourceFile))
	{
		zeError("File is corrupted. Can not read doors from file. (FileName : \"%s\")", ResourceFile->GetPath().GetValue());
		return false;
	}

	zeLog("Map file \"%s\" has been loaded.", ResourceFile->GetPath().GetValue());

	return true;
}

const char* ZEPortalMapResource::GetResourceType() const
{
	return "Portal Map Resource";
}

const ZEArray<ZETexture2DResource*>& ZEPortalMapResource::GetTextures()
{
	return TextureResources;
}

const ZEArray<ZEMaterial*>& ZEPortalMapResource::GetMaterials()
{
	return Materials;
}

const ZEArray<ZEPortalMapResourcePortal>& ZEPortalMapResource::GetPortals()
{
	return Portals;
}

const ZEArray<ZEPortalMapResourceDoor>& ZEPortalMapResource::GetDoors()
{
	return Doors;
}

ZEPortalMapResource* ZEPortalMapResource::LoadSharedResource(const ZEString& FileName)
{
	ZEString NewPath = ConstructResourcePath(FileName);

	// Try to get instance of shared ZEMap file from resource manager
	ZEPortalMapResource* Resource = (ZEPortalMapResource*)zeResources->GetResource(NewPath);
	
	if (Resource != NULL)
		return Resource;
	else
	{
		// If there is no shared instance of ZEMap file create and load new instance
		Resource = LoadResource(NewPath);
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

void ZEPortalMapResource::CacheResource(const ZEString& FileName)
{
	ZEString NewPath = ConstructResourcePath(FileName);

	// Try to get instance of shared ZEMap file from resource manager
	ZEPortalMapResource* Resource = (ZEPortalMapResource*)zeResources->GetResource(NewPath);
	if (Resource != NULL)
		Resource->Cached = true;
	else
	{
		// If there is no shared instance of ZEMap file create and load new instance
		Resource = LoadResource(NewPath);
		if (Resource != NULL)
		{
			// Flag as cached and add it to ZEResourceManager
			Resource->Cached = true;
			Resource->ReferenceCount = 0;
			zeResources->AddResource(Resource);
		}
	}
}

ZEPortalMapResource* ZEPortalMapResource::LoadResource(const ZEString& FileName)
{
	ZEString NewPath = ConstructResourcePath(FileName);

	bool Result;
	ZEFile ResourceFile;


	// Open ZEMap file
	Result = ResourceFile.Open(NewPath, ZE_FOM_READ, ZE_FCM_NONE);
	if (Result)
	{
		// Create ZEMapResource
		ZEPortalMapResource* MapResource = new ZEPortalMapResource();
		MapResource->SetFileName(NewPath);
		MapResource->Cached = false;
		MapResource->ReferenceCount = 0;

		if (!MapResource->ReadMapFromFile(&ResourceFile))
		{
			zeError("Can not load map resource. (FileName : \"%s\")", NewPath.GetValue());
			ResourceFile.Close();
			delete MapResource;
			return NULL;
		}
		
		ResourceFile.Close();

		return MapResource;
	}
	else
	{
		zeError("Map file does not exists. FileName : \"%s\"", NewPath.GetValue());
		return NULL;
	}
}

ZEPortalMapResource::~ZEPortalMapResource()
{
	for (ZESize I = 0; I < TextureResources.GetCount(); I++)
		TextureResources[I]->Release();
	TextureResources.Clear();
}




