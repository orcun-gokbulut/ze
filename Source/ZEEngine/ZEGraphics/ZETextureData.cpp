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
#include "ZEMath\ZEVector.h"
#include <intrin.h>
#include "ZEMath\ZEMath.h"


// Returns if a TextureData 3D level of a surface is valid or not
static bool IsLevelValid(ZESize Surface, ZESize Level)
{
	return Surface % (ZESize)ZEMath::Power(2.0f, (float)Level) == 0;
}

static bool IsCompressed(ZETexturePixelFormat Format)
{
	return Format == ZE_TPF_DXT1 || Format == ZE_TPF_DXT3 || Format == ZE_TPF_DXT5;
}

// Returns owner surface of level
ZETextureSurface* ZETextureLevel::GetOwner()
{
	return Owner;
}

// Returns level index of the level
ZESize ZETextureLevel::GetLevel()
{
	return Level;
}

// Calculates and returns width of requested level
ZESize ZETextureLevel::GetWidth()
{
	ZESize OwnerSurface			= Owner->GetSurface();
	ZETextureType TextureType	= Owner->GetOwner()->GetTextureType();
	
	// If texture is 3D and the level is empty return 0
	if (TextureType == ZE_TT_3D && !IsLevelValid(OwnerSurface, Level))
	{
		return 0;
	}

	ZESize TextureWidth			= Owner->GetOwner()->GetTextureWidth();
	ZETexturePixelFormat Format = Owner->GetOwner()->GetPixelFormat();
	ZESize LevelWidth			= TextureWidth >> Level;
	
	// Width of last 2 level of compressed textures (2x2 and 1x1) must be 4x4
	if (IsCompressed(Format) && LevelWidth < 4)
	{
		return 4;
	}
	// If not compressed return width
	else
	{
		return LevelWidth;
	}
}

// Calculates and returns height of requested level
ZESize ZETextureLevel::GetHeight()
{
	ZESize OwnerSurface			= Owner->GetSurface();
	ZETextureType TextureType	= Owner->GetOwner()->GetTextureType();

	// If texture is 3D and the level is empty return 0
	if (TextureType == ZE_TT_3D && !IsLevelValid(OwnerSurface, Level))
	{
		return 0;
	}

	ZETexturePixelFormat Format = Owner->GetOwner()->GetPixelFormat();
	ZESize TextureHeight		= Owner->GetOwner()->GetTextureHeight();
	ZESize LevelHeight			= TextureHeight >> Level;
	
	// Height of last 2 level of compressed textures (2x2 and 1x1) must be 4x4
	if (IsCompressed(Format) && LevelHeight < 4)
	{
		return 4;
	}
	// If not compressed return height
	else
	{
		return LevelHeight;
	}
}

// Calculates and returns the pitch of requested level
ZESize ZETextureLevel::GetPitch()
{
	ZESize Pitch = 0;
	ZESize LevelWidth = this->GetWidth();
	ZETexturePixelFormat PixelFormat = Owner->GetOwner()->GetPixelFormat();
	
	switch (PixelFormat)
	{
		case ZE_TPF_I8_4:
			Pitch = (LevelWidth / ZE_I8_4_COMPRESSION_BLOCK_WIDTH) * ZE_I8_4_OUTPUT_BLOCK_SIZE;
			break;
		case ZE_TPF_DXT1:
			Pitch = (LevelWidth / ZE_DXT_1_COMPRESSION_BLOCK_WIDTH) * ZE_DXT_1_OUTPUT_BLOCK_SIZE;
			break;
		case ZE_TPF_DXT3:
			Pitch = (LevelWidth / ZE_DXT_3_COMPRESSION_BLOCK_WIDTH) * ZE_DXT_3_OUTPUT_BLOCK_SIZE;
			break;
		case ZE_TPF_DXT5:
			Pitch = (LevelWidth / ZE_DXT_5_COMPRESSION_BLOCK_WIDTH) * ZE_DXT_5_OUTPUT_BLOCK_SIZE;
			break;

	}

	return Pitch;
}

