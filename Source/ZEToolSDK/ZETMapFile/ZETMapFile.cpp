//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETMapFile.cpp
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

#include "ZETOutput.h"
#include "ZETMapFile.h"
#include "ZETMapOctree.h"
#include "ZETMapFileFormat.h"

#include <stdio.h>
#include <string.h>

#pragma warning(push)
#pragma warning(disable:4307 4267 4018)


ZEMapFilePortal::ZEMapFilePortal()
{
	Octree = NULL;
}

ZEMapFilePortal::~ZEMapFilePortal()
{
	if (Octree != NULL)
		delete Octree;
}

void CalculateBoundingBox(ZEAABBox& BoundingBox, ZEArray<ZEMapFilePolygon>& Polygons)
{
	if (Polygons.GetCount() == 0)
		BoundingBox.Max = BoundingBox.Min = ZEVector3(0.0f, 0.0f ,0.0f);

	BoundingBox.Min = BoundingBox.Max = Polygons[0].Vertices[0].Position;
	for (ZESize I = 1; I < Polygons.GetCount(); I++)
		for (ZESize N = 0; N < 3; N++)
		{
			if (BoundingBox.Max.x < Polygons[I].Vertices[N].Position.x) 
				BoundingBox.Max.x = Polygons[I].Vertices[N].Position.x;

			if (BoundingBox.Max.y < Polygons[I].Vertices[N].Position.y) 
				BoundingBox.Max.y = Polygons[I].Vertices[N].Position.y;

			if (BoundingBox.Max.z < Polygons[I].Vertices[N].Position.z) 
				BoundingBox.Max.z = Polygons[I].Vertices[N].Position.z;

			if (BoundingBox.Min.x > Polygons[I].Vertices[N].Position.x) 
				BoundingBox.Min.x = Polygons[I].Vertices[N].Position.x;

			if (BoundingBox.Min.y > Polygons[I].Vertices[N].Position.y) 
				BoundingBox.Min.y = Polygons[I].Vertices[N].Position.y;

			if (BoundingBox.Min.z > Polygons[I].Vertices[N].Position.z) 
				BoundingBox.Min.z = Polygons[I].Vertices[N].Position.z;
		}
}

void WriteMaterialsToFile(FILE* File, ZEArray<ZEMapFileMaterial>& Materials)
{
	ZEUInt32 ChunkIndentifier = ZE_MPFL_MATERIAL_CHUNK;
	
	// Write materials
	for (ZESize I = 0; I < Materials.GetCount(); I++)
	{
		// Write Material Chunk Identifier
		fwrite(&ChunkIndentifier, sizeof(ZEUInt32), 1, File);

		// Write Material
		fwrite(&Materials[I], sizeof(ZEMapFileMaterial), 1, File);
	}
}

void WritePolygonsToFile(FILE* File, ZEArray<ZEMapFilePolygon>& Polygons)
{
	// Write chunk identifier
	ZEUInt32 ChunkIdentifier = ZE_MPFL_POLYGONS_CHUNK;
	fwrite(&ChunkIdentifier, sizeof(ZEUInt32), 1, File);

	// Normalize vertex elements (Normals, Binormals, Tangents)
	for(ZESize I = 0; I < Polygons.GetCount(); I++)
	{
		ZEMapFileVertex* CurrVertex = &Polygons[I].Vertices[0];
		ZEVector3::Normalize(CurrVertex->Normal, CurrVertex->Normal);
		ZEVector3::Normalize(CurrVertex->Tangent, CurrVertex->Tangent);
		ZEVector3::Normalize(CurrVertex->Binormal, CurrVertex->Binormal);

		CurrVertex = &Polygons[I].Vertices[1];
		ZEVector3::Normalize(CurrVertex->Normal, CurrVertex->Normal);
		ZEVector3::Normalize(CurrVertex->Tangent, CurrVertex->Tangent);
		ZEVector3::Normalize(CurrVertex->Binormal, CurrVertex->Binormal);

		CurrVertex = &Polygons[I].Vertices[2];
		ZEVector3::Normalize(CurrVertex->Normal, CurrVertex->Normal);
		ZEVector3::Normalize(CurrVertex->Tangent, CurrVertex->Tangent);
		ZEVector3::Normalize(CurrVertex->Binormal, CurrVertex->Binormal);
	}

	// Write polygons to file
	fwrite(Polygons.GetCArray(), sizeof(ZEMapFilePolygonChunk), Polygons.GetCount(), File);
}

