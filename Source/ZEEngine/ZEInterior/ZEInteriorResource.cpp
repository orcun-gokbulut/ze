//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEInteriorResource.cpp
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

#include "ZEInteriorResource.h"

#include "ZEError.h"
#include "ZEFile/ZEFile.h"
#include "ZECore/ZEConsole.h"
#include "ZECore/ZEResource.h"
#include "ZECore/ZEResourceManager.h"
#include "ZETexture/ZETexture2DResource.h"
#include "ZEGraphics/ZEFixedMaterial.h"
#include "ZEPhysics/ZEPhysicalMesh.h"
#include "ZEFile/ZEFileInfo.h"
#include "ZEML/ZEMLSerialReader.h"
#include "ZEFoundation/ZEPacking.h"

#include <string.h>

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

const ZETexture2D* ZEInteriorResource::ManageInteriorMaterialTextures(const ZEString& FileName)
{
	if (FileName == "")
		return NULL;

	for (ZESize I = 0; I < TextureResources.GetCount(); I++)
		if (TextureResources[I]->GetFileName() == FileName)
			return TextureResources[I]->GetTexture();

	ZETexture2DResource* NewTextureResource = ZETexture2DResource::LoadSharedResource(ZEFileInfo(this->GetFileName()).GetParentDirectory() + "\\" + FileName);
	if (NewTextureResource == NULL)
	{
		zeError("Can not load texture file. (FileName : \"%s\")", FileName.ToCString());
		return NULL;
	}
	TextureResources.Add(NewTextureResource);
	return NewTextureResource->GetTexture();
}

bool ZEInteriorResource::ReadInteriorFromFile(ZEFile* ResourceFile)
{ 
	ZEMLSerialReader Reader(ResourceFile);

	if(!Reader.Read())
	{
		zeError("Can not read map file.");
		return false;
	}

	if(Reader.GetItemName() != ZEString("ZEInterior"))
	{
		zeError("Resource file is not an interior file, file name : %s", ResourceFile->GetPath());
		return false;
	}

	ZEMLSerialPointer RoomsPointer, DoorsPointer, HelpersPointer, MaterialsPointer;
	ZEValue DoorCount, RoomCount, HelperCount, MaterialCount;

	ZEMLSerialListItem InteriorList[] = {
		ZEML_LIST_NODE("Rooms",					RoomsPointer,										true),
		ZEML_LIST_NODE("Doors",					DoorsPointer,										false),
		ZEML_LIST_NODE("Helpers",				HelpersPointer,										false),
		ZEML_LIST_NODE("Materials",				MaterialsPointer,									true),
		ZEML_LIST_PROPERTY("RoomCount",			RoomCount,			ZE_VRT_UNSIGNED_INTEGER_32,		true),
		ZEML_LIST_PROPERTY("DoorCount",			DoorCount,			ZE_VRT_UNSIGNED_INTEGER_32,		true),
		ZEML_LIST_PROPERTY("HelperCount",		HelperCount,		ZE_VRT_UNSIGNED_INTEGER_32,		true),
		ZEML_LIST_PROPERTY("MaterialCount",		MaterialCount,		ZE_VRT_UNSIGNED_INTEGER_32,		true)
	};

	if(!Reader.ReadPropertyList(InteriorList, 8))
	{
		zeError("Can not read map from file.");
		return false;
	}

	Rooms.SetCount(RoomCount.GetUInt32());
	Doors.SetCount(DoorCount.GetUInt32());
	Helpers.SetCount(HelperCount.GetUInt32());
	Materials.SetCount(MaterialCount.GetUInt32());

	Reader.SeekPointer(MaterialsPointer);
	if(!ReadMaterials(&Reader))
	{
		zeError("Can not read materials from file.");
		return false;
	}

	Reader.SeekPointer(RoomsPointer);
	if(!ReadRooms(&Reader))
	{
		zeError("Can not read rooms from file.");
		return false;
	}

	if (DoorsPointer != -1)
	{
		Reader.SeekPointer(DoorsPointer);

		if(!ReadDoors(&Reader))
		{
			zeError("Can not read doors from file.");
			return false;
		}
	}

	if (HelpersPointer != -1)
	{
		Reader.SeekPointer(HelpersPointer);

		if(!ReadHelpers(&Reader))
		{
			zeError("Can not read helpers from file.");
			return false;
		}
	}

	Reader.GoToCurrentPointer();

	return true;
}

