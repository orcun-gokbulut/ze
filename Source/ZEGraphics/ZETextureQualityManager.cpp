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


#define	ZE_TC_BLOCK_WIDTH	4 // Pixels
#define ZE_TC_BLOCK_HEIGHT	4 // Pixels
#define ZE_PIXEL_SIZE		4 // Bytes

ZETextureQualityManager::ZETextureQualityManager()
{
	// Empty
}

ZETextureQualityManager::~ZETextureQualityManager()
{
	// Empty
}

bool ZETextureQualityManager::Process(ZETextureData* TextureData, ZETextureOptions* FinalOptions)
{
	// If texture data is empty
	if(TextureData->IsEmpty())
	{
		zeLog("Texture Quality Manager", "Texture data is empty.");
		return false;
	}

	if (FinalOptions->CompressionType == ZE_TCT_NONE &&
		FinalOptions->DownSample == ZE_TDS_NONE &&
		FinalOptions->MipMapping == ZE_TMM_DISABLED)
		return true;

	unsigned int OutputBlockSize = 0;
	unsigned int InputBlockWidth = 0;
	unsigned int InputBlockHeight = 0;

	// Decide pixel format and block properties
	ZETexturePixelFormat PixelFormat;
	switch(FinalOptions->CompressionType)
	{
		case ZE_TCT_NONE:
			PixelFormat			= ZE_TPF_A8R8G8B8;
			InputBlockWidth		= 1; // Pixels
			InputBlockHeight	= 1; // Pixels
			OutputBlockSize		= 4; // Bytes
			break;	
	
		case ZE_TCT_DXT1:
			PixelFormat			= ZE_TPF_DXT1;
			InputBlockWidth		= ZE_TC_BLOCK_WIDTH;  // Pixels
			InputBlockHeight	= ZE_TC_BLOCK_HEIGHT; // Pixels
			OutputBlockSize		= 8; // Bytes
			break;

		case ZE_TCT_DXT3:
			PixelFormat			= ZE_TPF_DXT3;
			InputBlockWidth		= ZE_TC_BLOCK_WIDTH;  // Pixels
			InputBlockHeight	= ZE_TC_BLOCK_HEIGHT; // Pixels
			OutputBlockSize		= 16; // Bytes
			break;

		case ZE_TCT_DXT5:
			PixelFormat			= ZE_TPF_DXT5;
			InputBlockWidth		= ZE_TC_BLOCK_WIDTH;  // Pixels
			InputBlockHeight	= ZE_TC_BLOCK_HEIGHT; // Pixels
			OutputBlockSize		= 16; // Bytes
			break;
	}

	unsigned int MipmapCount	= FinalOptions->MaximumMipmapLevel;
	unsigned int SurfaceCount	= TextureData->GetDepth();
	unsigned int Width			= TextureData->GetWidth();
	unsigned int Height			= TextureData->GetHeight();
	
	bool Compress = FinalOptions->CompressionType == ZE_TCT_NONE ? false : true;
	bool Mipmap = FinalOptions->MipMapping == ZE_TMM_DISABLED ? false : true;

	// Create the temp buffer
	void* Buffer = (void*)malloc(TextureData->GetMipmapDataSize(0, 0));
	unsigned int BufferPitch = TextureData->GetMipmapRowSize(0, 0);

	// Add new mipmaps if it is needed
	if(Mipmap && FinalOptions->MaximumMipmapLevel > TextureData->GetMipmapCount())
		TextureData->AddMipmap(FinalOptions->MaximumMipmapLevel - 1);
	
	// For every surface
	for(unsigned int I = 0; I < SurfaceCount; I++)
	{
		// Get mipmap 0 of surface I to buffer
		TextureData->CopyMipmapDataTo(I, 0, Buffer, BufferPitch);

		// Downsample if it is needed
		for(unsigned int K = 0; K < (unsigned int)FinalOptions->DownSample; K++)
		{
			// Down Sample if requested
			ZETextureTools::DownSample2x(Buffer, BufferPitch, Buffer, BufferPitch, Width, Height);
			Width /= 2;
			Height /= 2;
		}

		TextureData->SetWidth(Width);
		TextureData->SetHeight(Height);
		TextureData->SetPixelFormat(PixelFormat);

		void* DestData			= 0;
		unsigned int DestPitch	= 0;
		unsigned int RowSize	= 0;
		unsigned int RowCount	= 0;		

		if (Compress && Mipmap)
		{
			// For every mipmap
			for(unsigned int J = 0; J < MipmapCount; J++)
			{
				// Do not change the row size and row count for the last 2 mipmaps
				// Since they wont be compressible, they will have the same data with compressed 4x4 mipmap 
				if (Width >= 4 && Height >= 4)
				{
					// Allocate for the new Mipmap
					RowSize = (Width / InputBlockWidth) * OutputBlockSize;
					RowCount = (Height / InputBlockHeight); 
				}
				TextureData->AllocateMipmap(I, J, RowSize, RowCount);

				DestData = TextureData->GetMipmapData(I, J);
				DestPitch = TextureData->GetMipmapRowSize(I, J);

				ZETextureTools::CompressTexture(DestData, DestPitch, Buffer, BufferPitch, Width, Height, FinalOptions);
				if (Width >= 4 && Height >= 4)
				{
					ZETextureTools::DownSample2x(Buffer, BufferPitch, Buffer, BufferPitch, Width, Height, false);
					Width /= 2;
					Height /= 2;
				}
			}
			
		}
		else if (Compress && !Mipmap)
		{
			// For every mipmap
			for(unsigned int J = 0; J < MipmapCount; J++)
			{
				// Allocate for the new Mipmap
				RowSize = (Width / InputBlockWidth) * OutputBlockSize;
				RowCount = (Height / InputBlockHeight); 
				TextureData->AllocateMipmap(I, J, RowSize, RowCount);

				DestData = TextureData->GetMipmapData(I, J);
				DestPitch = TextureData->GetMipmapRowSize(I, J);

				// Compress and send the result to destination
				ZETextureTools::CompressTexture(DestData, DestPitch, Buffer, BufferPitch, Width, Height, FinalOptions);
			}
		}
		else if (!Compress && Mipmap)
		{
			// For every mipmap
			for(unsigned int J = 0; J < MipmapCount; J++)
			{
				// Allocate for the new Mipmap
				RowSize = (Width / InputBlockWidth) * OutputBlockSize;
				RowCount = (Height / InputBlockHeight); 
				TextureData->AllocateMipmap(I, J, RowSize, RowCount);
	
				DestData = TextureData->GetMipmapData(I, J);
				DestPitch = TextureData->GetMipmapRowSize(I, J);

				TextureData->CopyMipmapDataFrom(I, J, Buffer, BufferPitch);
				ZETextureTools::DownSample2x(Buffer, BufferPitch, Buffer, BufferPitch, Width, Height);
				Width /= 2;
				Height /= 2;
			}
		}
		else if (!Compress && !Mipmap)
		{
			// For every mipmap
			for(unsigned int J = 0; J < MipmapCount; J++)
			{
				// Allocate for the new Mipmap
				RowSize = (Width / InputBlockWidth) * OutputBlockSize;
				RowCount = (Height / InputBlockHeight); 
				TextureData->AllocateMipmap(I, J, RowSize, RowCount);

				DestData = TextureData->GetMipmapData(I, J);
				DestPitch = TextureData->GetMipmapRowSize(I, J);

				TextureData->CopyMipmapDataFrom(I, J, Buffer, BufferPitch);
			}
		}
	}
	
	free(Buffer);
	return true;
}

