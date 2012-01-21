//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureData.cpp
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
#include "ZETextureData.h"
#include "ZEFile\ZEFile.h"
#include "ZETextureFileFormat.h"

#include <memory.h>


bool ZETextureData::CheckBoundaries(ZEUInt Depth)
{
	if(Depth < Texture.Depth)
		return true;

	return false;
}

bool ZETextureData::CheckBoundaries(ZEUInt Depth, ZEUInt MipLevel)
{
	if(Depth < Texture.Depth && MipLevel < Texture.MipmapCount)
		return true;

	return false;
}

void ZETextureData::CreateTexture(ZETexturePixelFormat PixelFormat, ZEUInt Depth, ZEUInt MipmapCount, ZEUInt Width, ZEUInt Height)
{
	// Allocate surfaces of the texture
	ZEUInt AllocationSize = sizeof(TextureData::SurfaceData) * Depth;

	Texture.Surfaces = (TextureData::SurfaceData*)malloc(AllocationSize);
	if(Texture.Surfaces == NULL)
		zeError("Cannot allocate memory for the surfaces!");

	memset(Texture.Surfaces, 0, AllocationSize);
	
	// Allocate the mipmaps of each surface
	AllocationSize = sizeof(TextureData::SurfaceData::MipmapData) * MipmapCount;

	for(ZESize I = 0; I < Depth; I++)
	{
		Texture.Surfaces[I].Mipmaps = (TextureData::SurfaceData::MipmapData*)malloc(AllocationSize);
		if(Texture.Surfaces->Mipmaps == NULL)
			zeError("Cannot allocate memory for the mipmaps of the surface level %u!", I);

		memset(Texture.Surfaces[I].Mipmaps, 0, AllocationSize);
	}

	Texture.Depth		= Depth;
	Texture.Width		= Width;
	Texture.Height		= Height;
	Texture.MipmapCount = MipmapCount;
	Texture.PixelFormat = PixelFormat;
}

void ZETextureData::DestroyTexture()
{
	// Deallocate
	for(ZESize I = 0; I < Texture.Depth; I++)
	{
		// Deallocate mipmap data
		for(ZESize J = 0; J < Texture.MipmapCount; J++)
		{
			if(Texture.Surfaces[I].Mipmaps[J].Data != NULL)
			{
				free(Texture.Surfaces[I].Mipmaps[J].Data);
				Texture.Surfaces[I].Mipmaps[J].Data = NULL;
			}
		}

		// Deallocate mipmaps
		if(Texture.Surfaces[I].Mipmaps != NULL)
		{
			free(Texture.Surfaces[I].Mipmaps);
			Texture.Surfaces[I].Mipmaps = NULL;
		}
	}

	// Deallocate surfaces
	if(Texture.Surfaces != NULL)
	{
		free(Texture.Surfaces);	
		Texture.Surfaces = NULL;
	}

	ZEUInt TextureDataSize = sizeof(TextureData);
	memset(&Texture, 0, TextureDataSize);
}

// Creates the mipmap data
void ZETextureData::AllocateMipmap(ZEUInt Depth, ZEUInt MipLevel, ZEUInt RowSize, ZEUInt RowCount)
{
	zeAssert(!CheckBoundaries(Depth, MipLevel), "Wrong Index");

	TextureData::SurfaceData::MipmapData* TargetMipmap = &Texture.Surfaces[Depth].Mipmaps[MipLevel];

	if(TargetMipmap->Data != NULL)
		FreeMipmap(Depth, MipLevel);
	
	ZEUInt AllocationSize = RowSize * RowCount;
	TargetMipmap->Data = (void*)malloc(AllocationSize);
	
	if(TargetMipmap->Data == NULL)
		zeError("Cannot allocate memory for the mipmap level %d of surface %d!", MipLevel, Depth);

	TargetMipmap->RowSize = RowSize;
	TargetMipmap->RowCount = RowCount;
	
}

// Destroys mipmap data
void ZETextureData::FreeMipmap(ZEUInt Depth, ZEUInt MipLevel)
{
	zeAssert(!CheckBoundaries(Depth, MipLevel), "Wrong Index");

	TextureData::SurfaceData::MipmapData* TargetMipmap = &Texture.Surfaces[Depth].Mipmaps[MipLevel];

	if(TargetMipmap->Data != NULL)
	{
		free(TargetMipmap->Data);
		TargetMipmap->Data = NULL;
	}

	TargetMipmap->RowSize = 0;
	TargetMipmap->RowCount = 0;

}

