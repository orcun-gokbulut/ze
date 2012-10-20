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

#include "ZEError.h"
#include "ZEFile/ZEFile.h"
#include "ZECore/ZEConsole.h"
#include "ZECore/ZEResource.h"
#include "ZECore/ZEResourceManager.h"
#include "ZETexture/ZETexture2DResource.h"
#include "ZEGraphics/ZEFixedMaterial.h"
#include "ZEPhysics/ZEPhysicalMesh.h"
#include <string.h>
#include "ZEFile/ZEFileInfo.h"
#include "ZEML/ZEMLSerialReader.h"
#include "ZEFile/ZEPathUtils.h"
#include "ZEFoundation/ZEPacking.h"

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

const ZETexture2D* ZEPortalMapResource::ManageMapMaterialTextures(const ZEString& FileName)
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

bool ZEPortalMapResource::ReadMapFromFile(ZEFile* ResourceFile)
{ 
	ZEMLSerialReader Reader(ResourceFile);

	if(!Reader.Read())
	{
		zeError("Can not read map file.");
		return false;
	}

	if(Reader.GetItemName() != ZEString("ZEMap"))
	{
		zeError("Resource file is not a map file, file name : %s", ResourceFile->GetPath());
		return false;
	}

	ZEMLSerialPointer PortalsPointer, DoorsPointer, MaterialsPointer;
	ZEVariant DoorCount, PortalCount, MaterialCount;

	ZEMLSerialListItem MapList[] = {
		ZEML_LIST_NODE("Portals",			PortalsPointer,		true),
		ZEML_LIST_NODE("Doors",				DoorsPointer,		true),
		ZEML_LIST_NODE("Materials",			MaterialsPointer,	true),
		ZEML_LIST_PROPERTY("DoorCount",		DoorCount,			ZE_VRT_UNSIGNED_INTEGER_32,	true),
		ZEML_LIST_PROPERTY("PortalCount",	PortalCount,		ZE_VRT_UNSIGNED_INTEGER_32,	true),
		ZEML_LIST_PROPERTY("MaterialCount", MaterialCount,		ZE_VRT_UNSIGNED_INTEGER_32,	true)
	};

	if(!Reader.ReadPropertyList(MapList, 6))
	{
		zeError("Can not read map from file.");
		return false;
	}

	Doors.SetCount(DoorCount.GetUInt32());
	Portals.SetCount(PortalCount.GetUInt32());
	Materials.SetCount(MaterialCount.GetUInt32());

	Reader.SeekPointer(MaterialsPointer);
	if(!ReadMaterials(&Reader))
	{
		zeError("Can not read materials from file.");
		return false;
	}

	Reader.SeekPointer(PortalsPointer);
	if(!ReadPortals(&Reader))
	{
		zeError("Can not read portals from file.");
		return false;
	}

	Reader.SeekPointer(DoorsPointer);
	if(!ReadPortalDoors(&Reader))
	{
		zeError("Can not read portal doors from file.");
		return false;
	}

	Reader.GoToCurrentPointer();

	return true;
}

