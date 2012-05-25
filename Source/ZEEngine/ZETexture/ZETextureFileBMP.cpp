//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureFileBMP.cpp
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

#include "ZETextureFileBMP.h"

#include "ZEError.h"
#include "ZEPacking.h"
#include "ZEEndian.h"
#include "ZETextureData.h"
#include "ZEDS/ZEPointer.h"
#include "ZEFile/ZEFile.h"
#include "ZETexturePixelConverter.h"

enum ZEBMPRLEDecompressorMode
{
	ZE_BMP_RLE_DM_NONE				= 0,
	ZE_BMP_RLE_DM_MARKER			= 1,
	ZE_BMP_RLE_DM_DELTA_INIT		= 2,
	ZE_BMP_RLE_DM_DELTA				= 3,
	ZE_BMP_RLE_DM_REPEAT_INIT		= 4,
	ZE_BMP_RLE_DM_REPEAT			= 5,
	ZE_BMP_RLE_DM_ABSOLUTE_PADDING	= 6,
	ZE_BMP_RLE_DM_ABSOLUTE			= 7,
	ZE_BMP_RLE_DM_ABSOLUTE_END		= 8,
	ZE_BMP_RLE_DM_FILL_BLACK		= 9
};

enum ZEBMPRLEDecompressorResult
{
	ZE_BMP_RLE_OUTPUT_FULL,
	ZE_BMP_RLE_INPUT_PROCESSED,
	ZE_BMP_RLE_ERROR
};

struct ZEBMPRLEDecompressorState
{
	// Last State
	ZEInt Mode;
	ZESize Count;
	ZEUInt8 Value;
	ZEInt DeltaX;
	bool Wrapped;

	// Palette
	ZEBGRA32* Palette;

	// Ouput
	ZEBGRA32* Output;
	ZESize OutputCursor;
	ZESize OutputSize;

	// Input
	ZEUInt8* Input;
	ZESize InputCursor;
	ZESize InputSize;

	ZEBMPRLEDecompressorState()
	{
		Mode = ZE_BMP_RLE_DM_NONE;
		Count = 0;
		Value = 0;
		DeltaX = 0;
		Wrapped = false;

		Palette = NULL;

		Output = NULL;
		OutputCursor = 0;
		OutputSize = 0;

		Input = NULL;
		InputCursor = 0;
		InputSize = 0;
	}
};

ZEPackStruct
(
	struct ZEBitmapHeader
	{
		ZELittleEndian<ZEUInt16> Header;
		ZELittleEndian<ZEUInt32> FileSize;
		ZELittleEndian<ZEUInt16> Reserved0;
		ZELittleEndian<ZEUInt16> Reserved1;
		ZELittleEndian<ZEUInt32> DataPosition;

		// DIB Header
		ZELittleEndian<ZEUInt32> Size;

		// V1-2
		ZELittleEndian<ZEInt32>  Width;
		ZELittleEndian<ZEInt32>  Height;
		ZELittleEndian<ZEUInt16> PlaneCount;
		ZELittleEndian<ZEUInt16> BitsPerPixel;

		// V3 
		ZELittleEndian<ZEUInt32> CompressionType;
		ZELittleEndian<ZEUInt32> DataSize;
		ZELittleEndian<ZEInt32>  HorizontalResolution;
		ZELittleEndian<ZEInt32>  VerticalResoulution;
		ZELittleEndian<ZEUInt32> PaletteEntryCount;
		ZELittleEndian<ZEUInt32> ImportantColorCount;

		// V4 Partial
		ZEBigEndian<ZEUInt32> RedMask; 
		ZEBigEndian<ZEUInt32> GreenMask;
		ZEBigEndian<ZEUInt32> BlueMask;
		ZEBigEndian<ZEUInt32> AlphaMask;
	}
);

static inline ZESize GetPixelSize(ZEUInt16 BPP)
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