bool ZEInteriorResource::ReadDoors(ZEMLSerialReader* Reader)
{
	ZESize SubItemCount = Reader->GetSubItemCount();
	for (ZESize I = 0; I < SubItemCount; I++)
	{
		if (!Reader->Read())
		{
			zeError("Can not read ZEML node.");
			return false;
		}

		if (Reader->GetItemType() != ZEML_IT_NODE || Reader->GetItemName() != "Door")
			continue;

		ZEValue DoorName, IsOpen, Width, Length, Position, Rotation, Scale, RoomAIndex, RoomBIndex, UserDefinedProperties;
		ZEMLSerialPointer RectanglePointer;

		ZEMLSerialListItem DoorPropertiesList[] = { 
			ZEML_LIST_PROPERTY("Name",						DoorName,					ZE_VRT_STRING,					true),  
			ZEML_LIST_PROPERTY("IsOpen",					IsOpen,						ZE_VRT_BOOLEAN, 				true),
			ZEML_LIST_PROPERTY("Width",						Width,						ZE_VRT_FLOAT,					true),
			ZEML_LIST_PROPERTY("Length",					Length,						ZE_VRT_FLOAT,					true),
			ZEML_LIST_PROPERTY("Position",					Position,					ZE_VRT_VECTOR3,					true),
			ZEML_LIST_PROPERTY("Rotation",					Rotation,					ZE_VRT_QUATERNION,				true),
			ZEML_LIST_PROPERTY("Scale",						Scale,						ZE_VRT_VECTOR3,					true),
			ZEML_LIST_PROPERTY("RoomAIndex",				RoomAIndex,					ZE_VRT_INTEGER_32,				true),  
			ZEML_LIST_PROPERTY("RoomBIndex",				RoomBIndex,					ZE_VRT_INTEGER_32,				true),
			ZEML_LIST_PROPERTY("UserDefinedProperties",		UserDefinedProperties,		ZE_VRT_STRING,					false)
		};

		Reader->ReadPropertyList(DoorPropertiesList, 10);

		ZEInteriorResourceDoor* Door = &Doors[I];

		strncpy(Door->Name, DoorName.GetString().ToCString(), ZE_MAX_NAME_SIZE);
		Door->IsOpen = IsOpen.GetBoolean();

		Door->Width = Width.GetFloat();
		Door->Length = Length.GetFloat();
		Door->Position = Position.GetVector3();
		Door->Rotation = Rotation.GetQuaternion();
		Door->Scale = Scale.GetVector3();

		if (UserDefinedProperties.GetType() == ZE_VRT_STRING)
			Door->UserDefinedProperties = UserDefinedProperties.GetString();

		Door->RoomIds[0] = RoomAIndex.GetInt32();
		Door->Rooms[0] = &Rooms[(ZESize)Door->RoomIds[0]];
		Door->Rooms[0]->DoorIds.Add(I);
		Door->Rooms[0]->Doors.Add(Door);

		Door->RoomIds[1] = RoomBIndex.GetInt32();
		Door->Rooms[1] = &Rooms[(ZESize)Door->RoomIds[1]];
		Door->Rooms[1]->DoorIds.Add(I);
		Door->Rooms[1]->Doors.Add(Door);

	}

	Reader->GoToCurrentPointer();

	return true;
}

