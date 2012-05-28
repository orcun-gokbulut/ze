//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureFileJPEGCommonDefinitions.h
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


#pragma once
#ifndef __ZE_TEXTURE_FILE_JPEG_COMMON_DEFINITIONS_H__
#define __ZE_TEXTURE_FILE_JPEG_COMMON_DEFINITIONS_H__


#include "ZETypes.h"
#include <intrin.h>


#define ZE_JPEG_MAX_SAMPLE_FREQUENCY		4
#define ZE_JPEG_MAX_COMPONENT_COUNT			10



#define ZE_JPEG_MAX_IMAGE_WIDTH				65500
#define ZE_JPEG_MAX_IMAGE_HEIGHT			65500

// Memory related definitions
#define	ZE_JPEG_INPUT_BUFFER_SIZE	(ZESize)4096

#define ZE_JPEG_MAX_SAMPLE_VALUE			255
#define ZE_JPEG_MID_SAMPLE_VALUE			128

// Index definitions to reach specific channels of a pixel

// Gray scale or Y
#define ZE_JPEG_GRAY_Y_INDEX		(ZESize)0

// RGB
#define ZE_JPEG_RGB_R_INDEX			(ZESize)0
#define ZE_JPEG_RGB_G_INDEX			(ZESize)1
#define ZE_JPEG_RGB_B_INDEX			(ZESize)2

// YCbCr
#define ZE_JPEG_YCBCR_Y_INDEX		(ZESize)0
#define ZE_JPEG_YCBCR_CB_INDEX		(ZESize)1
#define ZE_JPEG_YCBCR_CR_INDEX		(ZESize)2

// CMYK
#define ZE_JPEG_CMYK_C_INDEX		(ZESize)0
#define ZE_JPEG_CMYK_M_INDEX		(ZESize)1
#define ZE_JPEG_CMYK_Y_INDEX		(ZESize)2
#define ZE_JPEG_CMYK_K_INDEX		(ZESize)3

// YCCK
#define ZE_JPEG_YCCK_Y_INDEX		(ZESize)0
#define ZE_JPEG_YCCK_CB_INDEX		(ZESize)1
#define ZE_JPEG_YCCK_CR_INDEX		(ZESize)2
#define ZE_JPEG_YCCK_K_INDEX		(ZESize)3

// Pixel size definitions for different color models
#define ZE_JPEG_GRAY_PIXEL_SIZE		(ZESize)1
#define ZE_JPEG_RGB_PIXEL_SIZE		(ZESize)3
#define ZE_JPEG_YCBCR_PIXEL_SIZE	(ZESize)3
#define ZE_JPEG_YCCK_PIXEL_SIZE		(ZESize)4
#define ZE_JPEG_CMYK_PIXEL_SIZE		(ZESize)4


// Jpeg file specific definitions
#define ZE_JPEG_MAX_BLOCKS_IN_MCU			10
#define ZE_JPEG_QUANT_TABLE_COUNT			4
#define ZE_JPEG_HUFF_TABLE_COUNT			4
#define ZE_JPEG_MAX_SAMPLING_FACTOR			4
#define ZE_JPEG_MAX_COMPONENTS_IN_SCAN		4
#define ZE_JPEG_DCT_BLOCK_DIMENSION			(ZESize)8
#define ZE_JPEG_DCT_BLOCK_COEFF_COUNT		64

// Decoder definitions
#define	HUFFMAN_LOOK_AHEAD_BITS				(ZESize)8	// Bits
#define	HUFFMAN_BIT_BUFFER_SIZE						32	// Bits
#define	HUFFMAN_BIT_BUFFER_MIN_LOAD			(ZESize)25	// Bits
#define ZE_JPEG_RANGE_MASK							255 * 4 + 3

// Bit masks for 16 rightmost bits of 32 bits of an unsigned int. Right aligned
const ZEInt32 BitMask[16] =
{
	0x0000, 0x0001, 0x0003,	0x0007,	0x000F, 0x001F, 0x003F, 0x007F,
	0x00FF, 0x01FF, 0x03FF, 0x07FF, 0x0FFF, 0x1FFF, 0x3FFF, 0x7FFF
};

// Bit access macro for ZEJpegHuffmanDecoder
#define	BITMASK(NthBit)	BitMask[NthBit]

