//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelResource.cpp
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

#include "ZEError.h"
#include "ZEModelResource.h"
#include "ZEModelFileFormat.h"
#include "ZECore/ZEResourceManager.h"
#include "ZEGraphics/ZEVertexBuffer.h"
#include "ZEGraphics/ZEFixedMaterial.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZETexture2DResource.h"

#include <memory.h>
#include <string.h>
#include <float.h>

static ZEString ConstructResourcePath(const ZEString& Path)
{
	ZEString NewString = Path;
	unsigned int ConstLength = strlen("resources\\") - 1;

	if (Path[0] == '\\' || Path[0] == '/')
		NewString = NewString.SubString(1, Path.GetLength() - 1);

	// If it is guaranteed that there is no "resources\\" string in beginning
	if (NewString.GetLength() - 1 < ConstLength)
	{
		NewString.Insert(0, "resources\\");
		return NewString;
	}
	// Else check if there is "resources\\" in the beginning
	else if (_stricmp("resources\\", Path.SubString(0, ConstLength)) != 0)
	{
		NewString.Insert(0, "resources\\");
		return NewString;
	}

	return NewString;
}

ZEStaticVertexBuffer* ZEModelResourceMeshLOD::GetSharedVertexBuffer() const
{
	if (SharedVertexBuffer == NULL)
		if (this->SkinnedVertices.GetCount() != 0)
		{
			((ZEModelResourceMeshLOD*)this)->SharedVertexBuffer = zeGraphics->CreateStaticVertexBuffer();
			SharedVertexBuffer->Create(sizeof(ZESkinnedModelVertex) * SkinnedVertices.GetCount());
			
			void* Buffer = SharedVertexBuffer->Lock();
				memcpy(Buffer, SkinnedVertices.GetConstCArray(), sizeof(ZESkinnedModelVertex) * SkinnedVertices.GetCount());
			SharedVertexBuffer->Unlock();
		}
		else if (Vertices.GetCount() != 0)
		{
			((ZEModelResourceMeshLOD*)this)->SharedVertexBuffer = zeGraphics->CreateStaticVertexBuffer();
			SharedVertexBuffer->Create(sizeof(ZEModelVertex) * Vertices.GetCount());
			
			void* Buffer = SharedVertexBuffer->Lock();
				memcpy(Buffer, Vertices.GetConstCArray(), sizeof(ZEModelVertex) * Vertices.GetCount());
			SharedVertexBuffer->Unlock();
		}

	return SharedVertexBuffer;
}

ZEStaticVertexBuffer* ZEModelResourceMeshLOD::CreatePrivateVertexBuffer() const
{
	ZEStaticVertexBuffer* VertexBuffer = NULL;
	if (SharedVertexBuffer == NULL)
		if (this->SkinnedVertices.GetCount() != 0)
		{
			VertexBuffer = zeGraphics->CreateStaticVertexBuffer();
			VertexBuffer->Create(sizeof(ZESkinnedModelVertex) * SkinnedVertices.GetCount());

			void* Buffer = VertexBuffer->Lock();
				memcpy(Buffer, SkinnedVertices.GetConstCArray(), sizeof(ZESkinnedModelVertex) * SkinnedVertices.GetCount());
			VertexBuffer->Unlock();
		}
		else if (Vertices.GetCount() != 0)
		{
			VertexBuffer = zeGraphics->CreateStaticVertexBuffer();
			VertexBuffer->Create(sizeof(ZEModelVertex) * Vertices.GetCount());
			
			void* Buffer = VertexBuffer->Lock();
				memcpy(Buffer, Vertices.GetConstCArray(), sizeof(ZEModelVertex) * Vertices.GetCount());
			VertexBuffer->Unlock();
		}
	return VertexBuffer;
}

ZEModelResourceMeshLOD::ZEModelResourceMeshLOD()
{
	SharedVertexBuffer = NULL;
	LODLevel = 0;
	MaterialId = -1;
}

ZEModelResourceMeshLOD::~ZEModelResourceMeshLOD()
{
	if (SharedVertexBuffer != NULL)
		SharedVertexBuffer->Destroy();
}


//////////////////////////////////////////////////////////////// READ ////////////////////////////////////////////////////////////////

static const ZETexture2D* ManageModelMaterialTextures(char* FileName, ZESmartArray<ZETexture2DResource*>& TextureResources)
{
	if (strncmp(FileName, "", ZE_MDLF_MAX_FILENAME_SIZE) == 0)
		return NULL;

	for (size_t I = 0; I < TextureResources.GetCount(); I++)
	{
		//if (strnicmp(TextureResources[I]->GetFileName(), FileName, ZE_MDLF_MAX_FILENAME_SIZE) == 0)
		if (TextureResources[I]->GetFileName() == ZEString(FileName))
			return TextureResources[I]->GetTexture();
	}

	ZETexture2DResource* NewTextureResource = ZETexture2DResource::LoadSharedResource(FileName);
	if (NewTextureResource == NULL)
	{
		zeError("Can not load texture file. (FileName : \"%s\")", FileName);
		return NULL;
	}
	TextureResources.Add(NewTextureResource);
	return NewTextureResource->GetTexture();
}