// Calculates and returns RowCount of level
ZESize ZETextureLevel::GetRowCount()
{
	ZESize RowCount = 0;
	ZESize LevelHeight = this->GetHeight();
	ZETexturePixelFormat PixelFormat = Owner->GetOwner()->GetPixelFormat();

	switch (PixelFormat)
	{
		case ZE_TPF_I8_4:
			RowCount = LevelHeight / ZE_I8_4_COMPRESSION_BLOCK_HEIGHT;
			break;
		case ZE_TPF_DXT1:
			RowCount = LevelHeight / ZE_DXT_1_COMPRESSION_BLOCK_HEIGHT;
			break;
		case ZE_TPF_DXT3:
			RowCount = LevelHeight / ZE_DXT_3_COMPRESSION_BLOCK_HEIGHT;
			break;
		case ZE_TPF_DXT5:
			RowCount = LevelHeight / ZE_DXT_5_COMPRESSION_BLOCK_HEIGHT;
			break;

	}

	return RowCount;

}

// Returns size of the level
ZESize ZETextureLevel::GetSize()
{
	return this->GetRowCount() * this->GetPitch();
}

void* ZETextureLevel::GetData()
{
	return Data;
}

void ZETextureLevel::CopyFrom(void* SourceData, ZESize SourcePitch)
{
	zeAssert(SourceData == NULL || SourcePitch == 0, "Cannot copy from source. Either Source is NULL or pitch is 0.");

	void* Destination		= this->Data;
	ZEUInt DestinationPitch = this->GetPitch();
	ZEUInt LevelRowCount	= this->GetRowCount();

	for(ZESize I = 0; I < LevelRowCount; ++I)
		memcpy((unsigned char*)Destination + I * DestinationPitch, (unsigned char*)SourceData + I * SourcePitch, DestinationPitch);
}

void ZETextureLevel::CopyTo(void* Dest, ZESize DestPitch)
{
	zeAssert(Dest == NULL || DestPitch == 0, "Cannot copy to destination. Either destination is NULL or pitch is 0.");

	void* SourceData		= this->Data;
	ZESize SourcePitch		= this->GetPitch();
	ZESize LevelRowCount	= this->GetRowCount();

	for(ZESize I = 0; I < LevelRowCount; ++I)
		memcpy((unsigned char*)Dest + I * DestPitch, (unsigned char*)SourceData + I * SourcePitch, SourcePitch);
}

// Const
ZETextureLevel::ZETextureLevel()
{
	Data	= NULL;
	Owner	= NULL;
	Level	= 0;
	
}

// Dest
ZETextureLevel::~ZETextureLevel()
{
	if (Data)	
	{
		free(Data);
		Data = NULL;
	}
}

// Returns index of the surface
ZESize ZETextureSurface::GetSurface()
{
	return Surface;
}

// Returns owner of the surface
ZETextureData* ZETextureSurface::GetOwner()
{
	return Owner;
}

// Returns the array of levels
ZEArray<ZETextureLevel>& ZETextureSurface::GetLevels()
{
	return Levels;
}

// Calculates and returns the raw data size of surface including all levels
ZESize ZETextureSurface::GetSize()
{
	ZESize LevelCount = Levels.GetCount();
	ZESize SurfaceSize = 0;

	for (ZESize I = 0; I < LevelCount; ++I)
	{
		SurfaceSize += Levels[I].GetSize();
	}

	return SurfaceSize;
}

// Const
ZETextureSurface::ZETextureSurface()
{
	Owner = NULL;
	Levels.Clear(false);
}

// Dest
ZETextureSurface::~ZETextureSurface()
{
	Levels.Clear(false);
}

