//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMaterialResource.cpp
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

#include "ZEMaterialResource.h"
#include "ZEMaterialFileFormat.h"
#include "ZETextureCubeResource.h"
#include "ZETexture2DResource.h"
#include "ZEFile/ZEResourceFile.h"
#include "ZECore/ZEError.h"
#include "ZECore/ZEResourceManager.h"
#include <string.h>

ZEMaterialResource::ZEMaterialResource()
{
	Material = NULL;
}

ZEMaterialResource::~ZEMaterialResource()
{
	if (Material != NULL)
		Material->Release();
}

const char* ZEMaterialResource::GetResourceType() const
{
	return "Material";
}

const ZEMaterial* ZEMaterialResource::GetMaterial() const
{
	return Material;
}

const ZEArray<ZETextureResource*>& ZEMaterialResource::GetTextureResources() const
{
	return TextureResources;
}

const ZEArray<ZEAnimation>& ZEMaterialResource::GetAnimations() const
{
	return Animations;
}

const ZEAnimation* ZEMaterialResource::GetAnimationByName(const char* Name) const
{
	for (size_t I = 0; I < Animations.GetCount(); I++)
		if (strnicmp(Animations[I].Name, Name, ZE_MTLF_MAX_NAME_SIZE) == 0)
			return &Animations[I];

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
	return true;
}

bool ZEMaterialResource::LoadAnimations(ZEMaterialResource* MaterialResource, ZEResourceFile* ResourceFile)
{
	for (size_t I = 0; I < MaterialResource->Animations.GetCount(); I++)
		if (!ZEAnimation::ReadFromFile(ResourceFile, &MaterialResource->Animations[I]))
		{	
			zeError("Material Resource", "Can not read material animation. (FileName : \"%s\")", ResourceFile->GetFileName());
			return false;
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
	MaterialResource->Material = ZEFixedMaterial::CreateInstance();
	MaterialResource->Material->Unserialize(ResourceFile);

	// Load Animations
	MaterialResource->Animations.SetCount(HeaderChunk.AnimationCount);
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




