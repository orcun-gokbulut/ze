//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureFileJPEGHuffmanDecoder.h
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
#ifndef __ZE_TEXTURE_FILE_JPEG_HUFFMAN_DECODER_H__
#define __ZE_TEXTURE_FILE_JPEG_HUFFMAN_DECODER_H__


#include "ZETextureFileJPEGCommonDefinitions.h"

class ZEJpegFileMarkerBuffer;
class ZEJpegFileMarkerReader;

class HuffmanDecoderBitBuffer
{
	private:
		ZEJpegFileMarkerBuffer* MarkerBuffer;

		ZEInt32					Buffer;
		ZESize					BitsAvailable;

		bool					EndOfCompression;

								HuffmanDecoderBitBuffer();
								~HuffmanDecoderBitBuffer();

		bool					FillBuffer(ZEUInt MinLoad = 0);

	public:
		ZESize					GetAvailableBitCount() const;
		bool					GetEndOfCompression() const;

		bool					GetBits(ZEInt32& Bit, ZEUInt32 BitCount);
		bool					SeeBits(ZEInt32& Bit, ZEUInt32 BitCount);
		bool					PassBits(ZEUInt32 BitCount);

		bool					ResetBuffer();

		void					Initialize(ZEJpegFileMarkerBuffer* MarkerBuffer);
		void					Deinitialize();

		void					Destroy();
		static HuffmanDecoderBitBuffer* CreateInstance();
};

struct ZEJpegDerivedHuffmanTable
{
	ZEJpegHuffmanTable*			OwnerHuffmanTable;
	ZEInt32						MaxCodeLenght[18];	// -1 if not used
	ZEInt32						ValueOffsets[17];
	ZEInt						LookNBits[1 << HUFFMAN_LOOK_AHEAD_BITS];	// Precalculated look ahead bit counts
	ZEUInt8						LookSymbol[1 << HUFFMAN_LOOK_AHEAD_BITS];	// Output symbol

};

struct ZEJpegDeCompressionInfo;

// Can be used only for one jpeg per initialization
class ZEJpegHuffmanDecoder
{
	private:
		ZEJpegDeCompressionInfo*		Info;
		HuffmanDecoderBitBuffer*		BitBuffer;
		ZEJpegFileMarkerReader*			MarkerReeader;

		// Used per scan
		// --------------
		ZEUInt							McusLeftToRestart;
		ZEJpegDerivedHuffmanTable*		CurrentAcDerivedTable;
		// --------------

		// Used per image
		// --------------
		ZEUInt							SeenEobCount;
		ZEInt							LastDcValue[ZE_JPEG_MAX_COMPONENTS_IN_SCAN];

		ZEJpegDerivedHuffmanTable*		DerivedTables[ZE_JPEG_HUFF_TABLE_COUNT];

		ZEJpegDerivedHuffmanTable		DerivedDcTables[ZE_JPEG_HUFF_TABLE_COUNT];
		ZEJpegDerivedHuffmanTable		DerivedAcTables[ZE_JPEG_HUFF_TABLE_COUNT];
		// --------------

		// Used per block
		// --------------
		ZEJpegDerivedHuffmanTable*		CurrentDcTables[ZE_JPEG_MAX_BLOCKS_IN_MCU];
		ZEJpegDerivedHuffmanTable*		CurrentAcTables[ZE_JPEG_MAX_BLOCKS_IN_MCU];

		ZESize							CoefficientLimits[ZE_JPEG_MAX_BLOCKS_IN_MCU];
		// --------------

										ZEJpegHuffmanDecoder();
										~ZEJpegHuffmanDecoder();

										// Processes a restart marker
		bool							Restart();

										// Huffman bit decoding functions
		bool							HuffmanDecode(ZEInt& Output, ZEJpegDerivedHuffmanTable* Table);
		ZEInt							HuffmanDecodeFallBack(ZEJpegDerivedHuffmanTable* Table, ZEUInt32 NBits);

										// Jpeg MCU decoding functions
		bool							MCUDecodeDcFirstTime(ZEJpegMcu* McuData);
		bool							MCUDecodeAcFirstTime(ZEJpegMcu* McuData);

		bool							MCUDecodeDcRefine(ZEJpegMcu* McuData);
		bool							MCUDecodeAcRefine(ZEJpegMcu* McuData);

		bool							MCUDecodeFullBlock(ZEJpegMcu* McuData);
		bool							MCUDecodeSubBlock(ZEJpegMcu* McuData);

										// Lookup table creation function
		bool							CreateDerivedTable(ZEUInt TableId, ZEJpegDerivedHuffmanTable* Table, bool IsDc);

	public:
		bool							(ZEJpegHuffmanDecoder::*DecodeFunction)(ZEJpegMcu* McuData);

		void							Initialize(ZEJpegDeCompressionInfo* Info, ZEJpegFileMarkerReader* MarkerReader);
		void							Deinitialize();

		void							Destroy();
		static ZEJpegHuffmanDecoder*	CreateInstance();

};

#endif // __ZE_TEXTURE_FILE_JPEG_HUFFMAN_DECODER_H__