// Creates the texture by allocating surface, levels and enough memory based on texture type
void ZETextureData::CreateTexture(ZETextureType TextureType, ZETexturePixelFormat PixelFormat, ZESize SurfaceCount, ZESize LevelCount, ZESize Width, ZESize Height)
{
	if (!this->IsEmpty())
	{
		zeWarning("Texture data is not empty. Clearing existing data.");
		this->DestroyTexture();
	}

	// Set texture data
	this->Width			= Width;
	this->Height		= Height;
	this->LevelCount	= LevelCount;
	this->SurfaceCount	= SurfaceCount;
	this->TextureType	= TextureType;
	this->PixelFormat	= PixelFormat;

	// Create surfaces
	Surfaces.Resize(SurfaceCount);

	switch (this->TextureType)
	{
		case ZE_TT_3D:
		{				
			// Surface 0 level count
			ZESize InitialLevelCount = LevelCount;

			// Create Levels of each Surface
			for (ZESize I = 0; I < SurfaceCount; ++I)
			{
				Surfaces[I].Surface = I;
				Surfaces[I].Owner	= this;
				Surfaces[I].Levels.Resize(LevelCount);

				// Allocate each MipMap
				for (ZESize J = 0; J < LevelCount; ++J)
				{
					Surfaces[I].Levels[J].Level = J;
					Surfaces[I].Levels[J].Owner = &Surfaces[I];
						
					// Create level if Surface % (2^Level) == 0
					if ((I % (ZESize)ZEMath::Power(2.0f, (float)J)) == 0)
					{
						Surfaces[I].Levels[J].Data = malloc(Surfaces[I].Levels[J].GetSize());
					}
					// Create empty level
					else
					{
						Surfaces[I].Levels[J].Data	= NULL;
					}
				}
			}
			break;
		}

		case ZE_TT_2D:
		case ZE_TT_CUBE:
		{
			// Create Levels of each Surface
			for (ZESize I = 0; I < SurfaceCount; ++I)
			{
				Surfaces[I].Surface = I;
				Surfaces[I].Owner	= this;
				Surfaces[I].Levels.Resize(LevelCount);

				// Allocate each MipMap
				for (ZESize J = 0; J < LevelCount; ++J)
				{
					Surfaces[I].Levels[J].Level = J;
					Surfaces[I].Levels[J].Owner = &Surfaces[I];
					Surfaces[I].Levels[J].Data	= malloc(Surfaces[I].Levels[J].GetSize());
				}
			}
			break;
		}

		default:
		{
			zeCriticalError("Unknown Texture Type..");
			break;
		}
			
	}
}

void ZETextureData::CreateTexture(ZETextureData& TextureData)
{
	if (!this->IsEmpty())
		this->DestroyTexture();

	// Allocate surfaces, levels and level data
	this->Width			= TextureData.Width;
	this->Height		= TextureData.Height;
	this->LevelCount	= TextureData.LevelCount;
	this->SurfaceCount	= TextureData.SurfaceCount;
	this->TextureType	= TextureData.TextureType;
	this->PixelFormat	= TextureData.PixelFormat;

	// Create surfaces
	Surfaces.Resize(SurfaceCount);

	// Create Levels of each Surface
	for (ZESize I = 0; I < SurfaceCount; ++I)
	{
		Surfaces[I].Surface = I;
		Surfaces[I].Owner	= this;
		Surfaces[I].Levels.Resize(LevelCount);

		// Allocate each level data
		for (ZESize J = 0; J < LevelCount; ++J)
		{
			void* Source = TextureData.Surfaces[I].Levels[J].Data;

			// If surface is valid. Some 3D levels may be empty
			if (Source != NULL)
			{
				Surfaces[I].Levels[J].Level = J;
				Surfaces[I].Levels[J].Owner = &Surfaces[I];
				Surfaces[I].Levels[J].Data	= malloc(Surfaces[I].Levels[J].GetSize());

				ZESize SourcePitch	= TextureData.Surfaces[I].Levels[J].GetPitch();
				Surfaces[I].Levels[J].CopyFrom(Source, SourcePitch);
			
			}
			// Set level to empty
			else
			{
				Surfaces[I].Levels[J].Level = J;
				Surfaces[I].Levels[J].Owner = NULL;
				Surfaces[I].Levels[J].Data	= NULL;
			}
		}
	}
}

