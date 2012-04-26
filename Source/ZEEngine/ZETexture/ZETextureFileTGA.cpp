//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureFileTGA.cpp
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

#include "ZETextureFileTGA.h"

#include "ZEError.h"
#include "ZEPacking.h"
#include "ZEEndian.h"
#include "ZETextureData.h"
#include "ZEDS/ZEPointer.h"
#include "ZEFile/ZEFile.h"
#include "ZETexturePixelConverter.h"

#define ZE_TIT_TYPE_MASK				0x03
#define ZE_TIT_NO_IMAGE					0x00
#define ZE_TIT_INDEXED					0x01
#define ZE_TIT_COLOR					0x02
#define ZE_TIT_GRAYSCALE				0x03

#define ZE_TIT_COMPRESSED				0x08	

#define ZE_TIO_RIGHT_TO_LEFT			0x10
#define ZE_TIO_TOP_TO_BOTTOM			0x20


ZEPackStruct
(
	struct ZETargaHeader
	{
		ZEUInt8						IdSize;
		ZEUInt8						ColorMapType;
		ZEUInt8						ImageType;
		
		// Palette
		ZELittleEndian<ZEUInt16>	PalleteFirstEntry;
		ZELittleEndian<ZEUInt16>	PalleteSize;
		ZEUInt8						PalleteEntryBPP;
		
		// Image
		ZELittleEndian<ZEUInt16>	XOrgin;
		ZELittleEndian<ZEUInt16>	YOrgin;
		ZELittleEndian<ZEUInt16>	Width;
		ZELittleEndian<ZEUInt16>	Height;
		ZEUInt8						BPP;
		ZEUInt8						ImageDescriptor;
	}
);

ZEPackStruct
(
	struct ZETargaFooter
	{
		ZELittleEndian<ZEUInt32>	ExtensionOffset;
		ZELittleEndian<ZEUInt32>	DevelopperAreaOffset;
		char						Signature[18];
	}
);

static inline ZESize GetPixelSize(ZEUInt8 BPP)
{
	switch(BPP)
	{
		case 8:
			return 1;

		case 15:
			return 2;

		case 16:
			return 2;

		case 24:
			return 3;

		case 32:
			return 4;

		default:
			return 0;
	}
}

static __forceinline void ConvertColorRow(ZEARGB32* Destination, ZEUInt8* Source, ZESize BPP, ZESize Width)
{
	switch(BPP)
	{
		case 15:
			ZETexturePixelConverter::ConvertBGR15(Destination, Source,  Width);
			break;

		case 16:
			ZETexturePixelConverter::ConvertBGR16(Destination, Source, Width);
			break;

		case 24:
			ZETexturePixelConverter::ConvertBGR24(Destination, Source,  Width);
			break;

		case 32:
			ZETexturePixelConverter::ConvertBGRA32(Destination, Source, Width);
			break;
	}
}


static __forceinline void ConvertGrayscaleRow(ZEARGB32* Destination, ZEUInt8* Source, ZESize BPP, ZESize Width)
{
	switch(BPP)
	{
		case 8:
			ZETexturePixelConverter::ConvertG8(Destination, Source, Width);
			break;

		case 16:
			ZETexturePixelConverter::ConvertG16(Destination, Source, Width);
			break;

		case 32:
			ZETexturePixelConverter::ConvertG32(Destination, Source, Width);
			break;
	}
}

#define ZE_TGA_DECOMPRESS_BUFFER_SIZE 4096

