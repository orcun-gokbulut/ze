//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureFileJPEG.h
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

/*
INFORMATION ABOUT JPEG FILE FORMAT

A JPEG file consists of many segments, each begin with a marker. 
A marker contains two or more bytes. The first byte is 0xFF, 
the second byte indicates what marker it is. 
The optional length bytes indicates the size of the payload data of the marker 
(including the length bytes, excluding the first two marker bytes.) 
In case the marker payload data doesn't align with byte boundary, 
the left bits are set to 1. 

SPECIFIC MARKER INFO

ZE_JPG_FM_SOF0	->	Start of Frame n (Baseline DCT)
					Next two bytes tells the length of data including the two byte data length.
					First byte is the bits per sample. Jpeg also supports 12 or 16 bits per sample.
					Next two bytes are the height of the image
					Nest two bytes are width of the image
					Next one byte is the number of components. 1 for gray scale and 3 for YCbCr
					For every component there will be 3 bytes.
					01 XY ZW -> 01: component id 
								XY: component frequency, 0-3 bits vertical, 4-7 horizontal
								ZW: quantization table number
					02 XY ZW -> 02: component id 
								XY: component frequency, 0-3 bits vertical, 4-7 horizontal
								ZW: quantization table number
					03 XY ZW -> 03: component id 
								XY: component frequency, 0-3 bits vertical, 4-7 horizontal
								ZW: quantization table number

					This field represent if YCbCr encoding is
					(horiz freq + vert freq of first comp)4:(horiz freq + vert freq of second comp)2:(horiz freq + vert freq of third comp)2 
					Possible YCbCr formats are 4:4:4, 4:2:2, 4:2:0, 4:2:1, 2:2:2 etc.

ZE_JPG_FM_SOF2	->	Start of Frame n (Progressive DCT)

ZE_JPG_FM_DHT	->	Define Huffman Tables. Ther can be one or more huffman table.
					After marker, next two bytes tells the Huffman table info length in bytes including the length itself.
					Next byte gives the Huffman table info: 0-3 bits is Huffman table number. 4th bit is Huffman table type(0 for DC, 1 for AC) and 5-7 bits must be zero.
					Next 16 bytes, each byte represents the number of bytes for Huffman code of length a  particular length. For example, 00 means there are no bytes for Huffman code of length 1.
					As 00 + 01 + 05 + .. + 00 = 12.
					Next 3 markers are all DHT.

ZE_JPG_FM_RST0	->	Restart n

ZE_JPG_FM_SOI	->	Start of Image. No payload data
ZE_JPG_FM_EOI	->	End of Image. Last 2 bytes of the file

ZE_JPG_FM_SOS	->	Start of Scan. Begins a top-to-bottom scan of the image. 
					In baseline JPEG, there is usually a single scan.
					Next two bytes tell us the byte length of the scan information including the length itself.
					Following first byte is number of components in the scan. ex:3 for color image
					The next is like : 
					01/NM : component id 1 / bits 0-3 AC table, bits 4-7 DC table
					02/NM : component id 2 / bits 0-3 AC table, bits 4-7 DC table
					03/NM : component id 3 / bits 0-3 AC table, bits 4-7 DC table
					Last 3 bytes ignored (I do not know why).

ZE_JPG_FM_DQT	->	Define Quantization Table, followed by one or more quantization table. 
					Next two bytes tells the payload of the marker in terms of bytes including the payload itself. 
					Followed by one byte table info where bits 0-3 = table number 
					and bits 4-7 = data precision. Then 8x8 table comes. 
					According to precision the memory is allocated for the table.

ZE_JPG_FM_APP0	->	Application Specific marker, next two bytes(lets say x) tells 
					the payload of the marker including the x.

ZE_JPG_FM_COM	->	Comment

ZE_JPG_FM_DRI	->	Define Restart Interval

ZE_JPG_FM_MB	->	File marker begin byte

*/




#pragma once
#ifndef __ZE_TEXTURE_FILE_JPEG__
#define __ZE_TEXTURE_FILE_JPEG__