// Destroys the content
void ZETextureData::DestroyTexture()
{
	// free Levels of each Surface
	for (ZESize I = 0; I < SurfaceCount; ++I)
	{
		for (ZESize J = 0; J < LevelCount; ++J)
		{
			// Clear level data
			Surfaces[I].Levels[J].Level = 0;
			Surfaces[I].Levels[J].Owner = NULL;
			
			// If there is level image data
			if (Surfaces[I].Levels[J].Data != NULL)
			{
				free(Surfaces[I].Levels[J].Data);
				Surfaces[I].Levels[J].Data = NULL;
			}
		}

		// Clear surface data
		Surfaces[I].Surface = 0;
		Surfaces[I].Owner	= NULL;
		Surfaces[I].Levels.Clear(false);

	}

	// Clear surfaces
	Surfaces.Clear(false);

	// Clear texture data
	this->Width			= 0;
	this->Height		= 0;
	this->LevelCount	= 0;
	this->SurfaceCount	= 0;
	this->TextureType	= ZE_TT_2D;
	this->PixelFormat	= ZE_TPF_NOTSET;

}

// Returns array of surfaces
ZEArray<ZETextureSurface>& ZETextureData::GetSurfaces()
{
	return Surfaces;
}

// True if texture is empty
bool ZETextureData::IsEmpty()
{
	return SurfaceCount == 0 ? true : false;
}

// Returns texture level0 width
ZESize ZETextureData::GetTextureWidth()
{
	return Width;
}

// Returns texture level0 height
ZESize ZETextureData::GetTextureHeight()
{
	return Height;
}

// Returns level count of the texture
ZESize ZETextureData::GetTextureLevelCount()
{
	return LevelCount;
}

// Returns Surface count of the texture
ZESize ZETextureData::GetTextureSurfaceCount()
{
	return SurfaceCount;
}

// Returns texture type
ZETextureType ZETextureData::GetTextureType()
{
	return TextureType;
}

// Returns pixel format
ZETexturePixelFormat ZETextureData::GetPixelFormat()
{
	return PixelFormat;
}

// Returns size of total raw texture data
ZESize ZETextureData::GetSize()
{
	ZESize TextureSize = 0;

	for (ZESize I = 0; I < SurfaceCount; I++)
	{
		for (ZESize J = 0; J < LevelCount; J++)
		{
			TextureSize += Surfaces[I].Levels[J].GetSize();
		}
	}

	return TextureSize;
}

// Returns the size of raw texture data + extra data that will be held on disk
ZESize ZETextureData::GetSizeOnDisk()
{
	ZESize TextureSize = 0;
	ZEUInt ZETextureFileHeaderSize = sizeof(ZETextureFileHeader);
	ZEUInt ZETextureFileSurfaceChunkSize = sizeof(ZETextureFileSurfaceChunk);
	ZEUInt ZETExtureFileMipmapChunkSize = sizeof(ZETExtureFileMipmapChunk);

	TextureSize += ZETextureFileHeaderSize;

	for (ZESize I = 0; I < SurfaceCount; I++)
	{
		TextureSize += ZETextureFileSurfaceChunkSize;

		for (ZESize J = 0; J < LevelCount; J++)
		{
			TextureSize += ZETExtureFileMipmapChunkSize;
			TextureSize += Surfaces[I].Levels[J].GetSize();
		}
	}

	return TextureSize;
}

// Const
ZETextureData::ZETextureData()
{
	Width			= 0;
	Height			= 0;
	LevelCount		= 0;
	SurfaceCount	= 0;

	TextureType		= ZE_TT_2D;
	PixelFormat		= ZE_TPF_NOTSET;

	Surfaces.Clear(false);
}

//Dest
ZETextureData::~ZETextureData()
{
	Surfaces.Clear(false);
}