static bool LoadCompressedData(ZEUInt8* Data, ZEFile* File, ZETargaHeader* Header)
{
	ZESize PixelSize;

	if ((Header->ImageType & ZE_TIT_TYPE_MASK) == ZE_TIT_INDEXED)
		PixelSize = 1;
	else
		PixelSize = GetPixelSize(Header->BPP);
	
	ZESize TotalPixelCount = 0;
	ZEUInt8 Buffer[ZE_TGA_DECOMPRESS_BUFFER_SIZE];
	

	ZEUInt8* SourceEnd = Data + ZE_TGA_DECOMPRESS_BUFFER_SIZE;
	ZEUInt8* Source = SourceEnd;

	int State = 0;
	ZESize Count = 0;
	
	if (PixelSize == 1)
	{
		ZEUInt8* Destination = Data;
		ZEUInt8* DestinationEnd = Data + Header->Width * Header->Height;
		ZEUInt8 RepeatValue;
		while(true)
		{
			if (Destination == DestinationEnd)
				break;

			if (Source == SourceEnd)
			{
				ZEUInt64 BytesRead = File->Read(Buffer, 1, ZE_TGA_DECOMPRESS_BUFFER_SIZE);
				if (BytesRead == 0)
					return false;
				Source = Buffer;
				SourceEnd = Buffer + BytesRead;
			}

			switch(State)
			{
				case 0: // Command Mode
					State = (*Source & 0x80) == 0 ? 1 : 2;
					Count = *Source & 0x7F;
					if (Count == 0)
						return false;
					Count++;
					break;

				case 1: // Absolute Mode
					*Destination++ = *Source++;
					Count--;
					if (Count == 0)
						State = 0;
					break;

				case 2: // Repeat Mode Initial
					RepeatValue = *Source++;
					State = 3;
					break;
				
				case 3: // Repeat Mode				
					*Destination++ = RepeatValue;
					Count--;
					if (Count == 0)
						State = 0;
					break;
			}
		}
	}

	return true;
}

static __forceinline bool LoadRow(ZEUInt8* RowData, ZEFile* File, ZETargaHeader* Header)
{
	ZEUInt64 Result = File->Read(RowData, Header->Width * GetPixelSize(Header->BPP), 1);
	return Result == 1;
}

static ZETextureData* LoadData(ZEFile* File, ZETargaHeader* Header, ZEARGB32* Palette)
{
	ZESize Width = Header->Width;
	ZESize Height = Header->Height; 
	ZESize PixelSize = GetPixelSize(Header->BPP);
	ZESize BPP = Header->BPP;

	ZESSize Step = (Header->ImageDescriptor & ZE_TIO_TOP_TO_BOTTOM ? 1 : -1);
	ZESize Index = (Header->ImageDescriptor & ZE_TIO_TOP_TO_BOTTOM ? 0 :  Header->Height - 1);

	ZEPointer<ZETextureData> Texture = new ZETextureData();
	Texture->Create(ZE_TT_2D, ZE_TPF_I8_4, 1, 1, Width, Height);
	ZEARGB32* Destination = (ZEARGB32*)Texture->GetSurfaces()[0].GetLevels()[0].GetData();

	File->Seek(sizeof(ZETargaHeader) + Header->PalleteSize * GetPixelSize(Header->PalleteEntryBPP) + Header->IdSize, ZE_SF_BEGINING);

	ZEARGB32* DestinationRow = Destination + Index * Width;
	ZESSize	DestinationRowStep = Step * Width;

	if ((Header->ImageType & ZE_TIT_COMPRESSED) == 0)
	{
		ZEPointer<ZEUInt8> SourceRow = new ZEUInt8[PixelSize * Header->Width];
		if ((Header->ImageType & ZE_TIT_TYPE_MASK) == ZE_TIT_INDEXED)
		{
			for (ZESize I = 0; I < Height; I++)
			{
				LoadRow(SourceRow, File, Header);
				ZETexturePixelConverter::ConvertIndexed(DestinationRow, SourceRow, Width, Palette);
				DestinationRow += DestinationRowStep;
			}
		}
		else if ((Header->ImageType & ZE_TIT_TYPE_MASK) == ZE_TIT_GRAYSCALE)
		{
			for (ZESize I = 0; I < Height; I++)
			{
				LoadRow(SourceRow, File, Header);
				ConvertGrayscaleRow(DestinationRow, SourceRow, BPP, Width);
				DestinationRow += DestinationRowStep;
			}
		}
		else if ((Header->ImageType & ZE_TIT_TYPE_MASK) == ZE_TIT_COLOR)
		{
			for (ZESize I = 0; I < Height; I++)
			{
				LoadRow(SourceRow, File, Header);
				ConvertColorRow(DestinationRow, SourceRow, BPP, Width);
				DestinationRow += DestinationRowStep;
			}
		}
	}
	else
	{
		ZEPointer<ZEUInt8> UncompressedData = new ZEUInt8[Width * Height * PixelSize];
		ZEUInt8* SourceRow = UncompressedData;
		ZESSize SourceRowStep = Step * Width * PixelSize;

		if (!LoadCompressedData(UncompressedData, File, Header))
		{
			zeError("Can not uncompress pixel data.");
			return NULL;
		}

		ZESize LastIndex = 0;
		if ((Header->ImageType & ZE_TIT_TYPE_MASK) == ZE_TIT_INDEXED)
		{
			for (ZESize I = 0; I < Height; I++)
			{
				ZETexturePixelConverter::ConvertIndexed(DestinationRow, SourceRow, Width, Palette);
				DestinationRow += DestinationRowStep;
				SourceRow += SourceRowStep;
			}
		}
		else if ((Header->ImageType & ZE_TIT_TYPE_MASK) == ZE_TIT_GRAYSCALE)
		{
			for (ZESize I = 0; I < Height; I++)
			{
				ConvertGrayscaleRow(DestinationRow, SourceRow, BPP, Width);
				DestinationRow += DestinationRowStep;
				SourceRow += SourceRowStep;
			}
		}
		else if ((Header->ImageType & ZE_TIT_TYPE_MASK) == ZE_TIT_COLOR)
		{
			for (ZESize I = 0; I < Height; I++)
			{
				ConvertColorRow(DestinationRow, SourceRow, BPP, Width);
				DestinationRow += DestinationRowStep;
				SourceRow += SourceRowStep;
			}
		}
	}

	return Texture.Transfer();
}

