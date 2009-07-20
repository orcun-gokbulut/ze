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

		PhysicalShape->Type = PhysicalShapeChunk.Type;
		PhysicalShape->Position = PhysicalShapeChunk.Position;
		PhysicalShape->Orientation = PhysicalShapeChunk.Orientation;
		PhysicalShape->Mass = PhysicalShapeChunk.Mass;
		PhysicalShape->Material.Enabled = PhysicalShapeChunk.Material.Enabled;
		PhysicalShape->Material.StaticFriction = PhysicalShapeChunk.Material.StaticFriction;
		PhysicalShape->Material.DynamicFriction = PhysicalShapeChunk.Material.DynamicFriction;
		PhysicalShape->Material.Restitution = PhysicalShapeChunk.Material.Restitution;

		switch (PhysicalShape->Type)
		{
			case ZE_PST_BOX:
				PhysicalShape->Box.Widht = PhysicalShapeChunk.Box.Widht;
				PhysicalShape->Box.Height = PhysicalShapeChunk.Box.Height;
				PhysicalShape->Box.Lenght = PhysicalShapeChunk.Box.Lenght;
				break;
			case ZE_PST_SPHERE:
				PhysicalShape->Sphere.Radius = PhysicalShapeChunk.Sphere.Radius;
				break;
			case ZE_PST_CYLINDER:
				PhysicalShape->Cylinder.Height = PhysicalShapeChunk.Cylinder.Height;
				PhysicalShape->Cylinder.Radius = PhysicalShapeChunk.Cylinder.Radius;
				break;

			case ZE_PST_CAPSULE:
				PhysicalShape->Capsule.Height = PhysicalShapeChunk.Capsule.Height;
				PhysicalShape->Capsule.Radius = PhysicalShapeChunk.Capsule.Radius;
				break;
			case ZE_PST_MESH:
				PhysicalShape->Mesh.Indices.SetCount(PhysicalShapeChunk.Mesh.IndexCount);
				ResourceFile.Read(PhysicalShape->Mesh.Indices.GetCArray(), sizeof(ZEModelFilePhysicalPolygon), PhysicalShape->Mesh.Indices.GetCount());
				PhysicalShape->Mesh.Vertices.SetCount(PhysicalShapeChunk.Mesh.VertexCount);
				ResourceFile.Read(PhysicalShape->Mesh.Vertices.GetCArray(), sizeof(ZEVector3), PhysicalShape->Mesh.Vertices.GetCount());
				break;
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
		Mesh->PhysicalBody.Type = (ZEPhysicalBodyType)MeshChunk.PhysicalBody.Type;
		Mesh->PhysicalBody.Shapes.SetCount(MeshChunk.PhysicalBody.ShapeCount);

		if (!ReadPhysicalShapesFromFile(&Mesh->PhysicalBody.Shapes, ResourceFile))
			return false;

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
		Bone->PhysicalBody.Shapes.SetCount(BoneChunk.PhysicalBody.ShapeCount);
		Bone->PhysicalBody.Type = (ZEPhysicalBodyType)BoneChunk.PhysicalBody.Type;

		if (!ReadPhysicalShapesFromFile(&Bone->PhysicalBody.Shapes, ResourceFile))
			return false;
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
