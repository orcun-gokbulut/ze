//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZE3dsMaxModelExporterProcess.cpp
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

#include "ZE3dsMaxModelExporter.h"
#include "ZEToolComponents\ZEProgressDialog\ZEProgressDialog.h"
#include "ZEToolComponents\ZEResourceConfigurationWidget\ZEResourceConfigurationWidget.h"
#include "ZEFile/ZEFile.h"
#include "ZEFile/ZEFileInfo.h"
#include "ZEFile/ZEPathUtils.h"
#include "ZEFile\ZEDirectoryInfo.h"
#include "ZEFile\ZEFileOperations.h"
#include "ZEML/ZEMLSerialWriter.h"

// #include <IGame/IGameFx.h>
// #include <io.h>
// #include "IGame/IGameModifier.h"

enum ZEPhysicalBodyType
{
	ZE_PBT_NONE				= 0,
	ZE_PBT_RIGID			= 1,
	ZE_PBT_DEFORMABLE		= 2,
	ZE_PBT_CLOTH			= 3
};

enum ZEPhysicalShapeType
{
	ZE_PBST_BOX				= 0,
	ZE_PBST_SPHERE			= 1,
	ZE_PBST_CAPSULE			= 2,
	ZE_PBST_CYLINDER		= 3,
	ZE_PBST_CONVEX			= 4
};

enum ZEModelHelperOwnerType
{
	ZE_MHOT_MODEL			= 0,
	ZE_MHOT_MESH			= 1,
	ZE_MHOT_BONE			= 2
};

ZEPackStruct(
struct ZEModelFileSkinnedVertex
{
	ZEVector3									Position;
	ZEVector3									Normal;
	ZEVector3									Tangent;
	ZEVector3									Binormal;
	ZEVector2									Texcoord;
	ZEUInt8										BoneIndices[4];
	float										BoneWeights[4];
});

ZEPackStruct(
struct ZEModelFileVertex
{
	ZEVector3									Position;
	ZEVector3									Normal;
	ZEVector3									Tangent;
	ZEVector3									Binormal;
	ZEVector2									Texcoord;
});

ZEPackStruct(
struct ZEModelFileAnimationKey
{
	ZEUInt32									ItemId;
	ZEVector3									Position;
	ZEQuaternion								Rotation;
	ZEVector3									Scale;
});

ZEInt ZE3dsMaxModelExporter::GetMeshId(IGameNode* Node)
{
	for (ZESize I = 0; I < (ZESize)ProcessedMasterMeshes.Count(); I++)
		if (ProcessedMasterMeshes[I]->GetNodeID() == Node->GetNodeID())
			return (ZEInt)I;
	return -1;
}

ZEInt ZE3dsMaxModelExporter::GetBoneId(IGameNode* Node)
{
	for (ZESize I = 0; I < (ZESize)ProcessedBones.Count(); I++)
		if (ProcessedBones[I]->GetNodeID() == Node->GetNodeID())
			return (ZEInt)I;

	return -1;
}

ZEInt ZE3dsMaxModelExporter::ProcessMeshMaterial(IGameMaterial* Material)
{
	if (Material == NULL)
		return -1;

	for (ZESize I = 0; I < (ZESize)ProcessedMaterials.Count(); I++)
		if (ProcessedMaterials[I] == Material)
			return (ZEInt)I;
	
	ProcessedMaterials.Append(1, &Material);
	return ProcessedMaterials.Count() - 1;
}

bool ZE3dsMaxModelExporter::ProcessMaterials(const char* FileName)
{
	zeLog("Processing materials...");

	char DiffuseMap[ZE_EXFL_MAX_FILENAME_SIZE];
	char SpecularMap[ZE_EXFL_MAX_FILENAME_SIZE];
	char EmissiveMap[ZE_EXFL_MAX_FILENAME_SIZE];
	char OpacityMap[ZE_EXFL_MAX_FILENAME_SIZE];
	char NormalMap[ZE_EXFL_MAX_FILENAME_SIZE];
	char EnvironmentMap[ZE_EXFL_MAX_FILENAME_SIZE];

	ZESize MaterialCount = (ZESize)ProcessedMaterials.Count();

	if (MaterialCount == 0)
	{
		zeWarning("No materials found. Material processing aborted.");
		return true;
	}

	ZEMLNode* MaterialsNode = ModelNode.AddSubNode("Materials");

	zeLog("Material count : %d", MaterialCount);

	for (ZESize I = 0; I < MaterialCount; I++)
	{
		IGameMaterial* NodeMaterial = ProcessedMaterials[I];
		ZEString MaterialName = NodeMaterial->GetMaterialName();

		ZEResourceOption MaterialOption;
		if(!ResourceConfigurationDialog->GetOption(MaterialName + ".ZEMaterial", MaterialOption))
			zeError("Material export path not found in resource options. Resource identifier : %s");

		if(!ResourceConfigurationDialog->GetCopyState(MaterialOption.Identifier))
		{
			ZEMLNode* MaterialNode = MaterialsNode->AddSubNode("Material");
			MaterialNode->AddProperty("Name", MaterialName);
			MaterialNode->AddProperty("FilePath", ResourceConfigurationDialog->GetResourceRelativePath(ZEString(FileName) , MaterialName + ".ZEMATERIAL"));
			continue;
		}

		ZEString MaterialFilePath = MaterialOption.ExportPath + ZEPathUtils::GetSeperator() + MaterialOption.Identifier;

		ZEProgressDialog::GetInstance()->OpenTask(MaterialName);
		zeLog("Processing material \"%s\" (%u/%d).", MaterialName.ToCString(), I + 1, ProcessedMaterials.Count());

		ZeroMemory(DiffuseMap, sizeof(DiffuseMap));
		ZeroMemory(SpecularMap, sizeof(SpecularMap));
		ZeroMemory(EmissiveMap, sizeof(EmissiveMap));
		ZeroMemory(OpacityMap, sizeof(OpacityMap));
		ZeroMemory(NormalMap, sizeof(NormalMap));
		ZeroMemory(EnvironmentMap, sizeof(EnvironmentMap));

		ZEFile MaterialFile;
		if (!MaterialFile.Open(MaterialOption.ExportPath + ZEPathUtils::GetSeperator() + MaterialName + ".ZEMaterial", ZE_FOM_WRITE, ZE_FCM_OVERWRITE))
		{
			zeError("Material process failed. Unable to open file for material: \"%s\"", MaterialName);
			return false;
		}
		zeLog("Material file successfully opened.");

		ZEInt NumberOfMaps = NodeMaterial->GetNumberOfTextureMaps();
		ZEInt32 MapFlag = 0;
		ZEString ResourceRelativePath;

		for (ZEInt N = 0; N < NumberOfMaps; N++)
		{
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
					strncpy(DiffuseMap, ResourceRelativePath.ToCString() , ZE_EXFL_MAX_FILENAME_SIZE);

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
					ResourceRelativePath = ResourceConfigurationDialog->GetResourceRelativePath(MaterialFilePath, MaterialOption.Identifier);
					strncpy(SpecularMap, ResourceRelativePath.ToCString(), ZE_EXFL_MAX_FILENAME_SIZE);

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
					ResourceRelativePath = ResourceConfigurationDialog->GetResourceRelativePath(MaterialFilePath, MaterialOption.Identifier);
					strncpy(EmissiveMap, ResourceRelativePath.ToCString(), ZE_EXFL_MAX_FILENAME_SIZE);

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
					ResourceRelativePath = ResourceConfigurationDialog->GetResourceRelativePath(MaterialFilePath, MaterialOption.Identifier);
					strncpy(OpacityMap, ResourceRelativePath.ToCString(), ZE_EXFL_MAX_FILENAME_SIZE);

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
				/*
				if (!GetRelativePath(CurrentTexture->GetBitmapFileName(), RelativePath))
				{
				zeError("Material texture's relative path does not match with Zinek Engine Resource directory. Please check your Zinek Engine resource directory. Disabling Diffuse Map.\r\n"
				"\tMaterial Name : \"%s\", Texture Slot : Diffuse Map, File path : \"%s\" (Path is not in Zinek Engine resource directory)",
				MaterialName, CurrentTexture->GetBitmapFileName());
				continue;
				}
				CurrentMaterial->ShaderComponents |= ZESHADER_DIFFUSEMAP;
				strncpy(CurrentMaterial->DetailMap, RelativePath, ZE_EXFL_MAX_FILENAME_SIZE);
				*/
				break;
			case ID_BU: // Bump 
				if (!ResourceConfigurationDialog->GetOption(ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()), MaterialOption))
					zeError("Bump texture export path not found in resource options. Resource identifier : %s", ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()).ToCString());

				if(!ZEDirectoryInfo::IsDirectory(MaterialOption.ExportPath))
					zeError("Bump texture export path not valid . Resource identifier : %s", ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()).ToCString());
				else
				{
					MapFlag |= ZE_MTMP_NORMALMAP;
					ResourceRelativePath = ResourceConfigurationDialog->GetResourceRelativePath(MaterialFilePath, MaterialOption.Identifier);
					strncpy(NormalMap, ResourceRelativePath.ToCString(), ZE_EXFL_MAX_FILENAME_SIZE);

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
					ResourceRelativePath = ResourceConfigurationDialog->GetResourceRelativePath(MaterialFilePath, MaterialOption.Identifier);
					strncpy(EnvironmentMap, ResourceRelativePath.ToCString(), ZE_EXFL_MAX_FILENAME_SIZE);

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
				/*if (!GetRelativePath(CurrentTexture->GetBitmapFileName(), RelativePath))
				{
					zeError("Material texture's relative path does not match with Zinek Engine Resource directory. Please check your Zinek Engine resource directory. Disabling Refraction Map.\r\n"
						"\tMaterial Name : \"%s\", Texture Slot : Refraction Map, File path : \"%s\" (Path is not in Zinek Engine resource directory)",
						MaterialName, CurrentTexture->GetBitmapFileName());
					continue;
				}
				MapFlag |= ZE_MTMP_REFRACTION;
				strncpy(CurrentMaterial->RefractionMap, RelativePath, ZE_EXFL_MAX_FILENAME_SIZE);*/
				break;
			}
			
		}

		ZEMLSerialRootNode MaterialSerialNode("Material", &MaterialFile);
		MaterialSerialNode.WriteProperty("Name", MaterialName);
		ZEMLSerialNode MaterialConfigNode = MaterialSerialNode.OpenNode("Configuration");

		MaterialConfigNode.WriteProperty("Name", "Default");
		bool TempBooleanValue = false;
		ZE3dsMaxUtils::GetProperty(NodeMaterial, ZE_INT_PROP, "wire", TempBooleanValue);
		MaterialConfigNode.WriteProperty("Wireframe", TempBooleanValue);

		TempBooleanValue = false;
		ZE3dsMaxUtils::GetProperty(NodeMaterial, ZE_INT_PROP, "twoSided", TempBooleanValue);
		MaterialConfigNode.WriteProperty("TwoSided", TempBooleanValue);

		MaterialConfigNode.WriteProperty("LightningEnabled", true); // Lightning Enabled is forced true;

		float Opacity = 0.0f;
		NodeMaterial->GetOpacityData()->GetPropertyValue(Opacity);

		if (Opacity != 1.0f || strncmp(OpacityMap, "", ZE_EXFL_MAX_FILENAME_SIZE) !=0)
			MaterialConfigNode.WriteProperty("Transparant", true);
		else
			MaterialConfigNode.WriteProperty("Transparant", false);

		//AMBIENT
		MaterialConfigNode.WriteProperty("AmbientEnabled", false); //Ambient is forced false
		ZEVector3 TempVector3Value = ZEVector3::Zero;
		NodeMaterial->GetAmbientData()->GetPropertyValue(*(Point3*)&TempVector3Value);
		MaterialConfigNode.WriteProperty("AmbientColor", TempVector3Value);

		//DIFFUSE
		MaterialConfigNode.WriteProperty("DiffuseEnabled", true /*(MapFlag & ZE_MTMP_DIFFUSEMAP) != 0*/); //Diffuse is forced true
		TempVector3Value = ZEVector3::Zero;
		NodeMaterial->GetDiffuseData()->GetPropertyValue(*(Point3*)&TempVector3Value);
		MaterialConfigNode.WriteProperty("DiffuseColor", TempVector3Value);
		MaterialConfigNode.WriteProperty("BaseMap", DiffuseMap);

		//SPECULAR
		MaterialConfigNode.WriteProperty("SpecularEnabled", true /*(MapFlag & ZE_MTMP_SPECULARMAP) != 0*/); //Specular is forced true
		float TempFloatValue = 0.0f;
		NodeMaterial->GetSpecularLevelData()->GetPropertyValue(TempFloatValue);
		MaterialConfigNode.WriteProperty("SpecularFactor", TempFloatValue);

		TempVector3Value = ZEVector3::Zero;
		NodeMaterial->GetSpecularData()->GetPropertyValue(*(Point3*)&TempVector3Value);
		MaterialConfigNode.WriteProperty("SpecularColor", TempVector3Value);
		MaterialConfigNode.WriteProperty("SpecularMap", SpecularMap);

		//EMISSIVE
		MaterialConfigNode.WriteProperty("EmmisiveEnabled", (MapFlag & ZE_MTMP_EMISSIVEMAP) != 0);
		TempFloatValue = 0.0f;
		NodeMaterial->GetEmissiveAmtData()->GetPropertyValue(TempFloatValue);
		MaterialConfigNode.WriteProperty("EmmisiveFactor", TempFloatValue);

		TempVector3Value = ZEVector3::Zero;
		NodeMaterial->GetEmissiveData()->GetPropertyValue(*(Point3*)&TempVector3Value);
		MaterialConfigNode.WriteProperty("EmmisiveColor", TempVector3Value);
		MaterialConfigNode.WriteProperty("EmmisiveMap", EmissiveMap);

		//NORMAL
		MaterialConfigNode.WriteProperty("NormalMapEnabled", (MapFlag & ZE_MTMP_NORMALMAP) != 0);
		MaterialConfigNode.WriteProperty("NormalMap", NormalMap);

		//OPACITY
		MaterialConfigNode.WriteProperty("OpacityEnabled", (MapFlag & ZE_MTMP_OPACITYMAP) != 0);
		MaterialConfigNode.WriteProperty("Opacity", Opacity);
		MaterialConfigNode.WriteProperty("OpacityMap", OpacityMap);

		//ENVIRONMENT
		MaterialConfigNode.WriteProperty("EnvironmentMap", EnvironmentMap);

		MaterialConfigNode.CloseNode();
		MaterialSerialNode.CloseNode();
		MaterialFile.Close();

		ZEMLNode* MaterialDOMNode = MaterialsNode->AddSubNode("Material");
		MaterialDOMNode->AddProperty("Name", MaterialName);
		MaterialDOMNode->AddProperty("FilePath", ResourceConfigurationDialog->GetResourceRelativePath(ZEString(FileName) , MaterialName + ".ZEMaterial"));

		ZEProgressDialog::GetInstance()->CloseTask();
	}

	return true;
}

