//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZE3dsMaxInteriorExporterProcess.cpp
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


#include "ZE3dsMaxInteriorExporter.h"

#include "ZEMath\ZEAngle.h"
#include "ZEFile\ZEFile.h"
#include "ZEFile\ZEFileInfo.h"
#include "ZEFile\ZEDirectoryInfo.h"
#include "ZEML\ZEMLWriter.h"

#include "ZEProgressDialog.h"
#include "ZEResourceConfiguratorWidget.h"

enum ZEInteriorHelperOwnerType
{
	ZE_IHOT_INTERIOR		= 0,
	ZE_IHOT_ROOM			= 1
};

ZEPackStruct(
struct ZEInteriorFilePhysicalMeshPolygon
{
	ZEUInt32								Indices[3];
});

ZEPackStruct(
struct ZEInteriorFilePhysicalMesh
{
	ZEArray<ZEVector3>						Vertices;
	ZEArray<ZEInteriorFilePhysicalMeshPolygon>	Polygons;
});

ZEPackStruct(
struct ZEInteriorFileVertex
{
	ZEVector3								Position;
	ZEVector3								Normal;
	ZEVector3								Tangent;
	ZEVector3								Binormal;
	ZEVector2								Texcoord;
});

ZEPackStruct(
struct ZEInteriorFilePolygon
{	
	ZEUInt32								Material;
	ZEInteriorFileVertex					Vertices[3];
});

ZEInt32 ZE3dsMaxInteriorExporter::ProcessFaceMaterial(IGameMaterial* Material)
{
	for (ZESize I = 0; I < (ZESize)Materials.Count(); I++)
		if (Materials[I] == Material)
			return (ZEInt32)I;

	Materials.Append(1, &Material);

	return Materials.Count() - 1;
}

ZEInt32 ZE3dsMaxInteriorExporter::FindRoomIndex(IGameNode* Node)
{
	for (ZESize I = 0; I < (ZESize)Rooms.Count(); I++)
		if (Node == Rooms[I]) return (ZEInt32)I;

	return -1;
}

bool ZE3dsMaxInteriorExporter::ProcessDoors()
{
	zeLog("Processing Interior Doors...");

	if (Doors.Count() == 0)
	{
		zeLog("No doors with ZEInteriorDoorAttribute found. Interior door processing skipped.");
		return true;
	}

	ZEMLNode* DoorsNode = InteriorNode.AddNode("Doors");

	INode* RoomANode;
	INode* RoomBNode;

	for (ZESize I = 0; I < (ZESize)Doors.Count(); I++)
	{
		ZEMLNode* DoorNode = DoorsNode->AddNode("Door");

		IGameNode* CurrentNode = Doors[I];
		IGameObject* CurrentObject = CurrentNode->GetIGameObject();
		ZEProgressDialog::GetInstance()->OpenTask(CurrentNode->GetName());
		zeLog("Processing Door \"%s\" (%Iu/%d)", ZEString(CurrentNode->GetName()).ToCString(), I + 1, Doors.Count());
		
		DoorNode->AddProperty("Name")->SetString(ZEString(CurrentNode->GetName()));

		if (!ZE3dsMaxUtils::GetProperty(CurrentObject, ZE_BOOL_PROP, ZEString("IsOpen"), *DoorNode->AddProperty("IsOpen")))
			zeError("Can not find door property : \"IsOpen\".");

		if (!ZE3dsMaxUtils::GetProperty(CurrentObject, ZE_FLOAT_PROP, ZEString("Width"), *DoorNode->AddProperty("Width")))
			zeError("Can not find door property : \"Width\".");

		if (!ZE3dsMaxUtils::GetProperty(CurrentObject, ZE_FLOAT_PROP, ZEString("Length"), *DoorNode->AddProperty("Length")))
			zeError("Can not find door property : \"Length\".");

		ZEMatrix4x4 ObjectTM;
		ZEMatrix4x4::CreateOrientation(ObjectTM, 
			ZE3dsMaxUtils::MaxtoZE(CurrentNode->GetObjectTM().Translation()), 
			ZE3dsMaxUtils::MaxtoZE(CurrentNode->GetObjectTM().Rotation()), 
			ZE3dsMaxUtils::MaxtoZE(CurrentNode->GetObjectTM().Scaling()));

		ZEMatrix4x4 WorldTM;
		ZEMatrix4x4::CreateOrientation(WorldTM, 
			ZE3dsMaxUtils::MaxtoZE(CurrentNode->GetWorldTM().Translation()), 
			ZE3dsMaxUtils::MaxtoZE(CurrentNode->GetWorldTM().Rotation()), 
			ZE3dsMaxUtils::MaxtoZE(CurrentNode->GetWorldTM().Scaling()));

		ZEMatrix4x4 OffsetTransform = WorldTM.Inverse() * ObjectTM;

		if (!OffsetTransform.Equals(ZEMatrix4x4::Identity, 0.0000001f))
			zeWarning("Pivot transformations are omitted for ZEInteriorDoor. The pivot for ZEInteriorDoor: %s is returned to it's Identity state.", ZEString(CurrentNode->GetName()).ToCString());

		DoorNode->AddProperty("Position")->SetVector3(ZE3dsMaxUtils::MaxtoZE(CurrentNode->GetObjectTM().Translation()));
		DoorNode->AddProperty("Rotation")->SetQuaternion(ZE3dsMaxUtils::MaxtoZE(CurrentNode->GetObjectTM().Rotation()));
		DoorNode->AddProperty("Scale")->SetVector3(ZE3dsMaxUtils::MaxtoZE(CurrentNode->GetObjectTM().Scaling()));

		if (!ZE3dsMaxUtils::GetProperty(CurrentObject, ZEString("RoomA"), RoomANode))
			zeError("Can not find door property: RoomA");

		if (!ZE3dsMaxUtils::GetProperty(CurrentObject, ZEString("RoomB"), RoomBNode))
			zeError("Can not find door property: RoomB");

		if (RoomANode == NULL || RoomBNode == NULL || RoomANode == RoomBNode)
		{
			zeWarning("Interior door \"%s\" has wrong parameters.", ZEString(CurrentNode->GetName()).ToCString());
			return false;
		}

		ZEInt32 RoomAIndex = FindRoomIndex(Scene->GetIGameNode(RoomANode));
		
		if (RoomAIndex < 0)
			zeError("Can nor find RoomAIndex.");
		
		DoorNode->AddProperty("RoomAIndex")->SetInt32(RoomAIndex);

		ZEInt32 RoomBIndex = FindRoomIndex(Scene->GetIGameNode(RoomBNode));

		if (RoomBIndex < 0)
			zeError("Can nor find RoomBIndex.");

		DoorNode->AddProperty("RoomBIndex")->SetInt32(RoomBIndex);

		MSTR UserDefinedPropertiesBuffer;
		CurrentNode->GetMaxNode()->GetUserPropBuffer(UserDefinedPropertiesBuffer);

		if (!UserDefinedPropertiesBuffer.isNull())
			DoorNode->AddProperty("UserDefinedProperties")->SetString(ZEString(UserDefinedPropertiesBuffer.data()));
	
		ZEProgressDialog::GetInstance()->CloseTask();
	}

	return true;
}