#define ZE_MFSC_SKINTRANSFORM				1
#define ZE_MFSC_DIFFUSEMAP					2
#define ZE_MFSC_NORMALMAP					4
#define ZE_MFSC_SPECULARMAP					8
#define ZE_MFSC_EMMISIVEMAP					16
#define ZE_MFSC_OCAPASITYMAP				32
#define ZE_MFSC_DETAILDIFFUSEMAP			64
#define ZE_MFSC_DETAILNORMALMAP				128
#define ZE_MFSC_REFLECTION					256
#define ZE_MFSC_REFRACTION					512
#define ZE_MFSC_LIGHTMAP					1024
#define ZE_MFSC_DISTORTIONMAP				2048

static bool ReadMaterialsFromFile(ZEModelResource* Model, ZEFile* ResourceFile)
{
	for (size_t I = 0; I < Model->Materials.GetCount(); I++)
	{
		ZEModelFileMaterialChunk MaterialChunk;

		ResourceFile->Read(&MaterialChunk, sizeof(ZEModelFileMaterialChunk), 1);
		if(MaterialChunk.ChunkId != ZE_MDLF_MATERIAL_CHUNKID)
		{
			zeError("Corrupted ZEModel file. Materials chunk id does not matches.");
			return false;
		}

		ZEFixedMaterial* CurrentMaterial = ZEFixedMaterial::CreateInstance();
		Model->Materials[I] = CurrentMaterial;

		CurrentMaterial->SetDiffuseEnabled(true);
		CurrentMaterial->SetAmbientEnabled(false);
		CurrentMaterial->SetSpecularEnabled(true);
		CurrentMaterial->SetEmmisiveEnabled(true);
		
		CurrentMaterial->SetTwoSided(true);//MaterialChunk.TwoSided);
		CurrentMaterial->SetLightningEnabled(MaterialChunk.LightningEnabled);
		CurrentMaterial->SetWireframe(MaterialChunk.Wireframe);
		CurrentMaterial->SetTransparancyMode(ZE_MTM_NONE);//MaterialChunk.Transparant ? ZE_MTM_ADDAPTIVE: ZE_MTM_NOTRANSPARACY);
		CurrentMaterial->SetAlphaCullEnabled(false);
		CurrentMaterial->SetAlphaCullLimit(0.1f);
		CurrentMaterial->SetOpacity(1.0f);
		CurrentMaterial->SetDiffuseSubSurfaceScatteringFactor(0.0f);
		CurrentMaterial->SetOpacityEnabled(true);
		CurrentMaterial->SetOpacityComponent(ZE_MOC_BASE_MAP_ALPHA);

		CurrentMaterial->SetAmbientColor(MaterialChunk.AmbientColor/* ZEVector3(0.5, 0.5, 0.5)*/);
		CurrentMaterial->SetDiffuseColor(MaterialChunk.DiffuseColor);
		CurrentMaterial->SetSpecularColor(MaterialChunk.SpecularColor);
		CurrentMaterial->SetEmmisiveColor(MaterialChunk.EmmisiveColor);
		CurrentMaterial->SetEmmisiveFactor(MaterialChunk.EmmisiveFactor);
		CurrentMaterial->SetSpecularFactor(MaterialChunk.SpecularFactor);
		CurrentMaterial->SetOpacity(MaterialChunk.Opasity);
		CurrentMaterial->SetReflectionFactor(MaterialChunk.ReflectionFactor);
		CurrentMaterial->SetRefractionFactor(MaterialChunk.RefractionFactor);
		CurrentMaterial->SetDetailMapTiling(MaterialChunk.DetailMapTiling);

		CurrentMaterial->SetBaseMap(ManageModelMaterialTextures(MaterialChunk.DiffuseMap, Model->TextureResources));
		
		CurrentMaterial->SetNormalMapEnabled((MaterialChunk.ShaderComponents & ZE_MFSC_NORMALMAP) != 0);
		CurrentMaterial->SetNormalMap(ManageModelMaterialTextures(MaterialChunk.NormalMap, Model->TextureResources));
		CurrentMaterial->SetSpecularMap(ManageModelMaterialTextures(MaterialChunk.SpecularMap, Model->TextureResources));
		CurrentMaterial->SetEmmisiveMap(ManageModelMaterialTextures(MaterialChunk.EmmisiveMap, Model->TextureResources));
		CurrentMaterial->SetOpacityMap(ManageModelMaterialTextures(MaterialChunk.OpasityMap, Model->TextureResources));
		
		//CurrentMaterial->SetDetailMapEnabled(MaterialChunk.ShaderComponents & ZE_MFSC_DETAILNORMALMAP);
		//CurrentMaterial->SetDetailBaseMap(ManageModelMaterialTextures(MaterialChunk.DetailMap, Model->TextureResources));
		//CurrentMaterial->SetDetailNormalMap(ManageModelMaterialTextures(MaterialChunk.DetailNormalMap, Model->TextureResources));
		CurrentMaterial->SetReflectionEnabled(false);
		CurrentMaterial->SetRefractionEnabled(false);
		CurrentMaterial->SetEnvironmentMap(NULL);//ManageMapMaterialTextures(MaterialChunk.EnvironmentMap, TextureResources));

		CurrentMaterial->SetLightMapEnabled((MaterialChunk.ShaderComponents & ZE_MFSC_LIGHTMAP) != 0);
		CurrentMaterial->SetLightMap(ManageModelMaterialTextures(MaterialChunk.LightMap, Model->TextureResources));

 		CurrentMaterial->UpdateMaterial();
	}
	return true;
}