// Extends the value : Converts magnitude and extra bits into coefficient value
#define	EXTEND(ReadBits, BitCount)	( (ReadBits) <= BitMask[(ZESize)(BitCount - 1)] ) ? (ReadBits) - BitMask[(ZESize)BitCount] : (ReadBits) 

enum ZEJpegColorSpace
{
	ZE_JPEG_CS_UNKNOWN		= 0,	// Unknown

	ZE_JPEG_CS_GRAYSCALE	= 1,	// Gray Scale
	ZE_JPEG_CS_RGB			= 2,	// R-G-B
	ZE_JPEG_CS_YCbCr		= 3,	// Y-Cb-Cr (YUV)
	ZE_JPEG_CS_CMYK			= 4,	// C-M-Y-K
	ZE_JPEG_CS_YCCK			= 5,	// Y-Cb-Cr-K
};

// Two byte file markers that are followed by ZE_JPG_FM_MB
enum ZEJpegFileMarker
{
	ZE_JPG_FM_UNKNOWN		= 0x00, 

	ZE_JPG_FM_SOF0			= 0xC0, 
	ZE_JPG_FM_SOF1			= 0xC1,
	ZE_JPG_FM_SOF2			= 0xC2, 
	ZE_JPG_FM_SOF3			= 0xC3,

	ZE_JPG_FM_SOF5			= 0xC5,
	ZE_JPG_FM_SOF6			= 0xC6,
	ZE_JPG_FM_SOF7			= 0xC7,

	ZE_JPG_FM_JPG			= 0xC8,
	ZE_JPG_FM_SOF9			= 0xC9,
	ZE_JPG_FM_SOF10			= 0xCA,
	ZE_JPG_FM_SOF11			= 0xCB,

	ZE_JPG_FM_SOF13			= 0xCD,
	ZE_JPG_FM_SOF14			= 0xCE,
	ZE_JPG_FM_SOF15			= 0xCf,

	ZE_JPG_FM_DHT			= 0xC4,

	ZE_JPG_FM_DAC			= 0xCC,

	ZE_JPG_FM_RST0			= 0xD0,
	ZE_JPG_FM_RST1			= 0xD1,
	ZE_JPG_FM_RST2			= 0xD2,
	ZE_JPG_FM_RST3			= 0xD3,
	ZE_JPG_FM_RST4			= 0xD4,
	ZE_JPG_FM_RST5			= 0xD5,
	ZE_JPG_FM_RST6			= 0xD6,
	ZE_JPG_FM_RST7			= 0xD7,

	ZE_JPG_FM_SOI			= 0xD8,
	ZE_JPG_FM_EOI			= 0xD9,
	ZE_JPG_FM_SOS			= 0xDA,
	ZE_JPG_FM_DQT			= 0xDB,
	ZE_JPG_FM_DNL			= 0xDC,
	ZE_JPG_FM_DRI			= 0xDD,
	ZE_JPG_FM_DHP			= 0xDE,
	ZE_JPG_FM_EXP			= 0xDF,

	ZE_JPG_FM_APP0			= 0xE0,
	ZE_JPG_FM_APP1			= 0xE1,
	ZE_JPG_FM_APP2			= 0xE2,
	ZE_JPG_FM_APP3			= 0xE3,
	ZE_JPG_FM_APP4			= 0xE4,
	ZE_JPG_FM_APP5			= 0xE5,
	ZE_JPG_FM_APP6			= 0xE6,
	ZE_JPG_FM_APP7			= 0xE7,
	ZE_JPG_FM_APP8			= 0xE8,
	ZE_JPG_FM_APP9			= 0xE9,
	ZE_JPG_FM_APP10			= 0xEA,
	ZE_JPG_FM_APP11			= 0xEB,
	ZE_JPG_FM_APP12			= 0xEC,
	ZE_JPG_FM_APP13			= 0xED,
	ZE_JPG_FM_APP14			= 0xEE,
	ZE_JPG_FM_APP15			= 0xEF,

	ZE_JPG_FM_JPG0			= 0xF0,
	ZE_JPG_FM_JPG13			= 0xFD,
	ZE_JPG_FM_COM			= 0xFE,

	ZE_JPG_FM_TEM			= 0x01,

	ZE_JPG_FM_ERROR			= 0x100,

	// Internal ZE markers
	ZE_JPEG_FM_RSTN			= 0xA0,
	ZE_JPEG_FM_MARKER_BEGIN = 0xFF,
	
};

