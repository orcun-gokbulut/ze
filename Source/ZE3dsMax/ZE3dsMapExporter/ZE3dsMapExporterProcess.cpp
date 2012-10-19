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
#include "ZEToolComponents\ZEProgressDialog\ZEProgressDialog.h"
#include "ZEToolComponents\ZEResourceConfigurationWidget\ZEResourceConfigurationWidget.h"
#include "ZEML\ZEMLProperty.h"
#include "ZEML\ZEMLNode.h"
#include "ZEFile\ZEFile.h"
#include "ZEML\ZEMLSerialWriter.h"
#include "ZEFile\ZEFileInfo.h"
#include "ZEFile\ZEPathUtils.h"
#include "ZEFile\ZEDirectoryInfo.h"
#include "ZEFile\ZEFileOperations.h"

#define ZE_MTMP_DIFFUSEMAP						2
#define ZE_MTMP_NORMALMAP						4
#define ZE_MTMP_SPECULARMAP						8
#define ZE_MTMP_EMISSIVEMAP						16
#define ZE_MTMP_OPACITYMAP						32
#define ZE_MTMP_DETAILDIFFUSEMAP				64
#define ZE_MTMP_DETAILNORMALMAP					128
#define ZE_MTMP_ENVIRONMENTMAP					256
#define ZE_MTMP_REFRACTION						512
#define ZE_MTMP_LIGHTMAP						1024
#define ZE_MTMP_DISTORTIONMAP					2048

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
			if(Temp == 0)
				Value = false;
			else
				Value = true;

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
	zeLog("Processing Portal Doors...");
	ZEMLNode* DoorsNode = MapNode.AddSubNode("Doors");

	INode* PortalANode;
	INode* PortalBNode;
	bool IsOpen;
	float PlaneWidth, PlaneLength;

	for (ZESize I = 0; I < (ZESize)Doors.Count(); I++)
	{
		ZEMLNode* DoorNode = DoorsNode->AddSubNode("Door");

		IGameNode* CurrentNode = Doors[I];
		IGameObject* CurrentObject = CurrentNode->GetIGameObject();
		ZEProgressDialog::GetInstance()->OpenTask(CurrentNode->GetName());
		zeLog("Processing Portal Door \"%s\" (%Iu/%d)", CurrentNode->GetName(), I + 1, Doors.Count());
		
		DoorNode->AddProperty("Name", CurrentNode->GetName());

		if(!GetProperty(CurrentObject, IGAME_FLOAT_PROP, "Width", PlaneWidth))
			zeError("Can not find door property With");

		if(!GetProperty(CurrentObject, IGAME_FLOAT_PROP, "Length", PlaneLength))
			zeError("Can not find door property Height");

		if(!GetProperty(CurrentObject, IGAME_INT_PROP, "IsOpen", IsOpen))
			zeError("Can not find door property IsOpen");

		DoorNode->AddProperty("IsOpen", IsOpen);

		if(!GetProperty(CurrentObject, IGAME_UNKNOWN_PROP, "PortalA", PortalANode))
			zeError("Can not find door property PortalA");

		if(!GetProperty(CurrentObject, IGAME_UNKNOWN_PROP, "PortalB", PortalBNode))
			zeError("Can not find door property PortalB");

		if (PortalANode == NULL || PortalBNode == NULL || PortalANode == PortalBNode)
		{
			zeWarning("Portal door \"%s\" has wrong parameters.", CurrentNode->GetName());
			return false;
		}

		ZEUInt32 PortalAIndex = FindPortalIndex(Scene->GetIGameNode(PortalANode));
		
		if(PortalAIndex < 0)
			zeError("Can nor find PortalAIndex.");
		
		DoorNode->AddProperty("PortalAIndex", PortalAIndex);

		ZEUInt32 PortalBIndex = FindPortalIndex(Scene->GetIGameNode(PortalBNode));

		if(PortalBIndex < 0)
			zeError("Can nor find PortalBIndex.");

		DoorNode->AddProperty("PortalBIndex", PortalBIndex);
	
		ZEVector3 Point1, Point2, Point3, Point4;

		GMatrix Matrix = CurrentNode->GetObjectTM();
		ZEMatrix4x4 WorldMatrix;
		ZEMatrix4x4::Transpose(WorldMatrix, *((ZEMatrix4x4*)&Matrix));
		ZEMatrix4x4::Transform(Point1, WorldMatrix, ZEVector3(-PlaneWidth, 0.0f , PlaneLength));
		ZEMatrix4x4::Transform(Point2, WorldMatrix, ZEVector3(PlaneWidth, 0.0f , PlaneLength));
		ZEMatrix4x4::Transform(Point3, WorldMatrix, ZEVector3(PlaneWidth, 0.0f , -PlaneLength));
		ZEMatrix4x4::Transform(Point4, WorldMatrix, ZEVector3(-PlaneWidth, 0.0f , -PlaneLength));

		ZEMLNode* RetangleNode = DoorNode->AddSubNode("Rectangle");
		RetangleNode->AddProperty("Point1", Point1);
		RetangleNode->AddProperty("Point2", Point2);
		RetangleNode->AddProperty("Point3", Point3);
		RetangleNode->AddProperty("Point4", Point4);

		ZEProgressDialog::GetInstance()->CloseTask();
	}

	return true;
}

