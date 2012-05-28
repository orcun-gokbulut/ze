//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureFileJPEGHuffmanDecoder.cpp
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
#include "ZETypes.h"
#include "ZETextureFileJPEG.h"
#include "ZETextureFileJPEGHuffmanDecoder.h"
#include "ZETextureFileJPEGCommonDefinitions.h"
#include "ZETextureFileJPEGMarkerReader.h"

#include <intrin.h>
#include <stdio.h>


/************************************************************************/
/*                        HuffmanDecoderBitBuffer						*/
/************************************************************************/

__forceinline bool HuffmanDecoderBitBuffer::FillBuffer(ZEUInt MinLoad)
{
	bool Result = false;
	// If a marker is seen in previous FillBuffer attempt
	// EndOfCompression will be set, This means 
	// we wont be able to get mote bytes from buffer for decompression
	// Just return true and let HuffmanFallback do its job

	if (EndOfCompression)
	{
		return true;
	}
	
	// If there are enough bits return;
	if ((BitsAvailable > HUFFMAN_LOOK_AHEAD_BITS) && (BitsAvailable > (ZESize)MinLoad))
	{
		EndOfCompression = false;
		return true;
	}

	ZEUInt UsefulBytes = 0;

	// This is the point we load bytes into bit buffer
	while (BitsAvailable < HUFFMAN_BIT_BUFFER_MIN_LOAD)
	{
		ZEUInt8 Temp = 0;
		EndOfCompression = false;

		Result = MarkerBuffer->GetOneByte(Temp);
		STOPPROCESS(Result);

		// Check if byte is  marker begin 
		// In compressed data if 0xFF is not a marker begin then there must be 0x00 following 0xFF
		if (Temp == 0xFF)
		{
			do
			{
				Result = MarkerBuffer->GetOneByte(Temp);
				STOPPROCESS(Result);

			}while (Temp == 0xFF);

			// Found a stuffed byte the actual data is FF
			if (Temp == 0x00)
			{
				Temp = 0xFF;
			}
			else // Damn found a new marker
			{
				// Stop getting more inputs in the future,
				// until the marker is processed
				EndOfCompression = true;
				
				// Go back 2 bytes so that the marker buffer can read the same marker
				Result = this->MarkerBuffer->RewindTwoByte();
				STOPPROCESS(Result);

				// Error
				if ((ZESize)MinLoad > BitsAvailable)
				{
					// We did not read the MinLoad amount of bits

					// Set the rest of the buffer to 0
					Buffer <<= (ZEUInt32)(HUFFMAN_BIT_BUFFER_MIN_LOAD - BitsAvailable);
					BitsAvailable = HUFFMAN_BIT_BUFFER_MIN_LOAD;

					return false;
				}
				else 
				{
					// We did not fill the buffer but we got MinLoad amount of bits
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


__forceinline ZESize HuffmanDecoderBitBuffer::GetAvailableBitCount() const
{
	return BitsAvailable;
}

__forceinline bool HuffmanDecoderBitBuffer::GetEndOfCompression() const
{
	return EndOfCompression;
}

// Removes bits from buffer
__forceinline bool HuffmanDecoderBitBuffer::GetBits(ZEInt32& Bits, ZEUInt32 BitCount)
{
	bool Result = false;
	bool EndCompression = false;

	if (BitsAvailable < (ZESize)BitCount)
	{
		Result = FillBuffer(BitCount);
		STOPPROCESS(Result);
	}

	Bits = (ZEInt32)((Buffer >> (BitsAvailable -= (ZESize)BitCount)) & BITMASK(BitCount));
	return true;
}

// Does not remove the bits
__forceinline bool HuffmanDecoderBitBuffer::SeeBits(ZEInt32& Bits, ZEUInt32 BitCount)
{
	bool Result = false;

	if (BitsAvailable < (ZESize)BitCount)
	{
		Result = FillBuffer();
		STOPPROCESS(Result);
	}

	Bits = (ZEInt32)((Buffer >> (BitsAvailable - (ZESize)BitCount)) & BITMASK(BitCount));
	return true;
}

// Removes bits from buffer
__forceinline bool HuffmanDecoderBitBuffer::PassBits(ZEUInt32 BitCount)
{
	bool Result = false;

	if (BitsAvailable < (ZESize)BitCount)
	{
		Result = FillBuffer();
		STOPPROCESS(Result);
	}

	BitsAvailable -= (ZESize)BitCount;
	return true;
}

__forceinline bool HuffmanDecoderBitBuffer::ResetBuffer()
{
	EndOfCompression = false;
	BitsAvailable = 0;
	Buffer = 0;
	
	return true;
}

void HuffmanDecoderBitBuffer::Initialize(ZEJpegFileMarkerBuffer* MarkerBuffer)
{
	Buffer	= 0;
	BitsAvailable = 0;
	this->MarkerBuffer = MarkerBuffer;

	// Fill the buffer on initialization
	// FillBuffer();
}

void HuffmanDecoderBitBuffer::Deinitialize()
{
	Buffer	= 0;
	BitsAvailable = 0;
	MarkerBuffer = NULL;
	EndOfCompression = false;
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
	Buffer	= 0;
	BitsAvailable = 0;
	MarkerBuffer = NULL;
	EndOfCompression = false;
}

HuffmanDecoderBitBuffer::~HuffmanDecoderBitBuffer()
{
	this->Deinitialize();
}

/************************************************************************/
/*                        ZEJpegHuffmanDecoder							*/
/************************************************************************/

// Resets the bit buffer, last DC values, McusLeftToRestart
// Also 
__forceinline bool ZEJpegHuffmanDecoder::Restart()
{
	bool Result = false;

	// CleaBuffer
	Result = BitBuffer->ResetBuffer();
	STOPPROCESS(Result);

	// Process the marker
	ZEJpegFileMarker Marker = ZE_JPG_FM_UNKNOWN;
	Result = MarkerReeader->ReadMarkers(Marker);
	STOPPROCESS(Result);

	// Check if the returned marker is restart
	if (Marker != ZE_JPEG_FM_RSTN)
		return false;

	SeenEobCount = 0;

	// Reset saved DC values 
	memset(this->LastDcValue, 0, sizeof(ZEInt) * ZE_JPEG_MAX_COMPONENTS_IN_SCAN);

	// Reset McusLeftToRestart
	McusLeftToRestart = Info->RestartInterval;

	return true;
}

// Fall back function of HuffmanDecode. Does not use tables, computation is slow
__forceinline ZEInt ZEJpegHuffmanDecoder::HuffmanDecodeFallBack(ZEJpegDerivedHuffmanTable* Table, ZEUInt32 NBits)
{
	bool Result = false;
	ZEUInt32 MinBitCount = NBits; // We know that the bit stream we will read will be at least NB bits long
	ZEInt32 HuffmanCode = 0;
	ZEInt32 ReadBit;

	// Return -1 same as unused table entries
	if (!BitBuffer->GetBits(HuffmanCode, MinBitCount))
		return -1;

	// Collect the rest of the Huffman code one bit at a time.
	while (HuffmanCode > Table->MaxCodeLenght[MinBitCount])
	{
		HuffmanCode <<= 1;
		BitBuffer->GetBits(ReadBit, 1);
		HuffmanCode |= ReadBit;
		MinBitCount++;
	}

	// Error, huffman codes cannot be longer than 16 bits
	// Just return 0 and continue decoding
	if (MinBitCount > 16) 
	{
		return 0;
	}

	return Table->OwnerHuffmanTable->HuffmanValues[ (ZESize)(HuffmanCode + Table->ValueOffsets[ MinBitCount ]) ];
}

bool ZEJpegHuffmanDecoder::HuffmanDecode(ZEInt& Output, ZEJpegDerivedHuffmanTable* Table)
{
	bool Result = false;
	ZEInt NBits, CheckBits;
	
	// If end of compression just fall back
	if (BitBuffer->GetEndOfCompression())
	{
		// Slow decode
		NBits = 1;
		Output = HuffmanDecodeFallBack(Table, NBits);
		if (Output < 0) return false;

		return true;
	}

	// Get next HUFFMAN_LOOK_AHEAD_BITS
	Result = BitBuffer->SeeBits(CheckBits, HUFFMAN_LOOK_AHEAD_BITS);
	STOPPROCESS(Result);

	// If there is still not enough bits
	if (BitBuffer->GetAvailableBitCount() < HUFFMAN_LOOK_AHEAD_BITS)
	{
		// Slow decode using fall back
		NBits = 1;
		Output = HuffmanDecodeFallBack(Table, NBits);
		if (Output < 0) return false;

		return true;
	}

	// Fast decoding with lookup table 
	if ((NBits = Table->LookNBits[CheckBits]) != 0)
	{
		BitBuffer->PassBits(NBits);
		Output = Table->LookSymbol[CheckBits];
	}
	else // Table miss, fall back to slow decoding method
	{
		NBits = HUFFMAN_LOOK_AHEAD_BITS + 1;
		Output = HuffmanDecodeFallBack(Table, NBits);
		if (Output < 0) return false;
	}

	return true;
}

// Creates the intermediate lookup tables for decompression
bool ZEJpegHuffmanDecoder::CreateDerivedTable(ZEUInt TableId, ZEJpegDerivedHuffmanTable* DerivedTable, bool IsDc)
{
	ZEInt8 HuffmanSize[257];
	ZEUInt HuffmanCode[257];
	
	if (/*TableId < 0 || */ TableId > ZE_JPEG_HUFF_TABLE_COUNT)
		zeCriticalError("Wrong table id");

	ZEJpegHuffmanTable* SourceTable = (IsDc) ? &Info->DCHuffmanTables[TableId] : &Info->ACHuffmanTables[TableId];
	DerivedTable->OwnerHuffmanTable = SourceTable;

	ZESize P = 0;
	ZESize I = 0;
	ZESize SymbolCount;
	for (ZESize L = 1; L <= 16; L++)
	{
		I = SourceTable->Bits[L];
		if (P + I > 256) 
			zeCriticalError("Table Overrun");

		while (I--)
			HuffmanSize[P++] = (ZEInt8)L;
	}

	HuffmanSize[P] = 0;
	SymbolCount = P;

	// Generate the codes themselves
	// We also validate that the counts represent a legal Huffman code tree.
	
	ZEUInt Code = 0;
	ZEInt SizeI = (ZEInt)HuffmanSize[0];
	P = 0;
	while (HuffmanSize[P])
	{
		while ((ZEInt)HuffmanSize[P] == SizeI)
		{
			HuffmanCode[P++] = Code;
			Code++;
		}

		//	Code is now 1 more than the last code used for codelength si;
		//	But it must still fit in si bits, since no code is allowed to be all ones.
		
		Code <<= 1;
		SizeI++;
	}

	// Generate decoding tables for bit-sequential decoding
	P = 0;
	for (ZESize L = 1; L <= 16; ++L)
	{
		if (SourceTable->Bits[L] != 0)
		{
			/*
				ValueOffset[l] = HuffmanValues[] index of 1st symbol of code length l,
				minus the minimum code of length l
			*/

			DerivedTable->ValueOffsets[L] = (ZEInt32)P - (ZEInt32)HuffmanCode[P];
			P += SourceTable->Bits[L];
			DerivedTable->MaxCodeLenght[L] = HuffmanCode[P - 1];
		}
		else
		{
			DerivedTable->MaxCodeLenght[L] = -1; // Not used
		}
	}
	DerivedTable->MaxCodeLenght[17] = 0xFFFFFL;		/* ensures jpeg_huff_decode terminates */

	// Compute lookahead tables to speed up decoding. First we set all the table entries to 0, indicating "too long"; then we iterate through 
	// the Huffman codes that are short enough and fill in all the entries that correspond to bit sequences starting with that code.

	memset(DerivedTable->LookNBits, 0, sizeof(ZEInt) * (1 << HUFFMAN_LOOK_AHEAD_BITS));

	P = 0;
	for (ZESize L = 1; L <= HUFFMAN_LOOK_AHEAD_BITS; ++L)
	{
		for (ZESize K = 1; K <= (ZESize)SourceTable->Bits[L]; ++K, ++P)
		{
			// l = current code's length, p = its index in HuffmanCode[] & HuffmanValues[]. 
			// Generate left-justified code followed by all possible bit sequences.
			
			ZEInt LookBits = (HuffmanCode[P] << (HUFFMAN_LOOK_AHEAD_BITS - L));

			for (ZEInt Ctr = (1 << (HUFFMAN_LOOK_AHEAD_BITS - L)); Ctr > 0; --Ctr)
			{
				DerivedTable->LookNBits[LookBits] = (ZEInt)L;
				DerivedTable->LookSymbol[LookBits] = SourceTable->HuffmanValues[P];
				++LookBits;
			}
		}
	}


	// Check table values
	if (IsDc)
	{
		for (ZESize I = 0; I < SymbolCount; ++I)
		{
			ZEInt Sym = SourceTable->HuffmanValues[I];

			if (Sym < 0 || Sym > 15)
				zeCriticalError("Wrong table generation..");
		}
	}

	return true;
}

// NOT TESTED
// MCU DC initial scan. Can be used for both spectral selection and successive approximation modes
bool ZEJpegHuffmanDecoder::MCUDecodeDcFirstTime(ZEJpegMcu* McuData)
{
	if (Info->RestartInterval != 0)
		if (McusLeftToRestart == 0)
			STOPPROCESS(Restart());

	ZEInt AL = Info->ApproxBitPosLow;

	// Outer loop handles each block in the MCU
	for (ZESize BlockN = 0; BlockN < (ZESize)Info->DctBlocksPerMCU; ++BlockN)
	{
		ZEJpegDataBlock* Block = &McuData->McuData[BlockN];
		ZESize Member = (ZESize)Info->MCUMembership[BlockN];
		ZEJpegComponentInfo* Component = Info->CurrentCompInfo[Member];
		ZEJpegDerivedHuffmanTable* DerivedDecodeTable = DerivedTables[Component->DcTableNo];

		ZEInt32 ReadBits = 0;
		ZEInt32 BitCount = 0;
		ZEUInt8 OutputSymbol = 0;

		bool Result = HuffmanDecode(BitCount, DerivedDecodeTable);
		STOPPROCESS(Result);

		if (BitCount)
		{
			Result = BitBuffer->GetBits(ReadBits, BitCount);
			STOPPROCESS(Result);

			OutputSymbol = EXTEND(ReadBits, BitCount);
		}

		OutputSymbol += LastDcValue[Member];
		LastDcValue[Member] = OutputSymbol;

		Block->BlockData.OneDim[0] = OutputSymbol << AL;
	}

	McusLeftToRestart--;

	return true;
}

// NOT TESTED
// MCU AC initial scan. Can be used for both spectral selection and successive approximation modes
bool ZEJpegHuffmanDecoder::MCUDecodeAcFirstTime(ZEJpegMcu* McuData)
{
	if (Info->RestartInterval != 0)
		if (McusLeftToRestart == 0)
			STOPPROCESS(Restart());

	// NOTE: There is always only one block per MCU

	// ZeJpegZigZagToNaturalOrder8x8[I]
	ZEInt AL = Info->ApproxBitPosLow;
	ZESize SS = (ZESize)Info->SpectralSelectionStart;
	ZESize SE = (ZESize)Info->SpectralSelectionEnd;
	
	const ZESize* ZZToNTable = Info->NaturalOrderPosArray;
	// ZEJpegDerivedHuffmanTable* ACTable = this->

	ZEInt32 ReadBits = 0;
	ZEInt32 BitCount = 0;
	ZEUInt8 OutputSymbol = 0;

	if (SeenEobCount > 0 )
	{
		--SeenEobCount;
	}
	else
	{
		ZEJpegDataBlock* Block = &McuData->McuData[0];
		ZEJpegDerivedHuffmanTable* Table = CurrentAcDerivedTable;

		// Outer loop handles each block in the MCU
		for (ZESize I = SS; I <= SE; ++I)
		{
			bool Result = HuffmanDecode(BitCount, Table);
			STOPPROCESS(Result);

			ReadBits = BitCount >> 4;	// Get encoded 0 count
			BitCount &= 0x0F;			// Get valid 15 bits

			if (BitCount)
			{
				I += (ZESize)ReadBits;
				Result = BitBuffer->GetBits(ReadBits, BitCount);
				BitCount = EXTEND(ReadBits, BitCount);

				Block->BlockData.OneDim[ZZToNTable[I]] = BitCount << AL;
			}
			else
			{
				if (ReadBits == 15)
				{
					I += 15;
				}
				else
				{
					SeenEobCount = 1 << ReadBits;

					if (ReadBits)
					{
						Result = BitBuffer->GetBits(ReadBits, ReadBits);
						STOPPROCESS(Result);
						SeenEobCount += ReadBits;
					}

					--SeenEobCount;
					break;
				}
			}
		}
	}

	McusLeftToRestart--;

	return true;
}

// NOT TESTED
// MCU DC successive approximation refinement scan decoding
bool ZEJpegHuffmanDecoder::MCUDecodeDcRefine(ZEJpegMcu* McuData)
{
	if (Info->RestartInterval != 0)
		if (McusLeftToRestart == 0)
			STOPPROCESS(Restart());

	ZEInt PlusOne = 1 << Info->ApproxBitPosLow;
	
	ZEJpegDerivedHuffmanTable* Table = CurrentAcDerivedTable;

	// Outer loop handles each block in the MCU
	for (ZESize BlockN = 0; BlockN < (ZESize)Info->DctBlocksPerMCU; ++BlockN)
	{
		ZEJpegDataBlock* Block = &McuData->McuData[BlockN];

		ZEInt32 ReadBit = 0;
		// Encoded data is simply the next bit of the two's-complement DC value
		bool Result = BitBuffer->GetBits(ReadBit, 1);
		STOPPROCESS(Result);

		if (ReadBit)
		{
			Block->BlockData.OneDim[0] |= PlusOne;
		}
	}

	--McusLeftToRestart;

	return true;
}

// NOT TESTED
// MCU AC successive approximation refinement scan decoding
bool ZEJpegHuffmanDecoder::MCUDecodeAcRefine(ZEJpegMcu* McuData)
{
	if (Info->RestartInterval != 0)
		if (McusLeftToRestart == 0)
			STOPPROCESS(Restart());

	// There is always only one block per MCU

	ZEJpegDataBlock* Block = &McuData->McuData[0];
	ZESize SE = (ZESize)Info->SpectralSelectionEnd;
	ZEInt PlusOne = 1 << Info->ApproxBitPosLow;
	ZEInt MinusOne = (-1) << Info->ApproxBitPosLow;
	const ZESize* ZZNaturalTable = Info->NaturalOrderPosArray;

	ZESize BitIndex = (ZESize)Info->SpectralSelectionStart;
	ZEJpegDerivedHuffmanTable* DerivedDecodeTable = CurrentAcDerivedTable;

	bool Result = false;
	ZEInt32 ReadBits = 0;
	ZEInt32 BitCount = 0;
	ZEUInt8 OutputSymbol = 0;
	
	if (SeenEobCount == 0)
	{
		for (/* NoInit*/; BitIndex <= SE; ++BitIndex)
		{
			Result = HuffmanDecode(BitCount, DerivedDecodeTable);
			STOPPROCESS(Result);

			ReadBits = BitCount >> 4;
			BitCount &= 0x0F;

			if (BitCount)
			{
				if (BitCount != 1) // Must be one
				{
					STOPPROCESS(true);
				}

				ZEInt32 Dummy = 0;
				Result = BitBuffer->GetBits(Dummy, 1);
				STOPPROCESS(Result);

				if (Dummy)
				{
					BitCount = PlusOne;
				}
				else
				{
					BitCount = MinusOne;
				}
			}
			else
			{
				if (ReadBits != 15)
				{
					SeenEobCount = 1 << ReadBits;
					if (ReadBits)
					{
						Result = BitBuffer->GetBits(ReadBits, ReadBits);
						STOPPROCESS(Result);

						SeenEobCount += ReadBits;
					}

					break;
				}
			}

			do 
			{
				// Make a copy of coefficient
				ZEInt* Coefficient = &Block->BlockData.OneDim[ZZNaturalTable[BitIndex]];

				if (*Coefficient != 0)
				{
					ZEInt32 Dummy;
					Result = BitBuffer->GetBits(Dummy, 1);
					STOPPROCESS(Result);

					if (Dummy)
					{
						if ((*Coefficient & PlusOne) == 0)
						{
							if (*Coefficient >= 0)
							{
								*Coefficient += PlusOne;
							}
							else
							{
								*Coefficient += MinusOne;
							}
						}
					}
				}
				else
				{
					if (--ReadBits < 0)
						break;
				}

				++BitIndex;

			} while (BitIndex <= SE);


			if (BitCount)
			{
				ZESize Position = ZZNaturalTable[BitIndex];

				Block->BlockData.OneDim[Position] = BitCount;
			}
		}
	}

	if (SeenEobCount > 0)
	{
		for (/*NoInit*/; BitIndex < SE; ++BitIndex)
		{
			// Make a copy of coefficient
			ZEInt* Coefficient = &Block->BlockData.OneDim[ZZNaturalTable[BitIndex]];
			
			if (*Coefficient != 0)
			{
				ZEInt32 Dummy;
				Result = BitBuffer->GetBits(Dummy, 1);
				STOPPROCESS(Result);

				if (Dummy)
				{
					if ((*Coefficient & PlusOne) == 0)
					{
						if (*Coefficient >= 0)
						{
							*Coefficient += PlusOne;
						}
						else
						{
							*Coefficient += MinusOne;
						}
					}
				}
			}
		}

		--SeenEobCount;
	}

	--McusLeftToRestart;

	return true;
}

// TESTED
// Full size MCU block decoding, uses the precalculated tables
bool ZEJpegHuffmanDecoder::MCUDecodeFullBlock(ZEJpegMcu* McuData)
{
	// static ZEUInt MCUCount = 1;

	if (Info->RestartInterval != 0)
		if (McusLeftToRestart == 0)
			STOPPROCESS(Restart());
	
	bool EOB;
	
	
	// Clean the buffer
	ZEJpegMcu::ZeroData(McuData);

	// Outer loop handles each block in the MCU
	for (ZESize BlockN = 0; BlockN < (ZESize)Info->DctBlocksPerMCU; ++BlockN)
	{
		EOB = false;
		ZEJpegDataBlock* Block = &McuData->McuData[BlockN];

		ZEJpegDerivedHuffmanTable* DerivedDecodeTable = CurrentDcTables[BlockN];

		// Decode a single block's worth of coefficients

		// decode the DC coefficient difference
		ZEInt32 ReadBits = 0;
		ZEInt32 BitCount = 0;
		ZEInt OutputSymbol = 0;

		bool Result = this->HuffmanDecode(BitCount, DerivedDecodeTable);
		STOPPROCESS(Result);

		// Decoding block index
		ZESize I = 1;
		ZESize CoefficientLimit = CoefficientLimits[BlockN];

		if (CoefficientLimit)
		{
			// Convert DC difference to actual value, update last_dc_val
			if (BitCount)
			{
				Result = BitBuffer->GetBits(ReadBits, BitCount);
				STOPPROCESS(Result);

				OutputSymbol = EXTEND(ReadBits, BitCount);
			}

			ZEInt Member = Info->MCUMembership[BlockN];
			OutputSymbol += LastDcValue[Member];
			LastDcValue[Member] = OutputSymbol;

			// Output the DC coefficient
			Block->BlockData.OneDim[0] = OutputSymbol;

			// Decode AC coefficients
			// Change table to ac
			DerivedDecodeTable = CurrentAcTables[BlockN];
			for (/* No Init */; I < CoefficientLimit; ++I)
			{

				Result = HuffmanDecode(BitCount, DerivedDecodeTable);
				STOPPROCESS(Result);

				ReadBits = BitCount >> 4;
				BitCount &= 0x0F;

				if (BitCount)
				{
					I += (ZESize)ReadBits;
					Result = BitBuffer->GetBits(ReadBits, BitCount);
					STOPPROCESS(Result);

					OutputSymbol = EXTEND(ReadBits, BitCount);
					Block->BlockData.OneDim[ZeJpegZigZagToNaturalOrder8x8[I]] = OutputSymbol;
				}
				else
				{
					if (ReadBits != 15) // if 15 zeroes not found block is done
					{
						EOB = true;
						break;
					}
					
					I += 15;
				}
			}

			if (EOB)
				continue;
		}
		else
		{
			if (BitCount)
			{
				Result = BitBuffer->PassBits(BitCount);
				STOPPROCESS(Result);
			}
		}
		
		// Discard the rest of the values
		for (/* No Init */; I < ZE_JPEG_DCT_BLOCK_COEFF_COUNT; ++I)
		{
			if (EOB) break; // Do not process if EOB reached

			Result = HuffmanDecode(BitCount, DerivedDecodeTable);
			STOPPROCESS(Result);

			ReadBits = BitCount >> 4;
			BitCount &= 0x0F;

			if (BitCount)
			{
				I += (ZESize)ReadBits;
				Result = BitBuffer->PassBits(BitCount);
				STOPPROCESS(Result);
			}
			else
			{
				if (ReadBits != 15) // if 15 zeroes not found block is done
				{
					break;
				}

				I += 15;
			}
		}
		
	}

	McusLeftToRestart--;
	
	/*
	FILE* File = fopen("ZEMCUDATA.txt", "a");

	fprintf(File, "*********************\n");
	fprintf(File, "MCU %d - Start\n", MCUCount);
	fprintf(File, "*********************\n");

	for (ZESize BlockN = 0; BlockN < Info->DctBlocksPerMCU; ++BlockN)
	{
	ZEJpegDataBlock* Block = &McuData->McuData[BlockN];

	fprintf(File, "\n");
	fprintf(File, "---------------------\n");
	fprintf(File, "Block %d - Start\n", BlockN);
	fprintf(File, "---------------------\n");

	fprintf(File, "\n");
	for (ZESize Coef = 0; Coef < 64; ++Coef)
	{
	fprintf(File, "%d\n", Block->BlockData[Coef]);
	}
	fprintf(File, "\n");

	}
	fflush(File);
	fclose(File);
	
	MCUCount ++;
	*/
	
	return true;
}

// NOT TESTED
// Decodes a partial block
bool ZEJpegHuffmanDecoder::MCUDecodeSubBlock(ZEJpegMcu* McuData)
{
	if (Info->RestartInterval != 0)
		if (McusLeftToRestart == 0)
			STOPPROCESS(Restart());

	bool EOB = false;
	const ZESize* ZZNaturalTable = Info->NaturalOrderPosArray;
	ZEInt SE = Info->SpectralSelectionEnd;

	// Outer loop handles each block in the MCU
	for (ZESize BlockN = 0; BlockN < (ZESize)Info->DctBlocksPerMCU; ++BlockN)
	{

		ZEJpegDataBlock* Block = &McuData->McuData[BlockN];
		ZEJpegDerivedHuffmanTable* DerivedDecodeTable = CurrentDcTables[BlockN];

		// Decode a single block's worth of coefficients

		// decode the DC coefficient difference
		ZEInt32 ReadBits = 0;
		ZEInt32 BitCount = 0;
		ZEUInt8 OutputSymbol = 0;

		bool Result = this->HuffmanDecode(BitCount, DerivedDecodeTable);
		STOPPROCESS(Result);

		// Decoding block index
		ZESize I = 1;
		ZESize CoefficientLimit = CoefficientLimits[BlockN];

		if (CoefficientLimit)
		{
			// Convert DC difference to actual value, update last_dc_val
			if (BitCount)
			{
				Result = BitBuffer->GetBits(ReadBits, BitCount);
				STOPPROCESS(Result);

				OutputSymbol = EXTEND(ReadBits, BitCount);
			}

			ZEInt Member = Info->MCUMembership[BlockN];
			OutputSymbol += LastDcValue[Member];
			LastDcValue[Member] = OutputSymbol;

			// Output the DC coefficient
			Block->BlockData.OneDim[0] = OutputSymbol;

			// Decode AC coefficients
			// Change table to ac
			DerivedDecodeTable = CurrentAcTables[BlockN];
			for (/* No Init */; I < CoefficientLimit; ++I)
			{
				Result = HuffmanDecode(BitCount, DerivedDecodeTable);
				STOPPROCESS(Result);

				ReadBits = BitCount >> 4;
				BitCount &= 0x0F;

				if (BitCount)
				{
					I += (ZESize)ReadBits;
					Result = BitBuffer->GetBits(ReadBits, BitCount);
					STOPPROCESS(Result);

					OutputSymbol = EXTEND(ReadBits, BitCount);
					Block->BlockData.OneDim[ZZNaturalTable[I]] = OutputSymbol;
				}
				else
				{
					if (ReadBits != 15) // if 15 zeroes not found block is done
					{
						break;
					}

					I += 15;
				}
			}
		}
		else
		{
			if (BitCount)
			{
				Result = BitBuffer->PassBits(BitCount);
				STOPPROCESS(Result);
			}
		}

		// Discard the rest of the values of the BlockN
		for (/* No Init */; I < ZE_JPEG_DCT_BLOCK_COEFF_COUNT; ++I)
		{
			if (EOB) break; // Do not process if EOB reached

			Result = HuffmanDecode(BitCount, DerivedDecodeTable);
			STOPPROCESS(Result);

			ReadBits = BitCount >> 4;
			BitCount &= 0x0F;

			if (BitCount)
			{
				I += (ZESize)ReadBits;
				Result = BitBuffer->PassBits(BitCount);
				STOPPROCESS(Result);
			}
			else
			{
				if (ReadBits != 15) // if 15 zeroes not found block is done
				{
					break;
				}

				I += 15;
			}
		}
	}

	McusLeftToRestart--;

	return true;
}


// Creates the intermediate decoding tables which hold the all possible bit strings whose length are given in Huffman tables
void ZEJpegHuffmanDecoder::Initialize(ZEJpegDeCompressionInfo* Info, ZEJpegFileMarkerReader* MarkerReader)
{
	zeAssert(Info == NULL, "NULL Pointer");
	zeAssert(MarkerReader == NULL, "NULL Pointer");

	this->Info = Info;
	this->MarkerReeader = MarkerReader;

	BitBuffer = HuffmanDecoderBitBuffer::CreateInstance();
	BitBuffer->Initialize(MarkerReader->GetMarkerBuffer());

	// Allocate tables
	for (ZESize I = 0; I < ZE_JPEG_HUFF_TABLE_COUNT; ++I)
	{
		if (DerivedTables[I] == NULL)
			DerivedTables[I] = new ZEJpegDerivedHuffmanTable[ZE_JPEG_DCT_BLOCK_COEFF_COUNT];
	}
	

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
			if (Info->SpectralSelectionEnd < Info->SpectralSelectionStart ||
				Info->SpectralSelectionEnd > Info->SpectralSelectionEndLimit)
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

			for (ZESize Coeff = (ZESize)Info->SpectralSelectionStart; Coeff <= (ZESize)Info->SpectralSelectionEnd; ++Coeff)
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
				DecodeFunction = &ZEJpegHuffmanDecoder::MCUDecodeDcFirstTime;
			}
			else
			{
				DecodeFunction = &ZEJpegHuffmanDecoder::MCUDecodeAcFirstTime;
			}
		}
		else
		{
			if (Info->SpectralSelectionStart == 0)
			{
				this->DecodeFunction = &ZEJpegHuffmanDecoder::MCUDecodeDcRefine;
			}
			else
			{
				this->DecodeFunction = &ZEJpegHuffmanDecoder::MCUDecodeAcRefine;
			}
		}

		for (ZESize I = 0; I < Info->ComponentsInScan; ++I)
		{
			ZEJpegComponentInfo* ComponentInfo = Info->CurrentCompInfo[I];

			if (Info->SpectralSelectionStart == 0)
			{
				if (Info->ApproxBitPosHigh == 0)
				{
					// DC refinement needs no table
					this->CreateDerivedTable(ComponentInfo->DcTableNo, DerivedTables[(ZESize)ComponentInfo->DcTableNo], true);
				}
			}
			else
			{
				this->CreateDerivedTable(ComponentInfo->AcTableNo, DerivedTables[(ZESize)ComponentInfo->AcTableNo], false);
			}
		}

		SeenEobCount = 0;
	}
	else // Baseline
	{
		if (Info->SpectralSelectionStart != 0 || Info->ApproxBitPosHigh != 0 || Info->ApproxBitPosLow != 0 ||
			((Info->IsBaseLine || Info->SpectralSelectionEnd < ZE_JPEG_DCT_BLOCK_COEFF_COUNT) && Info->SpectralSelectionEnd != Info->SpectralSelectionEndLimit))
			zeCriticalError("Wong SOS aprameters for baseline decompression");

		if (Info->SpectralSelectionEnd != ZE_JPEG_DCT_BLOCK_COEFF_COUNT - 1)
		{
			this->DecodeFunction = &ZEJpegHuffmanDecoder::MCUDecodeSubBlock;
		}
		else
		{
			this->DecodeFunction = &ZEJpegHuffmanDecoder::MCUDecodeFullBlock;
		}

		for (ZESize I = 0; I < Info->ComponentsInScan; ++I)
		{
			ZEJpegComponentInfo* ComponentInfo = Info->CurrentCompInfo[I];
			CreateDerivedTable(ComponentInfo->DcTableNo, &DerivedDcTables[ComponentInfo->DcTableNo], true);
			
			if (Info->SpectralSelectionEndLimit != 0)
			{
				CreateDerivedTable(ComponentInfo->AcTableNo, &DerivedAcTables[ComponentInfo->AcTableNo], false);
			}
		}

		// Precalculate decoding info for each block in an MCU of this scan
		for (ZESize BlockI = 0; BlockI < (ZESize)Info->DctBlocksPerMCU; ++BlockI)
		{
			ZEJpegComponentInfo* ComponentInfo = Info->CurrentCompInfo[Info->MCUMembership[BlockI]];
			
			// Precalculate which table to use for each block
			CurrentDcTables[BlockI] = &DerivedDcTables[ComponentInfo->DcTableNo];
			CurrentAcTables[BlockI] = &DerivedAcTables[ComponentInfo->AcTableNo];

			// Decide whether we really care about the coefficient values
			if (ComponentInfo->ComponentValueNeeded)
			{
				ZEInt V = ComponentInfo->DctVertScaledSize;
				ZEInt H = ComponentInfo->DctHorzScaledSize;
				
				switch (Info->SpectralSelectionEndLimit)
				{
					case 1 * 1 -1:
						CoefficientLimits[BlockI] = 1;
						break;
					
					case 2 * 2 -1:
						if (V <= 0 || V > 2) V = 2;
						if (H <= 0 || H > 2) H = 2;
						CoefficientLimits[BlockI] = 1 + ZeJpegNaturalToZigZagOrder2x2[V - 1][H - 1];
						break;
					
					case 3 * 3 -1:
						if (V <= 0 || V > 3) V = 3;
						if (H <= 0 || H > 3) H = 3;
						CoefficientLimits[BlockI] = 1 + ZeJpegNaturalToZigZagOrder3x3[V - 1][H - 1];
						break;
					
					case 4 * 4 -1:
						if (V <= 0 || V > 4) V = 4;
						if (H <= 0 || H > 4) H = 4;
						CoefficientLimits[BlockI] = 1 + ZeJpegNaturalToZigZagOrder4x4[V - 1][H - 1];
						break;
					
					case 5 * 5 -1:
						if (V <= 0 || V > 5) V = 5;
						if (H <= 0 || H > 5) H = 5;
						CoefficientLimits[BlockI] = 1 + ZeJpegNaturalToZigZagOrder5x5[V - 1][H - 1];
						break;
					
					case 6 * 6 -1:
						if (V <= 0 || V > 6) V = 6;
						if (H <= 0 || H > 6) H = 6;
						CoefficientLimits[BlockI] = 1 + ZeJpegNaturalToZigZagOrder6x6[V - 1][H - 1];
						break;
					
					case 7 * 7 -1:
						if (V <= 0 || V > 7) V = 7;
						if (H <= 0 || H > 7) H = 7;
						CoefficientLimits[BlockI] = 1 + ZeJpegNaturalToZigZagOrder7x7[V - 1][H - 1];
						break;
					
					default:
					case 8 * 8 -1:
						if (V <= 0 || V > 8) V = 8;
						if (H <= 0 || H > 8) H = 8;
						CoefficientLimits[BlockI] = 1 + ZeJpegNaturalToZigZagOrder8x8[V - 1][H - 1];
						break;
					
				}
			}
			else
			{
				CoefficientLimits[BlockI] = 0;
			}
		}
	}

	McusLeftToRestart = Info->RestartInterval;
}

void ZEJpegHuffmanDecoder::Deinitialize()
{
	Info				= NULL;
	SeenEobCount		= 0;
	McusLeftToRestart	= 0;

	if (BitBuffer != NULL)
	{
		BitBuffer->Destroy();
		BitBuffer = NULL;
	}

	// Delete tables
	for (ZESize I = 0; I < ZE_JPEG_HUFF_TABLE_COUNT; ++I)
	{
		if (DerivedTables[I] != NULL)
		{
			delete [] DerivedTables[I];
			DerivedTables[I] = NULL;
		}
	}

	McusLeftToRestart	= 0;

	// Set pointers to other tables to zero
	memset(DerivedDcTables, 0, sizeof(ZEJpegDerivedHuffmanTable) * ZE_JPEG_HUFF_TABLE_COUNT);
	memset(DerivedAcTables, 0, sizeof(ZEJpegDerivedHuffmanTable) * ZE_JPEG_HUFF_TABLE_COUNT);

	memset(CurrentDcTables, 0, sizeof(ZEJpegDerivedHuffmanTable*) * ZE_JPEG_MAX_BLOCKS_IN_MCU);
	memset(CurrentAcTables, 0, sizeof(ZEJpegDerivedHuffmanTable*) * ZE_JPEG_MAX_BLOCKS_IN_MCU);

	memset(LastDcValue, 0, sizeof(ZEInt) * ZE_JPEG_MAX_COMPONENTS_IN_SCAN);
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
	Info				= NULL;
	BitBuffer			= NULL;
	SeenEobCount		= 0;
	McusLeftToRestart	= 0;
	
	memset(DerivedDcTables, 0, sizeof(ZEJpegDerivedHuffmanTable) * ZE_JPEG_HUFF_TABLE_COUNT);
	memset(DerivedAcTables, 0, sizeof(ZEJpegDerivedHuffmanTable) * ZE_JPEG_HUFF_TABLE_COUNT);
	
	memset(CurrentDcTables, 0, sizeof(ZEInt*) * ZE_JPEG_MAX_BLOCKS_IN_MCU);
	memset(CurrentAcTables, 0, sizeof(ZEInt*) * ZE_JPEG_MAX_BLOCKS_IN_MCU);

	memset(LastDcValue, 0, sizeof(ZEInt) * ZE_JPEG_MAX_COMPONENTS_IN_SCAN);
	memset(DerivedTables, 0, sizeof(ZEInt*) * ZE_JPEG_MAX_COMPONENTS_IN_SCAN);
	
}

ZEJpegHuffmanDecoder::~ZEJpegHuffmanDecoder()
{
	this->Deinitialize();
}