void WriteOctreeToFile(FILE* File, ZEMapFileOctree* Octree)
{
	ZEMapFileOctreeChunk FileOctree;
	
	FileOctree.ChunkIdentifier = ZE_MPFL_OCTREE_CHUNK;
	FileOctree.BoundingBox = Octree->BoundingBox;
	FileOctree.Depth = Octree->Depth;

	for (ZEInt I = 0; I < 8; I++)
		FileOctree.SubSpaces[I] = (Octree->SubTrees[I] == NULL ? false : true);

	if (Octree->IsLeaf == false)
	{
		FileOctree.IsLeaf = false;
		FileOctree.PolygonCount = 0;
	}
	else
	{
		FileOctree.IsLeaf = true;
		FileOctree.PolygonCount = (ZEUInt32)Octree->PolygonIds.GetCount();
	}

	fwrite(&FileOctree, sizeof(ZEMapFileOctreeChunk), 1, File);
	if (FileOctree.IsLeaf)
	{
		ZEUInt32 ChunkIdentifier = ZE_MPFL_OCTREE_POLYGONIDS_CHUNK;
		fwrite(&ChunkIdentifier, sizeof(ZEUInt32), 1, File);
		fwrite(Octree->PolygonIds.GetCArray(), sizeof(ZEUInt32), Octree->PolygonIds.GetCount(), File);
	}
	else
		for (ZEInt I = 0; I < 8; I++)
			if (Octree->SubTrees[I] != NULL)
				WriteOctreeToFile(File, Octree->SubTrees[I]);
}

void WritePhysicalMeshToFile(FILE* File, ZEMapFilePhysicalMesh& PhysicalMesh)
{
	// Write physical mesh header
	ZEMapFilePhysicalMeshChunk FilePhysicalMesh;
	FilePhysicalMesh.ChunkIdentifier = ZE_MPFL_PHYSICAL_MESH_CHUNK;
	FilePhysicalMesh.PolygonCount = (ZEUInt32)PhysicalMesh.Polygons.GetCount();
	FilePhysicalMesh.VertexCount = (ZEUInt32)PhysicalMesh.Vertices.GetCount();
	fwrite(&FilePhysicalMesh, sizeof(ZEMapFilePhysicalMeshChunk), 1, File);

	// Write physical mesh vertices
	ZEUInt32 ChunkIdentifier = ZE_MPFL_PHYSICAL_MESH_VERTICES_CHUNK;
	fwrite(&ChunkIdentifier, sizeof(ZEUInt32), 1, File);
	fwrite(PhysicalMesh.Vertices.GetCArray(), sizeof(ZEVector3), PhysicalMesh.Vertices.GetCount(), File);

	// Write physical mesh polygons
	ChunkIdentifier = ZE_MPFL_PHYSICAL_MESH_POLYGONS_CHUNK;
	fwrite(&ChunkIdentifier, sizeof(ZEUInt32), 1, File);
	fwrite(PhysicalMesh.Polygons.GetCArray(), sizeof(ZEMapFilePhysicalMeshPolygon), PhysicalMesh.Polygons.GetCount(), File);
}