static ZEBMPRLEDecompressorResult UncomressRLE(ZEBMPRLEDecompressorState* State)
{
	ZESize Remaining = 0;
	while(true)
	{
		if (State->InputCursor >= State->InputSize && State->Mode != ZE_BMP_RLE_DM_FILL_BLACK)
		{
			State->InputCursor = 0;
			return ZE_BMP_RLE_INPUT_PROCESSED;
		}

		if (State->OutputCursor >= State->OutputSize)
		{
			State->Wrapped = true;
			State->OutputCursor = 0;
			return ZE_BMP_RLE_OUTPUT_FULL;
		}

		ZEUInt8* Input = State->Input + State->InputCursor;
		ZEBGRA32* Output = State->Output + State->OutputCursor;

		switch(State->Mode)
		{
			case ZE_BMP_RLE_DM_NONE: // None
				if (*Input == 0)
					State->Mode = ZE_BMP_RLE_DM_MARKER;
				else
				{
					State->Mode = ZE_BMP_RLE_DM_REPEAT_INIT;
					State->Count = *Input;
				}
				State->InputCursor++;
				break;

			case ZE_BMP_RLE_DM_MARKER:
				if (*Input == 0) // End of Row
				{
					if (!State->Wrapped)
					{
						State->Count = State->OutputSize - State->OutputCursor;
						State->Wrapped = false;
						State->Mode = ZE_BMP_RLE_DM_FILL_BLACK;
					}
					else
						State->Mode = ZE_BMP_RLE_DM_NONE;
				}
				else if (*Input == 1) // End of Bitmap
				{
					State->Count = (ZESize)-1;
					State->Mode = ZE_BMP_RLE_DM_FILL_BLACK;
				}
				else if (*Input == 2) // Delta
				{
					State->Mode = ZE_BMP_RLE_DM_DELTA_INIT;
				}
				else // Absolute Mode
				{
					State->Count = *Input;
					/*if (State->Count % 2 == 1)
						State->Mode = ZE_BMP_RLE_DM_ABSOLUTE_PADDING;
					else*/
						State->Mode = ZE_BMP_RLE_DM_ABSOLUTE;
				}
				State->InputCursor++;
				break;

			case ZE_BMP_RLE_DM_DELTA_INIT:
				State->DeltaX = *Input;
				State->Mode = ZE_BMP_RLE_DM_DELTA;
				State->InputCursor++;
				break;

			case ZE_BMP_RLE_DM_DELTA:
				if (*Input > 0)
					State->Count = State->OutputSize * (ZESize)*Input - State->OutputCursor;
				State->Count += State->DeltaX;
				if (State->Count == 0)
					State->Mode = ZE_BMP_RLE_DM_NONE;
				State->Mode = ZE_BMP_RLE_DM_FILL_BLACK;
				State->InputCursor++;
				break;

			case ZE_BMP_RLE_DM_REPEAT_INIT:
				State->Value = *Input;
				State->Mode = ZE_BMP_RLE_DM_REPEAT;
				State->InputCursor++;
				break;

			case ZE_BMP_RLE_DM_REPEAT:

				if (State->OutputCursor + State->Count <= State->OutputSize)
					Remaining = State->Count;
				else
					Remaining = State->OutputSize - State->OutputCursor;

				for (ZESize I = 0; I < Remaining; I++)
					*Output++ = State->Palette[State->Value];
				
				State->Count -= Remaining;
				State->OutputCursor += Remaining;
				State->Wrapped = false;

				if (State->Count == 0)
					State->Mode = ZE_BMP_RLE_DM_NONE;
				break;

			case ZE_BMP_RLE_DM_ABSOLUTE_PADDING:
				if (*Input != 0)
					return ZE_BMP_RLE_ERROR;
				State->InputCursor++;
				State->Mode = ZE_BMP_RLE_DM_ABSOLUTE;
				break;

			case ZE_BMP_RLE_DM_ABSOLUTE:
				if (State->OutputCursor + State->Count <= State->OutputSize)
					Remaining = State->Count;
				else
					Remaining = State->OutputSize - State->OutputCursor;

				if (State->InputCursor + Remaining > State->InputSize)
					Remaining = State->InputSize - State->InputCursor;
								
				for (ZESize I = 0; I < Remaining; I++)
					*Output++ = State->Palette[*Input++];

				State->OutputCursor += Remaining;
				State->InputCursor += Remaining;
				State->Count -= Remaining;
				State->Wrapped = false;
				
				if (State->Count == 0)
					State->Mode = ZE_BMP_RLE_DM_ABSOLUTE_END;
				break;

			case ZE_BMP_RLE_DM_ABSOLUTE_END:
				if (*Input != 0)
					return ZE_BMP_RLE_ERROR;
				State->InputCursor++;
				State->Mode = ZE_BMP_RLE_DM_NONE;
				break;

			case ZE_BMP_RLE_DM_FILL_BLACK:
				if (State->OutputCursor + State->Count <= State->OutputSize)
					Remaining = State->Count;
				else
					Remaining = State->OutputSize - State->OutputCursor;

				memset(Output, 0, Remaining * sizeof(ZEBGRA32));

				State->OutputCursor += Remaining;
				State->Count -= Remaining;
				State->Wrapped = false;

				if (State->Count == 0)
					State->Mode = ZE_BMP_RLE_DM_NONE;

				break;
		}
	}
}

