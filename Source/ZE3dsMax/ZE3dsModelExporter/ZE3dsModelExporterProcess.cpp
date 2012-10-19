//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZE3dsModelExporterProcess.cpp
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
#include "ZE3dsModelExporter.h"
#include "ZEToolComponents\ZEProgressDialog\ZEProgressDialog.h"
#include "ZEToolComponents\ZEResourceConfigurationWidget\ZEResourceConfigurationWidget.h"
#include "ZEMath\ZEAABBox.h"
#include "ZEML/ZEMLProperty.h"
#include "ZEFile/ZEFileInfo.h"
#include "ZEFile/ZEPathUtils.h"
#include "ZEFile/ZEFile.h"
#include "ZEML/ZEMLSerialWriter.h"
#include "ZEFile\ZEDirectoryInfo.h"
#include "ZEFile\ZEFileOperations.h"

// #include <IGame/IGameFx.h>
// #include <io.h>
// #include "IGame/IGameModifier.h"

#define ZE_MTMP_SKINTRANSFORM					1
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

enum ZEPropType //Do not change order
{
	ZE_UNKNOWN_PROP			= 0,
	ZE_FLOAT_PROP			= 1,
	ZE_VECTOR3_PROP			= 2,
	ZE_INT_PROP				= 3,
	ZE_STRING_PROP			= 4,
	ZE_VECTOR4_PROP			= 5,
	ZE_BOOL_PROP			= 6
};

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

struct ZEModelFileSkinnedVertex
{
	ZEVector3									Position;
	ZEVector3									Normal;
	ZEVector3									Tangent;
	ZEVector3									Binormal;
	ZEVector2									Texcoord;
	ZEUInt8										BoneIndices[4];
	float										BoneWeights[4];
};

struct ZEModelFileVertex
{
	ZEVector3									Position;
	ZEVector3									Normal;
	ZEVector3									Tangent;
	ZEVector3									Binormal;
	ZEVector2									Texcoord;
};

struct ZEModelFileAnimationKey
{
	ZEUInt32									ItemId;
	ZEVector3									Position;
	ZEQuaternion								Rotation;
	ZEVector3									Scale;
};

struct ZEModelFileAnimationFrame
{
	ZEArray<ZEModelFileAnimationKey>			BoneKeys;
	ZEArray<ZEModelFileAnimationKey>			MeshKeys;
};

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
	Quat Temp = Quaternion.Inverse();
	return ZEQuaternion(Temp.w, Temp.x, Temp.y, Temp.z);
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

IGameScene* Scene_;

template <typename T>
bool GetProperty(IExportEntity * Object, ZEPropType Type, const char* Property, T& Value)
{ 
	if (Object == NULL)
		return false;

	IGameProperty* Prop = Object->GetIPropertyContainer()->QueryProperty(Property);
	if (Prop != NULL)
	{
		if(Type == ZE_BOOL_PROP)
			Type = ZE_INT_PROP;

		if (Prop->GetType() == Type)
		{
			if (Prop->GetPropertyValue(Value))
				return true;
			else
			{
				zeWarning("Can not read property value. (Property Name : \"%s\")", Property);
				return false;
			}
		}
		else
		{
			zeWarning("Wrong property type. (Property Name : \"%s\", Expected Type : %d, Type : %d)", Property, Prop->GetType(), Type);
			return false;
		}
	}
	else
	{
		zeWarning("Can not find property. (Property Name : \"%s\")", Property);
		return false;
	}
}

bool GetProperty(IExportEntity* Object, const char* Property, IGameNode*& Value)
{ 
	if (Object == NULL)
		return false;

	IGameProperty* Prop = Object->GetIPropertyContainer()->QueryProperty(Property);

	if (Prop != NULL)
	{	
		IParamBlock2* ParamBlock = Prop->GetMaxParamBlock2();
		ZEInt ParamId = ParamBlock->IndextoID(Prop->GetParamBlockIndex());

		INode* Node =  ParamBlock->GetINode(ParamId);
		if (Node == NULL)
		{
			Value = NULL;
			return true;
		}

		Value = Scene_->GetIGameNode(Node);
		return true;
	}
	else
	{
		zeWarning("Can not find property. (Property Name : \"%s\")", Property);
		return false;
	}
}

template <>
bool GetProperty<bool>(IExportEntity* Object, ZEPropType Type, const char* Property, bool& Value)
{ 
	if (Object == NULL)
		return false;

	IGameProperty* Prop = Object->GetIPropertyContainer()->QueryProperty(Property);
	if (Prop != NULL)
	{
		if(Type == ZE_BOOL_PROP)
			Type = ZE_INT_PROP;

		if (Prop->GetType() == Type)
		{
			ZEInt Temp;
			if (Prop->GetPropertyValue(Temp))
			{
				Value = (Temp != 0);
				return true;
			}
			else
			{
				zeWarning("Can not read property value. (Property Name : \"%s\")", Property);
				return false;
			}
		}
		else
		{
			zeWarning("Wrong property type. (Property Name : \"%s\", Expected Type : %d, Type : %d)", Property, Prop->GetType(), Type);
			return false;
		}
	}
	else
	{
		zeWarning("Can not find property. (Property Name : \"%s\")", Property);
		return false;
	}
}

template <>
bool GetProperty<const char*>(IExportEntity * Object, ZEPropType Type, const char* Property, const char*& Value)
{ 
	if (Object == NULL)
		return false;

	if(Type == ZE_BOOL_PROP)
		Type = ZE_INT_PROP;

	IGameProperty* Prop = Object->GetIPropertyContainer()->QueryProperty(Property);
	if (Prop != NULL && Prop->GetType() == Type)
		return Prop->GetPropertyValue(Value);
	else
		return false;
}

