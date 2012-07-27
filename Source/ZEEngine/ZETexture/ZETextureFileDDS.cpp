//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureFileDDS.cpp
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

#include "ZETextureFileDDS.h"

#include "ZEError.h"
#include "ZEPacking.h"
#include "ZEEndian.h"
#include "ZETextureData.h"
#include "ZEDS/ZEPointer.h"
#include "ZEFile/ZEFile.h"
#include "ZETexturePixelConverter.h"

#define ZE_DDS_MAGIC_NUMBER				542327876
#define ZE_DDSCAPS2_CUBEMAP				0x200
#define ZE_DDSCAPS2_CUBEMAP_POSITIVEX 	0x400
#define ZE_DDSCAPS2_CUBEMAP_NEGATIVEX 	0x800
#define ZE_DDSCAPS2_CUBEMAP_POSITIVEY 	0x1000
#define ZE_DDSCAPS2_CUBEMAP_NEGATIVEY 	0x2000
#define ZE_DDSCAPS2_CUBEMAP_POSITIVEZ 	0x4000
#define ZE_DDSCAPS2_CUBEMAP_NEGATIVEZ 	0x8000

enum ZEDDSEmulatedFormat
{
	ZE_DEF_A8R8G8B8,
	ZE_DEF_X8R8G8B8,
	ZE_DEF_X1R5G5B5,
	ZE_DEF_R8G8B8,
	ZE_DEF_R5G6B5,
	ZE_DEF_A8L8,
	ZE_DEF_A1R5G5B5,
	ZE_DEF_NOT_EMULATED
};

struct ZE_DDS_PIXELFORMAT
{
  ZELittleEndian<ZEUInt32> Size;
  ZELittleEndian<ZEUInt32> Flags;
  ZELittleEndian<ZEUInt32> FourCC;
  ZELittleEndian<ZEUInt32> RGBBitCount;
  ZELittleEndian<ZEUInt32> RBitMask;
  ZELittleEndian<ZEUInt32> GBitMask;
  ZELittleEndian<ZEUInt32> BBitMask;
  ZELittleEndian<ZEUInt32> ABitMask;
};

struct ZE_DDS_HEADER
{
  ZELittleEndian<ZEUInt32>				Size;
  ZELittleEndian<ZEUInt32>				Flags;
  ZELittleEndian<ZEUInt32>				Height;
  ZELittleEndian<ZEUInt32>				Width;
  ZELittleEndian<ZEUInt32>				PitchOrLinearSize;
  ZELittleEndian<ZEUInt32>				Depth;
  ZELittleEndian<ZEUInt32>				MipMapCount;
  ZELittleEndian<ZEUInt32>				Reserved1[11];
  ZE_DDS_PIXELFORMAT					PixelFormat;
  ZELittleEndian<ZEUInt32>				Caps;
  ZELittleEndian<ZEUInt32>				Caps2;
  ZELittleEndian<ZEUInt32>				Caps3;
  ZELittleEndian<ZEUInt32>				Caps4;
  ZELittleEndian<ZEUInt32>				Reserved2;
};