#include "ZETypes.h"
#include "ZETextureFile.h"

#define ZE_JPEG_MAX_SAMPLE_FREQUENCY		4
#define ZE_JPEG_MAX_COMPONENT_COUNT			10

#define ZE_JPEG_SUPPORTED_DATA_PRECISION	8

#define ZE_JPEG_MAX_IMAGE_WIDTH				65500
#define ZE_JPEG_MAX_IMAGE_HEIGHT			65500

// Memory related definitions
#define	ZE_JPEG_INPUT_BUFFER_SIZE	(ZESize)4096



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
#define ZE_JPEG_MAX_COMPONENTS_IN_SCAN		4

#define ZE_JPEG_MAX_MCU_BLOCKS				10

#define ZE_JPEG_QUANT_TABLE_COUNT			4
#define ZE_JPEG_HUFF_TABLE_COUNT			4
#define ZE_JPEG_MAX_SCAN_COMPONENTS			4
#define ZE_JPEG_MAX_SAMPLING_FACTOR			4

#define ZE_JPEG_DCT_ELEMENT_COUNT			8
#define ZE_JPEG_DCT_BLOCK_ELEMENT_COUNT		64


enum ZEJpegSampleFrequency
{
	ZE_JPEG_SF_UNKNOWN				= 0,	// Unknown

	ZE_JPEG_SF_4_4_4				= 1,	// 4:4:4 Frequency
	ZE_JPEG_SF_4_2_2				= 2,	// 4:2:2 Frequency
	ZE_JPEG_SF_4_2_0				= 3,	// 4:2:0 Frequency
	ZE_JPEG_SF_4_1_1				= 4,	// 4:1:1 Frequency
	ZE_JPEG_SF_4_1_0				= 5,	// 4:1:0 Frequency
};

enum ZEJpegColorSpace
{

	ZE_JPEG_CS_UNKNOWN				= 0,	// Unknown

	ZE_JPEG_CS_GRAYSCALE			= 1,	// Gray Scale
	ZE_JPEG_CS_RGB					= 2,	// R-G-B
	ZE_JPEG_CS_YCbCr				= 3,	// Y-Cb-Cr (YUV)
	ZE_JPEG_CS_CMYK					= 4,	// C-M-Y-K
	ZE_JPEG_CS_YCCK					= 5,	// Y-Cb-Cr-K
};

enum ZEJpegDitherMethod
{
	ZE_JPEG_DM_NONE					= 0,	// No dithering

	ZE_JPEG_DM_ORDERED				= 1,	// Simple ordered dither
	ZE_JPEG_DM_FLOYD_STEINBERG		= 2,	// Floyd-Steinberg error diffusion dither
};

enum ZEJpegDctMethod
{
	ZE_JPEG_DCTM_INTEGER_SLOW		= 0,
	ZE_JPEG_DCTM_INTEGER_FAST		= 1,
	ZE_JPEG_DCTM_FLOATING_POINT		= 2,	// Always floating point is used
};


// Two byte file markers that are followed by ZE_JPG_FM_MB
enum ZEJpegFileMarker
{
	ZE_JPG_FM_UNKNOWN	= 0x00, 

	ZE_JPG_FM_SOF0		= 0xC0, 
	ZE_JPG_FM_SOF1		= 0xC1,
	ZE_JPG_FM_SOF2		= 0xC2, 
	ZE_JPG_FM_SOF3		= 0xC3,

	ZE_JPG_FM_SOF5		= 0xC5,
	ZE_JPG_FM_SOF6		= 0xC6,
	ZE_JPG_FM_SOF7		= 0xC7,

	ZE_JPG_FM_JPG		= 0xC8,
	ZE_JPG_FM_SOF9		= 0xC9,
	ZE_JPG_FM_SOF10		= 0xCA,
	ZE_JPG_FM_SOF11		= 0xCB,

