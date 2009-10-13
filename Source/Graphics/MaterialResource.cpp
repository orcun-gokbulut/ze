//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - MaterialResource.cpp
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

#include "MaterialResource.h"
#include "MaterialFileFormat.h"
#include "Texture2DResource.h"
#include "TextureCubeResource.h"
#include "Core/ResourceFile.h"
#include "Core/Error.h"
#include "Core/ResourceManager.h"

const char* ZEMaterialResource::GetResourceType() const
{
	return "Material";
}

const ZEMaterial* ZEMaterialResource::GetMaterial() const
{
	return Material;
}

ZEMaterialType ZEMaterialResource::GetMaterialType() const
{
	return MaterialType;
}

const ZEArray<ZETextureResource*> ZEMaterialResource::GetTextures() const
{
	return TextureResources;
}

const ZEArray<ZEMaterialAnimation> ZEMaterialResource::GetAnimations() const
{
	return MaterialAnimations;
}

const ZEMaterialAnimation* ZEMaterialResource::GetAnimationByName(const char* Name) const
{
	for (size_t I = 0; I < MaterialAnimations.GetCount(); I++)
		if (strnicmp(MaterialAnimations[I].Name, Name, ZE_MTLF_MAX_NAME_SIZE) == 0)
			return &MaterialAnimations[I];

	return NULL;
}

bool ZEMaterialResource::LoadTextures(ZEMaterialResource* MaterialResource, ZEResourceFile* ResourceFile)
{

	for (size_t I = 0; I < MaterialResource->TextureResources.GetCount(); I++)
	{
		ZEMaterialFileTextureChunk TextureChunk;
		ZETextureResource* CurrentTextureResource = MaterialResource->TextureResources[I];

		ResourceFile->Read(&TextureChunk, sizeof(ZEMaterialFileTextureChunk), 1);
		if (TextureChunk.ChunkId != ZE_MTLF_TEXTURE_CHUNK_ID)
		{
			zeError("Material Resource", "Texture chunk id does not matches. (FileName : \"%s\"", ResourceFile->GetFileName());
			return false;
		}

		size_t OldPos;
		switch(TextureChunk.TextureType)
		{
			case ZE_TT_CUBE:
				if (TextureChunk.InternalSize != 0)
				{
					OldPos = ResourceFile->Tell();					
					CurrentTextureResource = ZETextureCubeResource::LoadResource(ResourceFile);
					ResourceFile->Seek(OldPos + TextureChunk.InternalSize, ZE_SF_BEGINING);
					if (CurrentTextureResource == NULL)
						zeWarning("Material Resource", "Can not load material internal texture. (Material FileName : \"%s\", Texture Index : %d)", ResourceFile->GetFileName(), I);
				}
				else
				{
					CurrentTextureResource = ZETextureCubeResource::LoadSharedResource(TextureChunk.FileName);
					if (CurrentTextureResource == NULL)
						zeWarning("Material Resource", "Can not load material texture. (Material FileName : \"%s\", Texture Index : %d, Texture FileName : \"%s\"", ResourceFile->GetFileName(), I, TextureChunk.FileName);
				}
				break;

			case ZE_TT_2D:
				if (TextureChunk.InternalSize != 0)
				{
					OldPos = ResourceFile->Tell();					
					CurrentTextureResource = ZETexture2DResource::LoadResource(ResourceFile);
					ResourceFile->Seek(OldPos + TextureChunk.InternalSize, ZE_SF_BEGINING);
					if (CurrentTextureResource == NULL)
						zeWarning("Material Resource", "Can not load material internal texture. (Material FileName : \"%s\", Texture Index : %d)", ResourceFile->GetFileName(), I);
				}
				else
				{
					CurrentTextureResource = ZETexture2DResource::LoadSharedResource(TextureChunk.FileName);
					if (CurrentTextureResource == NULL)
						zeWarning("Material Resource", "Can not load material texture. (Material FileName : \"%s\", Texture Index : %d, Texture FileName : \"%s\"", ResourceFile->GetFileName(), I, TextureChunk.FileName);
				}
				break;

			case ZE_TT_3D:
				if (TextureChunk.InternalSize != 0)
				{
					zeWarning("Material Resource", "3D Textures is not supported in materials. (Material FileName : \"%s\", Texture Index : %d)", ResourceFile->GetFileName(), I);
					ResourceFile->Seek(TextureChunk.InternalSize, ZE_SF_CURRENT);
				}
				else
					zeWarning("Material Resource", "3D Textures is not supported in materials. (Material FileName : \"%s\", Texture Index : %d, Texture FileName : \"%s\")", ResourceFile->GetFileName(), I);
				break;

			default:
				zeError("Material Resource", "Wrong texture type. Corrupted ZEMaterial file. (Material FileName : \"%s\", Texture Index : %d)", ResourceFile->GetFileName(), I);
				return false;
				break;
		}
	}

	return false;
}

