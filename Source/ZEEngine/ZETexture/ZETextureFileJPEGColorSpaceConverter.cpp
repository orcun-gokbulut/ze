//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureFileJPEGColorSpaceConverter.cpp
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
#include "ZEMath\ZEMath.h"
#include "ZETextureData.h"
#include "ZETextureFileJPEGColorSpaceConverter.h"

void ZEJpegColorSpaceConverter::NoChange(void* OutputBuffer, void* InputBuffer)
{

}

// Copies same single channel to the all channels of rgb buffer
void ZEJpegColorSpaceConverter::GrayScaleToRgb(void* OutputBuffer, void* InputBuffer)
{
	zeDebugCheck(OutputBuffer == NULL || InputBuffer == NULL, "Null pointer...");

	ZESize Width = Info->ImageWidth;
	ZESize Height = Info->ImageHeight;
	for (ZESize Column = 0; Column < Height; ++Column)
	{
		// Source and destination row pointers
		ZEUInt8* DestRowPtr = (ZEUInt8*)OutputBuffer + Column * Width * ZE_JPEG_RGB_PIXEL_SIZE;
		ZEUInt8* SrcRowPtr = (ZEUInt8*)InputBuffer + Column * Width * ZE_JPEG_GRAY_PIXEL_SIZE;;

		for (ZESize Row = 0; Row < Width; ++Row)
		{
			// Source and destination pixel pointers
			ZEUInt8* DestPixelPtr = DestRowPtr + Row * ZE_JPEG_RGB_PIXEL_SIZE;
			ZEUInt8* SrcPixelPtr = DestRowPtr + Row * ZE_JPEG_GRAY_PIXEL_SIZE;

			DestPixelPtr[ZE_JPEG_RGB_R_INDEX] = SrcPixelPtr[ZE_JPEG_GRAY_Y_INDEX];
			DestPixelPtr[ZE_JPEG_RGB_G_INDEX] = SrcPixelPtr[ZE_JPEG_GRAY_Y_INDEX];
			DestPixelPtr[ZE_JPEG_RGB_B_INDEX] = SrcPixelPtr[ZE_JPEG_GRAY_Y_INDEX];
		}
	}
}

// Converts gray scale image to YCbCr by only copying Gray value into Y Component of destination
void ZEJpegColorSpaceConverter::GrayScaleToYCbCr(void* OutputBuffer, void* InputBuffer)
{
	zeDebugCheck(OutputBuffer == NULL || InputBuffer == NULL, "Null pointer...");

	ZESize Width = Info->ImageWidth;
	ZESize Height = Info->ImageHeight;
	for (ZESize Column = 0; Column < Height; ++Column)
	{
		// Source and destination row pointers
		ZEUInt8* DestRowPtr = (ZEUInt8*)OutputBuffer + Column * Width * ZE_JPEG_YCBCR_PIXEL_SIZE;
		ZEUInt8* SrcRowPtr = (ZEUInt8*)InputBuffer + Column * Width * ZE_JPEG_GRAY_PIXEL_SIZE;

		for (ZESize Row = 0; Row < Width; ++Row)
		{
			// Source and destination pixel pointers
			ZEUInt8* DestPixelPtr = DestRowPtr + Row * ZE_JPEG_YCBCR_PIXEL_SIZE;
			ZEUInt8* SrcPixelPtr = DestRowPtr + Row * ZE_JPEG_GRAY_PIXEL_SIZE;

			DestPixelPtr[ZE_JPEG_YCBCR_Y_INDEX] = SrcPixelPtr[ZE_JPEG_GRAY_Y_INDEX];
		}
	}
}

// Converts rgb source image to Gray Scale image
void ZEJpegColorSpaceConverter::RgbToGrayScale(void* OutputBuffer, void* InputBuffer)
{
	zeDebugCheck(OutputBuffer == NULL || InputBuffer == NULL, "Null pointer...");

	ZESize Width = Info->ImageWidth;
	ZESize Height = Info->ImageHeight;
	for (ZESize Column = 0; Column < Height; ++Column)
	{
		// Source and destination row pointers
		ZEUInt8* DestRowPtr = (ZEUInt8*)OutputBuffer + Column * Width * ZE_JPEG_GRAY_PIXEL_SIZE;
		ZEUInt8* SrcRowPtr = (ZEUInt8*)InputBuffer + Column * Width * ZE_JPEG_RGB_PIXEL_SIZE;

		for (ZESize Row = 0; Row < Width; ++Row)
		{
			// Source and destination pixel pointers
			ZEUInt8* DestPixelPtr = DestRowPtr + Row * ZE_JPEG_GRAY_PIXEL_SIZE;
			ZEUInt8* SrcPixelPtr = DestRowPtr + Row * ZE_JPEG_RGB_PIXEL_SIZE;

			DestPixelPtr[ZE_JPEG_GRAY_Y_INDEX] = (ZEUInt8)(	(float)SrcPixelPtr[ZE_JPEG_RGB_R_INDEX] * 0.299f + 
															(float)SrcPixelPtr[ZE_JPEG_RGB_G_INDEX] * 0.587f +  
															(float)SrcPixelPtr[ZE_JPEG_RGB_B_INDEX] * 0.114f	);

		}
	}
}

