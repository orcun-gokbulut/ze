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

#include "ZE3dsModelExporter.h"
#include "ZETOutput.h"

#include <IGame/IGameFx.h>
#include <io.h>

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
bool GetProperty(IExportEntity * Object, PropType Type, const char* Property, T& Value)
{ 
	if (Object == NULL)
		return false;

	IGameProperty* Prop = Object->GetIPropertyContainer()->QueryProperty(Property);
	if (Prop != NULL)
	{
		if (Prop->GetType() == Type)
		{
			if (Prop->GetPropertyValue(Value))
				return true;
			else
			{
				zepdWarning("Can not read property value. (Property Name : \"%s\")", Property);
				return false;
			}
		}
		else
		{
			zepdWarning("Wrong property type. (Property Name : \"%s\", Expected Type : %d, Type : %d)", Property, Prop->GetType(), Type);
			return false;
		}
	}
	else
	{
		zepdWarning("Can not find property. (Property Name : \"%s\")", Property);
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
		int ParamId = ParamBlock->IndextoID(Prop->GetParamBlockIndex());

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
		zepdWarning("Can not find property. (Property Name : \"%s\")", Property);
		return false;
	}
}

template <>
bool GetProperty<bool>(IExportEntity* Object, PropType Type, const char* Property, bool& Value)
{ 
	if (Object == NULL)
		return false;

	IGameProperty* Prop = Object->GetIPropertyContainer()->QueryProperty(Property);
	if (Prop != NULL)
	{
		if (Prop->GetType() == Type)
		{
			int Temp;
			if (Prop->GetPropertyValue(Temp))
			{
				Value = Temp;
				return true;
			}
			else
			{
				zepdWarning("Can not read property value. (Property Name : \"%s\")", Property);
				return false;
			}
		}
		else
		{
			zepdWarning("Wrong property type. (Property Name : \"%s\", Expected Type : %d, Type : %d)", Property, Prop->GetType(), Type);
			return false;
		}
	}
	else
	{
		zepdWarning("Can not find property. (Property Name : \"%s\")", Property);
		return false;
	}
}

template <>
bool GetProperty<const char*>(IExportEntity * Object, PropType Type, const char* Property, const char*& Value)
{ 
	if (Object == NULL)
		return false;

	IGameProperty* Prop = Object->GetIPropertyContainer()->QueryProperty(Property);
	if (Prop != NULL && Prop->GetType() == Type)
		return Prop->GetPropertyValue(Value);
	else
		return false;
}