// Converts a single surface, single level texture data to 6 surface texture data
void ZETextureData::ConvertToCubeTextureData(ZETextureData* Output, ZETextureData* TextureData)
{
	if (TextureData->GetTextureSurfaceCount() != 1)
		return;

	ZESize TargetWidth	= TextureData->GetTextureWidth() / 3;
	ZESize TargetHeight = TextureData->GetTextureHeight() / 2;

	zeAssert(TargetWidth != TargetHeight, "Cannot convert texture data. Dimensions do not match..");

	// Empty Output
	if (!Output->IsEmpty())
	{
		zeWarning("Output Texture data is not empty. Clearing it.");
		Output->DestroyTexture();
	}

	// Create output
	Output->CreateTexture(ZE_TT_CUBE, TextureData->GetPixelFormat(), 6, 1, TargetWidth, TargetHeight);

	struct
	{
		ZESize OffsetX;
		ZESize OffsetY;

	} Offsets[6] = {					// Copy Order

		{2 * TargetWidth, 0},			// +X Face
		{0 * TargetWidth, 0},			// -X Face
		{2 * TargetWidth, TargetWidth},	// +Y Face
		{1 * TargetWidth, TargetWidth},	// -Y Face
		{1 * TargetWidth, 0},			// +Z Face
		{0 * TargetWidth, TargetWidth},	// -Z Face
	};

	void* SourceData	= TextureData->GetSurfaces().GetItem(0).GetLevels().GetItem(0).GetData();
	ZESize SourcePitch	= TextureData->GetSurfaces().GetItem(0).GetLevels().GetItem(0).GetPitch();
	
	ZESize SurfaceCount = Output->GetTextureSurfaceCount();

	for (ZESize I = 0; I < SurfaceCount; ++I)
	{
		void* TargetData	= Output->GetSurfaces().GetItem(I).GetLevels().GetItem(0).GetData();
		ZESize TargetPitch	= Output->GetSurfaces().GetItem(I).GetLevels().GetItem(0).GetPitch();
		
		for (ZEUInt J = 0; J < TargetHeight; ++J)
		{
			memcpy((unsigned char*)TargetData + (J * TargetPitch), (unsigned char*)SourceData + SourcePitch * (Offsets[I].OffsetY + J) + Offsets[I].OffsetX * 4, TargetPitch);
		}

	}
}

// Converts a single surface, single level texture data to n surface texture data
void ZETextureData::ConvertToVolumeTextureData(ZETextureData* Output, ZETextureData* TextureData, ZESize TileCountX, ZESize TileCountY)
{
	ZESize TargetWidth	= TextureData->GetTextureWidth() / TileCountX;
	ZESize TargetHeight = TextureData->GetTextureHeight() / TileCountY;

	void* SourceData	= TextureData->GetSurfaces().GetItem(0).GetLevels().GetItem(0).GetData();
	ZESize SourcePitch	= TextureData->GetSurfaces().GetItem(0).GetLevels().GetItem(0).GetPitch();

	// Empty Output
	if (!Output->IsEmpty())
	{
		zeWarning("Output Texture data is not empty. Clearing it.");
		Output->DestroyTexture();
	}

	Output->CreateTexture(ZE_TT_3D, TextureData->GetPixelFormat(), TileCountX * TileCountY, 1, TargetWidth, TargetHeight);

	for (ZESize I = 0; I < TileCountY; ++I)
	{
		for (ZESize J = 0; J < TileCountX; ++J)
		{
			void* TargetData	= Output->GetSurfaces().GetItem(TileCountX * I + J).GetLevels().GetItem(0).GetData();
			ZESize TargetPitch	= Output->GetSurfaces().GetItem(TileCountX * I + J).GetLevels().GetItem(0).GetPitch();
		
			for (ZESize K = 0; K < TargetHeight; ++K)
			{
				memcpy((unsigned char*)TargetData + (K * TargetPitch), (unsigned char*)SourceData + SourcePitch * (TargetHeight * I + K) + TargetPitch * J, TargetPitch);
			}
		}
	}
}