static ZETexturePixelFormat GetPixelFormat(ZE_DDS_PIXELFORMAT PixelFormat, ZEDDSEmulatedFormat* EmulatedFormat)
{
	if (PixelFormat.BBitMask == 0 &&
		PixelFormat.Flags == 131072 &&
		PixelFormat.FourCC == 0 &&
		PixelFormat.GBitMask == 0 &&
		PixelFormat.RBitMask == 255 &&
		PixelFormat.ABitMask == 0 &&
		PixelFormat.RGBBitCount == 8 &&
		PixelFormat.Size == 32)
	{
		*EmulatedFormat = ZE_DEF_NOT_EMULATED;
		return ZE_TPF_L8;
	}

	if (PixelFormat.BBitMask == 0 &&
		PixelFormat.Flags == 131072 &&
		PixelFormat.FourCC == 0 &&
		PixelFormat.GBitMask == 0 &&
		PixelFormat.RBitMask == 65535 &&
		PixelFormat.ABitMask == 0 &&
		PixelFormat.RGBBitCount == 16 &&
		PixelFormat.Size == 32)
	{
		*EmulatedFormat = ZE_DEF_NOT_EMULATED;
		return ZE_TPF_L16;
	}

	if (PixelFormat.BBitMask == 0 &&
		PixelFormat.Flags == 4 &&
		PixelFormat.FourCC == 111 &&
		PixelFormat.GBitMask == 0 &&
		PixelFormat.RBitMask == 0 &&
		PixelFormat.ABitMask == 0 &&
		PixelFormat.RGBBitCount == 0 &&
		PixelFormat.Size == 32)
	{
		*EmulatedFormat = ZE_DEF_NOT_EMULATED;
		return ZE_TPF_R16F;
	}

	if (PixelFormat.BBitMask == 0 &&
		PixelFormat.Flags == 4 &&
		PixelFormat.FourCC == 114 &&
		PixelFormat.GBitMask == 0 &&
		PixelFormat.RBitMask == 0 &&
		PixelFormat.ABitMask == 0 &&
		PixelFormat.RGBBitCount == 0 &&
		PixelFormat.Size == 32)
	{
		*EmulatedFormat = ZE_DEF_NOT_EMULATED;
		return ZE_TPF_R32F;
	}

	if (PixelFormat.BBitMask == 16711680 &&
		(PixelFormat.Flags == 65 || PixelFormat.Flags == 64) &&
		PixelFormat.FourCC == 0 &&
		PixelFormat.GBitMask == 65280 &&
		PixelFormat.RBitMask == 255 &&
		(PixelFormat.ABitMask == 4278190080 || PixelFormat.ABitMask == 0) &&
		PixelFormat.RGBBitCount == 32 &&
		PixelFormat.Size == 32)
	{
		*EmulatedFormat = ZE_DEF_NOT_EMULATED;
		return ZE_TPF_RGBA8;
	}

	if (PixelFormat.BBitMask == 0 &&
		PixelFormat.Flags == 4 &&
		PixelFormat.FourCC == 113 &&
		PixelFormat.GBitMask == 0 &&
		PixelFormat.RBitMask == 0 &&
		PixelFormat.ABitMask == 0 &&
		PixelFormat.RGBBitCount == 0 &&
		PixelFormat.Size == 32)
	{
		*EmulatedFormat = ZE_DEF_NOT_EMULATED;
		return ZE_TPF_RGBA16F;
	}

	if (PixelFormat.BBitMask == 0 &&
		PixelFormat.Flags == 4 &&
		PixelFormat.FourCC == 116 &&
		PixelFormat.GBitMask == 0 &&
		PixelFormat.RBitMask == 0 &&
		PixelFormat.ABitMask == 0 &&
		PixelFormat.RGBBitCount == 0 &&
		PixelFormat.Size == 32)
	{
		*EmulatedFormat = ZE_DEF_NOT_EMULATED;
		return ZE_TPF_RGBA32F;
	}

	if (PixelFormat.BBitMask == 0 &&
		PixelFormat.Flags == 4 &&
		PixelFormat.FourCC == 827611204 &&
		PixelFormat.GBitMask == 0 &&
		PixelFormat.RBitMask == 0 &&
		PixelFormat.ABitMask == 0 &&
		PixelFormat.RGBBitCount == 0 &&
		PixelFormat.Size == 32)
	{
		*EmulatedFormat = ZE_DEF_NOT_EMULATED;
		return ZE_TPF_DXT1;
	}

	if (PixelFormat.BBitMask == 0 &&
		PixelFormat.Flags == 4 &&
		(PixelFormat.FourCC == 861165636 || PixelFormat.FourCC == 844388420) &&
		PixelFormat.GBitMask == 0 &&
		PixelFormat.RBitMask == 0 &&
		PixelFormat.ABitMask == 0 &&
		PixelFormat.RGBBitCount == 0 &&
		PixelFormat.Size == 32)
	{
		*EmulatedFormat = ZE_DEF_NOT_EMULATED;
		return ZE_TPF_DXT3;
	}

	if (PixelFormat.BBitMask == 0 &&
		PixelFormat.Flags == 4 &&
		(PixelFormat.FourCC == 894720068 || PixelFormat.FourCC == 877942852) &&
		PixelFormat.GBitMask == 0 &&
		PixelFormat.RBitMask == 0 &&
		PixelFormat.ABitMask == 0 &&
		PixelFormat.Size == 32)
	{
		*EmulatedFormat = ZE_DEF_NOT_EMULATED;
		return ZE_TPF_DXT5;
	}

	if (PixelFormat.BBitMask == 0 &&
		PixelFormat.Flags == 64 &&
		PixelFormat.FourCC == 0 &&
		PixelFormat.GBitMask == -65536 &&
		PixelFormat.RBitMask == 65535 &&
		PixelFormat.ABitMask == 0 &&
		PixelFormat.RGBBitCount == 32 &&
		PixelFormat.Size == 32)
	{
		*EmulatedFormat = ZE_DEF_NOT_EMULATED;
		return ZE_TPF_LA16;
	}

	if (PixelFormat.BBitMask == 0 &&
		PixelFormat.Flags == 4 &&
		PixelFormat.FourCC == 112 &&
		PixelFormat.GBitMask == 0 &&
		PixelFormat.RBitMask == 0 &&
		PixelFormat.ABitMask == 0 &&
		PixelFormat.RGBBitCount == 0 &&
		PixelFormat.Size == 32)
	{
		*EmulatedFormat = ZE_DEF_NOT_EMULATED;
		return ZE_TPF_RG16F;
	}

	if (PixelFormat.BBitMask == 0 &&
		PixelFormat.Flags == 4 &&
		PixelFormat.FourCC == 115 &&
		PixelFormat.GBitMask == 0 &&
		PixelFormat.RBitMask == 0 &&
		PixelFormat.ABitMask == 0 &&
		PixelFormat.RGBBitCount == 0 &&
		PixelFormat.Size == 32)
	{
		*EmulatedFormat = ZE_DEF_NOT_EMULATED;
		return ZE_TPF_RG32F;
	}
	
	if (PixelFormat.BBitMask == 31 &&
		PixelFormat.Flags == 65 &&
		PixelFormat.FourCC == 0 &&
		PixelFormat.GBitMask == 992 &&
		PixelFormat.RBitMask == 31744 &&
		PixelFormat.ABitMask == 32768 &&
		PixelFormat.RGBBitCount == 16 &&
		PixelFormat.Size == 32)
	{
		*EmulatedFormat = ZE_DEF_A1R5G5B5;
		return ZE_TPF_RGBA8;
	}

	if (PixelFormat.BBitMask == 0 &&
		PixelFormat.Flags == 131073 &&
		PixelFormat.FourCC == 0 &&
		PixelFormat.GBitMask == 0 &&
		PixelFormat.RBitMask == 255 &&
		PixelFormat.ABitMask == 65280 &&
		PixelFormat.RGBBitCount == 16 &&
		PixelFormat.Size == 32)
	{
		*EmulatedFormat = ZE_DEF_A8L8;
		return ZE_TPF_LA8;
	}

	if (PixelFormat.BBitMask == 31 &&
		PixelFormat.Flags == 64 &&
		PixelFormat.FourCC == 0 &&
		PixelFormat.GBitMask == 2016 &&
		PixelFormat.RBitMask == 63488 &&
		PixelFormat.ABitMask == 0 &&
		PixelFormat.RGBBitCount == 16 &&
		PixelFormat.Size == 32)
	{
		*EmulatedFormat = ZE_DEF_R5G6B5;
		return ZE_TPF_RGBA8;
	}

	if (PixelFormat.BBitMask == 255 &&
		PixelFormat.Flags == 64 &&
		PixelFormat.FourCC == 0 &&
		PixelFormat.GBitMask == 65280 &&
		PixelFormat.RBitMask == 16711680 &&
		PixelFormat.ABitMask == 0 &&
		PixelFormat.RGBBitCount == 24 &&
		PixelFormat.Size == 32)
	{
		*EmulatedFormat = ZE_DEF_R8G8B8;
		return ZE_TPF_RGBA8;
	}

	if (PixelFormat.BBitMask == 31 &&
		PixelFormat.Flags == 64 &&
		PixelFormat.FourCC == 0 &&
		PixelFormat.GBitMask == 992 &&
		PixelFormat.RBitMask == 31744 &&
		PixelFormat.ABitMask == 0 &&
		PixelFormat.RGBBitCount == 16 &&
		PixelFormat.Size == 32)
	{
		*EmulatedFormat = ZE_DEF_X1R5G5B5;
		return ZE_TPF_RGBA8;
	}

	if (PixelFormat.BBitMask == 255 &&
		PixelFormat.Flags == 64 &&
		PixelFormat.FourCC == 0 &&
		PixelFormat.GBitMask == 65280 &&
		PixelFormat.RBitMask == 16711680 &&
		PixelFormat.ABitMask == 0 &&
		PixelFormat.RGBBitCount == 32 &&
		PixelFormat.Size == 32)
	{
		*EmulatedFormat = ZE_DEF_X8R8G8B8;
		return ZE_TPF_RGBA8;
	}

	if (PixelFormat.BBitMask == 255 &&
		PixelFormat.Flags == 65 &&
		PixelFormat.FourCC == 0 &&
		PixelFormat.GBitMask == 65280 &&
		PixelFormat.RBitMask == 16711680 &&
		PixelFormat.ABitMask == 4278190080 &&
		PixelFormat.RGBBitCount == 32 &&
		PixelFormat.Size == 32)
	{
		*EmulatedFormat = ZE_DEF_A8R8G8B8;
		return ZE_TPF_RGBA8;
	}
	zeError("Unsuppored DDS file pixel format.");
	return ZE_TPF_NOTSET;
}