void WritePortalsToFile(FILE* File, ZEArray<ZEMapFilePortal>& Portals)
{
	// Write portal header
	ZEMapFilePortalChunk FilePortal;
	ZEMapFileDoorChunk FileDoor;

	for (ZESize I = 0; I < Portals.GetCount(); I++)
	{
		ZEMapFilePortal* Portal = &Portals[I];
		FilePortal.ChunkIdentifier = ZE_MPFL_PORTAL_CHUNK;
		strncpy(FilePortal.Name, Portal->Name, ZE_MPFL_MAX_NAME_SIZE);
		CalculateBoundingBox(Portal->BoundingBox, Portal->Polygons);
		FilePortal.BoundingBox = Portal->BoundingBox;
		FilePortal.PolygonCount = (ZEUInt32)Portal->Polygons.GetCount();
		FilePortal.HasOctree = Portal->GenerateOctree;
		FilePortal.HasPhysicalMesh = Portal->PhysicalMeshEnabled;
		fwrite(&FilePortal, sizeof(ZEMapFilePortalChunk), 1, File);
		
		// Write Polygons
		WritePolygonsToFile(File, Portal->Polygons);

		// Write Octree
		if (Portal->GenerateOctree)
		{
			if (Portal->Octree != NULL)
				delete Portal->Octree;
			Portal->Octree = ZEMapFileOctree::GenerateOctree(Portal->Polygons, Portal->MaxOctreeDepth);
			WriteOctreeToFile(File, Portal->Octree);
		}

		// Physical Mesh
		if (Portal->PhysicalMeshEnabled)
			WritePhysicalMeshToFile(File, Portal->PhysicalMesh);
	}
}

void WriteDoorsToFile(FILE* File, ZEArray<ZEMapFileDoor>& Doors)
{
	// Write portal doors
	for (ZESize I = 0; I < Doors.GetCount(); I++)
	{
		ZEMapFileDoor* Door = &Doors[I];

		ZEMapFileDoorChunk FileDoor;
		FileDoor.ChunkIdentifier = ZE_MPFL_PORTAL_DOOR_CHUNK;
		strncpy(FileDoor.Name, Door->Name, ZE_MAX_NAME_SIZE);
		FileDoor.Rectangle = Door->Rectangle;
		FileDoor.PortalIds[0] = Door->PortalIds[0];
		FileDoor.PortalIds[1] = Door->PortalIds[1];
		FileDoor.IsOpen = Door->IsOpen;
		fwrite(&FileDoor, sizeof(ZEMapFileDoorChunk), 1, File);
	}
}

bool ZEMapFile::WriteToFile(const char* FileName, ZEInt Chunks)
{
	zesdkLog("Map File", "Writing ZEMap to file. (Filename : \"%s\")", FileName);

	if (!Validate())
	{
		zesdkError("Map File", "ZEMapFile could not pass validation cheks.");
		return false;
	}

	FILE* File = fopen(FileName,"wb");

	// Check for file is writable
	if (File == NULL)
	{
		zesdkError("Map File", "Can not open file for writing. (Filename : \"%s\")", FileName);
		return false;
	}

	ZEMapFileHeader FileHeader;
	FileHeader.Header = ZE_MPFL_HEADER;
	FileHeader.Version = ZE_MPFL_VERSION;
	FileHeader.MaterialCount = (ZEUInt32)Materials.GetCount();
	FileHeader.DoorCount = (ZEUInt32)Doors.GetCount();
	FileHeader.PortalCount = (ZEUInt32)Portals.GetCount();
	fwrite(&FileHeader, sizeof(ZEMapFileHeader), 1, File);

	// Write file parts to the file
	WriteMaterialsToFile(File, this->Materials);
	WritePortalsToFile(File, this->Portals);
	WriteDoorsToFile(File, this->Doors);

	fclose(File);

	zesdkLog("Map File", "ZEMap has written to file.", FileName);
	return true;
}

// Reading