void ZE3dsMaxModelExporter::ProcessPhysicalBodyConvexShape(IGameNode* Node, IGameNode* OwnerNode, ZEMLNode* ShapeNode)
{
	IGameMesh* Mesh = (IGameMesh*)Node->GetIGameObject();
	Mesh->InitializeData();

	GMatrix WorldTransform = Node->GetObjectTM() * Node->GetWorldTM().Inverse();
	GMatrix InvWorldTransform = WorldTransform;
	InvWorldTransform[3][0] = 0.0f;
	InvWorldTransform[3][1] = 0.0f;
	InvWorldTransform[3][2] = 0.0f;
	InvWorldTransform[3][3] = 1.0f;
	InvWorldTransform[2][3] = 0.0f;
	InvWorldTransform[1][3] = 0.0f;
	InvWorldTransform[0][3] = 0.0f;

	ZESize NumberofVertices = (ZESize)(Mesh->GetNumberOfVerts());
	ZEArray<ZEVector3> Vertices;
	Vertices.SetCount(NumberofVertices);

	for (ZESize I = 0; I < NumberofVertices; I++)
	{
		Point3 Vertex = Mesh->GetVertex((ZEInt)I, false) * WorldTransform;
		Vertices[I] = ZE3dsMaxUtils::MaxtoZE(Vertex);
	}

	ShapeNode->AddDataProperty("Vertices", Vertices.GetCArray(), sizeof(ZEVector3) * NumberofVertices, true);
}

bool ZE3dsMaxModelExporter::ProcessPhysicalShape(IGameNode* Node, IGameNode* OwnerNode, ZEMLNode* PhysicalShapeNode)
{
	IGameObject* Object = Node->GetIGameObject();

	if (Object == NULL)
		return false;

	const char* ZEType;
	if (!ZE3dsMaxUtils::GetProperty(Object, ZE_STRING_PROP, "ZEType", ZEType) || (strcmp(ZEType, "PhysicalShape") != 0))
	{
		zeError("Mesh is rejected because it does not contain ZEPhysicalShape. Node Name: \"%s\".", Node->GetName());
		return false;
	}

	GMatrix Transform = Node->GetWorldTM() * OwnerNode->GetWorldTM().Inverse();
	PhysicalShapeNode->AddProperty("Position", ZE3dsMaxUtils::MaxtoZE(Transform.Translation()));
	PhysicalShapeNode->AddProperty("Rotation", ZE3dsMaxUtils::MaxtoZE(Transform.Rotation()));

	ZEInt GeometryType;
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"PhysicalShape_MaterialRestitution",		*PhysicalShapeNode->AddProperty("Restitution"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"PhysicalShape_MaterialDynamicFriction",	*PhysicalShapeNode->AddProperty("DynamicFriction"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"PhysicalShape_MaterialStaticFriction",		*PhysicalShapeNode->AddProperty("StaticFriction"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_INT_PROP,		"PhysicalShape_GeometryType",				 GeometryType);

	if (GeometryType == 1)
	{
		char* ObjectClassName = Object->GetClassName();
		if (strcmp(ObjectClassName, "Box") == 0)
		{
			PhysicalShapeNode->AddProperty("Type", (ZEInt)ZE_PBST_BOX);
			ZEMLNode* ShapeNode = PhysicalShapeNode->AddSubNode("Shape");
			ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP, "width",  *ShapeNode->AddProperty("Width"));
			ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP, "height", *ShapeNode->AddProperty("Height"));
			ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP, "length", *ShapeNode->AddProperty("Length"));
		}
		else if (strcmp(ObjectClassName, "Sphere") == 0)
		{
			PhysicalShapeNode->AddProperty("Type", (ZEInt)ZE_PBST_SPHERE);
			ZEMLNode* ShapeNode = PhysicalShapeNode->AddSubNode("Shape");
			ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP, "radius", *ShapeNode->AddProperty("Radius"));
		}
		else if (strcmp(ObjectClassName, "Capsule") == 0)
		{
			PhysicalShapeNode->AddProperty("Type", (ZEInt)ZE_PBST_CAPSULE);
			ZEMLNode* ShapeNode = PhysicalShapeNode->AddSubNode("Shape");
			ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP, "radius", *ShapeNode->AddProperty("Radius"));
			ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP, "height", *ShapeNode->AddProperty("Height"));
		}
		else if (strcmp(ObjectClassName, "Cylinder") == 0)
		{
			PhysicalShapeNode->AddProperty("Type", (ZEInt)ZE_PBST_CYLINDER);
			ZEMLNode* ShapeNode = PhysicalShapeNode->AddSubNode("Shape");
			ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP, "radius", *ShapeNode->AddProperty("Radius"));
			ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP, "height", *ShapeNode->AddProperty("Height"));
		}
		else if (strcmp(ObjectClassName, "Editable Poly") == 0 || strcmp(ObjectClassName, "Editable Mesh") == 0 || strcmp(ObjectClassName, "Editable Patch") == 0)
		{
			PhysicalShapeNode->AddProperty("Type", (ZEInt)ZE_PBST_CONVEX);
			ProcessPhysicalBodyConvexShape(Node, OwnerNode, PhysicalShapeNode->AddSubNode("Shape"));
		}
		else
		{
			zeError("Shape type does not supported. Type : \"%s\", Node Name : \"%s\"", ObjectClassName, Node->GetName());
			return false;
		}
	}
	else if (GeometryType == 2) // Convex
	{
		PhysicalShapeNode->AddProperty("Type", (ZEInt)ZE_PBST_CONVEX);
		ProcessPhysicalBodyConvexShape(Node, OwnerNode, PhysicalShapeNode->AddSubNode("Shape"));
	}

	return true;
}