	ZE_JPG_FM_SOF13		= 0xCD,
	ZE_JPG_FM_SOF14		= 0xCE,
	ZE_JPG_FM_SOF15		= 0xCf,

	ZE_JPG_FM_DHT		= 0xC4,

	ZE_JPG_FM_DAC		= 0xCC,

	ZE_JPG_FM_RST0		= 0xD0,
	ZE_JPG_FM_RST1		= 0xD1,
	ZE_JPG_FM_RST2		= 0xD2,
	ZE_JPG_FM_RST3		= 0xD3,
	ZE_JPG_FM_RST4		= 0xD4,
	ZE_JPG_FM_RST5		= 0xD5,
	ZE_JPG_FM_RST6		= 0xD6,
	ZE_JPG_FM_RST7		= 0xD7,

	ZE_JPG_FM_SOI		= 0xD8,
	ZE_JPG_FM_EOI		= 0xD9,
	ZE_JPG_FM_SOS		= 0xDA,
	ZE_JPG_FM_DQT		= 0xDB,
	ZE_JPG_FM_DNL		= 0xDC,
	ZE_JPG_FM_DRI		= 0xDD,
	ZE_JPG_FM_DHP		= 0xDE,
	ZE_JPG_FM_EXP		= 0xDF,

	ZE_JPG_FM_APP0		= 0xE0,
	ZE_JPG_FM_APP1		= 0xE1,
	ZE_JPG_FM_APP2		= 0xE2,
	ZE_JPG_FM_APP3		= 0xE3,
	ZE_JPG_FM_APP4		= 0xE4,
	ZE_JPG_FM_APP5		= 0xE5,
	ZE_JPG_FM_APP6		= 0xE6,
	ZE_JPG_FM_APP7		= 0xE7,
	ZE_JPG_FM_APP8		= 0xE8,
	ZE_JPG_FM_APP9		= 0xE9,
	ZE_JPG_FM_APP10		= 0xEA,
	ZE_JPG_FM_APP11		= 0xEB,
	ZE_JPG_FM_APP12		= 0xEC,
	ZE_JPG_FM_APP13		= 0xED,
	ZE_JPG_FM_APP14		= 0xEE,
	ZE_JPG_FM_APP15		= 0xEF,

	ZE_JPG_FM_JPG0		= 0xF0,
	ZE_JPG_FM_JPG13		= 0xFD,
	ZE_JPG_FM_COM		= 0xFE,

	ZE_JPG_FM_TEM		= 0x01,

	ZE_JPG_FM_ERROR		= 0x100,

	ZE_JPEG_FM_MARKER_BEGIN = 0xFF,		
};

const ZESize ZeJpegZigZagToNaturalOrder8x8[ZE_JPEG_DCT_BLOCK_ELEMENT_COUNT + 16] = 
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
	 0,  1,  8, 16,  9,  2,  3, 10,
	17, 24, 32, 25, 18, 11,  4,  5,
	12, 19, 26, 33, 40, 48, 41, 34,
	27, 20, 13,  6, 14, 21, 28, 35,
	42, 49, 50, 43, 36, 29, 22, 30,
	37, 44, 51, 52, 45, 38, 46, 53,
	54,
	63, 63, 63, 63, 63, 63, 63, 63, // Extra entries
	63, 63, 63, 63, 63, 63, 63, 63
};

const ZESize ZeJpegZigZagToNaturalOrder6x6[6 * 6 + 16] = 
{
	 0,  1,  8, 16,  9,  2,  3, 10,
	17, 24, 32, 25, 18, 11,  4,  5,
	12, 19, 26, 33, 40, 41, 34, 27,
	20, 13, 21, 28, 35, 42, 43, 36,
	29, 37, 44, 45,
	63, 63, 63, 63, 63, 63, 63, 63, // Extra entries
	63, 63, 63, 63, 63, 63, 63, 63
};