#define	ZE_JPEG_APP0_DATA_LENGTH		14
#define	ZE_JPEG_APP14_DATA_LENGTH		12

const ZESize ZeJpegZigZagToNaturalOrder8x8[8 * 8 + 16] = 
{
	0,  1,  8, 16,  9,  2,  3, 10,
	17, 24, 32, 25, 18, 11,  4,  5,
	12, 19, 26, 33, 40, 48, 41, 34,
	27, 20, 13,  6,  7, 14, 21, 28,
	35, 42, 49, 56, 57, 50, 43, 36,
	29, 22, 15, 23, 30, 37, 44, 51,
	58, 59, 52, 45, 38, 31, 39, 46,
	53, 60, 61, 54, 47, 55, 62, 63,

	63, 63, 63, 63, 63, 63, 63, 63, // Extra entries
	63, 63, 63, 63, 63, 63, 63, 63
};

const ZESize ZeJpegZigZagToNaturalOrder7x7[7 * 7 + 16] = 
{
	0,  1,  8, 16,  9,  2,  3,
	10, 17, 24, 32, 25, 18, 11, 
	4,  5,  12, 19, 26, 33, 40, 
	48, 41, 34, 27, 20, 13,  6, 
	14, 21, 28, 35, 42, 49, 50,
	43, 36, 29, 22, 30, 37, 44, 
	51, 52, 45, 38, 46, 53, 54,

	63, 63, 63, 63, 63, 63, 63, 63, // Extra entries
	63, 63, 63, 63, 63, 63, 63, 63
};

const ZESize ZeJpegZigZagToNaturalOrder6x6[6 * 6 + 16] = 
{
	0,  1,   8,  16,  9,  2,  
	3,  10,  17, 24,  32, 25,
	18, 11,  4,  5,   12, 19,
	26, 33, 40, 41,   34, 27, 
	20, 13, 21, 28,   35, 42, 
	43, 36, 29, 37,   44, 45,

	63, 63, 63, 63, 63, 63, 63, 63, // Extra entries
	63, 63, 63, 63, 63, 63, 63, 63
};

const ZESize ZeJpegZigZagToNaturalOrder5x5[5 * 5 + 16] = 
{
	0,  1,  8,  16,  9, 
	2,  3,  10, 17,  24, 
	32, 25, 18, 11,  4, 
	12, 19, 26, 33,  34, 
	27, 20, 28, 35,  36,

	63, 63, 63, 63, 63, 63, 63, 63, // Extra entries
	63, 63, 63, 63, 63, 63, 63, 63
};

const ZESize ZeJpegZigZagToNaturalOrder4x4[4 * 4 + 16] = 
{
	0,  1,  8, 16,  
	9,  2,  3, 10,
	17, 24, 25, 18, 
	11, 19, 26, 27,

	63, 63, 63, 63, 63, 63, 63, 63, // Extra entries
	63, 63, 63, 63, 63, 63, 63, 63
};

const ZESize ZeJpegZigZagToNaturalOrder3x3[3 * 3 + 16] = 
{
	0,  1,  8, 
	16, 9,  2, 
	10, 17, 18,

	63, 63, 63, 63, 63, 63, 63, 63, // Extra entries
	63, 63, 63, 63, 63, 63, 63, 63
};

const ZESize ZeJpegZigZagToNaturalOrder2x2[2 * 2 + 16] = 
{
	0,  1,  
	8,  9,

	63, 63, 63, 63, 63, 63, 63, 63, // Extra entries
	63, 63, 63, 63, 63, 63, 63, 63
};

const ZESize ZeJpegNaturalToZigZagOrder8x8[8][8] = 
{
	{0,  1,  5,  6,  14, 15, 27, 28},
	{2,  4,  7,  13, 16, 26, 29, 42},
	{3,  8,  12, 17, 25, 30, 41, 43},
	{9,  11, 18, 24, 31, 40, 44, 53},
	{10, 19, 23, 32, 39, 45, 52, 54},
	{20, 22, 33, 38, 46, 51, 55, 60},
	{21, 34, 37, 47, 50, 56, 59, 61},
	{35, 36, 48, 49, 57, 58, 62, 63}
};