static bool ReadPhysicalBodyFromFile(ZEModelResourcePhysicalBody* Body, ZEFile* ResourceFile)
{
	ZEModelFilePhysicalBodyChunk BodyChunk;
	ResourceFile->Read(&BodyChunk, sizeof(ZEModelFilePhysicalBodyChunk), 1);

	if (BodyChunk.ChunkId != ZE_MDLF_PHYSICAL_BODY_CHUNKID)
	{
		zeError("Corrupted ZEModel file. Physical body chunk id does not matches.");
		return false;
	}

	Body->Type				= (ZEModelResourcePhysicalBodyType)BodyChunk.Type;
	Body->Enabled			= BodyChunk.Enabled;
	Body->Mass				= BodyChunk.Mass;

	//H.C -EDIT- Kinematic Attribute Removed from ModelFileFormat
	//Body->Kinematic			= BodyChunk.Kinematic;
	Body->AngularDamping	= BodyChunk.AngularDamping;
	Body->LinearDamping		= BodyChunk.LinearDamping;
	Body->MassCenter		= BodyChunk.MassCenter;

	Body->Shapes.SetCount(BodyChunk.ShapeCount);
	for (size_t I = 0; I < Body->Shapes.GetCount(); I++)
	{
		ZEModelResourcePhysicalShape* Shape = &Body->Shapes[I];

		ZEModelFilePhysicalShapeChunk ShapeChunk;
		ResourceFile->Read(&ShapeChunk, sizeof(ZEModelFilePhysicalShapeChunk), 1);

		if(ShapeChunk.ChunkId != ZE_MDLF_PHYSICAL_SHAPE_CHUNKID)
		{
			zeError("Corrupted ZEModel file. Physical shape chunk id does not matches.");
			return false;
		}

		Shape->Type						= (ZEPhysicalShapeType)ShapeChunk.Type;
		Shape->Position					= ShapeChunk.Position;
		Shape->Rotation					= ShapeChunk.Rotation;
		Shape->StaticFriction			= ShapeChunk.StaticFriction;
		Shape->DynamicFriction			= ShapeChunk.DynamicFriction;
		Shape->Restitution				= ShapeChunk.Restitution;

		switch (Shape->Type)
		{
			case ZE_MFPST_BOX:
			{
				Shape->Box.Width		= ShapeChunk.Box.Width;
				Shape->Box.Height		= ShapeChunk.Box.Height;
				Shape->Box.Length		= ShapeChunk.Box.Length;
				break;
			}
			case ZE_MFPST_SPHERE:
			{
				Shape->Sphere.Radius	= ShapeChunk.Sphere.Radius;
				break;
			}
			case ZE_MFPST_CAPSULE:
			{
				Shape->Capsule.Height	= ShapeChunk.Capsule.Height;
				Shape->Capsule.Radius	= ShapeChunk.Capsule.Radius;
				break;
			}
			case ZE_MFPST_CYLINDER:
			{
				Shape->Cylinder.Height	= ShapeChunk.Cylinder.Height;
				Shape->Cylinder.Radius	= ShapeChunk.Cylinder.Radius;
				break;
			}
			case ZE_MFPST_CONVEX:
			{
				ZEUInt32 ChunkId;
				ResourceFile->Read(&ChunkId, sizeof(ZEUInt32), 1);
				if (ChunkId != ZE_MDLF_PHYSICAL_SHAPE_VERTEX_CHUNKID)
				{
					zeError("Corrupted ZEModel file. Physical vertex chunk id does not matches.");
					return false;
				}
				
				Shape->Convex.Vertices.SetCount(ShapeChunk.Convex.VertexCount);
				ResourceFile->Read(Shape->Convex.Vertices.GetCArray(), sizeof(ZEVector3), Shape->Convex.Vertices.GetCount());
				break;
			}
			/*case _ZE_PST_TRIMESH:
			{
				//vertices
				ZEUInt32 ChunkId;
				ResourceFile->Read(&ChunkId, sizeof(ZEUInt32), 1);
				if (ChunkId != ZE_MDLF_PHYSICAL_BODY_SHAPE_VERTEX_CHUNKID)
				{
					zeError("Corrupted ZEModel file. Physical vertex chunk id does not matches.");
					return false;
				}
				
				Shape->TriMesh.Vertices.SetCount(ShapeChunk.TriMesh.VertexCount);
				ResourceFile->Read(Shape->TriMesh.Vertices.GetCArray(), sizeof(ZEVector3), Shape->TriMesh.Vertices.GetCount());
				
				//indices
				ResourceFile->Read(&ChunkId, sizeof(ZEUInt32), 1);
				if (ChunkId != ZE_MDLF_PHYSICAL_BODY_SHAPE_INDEX_CHUNKID)
				{
					zeError("Corrupted ZEModel file. Physical index chunk id does not matches.");
					return false;
				}
				
				Shape->TriMesh.Indices.SetCount(ShapeChunk.TriMesh.IndexCount);
				ResourceFile->Read(Shape->TriMesh.Indices.GetCArray(), sizeof(ZEModelFilePhysicalPolygonChunk), Shape->TriMesh.Indices.GetCount());
				break;
			}*/
			default:
				zeError("Wrong physical shape type. (Physical Shape : %d)", Shape->Type);
				return false;
		}
	}
	return true;
}