bool ZEInteriorResource::ReadRooms(ZEMLSerialReader* Reader)
{
	ZEPackStruct(
	struct ZEInteriorFileVertexChunk
	{
		ZEVector3				Position;
		ZEVector3				Normal;
		ZEVector3				Tangent;
		ZEVector3				Binormal;
		ZEVector2				Texcoord;
	});

	ZEPackStruct(
	struct ZEInteriorFilePolygonChunk
	{	
		ZEUInt32					Material;
		ZEInteriorFileVertexChunk	Vertices[3];
	});

	ZESize SubItemCount = Reader->GetSubItemCount();
	for (ZESize I = 0; I < SubItemCount; I++)
	{
		if (!Reader->Read())
		{
			zeError("Can not read ZEML node.");
			return false;
		}

		if (Reader->GetItemType() != ZEML_IT_NODE || Reader->GetItemName() != "Room")
			continue;

		ZEValue RoomName, Position, Rotation, Scale, PhysicalMeshEnabled, UserDefinedProperties, GenerateOctree;
		ZEMLSerialPointer PolygonsPointer, PhysicalMeshPointer;

		ZEMLSerialListItem RoomPropertiesList[] = {
			ZEML_LIST_PROPERTY("Name",						RoomName,				ZE_VRT_STRING,		true),
			ZEML_LIST_PROPERTY("Position",					Position,				ZE_VRT_VECTOR3,		true),
			ZEML_LIST_PROPERTY("Rotation",					Rotation,				ZE_VRT_QUATERNION,	true),
			ZEML_LIST_PROPERTY("Scale",						Scale,					ZE_VRT_VECTOR3,		true),
			ZEML_LIST_PROPERTY("GenerateOctree",			GenerateOctree,			ZE_VRT_BOOLEAN,		false),
			ZEML_LIST_PROPERTY("UserDefinedProperties",		UserDefinedProperties,	ZE_VRT_STRING,		false),
			ZEML_LIST_DATA("Polygons",						PolygonsPointer,							true),
			ZEML_LIST_NODE("PhysicalMesh",					PhysicalMeshPointer,						false)	
		};

		Reader->ReadPropertyList(RoomPropertiesList, 8);
		Reader->SeekPointer(PolygonsPointer);

		ZEInteriorResourceRoom* Room = &Rooms[I];
		strncpy(Room->Name, RoomName.GetString().ToCString(), ZE_MAX_NAME_SIZE);

		Room->Position = Position.GetVector3();
		Room->Rotation = Rotation.GetQuaternion();
		Room->Scale = Scale.GetVector3();
		
		if (UserDefinedProperties.GetType() == ZE_VRT_STRING)
			Room->UserDefinedProperties = UserDefinedProperties.GetString();

		ZEArray<ZEInteriorFilePolygonChunk> MapPolygons;
		MapPolygons.SetCount(Reader->GetDataSize() / sizeof(ZEInteriorFilePolygonChunk));
		Reader->GetData(MapPolygons.GetCArray(), Reader->GetDataSize());

		if(MapPolygons.GetCount() == 0)
			zeError("Polygon count is : 0. Room name : %s", Room->Name);

		Room->Polygons.SetCount(MapPolygons.GetCount());

		for (ZESize I = 0; I < Room->Polygons.GetCount(); I++)
		{
			Room->Polygons[I].LastIteration				= 0;
			Room->Polygons[I].Material					= Materials[(ZESize)MapPolygons[I].Material];
			Room->Polygons[I].Vertices[0].Position		= MapPolygons[I].Vertices[0].Position;
			Room->Polygons[I].Vertices[0].Normal		= MapPolygons[I].Vertices[0].Normal;
			Room->Polygons[I].Vertices[0].Tangent		= MapPolygons[I].Vertices[0].Tangent;
			Room->Polygons[I].Vertices[0].Binormal		= MapPolygons[I].Vertices[0].Binormal;
			Room->Polygons[I].Vertices[0].Texcoord		= MapPolygons[I].Vertices[0].Texcoord;

			Room->Polygons[I].Vertices[1].Position		= MapPolygons[I].Vertices[1].Position;
			Room->Polygons[I].Vertices[1].Normal		= MapPolygons[I].Vertices[1].Normal;
			Room->Polygons[I].Vertices[1].Tangent		= MapPolygons[I].Vertices[1].Tangent;
			Room->Polygons[I].Vertices[1].Binormal		= MapPolygons[I].Vertices[1].Binormal;
			Room->Polygons[I].Vertices[1].Texcoord		= MapPolygons[I].Vertices[1].Texcoord;

			Room->Polygons[I].Vertices[2].Position		= MapPolygons[I].Vertices[2].Position;
			Room->Polygons[I].Vertices[2].Normal		= MapPolygons[I].Vertices[2].Normal;
			Room->Polygons[I].Vertices[2].Tangent		= MapPolygons[I].Vertices[2].Tangent;
			Room->Polygons[I].Vertices[2].Binormal		= MapPolygons[I].Vertices[2].Binormal;
			Room->Polygons[I].Vertices[2].Texcoord		= MapPolygons[I].Vertices[2].Texcoord;
		}

		ZEAABBox BoundingBox(ZEVector3(FLT_MAX, FLT_MAX, FLT_MAX), ZEVector3(-FLT_MAX, -FLT_MAX, -FLT_MAX));

		for (ZESize I = 0; I < Room->Polygons.GetCount(); I++)
		{
			for (ZESize J = 0; J < 3; J++)
			{
				ZEVector3 Vertex = Room->Polygons[I].Vertices[J].Position;

				if (Vertex.x < BoundingBox.Min.x) BoundingBox.Min.x = Vertex.x;
				if (Vertex.y < BoundingBox.Min.y) BoundingBox.Min.y = Vertex.y;
				if (Vertex.z < BoundingBox.Min.z) BoundingBox.Min.z = Vertex.z;

				if (Vertex.x > BoundingBox.Max.x) BoundingBox.Max.x = Vertex.x;
				if (Vertex.y > BoundingBox.Max.y) BoundingBox.Max.y = Vertex.y;
				if (Vertex.z > BoundingBox.Max.z) BoundingBox.Max.z = Vertex.z;
			}
		}

		Room->BoundingBox = BoundingBox;

		if(PhysicalMeshPointer != -1)
		{
			ZEValue PhysicalMeshEnabledValue, PhysicalUserDefinedPropertiesValue;
			ZEMLSerialPointer PhysicalVerticesPointer, PhysicalPolygonsPointer;

			Reader->SeekPointer(PhysicalMeshPointer);

			ZEMLSerialListItem PhysicalMeshList[] = {
				ZEML_LIST_PROPERTY("PhysicalMeshEnabled",	PhysicalMeshEnabledValue,			ZE_VRT_BOOLEAN,	true),
				ZEML_LIST_PROPERTY("UserDefinedProperties",	PhysicalUserDefinedPropertiesValue,	ZE_VRT_BOOLEAN,	false),
				ZEML_LIST_DATA("Polygons",					PhysicalPolygonsPointer,							true),
				ZEML_LIST_DATA("Vertices",					PhysicalVerticesPointer,							true)
			};

			Reader->ReadPropertyList(PhysicalMeshList, 4);

			Room->PhysicalMesh.PhysicalMeshEnabled = PhysicalMeshEnabledValue;

			if (PhysicalUserDefinedPropertiesValue.GetType() == ZE_VRT_STRING)
				Room->PhysicalMesh.UserDefinedProperties = PhysicalUserDefinedPropertiesValue.GetString();

			Reader->SeekPointer(PhysicalPolygonsPointer);
			Room->PhysicalMesh.Polygons.SetCount(Reader->GetDataSize() / sizeof(ZEInteriorPhysicalMeshPolygon));
			Reader->GetData(Room->PhysicalMesh.Polygons.GetCArray(), Reader->GetDataSize());

			Reader->SeekPointer(PhysicalVerticesPointer);
			Room->PhysicalMesh.Vertices.SetCount(Reader->GetDataSize() / sizeof(ZEVector3));
			Reader->GetData(Room->PhysicalMesh.Vertices.GetCArray(), Reader->GetDataSize());

			Room->HasPhysicalMesh = true;
		}
		else
		{
			Room->HasPhysicalMesh = false;
			zeWarning("Room %s does not have physical mesh.", RoomName.GetString().ToCString());
		}

		if (true)//GenerateOctree.GetType() == ZE_VRT_BOOLEAN && GenerateOctree.GetBoolean())
		{
			Room->HasOctree = true;
			Room->Octree.SetBoundingBox(Room->BoundingBox);
			Room->Octree.SetMaxDepth(4);
			for (ZESize I = 0; I < Room->Polygons.GetCount(); I++)
			{
				ZETriangle Triangle(Room->Polygons[I].Vertices[0].Position, Room->Polygons[I].Vertices[1].Position, Room->Polygons[I].Vertices[2].Position);
				Room->Octree.AddItem(I, Triangle);
			}
		}
		else
		{
			Room->HasOctree = false;
		}
	}

	Reader->GoToCurrentPointer();

	return true;
}

