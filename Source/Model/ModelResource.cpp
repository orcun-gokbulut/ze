//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ModelResource.cpp
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

#include "ModelResource.h"
#include "ModelFileFormat.h"
#include "Core/Core.h"
#include "Graphics/GraphicsModule.h"
#include <memory.h>

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

const ZETexture* ManageModelMaterialTextures(char* Filename, ZESmartArray<ZETextureResource*>& TextureResources)
{
	if (strncmp(Filename, "", ZE_MDLF_MAX_FILENAME_SIZE) == 0)
		return NULL;

	for (size_t I = 0; I < TextureResources.GetCount(); I++)
		if (strnicmp(TextureResources[I]->GetFileName(), Filename, ZE_MDLF_MAX_FILENAME_SIZE) == 0)
			return TextureResources[I]->GetTexture();

	ZETextureResource* NewTextureResource = ZETextureResource::LoadSharedResource(Filename);
	if (NewTextureResource == NULL)
	{
		zeError("Map Resource", "Can not load texture file. (Filename : \"%s\")", Filename);
		return NULL;
	}
	TextureResources.Add(NewTextureResource);
	return NewTextureResource->GetTexture();
}

bool ReadMaterialsFromFile(ZEModelResource* Model, ZEResourceFile* ResourceFile)
{
	for (size_t I = 0; I < Model->Materials.GetCount(); I++)
	{
		ZEDefaultMaterial* Material = &Model->Materials[I]; 
		ZEModelFileMaterialChunk MaterialChunk;

		ResourceFile->Read(&MaterialChunk, sizeof(ZEModelFileMaterialChunk), 1);
		if(MaterialChunk.ChunkId != ZE_MDLF_MATERIAL_CHUNKID)
		{
			zeError("Model Resource", "Corrupted ZEModel file. Materials chunk id does not matches.");
			return false;
		}

		Material->TwoSided = MaterialChunk.TwoSided;
		Material->LightningEnabled = MaterialChunk.LightningEnabled;
		Material->Wireframe = MaterialChunk.Wireframe;
		Material->TransparancyMode = MaterialChunk.Transparant ? ZE_TM_ADDAPTIVE : ZE_TM_NOTRANSPARACY;
		Material->SetShaderComponents(MaterialChunk.ShaderComponents);

		Material->AmbientColor = MaterialChunk.AmbientColor;
		Material->DiffuseColor = MaterialChunk.DiffuseColor;
		Material->SpecularColor = MaterialChunk.SpecularColor;
		Material->EmmisiveColor = MaterialChunk.EmmisiveColor;
		Material->EmmisiveFactor = MaterialChunk.EmmisiveFactor;
		Material->SpecularFactor = (1.25f - MaterialChunk.SpecularFactor) * 128.0f;
		Material->Opasity = MaterialChunk.Opasity;
		Material->ReflectionFactor = MaterialChunk.ReflectionFactor;
		Material->RefractionFactor = MaterialChunk.RefractionFactor;
		Material->DetailMapTiling = MaterialChunk.DetailMapTiling;

		Material->DiffuseMap = ManageModelMaterialTextures(MaterialChunk.DiffuseMap, Model->TextureResources);
		Material->NormalMap = ManageModelMaterialTextures(MaterialChunk.NormalMap, Model->TextureResources);
		Material->SpecularMap = ManageModelMaterialTextures(MaterialChunk.SpecularMap, Model->TextureResources);
		Material->EmmisiveMap = ManageModelMaterialTextures(MaterialChunk.EmmisiveMap, Model->TextureResources);
		Material->OpacityMap = ManageModelMaterialTextures(MaterialChunk.OpasityMap, Model->TextureResources);
		Material->DetailMap = ManageModelMaterialTextures(MaterialChunk.DetailMap, Model->TextureResources);
		Material->DetailNormalMap = ManageModelMaterialTextures(MaterialChunk.DetailNormalMap, Model->TextureResources);
		Material->EnvironmentMap = NULL;//ManageModelMaterialTextures(MaterialChunk.EnvironmentMap, Model->TextureResources);
		Material->LightMap = ManageModelMaterialTextures(MaterialChunk.LightMap, Model->TextureResources);
	}
	return true;
}

