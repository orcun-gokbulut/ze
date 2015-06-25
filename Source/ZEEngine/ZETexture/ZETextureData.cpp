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
#include "ZEMath\ZEMath.h"
#include "ZETextureData.h"
#include "ZEFile\ZEFile.h"
#include "ZEMath\ZEVector.h"
#include "ZETextureFileFormat.h"

#include <memory.h>
#include <intrin.h>

// Compressed Dxt1 Block Size
#define		ZE_DXT_1_COMPRESSION_BLOCK_WIDTH	4	// Pixels
#define		ZE_DXT_1_COMPRESSION_BLOCK_HEIGHT	4	// Pixels
#define		ZE_DXT_1_INPUT_BLOCK_SIZE			64	// Bytes
#define		ZE_DXT_1_OUTPUT_BLOCK_SIZE			8	// Bytes

// Compressed Dxt3 Block Size
#define		ZE_DXT_3_COMPRESSION_BLOCK_WIDTH	4	// Pixels
#define		ZE_DXT_3_COMPRESSION_BLOCK_HEIGHT	4	// Pixels
#define		ZE_DXT_3_INPUT_BLOCK_SIZE			64	// Bytes
#define		ZE_DXT_3_OUTPUT_BLOCK_SIZE			16	// Bytes

// Compressed Dxt5 Block Size
#define		ZE_DXT_5_COMPRESSION_BLOCK_WIDTH	4	// Pixels
#define		ZE_DXT_5_COMPRESSION_BLOCK_HEIGHT	4	// Pixels
#define		ZE_DXT_5_INPUT_BLOCK_SIZE			64	// Bytes
#define		ZE_DXT_5_OUTPUT_BLOCK_SIZE			16	// Bytes

// Uncompressed ARGB size
#define		ZE_I8_4_COMPRESSION_BLOCK_WIDTH		1	// Pixels
#define		ZE_I8_4_COMPRESSION_BLOCK_HEIGHT	1	// Pixels
#define		ZE_I8_4_INPUT_BLOCK_SIZE			4	// Bytes
#define		ZE_I8_4_OUTPUT_BLOCK_SIZE			4	// Bytes

// Uncompressed ZEGR_TF_I8 size
#define		ZE_I8_COMPRESSION_BLOCK_WIDTH		1	// Pixels
#define		ZE_I8_COMPRESSION_BLOCK_HEIGHT		1	// Pixels
#define		ZE_I8_INPUT_BLOCK_SIZE				1	// Bytes
#define		ZE_I8_OUTPUT_BLOCK_SIZE				1	// Bytes

// Uncompressed ZEGR_TF_I16 size
#define		ZE_I16_COMPRESSION_BLOCK_WIDTH		1	// Pixels
#define		ZE_I16_COMPRESSION_BLOCK_HEIGHT		1	// Pixels
#define		ZE_I16_INPUT_BLOCK_SIZE				2	// Bytes
#define		ZE_I16_OUTPUT_BLOCK_SIZE			2	// Bytes

// Uncompressed ZEGR_TF_I16_2 size
#define		ZE_I16_2_COMPRESSION_BLOCK_WIDTH	1	// Pixels
#define		ZE_I16_2_COMPRESSION_BLOCK_HEIGHT	1	// Pixels
#define		ZE_I16_2_INPUT_BLOCK_SIZE			2	// Bytes
#define		ZE_I16_2_OUTPUT_BLOCK_SIZE			2	// Bytes

// Uncompressed ZEGR_TF_I32 size
#define		ZE_I32_COMPRESSION_BLOCK_WIDTH		1	// Pixels
#define		ZE_I32_COMPRESSION_BLOCK_HEIGHT		1	// Pixels
#define		ZE_I32_INPUT_BLOCK_SIZE				2	// Bytes
#define		ZE_I32_OUTPUT_BLOCK_SIZE			2	// Bytes