static void CalculateBoundingBox(ZEModelResourceMesh* Mesh)
{
	Mesh->BoundingBox.Min = ZEVector3(FLT_MAX, FLT_MAX, FLT_MAX);
	Mesh->BoundingBox.Max = ZEVector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (size_t I = 0; I < Mesh->LODs.GetCount(); I++)
	{
		ZEModelResourceMeshLOD* CurrentLOD = &Mesh->LODs[I];
		if (Mesh->IsSkinned)
		{
			for (size_t N = 0; N < CurrentLOD->SkinnedVertices.GetCount(); N++)
			{
				ZEVector3& Position = CurrentLOD->SkinnedVertices[N].Position;

				if (Position.x < Mesh->BoundingBox.Min.x) Mesh->BoundingBox.Min.x = Position.x;
				if (Position.y < Mesh->BoundingBox.Min.y) Mesh->BoundingBox.Min.y = Position.y;
				if (Position.z < Mesh->BoundingBox.Min.z) Mesh->BoundingBox.Min.z = Position.z;
				if (Position.x > Mesh->BoundingBox.Max.x) Mesh->BoundingBox.Max.x = Position.x;
				if (Position.y > Mesh->BoundingBox.Max.y) Mesh->BoundingBox.Max.y = Position.y;
				if (Position.z > Mesh->BoundingBox.Max.z) Mesh->BoundingBox.Max.z = Position.z;
			}
		}
		else
		{
			for (size_t N = 0; N < CurrentLOD->Vertices.GetCount(); N++)
			{
				ZEVector3& Position = CurrentLOD->Vertices[N].Position;

				if (Position.x < Mesh->BoundingBox.Min.x) Mesh->BoundingBox.Min.x = Position.x;
				if (Position.y < Mesh->BoundingBox.Min.y) Mesh->BoundingBox.Min.y = Position.y;
				if (Position.z < Mesh->BoundingBox.Min.z) Mesh->BoundingBox.Min.z = Position.z;
				if (Position.x > Mesh->BoundingBox.Max.x) Mesh->BoundingBox.Max.x = Position.x;
				if (Position.y > Mesh->BoundingBox.Max.y) Mesh->BoundingBox.Max.y = Position.y;
				if (Position.z > Mesh->BoundingBox.Max.z) Mesh->BoundingBox.Max.z = Position.z;
			}
		}
	}
}

