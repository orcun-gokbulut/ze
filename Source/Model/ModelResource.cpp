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

bool ReadMaterialsFromFile(ZEModelResource* Model, ZEResourceFile& ResourceFile)
{
	for (size_t I = 0; I < Model->Materials.GetCount(); I++)
	{
		ZEDefaultMaterial* Material = &Model->Materials[I]; 
		ZEModelFileMaterialChunk MaterialChunk;

		ResourceFile.Read(&MaterialChunk, sizeof(ZEModelFileMaterialChunk), 1);
		if(MaterialChunk.ChunkId != ZE_MDLF_MATERIAL_CHUNKID)
		{
			zeError("Model Resource", "Corrupted ZEModel file. Materials chunk id does not matches.\r\n");
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

bool ReadPhysicalShapesFromFile(ZEArray<ZEModelResourcePhysicalShape>* Shapes, ZEResourceFile& ResourceFile)
{
	for (size_t I = 0; I < Shapes->GetCount(); I++)
	{
		ZEModelResourcePhysicalShape* PhysicalShape = &(*Shapes)[I];;

		ZEModelFilePhysicalShapeChunk PhysicalShapeChunk;
		ResourceFile.Read(&PhysicalShapeChunk, sizeof(ZEModelFilePhysicalShapeChunk), 1);

		if(PhysicalShapeChunk.ChunkId != ZE_MDLF_PHYSICAL_SHAPE_CHUNKID)
		{
			zeError("Model Resource", "Corrupted ZEModel file. Physical shape chunk id does not matches.\r\n");
			return false;
		}

		PhysicalShape->Type = (ZEPhysicalShapeType)PhysicalShapeChunk.Type;
		PhysicalShape->LocalPosition = PhysicalShapeChunk.LocalPosition;
		PhysicalShape->LocalOrientation = PhysicalShapeChunk.LocalOrientation;
		PhysicalShape->Material.Friction = PhysicalShapeChunk.Material.Friction;
		PhysicalShape->Material.Restitution = PhysicalShapeChunk.Material.Restitution;
		PhysicalShape->Trigger = PhysicalShapeChunk.Trigger;
		PhysicalShape->Mask1 = PhysicalShapeChunk.Mask1;
		PhysicalShape->Mask2 = PhysicalShapeChunk.Mask2;
		PhysicalShape->Mask3 = PhysicalShapeChunk.Mask3;
		PhysicalShape->Mask4 = PhysicalShapeChunk.Mask4;

		switch (PhysicalShape->Type)
		{
			case ZE_PST_PLANE:
			{
				PhysicalShape->Plane.Height = PhysicalShapeChunk.Plane.Height;
				PhysicalShape->Plane.NormalX = PhysicalShapeChunk.Plane.NormalX;
				PhysicalShape->Plane.NormalY = PhysicalShapeChunk.Plane.NormalY;
				PhysicalShape->Plane.NormalZ = PhysicalShapeChunk.Plane.NormalZ;
				break;
			}
			case ZE_PST_BOX:
			{
				PhysicalShape->Box.Widht = PhysicalShapeChunk.Box.Widht;
				PhysicalShape->Box.Height = PhysicalShapeChunk.Box.Height;
				PhysicalShape->Box.Lenght = PhysicalShapeChunk.Box.Lenght;
				break;
			}
			case ZE_PST_SPHERE:
			{
				PhysicalShape->Sphere.Radius = PhysicalShapeChunk.Sphere.Radius;
				break;
			}
			case ZE_PST_CAPSULE:
			{
				PhysicalShape->Capsule.Height = PhysicalShapeChunk.Capsule.Height;
				PhysicalShape->Capsule.Radius = PhysicalShapeChunk.Capsule.Radius;
				break;
			}
			case ZE_PST_CONVEX:
			{
				PhysicalShape->Convex.Scale = PhysicalShapeChunk.Convex.Scale;
				PhysicalShape->Convex.Vertices.Clear();
				for (int i=0;i<PhysicalShapeChunk.Convex.Vertices.GetCount();i++)
				{
					PhysicalShape->Convex.Vertices.Add(PhysicalShapeChunk.Convex.Vertices[i]);
				}
				break;
			}
			case ZE_PST_TRIMESH:
			{
				PhysicalShape->Trimesh.Scale = PhysicalShapeChunk.Trimesh.Scale;
				PhysicalShape->Trimesh.Vertices.Clear();
				for (int i=0;i<PhysicalShapeChunk.Trimesh.Vertices.GetCount();i++)
				{
					PhysicalShape->Trimesh.Vertices.Add(PhysicalShapeChunk.Trimesh.Vertices[i]);
				}
				PhysicalShape->Trimesh.Indices.Clear();
				for (int i=0;i<PhysicalShapeChunk.Trimesh.Indices.GetCount();i++)
				{
					ZEModelResourcePhysicalPolygon p;
					p.VertexIndexes[0] = PhysicalShapeChunk.Trimesh.Indices[i].VertexIndexes[0];
					p.VertexIndexes[1] = PhysicalShapeChunk.Trimesh.Indices[i].VertexIndexes[1];
					p.VertexIndexes[2] = PhysicalShapeChunk.Trimesh.Indices[i].VertexIndexes[2];
					PhysicalShape->Trimesh.Indices.Add(p);
				}
				break;
			}
			default:
				zeError("Model Resource", "Wrong physical shape type.\r\n");
				return false;
		}
	}
	return true;
}

bool ReadMeshesFromFile(ZEModelResource* Model, ZEResourceFile& ResourceFile)
{
	for (size_t I = 0; I < Model->Meshes.GetCount(); I++)
	{
		ZEModelResourceMesh* Mesh = &Model->Meshes[I];

		ZEModelFileMeshChunk MeshChunk;
		ResourceFile.Read(&MeshChunk, sizeof(ZEModelFileMeshChunk), 1);
		
		if(MeshChunk.ChunkId != ZE_MDLF_MESH_CHUNKID)
		{
			zeError("Model Resource",  "Corrupted ZEModel file. Mesh's chunk id does not matches.\r\n");
			return false;
		}

		strncpy(Mesh->Name, MeshChunk.Name, ZE_MDLF_MAX_NAME_SIZE);
		Mesh->IsSkinned = MeshChunk.IsSkinned;
		Mesh->BoundingBox = MeshChunk.BoundingBox;
		Mesh->LODs.SetCount(MeshChunk.LODCount);
		Mesh->Position = MeshChunk.Position;
		Mesh->Orientation = MeshChunk.Orientation;
		//physical...
		Mesh->PhysicalBody.Type = (ZEPhysicalBodyType)MeshChunk.PhysicalBody.Type;
		Mesh->PhysicalBody.Mass = MeshChunk.PhysicalBody.Mass;
		Mesh->PhysicalBody.Kinematic = MeshChunk.PhysicalBody.Kinematic;
		Mesh->PhysicalBody.AngularDamp = MeshChunk.PhysicalBody.AngularDamp;
		Mesh->PhysicalBody.LinearDamp = MeshChunk.PhysicalBody.LinearDamp;
		Mesh->PhysicalBody.Position = MeshChunk.PhysicalBody.Position;
		Mesh->PhysicalBody.Orientation = MeshChunk.PhysicalBody.Orientation;
		Mesh->PhysicalBody.MassCenter = MeshChunk.PhysicalBody.MassCenter;
		Mesh->PhysicalBody.Shapes.SetCount(MeshChunk.PhysicalBody.ShapeCount);

		if (!ReadPhysicalShapesFromFile(&Mesh->PhysicalBody.Shapes, ResourceFile))
		{
			return false;
		}

		for (size_t I = 0; I < Mesh->LODs.GetCount(); I++)
		{
			ZEModelResourceMeshLOD* LOD = &Mesh->LODs[I];
			ZEModelFileMeshLODChunk MeshLODChunk;
			ResourceFile.Read(&MeshLODChunk, sizeof(ZEModelFileMeshLODChunk), 1);
			if (MeshLODChunk.ChunkId != ZE_MDLF_MESH_LOD_CHUNKID)
			{
				zeError("Model Resource",  "Corrupted ZEModel file. Mesh LOD's chunk id does not matches.\r\n");
				return false;			
			}

			LOD->MaterialId = MeshLODChunk.MaterialId;
			LOD->LODLevel = MeshLODChunk.LODLevel;
			if(MeshChunk.IsSkinned)
			{
				LOD->SkinnedVertices.SetCount(MeshLODChunk.VertexCount);				
				ResourceFile.Read(LOD->SkinnedVertices.GetCArray(), sizeof(ZESkinnedModelVertex),  LOD->SkinnedVertices.GetCount());
			}
			else
			{
				LOD->Vertices.SetCount(MeshLODChunk.VertexCount);				
				ResourceFile.Read(LOD->Vertices.GetCArray(), sizeof(ZEModelVertex),  LOD->Vertices.GetCount());
			}
		}
	}
	return true;
}

bool ReadBonesFromFile(ZEModelResource* Model, ZEResourceFile& ResourceFile)
{
	for (size_t I = 0; I < Model->Bones.GetCount(); I++)
	{
		ZEModelResourceBone* Bone = &Model->Bones[I];

		ZEModelFileBoneChunk BoneChunk;
		ResourceFile.Read(&BoneChunk, sizeof(ZEModelFileBoneChunk), 1);

		if(BoneChunk.ChunkId != ZE_MDLF_BONE_CHUNKID)
		{
			zeError("Model Resource",  "Corrupted ZEModel file. Bone chunk id does not matches.\r\n");
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
		
		Bone->PhysicalJoint.Type = BoneChunk.PhysicalJoint.Type;
		Bone->PhysicalJoint.Body1Id = BoneChunk.PhysicalJoint.Body1Id;
		Bone->PhysicalJoint.Body2Id = BoneChunk.PhysicalJoint.Body2Id;
		Bone->PhysicalJoint.Breakable = BoneChunk.PhysicalJoint.Breakable;
		Bone->PhysicalJoint.BreakForce = BoneChunk.PhysicalJoint.BreakForce;
		Bone->PhysicalJoint.BreakTorque = BoneChunk.PhysicalJoint.BreakTorque;
		Bone->PhysicalJoint.CollideBodies = BoneChunk.PhysicalJoint.CollideBodies;
		Bone->PhysicalJoint.UseGlobalAnchor = BoneChunk.PhysicalJoint.UseGlobalAnchor;
		Bone->PhysicalJoint.GlobalAnchor = BoneChunk.PhysicalJoint.GlobalAnchor;
		Bone->PhysicalJoint.UseGlobalAxis = BoneChunk.PhysicalJoint.UseGlobalAxis;
		Bone->PhysicalJoint.GlobalAxis = BoneChunk.PhysicalJoint.GlobalAxis;
		Bone->PhysicalJoint.LocalAnchor1 = BoneChunk.PhysicalJoint.LocalAnchor1;
		Bone->PhysicalJoint.LocalAnchor2 = BoneChunk.PhysicalJoint.LocalAnchor2;
		Bone->PhysicalJoint.LocalAxis1 = BoneChunk.PhysicalJoint.LocalAxis1;
		Bone->PhysicalJoint.LocalAxis2 = BoneChunk.PhysicalJoint.LocalAxis2;

		switch (Bone->PhysicalJoint.Type)
		{
			case ZE_PJT_SPHERICAL:
			{
				Bone->PhysicalJoint.Spherical.SwingLimit = BoneChunk.PhysicalJoint.Spherical.SwingLimit;
				Bone->PhysicalJoint.Spherical.SwingLimitRestitution = BoneChunk.PhysicalJoint.Spherical.SwingLimitRestitution;
				Bone->PhysicalJoint.Spherical.SwingLimitValue = BoneChunk.PhysicalJoint.Spherical.SwingLimitValue;
				Bone->PhysicalJoint.Spherical.TwistLimit = BoneChunk.PhysicalJoint.Spherical.TwistLimit;
				Bone->PhysicalJoint.Spherical.TwistLimitHighValue = BoneChunk.PhysicalJoint.Spherical.TwistLimitHighValue;
				Bone->PhysicalJoint.Spherical.TwistLimitLowValue = BoneChunk.PhysicalJoint.Spherical.TwistLimitLowValue;
				Bone->PhysicalJoint.Spherical.TwistLimitRestitution = BoneChunk.PhysicalJoint.Spherical.TwistLimitRestitution;
				break;
			}
			case ZE_PJT_REVOLUTE:
			{
				Bone->PhysicalJoint.Revolute.HasLimit = BoneChunk.PhysicalJoint.Revolute.HasLimit;
				Bone->PhysicalJoint.Revolute.HasMotor = BoneChunk.PhysicalJoint.Revolute.HasMotor;
				Bone->PhysicalJoint.Revolute.HasSpring = BoneChunk.PhysicalJoint.Revolute.HasSpring;
				Bone->PhysicalJoint.Revolute.LimitHighValue = BoneChunk.PhysicalJoint.Revolute.LimitHighValue;
				Bone->PhysicalJoint.Revolute.LimitLowValue = BoneChunk.PhysicalJoint.Revolute.LimitLowValue;
				Bone->PhysicalJoint.Revolute.LimitRestitution = BoneChunk.PhysicalJoint.Revolute.LimitRestitution;
				Bone->PhysicalJoint.Revolute.MotorForce = BoneChunk.PhysicalJoint.Revolute.MotorForce;
				Bone->PhysicalJoint.Revolute.MotorVelocity = BoneChunk.PhysicalJoint.Revolute.MotorVelocity;
				Bone->PhysicalJoint.Revolute.SpringDamper = BoneChunk.PhysicalJoint.Revolute.SpringDamper;
				Bone->PhysicalJoint.Revolute.SpringTarget = BoneChunk.PhysicalJoint.Revolute.SpringTarget;
				Bone->PhysicalJoint.Revolute.SpringValue = BoneChunk.PhysicalJoint.Revolute.SpringValue;
				break;
			}
			case ZE_PJT_DISTANCE:
			{
				Bone->PhysicalJoint.Distance.HasMaxLimit = BoneChunk.PhysicalJoint.Distance.HasMaxLimit;
				Bone->PhysicalJoint.Distance.HasMinLimit = BoneChunk.PhysicalJoint.Distance.HasMinLimit;
				Bone->PhysicalJoint.Distance.HasSpring = BoneChunk.PhysicalJoint.Distance.HasSpring;
				Bone->PhysicalJoint.Distance.MaxDistance = BoneChunk.PhysicalJoint.Distance.MaxDistance;
				Bone->PhysicalJoint.Distance.MinDistance = BoneChunk.PhysicalJoint.Distance.MinDistance;
				Bone->PhysicalJoint.Distance.SpringDamper = BoneChunk.PhysicalJoint.Distance.SpringDamper;
				Bone->PhysicalJoint.Distance.SpringValue = BoneChunk.PhysicalJoint.Distance.SpringValue;
				break;
			}
			case ZE_PJT_PULLEY:
			{
				Bone->PhysicalJoint.Pulley.Distance = BoneChunk.PhysicalJoint.Pulley.Distance;
				Bone->PhysicalJoint.Pulley.HasMotor = BoneChunk.PhysicalJoint.Pulley.HasMotor;
				Bone->PhysicalJoint.Pulley.IsRigid = BoneChunk.PhysicalJoint.Pulley.IsRigid;
				Bone->PhysicalJoint.Pulley.MotorForce = BoneChunk.PhysicalJoint.Pulley.MotorForce;
				Bone->PhysicalJoint.Pulley.MotorVelocity = BoneChunk.PhysicalJoint.Pulley.MotorVelocity;
				Bone->PhysicalJoint.Pulley.Pulley1 = BoneChunk.PhysicalJoint.Pulley.Pulley1;
				Bone->PhysicalJoint.Pulley.Pulley2 = BoneChunk.PhysicalJoint.Pulley.Pulley2;
				Bone->PhysicalJoint.Pulley.Ratio = BoneChunk.PhysicalJoint.Pulley.Ratio;
				Bone->PhysicalJoint.Pulley.Stiffness = BoneChunk.PhysicalJoint.Pulley.Stiffness;
				break;
			}
			case ZE_PJT_FREE:
			{
				Bone->PhysicalJoint.Free.AngularMotor = BoneChunk.PhysicalJoint.Free.AngularMotor;
				Bone->PhysicalJoint.Free.AngularMotorDamper = BoneChunk.PhysicalJoint.Free.AngularMotorDamper;
				Bone->PhysicalJoint.Free.AngularMotorForce = BoneChunk.PhysicalJoint.Free.AngularMotorForce;
				Bone->PhysicalJoint.Free.AngularMotorOrientation = BoneChunk.PhysicalJoint.Free.AngularMotorOrientation;
				Bone->PhysicalJoint.Free.AngularMotorSpring = BoneChunk.PhysicalJoint.Free.AngularMotorSpring;
				Bone->PhysicalJoint.Free.AngularMotorVelocity = BoneChunk.PhysicalJoint.Free.AngularMotorVelocity;
				Bone->PhysicalJoint.Free.LinearLimitDamper = BoneChunk.PhysicalJoint.Free.LinearLimitDamper;
				Bone->PhysicalJoint.Free.LinearLimitRestitution = BoneChunk.PhysicalJoint.Free.LinearLimitRestitution;
				Bone->PhysicalJoint.Free.LinearLimitSpring = BoneChunk.PhysicalJoint.Free.LinearLimitSpring;
				Bone->PhysicalJoint.Free.LinearLimitValue = BoneChunk.PhysicalJoint.Free.LinearLimitValue;
				Bone->PhysicalJoint.Free.LinearMotorPosition = BoneChunk.PhysicalJoint.Free.LinearMotorPosition;
				Bone->PhysicalJoint.Free.LinearMotorVelocity = BoneChunk.PhysicalJoint.Free.LinearMotorVelocity;
				Bone->PhysicalJoint.Free.LinearXMotor = BoneChunk.PhysicalJoint.Free.LinearXMotor;
				Bone->PhysicalJoint.Free.LinearXMotorDamper = BoneChunk.PhysicalJoint.Free.LinearXMotorDamper;
				Bone->PhysicalJoint.Free.LinearXMotorForce = BoneChunk.PhysicalJoint.Free.LinearXMotorForce;
				Bone->PhysicalJoint.Free.LinearXMotorSpring = BoneChunk.PhysicalJoint.Free.LinearXMotorSpring;
				Bone->PhysicalJoint.Free.LinearYMotor = BoneChunk.PhysicalJoint.Free.LinearYMotor;
				Bone->PhysicalJoint.Free.LinearYMotorDamper = BoneChunk.PhysicalJoint.Free.LinearYMotorDamper;
				Bone->PhysicalJoint.Free.LinearYMotorForce = BoneChunk.PhysicalJoint.Free.LinearYMotorForce;
				Bone->PhysicalJoint.Free.LinearYMotorSpring = BoneChunk.PhysicalJoint.Free.LinearYMotorSpring;
				Bone->PhysicalJoint.Free.LinearZMotor = BoneChunk.PhysicalJoint.Free.LinearZMotor;
				Bone->PhysicalJoint.Free.LinearZMotorDamper = BoneChunk.PhysicalJoint.Free.LinearZMotorDamper;
				Bone->PhysicalJoint.Free.LinearZMotorForce = BoneChunk.PhysicalJoint.Free.LinearZMotorForce;
				Bone->PhysicalJoint.Free.LinearZMotorSpring = BoneChunk.PhysicalJoint.Free.LinearZMotorSpring;
				Bone->PhysicalJoint.Free.Swing1LimitDamper = BoneChunk.PhysicalJoint.Free.Swing1LimitDamper;
				Bone->PhysicalJoint.Free.Swing1LimitRestitution = BoneChunk.PhysicalJoint.Free.Swing1LimitRestitution;
				Bone->PhysicalJoint.Free.Swing1LimitSpring = BoneChunk.PhysicalJoint.Free.Swing1LimitSpring;
				Bone->PhysicalJoint.Free.Swing1LimitValue = BoneChunk.PhysicalJoint.Free.Swing1LimitValue;
				Bone->PhysicalJoint.Free.Swing2LimitDamper = BoneChunk.PhysicalJoint.Free.Swing2LimitDamper;
				Bone->PhysicalJoint.Free.Swing2LimitRestitution = BoneChunk.PhysicalJoint.Free.Swing2LimitRestitution;
				Bone->PhysicalJoint.Free.Swing2LimitSpring = BoneChunk.PhysicalJoint.Free.Swing2LimitSpring;
				Bone->PhysicalJoint.Free.Swing2LimitValue = BoneChunk.PhysicalJoint.Free.Swing2LimitValue;
				Bone->PhysicalJoint.Free.SwingMotion1 = BoneChunk.PhysicalJoint.Free.SwingMotion1;
				Bone->PhysicalJoint.Free.SwingMotion2 = BoneChunk.PhysicalJoint.Free.SwingMotion2;
				Bone->PhysicalJoint.Free.TwistLimitDamper = BoneChunk.PhysicalJoint.Free.TwistLimitDamper;
				Bone->PhysicalJoint.Free.TwistLimitHighValue = BoneChunk.PhysicalJoint.Free.TwistLimitHighValue;
				Bone->PhysicalJoint.Free.TwistLimitLowValue = BoneChunk.PhysicalJoint.Free.TwistLimitLowValue;
				Bone->PhysicalJoint.Free.TwistLimitRestitution = BoneChunk.PhysicalJoint.Free.TwistLimitRestitution;
				Bone->PhysicalJoint.Free.TwistLimitSpring = BoneChunk.PhysicalJoint.Free.TwistLimitSpring;
				Bone->PhysicalJoint.Free.TwistMotion = BoneChunk.PhysicalJoint.Free.TwistMotion;
				Bone->PhysicalJoint.Free.XMotion = BoneChunk.PhysicalJoint.Free.XMotion;
				Bone->PhysicalJoint.Free.YMotion = BoneChunk.PhysicalJoint.Free.YMotion;
				Bone->PhysicalJoint.Free.ZMotion = BoneChunk.PhysicalJoint.Free.ZMotion;
				break;
			}
		}
	}

	return true;
}

bool ReadAnimationsFromFile(ZEModelResource* Model, ZEResourceFile& ResourceFile)
{
	for (size_t I = 0; I < Model->Animations.GetCount(); I++)
	{
		ZEModelResourceAnimation* Animation = &Model->Animations[I];

		ZEModelFileAnimationChunk AnimationChunk;
		ResourceFile.Read(&AnimationChunk, sizeof(ZEModelFileAnimationChunk), 1);

		if(AnimationChunk.ChunkId != ZE_MDLF_ANIMATION_CHUNKID)
		{
			zeError("Model Resource", "Corrupted ZEModel file. Animation chunk id does not matches.\r\n");
			return false;
		}

		Animation->Frames.SetCount(AnimationChunk.FrameCount);
		strncpy(Animation->Name, AnimationChunk.Name, ZE_MDLF_MAX_NAME_SIZE);

		for (size_t I = 0; I < Animation->Frames.GetCount(); I++)
		{
			ZEModelResourceAnimationFrame* CurrentAnimationFrame = &Animation->Frames[I];

			ZEModelFileAnimationFrameChunk FileAnimationFrame;
			ResourceFile.Read(&FileAnimationFrame, sizeof(ZEModelFileAnimationFrameChunk), 1);

			if(FileAnimationFrame.ChunkId != ZE_MDLF_ANIMATION_KEYFRAME_CHUNKID)
			{
				zeError("Model Resource", "Corrupted ZEModel file. Animation frame chunk id does not matches.\r\n");
				return false;
			}

			CurrentAnimationFrame->BoneKeys.SetCount(FileAnimationFrame.BoneKeyCount);
			ResourceFile.Read(CurrentAnimationFrame->BoneKeys.GetCArray(), sizeof(ZEModelResourceAnimationKey), CurrentAnimationFrame->BoneKeys.GetCount());

			CurrentAnimationFrame->MeshKeys.SetCount(FileAnimationFrame.MeshKeyCount);		
			ResourceFile.Read(CurrentAnimationFrame->MeshKeys.GetCArray(), sizeof(ZEModelResourceAnimationKey), CurrentAnimationFrame->MeshKeys.GetCount());
		}
	}
	return true;
}

bool ReadModelFromFile(ZEModelResource* Model, ZEResourceFile& ResourceFile)
{
	ZEModelFileHeaderChunk HeaderChunk;
	ResourceFile.Read(&HeaderChunk, sizeof(ZEModelFileHeaderChunk), 1);

	if(HeaderChunk.HEADER != ZE_MDLF_HEADER)
	{
		zeError("Model Resource", "Unknown ZEModel file format.\r\n");
		return false;
	}

	if(HeaderChunk.Version != ZE_MDLF_VERSION)
	{
		zeError("Model Resource", "ZEModel file version mismatched.\r\n");
		return false;	
	}

	Model->Materials.SetCount(HeaderChunk.MaterialCount);
	if (!ReadMaterialsFromFile(Model, ResourceFile))
	{
		zeError("Model Resource", "Corrupted ZEModel file. Can not read model file.\r\n");
		return false;
	}

	Model->Meshes.SetCount(HeaderChunk.MeshCount);
	if(!ReadMeshesFromFile(Model, ResourceFile))
	{
		zeError("Model Resource", "Corrupted ZEModel file. Can not read model file.\r\n");
		return false;
	}

	Model->Bones.SetCount(HeaderChunk.BoneCount);
	if(!ReadBonesFromFile(Model, ResourceFile))
	{
		zeError("Model Resource", "Corrupted ZEModel file. Can not read model file.\r\n");
		return false;
	}

	Model->Animations.SetCount(HeaderChunk.AnimationCount);
	if(!ReadAnimationsFromFile(Model, ResourceFile))
	{
		zeError("Model Resource", "Corrupted ZEModel file. Can not read model file.\r\n");
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
		if (!ReadModelFromFile(NewResource, ResourceFile))
		{
			zeError("Model Resource", "Can not load model file. (Filename : \"%s\")", Filename);
			delete NewResource;
			return NULL;
		}
		else
			return NewResource;
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
