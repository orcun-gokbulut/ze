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
#include "ZEGraphics/ZETextureTools.h"
#include "ZEGraphics/ZETexture2D.h"
#include "ZEGraphics/ZETexture2DResource.h"
#include "ZEModules/ZEDirect3D9/ZED3D9TextureResizer.h"

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

	zeAssert(TileWidth == 0 || TileHeight == 0, "Tile width or tile height is zero.");

	return ((TileWidth % 4 == 0) || (TileHeight % 4 == 0)) ? true : false;
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

// Surface count is used only when texture type is 3D 
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
			ZEUInt MaxMipX	= (ZEUInt)(ZEMath::Log((float)Width) / ZEMath::Log(2.0f));
			ZEUInt MaxMipY	= (ZEUInt)(ZEMath::Log((float)Height) / ZEMath::Log(2.0f));
			return ZEMath::Min(MaxMipX, MaxMipY) + 1;
			break;
		}
		case ZE_TT_CUBE:
		{
			ZEUInt TileWidth = Width / HorizTileCount;
			ZEUInt TileHeight = Height / VertTileCount;
			
			ZEUInt MaxMipX	= (ZEUInt)(ZEMath::Log((float)TileWidth) / ZEMath::Log(2.0f));
			ZEUInt MaxMipY	= (ZEUInt)(ZEMath::Log((float)TileHeight) / ZEMath::Log(2.0f));
			return ZEMath::Min(MaxMipX, MaxMipY) + 1;
			break;
		}
			
		case ZE_TT_3D:
		{
			ZEUInt TileWidth = Width / HorizTileCount;
			ZEUInt TileHeight = Height / VertTileCount;
			ZEUInt SurfaceCount = HorizTileCount * VertTileCount;

			ZEUInt MaxMipX	= (ZEUInt)(ZEMath::Log((float)Width) / ZEMath::Log(2.0f));
			ZEUInt MaxMipY	= (ZEUInt)(ZEMath::Log((float)Height) / ZEMath::Log(2.0f));
			ZEUInt MaxMipZ	= (ZEUInt)(ZEMath::Log((float)SurfaceCount) / ZEMath::Log(2.0f));
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

void ZETextureTools::CompressTexture(void* DestinationData, const ZESize DestinationPitch, const void* SourceData, const ZESize SourcePitch, const ZEUInt SourceWidth, const ZEUInt SourceHeight, const ZETextureOptions* CompressionOptions)
{
	ATI_TC_FORMAT	Format;
	ATI_TC_Speed	Speed;

	// Decide Compression Type
	switch(CompressionOptions->CompressionType)
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
	switch(CompressionOptions->CompressionQuality)
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
	srcTexture.dwSize		= sizeof(srcTexture);
	srcTexture.dwWidth		= SourceWidth;
	srcTexture.dwHeight		= SourceHeight;
	srcTexture.dwPitch		= (ATI_TC_DWORD)SourcePitch;
	srcTexture.format		= ATI_TC_FORMAT_ARGB_8888;
	srcTexture.pData		= (ATI_TC_BYTE*)SourceData;
	srcTexture.dwDataSize	= ATI_TC_CalculateBufferSize(&srcTexture);

	ATI_TC_Texture destTexture;  
	destTexture.dwSize		= sizeof(destTexture);
	destTexture.dwWidth		= SourceWidth;
	destTexture.dwHeight	= SourceHeight;
	destTexture.dwPitch		= (ATI_TC_DWORD)DestinationPitch;
	destTexture.format		= Format;
	destTexture.pData		= (ATI_TC_BYTE*)DestinationData;
	destTexture.dwDataSize	= ATI_TC_CalculateBufferSize(&destTexture);

	ATI_TC_CompressOptions options;
	memset(&options, 0, sizeof(options));

	options.dwSize				= sizeof(options);
	options.nCompressionSpeed	= Speed;

	ATI_TC_ConvertTexture(&srcTexture, &destTexture, &options, NULL, NULL, NULL);
}

void ZETextureTools::DownSample2x(void* DestinationData, const ZESize DestinationPitch, const void* SourceData, const ZESize SourcePitch, const ZEUInt SourceWidth, const ZEUInt SourceHeight, bool UseGpu)
{
	// Check width height
	if(SourceWidth <= 1 || SourceHeight <= 1)
		return;

	ZEUInt DestinationHeight = SourceHeight / 2;
	ZEUInt DestinationWidth = SourceWidth / 2;

	if (UseGpu)
	{
		static ZED3D9TextureResizer Resizer;

		Resizer.Initialize(DestinationData, DestinationPitch, DestinationWidth, DestinationHeight, SourceData, SourcePitch, SourceWidth, SourceHeight);
		Resizer.Process();
		Resizer.Deinitialize();

	}
	else
	{
		struct ZEColorARGB
		{
			ZEUInt8 Alpha;
			ZEUInt8 Red;
			ZEUInt8 Blue;
			ZEUInt8 Green;
		};

		for (ZESize y = 0; y < (ZESize)DestinationHeight; y++)
		{
			for (ZESize x = 0; x < (ZESize)DestinationWidth; x++)
			{
				ZEColorARGB* Source = (ZEColorARGB*)((ZEUInt8*)SourceData + SourcePitch * y * 2 + x * 8);

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

				Source = (ZEColorARGB*)((ZEUInt8*)Source + SourcePitch - 1 * 4);
				Alpha += Source->Alpha;
				Red   += Source->Red;
				Green += Source->Green;
				Blue  += Source->Blue;
				Source++;

				Alpha += Source->Alpha;
				Red   += Source->Red;
				Green += Source->Green;
				Blue  += Source->Blue;

				ZEColorARGB* Destination = (ZEColorARGB*)((ZEUInt8*)DestinationData + DestinationPitch * y + x * 4);
				Destination->Alpha = Alpha / 4;
				Destination->Red   = Red   / 4;
				Destination->Green = Green / 4;
				Destination->Blue  = Blue  / 4;
			}
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

	struct ZEColorARGB
	{
		ZEUInt8 Alpha;
		ZEUInt8 Red;
		ZEUInt8 Blue;
		ZEUInt8 Green;
	};

	for (ZESize y = 0; y < (ZESize)SourceHeight1; y++)
	{
		for (ZESize x = 0; x < (ZESize)SourceWidth1; x++)
		{
			ZEColorARGB* Source1 = (ZEColorARGB*)((ZEUInt8*)SourceData1 + SourcePitch1 * y + x * 4);
			ZEColorARGB* Source2 = (ZEColorARGB*)((ZEUInt8*)SourceData2 + SourcePitch2 * y + x * 4);
			ZEColorARGB* Destination = (ZEColorARGB*)((ZEUInt8*)DestinationData + DestinationPitch * y + x * 4);

			Destination->Alpha	= (Source1->Alpha + Source2->Alpha) / 2;
			Destination->Green	= (Source1->Green + Source2->Green) / 2;
			Destination->Blue	= (Source1->Blue + Source2->Blue) / 2;
			Destination->Red	= (Source1->Red + Source2->Red) / 2;
		}
	}
}