// Converts rgb to YCbCr
void ZEJpegColorSpaceConverter::RgbToYCbCr(void* OutputBuffer, void* InputBuffer)
{
	zeDebugCheck(OutputBuffer == NULL || InputBuffer == NULL, "Null pointer...");


	//Y	=  0.29900 * R + 0.58700 * G + 0.11400 * B
	//Cb	= -0.16874 * R - 0.33126 * G + 0.50000 * B + 128
	//Cr	=  0.50000 * R - 0.41869 * G - 0.08131 * B + 128


	ZESize Width = Info->ImageWidth;
	ZESize Height = Info->ImageHeight;
	for (ZESize Column = 0; Column < Height; ++Column)
	{
		// Source and destination row pointers
		ZEUInt8* DestRowPtr = (ZEUInt8*)OutputBuffer + Column * Width * ZE_JPEG_YCBCR_PIXEL_SIZE;
		ZEUInt8* SrcRowPtr = (ZEUInt8*)InputBuffer + Column * Width * ZE_JPEG_RGB_PIXEL_SIZE;

		for (ZESize Row = 0; Row < Width; ++Row)
		{
			// Source and destination pixel pointers
			ZEUInt8* DestPixelPtr = DestRowPtr + Row * ZE_JPEG_YCBCR_PIXEL_SIZE;
			ZEUInt8* SrcPixelPtr = DestRowPtr + Row * ZE_JPEG_RGB_PIXEL_SIZE;

			float R = (float)SrcPixelPtr[ZE_JPEG_RGB_R_INDEX];
			float G = (float)SrcPixelPtr[ZE_JPEG_RGB_G_INDEX];
			float B = (float)SrcPixelPtr[ZE_JPEG_RGB_B_INDEX];

			DestPixelPtr[ZE_JPEG_YCBCR_Y_INDEX]  = (ZEUInt8)( 0.29900f * R + 0.58700f * G * 0.11400f * B);
			DestPixelPtr[ZE_JPEG_YCBCR_CB_INDEX] = (ZEUInt8)(-0.68740f * R - 0.33126f * G + 0.50000f * B + 128.00000f);
			DestPixelPtr[ZE_JPEG_YCBCR_CR_INDEX] = (ZEUInt8)( 0.50000f * R - 0.41869f * G - 0.08131f * B + 128.00000f);
		}
	}
}


// Copies to Y channel data to destination gray channel
void ZEJpegColorSpaceConverter::YCbCrToGrayScale(void* OutputBuffer, void* InputBuffer)
{
	zeDebugCheck(OutputBuffer == NULL || InputBuffer == NULL, "Null pointer...");

	ZESize Width = Info->ImageWidth;
	ZESize Height = Info->ImageHeight;
	for (ZESize Column = 0; Column < Height; ++Column)
	{
		// Source and destination row pointers
		ZEUInt8* DestRowPtr = (ZEUInt8*)OutputBuffer + Column * Width * ZE_JPEG_GRAY_PIXEL_SIZE;
		ZEUInt8* SrcRowPtr = (ZEUInt8*)InputBuffer + Column * Width * ZE_JPEG_YCBCR_PIXEL_SIZE;

		for (ZESize Row = 0; Row < Width; ++Row)
		{
			// Source and destination pixel pointers
			ZEUInt8* DestPixelPtr = DestRowPtr + Row * ZE_JPEG_GRAY_PIXEL_SIZE;
			ZEUInt8* SrcPixelPtr = DestRowPtr + Row * ZE_JPEG_YCBCR_PIXEL_SIZE;

			// Copies to Y channel data to destination gray channel
			DestPixelPtr[ZE_JPEG_GRAY_Y_INDEX] = SrcPixelPtr[ZE_JPEG_YCBCR_Y_INDEX];
		}
	}
}