// Adds a surface to the end of the array
void ZETextureData::AddSurface(ZEUInt Count)
{
	// Allocate surfaces of the texture
	ZEUInt OldAllocationSize	= sizeof(TextureData::SurfaceData) * Texture.Depth;
	ZEUInt AllocationSize		= sizeof(TextureData::SurfaceData) * (Texture.Depth + Count);
	TextureData::SurfaceData* NewSurfaces = (TextureData::SurfaceData*)malloc(AllocationSize);
	memset(NewSurfaces, 0, AllocationSize);
	
	if (OldAllocationSize != 0)
	{
		// Copy old data to new one
		memcpy(NewSurfaces, Texture.Surfaces, OldAllocationSize);
		// Free previous one
		free(Texture.Surfaces);
	}	
	// Set new one
	Texture.Surfaces = NewSurfaces;
	NewSurfaces = NULL;

	// Allocate mipmaps array for the new surface
	ZEUInt MipmapsAllocationSize = sizeof(TextureData::SurfaceData::MipmapData) * Texture.MipmapCount;

	if (MipmapsAllocationSize != 0)
	{
		for(ZEUInt I = 0; I < Count; I++)
		{
			TextureData::SurfaceData::MipmapData* NewMipmaps = (TextureData::SurfaceData::MipmapData*)malloc(MipmapsAllocationSize);
			memset(NewMipmaps, 0, MipmapsAllocationSize);

			// Set mipmaps
			NewSurfaces[Texture.Depth].Mipmaps = NewMipmaps;
			NewMipmaps = NULL;
		}
	}
	Texture.Depth += Count;

}

void ZETextureData::RemoveSurface(ZEUInt Count)
{
	zeAssert(true, "NOT IMPLEMENTED!!!");
}

// Adds a mipmap to the end of the mipmap array of surface Depth
// Adds mipmap to all surfaces.
void ZETextureData::AddMipmap(ZEUInt Count)
{
	ZEUInt OldAllocationSize	= sizeof(TextureData::SurfaceData::MipmapData) * Texture.MipmapCount;
	ZEUInt AllocationSize		= sizeof(TextureData::SurfaceData::MipmapData) * (Texture.MipmapCount + Count);
	TextureData::SurfaceData::MipmapData* NewMipmaps = NULL;

	// For every surface
	for(ZEUInt I = 0; I < Texture.Depth; I++)
	{
		// Allocate new storage
		NewMipmaps = (TextureData::SurfaceData::MipmapData*)malloc(AllocationSize);
		memset(NewMipmaps, 0, AllocationSize);

		if (OldAllocationSize != 0)
		{
			// Copy old data to new one
			memcpy(NewMipmaps, Texture.Surfaces[I].Mipmaps, OldAllocationSize);
			// Free previous one
			free(Texture.Surfaces[I].Mipmaps);
		}
		// Set new one
		Texture.Surfaces[I].Mipmaps = NewMipmaps;
		NewMipmaps=  NULL;

	}

	Texture.MipmapCount += Count;
}

void ZETextureData::RemoveMipmap(ZEUInt Count)
{
	zeAssert(true, "NOT IMPLEMENTED!!!");
}

bool ZETextureData::IsEmpty()
{
	for(ZEUInt I = 0; I < Texture.Depth; I++)
	{
		for(ZEUInt J = 0; J < Texture.MipmapCount; J++)
		{
			if(Texture.Surfaces[I].Mipmaps[J].Data != NULL)
				return false;
		}
	}
	
	return true;
}

void ZETextureData::SetDepth(ZEUInt Value)
{
	Texture.Depth = Value;
}

ZEUInt ZETextureData::GetDepth()
{
	return Texture.Depth;
}

void ZETextureData::SetMipmapCount(ZEUInt Value)
{
	Texture.MipmapCount = Value;
}

ZEUInt ZETextureData::GetMipmapCount()
{
	return Texture.MipmapCount;
}

void ZETextureData::SetWidth(ZEUInt Value)
{
	Texture.Width = Value;
}

ZEUInt ZETextureData::GetWidth()
{
	return Texture.Width;
}

void ZETextureData::SetHeight(ZEUInt Value)
{
	Texture.Height = Value;
}

ZEUInt ZETextureData::GetHeight()
{
	return Texture.Height;
}

void ZETextureData::SetPixelFormat(ZETexturePixelFormat PixelFormat)
{
	Texture.PixelFormat = PixelFormat;
}

ZETexturePixelFormat ZETextureData::GetPixelFormat()
{
	return Texture.PixelFormat;
}

ZEUInt ZETextureData::GetMipmapRowCount(ZEUInt Depth, ZEUInt MipLevel)
{
	zeAssert(!CheckBoundaries(Depth, MipLevel), "Wrong Index");

	return Texture.Surfaces[Depth].Mipmaps[MipLevel].RowCount;

}

ZEUInt ZETextureData::GetMipmapRowSize(ZEUInt Depth, ZEUInt MipLevel)
{
	zeAssert(!CheckBoundaries(Depth, MipLevel), "Wrong Index");

	return Texture.Surfaces[Depth].Mipmaps[MipLevel].RowSize;

}