bool ZEInteriorResource::ReadHelpers(ZEMLSerialReader* Reader)
{
	ZESize SubItemCount = Reader->GetSubItemCount();
	for (ZESize I = 0; I < SubItemCount; I++)
	{
		if (!Reader->Read())
		{
			zeError("Can not read ZEML node.");
			return false;
		}

		if (Reader->GetItemType() != ZEML_IT_NODE || Reader->GetItemName() != "Helper")
			continue;

		ZEValue NameValue, OwnerTypeValue, OwnerIndexValue, PositionValue, RotationValue, ScaleValue, UserDefinedProperties;

		ZEMLSerialListItem HelperPropertiesList[] = { 
			ZEML_LIST_PROPERTY("Name",						NameValue,					ZE_VRT_STRING,			true),  
			ZEML_LIST_PROPERTY("OwnerType",					OwnerTypeValue,				ZE_VRT_INTEGER_32, 		true),
			ZEML_LIST_PROPERTY("OwnerIndex",				OwnerIndexValue,			ZE_VRT_INTEGER_32,		true),
			ZEML_LIST_PROPERTY("Position",					PositionValue,				ZE_VRT_VECTOR3,			true),
			ZEML_LIST_PROPERTY("Rotation",					RotationValue,				ZE_VRT_QUATERNION,		true),
			ZEML_LIST_PROPERTY("Scale",						ScaleValue,					ZE_VRT_VECTOR3,			true),
			ZEML_LIST_PROPERTY("UserDefinedProperties",		UserDefinedProperties,		ZE_VRT_STRING,			false)
		};

		Reader->ReadPropertyList(HelperPropertiesList, 7);

		ZEInteriorResourceHelper* Helper = &Helpers[I];

		strncpy(Helper->Name, NameValue.GetString().ToCString(), ZE_MAX_NAME_SIZE);

		Helper->Position = PositionValue;
		Helper->Rotation = RotationValue;
		Helper->Scale = ScaleValue;

		if (UserDefinedProperties.GetType() == ZE_VRT_STRING)
			Helper->UserDefinedProperties = UserDefinedProperties.GetString();

		Helper->OwnerType = (ZEInteriorResourceHelperOwnerType)OwnerTypeValue.GetInt32();
		Helper->OwnerIndex = OwnerIndexValue;

		if (Helper->OwnerType == ZE_IRHOT_ROOM)
			Helper->OwnerRoom = &Rooms[Helper->OwnerIndex];
	}

	Reader->GoToCurrentPointer();

	return true;
}