void ProcessPhysicalMesh(IGameObject* Object, ZEMLNode* PhysicalMeshNode)
{
	IGameMesh* Mesh = (IGameMesh*)Object;

	if(!Mesh->InitializeData())
		zeError("Can not initialize mesh data for phsical mesh.");

	ZEArray<ZEMapFilePhysicalMeshPolygon> Polygons;
	Polygons.SetCount(Mesh->GetNumberOfFaces());

	if(Mesh->GetNumberOfFaces() == 0)
		zeError("Physical mesh face count is : 0.");

	for (ZESize I = 0; I < (ZESize)Mesh->GetNumberOfFaces(); I++)
	{
		FaceEx* Face = Mesh->GetFace((ZEInt)I);
		Polygons[I].Indices[0] = Face->vert[0]; 
		Polygons[I].Indices[1] = Face->vert[1]; 
		Polygons[I].Indices[2] = Face->vert[2]; 
	}

	PhysicalMeshNode->AddDataProperty("Polygons", Polygons.GetCArray(), sizeof(ZEMapFilePhysicalMeshPolygon) * Polygons.GetCount(), true);

	ZEArray<ZEVector3> Vertices;
	Vertices.SetCount(Mesh->GetNumberOfVerts());

	for (ZESize I = 0; I < (ZESize)Mesh->GetNumberOfVerts(); I++)
	{
		Point3 Vertex =	Mesh->GetVertex((ZEInt)I, false);
		Vertices[I] = MAX_TO_ZE(Vertex);
	}

	PhysicalMeshNode->AddDataProperty("Vertices", Vertices.GetCArray(), sizeof(ZEVector3) * Vertices.GetCount(), true);
}

