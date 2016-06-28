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

#include "ZEProgressDialog.h"
#include "ZEResourceConfiguratorWidget.h"

#include "ZEMath/ZEMath.h"
#include "ZEFile/ZEFile.h"
#include "ZEFile/ZEFileInfo.h"
#include "ZEFile/ZEDirectoryInfo.h"
#include "ZEML/ZEMLWriter.h"

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

ZEInt32 ZE3dsMaxModelExporter::GetMeshId(IGameNode* Node)
{
	for (ZESize I = 0; I < (ZESize)ProcessedMasterMeshes.Count(); I++)
		if (ProcessedMasterMeshes[I]->GetNodeID() == Node->GetNodeID())
			return (ZEInt32)I;
	return -1;
}

ZEInt32 ZE3dsMaxModelExporter::GetBoneId(IGameNode* Node)
{
	for (ZESize I = 0; I < (ZESize)ProcessedBones.Count(); I++)
		if (ProcessedBones[I]->GetNodeID() == Node->GetNodeID())
			return (ZEInt32)I;

	return -1;
}

ZEInt32 ZE3dsMaxModelExporter::ProcessMeshMaterial(IGameMaterial* Material)
{
	if (Material == NULL)
		return -1;

	for (ZESize I = 0; I < (ZESize)ProcessedMaterials.Count(); I++)
		if (ProcessedMaterials[I] == Material)
			return (ZEInt32)I;
	
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

	ZEMLNode* MaterialsNode = ModelNode.AddNode("Materials");

	zeLog("Material count : %d", MaterialCount);

	for (ZESize I = 0; I < MaterialCount; I++)
	{
		IGameMaterial* NodeMaterial = ProcessedMaterials[I];
		ZEString MaterialName = NodeMaterial->GetMaterialName();

		ZEResourceOption MaterialOption;
		if (!ResourceConfiguratorDialog->GetOption(MaterialName + ".ZEMaterial", MaterialOption))
			zeError("Material export path not found in resource options. Resource identifier : %s", (MaterialName + ".ZEMaterial").ToCString());

		if (!ResourceConfiguratorDialog->GetCopyState(MaterialOption.Identifier))
		{
			ZEMLNode* MaterialNode = MaterialsNode->AddNode("Material");
			MaterialNode->AddProperty("Name")->SetString(MaterialName);
			MaterialNode->AddProperty("FilePath")->SetString(ResourceConfiguratorDialog->GetResourceRelativePath(ZEString(FileName) , MaterialName + ".ZEMaterial"));
			continue;
		}

		ZEString MaterialFilePath = MaterialOption.ExportPath + ZEString("/") + MaterialOption.Identifier;

		ZEProgressDialog::GetInstance()->OpenTask(MaterialName);
		zeLog("Processing material \"%s\" (%u/%d).", MaterialName.ToCString(), I + 1, ProcessedMaterials.Count());

		ZeroMemory(DiffuseMap, sizeof(DiffuseMap));
		ZeroMemory(SpecularMap, sizeof(SpecularMap));
		ZeroMemory(EmissiveMap, sizeof(EmissiveMap));
		ZeroMemory(OpacityMap, sizeof(OpacityMap));
		ZeroMemory(NormalMap, sizeof(NormalMap));
		ZeroMemory(EnvironmentMap, sizeof(EnvironmentMap));

		ZEFile MaterialFile;
		if (!MaterialFile.Open(MaterialOption.ExportPath + ZEString("/") + MaterialName + ".ZEMaterial", ZE_FOM_WRITE, ZE_FCM_OVERWRITE))
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
				if (!ResourceConfiguratorDialog->GetOption(ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName(), MaterialOption))
					zeError("Diffuse texture export path not found in resource options. Resource identifier : %s", ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName().ToCString());

				if (!ZEDirectoryInfo(MaterialOption.ExportPath).IsDirectory())
					zeError("Diffuse texture export path not valid . Resource identifier : %s", ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName().ToCString());
				else
				{
					MapFlag |= ZE_MTMP_DIFFUSEMAP;
					ResourceRelativePath = ResourceConfiguratorDialog->GetResourceRelativePath(MaterialFilePath, MaterialOption.Identifier);
					strncpy(DiffuseMap, ResourceRelativePath.ToCString() , ZE_EXFL_MAX_FILENAME_SIZE);

					if (ResourceConfiguratorDialog->GetCopyState(MaterialOption.Identifier))
					{
						if (!ZEFileInfo(MaterialOption.PhysicalPath).Copy(MaterialOption.ExportPath + "//" + MaterialOption.Identifier))
							zeError("Can not copy resource, resource identifier : %s", MaterialOption.Identifier.ToCString());
						else
							zeLog("Resource copied successfully, resource identifier : %s", MaterialOption.Identifier.ToCString());
					}
				}
				break;
			case ID_SP: // Specular
				if (!ResourceConfiguratorDialog->GetOption(ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName(), MaterialOption))
					zeError("Specular texture export path not found in resource options. Resource identifier : %s", ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName().ToCString());

				if (!ZEDirectoryInfo(MaterialOption.ExportPath).IsDirectory())
					zeError("Specular texture export path not valid . Resource identifier : %s", ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName().ToCString());
				else
				{
					MapFlag |= ZE_MTMP_SPECULARMAP;
					ResourceRelativePath = ResourceConfiguratorDialog->GetResourceRelativePath(MaterialFilePath, MaterialOption.Identifier);
					strncpy(SpecularMap, ResourceRelativePath.ToCString(), ZE_EXFL_MAX_FILENAME_SIZE);

					if (ResourceConfiguratorDialog->GetCopyState(MaterialOption.Identifier))
					{
						if (!ZEFileInfo(MaterialOption.PhysicalPath).Copy(MaterialOption.ExportPath + "//" + MaterialOption.Identifier))
							zeError("Can not copy resource, resource identifier : %s", MaterialOption.Identifier.ToCString());
						else
							zeLog("Resource copied successfully, resource identifier : %s", MaterialOption.Identifier.ToCString());
					}
				}
				break;
			case ID_SI:	// Emissive
				if (!ResourceConfiguratorDialog->GetOption(ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName(), MaterialOption))
					zeError("Emissive texture export path not found in resource options. Resource identifier : %s", ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName().ToCString());

				if (!ZEDirectoryInfo(MaterialOption.ExportPath).IsDirectory())
					zeError("Emissive texture export path not valid . Resource identifier : %s", ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName().ToCString());
				else
				{
					MapFlag |= ZE_MTMP_EMISSIVEMAP;
					ResourceRelativePath = ResourceConfiguratorDialog->GetResourceRelativePath(MaterialFilePath, MaterialOption.Identifier);
					strncpy(EmissiveMap, ResourceRelativePath.ToCString(), ZE_EXFL_MAX_FILENAME_SIZE);

					if (ResourceConfiguratorDialog->GetCopyState(MaterialOption.Identifier))
					{
						if (!ZEFileInfo(MaterialOption.PhysicalPath).Copy(MaterialOption.ExportPath + "//" + MaterialOption.Identifier))
							zeError("Can not copy resource, resource identifier : %s", MaterialOption.Identifier.ToCString());
						else
							zeLog("Resource copied successfully, resource identifier : %s", MaterialOption.Identifier.ToCString());
					}
				}
				break;
			case ID_OP:	// Opacity 
				if (!ResourceConfiguratorDialog->GetOption(ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName(), MaterialOption))
					zeError("Opacity texture export path not found in resource options. Resource identifier : %s", ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName().ToCString());

				if (!ZEDirectoryInfo(MaterialOption.ExportPath).IsDirectory())
					zeError("Opacity texture export path not valid . Resource identifier : %s", ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName().ToCString());
				else
				{
					MapFlag |= ZE_MTMP_OPACITYMAP;
					ResourceRelativePath = ResourceConfiguratorDialog->GetResourceRelativePath(MaterialFilePath, MaterialOption.Identifier);
					strncpy(OpacityMap, ResourceRelativePath.ToCString(), ZE_EXFL_MAX_FILENAME_SIZE);

					if (ResourceConfiguratorDialog->GetCopyState(MaterialOption.Identifier))
					{
						if (!ZEFileInfo(MaterialOption.PhysicalPath).Copy(MaterialOption.ExportPath + "//" + MaterialOption.Identifier))
							zeError("Can not copy resource, resource identifier : %s", MaterialOption.Identifier.ToCString());
						else
							zeLog("Resource copied successfully, resource identifier : %s", MaterialOption.Identifier.ToCString());
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
				if (!ResourceConfiguratorDialog->GetOption(ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName(), MaterialOption))
					zeError("Bump texture export path not found in resource options. Resource identifier : %s", ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName().ToCString());

				if (!ZEDirectoryInfo(MaterialOption.ExportPath).IsDirectory())
					zeError("Bump texture export path not valid . Resource identifier : %s", ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName().ToCString());
				else
				{
					MapFlag |= ZE_MTMP_NORMALMAP;
					ResourceRelativePath = ResourceConfiguratorDialog->GetResourceRelativePath(MaterialFilePath, MaterialOption.Identifier);
					strncpy(NormalMap, ResourceRelativePath.ToCString(), ZE_EXFL_MAX_FILENAME_SIZE);

					if (ResourceConfiguratorDialog->GetCopyState(MaterialOption.Identifier))
					{
						if (!ZEFileInfo(MaterialOption.PhysicalPath).Copy(MaterialOption.ExportPath + "//" + MaterialOption.Identifier))
							zeError("Can not copy resource, resource identifier : %s", MaterialOption.Identifier.ToCString());
						else
							zeLog("Resource copied successfully, resource identifier : %s", MaterialOption.Identifier.ToCString());
					}
				}
				break;
			case ID_RL: // Reflection - Environment
				if (!ResourceConfiguratorDialog->GetOption(ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName(), MaterialOption))
					zeError("Environment texture export path not found in resource options. Resource identifier : %s", ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName().ToCString());

				if (!ZEDirectoryInfo(MaterialOption.ExportPath).IsDirectory())
					zeError("Environment texture export path not valid . Resource identifier : %s", ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName().ToCString());
				else
				{
					MapFlag |= ZE_MTMP_ENVIRONMENTMAP;
					ResourceRelativePath = ResourceConfiguratorDialog->GetResourceRelativePath(MaterialFilePath, MaterialOption.Identifier);
					strncpy(EnvironmentMap, ResourceRelativePath.ToCString(), ZE_EXFL_MAX_FILENAME_SIZE);

					if (ResourceConfiguratorDialog->GetCopyState(MaterialOption.Identifier))
					{
						if (!ZEFileInfo(MaterialOption.PhysicalPath).Copy(MaterialOption.ExportPath + "//" + MaterialOption.Identifier))
							zeError("Can not copy resource, resource identifier : %s", MaterialOption.Identifier.ToCString());
						else
							zeLog("Resource copied successfully, resource identifier : %s", MaterialOption.Identifier.ToCString());
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

		ZEMLWriter MaterialWriter;
		MaterialWriter.Open(&MaterialFile);
		ZEMLWriterNode MaterialSerialNode;
		MaterialWriter.OpenRootNode("Material", MaterialSerialNode);
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
		MaterialConfigNode.WriteBool("AmbientEnabled", false); //Ambient is forced false
		ZEVector3 TempVector3Value = ZEVector3::Zero;
		NodeMaterial->GetAmbientData()->GetPropertyValue(*(Point3*)&TempVector3Value);
		MaterialConfigNode.WriteVector3("AmbientColor", TempVector3Value);

		//DIFFUSE
		MaterialConfigNode.WriteBool("DiffuseEnabled", true /*(MapFlag & ZE_MTMP_DIFFUSEMAP) != 0*/); //Diffuse is forced true
		TempVector3Value = ZEVector3::Zero;
		NodeMaterial->GetDiffuseData()->GetPropertyValue(*(Point3*)&TempVector3Value);
		MaterialConfigNode.WriteVector3("DiffuseColor", TempVector3Value);
		MaterialConfigNode.WriteString("BaseMap", DiffuseMap);

		//SPECULAR
		MaterialConfigNode.WriteBool("SpecularEnabled", true /*(MapFlag & ZE_MTMP_SPECULARMAP) != 0*/); //Specular is forced true
		float TempFloatValue = 0.0f;
		NodeMaterial->GetSpecularLevelData()->GetPropertyValue(TempFloatValue);
		MaterialConfigNode.WriteFloat("SpecularFactor", TempFloatValue);

		TempVector3Value = ZEVector3::Zero;
		NodeMaterial->GetSpecularData()->GetPropertyValue(*(Point3*)&TempVector3Value);
		MaterialConfigNode.WriteVector3("SpecularColor", TempVector3Value);
		MaterialConfigNode.WriteString("SpecularMap", SpecularMap);

		//EMISSIVE
		MaterialConfigNode.WriteBool("EmmisiveEnabled", (MapFlag & ZE_MTMP_EMISSIVEMAP) != 0);
		TempFloatValue = 0.0f;
		NodeMaterial->GetEmissiveAmtData()->GetPropertyValue(TempFloatValue);
		MaterialConfigNode.WriteFloat("EmmisiveFactor", TempFloatValue);

		TempVector3Value = ZEVector3::Zero;
		NodeMaterial->GetEmissiveData()->GetPropertyValue(*(Point3*)&TempVector3Value);
		MaterialConfigNode.WriteVector3("EmmisiveColor", TempVector3Value);
		MaterialConfigNode.WriteString("EmmisiveMap", EmissiveMap);

		//NORMAL
		MaterialConfigNode.WriteBool("NormalMapEnabled", (MapFlag & ZE_MTMP_NORMALMAP) != 0);
		MaterialConfigNode.WriteString("NormalMap", NormalMap);

		//OPACITY
		MaterialConfigNode.WriteBool("OpacityEnabled", (MapFlag & ZE_MTMP_OPACITYMAP) != 0);
		MaterialConfigNode.WriteFloat("Opacity", Opacity);
		MaterialConfigNode.WriteString("OpacityMap", OpacityMap);

		//ENVIRONMENT
		MaterialConfigNode.WriteString("EnvironmentMap", EnvironmentMap);

		MaterialConfigNode.CloseNode();
		MaterialSerialNode.CloseNode();
		MaterialWriter.Close();
		MaterialFile.Close();

		ZEMLNode* MaterialDOMNode = MaterialsNode->AddNode("Material");
		MaterialDOMNode->AddProperty("Name")->SetString(MaterialName);
		MaterialDOMNode->AddProperty("FilePath")->SetString(ResourceConfiguratorDialog->GetResourceRelativePath(ZEString(FileName) , MaterialName + ".ZEMaterial"));

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
		Point3 Vertex = Mesh->GetVertex((ZEInt32)I, false) * WorldTransform;
		Vertices[I] = ZE3dsMaxUtils::MaxtoZE(Vertex);
	}

	ShapeNode->AddData("Vertices")->SetData(Vertices.GetCArray(), sizeof(ZEVector3) * NumberofVertices, false);
}

bool ZE3dsMaxModelExporter::ProcessPhysicalShape(IGameNode* Node, IGameNode* OwnerNode, ZEMLNode* PhysicalShapeNode)
{
	IGameObject* Object = Node->GetIGameObject();

	if (Object == NULL)
		return false;

	ZEString ZEType;
	if (!ZE3dsMaxUtils::GetProperty(Object, ZE_STRING_PROP, ZEString("ZEType"), ZEType) || (!ZEType.Equals("PhysicalShape")))
	{
		zeError("Mesh is rejected because it does not contain ZEPhysicalShape. Node Name: \"%s\".", ZEString(Node->GetName()).ToCString());
		return false;
	}

	GMatrix Transform = Node->GetWorldTM() * OwnerNode->GetWorldTM().Inverse();
	PhysicalShapeNode->AddProperty("Position")->SetVector3(ZE3dsMaxUtils::MaxtoZE(Transform.Translation()));
	PhysicalShapeNode->AddProperty("Rotation")->SetQuaternion(ZE3dsMaxUtils::MaxtoZE(Transform.Rotation()));

	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("PhysicalShape_MaterialRestitution"),		*PhysicalShapeNode->AddProperty("Restitution"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("PhysicalShape_MaterialDynamicFriction"),	*PhysicalShapeNode->AddProperty("DynamicFriction"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("PhysicalShape_MaterialStaticFriction"),	*PhysicalShapeNode->AddProperty("StaticFriction"));

	MSTR UserDefinedPropertiesBuffer;
	Node->GetMaxNode()->GetUserPropBuffer(UserDefinedPropertiesBuffer);

	if (!UserDefinedPropertiesBuffer.isNull())
		PhysicalShapeNode->AddProperty("UserDefinedProperties")->SetString(ZEString(UserDefinedPropertiesBuffer.data()));

	ZEInt32 GeometryType;
	ZE3dsMaxUtils::GetProperty(Object, ZE_INT_PROP,	ZEString("PhysicalShape_GeometryType"),	GeometryType);

	if (GeometryType == 1)
	{
		ZEString ObjectClassName(Object->GetClassName());
		if (ObjectClassName.Equals("Box"))
		{
			PhysicalShapeNode->AddProperty("Type")->SetInt32((ZEInt32)ZE_PBST_BOX);
			ZEMLNode* ShapeNode = PhysicalShapeNode->AddNode("Shape");
			ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP, ZEString("width"),  *ShapeNode->AddProperty("Width"));
			ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP, ZEString("height"), *ShapeNode->AddProperty("Height"));
			ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP, ZEString("length"), *ShapeNode->AddProperty("Length"));
		}
		else if (ObjectClassName.Equals("Sphere"))
		{
			PhysicalShapeNode->AddProperty("Type")->SetInt32((ZEInt32)ZE_PBST_SPHERE);
			ZEMLNode* ShapeNode = PhysicalShapeNode->AddNode("Shape");
			ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP, ZEString("radius"), *ShapeNode->AddProperty("Radius"));
		}
		else if (ObjectClassName.Equals("Capsule"))
		{
			PhysicalShapeNode->AddProperty("Type")->SetInt32((ZEInt32)ZE_PBST_CAPSULE);
			ZEMLNode* ShapeNode = PhysicalShapeNode->AddNode("Shape");
			ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP, ZEString("radius"), *ShapeNode->AddProperty("Radius"));
			ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP, ZEString("height"), *ShapeNode->AddProperty("Height"));
		}
		else if (ObjectClassName.Equals("Cylinder"))
		{
			PhysicalShapeNode->AddProperty("Type")->SetInt32((ZEInt32)ZE_PBST_CYLINDER);
			ZEMLNode* ShapeNode = PhysicalShapeNode->AddNode("Shape");
			ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP, ZEString("radius"), *ShapeNode->AddProperty("Radius"));
			ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP, ZEString("height"), *ShapeNode->AddProperty("Height"));
		}
		else if (ObjectClassName.Equals("Editable Poly") || ObjectClassName.Equals("Editable Mesh") || ObjectClassName.Equals("Editable Patch"))
		{
			PhysicalShapeNode->AddProperty("Type")->SetInt32((ZEInt32)ZE_PBST_CONVEX);
			ProcessPhysicalBodyConvexShape(Node, OwnerNode, PhysicalShapeNode->AddNode("Shape"));
		}
		else
		{
			zeError("Shape type does not supported. Type : \"%s\", Node Name : \"%s\"", ObjectClassName.ToCString(), ZEString(Node->GetName()).ToCString());
			return false;
		}
	}
	else if (GeometryType == 2) // Convex
	{
		PhysicalShapeNode->AddProperty("Type")->SetInt32((ZEInt32)ZE_PBST_CONVEX);
		ProcessPhysicalBodyConvexShape(Node, OwnerNode, PhysicalShapeNode->AddNode("Shape"));
	}

	return true;
}

