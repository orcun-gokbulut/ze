//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureQualityManager.cpp
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
#include "ZETextureTools.h"
#include "ZETextureLoader.h"
#include "ZETextureOptions.h"
#include "ZECore\ZEConsole.h"
#include "ZETextureQualityManager.h"
#include "ZEMath\ZEMath.h"


#define	ZE_TC_BLOCK_WIDTH	4 // Pixels
#define ZE_TC_BLOCK_HEIGHT	4 // Pixels
#define ZE_PIXEL_SIZE		4 // Bytes

static bool IsCompressed(ZEGRTextureFormat PixelFormat)
{
	switch (PixelFormat)
	{
		case ZEGR_TF_DXT1:
		case ZEGR_TF_DXT3:
		case ZEGR_TF_DXT5:
			return true;
			break;
		default:
			return false;
			break;
	}

	return false;
}

static ZEUInt GetDownSampleCount(ZETextureDownSampling DownSample)
{
	switch (DownSample)
	{
		case ZE_TDS_NONE:
			return 0;
			break;
		case ZE_TDS_2X:
			return 1;
			break;
		case ZE_TDS_4X:
			return 2;
			break;
		case ZE_TDS_8X:
			return 3;
			break;
		default:
			return 0;
			break;
	}

	return 0;
}

static ZEGRTextureFormat GetCompressedPixelFormat(ZETextureCompressionType CompressionType)
{
	switch (CompressionType)
	{
		case ZE_TCT_DXT1:
			return ZEGR_TF_DXT1;
			break;
		case ZE_TCT_DXT3:
			return ZEGR_TF_DXT3;
			break;
		case ZE_TCT_DXT5:
			return ZEGR_TF_DXT5;
			break;
		default:
			return ZEGR_TF_NONE;
			break;
	}

	return ZEGR_TF_NONE;
}

static ZETextureCompressionType GetCompressionType(ZEGRTextureFormat PixelFormat)
{
	switch (PixelFormat)
	{
		case ZEGR_TF_I8_4:
			return ZE_TCT_NONE;
			break;
		case ZEGR_TF_DXT1:
			return ZE_TCT_DXT1;
			break;
		case ZEGR_TF_DXT3:
			return ZE_TCT_DXT3;
			break;
		case ZEGR_TF_DXT5:
			return ZE_TCT_DXT5;
			break;
		default:
			return ZE_TCT_NONE;
			break;
	}

	return ZE_TCT_NONE;
}

ZETextureQualityManager::ZETextureQualityManager()
{
	// Empty
}

ZETextureQualityManager::~ZETextureQualityManager()
{
	// Empty
}