bool ZEPortalMapResource::ReadPortalDoors(ZEMLSerialReader* Reader)
{
	ZEUInt32 DoorCounter = 0;

	while(Reader->Read())
	{
		if(Reader->GetItemName() != "Door")
			break;

		ZEVariant DoorName, IsOpen, PortalAIndex, PortalBIndex, Point1, Point2, Point3, Point4;
		ZEMLSerialPointer RectanglePointer;

		ZEMLSerialListItem DoorPropertiesList[] = { 
			ZEML_LIST_PROPERTY("Name",			DoorName,		ZE_VRT_STRING,	true),  
			ZEML_LIST_PROPERTY("IsOpen",		IsOpen,			ZE_VRT_BOOLEAN, 	true),
			ZEML_LIST_PROPERTY("PortalAIndex",	PortalAIndex,	ZE_VRT_UNSIGNED_INTEGER_32,	true),  
			ZEML_LIST_PROPERTY("PortalBIndex",	PortalBIndex,	ZE_VRT_UNSIGNED_INTEGER_32,	true),
			ZEML_LIST_NODE("Rectangle",			RectanglePointer,	true)
		};

		Reader->ReadPropertyList(DoorPropertiesList, 5);

		ZEMLSerialListItem RectangleList[] = { 
			ZEML_LIST_PROPERTY("Point1", Point1, ZE_VRT_VECTOR3, true),  
			ZEML_LIST_PROPERTY("Point2", Point2, ZE_VRT_VECTOR3, true),
			ZEML_LIST_PROPERTY("Point3", Point3, ZE_VRT_VECTOR3, true),  
			ZEML_LIST_PROPERTY("Point4", Point4, ZE_VRT_VECTOR3, true),
		};

		Reader->SeekPointer(RectanglePointer);
		Reader->ReadPropertyList(RectangleList, 4);

		ZEPortalMapResourceDoor* Door = &Doors[DoorCounter];

		strncpy(Door->Name, DoorName.GetString().ToCString(), ZE_MAX_NAME_SIZE);
		Door->IsOpen = IsOpen.GetBoolean();

		Door->Rectangle.P1 = Point1.GetVector3();
		Door->Rectangle.P2 = Point2.GetVector3();
		Door->Rectangle.P3 = Point3.GetVector3();
		Door->Rectangle.P4 = Point4.GetVector3();

		Door->PortalIds[0] = PortalAIndex.GetUInt32();
		Door->Portals[0] = &Portals[(ZESize)Door->PortalIds[0]];
		Door->Portals[0]->DoorIds.Add(DoorCounter);
		Door->Portals[0]->Doors.Add(Door);

		Door->PortalIds[1] = PortalBIndex.GetUInt32();
		Door->Portals[1] = &Portals[(ZESize)Door->PortalIds[1]];
		Door->Portals[1]->DoorIds.Add(DoorCounter);
		Door->Portals[1]->Doors.Add(Door);

		DoorCounter++;
	}

	Reader->GoToCurrentPointer();

	return true;
}