void ZE3dsMaxInteriorExporter::ProcessPhysicalMesh(IGameNode* ParentNode, IGameNode* Node, ZEMLNode* PhysicalMeshNode)
{
	IGameMesh* Mesh = (IGameMesh*)Node->GetIGameObject();

	if (!Mesh->InitializeData())
		zeError("Can not initialize mesh data for phsical mesh.");

	ZEArray<ZEInteriorFilePhysicalMeshPolygon> Polygons;
	Polygons.SetCount((ZESize)Mesh->GetNumberOfFaces());

	if (Mesh->GetNumberOfFaces() == 0)
		zeError("Physical mesh face count is : 0.");

	for (ZESize I = 0; I < (ZESize)Mesh->GetNumberOfFaces(); I++)
	{
		FaceEx* Face = Mesh->GetFace((ZEInt32)I);
		Polygons[I].Indices[0] = Face->vert[0]; 
		Polygons[I].Indices[1] = Face->vert[1]; 
		Polygons[I].Indices[2] = Face->vert[2]; 
	}

	PhysicalMeshNode->AddData("Polygons")->SetData(Polygons.GetCArray(), sizeof(ZEInteriorFilePhysicalMeshPolygon) * Polygons.GetCount(), false);

	ZEArray<ZEVector3> Vertices;
	Vertices.SetCount((ZESize)Mesh->GetNumberOfVerts());

	ZEMatrix4x4 ObjectTM;
	ZEMatrix4x4::CreateOrientation(ObjectTM, 
		ZE3dsMaxUtils::MaxtoZE(Node->GetObjectTM().Translation()), 
		ZE3dsMaxUtils::MaxtoZE(Node->GetObjectTM().Rotation()), 
		ZE3dsMaxUtils::MaxtoZE(Node->GetObjectTM().Scaling()));

	ZEMatrix4x4 WorldTM;
	ZEMatrix4x4::CreateOrientation(WorldTM, 
		ZE3dsMaxUtils::MaxtoZE(Node->GetWorldTM().Translation()), 
		ZE3dsMaxUtils::MaxtoZE(Node->GetWorldTM().Rotation()), 
		ZE3dsMaxUtils::MaxtoZE(Node->GetWorldTM().Scaling()));

	ZEMatrix4x4 ParentWorldTM;
	ZEMatrix4x4::CreateOrientation(ParentWorldTM,
		ZE3dsMaxUtils::MaxtoZE(ParentNode->GetWorldTM().Translation()), 
		ZE3dsMaxUtils::MaxtoZE(ParentNode->GetWorldTM().Rotation()), 
		ZE3dsMaxUtils::MaxtoZE(ParentNode->GetWorldTM().Scaling()));

	ZEMatrix4x4 TotalTransform = (ParentWorldTM.Inverse() * WorldTM) * (WorldTM.Inverse() * ObjectTM); //Contains Room-PhysicalMesh relativity transformation and Offset transformation.

	Point3		TempObjectVertex;

	for (ZESize I = 0; I < (ZESize)Mesh->GetNumberOfVerts(); I++)
	{
		TempObjectVertex = Mesh->GetVertex((ZEInt32)I, true);
		ZEMatrix4x4::Transform(Vertices[I], TotalTransform, ZE3dsMaxUtils::MaxtoZE(TempObjectVertex));
	}

	PhysicalMeshNode->AddData("Vertices")->SetData(Vertices.GetCArray(), sizeof(ZEVector3) * Vertices.GetCount(), false);
}

