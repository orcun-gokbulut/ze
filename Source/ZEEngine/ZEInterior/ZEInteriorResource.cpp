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
#include "ZERenderer/ZERNFixedMaterial.h"
#include "ZEPhysics/ZEPhysicalMesh.h"
#include "ZEFile/ZEFileInfo.h"
#include "ZEML/ZEMLReader.h"
#include "ZEFoundation/ZEPacking.h"
#include "ZEGraphics/ZEGRVertexLayout.h"

#include <string.h>

ZEGRVertexLayout ZEInteriorVertex::VertexLayout;
ZEGRVertexLayout* ZEInteriorVertex::GetVertexLayout()
{

	if (VertexLayout.GetElementCount() == 0)
	{
		ZEGRVertexElement ElementArray[] = 
		{
			{ZEGR_VES_POSITION, 0, ZEGR_VET_FLOAT3, 0, 0,  ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_NORMAL,	0, ZEGR_VET_FLOAT3, 0, 12, ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_TANGENT,	0, ZEGR_VET_FLOAT3, 0, 24, ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_BINORMAL,	0, ZEGR_VET_FLOAT3, 0, 36, ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_TEXCOORD,	0, ZEGR_VET_FLOAT2, 0, 48, ZEGR_VU_PER_VERTEX, 0}
		};

		VertexLayout.SetElements(ElementArray, 5);
	}

	return &VertexLayout;
}

const ZEGRTexture2D* ZEInteriorResource::ManageInteriorMaterialTextures(const ZEString& FileName)
{
	if (FileName == "")
		return NULL;

	for (ZESize I = 0; I < TextureResources.GetCount(); I++)
		if (TextureResources[I]->GetFileName() == FileName)
			return TextureResources[I]->GetTexture2D();

	ZETexture2DResource* NewTextureResource = ZETexture2DResource::LoadSharedResource(ZEFileInfo(this->GetFileName()).GetParentDirectory() + "\\" + FileName);
	if (NewTextureResource == NULL)
	{
		zeError("Can not load texture file. (FileName : \"%s\")", FileName.ToCString());
		return NULL;
	}
	TextureResources.Add(NewTextureResource);
	return NewTextureResource->GetTexture2D();
}

bool ZEInteriorResource::ReadInteriorFromFile(ZEFile* ResourceFile)
{ 
	ZEMLReader InteriorReader;
	if (!InteriorReader.Open(ResourceFile))
		return false;

	ZEMLReaderNode InteriorNode = InteriorReader.GetRootNode();
	if (!InteriorNode.IsValid())
	{
		zeError("Can not read map file.");
		return false;
	}

	if (strcmp(InteriorNode.GetName(), "ZEInterior") != 0)
	{
		zeError("Specified resource file is not an interior file : %s", ResourceFile->GetPath());
		return false;
	}

	Rooms.SetCount(InteriorNode.ReadUInt32("RoomCount"));
	Doors.SetCount(InteriorNode.ReadUInt32("DoorCount"));
	Helpers.SetCount(InteriorNode.ReadUInt32("HelperCount"));
	Materials.SetCount(InteriorNode.ReadUInt32("MaterialCount"));

	ZEMLReaderNode MaterialsNode = InteriorNode.GetNode("Materials");

	if (!ReadMaterials(&MaterialsNode))
	{
		zeError("Can not read materials from file.");
		return false;
	}

	ZEMLReaderNode RoomsNode = InteriorNode.GetNode("Rooms");

	if (!ReadRooms(&RoomsNode))
	{
		zeError("Can not read rooms from file.");
		return false;
	}

	if (InteriorNode.IsSubNodeExists("Doors"))
	{
		ZEMLReaderNode DoorsNode = InteriorNode.GetNode("Doors");

		if (!ReadDoors(&DoorsNode))
		{
			zeError("Can not read doors from file.");
			return false;
		}
	}

	if (InteriorNode.IsSubNodeExists("Helpers"))
	{
		ZEMLReaderNode HelpersNode = InteriorNode.GetNode("Helpers");

		if (!ReadHelpers(&HelpersNode))
		{
			zeError("Can not read helpers from file.");
			return false;
		}
	}

	InteriorReader.Close();

	return true;
}