// TESTED OK
void ZEJpegColorSpaceConverter::YCbCrToRgb(void* OutputBuffer, void* InputBuffer)
{
	zeDebugCheck(OutputBuffer == NULL || InputBuffer == NULL, "Null buffer pointer...");

	float Y, Cb, Cr;
	
	// Source and destination row pointers
	ZEBGRA32* OutputPixel = (ZEBGRA32*)OutputBuffer;
	ZEBGRA32* InputPixel = (ZEBGRA32*)InputBuffer;

	ZESize PixelCount = (ZESize)Info->ImageWidth * (ZESize)Info->ImageHeight;
	ZEUInt8* RangeLimit = Info->SampleRangeLimit + ZE_JPEG_MAX_SAMPLE_VALUE + 1;

	for (ZESize PixelN = 0; PixelN < PixelCount; ++PixelN)
	{
		// Get source data
		Y  = (float)InputPixel->B;
		Cb = (float)InputPixel->G;
		Cr = (float)InputPixel->R;

		// Convert to rgb
		OutputPixel->A = 255; // Set alpha to max
		OutputPixel->R = RangeLimit[(ZESize)(Y + 1.40200f * Cr - 179.456f)];
		OutputPixel->G = RangeLimit[(ZESize)(Y - 0.34414f * Cb - 0.71414f * Cr + 135.45984f)];
		OutputPixel->B = RangeLimit[(ZESize)(Y + 1.77200f * Cb - 226.816f)];
		
		++OutputPixel;
		++InputPixel;
		
	}
}



// NOT TESTED
// Converts YCbCr To Ycck by converting to rgb first, then to Ycck, source K channel is copied directly
void ZEJpegColorSpaceConverter::CmykToYcck(void* OutputBuffer, void* InputBuffer)
{
	zeDebugCheck(OutputBuffer == NULL || InputBuffer == NULL, "Null buffer pointer...");

	ZESize Width = Info->ImageWidth;
	ZESize Height = Info->ImageHeight;
	for (ZESize Column = 0; Column < Height; ++Column)
	{
		// Source and destination row pointers
		ZEUInt8* DestRowPtr = (ZEUInt8*)OutputBuffer + Column * Width * ZE_JPEG_YCCK_PIXEL_SIZE;
		ZEUInt8* SrcRowPtr  = (ZEUInt8*)InputBuffer + Column * Width * ZE_JPEG_CMYK_PIXEL_SIZE;

		for (ZESize Row = 0; Row < Width; ++Row)
		{
			// Source and destination pixel pointers
			ZEUInt8* DestPixelPtr = DestRowPtr + Row * ZE_JPEG_YCCK_PIXEL_SIZE;
			ZEUInt8* SrcPixelPtr  = DestRowPtr + Row * ZE_JPEG_CMYK_PIXEL_SIZE;
			
			// Convert to rgb
			float R = 255.0f - (float)SrcPixelPtr[ZE_JPEG_CMYK_C_INDEX];
			float G = 255.0f - (float)SrcPixelPtr[ZE_JPEG_CMYK_M_INDEX];
			float B = 255.0f - (float)SrcPixelPtr[ZE_JPEG_CMYK_Y_INDEX];

			// Convert to ycck
			DestPixelPtr[ZE_JPEG_YCCK_Y_INDEX]  = (ZEUInt8)( 0.29900f * R + 0.58600f * G + 0.11400f * B);
			DestPixelPtr[ZE_JPEG_YCCK_CB_INDEX] = (ZEUInt8)(-0.16874f * R - 0.33126f * G + 0.50000f * B + 128.0f);
			DestPixelPtr[ZE_JPEG_YCCK_CR_INDEX] = (ZEUInt8)( 0.50000f * R - 0.41869f * G - 0.08131f * B + 128.0f);
			DestPixelPtr[ZE_JPEG_YCCK_K_INDEX]  = SrcPixelPtr[ZE_JPEG_CMYK_K_INDEX];

		}
	}
}

// Converts Ycck To Cmyk by converting to rgb first, then to Cmyk
void ZEJpegColorSpaceConverter::YcckToCmyk(void* OutputBuffer, void* InputBuffer)
{
	zeDebugCheck(OutputBuffer == NULL || InputBuffer == NULL, "Null buffer pointer...");

	
	ZESize Width = (ZESize)Info->ImageWidth;
	ZESize Height = (ZESize)Info->ImageHeight;

	// Source and destination row pointers
	ZEBGRA32* OutputPixel = (ZEBGRA32*)OutputBuffer;
	ZEBGRA32* InputPixel = (ZEBGRA32*)InputBuffer;

	unsigned char* RangeLimit = Info->SampleRangeLimit + ZE_JPEG_MAX_SAMPLE_VALUE + 1;

	for (ZESize Column = 0; Column < Height; ++Column)
	{
		for (ZESize Row = 0; Row < Width; ++Row)
		{
			
			float Y	 = (float)InputPixel->B;
			float Cb = (float)InputPixel->G;
			float Cr = (float)InputPixel->R;

			// CMY = 255 - RGB, K is untouched
			OutputPixel->A = InputPixel->A;
			OutputPixel->R = RangeLimit[(ZESize)(255.0f - (Y + 1.40200f * Cr - 179.456f))];
			OutputPixel->G = RangeLimit[(ZESize)(255.0f - (Y - 0.34414f * Cb - 0.71414f * Cr + 135.45984f))];
			OutputPixel->B = RangeLimit[(ZESize)(255.0f - (Y + 1.77200f * Cb - 226.816f))];
			

			++OutputPixel;
			++InputPixel;
		}
	}
}