const ZESize ZeJpegZigZagToNaturalOrder5x5[5 * 5 + 16] = 
{
	 0,  1,  8, 16,  9,  2,  3, 10,
	17, 24, 32, 25, 18, 11,  4, 12,
	19, 26, 33, 34, 27, 20, 28, 35,
	36,
	63, 63, 63, 63, 63, 63, 63, 63, // Extra entries
	63, 63, 63, 63, 63, 63, 63, 63
};

const ZESize ZeJpegZigZagToNaturalOrder4x4[4 * 4 + 16] = 
{
	 0,  1,  8, 16,  9,  2,  3, 10,
	17, 24, 25, 18, 11, 19, 26, 27,
	63, 63, 63, 63, 63, 63, 63, 63, // Extra entries
	63, 63, 63, 63, 63, 63, 63, 63
};

const ZESize ZeJpegZigZagToNaturalOrder3x3[3 * 3 + 16] = 
{
	 0,  1,  8, 16,  9,  2, 10, 17,
	18,
	63, 63, 63, 63, 63, 63, 63, 63, // Extra entries
	63, 63, 63, 63, 63, 63, 63, 63
};

const ZESize ZeJpegZigZagToNaturalOrder2x2[2 * 2 + 16] = 
{
	 0,  1,  8,  9,
	63, 63, 63, 63, 63, 63, 63, 63, // Extra entries
	63, 63, 63, 63, 63, 63, 63, 63
};


struct ZEJpegQuantizationTable
{
	bool				Output;

	ZEUInt16			QuantizationValues[ZE_JPEG_DCT_BLOCK_ELEMENT_COUNT];

};

struct ZEJpegHuffmanTable
{
	bool				Output;

	ZEUInt8				Bits[17];
	ZEUInt8				HuffmanValues[256];

};

struct ZEJpegScanInfo
{
	ZEInt				ScanComponentCount;						
	ZEInt				ComponentIndex[ZE_JPEG_MAX_SCAN_COMPONENTS];	
	ZEInt				Ss;
	ZEInt				Se;								
	ZEInt				Ah;
	ZEInt				Al;

};

struct ZEJpegComponentInfo
{
	bool						ComponentValueNeeded;

	ZEUInt8						ComponentIndex;	
	ZEUInt8						ComponentId;

	ZEUInt8						HorizontalFreq;
	ZEUInt8						VerticalFreq;

	ZEUInt8						QuantizationTableNo;

	ZEUInt						WidthInBlocks;
	ZEUInt						HeightInBlocks;
	
	ZEUInt8						AcTableNo;
	ZEUInt8						DcTableNo;

	ZEInt						DctHorzScaledSize;
	ZEInt						DctVertScaledSize;
	
	ZEUInt						DownsampledWidth;
	ZEUInt						DownsampledHeight; 
	
	ZEInt						McuWidth;
	ZEInt						McuHeight;

	ZEInt						McuBlocks;
	
	ZEInt						McuSampleWidth;
	
	ZEInt						LastColumtWidth;
	ZEInt						LastRowHeight;
	
};

struct ZEJpegDeCompressionInfo
{
	// Input Image parameters
	void*						CompressedData;
	void*						Destination;

	ZEUInt16					ImageWidth;				// Image Width : SOF
	ZEUInt16					ImageHeight;			// Image Height : SOF

	ZEUInt8						ComponentCount;			// Component Count of Image : SOF
	
	ZEJpegColorSpace			InputColorSpace;		// Input color space : APPn
	ZEJpegColorSpace			OutputColorSpace;		// Output color space : RGB always
	
	// Decompression parameters
	ZEUInt						ScaleNumerator;			// Fraction to scale the image
	ZEUInt						ScaleDenominator;		// Fraction to scale the image

	ZEJpegDctMethod				DctMethod;

	bool						FancyUpsampling;
	bool						BlockSmoothing;
	

	// Output Image parameters
	ZEUInt						OutputWidth;
	ZEUInt						OutputHeight;

	float						OutputGamma;

	ZEUInt						OutputComponentCount;

	ZEUInt						ScanlineBufferHeight;

	ZEUInt						OutputScanline;					// 0 to OutputHeight - 1

