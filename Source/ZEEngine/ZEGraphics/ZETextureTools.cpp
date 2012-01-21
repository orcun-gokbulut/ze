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
#include "ZEFile/ZEFile.h"
#include "ZECore/ZEConsole.h"
#include "ZECore/ZEResourceManager.h"
#include "ZEGraphics/ZETextureTools.h"
#include "ZEGraphics/ZETexture2D.h"
#include "ZEGraphics/ZETexture2DResource.h"
#include "zedirect3d9/ZED3D9TextureResizer.h"


ZETextureTools::ZETextureTools()
{
	/* Empty */
}
ZETextureTools::~ZETextureTools()
{
	/* Empty */
}

// Is texture resizeable by 2
bool ZETextureTools::IsResizeable(ZEUInt Width, ZEUInt Height)
{
	return (((Width & (Width - 1)) != 0) || ((Height & (Height - 1)) != 0)) ? false : true;
}

// Is texture compressible by DXT3/BC2
bool ZETextureTools::IsCompressible(ZEUInt Width, ZEUInt Height)
{
	return ((Width % 4 != 0) || (Height % 4 != 0)) ? false : true;
}

ZEUInt ZETextureTools::GetMaxMipmapCount(ZEUInt Width, ZEUInt Height)
{
	ZEUInt WidthCount = (ZEUInt)(ZEMath::Log((float)Width) / ZEMath::Log(2.0f));
	ZEUInt HeightCount = (ZEUInt)(ZEMath::Log((float)Height) / ZEMath::Log(2.0f));

	/* also counts the original (level 0) texture as a mipmap */
	return WidthCount >= HeightCount ? HeightCount + 1 : WidthCount + 1;
}

void ZETextureTools::CompressTexture(void* DestinationData, ZEUInt DestinationPitch, void* SourceData, ZEUInt	SourcePitch, ZEUInt SourceWidth, ZEUInt SourceHeight, const ZETextureOptions* CompressionOptions)
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

void ZETextureTools::DownSample2x(void* DestinationData, ZEUInt DestinationPitch, void* SourceData, ZEUInt SourcePitch, ZEUInt SourceWidth, ZEUInt SourceHeight, bool UseGpu)
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

		for (ZESize y = 0; y < DestinationHeight; y++)
		{
			for (ZESize x = 0; x < DestinationWidth; x++)
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

// Can be used to copy a image buffer to another
// Can be used for getting a portion of a image ex: a face of cube map
void ZETextureTools::CopyTextureRegion(void* DestData, ZEUInt DestPitch, ZEUInt DestX, ZEUInt DestY, void *SourceData, ZEUInt SourcePitch, ZEUInt SourceBitsPP, ZEUInt SourceX, ZEUInt SourceY, ZEUInt CopyWidth, ZEUInt CopyHeight)
{
	ZEUInt SourceBytesPP = SourceBitsPP / 8;

	BYTE* DestRegion	= (BYTE*)DestData + DestPitch * DestY + DestX * SourceBytesPP;
	BYTE* SourceRegion	= (BYTE*)SourceData + SourcePitch * SourceY + SourceX * SourceBytesPP;

	for(ZEUInt I = 0; I < CopyHeight; I++)
	{
		memcpy(DestRegion, SourceRegion, CopyWidth * SourceBytesPP);
		
		DestRegion += DestPitch;
		SourceRegion += SourcePitch;
	}
}

void ZETextureTools::CopyTexture(void* DestData, ZEUInt DestPitch, void* SourceData, ZEUInt SourcePitch, ZEUInt RowSize, ZEUInt RowCount)
{
	for (ZEUInt I = 0; I < RowCount; I++)
	{
		memcpy((unsigned char*)DestData + I * DestPitch, (unsigned char*)SourceData + SourcePitch, RowSize);
	}
}