bool ZE3dsMaxInteriorExporter::ProcessRooms()
{
	zeLog("Processing rooms...");

	if (Rooms.Count() == 0)
	{
		zeError("No rooms with ZEInteriorRoomAttribute found. Room processing aborted. Interiors must contain at least single Room object.");
		return false;
	}

	ZEMLNode* RoomsNode = InteriorNode.AddNode("Rooms");

	for (ZESize I = 0; I < (ZESize)Rooms.Count(); I++)
	{
		ZEMLNode* RoomNode = RoomsNode->AddNode("Room");

		IGameNode* CurrentNode = Rooms[I];
		ZEProgressDialog::GetInstance()->OpenTask(CurrentNode->GetName());
		zeLog("Processing Room \"%s\" (%Iu/%d)", ZEString(CurrentNode->GetName()).ToCString(), I + 1, Rooms.Count());
		IGameObject* CurrentObject = CurrentNode->GetIGameObject();
		bool PhysicalMeshExists, PhysicalMeshEnabled, PhysicalMeshUseSelf, OctreeEnabled;
		INode* PhysicalMeshMaxNode;
		IGameNode* PhysicalMeshNode;

		RoomNode->AddProperty("Name")->SetString(ZEString(CurrentNode->GetName()));

		RoomNode->AddProperty("Position")->SetVector3(ZE3dsMaxUtils::MaxtoZE(CurrentNode->GetWorldTM().Translation()));
		RoomNode->AddProperty("Rotation")->SetQuaternion(ZE3dsMaxUtils::MaxtoZE(CurrentNode->GetWorldTM().Rotation()));
		RoomNode->AddProperty("Scale")->SetVector3(ZE3dsMaxUtils::MaxtoZE(CurrentNode->GetWorldTM().Scaling()));

		MSTR UserDefinedPropertiesBuffer;
		CurrentNode->GetMaxNode()->GetUserPropBuffer(UserDefinedPropertiesBuffer);

		if (!UserDefinedPropertiesBuffer.isNull())
			RoomNode->AddProperty("UserDefinedProperties")->SetString(ZEString(UserDefinedPropertiesBuffer.data()));

		if (!ZE3dsMaxUtils::GetProperty(CurrentObject, ZE_INT_PROP, ZEString("OctreeEnabled"), OctreeEnabled))
			zeError("Can not find room property : \"OctreeEnabled\".");
		else
			RoomNode->AddProperty("GenerateOctree")->SetBool(OctreeEnabled);

		if (!ZE3dsMaxUtils::GetProperty(CurrentObject, ZE_INT_PROP, ZEString("PhysicalMeshExists"), PhysicalMeshExists))
			zeError("Can not find room property : \"PhysicalMeshExists\".");

		if (!ZE3dsMaxUtils::GetProperty(CurrentObject, ZE_INT_PROP, ZEString("PhysicalMeshEnabled"), PhysicalMeshEnabled))
			zeError("Can not find room property : \"PhysicalMeshEnabled\".");

		if (!ZE3dsMaxUtils::GetProperty(CurrentObject, ZE_INT_PROP, ZEString("PhysicalMeshUseSelf"), PhysicalMeshUseSelf))
			zeError("Can not find room property : \"PhysicalMeshUseSelf\".");

		if (PhysicalMeshEnabled && !PhysicalMeshUseSelf)
			if (!ZE3dsMaxUtils::GetProperty(CurrentObject, ZEString("PhysicalMesh"), PhysicalMeshMaxNode))
				zeError("Can not find room property : \"PhysicalMesh\".");

		if (PhysicalMeshExists)
		{
			if (PhysicalMeshUseSelf)
			{
				ZEMLNode* PhysicalMeshZEMLNode = RoomNode->AddNode("PhysicalMesh");
				PhysicalMeshZEMLNode->AddProperty("PhysicalMeshEnabled")->SetBool(PhysicalMeshEnabled);

				MSTR UserDefinedPropertiesBuffer;
				CurrentNode->GetMaxNode()->GetUserPropBuffer(UserDefinedPropertiesBuffer);

				if (!UserDefinedPropertiesBuffer.isNull())
					PhysicalMeshZEMLNode->AddProperty("UserDefinedProperties")->SetString(ZEString(UserDefinedPropertiesBuffer.data()));

				ProcessPhysicalMesh(CurrentNode, CurrentNode, PhysicalMeshZEMLNode);
			}
			else
			{
				PhysicalMeshNode = Scene->GetIGameNode(PhysicalMeshMaxNode);
				if (PhysicalMeshNode == NULL)
				{
					zeWarning("No physical mesh found. Physical Mesh processing aborted.");
					PhysicalMeshEnabled = false;
				}
				else
				{
					ZEMLNode* PhysicalMeshZEMLNode = RoomNode->AddNode("PhysicalMesh");
					PhysicalMeshZEMLNode->AddProperty("PhysicalMeshEnabled")->SetBool(PhysicalMeshEnabled);

					MSTR UserDefinedPropertiesBuffer;
					PhysicalMeshNode->GetMaxNode()->GetUserPropBuffer(UserDefinedPropertiesBuffer);

					if (!UserDefinedPropertiesBuffer.isNull())
						PhysicalMeshZEMLNode->AddProperty("UserDefinedProperties")->SetString(ZEString(UserDefinedPropertiesBuffer.data()));

					ProcessPhysicalMesh(CurrentNode, PhysicalMeshNode, PhysicalMeshZEMLNode);
				}
			}
		}

		IGameMesh* Mesh = (IGameMesh*)CurrentObject;
		
		if (!Mesh->InitializeData())
			zeError("Can not initialize mesh data.");

		if (!Mesh->InitializeBinormalData())
			zeError("Can not initialize mesh binormal data.");

		Mesh->SetUseWeightedNormals();

		if (Mesh->GetNumberOfFaces() == 0)
			zeError("Face count is : 0.");

		ZEArray<ZEInteriorFilePolygon> Polygons;
		Polygons.SetCount((ZESize)Mesh->GetNumberOfFaces());
		
		ZEMatrix4x4 ObjectTM;
		ZEMatrix4x4::CreateOrientation(ObjectTM, 
			ZE3dsMaxUtils::MaxtoZE(CurrentNode->GetObjectTM().Translation()), 
			ZE3dsMaxUtils::MaxtoZE(CurrentNode->GetObjectTM().Rotation()), 
			ZE3dsMaxUtils::MaxtoZE(CurrentNode->GetObjectTM().Scaling()));

		ZEMatrix4x4 WorldTM;
		ZEMatrix4x4::CreateOrientation(WorldTM, 
			ZE3dsMaxUtils::MaxtoZE(CurrentNode->GetWorldTM().Translation()), 
			ZE3dsMaxUtils::MaxtoZE(CurrentNode->GetWorldTM().Rotation()), 
			ZE3dsMaxUtils::MaxtoZE(CurrentNode->GetWorldTM().Scaling()));

		ZEMatrix4x4 OffsetTransform = WorldTM.Inverse() * ObjectTM;

		for (ZESize I = 0; I < (ZESize)Mesh->GetNumberOfFaces(); I++)
		{
			FaceEx* Face;
			Face = Mesh->GetFace((ZEInt32)I);
			if (Mesh->GetMaterialFromFace((ZEInt32)I) == NULL)
			{
				zeError("Face %d of room \"%s\" does not have valid material.", I, ZEString(CurrentNode->GetName()).ToCString());
				return false;
			}

			Polygons[I].Material = ProcessFaceMaterial(Mesh->GetMaterialFromFace((ZEInt32)I));

			for (ZESize N = 0; N < 3; N++)
			{
				ZEInteriorFileVertex* Vertex = &Polygons[I].Vertices[N];
				ZEInt32 BinormalTangentIndex = Mesh->GetFaceVertexTangentBinormal((ZEInt32)I, (ZEInt32)N);

				Point3 Temp;
				if (!Mesh->GetVertex(Face->vert[N], Temp, true))
					zeError("Can not get vertex of face %d, vertex index : %d.", I, N);

				ZEMatrix4x4::Transform(Vertex->Position, OffsetTransform, ZE3dsMaxUtils::MaxtoZE(Temp));
				
				if (!Mesh->GetNormal(Face->norm[N], Temp, true))
					zeError("Can not get normal of face %d, normal index : %d.", I, N);

				ZEMatrix4x4::Transform3x3(Vertex->Normal, OffsetTransform, ZE3dsMaxUtils::MaxtoZE(Temp.Normalize()));

				if (!Mesh->GetTangent(BinormalTangentIndex, Temp))
					zeError("Can not get tangent of face %d, tangent index : %d.", I, BinormalTangentIndex);

				ZEMatrix4x4::Transform3x3(Vertex->Tangent, OffsetTransform, ZE3dsMaxUtils::MaxtoZE(Temp.Normalize()));

				if (!Mesh->GetBinormal(BinormalTangentIndex, Temp))
					zeError("Can not get binormal of face %d, binormal index : %d.", I, BinormalTangentIndex);

				ZEMatrix4x4::Transform3x3(Vertex->Binormal, OffsetTransform, ZE3dsMaxUtils::MaxtoZE(Temp));

				if (!Mesh->GetTexVertex(Face->texCoord[N], *(Point2*)&Vertex->Texcoord))
					zeError("Can not get texture coordinate of face %d vertex %d.", I, N, BinormalTangentIndex);
			}
		}

		RoomNode->AddData("Polygons")->SetData(Polygons.GetCArray(), sizeof(ZEInteriorFilePolygon) * Polygons.GetCount(), false);

		ZEProgressDialog::GetInstance()->CloseTask();
	}

	return true;
}

