//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureFileJPEGChromaUpsampler.cpp
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
#include "ZETextureFileJPEGChromaUpsampler.h"
#include "ZETextureFileJPEGCommonDefinitions.h"

// For grayscale images
void ZEJpegChromaUpsampler::UpSampleSingleComponent(void* Destination, ZEJpegComponentInfo* Component, ZEJpegDataBlock* Source, ZEUInt CopyWidth, ZEUInt CopyHeight, ZEUInt McuCoordX, ZEUInt McuCoordY)
{	
	ZESize RowN, ColN;
	ZESize DestPitch = Info->OutputPitch;
	ZEUInt8* DestRow = (ZEUInt8*)Destination;

	for (RowN = 0; RowN < (ZESize)CopyHeight; ++RowN)
	{
		for (ColN = 0; ColN < (ZESize)CopyWidth; ++ColN)
		{
			DestRow[ColN] = (ZEUInt8)Source->BlockData.TwoDim[RowN][ColN];
		}
		DestRow += DestPitch;
	}
}


void ZEJpegChromaUpsampler::UpSampleWithoutChange(void* Destination, ZEJpegComponentInfo* Component, ZEJpegDataBlock* Source, ZEUInt CopyWidth, ZEUInt CopyHeight, ZEUInt McuCoordX, ZEUInt McuCoordY)
{

}

// One source block is up sampled to maxverticalfreq * maxhorizfreq blocks
void ZEJpegChromaUpsampler::UpSampleH1V1(void* Destination, ZEJpegComponentInfo* Component, ZEJpegDataBlock* Source, ZEUInt CopyWidth, ZEUInt CopyHeight, ZEUInt McuCoordX, ZEUInt McuCoordY)
{
	ZESize RowN, ColN;
	ZESize DataY, DataX;
	ZESize ComponentIndex = (ZESize)Component->ComponentIndex;

	ZEUInt8* DestPixel;
	ZEUInt8* DestRow = (ZEUInt8*)Destination;
	
	for (RowN = 0; RowN < (ZESize)CopyHeight; ++RowN)
	{
		DestPixel = DestRow;
		DataY = RowN / Info->MaxVertSampleFactor;
		
		for (ColN = 0; ColN < (ZESize)CopyWidth; ++ColN)
		{
			DataX = ColN / Info->MaxHorzSampleFactor;
			DestPixel[ComponentIndex] = Source->BlockData.TwoDim[DataY][DataX];
			DestPixel += Info->OutputPixelSize;
		}

		DestRow += Info->OutputPitch;
	}
}

// Two source blocks are up sampled to maxverticalfreq * maxhorizfreq blocks 
void ZEJpegChromaUpsampler::UpSampleH2V1(void* Destination, ZEJpegComponentInfo* Component, ZEJpegDataBlock* Source, ZEUInt CopyWidth, ZEUInt CopyHeight, ZEUInt McuCoordX, ZEUInt McuCoordY)
{	
	ZEUInt8* DestPixel;
	ZEUInt8* DestRow = (ZEUInt8*)Destination;

	ZESize RowN, ColN;
	ZESize DataX, DataY, SrcBlockX;
	ZESize ComponentIndex = (ZESize)Component->ComponentIndex;

	// Create source pointer table
	ZEJpegDataBlock* SourcePtr[1][2];
	SourcePtr[0][0] = Source + 0;
	SourcePtr[0][1] = Source + 1;

	for (RowN = 0; RowN < (ZESize)CopyHeight; ++RowN)
	{
		DataY = RowN % 8;
		DestPixel = DestRow;
		
		for (ColN = 0; ColN < (ZESize)CopyWidth; ++ColN)
		{
			DataX = ColN % 8;
			SrcBlockX = ColN / ZE_JPEG_DCT_BLOCK_DIMENSION;
			DestPixel[ComponentIndex] = SourcePtr[0][SrcBlockX]->BlockData.TwoDim[DataY][DataX];
			DestPixel += Info->OutputPixelSize;
		}

		DestRow += Info->OutputPitch; 
	}
}

// Two source blocks are up sampled to maxverticalfreq * maxhorizfreq blocks 
void ZEJpegChromaUpsampler::UpSampleH1V2(void* Destination, ZEJpegComponentInfo* Component, ZEJpegDataBlock* Source, ZEUInt CopyWidth, ZEUInt CopyHeight, ZEUInt McuCoordX, ZEUInt McuCoordY)
{
	ZESize RowN, ColN;
	ZESize DataX, DataY, SrcBlockY;
	ZESize ComponentIndex = (ZESize)Component->ComponentIndex;

	ZEUInt8* DestPixel;
	ZEUInt8* DestRow = (ZEUInt8*)Destination;

	// Create source pointer table
	ZEJpegDataBlock* SourcePtr[2][1];
	SourcePtr[0][0] = Source + 0;
	SourcePtr[1][0] = Source + 1;

	for (RowN = 0; RowN < (ZESize)CopyHeight; ++RowN)
	{
		DataY = RowN % 8;
		DestPixel = DestRow;
		SrcBlockY = RowN / ZE_JPEG_DCT_BLOCK_DIMENSION;

		for (ColN = 0; ColN < (ZESize)CopyWidth; ++ColN)
		{
			DataX = ColN % 8;
			DestPixel[ComponentIndex] = SourcePtr[SrcBlockY][0]->BlockData.TwoDim[DataY][DataX];
			DestPixel += Info->OutputPixelSize;
		}

		DestRow += Info->OutputPitch; 
	}
}