static ZETextureData* LoadData(ZEFile* File, ZEBitmapHeader* Header, ZEBGRA32* Palette)
{
	File->Seek(Header->DataPosition, ZE_SF_BEGINING);

	ZESize PixelSize = GetPixelSize(Header->BitsPerPixel);
	ZESSize Width = Header->Width;
	ZESSize Height = (Header->Height < 0 ? -Header->Height : Header->Height);

	ZEPointer<ZETextureData> Texture = new ZETextureData();
	Texture->Create(ZE_TT_2D, ZE_TPF_I8_4, 1, 1, (ZEUInt)Width, (ZEUInt)Height);
	
	ZEBGRA32* Destination = (ZEBGRA32*)Texture->GetSurfaces()[0].GetLevels()[0].GetData();
	ZEBGRA32* DestinationRow = Destination + (Header->Height > 0 ? Height - 1 : 0) * Width;
	ZESSize DestinationRowStep = (Header->Height > 0 ? -1 : 1) * Width;
	ZEBGRA32* DestinationEnd = Destination + Width * Height;

	ZESize Align = (Header->BitsPerPixel / 8) * Width;
	Align += (Align % 4) != 0 ? 4 - (Align % 4) : 0;

	if (Header->CompressionType == 1)
	{
		const ZESize BlockSize = 32 * 1024;
		ZEUInt8 Block[BlockSize];
		ZESize BlockCount = Header->DataSize / BlockSize;
		ZESize LastBlockSize = BlockSize;
		if (Header->DataSize % BlockSize != 0)
		{
			BlockCount++;
			LastBlockSize = Header->DataSize % BlockSize;
		}

		ZEBMPRLEDecompressorState State;
		State.Input = Block;
		State.InputSize = BlockSize;
		State.OutputSize = Width;
		State.Output = DestinationRow;
		State.Palette = Palette;
		for (ZESize I = 0; I < BlockCount; I++)
		{
			if (I == BlockCount - 1)
				State.InputSize = LastBlockSize;

			ZESize BytesRead = File->Read(Block, 1, State.InputSize);
			if (BytesRead !=  State.InputSize)
				return false;

			while(true)
			{
				if (State.Output < Destination || State.Output >= DestinationEnd)
					break;

				ZEBMPRLEDecompressorResult Result = UncomressRLE(&State);
				if (Result == ZE_BMP_RLE_INPUT_PROCESSED)
					break;
				else if (Result == ZE_BMP_RLE_OUTPUT_FULL)
					State.Output += DestinationRowStep;
				else
					return NULL;
			}
		}
	}
	else
	{
		ZEPointer<ZEUInt8> SourceRow = new ZEUInt8[Align];
		switch(Header->BitsPerPixel)
		{
			case 8:
				for (ZESize I = 0; I < Height; I++)
				{
					if (File->Read(SourceRow, Align, 1) != 1)
						return NULL;

					ZETexturePixelConverter::ConvertIndexed(DestinationRow, SourceRow, Width, Palette);
					DestinationRow += DestinationRowStep;
				}
				break;

			case 16:
				if (Header->GreenMask == ZEEndian::Big(0x000003E0))
				{
					for (ZESize I = 0; I < Height; I++)
					{
						if (File->Read(SourceRow, Align, 1) != 1)
							return NULL;

						ZETexturePixelConverter::ConvertBGR555(DestinationRow, SourceRow, Width);
						DestinationRow += DestinationRowStep;
					}
				}
				else
				{
					for (ZESize I = 0; I < Height; I++)
					{
						if (File->Read(SourceRow, Align, 1) != 1)
							return NULL;

						ZETexturePixelConverter::ConvertBGR565(DestinationRow, SourceRow, Width);
						DestinationRow += DestinationRowStep;
					}
				}
				break;		

			case 24:
				for (ZESize I = 0; I < Height; I++)
				{
					if (File->Read(SourceRow, Align, 1) != 1)
						return NULL;

					ZETexturePixelConverter::ConvertBGR8(DestinationRow, SourceRow, Width);
					DestinationRow += DestinationRowStep;
				}
				break;

			case 32:
				for (ZESize I = 0; I < Height; I++)
				{
					if (File->Read(SourceRow, Align, 1) != 1)
						return NULL;

					ZETexturePixelConverter::ConvertABGR8(DestinationRow, SourceRow, Width);
					DestinationRow += DestinationRowStep;
				}
				break;
		}
	}

	return Texture.Transfer();
}

