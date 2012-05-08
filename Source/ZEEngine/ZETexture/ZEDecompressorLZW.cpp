//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDecompressorLZW.cpp
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

#include "ZEDecompressorLZW.h"

#include "ZEError.h"
#include <memory.h>

ZEUInt8 ZEDecompressorLZW::DictionaryConstants[256] =
{
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
	0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
	0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
	0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
	0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
	0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
	0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};

bool ZEDecompressorLZW::GetNextWord(ZEUInt16& Word)
{
	if (Input == NULL)
		return 0;

	if (InputPosition > InputSize)
		return false;

	ZEInt RemainingBitCount = WordSize - LastWordSize;
	Word = LastWord << RemainingBitCount;

	ZEInt PostRemainingBitCount;
	if (RemainingBitCount <= 8) 
	{
		// Byte Boundary
		ZEUInt8 PartialWord = *((ZEUInt8*)Input + InputPosition);
		LastWordSize = (8 - RemainingBitCount);
		Word |= PartialWord >> LastWordSize;
		LastWord = (0xFF >> RemainingBitCount) & PartialWord;
		InputPosition++;
	}
	else 
	{
		if (InputPosition + 1 > InputSize)
		{
			ZEUInt8 PartialWord = *((ZEUInt8*)Input + InputPosition);
			LastWord = (LastWord << 8) | PartialWord;
			LastWordSize += 8;
			InputPosition++;
			return false;
		}

		// Word Boundary
		ZEUInt16 PartialWord = *(ZEUInt16*)((ZEUInt8*)Input + InputPosition);
		LastWordSize = (16 - RemainingBitCount);
		Word |= PartialWord >> LastWordSize;
		LastWord = (0xFFFF >> RemainingBitCount) & PartialWord;
		InputPosition += 2;
	}

	return true;
}

bool ZEDecompressorLZW::WriteOutput(ZEDictionaryEntryLZW* Entry)
{
	if (Entry != NULL)
		OutputEntry = Entry;
	
	if (OutputPosition + OutputEntry->Size - OutputEntryByteIndex > OutputSize)
	{
		memcpy((ZEUInt8*)Output + OutputEntryByteIndex, (ZEUInt8*)OutputEntry->Data + OutputEntryByteIndex, OutputEntry->Size - OutputEntryByteIndex);
		return true;
	}
	else
	{
		memcpy((ZEUInt8*)Output + OutputEntryByteIndex, (ZEUInt8*)OutputEntry->Data + OutputEntryByteIndex, OutputEntry->Size - OutputEntryByteIndex);
		return false;
	}
}

void ZEDecompressorLZW::AddToDictionary(ZEDictionaryEntryLZW* Entry)
{
	if (PrevEntry == NULL || DictionarySize >= 4096)
		return;

	Dictionary[DictionarySize + 1].Data = new ZEUInt8[Entry->Size + 1];
	memcpy(Dictionary[DictionarySize + 1].Data, Entry->Data, Entry->Size);
	((ZEUInt8*)Dictionary[DictionarySize + 1].Data)[Entry->Size] = ((ZEUInt8*)PrevEntry->Data)[PrevEntry->Size - 1];

	DictionarySize++;

	if (DictionarySize == 512)
		DictionarySize = 10;
	else if (DictionarySize == 1024)
		DictionarySize = 11;
	else if (DictionarySize == 2048)
		DictionarySize = 12;
}

void ZEDecompressorLZW::ResetDictionary()
{
	for (ZESize I = 0; I < 256; I++)
	{
		Dictionary[I].Data = &DictionaryConstants[I];
		Dictionary[I].Size = 1;
	}

	for (ZESize I = 258; I < 4096; I++)
	{
		if (Dictionary[I].Data != NULL)
			delete[] Dictionary[I].Data;
		Dictionary[I].Size = 0;
	}

	DictionarySize = 258;
}

void ZEDecompressorLZW::SetInput(void* Buffer)
{
	Input = Buffer;
}

void* ZEDecompressorLZW::GetInput()
{
	return Input;
}

void ZEDecompressorLZW::SetInputSize(ZESize Size)
{
	InputSize = Size;
}

ZESize ZEDecompressorLZW::GetInputSize()
{
	return InputSize;
}

void ZEDecompressorLZW::SetOutput(void* Buffer)
{
	Output = Buffer;
	OutputPosition = 0;
}

void* ZEDecompressorLZW::GetOutput()
{
	return Output;
}

void ZEDecompressorLZW::SetOutputSize(ZESize Size)
{
	OutputSize = Size;
}

ZESize ZEDecompressorLZW::GetOutputSize()
{
	return OutputSize;
}

ZEDecompressorState ZEDecompressorLZW::Decompress()
{
	if (Input == NULL || Output == NULL)
		return ZE_DS_ERROR;

	if (State == ZE_DS_ERROR)
		return ZE_DS_ERROR;
	else if (State == ZE_DS_OUTPUT_FULL)
	{
		OutputPosition = 0;
		State = ZE_DS_NONE;
	}
	else if (State == ZE_DS_INPUT_PROCESSED)
	{
		InputPosition = 0;
		State = ZE_DS_NONE;
	}
	else if (State == ZE_DS_DONE)
		return ZE_DS_NONE;

	if (!WriteOutput())
		return ZE_DS_OUTPUT_FULL;

	while(true)
	{

		ZEUInt16 CurrentWord;
		if (!GetNextWord(CurrentWord))
			return ZE_DS_INPUT_PROCESSED;

		if (CurrentWord >= DictionarySize)
			return ZE_DS_ERROR;

		if (CurrentWord == 256)
			return ZE_DS_DONE;
		else if (CurrentWord == 257)
			ResetDictionary();
		else
		{
			ZEDictionaryEntryLZW* OutputEntry = &Dictionary[CurrentWord];

			if (OutputEntry->Data == NULL)
				return ZE_DS_ERROR;

			if (!WriteOutput(OutputEntry));
				return ZE_DS_OUTPUT_FULL;;

			AddToDictionary(OutputEntry);
			PrevEntry = &Dictionary[CurrentWord];
		}
	}
}

void ZEDecompressorLZW::Reset()
{
	InputPosition = 0;
	OutputPosition = 0;
	
	WordSize = 9;
	LastWord = 0;
	LastWordSize = 0;

	PrevEntry = NULL;
	OutputEntry = NULL;
	OutputEntryByteIndex = 0;

	State = ZE_DS_NONE;

	ResetDictionary();
}

ZEDecompressorLZW::ZEDecompressorLZW()
{
	Input = NULL;
	InputSize = 0;
	Output = NULL;
	OutputSize = 0;

	Reset();
}

ZEDecompressorLZW::~ZEDecompressorLZW()
{
	Reset();
}