bool ZE3dsMaxInteriorExporter::ProcessHelpers()
{
	zeLog("Processing Interior Helpers...");

	if (Helpers.Count() == 0)
	{
		zeWarning("No helpers with ZEHelperAttribute found. Interior helper processing aborted.");
		return true;
	}

	ZEMLNode* HelpersNode = InteriorNode.AddNode("Helpers");

	for (ZESize I = 0; I < (ZESize)Helpers.Count(); I++)
	{
		ZEMLNode* HelperNode = HelpersNode->AddNode("Helper");

		IGameNode* Node = Helpers[I];
		IGameObject* Helper = Node->GetIGameObject();

		ZEProgressDialog::GetInstance()->OpenTask(Node->GetName());
		zeLog("Processing helper \"%s\".", ZEString(Node->GetName()).ToCString());

		HelperNode->AddProperty("Name")->SetString(ZEString(Node->GetName()));

		INode* OwnerNode = NULL;
		ZEInt32 OwnerIndex;
		ZEInteriorHelperOwnerType OwnerType;

		if (!ZE3dsMaxUtils::GetProperty(Helper, ZEString("Owner"), OwnerNode))
			zeError("Can not find helper property: Owner");

		IGameNode* OwnerGameNode = Scene->GetIGameNode(OwnerNode);

		if (OwnerNode == NULL)
		{
			zeWarning("Helper \"%s\" has no immediate owner parameter. Interior will be set as owner.", ZEString(Node->GetName()).ToCString());

			OwnerIndex = -1;
			OwnerType = ZEInteriorHelperOwnerType::ZE_IHOT_INTERIOR;
		}
		else
		{
			ZEString Type;

			ZE3dsMaxUtils::GetProperty(OwnerGameNode->GetIGameObject(), ZE_STRING_PROP, ZEString("ZEType"), Type);

			if (Type.Equals("Room"))
			{
				OwnerIndex = ZE3dsMaxInteriorExporter::FindRoomIndex(OwnerGameNode);
				OwnerType = ZEInteriorHelperOwnerType::ZE_IHOT_ROOM;
			}
			else
			{
				zeError("Helper \"%s\" has invalid owner parameter.", ZEString(Node->GetName()).ToCString());
				return false;
			}
		}

		if (OwnerIndex < 0 && OwnerType != ZE_IHOT_INTERIOR)
		{
			zeError("Helper \"%s\" has invalid owner parameter.", ZEString(Node->GetName()).ToCString());
			return false;
		}

		HelperNode->AddProperty("OwnerType")->SetInt32((ZEInt32)OwnerType);
		HelperNode->AddProperty("OwnerIndex")->SetInt32(OwnerIndex);

		ZEMatrix4x4 FinalTransform;

		ZEMatrix4x4 WorldTM;
		ZEMatrix4x4::CreateOrientation(WorldTM, 
			ZE3dsMaxUtils::MaxtoZE(Node->GetWorldTM().Translation()), 
			ZE3dsMaxUtils::MaxtoZE(Node->GetWorldTM().Rotation()), 
			ZE3dsMaxUtils::MaxtoZE(Node->GetWorldTM().Scaling()));

		if (OwnerIndex != -1)
		{
			ZEMatrix4x4 OwnerWorldTM;
			ZEMatrix4x4::CreateOrientation(OwnerWorldTM,
				ZE3dsMaxUtils::MaxtoZE(OwnerGameNode->GetWorldTM().Translation()), 
				ZE3dsMaxUtils::MaxtoZE(OwnerGameNode->GetWorldTM().Rotation()), 
				ZE3dsMaxUtils::MaxtoZE(OwnerGameNode->GetWorldTM().Scaling()));

			FinalTransform = OwnerWorldTM.Inverse() * WorldTM;
		}
		else
		{
			FinalTransform = WorldTM;
		}

		HelperNode->AddProperty("Position")->SetVector3(FinalTransform.GetTranslation());
		HelperNode->AddProperty("Rotation")->SetQuaternion(FinalTransform.GetRotation());
		HelperNode->AddProperty("Scale")->SetVector3(FinalTransform.GetScale());

		MSTR UserDefinedPropertiesBuffer;
		Node->GetMaxNode()->GetUserPropBuffer(UserDefinedPropertiesBuffer);

		if (!UserDefinedPropertiesBuffer.isNull())
			HelperNode->AddProperty("UserDefinedProperties")->SetString(ZEString(UserDefinedPropertiesBuffer.data()));

		zeLog("Helper \"%s\" is processed.", ZEString(Node->GetName()).ToCString());
		ZEProgressDialog::GetInstance()->CloseTask();
	}

	return true;
}

