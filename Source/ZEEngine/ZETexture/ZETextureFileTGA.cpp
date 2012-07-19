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
#include "ZEFile/ZEFileInfo.h"
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

#define ZE_TGA_DECOMPRESS_BUFFER_SIZE 4096

enum ZETGARLEDecompressorResult
{
	ZE_TGA_RLE_OUTPUT_FULL,
	ZE_TGA_RLE_INPUT_PROCESSED,
	ZE_TGA_RLE_ERROR
};

enum ZETGARLDecompressorMode
{
	ZE_TGA_RLE_DM_NONE,
	ZE_TGA_RLE_DM_REPEAT_INIT,
	ZE_TGA_RLE_DM_REPEAT,
	ZE_TGA_RLE_DM_ABSOLUTE
};

struct ZETGARLEDecompressorState
{
	// Last State
	ZEInt Mode;
	ZESize Count;
	ZESize Count2;
	ZEUInt8 Value[4];

	ZESize PixelSize;

	// Ouput
	void* Output;
	ZESize OutputCursor;
	ZESize OutputSize;

	// Input
	void* Input;
	ZESize InputCursor;
	ZESize InputSize;

	ZETGARLEDecompressorState()
	{
		Mode = ZE_TGA_RLE_DM_NONE;
		Count = 0;
		Count2 = 0;
		Value[0] = Value[1] = Value[2] = Value[3] = 0;


		Output = NULL;
		OutputCursor = 0;
		OutputSize = 0;

		Input = NULL;
		InputCursor = 0;
		InputSize = 0;
	}
};

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

static __forceinline void ConvertColorRow(ZEBGRA32* Destination, ZEUInt8* Source, ZESize BPP, ZESize Width)
{
	switch(BPP)
	{
		case 16:
			ZETexturePixelConverter::ConvertBGR555(Destination, Source,  Width);
			break;

		case 15:
			ZETexturePixelConverter::ConvertBGR565(Destination, Source, Width);
			break;

		case 24:
			ZETexturePixelConverter::ConvertBGR8(Destination, Source,  Width);
			break;

		case 32:
			ZETexturePixelConverter::ConvertBGRA8(Destination, Source, Width);
			break;
	}
}

static __forceinline void ConvertGrayscaleRow(ZEBGRA32* Destination, ZEUInt8* Source, ZESize BPP, ZESize Width)
{
	switch(BPP)
	{
		case 8:
			ZETexturePixelConverter::ConvertL8(Destination, Source, Width);
			break;

		case 16:
			ZETexturePixelConverter::ConvertL16(Destination, Source, Width);
			break;
	}
}

static ZETGARLEDecompressorResult UncomressRLE(ZETGARLEDecompressorState* State)
{
	ZESize Remaining = 0;
	while(true)
	{
		if (State->InputCursor >= State->InputSize)
		{
			State->InputCursor = 0;
			return ZE_TGA_RLE_INPUT_PROCESSED;
		}

		if (State->OutputCursor >= State->OutputSize)
		{
			State->OutputCursor = 0;
			return ZE_TGA_RLE_OUTPUT_FULL;
		}

		ZEUInt8* Input = (ZEUInt8*)State->Input + State->InputCursor;
		ZEUInt8* Output = (ZEUInt8*)State->Output + State->OutputCursor;

		switch(State->Mode)
		{
			case ZE_TGA_RLE_DM_NONE: // None
				if ((*Input & 0x80) == 0x80)
				{
					State->Mode = ZE_TGA_RLE_DM_REPEAT_INIT;
					State->Count = ((*Input & 0x7F) + 1) * State->PixelSize;
					State->Count2 = State->PixelSize;
				}
				else
				{
					State->Mode = ZE_TGA_RLE_DM_ABSOLUTE;
					State->Count = ((*Input & 0x7F) + 1) * State->PixelSize;
				}
				State->InputCursor++;
				break;
	
			case ZE_TGA_RLE_DM_REPEAT_INIT:
				State->Value[State->PixelSize - State->Count2] = *Input;
				State->Count2--;
				State->InputCursor++;
				if (State->Count2 == 0)
					State->Mode = ZE_TGA_RLE_DM_REPEAT;
				break;

			case ZE_TGA_RLE_DM_REPEAT:
				if (State->OutputCursor + State->Count <= State->OutputSize * State->PixelSize)
					Remaining = State->Count;
				else
					Remaining = State->OutputSize - State->OutputCursor;

				for (ZESize I = 0; I < Remaining; I++)
				{
					*Output++ = State->Value[State->Count2 % State->PixelSize];
					State->Count2++;
				}

				State->Count -= Remaining;
				State->OutputCursor += Remaining;

				if (State->Count == 0)
					State->Mode = ZE_TGA_RLE_DM_NONE;
				break;

			case ZE_TGA_RLE_DM_ABSOLUTE:
				if (State->OutputCursor + State->Count <= State->OutputSize)
					Remaining = State->Count;
				else
					Remaining = State->OutputSize - State->OutputCursor;

				if (State->InputCursor + Remaining > State->InputSize)
					Remaining = State->InputSize - State->InputCursor;
								
				memcpy(Output, Input, Remaining);

				State->OutputCursor += Remaining;
				State->InputCursor += Remaining;
				State->Count -= Remaining;

				if (State->Count == 0)
					State->Mode = ZE_TGA_RLE_DM_NONE;
				break;
		}
	}
}