ZEJpegColorSpaceConverter::ZEJpegColorSpaceConverter()
{
	Convert = NULL;
	Info = NULL;
}

ZEJpegColorSpaceConverter::~ZEJpegColorSpaceConverter()
{
	this->Deinitialize();
}

void ZEJpegColorSpaceConverter::Initialize(ZEJpegDeCompressionInfo* Info)
{
	zeDebugCheck(Info == NULL, "Null Pointer...");
	zeDebugCheck(Info->InputColorSpace == ZE_JPEG_CS_UNKNOWN, "Unknown Color Space");
	zeDebugCheck(Info->OutputColorSpace == ZE_JPEG_CS_UNKNOWN, "Unknown Color Space");

	this->Info = Info;

	// Decide Which conversion function to use
	switch (Info->InputColorSpace)
	{
		case ZE_JPEG_CS_GRAYSCALE:
			switch (Info->OutputColorSpace)
			{
				case ZE_JPEG_CS_RGB:
					Convert = &ZEJpegColorSpaceConverter::GrayScaleToRgb;
					break;
				case ZE_JPEG_CS_YCbCr:
					Convert = &ZEJpegColorSpaceConverter::GrayScaleToYCbCr;
					break;
				case ZE_JPEG_CS_GRAYSCALE:
					Convert = &ZEJpegColorSpaceConverter::NoChange;
					break;
			} // switch (Info->OutputColorSpace)

			break;


		case ZE_JPEG_CS_RGB:
			switch (Info->OutputColorSpace)
			{
				case ZE_JPEG_CS_GRAYSCALE:	
					Convert = &ZEJpegColorSpaceConverter::RgbToGrayScale;
					break;
				case ZE_JPEG_CS_YCbCr:
					Convert = &ZEJpegColorSpaceConverter::RgbToYCbCr;
					break;
				case ZE_JPEG_CS_RGB:
					Convert = &ZEJpegColorSpaceConverter::NoChange;
					break;
			} // switch (Info->OutputColorSpace)

			break;


		case ZE_JPEG_CS_YCbCr:
			switch (Info->OutputColorSpace)
			{
				case ZE_JPEG_CS_GRAYSCALE:
					Convert = &ZEJpegColorSpaceConverter::YCbCrToGrayScale;
					break;
				case ZE_JPEG_CS_RGB:
					Convert = &ZEJpegColorSpaceConverter::YCbCrToRgb;
					break;
				case ZE_JPEG_CS_YCbCr:
					Convert = &ZEJpegColorSpaceConverter::NoChange;
					break;
			} // switch (Info->OutputColorSpace)

			break;
			

		case ZE_JPEG_CS_CMYK:
			switch (Info->OutputColorSpace)
			{
				case ZE_JPEG_CS_YCCK:
					Convert = &ZEJpegColorSpaceConverter::CmykToYcck;
					break;
				case ZE_JPEG_CS_CMYK:
					Convert = &ZEJpegColorSpaceConverter::NoChange;
					break;
			} // switch (Info->OutputColorSpace)

			break;

		case ZE_JPEG_CS_YCCK:
			switch (Info->OutputColorSpace)
			{
				case ZE_JPEG_CS_CMYK:
					Convert = &ZEJpegColorSpaceConverter::YcckToCmyk;
					break;
				case ZE_JPEG_CS_YCCK:
					Convert = &ZEJpegColorSpaceConverter::NoChange;
					break;
			} // switch (Info->OutputColorSpace)

			break;

	} // switch (Info->InputColorSpace)
}

void ZEJpegColorSpaceConverter::Deinitialize()
{
	//Convert = NULL;
	Info = NULL;
}

void ZEJpegColorSpaceConverter::Destroy()
{
	delete this;
}

ZEJpegColorSpaceConverter*	ZEJpegColorSpaceConverter::CreateInstance()
{
	return new ZEJpegColorSpaceConverter();
}