bool ZE3dsMaxInteriorExporter::ProcessMaterials(const char* FileName)
{
	zeLog("Processing materials...");
	
	char DiffuseMap[ZE_EXFL_MAX_FILENAME_SIZE];
	char SpecularMap[ZE_EXFL_MAX_FILENAME_SIZE];
	char EmissiveMap[ZE_EXFL_MAX_FILENAME_SIZE];
	char OpacityMap[ZE_EXFL_MAX_FILENAME_SIZE];
	char NormalMap[ZE_EXFL_MAX_FILENAME_SIZE];
	char EnvironmentMap[ZE_EXFL_MAX_FILENAME_SIZE];

	ZESize MaterialCount = (ZESize)Materials.Count();

	if (MaterialCount == 0)
	{
		zeWarning("No materials found. Material processing aborted.");
		return true;
	}

	ZEMLNode* MaterialsNode = InteriorNode.AddNode("Materials");

	zeLog("Material count : %d", MaterialCount);

	for (ZESize I = 0; I < MaterialCount; I++)
	{
		IGameMaterial* NodeMaterial = Materials[I];
		ZEString MaterialName = NodeMaterial->GetMaterialName();

		ZEResourceOption MaterialOption;
		if (!ResourceConfigurationDialog->GetOption(MaterialName + ".ZEMaterial", MaterialOption))
			zeError("Can not find resource option. Resource identifier : %s", (MaterialName + ".ZEMaterial").ToCString());

		if (!ResourceConfigurationDialog->GetCopyState(MaterialOption.Identifier))
		{
			ZEMLNode* MaterialNode = MaterialsNode->AddNode("Material");
			MaterialNode->AddProperty("Name")->SetString(MaterialName);
			MaterialNode->AddProperty("FilePath")->SetString(ResourceConfigurationDialog->GetResourceRelativePath(ZEString(FileName) , MaterialName + ".ZEMaterial"));
			continue;
		}

		ZEString MaterialFilePath = MaterialOption.ExportPath + "\\" + MaterialOption.Identifier;

		ZEProgressDialog::GetInstance()->OpenTask(MaterialName);
		zeLog("Processing material \"%s\" (%u/%d).", MaterialName.ToCString(), I + 1, Materials.Count());

		ZeroMemory(DiffuseMap, sizeof(DiffuseMap));
		ZeroMemory(SpecularMap, sizeof(SpecularMap));
		ZeroMemory(EmissiveMap, sizeof(EmissiveMap));
		ZeroMemory(OpacityMap, sizeof(OpacityMap));
		ZeroMemory(NormalMap, sizeof(NormalMap));
		ZeroMemory(EnvironmentMap, sizeof(EnvironmentMap));

		ZEFile MaterialFile;
		zeLog("Opening material file...");
		if (!MaterialFile.Open(MaterialOption.ExportPath + "\\" + MaterialName + ".ZEMaterial", ZE_FOM_WRITE, ZE_FCM_OVERWRITE))
		{
			zeError("Material process failed. Unable to open file for material: \"%s\"", MaterialName.ToCString());
			return false;
		}
		zeLog("Material file successfully opened.");

		ZEInt32 NumberOfMaps = NodeMaterial->GetNumberOfTextureMaps();
		ZEInt32 MapFlag = 0;
		ZEString ResourceRelativePath;

		for (ZEInt32 N = 0; N < NumberOfMaps; N++)
		{
			IGameTextureMap* CurrentTexture = NodeMaterial->GetIGameTextureMap(N);
			switch(CurrentTexture->GetStdMapSlot())
			{
			case ID_AM: // Ambient
				break;
			case ID_DI: // Diffuse
				if (!ResourceConfigurationDialog->GetOption(ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName(), MaterialOption))
					zeError("Diffuse texture export path not found in resource options. Resource identifier : %s", ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName().ToCString());

				if (!ZEDirectoryInfo(MaterialOption.ExportPath).IsDirectory())
					zeError("Diffuse texture export path not valid . Resource identifier : %s", ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName().ToCString());
				else
				{
					MapFlag |= ZE_MTMP_DIFFUSEMAP;
					ResourceRelativePath = ResourceConfigurationDialog->GetResourceRelativePath(MaterialFilePath, MaterialOption.Identifier);
					strncpy(DiffuseMap, ResourceRelativePath.ToCString() , ZE_EXFL_MAX_FILENAME_SIZE);

					if (ResourceConfigurationDialog->GetCopyState(MaterialOption.Identifier))
					{
						if (!ZEFileInfo(MaterialOption.PhysicalPath).Copy(MaterialOption.ExportPath + "//" + MaterialOption.Identifier))
							zeError("Can not copy resource, resource identifier : %s", MaterialOption.Identifier.ToCString());
						else
							zeLog("Resource copied successfully, resource identifier : %s", MaterialOption.Identifier.ToCString());
					}
				}
				break;
			case ID_SP: // Specular
				if (!ResourceConfigurationDialog->GetOption(ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName(), MaterialOption))
					zeError("Specular texture export path not found in resource options. Resource identifier : %s", ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName().ToCString());

				if (!ZEDirectoryInfo(MaterialOption.ExportPath).IsDirectory())
					zeError("Specular texture export path not valid . Resource identifier : %s", ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName().ToCString());
				else
				{
					MapFlag |= ZE_MTMP_SPECULARMAP;
					ResourceRelativePath = ResourceConfigurationDialog->GetResourceRelativePath(MaterialFilePath, MaterialOption.Identifier);
					strncpy(SpecularMap, ResourceRelativePath.ToCString(), ZE_EXFL_MAX_FILENAME_SIZE);

					if (ResourceConfigurationDialog->GetCopyState(MaterialOption.Identifier))
					{
						if (!ZEFileInfo(MaterialOption.PhysicalPath).Copy(MaterialOption.ExportPath + "//" + MaterialOption.Identifier))
							zeError("Can not copy resource, resource identifier : %s", MaterialOption.Identifier.ToCString());
						else
							zeLog("Resource copied successfully, resource identifier : %s", MaterialOption.Identifier.ToCString());
					}
				}
				break;
			case ID_SI:	// Emissive
				if (!ResourceConfigurationDialog->GetOption(ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName(), MaterialOption))
					zeError("Emissive texture export path not found in resource options. Resource identifier : %s", ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName().ToCString());

				if (!ZEDirectoryInfo(MaterialOption.ExportPath).IsDirectory())
					zeError("Emissive texture export path not valid . Resource identifier : %s", ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName().ToCString());
				else
				{
					MapFlag |= ZE_MTMP_EMISSIVEMAP;
					ResourceRelativePath = ResourceConfigurationDialog->GetResourceRelativePath(MaterialFilePath, MaterialOption.Identifier);
					strncpy(EmissiveMap, ResourceRelativePath.ToCString(), ZE_EXFL_MAX_FILENAME_SIZE);

					if (ResourceConfigurationDialog->GetCopyState(MaterialOption.Identifier))
					{
						if (!ZEFileInfo(MaterialOption.PhysicalPath).Copy(MaterialOption.ExportPath + "//" + MaterialOption.Identifier))
							zeError("Can not copy resource, resource identifier : %s", MaterialOption.Identifier.ToCString());
						else
							zeLog("Resource copied successfully, resource identifier : %s", MaterialOption.Identifier.ToCString());
					}
				}
				break;
			case ID_OP:	// Opacity 
				if (!ResourceConfigurationDialog->GetOption(ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName(), MaterialOption))
					zeError("Opacity texture export path not found in resource options. Resource identifier : %s", ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName().ToCString());

				if (!ZEDirectoryInfo(MaterialOption.ExportPath).IsDirectory())
					zeError("Opacity texture export path not valid . Resource identifier : %s", ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName().ToCString());
				else
				{
					MapFlag |= ZE_MTMP_OPACITYMAP;
					ResourceRelativePath = ResourceConfigurationDialog->GetResourceRelativePath(MaterialFilePath, MaterialOption.Identifier);
					strncpy(OpacityMap, ResourceRelativePath.ToCString(), ZE_EXFL_MAX_FILENAME_SIZE);

					if (ResourceConfigurationDialog->GetCopyState(MaterialOption.Identifier))
					{
						if (!ZEFileInfo(MaterialOption.PhysicalPath).Copy(MaterialOption.ExportPath + "//" + MaterialOption.Identifier))
							zeError("Can not copy resource, resource identifier : %s", MaterialOption.Identifier.ToCString());
						else
							zeLog("Resource copied successfully, resource identifier : %s", MaterialOption.Identifier.ToCString());
					}
				}
				break;
			case ID_FI:	// Filter color 
				break;
			case ID_BU: // Bump 
				if (!ResourceConfigurationDialog->GetOption(ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName(), MaterialOption))
					zeError("Bump texture export path not found in resource options. Resource identifier : %s", ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName().ToCString());

				if (!ZEDirectoryInfo(MaterialOption.ExportPath).IsDirectory())
					zeError("Bump texture export path not valid . Resource identifier : %s", ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName().ToCString());
				else
				{
					MapFlag |= ZE_MTMP_NORMALMAP;
					ResourceRelativePath = ResourceConfigurationDialog->GetResourceRelativePath(MaterialFilePath, MaterialOption.Identifier);
					strncpy(NormalMap, ResourceRelativePath.ToCString(), ZE_EXFL_MAX_FILENAME_SIZE);

					if (ResourceConfigurationDialog->GetCopyState(MaterialOption.Identifier))
					{
						if (!ZEFileInfo(MaterialOption.PhysicalPath).Copy(MaterialOption.ExportPath + "//" + MaterialOption.Identifier))
							zeError("Can not copy resource, resource identifier : %s", MaterialOption.Identifier.ToCString());
						else
							zeLog("Resource copied successfully, resource identifier : %s", MaterialOption.Identifier.ToCString());
					}
				}
				break;
			case ID_RL: // Reflection - Environment
				if (!ResourceConfigurationDialog->GetOption(ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName(), MaterialOption))
					zeError("Environment texture export path not found in resource options. Resource identifier : %s", ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName().ToCString());

				if (!ZEDirectoryInfo(MaterialOption.ExportPath).IsDirectory())
					zeError("Environment texture export path not valid . Resource identifier : %s", ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName().ToCString());
				else
				{
					MapFlag |= ZE_MTMP_ENVIRONMENTMAP;
					ResourceRelativePath = ResourceConfigurationDialog->GetResourceRelativePath(MaterialFilePath, MaterialOption.Identifier);
					strncpy(EnvironmentMap, ResourceRelativePath.ToCString(), ZE_EXFL_MAX_FILENAME_SIZE);

					if (ResourceConfigurationDialog->GetCopyState(MaterialOption.Identifier))
					{
						ZEFileInfo FileInfo(MaterialOption.PhysicalPath);
						if (!ZEFileInfo(MaterialOption.PhysicalPath).Copy(MaterialOption.ExportPath + "//" + MaterialOption.Identifier))
							zeError("Can not copy resource, resource identifier : %s", MaterialOption.Identifier.ToCString());
						else
							zeLog("Resource copied successfully, resource identifier : %s", MaterialOption.Identifier.ToCString());
					}
				}
				break; 
			case ID_RR: // Refraction 
				break;
			}
			
		}

		zeLog("Writing material file.");
		ZEMLWriter MaterialWriter;
		MaterialWriter.Open(&MaterialFile);
		ZEMLWriterNode MaterialSerialNode;
		MaterialWriter.OpenRootNode("ZEMaterial", MaterialSerialNode);
		MaterialSerialNode.WriteUInt8("MajorVersion", 1);
		MaterialSerialNode.WriteUInt8("MinorVersion", 0);
		MaterialSerialNode.WriteString("Name", MaterialName);
		ZEMLWriterNode MaterialConfigNode;
		MaterialSerialNode.OpenNode("Configuration", MaterialConfigNode);

		MaterialConfigNode.WriteString("Name", "Default");
		bool TempBooleanValue = false;
		ZE3dsMaxUtils::GetProperty(NodeMaterial, ZE_INT_PROP, ZEString("wire"), TempBooleanValue);
		MaterialConfigNode.WriteBool("Wireframe", TempBooleanValue);

		TempBooleanValue = false;
		ZE3dsMaxUtils::GetProperty(NodeMaterial, ZE_INT_PROP, ZEString("twoSided"), TempBooleanValue);
		MaterialConfigNode.WriteBool("TwoSided", TempBooleanValue);

		MaterialConfigNode.WriteBool("LightningEnabled", true); // Lightning Enabled is forced true;

		float Opacity = 0.0f;
		NodeMaterial->GetOpacityData()->GetPropertyValue(Opacity);

		if (Opacity != 1.0f || strncmp(OpacityMap, "", ZE_EXFL_MAX_FILENAME_SIZE) !=0)
			MaterialConfigNode.WriteBool("Transparant", true);
		else
			MaterialConfigNode.WriteBool("Transparant", false);

		//AMBIENT
		zeLog("Writing ambient data.");
		MaterialConfigNode.WriteBool("AmbientEnabled", false); //Ambient is forced false
		ZEVector3 TempVector3Value = ZEVector3::Zero;
		NodeMaterial->GetAmbientData()->GetPropertyValue(*(Point3*)&TempVector3Value);
		MaterialConfigNode.WriteVector3("AmbientColor", TempVector3Value);

		//DIFFUSE
		zeLog("Writing diffuse data.");
		MaterialConfigNode.WriteBool("DiffuseEnabled", true /*(MapFlag & ZE_MTMP_DIFFUSEMAP) != 0*/); //Diffuse is forced true
		TempVector3Value = ZEVector3::Zero;
		NodeMaterial->GetDiffuseData()->GetPropertyValue(*(Point3*)&TempVector3Value);
		MaterialConfigNode.WriteVector3("DiffuseColor", TempVector3Value);
		MaterialConfigNode.WriteString("BaseMap", DiffuseMap);

		//SPECULAR
		zeLog("Writing specular data.");
		MaterialConfigNode.WriteBool("SpecularEnabled", true /*(MapFlag & ZE_MTMP_SPECULARMAP) != 0*/); //Specular is forced true
		float TempFloatValue = 0.0f;
		NodeMaterial->GetSpecularLevelData()->GetPropertyValue(TempFloatValue);
		MaterialConfigNode.WriteFloat("SpecularFactor", TempFloatValue);

		TempVector3Value = ZEVector3::Zero;
		NodeMaterial->GetSpecularData()->GetPropertyValue(*(Point3*)&TempVector3Value);
		MaterialConfigNode.WriteVector3("SpecularColor", TempVector3Value);
		MaterialConfigNode.WriteString("SpecularMap", SpecularMap);

		//EMISSIVE
		zeLog("Writing emissive data.");
		MaterialConfigNode.WriteBool("EmmisiveEnabled", (MapFlag & ZE_MTMP_EMISSIVEMAP) != 0);
		TempFloatValue = 0.0f;
		NodeMaterial->GetEmissiveAmtData()->GetPropertyValue(TempFloatValue);
		MaterialConfigNode.WriteFloat("EmmisiveFactor", TempFloatValue);

		TempVector3Value = ZEVector3::Zero;
		NodeMaterial->GetEmissiveData()->GetPropertyValue(*(Point3*)&TempVector3Value);
		MaterialConfigNode.WriteVector3("EmmisiveColor", TempVector3Value);
		MaterialConfigNode.WriteString("EmmisiveMap", EmissiveMap);

		//NORMAL
		zeLog("Writing normals data.");
		MaterialConfigNode.WriteBool("NormalMapEnabled", (MapFlag & ZE_MTMP_NORMALMAP) != 0);
		MaterialConfigNode.WriteString("NormalMap", NormalMap);

		//OPACITY
		MaterialConfigNode.WriteBool("OpacityEnabled", (MapFlag & ZE_MTMP_OPACITYMAP) != 0);
		MaterialConfigNode.WriteFloat("Opacity", Opacity);
		MaterialConfigNode.WriteString("OpacityMap", OpacityMap);

		//ENVIRONMENT
		zeLog("Writing environment data.");
		MaterialConfigNode.WriteString("EnvironmentMap", EnvironmentMap);

		MaterialConfigNode.CloseNode();
		MaterialSerialNode.CloseNode();
		MaterialWriter.Close();
		zeLog("Closing material file.");
		MaterialFile.Close();

		ZEMLNode* MaterialDOMNode = MaterialsNode->AddNode("Material");
		MaterialDOMNode->AddProperty("Name")->SetString(MaterialName);
		MaterialDOMNode->AddProperty("FilePath")->SetString(ResourceConfigurationDialog->GetResourceRelativePath(ZEString(FileName) , MaterialName + ".ZEMaterial"));

		ZEProgressDialog::GetInstance()->CloseTask();
	}

	return true;
}