bool ReadMaterialsFromFile(FILE* File, ZEArray<ZEMapFileMaterial>& Materials)
{
	ZEUInt32 ChunkIndentifier = 0;
	
	// Read materials
	for (ZESize I = 0; I < Materials.GetCount(); I++)
	{
		// Read Material Chunk Identifier and check it
		fread(&ChunkIndentifier, sizeof(ZEUInt32), 1, File);
		if (ChunkIndentifier != ZE_MPFL_MATERIAL_CHUNK)
		{
			zesdkError("Map File", "Material chunk's id does not match.");
			return false;
		}

		// Read Material
		fread(&Materials[I], sizeof(ZEMapFileMaterial), 1, File);
	}
	return true;
}

bool ReadPolygonsFromFile(FILE* File, ZEArray<ZEMapFilePolygon>& Polygons)
{
	// Read chunk identifier and check it
	ZEUInt32 ChunkIdentifier = 0;
	fread(&ChunkIdentifier, sizeof(ZEUInt32), 1, File);
	if (ChunkIdentifier != ZE_MPFL_POLYGONS_CHUNK)
	{
		zesdkError("Map File", "Polygons chunk's id does not match.");
		return false;
	}

	// Read polygons from file
	fread(Polygons.GetCArray(), sizeof(ZEMapFilePolygon), Polygons.GetCount(), File);

	return true;
}

bool ReadOctreeNodeFromFile(FILE* File, ZEMapFileOctree* Octree)
{
	ZEMapFileOctreeChunk	FileOctree;
	
	// Read octree header
	fread(&FileOctree, sizeof(ZEMapFileOctreeChunk), 1, File);

	// Check chunk id
	if (FileOctree.ChunkIdentifier != ZE_MPFL_OCTREE_CHUNK)
	{
		zesdkError("Map File", "Octree chunk's id does not match.");
		return false;
	}

	// Map octree attributes
	Octree->BoundingBox = FileOctree.BoundingBox;
	Octree->IsLeaf = FileOctree.IsLeaf;
	if(!Octree->IsLeaf)
	{
		// if node is not leaf recursively create and load sub nodes
		for(ZEInt I = 0; I < 8; I++)
			if(FileOctree.SubSpaces[I])
			{
				// if sub note is available create it and load it
				Octree->SubTrees[I] = new ZEMapFileOctree();
				Octree->SubTrees[I]->Depth = Octree->Depth + 1;
				if (!ReadOctreeNodeFromFile(File, Octree->SubTrees[I]))
					return false;
			}
			else
				Octree->SubTrees[I] = NULL;
	}
	else
	{
		// if node is leaf node then read polygon ids.
		ZEUInt32 ChunkIndentifier;
		fread(&ChunkIndentifier, sizeof(ZEUInt32), 1, File);

		// Check chunk indentifier
		if (ChunkIndentifier != ZE_MPFL_OCTREE_POLYGONIDS_CHUNK)
		{
			zesdkError("Map File", "Octree Polygon Id's chunk id does not match.");
			return false;
		}

		// Read ids of polygons that is included in this octree node
		Octree->PolygonIds.SetCount((ZESize)FileOctree.PolygonCount);
		fread(Octree->PolygonIds.GetCArray(), sizeof(ZEInt), Octree->PolygonIds.GetCount(), File);
	}

	return true;
}

bool ReadOctreeFromFile(FILE* File, ZEMapFileOctree* Octree)
{
	// Create octree
	Octree = new ZEMapFileOctree();
	Octree->Depth = 0;

	// Load it
	if (!ReadOctreeNodeFromFile(File, Octree))
		return false;

	return true;
}