static ZESize GetTextureSize( ZETextureDataInfo* Info )
{
	switch (Info->PixelFormat)
	{
		case ZE_TPF_I8:
			return Info->Height * Info->Width;
		case ZE_TPF_I8_4:
			return Info->Height * Info->Width * 4;
		case ZE_TPF_I16:
			return Info->Height * Info->Width * 2;
		case ZE_TPF_I16_2:
			return Info->Height * Info->Width * 4;
		case ZE_TPF_F16:
			return Info->Height * Info->Width * 2;
		case ZE_TPF_F16_2:
			return Info->Height * Info->Width * 4;
		case ZE_TPF_F16_4:
			return Info->Height * Info->Width * 8;
		case ZE_TPF_F32:
			return Info->Height * Info->Width * 4;
		case ZE_TPF_F32_2:
			return Info->Height * Info->Width * 8;
		case ZE_TPF_F32_4:
			return Info->Height * Info->Width * 16;
		case ZE_TPF_DXT1:
			return ZEMath::Max((ZEUInt)1, Info->Width / 4) * ZEMath::Max((ZEUInt)1, Info->Height / 4) * 8;
		case ZE_TPF_DXT3:
			return ZEMath::Max((ZEUInt)1, Info->Width / 4) * ZEMath::Max((ZEUInt)1, Info->Height / 4) * 16;
		case ZE_TPF_DXT5:
			return ZEMath::Max((ZEUInt)1, Info->Width / 4) * ZEMath::Max((ZEUInt)1, Info->Height / 4) * 16;
		default:
			zeError("Unsuppored DDS file pixel format.");
			return 0;
	}
}

