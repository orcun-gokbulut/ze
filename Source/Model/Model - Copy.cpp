//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Model - Copy.cpp
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

#include "Model.h"
#include "ModelFileFormat.h"
#include <stdio.h>
#include "Core/Core.h"
/*
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CModelResource   CModelResource  CModelResource  CModelResource  CModelResource  CModelResource  CModelResource  CModelResource //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* ZEModelResource::GetResourceType() const
{
	return "Model";
}

void ZEModelResource::LoadHeader(ZEModelResource* ModelResource, ZEResourceFile& ResourceFile)
{
	ZEModelFileHeader		zemfHeader;

	ResourceFile.Read(&zemfHeader, sizeof(ZEModelFileHeader), 1);
	if (zemfHeader.HEADER != ZEMODELFILE_HEADER)
	{
		zeError->RaiseError("Model Resource", ZEERRORLEVEL_NONCRITICAL, "Wrong model ZEModel file format. (Filename : \"%s\")", ResourceFile.GetFilename());
		throw(0);
	}
	if (zemfHeader.Version != ZEMODELFILE_VERSION)
	{
		zeError->RaiseError("Model Resource", ZEERRORLEVEL_NONCRITICAL, "Wrong ZEModel file version. Current ZEModel version : %d.%d. (Filename : \"%s\", File Version : %d.%d)", 
			ZEFILE_GETMAJORVERSION(ZEMODELFILE_VERSION),
			ZEFILE_GETMINORVERSION(ZEMODELFILE_VERSION),
			ResourceFile.GetFilename(),	
			ZEFILE_GETMAJORVERSION(zemfHeader.Version),
			ZEFILE_GETMINORVERSION(zemfHeader.Version));
		throw(0);

	}
	ModelResource->BoundingBox = zemfHeader.BoundingBox;
	ModelResource->InternalTextureResourceCount = zemfHeader.TextureResourceCount;
	ModelResource->Textures.SetCount(zemfHeader.TextureCount);
	ModelResource->Materials.SetCount(zemfHeader.MaterialCount);	
	ModelResource->Meshes.SetCount(zemfHeader.MeshCount);
	ModelResource->Skeleton.SetCount(zemfHeader.BoneCount);
	ModelResource->Animations.SetCount(zemfHeader.AnimationCount);
	ModelResource->AnimationFrames.SetCount(zemfHeader.AnimationFrameCount);
}

void ZEModelResource::LoadMeshes(ZEModelResource* ModelResource, ZEResourceFile& ResourceFile)
{
	ZEModelFileMesh				zemfMesh;
	ZEModelFileVertex			zemfVertex;
	ZEModelFilePolygon			zemfPolygon;
	ZEModelFileVertexWeight		zemfWeight;
	ZEModelMesh*				CurrentMesh;
	
	for (size_t I = 0; I < ModelResource->Meshes.GetCount(); I++)
	{
		CurrentMesh = &ModelResource->Meshes[I];

		ResourceFile.Read(&zemfMesh, sizeof(ZEModelFileMesh), 1);

		strncpy(CurrentMesh->Name, zemfMesh.Name, ZEMODELFILE_MAXNAMESIZE);
		CurrentMesh->BoundingBox = zemfMesh.BoundingBox;
		CurrentMesh->IsSkinned = zemfMesh.Skinned;
		CurrentMesh->Material = &ModelResource->Materials[zemfMesh.MaterialId];

		for (size_t N = 0; N < zemfMesh.VertexCount; N++)
			ResourceFile.Read(&zemfVertex, sizeof(ZEModelFileVertex), 1);

		CurrentMesh->Polygons.SetCount(zemfMesh.PolygonCount);
		for (size_t N = 0; N < zemfMesh.PolygonCount; N++)
			ResourceFile.Read(CurrentMesh->Polygons.GetCArray(), sizeof(ZEModelFilePolygon), CurrentMesh->Polygons.GetCount());

		CurrentMesh->Weights.SetCount(zemfMesh.WeightCount);
		for (size_t N = 0; N < zemfMesh.WeightCount; N++)
			ResourceFile.Read(CurrentMesh->Weights.GetCArray(), sizeof(ZEModelFileVertexWeight), CurrentMesh->Weights.GetCount());
	}
}

void ZEModelResource::LoadBones(ZEModelResource* ModelResource, ZEResourceFile& ResourceFile)
{
	ZEBone*				CurrentBone;
	ZEModelFileBone		zemfBone;
	ZEPoint3			LocalPosition;
	ZEQuaternion		LocalOrientation;
	ZEMatrix4x4			OffsetTransform;

	for (size_t I = 0; I < ModelResource->Skeleton.GetCount(); I++)
	{
		CurrentBone = &ModelResource->Skeleton[I];
		ResourceFile.Read(&zemfBone, sizeof(ZEModelFileBone), 1);
		
		strncpy(CurrentBone->Name, zemfBone.Name, ZEMODELFILE_MAXNAMESIZE);
		CurrentBone->ParentBoneId = zemfBone.ParentBone;
		CurrentBone->BoundingBox = zemfBone.BoundingBox;
		
		CurrentBone->AbsolutePosition = zemfBone.AbsolutePosition;
		LocalPosition = zemfBone.AbsolutePosition;
		LocalOrientation = zemfBone.AbsoluteOrientation;
		
		ZEMatrix4x4::CreateOffset(CurrentBone->FwdTransform, LocalPosition, LocalOrientation);
		ZEMatrix4x4::Inverse(CurrentBone->InvTransform, CurrentBone->FwdTransform);
		MapVector3(CurrentBone->RelativePosition, zemfBone.RelativePosition);
		MapQuaternion(CurrentBone->RelativeOrientation, zemfBone.RelativeOrientation);
	}

	for (size_t I = 0; I < ModelResource->Skeleton.GetCount(); I++)
		for (size_t N = 0; N < ModelResource->Skeleton.GetCount(); N++)
			if (ModelResource->Skeleton[N].ParentBoneId == I)
				ModelResource->Skeleton[I].ChildBoneIds.Add(N);

}

void ZEModelResource::LoadAnimations(ZEModelResource* ModelResource, ZEResourceFile& ResourceFile)
{
	ZEAnimation*	CurrentAnimation;
	ZEModelFileAnimation	zemfAnimation;
	
	for (size_t I = 0; I < ModelResource->Animations.GetCount(); I++)
	{
		CurrentAnimation = &(ModelResource->Animations[I]);
		ResourceFile.Read(&zemfAnimation, sizeof(ZEModelFileAnimation), 1);

		strncpy(CurrentAnimation->Name, zemfAnimation.Name, ZEMODELFILE_MAXNAMESIZE);
		CurrentAnimation->StartFrame = zemfAnimation.StartFrame;
		CurrentAnimation->EndFrame = zemfAnimation.EndFrame;
	}
}

void ZEModelResource::LoadAnimationFrames(ZEModelResource* ModelResource, ZEResourceFile& ResourceFile)
{
	ZEModelFileBoneAnimationKey zemfBoneKey;
	ZEAnimationFrame* CurrentFrame;
	for (size_t FrameId = 0; FrameId < ModelResource->AnimationFrames.GetCount(); FrameId++)
	{
		CurrentFrame = &ModelResource->AnimationFrames[FrameId];
		CurrentFrame->BoneAnimationKeys.SetCount(ModelResource->Skeleton.GetCount());
		for (size_t BoneId = 0; BoneId < ModelResource->Skeleton.GetCount(); BoneId++)
		{
			ResourceFile.Read(&zemfBoneKey, sizeof(ZEModelFileBoneAnimationKey), 1);
			CurrentFrame->BoneAnimationKeys[BoneId].Orientation = zemfBoneKey.RelativeOrientation;
			CurrentFrame->BoneAnimationKeys[BoneId].Position = zemfBoneKey.RelativePosition;
		}
	}
}

void ZEModelResource::LoadTextures(ZEModelResource* ModelResource, ZEResourceFile& File)
{
	ZEArray<ZETextureResource*> InternalTextures;
	for (ZEDWORD I = 0; I < ModelResource->InternalTextureResourceCount; I++)
	{
		ZEDWORD Size;
		ZEPartialResourceFile PartialFile;
		File.Read(&Size, sizeof(ZEDWORD), 1);
		int CurPos = File.Tell();
		File.GetPartialResourceFile(PartialFile, File.Tell(), Size);
		InternalTextures.Add(ZETextureResource::LoadResource(&PartialFile));
		File.Seek(CurPos + Size, SEEKFROM_BEGINING);
	}

	for (ZEDWORD I = 0; I < ModelResource->Textures.GetCount(); I++)
	{
		ZEModelFileTexture	FileTexture;
		File.Read(&FileTexture, sizeof(ZEModelFileTexture), 1);
		if (FileTexture.ResourceId != -1)
			ModelResource->Textures[I] = InternalTextures[FileTexture.ResourceId];
		else
			ModelResource->Textures[I] = ZETextureResource::LoadSharedResource(FileTexture.Filename);
	}
}

void ZEModelResource::LoadMaterials(ZEModelResource* ModelResource, ZEResourceFile& File)
{
	for (ZEDWORD I = 0; I < ModelResource->Materials.GetCount(); I++)
	{
		char Filename[ZEMODELFILE_MAXNAMESIZE];
		ZEDWORD	ParameterCount, Type;
		File.Read(Filename, sizeof(char), ZEMODELFILE_MAXNAMESIZE);
		File.Read(&ParameterCount, sizeof(ZEDWORD), 1);
		ModelResource->Materials[I].SetShaderResource(ZEShaderResource::LoadResource(Filename));
		for (int N = 0; N < ParameterCount; N++)
		{
			int Int;
			bool Bool;
			float Float;
			ZEVector2 Vect2;
			ZEVector3 Vect3;
			ZEVector4 Vect4;
			ZEMatrix3x3 Mat3x3;
			ZEMatrix4x4 Mat4x4;

			File.Read(Filename, sizeof(char), ZEMODELFILE_MAXNAMESIZE);
			File.Read(&Type, sizeof(ZEDWORD), 1);
			
			switch(Type)
			{
				case ZESHADERPARAMETERTYPE_BOOLEAN:
					File.Read(&Bool, sizeof(bool), 1);
					ModelResource->Materials[I].SetShaderParameter(Filename, Bool);
					break;
				case ZESHADERPARAMETERTYPE_INTEGER:
					File.Read(&Int, sizeof(int), 1);
					ModelResource->Materials[I].SetShaderParameter(Filename, Int);
					break;
				case ZESHADERPARAMETERTYPE_FLOAT:
					File.Read(&Float, sizeof(float), 1);
					ModelResource->Materials[I].SetShaderParameter(Filename, Float);
					break;
				case ZESHADERPARAMETERTYPE_VECTOR2:
					File.Read(&Vect2, sizeof(ZEVector2), 1);
					ModelResource->Materials[I].SetShaderParameter(Filename, Vect2);
					break;
				case ZESHADERPARAMETERTYPE_VECTOR3:
					File.Read(&Vect3, sizeof(ZEVector3), 1);
					ModelResource->Materials[I].SetShaderParameter(Filename, Vect3);
					break;
				case ZESHADERPARAMETERTYPE_VECTOR4:
					File.Read(&Vect4, sizeof(ZEVector4), 1);
					ModelResource->Materials[I].SetShaderParameter(Filename, Vect4);
					break;
				case ZESHADERPARAMETERTYPE_MATRIX3X3:
					File.Read(&Mat3x3, sizeof(ZEMatrix3x3), 1);
					ModelResource->Materials[I].SetShaderParameter(Filename, Mat3x3);
					break;
				case ZESHADERPARAMETERTYPE_MATRIX4X4:
					File.Read(&Mat3x3, sizeof(ZEMatrix4x4), 1);
					ModelResource->Materials[I].SetShaderParameter(Filename, Mat4x4);
					break;
				case ZESHADERPARAMETERTYPE_TEXTURE:
					File.Read(&Int, sizeof(int), 1);
					ModelResource->Materials[I].SetShaderParameter(Filename, ModelResource->Textures[Int]);
					break;
			}
		}
	}
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
	ZEModelResource* NewResource = new ZEModelResource();
	NewResource->SetFilename(Filename);
	NewResource->Cached = false;
	NewResource->ReferenceCount = 0;
	
	try	
	{
		ZEResourceFile ResourceFile;
		if (ResourceFile.Open(Filename))
		{

				LoadHeader(NewResource, ResourceFile);
				LoadTextures(NewResource, ResourceFile);
				LoadMaterials(NewResource, ResourceFile);
				LoadMeshes(NewResource, ResourceFile);
				LoadBones(NewResource, ResourceFile);
				LoadAnimations(NewResource, ResourceFile);
				LoadAnimationFrames(NewResource, ResourceFile);
				ResourceFile.Close();
				return NewResource;		
		}
		else
		{
			zeError->RaiseError("Model Resource", ZEERRORLEVEL_NONCRITICAL, "Model file does not exists. (Filename : \"%s\")jjk", Filename);
			throw(0);
		}
	}
	catch(...)
	{
		delete NewResource;
		return NULL;			
	}
}

ZEModelResource::~ZEModelResource()
{
	for (int I = 0; I < Textures.GetCount(); I++)
		Textures[I]->Release();
}
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CModel  CModel  CModel  CModel  CModel  CModel  CModel  CModel  CModel  CModel  CModel  CModel  CModel  CModel  CModel  CModel //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ZEModel::AnimateSkeleton(size_t Frame)
{
	const ZEAnimationFrame* CurrentFrame = &ModelResource->AnimationFrames[Frame];
	for (size_t BoneId = 0; BoneId < Skeleton.GetCount(); BoneId++)
	{
		Skeleton[BoneId].RelativeOrientation = CurrentFrame->BoneAnimationKeys[BoneId].Orientation;
		Skeleton[BoneId].RelativePosition = CurrentFrame->BoneAnimationKeys[BoneId].Position;
	}
	TransformSkeleton();
}

void ZEModel::TransformBone(size_t BoneId)
{
	ZEBone& Bone = Skeleton[BoneId];
	ZEBone& ParentBone = Skeleton[Bone.ParentBoneId];

	if (Bone.ParentBoneId == -1)
	{
		ZEMatrix4x4::CreateOffset(Bone.FwdTransform, Bone.RelativePosition, Bone.RelativeOrientation);
		ZEMatrix4x4::Multiply(Bone.VertexTransform, Bone.InvTransform, Bone.FwdTransform);
	}
	else
	{
		ZEMatrix4x4 RelativeTransform;
		ZEMatrix4x4::CreateOffset(RelativeTransform, Bone.RelativePosition, Bone.RelativeOrientation);
		ZEMatrix4x4::Multiply(Bone.FwdTransform, RelativeTransform, ParentBone.FwdTransform);
		ZEMatrix4x4::Multiply(Bone.VertexTransform, Bone.InvTransform, Bone.FwdTransform);
	}

	ZEMatrix4x4::Multiply(Bone.VertexTransform, Bone.InvTransform, Bone.FwdTransform); 
	ZEMatrix4x4::Transform(Bone.AbsolutePosition, Bone.VertexTransform, ModelResource->Skeleton[BoneId].AbsolutePosition);

	for (size_t I = 0; I < Bone.ChildBoneIds.GetCount(); I++)
		TransformBone(Bone.ChildBoneIds[I]);
}

void ZEModel::TransformSkeleton()
{
	for (size_t I = 0; I < Skeleton.GetCount(); I++)
		if (Skeleton[I].ParentBoneId == -1)
			TransformBone(I);
	ReskinMeshes = true;
}

void ZEModel::SkinMeshes()
{
	/*ZEPoint3* CurrentVertex;

	for (size_t MeshId = 0; MeshId < ModelResource->Meshes.GetCount(); MeshId++)
	{
		const ZEModelMesh& Mesh = ModelResource->Meshes[MeshId];
		ZEMeshRenderList& Output = RenderList[MeshId];			

		Output.RenderState = ZERENDERSTATE_NOTCULLED;
		Output.Material = Mesh.Material;
		Output.WorldMatrix = WorldMatrix;
		Output.VertexNormalList.CopyFrom(Mesh.VertexNormalList);

		if (ReskinMeshes == true && Mesh.IsSkinned)
		{
			ReskinMeshes = false;
			for (size_t I = 0; I < Mesh.VertexPosition3List.GetCount(); I++)
			{
				CurrentVertex = &RenderList[MeshId].VertexPosition3List[I];
				CurrentVertex->x = 
				CurrentVertex->y =
				CurrentVertex->z = 0.0f;
			}

			const ZEWeight* CurrentWeight;
			ZEVector3 NewPos, NewNormal;
			float MinX, MinY, MinZ, MaxX, MaxY, MaxZ;
			bool FirstTime = true;

			for (size_t WeightId = 0; WeightId < Mesh.WeightList.GetCount(); WeightId++)
			{
				CurrentWeight = &Mesh.WeightList[WeightId];
				ZEVector3 &Position = Output.VertexPosition3List[CurrentWeight->VertexId];
				ZEMatrix4x4::Transform(NewPos, Skeleton[CurrentWeight->BoneId].VertexTransform , Mesh.VertexPosition3List[CurrentWeight->VertexId]);
				ZEVector3::Scale(NewPos, NewPos, CurrentWeight->Weight);
				ZEVector3::Add(Position, Position, NewPos);

				ZEVector3 &Normal = Output.VertexNormalList[CurrentWeight->VertexId];
				ZEMatrix4x4::Transform3x3(NewNormal, Skeleton[CurrentWeight->BoneId].VertexTransform , Mesh.VertexNormalList[CurrentWeight->VertexId]);
				ZEVector3::Scale(NewNormal, NewNormal, CurrentWeight->Weight);
				ZEVector3::Add(Normal, Normal, NewNormal);

				if (FirstTime)
				{
					FirstTime = false;
					MinX = Position.x; 
					MinY = Position.y; 
					MinZ = Position.z; 
					
					MaxX = Position.x; 
					MaxY = Position.y; 
					MaxZ = Position.z;
				}
				else
				{
					if (Position.x < MinX)
						MinX = Position.x;
					if (Position.y < MinY)
						MinY = Position.y;
					if (Position.z < MinZ)
						MinZ = Position.z;

					if (Position.x > MaxX)
						MaxX = Position.x;
					if (Position.y > MaxY)
						MaxY = Position.y;
					if (Position.z > MaxZ)
						MaxZ = Position.z;
				}
			}
			MapVector3ToXYZ(Output.BoundingBox.Min, MinX, MinY, MinZ);
			MapVector3ToXYZ(Output.BoundingBox.Max, MaxX, MaxY, MaxZ);
		}
	}*/
}