bool ReadPhysicalMeshFromFile(FILE* File, ZEMapFilePhysicalMesh& PhysicalMesh)
{
	// Read physical mesh header
	ZEMapFilePhysicalMeshChunk FilePhysicalMesh;
	fread(&FilePhysicalMesh, sizeof(ZEMapFilePhysicalMeshChunk), 1, File);

	// Check physical mesh chunk identifier
	if (FilePhysicalMesh.ChunkIdentifier != ZE_MPFL_PHYSICAL_MESH_CHUNK)
	{
		zesdkError("Map File", "Physical mesh chunk's id does not match.");
		return false;
	}

	PhysicalMesh.Polygons.SetCount((ZESize)FilePhysicalMesh.PolygonCount);
	PhysicalMesh.Vertices.SetCount((ZESize)FilePhysicalMesh.VertexCount);

	ZEUInt32 ChunkIdentifier = 0;

	// Check physical mesh vertices chunk identifier
	fread(&ChunkIdentifier, sizeof(ZEUInt32), 1, File);
	if (ChunkIdentifier != ZE_MPFL_PHYSICAL_MESH_VERTICES_CHUNK)
	{
		zesdkError("Map File", "Physical mesh vertices chunk's id does not match.");
		return false;
	}

	// Read physical mesh vertices
	fread(PhysicalMesh.Vertices.GetCArray(), sizeof(ZEVector3), PhysicalMesh.Vertices.GetCount(), File);

	// Check physical mesh polygons chunk identifier
	fread(&ChunkIdentifier, sizeof(ZEUInt32), 1, File);
	if (ChunkIdentifier != ZE_MPFL_PHYSICAL_MESH_POLYGONS_CHUNK)
	{
		zesdkError("Map File", "Physical mesh polygons chunk's id does not match.");
		return false;
	}

	// Read physical mesh polygons
	fread(PhysicalMesh.Polygons.GetCArray(), sizeof(ZEMapFilePhysicalMeshPolygon), PhysicalMesh.Polygons.GetCount(), File);

	return true;
}

bool ReadPortalsFromFile(FILE* File, ZEArray<ZEMapFilePortal>& Portals)
{
	ZEMapFilePortalChunk FilePortal;

	for (ZESize I = 0; I < Portals.GetCount(); I++)
	{
		ZEMapFilePortal* Portal = &Portals[I];

		fread(&FilePortal, sizeof(ZEMapFilePortalChunk), 1, File);

		if (FilePortal.ChunkIdentifier != ZE_MPFL_PORTAL_CHUNK)
		{
			zesdkError("Map File", "Portal chunk's id does not match.");
			return false;
		}

		strncpy(Portal->Name, FilePortal.Name, ZE_MPFL_MAX_NAME_SIZE);
		Portal->BoundingBox = FilePortal.BoundingBox;
		Portal->Polygons.SetCount((ZESize)FilePortal.PolygonCount);
		Portal->PhysicalMeshEnabled = FilePortal.HasPhysicalMesh;
		Portal->Octree = NULL;

		// Read Polygons
		if (!ReadPolygonsFromFile(File, Portal->Polygons))
			return false;

		// Read Octree
		if (FilePortal.HasOctree)
			if (!ReadOctreeFromFile(File, Portal->Octree))
				return false;

		if (FilePortal.HasPhysicalMesh)
			if (!ReadPhysicalMeshFromFile(File, Portal->PhysicalMesh))
				return false;
	}
	return true;
}

static bool ReadDoorsFromFile(FILE* File, ZEArray<ZEMapFileDoor>& Doors)
{
	// Read portal doors
	for (ZESize N = 0; N < Doors.GetCount(); N++)
	{
		ZEMapFileDoor* Door = &Doors[N];
		ZEMapFileDoorChunk FileDoor;

		fread(&FileDoor, sizeof(ZEMapFileDoorChunk), 1, File);

		if (FileDoor.ChunkIdentifier != ZE_MPFL_PORTAL_DOOR_CHUNK)
		{
			zesdkError("Map File", "Door chunk's id does not match.");
			return false;
		}

		strncpy(Door->Name, FileDoor.Name, ZE_MAX_NAME_SIZE);
		Door->Rectangle = FileDoor.Rectangle;
		Door->PortalIds[0] = FileDoor.PortalIds[0];
		Door->PortalIds[1] = FileDoor.PortalIds[1];
		Door->IsOpen = FileDoor.IsOpen;
	}

	return true;
}