static void ConvertRow(void* Data, void* Source, ZEDDSEmulatedFormat EmulatedFormat, ZEUInt Width)
{
	switch (EmulatedFormat)
	{
		case ZE_DEF_NOT_EMULATED:
			break;
		case ZE_DEF_A8R8G8B8:
			ZETexturePixelConverter::ConvertBGRA8((ZEPixelRGBA8*)Data, Source, Width);
			break;
		case ZE_DEF_X8R8G8B8:
			ZETexturePixelConverter::ConvertBGRX8((ZEPixelRGBA8*)Data, Source, Width);
			break;
		case ZE_DEF_X1R5G5B5:
			ZETexturePixelConverter::ConvertBGRX1555((ZEPixelRGBA8*)Data, Source, Width);
			break;
		case ZE_DEF_R8G8B8:
			ZETexturePixelConverter::ConvertBGR8((ZEPixelRGBA8*)Data, Source, Width);
			break;
		case ZE_DEF_R5G6B5:
			ZETexturePixelConverter::ConvertBGR565((ZEPixelRGBA8*)Data, Source, Width);
			break;
		case ZE_DEF_A8L8:
			ZETexturePixelConverter::ConvertAL16((ZEPixelLA16*)Data, Source, Width);
			break;
		case ZE_DEF_A1R5G5B5:
			ZETexturePixelConverter::ConvertBGRA1555((ZEPixelRGBA8*)Data, Source, Width);
			break;
	}
}