bool ZEMaterialResource::LoadFixedMaterial(ZEMaterialResource* MaterialResource, ZEResourceFile* ResourceFile)
{
	ZEMaterialFileFixedMaterialChunk MaterialChunk;
	ResourceFile->Read(&MaterialChunk, sizeof(ZEMaterialFileFixedMaterialChunk), 1);
	if (MaterialChunk.ChunkId != ZE_MTLF_MATERIAL_CHUNKID)
	{
		zeError("Material Resource", "Material chunk id does not match. (Material FileName : \"%s\")", ResourceFile->GetFileName());
		return false;
	}

	ZEFixedMaterial* Material = ZEFixedMaterial::CreateInstance();
	MaterialResource->Material = Material;

	Material->SetTwoSided(MaterialChunk.TwoSided);
	Material->SetLightningEnabled(MaterialChunk.LightningEnabled);
	Material->SetWireframe(MaterialChunk.Wireframe);
	Material->SetTransparancyMode(MaterialChunk.TransparancyMode);
	Material->SetRecivesShadow(MaterialChunk.RecivesShadow);
	Material->SetTransparancyCullLimit(MaterialChunk.TransparancyCullLimit);

	Material->SetAmbientEnabled(MaterialChunk.AmbientEnabled);
	Material->SetAmbientColor(MaterialChunk.AmbientColor);
	Material->SetAmbientFactor(MaterialChunk.AmbientFactor);
	
	Material->SetDiffuseEnabled(MaterialChunk.DiffuseEnabled);
	Material->SetDiffuseColor(MaterialChunk.DiffuseColor);
	Material->SetDiffuseFactor(MaterialChunk.DiffuseFactor);
	if (MaterialChunk.DiffuseMap != -1)
		Material->SetDiffuseMap(((ZETexture2DResource*)MaterialResource->TextureResources[MaterialChunk.DiffuseMap])->GetTexture());
	Material->SetDiffuseMapAddressModeU((ZETextureAddressMode)MaterialChunk.DiffuseMapAddressModeU);
	Material->SetDiffuseMapAddressModeV((ZETextureAddressMode)MaterialChunk.DiffuseMapAddressModeV);

	if (MaterialChunk.NormalMap != -1)
		Material->SetNormalMap(((ZETexture2DResource*)MaterialResource->TextureResources[MaterialChunk.NormalMap])->GetTexture());
	Material->SetNormalMapAddressModeU((ZETextureAddressMode)MaterialChunk.NormalMapAddressModeU);
	Material->SetNormalMapAddressModeV((ZETextureAddressMode)MaterialChunk.NormalMapAddressModeV);

	if (MaterialChunk.ParallaxMap != -1)
		Material->SetParallaxMap(((ZETexture2DResource*)MaterialResource->TextureResources[MaterialChunk.ParallaxMap])->GetTexture());
	Material->SetParallaxMapAddressModeU((ZETextureAddressMode)MaterialChunk.ParallaxMapAddressModeU);
	Material->SetParallaxMapAddressModeV((ZETextureAddressMode)MaterialChunk.ParallaxMapAddressModeV);

	Material->SetSpecularEnabled(MaterialChunk.SpecularEnabled);
	Material->SetSpecularColor(MaterialChunk.SpecularColor);
	Material->SetSpecularFactor(MaterialChunk.SpecularFactor);
	Material->SetSpecularShininess(MaterialChunk.SpecularShininess);		
	if (MaterialChunk.SpecularMap != -1)
		Material->SetSpecularMap(((ZETexture2DResource*)MaterialResource->TextureResources[MaterialChunk.SpecularMap])->GetTexture());
	Material->SetSpecularMapAddressModeU((ZETextureAddressMode)MaterialChunk.SpecularMapAddressModeU);
	Material->SetSpecularMapAddressModeV((ZETextureAddressMode)MaterialChunk.SpecularMapAddressModeV);


	Material->SetEmmisiveEnabled(MaterialChunk.EmmisiveEnabled);
	Material->SetEmmisiveColor(MaterialChunk.EmmisiveColor);
	Material->SetEmmisiveFactor(MaterialChunk.EmmisiveFactor);
	if (MaterialChunk.EmmisiveMap != -1)
		Material->SetEmmisiveMap(((ZETexture2DResource*)MaterialResource->TextureResources[MaterialChunk.EmmisiveMap])->GetTexture());
	Material->SetEmmisiveMapAddressModeU((ZETextureAddressMode)MaterialChunk.EmmisiveMapAddressModeU);
	Material->SetEmmisiveMapAddressModeV((ZETextureAddressMode)MaterialChunk.EmmisiveMapAddressModeV);

	Material->SetOpacityEnabled(MaterialChunk.OpacityEnabled);
	Material->SetOpacity(MaterialChunk.Opacity);
	if (MaterialChunk.OpacityMap != -1)
		Material->SetOpacityMap(((ZETexture2DResource*)MaterialResource->TextureResources[MaterialChunk.OpacityMap])->GetTexture());
	Material->SetOpacityMapAddressModeU((ZETextureAddressMode)MaterialChunk.OpacityMapAddressModeU);
	Material->SetOpacityMapAddressModeV((ZETextureAddressMode)MaterialChunk.OpacityMapAddressModeV);

	Material->SetDetailMapEnabled(MaterialChunk.DetailMapEnabled);
	Material->SetDetailMapUScale(MaterialChunk.DetailMapTiling.x);
	Material->SetDetailMapVScale(MaterialChunk.DetailMapTiling.y);
	if (MaterialChunk.DetailDiffuseMap != -1)
		Material->SetDetailDiffuseMap(((ZETexture2DResource*)MaterialResource->TextureResources[MaterialChunk.DetailDiffuseMap])->GetTexture());
	Material->SetDetailDiffuseMapAddressModeU((ZETextureAddressMode)MaterialChunk.DetailDiffuseMapAddressModeU);
	Material->SetDetailDiffuseMapAddressModeV((ZETextureAddressMode)MaterialChunk.DetailDiffuseMapAddressModeV);
	if (MaterialChunk.DetailNormalMap != -1)
		Material->SetDetailNormalMap(((ZETexture2DResource*)MaterialResource->TextureResources[MaterialChunk.DetailNormalMap])->GetTexture());
	Material->SetDetailNormalMapAddressModeU((ZETextureAddressMode)MaterialChunk.DetailNormalMapAddressModeU);
	Material->SetDetailNormalMapAddressModeV((ZETextureAddressMode)MaterialChunk.DetailNormalMapAddressModeV);

	Material->SetReflectionEnabled(MaterialChunk.ReflectionEnabled);
	Material->SetReflectionFactor(MaterialChunk.ReflectionFactor);
	if (MaterialChunk.ReflectionMap != -1)
		Material->SetReflectionMap(((ZETextureCubeResource*)MaterialResource->TextureResources[MaterialChunk.ReflectionMap])->GetTexture());
	Material->SetReflectionMapAddressModeU((ZETextureAddressMode)MaterialChunk.ReflectionMapAddressModeU);
	Material->SetReflectionMapAddressModeV((ZETextureAddressMode)MaterialChunk.ReflectionMapAddressModeV);
	Material->SetReflectionMapAddressModeW((ZETextureAddressMode)MaterialChunk.ReflectionMapAddressModeW);

	Material->SetRefractionEnabled(MaterialChunk.RefractionEnabled);
	Material->SetRefractionIndex(MaterialChunk.RefractionIndex);
	Material->SetRefractionFactor(MaterialChunk.RefractionFactor);
	if (MaterialChunk.RefractionMap != -1)
		Material->SetRefractionMap(((ZETextureCubeResource*)MaterialResource->TextureResources[MaterialChunk.RefractionMap])->GetTexture());
	Material->SetRefractionMapAddressModeU((ZETextureAddressMode)MaterialChunk.RefractionMapAddressModeU);
	Material->SetRefractionMapAddressModeV((ZETextureAddressMode)MaterialChunk.RefractionMapAddressModeV);
	Material->SetRefractionMapAddressModeW((ZETextureAddressMode)MaterialChunk.RefractionMapAddressModeW);

	Material->SetLightMapEnabled(MaterialChunk.LightMapEnabled);
	Material->SetLightMapFactor(MaterialChunk.LightMapFactor);
	if (MaterialChunk.LightMap != -1)
		Material->SetLightMap(((ZETexture2DResource*)MaterialResource->TextureResources[MaterialChunk.LightMap])->GetTexture());
	Material->SetLightMapAddressModeU((ZETextureAddressMode)MaterialChunk.LightMapAddressModeU);
	Material->SetLightMapAddressModeV((ZETextureAddressMode)MaterialChunk.LightMapAddressModeV);

	Material->SetDistortionEnabled(MaterialChunk.DistortionEnabled);
	Material->SetDistortionFactor(MaterialChunk.DistortionFactor);
	if (MaterialChunk.DistortionMap != -1)
		Material->SetDistortionMap(((ZETexture2DResource*)MaterialResource->TextureResources[MaterialChunk.DistortionMap])->GetTexture());
	Material->SetDistortionMapAddressModeU((ZETextureAddressMode)MaterialChunk.DistortionMapAddressModeU);
	Material->SetDistortionMapAddressModeV((ZETextureAddressMode)MaterialChunk.DistortionMapAddressModeV);	

	return true;
}