bool ZE3dsMaxInteriorExporter::ProcessScene()
{
	ZESize ElectedNodeCount = 0;
	ZESize RoomNodeCount = 0;
	ZESize DoorNodeCount = 0;
	ZESize HelperNodeCount = 0;
	ZESize EntityNodeCount = 0;
	Tab<IGameNode*> MeshNodes = Scene->GetIGameNodeByType(IGameObject::IGAME_MESH);

	IGameNode* CurrentNode = NULL;
	IGameObject* CurrentObject = NULL;
	ZEString NodeZEType;

	for (ZESize I = 0; I < (ZESize)MeshNodes.Count(); I++)
	{
		CurrentNode = MeshNodes[I];
		CurrentObject = CurrentNode->GetIGameObject();

		if (!ZE3dsMaxUtils::GetProperty(CurrentObject, ZE_STRING_PROP, ZEString("ZEType"), NodeZEType))
			continue;

		if (NodeZEType.Equals("Room"))
		{
			RoomNodeCount++;
			Rooms.Append(1, &CurrentNode);
		}
		else if (NodeZEType.Equals("Door"))
		{
			DoorNodeCount++;
			Doors.Append(1, &CurrentNode);
		}
		else if (NodeZEType.Equals("ZEInteriorFileBrush"))
		{
			EntityNodeCount++;
		}
		else if (NodeZEType.Equals("ZEInteriorFileEntity"))
		{
			//Why is is empty...
		}
		else
			ElectedNodeCount++;
	}

	Tab<IGameNode*> HelperNodes = Scene->GetIGameNodeByType(IGameObject::IGAME_HELPER);

	for (ZESize I = 0; I < (ZESize)HelperNodes.Count(); I++)
	{
		CurrentNode = HelperNodes[I];
		CurrentObject = CurrentNode->GetIGameObject();

		if (!ZE3dsMaxUtils::GetProperty(CurrentObject, ZE_STRING_PROP, ZEString("ZEType"), NodeZEType))
			continue;

		if (NodeZEType.Equals("Helper"))
		{
			HelperNodeCount++;
			Helpers.Append(1, &CurrentNode);
		}
		else
			ElectedNodeCount++;
	}

	zeLog("Room Count: %d, Door Count: %d, Helper Count: %d, Entity Count: %d, Elected Node Count: %d", RoomNodeCount, DoorNodeCount, HelperNodeCount, EntityNodeCount, ElectedNodeCount);

	return true;
}