void ZE3dsMaxModelExporter::ProcessPhysicalBody(IGameNode* Node, ZEMLNode* ParentNode)
{
	IGameObject* Object = Node->GetIGameObject();

	if (Object == NULL)
		return;

	ZEInt32 PhysicalBodyTypeValue;
	ZE3dsMaxUtils::GetProperty(Object, ZE_INT_PROP, ZEString("PhysicalBody_Type"), PhysicalBodyTypeValue);
	PhysicalBodyTypeValue--; //This action is made because Max Script Array index starts from 1 instead of 0

	ZEPhysicalBodyType PhysicalBodyType = (ZEPhysicalBodyType)PhysicalBodyTypeValue;

	if (PhysicalBodyType == ZE_PBT_NONE)
	{
		zeWarning("No physical body found. Processing physical body aborted. Node Name : \"%s\"", ZEString(Node->GetName()).ToCString());
		return;
	}
	else if (PhysicalBodyType == ZE_PBT_RIGID)
	{
		zeLog("Processing physical body of \"%s\"", ZEString(Node->GetName()).ToCString());

		ZEMLNode* PhysicalBodyNode = ParentNode->AddNode("PhysicalBody");

		ZE3dsMaxUtils::GetProperty(Object, ZE_BOOL_PROP, ZEString("PhysicalBody_Enabled"), *PhysicalBodyNode->AddProperty("Enabled"));

		ZEInt32 PhysicalBodyTypeValue;
		ZE3dsMaxUtils::GetProperty(Object, ZE_INT_PROP, ZEString("PhysicalBody_Type"), PhysicalBodyTypeValue);
		PhysicalBodyTypeValue--; // Array index start problem with Max Script and C++)
		PhysicalBodyNode->AddProperty("Type")->SetInt32(PhysicalBodyTypeValue);

		ZE3dsMaxUtils::GetProperty(Object, ZE_BOOL_PROP, ZEString("PhysicalBody_Kinematic"), *PhysicalBodyNode->AddProperty("IsKinematic"));

		ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP, ZEString("PhysicalBody_Mass"), *PhysicalBodyNode->AddProperty("Mass"));

		IGameNode* MassCenter = NULL;
		ZE3dsMaxUtils::GetProperty(Object, ZEString("PhysicalBody_CenterOfMass"), Scene, MassCenter);

		if (MassCenter == NULL)
			PhysicalBodyNode->AddProperty("MassCenter")->SetVector3(ZEVector3::Zero);
		else
			PhysicalBodyNode->AddProperty("MassCenter")->SetVector3(ZE3dsMaxUtils::MaxtoZE(MassCenter->GetObjectTM().Translation() - Node->GetObjectTM().Translation()));

		ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP, ZEString("PhysicalBody_LinearDamping"), *PhysicalBodyNode->AddProperty("LinearDamping"));
		ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP, ZEString("PhysicalBody_AngularDamping"), *PhysicalBodyNode->AddProperty("AngularDamping"));

		IGameProperty* ShapesProp = Object->GetIPropertyContainer()->QueryProperty(ZEString("PhysicalBody_Shapes"));
		if (ShapesProp == NULL)
		{
			zeError("Missing property \"PhysicalShapes\" in ZEAttribute. Node Name : \"%s\"", ZEString(Node->GetName()).ToCString());
			return;
		}

		ZEMLNode* PhysicalShapesNode = PhysicalBodyNode->AddNode("PhysicalShapes");
		IParamBlock2* ParamBlock = ShapesProp->GetMaxParamBlock2();
		ZEInt32 ParamId = ParamBlock->IndextoID(ShapesProp->GetParamIndex());

		for (ZESize I = 0; I < (ZESize)ParamBlock->Count(ParamId); I++)
		{
			ZEString Type;
			IGameNode* PhysicalShapeNode = Scene->GetIGameNode(ParamBlock->GetINode(ParamId, 0, (ZEInt32)I));
			if (PhysicalShapeNode == NULL || !ZE3dsMaxUtils::GetProperty(PhysicalShapeNode->GetIGameObject(), ZE_STRING_PROP, ZEString("ZEType"), Type) || (!Type.Equals("PhysicalShape")))
			{
				zeError("Physical body shape is not a valid ZEPhysicalShapeAttribute. Array Index : %d, Shape Name : \"%s\".", I, (PhysicalShapeNode != NULL ? ZEString(PhysicalShapeNode->GetName()).ToCString() : ZEString("NULL").ToCString()));
				return;
			}

			ProcessPhysicalShape(PhysicalShapeNode, Node, PhysicalShapesNode->AddNode("PhysicalShape"));
		}

		zeLog("Physical body is processed successfully.");

		return;
	}
	else
	{
		zeWarning("Physical body type not supported. Node Name \"%s\"", ZEString(ParentNode->GetName()).ToCString());
		return;
	}
}