bool ZEMaterialResource::LoadAnimations(ZEMaterialResource* MaterialResource, ZEResourceFile* ResourceFile)
{
	ZEMaterialFileAnimationChunk AnimationChunk;

	for (size_t I = 0; I < MaterialResource->MaterialAnimations.GetCount(); I++)
	{
		ZEMaterialAnimation* CurrentAnimation = &MaterialResource->MaterialAnimations[I];

		ResourceFile->Read(&AnimationChunk, sizeof(ZEMaterialFileAnimationChunk), 1);
		if (AnimationChunk.ChunkId != ZE_MTLF_MATERIAL_CHUNKID)
		{
			zeError("Material Resource", "Material animation chunk id does not match. (Material FileName : \"%s\", Animation Index : %d)", ResourceFile->GetFileName(), I);
			return false;
		}

		strncpy(CurrentAnimation->Name, AnimationChunk.Name, ZE_MTLF_MAX_NAME_SIZE);

		CurrentAnimation->Frames.SetCount(AnimationChunk.FrameCount);
		for (size_t N = 0; N < CurrentAnimation->Frames.GetCount(); N++)
		{
			ZEMaterialFileAnimationFrameChunk AnimationFrameChunk;
			ResourceFile->Read(&AnimationFrameChunk, sizeof(ZEMaterialFileAnimationFrameChunk), 1);
			
			if (AnimationFrameChunk.ChunkId != ZE_MTLF_ANIMATION_FRAME_CHUNKID)
			{
				zeError("Material Resource", "Material animation frame chunk id does not match. (Material FileName : \"%s\", Animation Index : %d, Frame Index : %d)", ResourceFile->GetFileName(), I, N);
				return false;
			}

			ZEMaterialAnimationFrame* CurrentFrame = &CurrentAnimation->Frames[N];
			ZEMaterialFileAnimationKeyChunk AnimationKeyChunk;
			for (size_t M = 0; M < CurrentFrame->Keys.GetCount(); M++)
			{
				ResourceFile->Read(&AnimationKeyChunk, sizeof(ZEMaterialFileAnimationKeyChunk), 1);
				CurrentFrame->Keys[M].PropertyIndex = AnimationKeyChunk.PropertyIndex;
				CurrentFrame->Keys[M].Value.Unserialize(ResourceFile);
			}
		}
	}

	return true;
}