	ZEUInt						SeenSosCount;					// SOS markers found in the file

	ZEUInt						CompletedImcuRowCount;
	ZEUInt						ReadImcuRowCount;

	ZEUInt						OutputScanNumber;

	ZEJpegComponentInfo			ComponentInfo[4];				// Some may be unused

	ZEInt						CoefficientBits[ZE_JPEG_DCT_ELEMENT_COUNT][ZE_JPEG_DCT_ELEMENT_COUNT];	// -1 or current Al value for each coefficient

	ZEJpegQuantizationTable		QuantizationTables[ZE_JPEG_QUANT_TABLE_COUNT];
	ZEJpegHuffmanTable			ACHuffmanTables[ZE_JPEG_HUFF_TABLE_COUNT];
	ZEJpegHuffmanTable			DCHuffmanTables[ZE_JPEG_HUFF_TABLE_COUNT];

	ZEUInt8						DataPrecision;

	bool						HasMultipleScan;

	bool						IsBaseLine;
	bool						IsProgressive;
	bool						IsHuffman;

	ZEUInt						RestartInterval;				// MCUs per restart interval, or 0 for no restart

	// Extra markers that are supported
	bool						JFIFMarkerFound;
	// JFIF Specific Data
	// -------------------------------------
	ZEUInt8						JFIFVersionMajor;
	ZEUInt8						JFIFVersionMinor;
	ZEUInt8						DensityUnit;
	ZEUInt16					DensityX;
	ZEUInt16					DensityY;
	// -------------------------------------

	bool						AdobeMarkerFound;
	// Adobe Specific Data
	// -------------------------------------
	ZEUInt8						AdobeTransform;		// Color transform code
	// -------------------------------------

	ZEUInt8						MaxHorzSampleFactor;
	ZEUInt8						MaxVertSampleFactor;

	ZEInt						MinDctHorzScaledSize;
	ZEInt						MinDctVertScaledSize;

	ZEUInt						TotaliMCURowCount;

	ZEUInt8*					SampleRangeLimit;

	// Parameters which are valid for per scan
	ZEUInt8						ComponentsInScan;
	ZEJpegComponentInfo*		CurrentCompInfo[ZE_JPEG_MAX_SCAN_COMPONENTS];

	ZEUInt						MCUCountPerRow;
	ZEUInt						MCURowCountInImage;

	ZEInt						DctBlocksPerMCU;
	ZEInt						MCUMembership[ZE_JPEG_MAX_MCU_BLOCKS];

	// Progressive parameters
	ZEInt						SpectralSelectionStart; // Ss
	ZEInt						SpectralSelectionEnd;	// Se
	ZEInt						ApproxBitPosHigh;		// Ah
	ZEInt						ApproxBitPosLow;		// Al

	ZEInt						DctBlockSize;
	const ZEUInt16*				NaturalOrderPosArray;
	ZEInt						SpectralSelectionEndLimit;

	bool						SetDefaultParametes();
	bool						SetDecompressionParametes();
	bool						SetScanParameters();

};

class ZEJpegColorSpaceConverter
{
		
	private:
							ZEJpegColorSpaceConverter();
							~ZEJpegColorSpaceConverter();

	public:
		static void			Convert(void* OutputBuffer, ZEJpegColorSpace OutputColorSpace, 
									void* InputBuffer, ZEJpegColorSpace InputColorSpace, 
									ZEUInt ImageWidth, ZEUInt ImageHeight);

};

class ZEJpegFrequencyUpsampler
{

	private:
							ZEJpegFrequencyUpsampler();
							~ZEJpegFrequencyUpsampler();

	public:
		static void			UpSample(void* OutputBuffer, ZEJpegSampleFrequency InputFrequency, 
									 void* InputBuffer, ZEJpegSampleFrequency OutputFrequency, 
									 ZEUInt ImageWidth, ZEUInt ImageHeight);

};

class ZEJpegInverseDctProcessor
{