bool ZE3dsMapExporter::ProcessPortals()
{
	zeLog("Processing portals...");

	ZEMLNode* PortalsNode = MapNode.AddSubNode("Portals");

	for (ZESize I = 0; I < (ZESize)Portals.Count(); I++)
	{
		ZEMLNode* PortalNode = PortalsNode->AddSubNode("Portal");

		IGameNode* CurrentNode = Portals[I];
		ZEProgressDialog::GetInstance()->OpenTask(CurrentNode->GetName());
		zeLog("Processing Portal \"%s\" (%Iu/%d)", CurrentNode->GetName(), I + 1, Portals.Count());
		IGameObject* CurrentObject = CurrentNode->GetIGameObject();
		bool PhysicalMeshEnabled, PhysicalMeshUseSelf;
		INode* PhysicalMeshMaxNode;
		IGameNode* PhysicalMeshNode;

		PortalNode->AddProperty("Name", CurrentNode->GetName());
		if(!GetProperty(CurrentObject, IGAME_INT_PROP, "PhysicalMeshEnabled", PhysicalMeshEnabled))
			zeError("Can not find portal property \"PhysicalMeshEnabled\".");

		if(!GetProperty(CurrentObject, IGAME_INT_PROP, "PhysicalMeshUseSelf", PhysicalMeshUseSelf))
			zeError("Can not find portal property \"PhysicalMeshUseSelf\".");

		if(PhysicalMeshEnabled && !PhysicalMeshUseSelf)
			if(GetProperty(CurrentObject, IGAME_UNKNOWN_PROP, "PhysicalMesh", PhysicalMeshMaxNode))
				zeError("Can not find portal property \"PhysicalMesh\".");

		if(PhysicalMeshEnabled)
		{
			if (PhysicalMeshUseSelf)
			{
				ZEMLNode* PhysicalMeshZEMLNode = PortalNode->AddSubNode("PhysicalMesh");
				PhysicalMeshZEMLNode->AddProperty("PhysicalMeshEnabled", PhysicalMeshEnabled);
				ProcessPhysicalMesh(CurrentObject, PhysicalMeshZEMLNode);
			}
			else
			{
				PhysicalMeshNode = Scene->GetIGameNode(PhysicalMeshMaxNode);
				if(PhysicalMeshNode == NULL)
				{
					zeWarning("Physical Mesh Node NULL.");
					zeWarning("Physical mesh disabled.");
					PhysicalMeshEnabled = false;
				}
				else
				{
					ZEMLNode* PhysicalMeshZEMLNode = PortalNode->AddSubNode("PhysicalMesh");
					PhysicalMeshZEMLNode->AddProperty("PhysicalMeshEnabled", PhysicalMeshEnabled);
					ProcessPhysicalMesh(PhysicalMeshNode->GetIGameObject(), PhysicalMeshZEMLNode);
				}
			}
		}

		IGameMesh* Mesh = (IGameMesh*)CurrentObject;
		
		if(!Mesh->InitializeData())
			zeError("Can not initialize mesh data.");

		if(!Mesh->InitializeBinormalData())
			zeError("Can not initialize mesh binormal data.");

		Mesh->SetUseWeightedNormals();

		ZEArray<ZEMapFilePolygon> Polygons;
		Polygons.SetCount(Mesh->GetNumberOfFaces());
		
		ZEMatrix4x4 WorldMatrix,  Temp4x4;
		ZEMatrix3x3 WorldInvTrspsMatrix, Temp3x3;

		memcpy(&Temp4x4, &CurrentNode->GetObjectTM().Inverse(), sizeof(ZEMatrix4x4));
		ZEMatrix4x4::Transpose(WorldMatrix, Temp4x4);
		ZEMatrix3x3::Create(Temp3x3, 
			WorldMatrix.M11, WorldMatrix.M12, WorldMatrix.M13, 
			WorldMatrix.M21, WorldMatrix.M22, WorldMatrix.M23, 
			WorldMatrix.M31, WorldMatrix.M32, WorldMatrix.M33);

		ZEMatrix3x3::Transpose(WorldInvTrspsMatrix, Temp3x3);

		if(Mesh->GetNumberOfFaces() == 0)
			zeError("Face count is : 0.");

		for (ZESize I = 0; I < (ZESize)Mesh->GetNumberOfFaces(); I++)
		{
			FaceEx* Face;
			Face = Mesh->GetFace((ZEInt)I);
			if (Mesh->GetMaterialFromFace((ZEInt)I) == NULL)
			{
				zeError("Face %d of portal \"%s\" does not have valid material.", I, CurrentNode->GetName());
				return false;
			}

			Polygons[I].Material = ProcessFaceMaterial(Mesh->GetMaterialFromFace((ZEInt)I));

			for (ZESize N = 0; N < 3; N++)
			{
				ZEMapFileVertex* Vertex = &Polygons[I].Vertices[N];
				ZEInt BinormalTangentIndex = Mesh->GetFaceVertexTangentBinormal((ZEInt)I, (ZEInt)N);

				Point3 Temp;
				if(!Mesh->GetVertex(Face->vert[N], Temp, false))
					zeError("Can not get vertex of face %d, vertex index : %d.", I, N);

				Vertex->Position = MAX_TO_ZE(Temp);
				
				ZEVector3 Normal;
				
				if(!Mesh->GetNormal(Face->norm[N], Temp, true))
					zeError("Can not get normal of face %d, normal index : %d.", I, N);

				Normal = MAX_TO_ZE(Temp);
				ZEMatrix3x3::Transform(Vertex->Normal, WorldInvTrspsMatrix, Normal);
				ZEVector3::Normalize(Vertex->Normal, Vertex->Normal);

				if(!Mesh->GetTangent(BinormalTangentIndex, Temp))
					zeError("Can not get tangent of face %d, tangent index : %d.", I, BinormalTangentIndex);

				Vertex->Tangent = MAX_TO_ZE(Temp);

				if(!Mesh->GetBinormal(BinormalTangentIndex, Temp))
					zeError("Can not get binormal of face %d, binormal index : %d.", I, BinormalTangentIndex);

				Vertex->Binormal = MAX_TO_ZE(Temp);

				if(!Mesh->GetTexVertex(Face->texCoord[N], *(Point2*)&Vertex->Texcoord))
					zeError("Can not get texture coordinate of face %d vertex %d.", I, N, BinormalTangentIndex);
			}
		}

		PortalNode->AddDataProperty("Polygons", Polygons.GetCArray(), sizeof(ZEMapFilePolygon) * Polygons.GetCount(), true);
		ZEProgressDialog::GetInstance()->CloseTask();
	}

	return true;
}