void ZEModel::Animate()
{
 	if (AnimationState != ANIMATIONSTATE_STOPPED && AnimationState != ANIMATIONSTATE_PAUSED)
		if (CurrentAnimation->StartFrame <= CurrentAnimationFrame && CurrentAnimation->EndFrame > CurrentAnimationFrame)
		{
			CurrentAnimationFrame++;
			AnimateSkeleton(CurrentAnimationFrame);
			TransformSkeleton();
		}
		else
			if (AnimationState == ANIMATIONSTATE_LOOPING)
			{
				CurrentAnimationFrame = 0;
				AnimateSkeleton(CurrentAnimationFrame);
				TransformSkeleton();
			}
			else
			{
				CurrentAnimationFrame = 0;
				AnimationState = ANIMATIONSTATE_STOPPED;
				AnimateSkeleton(CurrentAnimationFrame);
				TransformSkeleton();
			}
}

void ZEModel::Draw()
{
	if (ModelResource != NULL)
	{
		if (Visibility == true)
		{
			ZEMatrix4x4 RotationTransform, TranslationTransform;

			ZEMatrix4x4::CreateOrientation(RotationTransform, Orientation);
			ZEMatrix4x4::CreateTranslation(TranslationTransform, Position);
			ZEMatrix4x4::Multiply(WorldMatrix, RotationTransform, TranslationTransform);		
			SkinMeshes();
			for (size_t I = 0; I < RenderList.GetCount(); I++)
			{
				ZEBoundingBox WorldBoundingBox;
				ZEMatrix4x4::Transform(WorldBoundingBox.Max, WorldMatrix, RenderList[I].BoundingBox.Max);
				ZEMatrix4x4::Transform(WorldBoundingBox.Min, WorldMatrix, RenderList[I].BoundingBox.Min);
				if (zeGraphics->ViewFrustrumTest(WorldBoundingBox))
					zeGraphics->AddRenderListToFrame(&RenderList[I]);
			}
		}
	}
}