static bool LoadTextureInfo(ZETextureDataInfo* Info, ZEFile* File, ZEDDSEmulatedFormat* EmulatedFormat)
{
	if (File == NULL)
	{
		zeError("Null file pointer.");
		return false;
	}
	ZE_DDS_HEADER DDS;
	ZESize DdsMagicNumber;
	if (File->Read((void*)&DdsMagicNumber, 4, 1) != 1)
	{
		zeError("Can not read magic number.");
		return false;
	}
	if (DdsMagicNumber != ZE_DDS_MAGIC_NUMBER)
	{
		zeError("Not a DDS file");
		return false;
	}
	if (File->Read((void*)&DDS, sizeof(DDS), 1) != 1)
	{
		zeError("Can not read header.");
		return false;
	}
	Info->Height = DDS.Height;
	Info->Width = DDS.Width;
	if (Info->Height == 0 || Info->Width == 0)
	{
		zeError("Corrupted or malicious DDS file. Zero width or height.");
		return false;
	}

	if (Info->Width < 0)
	{
		zeError("Corrupted or malicious DDS file. Negative width.");
		return false;
	}

	if (DDS.MipMapCount == 0)
	{
		Info->LevelCount = 1;
	}
	else
	{
		Info->LevelCount = DDS.MipMapCount;
		if ((ZEMath::Max((ZEMath::Log(Info->Width) / ZEMath::Log(2)), (ZEMath::Log(Info->Height) / ZEMath::Log(2))) + 1) < DDS.MipMapCount)
		{
			zeError("Number of mipmaps is too much for the dimensions of the file");
			return false;
		}
		if (!(DDS.Width != 0 && ((DDS.Width & (DDS.Width - 1)) == 0) && DDS.Height != 0 && ((DDS.Height & (DDS.Height - 1)) == 0)))
		{
			zeError("Mipmaps are not supported for this file since width or height value is not a power of two");
			return false;
		}
	}

	if (DDS.Depth == 0)
		Info->SurfaceCount = 1;
	else
		Info->SurfaceCount = DDS.Depth; 

	if (DDS.Depth > 0)
	{
		Info->Type = ZE_TT_3D;
		if (Info->LevelCount > 1 && (Info->SurfaceCount & (Info->SurfaceCount - 1)) != 0)
		{
			zeError("Mipmaps are not supported for this file since the depth value is not a power of two");
			return false;
		}
	}
	else if ((DDS.Caps2 & ZE_DDSCAPS2_CUBEMAP) == ZE_DDSCAPS2_CUBEMAP)
	{
		Info->Type = ZE_TT_CUBE;
		if (DDS.Width != DDS.Height)
		{
			zeError("Cube maps must have square dimensions.");
			return false;
		}
		if (((DDS.Caps2 & ZE_DDSCAPS2_CUBEMAP_POSITIVEX) == ZE_DDSCAPS2_CUBEMAP_POSITIVEX) &&
			((DDS.Caps2 & ZE_DDSCAPS2_CUBEMAP_NEGATIVEX) == ZE_DDSCAPS2_CUBEMAP_NEGATIVEX) &&
			((DDS.Caps2 & ZE_DDSCAPS2_CUBEMAP_POSITIVEY) == ZE_DDSCAPS2_CUBEMAP_POSITIVEY) &&
			((DDS.Caps2 & ZE_DDSCAPS2_CUBEMAP_NEGATIVEY) == ZE_DDSCAPS2_CUBEMAP_NEGATIVEY) &&
			((DDS.Caps2 & ZE_DDSCAPS2_CUBEMAP_POSITIVEZ) == ZE_DDSCAPS2_CUBEMAP_POSITIVEZ) && 	
			((DDS.Caps2 & ZE_DDSCAPS2_CUBEMAP_NEGATIVEZ) == ZE_DDSCAPS2_CUBEMAP_NEGATIVEZ))
		{
			Info->SurfaceCount = 6;
		}
		else
		{
			zeError("Partial cube maps are not supported!");
		}
		
	}
	else
	{
		Info->Type = ZE_TT_2D;
	}

	Info->PixelFormat = GetPixelFormat(DDS.PixelFormat, EmulatedFormat);
	return true;
}

