//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureFileJPEG.cpp
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

#include "ZETextureFileJPEG.h"
#include "ZEMath\ZEMath.h"
#include "ZEError.h"
#include "ZEFile\ZEFile.h"
#include "ZEEndian.h"
#include "ZETextureData.h"


/************************************************************************/
/*                        ZEJpegColorSpaceConverter                     */
/************************************************************************/


// OK
// Copies same single channel to the all channels of rgb buffer
static void ConvertGrayScaleToRgb(void* OutputBuffer, void* InputBuffer, ZEUInt Width, ZEUInt Height)
{
	// Check parameters
	zeAssert(Width == 0 || Height == 0, "Zero dimension...");
	zeAssert(OutputBuffer == NULL || InputBuffer == NULL, "Null buffer pointer...");

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

// OK
// Converts gray scale image to YCbCr by only copying Gray value into Y Component of destination
static void ConvertGrayScaleToYCbCr(void* OutputBuffer, void* InputBuffer, ZEUInt Width, ZEUInt Height)
{
	// Check parameters
	zeAssert(Width == 0 || Height == 0, "Zero dimension...");
	zeAssert(OutputBuffer == NULL || InputBuffer == NULL, "Null buffer pointer...");

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

// OK
// Only converts gray data into K data and writes to destination other destination channels remains untouched
static void ConvertGrayScaleToCmyk(void* OutputBuffer, void* InputBuffer, ZEUInt Width, ZEUInt Height)
{
	// Check parameters
	zeAssert(Width == 0 || Height == 0, "Zero dimension...");
	zeAssert(OutputBuffer == NULL || InputBuffer == NULL, "Null buffer pointer...");

	for (ZESize Column = 0; Column < Height; ++Column)
	{
		// Source and destination row pointers
		ZEUInt8* DestRowPtr = (ZEUInt8*)OutputBuffer + Column * Width * ZE_JPEG_CMYK_PIXEL_SIZE;
		ZEUInt8* SrcRowPtr = (ZEUInt8*)InputBuffer + Column * Width * ZE_JPEG_GRAY_PIXEL_SIZE;

		for (ZESize Row = 0; Row < Width; ++Row)
		{
			// Source and destination pixel pointers
			ZEUInt8* DestPixelPtr = DestRowPtr + Row * ZE_JPEG_CMYK_PIXEL_SIZE;
			ZEUInt8* SrcPixelPtr = DestRowPtr + Row * ZE_JPEG_GRAY_PIXEL_SIZE;

			// Convert 0-255 gray data to 0-100 K data
			DestPixelPtr[ZE_JPEG_CMYK_K_INDEX] = (ZEUInt8)((1.0f - (float)SrcPixelPtr[ZE_JPEG_GRAY_Y_INDEX] / 255.0f) * 100.0f + 0.5f);
		}
	}
}

// Not needed
static void ConvertGrayScaleToYcck(void* OutputBuffer, void* InputBuffer, ZEUInt Width, ZEUInt Height)
{
	zeCriticalError("Not Implemented...");

	// Check parameters
	zeAssert(Width == 0 || Height == 0, "Zero dimension...");
	zeAssert(OutputBuffer == NULL || InputBuffer == NULL, "Null buffer pointer...");
}

// OK
// Converts rgb source image to Gray Scale image
static void ConvertRgbToGrayScale(void* OutputBuffer, void* InputBuffer, ZEUInt Width, ZEUInt Height)
{
	// Check parameters
	zeAssert(Width == 0 || Height == 0, "Zero dimension...");
	zeAssert(OutputBuffer == NULL || InputBuffer == NULL, "Null buffer pointer...");

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

// OK
// Converts rgb to YCbCr
static void ConvertRgbToYCbCr(void* OutputBuffer, void* InputBuffer, ZEUInt Width, ZEUInt Height)
{
	// Check parameters
	zeAssert(Width == 0 || Height == 0, "Zero dimension...");
	zeAssert(OutputBuffer == NULL || InputBuffer == NULL, "Null buffer pointer...");

	/*
	Y	=  0.29900 * R + 0.58700 * G + 0.11400 * B
	Cb	= -0.16874 * R - 0.33126 * G + 0.50000 * B + 128
	Cr	=  0.50000 * R - 0.41869 * G - 0.08131 * B + 128
	*/

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

// OK
// Converts rgb source image to Gray Scale image
static void ConvertRgbToCmyk(void* OutputBuffer, void* InputBuffer, ZEUInt Width, ZEUInt Height)
{
	// Check parameters
	zeAssert(Width == 0 || Height == 0, "Zero dimension...");
	zeAssert(OutputBuffer == NULL || InputBuffer == NULL, "Null buffer pointer...");

	for (ZESize Column = 0; Column < Height; ++Column)
	{
		// Source and destination row pointers
		ZEUInt8* DestRowPtr = (ZEUInt8*)OutputBuffer + Column * Width * ZE_JPEG_CMYK_PIXEL_SIZE;
		ZEUInt8* SrcRowPtr = (ZEUInt8*)InputBuffer + Column * Width * ZE_JPEG_RGB_PIXEL_SIZE;

		for (ZESize Row = 0; Row < Width; ++Row)
		{
			// Source and destination pixel pointers
			ZEUInt8* DestPixelPtr = DestRowPtr + Row * ZE_JPEG_CMYK_PIXEL_SIZE;
			ZEUInt8* SrcPixelPtr = DestRowPtr + Row * ZE_JPEG_RGB_PIXEL_SIZE;

			// Get CMY from rgb
			float C = (1.0f - ((float)SrcPixelPtr[ZE_JPEG_RGB_R_INDEX] / 255.0f));
			float M = (1.0f - ((float)SrcPixelPtr[ZE_JPEG_RGB_G_INDEX] / 255.0f));
			float Y = (1.0f - ((float)SrcPixelPtr[ZE_JPEG_RGB_B_INDEX] / 255.0f));
			
			float Min = ZEMath::Min(ZEMath::Min(C, M), Y);
			
			// Calculate real CMYK and convert to 0 - 100 scale
			DestPixelPtr[ZE_JPEG_CMYK_C_INDEX] = (ZEUInt8)(((C - Min) / (1.0f - Min)) * 100.0f + 0.5f);
			DestPixelPtr[ZE_JPEG_CMYK_M_INDEX] = (ZEUInt8)(((M - Min) / (1.0f - Min)) * 100.0f + 0.5f);
			DestPixelPtr[ZE_JPEG_CMYK_Y_INDEX] = (ZEUInt8)(((Y - Min) / (1.0f - Min)) * 100.0f + 0.5f);
			DestPixelPtr[ZE_JPEG_CMYK_K_INDEX] = (ZEUInt8)(((Min)				    ) * 100.0f + 0.5f);
		}
	}
}

// Not needed
static void ConvertRgbToYcck(void* OutputBuffer, void* InputBuffer, ZEUInt Width, ZEUInt Height)
{
	zeCriticalError("Not Implemented...");

	// Check parameters
	zeAssert(Width == 0 || Height == 0, "Zero dimension...");
	zeAssert(OutputBuffer == NULL || InputBuffer == NULL, "Null buffer pointer...");
}

// OK
// Copies to Y channel data to destination gray channel
static void ConvertYCbCrToGrayScale(void* OutputBuffer, void* InputBuffer, ZEUInt Width, ZEUInt Height)
{
	// Check parameters
	zeAssert(Width == 0 || Height == 0, "Zero dimension...");
	zeAssert(OutputBuffer == NULL || InputBuffer == NULL, "Null buffer pointer...");

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

// OK
static void ConvertYCbCrToRgb(void* OutputBuffer, void* InputBuffer, ZEUInt Width, ZEUInt Height)
{
	// Check parameters
	zeAssert(Width == 0 || Height == 0, "Zero dimension...");
	zeAssert(OutputBuffer == NULL || InputBuffer == NULL, "Null buffer pointer...");

	/*
	R = Y + 1.40200 * Cr - 179,456
	G = Y - 0.34414 * Cb - 0.71414 * Cr + 135.45984
	B = Y + 1.77200 * Cb - 226.816
	*/

	for (ZESize Column = 0; Column < Height; ++Column)
	{
		// Source and destination row pointers
		ZEUInt8* DestRowPtr = (ZEUInt8*)OutputBuffer + Column * Width * ZE_JPEG_RGB_PIXEL_SIZE;
		ZEUInt8* SrcRowPtr = (ZEUInt8*)InputBuffer + Column * Width * ZE_JPEG_YCBCR_PIXEL_SIZE;

		for (ZESize Row = 0; Row < Width; ++Row)
		{
			// Source and destination pixel pointers
			ZEUInt8* DestPixelPtr = DestRowPtr + Row * ZE_JPEG_RGB_PIXEL_SIZE;
			ZEUInt8* SrcPixelPtr = DestRowPtr + Row * ZE_JPEG_YCBCR_PIXEL_SIZE;

			// Get source data
			float Y  = (float)SrcPixelPtr[ZE_JPEG_YCBCR_Y_INDEX];
			float Cb = (float)SrcPixelPtr[ZE_JPEG_YCBCR_CB_INDEX];
			float Cr = (float)SrcPixelPtr[ZE_JPEG_YCBCR_CR_INDEX];

			// Convert to rgb
			DestPixelPtr[ZE_JPEG_RGB_R_INDEX] = (ZEUInt8)(Y + 1.40200f * Cr - 179.456f);
			DestPixelPtr[ZE_JPEG_RGB_G_INDEX] = (ZEUInt8)(Y - 0.34414f * Cb - 0.71414f * Cr + 135.45984f);
			DestPixelPtr[ZE_JPEG_RGB_B_INDEX] = (ZEUInt8)(Y + 1.77200f * Cb - 226.816f);
		}
	}
}

// Not needed
static void ConvertYCbCrToCmyk(void* OutputBuffer, void* InputBuffer, ZEUInt Width, ZEUInt Height)
{
	zeCriticalError("Not Implemented...");

	// Check parameters
	zeAssert(Width == 0 || Height == 0, "Zero dimension...");
	zeAssert(OutputBuffer == NULL || InputBuffer == NULL, "Null buffer pointer...");
}

// Not needed
static void ConvertYCbCrToYcck(void* OutputBuffer, void* InputBuffer, ZEUInt Width, ZEUInt Height)
{
	zeCriticalError("Not Implemented...");

	// Check parameters
	zeAssert(Width == 0 || Height == 0, "Zero dimension...");
	zeAssert(OutputBuffer == NULL || InputBuffer == NULL, "Null buffer pointer...");
}

// OK
// Convert to rgb data first then gray scale data
static void ConvertCmykToGrayScale(void* OutputBuffer, void* InputBuffer, ZEUInt Width, ZEUInt Height)
{
	// Check parameters
	zeAssert(Width == 0 || Height == 0, "Zero dimension...");
	zeAssert(OutputBuffer == NULL || InputBuffer == NULL, "Null buffer pointer...");
	
	// First convert to rgb then convert to gray scale
	for (ZESize Column = 0; Column < Height; ++Column)
	{
		// Source and destination row pointers
		ZEUInt8* DestRowPtr = (ZEUInt8*)OutputBuffer + Column * Width * ZE_JPEG_GRAY_PIXEL_SIZE;
		ZEUInt8* SrcRowPtr = (ZEUInt8*)InputBuffer + Column * Width * ZE_JPEG_CMYK_PIXEL_SIZE;

		for (ZESize Row = 0; Row < Width; ++Row)
		{
			// Source and destination pixel pointers
			ZEUInt8* DestPixelPtr = DestRowPtr + Row * ZE_JPEG_GRAY_PIXEL_SIZE;
			ZEUInt8* SrcPixelPtr = DestRowPtr + Row * ZE_JPEG_CMYK_PIXEL_SIZE;

			// Get Cmyk values in 0-1 range
			float C = (float)SrcPixelPtr[ZE_JPEG_CMYK_C_INDEX] / 100.0f;
			float M = (float)SrcPixelPtr[ZE_JPEG_CMYK_M_INDEX] / 100.0f;
			float Y = (float)SrcPixelPtr[ZE_JPEG_CMYK_Y_INDEX] / 100.0f;
			float K = (float)SrcPixelPtr[ZE_JPEG_CMYK_K_INDEX] / 100.0f;

			// Calculate rgb values in 0-1 range
			float R = C * (1.0f - K) + K;
			float G = M * (1.0f - K) + K;
			float B = Y * (1.0f - K) + K;

			// Convert to 0-255 scale
			R = (ZEUInt8)((1.0f - R) * 255.0f);
			G = (ZEUInt8)((1.0f - G) * 255.0f);
			B = (ZEUInt8)((1.0f - B) * 255.0f);

			// Calculate Gray scale
			DestPixelPtr[ZE_JPEG_GRAY_Y_INDEX] = (ZEUInt8)(R * 0.299f + G * 0.587f + B * 0.114f);
		}
	}
}

// OK
// Convert Cmyk to Rgb
static void ConvertCmykToRgb(void* OutputBuffer, void* InputBuffer, ZEUInt Width, ZEUInt Height)
{
	// Check parameters
	zeAssert(Width == 0 || Height == 0, "Zero dimension...");
	zeAssert(OutputBuffer == NULL || InputBuffer == NULL, "Null buffer pointer...");

	for (ZESize Column = 0; Column < Height; ++Column)
	{
		// Source and destination row pointers
		ZEUInt8* DestRowPtr = (ZEUInt8*)OutputBuffer + Column * Width * ZE_JPEG_RGB_PIXEL_SIZE;
		ZEUInt8* SrcRowPtr = (ZEUInt8*)InputBuffer + Column * Width * ZE_JPEG_CMYK_PIXEL_SIZE;

		for (ZESize Row = 0; Row < Width; ++Row)
		{
			// Source and destination pixel pointers
			ZEUInt8* DestPixelPtr = DestRowPtr + Row * ZE_JPEG_RGB_PIXEL_SIZE;
			ZEUInt8* SrcPixelPtr = DestRowPtr + Row * ZE_JPEG_CMYK_PIXEL_SIZE;

			// Get Cmyk source data in 0-1 range
			float C = (float)SrcPixelPtr[ZE_JPEG_CMYK_C_INDEX] / 100.0f;
			float M = (float)SrcPixelPtr[ZE_JPEG_CMYK_M_INDEX] / 100.0f;
			float Y = (float)SrcPixelPtr[ZE_JPEG_CMYK_Y_INDEX] / 100.0f;
			float K = (float)SrcPixelPtr[ZE_JPEG_CMYK_K_INDEX] / 100.0f;

			// Convert to rgb in 0-1 range
			float R = C * (1.0f - K) + K;
			float G = M * (1.0f - K) + K;
			float B = Y * (1.0f - K) + K;
			
			// Write to destination after converting range into 0-255
			DestPixelPtr[ZE_JPEG_RGB_R_INDEX] = (ZEUInt8)((1.0f - R) * 255.0f + 0.5f);
			DestPixelPtr[ZE_JPEG_RGB_G_INDEX] = (ZEUInt8)((1.0f - G) * 255.0f + 0.5f);
			DestPixelPtr[ZE_JPEG_RGB_B_INDEX] = (ZEUInt8)((1.0f - B) * 255.0f + 0.5f);
		}
	}
}

// Not needed
static void ConvertCmykToYCbCr(void* OutputBuffer, void* InputBuffer, ZEUInt Width, ZEUInt Height)
{
	zeCriticalError("Not Implemented...");

	// Check parameters
	zeAssert(Width == 0 || Height == 0, "Zero dimension...");
	zeAssert(OutputBuffer == NULL || InputBuffer == NULL, "Null buffer pointer...");
}

// OK
// Converts YCbCr To Ycck by converting to rgb first, then to Ycck, source K channel is copied directly
static void ConvertCmykToYcck(void* OutputBuffer, void* InputBuffer, ZEUInt Width, ZEUInt Height)
{
	// Check parameters
	zeAssert(Width == 0 || Height == 0, "Zero dimension...");
	zeAssert(OutputBuffer == NULL || InputBuffer == NULL, "Null buffer pointer...");

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

// OK
// Only the Y channel of source data is copied into destination
static void ConvertYcckToGrayScale(void* OutputBuffer, void* InputBuffer, ZEUInt Width, ZEUInt Height)
{
	// Check parameters
	zeAssert(Width == 0 || Height == 0, "Zero dimension...");
	zeAssert(OutputBuffer == NULL || InputBuffer == NULL, "Null buffer pointer...");

	for (ZESize Column = 0; Column < Height; ++Column)
	{
		// Source and destination row pointers
		ZEUInt8* DestRowPtr = (ZEUInt8*)OutputBuffer + Column * Width * ZE_JPEG_GRAY_PIXEL_SIZE;
		ZEUInt8* SrcRowPtr  = (ZEUInt8*)InputBuffer + Column * Width * ZE_JPEG_YCCK_PIXEL_SIZE;

		for (ZESize Row = 0; Row < Width; ++Row)
		{
			// Source and destination pixel pointers
			ZEUInt8* DestPixelPtr = DestRowPtr + Row * ZE_JPEG_GRAY_PIXEL_SIZE;
			ZEUInt8* SrcPixelPtr  = DestRowPtr + Row * ZE_JPEG_YCCK_PIXEL_SIZE;

			// Copy Y channel
			DestPixelPtr[ZE_JPEG_GRAY_Y_INDEX]  = SrcPixelPtr[ZE_JPEG_YCCK_Y_INDEX];
			
		}
	}
}

// Converts Ycck To Rgb, Source K channel is thrown away
static void ConvertYcckToRgb(void* OutputBuffer, void* InputBuffer, ZEUInt Width, ZEUInt Height)
{
	// Check parameters
	zeAssert(Width == 0 || Height == 0, "Zero dimension...");
	zeAssert(OutputBuffer == NULL || InputBuffer == NULL, "Null buffer pointer...");

	/*
	R = Y + 1.402 * Cr - 179.456 
	G = Y - 0.34414 * Cb - 0.71414 * Cr + 135.45984 
	B = Y + 1.772 * Cb - 226.816 
	*/

	for (ZESize Column = 0; Column < Height; ++Column)
	{
		// Source and destination row pointers
		ZEUInt8* DestRowPtr = (ZEUInt8*)OutputBuffer + Column * Width * ZE_JPEG_RGB_PIXEL_SIZE;
		ZEUInt8* SrcRowPtr  = (ZEUInt8*)InputBuffer + Column * Width * ZE_JPEG_YCCK_PIXEL_SIZE;

		for (ZESize Row = 0; Row < Width; ++Row)
		{
			// Source and destination pixel pointers
			ZEUInt8* DestPixelPtr = DestRowPtr + Row * ZE_JPEG_RGB_PIXEL_SIZE;
			ZEUInt8* SrcPixelPtr  = DestRowPtr + Row * ZE_JPEG_YCCK_PIXEL_SIZE;

			float Y	 = (float)SrcPixelPtr[ZE_JPEG_YCCK_Y_INDEX];
			float Cb = (float)SrcPixelPtr[ZE_JPEG_YCCK_CB_INDEX];
			float Cr = (float)SrcPixelPtr[ZE_JPEG_YCCK_CR_INDEX];

			// CMY = 255 - RGB, K is thrown away
			DestPixelPtr[ZE_JPEG_RGB_R_INDEX] = (ZEUInt8)(Y + 1.40200f * Cb - 179.456f);
			DestPixelPtr[ZE_JPEG_RGB_G_INDEX] = (ZEUInt8)(Y - 0.34414f * Cb - 0.71414f * Cr + 135.45984f);
			DestPixelPtr[ZE_JPEG_RGB_B_INDEX] = (ZEUInt8)(Y + 1.77200f * Cb - 226.816f);

		}
	}
}

// Not needed
static void ConvertYcckToYCbCr(void* OutputBuffer, void* InputBuffer, ZEUInt Width, ZEUInt Height)
{
	zeCriticalError("Not Implemented...");

	// Check parameters
	zeAssert(Width == 0 || Height == 0, "Zero dimension...");
	zeAssert(OutputBuffer == NULL || InputBuffer == NULL, "Null buffer pointer...");
}

// OK
// Converts Ycck To Cmyk by converting to rgb first, then to Cmyk
static void ConvertYcckToCmyk(void* OutputBuffer, void* InputBuffer, ZEUInt Width, ZEUInt Height)
{
	// Check parameters
	zeAssert(Width == 0 || Height == 0, "Zero dimension...");
	zeAssert(OutputBuffer == NULL || InputBuffer == NULL, "Null buffer pointer...");

	for (ZESize Column = 0; Column < Height; ++Column)
	{
		// Source and destination row pointers
		ZEUInt8* DestRowPtr = (ZEUInt8*)OutputBuffer + Column * Width * ZE_JPEG_CMYK_PIXEL_SIZE;
		ZEUInt8* SrcRowPtr  = (ZEUInt8*)InputBuffer + Column * Width * ZE_JPEG_YCCK_PIXEL_SIZE;

		for (ZESize Row = 0; Row < Width; ++Row)
		{
			// Source and destination pixel pointers
			ZEUInt8* DestPixelPtr = DestRowPtr + Row * ZE_JPEG_CMYK_PIXEL_SIZE;
			ZEUInt8* SrcPixelPtr  = DestRowPtr + Row * ZE_JPEG_YCCK_PIXEL_SIZE;

			float Y	 = (float)SrcPixelPtr[ZE_JPEG_YCCK_Y_INDEX];
			float Cb = (float)SrcPixelPtr[ZE_JPEG_YCCK_CB_INDEX];
			float Cr = (float)SrcPixelPtr[ZE_JPEG_YCCK_CR_INDEX];

			// CMY = 255 - RGB, K is untouched
			DestPixelPtr[ZE_JPEG_CMYK_C_INDEX] = (ZEUInt8)(255.0f - (Y + 1.40200f * Cb - 179.456f));
			DestPixelPtr[ZE_JPEG_CMYK_M_INDEX] = (ZEUInt8)(255.0f - (Y - 0.34414f * Cb - 0.71414f * Cr + 135.45984f));
			DestPixelPtr[ZE_JPEG_CMYK_Y_INDEX] = (ZEUInt8)(255.0f - (Y + 1.77200f * Cb - 226.816f));
			DestPixelPtr[ZE_JPEG_CMYK_K_INDEX] = SrcPixelPtr[ZE_JPEG_YCCK_K_INDEX];

		}
	}
}


// Called at the beginning
bool ZEJpegDeCompressionInfo::SetDefaultParametes()
{
	// Set color space
	switch (ZEJpegDeCompressionInfo::ComponentCount)
	{
		case 1:
			// If input is greyscale output is also greyscale
			ZEJpegDeCompressionInfo::InputColorSpace = ZE_JPEG_CS_GRAYSCALE;
			ZEJpegDeCompressionInfo::OutputColorSpace = ZE_JPEG_CS_GRAYSCALE;
			break;

		case 2:
			zeCriticalError("There cannot be two color components");
			break;
		
		case 3:
			if (ZEJpegDeCompressionInfo::JFIFMarkerFound)
			{
				ZEJpegDeCompressionInfo::InputColorSpace = ZE_JPEG_CS_YCbCr;
				ZEJpegDeCompressionInfo::OutputColorSpace = ZE_JPEG_CS_RGB;
			}
			else if (ZEJpegDeCompressionInfo::AdobeMarkerFound)
			{
				switch (ZEJpegDeCompressionInfo::AdobeTransform)
				{
					case 0:
						ZEJpegDeCompressionInfo::InputColorSpace = ZE_JPEG_CS_RGB;
						ZEJpegDeCompressionInfo::OutputColorSpace = ZE_JPEG_CS_RGB;
						break;
					case 1:
						ZEJpegDeCompressionInfo::InputColorSpace = ZE_JPEG_CS_YCbCr;
						ZEJpegDeCompressionInfo::OutputColorSpace = ZE_JPEG_CS_RGB;
						break;
					default:
						zeWarning("Cannot decide input color space. Assuming YCbCr.");
						ZEJpegDeCompressionInfo::InputColorSpace = ZE_JPEG_CS_YCbCr;
						ZEJpegDeCompressionInfo::OutputColorSpace = ZE_JPEG_CS_RGB;
						break;
				}
			}
			else // If there are no special markers try to guess
			{
				ZEUInt8 Comp0Id = ZEJpegDeCompressionInfo::ComponentInfo[0].ComponentId;
				ZEUInt8 Comp1Id = ZEJpegDeCompressionInfo::ComponentInfo[0].ComponentId;
				ZEUInt8 Comp2Id = ZEJpegDeCompressionInfo::ComponentInfo[0].ComponentId;
				
				if (Comp0Id == 1 && Comp1Id == 2 && Comp2Id == 3)
				{
					// if components are ordered assume it is YCbCr
					ZEJpegDeCompressionInfo::InputColorSpace = ZE_JPEG_CS_YCbCr;
					ZEJpegDeCompressionInfo::OutputColorSpace = ZE_JPEG_CS_RGB;
				}
				else if (Comp0Id == 82 && Comp1Id == 71 && Comp2Id == 66)
				{
					// AscII 'R' 'G' 'B' case
					ZEJpegDeCompressionInfo::InputColorSpace = ZE_JPEG_CS_RGB;
					ZEJpegDeCompressionInfo::OutputColorSpace = ZE_JPEG_CS_RGB;
				}
				else
				{
					zeWarning("Cannot decide input color space. Assuming YCbCr.");
					ZEJpegDeCompressionInfo::InputColorSpace = ZE_JPEG_CS_YCbCr;
					ZEJpegDeCompressionInfo::OutputColorSpace = ZE_JPEG_CS_RGB;
				}
			}
			break;

		case 4:
			if (ZEJpegDeCompressionInfo::AdobeMarkerFound)
			{
				switch (ZEJpegDeCompressionInfo::AdobeTransform)
				{
					case 0:
						ZEJpegDeCompressionInfo::InputColorSpace = ZE_JPEG_CS_CMYK;
						ZEJpegDeCompressionInfo::OutputColorSpace = ZE_JPEG_CS_RGB;
						break;
					case 1:
						ZEJpegDeCompressionInfo::InputColorSpace = ZE_JPEG_CS_YCCK;
						ZEJpegDeCompressionInfo::OutputColorSpace = ZE_JPEG_CS_RGB;
						break;
					default:
						zeWarning("Cannot decide input color space. Assuming YCbCr.");
						ZEJpegDeCompressionInfo::InputColorSpace = ZE_JPEG_CS_YCCK;
						ZEJpegDeCompressionInfo::OutputColorSpace = ZE_JPEG_CS_RGB;
						break;
				}
			}
			else	// If no special markers found
			{
				ZEJpegDeCompressionInfo::InputColorSpace = ZE_JPEG_CS_CMYK;
				ZEJpegDeCompressionInfo::OutputColorSpace = ZE_JPEG_CS_RGB;
			}
			break;

		default:
			zeCriticalError("Cannot decide input/output color space");
			return false;
			break;
	}

	// Other decompression parameters
	ZEJpegDeCompressionInfo::ScaleNumerator			= ZEJpegDeCompressionInfo::DctBlockSize;
	ZEJpegDeCompressionInfo::ScaleDenominator		= ZEJpegDeCompressionInfo::DctBlockSize;
	ZEJpegDeCompressionInfo::DctMethod				= ZE_JPEG_DCTM_FLOATING_POINT;
	ZEJpegDeCompressionInfo::OutputGamma			= 1.0f;
	ZEJpegDeCompressionInfo::FancyUpsampling		= true;
	ZEJpegDeCompressionInfo::BlockSmoothing			= true;
	ZEJpegDeCompressionInfo::OutputComponentCount	= 3;	// Output is always rgb

	return true;

}

// Called just before decompression, decides the decompression parameters
bool ZEJpegDeCompressionInfo::SetDecompressionParametes()
{
	// Check image size
	if (ImageWidth > ZE_JPEG_MAX_IMAGE_WIDTH || ImageWidth > ZE_JPEG_MAX_IMAGE_HEIGHT)
	{
		zeCriticalError("Image is larger than supported width / height");
		return false;
	}

	if (DataPrecision != ZE_JPEG_SUPPORTED_DATA_PRECISION)
	{
		zeCriticalError("Jpeg images other than 8 bit precision is not supported");
		return false;
	}


	if (ComponentCount > ZE_JPEG_MAX_COMPONENT_COUNT)
	{
		zeCriticalError("There are more components than the standard");
		return false;
	}

	// Internal sampling factors, find max h and v sampling factors by checking all components
	MaxHorzSampleFactor = 1;
	MaxVertSampleFactor = 1;

	ZEJpegComponentInfo* CurrentComp;
	for (ZESize I = 0; I < ComponentCount; ++I)
	{
		if (ComponentInfo[I].HorizontalFreq > ZE_JPEG_MAX_SAMPLE_FREQUENCY || ComponentInfo[I].VerticalFreq > ZE_JPEG_MAX_SAMPLE_FREQUENCY)
		{
			zeCriticalError("Sampling frequency of one of the components exceeds max possible frequency.");
			return false;
		}

		MaxHorzSampleFactor = ZEMath::Max(MaxHorzSampleFactor, ComponentInfo[I].HorizontalFreq);
		MaxVertSampleFactor = ZEMath::Max(MaxHorzSampleFactor, ComponentInfo[I].VerticalFreq);
	}

	// Decide block size, natural order and Spectral selection end limit

	// No pseudo SOS marker
	if (IsBaseLine || (IsProgressive && ComponentsInScan)) 
	{
		DctBlockSize = ZE_JPEG_DCT_ELEMENT_COUNT;
		SpectralSelectionEndLimit = ZE_JPEG_DCT_BLOCK_ELEMENT_COUNT - 1;
	}
	else
	{
		switch (SpectralSelectionEnd)
		{
			case 1 * 1 - 1:
				DctBlockSize = 1;
				SpectralSelectionEndLimit = SpectralSelectionEnd;
				break;
			case 2 * 2 - 1:
				DctBlockSize = 2;
				SpectralSelectionEndLimit = SpectralSelectionEnd;
				break;
			case 3 * 3 - 1:
				DctBlockSize = 3;
				SpectralSelectionEndLimit = SpectralSelectionEnd;
				break;
			case 4 * 4 - 1:
				DctBlockSize = 4;
				SpectralSelectionEndLimit = SpectralSelectionEnd;
				break;
			case 5 * 5 - 1:
				DctBlockSize = 5;
				SpectralSelectionEndLimit = SpectralSelectionEnd;
				break;
			case 6 * 6 - 1:
				DctBlockSize = 6;
				SpectralSelectionEndLimit = SpectralSelectionEnd;
				break;
			case 7 * 7 - 1:
				DctBlockSize = 7;
				SpectralSelectionEndLimit = SpectralSelectionEnd;
				break;
			case 8 * 8 - 1:
				DctBlockSize = 8;
				SpectralSelectionEndLimit = SpectralSelectionEnd;
				break;
			case 9 * 9 - 1:
				DctBlockSize = 9;
				SpectralSelectionEndLimit = ZE_JPEG_DCT_BLOCK_ELEMENT_COUNT - 1;
				break;
			case 10 * 10 - 1:
				DctBlockSize = 10;
				SpectralSelectionEndLimit = ZE_JPEG_DCT_BLOCK_ELEMENT_COUNT - 1;
				break;
			case 11 * 11 - 1:
				DctBlockSize = 11;
				SpectralSelectionEndLimit = ZE_JPEG_DCT_BLOCK_ELEMENT_COUNT - 1;
				break;
			case 12 * 12 - 1:
				DctBlockSize = 12;
				SpectralSelectionEndLimit = ZE_JPEG_DCT_BLOCK_ELEMENT_COUNT - 1;
				break;
			case 13 * 13 - 1:
				DctBlockSize = 13;
				SpectralSelectionEndLimit = ZE_JPEG_DCT_BLOCK_ELEMENT_COUNT - 1;
				break;
			case 14 * 14 - 1:
				DctBlockSize = 14;
				SpectralSelectionEndLimit = ZE_JPEG_DCT_BLOCK_ELEMENT_COUNT - 1;
				break;
			case 15 * 15 - 1:
				DctBlockSize = 15;
				SpectralSelectionEndLimit = ZE_JPEG_DCT_BLOCK_ELEMENT_COUNT - 1;
				break;
			case 16 * 16 - 1:
				DctBlockSize = 16;
				SpectralSelectionEndLimit = ZE_JPEG_DCT_BLOCK_ELEMENT_COUNT - 1;
				break;
			default:
				zeCriticalError("Cannot decide dct block size");
				return false;
				break;
		}
	}

	MinDctHorzScaledSize = DctBlockSize;
	MinDctVertScaledSize = DctBlockSize;

	// Compute component dimensions
	for (ZESize I = 0; I < ComponentCount; ++I)
	{
		ComponentInfo[I].DctHorzScaledSize = DctBlockSize;
		ComponentInfo[I].DctVertScaledSize = DctBlockSize;

		ComponentInfo[I].HeightInBlocks = ZEMath::Ceil(ImageWidth * ComponentInfo[I].HorizontalFreq / MaxHorzSampleFactor * DctBlockSize);
		ComponentInfo[I].WidthInBlocks  = ZEMath::Ceil(ImageHeight * ComponentInfo[I].VerticalFreq / MaxVertSampleFactor * DctBlockSize);

		ComponentInfo[I].DownsampledHeight = ZEMath::Ceil(ImageWidth * ComponentInfo[I].HorizontalFreq / MaxHorzSampleFactor);
		ComponentInfo[I].DownsampledWidth = ZEMath::Ceil(ImageHeight * ComponentInfo[I].VerticalFreq / MaxVertSampleFactor);

	}

	// Compute number of fully interleaved MCU rows.
	TotaliMCURowCount = ZEMath::Ceil(ImageHeight / MaxVertSampleFactor * DctBlockSize);

	/* Decide whether file contains multiple scans */
	if (ComponentsInScan < ComponentCount || IsProgressive)
		HasMultipleScan = true;
	else
		HasMultipleScan = false;

	return true;
}

bool ZEJpegDeCompressionInfo::SetScanParameters()
{
	// Noninterleaved (single-component) scan
	if (ComponentsInScan == 1)
	{
		// Only component 0 is used
		MCUCountPerRow = CurrentCompInfo[0]->WidthInBlocks;
		MCURowCountInImage = CurrentCompInfo[0]->HeightInBlocks;

		// For Noninterleaved scan there is always one block per MCU
		CurrentCompInfo[0]->McuWidth = 1;
		CurrentCompInfo[0]->McuHeight = 1;
		CurrentCompInfo[0]->McuBlocks = 1;
		CurrentCompInfo[0]->McuSampleWidth = CurrentCompInfo[0]->DctHorzScaledSize;
		CurrentCompInfo[0]->LastColumtWidth = 1;

		ZEUInt Temp = CurrentCompInfo[0]->HeightInBlocks % CurrentCompInfo[0]->VerticalFreq;
		
		CurrentCompInfo[0]->LastRowHeight = (Temp == 0) ? CurrentCompInfo[0]->VerticalFreq : Temp;

		// Prepare array describing MCU composition
		DctBlocksPerMCU = 1;
		MCUMembership[0] = 0;

	}
	else
	{
		// Interleaved (multi-component) scan

		if (ComponentsInScan > ZE_JPEG_MAX_COMPONENTS_IN_SCAN)
		{
			zeCriticalError("Component count in current scan exceeds the limits.");
			return false;
		}

		MCUCountPerRow = ZEMath::Ceil(ImageWidth / MaxHorzSampleFactor * DctBlockSize);
		MCURowCountInImage = ZEMath::Ceil(ImageHeight / MaxVertSampleFactor * DctBlockSize);

		DctBlocksPerMCU = 0;
		for (ZESize I = 0; I < ComponentsInScan; ++I)
		{
			ZEJpegComponentInfo* Component = CurrentCompInfo[I];

			// Sampling factors give # of blocks of component in each MCU
			Component->McuWidth = Component->HorizontalFreq;
			Component->McuHeight = Component->VerticalFreq;
			Component->McuBlocks = Component->McuWidth * Component->McuHeight;
			Component->McuSampleWidth = Component->McuWidth * Component->DctHorzScaledSize;

			// Figure number of non-dummy blocks in last MCU column & row
			ZEUInt Temp = Component->WidthInBlocks % Component->McuWidth;

			Component->LastColumtWidth = (Temp == 0) ? Component->McuWidth : Temp;

			Temp = Component->HeightInBlocks % Component->McuHeight;

			Component->LastRowHeight = (Temp == 0) ? Component->McuHeight : Temp;

			// Prepare array describing MCU composition
			ZEUInt MCUBlocks = Component->McuBlocks;

			while (MCUBlocks-- > 0)
			{
				MCUMembership[(ZESize)DctBlocksPerMCU++] = (ZEInt)I;
			}
		}



	}

	return true;
}

// Constructor
ZEJpegColorSpaceConverter::ZEJpegColorSpaceConverter()
{

}

// Destructor
ZEJpegColorSpaceConverter::~ZEJpegColorSpaceConverter()
{
	
}

// Converts given input data and writes to output
void ZEJpegColorSpaceConverter::Convert(void* OutputBuffer, ZEJpegColorSpace OutputColorSpace, void* InputBuffer, ZEJpegColorSpace InputColorSpace, ZEUInt ImageWidth, ZEUInt ImageHeight)
{	
	zeAssert(ImageWidth == 0, "Zero Image Dimension...");
	zeAssert(ImageHeight == 0, "Zero Image Dimension...");

	zeAssert(InputBuffer == NULL, "Null Buffer Pointer...");
	zeAssert(OutputBuffer == NULL, "Null Buffer Pointer...");
	
	zeAssert(InputColorSpace == ZE_JPEG_CS_UNKNOWN, "Unknown Color Space");
	zeAssert(OutputColorSpace == ZE_JPEG_CS_UNKNOWN, "Unknown Color Space");
	
	switch (InputColorSpace)
	{
		case ZE_JPEG_CS_GRAYSCALE:
		{
			switch (OutputColorSpace)
			{
				case ZE_JPEG_CS_RGB:
				{
					ConvertGrayScaleToRgb(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
				case ZE_JPEG_CS_YCbCr:
				{
					ConvertGrayScaleToYCbCr(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
				case ZE_JPEG_CS_CMYK:
				{
					ConvertGrayScaleToCmyk(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
				case ZE_JPEG_CS_YCCK:
				{
					ConvertGrayScaleToYcck(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
			}
			break;
		}
		
		case ZE_JPEG_CS_RGB:
		{
			switch (OutputColorSpace)
			{
				case ZE_JPEG_CS_GRAYSCALE:
				{
					ConvertRgbToGrayScale(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
				case ZE_JPEG_CS_YCbCr:
				{
					ConvertRgbToYCbCr(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
				case ZE_JPEG_CS_CMYK:
				{
					ConvertRgbToCmyk(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
				case ZE_JPEG_CS_YCCK:
				{
					ConvertRgbToYcck(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
			}
			break;
		}
		
		case ZE_JPEG_CS_YCbCr:
		{
			switch (OutputColorSpace)
			{
				case ZE_JPEG_CS_GRAYSCALE:
				{
					ConvertYCbCrToGrayScale(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
				case ZE_JPEG_CS_RGB:
				{
					ConvertYCbCrToRgb(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
				case ZE_JPEG_CS_CMYK:
				{
					ConvertYCbCrToCmyk(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
				case ZE_JPEG_CS_YCCK:
				{
					ConvertYCbCrToYcck(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
			}
			break;
		}
		
		case ZE_JPEG_CS_CMYK:
		{
			switch (OutputColorSpace)
			{
				case ZE_JPEG_CS_GRAYSCALE:
				{
					ConvertCmykToGrayScale(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
				case ZE_JPEG_CS_RGB:
				{
					ConvertCmykToRgb(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
				case ZE_JPEG_CS_YCbCr:
				{
					ConvertCmykToYCbCr(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
				case ZE_JPEG_CS_YCCK:
				{
					ConvertCmykToYcck(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
			}
			break;
		}
		
		case ZE_JPEG_CS_YCCK:
		{
			switch (OutputColorSpace)
			{
				case ZE_JPEG_CS_GRAYSCALE:
				{
					ConvertYcckToGrayScale(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
				case ZE_JPEG_CS_RGB:
				{
					ConvertYcckToRgb(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
				case ZE_JPEG_CS_YCbCr:
				{
					ConvertYcckToYCbCr(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
				case ZE_JPEG_CS_CMYK:
				{
					ConvertYcckToCmyk(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
			}
			break;
		}
	}
}

/************************************************************************/
/*                        ZEJpegFrequencyUpsampler                      */
/************************************************************************/

static void UpSampleNoChange444(void* OutputBuffer, void* InputBuffer, ZEUInt ImageWidth, ZEUInt ImageHeight)
{

}

static void UpSampleNoChange422(void* OutputBuffer, void* InputBuffer, ZEUInt ImageWidth, ZEUInt ImageHeight)
{

}

static void UpSampleNoChange420(void* OutputBuffer, void* InputBuffer, ZEUInt ImageWidth, ZEUInt ImageHeight)
{

}

static void UpSampleNoChange411(void* OutputBuffer, void* InputBuffer, ZEUInt ImageWidth, ZEUInt ImageHeight)
{

}

static void UpSampleNoChange410(void* OutputBuffer, void* InputBuffer, ZEUInt ImageWidth, ZEUInt ImageHeight)
{

}

static void UpSample422To444(void* OutputBuffer, void* InputBuffer, ZEUInt ImageWidth, ZEUInt ImageHeight)
{

}

static void UpSample420To444(void* OutputBuffer, void* InputBuffer, ZEUInt ImageWidth, ZEUInt ImageHeight)
{

}

static void UpSample411To444(void* OutputBuffer, void* InputBuffer, ZEUInt ImageWidth, ZEUInt ImageHeight)
{

}

static void UpSample410To444(void* OutputBuffer, void* InputBuffer, ZEUInt ImageWidth, ZEUInt ImageHeight)
{

}

static void UpSample420To422(void* OutputBuffer, void* InputBuffer, ZEUInt ImageWidth, ZEUInt ImageHeight)
{
	zeCriticalError("This conversion is not supported yet");
	return;
}

static void UpSample411To422(void* OutputBuffer, void* InputBuffer, ZEUInt ImageWidth, ZEUInt ImageHeight)
{
	zeCriticalError("This conversion is not supported yet");
	return;
}

static void UpSample411To420(void* OutputBuffer, void* InputBuffer, ZEUInt ImageWidth, ZEUInt ImageHeight)
{
	zeCriticalError("This conversion is not supported yet");
	return;
}

static void UpSample410To422(void* OutputBuffer, void* InputBuffer, ZEUInt ImageWidth, ZEUInt ImageHeight)
{
	zeCriticalError("This conversion is not supported yet");
	return;
}

static void UpSample410To420(void* OutputBuffer, void* InputBuffer, ZEUInt ImageWidth, ZEUInt ImageHeight)
{
	zeCriticalError("This conversion is not supported yet");
	return;
}

static void UpSample410To411(void* OutputBuffer, void* InputBuffer, ZEUInt ImageWidth, ZEUInt ImageHeight)
{
	zeCriticalError("This conversion is not supported yet");
	return;
}

void ZEJpegFrequencyUpsampler::UpSample(void* OutputBuffer, ZEJpegSampleFrequency InputFrequency, void* InputBuffer, ZEJpegSampleFrequency OutputFrequency, ZEUInt ImageWidth, ZEUInt ImageHeight)
{
	zeAssert(ImageWidth == 0, "Zero Image Dimension...");
	zeAssert(ImageHeight == 0, "Zero Image Dimension...");

	zeAssert(InputBuffer == NULL, "Null Buffer Pointer...");
	zeAssert(OutputBuffer == NULL, "Null Buffer Pointer...");

	zeAssert(InputFrequency == ZE_JPEG_SF_UNKNOWN, "Unknown Frequency");
	zeAssert(OutputFrequency == ZE_JPEG_SF_UNKNOWN, "Unknown Frequency");

	switch (InputFrequency)
	{
		case ZE_JPEG_SF_4_4_4:
		{
			switch (OutputFrequency)
			{
				case ZE_JPEG_SF_4_4_4:
				{
					UpSampleNoChange444(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
				case ZE_JPEG_SF_4_2_2:
				case ZE_JPEG_SF_4_2_0:
				case ZE_JPEG_SF_4_1_1:
				case ZE_JPEG_SF_4_1_0:
				{
					zeCriticalError("Frequency down sampling is not supported in this module");
					break;
				}
			}
			break;
		}
			
		case ZE_JPEG_SF_4_2_2:
		{
			switch (OutputFrequency)
			{
				case ZE_JPEG_SF_4_4_4:
				{
					break;
				}
				case ZE_JPEG_SF_4_2_2:
				{
					UpSampleNoChange422(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
				case ZE_JPEG_SF_4_2_0:
				case ZE_JPEG_SF_4_1_1:
				case ZE_JPEG_SF_4_1_0:
				{
					zeCriticalError("Frequency down sampling is not supported in this module");
					break;
				}
			}
			break;
		}
		case ZE_JPEG_SF_4_2_0:
		{
			switch (OutputFrequency)
			{
				case ZE_JPEG_SF_4_4_4:
				{
					UpSample420To444(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
				case ZE_JPEG_SF_4_2_2:
				{
					UpSample420To422(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
				case ZE_JPEG_SF_4_2_0:
				{
					UpSampleNoChange420(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
				case ZE_JPEG_SF_4_1_1:
				case ZE_JPEG_SF_4_1_0:
				{
					zeCriticalError("Frequency down sampling is not supported in this module");
					break;
				}
			}
			break;
		}
		case ZE_JPEG_SF_4_1_1:
		{
			switch (OutputFrequency)
			{
				case ZE_JPEG_SF_4_4_4:
				{
					UpSample411To444(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
				case ZE_JPEG_SF_4_2_2:
				{
					UpSample411To422(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
				case ZE_JPEG_SF_4_2_0:
				{
					UpSample411To420(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
				case ZE_JPEG_SF_4_1_1:
				{
					UpSampleNoChange411(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
				case ZE_JPEG_SF_4_1_0:
				{
					zeCriticalError("Frequency down sampling is not supported in this module");
					break;
				}
			}
			break;
		}
		case ZE_JPEG_SF_4_1_0:
		{
			switch (OutputFrequency)
			{
				case ZE_JPEG_SF_4_4_4:
				{
					UpSample410To444(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
				
				case ZE_JPEG_SF_4_2_2:
				{
					UpSample410To422(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
				case ZE_JPEG_SF_4_2_0:
				{
					UpSample410To420(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
				case ZE_JPEG_SF_4_1_1:
				{
					UpSample410To411(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
				case ZE_JPEG_SF_4_1_0:
				{
					UpSampleNoChange410(OutputBuffer, InputBuffer, ImageWidth, ImageHeight);
					break;
				}
			}
			break;
		}
	}
			
}			
			
ZEJpegFrequencyUpsampler::ZEJpegFrequencyUpsampler()
{

}

ZEJpegFrequencyUpsampler::~ZEJpegFrequencyUpsampler()
{

}

/************************************************************************/
/*                        ZEJpegFileMarkerBuffer                        */
/************************************************************************/

// Gets data from image file and fills the buffer
bool ZEJpegFileMarkerBuffer::FillBuffer()
{
	if (BytesAvailable != 0)
		return true;

	// Try to fill whole buffer
	if ((ZEUInt8)ImageFile->Read((void*)Buffer, ZE_JPEG_INPUT_BUFFER_SIZE, 1) != 1)
	{
		// There might be less available data than the ZE_JPEG_INPUT_BUFFER_SIZE
		ZESize Size = ImageFile->GetFileSize() - ImageFile->Tell();
		
		if (ImageFile->Read(Buffer + (ZE_JPEG_INPUT_BUFFER_SIZE - Size), Size, 1) != 1)
		{
			zeCriticalError("Cannot buffer the jpeg file...");
			return false;
		}
		
		BytesAvailable = (ZEUInt)Size;
		return true;
		
	}
	
	BytesAvailable = (ZEUInt)ZE_JPEG_INPUT_BUFFER_SIZE;
	return true;
	
}

// Skips data equal to DataLenght and refills the buffer
bool ZEJpegFileMarkerBuffer::SkipBufferData(ZEUInt32 DataLenght)
{
	if (DataLenght <= BytesAvailable)
	{
		BytesAvailable -= (ZESize)DataLenght;
		return true;
	}
	else
	{
		BytesAvailable = 0;
		
		// Go to new buffer start position
		if (!ImageFile->Seek((ZESize)DataLenght - BytesAvailable, ZE_SF_CURRENT))
		{
			zeCriticalError("Cannot skip data...");
			return false;
		}

		// Fill buffer again
		if (!this->FillBuffer())
			return false;

		return true;
	}
	
	return false;
}

ZEJpegFileMarkerBuffer::ZEJpegFileMarkerBuffer()
{
	ImageFile		= NULL;
	BytesAvailable	= 0;
}

ZEJpegFileMarkerBuffer::~ZEJpegFileMarkerBuffer()
{
	this->Deinitialize();
}

ZESize ZEJpegFileMarkerBuffer::GetAvailableByteCount() const
{
	return BytesAvailable;
}

// Gets one byte
bool ZEJpegFileMarkerBuffer::GetByte(ZEUInt8& Byte)
{
	// Fill the buffer if needed
	if (!this->FillBuffer())
		return false;

	Byte = Buffer[ZE_JPEG_INPUT_BUFFER_SIZE - BytesAvailable];
	
	BytesAvailable -= 1;
	return true;
}

// Gets two arbitrary bytes one by one
bool ZEJpegFileMarkerBuffer::GetTwoByte(ZEUInt8& Byte1, ZEUInt8& Byte2)
{
	if (!GetByte(Byte1))
		return false;
	if (!GetByte(Byte2))
		return false;
	
	return true;
}

// Gets two arbitrary byte and converts to little endian
bool ZEJpegFileMarkerBuffer::GetTwoByte(ZEUInt16& TwoByte)
{
	// Fill the buffer if needed
	if (!this->FillBuffer())
		return false;
	
	bool Result = false;

	// If there are not enough bytes available which is 2
	if (BytesAvailable < 2)
	{
		// Read one by one so the buffer will be filled when BytesAvailable == 0
		ZEUInt8 Byte1 = 0x00, Byte2 = 0x00;

		Result = GetByte(Byte1);
		Result = GetByte(Byte2);

		TwoByte = (ZEUInt16)Byte1 << 8 | (ZEUInt16)Byte2;
		return Result;
	}
	else
	{
		// Read two bytes once and convert to little endian
		ZEUInt16* TwoByteBuffer = (ZEUInt16*)(Buffer + (ZE_JPEG_INPUT_BUFFER_SIZE - BytesAvailable));
		
		TwoByte = ZEEndian::Swap(TwoByteBuffer[0]);
		BytesAvailable -= 2;

		return true;
	}

	return Result;
}

// Skips a variable length marker that file pointer is pointing right now
bool ZEJpegFileMarkerBuffer::SkipVariableLenghtMarker()
{
	ZEUInt16 DataLenght = 0;

	// Get data length
	if (!this->GetTwoByte(DataLenght))
		return false;

	// Subtract marker size
	DataLenght -= 2;

	if (!this->SkipBufferData((ZEUInt32)DataLenght))
		return false;

	return true;
}

// Checks the file byte by byte for a marker
bool ZEJpegFileMarkerBuffer::GetNextMarker(ZEJpegFileMarker& Marker)
{
	ZEUInt8 Byte;
	bool Found = false;

	while (true)
	{
		// Find a byte which is 0xFF (ZE_JPEG_FM_MARKER_BEGIN)
		do 
		{
			if (!this->GetByte(Byte))
				return false;

		} while (Byte != ZE_JPEG_FM_MARKER_BEGIN);

		// Then check if there are more arbitrary 0xFF
		// Arbitrary 0xFF are valid for padding
		do 
		{
			if (!this->GetByte(Byte))
				return false;

		} while (Byte == ZE_JPEG_FM_MARKER_BEGIN);

		// It can be a stuffed-zero data sequence (FF/00). Which can be seen in compressed data
		// If last byte we get is not 0x00 it is a valid marker
		if (Byte != 0x00)
		{
			Found = true;
			break;
		}
	}

	if (Found)
	{
		Marker = (ZEJpegFileMarker)Byte;
	}
	
	return Found;
}

void ZEJpegFileMarkerBuffer::Initialize(ZEFile* ImageFile)
{
	if (ImageFile != NULL && !ImageFile->IsOpen())
	{
		zeCriticalError("Jpeg file is not open");
		return;
	}

	this->ImageFile = ImageFile;

	ImageFile->Seek(0, ZE_SF_BEGINING);
}

void ZEJpegFileMarkerBuffer::Deinitialize()
{
	ImageFile		= NULL;
	BytesAvailable	= 0;
}

void ZEJpegFileMarkerBuffer::Destroy()
{
	delete this;
}

ZEJpegFileMarkerBuffer* ZEJpegFileMarkerBuffer::CreateInstance()
{
	return new ZEJpegFileMarkerBuffer;
}

bool ZEJpegFileMarkerBuffer::ValidateMarker(ZEUInt16 RawMarker)
{
	return ValidateMarker((ZEUInt8)(RawMarker >> 8), (ZEUInt8)RawMarker);
}

bool ZEJpegFileMarkerBuffer::ValidateMarker(ZEUInt16 RawMarker, ZEJpegFileMarker Marker)
{
	return ValidateMarker((ZEUInt8)(RawMarker >> 8), (ZEUInt8)RawMarker, Marker);
}

// Validates if the incoming bytes match with any one of the know file markers
bool ZEJpegFileMarkerBuffer::ValidateMarker(ZEUInt8 Byte1, ZEUInt8 Byte2)
{
	if (Byte1 == ZE_JPEG_FM_MARKER_BEGIN)
	{
		switch (Byte2)
		{
			case ZE_JPG_FM_SOF0:
			case ZE_JPG_FM_SOF1:
			case ZE_JPG_FM_SOF2:
			case ZE_JPG_FM_SOF3:
			case ZE_JPG_FM_SOF5:	
			case ZE_JPG_FM_SOF6:	
			case ZE_JPG_FM_SOF7:	
			case ZE_JPG_FM_JPG:	
			case ZE_JPG_FM_SOF9:	
			case ZE_JPG_FM_SOF10:
			case ZE_JPG_FM_SOF11:
			case ZE_JPG_FM_SOF13:
			case ZE_JPG_FM_SOF14:
			case ZE_JPG_FM_SOF15:
			case ZE_JPG_FM_DHT:
			case ZE_JPG_FM_DAC:
			case ZE_JPG_FM_RST0:	
			case ZE_JPG_FM_RST1:	
			case ZE_JPG_FM_RST2:	
			case ZE_JPG_FM_RST3:	
			case ZE_JPG_FM_RST4:	
			case ZE_JPG_FM_RST5:	
			case ZE_JPG_FM_RST6:	
			case ZE_JPG_FM_RST7:
			case ZE_JPG_FM_SOI:	
			case ZE_JPG_FM_EOI:	
			case ZE_JPG_FM_SOS:	
			case ZE_JPG_FM_DQT:	
			case ZE_JPG_FM_DNL:	
			case ZE_JPG_FM_DRI:	
			case ZE_JPG_FM_DHP:	
			case ZE_JPG_FM_EXP:	
			case ZE_JPG_FM_APP0:	
			case ZE_JPG_FM_APP1:	
			case ZE_JPG_FM_APP2:	
			case ZE_JPG_FM_APP3:	
			case ZE_JPG_FM_APP4:	
			case ZE_JPG_FM_APP5:	
			case ZE_JPG_FM_APP6:	
			case ZE_JPG_FM_APP7:	
			case ZE_JPG_FM_APP8:	
			case ZE_JPG_FM_APP9:	
			case ZE_JPG_FM_APP10:
			case ZE_JPG_FM_APP11:
			case ZE_JPG_FM_APP12:
			case ZE_JPG_FM_APP13:
			case ZE_JPG_FM_APP14:
			case ZE_JPG_FM_APP15: 
			case ZE_JPG_FM_JPG0:
			case ZE_JPG_FM_JPG13:
			case ZE_JPG_FM_COM:
			case ZE_JPG_FM_TEM:
			case ZE_JPG_FM_ERROR:
				return true;
				break;

			default:
				return false;
				break;
		}
	}

	return false;
}

// Validates the incoming bytes are equal to the Marker parameter.
bool ZEJpegFileMarkerBuffer::ValidateMarker(ZEUInt8 Byte1, ZEUInt8 Byte2, ZEJpegFileMarker Marker)
{
	return (Byte1 == ZE_JPEG_FM_MARKER_BEGIN) && (Byte2 == Marker);
}

/************************************************************************/
/*                        ZEJpegFileMarkerReader                        */
/************************************************************************/


// Just checks if the file stars with the SOI marker
bool ZEJpegFileMarkerReader::ProcessSOIMarker(ZEJpegDeCompressionInfo* Info)
{
	// There must be only one SOI
	if (SOIFound)
		return false;

	// Do nothing with SOI
	SOIFound = true;
	return true;
}

// Just checks if the file ends with the EOI marker
bool ZEJpegFileMarkerReader::ProcessEOIMarker(ZEJpegDeCompressionInfo* Info)
{
	// Do nothing with EOI
	EOIFound = true;
	return true;
}

// Loads JFIF data, does its best to get any information. Does not fails the decompression if error occurs
bool ZEJpegFileMarkerReader::ProcessAPP0Marker(ZEJpegDeCompressionInfo* Info)
{
	ZEUInt16 TotalDataLenght = 0;
	ZEUInt8 App0Data[14] = {0};

	MarkerBuffer->GetTwoByte(TotalDataLenght);
	TotalDataLenght -= 2;

	// Get all application data
	MarkerBuffer->GetByte(App0Data[0]);
	MarkerBuffer->GetByte(App0Data[1]);
	MarkerBuffer->GetByte(App0Data[2]);
	MarkerBuffer->GetByte(App0Data[3]);
	MarkerBuffer->GetByte(App0Data[4]);
	

	// Check if it is right
	if (TotalDataLenght >=ZE_JPEG_APP0_DATA_LENGTH && App0Data[0] == 0x4A && App0Data[1] == 0x46 && App0Data[2] == 0x49 && App0Data[3] == 0x46 && App0Data[4] == 0)
	{
		MarkerBuffer->GetByte(App0Data[5]);
		MarkerBuffer->GetByte(App0Data[6]);
		MarkerBuffer->GetByte(App0Data[7]);
		MarkerBuffer->GetByte(App0Data[8]);
		MarkerBuffer->GetByte(App0Data[9]);
		MarkerBuffer->GetByte(App0Data[10]);
		MarkerBuffer->GetByte(App0Data[11]);
		MarkerBuffer->GetByte(App0Data[12]);
		MarkerBuffer->GetByte(App0Data[13]);

		TotalDataLenght -= ZE_JPEG_APP0_DATA_LENGTH;

		// Get thumbnail dimensions and check total size
		if (TotalDataLenght != (ZEUInt16)App0Data[12] * (ZEUInt16)App0Data[13] * (ZEUInt16)3)
		{
			zeWarning("APP0 marker data lenght is wrong. Skipping APP0 data.");
			// Do not get any further data
			return true;
		}

		Info->JFIFMarkerFound = true;
		Info->JFIFVersionMajor = App0Data[5];
		Info->JFIFVersionMinor = App0Data[6];
		Info->DensityUnit = App0Data[7];
		Info->DensityX = (App0Data[8] << 8) + App0Data[9];
		Info->DensityY = (App0Data[10] << 8) + App0Data[11];

		// Always discard thumbnail data
	}
	else if (TotalDataLenght >= 6 && App0Data[0] == 0x4A && App0Data[1] == 0x46 && App0Data[2] == 0x58 && App0Data[3] == 0x58 && App0Data[4] == 0)
	{
		// Found JFIF "JFXX" extension APP0 marker
		// do nothing
		// not supported
		zeWarning("JFIF \"JFXX\" extension is not supported.");

		TotalDataLenght -= 5;

		// Skip the remaining data
		if (TotalDataLenght > 0)
			MarkerBuffer->SkipBufferData(TotalDataLenght);
	}
	else
	{
		TotalDataLenght -= 5;

		// Skip the remaining data
		if (TotalDataLenght > 0)
			MarkerBuffer->SkipBufferData(TotalDataLenght);

	}
	
	return true;
}

// Loads adobe data, does its best to get any information. Does not fails the decompression if error occurs
bool ZEJpegFileMarkerReader::ProcessAPP14Marker(ZEJpegDeCompressionInfo* Info)
{
	ZEUInt16 TotalDataLenght = 0;
	ZEUInt8 App0Data[12] = {0};

	MarkerBuffer->GetTwoByte(TotalDataLenght);
	TotalDataLenght -= 2;

	// Get all application data
	MarkerBuffer->GetByte(App0Data[0]);
	MarkerBuffer->GetByte(App0Data[1]);
	MarkerBuffer->GetByte(App0Data[2]);
	MarkerBuffer->GetByte(App0Data[3]);
	MarkerBuffer->GetByte(App0Data[4]);

	if (TotalDataLenght >= ZE_JPEG_APP14_DATA_LENGTH && App0Data[0] == 0x41 && App0Data[1] == 0x64 && App0Data[2] == 0x6F && App0Data[3] == 0x62 && App0Data[4] == 0x65)
	{
		// Adobe marker found
		MarkerBuffer->GetByte(App0Data[5]);
		MarkerBuffer->GetByte(App0Data[6]);
		MarkerBuffer->GetByte(App0Data[7]);
		MarkerBuffer->GetByte(App0Data[8]);
		MarkerBuffer->GetByte(App0Data[9]);
		MarkerBuffer->GetByte(App0Data[10]);
		MarkerBuffer->GetByte(App0Data[11]);

		TotalDataLenght -= ZE_JPEG_APP14_DATA_LENGTH;

		Info->AdobeMarkerFound = true;
		Info->AdobeTransform = App0Data[11];
	}
	else
	{
		zeWarning("APP14 marker data not recognized");

		TotalDataLenght -= 5;

		// Skip the remaining data
		if (TotalDataLenght > 0)
			MarkerBuffer->SkipBufferData(TotalDataLenght);

	}

	return true;
}


// Checks if marker is SOF and reads necessary data
bool ZEJpegFileMarkerReader::ProcessSOFMarker(ZEJpegDeCompressionInfo* Info, bool BaseLine, bool Progressive)
{

	if (SOFFound)
	{
		zeCriticalError("Duplicate SOF marker found.");
		return false;
	}

	ZEUInt8 ComponentFrequency = 0x00;
	ZEUInt16 SOFDataLenght = 0x0000;

	Info->IsHuffman		= true;
	Info->IsBaseLine	= BaseLine;
	Info->IsProgressive = Progressive;
	
	// Get data length and subtract read bytes
	MarkerBuffer->GetTwoByte(SOFDataLenght);
	SOFDataLenght -= 2;

	// Get data precision, width, height component count and subtract read bytes
	MarkerBuffer->GetByte(Info->DataPrecision);
	MarkerBuffer->GetTwoByte(Info->ImageHeight);
	MarkerBuffer->GetTwoByte(Info->ImageWidth);
	MarkerBuffer->GetByte(Info->ComponentCount);
	SOFDataLenght -= 6;

	if (Info->ImageHeight == 0 || Info->ImageWidth == 0 || Info->ComponentCount == 0)
	{
		zeCriticalError("Image width, height or component count is invalid.");
		return false;
	}

	if (SOFDataLenght != Info->ComponentCount * 3)
	{
		zeCriticalError("Wrong data lenght.");
		return false;
	}

	for (ZESize I = 0; I < Info->ComponentCount; I++)
	{
		// Set component index
		Info->ComponentInfo[I].ComponentIndex = (ZEUInt8)I;
		
		// Get component id
		MarkerBuffer->GetByte(Info->ComponentInfo[I].ComponentId);
		
		// Get component's vertical horizontal frequency
		MarkerBuffer->GetByte(ComponentFrequency);
		Info->ComponentInfo[I].HorizontalFreq = (ComponentFrequency >> 4) & 0x0F;
		Info->ComponentInfo[I].VerticalFreq = ComponentFrequency & 0x0F;
		
		// Get quantization table number
		MarkerBuffer->GetByte(Info->ComponentInfo[I].QuantizationTableNo);
	}

	SOFFound = true;
	return true;
}

// Checks if marker is SOS and reads necessary data
bool ZEJpegFileMarkerReader::ProcessSOSMarker(ZEJpegDeCompressionInfo* Info)
{
	ZEUInt8 ScanCompCount = 0;
	ZEUInt16 DataLenght = 0;

	// Return if SOF not found
	if (!SOFFound)
	{
		zeCriticalError("SOF marker not found");
		return false;
	}

	MarkerBuffer->GetTwoByte(DataLenght);
	MarkerBuffer->GetByte(ScanCompCount);

	/* pseudo SOS marker only allowed in progressive mode */
	if (DataLenght != (ScanCompCount * 2 + 6) || ScanCompCount > ZE_JPEG_MAX_COMPONENTS_IN_SCAN || (ScanCompCount == 0 && !Info->IsProgressive))
	{
		zeCriticalError("Unexpected data in SOS marker.");
		return false;
	}

	Info->ComponentsInScan = ScanCompCount;

	// Get component parameters
	for (ZESize I = 0; I < ScanCompCount; ++I)
	{
		ZEUInt8 AcDcTableSelector, ComponentSelector;
		MarkerBuffer->GetByte(ComponentSelector);
		MarkerBuffer->GetByte(AcDcTableSelector);
		
		for (ZESize J = 0; J < Info->ComponentCount; ++J)
		{
			if (ComponentSelector == Info->ComponentInfo[J].ComponentId)
			{
				
				Info->CurrentCompInfo[I] = &Info->ComponentInfo[J];
				
				Info->ComponentInfo[J].DcTableNo = (AcDcTableSelector >> 4) & 0x0F; // Get most significant 4 bits
				Info->ComponentInfo[J].AcTableNo = AcDcTableSelector & 0x0F;		// Get least significant 4 bits

				break;
			}
		}
	}

	ZEUInt8 Byte = 0;
	
	// Get the rest of the scan parameters
	MarkerBuffer->GetByte((ZEUInt8&)Info->SpectralSelectionStart);
	MarkerBuffer->GetByte((ZEUInt8&)Info->SpectralSelectionEnd);
	MarkerBuffer->GetByte(Byte);
	Info->ApproxBitPosHigh = (Byte >> 4) & 0x0F;
	Info->ApproxBitPosLow = Byte & 0x0F;

	// Increase seen sos count
	if (ScanCompCount)
		Info->SeenSosCount++;

	return true;
}

// Checks if marker is DHT and reads necessary data
bool ZEJpegFileMarkerReader::ProcessDHTMarker(ZEJpegDeCompressionInfo* Info)
{
	ZEUInt16 DataLenght = 0;
	ZEUInt8 Index = 0;
	ZEUInt8 Count = 0;
	ZEUInt8 Bits[17] = {0};
	ZEUInt8 HuffmanValues[256] = {0};
	ZEJpegHuffmanTable* TargetTable = NULL;

	MarkerBuffer->GetTwoByte(DataLenght);
	DataLenght -= 2;

	while (DataLenght > 16)
	{
		MarkerBuffer->GetByte(Index);

		Bits[0] = 0;
		Count = 0;
		for (ZESize I = 1; I <= 16; ++I)
		{
			MarkerBuffer->GetByte(Bits[I]);
			Count += Bits[I];
		}

		DataLenght -= 16 + 1;

		if (Count > 256 || (ZEUInt16)Count > DataLenght)
		{
			zeCriticalError("DHT marker data is not correct.");
			return false;
		}

		for (ZESize I = 0; I < Count; ++I)
		{
			MarkerBuffer->GetByte(HuffmanValues[I]);
			DataLenght -= 1;
		}

		if (Index & 0x10) // Then it is AC table
		{
			Index -= 0x10;
			TargetTable = &Info->ACHuffmanTables[Index];
		}
		else // Else it is DC table
		{
			TargetTable = &Info->DCHuffmanTables[Index];
		}

		if (Index > ZE_JPEG_HUFF_TABLE_COUNT)
		{
			zeCriticalError("There are more huffman tables than four.");
			return false;
		}

		// Copy Huffman table data to destination
		memcpy(TargetTable->Bits, Bits, 17);
		memcpy(TargetTable->HuffmanValues, HuffmanValues, Count/*256*/);

	}

	// Data length should be zero
	if (DataLenght != 0)
	{
		zeCriticalError("Extra data found in DHT marker");
		return false;
	}

	return true;
}

// Checks if marker is DQT and reads necessary data
bool ZEJpegFileMarkerReader::ProcessDQTMarker(ZEJpegDeCompressionInfo* Info)
{
	
	ZEUInt8 TableIdentifier = 0;
	ZEUInt8 TablePrecision = 0;
	ZEUInt16 QuantValue = 0;
	ZEUInt16 Count = 0;
	ZEUInt16 DataLenght = 0;
	ZEJpegQuantizationTable* TargetTable = NULL;

	MarkerBuffer->GetTwoByte(DataLenght);
	DataLenght -= 2;

	while (DataLenght > 0)
	{
		MarkerBuffer->GetByte(TableIdentifier);
		DataLenght -= 1;

		TablePrecision = TableIdentifier >> 4;			// Get 4 most significant bits = Table Precision.
		TableIdentifier = TableIdentifier & 0x0F;		// Get 4 least significant bits = Table identifier

		if (TableIdentifier > ZE_JPEG_QUANT_TABLE_COUNT)
		{
			zeCriticalError("More quantization tables than standard amount.");
			return false;
		}

		if (TablePrecision > 0)
		{
			Count = (DataLenght < ZE_JPEG_DCT_BLOCK_ELEMENT_COUNT * 2) ? DataLenght / 2 : ZE_JPEG_DCT_BLOCK_ELEMENT_COUNT;
		}
		else
		{
			Count = (DataLenght < ZE_JPEG_DCT_BLOCK_ELEMENT_COUNT) ? DataLenght : ZE_JPEG_DCT_BLOCK_ELEMENT_COUNT;
		}
		
		// Decide quantization table size and the table to convert to natural order
		const ZESize* ZigZagToNaturalOrder = NULL;
		switch (Count)
		{
			case 2 * 2:
				ZigZagToNaturalOrder = ZeJpegZigZagToNaturalOrder2x2;
				break;
			case 3 * 3:
				ZigZagToNaturalOrder = ZeJpegZigZagToNaturalOrder3x3;
				break;
			case 4 * 4:
				ZigZagToNaturalOrder = ZeJpegZigZagToNaturalOrder4x4;
				break;
			case 5 * 5:
				ZigZagToNaturalOrder = ZeJpegZigZagToNaturalOrder5x5;
				break;
			case 6 * 6:
				ZigZagToNaturalOrder = ZeJpegZigZagToNaturalOrder6x6;
				break;
			case 7 * 7:
				ZigZagToNaturalOrder = ZeJpegZigZagToNaturalOrder7x7;
				break;
			default:
				ZigZagToNaturalOrder = ZeJpegZigZagToNaturalOrder8x8;
				break;
		}

		for (ZESize I = 0; I < Count; ++I)
		{
			if (TablePrecision > 0)
			{
				MarkerBuffer->GetTwoByte(QuantValue);
			}
			else
			{
				MarkerBuffer->GetByte((ZEUInt8&)QuantValue);
			}

			Info->QuantizationTables[TableIdentifier].QuantizationValues[ZigZagToNaturalOrder[I]] = (ZEUInt16)QuantValue;

		}

		
		if (TablePrecision > 0)
			DataLenght -= Count * 2;
		else
			DataLenght -= Count;

	}

	if (DataLenght != 0)
	{
		zeCriticalError("Unexpected data lenght in DQT marker");
		return false;
	}

	return true;
}

// Checks if marker is DRI and reads necessary data
bool ZEJpegFileMarkerReader::ProcessDRIMarker(ZEJpegDeCompressionInfo* Info)
{
	ZEUInt16 RestartInterval = 0;
	ZEUInt16 DataLenght = 0;

	MarkerBuffer->GetTwoByte(DataLenght);

	if (DataLenght != 4)
	{
		zeCriticalError("Wrong DRI Marker data size");
		return false;
	}

	MarkerBuffer->GetTwoByte(RestartInterval);

	Info->RestartInterval = RestartInterval;
	return true;
}

// Checks if marker is RST and reads necessary data
bool ZEJpegFileMarkerReader::ProcessRSTMarker(ZEJpegDeCompressionInfo* Info)
{
	return true;
}

ZEJpegFileMarkerReader::ZEJpegFileMarkerReader()
{
	ImageFile		= NULL;
	MarkerBuffer	= NULL;
	SOFFound		= false;
	SOIFound		= false;
	EOIFound		= false;
	NextRestart		= 0;
}

ZEJpegFileMarkerReader::~ZEJpegFileMarkerReader()
{
	this->Deinitialize();
}

// Reads all supported / known markers and saves the data into ZEJpegDeCompressionInfo
bool ZEJpegFileMarkerReader::ReadMarkers(ZEJpegDeCompressionInfo* Info, ZEJpegFileMarker& StoppedAt)
{
	ZEJpegFileMarker FoundMarker = ZE_JPG_FM_UNKNOWN;

	while (true)
	{
		if (!MarkerBuffer->GetNextMarker(FoundMarker))
			break;

		switch (FoundMarker)
		{
			case ZE_JPG_FM_SOI:
			{
				if (!this->ProcessSOIMarker(Info))
					return false;

				// Continue reading markers
				break;
			}

			case ZE_JPG_FM_SOF0:	// Baseline
			{
				if (!this->ProcessSOFMarker(Info, true, false))
					return false;

				// Continue reading markers
				break;
			}

			case ZE_JPG_FM_SOF1:	// Extended sequential, Huffman
			{
				if (!this->ProcessSOFMarker(Info, false, false))
					return false;

				// Continue reading markers
				break;
			}

			case ZE_JPG_FM_SOF2:	// Progressive, Huffman
			{
				if (!this->ProcessSOFMarker(Info, false, true))
					return false;

				// Continue reading markers
				break;
			}

			case ZE_JPG_FM_SOF9:	// Extended sequential, arithmetic
			case ZE_JPG_FM_SOF10:	// Progressive, arithmetic
			case ZE_JPG_FM_SOF11:	// Lossless, arithmetic
			case ZE_JPG_FM_SOF13:	// Differential sequential, arithmetic
			case ZE_JPG_FM_SOF14:	// Differential progressive, arithmetic
			case ZE_JPG_FM_SOF15:	// Differential lossless, arithmetic
			{
				zeCriticalError("Arithmetic entropy encoding is not supported");

				// Continue reading markers
				break;
			}
			
			// Other unsupported markers
			case ZE_JPG_FM_JPG:		// Lossless, Huffman
			case ZE_JPG_FM_SOF3:	// Differential sequential, Huffman
			case ZE_JPG_FM_SOF5:	// Differential progressive, Huffman
			case ZE_JPG_FM_SOF6:	// Differential lossless, Huffman
			case ZE_JPG_FM_SOF7:	// Reserved for JPEG extensions
			{
				zeCriticalError("Lossless or Differential encoding is not supported");

				// Continue reading markers
				break;
			}

			case ZE_JPG_FM_SOS:
			{
				if (!this->ProcessSOSMarker(Info))
					return false;

				// Sos reached, sos info read from file
				// Let upper level take control and read compressed data from file, decode etc
				StoppedAt = ZE_JPG_FM_SOS;
				return true;

				break;
			}

			case ZE_JPG_FM_EOI:
			{
				if (!this->ProcessEOIMarker(Info))
					return false;

				// EOI reached, sos info read from file
				// Let upper level take control
				StoppedAt = ZE_JPG_FM_EOI;
				return true;

				break;
			}

			case ZE_JPG_FM_DAC:
			{
				if (!MarkerBuffer->SkipVariableLenghtMarker())
					return false;
				break;
			}

			case ZE_JPG_FM_DHT:
			{
				if (!this->ProcessDHTMarker(Info))
					return false;
				break;
			}

			case ZE_JPG_FM_DQT:
			{
				if (!this->ProcessDQTMarker(Info))
					return false;
				break;
			}

			case ZE_JPG_FM_DRI:
			{
				if (!this->ProcessDRIMarker(Info))
					return false;
				break;
			}

			case ZE_JPG_FM_APP0:	// JFIF Marker
			{
				if (!this->ProcessAPP0Marker(Info))
					return false;
				break;
			}
			
			case ZE_JPG_FM_APP14:
			{
				if (!this->ProcessAPP14Marker(Info))
					return false;
				break;
			}

			case ZE_JPG_FM_APP1:
			case ZE_JPG_FM_APP2:
			case ZE_JPG_FM_APP3:
			case ZE_JPG_FM_APP4:
			case ZE_JPG_FM_APP5:
			case ZE_JPG_FM_APP6:
			case ZE_JPG_FM_APP7:
			case ZE_JPG_FM_APP8:
			case ZE_JPG_FM_APP9:
			case ZE_JPG_FM_APP10:
			case ZE_JPG_FM_APP11:
			case ZE_JPG_FM_APP12:
			case ZE_JPG_FM_APP13:
			
			case ZE_JPG_FM_APP15:
			{
				zeWarning("Jpeg applicaition specific markers except 0 and 14 are not supported");
				if (!MarkerBuffer->SkipVariableLenghtMarker())
					return false;
				break;
			}

			case ZE_JPG_FM_COM:
			{
				zeWarning("Jpeg comment markers is not supported");
				if (!MarkerBuffer->SkipVariableLenghtMarker())
					return false;
				break;
			}

			case ZE_JPG_FM_RST0:	// RST markers are not our concern at this point
			case ZE_JPG_FM_RST1:
			case ZE_JPG_FM_RST2:
			case ZE_JPG_FM_RST3:
			case ZE_JPG_FM_RST4:
			case ZE_JPG_FM_RST5:
			case ZE_JPG_FM_RST6:
			case ZE_JPG_FM_RST7:
			case ZE_JPG_FM_TEM:
			{
				if (!this->ProcessRSTMarker(Info))
					return false;

				// Return control to upper level and let decompressor know a reset marker is found
				StoppedAt = ZE_JPG_FM_RST0;
				return true;
				
				break;
			}

			case ZE_JPG_FM_DNL:		// Skip DNL
			{
				if (!MarkerBuffer->SkipVariableLenghtMarker())
					return false;
				break;
			}

			default:	// Remaining markers are reserved ones: DHP, EXP, JPGn, or RESn. They should not appear in Jpeg file
			{
				zeCriticalError("Reserved marker found in jpeg file. Behaviour is not defined.");
				break;
			}

		}

	}

	return false;
}

bool ZEJpegFileMarkerReader::GetSOFFound() const
{
	return SOFFound;
}

bool ZEJpegFileMarkerReader::GetSOIFound() const
{
	return SOIFound;
}

bool ZEJpegFileMarkerReader::GetEOIFound() const
{
	return EOIFound;
}

void ZEJpegFileMarkerReader::Initialize(ZEFile* ImageFile)
{

	if (ImageFile != NULL && !ImageFile->IsOpen())
	{
		zeCriticalError("Jpeg file is not open");
		return;
	}

	if (MarkerBuffer != NULL || this->ImageFile != NULL)
	{
		zeCriticalError("MarkerReader is already initialized");
		return;
	}

	this->ImageFile = ImageFile;
	ImageFile->Seek(0, ZE_SF_BEGINING);
	
	MarkerBuffer = ZEJpegFileMarkerBuffer::CreateInstance();
	MarkerBuffer->Initialize(ImageFile);

}

void ZEJpegFileMarkerReader::Deinitialize()
{
	if (MarkerBuffer != NULL)
	{
		MarkerBuffer->Destroy();
		MarkerBuffer = NULL;
	}
	
	ImageFile		= NULL;
	SOIFound		= false;
	EOIFound		= false;
	SOFFound		= false;
	NextRestart		= 0;
}

void ZEJpegFileMarkerReader::Destroy()
{
	delete this;
}

ZEJpegFileMarkerReader* ZEJpegFileMarkerReader::CreateInstance()
{
	return new ZEJpegFileMarkerReader();
}


/************************************************************************/
/*                        ZEJpegDecompressor							*/
/************************************************************************/

ZEJpegDecompressor::ZEJpegDecompressor()
{

}

ZEJpegDecompressor::~ZEJpegDecompressor()
{

}

// File pointer should be at the first byte of the compressed data
void ZEJpegDecompressor::Decompress(ZEJpegDeCompressionInfo* Info)
{
	zeCriticalError("Not implemented");
}


/************************************************************************/
/*                        ZEJpegInverseDctProcessor						*/
/************************************************************************/


ZEJpegInverseDctProcessor::ZEJpegInverseDctProcessor()
{

}

ZEJpegInverseDctProcessor::~ZEJpegInverseDctProcessor()
{

}

void ZEJpegInverseDctProcessor::Process(ZEJpegDeCompressionInfo* Info)
{
	zeCriticalError("Not implemented");
}

/************************************************************************/
/*                        ZEJpegInverseQuantizatizator					*/
/************************************************************************/

ZEJpegDeuantizatizator::ZEJpegDeuantizatizator()
{

}

ZEJpegDeuantizatizator::~ZEJpegDeuantizatizator()
{

}

void ZEJpegDeuantizatizator::Process(ZEJpegDeCompressionInfo* Info)
{
	zeCriticalError("Not implemented");
}

/************************************************************************/
/*                        HuffmanDecoderBitBuffer						*/
/************************************************************************/

bool HuffmanDecoderBitBuffer::FillBuffer(bool& EndOfCompression)
{
	if (BitsAvailable != 0)
		return true;

	ZEUInt TotalReadBytes = 0;
	ZEUInt UsefulBytes = 0;

	while (BitsAvailable < HUFFMAN_BIT_BUFFER_MIN_LOAD)
	{
		ZEUInt8 Temp = 0;
		EndOfCompression = false;

		MarkerBuffer->GetByte(Temp);
		TotalReadBytes++;

		// Check if byte is  marker begin 
		// In compressed data if 0xFF is not a marker begin then there must be 0x00 following 0xFF
		if (Temp == 0xFF)
		{
			do 
			{
				MarkerBuffer->GetByte(Temp);
				TotalReadBytes++;

			}while (Temp = 0xFF);

			// Found a stuffed byte the actual data is FF
			if (Temp == 0x00)
			{
				Temp = 0xFF;
			}
			else // Damn found a new marker
			{
				EndOfCompression = true;
				// Go back 2 bytes so that the marker buffer can find the same marker
				
				// Go Back 2 Bytes
				// Go Back 2 Bytes
				// Go Back 2 Bytes
				// Go Back 2 Bytes
				
				if (UsefulBytes == 0)
				{
					// No useful data is read just terminate the decoding
					BitsAvailable = 0;

					// Fill the rest of the buffer with zero

					return false;
				}
				else
				{
					// Let decompressor go on that is left which is UsefulBytes
					return true;
				}

			}
		}

		// Load into the bit buffer
		Buffer = (Buffer << 8) | Temp;
		BitsAvailable += 8;
		UsefulBytes++;

	}

	return true;
}


ZESize HuffmanDecoderBitBuffer::GetAvailableBitCount() const
{
	return BitsAvailable;
}

// Removes bits from buffer
bool HuffmanDecoderBitBuffer::GetBits(ZEUInt32& Bits, ZEUInt32 BitCount)
{
	bool EndCompression = false;

	if (BitsAvailable < BitCount)
		if (!FillBuffer(EndCompression))
			return false;

	Bits = Buffer >> (BitsAvailable -= BitCount);
	return true;
}

// Does not delete the bits
bool HuffmanDecoderBitBuffer::SeeBits(ZEUInt32& Bits, ZEUInt32 BitCount)
{
	bool EndCompression = false;

	if (BitsAvailable < BitCount)
		if (!FillBuffer(EndCompression))
			return false;

	Bits = Buffer >> (BitsAvailable - BitCount);
	return true;
}

// Delete bits from buffer
bool HuffmanDecoderBitBuffer::PassBits(ZEUInt32 BitCount)
{
	bool EndCompression = false;

	if (BitsAvailable < BitCount)
		if (!FillBuffer(EndCompression))
			return false;

	BitsAvailable -= BitCount;
	return true;
}

void HuffmanDecoderBitBuffer::Initialize(ZEJpegFileMarkerBuffer* MarkerBuffer)
{
	this->Buffer	= 0;
	this->BitsAvailable = 0;
	this->MarkerBuffer = MarkerBuffer;

}

void HuffmanDecoderBitBuffer::Deinitialize()
{
	this->Buffer	= 0;
	this->BitsAvailable = 0;
	this->MarkerBuffer = NULL;
}

void HuffmanDecoderBitBuffer::Destroy()
{
	delete this;
}

HuffmanDecoderBitBuffer* HuffmanDecoderBitBuffer::CreateInstance()
{
	return new HuffmanDecoderBitBuffer();
}

HuffmanDecoderBitBuffer::HuffmanDecoderBitBuffer()
{
	this->Buffer	= 0;
	this->BitsAvailable = 0;
	this->MarkerBuffer = NULL;
}

HuffmanDecoderBitBuffer::~HuffmanDecoderBitBuffer()
{
	this->Destroy();
}

/************************************************************************/
/*                        ZEJpegHuffmanDecoder							*/
/************************************************************************/

void ZEJpegHuffmanDecoder::Decode(ZEJpegDeCompressionInfo* Info)
{

}

// Creates the intermediate decoding tables which hold the all possible bit strings whose length are given in Huffman tables
void ZEJpegHuffmanDecoder::Initialize(ZEJpegDeCompressionInfo* Info)
{
	// For progressive mode
	if (Info->IsProgressive)
	{
		// Validate progressive scan parameters
		if (Info->SpectralSelectionStart == 0)
		{
			// Spectral selection end cannot be zero
			if (Info->SpectralSelectionEnd != 0)
			{
				zeCriticalError("Wrong Input parameter for decompressor");
			}
		}
		else
		{
			// Spectral Selection End cannot be smaller than Spectral Selection Start
			if (Info->SpectralSelectionEnd < Info->SpectralSelectionStart || Info->SpectralSelectionEnd > Info->SpectralSelectionEndLimit)
			{
				zeCriticalError("Wrong Input parameter for decompressor");
			}

			// There have to be only 1 component per scan
			if (Info->ComponentsInScan != 1)
			{
				zeCriticalError("Wrong Input parameter for decompressor");
			}
		}

		if (Info->ApproxBitPosHigh != 0)
		{
			if (Info->ApproxBitPosLow != Info->ApproxBitPosHigh - 1)
			{
				zeCriticalError("Wrong Input parameter for decompressor");
			}
		}

		if (Info->ApproxBitPosLow > 13)
		{
			zeCriticalError("Wrong Input parameter for decompressor");
		}

		for (ZESize	I = 0; I < Info->ComponentsInScan; I++)
		{
			ZEUInt8 CurCompIndex = Info->CurrentCompInfo[I]->ComponentIndex;
			ZEInt* CoefBit = &Info->CoefficientBits[CurCompIndex][0];

			for (ZESize Coeff = Info->SpectralSelectionStart; Coeff <= Info->SpectralSelectionEnd; ++Coeff)
			{
					ZEInt Expected = (CoefBit[Coeff] < 0) ? 0 : CoefBit[Coeff];
					if (Info->ApproxBitPosHigh != Expected)
					{
						zeCriticalError("Wrong Input parameter for decompressor");
					}
					CoefBit[Coeff] = Info->ApproxBitPosLow;
			}
		}


		if (Info->ApproxBitPosHigh != 0)
		{
			if (Info->SpectralSelectionStart == 0)
			{
				UseDecodeMcuDcFirstTime = true;
			}
			else
			{
				UseDecodeMcuAcFirstTime = true;
			}
		}
		else
		{
			if (Info->SpectralSelectionStart == 0)
			{
				UseDecodeMcuDcRefine = true;
			}
			else
			{
				UseDecodeMcuAcRefine = true;
			}
		}

		for (ZESize I = 0; I < Info->ComponentsInScan; ++I)
		{
			ZEJpegComponentInfo* ComponentInfo = Info->CurrentCompInfo[I];

			if (Info->SpectralSelectionStart == 0)
			{
				if (Info->ApproxBitPosHigh != 0)
				{
					//this->CreateTable(Info, );
				}
			}
			else
			{

			}
		}

	}
	else
	{

	}
}

void ZEJpegHuffmanDecoder::Deinitialize()
{
	if (BitBuffer != NULL)
	{
		BitBuffer->Destroy();
		BitBuffer = NULL;
	}

	memset(YTableDC, 0, sizeof (ZEInt) * ZE_JPEG_DCT_BLOCK_ELEMENT_COUNT);
	memset(YTableAC, 0, sizeof (ZEInt) * ZE_JPEG_DCT_BLOCK_ELEMENT_COUNT);
	memset(CromTableDC, 0, sizeof (ZEInt) * ZE_JPEG_DCT_BLOCK_ELEMENT_COUNT);
	memset(CromTableAC, 0, sizeof (ZEInt) * ZE_JPEG_DCT_BLOCK_ELEMENT_COUNT);
}

void ZEJpegHuffmanDecoder::Destroy()
{
	delete this;
}

ZEJpegHuffmanDecoder* ZEJpegHuffmanDecoder::CreateInstance()
{
	return new ZEJpegHuffmanDecoder();
}

ZEJpegHuffmanDecoder::ZEJpegHuffmanDecoder()
{
	BitBuffer = NULL;
}

ZEJpegHuffmanDecoder::~ZEJpegHuffmanDecoder()
{
	this->Deinitialize();
}

/************************************************************************/
/*                        ZETextureFileJpeg								*/
/************************************************************************/

ZETextureFileJpeg::ZETextureFileJpeg()
{
	this->ImageFile = NULL;
	
	this->JpegInfo = new ZEJpegDeCompressionInfo();
	memset(JpegInfo, 0, sizeof(ZEJpegDeCompressionInfo));

	this->MarkerReader = ZEJpegFileMarkerReader::CreateInstance();
}

ZETextureFileJpeg::~ZETextureFileJpeg()
{
	zeCriticalError("Not implemented");
}

bool ZETextureFileJpeg::CanLoad(ZEFile* File)
{
	zeCriticalError("Not implemented");
	return false;
}

ZETextureData* ZETextureFileJpeg::Load(ZEFile* File)
{
	if (ImageFile != NULL && !ImageFile->IsOpen())
	{
		zeCriticalError("Jpeg file is not open");
		return NULL;
	}

	// Initialize marker reader
	MarkerReader->Initialize(File);
	ZETextureData* TextureData = new ZETextureData();
	

	// Read markers
	ZEJpegFileMarker StoppedAt = ZE_JPG_FM_UNKNOWN;
	while (true)
	{
		MarkerReader->ReadMarkers(JpegInfo, StoppedAt);
		
		// Image info should be read till now
		// Create TextureData once and set the target data
		if (TextureData->IsEmpty())
		{
			TextureData->Create(ZE_TT_2D, ZE_TPF_I8_4, 1, 1, (ZEUInt)JpegInfo->ImageWidth, (ZEUInt)JpegInfo->ImageHeight);
			JpegInfo->SetDefaultParametes();
			JpegInfo->Destination = TextureData->GetSurfaces().GetItem(0).GetLevels().GetItem(0).GetData();
		}

		switch (StoppedAt)
		{
			case ZE_JPG_FM_SOS:
				// SOS marker found let ZEJpegDecompressor, ZEJpegFrequencyUpsampler, ZEJpegColorSpaceConverter handle the rest
				ZEJpegDecompressor::Decompress(JpegInfo);
				// ZEJpegFrequencyUpsampler::UpSample();
				// ZEJpegColorSpaceConverter::Convert();
				break;

			case ZE_JPG_FM_EOI:
				// Everything went well and we found the EOI
				// Clean up and return
				MarkerReader->Destroy();
				return TextureData;

				break;

			default:
			case ZE_JPG_FM_UNKNOWN:
				zeCriticalError("Cannot load jpeg image file: \"%s\".", ImageFile->GetFilePath().GetValue());
				return NULL;
				break;
		}
	}





}