	private:
							ZEJpegInverseDctProcessor();
							~ZEJpegInverseDctProcessor();

	public:
		static void			Process(ZEJpegDeCompressionInfo* Info);

};

class ZEJpegDeuantizatizator
{

	private:
							ZEJpegDeuantizatizator();
							~ZEJpegDeuantizatizator();

	public:
		static void			Process(ZEJpegDeCompressionInfo* Info);

};

#define		HUFFMAN_BIT_BUFFER_SIZE			32
#define		HUFFMAN_BIT_BUFFER_MIN_LOAD		25

class ZEJpegFileMarkerBuffer;

class HuffmanDecoderBitBuffer
{
	private:
		ZEJpegFileMarkerBuffer* MarkerBuffer;

		ZEUInt32				Buffer;
		ZESize					BitsAvailable;

								HuffmanDecoderBitBuffer();
								~HuffmanDecoderBitBuffer();

		bool					FillBuffer(bool& EndOfCompression);

	public:
		ZESize					GetAvailableBitCount() const;

		bool					GetBits(ZEUInt32& Bit, ZEUInt32 BitCount);
		bool					SeeBits(ZEUInt32& Bit, ZEUInt32 BitCount);
		bool					PassBits(ZEUInt32 BitCount);

		void					Initialize(ZEJpegFileMarkerBuffer* MarkerBuffer);
		void					Deinitialize();

		void					Destroy();
		static HuffmanDecoderBitBuffer* CreateInstance();
};

// Bit masks for 16 rightmost bits of 32 bits of an 32 bit unsigned int
// Used by Huffman decoder
// Right aligned
const ZEUInt BitMask[16] =
{
	0x0000,	// Mask for 0 bits
	0x0001,	// Mask for 1 bits
	0x0003,	// Mask for 2 bits
	0x0007,	// Mask for 3 bits
	0x000F,	// Mask for 4 bits
	0x001F,
	0x003F,
	0x007F,
	0x00FF,
	0x01FF,
	0x03FF,
	0x07FF,
	0x0FFF,
	0x1FFF,
	0x3FFF,
	0x7FFF	// Mask for 15 bits, no more than 15 bits huffman code is not possible in jpeg
};

#define		HUFFMAN_LOOK_AHEAD_BITS		8

// Bit access macro for ZEJpegHuffmanDecoder
#define		BITMASK(NthBit)		BitMask[NthBit]

// Can be used for only one jpeg per initialization
class ZEJpegHuffmanDecoder
{
	private:
		HuffmanDecoderBitBuffer*	BitBuffer;

		ZEInt						YTableDC[ZE_JPEG_DCT_BLOCK_ELEMENT_COUNT];
		ZEInt						YTableAC[ZE_JPEG_DCT_BLOCK_ELEMENT_COUNT];
		ZEInt						CromTableDC[ZE_JPEG_DCT_BLOCK_ELEMENT_COUNT];
		ZEInt						CromTableAC[ZE_JPEG_DCT_BLOCK_ELEMENT_COUNT];

		bool						UseDecodeMcuDcFirstTime;
		bool						UseDecodeMcuAcFirstTime;

		bool						UseDecodeMcuDcRefine;
		bool						UseDecodeMcuAcRefine;

		bool						UseDecodeMcu;
		bool						UseDecodeMcuSub;

									ZEJpegHuffmanDecoder();
									~ZEJpegHuffmanDecoder();

		bool						CreateTable(ZEJpegDeCompressionInfo* Info, ZEInt* Table, bool IsDc);

	public:
		static void					Decode(ZEJpegDeCompressionInfo* Info);

		void						Initialize(ZEJpegDeCompressionInfo* Info);
		void						Deinitialize();

		void						Destroy();
		static ZEJpegHuffmanDecoder* CreateInstance();


};


class ZEFile;

// Buffers data from file and finds markers
class ZEJpegFileMarkerBuffer
{

	private:
		ZEFile*					ImageFile;
		