bool ZE3dsMapExporter::ProcessMaterials(const char* FileName)
{
	zeLog("Processing materials...");
	ZEMLNode* MaterialsNode = MapNode.AddSubNode("Materials");

	ZEString FileParentPath = ZEFileInfo::GetParentDirectory(FileName) + ZEPathUtils::GetSeperator();

	char DiffuseMap[ZE_MPFL_MAX_FILENAME_SIZE];
	char SpecularMap[ZE_MPFL_MAX_FILENAME_SIZE];
	char EmissiveMap[ZE_MPFL_MAX_FILENAME_SIZE];
	char OpacityMap[ZE_MPFL_MAX_FILENAME_SIZE];
	char NormalMap[ZE_MPFL_MAX_FILENAME_SIZE];
	char EnvironmentMap[ZE_MPFL_MAX_FILENAME_SIZE];

	ZESize MaterialCount = (ZESize)Materials.Count();
	zeLog("Material count : %d", MaterialCount);

	for (ZESize I = 0; I < MaterialCount; I++)
	{
		IGameMaterial* NodeMaterial = Materials[I];
		ZEString MaterialName = NodeMaterial->GetMaterialName();

		ZEResourceOption MaterialOption;
		if(!ResourceConfigurationDialog->GetOption(MaterialName + ".ZEMATERIAL", MaterialOption))
			zeError("Can not find resource option. Resource identifier : %s", (MaterialName + ".ZEMATERIAL").ToCString());

		if(!ResourceConfigurationDialog->GetCopyState(MaterialOption.Identifier))
			continue;

		ZEString MaterialFilePath = MaterialOption.ExportPath + ZEPathUtils::GetSeperator() + MaterialOption.Identifier;

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
		if (!MaterialFile.Open(MaterialOption.ExportPath + "\\" + MaterialName + ".ZEMATERIAL", ZE_FOM_WRITE, ZE_FCM_OVERWRITE))
		{
			zeError("Material process failed. Unable to open file for material: \"%s\"", MaterialName);
			return false;
		}
		zeLog("Material file succesfuly opened.");

		ZEInt NumberOfMaps = NodeMaterial->GetNumberOfTextureMaps();
		ZEInt32 MapFlag = 0;
		ZEString ResourceRelativePath;

		for (ZEInt N = 0; N < NumberOfMaps; N++)
		{
			char RelativePath[ZE_MPFL_MAX_FILENAME_SIZE];

			IGameTextureMap* CurrentTexture = NodeMaterial->GetIGameTextureMap(N);
			switch(CurrentTexture->GetStdMapSlot())
			{
			case ID_AM: // Ambient
				break;
			case ID_DI: // Diffuse
				if (!ResourceConfigurationDialog->GetOption(ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()), MaterialOption))
					zeError("Diffuse texture export path not found in resource options. Resource identifier : %s", ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()).ToCString());

				if(!ZEDirectoryInfo::IsDirectory(MaterialOption.ExportPath))
					zeError("Diffuse texture export path not valid . Resource identifier : %s", ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()).ToCString());
				else
				{
					MapFlag |= ZE_MTMP_DIFFUSEMAP;
					ResourceRelativePath = ResourceConfigurationDialog->GetResourceRelativePath(MaterialFilePath, MaterialOption.Identifier);
					strncpy(DiffuseMap, ResourceRelativePath.ToCString() , ZE_MPFL_MAX_FILENAME_SIZE);

					if(ResourceConfigurationDialog->GetCopyState(MaterialOption.Identifier))
					{
						ZEFileInfo FileInfo(MaterialOption.PhysicalPath);
						if(!ZEFileOperations::Copy(MaterialOption.ExportPath, &FileInfo, true))
							zeError("Can not copy resource, resource identifier : %s", MaterialOption.Identifier.ToCString());
						else
							zeLog("Resource copied successfully, resource identifier : %s", MaterialOption.Identifier);
					}
				}
				break;
			case ID_SP: // Specular
				if (!ResourceConfigurationDialog->GetOption(ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()), MaterialOption))
					zeError("Specular texture export path not found in resource options. Resource identifier : %s", ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()).ToCString());

				if(!ZEDirectoryInfo::IsDirectory(MaterialOption.ExportPath))
					zeError("Specular texture export path not valid . Resource identifier : %s", ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()).ToCString());
				else
				{
					MapFlag |= ZE_MTMP_SPECULARMAP;
					ResourceRelativePath = ResourceConfigurationDialog->GetResourceRelativePath(MaterialFilePath, ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()));
					strncpy(SpecularMap, ResourceRelativePath.ToCString(), ZE_MPFL_MAX_FILENAME_SIZE);

					if(ResourceConfigurationDialog->GetCopyState(MaterialOption.Identifier))
					{
						ZEFileInfo FileInfo(MaterialOption.PhysicalPath);
						if(!ZEFileOperations::Copy(MaterialOption.ExportPath, &FileInfo, true))
							zeError("Can not copy resource, resource identifier : %s", MaterialOption.Identifier.ToCString());
						else
							zeLog("Resource copied successfully, resource identifier : %s", MaterialOption.Identifier);
					}
				}
				break;
			case ID_SI:	// Emissive
				if (!ResourceConfigurationDialog->GetOption(ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()), MaterialOption))
					zeError("Emissive texture export path not found in resource options. Resource identifier : %s", ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()).ToCString());

				if(!ZEDirectoryInfo::IsDirectory(MaterialOption.ExportPath))
					zeError("Emissive texture export path not valid . Resource identifier : %s", ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()).ToCString());
				else
				{
					MapFlag |= ZE_MTMP_EMISSIVEMAP;
					ResourceRelativePath = ResourceConfigurationDialog->GetResourceRelativePath(MaterialFilePath, ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()));
					strncpy(EmissiveMap, ResourceRelativePath.ToCString(), ZE_MPFL_MAX_FILENAME_SIZE);

					if(ResourceConfigurationDialog->GetCopyState(MaterialOption.Identifier))
					{
						ZEFileInfo FileInfo(MaterialOption.PhysicalPath);
						if(!ZEFileOperations::Copy(MaterialOption.ExportPath, &FileInfo, true))
							zeError("Can not copy resource, resource identifier : %s", MaterialOption.Identifier.ToCString());
						else
							zeLog("Resource copied successfully, resource identifier : %s", MaterialOption.Identifier);
					}
				}
				break;
			case ID_OP:	// Opacity 
				if (!ResourceConfigurationDialog->GetOption(ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()), MaterialOption))
					zeError("Opacity texture export path not found in resource options. Resource identifier : %s", ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()).ToCString());

				if(!ZEDirectoryInfo::IsDirectory(MaterialOption.ExportPath))
					zeError("Opacity texture export path not valid . Resource identifier : %s", ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()).ToCString());
				else
				{
					MapFlag |= ZE_MTMP_OPACITYMAP;
					ResourceRelativePath = ResourceConfigurationDialog->GetResourceRelativePath(MaterialFilePath, ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()));
					strncpy(OpacityMap, ResourceRelativePath.ToCString(), ZE_MPFL_MAX_FILENAME_SIZE);

					if(ResourceConfigurationDialog->GetCopyState(MaterialOption.Identifier))
					{
						ZEFileInfo FileInfo(MaterialOption.PhysicalPath);
						if(!ZEFileOperations::Copy(MaterialOption.ExportPath, &FileInfo, true))
							zeError("Can not copy resource, resource identifier : %s", MaterialOption.Identifier.ToCString());
						else
							zeLog("Resource copied successfully, resource identifier : %s", MaterialOption.Identifier);
					}
				}
				break;
			case ID_FI:	// Filter color 
				break;
			case ID_BU: // Bump 
				if (!ResourceConfigurationDialog->GetOption(ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()), MaterialOption))
					zeError("Bump texture export path not found in resource options. Resource identifier : %s", ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()).ToCString());

				if(!ZEDirectoryInfo::IsDirectory(MaterialOption.ExportPath))
					zeError("Bump texture export path not valid . Resource identifier : %s", ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()).ToCString());
				else
				{
					MapFlag |= ZE_MTMP_NORMALMAP;
					ResourceRelativePath = ResourceConfigurationDialog->GetResourceRelativePath(MaterialFilePath, ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()));
					strncpy(NormalMap, ResourceRelativePath.ToCString(), ZE_MPFL_MAX_FILENAME_SIZE);

					if(ResourceConfigurationDialog->GetCopyState(MaterialOption.Identifier))
					{
						ZEFileInfo FileInfo(MaterialOption.PhysicalPath);
						if(!ZEFileOperations::Copy(MaterialOption.ExportPath, &FileInfo, true))
							zeError("Can not copy resource, resource identifier : %s", MaterialOption.Identifier.ToCString());
						else
							zeLog("Resource copied successfully, resource identifier : %s", MaterialOption.Identifier);
					}
				}
				break;
			case ID_RL: // Reflection - Environment
				if (!ResourceConfigurationDialog->GetOption(ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()), MaterialOption))
					zeError("Environment texture export path not found in resource options. Resource identifier : %s", ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()).ToCString());

				if(!ZEDirectoryInfo::IsDirectory(MaterialOption.ExportPath))
					zeError("Environment texture export path not valid . Resource identifier : %s", ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()).ToCString());
				else
				{
					MapFlag |= ZE_MTMP_ENVIRONMENTMAP;
					ResourceRelativePath = ResourceConfigurationDialog->GetResourceRelativePath(MaterialFilePath, ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()));
					strncpy(EnvironmentMap, ResourceRelativePath.ToCString(), ZE_MPFL_MAX_FILENAME_SIZE);

					if(ResourceConfigurationDialog->GetCopyState(MaterialOption.Identifier))
					{
						ZEFileInfo FileInfo(MaterialOption.PhysicalPath);
						if(!ZEFileOperations::Copy(MaterialOption.ExportPath, &FileInfo, true))
							zeError("Can not copy resource, resource identifier : %s", MaterialOption.Identifier.ToCString());
						else
							zeLog("Resource copied successfully, resource identifier : %s", MaterialOption.Identifier);
					}
				}
				break; 
			case ID_RR: // Refraction 
				break;
			}
			
		}

		zeLog("Writing material file.");
		ZEMLSerialRootNode MaterialSerialNode("Material", &MaterialFile);
		MaterialSerialNode.WriteProperty("Name", MaterialName);
		ZEMLSerialNode MaterialConfigNode = MaterialSerialNode.OpenNode("Configuration");

		MaterialConfigNode.WriteProperty("Name", "Default");
		bool TempBooleanValue = false;
		GetProperty(NodeMaterial, IGAME_INT_PROP, "wire", TempBooleanValue);
		MaterialConfigNode.WriteProperty("Wireframe", TempBooleanValue);

		TempBooleanValue = false;
		GetProperty(NodeMaterial, IGAME_INT_PROP, "twoSided", TempBooleanValue);
		MaterialConfigNode.WriteProperty("TwoSided", TempBooleanValue);

		MaterialConfigNode.WriteProperty("LightningEnabled", true); // Lightning Enabled is forced true;

		float Opacity = 0.0f;
		NodeMaterial->GetOpacityData()->GetPropertyValue(Opacity);

		if (Opacity != 1.0f || strncmp(OpacityMap, "", ZE_MPFL_MAX_FILENAME_SIZE) !=0)
			MaterialConfigNode.WriteProperty("Transparant", true);
		else
			MaterialConfigNode.WriteProperty("Transparant", false);

		//AMBIENT
		zeLog("Writing ambient data.");
		MaterialConfigNode.WriteProperty("AmbientEnabled", false); //Ambient is forced false
		ZEVector3 TempVector3Value = ZEVector3::Zero;
		NodeMaterial->GetAmbientData()->GetPropertyValue(*(Point3*)&TempVector3Value);
		MaterialConfigNode.WriteProperty("AmbientColor", TempVector3Value);

		//DIFFUSE
		zeLog("Writing diffuse data.");
		MaterialConfigNode.WriteProperty("DiffuseEnabled", (MapFlag & ZE_MTMP_DIFFUSEMAP) != 0);
		TempVector3Value = ZEVector3::Zero;
		NodeMaterial->GetDiffuseData()->GetPropertyValue(*(Point3*)&TempVector3Value);
		MaterialConfigNode.WriteProperty("DiffuseColor", TempVector3Value);
		MaterialConfigNode.WriteProperty("BaseMap", DiffuseMap);

		//SPECULAR
		zeLog("Writing specular data.");
		MaterialConfigNode.WriteProperty("SpecularEnabled", (MapFlag & ZE_MTMP_SPECULARMAP) != 0);
		float TempFloatValue = 0.0f;
		NodeMaterial->GetSpecularLevelData()->GetPropertyValue(TempFloatValue);
		MaterialConfigNode.WriteProperty("SpecularFactor", TempFloatValue);

		TempVector3Value = ZEVector3::Zero;
		NodeMaterial->GetSpecularData()->GetPropertyValue(*(Point3*)&TempVector3Value);
		MaterialConfigNode.WriteProperty("SpecularColor", TempVector3Value);
		MaterialConfigNode.WriteProperty("SpecularMap", SpecularMap);

		//EMISSIVE
		zeLog("Writing emissive data.");
		MaterialConfigNode.WriteProperty("EmmisiveEnabled", (MapFlag & ZE_MTMP_EMISSIVEMAP) != 0);
		TempFloatValue = 0.0f;
		NodeMaterial->GetEmissiveAmtData()->GetPropertyValue(TempFloatValue);
		MaterialConfigNode.WriteProperty("EmmisiveFactor", TempFloatValue);

		TempVector3Value = ZEVector3::Zero;
		NodeMaterial->GetEmissiveData()->GetPropertyValue(*(Point3*)&TempVector3Value);
		MaterialConfigNode.WriteProperty("EmmisiveColor", TempVector3Value);
		MaterialConfigNode.WriteProperty("EmmisiveMap", EmissiveMap);

		//NORMAL
		zeLog("Writing normals data.");
		MaterialConfigNode.WriteProperty("NormalMapEnabled", (MapFlag & ZE_MTMP_NORMALMAP) != 0);
		MaterialConfigNode.WriteProperty("NormalMap", NormalMap);

		//OPACITY
		MaterialConfigNode.WriteProperty("OpacityEnabled", (MapFlag & ZE_MTMP_OPACITYMAP) != 0);
		MaterialConfigNode.WriteProperty("Opacity", Opacity);
		MaterialConfigNode.WriteProperty("OpacityMap", OpacityMap);

		//ENVIRONMENT
		zeLog("Writing environment data.");
		MaterialConfigNode.WriteProperty("EnvironmentMap", EnvironmentMap);

		MaterialConfigNode.CloseNode();
		MaterialSerialNode.CloseNode();
		zeLog("Closing material file.");
		MaterialFile.Close();

		ZEMLNode* MaterialDOMNode = MaterialsNode->AddSubNode("Material");
		MaterialDOMNode->AddProperty("Name", MaterialName);
		MaterialDOMNode->AddProperty("FilePath", ResourceConfigurationDialog->GetResourceRelativePath(ZEString(FileName) , MaterialName + ".ZEMATERIAL"));

		ZEProgressDialog::GetInstance()->CloseTask();
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
	zeLog("Portal Count: %d, Portal Doors Count: %d, Entity Count: %d, Elected Node Count: %d", PortalNodeCount, PortalDoorNodeCount, EntityNodeCount, ElectedNodeCount);

	return true;
}

