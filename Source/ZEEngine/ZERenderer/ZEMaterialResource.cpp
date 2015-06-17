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

#include "ZEError.h"
#include "ZEFile/ZEFile.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZETexture/ZETextureOptions.h"
#include "ZEMaterialResource.h"
#include "ZETexture/ZETexture2DResource.h"
#include "ZEMaterialFileFormat.h"
#include "ZETexture/ZETextureCubeResource.h"
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

bool ZEMaterialResource::LoadTextures(ZEMaterialResource* MaterialResource, ZEFile* ResourceFile, const ZETextureOptions* UserOptions)
{
	for (ZESize I = 0; I < MaterialResource->TextureResources.GetCount(); I++)
	{
		ZEMaterialFileTextureChunk TextureChunk;
		ZETextureResource* CurrentTextureResource = MaterialResource->TextureResources[I];

		ResourceFile->Read(&TextureChunk, sizeof(ZEMaterialFileTextureChunk), 1);
		if (TextureChunk.ChunkId != ZE_MTLF_TEXTURE_CHUNK_ID)
		{
			zeError("Texture chunk id does not match. (FileName : \"%s\"", ResourceFile->GetPath().GetValue());
			return false;
		}

		ZEUInt64 OldPos;
		switch(TextureChunk.TextureType)
		{
			case ZEGR_TT_CUBE:
				if (TextureChunk.InternalSize != 0)
				{
					OldPos = ResourceFile->Tell();
					CurrentTextureResource = ZETextureCubeResource::LoadResource(ResourceFile, UserOptions);
					ResourceFile->Seek(OldPos + TextureChunk.InternalSize, ZE_SF_BEGINING);
					if (CurrentTextureResource == NULL)
						zeWarning("Can not load material internal texture. (Material FileName : \"%s\", Texture Index : %d)", ResourceFile->GetPath().GetValue(), I);
				}
				else
				{
					CurrentTextureResource = ZETextureCubeResource::LoadSharedResource(TextureChunk.FileName, UserOptions);
					if (CurrentTextureResource == NULL)
						zeWarning("Can not load material texture. (Material FileName : \"%s\", Texture Index : %d, Texture FileName : \"%s\"", ResourceFile->GetPath().GetValue(), I, TextureChunk.FileName);
				}
				break;

			case ZEGR_TT_2D:
				if (TextureChunk.InternalSize != 0)
				{
					OldPos = ResourceFile->Tell();					
					CurrentTextureResource = ZETexture2DResource::LoadResource(ResourceFile, UserOptions);
					ResourceFile->Seek(OldPos + TextureChunk.InternalSize, ZE_SF_BEGINING);
					if (CurrentTextureResource == NULL)
						zeWarning("Can not load material internal texture. (Material FileName : \"%s\", Texture Index : %d)", ResourceFile->GetPath().GetValue(), I);
				}
				else
				{
					CurrentTextureResource = ZETexture2DResource::LoadSharedResource(TextureChunk.FileName, UserOptions);
					if (CurrentTextureResource == NULL)
						zeWarning("Can not load material texture. (Material FileName : \"%s\", Texture Index : %d, Texture FileName : \"%s\"", ResourceFile->GetPath().GetValue(), I, TextureChunk.FileName);
				}
				break;

			case ZEGR_TT_3D:
				if (TextureChunk.InternalSize != 0)
				{
					zeWarning("3D Textures is not supported in materials. (Material FileName : \"%s\", Texture Index : %d)", ResourceFile->GetPath().GetValue(), I);
					ResourceFile->Seek(TextureChunk.InternalSize, ZE_SF_CURRENT);
				}
				else
					zeWarning("3D Textures is not supported in materials. (Material FileName : \"%s\", Texture Index : %d, Texture FileName : \"%s\")", ResourceFile->GetPath().GetValue(), I);
				break;

			default:
				zeError("Wrong texture type. Corrupted ZEMaterial file. (Material FileName : \"%s\", Texture Index : %d)", ResourceFile->GetPath().GetValue(), I);
				return false;
				break;
		}
	}

	return false;
}

bool ZEMaterialResource::LoadFixedMaterial(ZEMaterialResource* MaterialResource, ZEFile* ResourceFile)
{
	return true;
}

ZEMaterialResource* ZEMaterialResource::LoadResource(ZEFile* ResourceFile, const ZETextureOptions* UserOptions)
{
	if(UserOptions == NULL)
		UserOptions = zeGraphics->GetTextureOptions();

	ZEMaterialFileHeaderChunk HeaderChunk;

	ResourceFile->Read(&HeaderChunk, sizeof(ZEMaterialFileHeaderChunk), 1);
	if (HeaderChunk.HEADER != ZE_MTLF_HEADER)
	{
		zeError("Unknown ZEMaterial file format. (FileName : \"%s\")", ResourceFile->GetPath().GetValue());
		return NULL;
	}

	if (HeaderChunk.Version != ZE_MTLF_VERSION)
	{
		zeError("ZEMaterial file version mismatched. (FileName : \"%s\")", ResourceFile->GetPath().GetValue());
		return NULL;
	}
	
	ZEMaterialResource* MaterialResource = new ZEMaterialResource();
	
	// Load Textures
	MaterialResource->TextureResources.SetCount((ZESize)HeaderChunk.TextureCount);
	if (!LoadTextures(MaterialResource, ResourceFile, UserOptions))
	{
		zeError("Corrupted material file. (FileName : \"%s\")", ResourceFile->GetPath().GetValue());
		delete MaterialResource;
		return NULL;
	}

	// Load Material
	MaterialResource->Material = ZEFixedMaterial::CreateInstance();
	MaterialResource->SetFileName(ResourceFile->GetPath().GetValue());
	MaterialResource->Cached = false;
	MaterialResource->Shared = false;

	return MaterialResource;
}

ZEMaterialResource* ZEMaterialResource::LoadResource(const ZEString& FileName, const ZETextureOptions* UserOptions)
{
	ZEMaterialResource* MaterialResource = NULL;
	
	bool Result;
	ZEFile File;
	Result = File.Open(FileName, ZE_FOM_READ, ZE_FCM_NONE);
	if (Result)
	{
		if(UserOptions == NULL)
			UserOptions = zeGraphics->GetTextureOptions();

		MaterialResource = LoadResource(&File, UserOptions);
		File.Close();

		return MaterialResource;
	}
	else
	{
		zeError("Material file not found. FilePath : \"%s\"", FileName.GetValue());
		return NULL;
	}
}

const ZEMaterialResource* ZEMaterialResource::LoadSharedResource(const ZEString& FileName, const ZETextureOptions* UserOptions)
{
	ZEMaterialResource* NewResource =(ZEMaterialResource*)zeResources->GetResource(FileName);
	if (NewResource == NULL)
	{
		if(UserOptions == NULL)
			UserOptions = zeGraphics->GetTextureOptions();

		NewResource = LoadResource(FileName, UserOptions);
		if (NewResource != NULL)
		{
			NewResource->Shared = true;
			NewResource->Cached = false;
			NewResource->ReferenceCount = 1;
			zeResources->AddResource(NewResource);
			return NewResource;
		}
		else
		{
			return NULL;
		}
	}
	else
		return NewResource;
}