static bool ReadMeshesFromFile(ZEModelResource* Model, ZEFile* ResourceFile)
{
	for (size_t I = 0; I < Model->Meshes.GetCount(); I++)
	{
		ZEModelResourceMesh* Mesh = &Model->Meshes[I];

		ZEModelFileMeshChunk MeshChunk;
		ResourceFile->Read(&MeshChunk, sizeof(ZEModelFileMeshChunk), 1);
		
		if(MeshChunk.ChunkId != ZE_MDLF_MESH_CHUNKID)
		{
			zeError("Corrupted ZEModel file. Mesh's chunk id does not matches.");
			return false;
		}

		strncpy(Mesh->Name, MeshChunk.Name, ZE_MDLF_MAX_NAME_SIZE);
		Mesh->IsSkinned		= MeshChunk.IsSkinned;
		Mesh->Position		= MeshChunk.Position;
		Mesh->Rotation		= MeshChunk.Rotation;
		Mesh->Scale			= MeshChunk.Scale;
		//should be taken from scene or map file !!!!!!!
		Mesh->PhysicalBody.Position = ZEVector3(0,1,0);
		Mesh->PhysicalBody.Orientation = ZEQuaternion::Identity;

		Mesh->PhysicalBody.Position = ZEVector3(0,1,0);
		Mesh->PhysicalBody.Orientation = ZEQuaternion::Identity;

		Mesh->LODs.SetCount(MeshChunk.LODCount);
		for (size_t I = 0; I < Mesh->LODs.GetCount(); I++)
		{
			ZEModelResourceMeshLOD* LOD = &Mesh->LODs[I];
			ZEModelFileMeshLODChunk MeshLODChunk;
			ResourceFile->Read(&MeshLODChunk, sizeof(ZEModelFileMeshLODChunk), 1);
			if (MeshLODChunk.ChunkId != ZE_MDLF_MESH_LOD_CHUNKID)
			{
				zeError("Corrupted ZEModel file. Mesh LOD's chunk id does not matches.");
				return false;			
			}

			LOD->MaterialId = MeshLODChunk.MaterialId;
			LOD->LODLevel = MeshLODChunk.LODLevel;
			if(MeshChunk.IsSkinned)
			{
				LOD->AffectingBoneIds.SetCount(MeshLODChunk.AffectingBoneCount);
				ResourceFile->Read(LOD->AffectingBoneIds.GetCArray(), sizeof(ZEUInt32), LOD->AffectingBoneIds.GetCount());

				LOD->SkinnedVertices.SetCount(MeshLODChunk.VertexCount);				
				ResourceFile->Read(LOD->SkinnedVertices.GetCArray(), sizeof(ZESkinnedModelVertex),  LOD->SkinnedVertices.GetCount());
			}
			else
			{
				LOD->Vertices.SetCount(MeshLODChunk.VertexCount);				
				ResourceFile->Read(LOD->Vertices.GetCArray(), sizeof(ZEModelVertex),  LOD->Vertices.GetCount());
			}
		}

		CalculateBoundingBox(Mesh);

		if (MeshChunk.HasPhysicalBody)
		{
			if (!ReadPhysicalBodyFromFile(&Mesh->PhysicalBody, ResourceFile))
				return false;
		}
		else
		{
			Mesh->PhysicalBody.Shapes.Clear();
			memset(&Mesh->PhysicalBody, 0, sizeof(ZEModelResourcePhysicalBody));
		}
	}
	return true;
}