const ZESize ZeJpegNaturalToZigZagOrder7x7[7][7] = 
{
	{0,  1,  5,  6,  14, 15, 27},
	{2,  4,  7,  13, 16, 26, 28},
	{3,  8,  12, 17, 25, 29, 38},
	{9,  11, 18, 24, 30, 37, 39},
	{10, 19, 23, 31, 36, 40, 45},
	{20, 22, 32, 35, 41, 44, 46},
	{21, 33, 34, 42, 43, 47, 48}
};

const ZESize ZeJpegNaturalToZigZagOrder6x6[6][6] = 
{
	{0,  1,  5,  6,  14, 15}, 
	{2,  4,  7,  13, 16, 25}, 
	{3,  8,  12, 17, 24, 26}, 
	{9,  11, 18, 23, 27, 32}, 
	{10, 19, 22, 28, 31, 33}, 
	{20, 21, 29, 30, 34, 35}
};

const ZESize ZeJpegNaturalToZigZagOrder5x5[5][5] = 
{
	{0,  1,  5,  6,  14}, 
	{2,  4,  7,  13, 15}, 
	{3,  8,  12, 16, 21}, 
	{9,  11, 17, 20, 22}, 
	{10, 18, 19, 23, 24}
};

const ZESize ZeJpegNaturalToZigZagOrder4x4[4][4] = 
{
	{0,  1,  5,  6 },  
	{2,  4,  7,  12}, 
	{3,  8,  11, 13}, 
	{9,  10, 14, 15}
};

const ZESize ZeJpegNaturalToZigZagOrder3x3[3][3] = 
{
	{0,  1,  5}, 
	{2,  4,  6}, 
	{3,  7,  8}
};

const ZESize ZeJpegNaturalToZigZagOrder2x2[2][2] = 
{
	{0,  1},
	{2,  3}
};

#define STOPPROCESS(Result)				\
{										\
	if (!(Result))	return false;		\
}


// A block of MCU.
struct ZEJpegDataBlock
{
	union
	{
		ZEInt OneDim[ZE_JPEG_DCT_BLOCK_COEFF_COUNT];
		ZEInt TwoDim[ZE_JPEG_DCT_BLOCK_DIMENSION][ZE_JPEG_DCT_BLOCK_DIMENSION];

	} BlockData;
	
	__forceinline static void ZeroData(ZEJpegDataBlock* Block)
	{
		memset(Block, 0, sizeof(ZEJpegDataBlock));
	}
};

// A single MCU block
struct ZEJpegMcu
{
	ZEJpegDataBlock McuData[ZE_JPEG_MAX_BLOCKS_IN_MCU];

	__forceinline static void ZeroData(ZEJpegMcu* Mcu)
	{
		memset(Mcu, 0, sizeof(ZEJpegMcu));
	}
};

// Quantization table.
struct ZEJpegQuantizationTable
{
	// Quantization values for each coefficient
	ZEUInt16			QuantizationValues[ZE_JPEG_DCT_BLOCK_COEFF_COUNT];

};

// Huffman table
struct ZEJpegHuffmanTable
{
	bool				Output;

	ZEUInt8				Bits[17];				// Number of symbols encoded using the indexed bit
	ZEUInt8				HuffmanValues[256];		// Symbols in the increasing code length

};

// Scan Info
struct ZEJpegScanInfo
{
	ZEInt				ScanComponentCount;								// Number of components encoded in the scan 			
	ZEInt				ComponentIndex[ZE_JPEG_MAX_COMPONENTS_IN_SCAN];	// Indexes of the components
	ZEInt				Ss;												// Progressive jpeg, Spectral Selection Start
	ZEInt				Se;												// Progressive jpeg, Spectral Selection End
	ZEInt				Ah;												// Progressive jpeg, Successive Approximation High
	ZEInt				Al;												// Progressive jpeg, Successive Approximation Low

};