static ZEInt GetRowSize(ZETextureDataInfo* Info, ZEDDSEmulatedFormat EmulatedFormat)
{
	if (EmulatedFormat == ZE_DEF_NOT_EMULATED)
	{
		switch (Info->PixelFormat)
		{
			case ZE_TPF_I8:
				return Info->Width;
			case ZE_TPF_I8_4:
				return Info->Width * 4;
			case ZE_TPF_I16:
				return Info->Width * 2;
			case ZE_TPF_I16_2:
				return Info->Width * 4;
			case ZE_TPF_F16:
				return Info->Width * 2;
			case ZE_TPF_F16_2:
				return Info->Width * 4;
			case ZE_TPF_F16_4:
				return Info->Width * 8;
			case ZE_TPF_F32:
				return Info->Width * 4;
			case ZE_TPF_F32_2:
				return Info->Width * 8;
			case ZE_TPF_F32_4:
				return Info->Width * 16;
			case ZE_TPF_DXT1:
				return ZEMath::Max((ZEUInt)1, Info->Width / 4) * 8;
			case ZE_TPF_DXT3:
				return ZEMath::Max((ZEUInt)1, Info->Width / 4) * 16;
			case ZE_TPF_DXT5:
				return ZEMath::Max((ZEUInt)1, Info->Width / 4) * 16;
			default:
				zeError("Unsuppored DDS file pixel format.");
				return 0;
		}
	}
	else
	{
		switch (EmulatedFormat)
		{
			case ZE_DEF_X8R8G8B8:
				return Info->Width*4;
			case ZE_DEF_A8R8G8B8:
				return Info->Width*4;
			case ZE_DEF_X1R5G5B5:
				return Info->Width*2;
			case ZE_DEF_R8G8B8:
				return Info->Width*3;
			case ZE_DEF_R5G6B5:
				return Info->Width*2;
			case ZE_DEF_A8L8:
				return Info->Width*2;
			case ZE_DEF_A1R5G5B5:
				return Info->Width*2;
			default:
				zeError("Unknown Emulated Format");
				return 0;
		}
	}
}

bool ZETextureFileDDS::LoadInfo(ZETextureDataInfo* Info, ZEFile* File)
{
	ZEDDSEmulatedFormat EmulatedFormat;
	return LoadTextureInfo(Info, File, &EmulatedFormat);
}