bool ZEInteriorResource::ReadMaterials(ZEMLSerialReader* Reader)
{
	for(ZESize I = 0; I < Materials.GetCount(); I++)
		Materials[I] = ZEFixedMaterial::CreateInstance();

	ZESize SubItemCount = Reader->GetSubItemCount();
	for (ZESize I = 0; I < SubItemCount; I++)
	{
		if (!Reader->Read())
		{
			zeError("Can not read ZEML node.");
			return false;
		}

		if (Reader->GetItemType() != ZEML_IT_NODE || Reader->GetItemName() != "Material")
			continue;

		ZEValue MaterialName, MaterialRelativePath;

		ZEMLSerialListItem MaterialList[] = {
			ZEML_LIST_PROPERTY("Name",		MaterialName,			ZE_VRT_STRING,	true),
			ZEML_LIST_PROPERTY("FilePath",	MaterialRelativePath,	ZE_VRT_STRING,	true)
		};

		Reader->ReadPropertyList(MaterialList, 2);
		
		ZEString MaterialPath = ZEFileInfo(GetFileName()).GetParentDirectory() + "/" + MaterialRelativePath.GetString();

		ZEFixedMaterial* CurrentMaterial = (ZEFixedMaterial*)Materials[I];
		CurrentMaterial->ReadFromFile(MaterialPath);

	}

	Reader->GoToCurrentPointer();

	return true;
}