void* ZETextureData::GetMipmapData(ZEUInt Depth, ZEUInt MipLevel)
{
	zeAssert(!CheckBoundaries(Depth, MipLevel), "Wrong Index");
	
	return Texture.Surfaces[Depth].Mipmaps[MipLevel].Data;

}

ZEUInt ZETextureData::GetMipmapDataSize(ZEUInt Depth, ZEUInt MipLevel)
{
	zeAssert(!CheckBoundaries(Depth, MipLevel), "Wrong Index");

	return Texture.Surfaces[Depth].Mipmaps[MipLevel].RowCount * Texture.Surfaces[Depth].Mipmaps[MipLevel].RowSize;

}

ZEUInt ZETextureData::GetSurfaceSize(ZEUInt Depth)
{
	zeAssert(!CheckBoundaries(Depth), "Wrong Index");
	ZEUInt SurfaceSize = 0;

	for(ZEUInt J = 0; J < Texture.MipmapCount; J++)
	{
		SurfaceSize += Texture.Surfaces[Depth].Mipmaps[J].RowCount * Texture.Surfaces[Depth].Mipmaps[J].RowSize;
	}

	return SurfaceSize;
}

ZEUInt ZETextureData::GetTextureSize()
{
	ZEUInt TotalTextureSize = 0;
	
	for(ZEUInt I = 0; I < Texture.Depth; I++)
		for(ZEUInt J = 0; J < Texture.MipmapCount; J++)
			TotalTextureSize += Texture.Surfaces[I].Mipmaps[J].RowCount * Texture.Surfaces[I].Mipmaps[J].RowSize;
	
	return TotalTextureSize;

}

// Returns the total size needed to open a ZEpartialFile in cache or in pack
ZEUInt ZETextureData::GetSizeOnDisk()
{
	ZEUInt ZETextureFileHeaderSize = sizeof(ZETextureFileHeader);
	ZEUInt ZETextureFileSurfaceChunkSize = sizeof(ZETextureFileSurfaceChunk);
	ZEUInt ZETExtureFileMipmapChunkSize = sizeof(ZETExtureFileMipmapChunk);

	ZEUInt TotalSurfaceChunkSize = 0;
	ZEUInt TotalMipmapChunkSize = 0;
	ZEUInt TotalMipmapDataSize = 0;

	for(ZEUInt I = 0; I < Texture.Depth; I++)
	{
		TotalSurfaceChunkSize += ZETextureFileSurfaceChunkSize;
		for(ZEUInt J = 0; J < Texture.MipmapCount; J++)
		{
			TotalMipmapChunkSize += ZETExtureFileMipmapChunkSize;
			TotalMipmapDataSize += Texture.Surfaces[I].Mipmaps[J].RowCount * Texture.Surfaces[I].Mipmaps[J].RowSize;
		}
	}

	return  ZETextureFileHeaderSize + TotalSurfaceChunkSize + TotalMipmapChunkSize + TotalMipmapDataSize;

}

void ZETextureData::CopyMipmapDataFrom(ZEUInt Depth, ZEUInt MipLevel, void* SourceData, ZEUInt SourcePitch)
{
	zeAssert(!CheckBoundaries(Depth, MipLevel), "Wrong Index");

	void* Destination = Texture.Surfaces[Depth].Mipmaps[MipLevel].Data;
	ZEUInt DestinationPitch = Texture.Surfaces[Depth].Mipmaps[MipLevel].RowSize;
	ZEUInt RowCount = Texture.Surfaces[Depth].Mipmaps[MipLevel].RowCount;
	ZEUInt RowSize = Texture.Surfaces[Depth].Mipmaps[MipLevel].RowSize;
	
	for(ZESize I = 0; I < RowCount; I++)
		memcpy((unsigned char*)Destination + I * DestinationPitch, (unsigned char*)SourceData + I * SourcePitch, RowSize);

}

void ZETextureData::CopyMipmapDataTo(ZEUInt Depth, ZEUInt MipLevel, void* Dest, ZEUInt DestPitch)
{
	zeAssert(!CheckBoundaries(Depth, MipLevel), "Wrong Index");

	void* Source = Texture.Surfaces[Depth].Mipmaps[MipLevel].Data;
	ZEUInt SourcePitch = Texture.Surfaces[Depth].Mipmaps[MipLevel].RowSize;
	ZEUInt RowCount = Texture.Surfaces[Depth].Mipmaps[MipLevel].RowCount;
	ZEUInt RowSize = Texture.Surfaces[Depth].Mipmaps[MipLevel].RowSize;
	
	for(ZESize I = 0; I < RowCount; I++)
		memcpy((unsigned char*)Dest + I * DestPitch, (unsigned char*)Source + I * SourcePitch, RowSize);

}

ZETextureData::ZETextureData()
{
	ZEUInt TexturedataSize = sizeof(TextureData);
	memset(&Texture, 0, TexturedataSize);

}

ZETextureData::~ZETextureData()
{
	DestroyTexture();

}