void ZE3dsMaxInteriorExporter::CollectResources()
{
	ZESize RoomNodeCount = 0;

	Tab<IGameNode*>		Nodes = Scene->GetIGameNodeByType(IGameObject::IGAME_MESH);
	Tab<IGameNode*>		ResourceRooms;
	Tab<IGameMaterial*>	ResourceMaterials;

	for (ZESize I = 0; I < (ZESize)Nodes.Count(); I++)
	{
		IGameNode* CurrentNode = Nodes[I];
		IGameObject* CurrentObject = CurrentNode->GetIGameObject();
		ZEString NodeZEType;

		if (!ZE3dsMaxUtils::GetProperty(CurrentObject, ZE_STRING_PROP, ZEString("ZEType"), NodeZEType))
			continue;

		if (NodeZEType.Equals("Room"))
		{
			RoomNodeCount++;
			ResourceRooms.Append(1, &CurrentNode);
		}
	}

	for (ZESize I = 0; I < (ZESize)ResourceRooms.Count(); I++)
	{
		IGameNode* CurrentNode = ResourceRooms[I];
		IGameObject* CurrentObject = CurrentNode->GetIGameObject();

		IGameMesh* Mesh = (IGameMesh*)CurrentObject;
		Mesh->InitializeData();
		Mesh->InitializeBinormalData();
		Mesh->SetUseWeightedNormals();

		for (ZESize I = 0; I < (ZESize)Mesh->GetNumberOfFaces(); I++)
		{
			FaceEx* Face;
			Face = Mesh->GetFace((ZEInt32)I);
			IGameMaterial* CurrentMaterial = Mesh->GetMaterialFromFace((ZEInt32)I);

			if (CurrentMaterial == NULL)
				zeError("Face %d of room \"%s\" does not have valid material. Can not collect resource.", I, ZEString(CurrentNode->GetName()).ToCString());

			bool IsFound = false;

			for(ZESize J = 0; J < (ZESize)ResourceMaterials.Count(); J++)
				if (ResourceMaterials[J] == CurrentMaterial)
					IsFound = true;

			if (!IsFound)
				ResourceMaterials.Append(1, &CurrentMaterial);
		}
	}

	for (ZESize I = 0; I < (ZESize)ResourceMaterials.Count(); I++)
	{
		IGameMaterial* NodeMaterial = ResourceMaterials[I];
		ZEString MaterialName = NodeMaterial->GetMaterialName();

		ResourceConfigurationDialog->AddResource(MaterialName + ".ZEMaterial", "Material", ZEString(), ZE_ROAA_COPY_OVERWRITE);

		ZESize NumberOfMaps = (ZESize)NodeMaterial->GetNumberOfTextureMaps();

		for (ZESize N = 0; N < NumberOfMaps; N++)
		{
			IGameTextureMap* CurrentTexture = NodeMaterial->GetIGameTextureMap((ZEInt32)N);
			switch(CurrentTexture->GetStdMapSlot())
			{
				case ID_AM: // Ambient
					break;

				case ID_DI: // Diffuse
					ResourceConfigurationDialog->AddResource(ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName(), "Image", CurrentTexture->GetBitmapFileName(), ZEString());
					break;

				case ID_SP: // Specular
					ResourceConfigurationDialog->AddResource(ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName(), "Image", CurrentTexture->GetBitmapFileName(), ZEString());
					break;

				case ID_SI:	// Emissive
					ResourceConfigurationDialog->AddResource(ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName(), "Image", CurrentTexture->GetBitmapFileName(), ZEString());
					break;

				case ID_OP:	// Opacity 
					ResourceConfigurationDialog->AddResource(ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName(), "Image", CurrentTexture->GetBitmapFileName(), ZEString());
					break;

				case ID_FI:	// Filter color 
					break;

				case ID_BU: // Bump 
					ResourceConfigurationDialog->AddResource(ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName(), "Image", CurrentTexture->GetBitmapFileName(), ZEString());
					break;

				case ID_RL: // Reflection - Environment
					ResourceConfigurationDialog->AddResource(ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName(), "Image", CurrentTexture->GetBitmapFileName(), ZEString());
					break;

				case ID_RR: // Refraction 
					break;

				default:
					break;
			}
			
		}
	}
}
