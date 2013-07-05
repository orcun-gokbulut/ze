//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureTools.cpp
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
#include "ZECore/ZEConsole.h"
#include "ZECore/ZEResourceManager.h"
#include "ZETextureTools.h"
#include "ZEGraphics/ZETexture2D.h"
#include "ZETexture2DResource.h"
// #include "ZEModules/ZEDirect3D9/ZED3D9TextureResizer.h"

#include <windows.h>
#include <ATI_Compress.h>

ZETextureTools::ZETextureTools()
{
	
}

ZETextureTools::~ZETextureTools()
{
	
}

// Is texture compressible by DXTn/BCn
// Independent of texture type and surface count
bool ZETextureTools::IsCompressible(const ZEUInt Width, const ZEUInt Height, const ZEUInt HorizTileCount, const ZEUInt VertTileCount)
{
	ZEUInt TileWidth = Width / HorizTileCount;
	ZEUInt TileHeight = Height / VertTileCount;

	zeDebugCheck(TileWidth == 0 || TileHeight == 0, "Tile width or tile height is zero.");

	return ((TileWidth % 4 == 0) && (TileHeight % 4 == 0)) ? true : false;
}

// Surface count is used only when texture type is 3D 
bool ZETextureTools::IsResizeable(const ZEUInt Width, const ZEUInt Height, const ZEUInt HorizTileCount, const ZEUInt VertTileCount, const ZETextureType TextureType)
{
	switch (TextureType)
	{
		case ZE_TT_2D:
		{
			if (Width < 2 || Height < 2)
			{
				return false;
			}
			else
			{
				return ((Width & (Width - 1)) == 0) && ((Height & (Height - 1)) == 0) ? true : false;
			}

			break;
		}

		case ZE_TT_CUBE:
		{
			ZEUInt TileWidth = Width / HorizTileCount;
			ZEUInt TileHeight = Height / VertTileCount;

			if (TileWidth < 2 || TileHeight < 2)
			{
				return false;
			}
			else
			{
				return ((TileWidth & (TileWidth - 1)) == 0) && ((TileHeight & (TileHeight - 1)) == 0) ? true : false;
			}
			
			break;
		}
			
		case ZE_TT_3D:
		{
			ZEUInt TileWidth = Width / HorizTileCount;
			ZEUInt TileHeight = Height / VertTileCount;
			ZEUInt SurfaceCount = HorizTileCount * VertTileCount;

			if (TileWidth < 2 || TileHeight < 2 || SurfaceCount < 2)
			{
				return false;
			}
			else
			{
				return (((TileWidth & (TileWidth - 1)) == 0) && ((TileHeight & (TileHeight - 1)) == 0) && ((SurfaceCount & (SurfaceCount - 1)) == 0)) ? true : false;
			}

			break;
		}
			
		default:
		{
			return false;
			break;
		}
	}
}