// Four source blocks are up sampled to maxverticalfreq * maxhorizfreq blocks
void ZEJpegChromaUpsampler::UpSampleH2V2(void* Destination, ZEJpegComponentInfo* Component, ZEJpegDataBlock* Source, ZEUInt CopyWidth, ZEUInt CopyHeight, ZEUInt McuCoordX, ZEUInt McuCoordY)
{
	ZESize RowN, ColN;
	ZESize DataX, DataY;
	ZESize SrcBlockX, SrcBlockY;
	ZESize ComponentIndex = (ZESize)Component->ComponentIndex;
	
	ZEUInt8* DestPixel;
	ZEUInt8* DestRow = (ZEUInt8*)Destination;

	// Create source pointer table
	ZEJpegDataBlock* SourcePtr[2][2];
	SourcePtr[0][0] = Source + 0;
	SourcePtr[0][1] = Source + 1;
	SourcePtr[1][0] = Source + 2;
	SourcePtr[1][1] = Source + 3;

	for (RowN = 0; RowN < (ZESize)CopyHeight; ++RowN)
	{
		DataY = RowN % 8;
		SrcBlockY = RowN / ZE_JPEG_DCT_BLOCK_DIMENSION;
		DestPixel = DestRow;

		for (ColN = 0; ColN < (ZESize)CopyWidth; ++ColN)
		{
			DataX = ColN % 8;
			SrcBlockX = ColN / ZE_JPEG_DCT_BLOCK_DIMENSION;
			DestPixel[ComponentIndex] = SourcePtr[SrcBlockY][SrcBlockX]->BlockData.TwoDim[DataY][DataX];
			DestPixel += Info->OutputPixelSize;
		}

		DestRow += Info->OutputPitch; 
	}
}

ZEJpegChromaUpsampler::ZEJpegChromaUpsampler()
{
	Info = NULL;

	//Clean the function pointers
	memset(UpSamplePointers, 0, sizeof (UpSampleFunction) * ZE_JPEG_MAX_COMPONENT_COUNT);
}

ZEJpegChromaUpsampler::~ZEJpegChromaUpsampler()
{
	this->Deinitialize();
}

void ZEJpegChromaUpsampler::Initialize(ZEJpegDeCompressionInfo* Info)
{
	zeDebugCheck(Info == NULL, "NULL Pointer");

	this->Info = Info;

	//Clean the function pointers
	memset(UpSamplePointers, 0, sizeof (UpSampleFunction) * ZE_JPEG_MAX_COMPONENT_COUNT);

	if (Info->ComponentCount == 1)
	{
		UpSamplePointers[0] = &ZEJpegChromaUpsampler::UpSampleSingleComponent;
	}
	else
	{
		// Create function pointers for each component
		for (ZESize CompN = 0; CompN < Info->ComponentCount; ++CompN)
		{
			ZEJpegComponentInfo* CurComp = Info->CurrentCompInfo[CompN];

			if (CurComp->HorizontalFreq == 1 && CurComp->VerticalFreq == 1)
			{
				UpSamplePointers[CompN] = &ZEJpegChromaUpsampler::UpSampleH1V1;
			}
			else if (CurComp->HorizontalFreq == 2 && CurComp->VerticalFreq == 1)
			{
				UpSamplePointers[CompN] = &ZEJpegChromaUpsampler::UpSampleH2V1;
			}
			else if (CurComp->HorizontalFreq == 1 && CurComp->VerticalFreq == 2)
			{
				UpSamplePointers[CompN] = &ZEJpegChromaUpsampler::UpSampleH1V2;
			}
			else if (CurComp->HorizontalFreq == 2 && CurComp->VerticalFreq == 2)
			{
				UpSamplePointers[CompN] = &ZEJpegChromaUpsampler::UpSampleH2V2;
			}
			else
			{
				zeCriticalError("The chroma subsampling used in this jpeg is not supported");
			}
		}
	}
}

void ZEJpegChromaUpsampler::Deinitialize()
{
	Info = NULL;

	//Clean the function pointers
	memset(UpSamplePointers, 0, sizeof (UpSampleFunction) * ZE_JPEG_MAX_COMPONENT_COUNT);

}

void ZEJpegChromaUpsampler::Destroy()
{
	delete this;
}

ZEJpegChromaUpsampler* ZEJpegChromaUpsampler::CreateInstance()
{
	return new ZEJpegChromaUpsampler();
}
