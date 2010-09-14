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

#include "ZEModelResource.h"
#include "ZECore\ZEResourceManager.h"
#include "ZECore\ZEConsole.h"
#include "ZECore\ZEError.h"
#include "ZEGraphics\ZEGraphicsModule.h"
#include "ZEGraphics\ZETexture2DResource.h"
#include "ZEGraphics\ZEVertexBuffer.h"
#include "ZEGraphics\ZEFixedMaterial.h"
#include "ZEModelFileFormat.h"
#include <memory.h>
#include <string.h>

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
		if (strnicmp(TextureResources[I]->GetFileName(), FileName, ZE_MDLF_MAX_FILENAME_SIZE) == 0)
			return TextureResources[I]->GetTexture();

	ZETexture2DResource* NewTextureResource = ZETexture2DResource::LoadSharedResource(FileName);
	if (NewTextureResource == NULL)
	{
		zeError("Map Resource", "Can not load texture file. (FileName : \"%s\")", FileName);
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

static bool ReadMaterialsFromFile(ZEModelResource* Model, ZEResourceFile* ResourceFile)
{
	for (size_t I = 0; I < Model->Materials.GetCount(); I++)
	{
		ZEMaterial* Material = Model->Materials[I]; 
		ZEModelFileMaterialChunk MaterialChunk;

		ResourceFile->Read(&MaterialChunk, sizeof(ZEModelFileMaterialChunk), 1);
		if(MaterialChunk.ChunkId != ZE_MDLF_MATERIAL_CHUNKID)
		{
			zeError("Model Resource", "Corrupted ZEModel file. Materials chunk id does not matches.");
			return false;
		}

		ZEFixedMaterial* CurrentMaterial = ZEFixedMaterial::CreateInstance();
		Model->Materials[I] = CurrentMaterial;

		CurrentMaterial->SetZero();
		CurrentMaterial->SetDiffuseEnabled(true);
		CurrentMaterial->SetAmbientEnabled(true);
		CurrentMaterial->SetSpecularEnabled(true);
		CurrentMaterial->SetEmmisiveEnabled(true);
		
		CurrentMaterial->SetTwoSided(MaterialChunk.TwoSided);
		CurrentMaterial->SetLightningEnabled(MaterialChunk.LightningEnabled);
		CurrentMaterial->SetWireframe(MaterialChunk.Wireframe);
		CurrentMaterial->SetTransparancyMode(ZE_MTM_NOTRANSPARACY);//MaterialChunk.Transparant ? ZE_MTM_ADDAPTIVE: ZE_MTM_NOTRANSPARACY);

		CurrentMaterial->SetAmbientColor(MaterialChunk.AmbientColor);
		CurrentMaterial->SetDiffuseColor(MaterialChunk.DiffuseColor);
		CurrentMaterial->SetSpecularColor(MaterialChunk.SpecularColor);
		CurrentMaterial->SetEmmisiveColor(MaterialChunk.EmmisiveColor);
		CurrentMaterial->SetEmmisiveFactor(MaterialChunk.EmmisiveFactor);
		CurrentMaterial->SetSpecularFactor(1.0f);
		CurrentMaterial->SetSpecularShininess((1.25f - MaterialChunk.SpecularFactor) * 128.0f);
		CurrentMaterial->SetOpacity(MaterialChunk.Opasity);
		CurrentMaterial->SetReflectionFactor(MaterialChunk.ReflectionFactor);
		CurrentMaterial->SetRefractionFactor(MaterialChunk.RefractionFactor);
		CurrentMaterial->SetDetailMapTiling(MaterialChunk.DetailMapTiling);

		CurrentMaterial->SetDiffuseMap(ManageModelMaterialTextures(MaterialChunk.DiffuseMap, Model->TextureResources));
		
		CurrentMaterial->SetNormalMapEnabled(MaterialChunk.ShaderComponents & ZE_MFSC_NORMALMAP);
		CurrentMaterial->SetNormalMap(ManageModelMaterialTextures(MaterialChunk.NormalMap, Model->TextureResources));
		CurrentMaterial->SetSpecularMap(ManageModelMaterialTextures(MaterialChunk.SpecularMap, Model->TextureResources));
		CurrentMaterial->SetEmmisiveMap(ManageModelMaterialTextures(MaterialChunk.EmmisiveMap, Model->TextureResources));
		CurrentMaterial->SetOpacityMap(ManageModelMaterialTextures(MaterialChunk.OpasityMap, Model->TextureResources));
		
		CurrentMaterial->SetDetailMapEnabled(MaterialChunk.ShaderComponents & ZE_MFSC_DETAILNORMALMAP);
		CurrentMaterial->SetDetailDiffuseMap(ManageModelMaterialTextures(MaterialChunk.DetailMap, Model->TextureResources));
		CurrentMaterial->SetDetailNormalMap(ManageModelMaterialTextures(MaterialChunk.DetailNormalMap, Model->TextureResources));
		CurrentMaterial->SetReflectionEnabled(false);
		CurrentMaterial->SetRefractionMap(NULL);//ManageMapMaterialTextures(MaterialChunk.EnvironmentMap, TextureResources);
		CurrentMaterial->SetRefractionEnabled(false);
		CurrentMaterial->SetRefractionMap(NULL);

		CurrentMaterial->SetLightMapEnabled(MaterialChunk.ShaderComponents & ZE_MFSC_LIGHTMAP);
		CurrentMaterial->SetLightMap(ManageModelMaterialTextures(MaterialChunk.LightMap, Model->TextureResources));

 		CurrentMaterial->UpdateMaterial();
	}
	return true;
}

enum ZEPhysicalShapeType_
{
	_ZE_PST_PLANE           = 0,
	_ZE_PST_BOX				= 1,
	_ZE_PST_SPHERE			= 2,
	_ZE_PST_CAPSULE			= 3,
	_ZE_PST_CONVEX			= 4,
	_ZE_PST_TRIMESH         = 5
};

static bool ReadPhysicalBodyFromFile(ZEModelResourcePhysicalBody* Body, ZEResourceFile* ResourceFile)
{
	ZEModelFilePhysicalBodyChunk BodyChunk;
	ResourceFile->Read(&BodyChunk, sizeof(ZEModelFilePhysicalBodyChunk), 1);

	if (BodyChunk.ChunkId != ZE_MDLF_PHYSICAL_BODY_CHUNKID)
	{
		zeError("Model Resource", "Corrupted ZEModel file. Physical body chunk id does not matches.");
		return false;
	}

	Body->Type				= (ZEPhysicalBodyType)BodyChunk.Type;
	Body->Enabled			= BodyChunk.Enabled;
	Body->Mass				= BodyChunk.Mass;
	Body->Kinematic			= BodyChunk.Kinematic;
	Body->AngularDamping	= BodyChunk.AngularDamping;
	Body->LinearDamping		= BodyChunk.LinearDamping;
	Body->MassCenter		= BodyChunk.MassCenter;

	Body->Shapes.SetCount(BodyChunk.ShapeCount);
	for (size_t I = 0; I < Body->Shapes.GetCount(); I++)
	{
		ZEModelResourcePhysicalShape* Shape = &Body->Shapes[I];

		ZEModelFilePhysicalBodyShapeChunk ShapeChunk;
		ResourceFile->Read(&ShapeChunk, sizeof(ZEModelFilePhysicalBodyShapeChunk), 1);

		if(ShapeChunk.ChunkId != ZE_MDLF_PHYSICAL_BODY_SHAPE_CHUNKID)
		{
			zeError("Model Resource", "Corrupted ZEModel file. Physical shape chunk id does not matches.");
			return false;
		}

		Shape->Type						= (ZEPhysicalShapeType)ShapeChunk.Type;
		Shape->Position					= ShapeChunk.Position;
		Shape->Rotation					= ShapeChunk.Rotation;
		Shape->StaticFriction			= ShapeChunk.StaticFriction;
		Shape->DynamicFriction			= ShapeChunk.DynamicFriction;
		Shape->Restitution				= ShapeChunk.Restitution;
		Shape->Trigger					= ShapeChunk.Trigger;
		Shape->CollisionMask1 			= ShapeChunk.CollisionMask1;
		Shape->CollisionMask2			= ShapeChunk.CollisionMask2;
		Shape->CollisionMask3			= ShapeChunk.CollisionMask3;
		Shape->CollisionMask4			= ShapeChunk.CollisionMask4;

		switch (Shape->Type)
		{
			case _ZE_PST_PLANE:
			{
				Shape->Plane.Height		= ShapeChunk.Plane.Height;
				Shape->Plane.NormalX	= ShapeChunk.Plane.NormalX;
				Shape->Plane.NormalY	= ShapeChunk.Plane.NormalY;
				Shape->Plane.NormalZ	= ShapeChunk.Plane.NormalZ;
				break;
			}
			case _ZE_PST_BOX:
			{
				Shape->Box.Width		= 0.5 * ShapeChunk.Box.Width;
				Shape->Box.Height		= 0.5 * ShapeChunk.Box.Height;
				Shape->Box.Length		= 0.5 * ShapeChunk.Box.Length;
				break;
			}
			case _ZE_PST_SPHERE:
			{
				Shape->Sphere.Radius	= ShapeChunk.Sphere.Radius;
				break;
			}
			case _ZE_PST_CAPSULE:
			{
				Shape->Capsule.Height	= ShapeChunk.Capsule.Height;
				Shape->Capsule.Radius	= ShapeChunk.Capsule.Radius;
				break;
			}
			case _ZE_PST_CONVEX:
			{
				ZEDWORD ChunkId;
				ResourceFile->Read(&ChunkId, sizeof(ZEDWORD), 1);
				if (ChunkId != ZE_MDLF_PHYSICAL_BODY_SHAPE_VERTEX_CHUNKID)
				{
					zeError("Model Resource", "Corrupted ZEModel file. Physical vertex chunk id does not matches.");
					return false;
				}
				
				Shape->Convex.Vertices.SetCount(ShapeChunk.Convex.VertexCount);
				ResourceFile->Read(Shape->Convex.Vertices.GetCArray(), sizeof(ZEVector3), Shape->Convex.Vertices.GetCount());
				break;
			}
			case _ZE_PST_TRIMESH:
			{
				//vertices
				ZEDWORD ChunkId;
				ResourceFile->Read(&ChunkId, sizeof(ZEDWORD), 1);
				if (ChunkId != ZE_MDLF_PHYSICAL_BODY_SHAPE_VERTEX_CHUNKID)
				{
					zeError("Model Resource", "Corrupted ZEModel file. Physical vertex chunk id does not matches.");
					return false;
				}
				
				Shape->TriMesh.Vertices.SetCount(ShapeChunk.TriMesh.VertexCount);
				ResourceFile->Read(Shape->TriMesh.Vertices.GetCArray(), sizeof(ZEVector3), Shape->TriMesh.Vertices.GetCount());
				
				//indices
				ResourceFile->Read(&ChunkId, sizeof(ZEDWORD), 1);
				if (ChunkId != ZE_MDLF_PHYSICAL_BODY_SHAPE_INDEX_CHUNKID)
				{
					zeError("Model Resource", "Corrupted ZEModel file. Physical index chunk id does not matches.");
					return false;
				}
				
				Shape->TriMesh.Indices.SetCount(ShapeChunk.TriMesh.IndexCount);
				ResourceFile->Read(Shape->TriMesh.Indices.GetCArray(), sizeof(ZEModelFilePhysicalPolygonChunk), Shape->TriMesh.Indices.GetCount());
				break;
			}
			default:
				zeError("Model Resource", "Wrong physical shape type. (Physical Shape : %d)", Shape->Type);
				return false;
		}
	}
	return true;
}

static bool ReadMeshesFromFile(ZEModelResource* Model, ZEResourceFile* ResourceFile)
{
	for (size_t I = 0; I < Model->Meshes.GetCount(); I++)
	{
		ZEModelResourceMesh* Mesh = &Model->Meshes[I];

		ZEModelFileMeshChunk MeshChunk;
		ResourceFile->Read(&MeshChunk, sizeof(ZEModelFileMeshChunk), 1);
		
		if(MeshChunk.ChunkId != ZE_MDLF_MESH_CHUNKID)
		{
			zeError("Model Resource",  "Corrupted ZEModel file. Mesh's chunk id does not matches.");
			return false;
		}

		strncpy(Mesh->Name, MeshChunk.Name, ZE_MDLF_MAX_NAME_SIZE);
		Mesh->IsSkinned		= MeshChunk.IsSkinned;
		Mesh->BoundingBox	= MeshChunk.BoundingBox;
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
				zeError("Model Resource",  "Corrupted ZEModel file. Mesh LOD's chunk id does not matches.");
				return false;			
			}

			LOD->MaterialId = MeshLODChunk.MaterialId;
			LOD->LODLevel = MeshLODChunk.LODLevel;
			if(MeshChunk.IsSkinned)
			{
				LOD->AffectingBoneIds.SetCount(MeshLODChunk.AffectingBoneCount);
				ResourceFile->Read(LOD->AffectingBoneIds.GetCArray(), sizeof(ZEDWORD), LOD->AffectingBoneIds.GetCount());

				LOD->SkinnedVertices.SetCount(MeshLODChunk.VertexCount);				
				ResourceFile->Read(LOD->SkinnedVertices.GetCArray(), sizeof(ZESkinnedModelVertex),  LOD->SkinnedVertices.GetCount());
			}
			else
			{
				LOD->Vertices.SetCount(MeshLODChunk.VertexCount);				
				ResourceFile->Read(LOD->Vertices.GetCArray(), sizeof(ZEModelVertex),  LOD->Vertices.GetCount());
			}
		}

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

static bool ReadPhysicalJointFromFile(ZEModelResourcePhysicalJoint* Joint, ZEResourceFile* ResourceFile)
{
	ZEModelFilePhysicalJointChunk JointChunk;
	ResourceFile->Read(&JointChunk, sizeof(ZEPhysicalJoint), 1);

	if (JointChunk.ChunkId != ZE_MDLF_PHYSICAL_JOINT_CHUNKID)
	{
		zeError("Model Resource", "Corrupted ZEModel file. Physical joint's chunk id does not matches.");
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

	Joint->LinearMotorPosition			= JointChunk.LinearMotorPosition;
	Joint->LinearMotorVelocity			= JointChunk.LinearMotorVelocity;
	Joint->LinearXMotor					= JointChunk.LinearXMotor;
	Joint->LinearXMotorDamper			= JointChunk.LinearXMotorDamper;
	Joint->LinearXMotorForce			= JointChunk.LinearXMotorForce;
	Joint->LinearXMotorSpring			= JointChunk.LinearXMotorSpring;
	Joint->LinearYMotor					= JointChunk.LinearYMotor;
	Joint->LinearYMotorDamper			= JointChunk.LinearYMotorDamper;
	Joint->LinearYMotorForce			= JointChunk.LinearYMotorForce;
	Joint->LinearYMotorSpring			= JointChunk.LinearYMotorSpring;
	Joint->LinearZMotor					= JointChunk.LinearZMotor;
	Joint->LinearZMotorDamper			= JointChunk.LinearZMotorDamper;
	Joint->LinearZMotorForce			= JointChunk.LinearZMotorForce;
	Joint->LinearZMotorSpring			= JointChunk.LinearZMotorSpring;
	
	Joint->AngularMotor					= JointChunk.AngularMotor;
	Joint->AngularMotorDamper			= JointChunk.AngularMotorDamper;
	Joint->AngularMotorForce			= JointChunk.AngularMotorForce;
	Joint->AngularMotorOrientation		= JointChunk.AngularMotorOrientation;
	Joint->AngularMotorSpring			= JointChunk.AngularMotorSpring;
	Joint->AngularMotorVelocity			= JointChunk.AngularMotorVelocity;

	return true;
}

static void ProcessBones(ZEModelResource* Model, ZEModelResourceBone* Bone, int BoneId)
{
	ZEMatrix4x4::CreateOrientation(Bone->RelativeTransform, Bone->RelativePosition, Bone->RelativeRotation, Bone->RelativeScale);

	if (Bone->ParentBone != -1)
	{

		ZEMatrix4x4::Multiply(Bone->ForwardTransform, Bone->RelativeTransform, Model->Bones[Bone->ParentBone].ForwardTransform);
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

static bool ReadBonesFromFile(ZEModelResource* Model, ZEResourceFile* ResourceFile)
{
	for (size_t I = 0; I < Model->Bones.GetCount(); I++)
	{
		ZEModelResourceBone* Bone = &Model->Bones[I];

		ZEModelFileBoneChunk BoneChunk;
		ResourceFile->Read(&BoneChunk, sizeof(ZEModelFileBoneChunk), 1);

		if(BoneChunk.ChunkId != ZE_MDLF_BONE_CHUNKID)
		{
			zeError("Model Resource",  "Corrupted ZEModel file. Bone chunk id does not matches.");
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

		if (BoneChunk.HasPhysicalJoint)
			ReadPhysicalJointFromFile(&Bone->PhysicalJoint, ResourceFile);
	}

	for (size_t I = 0; I < Model->Bones.GetCount(); I++)
		if (Model->Bones[I].ParentBone == -1)
			ProcessBones(Model, &Model->Bones[I], I);
	return true;
}

static bool ReadAnimationsFromFile(ZEModelResource* Model, ZEResourceFile* ResourceFile)
{
	for (size_t I = 0; I < Model->Animations.GetCount(); I++)
	{
		ZEModelResourceAnimation* Animation = &Model->Animations[I];

		ZEModelFileAnimationChunk AnimationChunk;
		ResourceFile->Read(&AnimationChunk, sizeof(ZEModelFileAnimationChunk), 1);

		if(AnimationChunk.ChunkId != ZE_MDLF_ANIMATION_CHUNKID)
		{
			zeError("Model Resource", "Corrupted ZEModel file. Animation chunk id does not matches.");
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
				zeError("Model Resource", "Corrupted ZEModel file. Animation frame chunk id does not matches.");
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

static bool ReadModelFromFile(ZEModelResource* Model, ZEResourceFile* ResourceFile)
{
	zeLog("Model Resource", "Loading model file \"%s\".", ResourceFile->GetFileName());
	ZEModelFileHeaderChunk HeaderChunk;
	ResourceFile->Read(&HeaderChunk, sizeof(ZEModelFileHeaderChunk), 1);

	if(HeaderChunk.HEADER != ZE_MDLF_HEADER)
	{
		zeError("Model Resource", "Unknown ZEModel file format.");
		return false;
	}

	if(HeaderChunk.Version != ZE_MDLF_VERSION)
	{
		zeError("Model Resource", "ZEModel file version mismatched.");
		return false;	
	}

	Model->Materials.SetCount(HeaderChunk.MaterialCount);
	if (!ReadMaterialsFromFile(Model, ResourceFile))
	{
		zeError("Model Resource", "Corrupted ZEModel file. Can not read model file.");
		return false;
	}

	Model->Meshes.SetCount(HeaderChunk.MeshCount);
	if(!ReadMeshesFromFile(Model, ResourceFile))
	{
		zeError("Model Resource", "Corrupted ZEModel file. Can not read model file.");
		return false;
	}

	Model->Bones.SetCount(HeaderChunk.BoneCount);
	if(!ReadBonesFromFile(Model, ResourceFile))
	{
		zeError("Model Resource", "Corrupted ZEModel file. Can not read model file.");
		return false;
	}

	Model->Animations.SetCount(HeaderChunk.AnimationCount);
	if(!ReadAnimationsFromFile(Model, ResourceFile))
	{
		zeError("Model Resource", "Corrupted ZEModel file. Can not read model file.");
		return false;
	}

	zeLog("Model Resource", "Model file \"%s\" loaded.", ResourceFile->GetFileName());

	return true;
}

const char* ZEModelResource::GetResourceType() const
{
	return "Model";
}


ZEModelResource* ZEModelResource::LoadSharedResource(const char* FileName)
{
	ZEModelResource* Resource = (ZEModelResource*)zeResources->GetResource(FileName);
	
	if (Resource != NULL)
		return Resource;
	else
	{
		Resource = LoadResource(FileName);
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

void ZEModelResource::CacheResource(const char* FileName)
{
	ZEModelResource* Resource = (ZEModelResource*)zeResources->GetResource(FileName);
	if (Resource != NULL)
		Resource->Cached = true;
	else
	{
		Resource = LoadResource(FileName);
		Resource->Cached = true;
		Resource->ReferenceCount = 0;
		zeResources->AddResource(Resource);
	}
}

ZEModelResource* ZEModelResource::LoadResource(const char* FileName)
{
	ZEResourceFile ResourceFile;
	if (ResourceFile.Open(FileName))
	{
		ZEModelResource* NewResource = new ZEModelResource();
		NewResource->SetFileName(FileName);
		NewResource->Cached = false;
		NewResource->ReferenceCount = 0;
		if (!ReadModelFromFile(NewResource, &ResourceFile))
		{
			zeError("Model Resource", "Can not load model file. (FileName : \"%s\")", FileName);
			ResourceFile.Close();
			delete NewResource;
			return NULL;
		}
		else
		{
			ResourceFile.Close();
			return NewResource;
		}
	}
	else
	{
		zeError("Model Resource", "Model file does not exists. (FileName : \"%s\")", FileName);
		return NULL;
	}
}

ZEModelResource::~ZEModelResource()
{
	for (int I = 0; I < TextureResources.GetCount(); I++)
		TextureResources[I]->Release();
}