template <>
bool GetProperty<unsigned int>(IExportEntity* Object, PropType Type, const char* Property, unsigned int& Value)
{ 
	int Temp;
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


bool ZEModelExporter::GetRelativePath(const char* RealPath, char* RelativePath)
{
	if (strnicmp(RealPath, ExporterOptions.ResourceDirectory, strlen(ExporterOptions.ResourceDirectory)) == 0)
	{
		strcpy(RelativePath, RealPath + strlen(ExporterOptions.ResourceDirectory) + 1);
		return true;
	}
	else
	{
		RelativePath[0] = '\0';
		return false;
	}
}

int ZEModelExporter::GetMeshId(IGameNode* Node)
{
	for (int I = 0; I < ProcessedMasterMeshes.Count(); I++)
		if (ProcessedMasterMeshes[I]->GetNodeID() == Node->GetNodeID())
			return I;
	return -1;
}

int ZEModelExporter::GetBoneId(IGameNode* Node)
{
	for (int I = 0; I < ProcessedBones.Count(); I++)
		if (ProcessedBones[I]->GetNodeID() == Node->GetNodeID())
			return I;

	return -1;
}


void CalculateLocalBoundingBox(ZEAABoundingBox& BoundingBox, IGameMesh* Mesh)
{
	if (Mesh->GetNumberOfFaces() == 0)
	{
		BoundingBox.Min = BoundingBox.Max = ZEVector3(0.0f, 0.0f, 0.0f);
		return;
	}
	else
		BoundingBox.Min = BoundingBox.Max = *(ZEVector3*)&Mesh->GetVertex(Mesh->GetFace(0)->vert[0], true);

	for (size_t I = 0; I < Mesh->GetNumberOfFaces(); I++)
	{
		FaceEx* Face = Mesh->GetFace(I);
		for (size_t N = 0; N < 3; N++)
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

int ZEModelExporter::ProcessMeshMaterial(IGameMaterial* Material)
{
	if (Material == NULL)
		return -1;

	for (int I = 0; I < Materials.Count(); I++)
		if (Materials[I] == Material)
			return I;
	Materials.Append(1, &Material);
	return Materials.Count() - 1;
}

bool ZEModelExporter::ProcessMaterials()
{
	zepdLog("Processing materials...");

	ModelFile.Materials.SetCount(Materials.Count());
	for (int I = 0; I < Materials.Count(); I++)
	{
		IGameMaterial* NodeMaterial = Materials[I];

		zepdLog("\tProcessing material \"%s\" (%d/%d).", NodeMaterial->GetMaterialName(), I + 1, Materials.Count());
		ZEModelFileMaterial* CurrentMaterial = &ModelFile.Materials[I];
		ZeroMemory(CurrentMaterial, sizeof(ZEModelFileMaterial));

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

		for (int N = 0; N < NodeMaterial->GetNumberOfTextureMaps(); N++)
		{
			char RelativePath[ZE_MDLF_MAX_FILENAME_SIZE];

			IGameTextureMap* CurrentTexture = NodeMaterial->GetIGameTextureMap(N);
			switch(CurrentTexture->GetStdMapSlot())
			{
			case ID_AM: // Ambient
				break;
			case ID_DI: // Diffuse
				if (!GetRelativePath(CurrentTexture->GetBitmapFileName(), RelativePath))
				{
					zepdError("Material texture's relative path does not matches with Zinek Engine Resource directory. Please check your Zinek Engine resource directory. disabling Diffuse Map.\r\n"
						"\tMaterial Name : \"%s\", Texture Slot : Diffuse Map, File path : \"%s\" (Path is not in Zinek Engine resource directory)",
						NodeMaterial->GetMaterialName(), CurrentTexture->GetBitmapFileName());
					continue;
				}
				CurrentMaterial->ShaderComponents |= ZESHADER_DIFFUSEMAP;
				strncpy(CurrentMaterial->DiffuseMap, RelativePath , ZE_MDLF_MAX_FILENAME_SIZE);
				break;
			case ID_SP: // Specular
				if (!GetRelativePath(CurrentTexture->GetBitmapFileName(), RelativePath))
				{
					zepdError("Material texture's relative path does not matches with Zinek Engine Resource directory. Please check your Zinek Engine resource directory. disabling Specular Map.\r\n"
						"\tMaterial Name : \"%s\", Texture Slot : Specular Map, File path : \"%s\" (Path is not in Zinek Engine resource directory)",
						NodeMaterial->GetMaterialName(), CurrentTexture->GetBitmapFileName());
					continue;
				}
				CurrentMaterial->ShaderComponents |= ZESHADER_SPECULARMAP;
				strncpy(CurrentMaterial->SpecularMap, RelativePath, ZE_MDLF_MAX_FILENAME_SIZE);
				break;
			case ID_SI:	// Emmisive
				if (!GetRelativePath(CurrentTexture->GetBitmapFileName(), RelativePath))
				{
					zepdError("Material texture's relative path does not matches with Zinek Engine Resource directory. Please check your Zinek Engine resource directory. disabling Emmisive Map.\r\n"
						"\tMaterial Name : \"%s\", Texture Slot : Self Ilimunation Map, File path : \"%s\" (Path is not in Zinek Engine resource directory)",
						NodeMaterial->GetMaterialName(), CurrentTexture->GetBitmapFileName());
					continue;
				}
				CurrentMaterial->ShaderComponents |= ZESHADER_EMMISIVEMAP;
				strncpy(CurrentMaterial->EmmisiveMap, RelativePath, ZE_MDLF_MAX_FILENAME_SIZE);
				break;
			case ID_OP:	// Opacity 
				if (!GetRelativePath(CurrentTexture->GetBitmapFileName(), RelativePath))
				{
					zepdError("Material texture's relative path does not matches with Zinek Engine Resource directory. Please check your Zinek Engine resource directory. disabling Opacity Map.\r\n"
						"\tMaterial Name : \"%s\", Texture Slot : Opacity Map, File path : \"%s\" (Path is not in Zinek Engine resource directory)",
						NodeMaterial->GetMaterialName(), CurrentTexture->GetBitmapFileName());
					continue;
				}
				CurrentMaterial->ShaderComponents |= ZESHADER_OCAPASITYMAP;
				strncpy(CurrentMaterial->OpasityMap, RelativePath, ZE_MDLF_MAX_FILENAME_SIZE);
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
				strncpy(CurrentMaterial->DetailMap, RelativePath, ZE_MDLF_MAX_FILENAME_SIZE);
				*/
				break;
			case ID_BU: // Bump 
				if (!GetRelativePath(CurrentTexture->GetBitmapFileName(), RelativePath))
				{
					zepdError("Material texture's relative path does not matches with Zinek Engine Resource directory. Please check your Zinek Engine resource directory. disabling Bump Map.\r\n"
						"\tMaterial Name : \"%s\", Texture Slot : Bump Map, File path : \"%s\" (Path is not in Zinek Engine resource directory)",
						NodeMaterial->GetMaterialName(), CurrentTexture->GetBitmapFileName());
					continue;
				}
				CurrentMaterial->ShaderComponents |= ZESHADER_NORMALMAP;
				GetRelativePath(CurrentTexture->GetBitmapFileName(), RelativePath);
				strncpy(CurrentMaterial->NormalMap, RelativePath, ZE_MDLF_MAX_FILENAME_SIZE);
				break;
			case ID_RL: // Reflection 
				if (!GetRelativePath(CurrentTexture->GetBitmapFileName(), RelativePath))
				{
					zepdError("Material texture's relative path does not matches with Zinek Engine Resource directory. Please check your Zinek Engine resource directory. disabling Reflection Map.\r\n"
						"\tMaterial Name : \"%s\", Texture Slot : Reflection Map, File path : \"%s\" (Path is not in Zinek Engine resource directory)",
						NodeMaterial->GetMaterialName(), CurrentTexture->GetBitmapFileName());
					continue;
				}
				CurrentMaterial->ShaderComponents |= ZESHADER_REFLECTION;
				GetRelativePath(CurrentTexture->GetBitmapFileName(), RelativePath);
				strncpy(CurrentMaterial->ReflectionMap, RelativePath, ZE_MDLF_MAX_FILENAME_SIZE);
				break; 
			case ID_RR: // Refraction 
				if (!GetRelativePath(CurrentTexture->GetBitmapFileName(), RelativePath))
				{
					zepdError("Material texture's relative path does not matches with Zinek Engine Resource directory. Please check your Zinek Engine resource directory. disabling Refraction Map.\r\n"
						"\tMaterial Name : \"%s\", Texture Slot : Refraction Map, File path : \"%s\" (Path is not in Zinek Engine resource directory)",
						NodeMaterial->GetMaterialName(), CurrentTexture->GetBitmapFileName());
					continue;
				}
				CurrentMaterial->ShaderComponents |= ZESHADER_REFRACTION;
				GetRelativePath(CurrentTexture->GetBitmapFileName(), RelativePath);
				strncpy(CurrentMaterial->RefractionMap, RelativePath, ZE_MDLF_MAX_FILENAME_SIZE);
				break;
			}

			if (CurrentMaterial->Opasity != 1.0f || strncmp(CurrentMaterial->OpasityMap, "", ZE_MDLF_MAX_FILENAME_SIZE) !=0)
				CurrentMaterial->Transparant = true;
		}		
	}
	return true;
}

void ZEModelExporter::ProcessPhysicalBodyConvexShape(IGameNode* Node, IGameNode* OwnerNode, ZEModelFilePhysicalShape* Shape)
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

	Shape->Type = ZE_PBST_CONVEX;

	Shape->Convex.Vertices.SetCount(Mesh->GetNumberOfVerts());
	for (int I = 0; I < Mesh->GetNumberOfVerts(); I++)
	{
		Point3 Vertex = Mesh->GetVertex(I, false) * WorldTransform;
		Shape->Convex.Vertices[I] = MAX_TO_ZE(Vertex);
	}
}

bool ZEModelExporter::ProcessPhysicalShape(IGameNode* Node, IGameNode* OwnerNode, ZEModelFilePhysicalShape* PhysicalShape)
{
	IGameObject* Object = Node->GetIGameObject();

	const char* ZEType;
	if (!GetProperty(Object, IGAME_STRING_PROP, "ZEType", ZEType) || (strcmp(ZEType, "PhysicalShape") != 0))
	{
		zepdError("Mesh is rejected because it does not contain ZEPhysicalShape. Node Name: \"%s\".", Node->GetName());
		return false;
	}

	GMatrix Transform = Node->GetWorldTM() * OwnerNode->GetWorldTM().Inverse();
	PhysicalShape->Position = MAX_TO_ZE(Transform.Translation());
	PhysicalShape->Rotation = MAX_TO_ZE(Transform.Rotation());

	int GeometryType;
	GetProperty(Object, IGAME_FLOAT_PROP, "PhysicalShape_MaterialRestitution", PhysicalShape->Restitution);
	GetProperty(Object, IGAME_FLOAT_PROP, "PhysicalShape_MaterialDynamicFriction", PhysicalShape->DynamicFriction);
	GetProperty(Object, IGAME_FLOAT_PROP, "PhysicalShape_MaterialStaticFriction", PhysicalShape->StaticFriction);
	GetProperty(Object, IGAME_INT_PROP, "PhysicalShape_GeometryType", GeometryType);

	if (GeometryType == 1)
	{
		char* ObjectClassName = Object->GetClassName();
		if (strcmp(ObjectClassName, "Box") == 0)
		{
			PhysicalShape->Type = ZE_PBST_BOX;
			GetProperty(Object, IGAME_FLOAT_PROP, "width", PhysicalShape->Box.Width);
			GetProperty(Object, IGAME_FLOAT_PROP, "height", PhysicalShape->Box.Height);
			GetProperty(Object, IGAME_FLOAT_PROP, "length", PhysicalShape->Box.Length);
		}
		else if (strcmp(ObjectClassName, "Sphere") == 0)
		{
			PhysicalShape->Type = ZE_PBST_SPHERE;
			GetProperty(Object, IGAME_FLOAT_PROP, "radius", PhysicalShape->Sphere.Radius);
		}
		else if (strcmp(ObjectClassName, "Capsule") == 0)
		{
			PhysicalShape->Type = ZE_PBST_CAPSULE;
			GetProperty(Object, IGAME_FLOAT_PROP, "radius", PhysicalShape->Capsule.Radius);
			GetProperty(Object, IGAME_FLOAT_PROP, "height", PhysicalShape->Capsule.Height);
		}
		else if (strcmp(ObjectClassName, "Cylinder") == 0)
		{
			PhysicalShape->Type = ZE_PBST_CYLINDER;
			GetProperty(Object, IGAME_FLOAT_PROP, "radius", PhysicalShape->Cylinder.Radius);
			GetProperty(Object, IGAME_FLOAT_PROP, "height", PhysicalShape->Cylinder.Height);
		}
		else if (strcmp(ObjectClassName, "Editable Poly") == 0 || strcmp(ObjectClassName, "Editable Mesh") == 0 || strcmp(ObjectClassName, "Editable Patch") == 0)
		{
			PhysicalShape->Type = ZE_PBST_CONVEX;
			ProcessPhysicalBodyConvexShape(Node, OwnerNode, PhysicalShape);
		}
		else
		{
			zepdError("Shape type does not supported. Type : \"%s\", Node Name : \"%s\"", ObjectClassName, Node->GetName());
			return false;
		}
	}
	else if (GeometryType == 2) // Convex
	{
		PhysicalShape->Type = ZE_PBST_CONVEX;
		ProcessPhysicalBodyConvexShape(Node, OwnerNode, PhysicalShape);
	}

	return true;
}

bool ZEModelExporter::ProcessPhysicalBody(IGameNode* Node, ZEModelFilePhysicalBody* PhysicalBody)
{
	IGameObject* Object = Node->GetIGameObject();
	if (Object == NULL)
		return false;

	//GetProperty(Object, IGAME_INT_PROP, "PhysicalBody_Enabled", (int&)PhysicalBodyEnabled);

	GetProperty(Object, IGAME_INT_PROP, "PhysicalBody_Enabled", PhysicalBody->Enabled);
	int Temp;
	GetProperty(Object, IGAME_INT_PROP, "PhysicalBody_Type", Temp);
	PhysicalBody->Type = (ZEPhysicalBodyType)(Temp - 1); // Array index start problem with Max Script and C++)			
	GetProperty(Object, IGAME_FLOAT_PROP, "PhysicalBody_Mass", PhysicalBody->Mass);

	IGameNode* MassHelper = NULL;

	GetProperty(Object, "PhysicalBody_CenterOfMass", MassHelper);
	if (MassHelper == NULL)
		PhysicalBody->MassCenter = ZEVector3(0.0f, 0.0f, 0.0f);
	else
		PhysicalBody->MassCenter = MAX_TO_ZE(MassHelper->GetObjectTM().Translation() - Node->GetObjectTM().Translation());

	GetProperty(Object, IGAME_FLOAT_PROP, "PhysicalBody_LinearDamping" , PhysicalBody->LinearDamping);
	GetProperty(Object, IGAME_FLOAT_PROP, "PhysicalBody_AngularDamping", PhysicalBody->AngularDamping);	

	IGameProperty* ShapesProp = Object->GetIPropertyContainer()->QueryProperty("PhysicalBody_Shapes");
	if (ShapesProp == NULL)
	{
		zepdError("Missing property \"PhysicalShapes\" in ZEBoneAttribute. Node Name : \"%s\"", Node->GetName());
		return false;
	}
	IParamBlock2* ParamBlock = ShapesProp->GetMaxParamBlock2();
	int ParamId = ParamBlock->IndextoID(ShapesProp->GetParamBlockIndex());
	PhysicalBody->Shapes.SetCount(ParamBlock->Count(ParamId));
	for (int I = 0; I < ParamBlock->Count(ParamId); I++)
	{
		const char* Type;
		IGameNode* PhysicalShapeNode = Scene->GetIGameNode(ParamBlock->GetINode(ParamId, 0, I));
		if (PhysicalShapeNode == NULL || !GetProperty(PhysicalShapeNode->GetIGameObject(), IGAME_STRING_PROP, "ZEType", Type) || strcmp(Type, "PhysicalShape") != 0)
		{
			zepdError("Physical body shape is not a valid ZEPhysicalShapeAttribute. Array Index : %d, Shape Name : \"%s\".", I, (PhysicalShapeNode != NULL ? PhysicalShapeNode->GetName() : "NULL"));
			return false;
		}
		ProcessPhysicalShape(PhysicalShapeNode, Node, &PhysicalBody->Shapes[I]);
	}
	return true;
}

bool ZEModelExporter::ProcessPhysicalJoint(IGameNode* Node, ZEModelFilePhysicalJoint* Joint)
{
	IGameObject* Object = Node->GetIGameObject();
	if (Object == NULL)
		return false;

	memset(Joint, 0, sizeof(ZEModelFilePhysicalJoint));

	GetProperty(Object, IGAME_INT_PROP, "Joint_Enabled", Joint->Enabled);

	IGameNode* Body1;
	GetProperty(Object, "Joint_Body1", Body1); 								
	if (Body1 != NULL)
		Joint->Body1Id = GetMeshId(Body1);
	else
		Joint->Body1Id = -1;

	IGameNode* Body2;
	GetProperty(Object, "Joint_Body2", Body2);
	if (Body2 != NULL)
		Joint->Body2Id = GetMeshId(Body2);
	else
		Joint->Body2Id = -1;

	GetProperty(Object, IGAME_INT_PROP, "Joint_UseGlobalAnchorAxis", (int&)Joint->UseGlobalAnchorAxis);
	if (!Joint->UseGlobalAnchorAxis)
	{
		IGameNode* BodyAnchor1;
		GetProperty(Object, "Joint_Body1_Anchor", BodyAnchor1);
		if (BodyAnchor1 != NULL)
		{
			GMatrix LocalTransform =  BodyAnchor1->GetObjectTM() * Node->GetObjectTM().Inverse();
			Joint->LocalAnchor1 = MAX_TO_ZE(LocalTransform.Translation());
			Joint->LocalAxis1 = MAX_TO_ZE(LocalTransform.Rotation());
		}

		IGameNode* BodyAnchor2;
		GetProperty(Object, "Joint_Body2_Anchor", BodyAnchor2);
		if (BodyAnchor2 != NULL)
		{
			GMatrix LocalTransform = BodyAnchor2->GetObjectTM() * Node->GetObjectTM().Inverse();
			Joint->LocalAnchor2 = MAX_TO_ZE(LocalTransform.Translation());
			Joint->LocalAxis2 = MAX_TO_ZE(LocalTransform.Rotation());
		}
	}
	else
	{
		Joint->GlobalAnchor = MAX_TO_ZE(Node->GetObjectTM().Translation());
		Joint->GlobalAxis = MAX_TO_ZE(Node->GetObjectTM().Rotation());
	}

	GetProperty(Object, IGAME_INT_PROP,		"Joint_Type",						Joint->JointType);
	Joint->JointType--;
	GetProperty(Object, IGAME_INT_PROP,		"Joint_CollideBodies",				Joint->CollideBodies);					
	GetProperty(Object, IGAME_INT_PROP,		"Joint_Breakable",					Joint->Breakable);	
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_Break_Force",				Joint->BreakForce);					
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_Break_Torque",				Joint->BreakTorque);

	//Added : Free Joint Properties:

	GetProperty(Object, IGAME_INT_PROP,		"Joint_XMotion", 					Joint->XMotion);
	GetProperty(Object, IGAME_INT_PROP,		"Joint_YMotion", 					Joint->YMotion);
	GetProperty(Object, IGAME_INT_PROP,		"Joint_ZMotion", 					Joint->ZMotion);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_LinearLimitValue", 			Joint->LinearLimitValue);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_LinearLimitRestitution", 	Joint->LinearLimitRestitution);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_LinearLimitSpring", 			Joint->LinearLimitSpring);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_LinearLimitDamping", 		Joint->LinearLimitDamping);

	GetProperty(Object, IGAME_INT_PROP,		"Joint_TwistMotion", 				Joint->TwistMotion);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_TwistLowLimitValue", 		Joint->TwistLowLimitValue);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_TwistLowLimitRestitution", 	Joint->TwistLowLimitRestitution);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_TwistLowLimitSpring", 		Joint->TwistLowLimitSpring);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_TwistLowLimitDamping", 		Joint->TwistLowLimitDamping);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_TwistHighLimitValue", 		Joint->TwistHighLimitValue);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_TwistHighLimitRestitution",	Joint->TwistHighLimitRestitution);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_TwistHighLimitSpring", 		Joint->TwistHighLimitSpring);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_TwistHighLimitDamping", 		Joint->TwistHighLimitDamping);

	GetProperty(Object, IGAME_INT_PROP,		"Joint_Swing1Motion", 				Joint->Swing1Motion);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_Swing1LimitValue", 			Joint->Swing1LimitValue);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_Swing1LimitRestitution", 	Joint->Swing1LimitRestitution);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_Swing1LimitSpring", 			Joint->Swing1LimitSpring);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_Swing1LimitDamping", 		Joint->Swing1LimitDamping);

	GetProperty(Object, IGAME_INT_PROP,		"Joint_Swing2Motion", 				Joint->Swing2Motion);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_Swing2LimitValue", 			Joint->Swing2LimitValue);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_Swing2LimitRestitution", 	Joint->Swing2LimitRestitution);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_Swing2LimitSpring", 			Joint->Swing2LimitSpring);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_Swing2LimitDamping", 		Joint->Swing2LimitDamping);

	GetProperty(Object, IGAME_INT_PROP,		"Joint_XLinearMotor", 				Joint->LinearXMotor);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_XLinearMotorForce", 			Joint->LinearXMotorForce);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_XLinearMotorSpring", 		Joint->LinearXMotorSpring);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_XLinearMotorDamper", 		Joint->LinearXMotorDamper);

	GetProperty(Object, IGAME_INT_PROP,		"Joint_YLinearMotor", 				Joint->LinearYMotor);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_YLinearMotorForce", 			Joint->LinearYMotorForce);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_YLinearMotorSpring", 		Joint->LinearYMotorSpring);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_YLinearMotorDamper", 		Joint->LinearYMotorDamper);

	GetProperty(Object, IGAME_INT_PROP,		"Joint_ZLinearMotor", 				Joint->LinearZMotor);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_ZLinearMotorForce", 			Joint->LinearZMotorForce);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_ZLinearMotorSpring", 		Joint->LinearZMotorSpring);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_ZLinearMotorDamper", 		Joint->LinearZMotorDamper);

	GetProperty(Object, IGAME_INT_PROP,		"Joint_AngularSwingMotor", 			Joint->AngularSwingMotor);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_AngularSwingMotorForce", 	Joint->AngularSwingMotorForce);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_AngularSwingMotorSpring", 	Joint->AngularSwingMotorSpring);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_AngularSwingMotorDamper", 	Joint->AngularSwingMotorDamper);

	GetProperty(Object, IGAME_INT_PROP,		"Joint_AngularTwistMotor", 			Joint->AngularTwistMotor);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_AngularTwistMotorForce", 	Joint->AngularTwistMotorForce);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_AngularTwistMotorSpring",	Joint->AngularTwistMotorSpring);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_AngularTwistMotorDamper",	Joint->AngularTwistMotorDamper);

	GetProperty(Object, IGAME_INT_PROP,		"Joint_AngularSlerpMotor", 			Joint->AngularSlerpMotor);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_AngularSlerpMotorForce", 	Joint->AngularSlerpMotorForce);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_AngularSlerpMotorSpring", 	Joint->AngularSlerpMotorSpring);
	GetProperty(Object, IGAME_FLOAT_PROP,	"Joint_AngularSlerpMotorDamper", 	Joint->AngularSlerpMotorDamper);

	Quat Quaternion;
	Point3 Point;

	IGameNode* MotorTargetPosition = NULL;
	GetProperty(Object, "Joint_MotorTargetPosition", MotorTargetPosition);
	if (MotorTargetPosition != NULL)
	{
		Joint->MotorTargetPosition = MAX_TO_ZE(MotorTargetPosition->GetObjectTM().Translation() - Node->GetObjectTM().Translation());
	}
	else
		Joint->MotorTargetPosition = ZEVector3(0.0f, 0.0f, 0.0f);

	GetProperty(Object, IGAME_POINT3_PROP,		"Joint_MotorTargetVelocity", 		(Point3&)Joint->MotorTargetVelocity);

	IGameNode* MotorTargetOrientation = NULL;
	GetProperty(Object, "Joint_MotorTargetOrientation", MotorTargetOrientation);
	if (MotorTargetOrientation != NULL)
	{
		GMatrix Orientation = MotorTargetPosition->GetObjectTM() * Node->GetObjectTM().Inverse();

		// ERROR ERROR
		//Joint->Joint.LinearMotorPosition = MAX_TO_ZE(Orientation.Rotation());
	}
	else
		Joint->MotorTargetOrientation = ZEQuaternion(1.0f, 0.0f, 0.0f, 0.0f);

	GetProperty(Object, IGAME_POINT3_PROP,		"Joint_MotorTargetAngularVelocity", 		(Point3&)Joint->MotorTargetAngularVelocity);

	return true;
}

