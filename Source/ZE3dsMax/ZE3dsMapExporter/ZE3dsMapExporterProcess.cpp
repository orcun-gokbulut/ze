//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZE3dsMapExporterProcess.cpp
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


#include "ZETypes.h"
#include "ZE3dsMapExporter.h"


inline ZEVector2 MAX_TO_ZE(const Point2& Point)
{
	return ZEVector2(Point.x, Point.y);
}

inline ZEVector3 MAX_TO_ZE(const Point3& Point)
{
	return ZEVector3(Point.x, Point.y, Point.z);
}

inline ZEQuaternion MAX_TO_ZE(const Quat& Quaternion)
{
	return ZEQuaternion(Quaternion.w, Quaternion.x, Quaternion.y, Quaternion.z);
}

inline Point2 ZE_TO_MAX(const ZEVector2& Vector)
{
	return Point2(Vector.x, Vector.y);
}

inline Point3 ZE_TO_MAX(const ZEVector3& Vector)
{
	return Point3(Vector.x, Vector.y, Vector.z);
}

inline Quat ZE_TO_MAX(const ZEQuaternion& Quaternion)
{
	return Quat(Quaternion.x, Quaternion.y, Quaternion.z, Quaternion.w);
}

template <typename T>
bool GetProperty(IExportEntity * Object, PropType Type, const char* Property, T& Value)
{ 
	if (Object == NULL)
		return false;

	IGameProperty* Prop = Object->GetIPropertyContainer()->QueryProperty(Property);
	if (Prop != NULL && Prop->GetType() == Type)
	{
		if (Prop->GetPropertyValue(Value))
			return true;
		else
			return false;
	}
	else
		return false;
}