ZEUInt ZETextureTools::GetMaxMipmapCount(const ZEUInt Width, const ZEUInt Height, const ZEUInt HorizTileCount, const ZEUInt VertTileCount, const ZETextureType TextureType)
{
	if (!IsResizeable(Width, Height, HorizTileCount, VertTileCount, TextureType))
	{
		return 1;
	}
	
	switch (TextureType)
	{
		case ZE_TT_2D:
		{
			float Log2 = ZEMath::Log(2.0f);
			ZEUInt MaxMipX	= (ZEUInt)(ZEMath::Log((float)Width) / Log2);
			ZEUInt MaxMipY	= (ZEUInt)(ZEMath::Log((float)Height) / Log2);
			return ZEMath::Min(MaxMipX, MaxMipY) + 1;
			break;
		}
		case ZE_TT_CUBE:
		{
			ZEUInt TileWidth = Width / HorizTileCount;
			ZEUInt TileHeight = Height / VertTileCount;
			
			float Log2 = ZEMath::Log(2.0f);
			ZEUInt MaxMipX	= (ZEUInt)(ZEMath::Log((float)TileWidth) / Log2);
			ZEUInt MaxMipY	= (ZEUInt)(ZEMath::Log((float)TileHeight) / Log2);
			return ZEMath::Min(MaxMipX, MaxMipY) + 1;
			break;
		}
			
		case ZE_TT_3D:
		{
			ZEUInt TileWidth = Width / HorizTileCount;
			ZEUInt TileHeight = Height / VertTileCount;
			ZEUInt SurfaceCount = HorizTileCount * VertTileCount;

			float Log2 = ZEMath::Log(2.0f);
			ZEUInt MaxMipX	= (ZEUInt)(ZEMath::Log((float)Width) / Log2);
			ZEUInt MaxMipY	= (ZEUInt)(ZEMath::Log((float)Height) / Log2);
			ZEUInt MaxMipZ	= (ZEUInt)(ZEMath::Log((float)SurfaceCount) / Log2);
			return ZEMath::Min(ZEMath::Min(MaxMipX, MaxMipY), MaxMipZ) + 1;
			break;
		}
		
		default:
		{
			return 1;
			break;
		}
	}
}
/*
class ZEOutputHandler : public OutputHandler
{
	private:
		void* Output;
		ZESize Cursor;

		ZESize Size;
		ZEUInt32 Width;
		ZEUInt32 Height;
		ZEUInt32 Depth;
		ZEUInt32 Face;
		ZEUInt32 MipLevel;

	public:
		void SetOutput(void* Output)
		{
			this->Output = Output;
		}
		
		void beginImage(int size, int width, int height, int depth, int face, int miplevel)
		{
			Size = size;
			Width = width;
			Height = height;
			Depth = depth;
			Face = face;
			MipLevel = miplevel;
		}
	
		bool writeData(const void * data, int size)
		{
			if (Output == NULL)
				return false;

			if (Cursor + size == Size)
				int x = 3;

			void* Destination = (void*)((ZEUInt8*)Output + Cursor);
			if (memcpy(Destination, data, size) == Destination)
			{
				Cursor += size;
				return true;
			}

			return false;
		}

		ZEOutputHandler()
		{
			Cursor = 0;
			Output = NULL;
		}
	
		virtual ~ZEOutputHandler()
		{

		}
};

class ZEErrorHandler : public ErrorHandler
{
	public:
		void error(Error e)
		{
			switch(e)
			{
				case Error_Unknown:
					zeWarning("Cannot open file for compression.");
					break;
				case Error_InvalidInput:
					zeWarning("Invalid compression output.");
					break;
				case Error_UnsupportedFeature:
					zeWarning("Unsupported compression feature.");
					break;
				case Error_CudaError:
					zeWarning("Cuda error during compression.");
					break;
				case Error_FileOpen:
					zeWarning("Cannot open file for compression.");
					break;
				case Error_FileWrite:
					zeWarning("Cannot write to file");
					break;
			}
		}
};
*/
void ZETextureTools::Compress(void* DestinationData, const ZESize DestinationPitch, const void* SourceData, const ZESize SourcePitch, const ZEUInt SourceWidth, const ZEUInt SourceHeight, const ZETextureOptions* Options)
{
	/*
	Format TargetFormat;
	Quality TargetQuality;

	// Decide Compression Type
	switch(Options->CompressionType)
	{
		case ZE_TCT_DXT1:
			TargetFormat = Format_DXT1;
			break;

		case ZE_TCT_DXT3:
			TargetFormat = Format_DXT3;
			break;

		case ZE_TCT_DXT5:
			TargetFormat = Format_DXT5;
			break;

		default:
			return;
			break;
	};

	// Decide compression Speed
	switch(Options->CompressionQuality)
	{
		case ZE_TCQ_HIGH:
			TargetQuality = Quality_Highest;
			break;

		
		case ZE_TCQ_NORMAL:
			TargetQuality = Quality_Normal;
			break;

		default:
		case ZE_TCQ_LOW:
			TargetQuality = Quality_Fastest;
			break;
	};

	TargetQuality = Quality_Fastest;

	InputOptions Input;
	Input.reset();
	Input.setMipmapGeneration(false);
	Input.setNormalizeMipmaps(false);
	Input.setConvertToNormalMap(false);
	Input.setAlphaMode(AlphaMode_None);
	Input.setRoundMode(RoundMode_None);
	Input.setWrapMode(WrapMode_Mirror);
	Input.setFormat(InputFormat_BGRA_8UB);
	Input.setColorTransform(ColorTransform_None);
	Input.setTextureLayout(TextureType_2D, SourceWidth, SourceHeight, 1);
	Input.setMipmapData(SourceData, SourceWidth, SourceHeight, 1, 0, 0);
	
	ZEErrorHandler ErrHandler;

	ZEOutputHandler	OutHandler;
	OutHandler.SetOutput(DestinationData);
	
	OutputOptions Output;
	Output.reset();
	Output.setOutputHeader(false);
	Output.setErrorHandler(&ErrHandler);
	Output.setOutputHandler(&OutHandler);
	
	CompressionOptions CompOptions;
	CompOptions.reset();
	CompOptions.setFormat(TargetFormat);
	CompOptions.setQuality(TargetQuality);
	CompOptions.setColorWeights(1.0f, 1.0f, 1.0f, 1.0f);
	CompOptions.setPixelFormat(32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);

	Compressor Comp;
	Comp.enableCudaAcceleration(false);

	bool Result = Comp.process(Input, CompOptions, Output);
	*/


	ATI_TC_FORMAT Format = ATI_TC_FORMAT_Unknown;
	ATI_TC_Speed Speed = ATI_TC_Speed_SuperFast;

	// Decide Compression Type
	switch(Options->CompressionType)
	{
		case ZE_TCT_DXT1:
			Format = ATI_TC_FORMAT_DXT1;
			break;

		case ZE_TCT_DXT3:
			Format = ATI_TC_FORMAT_DXT3;
			break;

		case ZE_TCT_DXT5:
			Format = ATI_TC_FORMAT_DXT5;
			break;

		case ZE_TCT_3DC:
			Format = ATI_TC_FORMAT_BC5;
			break;

		default:
			break;
	};

	// Decide compression Speed
	switch(Options->CompressionQuality)
	{
		case ZE_TCQ_HIGH:
			Speed = ATI_TC_Speed_Normal;
			break;

		default:
		case ZE_TCQ_NORMAL:
			Speed = ATI_TC_Speed_Fast;
			break;

		case ZE_TCQ_LOW:
			Speed = ATI_TC_Speed_SuperFast;
			break;
	};

	ATI_TC_Texture srcTexture;
	srcTexture.dwSize = sizeof(srcTexture);
	srcTexture.dwWidth = SourceWidth;
	srcTexture.dwHeight	= SourceHeight;
	srcTexture.dwPitch = (ATI_TC_DWORD)SourcePitch;
	srcTexture.format = ATI_TC_FORMAT_ARGB_8888;
	srcTexture.pData = (ATI_TC_BYTE*)SourceData;
	srcTexture.dwDataSize = ATI_TC_CalculateBufferSize(&srcTexture);

	ATI_TC_Texture destTexture;  
	destTexture.dwSize = sizeof(destTexture);
	destTexture.dwWidth	= SourceWidth;
	destTexture.dwHeight = SourceHeight;
	destTexture.dwPitch	= (ATI_TC_DWORD)DestinationPitch;
	destTexture.format = Format;
	destTexture.pData = (ATI_TC_BYTE*)DestinationData;
	destTexture.dwDataSize = ATI_TC_CalculateBufferSize(&destTexture);

	ATI_TC_CompressOptions options;
	memset(&options, 0, sizeof(options));

	options.dwSize = sizeof(options);
	options.nCompressionSpeed = Speed;

	ATI_TC_ConvertTexture(&srcTexture, &destTexture, &options, NULL, NULL, NULL);
}