bool ZEModelExporter::ProcessBone(IGameNode* Node)
{
	zepdLog("Processing bone \"%s\".", Node->GetName());
	IGameObject* Bone = Node->GetIGameObject();
	ZEModelFileBone* CurrentBone = ModelFile.Bones.Add();

	Point3 BoneScale = Node->GetLocalTM().Scaling();
	if (Node->GetWorldTM().Parity() == -1)
	{
		zepdError("The bone is mirrored. Bone Name : \"%s\". Mirror transformation operation is not supported. Use bone mirror operation instead.", Node->GetName());
		return false;
	}

	if (fabs(BoneScale.x - 1.0f) > 0.001 || fabs(BoneScale.y - 1.0f) > 0.001 || fabs(BoneScale.z - 1.0f) > 0.001)
		zepdWarning("The bone has been scaled. It can cause problems.");

	/*// Process Attributes
	const char* ZEType;
	if (!GetProperty(Bone, IGAME_STRING_PROP, "ZEType", ZEType) || strcmp(ZEType, "Bone") != 0)
	{
	zepdError("Bone is rejected because it does not contain ZEBoneAttributes. Node Name : \"%s\"", Node->GetName());
	return true;
	}

	// Process Bone
	if ((Node->GetNodeParent() == NULL) || (strcmp(Node->GetNodeParent()->GetIGameObject()->GetClassName(), "BoneGeometry") == 0))
	CurrentBone->ParentBone = -1;
	else
	CurrentBone->ParentBone = GetBoneId(Node->GetNodeParent());*/

	// Process Bone
	CurrentBone->ParentBone = -1;
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
				CurrentBone->ParentBone = GetBoneId(CurrentNode->GetNodeParent());
				break;
			}
	}

	strncpy(CurrentBone->Name, Node->GetName(), ZE_MDLF_MAX_NAME_SIZE);
	Box3 BB;
	Bone->GetBoundingBox(BB);
	CurrentBone->BoundingBox.Min = MAX_TO_ZE(BB.Min());
	CurrentBone->BoundingBox.Max = MAX_TO_ZE(BB.Max());

	if (CurrentBone->ParentBone != -1)
	{
		GMatrix Transform = Node->GetWorldTM() * ProcessedBones[CurrentBone->ParentBone]->GetWorldTM().Inverse();
		CurrentBone->RelativePosition = MAX_TO_ZE(Transform.Translation() * Node->GetWorldTM().Scaling());
		CurrentBone->RelativeRotation = MAX_TO_ZE(Transform.Rotation());
		CurrentBone->RelativeScale = MAX_TO_ZE(Transform.Scaling());
	}
	else
	{ 
		zepdLog("Root Bone");
		CurrentBone->RelativePosition = MAX_TO_ZE(Node->GetWorldTM().Translation() * Node->GetWorldTM().Scaling());
		CurrentBone->RelativeRotation = MAX_TO_ZE(Node->GetWorldTM().Rotation());
		CurrentBone->RelativeScale = MAX_TO_ZE(Node->GetWorldTM().Scaling());
	}

	// Process Physical Properties
	ProcessPhysicalJoint(Node, &CurrentBone->PhysicalJoint);
	ProcessPhysicalBody(Node, &CurrentBone->PhysicalBody);
	return true;
}