static ZETextureData* LoadData(ZEFile* File, ZETargaHeader* Header, ZEBGRA32* Palette)
{
	ZESize Width = Header->Width;
	ZESize Height = Header->Height; 
	ZESize PixelSize = GetPixelSize(Header->BPP);
	ZESize BPP = Header->BPP;

	ZESSize Step = (Header->ImageDescriptor & ZE_TIO_TOP_TO_BOTTOM ? 1 : -1);
	ZESize Index = (Header->ImageDescriptor & ZE_TIO_TOP_TO_BOTTOM ? 0 :  Header->Height - 1);

	ZEPointer<ZETextureData> Texture = new ZETextureData();
	Texture->Create(ZE_TT_2D, ZE_TPF_I8_4, 1, 1, Width, Height);
	ZEBGRA32* Destination = (ZEBGRA32*)Texture->GetSurfaces()[0].GetLevels()[0].GetData();
	ZEBGRA32* DestinationEnd = Destination + Height * Width;

	File->Seek(sizeof(ZETargaHeader) + Header->PalleteSize * GetPixelSize(Header->PalleteEntryBPP) + Header->IdSize, ZE_SF_BEGINING);

	ZEBGRA32* DestinationRow = Destination + Index * Width;
	ZESSize	DestinationRowStep = Step * Width;
	
	ZESize SourceRowSize = Header->Width * PixelSize;
	ZEPointer<ZEUInt8> SourceRow = new ZEUInt8[SourceRowSize];

	if ((Header->ImageType & ZE_TIT_COMPRESSED) == 0)
	{
		if ((Header->ImageType & ZE_TIT_TYPE_MASK) == ZE_TIT_COLOR)
		{
			for (ZESize I = 0; I < Height; I++)
			{
				if (File->Read(SourceRow, SourceRowSize, 1) != 1)
					return NULL;

				ConvertColorRow(DestinationRow, SourceRow, BPP, Width);
				DestinationRow += DestinationRowStep;
			}
		}
		else if ((Header->ImageType & ZE_TIT_TYPE_MASK) == ZE_TIT_GRAYSCALE)
		{
			for (ZESize I = 0; I < Height; I++)
			{
				if (File->Read(SourceRow, SourceRowSize, 1) != 1)
					return NULL;

				ConvertGrayscaleRow(DestinationRow, SourceRow, BPP, Width);
				DestinationRow += DestinationRowStep;
			}
		}
		else if ((Header->ImageType & ZE_TIT_TYPE_MASK) == ZE_TIT_INDEXED)
		{
			for (ZESize I = 0; I < Height; I++)
			{
				if (File->Read(SourceRow, SourceRowSize, 1) != 1)
					return NULL;

				ZETexturePixelConverter::ConvertIndexed(DestinationRow, SourceRow, Width, Palette);
				DestinationRow += DestinationRowStep;
			}
		}
	}
	else
	{
		const ZESize BlockSize = 32 * 1024;
		ZEUInt8 Block[BlockSize];

		ZETGARLEDecompressorState State;
		State.PixelSize = PixelSize;
		State.Input = Block;
		State.InputSize = BlockSize;
		State.OutputSize = Width * PixelSize;
		State.Output = (ZEUInt8*)SourceRow;

		while (true)
		{
			if (DestinationRow < Destination || DestinationRow >= DestinationEnd)
				break;

			State.InputSize = File->Read(Block, 1, BlockSize);
			if (State.InputSize ==  0)
				return NULL;

			while(true)
			{
				if (DestinationRow < Destination || DestinationRow >= DestinationEnd)
					break;

				ZETGARLEDecompressorResult Result = UncomressRLE(&State);
				if (Result == ZE_TGA_RLE_INPUT_PROCESSED)
					break;
				else if (Result == ZE_TGA_RLE_OUTPUT_FULL)
				{
					if ((Header->ImageType & ZE_TIT_TYPE_MASK) == ZE_TIT_COLOR)
						ConvertColorRow(DestinationRow, SourceRow, BPP, Width);
					else if ((Header->ImageType & ZE_TIT_TYPE_MASK) == ZE_TIT_GRAYSCALE)
						ConvertGrayscaleRow(DestinationRow, SourceRow, BPP, Width);
					else if ((Header->ImageType & ZE_TIT_TYPE_MASK) == ZE_TIT_INDEXED)
						ZETexturePixelConverter::ConvertIndexed(DestinationRow, SourceRow, Width, Palette);
					DestinationRow = DestinationRow + DestinationRowStep;
				}
				else
					return NULL;
			}
		}
	}

	return Texture.Transfer();
}