void ZETextureTools::DownSample2x(void* DestinationData, const ZESize DestinationPitch, const void* SourceData, const ZESize SourcePitch, const ZEUInt SourceWidth, const ZEUInt SourceHeight)
{
	// Check width height
	if(SourceWidth <= 1 || SourceHeight <= 1)
		return;

	ZEUInt DestinationHeight = SourceHeight / 2;
	ZEUInt DestinationWidth = SourceWidth / 2;

	struct ZEColorRGBA
	{
		ZEUInt8 Red;
		ZEUInt8 Green;
		ZEUInt8 Blue;
		ZEUInt8 Alpha;
	};

	for (ZESize y = 0; y < (ZESize)DestinationHeight; y++)
	{
		for (ZESize x = 0; x < (ZESize)DestinationWidth; x++)
		{
			ZEColorRGBA* Source = (ZEColorRGBA*)((ZEUInt8*)SourceData + SourcePitch * y * 2 + x * 8);

			ZEUInt16 Red, Green, Blue, Alpha;
			Alpha = Source->Alpha;
			Red   = Source->Red;
			Green = Source->Green;
			Blue  = Source->Blue;
			Source++;

			Alpha += Source->Alpha;
			Red   += Source->Red;
			Green += Source->Green;
			Blue  += Source->Blue;

			Source = (ZEColorRGBA*)((ZEUInt8*)Source + SourcePitch - 1 * 4);
			Alpha += Source->Alpha;
			Red   += Source->Red;
			Green += Source->Green;
			Blue  += Source->Blue;
			Source++;

			Alpha += Source->Alpha;
			Red   += Source->Red;
			Green += Source->Green;
			Blue  += Source->Blue;

			ZEColorRGBA* Destination = (ZEColorRGBA*)((ZEUInt8*)DestinationData + DestinationPitch * y + x * 4);
			Destination->Alpha = Alpha / 4;
			Destination->Red   = Red   / 4;
			Destination->Green = Green / 4;
			Destination->Blue  = Blue  / 4;
		}
	}
}