bool ZEModelExporter::ProcessBones()
{
	zepdLog("Processing bones...");
	Tab<IGameNode*> Nodes = Scene->GetIGameNodeByType(IGameObject::IGAME_BONE);
	const char* Type;
	for (int I = 0; I < Nodes.Count(); I++)
	{
		Type = NULL;
		if (GetProperty(Nodes[I]->GetIGameObject(), IGAME_STRING_PROP, "ZEType", Type) && strcmp(Type, "Bone") == 0)
		{
			bool Found = false;
			for (int N = 0; N < ProcessedBones.Count(); N++)
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

	for (int I = 0; I < ProcessedBones.Count(); I++)
		if (!ProcessBone(ProcessedBones[I]))
			return false;

	return true;
}

bool ZEModelExporter::ProcessMeshLODVertices(IGameNode* Node, ZEModelFileMeshLOD* ZEMeshLod)
{
	IGameMesh* Mesh = (IGameMesh*)Node->GetIGameObject();
	Mesh->InitializeData();
	IGameSkin* Skin = Mesh->GetIGameSkin();

	ZEMeshLod->MaterialId = ProcessMeshMaterial(Node->GetNodeMaterial());

	if (!Mesh->IsObjectSkinned())
		ZEMeshLod->Vertices.SetCount(Mesh->GetNumberOfFaces() * 3);
	else
		ZEMeshLod->SkinnedVertices.SetCount(Mesh->GetNumberOfFaces() * 3);

	zepdLog("Processing vertices of mesh \"%s\". Polygon Count : %d, Vertex Count : %d.", Node->GetName(), Mesh->GetNumberOfFaces(), Mesh->GetNumberOfFaces() * 3);
	Mesh->InitializeBinormalData();

	GMatrix WorldTransform = Node->GetObjectTM() * Node->GetWorldTM().Inverse();
	GMatrix InvWorldTransform = WorldTransform;

	InvWorldTransform[3][0] = 0.0f;
	InvWorldTransform[3][1] = 0.0f;
	InvWorldTransform[3][2] = 0.0f;
	InvWorldTransform[3][3] = 1.0f;
	InvWorldTransform[2][3] = 0.0f;
	InvWorldTransform[1][3] = 0.0f;
	InvWorldTransform[0][3] = 0.0f;

	bool GotError = false;
	bool BoneCountWarning = false; 
	ZEMeshLod->AffectingBoneIds.SetCount(0);
	for (int I = 0; I < Mesh->GetNumberOfFaces(); I++)
	{
		FaceEx* Face;
		Face = Mesh->GetFace(I);
		if (!Mesh->IsObjectSkinned())
		{
			for (int N = 0; N < 3; N++)
			{	
				ZEModelFileVertex* Vertex = &ZEMeshLod->Vertices[3*I + N];

				Point3 Temp;

				Temp = Mesh->GetVertex(Face->vert[N], true) * WorldTransform;
				Vertex->Position = MAX_TO_ZE(Temp); 

				Temp = Mesh->GetNormal(Face->norm[N], true) * InvWorldTransform;

				Vertex->Normal = MAX_TO_ZE(Temp.Normalize());

				int BinormalTangentIndex = Mesh->GetFaceVertexTangentBinormal(I, N);

				Temp = Mesh->GetTangent(BinormalTangentIndex) * InvWorldTransform;
				Vertex->Tangent = MAX_TO_ZE(Temp.Normalize());

				Temp = Mesh->GetBinormal(BinormalTangentIndex) * InvWorldTransform;
				Vertex->Binormal = MAX_TO_ZE(Temp.Normalize());

				Vertex->Texcoord = MAX_TO_ZE(Mesh->GetTexVertex(Face->texCoord[N]));
				Vertex++;
			}
		}
		else
		{
			for (int N = 0; N < 3; N++)
			{
				ZEModelFileSkinnedVertex* Vertex = &ZEMeshLod->SkinnedVertices[3*I + N];

				Point3 Temp;

				Temp = Mesh->GetVertex(Face->vert[N], true) * WorldTransform;
				Vertex->Position = MAX_TO_ZE(Temp); 

				Temp = Mesh->GetNormal(Face->norm[N], true) * InvWorldTransform;
				Vertex->Normal = MAX_TO_ZE(Temp.Normalize());

				int BinormalTangentIndex = Mesh->GetFaceVertexTangentBinormal(I, N);

				Temp = Mesh->GetTangent(BinormalTangentIndex) * InvWorldTransform;
				Vertex->Tangent = MAX_TO_ZE(Temp.Normalize());

				Temp = Mesh->GetBinormal(BinormalTangentIndex) * InvWorldTransform;
				Vertex->Binormal = MAX_TO_ZE(Temp.Normalize());

				Vertex->Texcoord = MAX_TO_ZE(Mesh->GetTexVertex(Face->texCoord[N]));

				int BoneCount = Skin->GetNumberOfBones(Face->vert[N]);
				if (BoneCount > 4 && !BoneCountWarning)
				{
					zepdWarning("Vertex can be affected maximum 4 bones. Exporter is goint to try reducing affecting bones by removing bones with small weights.\r\n"
						"(Treshold: Weights < 0.00001) Node Name : \"%s\".", Node->GetName()); 
					BoneCountWarning = true;
				}
				ZeroMemory(Vertex->BoneIndices, sizeof(unsigned char) * 4);
				ZeroMemory(Vertex->BoneWeights, sizeof(float) * 4);

				int BoneIndex = 0;
				for (int M = 0; M < BoneCount; M++)
				{
					int BoneId = GetBoneId(Skin->GetIGameBone(Face->vert[N], M));

					if (BoneId == -1)
					{
						zepdError("Bone that is used for skinning does not have ZEBone attributes. Node Name : \"%s\". 3ds Max Vertex Id : %d. Bone Name : %s.", Node->GetName(), Face->vert[N],  Skin->GetIGameBone(Face->vert[N], M)->GetName());
						GotError = true;
						continue;
					}

					int AffectingBoneId = -1;
					for (size_t J = 0; J < ZEMeshLod->AffectingBoneIds.GetCount(); J++)
					{
						if (ZEMeshLod->AffectingBoneIds[J] == BoneId)
						{
							AffectingBoneId = J;
							break;
						}
					}

					if (AffectingBoneId == -1)
					{
						ZEMeshLod->AffectingBoneIds.Add(BoneId);
						AffectingBoneId = ZEMeshLod->AffectingBoneIds.GetCount() - 1;
						if (ZEMeshLod->AffectingBoneIds.GetCount() >= 58)
							zepdWarning("Affecting bone count exceeds 57. Hardware accelerated skinning may not be posible. Node Name : \"%s\", Bone Count : %d.", Node->GetName(),  ZEMeshLod->AffectingBoneIds.GetCount());
					}

					float BoneWeight = Skin->GetWeight(Face->vert[N], M);
					if (BoneWeight > 0.00001)
					{
						if (BoneIndex >= 4)
						{
							zepdError("Can not reduce bone count. Node Name : \"%s\". 3ds Max Vertex Id : %d.", Node->GetName(), Face->vert[N]);
							GotError = true;
							continue;
						}
						Vertex->BoneIndices[BoneIndex] = AffectingBoneId;
						Vertex->BoneWeights[BoneIndex] = BoneWeight;
						assert(BoneId == ZEMeshLod->AffectingBoneIds[AffectingBoneId]);
						BoneIndex++;
					}
				}
				Vertex++;
			}
		}
	}

	return !GotError;
}

bool ZEModelExporter::ProcessMesh(IGameNode* Node)
{
	zepdLog("Processing mesh \"%s\".", Node->GetName());
	IGameMesh* Mesh = (IGameMesh*)Node->GetIGameObject();

	// Process Attributes
	const char* ZEType;
	int MeshLOD;

	if (!GetProperty(Mesh, IGAME_STRING_PROP, "ZEType", ZEType) || strcmp(ZEType, "Mesh") != 0)
	{
		zepdError("Mesh is rejected because it does not contain ZEMeshAttributes. Node Name : \"%s\"", Node->GetName());
		return true;
	}

	GetProperty(Mesh, IGAME_INT_PROP, "Mesh_LOD", MeshLOD);

	// Process ZEModelFileMesh
	zepdLog("Processin ZEModelFileMesh.");
	ZEModelFileMesh* CurrentMesh = NULL;
	for (size_t I = 0; I < ModelFile.Meshes.GetCount(); I++)
		if (strncmp(ModelFile.Meshes[I].Name, Node->GetName(), ZE_MDLF_MAX_NAME_SIZE) == 0)
		{
			CurrentMesh = &ModelFile.Meshes[I];
			zepdLog("Mesh \"%s\" has found in ZEModelFile. Using it as base mesh.", CurrentMesh->Name);
			break;
		}

		if (CurrentMesh == NULL)
		{
			CurrentMesh = ModelFile.Meshes.Add();
			strncpy(CurrentMesh->Name, Node->GetName(), ZE_MDLF_MAX_NAME_SIZE);
			zepdLog("Mesh \"%s\" did not found ZEModelFile. New mesh created.", CurrentMesh->Name);
		}

		zepdLog("Processing ZEModelFileLOD.");
		for (size_t I = 0; I < CurrentMesh->LODs.GetCount(); I++)
			if (CurrentMesh->LODs[I].LODLevel == MeshLOD)
			{
				zepdError("Mesh has two LOD with the same LOD Level. (Mesh Name : \"%s\", LodLevel : %d)", CurrentMesh->Name, CurrentMesh->LODs[I].LODLevel);
				return false;
			}

			if (MeshLOD == 0)
			{
				zepdLog("Node is a master LOD. Setting ZEModelFileMesh properties by using this node.");
				ProcessedMasterMeshes.Append(1, &Node);
				Mesh->InitializeData();

				CalculateLocalBoundingBox(CurrentMesh->BoundingBox, Mesh);
				CurrentMesh->IsSkinned = Mesh->IsObjectSkinned();
				CurrentMesh->Position = MAX_TO_ZE(Node->GetWorldTM().Translation());
				CurrentMesh->Rotation = MAX_TO_ZE(Node->GetWorldTM().Rotation());
				CurrentMesh->Scale = MAX_TO_ZE(Node->GetWorldTM().Scaling());

				ZEModelFileMeshLOD* CurrentLod = CurrentMesh->LODs.Add();
				CurrentLod->LODLevel = MeshLOD;
				if (!ProcessMeshLODVertices(Node, CurrentLod))
					return false;

				ProcessPhysicalBody(Node, &CurrentMesh->PhysicalBody);
			}

			ProcessedMeshes.Append(1, &Node);
			return true;
}

bool ZEModelExporter::ProcessMeshes()
{
	zepdLog("Processing Meshes...");
	Scene_ = Scene;
	Tab<IGameNode*> Nodes = Scene->GetIGameNodeByType(IGameObject::IGAME_MESH);
	for (int I = 0; I < Nodes.Count(); I++)
	{
		if (GetBoneId(Nodes[I]) == -1)
			if (!ProcessMesh(Nodes[I]))
				return false;
	}
	return true;
}

bool ZEModelExporter::ProcessAnimation()
{
	zepdLog("Processing Animations...");
	Tab<IGameNode*> AnimationEnabledBones;
	Tab<IGameNode*> AnimationEnabledMeshes;
	GMatrix Matrix;
	for (int I = 0; I < ProcessedMasterMeshes.Count(); I++)
	{
		Matrix = ProcessedMasterMeshes[I]->GetWorldTM(I * TicksPerFrame);
		for (int N = 0; N < FrameCount; N++)
			if (!(ProcessedMeshes[I]->GetWorldTM(N * TicksPerFrame) == Matrix))
			{
				AnimationEnabledMeshes.Append(1, &ProcessedMeshes[I]);
				break;
			}
	}

	for (int I = 0; I < ProcessedBones.Count(); I++)
	{
		Matrix = ProcessedBones[I]->GetWorldTM(I * TicksPerFrame);
		for (int N = 0; N < FrameCount; N++)
			if (!(ProcessedBones[I]->GetWorldTM(N * TicksPerFrame) == Matrix))
			{
				AnimationEnabledBones.Append(1, &ProcessedBones[I]);
				break;
			}
	}

	ModelFile.Animations.SetCount(1);
	ZEModelFileAnimation* CurrentAnimation = &ModelFile.Animations[0];
	strncpy(CurrentAnimation->Name, ExporterOptions.AnimationName, ZE_MDLF_MAX_NAME_SIZE);
	CurrentAnimation->Frames.SetCount(FrameCount);

	zepdLog("Total Frame Count : %d", FrameCount);
	zepdOutput("Processing Animation Frame: ");
	for (int I = 0; I < FrameCount; I++)
	{
		zepdOutput("%d ", I);
		if (I % 20 == 0)
			zepdOutput("\r\n");

		ZEModelFileAnimationFrame* CurrentFrame = &CurrentAnimation->Frames[I];
		ZEModelFileAnimationKey* Key;
		CurrentFrame->BoneKeys.SetCount(AnimationEnabledBones.Count());
		for (int N = 0; N < AnimationEnabledBones.Count(); N++)
		{
			Key = &CurrentFrame->BoneKeys[N];
			Key->ItemId = GetBoneId(AnimationEnabledBones[N]);
			if (ModelFile.Bones[Key->ItemId].ParentBone == -1)
			{
				GMatrix Matrix = AnimationEnabledBones[N]->GetWorldTM(I * TicksPerFrame);
				Key->Position = MAX_TO_ZE(Matrix.Translation());
				Key->Rotation = MAX_TO_ZE(Matrix.Rotation());
				Key->Scale = MAX_TO_ZE(Matrix.Scaling());
			}
			else
			{
				GMatrix Matrix = AnimationEnabledBones[N]->GetWorldTM(I * TicksPerFrame) * 
					ProcessedBones[ModelFile.Bones[Key->ItemId].ParentBone]->GetWorldTM(I * TicksPerFrame).Inverse();
				//AnimationEnabledBones[N]->GetNodeParent()->GetWorldTM(I * TicksPerFrame).Inverse();
				Key->Position = MAX_TO_ZE(Matrix.Translation());
				Key->Rotation = MAX_TO_ZE(Matrix.Rotation());
				Key->Scale = MAX_TO_ZE(Matrix.Scaling());
			}
		}

		CurrentFrame->MeshKeys.SetCount(AnimationEnabledMeshes.Count());
		for (int N = 0; N < AnimationEnabledMeshes.Count(); N++)
		{
			Key = &CurrentFrame->MeshKeys[N];
			Key->ItemId = GetMeshId(AnimationEnabledMeshes[N]);
			GMatrix Matrix = AnimationEnabledMeshes[N]->GetWorldTM(I * TicksPerFrame);
			Key->Position = MAX_TO_ZE(Matrix.Translation());
			Key->Rotation = MAX_TO_ZE(Matrix.Rotation());
			Key->Scale = MAX_TO_ZE(Matrix.Scaling());
		}
	}
	zepdOutput("\r\n");
	return true;
}

bool ZEModelExporter::DumpPropertyContainer(IExportEntity* Node)
{
	if (Node == NULL)
	{
		return false;
	}

	IPropertyContainer* Properties = Node->GetIPropertyContainer();
	if (Properties == NULL)
	{
		zepdError("There is no property container. Class Name : \"%s\".", Node->GetClassName());
		return false;
	}
	zepdOutput("Dumping properties of entity. Class Name : \"%s\".\r\n", Node->GetClassName());
	for (int I = 0; I < Properties->GetNumberOfProperties(); I++)
	{
		IGameProperty* Property = Properties->GetProperty(I);
		zepdOutput("Property %d. Name : \"%s\", Index : %d, Animated : %s, ", I, Property->GetName(), Property->GetParamBlockIndex(), (Property->IsPropAnimated() ? "Yes" : "No"));

		float FloatValue;
		int IntValue;
		Point3 Point3Value;
		Point4 Point4Value;
		const char* StringValue;
		IParamBlock2* ParamBlock;
		int ParamId;
		ParamType2 ParamType;
		switch(Property->GetType())
		{
		case IGAME_UNKNOWN_PROP:
			ParamBlock =  Property->GetMaxParamBlock2();
			ParamId = ParamBlock->IndextoID(Property->GetParamBlockIndex());
			ParamType = ParamBlock->GetParameterType(ParamId);
			switch(ParamBlock->GetParameterType(ParamId))
			{
			case TYPE_INODE:
				zepdOutput("Type : INode, Value : \"%s\".", (ParamBlock->GetINode(ParamId, 0, 0)->GetName() != NULL ? ParamBlock->GetINode(ParamId, 0, 0)->GetName() : "NULL"));
				break;
			case TYPE_INODE_TAB:
				zepdOutput("Type : INodeTab, Values : [");
				for (int N = 0 ; N < ParamBlock->Count(ParamId); N++)
					zepdOutput("\"%s\", ", ParamBlock->GetINode(ParamId, 0, N)->GetName());
				zepdOutput("].\r\n");
				break;
			};
			break;
		case IGAME_FLOAT_PROP:
			Property->GetPropertyValue(FloatValue);
			zepdOutput("Type : FLOAT, Value : %f.\r\n", FloatValue);
			break;
		case IGAME_POINT3_PROP:
			Property->GetPropertyValue(Point3Value);
			zepdOutput("Type : POINT3, Value : <%f, %f, %f>\r\n", Point3Value.x, Point3Value.y, Point3Value.z);
			break;
		case IGAME_INT_PROP:
			Property->GetPropertyValue(IntValue);
			zepdOutput("Type : INT, Value : %d.\r\n", IntValue);
			break;
		case IGAME_STRING_PROP:
			Property->GetPropertyValue(StringValue);
			zepdOutput("Type : STRING, Value : \"%s\".\r\n",StringValue);
			break;
		case IGAME_POINT4_PROP:
			Property->GetPropertyValue(Point4Value);
			zepdOutput("Type : POINT4, Value : <%f, %f, %f, %f>.\r\n", Point4Value.x, Point4Value.y, Point4Value.z, Point4Value.w);
			break;
		default:
			zepdOutput("Property Type : ERROR. \r\n.");
		}
	}
	return true;
}