bool ZETextureQualityManager::Process(ZETextureData* Output, ZETextureData* TextureData, ZETextureOptions* FinalOptions)
{
	// If texture data is empty
	if(TextureData->IsEmpty())
	{
		zeWarning("Cannot process texture data. Texture data is empty.");
		return false;
	}

	// Empty Output
	if (!Output->IsEmpty())
	{
		zeWarning("Output Texture data is not empty. Clearing it.");
		Output->Destroy();
	}

	// If already compressed
	if (IsCompressed(TextureData->GetPixelFormat()))
	{
		zeWarning("Texture data is already compressed. Texture will not be processed.");
		return true;
	}

	// If nothing requested 
	if (FinalOptions->CompressionType == ZE_TCT_NONE && FinalOptions->DownSample == ZE_TDS_NONE && FinalOptions->MipMapping == ZE_TMM_DISABLED)
	{
		// Copy and return 
		Output->Create(*TextureData);
		return true;
	}

	// If no change is needed
	if (FinalOptions->MaximumMipmapLevel == TextureData->GetLevelCount() && FinalOptions->CompressionType == GetCompressionType(TextureData->GetPixelFormat()))
	{
		// Copy and return 
		Output->Create(*TextureData); 
		return true;
	}

	// Process flags
	bool Compress			= (FinalOptions->CompressionType == ZE_TCT_NONE) ? false : true;
	bool GenerateMipmaps	= (FinalOptions->MipMapping == ZE_TMM_DISABLED) ? false : true;

	
	// Create output
	ZEUInt DestLevelCount					= FinalOptions->MaximumMipmapLevel;
	ZEUInt SrcSurfaceCount					= TextureData->GetSurfaceCount();
	ZEGRTextureType SrcTextureType			= TextureData->GetType();
	ZEGRTextureFormat DestPixelFormat	= GetCompressedPixelFormat(FinalOptions->CompressionType) == ZEGR_TF_NONE ? TextureData->GetPixelFormat() : GetCompressedPixelFormat(FinalOptions->CompressionType);

	ZEUInt SrcWidth			= TextureData->GetWidth();
	ZEUInt SrcHeight		= TextureData->GetHeight();
	ZEUInt SrcLevelCount	= TextureData->GetLevelCount();

	// Get down sample count
	ZEUInt DownSampleCount			= GetDownSampleCount(FinalOptions->DownSample);
	
	Output->Create(SrcTextureType, DestPixelFormat, SrcSurfaceCount, DestLevelCount, SrcWidth >> DownSampleCount, SrcHeight >> DownSampleCount);

	// Compress and generate MipMaps according to texture type
	switch (SrcTextureType)
	{
		// ----------------------------------------------------------------------
		// Create single buffer and process single surface
		// ----------------------------------------------------------------------
		case ZEGR_TT_2D:
		{
			// Create the temp buffer
			ZESize BufferSize		= TextureData->GetSurfaces().GetItem(0).GetLevels().GetItem(0).GetSize();
			ZESize BufferPitch		= TextureData->GetSurfaces().GetItem(0).GetLevels().GetItem(0).GetPitch();
			void* Buffer			= new ZEUInt8[BufferSize];

			// Compress and generate MipMaps
			if (Compress && GenerateMipmaps)
			{
				// Get level 0 of surface 0 to buffer
				TextureData->GetSurfaces().GetItem(0).GetLevels().GetItem(0).CopyTo(Buffer, BufferPitch);

				// Resize if it is requested
				for(ZEUInt K = 0; K < DownSampleCount; ++K)
				{
					ZETextureTools::DownSample2x(Buffer, BufferPitch, Buffer, BufferPitch, SrcWidth >> K, SrcHeight >> K);
				}

				// For every level
				for(ZESize Level = 0; Level < (ZESize)DestLevelCount; ++Level)
				{
					void* DestData		= Output->GetSurfaces().GetItem(0).GetLevels().GetItem(Level).GetData();
					ZESize DestPitch	= Output->GetSurfaces().GetItem(0).GetLevels().GetItem(Level).GetPitch();

					// Compress
					ZETextureTools::CompressTexture(DestData, DestPitch, Buffer, BufferPitch, SrcWidth >> (DownSampleCount + (ZEUInt)Level), SrcHeight >> (DownSampleCount + (ZEUInt)Level), FinalOptions);

					// Do not resize the last 2 levels since 4x4 block is needed for compression
					if (SrcWidth > 4 && SrcHeight > 4)
					{
						ZETextureTools::DownSample2x(Buffer, BufferPitch, Buffer, BufferPitch, SrcWidth >> (DownSampleCount + (ZEUInt)Level), SrcHeight >> (DownSampleCount + (ZEUInt)Level), true);
					}
				}

			}
			// Compress but do not generate MipMaps
			else if (Compress && !GenerateMipmaps)
			{
				// Get level 0 of surface 0 to buffer
				TextureData->GetSurfaces().GetItem(0).GetLevels().GetItem(0).CopyTo(Buffer, BufferPitch);

				// Resize if it is requested
				for(ZEUInt K = 0; K < DownSampleCount; ++K)
				{
					ZETextureTools::DownSample2x(Buffer, BufferPitch, Buffer, BufferPitch, SrcWidth >> K, SrcHeight >> K);
				}

				// Compress the only level
				void* DestData		= Output->GetSurfaces().GetItem(0).GetLevels().GetItem(0).GetData();
				ZESize DestPitch	= Output->GetSurfaces().GetItem(0).GetLevels().GetItem(0).GetPitch();

				// Compress and send the result to destination
				ZETextureTools::CompressTexture(DestData, DestPitch, Buffer, BufferPitch, SrcWidth >> DownSampleCount, SrcHeight >> DownSampleCount, FinalOptions);
				
			}
			// Do not compress but generate MipMaps
			else if (!Compress && GenerateMipmaps)
			{
				// Get level 0 of surface 0 to buffer
				TextureData->GetSurfaces().GetItem(0).GetLevels().GetItem(0).CopyTo(Buffer, BufferPitch);

				// Resize if it is requested
				for(ZEUInt K = 0; K < DownSampleCount; ++K)
				{
					ZETextureTools::DownSample2x(Buffer, BufferPitch, Buffer, BufferPitch, SrcWidth >> K, SrcHeight >> K);
				}

				// For every level
				for(ZESize Level = 0; Level < (ZESize)DestLevelCount; ++Level)
				{
					void* DestData		= Output->GetSurfaces().GetItem(0).GetLevels().GetItem(Level).GetData();
					ZESize DestPitch	= Output->GetSurfaces().GetItem(0).GetLevels().GetItem(Level).GetPitch();

					// Copy the down sampled level from buffer
					Output->GetSurfaces().GetItem(0).GetLevels().GetItem(Level).CopyFrom(Buffer, BufferPitch);

					// Down Sample
					ZETextureTools::DownSample2x(Buffer, BufferPitch, Buffer, BufferPitch, SrcWidth >> (DownSampleCount + (ZEUInt)Level), SrcHeight >> (DownSampleCount + (ZEUInt)Level), true);
				}

			}
			// Do not compress and do not generate MipMaps
			else if (!Compress && !GenerateMipmaps)
			{
				// Get level 0 of surface 0 to buffer
				TextureData->GetSurfaces().GetItem(0).GetLevels().GetItem(0).CopyTo(Buffer, BufferPitch);

				// Initial resize if it is requested
				for(ZEUInt K = 0; K < DownSampleCount; ++K)
				{
					ZETextureTools::DownSample2x(Buffer, BufferPitch, Buffer, BufferPitch, SrcWidth >> K, SrcHeight >> K);
				}
				
				// Copy the only level from buffer
				Output->GetSurfaces().GetItem(0).GetLevels().GetItem(0).CopyFrom(Buffer, BufferPitch);
			}

			delete[] Buffer;
			return true;
			break;
		}


		// ----------------------------------------------------------------------
		// Create single buffer and process 6 surface of cube map
		// ----------------------------------------------------------------------
		case ZEGR_TT_CUBE:
		{
			// Create the temp buffer
			ZESize BufferSize		= TextureData->GetSurfaces().GetItem(0).GetLevels().GetItem(0).GetSize();
			ZESize BufferPitch		= TextureData->GetSurfaces().GetItem(0).GetLevels().GetItem(0).GetPitch();
			void* Buffer			= new ZEUInt8[BufferSize];

			// Compress and generate MipMaps
			if (Compress && GenerateMipmaps)
			{
				// For every surface
				for(ZESize Surface = 0; Surface < (ZESize)SrcSurfaceCount; ++Surface)
				{
					// Get level 0 of surface I into buffer
					TextureData->GetSurfaces().GetItem(Surface).GetLevels().GetItem(0).CopyTo(Buffer, BufferPitch);

					// Resize the surface if it is requested
					for(ZEUInt K = 0; K < DownSampleCount; ++K)
					{
						ZETextureTools::DownSample2x(Buffer, BufferPitch, Buffer, BufferPitch, SrcWidth >> K, SrcHeight >> K);
					}

					// For every level
					for(ZESize Level = 0; Level < (ZESize)DestLevelCount; ++Level)
					{
						void* DestData		= Output->GetSurfaces().GetItem(Surface).GetLevels().GetItem(Level).GetData();
						ZESize DestPitch	= Output->GetSurfaces().GetItem(Surface).GetLevels().GetItem(Level).GetPitch();

						// Compress
						ZETextureTools::CompressTexture(DestData, DestPitch, Buffer, BufferPitch, SrcWidth >> (DownSampleCount + (ZEUInt)Level), SrcHeight >> (DownSampleCount + (ZEUInt)Level), FinalOptions);

						// Do not resize the last 2 levels since 4x4 block is needed for compression
						if (SrcWidth > 4 && SrcHeight > 4)
						{
							ZETextureTools::DownSample2x(Buffer, BufferPitch, Buffer, BufferPitch, SrcWidth >> (DownSampleCount + (ZEUInt)Level), SrcHeight >> (DownSampleCount + (ZEUInt)Level), true);
						}
					}
				}
			}
			// Compress but do not generate MipMaps
			else if (Compress && !GenerateMipmaps)
			{

				// For every surface
				for(ZESize Surface = 0; Surface < (ZESize)SrcSurfaceCount; ++Surface)
				{
					// Get level 0 of surface I into buffer
					TextureData->GetSurfaces().GetItem(Surface).GetLevels().GetItem(0).CopyTo(Buffer, BufferPitch);

					// Resize the surface if it is requested
					for(ZEUInt K = 0; K < DownSampleCount; ++K)
					{
						ZETextureTools::DownSample2x(Buffer, BufferPitch, Buffer, BufferPitch, SrcWidth >> K, SrcHeight >> K);
					}

					void* DestData		= Output->GetSurfaces().GetItem(Surface).GetLevels().GetItem(0).GetData();
					ZESize DestPitch	= Output->GetSurfaces().GetItem(Surface).GetLevels().GetItem(0).GetPitch();

					// Compress and send the result to destination
					ZETextureTools::CompressTexture(DestData, DestPitch, Buffer, BufferPitch, SrcWidth >> DownSampleCount, SrcHeight >> DownSampleCount, FinalOptions);

				}
			}
			// Do not compress but generate MipMaps
			else if (!Compress && GenerateMipmaps)
			{
				// For every surface
				for(ZESize Surface = 0; Surface < (ZESize)SrcSurfaceCount; ++Surface)
				{
					// Get level 0 of surface I into buffer
					TextureData->GetSurfaces().GetItem(Surface).GetLevels().GetItem(0).CopyTo(Buffer, BufferPitch);

					// Resize the surface if it is requested
					for(ZEUInt K = 0; K < DownSampleCount; ++K)
					{
						ZETextureTools::DownSample2x(Buffer, BufferPitch, Buffer, BufferPitch, SrcWidth >> K, SrcHeight >> K);
					}

					// For every level
					for(ZESize Level = 0; Level < (ZESize)DestLevelCount; ++Level)
					{
						// Copy the down sampled level from buffer
						Output->GetSurfaces().GetItem(Surface).GetLevels().GetItem(Level).CopyFrom(Buffer, BufferPitch);

						// Down Sample
						ZETextureTools::DownSample2x(Buffer, BufferPitch, Buffer, BufferPitch, SrcWidth >> (DownSampleCount + (ZEUInt)Level), SrcHeight >> (DownSampleCount  + (ZEUInt)Level), true);

					}
				}
			}
			// Do not compress and do not generate MipMaps
			else if (!Compress && !GenerateMipmaps)
			{
		
				// For every surface
				for(ZESize Surface = 0; Surface < (ZESize)SrcSurfaceCount; ++Surface)
				{
					// Get level 0 of surface I into buffer
					TextureData->GetSurfaces().GetItem(Surface).GetLevels().GetItem(0).CopyTo(Buffer, BufferPitch);
					
					// Resize the surface if it is requested
					for(ZEUInt K = 0; K < DownSampleCount; ++K)
					{
						ZETextureTools::DownSample2x(Buffer, BufferPitch, Buffer, BufferPitch, SrcWidth >> K, SrcHeight >> K);
					}

					// Copy the level from buffer to destination level
					Output->GetSurfaces().GetItem(Surface).GetLevels().GetItem(0).CopyFrom(Buffer, BufferPitch);
				}
			}

			delete[] Buffer;
			return true;
			break;
		}

		// ----------------------------------------------------------------------
		// Create double buffer & a temp TextureData to process n surface of volume texture
		// ----------------------------------------------------------------------
		case ZEGR_TT_3D:
		{
			// Create the temp buffer
			ZESize BufferSize		= TextureData->GetSurfaces().GetItem(0).GetLevels().GetItem(0).GetSize();
			ZESize BufferPitch		= TextureData->GetSurfaces().GetItem(0).GetLevels().GetItem(0).GetPitch();
			void* Buffer1			= new ZEUInt8[BufferSize];
			void* Buffer2			= new ZEUInt8[BufferSize];

			void* TempSurface1		= NULL;	// first slice of 2x2 level
			void* TempSUrface2		= NULL; // second slice of 2x2 level
			void* TempSUrface3		= NULL; // 1x1 level

			ZETextureData TempTextureData;
			TempTextureData.Create(ZEGR_TT_3D, TextureData->GetPixelFormat(), SrcSurfaceCount, DestLevelCount, SrcWidth >> DownSampleCount, SrcHeight >> DownSampleCount);
			
			// Compress and generate MipMaps
			if (Compress && GenerateMipmaps)
			{
				// First copy all unprocessed level 0 to output as it is
				for (ZESize Surface = 0; Surface < (ZESize)SrcSurfaceCount; ++Surface)
				{
					// Get level 0 of surface I into buffer
					TextureData->GetSurfaces().GetItem(Surface).GetLevels().GetItem(0).CopyTo(Buffer1, BufferPitch);

					// Resize the surface if it is requested
					for(ZEUInt K = 0; K < DownSampleCount; ++K)
					{
						ZETextureTools::DownSample2x(Buffer1, BufferPitch, Buffer1, BufferPitch, SrcWidth >> K, SrcHeight >> K);
					}

					// Write uncompressed data to level 0 
					TempTextureData.GetSurfaces().GetItem(Surface).GetLevels().GetItem(0).CopyFrom(Buffer1, BufferPitch);

					void* DestData		= Output->GetSurfaces().GetItem(Surface).GetLevels().GetItem(0).GetData();
					ZESize DestPitch	= Output->GetSurfaces().GetItem(Surface).GetLevels().GetItem(0).GetPitch();

					void* SrcData		= TempTextureData.GetSurfaces().GetItem(Surface).GetLevels().GetItem(0).GetData();
					ZESize SrcPitch		= TempTextureData.GetSurfaces().GetItem(Surface).GetLevels().GetItem(0).GetPitch();
					ZEUInt SrcWidth		= TempTextureData.GetSurfaces().GetItem(Surface).GetLevels().GetItem(0).GetWidth();
					ZEUInt SrcHeight	= TempTextureData.GetSurfaces().GetItem(Surface).GetLevels().GetItem(0).GetHeight();

					// Get data from TempTextureData, Compress and write to Output level 0
					ZETextureTools::CompressTexture(DestData, DestPitch, SrcData, SrcPitch, SrcWidth, SrcHeight, FinalOptions);
				}

				// For every valid level starting from 1. (level 0 is already filled)
				for(ZESize Level = 1,  SurfaceIncrement = 2; Level < (ZESize)DestLevelCount; ++Level, SurfaceIncrement *= 2)
				{
					ZESize UpperLevelIncrement = SurfaceIncrement / 2;

					// For every valid surface
					for(ZESize Surface = 0; Surface < (ZESize)SrcSurfaceCount; Surface += SurfaceIncrement)
					{
						// Get 2 upper level data
						void* SrcData1		= TempTextureData.GetSurfaces().GetItem(Surface).GetLevels().GetItem(Level - 1).GetData();
						ZESize SrcPitch1	= TempTextureData.GetSurfaces().GetItem(Surface).GetLevels().GetItem(Level - 1).GetPitch();
						ZEUInt SrcWidth1	= TempTextureData.GetSurfaces().GetItem(Surface).GetLevels().GetItem(Level - 1).GetWidth();
						ZEUInt SrcHeight1	= TempTextureData.GetSurfaces().GetItem(Surface).GetLevels().GetItem(Level - 1).GetHeight();

						void* SrcData2		= TempTextureData.GetSurfaces().GetItem(Surface + UpperLevelIncrement).GetLevels().GetItem(Level - 1).GetData();
						ZESize SrcPitch2	= TempTextureData.GetSurfaces().GetItem(Surface + UpperLevelIncrement).GetLevels().GetItem(Level - 1).GetPitch();
						ZEUInt SrcWidth2	= TempTextureData.GetSurfaces().GetItem(Surface + UpperLevelIncrement).GetLevels().GetItem(Level - 1).GetWidth();
						ZEUInt SrcHeight2	= TempTextureData.GetSurfaces().GetItem(Surface + UpperLevelIncrement).GetLevels().GetItem(Level - 1).GetHeight();

					//	// Check if it is the level 2x2 and 1x1 levels. if so do not resize since they wont be compressible after the resize
					//	//if (SrcWidth1 > 4 && SrcHeight1 > 4 && SrcWidth2 > 4 && SrcHeight2 > 4)
					//	//{
							// Copy To Buffer
							TempTextureData.GetSurfaces().GetItem(Surface).GetLevels().GetItem(Level - 1).CopyTo(Buffer1, BufferPitch);
							TempTextureData.GetSurfaces().GetItem(Surface + UpperLevelIncrement).GetLevels().GetItem(Level - 1).CopyTo(Buffer2, BufferPitch);

							// Down Sample each buffer
							ZETextureTools::DownSample2x(Buffer1, BufferPitch, Buffer1, BufferPitch, SrcWidth1, SrcHeight1, true);
							ZETextureTools::DownSample2x(Buffer2, BufferPitch, Buffer2, BufferPitch, SrcWidth2, SrcHeight2, true);

							// Average into buffer 1 (for trilinear interpolation)
							ZETextureTools::Average(Buffer1, BufferPitch, Buffer1, BufferPitch, SrcWidth1, SrcHeight1, Buffer2, BufferPitch, SrcWidth2, SrcHeight2);

							// Copy processed data back to TempTextureData
							TempTextureData.GetSurfaces().GetItem(Surface).GetLevels().GetItem(Level).CopyFrom(Buffer1, BufferPitch);

							// Get target info
							void* DestData		= Output->GetSurfaces().GetItem(Surface).GetLevels().GetItem(Level).GetData();
							ZESize DestPitch	= Output->GetSurfaces().GetItem(Surface).GetLevels().GetItem(Level).GetPitch();

							// Source info
							void* SrcData		= TempTextureData.GetSurfaces().GetItem(Surface).GetLevels().GetItem(Level).GetData();
							ZESize SrcPitch		= TempTextureData.GetSurfaces().GetItem(Surface).GetLevels().GetItem(Level).GetPitch();
							ZEUInt SrcWidth		= TempTextureData.GetSurfaces().GetItem(Surface).GetLevels().GetItem(Level).GetWidth();
							ZEUInt SrcHeight	= TempTextureData.GetSurfaces().GetItem(Surface).GetLevels().GetItem(Level).GetHeight();

							// Compress TempTextureData and write to output
							ZETextureTools::CompressTexture(DestData, DestPitch, SrcData, SrcPitch, SrcWidth, SrcHeight, FinalOptions);
					//	}
					//	else
					//	{
					//		// Copy To Buffer
					//		TempTextureData.GetSurfaces().GetItem(Surface).GetLevels().GetItem(Level - 1).CopyTo(Buffer1, BufferPitch);
					//		TempTextureData.GetSurfaces().GetItem(Surface + UpperLevelIncrement).GetLevels().GetItem(Level - 1).CopyTo(Buffer2, BufferPitch);
					//
					//		// Average into buffer 1 (for trilinear interpolation)
					//		ZETextureTools::Average(Buffer1, BufferPitch, Buffer1, BufferPitch, SrcWidth1, SrcHeight1, Buffer2, BufferPitch, SrcWidth2, SrcHeight2);
					//	}
					}
				}
				/*
				// Compress the TempTextureData and write to Output
				for(ZESize Level = 0,  SurfaceIncrement = 1; Level < DestLevelCount; ++Level, SurfaceIncrement *= 2)
				{
					// For every valid surface
					for(ZESize Surface = 0; Surface < SrcSurfaceCount; Surface += SurfaceIncrement)
					{
						void* DestData = Output->GetSurfaces().GetItem(Surface).GetLevels().GetItem(Level).GetData();
						ZESize DestPitch = Output->GetSurfaces().GetItem(Surface).GetLevels().GetItem(Level).GetPitch();
						
						void* SourceData = TempTextureData.GetSurfaces().GetItem(Surface).GetLevels().GetItem(Level).GetData();
						ZESize SourcePitch = TempTextureData.GetSurfaces().GetItem(Surface).GetLevels().GetItem(Level).GetPitch();
						ZESize SourceWidth = TempTextureData.GetSurfaces().GetItem(Surface).GetLevels().GetItem(Level).GetWidth();
						ZESize SourceHeight = TempTextureData.GetSurfaces().GetItem(Surface).GetLevels().GetItem(Level).GetHeight();

						// Compress and send the result to destination
						ZETextureTools::CompressTexture(DestData, DestPitch, SourceData, SourcePitch, SourceWidth, SourceHeight, FinalOptions);
					}
				}
				*/
			}
			// Compress but do not generate MipMaps
			else if (Compress && !GenerateMipmaps)
			{
				// For every surface of source texture
				for(ZESize Surface = 0; Surface < (ZESize)SrcSurfaceCount; ++Surface)
				{
					// Get level 0 of surface I into buffer
					TextureData->GetSurfaces().GetItem(Surface).GetLevels().GetItem(0).CopyTo(Buffer1, BufferPitch);

					// Resize the surface if it is requested
					for(ZEUInt K = 0; K < DownSampleCount; ++K)
					{
						ZETextureTools::DownSample2x(Buffer1, BufferPitch, Buffer1, BufferPitch, SrcWidth >> K, SrcHeight >> K);
					}

					// Get Destination info
					void* DestData		= Output->GetSurfaces().GetItem(Surface).GetLevels().GetItem(0).GetData();
					ZESize DestPitch	= Output->GetSurfaces().GetItem(Surface).GetLevels().GetItem(0).GetPitch();

					// Compress and write to destination
					ZETextureTools::CompressTexture(DestData, DestPitch, Buffer1, BufferPitch, SrcWidth >> DownSampleCount, SrcHeight >> DownSampleCount, FinalOptions);

				}
			}
			// Do not compress but generate MipMaps
			else if (!Compress && GenerateMipmaps)
			{
				// First copy all unprocessed level 0 to output as it is
				for (ZESize Surface = 0; Surface < (ZESize)SrcSurfaceCount; ++Surface)
				{
					// Get level 0 of surface I into buffer
					TextureData->GetSurfaces().GetItem(Surface).GetLevels().GetItem(0).CopyTo(Buffer1, BufferPitch);

					// Resize the surface if it is requested
					for(ZEUInt K = 0; K < DownSampleCount; ++K)
					{
						ZETextureTools::DownSample2x(Buffer1, BufferPitch, Buffer1, BufferPitch, SrcWidth >> K, SrcHeight >> K);
					}

					Output->GetSurfaces().GetItem(Surface).GetLevels().GetItem(0).CopyFrom(Buffer1, BufferPitch);
				}


				// For every valid level starting from 1. (level 0 is already filled)
				for(ZESize Level = 1,  SurfaceIncrement = 2; Level < (ZESize)DestLevelCount; ++Level, SurfaceIncrement *= 2)
				{
					ZESize UpperLevelIncrement = SurfaceIncrement / 2;

					// For every valid surface
					for(ZESize Surface = 0; Surface < (ZESize)SrcSurfaceCount; Surface += SurfaceIncrement)
					{
						// Get 2 upper level
						ZETextureLevel* SourceLevel1 = &Output->GetSurfaces().GetItem(Surface).GetLevels().GetItem(Level - 1);
						ZETextureLevel* SourceLevel2 = &Output->GetSurfaces().GetItem(Surface + UpperLevelIncrement).GetLevels().GetItem(Level - 1);

						void* SrcData1		= SourceLevel1->GetData();
						ZESize SrcPitch1	= SourceLevel1->GetPitch();
						ZEUInt SrcWidth1	= SourceLevel1->GetWidth();
						ZEUInt SrcHeight1	= SourceLevel1->GetHeight();

						void* SrcData2		= SourceLevel2->GetData();
						ZESize SrcPitch2	= SourceLevel2->GetPitch();
						ZEUInt SrcWidth2	= SourceLevel2->GetWidth();
						ZEUInt SrcHeight2	= SourceLevel2->GetHeight();

						// Copy To Buffer
						SourceLevel1->CopyTo(Buffer1, BufferPitch);
						SourceLevel2->CopyTo(Buffer2, BufferPitch);

						// Down Sample each buffer
						ZETextureTools::DownSample2x(Buffer1, BufferPitch, Buffer1, BufferPitch, SrcWidth1, SrcHeight1, true);
						ZETextureTools::DownSample2x(Buffer2, BufferPitch, Buffer2, BufferPitch, SrcWidth2, SrcHeight2, true);

						ZETextureTools::Average(Buffer1, BufferPitch, Buffer1, BufferPitch, SrcWidth1, SrcHeight1, Buffer2, BufferPitch, SrcWidth2, SrcHeight2);

						// Copy processed data back to destination
						Output->GetSurfaces().GetItem(Surface).GetLevels().GetItem(Level).CopyFrom(Buffer1, BufferPitch);

					}
				}
			}
			// Do not compress and do not generate MipMaps
			else if (!Compress && !GenerateMipmaps)
			{
				ZESize SurfaceIncrement = 1;

				// For every level
				for(ZESize Level = 0; Level < (ZESize)DestLevelCount; ++Level, SurfaceIncrement *= 2)
				{
					// For every valid surface
					for(ZESize Surface = 0; Surface < (ZESize)SrcSurfaceCount; Surface += SurfaceIncrement)
					{
						// Get level 0 of surface I into buffer
						TextureData->GetSurfaces().GetItem(Surface).GetLevels().GetItem(0).CopyTo(Buffer1, BufferPitch);

						// Resize the surface if it is requested
						for(ZEUInt K = 0; K < DownSampleCount; ++K)
						{
							ZETextureTools::DownSample2x(Buffer1, BufferPitch, Buffer1, BufferPitch, SrcWidth >> K, SrcHeight >> K);
						}

						Output->GetSurfaces().GetItem(Surface).GetLevels().GetItem(Level).CopyFrom(Buffer1, BufferPitch);
					}
				}
			}

			TempTextureData.Destroy();
			delete[] Buffer1;
			delete[] Buffer2;
			return true;
			break;
		}

		// Unknown texture type
		default:
		{
			zeError("Unknown texture type. Cannot process texture.");
			return false;
			break;
		}

	}

	return false;
}