void ZE3dsMaxModelExporter::ProcessPhysicalBody(IGameNode* Node, ZEMLNode* ParentNode)
{
	IGameObject* Object = Node->GetIGameObject();

	if (Object == NULL)
		return;

	ZEInt PhysicalBodyTypeValue;
	ZE3dsMaxUtils::GetProperty(Object, ZE_INT_PROP, "PhysicalBody_Type", PhysicalBodyTypeValue);
	PhysicalBodyTypeValue--; //This action is made because Max Script Array index starts from 1 instead of 0

	ZEPhysicalBodyType PhysicalBodyType = (ZEPhysicalBodyType)PhysicalBodyTypeValue;

	if (PhysicalBodyType == ZE_PBT_NONE)
	{
		zeWarning("No physical body found. Processing physical body aborted. Node Name : \"%s\"", Node->GetName());
		return;
	}
	else if (PhysicalBodyType == ZE_PBT_RIGID)
	{
		zeLog("Processing physical body of \"%s\"", Node->GetName());

		ZEMLNode* PhysicalBodyNode = ParentNode->AddSubNode("PhysicalBody");

		ZE3dsMaxUtils::GetProperty(Object, ZE_BOOL_PROP, "PhysicalBody_Enabled", *PhysicalBodyNode->AddProperty("Enabled"));

		ZEInt PhysicalBodyTypeValue;
		ZE3dsMaxUtils::GetProperty(Object, ZE_INT_PROP, "PhysicalBody_Type", PhysicalBodyTypeValue);
		PhysicalBodyTypeValue--; // Array index start problem with Max Script and C++)
		PhysicalBodyNode->AddProperty("Type", PhysicalBodyTypeValue);

		ZE3dsMaxUtils::GetProperty(Object, ZE_BOOL_PROP, "PhysicalBody_IsKinematic", *PhysicalBodyNode->AddProperty("IsKinematic"));

		ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP, "PhysicalBody_Mass", *PhysicalBodyNode->AddProperty("Mass"));

		IGameNode* MassCenter = NULL;
		ZE3dsMaxUtils::GetProperty(Object, "PhysicalBody_CenterOfMass", Scene, MassCenter);

		if (MassCenter == NULL)
			PhysicalBodyNode->AddProperty("MassCenter", ZEVector3::Zero);
		else
			PhysicalBodyNode->AddProperty("MassCenter", ZE3dsMaxUtils::MaxtoZE(MassCenter->GetObjectTM().Translation() - Node->GetObjectTM().Translation()));

		ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP, "PhysicalBody_LinearDamping", *PhysicalBodyNode->AddProperty("LinearDamping"));
		ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP, "PhysicalBody_AngularDamping", *PhysicalBodyNode->AddProperty("AngularDamping"));

		IGameProperty* ShapesProp = Object->GetIPropertyContainer()->QueryProperty("PhysicalBody_Shapes");
		if (ShapesProp == NULL)
		{
			zeError("Missing property \"PhysicalShapes\" in ZEAttribute. Node Name : \"%s\"", Node->GetName());
			return;
		}

		ZEMLNode* PhysicalShapesNode = PhysicalBodyNode->AddSubNode("PhysicalShapes");
		IParamBlock2* ParamBlock = ShapesProp->GetMaxParamBlock2();
		ZEInt ParamId = ParamBlock->IndextoID(ShapesProp->GetParamBlockIndex());

		for (ZESize I = 0; I < (ZESize)ParamBlock->Count(ParamId); I++)
		{
			const char* Type;
			IGameNode* PhysicalShapeNode = Scene->GetIGameNode(ParamBlock->GetINode(ParamId, 0, (ZEInt)I));
			if (PhysicalShapeNode == NULL || !ZE3dsMaxUtils::GetProperty(PhysicalShapeNode->GetIGameObject(), ZE_STRING_PROP, "ZEType", Type) || strcmp(Type, "PhysicalShape") != 0)
			{
				zeError("Physical body shape is not a valid ZEPhysicalShapeAttribute. Array Index : %d, Shape Name : \"%s\".", I, (PhysicalShapeNode != NULL ? PhysicalShapeNode->GetName() : "NULL"));
				return;
			}

			ProcessPhysicalShape(PhysicalShapeNode, Node, PhysicalShapesNode->AddSubNode("PhysicalShape"));
		}

		zeLog("Physical body is processed successfully.");

		return;
	}
	else
	{
		zeWarning("Physical body type not supported. Node Name \"%s\"", ParentNode->GetName());
		return;
	}
}