void ZEModel::Tick()
{
	if (ModelResource != NULL)
		Animate();
}

void ZEModel::LoadModelResource(const ZEModelResource* NewModelResource)
{
	if (ModelResource != NULL && ModelResource->IsShared())
		ModelResource->Release();
	
	ModelResource = NewModelResource;

	if (ModelResource != NULL)
	{	
		RenderList.SetCount(ModelResource->Meshes.GetCount());
		for (size_t I = 0; I < ModelResource->Meshes.GetCount(); I++)
		{
			const ZEModelMesh &CurMesh = ModelResource->Meshes[I];
			ZERenderList &CurRL = RenderList[I];
			CurRL.Material = CurMesh.Material;
			CurRL.IsUsesStaticBuffers = true;
			CurRL.IsInstanced = false;
			CurRL.IsSkinned = true;
			CurRL.IsIndexed = true;
			CurRL.Lightning = ZEMESHLIGHTNING_ENABLEBOTH;
			CurRL.CullingOperations = ZEMESHCULLING_FRUSTRUMCULLING | ZEMESHCULLING_OCCLUSIONCULLING;
			CurRL.IsOccluder = false;
			CurRL.CastShadow = true;
			CurRL.VertexType = (CurMesh.IsSkinned ? ZEVERTEXTYPE_SKINNEDMODELVERTEX : ZEVERTEXTYPE_MODELVERTEX);
			CurRL.StaticIndexBufferId = zeGraphics->CreateStaticIndexBuffer(sizeof(int) * 3 * CurMesh.Polygons.GetCount());
			
			void* IndexBuffer;
			zeGraphics->LockStaticIndexBuffer(CurRL.StaticIndexBufferId, &IndexBuffer);
			memcpy(IndexBuffer, CurMesh.Polygons.GetConstCArray(), CurMesh.Polygons.GetCount() * sizeof(ZEIndexedPolygon));
			zeGraphics->UnlockStaticIndexBuffer(CurRL.StaticIndexBufferId);
				
			CurRL.StaticVertexBufferId = zeGraphics->CreateStaticVertexBuffer((CurMesh.IsSkinned ? sizeof(ZESkinnedModelVertex) : sizeof(ZEModelVertex)) * CurMesh.Vertices.GetCount());
			if (CurMesh.IsSkinned)
			{
				ZESkinnedModelVertex* Buffer;
				zeGraphics->LockStaticVertexBuffer(CurRL.StaticVertexBufferId, (void**)&Buffer);
				for (int I = 0; I < CurMesh.Vertices.GetCount(); I++)
				{
					memcpy(&Buffer[I], &CurMesh.Vertices[I], sizeof(ZEModelVertex));
					memcpy((unsigned char*)&Buffer[I] + sizeof(ZEModelVertex), &CurMesh.Weights[I], sizeof(ZEVertexWeight));
				}
				zeGraphics->UnlockStaticVertexBuffer(CurRL.StaticVertexBufferId);
			}
			else
			{
				void* Buffer;
				zeGraphics->LockStaticVertexBuffer(CurRL.StaticVertexBufferId, &Buffer);
				memcpy(Buffer, CurMesh.Vertices.GetConstCArray(), sizeof(ZEModelVertex) * CurMesh.Vertices.GetCount());			
				zeGraphics->UnlockStaticVertexBuffer(CurRL.StaticVertexBufferId);
			}
		}

		Skeleton.CopyFrom(ModelResource->Skeleton);
		this->SetStaticFrame(0);
	}
	else
		ModelResource = NULL;
}