static bool ReadPhysicalJointFromFile(ZEModelResourcePhysicalJoint* Joint, ZEFile* ResourceFile)
{
	ZEModelFilePhysicalJointChunk JointChunk;
	ResourceFile->Read(&JointChunk, sizeof(ZEModelFilePhysicalJointChunk), 1);

	if (JointChunk.ChunkId != ZE_MDLF_PHYSICAL_JOINT_CHUNKID)
	{
		zeError("Corrupted ZEModel file. Physical joint's chunk id does not matches.");
		return false;
	}

	Joint->JointType					= (ZEPhysicalJointType)JointChunk.JointType;
	Joint->Enabled						= JointChunk.Enabled;
	Joint->Body1Id						= JointChunk.Body1Id;
	Joint->Body2Id						= JointChunk.Body2Id;
	Joint->Breakable					= JointChunk.Breakable;
	Joint->BreakForce					= JointChunk.BreakForce;
	Joint->BreakTorque					= JointChunk.BreakTorque;
	Joint->CollideBodies				= JointChunk.CollideBodies;
	Joint->UseGlobalAnchorAxis			= JointChunk.UseGlobalAnchorAxis;
	Joint->GlobalAnchor					= JointChunk.GlobalAnchor;
	Joint->GlobalAxis					= JointChunk.GlobalAxis;
	Joint->LocalAnchor1					= JointChunk.LocalAnchor1;
	Joint->LocalAnchor2					= JointChunk.LocalAnchor2;
	Joint->LocalAxis1					= JointChunk.LocalAxis1;
	Joint->LocalAxis2					= JointChunk.LocalAxis2;

	Joint->XMotion						= (ZEPhysicalJointMotion)JointChunk.XMotion;
	Joint->YMotion						= (ZEPhysicalJointMotion)JointChunk.YMotion;
	Joint->ZMotion						= (ZEPhysicalJointMotion)JointChunk.ZMotion;

	Joint->LinearLimitDamping			= JointChunk.LinearLimitDamping;
	Joint->LinearLimitRestitution		= JointChunk.LinearLimitRestitution;
	Joint->LinearLimitSpring			= JointChunk.LinearLimitSpring;
	Joint->LinearLimitValue				= JointChunk.LinearLimitValue;

	Joint->Swing1Motion					= (ZEPhysicalJointMotion)JointChunk.Swing1Motion;
	Joint->Swing1LimitDamping			= JointChunk.Swing1LimitDamping;
	Joint->Swing1LimitRestitution		= JointChunk.Swing1LimitRestitution;
	Joint->Swing1LimitSpring			= JointChunk.Swing1LimitSpring;
	Joint->Swing1LimitValue				= JointChunk.Swing1LimitValue;

	Joint->Swing2Motion					= (ZEPhysicalJointMotion)JointChunk.Swing2Motion;
	Joint->Swing2LimitDamping			= JointChunk.Swing2LimitDamping;
	Joint->Swing2LimitRestitution		= JointChunk.Swing2LimitRestitution;
	Joint->Swing2LimitSpring			= JointChunk.Swing2LimitSpring;
	Joint->Swing2LimitValue				= JointChunk.Swing2LimitValue;

	Joint->TwistMotion					= (ZEPhysicalJointMotion)JointChunk.TwistMotion;
	Joint->TwistLowLimitDamping			= JointChunk.TwistLowLimitDamping;
	Joint->TwistLowLimitValue			= JointChunk.TwistLowLimitValue;
	Joint->TwistLowLimitValue			= JointChunk.TwistLowLimitValue;
	Joint->TwistLowLimitRestitution		= JointChunk.TwistLowLimitRestitution;
	Joint->TwistLowLimitSpring			= JointChunk.TwistLowLimitSpring;
	Joint->TwistHighLimitDamping		= JointChunk.TwistHighLimitDamping;
	Joint->TwistHighLimitValue			= JointChunk.TwistHighLimitValue;
	Joint->TwistHighLimitValue			= JointChunk.TwistHighLimitValue;
	Joint->TwistHighLimitRestitution	= JointChunk.TwistHighLimitRestitution;
	Joint->TwistHighLimitSpring			= JointChunk.TwistHighLimitSpring;

	Joint->MotorTargetPosition			= JointChunk.MotorTargetPosition;
	Joint->MotorTargetOrientation		= JointChunk.MotorTargetOrientation;
	Joint->MotorTargetVelocity			= JointChunk.MotorTargetVelocity;
	Joint->MotorTargetAngularVelocity	= JointChunk.MotorTargetAngularVelocity;

	Joint->LinearXMotor					= (ZEPhysicalJointMotorType)JointChunk.LinearXMotor;
	Joint->LinearXMotorDamper			= JointChunk.LinearXMotorDamper;
	Joint->LinearXMotorForce			= JointChunk.LinearXMotorForce;
	Joint->LinearXMotorSpring			= JointChunk.LinearXMotorSpring;

	Joint->LinearYMotor					= (ZEPhysicalJointMotorType)JointChunk.LinearYMotor;
	Joint->LinearYMotorDamper			= JointChunk.LinearYMotorDamper;
	Joint->LinearYMotorForce			= JointChunk.LinearYMotorForce;
	Joint->LinearYMotorSpring			= JointChunk.LinearYMotorSpring;

	Joint->LinearZMotor					= (ZEPhysicalJointMotorType)JointChunk.LinearZMotor;
	Joint->LinearZMotorDamper			= JointChunk.LinearZMotorDamper;
	Joint->LinearZMotorForce			= JointChunk.LinearZMotorForce;
	Joint->LinearZMotorSpring			= JointChunk.LinearZMotorSpring;

	Joint->AngularSwingMotor			= (ZEPhysicalJointMotorType)JointChunk.AngularSwingMotor;
	Joint->AngularSwingMotorDamper		= JointChunk.AngularSwingMotorDamper;
	Joint->AngularSwingMotorForce		= JointChunk.AngularSwingMotorForce;
	Joint->AngularSwingMotorSpring		= JointChunk.AngularSwingMotorSpring;

	Joint->AngularTwistMotor			= (ZEPhysicalJointMotorType)JointChunk.AngularTwistMotor;
	Joint->AngularTwistMotorDamper		= JointChunk.AngularTwistMotorDamper;
	Joint->AngularTwistMotorForce		= JointChunk.AngularTwistMotorForce;
	Joint->AngularTwistMotorSpring		= JointChunk.AngularTwistMotorSpring;

	Joint->AngularSlerpMotor			= (ZEPhysicalJointMotorType)JointChunk.AngularSlerpMotor;
	Joint->AngularSlerpMotorDamper		= JointChunk.AngularSlerpMotorDamper;
	Joint->AngularSlerpMotorForce		= JointChunk.AngularSlerpMotorForce;
	Joint->AngularSlerpMotorSpring		= JointChunk.AngularSlerpMotorSpring;

	return true;
}