bool ZE3dsMaxModelExporter::ProcessPhysicalJoint(IGameNode* Node, ZEMLNode* PhysicalJointNode)
{
	IGameObject* Object = Node->GetIGameObject();

	if (Object == NULL)
		return false;

	zeLog("Processing physical joint of \"%s\"", Node->GetName());

	ZE3dsMaxUtils::GetProperty(Object, ZE_BOOL_PROP, "Joint_Enabled", *PhysicalJointNode->AddProperty("Enabled"));

	IGameNode* Body1 = NULL;
	ZE3dsMaxUtils::GetProperty(Object, "Joint_Body1", Scene, Body1); 								
	if (Body1 != NULL)
		PhysicalJointNode->AddProperty("Body1Id", GetMeshId(Body1));
	else
		PhysicalJointNode->AddProperty("Body1Id", -1);

	IGameNode* Body2 = NULL;
	ZE3dsMaxUtils::GetProperty(Object, "Joint_Body2", Scene, Body2);
	if (Body2 != NULL)
		PhysicalJointNode->AddProperty("Body2Id", GetMeshId(Body2));
	else
		PhysicalJointNode->AddProperty("Body2Id", -1);

	ZEMLProperty* GlobalAnchorAxisProperty = PhysicalJointNode->AddProperty("UseGlobalAnchorAxis");
	ZE3dsMaxUtils::GetProperty(Object, ZE_BOOL_PROP, "Joint_UseGlobalAnchorAxis", *GlobalAnchorAxisProperty);
	if (!GlobalAnchorAxisProperty->GetValue().GetBoolean())
	{
		IGameNode* BodyAnchor1 = NULL;
		ZE3dsMaxUtils::GetProperty(Object, "Joint_Body1_Anchor", Scene, BodyAnchor1);
		if (BodyAnchor1 != NULL)
		{
			GMatrix LocalTransform =  BodyAnchor1->GetObjectTM() * Node->GetObjectTM().Inverse();
			PhysicalJointNode->AddProperty("LocalAnchor1", ZE3dsMaxUtils::MaxtoZE(LocalTransform.Translation()));
			PhysicalJointNode->AddProperty("LocalAxis1", ZE3dsMaxUtils::MaxtoZE(LocalTransform.Rotation()));
		}

		IGameNode* BodyAnchor2 = NULL;
		ZE3dsMaxUtils::GetProperty(Object, "Joint_Body2_Anchor", Scene, BodyAnchor2);
		if (BodyAnchor2 != NULL)
		{
			GMatrix LocalTransform = BodyAnchor2->GetObjectTM() * Node->GetObjectTM().Inverse();
			PhysicalJointNode->AddProperty("LocalAnchor2", ZE3dsMaxUtils::MaxtoZE(LocalTransform.Translation()));
			PhysicalJointNode->AddProperty("LocalAxis2", ZE3dsMaxUtils::MaxtoZE(LocalTransform.Rotation()));
		}
	}
	else
	{
		PhysicalJointNode->AddProperty("GlobalAnchor", ZE3dsMaxUtils::MaxtoZE(Node->GetObjectTM().Translation()));
		PhysicalJointNode->AddProperty("GlobalAxis", ZE3dsMaxUtils::MaxtoZE(Node->GetObjectTM().Rotation()));
	}

	ZEInt JointTypeValue = 0;
	ZE3dsMaxUtils::GetProperty(Object, ZE_INT_PROP,	"Joint_Type", JointTypeValue);
	JointTypeValue--;
	PhysicalJointNode->AddProperty("JointType", JointTypeValue);

	ZE3dsMaxUtils::GetProperty(Object, ZE_BOOL_PROP,		"Joint_CollideBodies",				*PhysicalJointNode->AddProperty("CollideBodies"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_BOOL_PROP,		"Joint_Breakable",					*PhysicalJointNode->AddProperty("Breakable"));	
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_Break_Force",				*PhysicalJointNode->AddProperty("BreakForce"));					
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_Break_Torque",				*PhysicalJointNode->AddProperty("BreakTorque"));

	//Added : Free Joint Properties:

	ZE3dsMaxUtils::GetProperty(Object, ZE_INT_PROP,		"Joint_XMotion", 					*PhysicalJointNode->AddProperty("XMotion"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_INT_PROP,		"Joint_YMotion", 					*PhysicalJointNode->AddProperty("YMotion"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_INT_PROP,		"Joint_ZMotion", 					*PhysicalJointNode->AddProperty("ZMotion"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_LinearLimitValue", 			*PhysicalJointNode->AddProperty("LinearLimitValue"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_LinearLimitRestitution", 	*PhysicalJointNode->AddProperty("LinearLimitRestitution"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_LinearLimitSpring", 			*PhysicalJointNode->AddProperty("LinearLimitSpring"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_LinearLimitDamping", 		*PhysicalJointNode->AddProperty("LinearLimitDamping"));

	ZE3dsMaxUtils::GetProperty(Object, ZE_INT_PROP,		"Joint_TwistMotion", 				*PhysicalJointNode->AddProperty("TwistMotion"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_TwistLowLimitValue", 		*PhysicalJointNode->AddProperty("TwistLowLimitValue"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_TwistLowLimitRestitution", 	*PhysicalJointNode->AddProperty("TwistLowLimitRestitution"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_TwistLowLimitSpring", 		*PhysicalJointNode->AddProperty("TwistLowLimitSpring"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_TwistLowLimitDamping", 		*PhysicalJointNode->AddProperty("TwistLowLimitDamping"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_TwistHighLimitValue", 		*PhysicalJointNode->AddProperty("TwistHighLimitValue"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_TwistHighLimitRestitution",	*PhysicalJointNode->AddProperty("TwistHighLimitRestitution"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_TwistHighLimitSpring", 		*PhysicalJointNode->AddProperty("TwistHighLimitSpring"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_TwistHighLimitDamping", 		*PhysicalJointNode->AddProperty("TwistHighLimitDamping"));

	ZE3dsMaxUtils::GetProperty(Object, ZE_INT_PROP,		"Joint_Swing1Motion", 				*PhysicalJointNode->AddProperty("Swing1Motion"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_Swing1LimitValue", 			*PhysicalJointNode->AddProperty("Swing1LimitValue"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_Swing1LimitRestitution", 	*PhysicalJointNode->AddProperty("Swing1LimitRestitution"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_Swing1LimitSpring", 			*PhysicalJointNode->AddProperty("Swing1LimitSpring"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_Swing1LimitDamping", 		*PhysicalJointNode->AddProperty("Swing1LimitDamping"));

	ZE3dsMaxUtils::GetProperty(Object, ZE_INT_PROP,		"Joint_Swing2Motion", 				*PhysicalJointNode->AddProperty("Swing2Motion"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_Swing2LimitValue", 			*PhysicalJointNode->AddProperty("Swing2LimitValue"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_Swing2LimitRestitution", 	*PhysicalJointNode->AddProperty("Swing2LimitRestitution"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_Swing2LimitSpring", 			*PhysicalJointNode->AddProperty("Swing2LimitSpring"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_Swing2LimitDamping", 		*PhysicalJointNode->AddProperty("Swing2LimitDamping"));

	ZE3dsMaxUtils::GetProperty(Object, ZE_INT_PROP,		"Joint_XLinearMotor", 				*PhysicalJointNode->AddProperty("LinearXMotor"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_XLinearMotorForce", 			*PhysicalJointNode->AddProperty("LinearXMotorForce"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_XLinearMotorSpring", 		*PhysicalJointNode->AddProperty("LinearXMotorSpring"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_XLinearMotorDamper", 		*PhysicalJointNode->AddProperty("LinearXMotorDamper"));

	ZE3dsMaxUtils::GetProperty(Object, ZE_INT_PROP,		"Joint_YLinearMotor", 				*PhysicalJointNode->AddProperty("LinearYMotor"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_YLinearMotorForce", 			*PhysicalJointNode->AddProperty("LinearYMotorForce"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_YLinearMotorSpring", 		*PhysicalJointNode->AddProperty("LinearYMotorSpring"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_YLinearMotorDamper", 		*PhysicalJointNode->AddProperty("LinearYMotorDamper"));

	ZE3dsMaxUtils::GetProperty(Object, ZE_INT_PROP,		"Joint_ZLinearMotor", 				*PhysicalJointNode->AddProperty("LinearZMotor"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_ZLinearMotorForce", 			*PhysicalJointNode->AddProperty("LinearZMotorForce"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_ZLinearMotorSpring", 		*PhysicalJointNode->AddProperty("LinearZMotorSpring"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_ZLinearMotorDamper", 		*PhysicalJointNode->AddProperty("LinearZMotorDamper"));

	ZE3dsMaxUtils::GetProperty(Object, ZE_INT_PROP,		"Joint_AngularSwingMotor", 			*PhysicalJointNode->AddProperty("AngularSwingMotor"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_AngularSwingMotorForce", 	*PhysicalJointNode->AddProperty("AngularSwingMotorForce"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_AngularSwingMotorSpring", 	*PhysicalJointNode->AddProperty("AngularSwingMotorSpring"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_AngularSwingMotorDamper", 	*PhysicalJointNode->AddProperty("AngularSwingMotorDamper"));

	ZE3dsMaxUtils::GetProperty(Object, ZE_INT_PROP,		"Joint_AngularTwistMotor", 			*PhysicalJointNode->AddProperty("AngularTwistMotor"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_AngularTwistMotorForce", 	*PhysicalJointNode->AddProperty("AngularTwistMotorForce"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_AngularTwistMotorSpring",	*PhysicalJointNode->AddProperty("AngularTwistMotorSpring"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_AngularTwistMotorDamper",	*PhysicalJointNode->AddProperty("AngularTwistMotorDamper"));

	ZE3dsMaxUtils::GetProperty(Object, ZE_INT_PROP,		"Joint_AngularSlerpMotor", 			*PhysicalJointNode->AddProperty("AngularSlerpMotor"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_AngularSlerpMotorForce", 	*PhysicalJointNode->AddProperty("AngularSlerpMotorForce"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_AngularSlerpMotorSpring", 	*PhysicalJointNode->AddProperty("AngularSlerpMotorSpring"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	"Joint_AngularSlerpMotorDamper", 	*PhysicalJointNode->AddProperty("AngularSlerpMotorDamper"));

	Quat Quaternion;
	Point3 Point;

	IGameNode* MotorTargetPosition = NULL;
	ZE3dsMaxUtils::GetProperty(Object, "Joint_MotorTargetPosition", Scene, MotorTargetPosition);
	if (MotorTargetPosition != NULL)
	{
		PhysicalJointNode->AddProperty("MotorTargetPosition", ZE3dsMaxUtils::MaxtoZE(MotorTargetPosition->GetObjectTM().Translation() - Node->GetObjectTM().Translation()));
	}
	else
		PhysicalJointNode->AddProperty("MotorTargetPosition", ZEVector3::Zero);

	ZE3dsMaxUtils::GetProperty(Object, ZE_VECTOR3_PROP, "Joint_MotorTargetVelocity", *PhysicalJointNode->AddProperty("MotorTargetVelocity"));

	IGameNode* MotorTargetOrientation = NULL;
	ZE3dsMaxUtils::GetProperty(Object, "Joint_MotorTargetOrientation", Scene, MotorTargetOrientation);
	if (MotorTargetOrientation != NULL)
	{
		GMatrix Orientation = MotorTargetPosition->GetObjectTM() * Node->GetObjectTM().Inverse();
		PhysicalJointNode->AddProperty("MotorTargetOrientation", ZE3dsMaxUtils::MaxtoZE(Orientation.Rotation())); //Investigate why there's error
		// ERROR ERROR
		//Joint->Joint.LinearMotorPosition = ZE3dsMaxUtils::MaxtoZE(Orientation.Rotation());
	}
	else
		PhysicalJointNode->AddProperty("MotorTargetOrientation", ZEQuaternion::Identity);

	ZE3dsMaxUtils::GetProperty(Object, ZE_VECTOR3_PROP, "Joint_MotorTargetAngularVelocity", *PhysicalJointNode->AddProperty("MotorTargetAngularVelocity"));

	zeLog("Physical joint is processed successfully.");

	return true;
}

bool ZE3dsMaxModelExporter::ProcessBone(IGameNode* Node, ZEMLNode* BonesNode)
{
	ZEProgressDialog::GetInstance()->OpenTask(Node->GetName());
	zeLog("Processing bone \"%s\".", Node->GetName());
	ZEMLNode* BoneNode = BonesNode->AddSubNode("Bone");

	IGameObject* Bone = Node->GetIGameObject();
	BoneNode->AddProperty("Name", Node->GetName());

	Point3 BoneScale = Node->GetLocalTM().Scaling();
	if (Node->GetWorldTM().Parity() == -1)
	{
		zeError("The bone is mirrored. Bone Name : \"%s\". Mirror transformation operation is not supported. Use bone mirror operation instead.", Node->GetName());
		return false;
	}

	if (ZEMath::Abs(BoneScale.x - 1.0f) > 0.001 || ZEMath::Abs(BoneScale.y - 1.0f) > 0.001 || ZEMath::Abs(BoneScale.z - 1.0f) > 0.001)
		zeWarning("The bone has been scaled. It can cause problems.");

	// Process Bone

	ZEInt32 ParentBoneId = -1;
	if (Node->GetNodeParent() != NULL)
	{
		IGameNode* CurrentNode = Node;
		while(CurrentNode->GetNodeParent() != NULL)
			if (GetBoneId(CurrentNode->GetNodeParent()) == -1)
			{
				CurrentNode = CurrentNode->GetNodeParent();
				continue;
			}
			else
			{
				ParentBoneId = GetBoneId(CurrentNode->GetNodeParent());
				break;
			}
	}

	BoneNode->AddProperty("ParentBone", ParentBoneId);

	Box3 BB;
	Bone->GetBoundingBox(BB);
	ZEMLNode* BoundingBoxNode = BoneNode->AddSubNode("BoundingBox");
	BoundingBoxNode->AddProperty("Min", ZE3dsMaxUtils::MaxtoZE(BB.Min()));
	BoundingBoxNode->AddProperty("Max", ZE3dsMaxUtils::MaxtoZE(BB.Max()));

	if (ParentBoneId != -1)
	{
		GMatrix Transform = Node->GetWorldTM() * ProcessedBones[(ZESize)ParentBoneId]->GetWorldTM().Inverse();
		BoneNode->AddProperty("RelativePosition", ZE3dsMaxUtils::MaxtoZE(Transform.Translation() * Node->GetWorldTM().Scaling()));
		BoneNode->AddProperty("RelativeRotation", ZE3dsMaxUtils::MaxtoZE(Transform.Rotation()));
		BoneNode->AddProperty("RelativeScale", ZE3dsMaxUtils::MaxtoZE(Transform.Scaling()));
	}
	else
	{ 
		zeLog("\"%s\" is the Root Bone", Node->GetName());
		BoneNode->AddProperty("RelativePosition", ZE3dsMaxUtils::MaxtoZE(Node->GetWorldTM().Translation() * Node->GetWorldTM().Scaling()));
		BoneNode->AddProperty("RelativeRotation", ZE3dsMaxUtils::MaxtoZE(Node->GetWorldTM().Rotation()));
		BoneNode->AddProperty("RelativeScale", ZE3dsMaxUtils::MaxtoZE(Node->GetWorldTM().Scaling()));
	}

	// Process Physical Properties
	ProcessPhysicalJoint(Node, BoneNode->AddSubNode("PhysicalJoint"));

	bool IsBonePhysicalBodyExportEnabled = ((ZEMLProperty*)(ExportOptions->GetProperty("IsBonePhysicalBodyExportEnabled")))->GetValue().GetBoolean();

	if (IsBonePhysicalBodyExportEnabled)
		ProcessPhysicalBody(Node, BoneNode);

	zeLog("Bone \"%s\" is processed.", Node->GetName());
	ZEProgressDialog::GetInstance()->CloseTask();

	return true;
}

bool ZE3dsMaxModelExporter::ProcessBones()
{
	zeLog("Processing bones...");

	Tab<IGameNode*> Nodes = Scene->GetIGameNodeByType(IGameObject::IGAME_BONE);
	const char* Type;

	for (ZESize I = 0; I < (ZESize)Nodes.Count(); I++)
	{
		Type = NULL;
		if (ZE3dsMaxUtils::GetProperty(Nodes[I]->GetIGameObject(), ZE_STRING_PROP, "ZEType", Type) && strcmp(Type, "Bone") == 0)
		{
			bool Found = false;
			for (ZESize N = 0; N < (ZESize)ProcessedBones.Count(); N++)
			{
				if (Nodes[I]->GetNodeID() == ProcessedBones[N]->GetNodeID())
				{
					Found = true;
					break;
				}			
			}
			if (Found == false)
				ProcessedBones.Append(1, &Nodes[I]);
		}
	}

	if (ProcessedBones.Count() == 0)
	{
		zeWarning("No bones with ZEBoneAttribute found. Bone processing aborted.");
		return true;
	}

	ZEMLNode* BonesNode = ModelNode.AddSubNode("Bones");

	for (ZESize I = 0; I < (ZESize)ProcessedBones.Count(); I++)
		if (!ProcessBone(ProcessedBones[I], BonesNode))
			return false;

	return true;
}

bool ZE3dsMaxModelExporter::ProcessMeshLODVertices(IGameNode* Node, ZEMLNode* LODNode)
{
	IGameMesh* Mesh = (IGameMesh*)Node->GetIGameObject();

	if(!Mesh->InitializeData())
		zeError("Can not initialize mesh data for mesh.");

	IGameSkin* Skin = Mesh->GetIGameSkin();

	ZEArray<ZEModelFileVertex> Vertices;
	ZEArray<ZEModelFileSkinnedVertex> SkinnedVertices;
	ZEArray<ZEUInt32> AffectingBoneIds;

	LODNode->AddProperty("MaterialId", ProcessMeshMaterial(Node->GetNodeMaterial()));

	if(Mesh->GetNumberOfFaces() == 0)
		zeError("Mesh face count is : 0.");

	if (!Mesh->IsObjectSkinned())
		Vertices.SetCount((ZESize)Mesh->GetNumberOfFaces() * 3);
	else
		SkinnedVertices.SetCount((ZESize)Mesh->GetNumberOfFaces() * 3);

	zeLog("Processing vertices of mesh \"%s\". Polygon Count : %d, Vertex Count : %d.", Node->GetName(), Mesh->GetNumberOfFaces(), Mesh->GetNumberOfFaces() * 3);

	if (!Mesh->InitializeBinormalData())
		zeError("Can not initialize mesh binormal data.");

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

	ZEMatrix4x4 WorldTransform = WorldTM.Inverse() * ObjectTM;

	bool GotError = false;
	bool BoneCountWarning = false; 
	AffectingBoneIds.SetCount(0);
	for (ZESize I = 0; I < (ZESize)Mesh->GetNumberOfFaces(); I++)
	{
		FaceEx* Face;
		Face = Mesh->GetFace((ZEInt)I);
		if (!Mesh->IsObjectSkinned())
		{
			for (ZESize N = 0; N < 3; N++)
			{	
				ZEModelFileVertex* Vertex = &(Vertices[3*I + N]);

				Point3 Temp;

				if (!Mesh->GetVertex(Face->vert[N], Temp, true))
					zeError("Can not get vertex of face %d, vertex index : %d.", I, N);

				Vertex->Position = WorldTransform * ZE3dsMaxUtils::MaxtoZE(Temp);

				if (!Mesh->GetNormal(Face->norm[N], Temp, true))
					zeError("Can not get normal of face %d, normal index : %d.", I, N);
				
				ZEMatrix4x4::Transform3x3(Vertex->Normal, WorldTransform, ZE3dsMaxUtils::MaxtoZE(Temp));

				ZEInt BinormalTangentIndex = Mesh->GetFaceVertexTangentBinormal((ZEInt)I, (ZEInt)N);

				if (!Mesh->GetTangent(BinormalTangentIndex, Temp))
					zeError("Can not get tangent of face %d, tangent index : %d.", I, BinormalTangentIndex);

				ZEMatrix4x4::Transform3x3(Vertex->Tangent, WorldTransform, ZE3dsMaxUtils::MaxtoZE(Temp.Normalize()));

				if (!Mesh->GetBinormal(BinormalTangentIndex, Temp))
					zeError("Can not get binormal of face %d, binormal index : %d.", I, BinormalTangentIndex);

				ZEMatrix4x4::Transform3x3(Vertex->Binormal, WorldTransform, ZE3dsMaxUtils::MaxtoZE(Temp));

				if (!Mesh->GetTexVertex(Face->texCoord[N], *(Point2*)&Vertex->Texcoord))
					zeError("Can not get texture coordinate of face %d vertex %d.", I, N, BinormalTangentIndex);

				Vertex++;
			}
		}
		else
		{
			for (ZESize N = 0; N < 3; N++)
			{
				ZEModelFileSkinnedVertex* Vertex = &(SkinnedVertices[3*I + N]);

				Point3 Temp;

				if (!Mesh->GetVertex(Face->vert[N], Temp, true))
					zeError("Can not get vertex of face %d, vertex index : %d.", I, N);

				Vertex->Position = WorldTransform * ZE3dsMaxUtils::MaxtoZE(Temp);

				if (!Mesh->GetNormal(Face->norm[N], Temp, true))
					zeError("Can not get normal of face %d, normal index : %d.", I, N);

				ZEMatrix4x4::Transform3x3(Vertex->Normal, WorldTransform, ZE3dsMaxUtils::MaxtoZE(Temp));

				ZEInt BinormalTangentIndex = Mesh->GetFaceVertexTangentBinormal((ZEInt)I, (ZEInt)N);

				if (!Mesh->GetTangent(BinormalTangentIndex, Temp))
					zeError("Can not get tangent of face %d, tangent index : %d.", I, BinormalTangentIndex);

				ZEMatrix4x4::Transform3x3(Vertex->Tangent, WorldTransform, ZE3dsMaxUtils::MaxtoZE(Temp.Normalize()));

				if (!Mesh->GetBinormal(BinormalTangentIndex, Temp))
					zeError("Can not get texture coordinate of face %d vertex %d.", I, N, BinormalTangentIndex);

				ZEMatrix4x4::Transform3x3(Vertex->Binormal, WorldTransform, ZE3dsMaxUtils::MaxtoZE(Temp));

				if (!Mesh->GetTexVertex(Face->texCoord[N], *(Point2*)&Vertex->Texcoord))
					zeError("Can not get texture coordinate of face %d vertex %d.", I, N, BinormalTangentIndex);


				ZEInt BoneCount = Skin->GetNumberOfBones(Face->vert[N]);
				if (BoneCount > 4 && !BoneCountWarning)
				{
					zeWarning("Vertex can be affected maximum 4 bones. Exporter is goint to try reducing affecting bones by removing bones with small weights.\r\n"
						"(Treshold: Weights < 0.001) Node Name : \"%s\".", Node->GetName()); 
					BoneCountWarning = true;
				}
				ZeroMemory(Vertex->BoneIndices, sizeof(unsigned char) * 4);
				ZeroMemory(Vertex->BoneWeights, sizeof(float) * 4);

				ZESize BoneIndex = 0;
				for (ZESize M = 0; M < (ZESize)BoneCount; M++)
				{
					ZEInt BoneId = GetBoneId(Skin->GetIGameBone((ZEInt)Face->vert[N], (ZEInt)M));

					if (BoneId == -1)
					{
						zeError("Bone that is used for skinning does not have ZEBone attributes. Node Name : \"%s\". 3ds Max Vertex Id : %d. Bone Name : %s.", Node->GetName(), Face->vert[N],  Skin->GetIGameBone((ZEInt)Face->vert[N], (ZEInt)M)->GetName());
						GotError = true;
						continue;
					}

					ZESSize AffectingBoneId = -1;
					for (ZESize J = 0; J < AffectingBoneIds.GetCount(); J++)
					{
						if (AffectingBoneIds[J] == BoneId)
						{
							AffectingBoneId = J;
							break;
						}
					}

					if (AffectingBoneId == -1)
					{
						AffectingBoneIds.Add(BoneId);
						AffectingBoneId = AffectingBoneIds.GetCount() - 1;
						if (AffectingBoneIds.GetCount() >= 58)
							zeWarning("Affecting bone count exceeds 57. Hardware accelerated skinning may not be posible. Node Name : \"%s\", Bone Count : %d.", Node->GetName(), AffectingBoneIds.GetCount());
					}

					float BoneWeight = Skin->GetWeight(Face->vert[N], (ZEInt)M);
					if (BoneWeight > 0.001f)
					{
						if (BoneIndex >= 4)
						{
							zeError("Can not reduce bone count. Node Name : \"%s\". 3ds Max Vertex Id : %d.", Node->GetName(), Face->vert[N]);
							GotError = true;
							continue;
						}
						Vertex->BoneIndices[BoneIndex] = AffectingBoneId;
						Vertex->BoneWeights[BoneIndex] = BoneWeight;
						assert(BoneId == AffectingBoneIds[AffectingBoneId]);
						BoneIndex++;
					}
				}
				Vertex++;
			}
		}
	}

	if (Mesh->IsObjectSkinned())
	{
		LODNode->AddDataProperty("Vertices", SkinnedVertices.GetCArray(), SkinnedVertices.GetCount() * sizeof(ZEModelFileSkinnedVertex), true);
		LODNode->AddDataProperty("AffectingBoneIds", AffectingBoneIds.GetCArray(), AffectingBoneIds.GetCount() * sizeof(ZEUInt32), true);
	}
	else
		LODNode->AddDataProperty("Vertices", Vertices.GetCArray(), Vertices.GetCount() * sizeof(ZEModelFileVertex), true);

	return !GotError;
}

bool ZE3dsMaxModelExporter::ProcessMasterMesh(IGameNode* Node, ZEMLNode* MeshesNode)
{
	zeLog("Processing mesh \"%s\".", Node->GetName());
	IGameMesh* Mesh = (IGameMesh*)Node->GetIGameObject();

	// Process Attributes
	const char* ZEType;


	if (!ZE3dsMaxUtils::GetProperty(Mesh, ZE_STRING_PROP, "ZEType", ZEType) || strcmp(ZEType, "Mesh") != 0)
	{
		zeError("Mesh is rejected because it does not contain ZEMeshAttributes. Node Name : \"%s\"", Node->GetName());
		return false;
	}

	ZEMLNode* CurrentMeshNode = MeshesNode->AddSubNode("Mesh");
	CurrentMeshNode->AddProperty("Name", Node->GetName());
	zeLog("Mesh \"%s\" is created as base mesh.", Node->GetName());

	zeLog("Mesh is a master LOD. Setting mesh properties by using this mesh.");
	ProcessedMasterMeshes.Append(1, &Node);
	
	if (!Mesh->InitializeData())
		zeError("Can not initialize mesh data.");


	ZEAABBox BoundingBox;
	ZE3dsMaxUtils::CalculateLocalBoundingBox(BoundingBox, Mesh);
	ZEMLNode* BBoxNode = CurrentMeshNode->AddSubNode("BoundingBox");
	BBoxNode->AddProperty("Min", BoundingBox.Min);
	BBoxNode->AddProperty("Max", BoundingBox.Max);

	ZE3dsMaxUtils::GetProperty(Mesh,  ZE_BOOL_PROP, "Mesh_Visibility", *CurrentMeshNode->AddProperty("IsVisible"));

	CurrentMeshNode->AddProperty("IsSkinned", Mesh->IsObjectSkinned());
	CurrentMeshNode->AddProperty("Position", ZE3dsMaxUtils::MaxtoZE(Node->GetWorldTM().Translation()));
	CurrentMeshNode->AddProperty("Rotation", ZE3dsMaxUtils::MaxtoZE(Node->GetWorldTM().Rotation()));
	CurrentMeshNode->AddProperty("Scale", ZE3dsMaxUtils::MaxtoZE(Node->GetWorldTM().Scaling()));

	ZEMLNode* MainLODsNode = CurrentMeshNode->AddSubNode("LODs");
	ZEMLNode* LODNode = MainLODsNode->AddSubNode("LOD");

	ZEInt MeshLOD;
	ZE3dsMaxUtils::GetProperty(Mesh, ZE_INT_PROP, "Mesh_LOD", MeshLOD);

	LODNode->AddProperty("LODLevel", MeshLOD);

	if (!ProcessMeshLODVertices(Node, LODNode))
		return false;

	bool IsMeshPhysicalBodyExportEnabled = ((ZEMLProperty*)(ExportOptions->GetProperty("IsMeshPhysicalBodyExportEnabled")))->GetValue().GetBoolean();

	if (IsMeshPhysicalBodyExportEnabled)
		ProcessPhysicalBody(Node, CurrentMeshNode);

	ProcessedMeshes.Append(1, &Node);

	zeLog("Mesh \"%s\" is processed.", Node->GetName());
	return true;
}

bool ZE3dsMaxModelExporter::ProcessMeshLODs(IGameNode* Node, ZEMLNode* MeshesNode)
{
	IGameMesh* Mesh = (IGameMesh*)Node->GetIGameObject();
	const char* LODName = Node->GetName();

	const char* ZEType;

	if (!ZE3dsMaxUtils::GetProperty(Mesh, ZE_STRING_PROP, "ZEType", ZEType) || strcmp(ZEType, "Mesh") != 0)
	{
		zeError("Mesh is rejected because it does not contain ZEMeshAttributes. Node Name : \"%s\"", Node->GetName());
		return false;
	}
	
	ZEInt CurrentMeshLODLevel;
	ZE3dsMaxUtils::GetProperty(Mesh, ZE_INT_PROP, "Mesh_LOD", CurrentMeshLODLevel);

	ZEArray<ZEMLNode*> Meshes = MeshesNode->GetSubNodes("Mesh");
	ZEMLNode* MasterMesh = NULL;

	for (ZESize I = 0; I < Meshes.GetCount(); I++)
	{
		if (((ZEMLProperty*)Meshes[I]->GetProperty("Name"))->GetValue().GetString() == LODName)
			MasterMesh = Meshes[I];
	}

	ZEMLNode* MainLODsNode = MasterMesh->GetSubNodes("LODs").GetFirstItem();

	ZESize MainLODsNodeCount = MainLODsNode->GetSubNodes().GetCount();
	ZEArray<ZEMLItem*> TempLODLevel;
	ZEMLNode* LODNode = NULL;

	for (ZESize I = 0; I < MainLODsNodeCount; I++)
	{
		TempLODLevel.Clear();
		TempLODLevel = (MainLODsNode->GetSubNodes()[I])->GetProperties();
		ZEInt MeshLODLevel = ((ZEMLProperty*)TempLODLevel.GetFirstItem())->GetValue().GetInt32();

		if (I + 1 < MainLODsNodeCount)
		{
			TempLODLevel = (MainLODsNode->GetSubNodes()[I + 1])->GetProperties();
			ZEInt NextMeshLODLevel = ((ZEMLProperty*)TempLODLevel.GetFirstItem())->GetValue().GetInt32();

			if (MeshLODLevel < CurrentMeshLODLevel && CurrentMeshLODLevel < NextMeshLODLevel)
			{
				LODNode = MainLODsNode->InsertSubNode("LOD", I + 1);
				break;
			}
		}
		else
		{
			if (MeshLODLevel < CurrentMeshLODLevel)
			{
				LODNode = MainLODsNode->AddSubNode("LOD");
				break;
			}
			else
			{
				//Error master lod error
				return false;
			}
		}
	}

	LODNode->AddProperty("LODLevel", CurrentMeshLODLevel);

	if (!ProcessMeshLODVertices(Node, LODNode))
		return false;

	ProcessedMeshes.Append(1, &Node);

	return true;
}

bool ZE3dsMaxModelExporter::ProcessMeshes()
{
	zeLog("Processing Meshes...");

	ZEArray<IGameNode*> MasterMeshes;
	ZEArray<IGameNode*>	MeshLODs;
	bool MeshExists = false;
	Tab<IGameNode*> Nodes = Scene->GetIGameNodeByType(IGameObject::IGAME_MESH);

	for (ZESize I = 0; I < (ZESize)Nodes.Count(); I++)
	{
		if (GetBoneId(Nodes[I]) == -1)
		{
			MeshExists = false;

			const char* ZEType;

			IGameMesh* Mesh = (IGameMesh*)Nodes[I]->GetIGameObject();
			if (!ZE3dsMaxUtils::GetProperty(Mesh, ZE_STRING_PROP, "ZEType", ZEType) || strcmp(ZEType, "Mesh") != 0)	
				continue;

			for (ZESize J = 0; J < MasterMeshes.GetCount(); J++)
			{
				if (strncmp(MasterMeshes[J]->GetName(), Nodes[I]->GetName(), ZE_EXFL_MAX_NAME_SIZE) == 0)
				{
					MeshExists = true;
					ZEInt MasterMeshLOD;
					ZEInt CurrentMeshLOD;

					IGameMesh* MasterMesh = (IGameMesh*)MasterMeshes[J]->GetIGameObject();
					ZE3dsMaxUtils::GetProperty(MasterMesh, ZE_INT_PROP, "Mesh_LOD", MasterMeshLOD);

					IGameMesh* CurrentMesh = (IGameMesh*)Nodes[I]->GetIGameObject();
					ZE3dsMaxUtils::GetProperty(CurrentMesh, ZE_INT_PROP, "Mesh_LOD", CurrentMeshLOD);


					if (MasterMeshLOD == CurrentMeshLOD)
					{
						zeError("Mesh has two LOD with the same LOD Level. (Mesh Name : \"%s\", LodLevel : %d)", Nodes[I]->GetName(), MasterMeshLOD);
						return false;
					}

					if (CurrentMeshLOD < MasterMeshLOD)
					{
						MeshLODs.Add(MasterMeshes[J]);
						MasterMeshes[J] = Nodes[I];
					}
				}
			}

			if (!MeshExists)
			{
				MasterMeshes.Add(Nodes[I]);
			}
		}
	}

	if (MasterMeshes.GetCount() == 0)
	{
		zeWarning("No meshes with ZEMeshAttribute found. Mesh processing aborted.");
		return true;
	}

	ZEMLNode* MeshesNode = ModelNode.AddSubNode("Meshes");

	for (ZESize I = 0; I < MasterMeshes.GetCount(); I++)
	{
		ZEProgressDialog::GetInstance()->OpenTask(Nodes[I]->GetName());

		if (!ProcessMasterMesh(MasterMeshes[I], MeshesNode))
		{
			ZEProgressDialog::GetInstance()->CloseTask();
			return false;
		}

		ZEProgressDialog::GetInstance()->CloseTask();
	}

	for (ZESize I = 0; I < MeshLODs.GetCount(); I++)
	{
		if (!ProcessMeshLODs(MeshLODs[I], MeshesNode))
			return false;
	}

	return true;
}

void ZE3dsMaxModelExporter::ProcessAnimationFrames(ZESize AnimationStartFrame, ZESize AnimationFrameCount, ZEMLNode* AnimationNode)
{
	if (AnimationStartFrame + AnimationFrameCount - 1 > (ZESize)TotalFrameCount)
		zeError("Specified animation frame range is exceeds 3ds Max Scene frame count.");

	Tab<IGameNode*>	CurrentAnimationAffectedBones;
	Tab<IGameNode*>	CurrentAnimationAffectedMeshes;
	ZEInt ActualAnimationFrame = (ZEInt)AnimationStartFrame;

	GMatrix Matrix;
	for (ZESize I = 0; I < (ZESize)ProcessedMasterMeshes.Count(); I++)
	{
		//Matrix = ProcessedMasterMeshes[I]->GetWorldTM(); //Karsılaştır.
		Matrix = ProcessedMasterMeshes[I]->GetWorldTM((ZEInt)AnimationStartFrame * TicksPerFrame);

		ActualAnimationFrame = (ZEInt)AnimationStartFrame;
		for (ZESize N = 0; N < AnimationFrameCount; N++)
		{
			if (!(ProcessedMeshes[I]->GetWorldTM(ActualAnimationFrame * TicksPerFrame) == Matrix))
			{
				CurrentAnimationAffectedMeshes.Append(1, &ProcessedMeshes[I]);
				break;
			}

			ActualAnimationFrame++;
		}
	}

	for (ZESize I = 0; I < (ZESize)ProcessedBones.Count(); I++)
	{
		//Matrix = ProcessedBones[I]->GetWorldTM();
		Matrix = ProcessedBones[I]->GetWorldTM((ZEInt)AnimationStartFrame * TicksPerFrame);

		ActualAnimationFrame = (ZEInt)AnimationStartFrame;
		for (ZESize N = 0; N < AnimationFrameCount; N++)
		{
			if (!(ProcessedBones[I]->GetWorldTM(ActualAnimationFrame * TicksPerFrame) == Matrix))
			{
				CurrentAnimationAffectedBones.Append(1, &ProcessedBones[I]);
				break;
			}

			ActualAnimationFrame++;
		}
	}

	ZESize MeshKeyCount = (ZESize)CurrentAnimationAffectedMeshes.Count();
	ZESize BoneKeyCount = (ZESize)CurrentAnimationAffectedBones.Count();
	ZESize FrameKeyCount = MeshKeyCount + BoneKeyCount;

	AnimationNode->AddProperty("BoneKeyCount", BoneKeyCount);
	AnimationNode->AddProperty("MeshKeyCount", MeshKeyCount);

	ZEArray<ZEModelFileAnimationKey> Frames;
	Frames.SetCount((MeshKeyCount + BoneKeyCount) * AnimationFrameCount);
	ActualAnimationFrame = (ZEInt)AnimationStartFrame;

	zeLog("Frame Count : %d", AnimationFrameCount);
	zeLog("Processing Animation Frames: %d - %d", AnimationStartFrame, AnimationStartFrame + AnimationFrameCount - 1);

	for (ZESize I = 0; I < AnimationFrameCount; I++)
	{
		ZEModelFileAnimationKey* Key;

		for (ZESize N = 0; N < (ZESize)CurrentAnimationAffectedBones.Count(); N++)
		{
			Key = &Frames[(I * FrameKeyCount) + N];
			Key->ItemId = GetBoneId(CurrentAnimationAffectedBones[N]);

			ZEMLNode* MainBonesNode = ModelNode.GetSubNodes("Bones").GetFirstItem();
			ZEArray<ZEMLNode*> BoneNodes = MainBonesNode->GetSubNodes("Bone");
			ZEInt32 ParentBoneId = ((ZEMLProperty*)BoneNodes[(ZESize)Key->ItemId]->GetProperty("ParentBone"))->GetValue().GetInt32();

			if (ParentBoneId == -1)
			{
				GMatrix Matrix = CurrentAnimationAffectedBones[N]->GetWorldTM(ActualAnimationFrame * TicksPerFrame);
				Key->Position = ZE3dsMaxUtils::MaxtoZE(Matrix.Translation());
				Key->Rotation = ZE3dsMaxUtils::MaxtoZE(Matrix.Rotation());
				Key->Scale = ZE3dsMaxUtils::MaxtoZE(Matrix.Scaling());
			}
			else
			{
				GMatrix Matrix = CurrentAnimationAffectedBones[N]->GetWorldTM(ActualAnimationFrame * TicksPerFrame) * 
					ProcessedBones[(ZESize)ParentBoneId]->GetWorldTM(ActualAnimationFrame * TicksPerFrame).Inverse();
				//AnimationEnabledBones[N]->GetNodeParent()->GetWorldTM(I * TicksPerFrame).Inverse();
				Key->Position = ZE3dsMaxUtils::MaxtoZE(Matrix.Translation());
				Key->Rotation = ZE3dsMaxUtils::MaxtoZE(Matrix.Rotation());
				Key->Scale = ZE3dsMaxUtils::MaxtoZE(Matrix.Scaling());
			}
		}

		for (ZESize N = 0; N < (ZESize)CurrentAnimationAffectedMeshes.Count(); N++)
		{
			Key = &Frames[(I * FrameKeyCount) + BoneKeyCount + N];
			Key->ItemId = GetMeshId(CurrentAnimationAffectedMeshes[N]);
			GMatrix Matrix = CurrentAnimationAffectedMeshes[N]->GetWorldTM(ActualAnimationFrame * TicksPerFrame);
			Key->Position = ZE3dsMaxUtils::MaxtoZE(Matrix.Translation());
			Key->Rotation = ZE3dsMaxUtils::MaxtoZE(Matrix.Rotation());
			Key->Scale = ZE3dsMaxUtils::MaxtoZE(Matrix.Scaling());
		}

		ActualAnimationFrame++;
	}

	AnimationNode->AddDataProperty("Frames", Frames.GetCArray(), Frames.GetCount() * sizeof(ZEModelFileAnimationKey), true);

}

bool ZE3dsMaxModelExporter::ProcessAnimations(ZEMLNode* AnimationsNode)
{
	zeLog("Processing Animations...");

	if (ExportOptions->GetSubNodes("Animations").GetCount() > 0)
	{
		ZEMLNode* ExportAnimationsNode = ExportOptions->GetSubNodes("Animations").GetFirstItem();
		ZESize AnimationCount = ExportAnimationsNode->GetSubNodes("Animation").GetCount();
		ZEString AnimationName;

		for (ZESize I = 0; I < AnimationCount; I++)
		{
			ZEMLNode* AnimationNode = AnimationsNode->AddSubNode("Animation");
			ZEMLNode* ExportAnimationNode = ExportAnimationsNode->GetSubNodes("Animation").GetItem(I);
			AnimationName = ((ZEMLProperty*)ExportAnimationNode->GetProperty("Name"))->GetValue().GetString();
			AnimationNode->AddProperty("Name", AnimationName);

			ZEInt CurrentAnimationStartFrame = ((ZEMLProperty*)ExportAnimationNode->GetProperty("StartFrame"))->GetValue().GetString().ToInt32();
			ZEInt CurrentAnimationEndFrame = ((ZEMLProperty*)ExportAnimationNode->GetProperty("EndFrame"))->GetValue().GetString().ToInt32();
			ZESize CurrentAnimationFrameCount = ZESize(CurrentAnimationEndFrame - CurrentAnimationStartFrame + 1);

			ZEProgressDialog::GetInstance()->OpenTask(AnimationName);
			zeLog("Processing animation \"%s\".", AnimationName);
			ProcessAnimationFrames((ZESize)CurrentAnimationStartFrame, CurrentAnimationFrameCount, AnimationNode);
			zeLog("Animation \"%s\" is processed.", AnimationName);
			ZEProgressDialog::GetInstance()->CloseTask();
		}
	}
	else
	{
		ZEMLNode* AnimationNode = AnimationsNode->AddSubNode("Animation");
		AnimationNode->AddProperty("Name", "Default");

		ZEProgressDialog::GetInstance()->OpenTask("Default");
		zeLog("Processing animation \"Default\".");
		ProcessAnimationFrames(0, TotalFrameCount, AnimationNode);
		zeLog("Animation \"Default\" is processed.");
		ZEProgressDialog::GetInstance()->CloseTask();
	}

	return true;
}

bool ZE3dsMaxModelExporter::ProcessHelper(IGameNode* Node, ZEMLNode* HelpersNode)
{
	ZEProgressDialog::GetInstance()->OpenTask(Node->GetName());
	zeLog("Processing helper \"%s\".", Node->GetName());
	ZEMLNode* HelperNode = HelpersNode->AddSubNode("Helper");

	IGameObject* Helper = Node->GetIGameObject();
	HelperNode->AddProperty("Name", Node->GetName());

	INode* OwnerNode = NULL;
	ZEInt OwnerId;
	ZEModelHelperOwnerType OwnerType;
	
	if(!ZE3dsMaxUtils::GetProperty(Helper, "Owner", OwnerNode))
		zeError("Can not find helper property: Owner");

	IGameNode* OwnerGameNode = Scene->GetIGameNode(OwnerNode);

	if (OwnerNode == NULL)
	{
		zeWarning("Helper \"%s\" has no immediate owner parameter. Model will be set as owner.", Node->GetName());

		OwnerId = -1;
		OwnerType = ZEModelHelperOwnerType::ZE_MHOT_MODEL;
	}
	else
	{
		const char* Type;
		bool CurrentExportOption;

		ZE3dsMaxUtils::GetProperty(OwnerGameNode->GetIGameObject(), ZE_STRING_PROP, "ZEType", Type);

		if (strcmp(Type, "Mesh") == 0)
		{
			CurrentExportOption = ((ZEMLProperty*)(ExportOptions->GetProperty("IsMeshExportEnabled")))->GetValue().GetBoolean();

			if (CurrentExportOption)
			{
				OwnerId = ZE3dsMaxModelExporter::GetMeshId(OwnerGameNode);
				OwnerType = ZEModelHelperOwnerType::ZE_MHOT_MESH;
			}
			else
			{
				OwnerId = -1;
				OwnerType = ZEModelHelperOwnerType::ZE_MHOT_MODEL;

				zeWarning("Since mesh export option is disabled, Helper \"%s\" will be exported without an owner.", Node->GetName());
			}
		}
		else if (strcmp(Type, "Bone") == 0)
		{
			CurrentExportOption = ((ZEMLProperty*)(ExportOptions->GetProperty("IsBoneExportEnabled")))->GetValue().GetBoolean();

			if (CurrentExportOption)
			{
				OwnerId = ZE3dsMaxModelExporter::GetBoneId(OwnerGameNode);
				OwnerType = ZEModelHelperOwnerType::ZE_MHOT_BONE;	
			}
			else
			{
				OwnerId = -1;
				OwnerType = ZEModelHelperOwnerType::ZE_MHOT_MODEL;

				zeWarning("Since bone export option is disabled, Helper \"%s\" will be exported without an owner.", Node->GetName());
			}

		}
		else
		{
			zeError("Helper \"%s\" has invalid owner parameter.", Node->GetName());
			return false;
		}
	}

	if (OwnerId < 0 && OwnerType != ZE_MHOT_MODEL)
	{
		zeError("Helper \"%s\" has invalid owner parameter.", Node->GetName());
		return false;
	}

	HelperNode->AddProperty("OwnerType", OwnerType);
	HelperNode->AddProperty("OwnerId", OwnerId);

	ZEMatrix4x4 FinalTransform;

	ZEMatrix4x4 WorldTM;
	ZEMatrix4x4::CreateOrientation(WorldTM, 
		ZE3dsMaxUtils::MaxtoZE(Node->GetWorldTM().Translation()), 
		ZE3dsMaxUtils::MaxtoZE(Node->GetWorldTM().Rotation()), 
		ZE3dsMaxUtils::MaxtoZE(Node->GetWorldTM().Scaling()));

	if (OwnerId != -1)
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

	HelperNode->AddProperty("Position", FinalTransform.GetTranslation());
	HelperNode->AddProperty("Rotation", FinalTransform.GetRotation());
	HelperNode->AddProperty("Scale", FinalTransform.GetScale());


	zeLog("Helper \"%s\" is processed.", Node->GetName());
	ZEProgressDialog::GetInstance()->CloseTask();

	return true;
}

bool ZE3dsMaxModelExporter::ProcessHelpers()
{
	zeLog("Processing helpers...");

	Tab<IGameNode*> Nodes = Scene->GetIGameNodeByType(IGameObject::IGAME_HELPER);
	const char* Type;

	for (ZESize I = 0; I < (ZESize)Nodes.Count(); I++)
	{
		Type = NULL;
		if (ZE3dsMaxUtils::GetProperty(Nodes[I]->GetIGameObject(), ZE_STRING_PROP, "ZEType", Type) && strcmp(Type, "Helper") == 0)
		{
			bool Found = false;
			for (ZESize N = 0; N < (ZESize)ProcessedHelpers.Count(); N++)
			{
				if (Nodes[I]->GetNodeID() == ProcessedHelpers[N]->GetNodeID())
				{
					Found = true;
					break;
				}			
			}
			if (Found == false)
				ProcessedHelpers.Append(1, &Nodes[I]);
		}
	}

	if (ProcessedHelpers.Count() == 0)
	{
		zeLog("No helpers with ZEHelperAttribute found. Helper processing skipped.");
		return true;
	}

	ZEMLNode* HelpersNode = ModelNode.AddSubNode("Helpers");

	for (ZESize I = 0; I < (ZESize)ProcessedHelpers.Count(); I++)
		if (!ProcessHelper(ProcessedHelpers[I], HelpersNode))
			return false;

	return true;
}

void ZE3dsMaxModelExporter::CollectResources()
{
	ZESize MeshNodeCount = 0;

	Tab<IGameNode*>		Nodes = Scene->GetIGameNodeByType(IGameObject::IGAME_MESH);
	Tab<IGameNode*>		ResourceMeshes;
	Tab<IGameMaterial*>	ResourceMaterials;

	for (ZESize I = 0; I < (ZESize)Nodes.Count(); I++)
	{
		IGameNode* CurrentNode = Nodes[I];;
		IGameObject* CurrentObject = CurrentNode->GetIGameObject();
		const char* NodeZEType;

		if (!ZE3dsMaxUtils::GetProperty(CurrentObject, ZE_STRING_PROP, "ZEType", NodeZEType))
			continue;

		if (strcmp(NodeZEType, "Mesh") == 0)
		{
			MeshNodeCount++;
			ResourceMeshes.Append(1, &CurrentNode);
		}
	}

	for (ZESize I = 0; I < (ZESize)ResourceMeshes.Count(); I++)
	{
		IGameNode* CurrentNode = ResourceMeshes[I];
		IGameMaterial* CurrentMaterial = CurrentNode->GetNodeMaterial();

		if (CurrentMaterial == NULL)
			zeError("Mesh \"%s\" does not have valid material. Can not collect resource.", CurrentNode->GetName());

		bool IsFound = false;

		for(ZESize J = 0; J < (ZESize)ResourceMaterials.Count(); J++)
			if(ResourceMaterials[J] == CurrentMaterial)
				IsFound = true;

		if(!IsFound)
			ResourceMaterials.Append(1, &CurrentMaterial);
	}

	for (ZESize I = 0; I < (ZESize)ResourceMaterials.Count(); I++)
	{
		IGameMaterial* NodeMaterial = ResourceMaterials[I];
		ZEString MaterialName = NodeMaterial->GetMaterialName();

		ResourceConfigurationDialog->AddResource(MaterialName + ".ZEMaterial", "Material", ZEString(), ZE_ROAA_COPY_OVERWRITE);

		ZEInt NumberOfMaps = NodeMaterial->GetNumberOfTextureMaps();

		for (ZEInt N = 0; N < NumberOfMaps; N++)
		{
			IGameTextureMap* CurrentTexture = NodeMaterial->GetIGameTextureMap(N);
			switch(CurrentTexture->GetStdMapSlot())
			{
			case ID_AM: // Ambient
				break;
			case ID_DI: // Diffuse
				ResourceConfigurationDialog->AddResource(ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()), "Image", CurrentTexture->GetBitmapFileName(), ZEString());
				break;
			case ID_SP: // Specular
				ResourceConfigurationDialog->AddResource(ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()), "Image", CurrentTexture->GetBitmapFileName(), ZEString());
				break;
			case ID_SI:	// Emissive
				ResourceConfigurationDialog->AddResource(ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()), "Image", CurrentTexture->GetBitmapFileName(), ZEString());
				break;
			case ID_OP:	// Opacity 
				ResourceConfigurationDialog->AddResource(ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()), "Image", CurrentTexture->GetBitmapFileName(), ZEString());
				break;
			case ID_FI:	// Filter color 
				break;
			case ID_BU: // Bump 
				ResourceConfigurationDialog->AddResource(ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()), "Image", CurrentTexture->GetBitmapFileName(), ZEString());
				break;
			case ID_RL: // Reflection - Environment
				ResourceConfigurationDialog->AddResource(ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()), "Image", CurrentTexture->GetBitmapFileName(), ZEString());
				break; 
			case ID_RR: // Refraction 
				break;
			}

		}
	}
}