static bool LoadHeader(ZEFile* File, ZETargaHeader* Header, ZEARGB32* Palette)
{
	File->Seek(-(ZESSize)sizeof(ZETargaFooter), ZE_SF_END);
	
	ZETargaFooter Footer;

	ZEInt64 Result;
	Result = File->Read(&Footer, sizeof(ZETargaFooter), 1);
	if (Result != 1)
	{
		zeError("Can not load TGA file footer.");
		return false;
	}

	if (strncmp(Footer.Signature, "TRUEVISION-XFILE.", 17) != 0)
	{
		if (ZEFile::GetFileExtension(File->GetFilePath()).Lower() != ".tga")
		{
			zeError("Can not identifty the file. File is not a TGA 2.0 file and it's extension is not .TGA.");
			return false;
		}
	}

	File->Seek(0, ZE_SF_BEGINING);
	Result = File->Read(Header, sizeof(ZETargaHeader), 1);
	if (Result != 1)
	{
		zeError("Can not load TGA file header.");
		return false;
	}

	if (Header->BPP != 8 && Header->BPP != 16 && Header->BPP != 24 && Header->BPP != 32)
	{
		zeError("Unsupported pixel size.");
		return false;
	}

	if (Header->Width == 0 || Header->Height == 0)
	{
		zeError("Zero sized TGAs are not supported.");
		return false;
	}

	if ((Header->ImageDescriptor & ZE_TIO_RIGHT_TO_LEFT) != 0)
	{
		zeError("Right to left images are not supported.");
		return false;
	}

	if (Header->ImageType != ZE_TIT_INDEXED && 
		Header->ImageType != ZE_TIT_GRAYSCALE &&
		Header->ImageType != ZE_TIT_COLOR && 
		Header->ImageType != (ZE_TIT_COMPRESSED | ZE_TIT_INDEXED) && 
		Header->ImageType != (ZE_TIT_COMPRESSED | ZE_TIT_GRAYSCALE) && 
		Header->ImageType != (ZE_TIT_COMPRESSED | ZE_TIT_COLOR))
	{
		zeError("Unsupported image type.");
		return false;
	}

	if ((Header->ImageDescriptor & ZE_TIO_RIGHT_TO_LEFT) != 0)
	{
		zeError("Right to left image data is not supported.");
		return false;
	}

	ZESize PalletePixelSize = GetPixelSize(Header->PalleteEntryBPP);

	if (Header->ColorMapType == 1)
	{
		if (Header->BPP != 8)
		{
			zeError("Unsupported indexed pixel size.");
			return false;
		}

		ZESize PalleteSize = (Header->PalleteSize > 256 ? 256 : Header->PalleteSize);
		memset(Palette, 0, sizeof(ZEARGB32) * 256);

		ZEUInt32 PalleteTemp[256];
		Result = File->Read(PalleteTemp, PalletePixelSize, PalleteSize);
		if (Result != PalleteSize)
		{
			zeError("Can not load pallete.");
			return false;
		}

		ConvertColorRow(Palette, (ZEUInt8*)PalleteTemp, Header->PalleteEntryBPP, 256);
	}

	return true;
}