bool ZEMapFile::ReadFromFile(const char* FileName, ZEInt Chunks)
{
	Clear();
	FILE* File = fopen(FileName,"rb");
	ZEMapFileHeader TempHeader;
	fread(&TempHeader, sizeof(ZEMapFileHeader), 1, File);

	if(TempHeader.Header!= ZE_MPFL_HEADER)
	{
		zesdkError("Map File", "Unknown ZEMap file format.");
		return false;
	}
	
	if(TempHeader.Version != ZE_MPFL_VERSION)
	{	
		fclose(File);
		zesdkError("Map File", "ZEMap file version mismatched.");
		return false;
	}

	Portals.SetCount((ZESize)TempHeader.PortalCount);
	Materials.SetCount((ZESize)TempHeader.MaterialCount);
	Doors.SetCount((ZESize)TempHeader.DoorCount);

	if (!ReadMaterialsFromFile(File, this->Materials))
	{
		fclose(File);
		zesdkError("Map File", "File is corrupted. Can not read materials from file.");
		return false;
	}

	if (!ReadPortalsFromFile(File, this->Portals))
	{
		fclose(File);
		zesdkError("Map File", "File is corrupted. Can not read portals from file.");
		return false;
	}

	if (!ReadDoorsFromFile(File, this->Doors))
	{
		fclose(File);
		zesdkError("Map File", "File is corrupted. Can not read doors from file.");
		return false;
	}

	fclose(File);

	if (!Validate())
	{
		zesdkWarning("Map File", "ZEMapFile has been readed but it has failed validation checks.");
	}

	return true;
}

void ZEMapFile::Clear()
{
	Materials.Clear();
	Doors.Clear();
	Portals.Clear();
}

bool ZEMapFile::Validate()
{
	bool Validated = true;

	// Check portals
	for (ZESize I = 0; I < Portals.GetCount(); I++)
	{
		// Check polygon materials
		for(ZESize N = 0; N < Portals[I].Polygons.GetCount(); N++)
		{
			if(Portals[I].Polygons[N].Material >= (ZEUInt32)Materials.GetCount())
			{		
				zesdkWarning("Map File Validation", "Polygon does not have a valid material id. (Portal Index : %d, Polygon Index : %d)", I, N);
				Validated = false;
			}
		}
	}

	// Check portal door destinations
	for (ZESize I = 0; I < Doors.GetCount(); I++)
	{
		if(Doors[I].PortalIds[0] >= (ZEUInt32)Portals.GetCount())
		{		
			zesdkWarning("Map File Validation", "Portal door destination is not valid. (Door Index : %d, Destination Portal)", I, Doors[I].PortalIds[0]);
			Validated = false;
		}

		if(Doors[I].PortalIds[1] >= (ZEUInt32)Portals.GetCount())
		{		
			zesdkWarning("Map File Validation", "Portal door destination is not valid. (Door Index : %d, Destination Portal)", I, Doors[I].PortalIds[1]);
			Validated = false;
		}
	}

	// Check for unused materials
	for(ZESize I = 0; I < Materials.GetCount(); I++)
	{
		bool Used = false;
		for (ZESize N = 0; N < Portals.GetCount(); N++)
		{
			for(ZESize M = 0; M < Portals[N].Polygons.GetCount(); M++)
			{
				if(Portals[N].Polygons[M].Material == (ZEUInt32)I)
				{
					Used = true;
				}
			}
		}
		
		if(!Used)
		{
			zesdkNotice("Map File Validation", "Material %d is not used by any polygons.\n",I);
		}
	}

	return Validated;
}


ZEMapFile::ZEMapFile()
{

}

ZEMapFile::~ZEMapFile()
{

}

#pragma warning(pop)