template <>
bool GetProperty<const char*>(IExportEntity * Object, PropType Type, const char* Property, const char*& Value)
{ 
	if (Object == NULL)
		return false;

	IGameProperty* Prop = Object->GetIPropertyContainer()->QueryProperty(Property);
	if (Prop != NULL && Prop->GetType() == Type)
	{
		if (Prop->GetPropertyValue(Value))
		{
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

template <>
bool GetProperty<bool>(IExportEntity* Object, PropType Type, const char* Property, bool& Value)
{ 
	ZEInt Temp;
	if (Object == NULL)
		return false;

	IGameProperty* Prop = Object->GetIPropertyContainer()->QueryProperty(Property);
	if (Prop != NULL && Prop->GetType() == Type)
	{
		if (Prop->GetPropertyValue(Temp))
		{
			Value = Temp == 0;
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

template <>
bool GetProperty<ZEUInt>(IExportEntity* Object, PropType Type, const char* Property, ZEUInt& Value)
{ 
	ZEInt Temp;
	if (Object == NULL)
		return false;

	IGameProperty* Prop = Object->GetIPropertyContainer()->QueryProperty(Property);
	if (Prop != NULL && Prop->GetType() == Type)
	{
		if (Prop->GetPropertyValue(Temp))
		{
			Value = Temp;
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

template<>
bool GetProperty<INode*>(IExportEntity* Object, PropType Type, const char* Property, INode*& Value)
{
	if (Object == NULL)
		return false;
	IGameProperty* Prop = Object->GetIPropertyContainer()->QueryProperty(Property);
	if (Prop != NULL)
	{
		IParamBlock2* ParamBlock =  Prop->GetMaxParamBlock2();
		ZEInt ParamId = ParamBlock->IndextoID(Prop->GetParamBlockIndex());
		if (ParamBlock->GetParameterType(ParamId) != TYPE_INODE)
			return false;

		Value = ParamBlock->GetINode(ParamId);
		return true;
	}
	else
	{
		Value = NULL;
		return false;
	}
}

bool ZE3dsMapExporter::GetRelativePath(const char* RealPath, char* RelativePath)
{
	if (strnicmp(RealPath, Options.ZinekDir, strlen(Options.ZinekDir)) == 0)
	{
		strcpy(RelativePath, RealPath + strlen(Options.ZinekDir) + 1);
		return true;
	}
	else
	{
		RelativePath[0] = '\0';
		return false;
	}
}

ZEInt ZE3dsMapExporter::ProcessFaceMaterial(IGameMaterial* Material)
{
	for (ZESize I = 0; I < (ZESize)Materials.Count(); I++)
		if (Materials[I] == Material)
			return (ZEInt)I;
	Materials.Append(1, &Material);
	return Materials.Count() - 1;
}

ZEInt ZE3dsMapExporter::FindPortalIndex(IGameNode* Node)
{
	for (ZESize I = 0; I < (ZESize)Portals.Count(); I++)
		if (Node == Portals[I]) return (ZEInt)I;

	return -1;
}

bool ZE3dsMapExporter::ProcessDoors()
{
	zepdLog("Processing Portal Doors...");

	INode* PortalANode;
	INode* PortalBNode;
	bool IsOpen;
	float PlaneWidth, PlaneLength;
	for (ZESize I = 0; I < (ZESize)Doors.Count(); I++)
	{
		IGameNode* CurrentNode = Doors[I];
		IGameObject* CurrentObject = CurrentNode->GetIGameObject();

		zepdLog("\tProcessing Portal Door \"%s\" (%Iu/%d)", CurrentNode->GetName(), I + 1, Doors.Count());

		GetProperty(CurrentObject, IGAME_FLOAT_PROP, "Width", PlaneWidth);
		GetProperty(CurrentObject, IGAME_FLOAT_PROP, "Length", PlaneLength);
		GetProperty(CurrentObject, IGAME_UNKNOWN_PROP, "PortalA", PortalANode);
		GetProperty(CurrentObject, IGAME_UNKNOWN_PROP, "PortalB", PortalBNode);
		GetProperty(CurrentObject, IGAME_INT_PROP, "IsOpen", IsOpen);
		if (PortalANode == NULL || PortalBNode == NULL || PortalANode == PortalBNode)
		{
			zepdWarning("Portal door \"%s\" has wrong parameters.", CurrentNode->GetName());
			return false;
		}

		ZEInt PortalAIndex = FindPortalIndex(Scene->GetIGameNode(PortalANode));
		ZEInt PortalBIndex = FindPortalIndex(Scene->GetIGameNode(PortalBNode));

		ZEMapFileDoor* Door = Map.Doors.Add();
	
		strncpy(Door->Name, CurrentNode->GetName(), ZE_MPFL_MAX_NAME_SIZE);
		Door->IsOpen = IsOpen;
		Door->PortalIds[0] = PortalAIndex;
		Door->PortalIds[1] = PortalBIndex;
		
		GMatrix Matrix = CurrentNode->GetObjectTM();
		ZEMatrix4x4 WorldMatrix;
		ZEMatrix4x4::Transpose(WorldMatrix, *((ZEMatrix4x4*)&Matrix));
		ZEMatrix4x4::Transform(Door->Rectangle.P1, WorldMatrix, ZEVector3(-PlaneWidth, 0.0f , PlaneLength));
		ZEMatrix4x4::Transform(Door->Rectangle.P2, WorldMatrix, ZEVector3(PlaneWidth, 0.0f , PlaneLength));
		ZEMatrix4x4::Transform(Door->Rectangle.P3, WorldMatrix, ZEVector3(PlaneWidth, 0.0f , -PlaneLength));
		ZEMatrix4x4::Transform(Door->Rectangle.P4, WorldMatrix, ZEVector3(-PlaneWidth, 0.0f , -PlaneLength));
	}

	return true;
}

void ProcessPhysicalMesh(ZEMapFilePhysicalMesh* PhysicalMesh, IGameObject* Object)
{
	IGameMesh* Mesh = (IGameMesh*)Object;
	Mesh->InitializeData();
	PhysicalMesh->Polygons.SetCount(Mesh->GetNumberOfFaces());
	for (ZESize I = 0; I < (ZESize)Mesh->GetNumberOfFaces(); I++)
	{
		FaceEx* Face = Mesh->GetFace((ZEInt)I);
		PhysicalMesh->Polygons[I].Indices[0] = Face->vert[0]; 
		PhysicalMesh->Polygons[I].Indices[1] = Face->vert[1]; 
		PhysicalMesh->Polygons[I].Indices[2] = Face->vert[2]; 
	}

	PhysicalMesh->Vertices.SetCount(Mesh->GetNumberOfVerts());
	for (ZESize I = 0; I < (ZESize)Mesh->GetNumberOfVerts(); I++)
	{
		Point3 Vertex =	Mesh->GetVertex((ZEInt)I, false);
		PhysicalMesh->Vertices[I] = MAX_TO_ZE(Vertex);
	}
}

bool ZE3dsMapExporter::ProcessPortals()
{
	zepdLog("Processing portals...");
	Map.Portals.SetCount((ZESize)Portals.Count());
	for (ZESize I = 0; I < (ZESize)Portals.Count(); I++)
	{
		IGameNode* CurrentNode = Portals[I];
		zepdOutput("\tProcessing Portal \"%s\" (%Iu/%d)\r\n", CurrentNode->GetName(), I + 1, Portals.Count());
		IGameObject* CurrentObject = CurrentNode->GetIGameObject();
		ZEMapFilePortal* CurrentFilePortal = &Map.Portals[I];
		bool PhysicalMeshEnabled, PhysicalMeshUseSelf;
		INode* PhysicalMeshMaxNode;
		IGameNode* PhysicalMeshNode;

		strncpy(CurrentFilePortal->Name, CurrentNode->GetName(), ZE_MPFL_MAX_NAME_SIZE);

		if (CurrentFilePortal->GenerateOctree)
			zepdWarning("Octree is not supported. Portal : \"%s\"", CurrentFilePortal->Name);
		//GetProperty(CurrentObject, IGAME_INT_PROP, "GenerateOctree", CurrentFilePortal->GenerateOctree);
		//GetProperty(CurrentObject, IGAME_INT_PROP, "MaxOctreeDepth", CurrentFilePortal->MaxOctreeDepth);
		CurrentFilePortal->GenerateOctree = false;
		CurrentFilePortal->MaxOctreeDepth = 0;

		GetProperty(CurrentObject, IGAME_INT_PROP, "PhysicalMeshEnabled", PhysicalMeshEnabled);
		GetProperty(CurrentObject, IGAME_INT_PROP, "PhysicalMeshUseSelf", PhysicalMeshUseSelf);
		GetProperty(CurrentObject, IGAME_UNKNOWN_PROP, "PhysicalMesh", PhysicalMeshMaxNode);
		
		// Load physical mesh
		if (PhysicalMeshEnabled)
			if (PhysicalMeshUseSelf)
				ProcessPhysicalMesh(&CurrentFilePortal->PhysicalMesh, CurrentObject);
			else
			{
				PhysicalMeshNode = Scene->GetIGameNode(PhysicalMeshMaxNode);
				ProcessPhysicalMesh(&CurrentFilePortal->PhysicalMesh, PhysicalMeshNode->GetIGameObject());
			}
		else
		{
			CurrentFilePortal->PhysicalMesh.Polygons.SetCount(0);
			CurrentFilePortal->PhysicalMesh.Vertices.SetCount(0);
		}

		// Load geometry
		IGameMesh* Mesh = (IGameMesh*)CurrentObject;
		Mesh->InitializeData();
		Mesh->InitializeBinormalData();
		Mesh->SetUseWeightedNormals();
		CurrentFilePortal->Polygons.SetCount(Mesh->GetNumberOfFaces());
		
		ZEMatrix4x4 WorldMatrix,  Temp4x4;
		ZEMatrix3x3 WorldInvTrspsMatrix, Temp3x3;

		memcpy(&Temp4x4, &CurrentNode->GetObjectTM().Inverse(), sizeof(ZEMatrix4x4));
		ZEMatrix4x4::Transpose(WorldMatrix, Temp4x4);
		ZEMatrix3x3::Create(Temp3x3, 
			WorldMatrix.M11, WorldMatrix.M12, WorldMatrix.M13, 
			WorldMatrix.M21, WorldMatrix.M22, WorldMatrix.M23, 
			WorldMatrix.M31, WorldMatrix.M32, WorldMatrix.M33);

		ZEMatrix3x3::Transpose(WorldInvTrspsMatrix, Temp3x3);

		for (ZESize I = 0; I < (ZESize)Mesh->GetNumberOfFaces(); I++)
		{
			FaceEx* Face;
			Face = Mesh->GetFace((ZEInt)I);
			if (Mesh->GetMaterialFromFace((ZEInt)I) == NULL)
			{
				zepdError("Face %d of portal \"%s\" does not have valid material.", I, CurrentNode->GetName());
				return false;
			}

			CurrentFilePortal->Polygons[I].Material = ProcessFaceMaterial(Mesh->GetMaterialFromFace((ZEInt)I));
			for (ZESize N = 0; N < 3; N++)
			{
				ZEMapFileVertex* Vertex = &CurrentFilePortal->Polygons[I].Vertices[N];
				ZEInt BinormalTangentIndex = Mesh->GetFaceVertexTangentBinormal((ZEInt)I, (ZEInt)N);

				Point3 Temp;
				Mesh->GetVertex(Face->vert[N], Temp, false);
				Vertex->Position = MAX_TO_ZE(Temp);
				
				/*Mesh->GetNormal(I, N, *(Point3*)&Vertex->Normal, false);
				ZEVector3::Normalize(Vertex->Normal, Vertex->Normal);
				
				Mesh->GetTangent(BinormalTangentIndex, *(Point3*)&Vertex->Tangent);
				ZEVector3::Normalize(Vertex->Tangent, Vertex->Tangent);
				
				Mesh->GetBinormal(BinormalTangentIndex, *(Point3*)&Vertex->Binormal);
				ZEVector3::Normalize(Vertex->Binormal, Vertex->Binormal);
				
				Mesh->GetTexVertex(Face->texCoord[N], *(Point2*)&Vertex->Texcoord);*/

				
				ZEVector3 Normal;
				Mesh->GetNormal(Face->norm[N], Temp, true);
				Normal = MAX_TO_ZE(Temp);
				ZEMatrix3x3::Transform(Vertex->Normal, WorldInvTrspsMatrix, Normal);
				ZEVector3::Normalize(Vertex->Normal, Vertex->Normal);

				Mesh->GetTangent(BinormalTangentIndex, Temp);
				Vertex->Tangent = MAX_TO_ZE(Temp);
				Mesh->GetBinormal(BinormalTangentIndex, Temp);
				Vertex->Binormal = MAX_TO_ZE(Temp);


				/*
				ZEMatrix3x3::Transform(Vertex->Tangent, WorldInvTrspsMatrix, *(ZEVector3*)&Mesh->GetTangent(BinormalTangentIndex));
				ZEVector3::Normalize(Vertex->Tangent, Vertex->Tangent);
				ZEMatrix3x3::Transform(Vertex->Binormal, WorldInvTrspsMatrix, *(ZEVector3*)&Mesh->GetBinormal(BinormalTangentIndex));
				ZEVector3::Normalize(Vertex->Binormal, Vertex->Binormal);*/

				Mesh->GetTexVertex(Face->texCoord[N], *(Point2*)&Vertex->Texcoord);
			}
		}
	}

	return true;
}

bool ZE3dsMapExporter::ProcessMaterials()
{
	zepdLog("Processing materials...");
	Map.Materials.SetCount((ZESize)Materials.Count());
	for (ZESize I = 0; I < (ZESize)Materials.Count(); I++)
	{
		IGameMaterial* NodeMaterial = Materials[I];

		zepdLog("\tProcessing material \"%s\" (%Iu/%d)", NodeMaterial->GetMaterialName(), I + 1, Materials.Count());
		ZEMapFileMaterial* CurrentMaterial = &Map.Materials[I];
		ZeroMemory(CurrentMaterial, sizeof(ZEMapFileMaterial));

		CurrentMaterial->LightningEnabled = true;

		bool ExportBool = false;
		GetProperty(NodeMaterial, IGAME_INT_PROP, "wire", ExportBool);
		CurrentMaterial->Wireframe = ExportBool;
		ExportBool = false;
		GetProperty(NodeMaterial, IGAME_INT_PROP, "twoSided", ExportBool);
		CurrentMaterial->TwoSided = ExportBool;

		NodeMaterial->GetOpacityData()->GetPropertyValue(CurrentMaterial->Opasity);
		NodeMaterial->GetAmbientData()->GetPropertyValue(*(Point3*)&CurrentMaterial->AmbientColor);
		NodeMaterial->GetDiffuseData()->GetPropertyValue(*(Point3*)&CurrentMaterial->DiffuseColor);
		NodeMaterial->GetSpecularData()->GetPropertyValue(*(Point3*)&CurrentMaterial->SpecularColor);
		NodeMaterial->GetEmissiveData()->GetPropertyValue(*(Point3*)&CurrentMaterial->EmmisiveColor);
		NodeMaterial->GetSpecularLevelData()->GetPropertyValue(CurrentMaterial->SpecularFactor);
		NodeMaterial->GetEmissiveAmtData()->GetPropertyValue(CurrentMaterial->EmmisiveFactor);

		CurrentMaterial->ShaderComponents = 0;

		for (ZEInt N = 0; N < NodeMaterial->GetNumberOfTextureMaps(); N++)
		{
			// char RelativePath[ZE_MPFL_MAX_FILENAME_SIZE];

			IGameTextureMap* CurrentTexture = NodeMaterial->GetIGameTextureMap(N);
			switch(CurrentTexture->GetStdMapSlot())
			{
				case ID_AM: // Ambient
					break;
				case ID_DI: // Diffuse
					/*if (!GetRelativePath(CurrentTexture->GetBitmapFileName(), RelativePath))
					{
						zepdError("Material texture's relative path does not matches with Zinek Engine Resource directory. Please check your Zinek Engine resource directory. disabling Diffuse Map.\r\n"
						"\tMaterial Name : \"%s\", Texture Slot : Diffuse Map, File path : \"%s\" (Path is not in Zinek Engine resource directory)",
						NodeMaterial->GetMaterialName(), CurrentTexture->GetBitmapFileName());
						continue;
					}*/
					CurrentMaterial->ShaderComponents |= ZESHADER_DIFFUSEMAP;
					strncpy(CurrentMaterial->DiffuseMap, CurrentTexture->GetBitmapFileName(), ZE_MPFL_MAX_FILENAME_SIZE);
					break;
				case ID_SP: // Specular
					/*if (!GetRelativePath(CurrentTexture->GetBitmapFileName(), RelativePath))
					{
						zepdError("Material texture's relative path does not matches with Zinek Engine Resource directory. Please check your Zinek Engine resource directory. disabling Specular Map.\r\n"
						"\tMaterial Name : \"%s\", Texture Slot : Specular Map, File path : \"%s\" (Path is not in Zinek Engine resource directory)",
						NodeMaterial->GetMaterialName(), CurrentTexture->GetBitmapFileName());
						continue;
					}*/
					CurrentMaterial->ShaderComponents |= ZESHADER_SPECULARMAP;
					strncpy(CurrentMaterial->SpecularMap, CurrentTexture->GetBitmapFileName(), ZE_MPFL_MAX_FILENAME_SIZE);
					break;
				case ID_SI:	// Emmisive
					/*if (!GetRelativePath(CurrentTexture->GetBitmapFileName(), RelativePath))
					{
						zepdError("Material texture's relative path does not matches with Zinek Engine Resource directory. Please check your Zinek Engine resource directory. disabling Emmisive Map.\r\n"
						"\tMaterial Name : \"%s\", Texture Slot : Self Ilimunation Map, File path : \"%s\" (Path is not in Zinek Engine resource directory)",
						NodeMaterial->GetMaterialName(), CurrentTexture->GetBitmapFileName());
						continue;
					}*/
					CurrentMaterial->ShaderComponents |= ZESHADER_EMMISIVEMAP;
					strncpy(CurrentMaterial->EmmisiveMap, CurrentTexture->GetBitmapFileName(), ZE_MPFL_MAX_FILENAME_SIZE);
					break;
				case ID_OP:	// Opacity 
					/*if (!GetRelativePath(CurrentTexture->GetBitmapFileName(), RelativePath))
					{
						zepdError("Material texture's relative path does not matches with Zinek Engine Resource directory. Please check your Zinek Engine resource directory. disabling Opacity Map.\r\n"
						"\tMaterial Name : \"%s\", Texture Slot : Opacity Map, File path : \"%s\" (Path is not in Zinek Engine resource directory)",
						NodeMaterial->GetMaterialName(), CurrentTexture->GetBitmapFileName());
						continue;
					}*/
					CurrentMaterial->ShaderComponents |= ZESHADER_OCAPASITYMAP;
					strncpy(CurrentMaterial->OpasityMap, CurrentTexture->GetBitmapFileName(), ZE_MPFL_MAX_FILENAME_SIZE);
					break;
				case ID_FI:	// Filter color 
					/*
					if (!GetRelativePath(CurrentTexture->GetBitmapFileName(), RelativePath))
					{
						zepdError("Material texture's relative path does not matches with Zinek Engine Resource directory. Please check your Zinek Engine resource directory. disabling Diffuse Map.\r\n"
						"\tMaterial Name : \"%s\", Texture Slot : Diffuse Map, File path : \"%s\" (Path is not in Zinek Engine resource directory)",
						NodeMaterial->GetMaterialName(), CurrentTexture->GetBitmapFileName());
						continue;
					}
					CurrentMaterial->ShaderComponents |= ZESHADER_DIFFUSEMAP;
					GetRelativePath(CurrentTexture->GetBitmapFileName(), RelativePath);
					strncpy(CurrentMaterial->DetailMap, RelativePath, ZE_MPFL_MAX_FILENAME_SIZE);
					*/
					break;
				case ID_BU: // Bump 
					/*if (!GetRelativePath(CurrentTexture->GetBitmapFileName(), RelativePath))
					{
						zepdError("Material texture's relative path does not matches with Zinek Engine Resource directory. Please check your Zinek Engine resource directory. disabling Bump Map.\r\n"
						"\tMaterial Name : \"%s\", Texture Slot : Bump Map, File path : \"%s\" (Path is not in Zinek Engine resource directory)",
						NodeMaterial->GetMaterialName(), CurrentTexture->GetBitmapFileName());
						continue;
					}*/
					CurrentMaterial->ShaderComponents |= ZESHADER_NORMALMAP;
					strncpy(CurrentMaterial->NormalMap, CurrentTexture->GetBitmapFileName(), ZE_MPFL_MAX_FILENAME_SIZE);
					break;
				case ID_RL: // Reflection 
					/*if (!GetRelativePath(CurrentTexture->GetBitmapFileName(), RelativePath))
					{
						zepdError("Material texture's relative path does not matches with Zinek Engine Resource directory. Please check your Zinek Engine resource directory. disabling Reflection Map.\r\n"
						"\tMaterial Name : \"%s\", Texture Slot : Reflection Map, File path : \"%s\" (Path is not in Zinek Engine resource directory)",
						NodeMaterial->GetMaterialName(), CurrentTexture->GetBitmapFileName());
						continue;
					}*/
					CurrentMaterial->ShaderComponents |= ZESHADER_REFLECTION;
					strncpy(CurrentMaterial->ReflectionMap, CurrentTexture->GetBitmapFileName(), ZE_MPFL_MAX_FILENAME_SIZE);
					break; 
				case ID_RR: // Refraction 
					/*if (!GetRelativePath(CurrentTexture->GetBitmapFileName(), RelativePath))
					{
						zepdError("Material texture's relative path does not matches with Zinek Engine Resource directory. Please check your Zinek Engine resource directory. disabling Refraction Map.\r\n"
						"\tMaterial Name : \"%s\", Texture Slot : Refraction Map, File path : \"%s\" (Path is not in Zinek Engine resource directory)",
						NodeMaterial->GetMaterialName(), CurrentTexture->GetBitmapFileName());
						continue;
					}*/
					CurrentMaterial->ShaderComponents |= ZESHADER_REFRACTION;
					strncpy(CurrentMaterial->RefractionMap, CurrentTexture->GetBitmapFileName(), ZE_MPFL_MAX_FILENAME_SIZE);
					break;
			}

			if (CurrentMaterial->Opasity != 1.0f || strncmp(CurrentMaterial->OpasityMap, "", ZE_MPFL_MAX_FILENAME_SIZE) !=0)
				CurrentMaterial->Transparant = true;
		}		
	}

	return true;
}

bool ZE3dsMapExporter::ProcessScene()
{
	ZESize ElectedNodeCount = 0;
	ZESize PortalNodeCount = 0;
	ZESize PortalDoorNodeCount = 0;
	ZESize EntityNodeCount = 0;
	Tab<IGameNode*> Nodes = Scene->GetIGameNodeByType(IGameObject::IGAME_MESH);

	for (ZESize I = 0; I < (ZESize)Nodes.Count(); I++)
	{
		IGameNode* CurrentNode = Nodes[I];;
		IGameObject* CurrentObject = CurrentNode->GetIGameObject();
		const char* NodeZEType;

		if (!GetProperty(CurrentObject, IGAME_STRING_PROP, "ZEType", NodeZEType))
			continue;

		if (strcmp(NodeZEType, "Portal") == 0)
		{
			PortalNodeCount++;
			Portals.Append(1, &CurrentNode);
		}
		else if (strcmp(NodeZEType, "PortalDoor") == 0)
		{
			PortalDoorNodeCount++;
			Doors.Append(1, &CurrentNode);
		}
		else if (strcmp(NodeZEType, "ZEMapFileBrush") == 0)
		{
			EntityNodeCount++;
		}
		else if (strcmp(NodeZEType, "ZEMapFileEntity") == 0)
		{
		}
		else
			ElectedNodeCount++;
	}
	zepdLog("\tPortal Count: %d, Portal Doors Count: %d, Entity Count: %d, Elected Node Count: %d", PortalNodeCount, PortalDoorNodeCount, EntityNodeCount, ElectedNodeCount);

	return true;
}