// Uncompressed ZEGR_TF_F32 size
#define		ZE_F32_COMPRESSION_BLOCK_WIDTH		1	// Pixels
#define		ZE_F32_COMPRESSION_BLOCK_HEIGHT		1	// Pixels
#define		ZE_F32_INPUT_BLOCK_SIZE				4	// Bytes
#define		ZE_F32_OUTPUT_BLOCK_SIZE			4	// Bytes

// Uncompressed ZEGR_TF_F32_2 size
#define		ZE_F32_2_COMPRESSION_BLOCK_WIDTH	1	// Pixels
#define		ZE_F32_2_COMPRESSION_BLOCK_HEIGHT	1	// Pixels
#define		ZE_F32_2_INPUT_BLOCK_SIZE			8	// Bytes
#define		ZE_F32_2_OUTPUT_BLOCK_SIZE			8	// Bytes

// Uncompressed ZEGR_TF_F32_4 size
#define		ZE_F32_4_COMPRESSION_BLOCK_WIDTH	1	// Pixels
#define		ZE_F32_4_COMPRESSION_BLOCK_HEIGHT	1	// Pixels
#define		ZE_F32_4_INPUT_BLOCK_SIZE			16	// Bytes
#define		ZE_F32_4_OUTPUT_BLOCK_SIZE			16	// Bytes

// Returns if a TextureData 3D level of a surface is valid or not
static bool IsLevelValid(ZEUInt Surface, ZEUInt Level)
{
	return Surface % (ZEUInt)ZEMath::Power(2.0f, (float)Level) == 0;
}

static bool IsCompressed(ZEGRFormat Format)
{
	return Format == ZEGR_TF_DXT1 || Format == ZEGR_TF_DXT3 || Format == ZEGR_TF_DXT5;
}

// Returns owner surface of level
ZETextureSurface* ZETextureLevel::GetOwner()
{
	return Owner;
}

// Returns level index of the level
ZEUInt ZETextureLevel::GetLevel()
{
	return Level;
}