bool ZEInteriorResource::ReadDoors(ZEMLReaderNode* DoorsNode)
{
	if (DoorsNode == NULL)
		return false;

	if (!DoorsNode->IsValid())
		return false;

	ZESize SubNodeCount = DoorsNode->GetNodeCount("Door");

	for (ZESize I = 0; I < SubNodeCount; I++)
	{
		ZEMLReaderNode DoorNode = DoorsNode->GetNode("Door", I);

		if (!DoorNode.IsValid())
			return false;

		ZEInteriorResourceDoor* Door = &Doors[I];

		strncpy(Door->Name, DoorNode.ReadString("Name").ToCString(), ZE_MAX_NAME_SIZE);
		Door->IsOpen = DoorNode.ReadBoolean("IsOpen");

		Door->Width = DoorNode.ReadFloat("Width");
		Door->Length = DoorNode.ReadFloat("Length");
		Door->Position = DoorNode.ReadVector3("Position");
		Door->Rotation = DoorNode.ReadQuaternion("Rotation");
		Door->Scale = DoorNode.ReadVector3("Scale");

		if (DoorNode.IsPropertyExists("UserDefinedProperties"))
			Door->UserDefinedProperties = DoorNode.ReadString("UserDefinedProperties");

		Door->RoomIds[0] = DoorNode.ReadInt32("RoomAIndex");
		Door->Rooms[0] = &Rooms[(ZESize)Door->RoomIds[0]];
		Door->Rooms[0]->DoorIds.Add(I);
		Door->Rooms[0]->Doors.Add(Door);
		
		Door->RoomIds[1] = DoorNode.ReadInt32("RoomBIndex");
		Door->Rooms[1] = &Rooms[(ZESize)Door->RoomIds[1]];
		Door->Rooms[1]->DoorIds.Add(I);
		Door->Rooms[1]->Doors.Add(Door);

	}

	return true;
}