static void ProcessBones(ZEModelResource* Model, ZEModelResourceBone* Bone, int BoneId)
{
	ZEMatrix4x4::CreateOrientation(Bone->RelativeTransform, Bone->RelativePosition, Bone->RelativeRotation, Bone->RelativeScale);

	if (Bone->ParentBone != -1)
	{

		ZEMatrix4x4::Multiply(Bone->ForwardTransform, Model->Bones[Bone->ParentBone].ForwardTransform, Bone->RelativeTransform);
		ZEMatrix4x4::Inverse(Bone->InverseTransform, Bone->ForwardTransform);
	}
	else
	{
		Bone->ForwardTransform = Bone->RelativeTransform;
		ZEMatrix4x4::Inverse(Bone->InverseTransform, Bone->ForwardTransform);
	}

	for (size_t I = 0; I < Model->Bones.GetCount(); I++)
		if (Model->Bones[I].ParentBone == BoneId)
			ProcessBones(Model, &Model->Bones[I], I);
}

static bool ReadBonesFromFile(ZEModelResource* Model, ZEFile* ResourceFile)
{
	for (size_t I = 0; I < Model->Bones.GetCount(); I++)
	{
		ZEModelResourceBone* Bone = &Model->Bones[I];

		ZEModelFileBoneChunk BoneChunk;
		ResourceFile->Read(&BoneChunk, sizeof(ZEModelFileBoneChunk), 1);

		if(BoneChunk.ChunkId != ZE_MDLF_BONE_CHUNKID)
		{
			zeError("Corrupted ZEModel file. Bone chunk id does not matches.");
			return false;
		}

		strncpy(Bone->Name, BoneChunk.Name, ZE_MDLF_MAX_NAME_SIZE);
		Bone->BoundingBox = BoneChunk.BoundingBox;
		Bone->ParentBone = BoneChunk.ParentBone;
		Bone->RelativePosition = BoneChunk.RelativePosition;
		Bone->RelativeRotation = BoneChunk.RelativeRotation;
		Bone->RelativeScale = BoneChunk.RelativeScale;
		if (BoneChunk.HasPhysicalBody)
			ReadPhysicalBodyFromFile(&Bone->PhysicalBody, ResourceFile);
		else
		{
			memset(&Bone->PhysicalBody, 0, sizeof(ZEModelResourcePhysicalBody));
			Bone->PhysicalBody.Type = ZE_MRPBT_NONE;
		}


		if (BoneChunk.HasPhysicalJoint)
			ReadPhysicalJointFromFile(&Bone->PhysicalJoint, ResourceFile);
		else
		{
			memset(&Bone->PhysicalJoint, 0, sizeof(ZEModelResourcePhysicalJoint));
			Bone->PhysicalJoint.JointType = ZE_PJT_NONE;
		}
	}

	for (size_t I = 0; I < Model->Bones.GetCount(); I++)
		if (Model->Bones[I].ParentBone == -1)
			ProcessBones(Model, &Model->Bones[I], I);
	return true;
}

static bool ReadAnimationsFromFile(ZEModelResource* Model, ZEFile* ResourceFile)
{
	for (size_t I = 0; I < Model->Animations.GetCount(); I++)
	{
		ZEModelResourceAnimation* Animation = &Model->Animations[I];

		ZEModelFileAnimationChunk AnimationChunk;
		ResourceFile->Read(&AnimationChunk, sizeof(ZEModelFileAnimationChunk), 1);

		if(AnimationChunk.ChunkId != ZE_MDLF_ANIMATION_CHUNKID)
		{
			zeError("Corrupted ZEModel file. Animation chunk id does not matches.");
			return false;
		}

		Animation->Frames.SetCount(AnimationChunk.FrameCount);
		strncpy(Animation->Name, AnimationChunk.Name, ZE_MDLF_MAX_NAME_SIZE);

		for (size_t I = 0; I < Animation->Frames.GetCount(); I++)
		{
			ZEModelResourceAnimationFrame* CurrentAnimationFrame = &Animation->Frames[I];

			ZEModelFileAnimationFrameChunk FileAnimationFrame;
			ResourceFile->Read(&FileAnimationFrame, sizeof(ZEModelFileAnimationFrameChunk), 1);

			if(FileAnimationFrame.ChunkId != ZE_MDLF_ANIMATION_KEYFRAME_CHUNKID)
			{
				zeError("Corrupted ZEModel file. Animation frame chunk id does not matches.");
				return false;
			}

			CurrentAnimationFrame->BoneKeys.SetCount(FileAnimationFrame.BoneKeyCount);
			ResourceFile->Read(CurrentAnimationFrame->BoneKeys.GetCArray(), sizeof(ZEModelResourceAnimationKey), CurrentAnimationFrame->BoneKeys.GetCount());

			CurrentAnimationFrame->MeshKeys.SetCount(FileAnimationFrame.MeshKeyCount);		
			ResourceFile->Read(CurrentAnimationFrame->MeshKeys.GetCArray(), sizeof(ZEModelResourceAnimationKey), CurrentAnimationFrame->MeshKeys.GetCount());
		}
	}
	return true;
}