ZETextureData* ZETextureFileDDS::Load(ZEFile* File)
{
	if (File == NULL)
	{
		zeError("Null file pointer.");
		return NULL;
	}
	ZETextureDataInfo Info;
	ZEDDSEmulatedFormat EmulatedFormat;
	LoadTextureInfo(&Info, File, &EmulatedFormat);
	ZEPointer<ZETextureData> Texture = new ZETextureData();
	Texture->Create(Info.Type, Info.PixelFormat, Info.SurfaceCount, Info.LevelCount, Info.Width, Info.Height);
	
	ZESize TextureSize = GetTextureSize(&Info);
	void* SourceRow = malloc(GetRowSize(&Info, EmulatedFormat));
	if (Info.Type == ZE_TT_CUBE)
	{
		for (ZEInt I = 0; I < Info.SurfaceCount; I++)
		{
			for (ZEInt J = 0; J < Info.LevelCount; J++)
			{
				if (EmulatedFormat == ZE_DEF_NOT_EMULATED)
				{
					if (File->Read(Texture->GetSurfaces()[I].GetLevels()[J].GetData(), TextureSize, 1) != 1)
					{
						zeError("Can not read data.");
						return NULL;
					}
				}
				else
				{
					for (ZEInt K = 0; K < Info.Height; K++)
					{
						if (File->Read(SourceRow, GetRowSize(&Info,EmulatedFormat), 1) != 1)
						{
							zeError("Can not read data.");
							return NULL;
						}
						else
						{
							ConvertRow((ZEUInt32*)Texture->GetSurfaces()[I].GetLevels()[J].GetData() + K * Info.Width, SourceRow, EmulatedFormat, Info.Width);
						}
					}
				}
				Info.Height /= 2;
				Info.Width /= 2;
				TextureSize = GetTextureSize(&Info);
			}
			if (Info.Height == 0)
				Info.Height = ZEMath::Power(2, Info.LevelCount - 1);
			else
				Info.Height *= ZEMath::Power(2, Info.LevelCount - 1);
			if (Info.Width == 0)
				Info.Width = ZEMath::Power(2, Info.LevelCount - 1);
			else
				Info.Width *= ZEMath::Power(2, Info.LevelCount - 1);
			TextureSize = GetTextureSize(&Info);
		}
	}
	else if (Info.Type == ZE_TT_2D)
	{
		for (ZEInt I = 0; I < Info.LevelCount; I++)
		{
			if (EmulatedFormat == ZE_DEF_NOT_EMULATED)
			{
				if (File->Read(Texture->GetSurfaces()[0].GetLevels()[I].GetData(), TextureSize, 1) != 1)
				{
					zeError("Can not read data.");
					return NULL;
				}
			}
			else
			{
				for (ZEInt K = 0; K < Info.Height; K++)
				{
					if (File->Read(SourceRow, GetRowSize(&Info,EmulatedFormat), 1) != 1)
					{
						zeError("Can not read data.");
						return NULL;
					}
					else
					{
						ConvertRow((ZEUInt32*)Texture->GetSurfaces()[0].GetLevels()[I].GetData() + K * Info.Width, SourceRow, EmulatedFormat, Info.Width);
					}
				}
			}
			Info.Height /= 2;
			Info.Width /= 2;
			TextureSize = GetTextureSize(&Info);
			break;
		}
		if (Info.Height == 0)
			Info.Height = ZEMath::Power(2, Info.LevelCount - 1);
		else
			Info.Height *= ZEMath::Power(2, Info.LevelCount - 1);
		if (Info.Width == 0)
			Info.Width = ZEMath::Power(2, Info.LevelCount - 1);
		else
			Info.Width *= ZEMath::Power(2, Info.LevelCount - 1);
		TextureSize = GetTextureSize(&Info);
	}
	else if (Info.Type == ZE_TT_3D)
	{
		for (ZEInt I = 0; I < Info.LevelCount; I++)
		{
			for (ZEInt J = 0; J < Info.SurfaceCount; J++)
			{
				if (J % (ZEInt)(ZEMath::Power((ZEInt)2,I)) == 0)
				{
					if (EmulatedFormat == ZE_DEF_NOT_EMULATED)
					{
						if (File->Read(Texture->GetSurfaces()[J].GetLevels()[I].GetData(), TextureSize, 1) != 1)
						{
							zeError("Can not read data.");
							return NULL;
						}
					}
					else
					{
						for (ZEInt K = 0; K < Info.Height; K++)
						{
							if (File->Read(SourceRow, GetRowSize(&Info,EmulatedFormat), 1) != 1)
							{
								zeError("Can not read data.");
								return NULL;
							}
							else
							{
								ConvertRow((ZEUInt32*)Texture->GetSurfaces()[J].GetLevels()[I].GetData() + K * Info.Width, SourceRow, EmulatedFormat, Info.Width);
							}
						}
					}
				}
			}
			Info.Height /= 2;
			Info.Width /= 2;
			TextureSize = GetTextureSize(&Info);
		}
		if (Info.Height == 0)
			Info.Height = ZEMath::Power(2, Info.LevelCount - 1);
		else
			Info.Height *= ZEMath::Power(2, Info.LevelCount - 1);
		if (Info.Width == 0)
			Info.Width = ZEMath::Power(2, Info.LevelCount - 1);
		else
			Info.Width *= ZEMath::Power(2, Info.LevelCount - 1);
		TextureSize = GetTextureSize(&Info);
	}
	delete SourceRow;
	
	return Texture.Transfer();
}