// Calculates and returns width of requested level
ZEUInt ZETextureLevel::GetWidth()
{
	ZEUInt OwnerSurface			= Owner->GetSurface();
	ZEGRTextureType TextureType	= Owner->GetOwner()->GetType();
	
	// If texture is 3D and the level is empty return 0
	if (TextureType == ZEGR_TT_3D && !IsLevelValid(OwnerSurface, Level))
	{
		return 0;
	}

	ZEUInt TextureWidth			= Owner->GetOwner()->GetWidth();
	ZEGRFormat Format = Owner->GetOwner()->GetPixelFormat();
	ZEUInt LevelWidth			= TextureWidth >> Level;
	
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
ZEUInt ZETextureLevel::GetHeight()
{
	ZEUInt OwnerSurface			= Owner->GetSurface();
	ZEGRTextureType TextureType	= Owner->GetOwner()->GetType();

	// If texture is 3D and the level is empty return 0
	if (TextureType == ZEGR_TT_3D && !IsLevelValid(OwnerSurface, Level))
	{
		return 0;
	}

	ZEGRFormat Format = Owner->GetOwner()->GetPixelFormat();
	ZEUInt TextureHeight		= Owner->GetOwner()->GetHeight();
	ZEUInt LevelHeight			= TextureHeight >> Level;
	
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
	ZEUInt LevelWidth = this->GetWidth();
	ZEGRFormat PixelFormat = Owner->GetOwner()->GetPixelFormat();
	
	switch (PixelFormat)
	{		
		case ZEGR_TF_I8:
			Pitch = ((ZESize)LevelWidth / ZE_I8_COMPRESSION_BLOCK_WIDTH) * ZE_I8_OUTPUT_BLOCK_SIZE;
			break;
		case ZEGR_TF_I16:
			Pitch = ((ZESize)LevelWidth / ZE_I16_COMPRESSION_BLOCK_WIDTH) * ZE_I16_OUTPUT_BLOCK_SIZE;
			break;
		case ZEGR_TF_I16_2:
			Pitch = ((ZESize)LevelWidth / ZE_I16_2_COMPRESSION_BLOCK_WIDTH) * ZE_I16_2_OUTPUT_BLOCK_SIZE;
			break;
		case ZEGR_TF_I32:
			Pitch = ((ZESize)LevelWidth / ZE_I32_COMPRESSION_BLOCK_WIDTH) * ZE_I32_OUTPUT_BLOCK_SIZE;
			break;
		case ZEGR_TF_F32:
			Pitch = ((ZESize)LevelWidth / ZE_F32_COMPRESSION_BLOCK_WIDTH) * ZE_F32_OUTPUT_BLOCK_SIZE;
			break;
		case ZEGR_TF_F32_2:
			Pitch = ((ZESize)LevelWidth / ZE_F32_2_COMPRESSION_BLOCK_WIDTH) * ZE_F32_2_OUTPUT_BLOCK_SIZE;
			break;
		case ZEGR_TF_F32_4:
			Pitch = ((ZESize)LevelWidth / ZE_F32_4_COMPRESSION_BLOCK_WIDTH) * ZE_F32_4_OUTPUT_BLOCK_SIZE;
			break;
		case ZEGR_TF_I8_4:
			Pitch = ((ZESize)LevelWidth / ZE_I8_4_COMPRESSION_BLOCK_WIDTH) * ZE_I8_4_OUTPUT_BLOCK_SIZE;
			break;
		case ZEGR_TF_DXT1:
			Pitch = ((ZESize)LevelWidth / ZE_DXT_1_COMPRESSION_BLOCK_WIDTH) * ZE_DXT_1_OUTPUT_BLOCK_SIZE;
			break;
		case ZEGR_TF_DXT3:
			Pitch = ((ZESize)LevelWidth / ZE_DXT_3_COMPRESSION_BLOCK_WIDTH) * ZE_DXT_3_OUTPUT_BLOCK_SIZE;
			break;
		case ZEGR_TF_DXT5:
			Pitch = ((ZESize)LevelWidth / ZE_DXT_5_COMPRESSION_BLOCK_WIDTH) * ZE_DXT_5_OUTPUT_BLOCK_SIZE;
			break;
		case ZEGR_TF_F16:
		case ZEGR_TF_F16_2:
		case ZEGR_TF_F16_4:
			zeCriticalError("16 Bit floating point data is not supported.");
			break;
		case ZEGR_TF_NONE:
		default:
			zeCriticalError("Unknown Pixel Format");
			break;

	}

	return Pitch;
}

// Calculates and returns RowCount of level
ZEUInt ZETextureLevel::GetRowCount()
{
	ZEUInt RowCount = 0;
	ZEUInt LevelHeight = this->GetHeight();
	ZEGRFormat PixelFormat = Owner->GetOwner()->GetPixelFormat();

	switch (PixelFormat)
	{
		case ZEGR_TF_I8:
			RowCount = LevelHeight / ZE_I8_COMPRESSION_BLOCK_HEIGHT;
			break;
		case ZEGR_TF_I16:
			RowCount = LevelHeight / ZE_I16_COMPRESSION_BLOCK_HEIGHT;
			break;
		case ZEGR_TF_I16_2:
			RowCount = LevelHeight / ZE_I16_2_COMPRESSION_BLOCK_HEIGHT;
			break;
		case ZEGR_TF_I32:
			RowCount = LevelHeight / ZE_I32_COMPRESSION_BLOCK_HEIGHT;
			break;
		case ZEGR_TF_F32:
			RowCount = LevelHeight / ZE_F32_COMPRESSION_BLOCK_HEIGHT;
			break;
		case ZEGR_TF_F32_2:
			RowCount = LevelHeight / ZE_F32_2_COMPRESSION_BLOCK_HEIGHT;
			break;
		case ZEGR_TF_F32_4:
			RowCount = LevelHeight / ZE_F32_4_COMPRESSION_BLOCK_HEIGHT;
			break;
		case ZEGR_TF_I8_4:
			RowCount = LevelHeight / ZE_I8_4_COMPRESSION_BLOCK_HEIGHT;
			break;
		case ZEGR_TF_DXT1:
			RowCount = LevelHeight / ZE_DXT_1_COMPRESSION_BLOCK_HEIGHT;
			break;
		case ZEGR_TF_DXT3:
			RowCount = LevelHeight / ZE_DXT_3_COMPRESSION_BLOCK_HEIGHT;
			break;
		case ZEGR_TF_DXT5:
			RowCount = LevelHeight / ZE_DXT_5_COMPRESSION_BLOCK_HEIGHT;
			break;
		case ZEGR_TF_F16:
		case ZEGR_TF_F16_2:
		case ZEGR_TF_F16_4:
			zeCriticalError("16 Bit floating point data is not supported.");
			break;
		case ZEGR_TF_NONE:
		default:
			zeCriticalError("Unknown Pixel Format");
			break;
	}

	return RowCount;

}

// Returns size of the level
ZESize ZETextureLevel::GetSize()
{
	return (ZESize)this->GetRowCount() * this->GetPitch();
}

void* ZETextureLevel::GetData()
{
	return Data;
}

void ZETextureLevel::CopyFrom(void* SourceData, ZESize SourcePitch)
{
	zeDebugCheck(SourceData == NULL || SourcePitch == 0, "Cannot copy from source. Either Source is NULL or pitch is 0.");

	void* Destination		= this->Data;
	ZESize DestinationPitch = this->GetPitch();
	ZESize LevelRowCount	= (ZESize)this->GetRowCount();

	for(ZESize I = 0; I < LevelRowCount; ++I)
		memcpy((ZEUInt8*)Destination + I * DestinationPitch, (ZEUInt8*)SourceData + I * SourcePitch, DestinationPitch);
}


void ZETextureLevel::CopyTo(void* Dest, ZESize DestPitch)
{
	zeDebugCheck(Dest == NULL || DestPitch == 0, "Cannot copy to destination. Either destination is NULL or pitch is 0.");

	void* SourceData		= this->Data;
	ZESize SourcePitch		= this->GetPitch();
	ZESize LevelRowCount	= (ZESize)this->GetRowCount();

	for(ZESize I = 0; I < LevelRowCount; ++I)
		memcpy((ZEUInt8*)Dest + I * DestPitch, (ZEUInt8*)SourceData + I * SourcePitch, SourcePitch);
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
		delete [] Data;
		Data = NULL;
	}
}