bool ZEPortalMapResource::ReadPortals(ZEMLSerialReader* Reader)
{
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
	struct ZEMapFilePolygonChunk
	{	
		ZEUInt32				Material;
		ZEMapFileVertexChunk	Vertices[3];
	});

	ZEUInt32 PortalCounter = 0;

	while(Reader->Read())
	{
		if(Reader->GetItemName() != "Portal")
			break;

		ZEVariant PortalName, PhysicalMeshEnabled;
		ZEMLSerialPointer PolygonsPointer, PhysicalMeshPointer;

		ZEMLSerialListItem PortalPropertiesList[] = {
			ZEML_LIST_PROPERTY("Name",					PortalName, ZE_VRT_STRING,	true),
			ZEML_LIST_DATA("Polygons",					PolygonsPointer,		true),
			ZEML_LIST_NODE("PhysicalMesh",				PhysicalMeshPointer,	false)
		};

		Reader->ReadPropertyList(PortalPropertiesList, 3);
		Reader->SeekPointer(PolygonsPointer);

		ZEPortalMapResourcePortal* Portal = &Portals[PortalCounter];
		strncpy(Portal->Name, PortalName.GetString().ToCString(), ZE_MAX_NAME_SIZE);

		ZEArray<ZEMapFilePolygonChunk> MapPolygons;
		MapPolygons.SetCount(Reader->GetDataSize() / sizeof(ZEMapFilePolygonChunk));
		Reader->GetData(MapPolygons.GetCArray(), Reader->GetDataSize());

		if(MapPolygons.GetCount() == 0)
			zeError("Polygon count is : 0. Portal name : %s", Portal->Name);

		Portal->Polygons.SetCount(MapPolygons.GetCount());

		for (ZESize I = 0; I < Portal->Polygons.GetCount(); I++)
		{
			Portal->Polygons[I].LastIteration			= 0;
			Portal->Polygons[I].Material				= Materials[(ZESize)MapPolygons[I].Material];
			Portal->Polygons[I].Vertices[0].Position	= MapPolygons[I].Vertices[0].Position;
			Portal->Polygons[I].Vertices[0].Normal		= MapPolygons[I].Vertices[0].Normal;
			Portal->Polygons[I].Vertices[0].Tangent		= MapPolygons[I].Vertices[0].Tangent;
			Portal->Polygons[I].Vertices[0].Binormal	= MapPolygons[I].Vertices[0].Binormal;
			Portal->Polygons[I].Vertices[0].Texcoord	= MapPolygons[I].Vertices[0].Texcoord;

			Portal->Polygons[I].Vertices[1].Position	= MapPolygons[I].Vertices[1].Position;
			Portal->Polygons[I].Vertices[1].Normal		= MapPolygons[I].Vertices[1].Normal;
			Portal->Polygons[I].Vertices[1].Tangent		= MapPolygons[I].Vertices[1].Tangent;
			Portal->Polygons[I].Vertices[1].Binormal	= MapPolygons[I].Vertices[1].Binormal;
			Portal->Polygons[I].Vertices[1].Texcoord	= MapPolygons[I].Vertices[1].Texcoord;

			Portal->Polygons[I].Vertices[2].Position	= MapPolygons[I].Vertices[2].Position;
			Portal->Polygons[I].Vertices[2].Normal		= MapPolygons[I].Vertices[2].Normal;
			Portal->Polygons[I].Vertices[2].Tangent		= MapPolygons[I].Vertices[2].Tangent;
			Portal->Polygons[I].Vertices[2].Binormal	= MapPolygons[I].Vertices[2].Binormal;
			Portal->Polygons[I].Vertices[2].Texcoord	= MapPolygons[I].Vertices[2].Texcoord;
		}

		ZEAABBox BoundingBox(ZEVector3(FLT_MAX, FLT_MAX, FLT_MAX), ZEVector3(-FLT_MAX, -FLT_MAX, -FLT_MAX));

		for (ZESize I = 0; I < Portal->Polygons.GetCount(); I++)
		{
			for (ZESize J = 0; J < 3; J++)
			{
				ZEVector3 Vertex = Portal->Polygons[I].Vertices[J].Position;

				if (Vertex.x < BoundingBox.Min.x) BoundingBox.Min.x = Vertex.x;
				if (Vertex.y < BoundingBox.Min.y) BoundingBox.Min.y = Vertex.y;
				if (Vertex.z < BoundingBox.Min.z) BoundingBox.Min.z = Vertex.z;

				if (Vertex.x > BoundingBox.Max.x) BoundingBox.Max.x = Vertex.x;
				if (Vertex.y > BoundingBox.Max.y) BoundingBox.Max.y = Vertex.y;
				if (Vertex.z > BoundingBox.Max.z) BoundingBox.Max.z = Vertex.z;
			}
		}

		Portal->BoundingBox = BoundingBox;

		if(PhysicalMeshPointer != -1)
		{
			ZEMLSerialPointer PhysicalVerticesPointer, PhysicalPolygonsPointer;

			Reader->SeekPointer(PhysicalMeshPointer);

			ZEMLSerialListItem PhysicalMeshList[] = {
				ZEML_LIST_DATA("Polygons", PhysicalPolygonsPointer, true),
				ZEML_LIST_DATA("Vertices", PhysicalVerticesPointer, true)
			};

			Reader->ReadPropertyList(PhysicalMeshList, 2);
			Reader->SeekPointer(PhysicalVerticesPointer);
			Reader->Read();
			Portal->PhysicalMesh.Vertices.SetCount(Reader->GetDataSize() / sizeof(ZEVector3));
			Reader->GetData(Portal->PhysicalMesh.Vertices.GetCArray(), Reader->GetDataSize());

			Reader->SeekPointer(PhysicalPolygonsPointer);
			Reader->Read();
			Portal->PhysicalMesh.Polygons.SetCount(Reader->GetDataSize() / sizeof(ZEPortalMapPhysicalMeshPolygon));
			Reader->GetData(Portal->PhysicalMesh.Polygons.GetCArray(), Reader->GetDataSize());
		}
		else
			zeWarning("Portal %s does not have physical mesh.", PortalName.GetString().ToCString());

		PortalCounter++;
	}

	Reader->GoToCurrentPointer();

	return true;
}

bool ZEPortalMapResource::ReadMaterials(ZEMLSerialReader* Reader)
{
	for(ZESize I = 0; I < Materials.GetCount(); I++)
		Materials[I] = ZEFixedMaterial::CreateInstance();

	ZEUInt32 MaterialCounter = 0;

	while(Reader->Read())
	{
		if(Reader->GetItemName() != "Material")
			break;

		ZEVariant MaterialName, MaterialRelativePath;

		ZEMLSerialListItem MaterialList[] = {
			ZEML_LIST_PROPERTY("Name", MaterialName, ZE_VRT_STRING, true),
			ZEML_LIST_PROPERTY("FilePath", MaterialRelativePath, ZE_VRT_STRING, true)
		};

		Reader->ReadPropertyList(MaterialList, 2);
		
		ZEString MaterialPath = ZEFileInfo::GetParentDirectory(GetFileName()) + ZEPathUtils::GetSeperator() + MaterialRelativePath.GetString();

		ZEFixedMaterial* CurrentMaterial = (ZEFixedMaterial*)Materials[MaterialCounter];
		CurrentMaterial->ReadFromFile(MaterialPath);

		MaterialCounter++;
	}

	Reader->GoToCurrentPointer();

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

	Resource->SetFileName(FileName);
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