static bool LoadHeader(ZEFile* File, ZETargaHeader* Header, ZEBGRA32* Palette)
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
		if (ZEFileInfo::GetFileExtension(File->GetPath()).Lower() != ".tga")
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

	if (Header->Height == 0 || Header->Width == 0)
	{
		zeError("Corrupted or malicious TGA file. Zero width or height.");
		return false;
	}

	if (Header->Width * Header->Height * 4 == 0)
	{
		zeError("Corrupted or malicious TGA file. 32bit size overflow detected.");
		return false;
	}

	if (Header->ImageType & ZE_TIT_COMPRESSED)
	{
		ZESize PotentialSize = Header->Width * Header->Height * (Header->BPP / 8) / 128;
		if (File->GetSize() < PotentialSize) 
		{
			zeError("Corrupted or malicious TGA file. File size is too small.");
			return false;
		}
	}
	else
	{
		ZESize PotentialSize = Header->Width * Header->Height * (Header->BPP / 8);
		if (File->GetSize() < PotentialSize) 
		{
			zeError("Corrupted or malicious TGA file. File size is too small.");
			return false;
		}
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
		memset(Palette, 0, sizeof(ZEBGRA32) * 256);

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

static ZETextureData* LoadImage(ZEFile* File)
{
	ZETargaHeader Header;
	ZEBGRA32 Palette[256];

	if (!LoadHeader(File, &Header, Palette))
		return NULL;
	
	return LoadData(File, &Header, Palette);
}

bool ZETextureFileTGA::LoadInfo(ZETextureDataInfo* Info, ZEFile* File)
{
	if (File == NULL)
		return false;

	ZEBGRA32 Palette[256];
	ZETargaHeader Header;
	if (!LoadHeader(File, &Header, Palette))
		return false;

	Info->Width = Header.Width;
	Info->Height = Header.Height;
	Info->PixelFormat = ZE_TPF_I8_4;
	Info->Type = ZE_TT_2D;
	Info->SurfaceCount = 1;
	Info->LevelCount = 1;

	return true;
}

ZETextureData* ZETextureFileTGA::Load(ZEFile* File)
{
	if (File == NULL)
	{
		zeError("Null file pointer.");
		return NULL;
	}

	ZETextureData* Texture = LoadImage(File);
	if (Texture == NULL)
		zeError("Can not load TGA file.");

	return Texture;
}