// Takes average of two uncompressed image which have same dimensions
void ZETextureTools::Average(void* DestinationData, const ZESize DestinationPitch, const void* SourceData1, const ZESize SourcePitch1, const ZEUInt SourceWidth1, const ZEUInt SourceHeight1, const void* SourceData2, const ZESize SourcePitch2, const ZEUInt SourceWidth2, const ZEUInt SourceHeight2)
{
	if (SourceHeight1 != SourceHeight2 || SourceWidth1 != SourceWidth2)
	{
		zeError("Cannot calculate average of two image with different dimensions");
		return;
	}

	struct ZEColorRGBA
	{
		ZEUInt8 Red;
		ZEUInt8 Green;
		ZEUInt8 Blue;
		ZEUInt8 Alpha;
	};

	for (ZESize y = 0; y < (ZESize)SourceHeight1; y++)
	{
		for (ZESize x = 0; x < (ZESize)SourceWidth1; x++)
		{
			ZEColorRGBA* Source1 = (ZEColorRGBA*)((ZEUInt8*)SourceData1 + SourcePitch1 * y + x * 4);
			ZEColorRGBA* Source2 = (ZEColorRGBA*)((ZEUInt8*)SourceData2 + SourcePitch2 * y + x * 4);
			ZEColorRGBA* Destination = (ZEColorRGBA*)((ZEUInt8*)DestinationData + DestinationPitch * y + x * 4);

			Destination->Alpha	= (Source1->Alpha + Source2->Alpha) / 2;
			Destination->Green	= (Source1->Green + Source2->Green) / 2;
			Destination->Blue	= (Source1->Blue + Source2->Blue) / 2;
			Destination->Red	= (Source1->Red + Source2->Red) / 2;
		}
	}
}