void ZEModel::SetVisible()
{
	Visibility = true;
}

void ZEModel::SetHidden() 
{
	Visibility = false;
}

bool ZEModel::IsVisible() 
{
	return Visibility;
}

ZEModelAnimationState ZEModel::GetAnimationState()
{
	return AnimationState;
}


float ZEModel::GetDensity()
{
	return Density;
}

size_t ZEModel::GetCurrentAnimationFrame()
{
	return CurrentAnimationFrame;
}

const char* ZEModel::GetCurrentAnimationName()
{
	return CurrentAnimation->Name;
}

void ZEModel::ResumeAnimation()
{
	if (AnimationState == ANIMATIONSTATE_PAUSED)
		AnimationState = ANIMATIONSTATE_PLAYING;
}

void ZEModel::PlayAnimation(const char* AnimationName, bool Loop)
{
	if (ModelResource != NULL)
	{
		for (size_t AnimationId = 0; AnimationId < ModelResource->Animations.GetCount(); AnimationId++)
			if (_stricmp(AnimationName, ModelResource->Animations[AnimationId].Name) == 0)
			{
				CurrentAnimation = &ModelResource->Animations[AnimationId];
				CurrentAnimationFrame = CurrentAnimation->StartFrame;
				AnimationState = (Loop ? ANIMATIONSTATE_LOOPING : ANIMATIONSTATE_PLAYING);
				return;
			}
		
		zeError->RaiseError("Model", ZEERRORLEVEL_NONCRITICAL, "Can not found animation \"%s\". Filename : \"%s\"", AnimationName, ModelResource->GetFileName());
	}
}

void ZEModel::StopAnimation()
{
	AnimationState = ANIMATIONSTATE_STOPPED;
}

void ZEModel::SetStaticFrame(size_t Frame)
{
	AnimationState = ANIMATIONSTATE_STOPPED;
	CurrentAnimationFrame = Frame;
	AnimateSkeleton(Frame);
}

ZEModel::ZEModel()
{	
	ModelResource = NULL;
	Visibility = true;
	AnimationState = ANIMATIONSTATE_STOPPED;
}

ZEModel::~ZEModel()
{
	if (ModelResource != NULL)
		ModelResource->Release();
}