bool ZETextureQualityManager::GetFinalTextureOptions(ZETextureOptions* FinalOptions, ZEFile* ResourceFile, const ZETextureOptions* UserOptions)
{
	if(UserOptions == NULL || FinalOptions == NULL)
		return false;

	// Only for test purposes
	ZETextureOptions *DefaultOptions = zeGraphics->GetTextureOptions();
	
	// UNCOMMENT ABOVE LINE AND DELETE BELOW LINE AFTER TEST
	//static ZETextureOptions Low			= {ZE_TCT_DXT3,	ZE_TCQ_NORMAL,	ZE_TDS_2X,		ZE_TFC_ENABLED,		ZE_TMM_ENABLED, 25};
	//ZETextureOptions *DefaultOptions = &Low;

	// Eliminate the auto options
	FinalOptions->CompressionType		= (UserOptions->CompressionType == ZE_TCT_AUTO)		? DefaultOptions->CompressionType		: UserOptions->CompressionType;
	FinalOptions->CompressionQuality	= (UserOptions->CompressionQuality == ZE_TCQ_AUTO)	? DefaultOptions->CompressionQuality	: UserOptions->CompressionQuality;
	FinalOptions->DownSample			= (UserOptions->DownSample == ZE_TDS_AUTO)			? DefaultOptions->DownSample			: UserOptions->DownSample;
	FinalOptions->MipMapping			= (UserOptions->MipMapping == ZE_TMM_AUTO)			? DefaultOptions->MipMapping			: UserOptions->MipMapping;
	FinalOptions->FileCaching			= (UserOptions->FileCaching == ZE_TFC_AUTO)			? DefaultOptions->FileCaching			: UserOptions->FileCaching;

	ZETextureInfo TextureInfo = {0};
	// Tries to get texture info from pack or standalone ZETextureFile
	// If fails tries to get it from the source image file
	if(!ZETextureLoader::GetTextureInfo(&TextureInfo, ResourceFile))
	{
		if(!ZETextureLoader::GetImageInfo(&TextureInfo, ResourceFile))
		{
			// There is no more place to look for
			zeAssert(true, "Cannot get image or texture info. File Path : \"%s\"", ResourceFile->GetFilePath().GetValue());
			return false;
		}
	}

	// Creating texture options constrains
	// Check if it is Resizeable (power of 2)
	if (!ZETextureTools::IsResizeable(TextureInfo.Width, TextureInfo.Height))
	{
		FinalOptions->DownSample = ZE_TDS_NONE;
		FinalOptions->MipMapping = ZE_TMM_DISABLED;
		FinalOptions->MaximumMipmapLevel = 1;
	}

	// Check if it is Compressible (divisible by 4)
	if (!ZETextureTools::IsCompressible(TextureInfo.Width, TextureInfo.Height))
		FinalOptions->CompressionType = ZE_TCT_NONE;

	// If there is MipMapping
	if (FinalOptions->MipMapping != ZE_TMM_DISABLED)
	{
		unsigned int PossibleMaxMipmapNumber = 1;
		unsigned int MipMapCount = ZETextureTools::GetMaxMipmapCount(TextureInfo.Width, TextureInfo.Height);

		// Subtract the mipmap count if there is a downsample before the process
		PossibleMaxMipmapNumber = MipMapCount - (unsigned int)(FinalOptions->DownSample);
		// Set final mipmap count
		FinalOptions->MaximumMipmapLevel = (UserOptions->MaximumMipmapLevel > PossibleMaxMipmapNumber) ? PossibleMaxMipmapNumber : UserOptions->MaximumMipmapLevel;
	}
	else
	{
		FinalOptions->MaximumMipmapLevel = 1;
	}
		

	return true;
}