static bool LoadHeader(ZEFile* File, ZEBitmapHeader* Header, ZEBGRA32* Palette)
{
	ZEUInt64 FileSize = File->GetFileSize();

	ZEUInt64 Result;
	File->Seek(0, ZE_SF_BEGINING);
	Result = File->Read(Header, 1, sizeof(ZEBitmapHeader));
	if (Result < 54)
	{
		zeError("Can not load BMP file header.");
		return false;
	}

	if (Header->Header != 'MB')
	{
		zeError("Unknown BMP file format.");
		return false;
	}

	ZEInt Width = ZEMath::Abs((ZEInt32)Header->Width);
	ZEInt Height = ZEMath::Abs((ZEInt32)Header->Height);

	if (Header->Height == 0 || Header->Width == 0)
	{
		zeError("Corrupted or malicious BMP file. Zero width or height.");
		return false;
	}

	if (Header->Width < 0)
	{
		zeError("Corrupted or malicious BMP file. Negative width.");
		return false;
	}

	if (Width > 65536 || Height > 65536)
	{
		zeError("Corrupted or malicious BMP file. Width or height of a BMP file can not exceed 65536.");
		return false;
	}

	if (Width * Height * 4 == 0)
	{
		zeError("Corrupted or malicious BMP file. 32bit size overflow detected.");
		return false;
	}

	if (Header->DataSize > FileSize) 
	{
		zeError("Corrupted or malicious BMP file. File size is too small.");
		return false;
	}

	if (Header->CompressionType != 3)
	{
		if (Header->BitsPerPixel == 16) 
		{
			Header->RedMask   = ZEEndian::Big(0x00007C00);
			Header->GreenMask = ZEEndian::Big(0x000003E0); 
			Header->BlueMask  = ZEEndian::Big(0x0000001F);
		}
		else if (Header->BitsPerPixel == 24)
		{
			Header->RedMask   = ZEEndian::Big(0x00FF0000); 
			Header->GreenMask = ZEEndian::Big(0x0000FF00); 
			Header->BlueMask  = ZEEndian::Big(0x000000FF);
		}
		else if (Header->BitsPerPixel == 32)
		{
			Header->RedMask   = ZEEndian::Big(0x00FF0000); 
			Header->GreenMask = ZEEndian::Big(0x0000FF00); 
			Header->BlueMask  = ZEEndian::Big(0x000000FF);
		}
	}

	if (Header->BitsPerPixel != 8 && Header->BitsPerPixel != 16 && Header->BitsPerPixel != 24 && Header->BitsPerPixel != 32)
	{
		zeError("Unsuppored BMP file pixel format. Only 8, 16, 24 and 32 bit pixel formats supported.");
		return false;
	}
	
	switch(Header->CompressionType)
	{
		case 0:
			break;

		case 1:
			if (Header->BitsPerPixel != 8)
			{
				zeError("Unsupported compressed format.");
				return false;
			}
			break;

		case 3:
			if (!(Header->BitsPerPixel == 16 && 
				Header->RedMask   == ZEEndian::Big(0x0000F800) && 
				Header->GreenMask == ZEEndian::Big(0x000007E0) && 
				Header->BlueMask  == ZEEndian::Big(0x0000001F)) &&
				!(Header->BitsPerPixel == 16 &&
				Header->RedMask   == ZEEndian::Big(0x00007C00) && 
				Header->GreenMask == ZEEndian::Big(0x000003E0) && 
				Header->BlueMask  == ZEEndian::Big(0x0000001F)) &&
				!(Header->BitsPerPixel == 24 &&
				Header->RedMask   == ZEEndian::Big(0x00FF0000) && 
				Header->GreenMask == ZEEndian::Big(0x0000FF00) && 
				Header->BlueMask  == ZEEndian::Big(0x000000FF)) &&
				!(Header->BitsPerPixel == 32 &&
				Header->RedMask   == ZEEndian::Big(0x00FF0000) && 
				Header->GreenMask == ZEEndian::Big(0x0000FF00) && 
				Header->BlueMask  == ZEEndian::Big(0x000000FF)))
			{
				zeError("Unsupported color mask.");
				return false;
			}
			break;

		default:
			zeError("Unsupported encoding method.");
			return false;
	}

	memset(Palette, 0, 256 * sizeof(ZEBGRA32));
	if (Header->BitsPerPixel == 8)
	{
		if (Header->PaletteEntryCount == 0)
			Header->PaletteEntryCount = 256;
		
		if (Header->PaletteEntryCount > 256)
		{
			zeError("Only 256 pallette entries supported.");
			return false;
		}

		File->Seek(14 + Header->Size, ZE_SF_BEGINING);
		if (File->Read(Palette, sizeof(ZEBGRA32), Header->PaletteEntryCount) != Header->PaletteEntryCount)
		{
			zeError("Can not load BMP file's palate entries.");
			return false;
		}

		ZETexturePixelConverter::ConvertBGRA8(Palette, Palette, Header->PaletteEntryCount);
	}

	return true;
}