const char* ZEInteriorResource::GetResourceType() const
{
	return "Interior Resource";
}

const ZEArray<ZETexture2DResource*>& ZEInteriorResource::GetTextures() const
{
	return TextureResources;
}

const ZEArray<ZEMaterial*>& ZEInteriorResource::GetMaterials() const
{
	return Materials;
}

const ZEArray<ZEInteriorResourceRoom>& ZEInteriorResource::GetRooms() const
{
	return Rooms;
}

const ZEArray<ZEInteriorResourceDoor>& ZEInteriorResource::GetDoors() const
{
	return Doors;
}

const ZEArray<ZEInteriorResourceHelper>& ZEInteriorResource::GetHelpers() const
{
	return Helpers;
}

ZEInteriorResource* ZEInteriorResource::LoadSharedResource(const ZEString& FileName)
{
	ZEInteriorResource* Resource = (ZEInteriorResource*)zeResources->GetResource(FileName);
	if (Resource != NULL)
		return Resource;
	else
	{
		// If there is no shared instance of ZEInterior file create and load new instance
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

void ZEInteriorResource::CacheResource(const ZEString& FileName)
{
	// Try to get instance of shared ZEInterior file from resource manager
	ZEInteriorResource* Resource = (ZEInteriorResource*)zeResources->GetResource(FileName);
	if (Resource != NULL)
		Resource->Cached = true;
	else
	{
		// If there is no shared instance of ZEInterior file create and load new instance
		Resource = LoadResource(FileName);
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

ZEInteriorResource* ZEInteriorResource::LoadResource(const ZEString& FileName)
{
	bool Result;

	ZEFile ResourceFile;
	Result = ResourceFile.Open(FileName, ZE_FOM_READ, ZE_FCM_NONE);
	if (Result)
	{
		// Create ZEInteriorResource
		ZEInteriorResource* MapResource = new ZEInteriorResource();
		MapResource->SetFileName(FileName);
		MapResource->Cached = false;
		MapResource->ReferenceCount = 0;

		if (!MapResource->ReadInteriorFromFile(&ResourceFile))
		{
			zeError("Can not load interior resource. (FileName : \"%s\")", FileName.GetValue());
			ResourceFile.Close();
			delete MapResource;
			return NULL;
		}
		
		ResourceFile.Close();

		return MapResource;
	}
	else
	{
		zeError("Interior file does not exists. FileName : \"%s\"", FileName.GetValue());
		return NULL;
	}
}

ZEInteriorResource::~ZEInteriorResource()
{
	for (ZESize I = 0; I < TextureResources.GetCount(); I++)
		TextureResources[I]->Release();

	TextureResources.Clear();
}