bool ZETextureQualityManager::GetFinalTextureOptions(ZETextureOptions* FinalOptions, ZEFile* ResourceFile, const ZETextureOptions* UserOptions, const ZEUInt HorizTileCount, const ZEUInt VertTileCount, const ZEGRTextureType TextureType)
{
	if(UserOptions == NULL || FinalOptions == NULL)
	{
		zeCriticalError("Cannot proceed. UserOptions or FinalOptions is NULL, in function GetFinalTextureOptions()");
		return false;
	}

	ZETextureOptions *DefaultOptions = zeGraphics->GetTextureOptions();

	// Eliminate the auto options
	FinalOptions->CompressionType		= (UserOptions->CompressionType == ZE_TCT_AUTO)		? DefaultOptions->CompressionType		: UserOptions->CompressionType;
	FinalOptions->CompressionQuality	= (UserOptions->CompressionQuality == ZE_TCQ_AUTO)	? DefaultOptions->CompressionQuality	: UserOptions->CompressionQuality;
	FinalOptions->DownSample			= (UserOptions->DownSample == ZE_TDS_AUTO)			? DefaultOptions->DownSample			: UserOptions->DownSample;
	FinalOptions->MipMapping			= (UserOptions->MipMapping == ZE_TMM_AUTO)			? DefaultOptions->MipMapping			: UserOptions->MipMapping;
	FinalOptions->FileCaching			= (UserOptions->FileCaching == ZE_TFC_AUTO)			? DefaultOptions->FileCaching			: UserOptions->FileCaching;

	ZETextureDataInfo TextureInfo = {0};
	
	// Tries to get texture info from pack or standalone ZETextureFile
	if(!ZETextureLoader::GetTextureInfo(&TextureInfo, ResourceFile))
	{
		// If fails tries to get it from the source image file
		if(!ZETextureLoader::GetImageInfo(&TextureInfo, ResourceFile))
		{
			// There is no more place to look for
			zeCriticalError("Cannot proceed. Resource info is not found in \"%s\".", ResourceFile->GetPath().GetValue());
			return false;
		}
	}

	// Creating texture options constrains

	// Check if it is possible to resize (is it power of 2)
	if (!ZETextureTools::IsResizeable(TextureInfo.Width, TextureInfo.Height, HorizTileCount, VertTileCount, TextureType))
	{
		zeWarning("Texture is not resizeable. Mip Mapping disabled.");

		FinalOptions->DownSample = ZE_TDS_NONE;
		FinalOptions->MipMapping = ZE_TMM_DISABLED;
		FinalOptions->MaximumMipmapLevel = 1;
	}

	// Check if it is Compressible (divisible by 4)
	if (!ZETextureTools::IsCompressible(TextureInfo.Width, TextureInfo.Height, HorizTileCount, VertTileCount))
	{
		zeWarning("Texture is not compressible. Compression disabled.");
		FinalOptions->CompressionType = ZE_TCT_NONE;
	}

	// If there is MipMapping
	if (FinalOptions->MipMapping != ZE_TMM_DISABLED)
	{
		ZEUInt PossibleMaxMipmapNumber;
		ZEUInt MipMapCount = ZETextureTools::GetMaxMipmapCount(TextureInfo.Width, TextureInfo.Height, HorizTileCount, VertTileCount, TextureType);

		// Subtract the level count if there is a down sample before the process
		ZEInt Test = (ZEInt)MipMapCount - (ZEInt)GetDownSampleCount(FinalOptions->DownSample);
		// Check if greater than 0
		PossibleMaxMipmapNumber = Test > 0 ? (ZEUInt)Test : 1;
		
		// Set final level count
		FinalOptions->MaximumMipmapLevel = ZEMath::Min(UserOptions->MaximumMipmapLevel, PossibleMaxMipmapNumber);
	}
	else
	{
		FinalOptions->MaximumMipmapLevel = 1;
	}
	

	return true;
}