static ZETextureData* Load(ZEFile* File)
{
	ZETargaHeader Header;
	ZEARGB32 Palette[256];

	if (!LoadHeader(File, &Header, Palette))
		return NULL;
	
	return LoadData(File, &Header, Palette);
}

bool ZETextureFileTGA::CanLoad(ZEFile* File)
{
	File->Seek(-(ZESSize)sizeof(ZETargaFooter), ZE_SF_END);

	ZETargaFooter Footer;

	ZEInt64 Result;
	Result = File->Read(&Footer, sizeof(ZETargaFooter), 1);
	if (Result != 1)
		return false;

	if (strncmp(Footer.Signature, "TRUEVISION-XFILE.", 17) == 0)
		return true;

	if (ZEFile::GetFileExtension(File->GetFilePath()).Lower() != "tga")
		return false;

	return true;
}

ZETextureData* ZETextureFileTGA::Load(ZEFile* File)
{
	if (File == NULL)
	{
		zeError("Null file pointer.");
		return NULL;
	}
	ZETextureData* Texture = ::Load(File);
	if (Texture == NULL)
		zeError("Can not load TGA file.");

	return Texture;
}

#include <stdio.h>
#include "ZECore/ZEProfilerCounter.h"
#include "ZETexture/ZEBitmap.h"

void TestTGA(ZEString FileName)
{
	ZEProfilerCounter A;
	zeLog("Testing file: %s", FileName.ToCString());
	ZETextureFileTGA Loader;

	A.Start();
	ZEPointer<ZEFile> File = ZEFile::Open(FileName);
	ZETextureData* Data = Loader.Load(File);
	File->Close();
	A.Stop();
	ZEUInt64 TotalTimeA = A.GetFrameTotalTime();

	ZEProfilerCounter B;
	ZEBitmap Temp;

	B.Start();
	Temp.Load(FileName);
	B.Stop();

	ZEUInt64 TotalTimeB = B.GetFrameTotalTime();


	zeLog("File : %s, Zinek : %llu, FreeImage : %llu, Ratio : %f", FileName.ToCString(), TotalTimeA, TotalTimeB, (float)TotalTimeA / (float)TotalTimeB);

	if (Data == NULL)
		zeCriticalError("Failed");

	ZETextureLevel* Level = &Data->GetSurfaces()[0].GetLevels()[0];

	ZEBitmap Bitmap;
	Bitmap.Create(Level->GetWidth(), Level->GetHeight(), 4);
	Bitmap.CopyFrom(Level->GetData(), Level->GetPitch(), Level->GetWidth(), Level->GetHeight());
	Bitmap.Save(FileName + ".result.bmp", ZE_BFF_BMP);
}