static bool ReadModelFromFile(ZEModelResource* Model, ZEFile* ResourceFile)
{
	ZEModelFileHeaderChunk HeaderChunk;
	ResourceFile->Read(&HeaderChunk, sizeof(ZEModelFileHeaderChunk), 1);

	if(HeaderChunk.HEADER != ZE_MDLF_HEADER)
	{
		zeError("Unknown ZEModel file format.");
		return false;
	}

	if(HeaderChunk.Version != ZE_MDLF_VERSION)
	{
		zeError("ZEModel file version mismatched.");
		return false;	
	}

	Model->Materials.SetCount(HeaderChunk.MaterialCount);
	if (!ReadMaterialsFromFile(Model, ResourceFile))
	{
		zeError("Corrupted ZEModel file. Can not read model file.");
		return false;
	}

	Model->Meshes.SetCount(HeaderChunk.MeshCount);
	if(!ReadMeshesFromFile(Model, ResourceFile))
	{
		zeError("Corrupted ZEModel file. Can not read model file.");
		return false;
	}

	Model->Bones.SetCount(HeaderChunk.BoneCount);
	if(!ReadBonesFromFile(Model, ResourceFile))
	{
		zeError("Corrupted ZEModel file. Can not read model file.");
		return false;
	}

	Model->Animations.SetCount(HeaderChunk.AnimationCount);
	if(!ReadAnimationsFromFile(Model, ResourceFile))
	{
		zeError("Corrupted ZEModel file. Can not read model file.");
		return false;
	}

	return true;
}

const char* ZEModelResource::GetResourceType() const
{
	return "Model";
}


ZEModelResource* ZEModelResource::LoadSharedResource(const ZEString& FileName)
{
	ZEString NewPath = ConstructResourcePath(FileName);

	ZEModelResource* Resource = (ZEModelResource*)zeResources->GetResource(NewPath);
	
	if (Resource != NULL)
		return Resource;
	else
	{
		Resource = LoadResource(NewPath);
		if (Resource != NULL)
		{
			Resource->Shared = true;
			Resource->Cached = false;
			Resource->ReferenceCount = 1;
			zeResources->AddResource(Resource);
			return Resource;
		}
		else
			return NULL;
	}
}

void ZEModelResource::CacheResource(const ZEString& FileName)
{
	ZEString NewPath = ConstructResourcePath(FileName);

	ZEModelResource* Resource = (ZEModelResource*)zeResources->GetResource(NewPath);
	if (Resource != NULL)
		Resource->Cached = true;
	else
	{
		Resource = LoadResource(NewPath);
		Resource->Cached = true;
		Resource->ReferenceCount = 0;
		zeResources->AddResource(Resource);
	}
}

ZEModelResource* ZEModelResource::LoadResource(const ZEString& FileName)
{
	ZEString NewPath = ConstructResourcePath(FileName);

	ZEFile* ResourceFile = ZEFile::Open(NewPath);
	if (ResourceFile != NULL && ResourceFile->IsOpen())
	{
		ZEModelResource* NewResource = new ZEModelResource();
		NewResource->SetFileName(NewPath);
		NewResource->Cached = false;
		NewResource->ReferenceCount = 0;
		if (!ReadModelFromFile(NewResource, ResourceFile))
		{
			zeError("Can not load model file. (FileName : \"%s\")", FileName);
			ResourceFile->Close();
			delete ResourceFile;
			delete NewResource;
			return NULL;
		}
		else
		{
			ResourceFile->Close();
			delete ResourceFile;
			return NewResource;
		}
	}
	else
	{
		zeError("Model file does not exists. (FileName : \"%s\")", FileName);
		return NULL;
	}
}

ZEModelResource::~ZEModelResource()
{
	for (size_t I = 0; I < TextureResources.GetCount(); I++)
		TextureResources[I]->Release();
}