void ZE3dsMapExporter::CollectResources()
{
	ZESize PortalNodeCount = 0;

	Tab<IGameNode*>		Nodes = Scene->GetIGameNodeByType(IGameObject::IGAME_MESH);
	Tab<IGameNode*>		ResourcePortals;
	Tab<IGameMaterial*>	ResourceMaterials;

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
			ResourcePortals.Append(1, &CurrentNode);
		}
	}

	for (ZESize I = 0; I < (ZESize)ResourcePortals.Count(); I++)
	{
		IGameNode* CurrentNode = ResourcePortals[I];
		IGameObject* CurrentObject = CurrentNode->GetIGameObject();

		IGameMesh* Mesh = (IGameMesh*)CurrentObject;
		Mesh->InitializeData();
		Mesh->InitializeBinormalData();
		Mesh->SetUseWeightedNormals();

		for (ZESize I = 0; I < (ZESize)Mesh->GetNumberOfFaces(); I++)
		{
			FaceEx* Face;
			Face = Mesh->GetFace((ZEInt)I);
			IGameMaterial* CurrentMaterial = Mesh->GetMaterialFromFace((ZEInt)I);

			if (CurrentMaterial == NULL)
				zeError("Face %d of portal \"%s\" does not have valid material. Can not collect resource.", I, CurrentNode->GetName());

			bool IsFound = false;

			for(ZESize J = 0; J < ResourceMaterials.Count(); J++)
				if(ResourceMaterials[J] == CurrentMaterial)
					IsFound = true;

			if(!IsFound)
				ResourceMaterials.Append(1, &CurrentMaterial);
		}
	}

	for (ZESize I = 0; I < ResourceMaterials.Count(); I++)
	{
		IGameMaterial* NodeMaterial = ResourceMaterials[I];
		ZEString MaterialName = NodeMaterial->GetMaterialName();

		ResourceConfigurationDialog->AddResource(MaterialName + ".ZEMATERIAL", ZEString(), ZE_ROAA_COPY_OVERWRITE);

		ZEInt NumberOfMaps = NodeMaterial->GetNumberOfTextureMaps();

		for (ZEInt N = 0; N < NumberOfMaps; N++)
		{
			char RelativePath[ZE_MPFL_MAX_FILENAME_SIZE];

			IGameTextureMap* CurrentTexture = NodeMaterial->GetIGameTextureMap(N);
			switch(CurrentTexture->GetStdMapSlot())
			{
			case ID_AM: // Ambient
				break;
			case ID_DI: // Diffuse
				ResourceConfigurationDialog->AddResource(ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()), CurrentTexture->GetBitmapFileName(), ZEString());
				break;
			case ID_SP: // Specular
				ResourceConfigurationDialog->AddResource(ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()), CurrentTexture->GetBitmapFileName(), ZEString());
				break;
			case ID_SI:	// Emissive
				ResourceConfigurationDialog->AddResource(ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()), CurrentTexture->GetBitmapFileName(), ZEString());
				break;
			case ID_OP:	// Opacity 
				ResourceConfigurationDialog->AddResource(ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()), CurrentTexture->GetBitmapFileName(), ZEString());
				break;
			case ID_FI:	// Filter color 
				break;
			case ID_BU: // Bump 
				ResourceConfigurationDialog->AddResource(ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()), CurrentTexture->GetBitmapFileName(), ZEString());
				break;
			case ID_RL: // Reflection - Environment
				ResourceConfigurationDialog->AddResource(ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()), CurrentTexture->GetBitmapFileName(), ZEString());
				break; 
			case ID_RR: // Refraction 
				break;
			}
			
		}
	}
}