bool ZEInteriorResource::ReadRooms(ZEMLReaderNode* RoomsNode)
{
	if (RoomsNode == NULL)
		return false;

	if (!RoomsNode->IsValid())
		return false;

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

	ZESize SubNodeCount = RoomsNode->GetNodeCount("Room");

	for (ZESize I = 0; I < SubNodeCount; I++)
	{
		ZEMLReaderNode RoomNode = RoomsNode->GetNode("Room", I);

		if (!RoomNode.IsValid())
			return false;

		ZEInteriorResourceRoom* Room = &Rooms[I];

		strncpy(Room->Name, RoomNode.ReadString("Name").ToCString(), ZE_MAX_NAME_SIZE);
		Room->Position = RoomNode.ReadVector3("Position");
		Room->Rotation = RoomNode.ReadQuaternion("Rotation");
		Room->Scale = RoomNode.ReadVector3("Scale");
		
		if (RoomNode.IsPropertyExists("UserDefinedProperties"))
			Room->UserDefinedProperties = RoomNode.ReadString("UserDefinedProperties");

		ZEArray<ZEInteriorFilePolygonChunk> MapPolygons;
		MapPolygons.SetCount(RoomNode.ReadDataSize("Polygons") / sizeof(ZEInteriorFilePolygonChunk));
		if (!RoomNode.ReadDataItems("Polygons", MapPolygons.GetCArray(), sizeof(ZEInteriorFilePolygonChunk), MapPolygons.GetCount()))
			return false;
		
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

		if(RoomNode.IsPropertyExists("PhysicalMesh"))
		{
			ZEMLReaderNode PhysicalMeshNode = RoomNode.GetNode("PhysicalMesh");

			Room->PhysicalMesh.PhysicalMeshEnabled = PhysicalMeshNode.ReadBoolean("PhysicalMeshEnabled");

			if (PhysicalMeshNode.IsPropertyExists("UserDefinedProperties"))
				Room->PhysicalMesh.UserDefinedProperties = PhysicalMeshNode.ReadString("UserDefinedProperties");

			Room->PhysicalMesh.Polygons.SetCount(PhysicalMeshNode.ReadDataSize("Polygons") / sizeof(ZEInteriorPhysicalMeshPolygon));
			if (!RoomNode.ReadDataItems("Polygons", Room->PhysicalMesh.Polygons.GetCArray(), sizeof(ZEInteriorPhysicalMeshPolygon), Room->PhysicalMesh.Polygons.GetCount()))
				return false;

			Room->PhysicalMesh.Vertices.SetCount(PhysicalMeshNode.ReadDataSize("Vertices") / sizeof(ZEVector3));
			if (!RoomNode.ReadDataItems("Vertices", Room->PhysicalMesh.Vertices.GetCArray(), sizeof(ZEVector3), Room->PhysicalMesh.Vertices.GetCount()))
				return false;

			Room->HasPhysicalMesh = true;
		}
		else
		{
			Room->HasPhysicalMesh = false;
			zeWarning("Room %s does not have physical mesh.", Room->Name);
		}

		if (RoomNode.IsPropertyExists("GenerateOctree"))
		{
			if (RoomNode.ReadBoolean("GenerateOctree"))
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
		else
		{
			Room->HasOctree = false;
		}
	}

	return true;
}

bool ZEInteriorResource::ReadHelpers(ZEMLReaderNode* HelpersNode)
{
	if (HelpersNode == NULL)
		return false;

	if (!HelpersNode->IsValid())
		return false;

	ZESize SubNodeCount = HelpersNode->GetNodeCount("Helper");

	for (ZESize I = 0; I < SubNodeCount; I++)
	{
		ZEMLReaderNode HelperNode = HelpersNode->GetNode("Helper", I);

		if (!HelperNode.IsValid())
			return false;

		ZEInteriorResourceHelper* Helper = &Helpers[I];
		
		strncpy(Helper->Name, HelperNode.ReadString("Name").ToCString(), ZE_MAX_NAME_SIZE);
		
		Helper->Position = HelperNode.ReadVector3("Position");
		Helper->Rotation = HelperNode.ReadQuaternion("Rotation");
		Helper->Scale = HelperNode.ReadVector3("Scale");

		if (HelperNode.IsPropertyExists("UserDefinedProperties"))
			Helper->UserDefinedProperties = HelperNode.ReadString("UserDefinedProperties");
		
		Helper->OwnerType = (ZEInteriorResourceHelperOwnerType)HelperNode.ReadInt32("OwnerType");
		Helper->OwnerIndex = HelperNode.ReadInt32("OwnerIndex");
		
		if (Helper->OwnerType == ZE_IRHOT_ROOM)
			Helper->OwnerRoom = &Rooms[Helper->OwnerIndex];
	}

	return true;
}

bool ZEInteriorResource::ReadMaterials(ZEMLReaderNode* MaterialsNode)
{
	if (MaterialsNode == NULL)
		return false;

	if (!MaterialsNode->IsValid())
		return false;

	for(ZESize I = 0; I < Materials.GetCount(); I++)
		Materials[I] = ZERNFixedMaterial::CreateInstance();

	ZESize SubNodeCount = MaterialsNode->GetNodeCount("Material");

	for (ZESize I = 0; I < SubNodeCount; I++)
	{
		ZEMLReaderNode MaterialNode = MaterialsNode->GetNode("Material", I);

		if (!MaterialNode.IsValid())
			return false;

		ZEString MaterialPath = ZEFileInfo(GetFileName()).GetParentDirectory() + "/" + MaterialNode.ReadString("FilePath");

		ZERNFixedMaterial* CurrentMaterial = (ZERNFixedMaterial*)Materials[I];
		CurrentMaterial->ReadFromFile(MaterialPath);
	}

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

const ZEArray<ZERNMaterial*>& ZEInteriorResource::GetMaterials() const
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

	return NULL;
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