		ZEUInt8					Buffer[ZE_JPEG_INPUT_BUFFER_SIZE];
		ZESize					BytesAvailable;

		bool					FillBuffer();
		
								ZEJpegFileMarkerBuffer();
								~ZEJpegFileMarkerBuffer();
	public :
		ZESize					GetAvailableByteCount() const;

		bool					GetByte(ZEUInt8& Byte);
		bool					GetTwoByte(ZEUInt16& TwoByte);
		bool					GetTwoByte(ZEUInt8& Byte1, ZEUInt8& Byte2);

		bool					SkipVariableLenghtMarker();
		bool					SkipBufferData(ZEUInt32 DataLenght);

		bool					GetNextMarker(ZEJpegFileMarker& Marker);

		void					Initialize(ZEFile* ImageFile);
		void					Deinitialize();

		void					Destroy();
		static ZEJpegFileMarkerBuffer* CreateInstance();

		static bool				ValidateMarker(ZEUInt16 RawMarker);
		static bool				ValidateMarker(ZEUInt8 Byte1, ZEUInt8 Byte2);
		static bool				ValidateMarker(ZEUInt16 RawMarker, ZEJpegFileMarker Marker);
		static bool				ValidateMarker(ZEUInt8 Byte1, ZEUInt8 Byte2, ZEJpegFileMarker Marker);

};

#define		ZE_JPEG_APP0_DATA_LENGTH		14
#define		ZE_JPEG_APP14_DATA_LENGTH		12

class ZEJpegFileMarkerReader
{
		
	private:
		ZEFile*					ImageFile;
		ZEJpegFileMarkerBuffer* MarkerBuffer;

		bool					SOFFound;
		bool					SOIFound;
		bool					EOIFound;
		ZEUInt8					NextRestart;

		bool					ProcessSOIMarker(ZEJpegDeCompressionInfo* Info);
		bool					ProcessEOIMarker(ZEJpegDeCompressionInfo* Info);

		bool					ProcessAPP0Marker(ZEJpegDeCompressionInfo* Info);
		bool					ProcessAPP14Marker(ZEJpegDeCompressionInfo* Info);

		bool					ProcessSOFMarker(ZEJpegDeCompressionInfo* Info, bool BaseLine, bool Progressive);
		bool					ProcessSOSMarker(ZEJpegDeCompressionInfo* Info);
		bool					ProcessDHTMarker(ZEJpegDeCompressionInfo* Info);
		bool					ProcessDQTMarker(ZEJpegDeCompressionInfo* Info);
		bool					ProcessDRIMarker(ZEJpegDeCompressionInfo* Info);
		bool					ProcessRSTMarker(ZEJpegDeCompressionInfo* Info);

								ZEJpegFileMarkerReader();
								~ZEJpegFileMarkerReader();

	public:
		bool					ReadMarkers(ZEJpegDeCompressionInfo* Info, ZEJpegFileMarker& StoppedAt);

		bool					GetSOFFound() const;
		bool					GetSOIFound() const;
		bool					GetEOIFound() const;

		void					Initialize(ZEFile* ImageFile);
		void					Deinitialize();

		void					Destroy();
		static ZEJpegFileMarkerReader* CreateInstance();

};

struct ZEJpegDeCompressionInfo;
class ZEJpegFileMarkerReader;

// Decompresses a block and writes output to destination
class ZEJpegDecompressor
{

	private:
									ZEJpegDecompressor();
									~ZEJpegDecompressor();
	public:

		static void					Decompress(ZEJpegDeCompressionInfo* Info);

};


class ZETextureFileJpeg : public ZETextureFile
{
	
	private:
		ZEFile*							ImageFile;
		
		ZEJpegDeCompressionInfo*		JpegInfo;
		ZEJpegFileMarkerReader*			MarkerReader;

	public:
										ZETextureFileJpeg();
		virtual							~ZETextureFileJpeg();

		virtual bool					CanLoad(ZEFile* File);
		virtual ZETextureData*			Load(ZEFile* File);

};

#endif