template <>
bool GetProperty<ZEUInt>(IExportEntity* Object, ZEPropType Type, const char* Property, ZEUInt& Value)
{ 
	ZEInt Temp;
	if (Object == NULL)
		return false;

	if(Type == ZE_BOOL_PROP)
		Type = ZE_INT_PROP;

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

template <>
bool GetProperty<ZEMLProperty>(IExportEntity* Object, ZEPropType Type, const char* Property, ZEMLProperty& Value)
{
	if (Object == NULL)
		return false;

	bool IsPropertyBoolean = false;

	if(Type == ZE_BOOL_PROP)
	{
		Type = ZE_INT_PROP;
		IsPropertyBoolean = true;
	};

	IGameProperty* Prop = Object->GetIPropertyContainer()->QueryProperty(Property);
	if (Prop != NULL && Prop->GetType() == Type)
	{
		bool Result;
		ZEVariant TempValue;

		switch (Type)
		{
			case ZE_FLOAT_PROP:
			{
				float FloatValue;
				Result = Prop->GetPropertyValue(FloatValue);
				TempValue.SetFloat(FloatValue);
				break;
			}
			case ZE_INT_PROP:
			{
				ZEInt IntegerValue;
				Result = Prop->GetPropertyValue(IntegerValue);

				if (IsPropertyBoolean)
				{
					bool BooleanValue = (IntegerValue != 0);
					TempValue.SetBoolean(BooleanValue);
				}
				else
				{
					TempValue.SetInt32(IntegerValue);
				}
				
				break;
			}
			case ZE_VECTOR3_PROP:
			{
				ZEVector3 Vector3Value;
				Result = Prop->GetPropertyValue((Point3&)Vector3Value);
				TempValue.SetVector3(Vector3Value);
				break;
			}
			case ZE_VECTOR4_PROP:
			{
				ZEVector4 Vector4Value;
				Result = Prop->GetPropertyValue((Point4&)Vector4Value);
				TempValue.SetVector4(Vector4Value);
				break;
			}
			case ZE_STRING_PROP:
			{
				const char* StringValue;
				Result = Prop->GetPropertyValue(StringValue);
				TempValue.SetString(StringValue);
				break;
			}
		}

		if(Result)
		{
			Value.SetValue(TempValue);
			return true;
		}
		else
			return false;
	}
	else
		return false;
}


bool ZE3dsModelExporter::GetRelativePath(const char* RealPath, char* RelativePath)
{
	ZEString ZinekDir;

	if(ExportOptions != NULL)
		ZinekDir = ((ZEMLProperty*)(ExportOptions->GetProperty("ZinekEngineWorkingDirectory")))->GetValue().GetString();
	else
		zeError("Can not get Zinek Working Directory from options.");

	if (strnicmp(RealPath, ZinekDir.ToCString(), ZinekDir.GetLength()) == 0)
	{
		strcpy(RelativePath, RealPath + ZinekDir.GetLength() + 1);
		return true;
	}
	else
	{
		RelativePath[0] = '\0';
		return false;
	}

	return false;
}

ZEInt ZE3dsModelExporter::GetMeshId(IGameNode* Node)
{
	for (ZESize I = 0; I < (ZESize)ProcessedMasterMeshes.Count(); I++)
		if (ProcessedMasterMeshes[I]->GetNodeID() == Node->GetNodeID())
			return (ZEInt)I;
	return -1;
}

ZEInt ZE3dsModelExporter::GetBoneId(IGameNode* Node)
{
	for (ZESize I = 0; I < (ZESize)ProcessedBones.Count(); I++)
		if (ProcessedBones[I]->GetNodeID() == Node->GetNodeID())
			return (ZEInt)I;

	return -1;
}

void CalculateLocalBoundingBox(ZEAABBox& BoundingBox, IGameMesh* Mesh)
{
	if (Mesh->GetNumberOfFaces() == 0)
	{
		BoundingBox.Min = BoundingBox.Max = ZEVector3(0.0f, 0.0f, 0.0f);
		return;
	}
	else
		BoundingBox.Min = BoundingBox.Max = *(ZEVector3*)&Mesh->GetVertex(Mesh->GetFace(0)->vert[0], true);

	for (ZEInt I = 0; I < Mesh->GetNumberOfFaces(); I++)
	{
		FaceEx* Face = Mesh->GetFace(I);
		for (ZESize N = 0; N < 3; N++)
		{
			Point3 Point;
			Mesh->GetVertex(Face->vert[N], Point, true);
			if (Point.x < BoundingBox.Min.x) BoundingBox.Min.x = Point.x;
			if (Point.y < BoundingBox.Min.y) BoundingBox.Min.y = Point.y;
			if (Point.z < BoundingBox.Min.z) BoundingBox.Min.z = Point.z;
			if (Point.x > BoundingBox.Max.x) BoundingBox.Max.x = Point.x;
			if (Point.y > BoundingBox.Max.y) BoundingBox.Max.y = Point.y;
			if (Point.z > BoundingBox.Max.z) BoundingBox.Max.z = Point.z;
		}
	}

}

ZEInt ZE3dsModelExporter::ProcessMeshMaterial(IGameMaterial* Material)
{
	if (Material == NULL)
		return -1;

	for (ZESize I = 0; I < (ZESize)Materials.Count(); I++)
		if (Materials[I] == Material)
			return (ZEInt)I;
	
	Materials.Append(1, &Material);
	return Materials.Count() - 1;
}

bool ZE3dsModelExporter::ProcessMaterials(const char* FileName, ZEMLNode* MaterialsNode)
{
	zeLog("Processing materials...");

	ZEString FileParentPath = ZEFileInfo::GetParentDirectory(FileName) + ZEPathUtils::GetSeperator();

	char DiffuseMap[ZE_MDLF_MAX_FILENAME_SIZE];
	char SpecularMap[ZE_MDLF_MAX_FILENAME_SIZE];
	char EmissiveMap[ZE_MDLF_MAX_FILENAME_SIZE];
	char OpacityMap[ZE_MDLF_MAX_FILENAME_SIZE];
	char NormalMap[ZE_MDLF_MAX_FILENAME_SIZE];
	char EnvironmentMap[ZE_MDLF_MAX_FILENAME_SIZE];

	ZESize MaterialCount = (ZESize)Materials.Count();
	zeLog("Material count : %d", MaterialCount);

	for (ZESize I = 0; I < MaterialCount; I++)
	{
		IGameMaterial* NodeMaterial = Materials[I];
		ZEString MaterialName = NodeMaterial->GetMaterialName();

		ZEResourceOption MaterialOption;
		if(!ResourceConfigurationDialog->GetOption(MaterialName + ".ZEMaterial", MaterialOption))
			zeError("Material export path not found in resource options. Resource identifier : %s");

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
			char RelativePath[ZE_MDLF_MAX_FILENAME_SIZE];

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
					strncpy(DiffuseMap, ResourceRelativePath.ToCString() , ZE_MDLF_MAX_FILENAME_SIZE);

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
					strncpy(SpecularMap, ResourceRelativePath.ToCString(), ZE_MDLF_MAX_FILENAME_SIZE);

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
					strncpy(EmissiveMap, ResourceRelativePath.ToCString(), ZE_MDLF_MAX_FILENAME_SIZE);

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
					strncpy(OpacityMap, ResourceRelativePath.ToCString(), ZE_MDLF_MAX_FILENAME_SIZE);

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
				strncpy(CurrentMaterial->DetailMap, RelativePath, ZE_MDLF_MAX_FILENAME_SIZE);
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
					ResourceRelativePath = ResourceConfigurationDialog->GetResourceRelativePath(MaterialFilePath, ZEFileInfo::GetFileName(CurrentTexture->GetBitmapFileName()));
					strncpy(NormalMap, ResourceRelativePath.ToCString(), ZE_MDLF_MAX_FILENAME_SIZE);

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
					strncpy(EnvironmentMap, ResourceRelativePath.ToCString(), ZE_MDLF_MAX_FILENAME_SIZE);

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
				strncpy(CurrentMaterial->RefractionMap, RelativePath, ZE_MDLF_MAX_FILENAME_SIZE);*/
				break;
			}
			
		}

		ZEMLSerialRootNode MaterialSerialNode("Material", &MaterialFile);
		MaterialSerialNode.WriteProperty("Name", MaterialName);
		ZEMLSerialNode MaterialConfigNode = MaterialSerialNode.OpenNode("Configuration");

		MaterialConfigNode.WriteProperty("Name", "Default");
		bool TempBooleanValue = false;
		GetProperty(NodeMaterial, ZE_INT_PROP, "wire", TempBooleanValue);
		MaterialConfigNode.WriteProperty("Wireframe", TempBooleanValue);

		TempBooleanValue = false;
		GetProperty(NodeMaterial, ZE_INT_PROP, "twoSided", TempBooleanValue);
		MaterialConfigNode.WriteProperty("TwoSided", TempBooleanValue);

		MaterialConfigNode.WriteProperty("LightningEnabled", true); // Lightning Enabled is forced true;

		float Opacity = 0.0f;
		NodeMaterial->GetOpacityData()->GetPropertyValue(Opacity);

		if (Opacity != 1.0f || strncmp(OpacityMap, "", ZE_MDLF_MAX_FILENAME_SIZE) !=0)
			MaterialConfigNode.WriteProperty("Transparant", true);
		else
			MaterialConfigNode.WriteProperty("Transparant", false);

		//AMBIENT
		MaterialConfigNode.WriteProperty("AmbientEnabled", false); //Ambient is forced false
		ZEVector3 TempVector3Value = ZEVector3::Zero;
		NodeMaterial->GetAmbientData()->GetPropertyValue(*(Point3*)&TempVector3Value);
		MaterialConfigNode.WriteProperty("AmbientColor", TempVector3Value);

		//DIFFUSE
		MaterialConfigNode.WriteProperty("DiffuseEnabled", (MapFlag & ZE_MTMP_DIFFUSEMAP) != 0);
		TempVector3Value = ZEVector3::Zero;
		NodeMaterial->GetDiffuseData()->GetPropertyValue(*(Point3*)&TempVector3Value);
		MaterialConfigNode.WriteProperty("DiffuseColor", TempVector3Value);
		MaterialConfigNode.WriteProperty("BaseMap", DiffuseMap);

		//SPECULAR
		MaterialConfigNode.WriteProperty("SpecularEnabled", (MapFlag & ZE_MTMP_SPECULARMAP) != 0);
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

void ZE3dsModelExporter::ProcessPhysicalBodyConvexShape(IGameNode* Node, IGameNode* OwnerNode, ZEMLNode* ShapeNode)
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
		Vertices[I] = MAX_TO_ZE(Vertex);
	}

	ShapeNode->AddDataProperty("Vertices", Vertices.GetCArray(), sizeof(ZEVector3) * NumberofVertices, true);
}