static ZETextureData* Load(ZEFile* File)
{
	ZEBitmapHeader Header;
	ZEBGRA32 Palette[256];

	if (!LoadHeader(File, &Header, Palette))
		return NULL;

	ZETextureData* Data = LoadData(File, &Header, Palette);

	if (Data == NULL)
		zeError("Can not load pixel data.");
	
	return Data;
}

bool ZETextureFileBMP::LoadInfo(ZETextureDataInfo* Info, ZEFile* File)
{
	if (File == NULL)
		return false;

	ZEBGRA32 Palette[256];
	ZEBitmapHeader Header;
	if (!LoadHeader(File, &Header, Palette))
		return false;

	Info->Width = ZEMath::Abs((ZEInt16)Header.Width);
	Info->Height = ZEMath::Abs((ZEInt16)Header.Height);
	Info->PixelFormat = ZE_TPF_I8_4;
	Info->Type = ZE_TT_2D;
	Info->SurfaceCount = 1;
	Info->LevelCount = 1;

	return true;
}

ZETextureData* ZETextureFileBMP::Load(ZEFile* File)
{
	if (File == NULL)
	{
		zeError("Null file pointer.");
		return NULL;
	}

	ZETextureData* Texture = ::Load(File);
	if (Texture == NULL)
		zeError("Can not load BMP file.");

	return Texture;
}
