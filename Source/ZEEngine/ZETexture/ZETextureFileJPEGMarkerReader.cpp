//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureFileJPEGMarkerReader.cpp
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

#include "ZETextureFileJPEGMarkerReader.h"
#include "ZETextureFileJPEG.h"
#include "ZEFile/ZEFile.h"
#include "ZEEndian.h"
#include "ZETypes.h"
#include "ZEError.h"
#include "ZETextureFileJPEGCommonDefinitions.h"




/************************************************************************/
/*                        ZEJpegFileMarkerBuffer                        */
/************************************************************************/

// Gets data from image file and fills the buffer
_inline bool ZEJpegFileMarkerBuffer::FillBuffer(bool ForceFill)
{
	// If not forced to fill buffer
	if (!ForceFill)
	{
		// If all the data is not consumed
		if (BytesAvailable != 0)
			return true;
	}
	

	// Try to fill whole buffer
	if (ImageFile->Read((void*)Buffer, ZE_JPEG_INPUT_BUFFER_SIZE, 1) != 1)
	{
		// There might be less available data than the ZE_JPEG_INPUT_BUFFER_SIZE
		ZESize Size = ImageFile->GetFileSize() - ImageFile->Tell();
		// Confirm that the remaining data is less than ZE_JPEG_INPUT_BUFFER_SIZE
		if (Size >= ZE_JPEG_INPUT_BUFFER_SIZE)
		{
			zeCriticalError("Cannot read the jpeg file...");
			return false;
		}

		if (ImageFile->Read(Buffer + (ZE_JPEG_INPUT_BUFFER_SIZE - Size), Size, 1) != 1)
		{
			zeCriticalError("Cannot read the jpeg file...");
			return false;
		}

		BytesAvailable = Size;
		BytesRead = BytesAvailable;
		return true;

	}
	
	BytesAvailable = ZE_JPEG_INPUT_BUFFER_SIZE;
	BytesRead = BytesAvailable;
	return true;

}

// Skips data equal to DataLenght and refills the buffer
bool ZEJpegFileMarkerBuffer::SkipBufferData(ZEUInt32 DataLenght)
{
	if ((ZESize)DataLenght <= BytesAvailable)
	{
		BytesAvailable -= (ZESize)DataLenght;
		return true;
	}
	else
	{
		// Drop available bytes
		ZESize Skipped = BytesAvailable;

		while (Skipped <= (ZESize)DataLenght)
		{
			// Force to fill the buffer to skip data
			if (!this->FillBuffer(true))
				return false;

			Skipped += BytesRead;
		}

		BytesAvailable = Skipped - (ZESize)DataLenght;

		return true;
	}

	return false;
}

ZEJpegFileMarkerBuffer::ZEJpegFileMarkerBuffer()
{
	ImageFile		= NULL;
	BytesAvailable	= 0;
	BytesRead		= 0;
}

ZEJpegFileMarkerBuffer::~ZEJpegFileMarkerBuffer()
{
	this->Deinitialize();
}

_inline ZESize ZEJpegFileMarkerBuffer::GetAvailableByteCount() const
{
	return BytesAvailable;
}

// Gets one byte
_inline bool ZEJpegFileMarkerBuffer::GetOneByte(ZEUInt8& Byte)
{
	// Fill the buffer if needed
	if (!this->FillBuffer())
		return false;

	Byte = Buffer[ZE_JPEG_INPUT_BUFFER_SIZE - BytesAvailable];

	BytesAvailable -= 1;
	return true;
}

// Gets two arbitrary bytes one by one
_inline bool ZEJpegFileMarkerBuffer::GetTwoByte(ZEUInt8& Byte1, ZEUInt8& Byte2)
{
	if (!GetOneByte(Byte1))
		return false;
	if (!GetOneByte(Byte2))
		return false;

	return true;
}