bool ZE3dsMaxModelExporter::ProcessPhysicalJoint(IGameNode* Node, ZEMLNode* PhysicalJointNode)
{
	IGameObject* Object = Node->GetIGameObject();

	if (Object == NULL)
		return false;

	zeLog("Processing physical joint of \"%s\"", ZEString(Node->GetName()).ToCString());

	ZE3dsMaxUtils::GetProperty(Object, ZE_BOOL_PROP, ZEString("Joint_Enabled"), *PhysicalJointNode->AddProperty("Enabled"));

	IGameNode* Body1 = NULL;
	ZE3dsMaxUtils::GetProperty(Object, ZEString("Joint_Body1"), Scene, Body1); 								
	if (Body1 != NULL)
		PhysicalJointNode->AddProperty("Body1Id")->SetInt32(GetMeshId(Body1));
	else
		PhysicalJointNode->AddProperty("Body1Id")->SetInt32((ZEInt32)-1);

	IGameNode* Body2 = NULL;
	ZE3dsMaxUtils::GetProperty(Object, ZEString("Joint_Body2"), Scene, Body2);
	if (Body2 != NULL)
		PhysicalJointNode->AddProperty("Body2Id")->SetInt32(GetMeshId(Body2));
	else
		PhysicalJointNode->AddProperty("Body2Id")->SetInt32((ZEInt32)-1);

	ZEMLProperty* GlobalAnchorAxisProperty = PhysicalJointNode->AddProperty("UseGlobalAnchorAxis");
	ZE3dsMaxUtils::GetProperty(Object, ZE_BOOL_PROP, ZEString("Joint_UseGlobalAnchorAxis"), *GlobalAnchorAxisProperty);
	if (!GlobalAnchorAxisProperty->GetValue().GetBoolean())
	{
		IGameNode* BodyAnchor1 = NULL;
		ZE3dsMaxUtils::GetProperty(Object, ZEString("Joint_Body1_Anchor"), Scene, BodyAnchor1);
		if (BodyAnchor1 != NULL)
		{
			GMatrix LocalTransform =  BodyAnchor1->GetObjectTM() * Node->GetObjectTM().Inverse();
			PhysicalJointNode->AddProperty("LocalAnchor1")->SetVector3(ZE3dsMaxUtils::MaxtoZE(LocalTransform.Translation()));
			PhysicalJointNode->AddProperty("LocalAxis1")->SetQuaternion(ZE3dsMaxUtils::MaxtoZE(LocalTransform.Rotation()));
		}

		IGameNode* BodyAnchor2 = NULL;
		ZE3dsMaxUtils::GetProperty(Object, ZEString("Joint_Body2_Anchor"), Scene, BodyAnchor2);
		if (BodyAnchor2 != NULL)
		{
			GMatrix LocalTransform = BodyAnchor2->GetObjectTM() * Node->GetObjectTM().Inverse();
			PhysicalJointNode->AddProperty("LocalAnchor2")->SetVector3(ZE3dsMaxUtils::MaxtoZE(LocalTransform.Translation()));
			PhysicalJointNode->AddProperty("LocalAxis2")->SetQuaternion(ZE3dsMaxUtils::MaxtoZE(LocalTransform.Rotation()));
		}
	}
	else
	{
		PhysicalJointNode->AddProperty("GlobalAnchor")->SetVector3(ZE3dsMaxUtils::MaxtoZE(Node->GetObjectTM().Translation()));
		PhysicalJointNode->AddProperty("GlobalAxis")->SetQuaternion(ZE3dsMaxUtils::MaxtoZE(Node->GetObjectTM().Rotation()));
	}

	ZEInt32 JointTypeValue = 0;
	ZE3dsMaxUtils::GetProperty(Object, ZE_INT_PROP,	ZEString("Joint_Type"), JointTypeValue);
	JointTypeValue--;
	PhysicalJointNode->AddProperty("JointType")->SetInt32(JointTypeValue);

	ZE3dsMaxUtils::GetProperty(Object, ZE_BOOL_PROP,	ZEString("Joint_CollideBodies"),			*PhysicalJointNode->AddProperty("CollideBodies"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_BOOL_PROP,	ZEString("Joint_Breakable"),				*PhysicalJointNode->AddProperty("Breakable"));	
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_Break_Force"),				*PhysicalJointNode->AddProperty("BreakForce"));					
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_Break_Torque"),				*PhysicalJointNode->AddProperty("BreakTorque"));

	//Added : Free Joint Properties:

	ZE3dsMaxUtils::GetProperty(Object, ZE_INT_PROP,		ZEString("Joint_XMotion"), 					*PhysicalJointNode->AddProperty("XMotion"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_INT_PROP,		ZEString("Joint_YMotion"), 					*PhysicalJointNode->AddProperty("YMotion"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_INT_PROP,		ZEString("Joint_ZMotion"), 					*PhysicalJointNode->AddProperty("ZMotion"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_LinearLimitValue"),			*PhysicalJointNode->AddProperty("LinearLimitValue"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_LinearLimitRestitution"), 	*PhysicalJointNode->AddProperty("LinearLimitRestitution"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_LinearLimitSpring"), 		*PhysicalJointNode->AddProperty("LinearLimitSpring"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_LinearLimitDamping"), 		*PhysicalJointNode->AddProperty("LinearLimitDamping"));

	ZE3dsMaxUtils::GetProperty(Object, ZE_INT_PROP,		ZEString("Joint_TwistMotion"), 					*PhysicalJointNode->AddProperty("TwistMotion"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_TwistLowLimitValue"), 			*PhysicalJointNode->AddProperty("TwistLowLimitValue"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_TwistLowLimitRestitution"), 	*PhysicalJointNode->AddProperty("TwistLowLimitRestitution"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_TwistLowLimitSpring"), 			*PhysicalJointNode->AddProperty("TwistLowLimitSpring"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_TwistLowLimitDamping"), 		*PhysicalJointNode->AddProperty("TwistLowLimitDamping"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_TwistHighLimitValue"), 			*PhysicalJointNode->AddProperty("TwistHighLimitValue"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_TwistHighLimitRestitution"),	*PhysicalJointNode->AddProperty("TwistHighLimitRestitution"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_TwistHighLimitSpring"), 		*PhysicalJointNode->AddProperty("TwistHighLimitSpring"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_TwistHighLimitDamping"), 		*PhysicalJointNode->AddProperty("TwistHighLimitDamping"));

	ZE3dsMaxUtils::GetProperty(Object, ZE_INT_PROP,		ZEString("Joint_Swing1Motion"), 				*PhysicalJointNode->AddProperty("Swing1Motion"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_Swing1LimitValue"), 			*PhysicalJointNode->AddProperty("Swing1LimitValue"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_Swing1LimitRestitution"), 		*PhysicalJointNode->AddProperty("Swing1LimitRestitution"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_Swing1LimitSpring"), 			*PhysicalJointNode->AddProperty("Swing1LimitSpring"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_Swing1LimitDamping"), 			*PhysicalJointNode->AddProperty("Swing1LimitDamping"));

	ZE3dsMaxUtils::GetProperty(Object, ZE_INT_PROP,		ZEString("Joint_Swing2Motion"), 				*PhysicalJointNode->AddProperty("Swing2Motion"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_Swing2LimitValue"), 			*PhysicalJointNode->AddProperty("Swing2LimitValue"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_Swing2LimitRestitution"), 		*PhysicalJointNode->AddProperty("Swing2LimitRestitution"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_Swing2LimitSpring"), 			*PhysicalJointNode->AddProperty("Swing2LimitSpring"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_Swing2LimitDamping"), 			*PhysicalJointNode->AddProperty("Swing2LimitDamping"));

	ZE3dsMaxUtils::GetProperty(Object, ZE_INT_PROP,		ZEString("Joint_XLinearMotor"), 				*PhysicalJointNode->AddProperty("LinearXMotor"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_XLinearMotorForce"), 			*PhysicalJointNode->AddProperty("LinearXMotorForce"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_XLinearMotorSpring"), 			*PhysicalJointNode->AddProperty("LinearXMotorSpring"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_XLinearMotorDamper"), 			*PhysicalJointNode->AddProperty("LinearXMotorDamper"));

	ZE3dsMaxUtils::GetProperty(Object, ZE_INT_PROP,		ZEString("Joint_YLinearMotor"), 				*PhysicalJointNode->AddProperty("LinearYMotor"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_YLinearMotorForce"), 			*PhysicalJointNode->AddProperty("LinearYMotorForce"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_YLinearMotorSpring"), 			*PhysicalJointNode->AddProperty("LinearYMotorSpring"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_YLinearMotorDamper"), 			*PhysicalJointNode->AddProperty("LinearYMotorDamper"));
														
	ZE3dsMaxUtils::GetProperty(Object, ZE_INT_PROP,		ZEString("Joint_ZLinearMotor"), 				*PhysicalJointNode->AddProperty("LinearZMotor"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_ZLinearMotorForce"), 			*PhysicalJointNode->AddProperty("LinearZMotorForce"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_ZLinearMotorSpring"), 			*PhysicalJointNode->AddProperty("LinearZMotorSpring"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_ZLinearMotorDamper"), 			*PhysicalJointNode->AddProperty("LinearZMotorDamper"));
														
	ZE3dsMaxUtils::GetProperty(Object, ZE_INT_PROP,		ZEString("Joint_AngularSwingMotor"), 			*PhysicalJointNode->AddProperty("AngularSwingMotor"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_AngularSwingMotorForce"), 		*PhysicalJointNode->AddProperty("AngularSwingMotorForce"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_AngularSwingMotorSpring"), 		*PhysicalJointNode->AddProperty("AngularSwingMotorSpring"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_AngularSwingMotorDamper"), 		*PhysicalJointNode->AddProperty("AngularSwingMotorDamper"));
														
	ZE3dsMaxUtils::GetProperty(Object, ZE_INT_PROP,		ZEString("Joint_AngularTwistMotor"), 			*PhysicalJointNode->AddProperty("AngularTwistMotor"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_AngularTwistMotorForce"), 		*PhysicalJointNode->AddProperty("AngularTwistMotorForce"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_AngularTwistMotorSpring"),		*PhysicalJointNode->AddProperty("AngularTwistMotorSpring"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_AngularTwistMotorDamper"),		*PhysicalJointNode->AddProperty("AngularTwistMotorDamper"));
														
	ZE3dsMaxUtils::GetProperty(Object, ZE_INT_PROP,		ZEString("Joint_AngularSlerpMotor"), 			*PhysicalJointNode->AddProperty("AngularSlerpMotor"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_AngularSlerpMotorForce"), 		*PhysicalJointNode->AddProperty("AngularSlerpMotorForce"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_AngularSlerpMotorSpring"), 		*PhysicalJointNode->AddProperty("AngularSlerpMotorSpring"));
	ZE3dsMaxUtils::GetProperty(Object, ZE_FLOAT_PROP,	ZEString("Joint_AngularSlerpMotorDamper"), 		*PhysicalJointNode->AddProperty("AngularSlerpMotorDamper"));

	Quat Quaternion;
	Point3 Point;

	IGameNode* MotorTargetPosition = NULL;
	ZE3dsMaxUtils::GetProperty(Object, ZEString("Joint_MotorTargetPosition"), Scene, MotorTargetPosition);
	if (MotorTargetPosition != NULL)
	{
		PhysicalJointNode->AddProperty("MotorTargetPosition")->SetVector3(ZE3dsMaxUtils::MaxtoZE(MotorTargetPosition->GetObjectTM().Translation() - Node->GetObjectTM().Translation()));
	}
	else
		PhysicalJointNode->AddProperty("MotorTargetPosition")->SetVector3(ZEVector3::Zero);

	ZE3dsMaxUtils::GetProperty(Object, ZE_VECTOR3_PROP, ZEString("Joint_MotorTargetVelocity"), *PhysicalJointNode->AddProperty("MotorTargetVelocity"));

	IGameNode* MotorTargetOrientation = NULL;
	ZE3dsMaxUtils::GetProperty(Object, ZEString("Joint_MotorTargetOrientation"), Scene, MotorTargetOrientation);
	if (MotorTargetOrientation != NULL)
	{
		GMatrix Orientation = MotorTargetPosition->GetObjectTM() * Node->GetObjectTM().Inverse();
		PhysicalJointNode->AddProperty("MotorTargetOrientation")->SetQuaternion(ZE3dsMaxUtils::MaxtoZE(Orientation.Rotation())); //Investigate why there's error
		// ERROR ERROR
		//Joint->Joint.LinearMotorPosition = ZE3dsMaxUtils::MaxtoZE(Orientation.Rotation());
	}
	else
		PhysicalJointNode->AddProperty("MotorTargetOrientation")->SetQuaternion(ZEQuaternion::Identity);

	ZE3dsMaxUtils::GetProperty(Object, ZE_VECTOR3_PROP, ZEString("Joint_MotorTargetAngularVelocity"), *PhysicalJointNode->AddProperty("MotorTargetAngularVelocity"));

	zeLog("Physical joint is processed successfully.");

	return true;
}

bool ZE3dsMaxModelExporter::ProcessBone(IGameNode* Node, ZEMLNode* BonesNode)
{
	ZEProgressDialog::GetInstance()->OpenTask(Node->GetName());
	zeLog("Processing bone \"%s\".", ZEString(Node->GetName()).ToCString());
	ZEMLNode* BoneNode = BonesNode->AddNode("Bone");

	IGameObject* Bone = Node->GetIGameObject();
	BoneNode->AddProperty("Name")->SetString(ZEString(Node->GetName()));

	Point3 BoneScale = Node->GetLocalTM().Scaling();
	if (Node->GetWorldTM().Parity() == -1)
	{
		zeError("The bone is mirrored. Bone Name : \"%s\". Mirror transformation operation is not supported. Use bone mirror operation instead.", ZEString(Node->GetName()).ToCString());
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

	BoneNode->AddProperty("ParentBone")->SetInt32(ParentBoneId);

	Box3 BB;
	Bone->GetMaxObject()->GetLocalBoundBox(0, Node->GetMaxNode(), NULL, BB);
	ZEMLNode* BoundingBoxNode = BoneNode->AddNode("BoundingBox");

	Point3 Min, Max;
	Min.x = BB.Min().x;
	Min.y = BB.Min().z;
	Min.z = BB.Min().y;

	Max.x = BB.Max().x;
	Max.y = BB.Max().z;
	Max.z = BB.Max().y;

	BoundingBoxNode->AddProperty("Min")->SetVector3(ZE3dsMaxUtils::MaxtoZE(Min));
	BoundingBoxNode->AddProperty("Max")->SetVector3(ZE3dsMaxUtils::MaxtoZE(Max));

	if (ParentBoneId != -1)
	{
		GMatrix Transform = Node->GetWorldTM() * ProcessedBones[(ZESize)ParentBoneId]->GetWorldTM().Inverse();
		BoneNode->AddProperty("RelativePosition")->SetVector3(ZE3dsMaxUtils::MaxtoZE(Transform.Translation() * Node->GetWorldTM().Scaling()));
		BoneNode->AddProperty("RelativeRotation")->SetQuaternion(ZE3dsMaxUtils::MaxtoZE(Transform.Rotation()));
		BoneNode->AddProperty("RelativeScale")->SetVector3(ZE3dsMaxUtils::MaxtoZE(Transform.Scaling()));
	}
	else
	{ 
		zeLog("\"%s\" is the Root Bone", ZEString(Node->GetName()).ToCString());
		BoneNode->AddProperty("RelativePosition")->SetVector3(ZE3dsMaxUtils::MaxtoZE(Node->GetWorldTM().Translation() * Node->GetWorldTM().Scaling()));
		BoneNode->AddProperty("RelativeRotation")->SetQuaternion(ZE3dsMaxUtils::MaxtoZE(Node->GetWorldTM().Rotation()));
		BoneNode->AddProperty("RelativeScale")->SetVector3(ZE3dsMaxUtils::MaxtoZE(Node->GetWorldTM().Scaling()));
	}

	MSTR UserDefinedPropertiesBuffer;
	Node->GetMaxNode()->GetUserPropBuffer(UserDefinedPropertiesBuffer);

	if (!UserDefinedPropertiesBuffer.isNull())
		BoneNode->AddProperty("UserDefinedProperties")->SetString(ZEString(UserDefinedPropertiesBuffer.data()));


	// Process Physical Properties
	ProcessPhysicalJoint(Node, BoneNode->AddNode("PhysicalJoint"));

	bool IsBonePhysicalBodyExportEnabled = ((ZEMLProperty*)(ExportOptions->GetProperty("IsBonePhysicalBodyExportEnabled")))->GetValue().GetBoolean();

	if (IsBonePhysicalBodyExportEnabled)
		ProcessPhysicalBody(Node, BoneNode);

	zeLog("Bone \"%s\" is processed.", ZEString(Node->GetName()).ToCString());
	ZEProgressDialog::GetInstance()->CloseTask();

	return true;
}

bool ZE3dsMaxModelExporter::ProcessBones()
{
	zeLog("Processing bones...");

	Tab<IGameNode*> Nodes = Scene->GetIGameNodeByType(IGameObject::IGAME_BONE);
	ZEString Type;

	for (ZESize I = 0; I < (ZESize)Nodes.Count(); I++)
	{
		Type.Clear();
		if (ZE3dsMaxUtils::GetProperty(Nodes[I]->GetIGameObject(), ZE_STRING_PROP, ZEString("ZEType"), Type) && Type.Equals("Bone"))
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
		bool IsBoneExportEnabled = ((ZEMLProperty*)(ExportOptions->GetProperty("IsBoneExportEnabled")))->GetValue().GetBoolean();

		if (IsBoneExportEnabled)
			zeWarning("No bones with ZEBoneAttribute found. Bone processing aborted.");
		else
			zeLog("Since bone export is not enabled. Bone export process is skipped.");

		return true;
	}

	ZEMLNode* BonesNode = ModelNode.AddNode("Bones");

	for (ZESize I = 0; I < (ZESize)ProcessedBones.Count(); I++)
		if (!ProcessBone(ProcessedBones[I], BonesNode))
			return false;

	return true;
}

bool ZE3dsMaxModelExporter::ProcessMeshLODVertices(IGameNode* Node, ZEMLNode* LODNode)
{
	IGameMesh* Mesh = (IGameMesh*)Node->GetIGameObject();

	if (!Mesh->InitializeData())
		zeError("Can not initialize mesh data for mesh.");

	IGameSkin* Skin = Mesh->GetIGameSkin();

	ZEArray<ZEModelFileVertex> Vertices;
	ZEArray<ZEModelFileSkinnedVertex> SkinnedVertices;
	ZEArray<ZEUInt32> AffectingBoneIds;

	LODNode->AddProperty("MaterialId")->SetInt32(ProcessMeshMaterial(Node->GetNodeMaterial()));

	if (Mesh->GetNumberOfFaces() == 0)
		zeError("Mesh face count is : 0.");

	if (!Mesh->IsObjectSkinned())
		Vertices.SetCount((ZESize)Mesh->GetNumberOfFaces() * 3);
	else
		SkinnedVertices.SetCount((ZESize)Mesh->GetNumberOfFaces() * 3);

	zeLog("Processing vertices of mesh \"%s\". Polygon Count : %d, Vertex Count : %d.", ZEString(Node->GetName()).ToCString(), Mesh->GetNumberOfFaces(), Mesh->GetNumberOfFaces() * 3);

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

	ZEMatrix4x4 InvWorldTM = WorldTM.Inverse();

	// BIG INFO
	// Transform Order: WorldParentNodeTM -> LocalNodeTM -> PivotTM -> Vertex
	// Zinek cannot represent pivots. Therefore pivots must be eliminated. Pivot transform must be applied to vertices.
	// Vertex Transform does this. It applies ObjectTransform to vertices to assimilate Pivot into Node Transform.

	ZEMatrix4x4 VertexTransform = InvWorldTM * ObjectTM;
	ZEMatrix4x4 TangentTransform = InvWorldTM;

	bool GotError = false;
	bool BoneCountWarning = false; 
	AffectingBoneIds.SetCount(0);
	for (ZESize I = 0; I < (ZESize)Mesh->GetNumberOfFaces(); I++)
	{
		FaceEx* Face;
		Face = Mesh->GetFace((ZEInt32)I);
		if (!Mesh->IsObjectSkinned())
		{
			for (ZESize N = 0; N < 3; N++)
			{	
				ZEModelFileVertex* Vertex = &(Vertices[3*I + N]);

				Point3 Temp;

				if (!Mesh->GetVertex(Face->vert[N], Temp, true))
					zeError("Can not get vertex of face %d, vertex index : %d.", I, N);

				Vertex->Position = VertexTransform * ZE3dsMaxUtils::MaxtoZE(Temp);

				if (!Mesh->GetNormal(Face->norm[N], Temp, true))
					zeError("Can not get normal of face %d, normal index : %d.", I, N);
				
				ZEMatrix4x4::Transform3x3(Vertex->Normal, VertexTransform, ZE3dsMaxUtils::MaxtoZE(Temp));

				ZEInt32 BinormalTangentIndex = Mesh->GetFaceVertexTangentBinormal((ZEInt32)I, (ZEInt32)N);

				if (!Mesh->GetTangent(BinormalTangentIndex, Temp))
					zeError("Can not get tangent of face %d, tangent index : %d.", I, BinormalTangentIndex);

				ZEMatrix4x4::Transform3x3(Vertex->Tangent, TangentTransform, ZE3dsMaxUtils::MaxtoZE(Temp.Normalize()));

				if (!Mesh->GetBinormal(BinormalTangentIndex, Temp))
					zeError("Can not get binormal of face %d, binormal index : %d.", I, BinormalTangentIndex);

				ZEVector3 InverseBinormal = -(ZE3dsMaxUtils::MaxtoZE(Temp));
				ZEMatrix4x4::Transform3x3(Vertex->Binormal, TangentTransform, InverseBinormal);

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

				Vertex->Position = VertexTransform * ZE3dsMaxUtils::MaxtoZE(Temp);

				if (!Mesh->GetNormal(Face->norm[N], Temp, true))
					zeError("Can not get normal of face %d, normal index : %d.", I, N);

				ZEMatrix4x4::Transform3x3(Vertex->Normal, VertexTransform, ZE3dsMaxUtils::MaxtoZE(Temp));

				ZEInt32 BinormalTangentIndex = Mesh->GetFaceVertexTangentBinormal((ZEInt32)I, (ZEInt32)N);

				if (!Mesh->GetTangent(BinormalTangentIndex, Temp))
					zeError("Can not get tangent of face %d, tangent index : %d.", I, BinormalTangentIndex);

				ZEMatrix4x4::Transform3x3(Vertex->Tangent, TangentTransform, ZE3dsMaxUtils::MaxtoZE(Temp.Normalize()));

				if (!Mesh->GetBinormal(BinormalTangentIndex, Temp))
					zeError("Can not get texture coordinate of face %d vertex %d.", I, N, BinormalTangentIndex);

				ZEVector3 InverseBinormal = -(ZE3dsMaxUtils::MaxtoZE(Temp));
				ZEMatrix4x4::Transform3x3(Vertex->Binormal, TangentTransform, InverseBinormal);

				if (!Mesh->GetTexVertex(Face->texCoord[N], *(Point2*)&Vertex->Texcoord))
					zeError("Can not get texture coordinate of face %d vertex %d.", I, N, BinormalTangentIndex);


				ZEInt32 BoneCount = Skin->GetNumberOfBones(Face->vert[N]);
				if (BoneCount > 4 && !BoneCountWarning)
				{
					zeWarning("Vertex can be affected maximum 4 bones. Exporter is goint to try reducing affecting bones by removing bones with small weights.\r\n"
						"(Treshold: Weights < 0.001) Node Name : \"%s\".", ZEString(Node->GetName()).ToCString()); 
					BoneCountWarning = true;
				}
				ZeroMemory(Vertex->BoneIndices, sizeof(unsigned char) * 4);
				ZeroMemory(Vertex->BoneWeights, sizeof(float) * 4);

				ZESize BoneIndex = 0;
				for (ZESize M = 0; M < (ZESize)BoneCount; M++)
				{
					ZEInt32 BoneId = GetBoneId(Skin->GetIGameBone((ZEInt32)Face->vert[N], (ZEInt32)M));

					if (BoneId == -1)
					{
						zeError("Bone that is used for skinning does not have ZEBone attributes. Node Name : \"%s\". 3ds Max Vertex Id : %d. Bone Name : %s.", ZEString(Node->GetName()).ToCString(), Face->vert[N],  ZEString(Skin->GetIGameBone((ZEInt32)Face->vert[N], (ZEInt32)M)->GetName()));
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
							zeWarning("Affecting bone count exceeds 57. Hardware accelerated skinning may not be posible. Node Name : \"%s\", Bone Count : %d.", ZEString(Node->GetName()).ToCString(), AffectingBoneIds.GetCount());
					}

					float BoneWeight = Skin->GetWeight(Face->vert[N], (ZEInt32)M);
					if (BoneWeight > 0.001f)
					{
						if (BoneIndex >= 4)
						{
							zeError("Can not reduce bone count. Node Name : \"%s\". 3ds Max Vertex Id : %d.", ZEString(Node->GetName()).ToCString(), Face->vert[N]);
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
		LODNode->AddData("Vertices")->SetData(SkinnedVertices.GetCArray(), SkinnedVertices.GetCount() * sizeof(ZEModelFileSkinnedVertex), false);
		LODNode->AddData("AffectingBoneIds")->SetData(AffectingBoneIds.GetCArray(), AffectingBoneIds.GetCount() * sizeof(ZEUInt32), false);
	}
	else
		LODNode->AddData("Vertices")->SetData(Vertices.GetCArray(), Vertices.GetCount() * sizeof(ZEModelFileVertex), false);

	return !GotError;
}

bool ZE3dsMaxModelExporter::ProcessMasterMesh(IGameNode* Node, ZEMLNode* MeshesNode)
{
	zeLog("Processing mesh \"%s\".", ZEString(Node->GetName()).ToCString());
	IGameMesh* Mesh = (IGameMesh*)Node->GetIGameObject();

	// Process Attributes
	ZEString Type;

	if (!ZE3dsMaxUtils::GetProperty(Mesh, ZE_STRING_PROP, ZEString("ZEType"), Type) || (!Type.Equals("Mesh")))
	{
		zeError("Mesh is rejected because it does not contain ZEMeshAttributes. Node Name : \"%s\"", ZEString(Node->GetName()).ToCString());
		return false;
	}

	ZEMLNode* CurrentMeshNode = MeshesNode->AddNode("Mesh");
	CurrentMeshNode->AddProperty("Name")->SetString(ZEString(Node->GetName()));
	zeLog("Mesh \"%s\" is created as base mesh.", ZEString(Node->GetName()).ToCString());

	zeLog("Mesh is a master LOD. Setting mesh properties by using this mesh.");
	ProcessedMasterMeshes.Append(1, &Node);
	
	if (!Mesh->InitializeData())
		zeError("Can not initialize mesh data.");

	ZEInt32 ParentMeshId = -1;
	if (Node->GetNodeParent() != NULL)
	{
		IGameNode* CurrentNode = Node;
		while(CurrentNode->GetNodeParent() != NULL)
			if (GetMeshId(CurrentNode->GetNodeParent()) == -1)
			{
				CurrentNode = CurrentNode->GetNodeParent();
				continue;
			}
			else
			{
				ParentMeshId = GetMeshId(CurrentNode->GetNodeParent());
				break;
			}
	}

	CurrentMeshNode->AddProperty("ParentMesh")->SetInt32(ParentMeshId);

	ZEAABBox BoundingBox;
	ZE3dsMaxUtils::CalculateLocalBoundingBox(BoundingBox, Mesh);
	ZEMLNode* BBoxNode = CurrentMeshNode->AddNode("BoundingBox");
	BBoxNode->AddProperty("Min")->SetVector3(BoundingBox.Min);
	BBoxNode->AddProperty("Max")->SetVector3(BoundingBox.Max);

	ZE3dsMaxUtils::GetProperty(Mesh,  ZE_BOOL_PROP, ZEString("Mesh_Visibility"), *CurrentMeshNode->AddProperty("IsVisible"));

	CurrentMeshNode->AddProperty("IsSkinned")->SetBool(Mesh->IsObjectSkinned());

	if (ParentMeshId != -1)
	{
		GMatrix Transform = Node->GetWorldTM() * ProcessedMasterMeshes[(ZESize)ParentMeshId]->GetWorldTM().Inverse();
		CurrentMeshNode->AddProperty("Position")->SetVector3(ZE3dsMaxUtils::MaxtoZE(Transform.Translation()));
		CurrentMeshNode->AddProperty("Rotation")->SetQuaternion(ZE3dsMaxUtils::MaxtoZE(Transform.Rotation()));
		CurrentMeshNode->AddProperty("Scale")->SetVector3(ZE3dsMaxUtils::MaxtoZE(Transform.Scaling()));
	}
	else
	{ 
		//zeLog("\"%s\" is the Root Bone", Node->GetName());
		CurrentMeshNode->AddProperty("Position")->SetVector3(ZE3dsMaxUtils::MaxtoZE(Node->GetWorldTM().Translation()));
		CurrentMeshNode->AddProperty("Rotation")->SetQuaternion(ZE3dsMaxUtils::MaxtoZE(Node->GetWorldTM().Rotation()));
		CurrentMeshNode->AddProperty("Scale")->SetVector3(ZE3dsMaxUtils::MaxtoZE(Node->GetWorldTM().Scaling()));
	}

	MSTR UserDefinedPropertiesBuffer;
	Node->GetMaxNode()->GetUserPropBuffer(UserDefinedPropertiesBuffer);

	if (!UserDefinedPropertiesBuffer.isNull())
		CurrentMeshNode->AddProperty("UserDefinedProperties")->SetString(ZEString(UserDefinedPropertiesBuffer.data()));

	ZEMLNode* MainLODsNode = CurrentMeshNode->AddNode("LODs");
	ZEMLNode* LODNode = MainLODsNode->AddNode("LOD");

	ZEInt32 MeshLOD;
	ZE3dsMaxUtils::GetProperty(Mesh, ZE_INT_PROP, ZEString("Mesh_LOD"), MeshLOD);

	ZEInt32 CurrentMeshLODLevelStartDistance;
	ZE3dsMaxUtils::GetProperty(Mesh, ZE_INT_PROP, ZEString("Mesh_LOD_Start_Distance"), CurrentMeshLODLevelStartDistance);

	ZEInt32 CurrentMeshLODLevelEndDistance;
	ZE3dsMaxUtils::GetProperty(Mesh, ZE_INT_PROP, ZEString("Mesh_LOD_End_Distance"), CurrentMeshLODLevelEndDistance);

	LODNode->AddProperty("LODLevel")->SetInt32(MeshLOD);
	LODNode->AddProperty("LODStartDistance")->SetInt32(CurrentMeshLODLevelStartDistance);
	LODNode->AddProperty("LODEndDistance")->SetInt32(CurrentMeshLODLevelEndDistance);

	if (!ProcessMeshLODVertices(Node, LODNode))
		return false;

	bool IsMeshPhysicalBodyExportEnabled = ((ZEMLProperty*)(ExportOptions->GetProperty("IsMeshPhysicalBodyExportEnabled")))->GetValue().GetBoolean();

	if (IsMeshPhysicalBodyExportEnabled)
		ProcessPhysicalBody(Node, CurrentMeshNode);

	ProcessedMeshes.Append(1, &Node);

	zeLog("Mesh \"%s\" is processed.", ZEString(Node->GetName()).ToCString());
	return true;
}

bool ZE3dsMaxModelExporter::ProcessMeshLODs(IGameNode* Node, ZEMLNode* MeshesNode)
{
	IGameMesh* Mesh = (IGameMesh*)Node->GetIGameObject();
	ZEString LODName(Node->GetName());

	ZEString Type;

	if (!ZE3dsMaxUtils::GetProperty(Mesh, ZE_STRING_PROP, ZEString("ZEType"), Type) || (!Type.Equals("Mesh")))
	{
		zeError("Mesh is rejected because it does not contain ZEMeshAttributes. Node Name : \"%s\"", ZEString(Node->GetName()).ToCString());
		return false;
	}
	
	ZEInt32 CurrentMeshLODLevel;
	ZE3dsMaxUtils::GetProperty(Mesh, ZE_INT_PROP, ZEString("Mesh_LOD"), CurrentMeshLODLevel);

	ZEInt32 CurrentMeshLODLevelStartDistance;
	ZE3dsMaxUtils::GetProperty(Mesh, ZE_INT_PROP, ZEString("Mesh_LOD_Start_Distance"), CurrentMeshLODLevelStartDistance);

	ZEInt32 CurrentMeshLODLevelEndDistance;
	ZE3dsMaxUtils::GetProperty(Mesh, ZE_INT_PROP, ZEString("Mesh_LOD_End_Distance"), CurrentMeshLODLevelEndDistance);

	ZEArray<ZEMLNode*> Meshes = MeshesNode->GetNodes("Mesh");
	ZEMLNode* MasterMesh = NULL;

	for (ZESize I = 0; I < Meshes.GetCount(); I++)
	{
		if (((ZEMLProperty*)Meshes[I]->GetProperty("Name"))->GetValue().GetString() == LODName)
			MasterMesh = Meshes[I];
	}

	ZEMLNode* MainLODsNode = MasterMesh->GetNodes("LODs").GetFirstItem();

	ZESize MainLODsNodeCount = MainLODsNode->GetNodes().GetCount();
	ZEArray<ZEMLElement*> TempLODLevel;
	ZEMLNode* LODNode = NULL;

	for (ZESize I = 0; I < MainLODsNodeCount; I++)
	{
		TempLODLevel.Clear();
		TempLODLevel = (MainLODsNode->GetNodes()[I])->GetElements(ZEML_ET_PROPERTY);
		ZEInt32 MeshLODLevel = ((ZEMLProperty*)TempLODLevel.GetFirstItem())->GetValue().GetInt32();

		if (I + 1 < MainLODsNodeCount)
		{
			TempLODLevel = (MainLODsNode->GetNodes()[I + 1])->GetElements(ZEML_ET_PROPERTY);
			ZEInt32 NextMeshLODLevel = ((ZEMLProperty*)TempLODLevel.GetFirstItem())->GetValue().GetInt32();

			if (MeshLODLevel < CurrentMeshLODLevel && CurrentMeshLODLevel < NextMeshLODLevel)
			{
				LODNode = MainLODsNode->AddNode("LOD");
				break;
			}
		}
		else
		{
			if (MeshLODLevel < CurrentMeshLODLevel)
			{
				LODNode = MainLODsNode->AddNode("LOD");
				break;
			}
			else
			{
				//Error master lod error
				return false;
			}
		}
	}

	LODNode->AddProperty("LODLevel")->SetInt32(CurrentMeshLODLevel);
	LODNode->AddProperty("LODStartDistance")->SetInt32(CurrentMeshLODLevelStartDistance);
	LODNode->AddProperty("LODEndDistance")->SetInt32(CurrentMeshLODLevelEndDistance);

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
	ZEString Type;
	ZEString NodeName;
	ZEString MasterMeshName;

	for (ZESize I = 0; I < (ZESize)Nodes.Count(); I++)
	{
		if (GetBoneId(Nodes[I]) == -1)
		{
			MeshExists = false;

			Type.Clear();
			NodeName.Clear();

			IGameMesh* Mesh = (IGameMesh*)Nodes[I]->GetIGameObject();
			if (!ZE3dsMaxUtils::GetProperty(Mesh, ZE_STRING_PROP, ZEString("ZEType"), Type) || (!Type.Equals("Mesh")))
				continue;

			NodeName.SetValue(Nodes[I]->GetName());

			for (ZESize J = 0; J < MasterMeshes.GetCount(); J++)
			{
				MasterMeshName.SetValue(MasterMeshes[J]->GetName());

				if (strncmp(MasterMeshName.ToCString(), NodeName.ToCString(), ZE_EXFL_MAX_NAME_SIZE) == 0)
				{
					if (MasterMeshes[J]->GetNodeID() == Nodes[I]->GetNodeID())
					{
						// This occurs due to hierarchical links of max nodes. 3ds Max makes a copy of linked object.
						MeshExists = true;
						break;
					}

					MeshExists = true;
					ZEInt32 MasterMeshLOD;
					ZEInt32 CurrentMeshLOD;

					IGameMesh* MasterMesh = (IGameMesh*)MasterMeshes[J]->GetIGameObject();
					ZE3dsMaxUtils::GetProperty(MasterMesh, ZE_INT_PROP, ZEString("Mesh_LOD"), MasterMeshLOD);

					IGameMesh* CurrentMesh = (IGameMesh*)Nodes[I]->GetIGameObject();
					ZE3dsMaxUtils::GetProperty(CurrentMesh, ZE_INT_PROP, ZEString("Mesh_LOD"), CurrentMeshLOD);


					if (MasterMeshLOD == CurrentMeshLOD)
					{
						zeError("Mesh has two LOD with the same LOD Level. (Mesh Name : \"%s\", LodLevel : %d)", NodeName.ToCString(), MasterMeshLOD);
						return false;
					}

					if (CurrentMeshLOD < MasterMeshLOD)
					{
						MeshLODs.Add(MasterMeshes[J]);
						MasterMeshes[J] = Nodes[I];
					}

					if (CurrentMeshLOD > MasterMeshLOD)
						MeshLODs.Add(Nodes[I]);

					break;
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
		bool IsMeshExportEnabled = ((ZEMLProperty*)(ExportOptions->GetProperty("IsMeshExportEnabled")))->GetValue().GetBoolean();

		if (IsMeshExportEnabled)
			zeWarning("No meshes with ZEMeshAttribute found. Mesh processing aborted.");
		else
			zeLog("Since mesh export is not enabled. Mesh export process is skipped.");

		return true;
	}

	ZEMLNode* MeshesNode = ModelNode.AddNode("Meshes");

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

	if (AnimationStartFrame + AnimationFrameCount - 1 < 0.0f)
		zeError("Specified animation frame range is not valid.");

	Tab<IGameNode*>	CurrentAnimationAffectedBones;
	Tab<IGameNode*>	CurrentAnimationAffectedMeshes;
	ZEInt32 ActualAnimationFrame = (ZEInt32)AnimationStartFrame;

	GMatrix Matrix;

	for (ZESize I = 0; I < (ZESize)ProcessedMasterMeshes.Count(); I++)
	{
		IGameControl* MeshAnimationController = ProcessedMasterMeshes[I]->GetIGameControl();

		bool Result = ProcessedMasterMeshes[I]->GetNodeParent() == NULL;

		for (ZEInt32 J = 0; J < 13; J++)
		{
			if (MeshAnimationController->IsAnimated((IGameControlType)J))
			{
				CurrentAnimationAffectedMeshes.Append(1, &ProcessedMasterMeshes[I]);
				break;
			}
		}
	}

	for (ZESize I = 0; I < (ZESize)ProcessedBones.Count(); I++)
	{
		IGameControl* BoneAnimationController = ProcessedBones[I]->GetIGameControl();

		for (ZEInt32 J = 0; J < 13; J++)
		{
			if (BoneAnimationController->IsAnimated((IGameControlType)J))
			{
				CurrentAnimationAffectedBones.Append(1, &ProcessedBones[I]);
				break;
			}

		}
	}

	ZESize MeshKeyCount = (ZESize)CurrentAnimationAffectedMeshes.Count();
	ZESize BoneKeyCount = (ZESize)CurrentAnimationAffectedBones.Count();
	ZESize FrameKeyCount = MeshKeyCount + BoneKeyCount;

	AnimationNode->AddProperty("BoneKeyCount")->SetUInt32((ZEUInt32)BoneKeyCount);
	AnimationNode->AddProperty("MeshKeyCount")->SetUInt32((ZEUInt32)MeshKeyCount);

	ZEArray<ZEModelFileAnimationKey> Frames;
	Frames.SetCount((MeshKeyCount + BoneKeyCount) * AnimationFrameCount);
	ActualAnimationFrame = (ZEInt32)AnimationStartFrame;

	zeLog("Frame Count : %d", AnimationFrameCount);
	zeLog("Processing Animation Frames: %d - %d", AnimationStartFrame, AnimationStartFrame + AnimationFrameCount - 1);

	for (ZESize I = 0; I < (ZESize)CurrentAnimationAffectedBones.Count(); I++)
	{
		IGameControl* CurrentBoneAnimationController = CurrentAnimationAffectedBones[I]->GetIGameControl();
		IGameKeyTab CurrentBoneTotalAnimationSampleKeys;
		CurrentBoneAnimationController->GetFullSampledKeys(CurrentBoneTotalAnimationSampleKeys, 1, IGAME_TM);

		ActualAnimationFrame = (ZEInt32)AnimationStartFrame;

		for (ZESize N = 0; N < AnimationFrameCount; N++)
		{
			ZEModelFileAnimationKey* Key = &Frames[I + (FrameKeyCount * N)];
			Key->ItemId = GetBoneId(CurrentAnimationAffectedBones[I]);

			GMatrix Matrix = CurrentBoneTotalAnimationSampleKeys[ActualAnimationFrame].sampleKey.gval;
			Key->Position = ZE3dsMaxUtils::MaxtoZE(Matrix.Translation());
			Key->Rotation = ZE3dsMaxUtils::MaxtoZE(Matrix.Rotation());
			Key->Scale = ZE3dsMaxUtils::MaxtoZE(Matrix.Scaling());

			ActualAnimationFrame++;
		}
	}

	for (ZESize I = 0; I < (ZESize)CurrentAnimationAffectedMeshes.Count(); I++)
	{
		IGameControl* CurrentMeshAnimationController = CurrentAnimationAffectedMeshes[I]->GetIGameControl();
		IGameKeyTab CurrentMeshTotalAnimationSampleKeys;
		CurrentMeshAnimationController->GetFullSampledKeys(CurrentMeshTotalAnimationSampleKeys, 1, IGAME_TM);

		ActualAnimationFrame = (ZEInt32)AnimationStartFrame;

		for (ZESize N = 0; N < AnimationFrameCount; N++)
		{
			ZEModelFileAnimationKey* Key = &Frames[I + BoneKeyCount + (FrameKeyCount * N)];
			Key->ItemId = GetMeshId(CurrentAnimationAffectedMeshes[I]);

			GMatrix Matrix = CurrentMeshTotalAnimationSampleKeys[ActualAnimationFrame].sampleKey.gval;
			Key->Position = ZE3dsMaxUtils::MaxtoZE(Matrix.Translation());
			Key->Rotation = ZE3dsMaxUtils::MaxtoZE(Matrix.Rotation());
			Key->Scale = ZE3dsMaxUtils::MaxtoZE(Matrix.Scaling());

			ActualAnimationFrame++;
		}

	}

	AnimationNode->AddData("Frames")->SetData(Frames.GetCArray(), Frames.GetCount() * sizeof(ZEModelFileAnimationKey), false);

}

bool ZE3dsMaxModelExporter::ProcessAnimations(ZEMLNode* AnimationsNode)
{
	zeLog("Processing Animations...");

	if (ExportOptions->GetNodes("Animations").GetCount() > 0)
	{
		ZEMLNode* ExportAnimationsNode = ExportOptions->GetNodes("Animations").GetFirstItem();
		ZESize AnimationCount = ExportAnimationsNode->GetNodes("Animation").GetCount();
		ZEString AnimationName;

		for (ZESize I = 0; I < AnimationCount; I++)
		{
			ZEMLNode* AnimationNode = AnimationsNode->AddNode("Animation");
			ZEMLNode* ExportAnimationNode = ExportAnimationsNode->GetNodes("Animation").GetItem(I);
			AnimationName = ((ZEMLProperty*)ExportAnimationNode->GetProperty("Name"))->GetValue().GetString();
			AnimationNode->AddProperty("Name")->SetString(AnimationName);

			ZEInt32 CurrentAnimationStartFrame = ((ZEMLProperty*)ExportAnimationNode->GetProperty("StartFrame"))->GetValue().GetString().ToInt32();
			ZEInt32 CurrentAnimationEndFrame = ((ZEMLProperty*)ExportAnimationNode->GetProperty("EndFrame"))->GetValue().GetString().ToInt32();
			ZESize CurrentAnimationFrameCount = ZESize(CurrentAnimationEndFrame - CurrentAnimationStartFrame + 1);

			ZEProgressDialog::GetInstance()->OpenTask(AnimationName);
			zeLog("Processing animation \"%s\".", AnimationName.ToCString());
			ProcessAnimationFrames((ZESize)CurrentAnimationStartFrame, CurrentAnimationFrameCount, AnimationNode);
			zeLog("Animation \"%s\" is processed.", AnimationName.ToCString());
			ZEProgressDialog::GetInstance()->CloseTask();
		}
	}
	else
	{
		ZEMLNode* AnimationNode = AnimationsNode->AddNode("Animation");
		AnimationNode->AddProperty("Name")->SetString("Default");

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
	ZEString NodeName(Node->GetName());
	ZEProgressDialog::GetInstance()->OpenTask(NodeName);
	zeLog("Processing helper \"%s\".", NodeName.ToCString());
	ZEMLNode* HelperNode = HelpersNode->AddNode("Helper");

	IGameObject* Helper = Node->GetIGameObject();
	HelperNode->AddProperty("Name")->SetString(NodeName);

	INode* OwnerNode = NULL;
	ZEInt32 OwnerId;
	ZEModelHelperOwnerType OwnerType;
	
	if (!ZE3dsMaxUtils::GetProperty(Helper, ZEString("Owner"), OwnerNode))
		zeError("Can not find helper property: Owner");

	IGameNode* OwnerGameNode = Scene->GetIGameNode(OwnerNode);

	if (OwnerNode == NULL)
	{
		zeWarning("Helper \"%s\" has no immediate owner parameter. Model will be set as owner.", NodeName.ToCString());

		OwnerId = -1;
		OwnerType = ZEModelHelperOwnerType::ZE_MHOT_MODEL;
	}
	else
	{
		ZEString Type;
		bool CurrentExportOption;

		ZE3dsMaxUtils::GetProperty(OwnerGameNode->GetIGameObject(), ZE_STRING_PROP, ZEString("ZEType"), Type);

		if (Type.Equals("Mesh"))
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

				zeWarning("Since mesh export option is disabled, Helper \"%s\" will be exported without an owner.", NodeName.ToCString());
			}
		}
		else if (Type.Equals("Bone"))
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

				zeWarning("Since bone export option is disabled, Helper \"%s\" will be exported without an owner.", NodeName.ToCString());
			}

		}
		else
		{
			zeError("Helper \"%s\" has invalid owner parameter.", NodeName.ToCString());
			return false;
		}
	}

	if (OwnerId < 0 && OwnerType != ZE_MHOT_MODEL)
	{
		zeError("Helper \"%s\" has invalid owner parameter.", NodeName.ToCString());
		return false;
	}

	HelperNode->AddProperty("OwnerType")->SetInt32((ZEInt32)OwnerType);
	HelperNode->AddProperty("OwnerId")->SetInt32(OwnerId);

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

	HelperNode->AddProperty("Position")->SetVector3(FinalTransform.GetTranslation());
	HelperNode->AddProperty("Rotation")->SetQuaternion(FinalTransform.GetRotation());
	HelperNode->AddProperty("Scale")->SetVector3(FinalTransform.GetScale());

	MSTR UserDefinedPropertiesBuffer;
	Node->GetMaxNode()->GetUserPropBuffer(UserDefinedPropertiesBuffer);

	if (!UserDefinedPropertiesBuffer.isNull())
		HelperNode->AddProperty("UserDefinedProperties")->SetString(ZEString(UserDefinedPropertiesBuffer.data()));

	zeLog("Helper \"%s\" is processed.", NodeName.ToCString());
	ZEProgressDialog::GetInstance()->CloseTask();

	return true;
}

bool ZE3dsMaxModelExporter::ProcessHelpers()
{
	zeLog("Processing helpers...");

	Tab<IGameNode*> Nodes = Scene->GetIGameNodeByType(IGameObject::IGAME_HELPER);
	ZEString Type;

	for (ZESize I = 0; I < (ZESize)Nodes.Count(); I++)
	{
		Type.Clear();
		if (ZE3dsMaxUtils::GetProperty(Nodes[I]->GetIGameObject(), ZE_STRING_PROP, ZEString("ZEType"), Type) && Type.Equals("Helper"))
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

	ZEMLNode* HelpersNode = ModelNode.AddNode("Helpers");

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
	ZEString NodeZEType;

	for (ZESize I = 0; I < (ZESize)Nodes.Count(); I++)
	{
		IGameNode* CurrentNode = Nodes[I];
		IGameObject* CurrentObject = CurrentNode->GetIGameObject();
		NodeZEType.Clear();

		if (!ZE3dsMaxUtils::GetProperty(CurrentObject, ZE_STRING_PROP, ZEString("ZEType"), NodeZEType))
			continue;

		if (NodeZEType.Equals("Mesh"))
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
			zeError("Mesh \"%s\" does not have valid material. Can not collect resource.", ZEString(CurrentNode->GetName()).ToCString());

		bool IsFound = false;

		for(ZESize J = 0; J < (ZESize)ResourceMaterials.Count(); J++)
			if (ResourceMaterials[J] == CurrentMaterial)
				IsFound = true;

		if (!IsFound)
			ResourceMaterials.Append(1, &CurrentMaterial);
	}

	for (ZESize I = 0; I < (ZESize)ResourceMaterials.Count(); I++)
	{
		IGameMaterial* NodeMaterial = ResourceMaterials[I];
		ZEString MaterialName = NodeMaterial->GetMaterialName();

		ResourceConfiguratorDialog->AddResource(MaterialName + ".ZEMaterial", "Material", ZEString(), ZE_ROAA_COPY_OVERWRITE);

		ZEInt32 NumberOfMaps = NodeMaterial->GetNumberOfTextureMaps();

		for (ZEInt32 N = 0; N < NumberOfMaps; N++)
		{
			IGameTextureMap* CurrentTexture = NodeMaterial->GetIGameTextureMap(N);
			switch(CurrentTexture->GetStdMapSlot())
			{
				case ID_AM: // Ambient
					break;

				case ID_DI: // Diffuse
					ResourceConfiguratorDialog->AddResource(ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName(), "Image", CurrentTexture->GetBitmapFileName(), ZEString());
					break;

				case ID_SP: // Specular
					ResourceConfiguratorDialog->AddResource(ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName(), "Image", CurrentTexture->GetBitmapFileName(), ZEString());
					break;

				case ID_SI:	// Emissive
					ResourceConfiguratorDialog->AddResource(ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName(), "Image", CurrentTexture->GetBitmapFileName(), ZEString());
					break;

				case ID_OP:	// Opacity 
					ResourceConfiguratorDialog->AddResource(ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName(), "Image", CurrentTexture->GetBitmapFileName(), ZEString());
					break;

				case ID_FI:	// Filter color 
					break;

				case ID_BU: // Bump 
					ResourceConfiguratorDialog->AddResource(ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName(), "Image", CurrentTexture->GetBitmapFileName(), ZEString());
					break;

				case ID_RL: // Reflection - Environment
					ResourceConfiguratorDialog->AddResource(ZEFileInfo(ZEString(CurrentTexture->GetBitmapFileName())).GetFileName(), "Image", CurrentTexture->GetBitmapFileName(), ZEString());
					break;

				case ID_RR: // Refraction 
					break;
			}

		}
	}
}