bool ZE3dsModelExporter::ProcessPhysicalShape(IGameNode* Node, IGameNode* OwnerNode, ZEMLNode* PhysicalShapeNode)
{
	IGameObject* Object = Node->GetIGameObject();

	if (Object == NULL)
		return false;

	const char* ZEType;
	if (!GetProperty(Object, ZE_STRING_PROP, "ZEType", ZEType) || (strcmp(ZEType, "PhysicalShape") != 0))
	{
		zeError("Mesh is rejected because it does not contain ZEPhysicalShape. Node Name: \"%s\".", Node->GetName());
		return false;
	}

	GMatrix Transform = Node->GetWorldTM() * OwnerNode->GetWorldTM().Inverse();
	PhysicalShapeNode->AddProperty("Position", MAX_TO_ZE(Transform.Translation()));
	PhysicalShapeNode->AddProperty("Rotation", MAX_TO_ZE(Transform.Rotation()));

	ZEInt GeometryType;
	GetProperty(Object, ZE_FLOAT_PROP,	"PhysicalShape_MaterialRestitution",		*PhysicalShapeNode->AddProperty("Restitution"));
	GetProperty(Object, ZE_FLOAT_PROP,	"PhysicalShape_MaterialDynamicFriction",	*PhysicalShapeNode->AddProperty("DynamicFriction"));
	GetProperty(Object, ZE_FLOAT_PROP,	"PhysicalShape_MaterialStaticFriction",		*PhysicalShapeNode->AddProperty("StaticFriction"));
	GetProperty(Object, ZE_INT_PROP,		"PhysicalShape_GeometryType",				 GeometryType);

	if (GeometryType == 1)
	{
		char* ObjectClassName = Object->GetClassName();
		if (strcmp(ObjectClassName, "Box") == 0)
		{
			PhysicalShapeNode->AddProperty("Type", ZE_PBST_BOX);
			ZEMLNode* ShapeNode = PhysicalShapeNode->AddSubNode("Shape");
			GetProperty(Object, ZE_FLOAT_PROP, "width",  *ShapeNode->AddProperty("Width"));
			GetProperty(Object, ZE_FLOAT_PROP, "height", *ShapeNode->AddProperty("Height"));
			GetProperty(Object, ZE_FLOAT_PROP, "length", *ShapeNode->AddProperty("Length"));
		}
		else if (strcmp(ObjectClassName, "Sphere") == 0)
		{
			PhysicalShapeNode->AddProperty("Type", ZE_PBST_SPHERE);
			ZEMLNode* ShapeNode = PhysicalShapeNode->AddSubNode("Shape");
			GetProperty(Object, ZE_FLOAT_PROP, "radius", *ShapeNode->AddProperty("Radius"));
		}
		else if (strcmp(ObjectClassName, "Capsule") == 0)
		{
			PhysicalShapeNode->AddProperty("Type", ZE_PBST_CAPSULE);
			ZEMLNode* ShapeNode = PhysicalShapeNode->AddSubNode("Shape");
			GetProperty(Object, ZE_FLOAT_PROP, "radius", *ShapeNode->AddProperty("Radius"));
			GetProperty(Object, ZE_FLOAT_PROP, "height", *ShapeNode->AddProperty("Height"));
		}
		else if (strcmp(ObjectClassName, "Cylinder") == 0)
		{
			PhysicalShapeNode->AddProperty("Type", ZE_PBST_CYLINDER);
			ZEMLNode* ShapeNode = PhysicalShapeNode->AddSubNode("Shape");
			GetProperty(Object, ZE_FLOAT_PROP, "radius", *ShapeNode->AddProperty("Radius"));
			GetProperty(Object, ZE_FLOAT_PROP, "height", *ShapeNode->AddProperty("Height"));
		}
		else if (strcmp(ObjectClassName, "Editable Poly") == 0 || strcmp(ObjectClassName, "Editable Mesh") == 0 || strcmp(ObjectClassName, "Editable Patch") == 0)
		{
			PhysicalShapeNode->AddProperty("Type", ZE_PBST_CONVEX);
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
		PhysicalShapeNode->AddProperty("Type", ZE_PBST_CONVEX);
		ProcessPhysicalBodyConvexShape(Node, OwnerNode, PhysicalShapeNode->AddSubNode("Shape"));
	}

	return true;
}

void ZE3dsModelExporter::ProcessPhysicalBody(IGameNode* Node, ZEMLNode* ParentNode)
{
	IGameObject* Object = Node->GetIGameObject();

	if (Object == NULL)
		return;

	ZEInt PhysicalBodyTypeValue;
	GetProperty(Object, ZE_INT_PROP, "PhysicalBody_Type", PhysicalBodyTypeValue);
	PhysicalBodyTypeValue--; // Array index start problem with Max Script and C++)

	ZEPhysicalBodyType PhysicalBodyType = (ZEPhysicalBodyType)PhysicalBodyTypeValue;

	if (PhysicalBodyType == ZE_PBT_NONE)
	{
		zeWarning("No physical body found to export. Node Name : \"%s\"", Node->GetName());
		return;
	}
	else if (PhysicalBodyType == ZE_PBT_RIGID)
	{
		zeLog("Processing physical body of \"%s\"", Node->GetName());

		ZEMLNode* PhysicalBodyNode = ParentNode->AddSubNode("PhysicalBody");

		GetProperty(Object, ZE_BOOL_PROP, "PhysicalBody_Enabled", *PhysicalBodyNode->AddProperty("Enabled"));

		ZEInt PhysicalBodyTypeValue;
		GetProperty(Object, ZE_INT_PROP, "PhysicalBody_Type", PhysicalBodyTypeValue);
		PhysicalBodyTypeValue--; // Array index start problem with Max Script and C++)
		PhysicalBodyNode->AddProperty("Type", PhysicalBodyTypeValue);

		GetProperty(Object, ZE_FLOAT_PROP, "PhysicalBody_Mass", *PhysicalBodyNode->AddProperty("Mass"));

		IGameNode* MassCenter = NULL;
		GetProperty(Object, "PhysicalBody_CenterOfMass", MassCenter);

		if (MassCenter == NULL)
			PhysicalBodyNode->AddProperty("MassCenter", ZEVector3::Zero);
		else
			PhysicalBodyNode->AddProperty("MassCenter", MAX_TO_ZE(MassCenter->GetObjectTM().Translation() - Node->GetObjectTM().Translation()));

		GetProperty(Object, ZE_FLOAT_PROP, "PhysicalBody_LinearDamping", *PhysicalBodyNode->AddProperty("LinearDamping"));
		GetProperty(Object, ZE_FLOAT_PROP, "PhysicalBody_AngularDamping", *PhysicalBodyNode->AddProperty("AngularDamping"));

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
			if (PhysicalShapeNode == NULL || !GetProperty(PhysicalShapeNode->GetIGameObject(), ZE_STRING_PROP, "ZEType", Type) || strcmp(Type, "PhysicalShape") != 0)
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

bool ZE3dsModelExporter::ProcessPhysicalJoint(IGameNode* Node, ZEMLNode* PhysicalJointNode)
{
	IGameObject* Object = Node->GetIGameObject();

	if (Object == NULL)
		return false;

	zeLog("Processing physical joint of \"%s\"", Node->GetName());

	GetProperty(Object, ZE_INT_PROP, "Joint_Enabled", *PhysicalJointNode->AddProperty("Enabled"));

	IGameNode* Body1 = NULL;
	GetProperty(Object, "Joint_Body1", Body1); 								
	if (Body1 != NULL)
		PhysicalJointNode->AddProperty("Body1Id", GetMeshId(Body1));
	else
		PhysicalJointNode->AddProperty("Body1Id", -1);

	IGameNode* Body2 = NULL;
	GetProperty(Object, "Joint_Body2", Body2);
	if (Body2 != NULL)
		PhysicalJointNode->AddProperty("Body2Id", GetMeshId(Body2));
	else
		PhysicalJointNode->AddProperty("Body2Id", -1);

	ZEMLProperty* GlobalAnchorAxisProperty = PhysicalJointNode->AddProperty("UseGlobalAnchorAxis");
	GetProperty(Object, ZE_BOOL_PROP, "Joint_UseGlobalAnchorAxis", *GlobalAnchorAxisProperty);
	if (!GlobalAnchorAxisProperty->GetValue().GetBoolean())
	{
		IGameNode* BodyAnchor1 = NULL;
		GetProperty(Object, "Joint_Body1_Anchor", BodyAnchor1);
		if (BodyAnchor1 != NULL)
		{
			GMatrix LocalTransform =  BodyAnchor1->GetObjectTM() * Node->GetObjectTM().Inverse();
			PhysicalJointNode->AddProperty("LocalAnchor1", MAX_TO_ZE(LocalTransform.Translation()));
			PhysicalJointNode->AddProperty("LocalAxis1", MAX_TO_ZE(LocalTransform.Rotation()));
		}

		IGameNode* BodyAnchor2 = NULL;
		GetProperty(Object, "Joint_Body2_Anchor", BodyAnchor2);
		if (BodyAnchor2 != NULL)
		{
			GMatrix LocalTransform = BodyAnchor2->GetObjectTM() * Node->GetObjectTM().Inverse();
			PhysicalJointNode->AddProperty("LocalAnchor2", MAX_TO_ZE(LocalTransform.Translation()));
			PhysicalJointNode->AddProperty("LocalAxis2", MAX_TO_ZE(LocalTransform.Rotation()));
		}
	}
	else
	{
		PhysicalJointNode->AddProperty("GlobalAnchor", MAX_TO_ZE(Node->GetObjectTM().Translation()));
		PhysicalJointNode->AddProperty("GlobalAxis", MAX_TO_ZE(Node->GetObjectTM().Rotation()));
	}

	ZEInt JointTypeValue = 0;
	GetProperty(Object, ZE_INT_PROP,		"Joint_Type",						JointTypeValue);
	JointTypeValue--;
	PhysicalJointNode->AddProperty("JointType", JointTypeValue);

	GetProperty(Object, ZE_BOOL_PROP,		"Joint_CollideBodies",				*PhysicalJointNode->AddProperty("CollideBodies"));
	GetProperty(Object, ZE_BOOL_PROP,		"Joint_Breakable",					*PhysicalJointNode->AddProperty("Breakable"));	
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_Break_Force",				*PhysicalJointNode->AddProperty("BreakForce"));					
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_Break_Torque",				*PhysicalJointNode->AddProperty("BreakTorque"));

	//Added : Free Joint Properties:

	GetProperty(Object, ZE_INT_PROP,		"Joint_XMotion", 					*PhysicalJointNode->AddProperty("XMotion"));
	GetProperty(Object, ZE_INT_PROP,		"Joint_YMotion", 					*PhysicalJointNode->AddProperty("YMotion"));
	GetProperty(Object, ZE_INT_PROP,		"Joint_ZMotion", 					*PhysicalJointNode->AddProperty("ZMotion"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_LinearLimitValue", 			*PhysicalJointNode->AddProperty("LinearLimitValue"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_LinearLimitRestitution", 	*PhysicalJointNode->AddProperty("LinearLimitRestitution"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_LinearLimitSpring", 			*PhysicalJointNode->AddProperty("LinearLimitSpring"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_LinearLimitDamping", 		*PhysicalJointNode->AddProperty("LinearLimitDamping"));

	GetProperty(Object, ZE_INT_PROP,		"Joint_TwistMotion", 				*PhysicalJointNode->AddProperty("TwistMotion"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_TwistLowLimitValue", 		*PhysicalJointNode->AddProperty("TwistLowLimitValue"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_TwistLowLimitRestitution", 	*PhysicalJointNode->AddProperty("TwistLowLimitRestitution"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_TwistLowLimitSpring", 		*PhysicalJointNode->AddProperty("TwistLowLimitSpring"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_TwistLowLimitDamping", 		*PhysicalJointNode->AddProperty("TwistLowLimitDamping"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_TwistHighLimitValue", 		*PhysicalJointNode->AddProperty("TwistHighLimitValue"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_TwistHighLimitRestitution",	*PhysicalJointNode->AddProperty("TwistHighLimitRestitution"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_TwistHighLimitSpring", 		*PhysicalJointNode->AddProperty("TwistHighLimitSpring"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_TwistHighLimitDamping", 		*PhysicalJointNode->AddProperty("TwistHighLimitDamping"));

	GetProperty(Object, ZE_INT_PROP,		"Joint_Swing1Motion", 				*PhysicalJointNode->AddProperty("Swing1Motion"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_Swing1LimitValue", 			*PhysicalJointNode->AddProperty("Swing1LimitValue"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_Swing1LimitRestitution", 	*PhysicalJointNode->AddProperty("Swing1LimitRestitution"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_Swing1LimitSpring", 			*PhysicalJointNode->AddProperty("Swing1LimitSpring"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_Swing1LimitDamping", 		*PhysicalJointNode->AddProperty("Swing1LimitDamping"));

	GetProperty(Object, ZE_INT_PROP,		"Joint_Swing2Motion", 				*PhysicalJointNode->AddProperty("Swing2Motion"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_Swing2LimitValue", 			*PhysicalJointNode->AddProperty("Swing2LimitValue"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_Swing2LimitRestitution", 	*PhysicalJointNode->AddProperty("Swing2LimitRestitution"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_Swing2LimitSpring", 			*PhysicalJointNode->AddProperty("Swing2LimitSpring"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_Swing2LimitDamping", 		*PhysicalJointNode->AddProperty("Swing2LimitDamping"));

	GetProperty(Object, ZE_INT_PROP,		"Joint_XLinearMotor", 				*PhysicalJointNode->AddProperty("LinearXMotor"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_XLinearMotorForce", 			*PhysicalJointNode->AddProperty("LinearXMotorForce"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_XLinearMotorSpring", 		*PhysicalJointNode->AddProperty("LinearXMotorSpring"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_XLinearMotorDamper", 		*PhysicalJointNode->AddProperty("LinearXMotorDamper"));

	GetProperty(Object, ZE_INT_PROP,		"Joint_YLinearMotor", 				*PhysicalJointNode->AddProperty("LinearYMotor"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_YLinearMotorForce", 			*PhysicalJointNode->AddProperty("LinearYMotorForce"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_YLinearMotorSpring", 		*PhysicalJointNode->AddProperty("LinearYMotorSpring"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_YLinearMotorDamper", 		*PhysicalJointNode->AddProperty("LinearYMotorDamper"));

	GetProperty(Object, ZE_INT_PROP,		"Joint_ZLinearMotor", 				*PhysicalJointNode->AddProperty("LinearZMotor"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_ZLinearMotorForce", 			*PhysicalJointNode->AddProperty("LinearZMotorForce"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_ZLinearMotorSpring", 		*PhysicalJointNode->AddProperty("LinearZMotorSpring"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_ZLinearMotorDamper", 		*PhysicalJointNode->AddProperty("LinearZMotorDamper"));

	GetProperty(Object, ZE_INT_PROP,		"Joint_AngularSwingMotor", 			*PhysicalJointNode->AddProperty("AngularSwingMotor"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_AngularSwingMotorForce", 	*PhysicalJointNode->AddProperty("AngularSwingMotorForce"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_AngularSwingMotorSpring", 	*PhysicalJointNode->AddProperty("AngularSwingMotorSpring"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_AngularSwingMotorDamper", 	*PhysicalJointNode->AddProperty("AngularSwingMotorDamper"));

	GetProperty(Object, ZE_INT_PROP,		"Joint_AngularTwistMotor", 			*PhysicalJointNode->AddProperty("AngularTwistMotor"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_AngularTwistMotorForce", 	*PhysicalJointNode->AddProperty("AngularTwistMotorForce"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_AngularTwistMotorSpring",	*PhysicalJointNode->AddProperty("AngularTwistMotorSpring"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_AngularTwistMotorDamper",	*PhysicalJointNode->AddProperty("AngularTwistMotorDamper"));

	GetProperty(Object, ZE_INT_PROP,		"Joint_AngularSlerpMotor", 			*PhysicalJointNode->AddProperty("AngularSlerpMotor"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_AngularSlerpMotorForce", 	*PhysicalJointNode->AddProperty("AngularSlerpMotorForce"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_AngularSlerpMotorSpring", 	*PhysicalJointNode->AddProperty("AngularSlerpMotorSpring"));
	GetProperty(Object, ZE_FLOAT_PROP,	"Joint_AngularSlerpMotorDamper", 	*PhysicalJointNode->AddProperty("AngularSlerpMotorDamper"));

	Quat Quaternion;
	Point3 Point;

	IGameNode* MotorTargetPosition = NULL;
	GetProperty(Object, "Joint_MotorTargetPosition", MotorTargetPosition);
	if (MotorTargetPosition != NULL)
	{
		PhysicalJointNode->AddProperty("MotorTargetPosition", MAX_TO_ZE(MotorTargetPosition->GetObjectTM().Translation() - Node->GetObjectTM().Translation()));
	}
	else
		PhysicalJointNode->AddProperty("MotorTargetPosition", ZEVector3::Zero);

	GetProperty(Object, ZE_VECTOR3_PROP, "Joint_MotorTargetVelocity", *PhysicalJointNode->AddProperty("MotorTargetVelocity"));

	IGameNode* MotorTargetOrientation = NULL;
	GetProperty(Object, "Joint_MotorTargetOrientation", MotorTargetOrientation);
	if (MotorTargetOrientation != NULL)
	{
		GMatrix Orientation = MotorTargetPosition->GetObjectTM() * Node->GetObjectTM().Inverse();
		PhysicalJointNode->AddProperty("MotorTargetOrientation", MAX_TO_ZE(Orientation.Rotation())); //Investigate why there's error
		// ERROR ERROR
		//Joint->Joint.LinearMotorPosition = MAX_TO_ZE(Orientation.Rotation());
	}
	else
		PhysicalJointNode->AddProperty("MotorTargetOrientation", ZEQuaternion::Identity);

	GetProperty(Object, ZE_VECTOR3_PROP, "Joint_MotorTargetAngularVelocity", *PhysicalJointNode->AddProperty("MotorTargetAngularVelocity"));

	zeLog("Physical joint is processed successfully.");

	return true;
}

bool ZE3dsModelExporter::ProcessBone(IGameNode* Node, ZEMLNode* BonesNode)
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
	BoundingBoxNode->AddProperty("Min", MAX_TO_ZE(BB.Min()));
	BoundingBoxNode->AddProperty("Max", MAX_TO_ZE(BB.Max()));

	if (ParentBoneId != -1)
	{
		GMatrix Transform = Node->GetWorldTM() * ProcessedBones[(ZESize)ParentBoneId]->GetWorldTM().Inverse();
		BoneNode->AddProperty("RelativePosition", MAX_TO_ZE(Transform.Translation() * Node->GetWorldTM().Scaling()));
		BoneNode->AddProperty("RelativeRotation", MAX_TO_ZE(Transform.Rotation()));
		BoneNode->AddProperty("RelativeScale", MAX_TO_ZE(Transform.Scaling()));
	}
	else
	{ 
		zeLog("\"%s\" is the Root Bone", Node->GetName());
		BoneNode->AddProperty("RelativePosition", MAX_TO_ZE(Node->GetWorldTM().Translation() * Node->GetWorldTM().Scaling()));
		BoneNode->AddProperty("RelativeRotation", MAX_TO_ZE(Node->GetWorldTM().Rotation()));
		BoneNode->AddProperty("RelativeScale", MAX_TO_ZE(Node->GetWorldTM().Scaling()));
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

bool ZE3dsModelExporter::ProcessBones(ZEMLNode* BonesNode)
{
	zeLog("Processing bones...");

	Tab<IGameNode*> Nodes = Scene->GetIGameNodeByType(IGameObject::IGAME_BONE);
	const char* Type;

	for (ZESize I = 0; I < (ZESize)Nodes.Count(); I++)
	{
		Type = NULL;
		if (GetProperty(Nodes[I]->GetIGameObject(), ZE_STRING_PROP, "ZEType", Type) && strcmp(Type, "Bone") == 0)
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

	for (ZESize I = 0; I < (ZESize)ProcessedBones.Count(); I++)
		if (!ProcessBone(ProcessedBones[I], BonesNode))
			return false;

	return true;
}

bool ZE3dsModelExporter::ProcessMeshLODVertices(IGameNode* Node, ZEMLNode* LODNode)
{
	IGameMesh* Mesh = (IGameMesh*)Node->GetIGameObject();
	Mesh->InitializeData();
	IGameSkin* Skin = Mesh->GetIGameSkin();

	ZEArray<ZEModelFileVertex> Vertices;
	ZEArray<ZEModelFileSkinnedVertex> SkinnedVertices;
	ZEArray<ZEUInt32> AffectingBoneIds;

	LODNode->AddProperty("MaterialId", ProcessMeshMaterial(Node->GetNodeMaterial()));

	if (!Mesh->IsObjectSkinned())
		Vertices.SetCount((ZESize)Mesh->GetNumberOfFaces() * 3);
	else
		SkinnedVertices.SetCount((ZESize)Mesh->GetNumberOfFaces() * 3);

	zeLog("Processing vertices of mesh \"%s\". Polygon Count : %d, Vertex Count : %d.", Node->GetName(), Mesh->GetNumberOfFaces(), Mesh->GetNumberOfFaces() * 3);
	Mesh->InitializeBinormalData();

	ZEMatrix4x4 ObjectTM;
	ZEMatrix4x4::CreateOrientation(ObjectTM, 
		MAX_TO_ZE(Node->GetObjectTM().Translation()), 
		MAX_TO_ZE(Node->GetObjectTM().Rotation()), 
		MAX_TO_ZE(Node->GetObjectTM().Scaling()));

	ZEMatrix4x4 WorldTM;
	ZEMatrix4x4::CreateOrientation(WorldTM, 
		MAX_TO_ZE(Node->GetWorldTM().Translation()), 
		MAX_TO_ZE(Node->GetWorldTM().Rotation()), 
		MAX_TO_ZE(Node->GetWorldTM().Scaling()));

	ZEMatrix4x4 WorldTransform = WorldTM.Inverse() * ObjectTM;

	bool GotError = false;
	bool BoneCountWarning = false; 
	AffectingBoneIds.SetCount(0);
	for (ZESize I = 0; I < Mesh->GetNumberOfFaces(); I++)
	{
		FaceEx* Face;
		Face = Mesh->GetFace((ZEInt)I);
		if (!Mesh->IsObjectSkinned())
		{
			for (ZESize N = 0; N < 3; N++)
			{	
				ZEModelFileVertex* Vertex = &(Vertices[3*I + N]);

				Vertex->Position = WorldTransform * MAX_TO_ZE(Mesh->GetVertex(Face->vert[N], true)); 
				ZEMatrix4x4::Transform3x3(Vertex->Normal, WorldTransform, MAX_TO_ZE(Mesh->GetNormal(Face->norm[N], true)));

				ZEInt BinormalTangentIndex = Mesh->GetFaceVertexTangentBinormal((ZEInt)I, (ZEInt)N);
				ZEMatrix4x4::Transform3x3(Vertex->Tangent, WorldTransform, MAX_TO_ZE(Mesh->GetTangent(BinormalTangentIndex).Normalize()));
				ZEMatrix4x4::Transform3x3(Vertex->Binormal, WorldTransform, MAX_TO_ZE(Mesh->GetBinormal(BinormalTangentIndex)));

				Vertex->Texcoord = MAX_TO_ZE(Mesh->GetTexVertex(Face->texCoord[N]));
				Vertex++;
			}
		}
		else
		{
			for (ZESize N = 0; N < 3; N++)
			{
				ZEModelFileSkinnedVertex* Vertex = &(SkinnedVertices[3*I + N]);

				Vertex->Position = WorldTransform * MAX_TO_ZE(Mesh->GetVertex(Face->vert[N], true)); 
				ZEMatrix4x4::Transform3x3(Vertex->Normal, WorldTransform, MAX_TO_ZE(Mesh->GetNormal(Face->norm[N], true)));

				ZEInt BinormalTangentIndex = Mesh->GetFaceVertexTangentBinormal((ZEInt)I, (ZEInt)N);
				ZEMatrix4x4::Transform3x3(Vertex->Tangent, WorldTransform, MAX_TO_ZE(Mesh->GetTangent(BinormalTangentIndex).Normalize()));
				ZEMatrix4x4::Transform3x3(Vertex->Binormal, WorldTransform, MAX_TO_ZE(Mesh->GetBinormal(BinormalTangentIndex)));

				Vertex->Texcoord = MAX_TO_ZE(Mesh->GetTexVertex(Face->texCoord[N]));

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

bool ZE3dsModelExporter::ProcessMasterMesh(IGameNode* Node, ZEMLNode* MeshesNode)
{
	zeLog("Processing mesh \"%s\".", Node->GetName());
	IGameMesh* Mesh = (IGameMesh*)Node->GetIGameObject();

	// Process Attributes
	const char* ZEType;


	if (!GetProperty(Mesh, ZE_STRING_PROP, "ZEType", ZEType) || strcmp(ZEType, "Mesh") != 0)
	{
		zeError("Mesh is rejected because it does not contain ZEMeshAttributes. Node Name : \"%s\"", Node->GetName());
		return false;
	}

	ZEMLNode* CurrentMeshNode = MeshesNode->AddSubNode("Mesh");
	CurrentMeshNode->AddProperty("Name", Node->GetName());
	zeLog("Mesh \"%s\" is created as base mesh.", Node->GetName());

	zeLog("Mesh is a master LOD. Setting mesh properties by using this mesh.");
	ProcessedMasterMeshes.Append(1, &Node);
	Mesh->InitializeData();


	ZEAABBox BoundingBox;
	CalculateLocalBoundingBox(BoundingBox, Mesh);
	ZEMLNode* BBoxNode = CurrentMeshNode->AddSubNode("BoundingBox");
	BBoxNode->AddProperty("Min", BoundingBox.Min);
	BBoxNode->AddProperty("Max", BoundingBox.Max);

	CurrentMeshNode->AddProperty("IsSkinned", Mesh->IsObjectSkinned());
	CurrentMeshNode->AddProperty("Position", MAX_TO_ZE(Node->GetWorldTM().Translation()));
	CurrentMeshNode->AddProperty("Rotation", MAX_TO_ZE(Node->GetWorldTM().Rotation()));
	CurrentMeshNode->AddProperty("Scale", MAX_TO_ZE(Node->GetWorldTM().Scaling()));

	ZEMLNode* MainLODsNode = CurrentMeshNode->AddSubNode("LODs");
	ZEMLNode* LODNode = MainLODsNode->AddSubNode("LOD");

	ZEInt MeshLOD;
	GetProperty(Mesh, ZE_INT_PROP, "Mesh_LOD", MeshLOD);

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

bool ZE3dsModelExporter::ProcessMeshLODs(IGameNode* Node, ZEMLNode* MeshesNode)
{
	IGameMesh* Mesh = (IGameMesh*)Node->GetIGameObject();
	const char* LODName = Node->GetName();

	const char* ZEType;

	if (!GetProperty(Mesh, ZE_STRING_PROP, "ZEType", ZEType) || strcmp(ZEType, "Mesh") != 0)
	{
		zeError("Mesh is rejected because it does not contain ZEMeshAttributes. Node Name : \"%s\"", Node->GetName());
		return false;
	}
	
	ZEInt CurrentMeshLODLevel;
	GetProperty(Mesh, ZE_INT_PROP, "Mesh_LOD", CurrentMeshLODLevel);

	ZEArray<ZEMLNode*> Meshes = MeshesNode->GetSubNodes("Mesh");
	ZEMLNode* MasterMesh = NULL;

	for (ZESize I = 0; I < Meshes.GetCount(); I++)
	{
		if (((ZEMLProperty*)Meshes[I]->GetProperty("Name"))->GetValue().GetString() == LODName);
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

bool ZE3dsModelExporter::ProcessMeshes(ZEMLNode* MeshesNode)
{
	zeLog("Processing Meshes...");

	ZEArray<IGameNode*> MasterMeshes;
	ZEArray<IGameNode*>	MeshLODs;
	bool MeshExists = false;
	Scene_ = Scene;
	Tab<IGameNode*> Nodes = Scene->GetIGameNodeByType(IGameObject::IGAME_MESH);

	for (ZESize I = 0; I < (ZESize)Nodes.Count(); I++)
	{
		if (GetBoneId(Nodes[I]) == -1)
		{
			MeshExists = false;

			const char* ZEType;

			IGameMesh* Mesh = (IGameMesh*)Nodes[I]->GetIGameObject();
			if (GetProperty(Mesh, ZE_STRING_PROP, "ZEType", ZEType) == true)	
				if (strcmp(ZEType, "PhysicalShape") == 0) 
					continue;

			for (ZESize J = 0; J < MasterMeshes.GetCount(); J++)
			{
				if (strncmp(MasterMeshes[J]->GetName(), Nodes[I]->GetName(), ZE_MDLF_MAX_NAME_SIZE) == 0)
				{
					MeshExists = true;
					ZEInt MasterMeshLOD;
					ZEInt CurrentMeshLOD;

					IGameMesh* MasterMesh = (IGameMesh*)MasterMeshes[J]->GetIGameObject();
					GetProperty(MasterMesh, ZE_INT_PROP, "Mesh_LOD", MasterMeshLOD);

					IGameMesh* CurrentMesh = (IGameMesh*)Nodes[I]->GetIGameObject();
					GetProperty(CurrentMesh, ZE_INT_PROP, "Mesh_LOD", CurrentMeshLOD);


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

void ZE3dsModelExporter::ProcessAnimationFrames(ZESize AnimationStartFrame, ZESize AnimationFrameCount, ZEMLNode* AnimationNode)
{
	if (AnimationStartFrame + AnimationFrameCount - 1 > TotalFrameCount)
		zeError("Specified animation frame range is exceeds 3ds Max Scene frame count.");

	ZEArray<ZEModelFileAnimationFrame> Frames;
	Frames.SetCount(AnimationFrameCount);
	ZEInt ActualAnimationFrame = (ZEInt)AnimationStartFrame;

	zeLog("Frame Count : %d", AnimationFrameCount);
	zeLog("Processing Animation Frames: %d - %d", AnimationStartFrame, AnimationStartFrame + AnimationFrameCount - 1);

	for (ZESize I = 0; I < AnimationFrameCount; I++)
	{
		ZEModelFileAnimationFrame* CurrentFrame = &(Frames[I]);
		ZEModelFileAnimationKey* Key;

		CurrentFrame->BoneKeys.SetCount((ZESize)AnimationEnabledBones.Count());
		for (ZESize N = 0; N < (ZESize)AnimationEnabledBones.Count(); N++)
		{
			Key = &CurrentFrame->BoneKeys[N];
			Key->ItemId = GetBoneId(AnimationEnabledBones[N]);

			ZEMLNode* MainBonesNode = ModelNode.GetSubNodes("Bones").GetFirstItem();
			ZEArray<ZEMLNode*> BoneNodes = MainBonesNode->GetSubNodes("Bone");
			ZEInt32 ParentBoneId = ((ZEMLProperty*)BoneNodes[(ZESize)Key->ItemId]->GetProperty("ParentBone"))->GetValue().GetInt32();

			if (ParentBoneId == -1)
			{
				GMatrix Matrix = AnimationEnabledBones[N]->GetWorldTM(ActualAnimationFrame * TicksPerFrame);
				Key->Position = MAX_TO_ZE(Matrix.Translation());
				Key->Rotation = MAX_TO_ZE(Matrix.Rotation());
				Key->Scale = MAX_TO_ZE(Matrix.Scaling());
			}
			else
			{
				GMatrix Matrix = AnimationEnabledBones[N]->GetWorldTM(ActualAnimationFrame * TicksPerFrame) * 
					ProcessedBones[(ZESize)ParentBoneId]->GetWorldTM(ActualAnimationFrame * TicksPerFrame).Inverse();
				//AnimationEnabledBones[N]->GetNodeParent()->GetWorldTM(I * TicksPerFrame).Inverse();
				Key->Position = MAX_TO_ZE(Matrix.Translation());
				Key->Rotation = MAX_TO_ZE(Matrix.Rotation());
				Key->Scale = MAX_TO_ZE(Matrix.Scaling());
			}
		}

		CurrentFrame->MeshKeys.SetCount((ZESize)AnimationEnabledMeshes.Count());
		for (ZESize N = 0; N < (ZESize)AnimationEnabledMeshes.Count(); N++)
		{
			Key = &CurrentFrame->MeshKeys[N];
			Key->ItemId = GetMeshId(AnimationEnabledMeshes[N]);
			GMatrix Matrix = AnimationEnabledMeshes[N]->GetWorldTM(ActualAnimationFrame * TicksPerFrame);
			Key->Position = MAX_TO_ZE(Matrix.Translation());
			Key->Rotation = MAX_TO_ZE(Matrix.Rotation());
			Key->Scale = MAX_TO_ZE(Matrix.Scaling());
		}

		ActualAnimationFrame++;
	}

	AnimationNode->AddDataProperty("Frames", Frames.GetCArray(), Frames.GetCount() * sizeof(ZEModelFileAnimationFrame), true);

}

bool ZE3dsModelExporter::ProcessAnimations(ZEMLNode* AnimationsNode)
{
	zeLog("Processing Animations...");

	GMatrix Matrix;
	for (ZESize I = 0; I < (ZESize)ProcessedMasterMeshes.Count(); I++)
	{
		Matrix = ProcessedMasterMeshes[I]->GetWorldTM((ZEInt)I * TicksPerFrame);
		for (ZEInt N = 0; N < TotalFrameCount; N++)
		{
			if (!(ProcessedMeshes[I]->GetWorldTM(N * TicksPerFrame) == Matrix))
			{
				AnimationEnabledMeshes.Append(1, &ProcessedMeshes[I]);
				break;
			}
		}
	}

	for (ZESize I = 0; I < (ZESize)ProcessedBones.Count(); I++)
	{
		Matrix = ProcessedBones[I]->GetWorldTM((ZEInt)I * TicksPerFrame);
		for (ZEInt N = 0; N < TotalFrameCount; N++)
			if (!(ProcessedBones[I]->GetWorldTM(N * TicksPerFrame) == Matrix))
			{
				AnimationEnabledBones.Append(1, &ProcessedBones[I]);
				break;
			}
	}

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

bool ZE3dsModelExporter::DumpPropertyContainer(IExportEntity* Node)
{
	if (Node == NULL)
	{
		return false;
	}

	IPropertyContainer* Properties = Node->GetIPropertyContainer();
	if (Properties == NULL)
	{
		zeError("There is no property container. Class Name : \"%s\".", Node->GetClassName());
		return false;
	}
	zeLog("Dumping properties of entity. Class Name : \"%s\".", Node->GetClassName());
	for (ZEInt I = 0; I < Properties->GetNumberOfProperties(); I++)
	{
		IGameProperty* Property = Properties->GetProperty(I);
		zeLog("Property %d. Name : \"%s\", Index : %d, Animated : %s, ", I, Property->GetName(), Property->GetParamBlockIndex(), (Property->IsPropAnimated() ? "Yes" : "No"));

		float FloatValue;
		ZEInt IntValue;
		Point3 Point3Value;
		Point4 Point4Value;
		const char* StringValue;
		IParamBlock2* ParamBlock;
		ZEInt ParamId;
		ParamType2 ParamType;
		switch(Property->GetType())
		{
		case ZE_UNKNOWN_PROP:
			ParamBlock =  Property->GetMaxParamBlock2();
			ParamId = ParamBlock->IndextoID(Property->GetParamBlockIndex());
			ParamType = ParamBlock->GetParameterType(ParamId);
			switch(ParamBlock->GetParameterType(ParamId))
			{
			case TYPE_INODE:
				zeLog("Type : INode, Value : \"%s\".", (ParamBlock->GetINode(ParamId, 0, 0)->GetName() != NULL ? ParamBlock->GetINode(ParamId, 0, 0)->GetName() : "NULL"));
				break;
			case TYPE_INODE_TAB:
				zeLog("Type : INodeTab, Values : [");
				for (ZEInt N = 0 ; N < ParamBlock->Count(ParamId); N++)
					zeLog("\"%s\", ", ParamBlock->GetINode(ParamId, 0, N)->GetName());
				zeLog("].");
				break;
			};
			break;
		case ZE_FLOAT_PROP:
			Property->GetPropertyValue(FloatValue);
			zeLog("Type : FLOAT, Value : %f.", FloatValue);
			break;
		case ZE_VECTOR3_PROP:
			Property->GetPropertyValue(Point3Value);
			zeLog("Type : POINT3, Value : <%f, %f, %f>", Point3Value.x, Point3Value.y, Point3Value.z);
			break;
		case ZE_INT_PROP:
			Property->GetPropertyValue(IntValue);
			zeLog("Type : INT, Value : %d.", IntValue);
			break;
		case ZE_STRING_PROP:
			Property->GetPropertyValue(StringValue);
			zeLog("Type : STRING, Value : \"%s\".",StringValue);
			break;
		case ZE_VECTOR4_PROP:
			Property->GetPropertyValue(Point4Value);
			zeLog("Type : POINT4, Value : <%f, %f, %f, %f>.", Point4Value.x, Point4Value.y, Point4Value.z, Point4Value.w);
			break;
		default:
			zeLog("Property Type : ERROR.");
		}
	}
	return true;
}

void ZE3dsModelExporter::CollectResources()
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

		if (!GetProperty(CurrentObject, ZE_STRING_PROP, "ZEType", NodeZEType))
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
			zeError("Mesh \"%s\" does not have valid material. Can not collect resource.", I, CurrentNode->GetName());

		bool IsFound = false;

		for(ZESize J = 0; J < ResourceMaterials.Count(); J++)
			if(ResourceMaterials[J] == CurrentMaterial)
				IsFound = true;

		if(!IsFound)
			ResourceMaterials.Append(1, &CurrentMaterial);
	}

	for (ZESize I = 0; I < ResourceMaterials.Count(); I++)
	{
		IGameMaterial* NodeMaterial = ResourceMaterials[I];
		ZEString MaterialName = NodeMaterial->GetMaterialName();

		ResourceConfigurationDialog->AddResource(MaterialName + ".ZEMaterial", ZEString(), ZE_ROAA_COPY_OVERWRITE);

		ZEInt NumberOfMaps = NodeMaterial->GetNumberOfTextureMaps();

		for (ZEInt N = 0; N < NumberOfMaps; N++)
		{
			char RelativePath[ZE_MDLF_MAX_FILENAME_SIZE];

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