// Gets two arbitrary byte and converts to little endian
_inline bool ZEJpegFileMarkerBuffer::GetTwoByte(ZEUInt16& TwoByte)
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

		Result = GetOneByte(Byte1);
		STOPPROCESS(Result);
		Result = GetOneByte(Byte2);
		STOPPROCESS(Result);

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

bool ZEJpegFileMarkerBuffer::RewindOneByte()
{
	// This buffer is just loaded to we need to read previous buffer data
	if (BytesAvailable == ZE_JPEG_INPUT_BUFFER_SIZE)
	{
		// Go to previous buffer start position
		if (!ImageFile->Seek(-2 * (ZESSize)BytesRead, ZE_SF_CURRENT))
		{
			zeCriticalError("Cannot read jpeg data");
			return false;
		}
		
		// Discard available bytes
		BytesAvailable = 0;
		
		// Load
		if (!this->FillBuffer())
			return false;
		
		BytesAvailable = 1;
		return true;
	}
	else // Just move pointer one byte back
	{
		BytesAvailable += 1;
		return true;
	}
}
// Only use(d) in decompression !!!
bool ZEJpegFileMarkerBuffer::RewindTwoByte()
{
	// This buffer is just loaded to we need to read previous buffer data
	if (BytesAvailable > ZE_JPEG_INPUT_BUFFER_SIZE - 2)
	{
		// Rewind by one byte so the buffer will be loaded automatically
		if (!RewindOneByte())
			return false;
		if (!RewindOneByte())
			return false;

		return true;
	}
	else
	{
		BytesAvailable += 2;
		return true;
	}
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
			if (!this->GetOneByte(Byte))
				return false;

		} while (Byte != ZE_JPEG_FM_MARKER_BEGIN);

		// Then check if there are more arbitrary 0xFF
		// Arbitrary 0xFF are valid for padding
		do 
		{
			if (!this->GetOneByte(Byte))
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

/************************************************************************/
/*                        ZEJpegFileMarkerReader                        */
/************************************************************************/

// Just checks if the file stars with the SOI marker
bool ZEJpegFileMarkerReader::ProcessSOIMarker()
{
	// There must be only one SOI
	if (SOIFound)
		return false;

	// Do nothing with SOI
	SOIFound = true;
	return true;
}

// Just checks if the file ends with the EOI marker
bool ZEJpegFileMarkerReader::ProcessEOIMarker()
{
	// Do nothing with EOI
	EOIFound = true;
	return true;
}

// Loads JFIF data, does its best to get any information. Does not fails the decompression if error occurs
bool ZEJpegFileMarkerReader::ProcessAPP0Marker()
{
	ZEUInt16 TotalDataLenght = 0;
	ZEUInt8 App0Data[14] = {0};

	MarkerBuffer->GetTwoByte(TotalDataLenght);
	TotalDataLenght -= 2;

	// Get all application data
	MarkerBuffer->GetOneByte(App0Data[0]);
	MarkerBuffer->GetOneByte(App0Data[1]);
	MarkerBuffer->GetOneByte(App0Data[2]);
	MarkerBuffer->GetOneByte(App0Data[3]);
	MarkerBuffer->GetOneByte(App0Data[4]);


	// Check if it is right
	if (TotalDataLenght >=ZE_JPEG_APP0_DATA_LENGTH && App0Data[0] == 0x4A && App0Data[1] == 0x46 && App0Data[2] == 0x49 && App0Data[3] == 0x46 && App0Data[4] == 0)
	{
		MarkerBuffer->GetOneByte(App0Data[5]);
		MarkerBuffer->GetOneByte(App0Data[6]);
		MarkerBuffer->GetOneByte(App0Data[7]);
		MarkerBuffer->GetOneByte(App0Data[8]);
		MarkerBuffer->GetOneByte(App0Data[9]);
		MarkerBuffer->GetOneByte(App0Data[10]);
		MarkerBuffer->GetOneByte(App0Data[11]);
		MarkerBuffer->GetOneByte(App0Data[12]);
		MarkerBuffer->GetOneByte(App0Data[13]);

		TotalDataLenght -= ZE_JPEG_APP0_DATA_LENGTH;

		// Get thumbnail dimensions and check total size
		if (TotalDataLenght != (ZEUInt16)App0Data[12] * (ZEUInt16)App0Data[13] * (ZEUInt16)3)
		{
			zeWarning("APP0 marker data lenght is wrong. Skipping APP0 data.");
			// Do not get any further data
			return true;
		}

		Info->JFIFMarkerFound = true;
// 		Info->JFIFVersionMajor = App0Data[5];
// 		Info->JFIFVersionMinor = App0Data[6];
// 		Info->DensityUnit = App0Data[7];
// 		Info->DensityX = (App0Data[8] << 8) + App0Data[9];
// 		Info->DensityY = (App0Data[10] << 8) + App0Data[11];

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
bool ZEJpegFileMarkerReader::ProcessAPP14Marker()
{
	ZEUInt16 TotalDataLenght = 0;
	ZEUInt8 App0Data[12] = {0};

	MarkerBuffer->GetTwoByte(TotalDataLenght);
	TotalDataLenght -= 2;

	// Get all application data
	MarkerBuffer->GetOneByte(App0Data[0]);
	MarkerBuffer->GetOneByte(App0Data[1]);
	MarkerBuffer->GetOneByte(App0Data[2]);
	MarkerBuffer->GetOneByte(App0Data[3]);
	MarkerBuffer->GetOneByte(App0Data[4]);

	if (TotalDataLenght >= ZE_JPEG_APP14_DATA_LENGTH && App0Data[0] == 0x41 && App0Data[1] == 0x64 && App0Data[2] == 0x6F && App0Data[3] == 0x62 && App0Data[4] == 0x65)
	{
		// Adobe marker found
		MarkerBuffer->GetOneByte(App0Data[5]);
		MarkerBuffer->GetOneByte(App0Data[6]);
		MarkerBuffer->GetOneByte(App0Data[7]);
		MarkerBuffer->GetOneByte(App0Data[8]);
		MarkerBuffer->GetOneByte(App0Data[9]);
		MarkerBuffer->GetOneByte(App0Data[10]);
		MarkerBuffer->GetOneByte(App0Data[11]);

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
bool ZEJpegFileMarkerReader::ProcessSOFMarker(bool BaseLine, bool Progressive)
{

	if (SOFFound)
	{
		zeCriticalError("Duplicate SOF marker found.");
		return false;
	}

	ZEUInt8 ComponentFrequency = 0x00;
	ZEUInt16 SOFDataLenght = 0x0000;

	Info->IsBaseLine	= BaseLine;
	Info->IsProgressive = Progressive;

	// Get data length and subtract read bytes
	MarkerBuffer->GetTwoByte(SOFDataLenght);
	SOFDataLenght -= 2;

	// Get data precision, width, height component count and subtract read bytes
	
	// Do not store DataPrecision, just read and check
	ZEUInt8 DataPrecision;
	MarkerBuffer->GetOneByte(DataPrecision);
	if (DataPrecision != 8)
	{
		zeCriticalError("Only jepg files with 8 bit data precision are supported.");
		return false;
	}

	MarkerBuffer->GetTwoByte(Info->ImageHeight);
	MarkerBuffer->GetTwoByte(Info->ImageWidth);
	MarkerBuffer->GetOneByte(Info->ComponentCount);
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
		MarkerBuffer->GetOneByte(Info->ComponentInfo[I].ComponentId);

		// Get component's vertical horizontal frequency
		MarkerBuffer->GetOneByte(ComponentFrequency);
		Info->ComponentInfo[I].HorizontalFreq = (ComponentFrequency >> 4) & 0x0F;
		Info->ComponentInfo[I].VerticalFreq = ComponentFrequency & 0x0F;

		// Get quantization table number
		MarkerBuffer->GetOneByte(Info->ComponentInfo[I].QuantizationTableNo);
	}

	SOFFound = true;
	return true;
}

// Checks if marker is SOS and reads necessary data
bool ZEJpegFileMarkerReader::ProcessSOSMarker()
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
	MarkerBuffer->GetOneByte(ScanCompCount);

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
		MarkerBuffer->GetOneByte(ComponentSelector);
		MarkerBuffer->GetOneByte(AcDcTableSelector);

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
	MarkerBuffer->GetOneByte((ZEUInt8&)Info->SpectralSelectionStart);
	MarkerBuffer->GetOneByte((ZEUInt8&)Info->SpectralSelectionEnd);
	MarkerBuffer->GetOneByte(Byte);
	Info->ApproxBitPosHigh = (Byte >> 4) & 0x0F;
	Info->ApproxBitPosLow = Byte & 0x0F;

	// Increase seen sos count
	if (ScanCompCount)
		Info->SeenSosCount++;

	return true;
}

// Checks if marker is DHT and reads necessary data
bool ZEJpegFileMarkerReader::ProcessDHTMarker()
{
	ZEUInt16 DataLenght = 0;
	ZEUInt8 Index = 0;
	ZESize Count = 0;
	ZEUInt8 Bits[17] = {0};
	ZEUInt8 HuffmanValues[256] = {0};
	ZEJpegHuffmanTable* TargetTable = NULL;

	MarkerBuffer->GetTwoByte(DataLenght);
	DataLenght -= 2;

	while (DataLenght > 16)
	{
		MarkerBuffer->GetOneByte(Index);

		Bits[0] = 0;
		Count = 0;
		for (ZESize I = 1; I <= 16; ++I)
		{
			MarkerBuffer->GetOneByte(Bits[I]);
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
			MarkerBuffer->GetOneByte(HuffmanValues[I]);
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
bool ZEJpegFileMarkerReader::ProcessDQTMarker()
{

	ZEUInt8 TableIdentifier = 0;
	ZEUInt8 TablePrecision = 0;
	ZEUInt16 QuantValue = 0;
	ZEUInt16 Count = 0;
	ZEUInt16 DataLenght = 0;
	ZEJpegQuantizationTable* TargetTable = NULL;

	MarkerBuffer->GetTwoByte(DataLenght);
	DataLenght -= 2;

	// There should not be any optional data
	// Just read until the data length reaches 0
	while (DataLenght > 0)
	{
		MarkerBuffer->GetOneByte(TableIdentifier);
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
			Count = (DataLenght < ZE_JPEG_DCT_BLOCK_COEFF_COUNT * 2) ? DataLenght / 2 : ZE_JPEG_DCT_BLOCK_COEFF_COUNT;
		}
		else
		{
			Count = (DataLenght < ZE_JPEG_DCT_BLOCK_COEFF_COUNT) ? DataLenght : ZE_JPEG_DCT_BLOCK_COEFF_COUNT;
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
				MarkerBuffer->GetOneByte((ZEUInt8&)QuantValue);
			}

			Info->QuantizationTables[TableIdentifier].QuantizationValues[ZigZagToNaturalOrder[I]] = (ZEUInt16)QuantValue;
			//Info->QuantizationTables[TableIdentifier].QuantizationValues[I] = (ZEUInt16)QuantValue;
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
bool ZEJpegFileMarkerReader::ProcessDRIMarker()
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
bool ZEJpegFileMarkerReader::ProcessRSTMarker(ZEJpegFileMarker RstMarker)
{
	// If it is first RSTn marker seen then initialize
	// else if check if it is the right marker

	if (NextRestart == 0)
	{
		// Initialize the NextRestart
		NextRestart = 0xD0 + ((RstMarker + 1) & 0x07);
		return true;
	}
	else if (NextRestart == (ZEUInt8)RstMarker)
	{
		// Increment expected marker
		NextRestart = 0xD0 + ((RstMarker + 1) & 0x07);
		return true;
	}

	return false;
}

ZEJpegFileMarkerReader::ZEJpegFileMarkerReader()
{
	ImageFile		= NULL;
	MarkerBuffer	= NULL;
	SOFFound		= false;
	SOIFound		= false;
	EOIFound		= false;
	NextRestart		= 0;
	SeenSOSCount	= 0;
}

ZEJpegFileMarkerReader::~ZEJpegFileMarkerReader()
{
	this->Deinitialize();
}

// Reads all supported / known markers and saves the data into ZEJpegDeCompressionInfo
bool ZEJpegFileMarkerReader::ReadMarkers(ZEJpegFileMarker& StoppedAt)
{
	ZEJpegFileMarker FoundMarker = ZE_JPG_FM_UNKNOWN;

	while (true)
	{
		if (!MarkerBuffer->GetNextMarker(FoundMarker))
			false;

		switch (FoundMarker)
		{
			case ZE_JPG_FM_SOI:
			{
				if (!this->ProcessSOIMarker())
					return false;

				// Continue reading markers
				break;
			}

			case ZE_JPG_FM_SOF0:	// Baseline
			{
				if (!this->ProcessSOFMarker(true, false))
					return false;

				// Continue reading markers
				break;
			}

			case ZE_JPG_FM_SOF1:	// Extended sequential, Huffman
			{
				if (!this->ProcessSOFMarker(false, false))
					return false;

				// Continue reading markers
				break;
			}

			case ZE_JPG_FM_SOF2:	// Progressive, Huffman
			{
				if (!this->ProcessSOFMarker(false, true))
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
				if (!this->ProcessSOSMarker())
					return false;

				// Sos reached, sos info read from file
				// Let upper level take control and read compressed data from file, decode etc
				StoppedAt = ZE_JPG_FM_SOS;
				return true;

				break;
			}

			case ZE_JPG_FM_EOI:
			{
				if (!this->ProcessEOIMarker())
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
				if (!this->ProcessDHTMarker())
					return false;
				break;
			}

			case ZE_JPG_FM_DQT:
			{
				if (!this->ProcessDQTMarker())
					return false;
				break;
			}

			case ZE_JPG_FM_DRI:
			{
				if (!this->ProcessDRIMarker())
					return false;
				break;
			}

			case ZE_JPG_FM_APP0:	// JFIF Marker
			{
				if (!this->ProcessAPP0Marker())
					return false;
				break;
			}

			case ZE_JPG_FM_APP14:	// Adobe marker
			{
				if (!this->ProcessAPP14Marker())
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
				if (!this->ProcessRSTMarker(FoundMarker))
					return false;

				// Return control to upper level and let decompressor know a reset marker is found
				StoppedAt = ZE_JPEG_FM_RSTN;
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

ZEUInt ZEJpegFileMarkerReader::GetSeenSOSCount() const
{
	return SeenSOSCount;
}

ZEJpegFileMarkerBuffer* ZEJpegFileMarkerReader::GetMarkerBuffer() const
{
	return MarkerBuffer;
}

void ZEJpegFileMarkerReader::Initialize(ZEFile* ImageFile, ZEJpegDeCompressionInfo* Info)
{
	zeDebugCheck(MarkerBuffer != NULL || this->ImageFile != NULL, "MarkerReader is already initialized.");
	zeDebugCheck(ImageFile == NULL || Info == NULL, "Function call with null pointer");
	zeDebugCheck(!ImageFile->IsOpen(), "Jpeg file is not open.");
	
	this->Info = Info;
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

	Info			= NULL;
	ImageFile		= NULL;
	SOIFound		= false;
	EOIFound		= false;
	SOFFound		= false;
	NextRestart		= 0;
	SeenSOSCount	= 0;

}

void ZEJpegFileMarkerReader::Destroy()
{
	delete this;
}

ZEJpegFileMarkerReader* ZEJpegFileMarkerReader::CreateInstance()
{
	return new ZEJpegFileMarkerReader();
}