// Component Info
struct ZEJpegComponentInfo
{
	bool						ComponentValueNeeded;	// Will component be used
	ZEUInt8						ComponentIndex;			// Index of the component
	ZEUInt8						ComponentId;			// Id of the component, does not have to be arbitrary
	ZEUInt8						HorizontalFreq;			// Horizontal Sampling Frequency
	ZEUInt8						VerticalFreq;			// Vertical Sampling Frequency
	ZEUInt8						QuantizationTableNo;	// Quantization table no to use while decoding
	ZEUInt						MCUWidthInBlocks;		// Dummy blocks are not counted
	ZEUInt						MCUHeightInBlocks;		// Dummy blocks are not counted
	ZEUInt8						AcTableNo;				// AC Huffman table to use while decoding
	ZEUInt8						DcTableNo;				// DC Huffman table to use while decoding
	ZEInt						DctHorzScaledSize;		// DCT scaling applied during compression
	ZEInt						DctVertScaledSize;		// DCT scaling applied during compression
	ZEInt						McuWidth;				// Horizontal block count in MCU
	ZEInt						McuHeight;				// Vertical block count in MCU
	ZEInt						McuBlocks;				// McuWidth * McuHeight
	ZEInt						McuSampleWidth;			// MCU width in terms of samples which is 
	ZEInt						LastColumtWidth;		// Number of non-dummy horizontal blocks in last MCU
	ZEInt						LastRowHeight;			// Number of non-dummy vertical blocks in last MCU
	ZEJpegQuantizationTable*	QuantizationTable;							// Pointer to Quantization Table for the component
	float						DCTTable[ZE_JPEG_DCT_BLOCK_COEFF_COUNT];	// Precalculated table to use in IDCT processor

};

struct ZEJpegDeCompressionInfo
{
	void*						OutputData;
	ZESize						OutputPitch;
	ZESize						OutputPixelSize;
	ZEUInt16					ImageWidth;										// Image width
	ZEUInt16					ImageHeight;									// Image height
	ZEUInt8						ComponentCount;									// Component count of image
	ZEJpegColorSpace			InputColorSpace;								// Input color space
	ZEJpegColorSpace			OutputColorSpace;								// Output color space
	ZEUInt						SeenSosCount;									// Sos marker count
	ZEJpegComponentInfo			ComponentInfo[4];								// Component infos
	bool						IsBaseLine;
	bool						IsProgressive;
	bool						HasMultipleScan;
	ZEUInt						RestartInterval;								// MCU count per restart interval, or 0 for no restart
	bool						JFIFMarkerFound;
	bool						AdobeMarkerFound;
	ZEUInt8						AdobeTransform;									// Color transform code
	ZEUInt8						MaxHorzSampleFactor;							// Used in upsampler
	ZEUInt8						MaxVertSampleFactor;							// Used in upsampler
	ZEUInt8						ComponentsInScan;								// Component Count in scan
	ZEUInt						MCUCountPerRow;									// MCU count in image width
	ZEUInt						MCURowCountInImage;								// MCU row count in image
	ZEInt						DctBlocksPerMCU;								// Block count in an MCU
	ZEInt						SpectralSelectionStart;							// Ss
	ZEInt						SpectralSelectionEnd;							// Se
	ZEInt						ApproxBitPosHigh;								// Ah
	ZEInt						ApproxBitPosLow;								// Al
	ZEInt						DctBlockSize;									// 1 to 16 in width and height
	const ZESize*				NaturalOrderPosArray;							// ZigZag to natural array conversion table
	ZEInt						SpectralSelectionEndLimit;						// min( SpectralSelectionEnd, ZE_JPEG_DCT_BLOCK_COEFF_COUNT-1 ) for entropy decode
	ZEInt						MCUMembership[ZE_JPEG_MAX_BLOCKS_IN_MCU];		// Tells which block is owned by which component
	ZEJpegHuffmanTable			ACHuffmanTables[ZE_JPEG_HUFF_TABLE_COUNT];		// AC Huffman Tables
	ZEJpegHuffmanTable			DCHuffmanTables[ZE_JPEG_HUFF_TABLE_COUNT];		// DC Huffman Tables
	ZEJpegQuantizationTable		QuantizationTables[ZE_JPEG_QUANT_TABLE_COUNT];	// Quantization Tables
	ZEJpegComponentInfo*		CurrentCompInfo[ZE_JPEG_MAX_COMPONENTS_IN_SCAN];	// Component pointers in an ordered fashion
	ZEInt						CoefficientBits[ZE_JPEG_DCT_BLOCK_DIMENSION][ZE_JPEG_DCT_BLOCK_DIMENSION];	// Current Al value for each coefficient
	ZEUInt8						SampleRangeLimit[5 * (ZE_JPEG_MAX_SAMPLE_VALUE + 1) + ZE_JPEG_MID_SAMPLE_VALUE]; // Range limiting for color conversion and idct process

	__forceinline static void ZeroData(ZEJpegDeCompressionInfo* Info)
	{
		memset(Info, 0, sizeof(ZEJpegDeCompressionInfo));
	}
};

#endif 