// Returns index of the surface
ZEUInt ZETextureSurface::GetSurface()
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

ZEUInt ZETextureSurface::GetWidth()
{
	return Owner->GetWidth();
}

ZEUInt ZETextureSurface::GetHeight()
{
	return Owner->GetHeight();
}

// Calculates and returns the raw data size of surface including all levels
ZESize ZETextureSurface::GetSize()
{
	ZESize LevelCount = (ZESize)Levels.GetCount();
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
void ZETextureData::Create(ZEGRTextureType TextureType, ZEGRFormat PixelFormat, ZEUInt SurfaceCount, ZEUInt LevelCount, ZEUInt Width, ZEUInt Height)
{
	if (!this->IsEmpty())
	{
		zeWarning("Texture data is not empty. Clearing existing data.");
		this->Destroy();
	}

	// Set texture data
	this->Info.Width		= Width;
	this->Info.Height		= Height;
	this->Info.LevelCount	= LevelCount;
	this->Info.SurfaceCount	= SurfaceCount;
	this->Info.Type			= TextureType;
	this->Info.PixelFormat	= PixelFormat;

	ZESize TempLevelCount	= (ZESize)LevelCount;
	ZESize TempSurfaceCount = (ZESize)SurfaceCount;

	// Create surfaces
	Surfaces.Resize(TempSurfaceCount);

	switch (this->Info.Type)
	{
		case ZEGR_TT_3D:
		{				
			// Surface 0 level count
			ZEUInt InitialLevelCount = LevelCount;

			// Create Levels of each Surface
			for (ZESize I = 0; I < TempSurfaceCount; ++I)
			{
				Surfaces[I].Surface = (ZEUInt)I;
				Surfaces[I].Owner	= this;
				Surfaces[I].Levels.Resize(TempLevelCount);

				// Allocate each MipMap
				for (ZESize J = 0; J < TempLevelCount; ++J)
				{
					Surfaces[I].Levels[J].Level = (ZEUInt)J;
					Surfaces[I].Levels[J].Owner = &Surfaces[I];
						
					// Create level if Surface % (2^Level) == 0
					if ((I % (ZESize)ZEMath::Power(2.0f, (float)J)) == 0)
					{
						Surfaces[I].Levels[J].Data = new ZEUInt8[Surfaces[I].Levels[J].GetSize()];
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

		case ZEGR_TT_2D:
		case ZEGR_TT_CUBE:
		{
			// Create Levels of each Surface
			for (ZESize I = 0; I < TempSurfaceCount; ++I)
			{
				Surfaces[I].Surface = (ZEUInt)I;
				Surfaces[I].Owner	= this;
				Surfaces[I].Levels.Resize(TempLevelCount);

				// Allocate each MipMap
				for (ZESize J = 0; J < TempLevelCount; ++J)
				{
					Surfaces[I].Levels[J].Level = (ZEUInt)J;
					Surfaces[I].Levels[J].Owner = &Surfaces[I];
					Surfaces[I].Levels[J].Data	= new ZEUInt8[Surfaces[I].Levels[J].GetSize()];
				}
			}
			break;
		}

		default:
		{
			zeError("Unknown Texture Type..");
			break;
		}
			
	}
}

void ZETextureData::Create(ZETextureData& TextureData)
{
	if (!this->IsEmpty())
	{
		this->Destroy();
	}

	// Allocate surfaces, levels and level data
	this->Info.Width		= TextureData.Info.Width;
	this->Info.Height		= TextureData.Info.Height;
	this->Info.LevelCount	= TextureData.Info.LevelCount;
	this->Info.SurfaceCount	= TextureData.Info.SurfaceCount;
	this->Info.Type			= TextureData.Info.Type;
	this->Info.PixelFormat	= TextureData.Info.PixelFormat;

	ZESize TempLevelCount	= (ZESize)Info.LevelCount;
	ZESize TempSurfaceCount = (ZESize)Info.SurfaceCount;

	// Create surfaces
	Surfaces.Resize(TempSurfaceCount);

	// Create Levels of each Surface
	for (ZESize I = 0; I < TempSurfaceCount; ++I)
	{
		Surfaces[I].Surface = (ZEUInt)I;
		Surfaces[I].Owner	= this;
		Surfaces[I].Levels.Resize(TempLevelCount);

		// Allocate each level data
		for (ZESize J = 0; J < TempLevelCount; ++J)
		{
			void* Source = TextureData.Surfaces[I].Levels[J].Data;

			Surfaces[I].Levels[J].Level = (ZEUInt)J;
			Surfaces[I].Levels[J].Owner = &Surfaces[I];

			if (Source != NULL)
			{
				// If surface is valid. Some 3D levels may be empty
				Surfaces[I].Levels[J].Data	= new ZEUInt8[Surfaces[I].Levels[J].GetSize()];

				ZESize SourcePitch	= TextureData.Surfaces[I].Levels[J].GetPitch();
				Surfaces[I].Levels[J].CopyFrom(Source, SourcePitch);
			
			}
			else
			{
				// Set level to empty
				Surfaces[I].Levels[J].Data	= NULL;
			}
		}
	}
}

// Destroys the content
void ZETextureData::Destroy()
{
	ZESize TempLevelCount	= (ZESize)Info.LevelCount;
	ZESize TempSurfaceCount = (ZESize)Info.SurfaceCount;

	// free Levels of each Surface
	for (ZESize I = 0; I < TempSurfaceCount; ++I)
	{
		for (ZESize J = 0; J < TempLevelCount; ++J)
		{
			// Clear level data
			Surfaces[I].Levels[J].Level = 0;
			Surfaces[I].Levels[J].Owner = NULL;
			
			// If there is level image data
			if (Surfaces[I].Levels[J].Data != NULL)
			{
				delete[] Surfaces[I].Levels[J].Data;
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
	this->Info.Width		= 0;
	this->Info.Height		= 0;
	this->Info.LevelCount	= 0;
	this->Info.SurfaceCount	= 0;
	this->Info.Type			= ZEGR_TT_2D;
	this->Info.PixelFormat	= ZEGR_TF_NONE;

}

// Returns array of surfaces
ZEArray<ZETextureSurface>& ZETextureData::GetSurfaces()
{
	return Surfaces;
}

// True if texture is empty
bool ZETextureData::IsEmpty()
{
	return Info.SurfaceCount == 0 ? true : false;
}

const ZETextureDataInfo& ZETextureData::GetInfo()
{
	return Info;
}

// Returns texture level0 width
ZEUInt ZETextureData::GetWidth()
{
	return Info.Width;
}

// Returns texture level0 height
ZEUInt ZETextureData::GetHeight()
{
	return Info.Height;
}

// Returns level count of the texture
ZEUInt ZETextureData::GetLevelCount()
{
	return Info.LevelCount;
}

// Returns Surface count of the texture
ZEUInt ZETextureData::GetSurfaceCount()
{
	return Info.SurfaceCount;
}

// Returns texture type
ZEGRTextureType ZETextureData::GetType()
{
	return Info.Type;
}

// Returns pixel format
ZEGRFormat ZETextureData::GetPixelFormat()
{
	return Info.PixelFormat;
}

// Returns size of total raw texture data
ZESize ZETextureData::GetSize()
{
	ZESize TextureSize = 0;
	ZESize TempLevelCount	= (ZESize)Info.LevelCount;
	ZESize TempSurfaceCount = (ZESize)Info.SurfaceCount;

	for (ZESize I = 0; I < TempSurfaceCount; I++)
	{
		for (ZESize J = 0; J < TempLevelCount; J++)
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
	ZESize TempLevelCount					= (ZESize)Info.LevelCount;
	ZESize TempSurfaceCount					= (ZESize)Info.SurfaceCount;

	ZESize ZETextureFileHeaderSize			= sizeof(ZETextureFileHeader);
	ZESize ZETextureFileSurfaceChunkSize	= sizeof(ZETextureFileSurfaceChunk);
	ZESize ZETExtureFileMipmapChunkSize		= sizeof(ZETExtureFileMipmapChunk);

	TextureSize += ZETextureFileHeaderSize;

	for (ZESize I = 0; I < TempSurfaceCount; I++)
	{
		TextureSize += ZETextureFileSurfaceChunkSize;

		for (ZESize J = 0; J < TempLevelCount; J++)
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
	Info.Width			= 0;
	Info.Height			= 0;
	Info.LevelCount		= 0;
	Info.SurfaceCount	= 0;
	Info.Type			= ZEGR_TT_2D;
	Info.PixelFormat	= ZEGR_TF_NONE;

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
	if (TextureData->GetSurfaceCount() != 1)
		return;

	ZEUInt TargetWidth	= TextureData->GetWidth() / 3;
	ZEUInt TargetHeight = TextureData->GetHeight() / 2;

	zeDebugCheck(TargetWidth != TargetHeight, "Cannot convert texture data. Dimensions do not match..");

	// Empty Output
	if (!Output->IsEmpty())
	{
		zeWarning("Output Texture data is not empty. Clearing it.");
		Output->Destroy();
	}

	// Create output
	Output->Create(ZEGR_TT_CUBE, TextureData->GetPixelFormat(), 6, 1, TargetWidth, TargetHeight);

	struct
	{
		ZESize OffsetX;
		ZESize OffsetY;

	} Offsets[6] = {									// Copy Order

		{2 * (ZESize)TargetWidth, 0},					// +X Face
		{0 * (ZESize)TargetWidth, 0},					// -X Face
		{2 * (ZESize)TargetWidth, (ZESize)TargetWidth},	// +Y Face
		{1 * (ZESize)TargetWidth, (ZESize)TargetWidth},	// -Y Face
		{1 * (ZESize)TargetWidth, 0},					// +Z Face
		{0 * (ZESize)TargetWidth, (ZESize)TargetWidth},	// -Z Face
	};

	void* SourceData	= TextureData->GetSurfaces().GetItem(0).GetLevels().GetItem(0).GetData();
	ZESize SourcePitch	= TextureData->GetSurfaces().GetItem(0).GetLevels().GetItem(0).GetPitch();
	
	ZESize TempTargetHeight = (ZESize)TargetHeight;
	ZESize TempSurfaceCount = (ZESize)Output->GetSurfaceCount();


	for (ZESize I = 0; I < TempSurfaceCount; ++I)
	{
		void* TargetData	= Output->GetSurfaces().GetItem(I).GetLevels().GetItem(0).GetData();
		ZESize TargetPitch	= Output->GetSurfaces().GetItem(I).GetLevels().GetItem(0).GetPitch();
		
		for (ZESize J = 0; J < TempTargetHeight; ++J)
		{
			memcpy((ZEUInt8*)TargetData + (J * TargetPitch), (ZEUInt8*)SourceData + SourcePitch * (Offsets[I].OffsetY + J) + Offsets[I].OffsetX * 4, TargetPitch);
		}
	}
}

// Converts a single surface, single level texture data to n surface texture data
void ZETextureData::ConvertToVolumeTextureData(ZETextureData* Output, ZETextureData* TextureData, ZEUInt HorizTileCount, ZEUInt VertTileCount)
{
	ZEUInt TargetWidth	= TextureData->GetWidth() / HorizTileCount;
	ZEUInt TargetHeight = TextureData->GetHeight() / VertTileCount;

	void* SourceData	= TextureData->GetSurfaces().GetItem(0).GetLevels().GetItem(0).GetData();
	ZESize SourcePitch	= TextureData->GetSurfaces().GetItem(0).GetLevels().GetItem(0).GetPitch();

	// Empty Output
	if (!Output->IsEmpty())
	{
		zeWarning("Output Texture data is not empty. Clearing it.");
		Output->Destroy();
	}

	Output->Create(ZEGR_TT_3D, TextureData->GetPixelFormat(), HorizTileCount * VertTileCount, 1, TargetWidth, TargetHeight);


	ZESize TempTargetHeight		= (ZESize)TargetHeight;
	ZESize TempVertTileCount	= (ZESize)VertTileCount;
	ZESize TempHorizTileCount	= (ZESize)HorizTileCount;

	for (ZESize I = 0; I < TempVertTileCount; ++I)
	{
		for (ZESize J = 0; J < TempHorizTileCount; ++J)
		{
			void* TargetData	= Output->GetSurfaces().GetItem(TempHorizTileCount * I + J).GetLevels().GetItem(0).GetData();
			ZESize TargetPitch	= Output->GetSurfaces().GetItem(TempHorizTileCount * I + J).GetLevels().GetItem(0).GetPitch();
		
			for (ZESize K = 0; K < (ZESize)TargetHeight; ++K)
			{
				memcpy((ZEUInt8*)TargetData + (K * TargetPitch), (ZEUInt8*)SourceData + SourcePitch * (TempTargetHeight * I + K) + TargetPitch * J, TargetPitch);
			}
		}
	}
}