bool ReadPhysicalBodyFromFile(ZEModelResourcePhysicalBody* Body, ZEResourceFile* ResourceFile)
{
	ZEModelFilePhysicalBodyChunk BodyChunk;
	ResourceFile->Read(&BodyChunk, sizeof(ZEModelFilePhysicalBodyChunk), 1);

	if (BodyChunk.ChunkId != ZE_MDLF_PHYSICAL_BODY_CHUNKID)
	{
		zeError("Model Resource", "Corrupted ZEModel file. Physical body chunk id does not matches.");
		return false;
	}

	Body->Type				= (ZEPhysicalBodyType)BodyChunk.Type;
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
		Shape->LocalPosition			= ShapeChunk.LocalPosition;
		Shape->LocalOrientation			= ShapeChunk.LocalOrientation;
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
			case ZE_PST_PLANE:
			{
				Shape->Plane.Height		= ShapeChunk.Plane.Height;
				Shape->Plane.NormalX	= ShapeChunk.Plane.NormalX;
				Shape->Plane.NormalY	= ShapeChunk.Plane.NormalY;
				Shape->Plane.NormalZ	= ShapeChunk.Plane.NormalZ;
				break;
			}
			case ZE_PST_BOX:
			{
				Shape->Box.Width		= ShapeChunk.Box.Width;
				Shape->Box.Height		= ShapeChunk.Box.Height;
				Shape->Box.Length		= ShapeChunk.Box.Length;
				break;
			}
			case ZE_PST_SPHERE:
			{
				Shape->Sphere.Radius	= ShapeChunk.Sphere.Radius;
				break;
			}
			case ZE_PST_CAPSULE:
			{
				Shape->Capsule.Height	= ShapeChunk.Capsule.Height;
				Shape->Capsule.Radius	= ShapeChunk.Capsule.Radius;
				break;
			}
			case ZE_PST_CONVEX:
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
			case ZE_PST_TRIMESH:
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

bool ReadMeshesFromFile(ZEModelResource* Model, ZEResourceFile* ResourceFile)
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
		Mesh->Orientation	= MeshChunk.Orientation;

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

bool ReadPhysicalJointFromFile(ZEModelResourcePhysicalJoint* Joint, ZEResourceFile* ResourceFile)
{
	ZEModelFilePhysicalJointChunk JointChunk;
	ResourceFile->Read(&JointChunk, sizeof(ZEModelResourcePhysicalJoint), 1);

	if (JointChunk.ChunkId != ZE_MDLF_PHYSICAL_JOINT_CHUNKID)
	{
		zeError("Model Resource", "Corrupted ZEModel file. Physical joint's chunk id does not matches.");
		return false;
	}

	Joint->JointType			= JointChunk.Type;
	Joint->Body1Id				= JointChunk.Body1Id;
	Joint->Body2Id				= JointChunk.Body2Id;
	Joint->Breakable			= JointChunk.Breakable;
	Joint->BreakForce			= JointChunk.BreakForce;
	Joint->BreakTorque			= JointChunk.BreakTorque;
	Joint->CollideBodies		= JointChunk.CollideBodies;
	Joint->UseGlobalAnchorAxis	= JointChunk.UseGlobalAnchorAxis;
	Joint->GlobalAnchor			= JointChunk.GlobalAnchor;
	Joint->GlobalAxis			= JointChunk.GlobalAxis;
	Joint->LocalAnchor1			= JointChunk.LocalAnchor1;
	Joint->LocalAnchor2			= JointChunk.LocalAnchor2;
	Joint->LocalAxis1			= JointChunk.LocalAxis1;
	Joint->LocalAxis2			= JointChunk.LocalAxis2;

	switch (Joint->JointType)
	{
		case ZE_PJT_SPHERICAL:
			Joint->Spherical.SwingLimit				= JointChunk.Spherical.SwingLimit;
			Joint->Spherical.SwingLimitRestitution	= JointChunk.Spherical.SwingLimitRestitution;
			Joint->Spherical.SwingLimitValue		= JointChunk.Spherical.SwingLimitValue;
			Joint->Spherical.TwistLimit				= JointChunk.Spherical.TwistLimit;
			Joint->Spherical.TwistLimitHighValue	= JointChunk.Spherical.TwistLimitHighValue;
			Joint->Spherical.TwistLimitLowValue		= JointChunk.Spherical.TwistLimitLowValue;
			Joint->Spherical.TwistLimitRestitution	= JointChunk.Spherical.TwistLimitRestitution;
			break;

		case ZE_PJT_REVOLUTE:
			Joint->Revolute.HasLimit				= JointChunk.Revolute.HasLimit;
			Joint->Revolute.HasMotor				= JointChunk.Revolute.HasMotor;
			Joint->Revolute.HasSpring				= JointChunk.Revolute.HasSpring;
			Joint->Revolute.LimitHighValue			= JointChunk.Revolute.LimitHighValue;
			Joint->Revolute.LimitLowValue			= JointChunk.Revolute.LimitLowValue;
			Joint->Revolute.LimitRestitution		= JointChunk.Revolute.LimitRestitution;
			Joint->Revolute.MotorForce				= JointChunk.Revolute.MotorForce;
			Joint->Revolute.MotorVelocity			= JointChunk.Revolute.MotorVelocity;
			Joint->Revolute.SpringDamper			= JointChunk.Revolute.SpringDamper;
			Joint->Revolute.SpringTarget			= JointChunk.Revolute.SpringTarget;
			Joint->Revolute.SpringValue				= JointChunk.Revolute.SpringValue;
			break;

		case ZE_PJT_DISTANCE:
			Joint->Distance.HasMaxLimit				= JointChunk.Distance.HasMaxLimit;
			Joint->Distance.HasMinLimit				= JointChunk.Distance.HasMinLimit;
			Joint->Distance.HasSpring				= JointChunk.Distance.HasSpring;
			Joint->Distance.MaxDistance				= JointChunk.Distance.MaxDistance;
			Joint->Distance.MinDistance				= JointChunk.Distance.MinDistance;
			Joint->Distance.SpringDamper			= JointChunk.Distance.SpringDamper;
			Joint->Distance.SpringValue				= JointChunk.Distance.SpringValue;
			break;

		case ZE_PJT_PULLEY:
			Joint->Pulley.Distance					= JointChunk.Pulley.Distance;
			Joint->Pulley.HasMotor					= JointChunk.Pulley.HasMotor;
			Joint->Pulley.IsRigid					= JointChunk.Pulley.IsRigid;
			Joint->Pulley.MotorForce				= JointChunk.Pulley.MotorForce;
			Joint->Pulley.MotorVelocity				= JointChunk.Pulley.MotorVelocity;
			Joint->Pulley.Pulley1					= JointChunk.Pulley.Pulley1;
			Joint->Pulley.Pulley2					= JointChunk.Pulley.Pulley2;
			Joint->Pulley.Ratio						= JointChunk.Pulley.Ratio;
			Joint->Pulley.Stiffness					= JointChunk.Pulley.Stiffness;
			break;

		case ZE_PJT_FREE:
			Joint->Free.XMotion						= JointChunk.Free.XMotion;
			Joint->Free.YMotion						= JointChunk.Free.YMotion;
			Joint->Free.ZMotion						= JointChunk.Free.ZMotion;
			Joint->Free.LinearLimitDamper			= JointChunk.Free.LinearLimitDamper;
			Joint->Free.LinearLimitRestitution		= JointChunk.Free.LinearLimitRestitution;
			Joint->Free.LinearLimitSpring			= JointChunk.Free.LinearLimitSpring;
			Joint->Free.LinearLimitValue			= JointChunk.Free.LinearLimitValue;

			Joint->Free.Swing1Motion				= JointChunk.Free.Swing1Motion;
			Joint->Free.Swing1LimitDamper			= JointChunk.Free.Swing1LimitDamper;
			Joint->Free.Swing1LimitRestitution		= JointChunk.Free.Swing1LimitRestitution;
			Joint->Free.Swing1LimitSpring			= JointChunk.Free.Swing1LimitSpring;
			Joint->Free.Swing1LimitValue			= JointChunk.Free.Swing1LimitValue;

			Joint->Free.Swing2Motion				= JointChunk.Free.Swing2Motion;
			Joint->Free.Swing2LimitDamper			= JointChunk.Free.Swing2LimitDamper;
			Joint->Free.Swing2LimitRestitution		= JointChunk.Free.Swing2LimitRestitution;
			Joint->Free.Swing2LimitSpring			= JointChunk.Free.Swing2LimitSpring;
			Joint->Free.Swing2LimitValue			= JointChunk.Free.Swing2LimitValue;

			Joint->Free.TwistMotion					= JointChunk.Free.TwistMotion;
			Joint->Free.TwistLimitDamper			= JointChunk.Free.TwistLimitDamper;
			Joint->Free.TwistLimitHighValue			= JointChunk.Free.TwistLimitHighValue;
			Joint->Free.TwistLimitLowValue			= JointChunk.Free.TwistLimitLowValue;
			Joint->Free.TwistLimitRestitution		= JointChunk.Free.TwistLimitRestitution;
			Joint->Free.TwistLimitSpring			= JointChunk.Free.TwistLimitSpring;

			Joint->Free.LinearMotorPosition			= JointChunk.Free.LinearMotorPosition;
			Joint->Free.LinearMotorVelocity			= JointChunk.Free.LinearMotorVelocity;
			Joint->Free.LinearXMotor				= JointChunk.Free.LinearXMotor;
			Joint->Free.LinearXMotorDamper			= JointChunk.Free.LinearXMotorDamper;
			Joint->Free.LinearXMotorForce			= JointChunk.Free.LinearXMotorForce;
			Joint->Free.LinearXMotorSpring			= JointChunk.Free.LinearXMotorSpring;
			Joint->Free.LinearYMotor				= JointChunk.Free.LinearYMotor;
			Joint->Free.LinearYMotorDamper			= JointChunk.Free.LinearYMotorDamper;
			Joint->Free.LinearYMotorForce			= JointChunk.Free.LinearYMotorForce;
			Joint->Free.LinearYMotorSpring			= JointChunk.Free.LinearYMotorSpring;
			Joint->Free.LinearZMotor				= JointChunk.Free.LinearZMotor;
			Joint->Free.LinearZMotorDamper			= JointChunk.Free.LinearZMotorDamper;
			Joint->Free.LinearZMotorForce			= JointChunk.Free.LinearZMotorForce;
			Joint->Free.LinearZMotorSpring			= JointChunk.Free.LinearZMotorSpring;

			Joint->Free.AngularMotor				= JointChunk.Free.AngularMotor;
			Joint->Free.AngularMotorDamper			= JointChunk.Free.AngularMotorDamper;
			Joint->Free.AngularMotorForce			= JointChunk.Free.AngularMotorForce;
			Joint->Free.AngularMotorOrientation		= JointChunk.Free.AngularMotorOrientation;
			Joint->Free.AngularMotorSpring			= JointChunk.Free.AngularMotorSpring;
			Joint->Free.AngularMotorVelocity		= JointChunk.Free.AngularMotorVelocity;
			break;
		default:
			zeError("Model Resource", "Corrupted ZEModel file. There is no such a joint type. (Joint Type : %d)", JointChunk.Type);
			return false;
	}
	return true;
}

bool ReadBonesFromFile(ZEModelResource* Model, ZEResourceFile* ResourceFile)
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
		Bone->AbsoluteOrientation = BoneChunk.AbsoluteOrientation;
		Bone->AbsolutePosition = BoneChunk.AbsolutePosition;
		ZEMatrix4x4::CreateOffset(Bone->ForwardTransform, BoneChunk.AbsolutePosition, BoneChunk.AbsoluteOrientation);
		ZEMatrix4x4::Inverse(Bone->InverseTransform, Bone->ForwardTransform);
		Bone->BoundingBox = BoneChunk.BoundingBox;
		Bone->ParentBone = BoneChunk.ParentBone;
		Bone->RelativeOrientation = BoneChunk.RelativeOrientation;
		Bone->RelativePosition = BoneChunk.RelativePosition;
		

	}

	return true;
}