ZEMaterialResource* ZEMaterialResource::LoadResource(ZEResourceFile* ResourceFile)
{
	ZEMaterialFileHeaderChunk HeaderChunk;

	ResourceFile->Read(&HeaderChunk, sizeof(ZEMaterialFileHeaderChunk), 1);
	if (HeaderChunk.HEADER != ZE_MTLF_HEADER)
	{
		zeError("Material Resource", "Unknown ZEMaterial file format. (FileName : \"%s\")", ResourceFile->GetFileName());
		return NULL;
	}

	if (HeaderChunk.Version != ZE_MTLF_VERSION)
	{
		zeError("Material Resource", "ZEMaterial file version mismatched. (FileName : \"%s\")", ResourceFile->GetFileName());
		return NULL;
	}
	ZEMaterialResource* MaterialResource = new ZEMaterialResource();
	
	// Load Textures
	MaterialResource->TextureResources.SetCount(HeaderChunk.TextureCount);
	if (!LoadTextures(MaterialResource, ResourceFile))
	{
		zeError("Material File", "Corrupted material file. (FileName : \"%s\")", ResourceFile->GetFileName());
		delete MaterialResource;
		return NULL;
	}


	// Load Material
	switch(HeaderChunk.MaterialType)
	{
		case ZE_MT_FIXED:
			if (!LoadFixedMaterial(MaterialResource, ResourceFile))
			{
				zeError("Material File", "Corrupted material file. (FileName : \"%s\")", ResourceFile->GetFileName());
				delete MaterialResource;
				return NULL;
			}
			break;
	}

	// Load Animations
	MaterialResource->MaterialAnimations.SetCount(HeaderChunk.AnimationCount);
	if (!LoadAnimations(MaterialResource, ResourceFile))
	{
		zeError("Material File", "Corrupted material file. (FileName : \"%s\")", ResourceFile->GetFileName());
		delete MaterialResource;
		return NULL;
	}

	MaterialResource->SetFileName(ResourceFile->GetFileName());
	MaterialResource->Cached = false;
	MaterialResource->Shared = false;

	return MaterialResource;
}

ZEMaterialResource* ZEMaterialResource::LoadResource(const char* FileName)
{
	ZEResourceFile ResourceFile;

	if (!ResourceFile.Open(FileName))
	{
		zeError("Material File", "Can not open material file. (FileName : \"%s\")", FileName);
		return NULL;
	}
	return LoadResource(&ResourceFile);
}

const ZEMaterialResource* ZEMaterialResource::LoadSharedResource(const char* FileName)
{
	ZEMaterialResource* NewResource =(ZEMaterialResource*)zeResources->GetResource(FileName);
	if (NewResource == NULL)
	{
		NewResource = LoadResource(FileName);
		if (NewResource != NULL)
		{
			NewResource->Shared = true;
			NewResource->Cached = false;
			NewResource->ReferenceCount = 1;
			zeResources->AddResource(NewResource);
			return NewResource;
		}
		else
			return NULL;
	}
	else
		return NewResource;
}
