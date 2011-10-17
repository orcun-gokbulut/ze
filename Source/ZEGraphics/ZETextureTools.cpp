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

#include <windows.h>
#include <ATI_Compress.h>



#include "ZEError.h"
#include "ZECore/ZEConsole.h"
#include "ZECore/ZEResourceManager.h"
#include "ZEFile/ZEResourceFile.h"
#include "ZEGraphics/ZETextureTools.h"
#include "ZEGraphics/ZETexture2D.h"
#include "ZEGraphics/ZETexture2DResource.h"



ZETextureTools::ZETextureTools(void)
{
	/* Empty */
}
ZETextureTools::~ZETextureTools(void)
{
	/* Empty */
}


unsigned int ZETextureTools::GetMaxMipmapCount(ZETextureLoaderInfo &TextureInfo)
{
	unsigned int WidthCount = (unsigned int)(logf((float)TextureInfo.TextureWidth) / logf(2.0f));
	unsigned int HeightCount = (unsigned int)(logf((float)TextureInfo.TextureHeight) / logf(2.0f));

	/* also counts the original (level 0) texture as a mipmap */
	return WidthCount >= HeightCount ? HeightCount + 1 : WidthCount + 1;
}

void ZETextureTools::CompressTexture(void* DestinationData, unsigned int DestinationPitch, void* SourceData, unsigned int	SourcePitch, unsigned int SourceWidth, unsigned int SourceHeight, const ZETextureOptions* CompressionOptions)
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
	srcTexture.dwSize = sizeof(srcTexture);
	srcTexture.dwWidth = SourceWidth;
	srcTexture.dwHeight = SourceHeight;
	srcTexture.dwPitch = SourcePitch;
	srcTexture.format = ATI_TC_FORMAT_ARGB_8888;
	srcTexture.pData = (ATI_TC_BYTE*)SourceData;
	srcTexture.dwDataSize = ATI_TC_CalculateBufferSize(&srcTexture);

	ATI_TC_Texture destTexture;  
	destTexture.dwSize = sizeof(destTexture);
	destTexture.dwWidth = SourceWidth;
	destTexture.dwHeight = SourceHeight;
	destTexture.dwPitch = DestinationPitch;
	destTexture.format = Format;
	destTexture.pData = (ATI_TC_BYTE*)DestinationData;
	destTexture.dwDataSize = ATI_TC_CalculateBufferSize(&destTexture);

	ATI_TC_CompressOptions options;
	memset(&options, 0, sizeof(options));
	options.dwSize = sizeof(options);
	options.nCompressionSpeed = Speed;

	ATI_TC_ConvertTexture(&srcTexture, &destTexture, &options, NULL, NULL, NULL);
}

void ZETextureTools::DownSample2x(void* DestinationData, unsigned int DestinationPitch, void* SourceData, unsigned int SourcePitch, unsigned int SourceWidth, unsigned int SourceHeight)
{
	struct ZEColorARGB
	{
		ZEBYTE Alpha;
		ZEBYTE Red;
		ZEBYTE Blue;
		ZEBYTE Green;
	};

	if(SourceWidth < 1 || SourceHeight < 1)
		return;

	unsigned int DestinationHeight = SourceHeight / 2;
	unsigned int DestinationWidth = SourceWidth / 2;

	for (size_t y = 0; y < DestinationHeight; y++)
	{
		for (size_t x = 0; x < DestinationWidth; x++)
		{
			ZEColorARGB* Source = (ZEColorARGB*)((ZEBYTE*)SourceData + SourcePitch * y * 2 + x * 8);

			ZEWORD Red, Green, Blue, Alpha;
			Alpha = Source->Alpha;
			Red   = Source->Red;
			Green = Source->Green;
			Blue  = Source->Blue;
			Source++;

			Alpha += Source->Alpha;
			Red   += Source->Red;
			Green += Source->Green;
			Blue  += Source->Blue;

			Source = (ZEColorARGB*)((ZEBYTE*)Source + SourcePitch - 1 * 4);
			Alpha += Source->Alpha;
			Red   += Source->Red;
			Green += Source->Green;
			Blue  += Source->Blue;
			Source++;

			Alpha += Source->Alpha;
			Red   += Source->Red;
			Green += Source->Green;
			Blue  += Source->Blue;

			ZEColorARGB* Destination = (ZEColorARGB*)((ZEBYTE*)DestinationData + DestinationPitch * y + x * 4);
			Destination->Alpha = Alpha / 4;
			Destination->Red   = Red   / 4;
			Destination->Green = Green / 4;
			Destination->Blue  = Blue  / 4;
		}
	}
}

// Can be used to copy a image buffer to another
// Can be used for getting a portion of a image ex: a face of cube map
void ZETextureTools::CopyTextureRegion(void *DestData, unsigned int DestPitch, unsigned int DestX, unsigned int DestY, void *SourceData, unsigned int SourcePitch, unsigned int SourceBitsPP, unsigned int SourceX, unsigned int SourceY, unsigned int CopyWidth, unsigned int CopyHeight)
{
	unsigned int SourceBytesPP = SourceBitsPP / 8;

	BYTE *DestRegion	= (BYTE*)DestData + DestPitch * DestY + DestX * SourceBitsPP;
	BYTE *SourceRegion	= (BYTE*)SourceData + SourcePitch * SourceY + SourceX * SourceBytesPP;

	for(unsigned int I = 0; I < CopyHeight; I++)
	{
		memcpy(DestRegion, SourceRegion, CopyWidth * SourceBytesPP);
		SourceRegion += SourcePitch;
		DestRegion += DestPitch;
	}
}

// IS texture resizeable by 2
bool ZETextureTools::IsResizeable(ZETextureLoaderInfo &TextureInfo)
{
	return (((TextureInfo.TextureWidth & (TextureInfo.TextureWidth - 1)) != 0) || ((TextureInfo.TextureHeight & (TextureInfo.TextureHeight - 1)) != 0)) ? false : true;
}
// IS texture compressible by DXT3/BC2
bool ZETextureTools::IsCompressible(ZETextureLoaderInfo &TextureInfo)
{
	return ((TextureInfo.TextureWidth % 4 != 0) || (TextureInfo.TextureHeight % 4 != 0)) ? false : true;
}