bool ReadAnimationsFromFile(ZEModelResource* Model, ZEResourceFile* ResourceFile)
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

bool ReadModelFromFile(ZEModelResource* Model, ZEResourceFile* ResourceFile)
{
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

	return true;
}

const char* ZEModelResource::GetResourceType() const
{
	return "Model";
}


const ZEModelResource* ZEModelResource::LoadSharedResource(const char* Filename)
{
	ZEModelResource* Resource = (ZEModelResource*)zeResources->GetResource(Filename);
	
	if (Resource != NULL)
		return Resource;
	else
	{
		Resource = LoadResource(Filename);
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

void ZEModelResource::CacheResource(const char* Filename)
{
	ZEModelResource* Resource = (ZEModelResource*)zeResources->GetResource(Filename);
	if (Resource != NULL)
		Resource->Cached = true;
	else
	{
		Resource = LoadResource(Filename);
		Resource->Cached = true;
		Resource->ReferenceCount = 0;
		zeResources->AddResource(Resource);
	}
}

ZEModelResource* ZEModelResource::LoadResource(const char* Filename)
{
	ZEResourceFile ResourceFile;
	if (ResourceFile.Open(Filename))
	{
		ZEModelResource* NewResource = new ZEModelResource();
		NewResource->SetFilename(Filename);
		NewResource->Cached = false;
		NewResource->ReferenceCount = 0;
		if (!ReadModelFromFile(NewResource, &ResourceFile))
		{
			zeError("Model Resource", "Can not load model file. (Filename : \"%s\")", Filename);
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
		zeError("Model Resource", "Model file does not exists. (Filename : \"%s\")", Filename);
		return NULL;
	}
}

ZEModelResource::~ZEModelResource()
{
	for (int I = 0; I < TextureResources.GetCount(); I++)
		TextureResources[I]->Release();
}
