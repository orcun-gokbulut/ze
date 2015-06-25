//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureFileJPEGMainController.cpp
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
#include "ZETextureFileJPEGMarkerReader.h"
#include "ZETextureFileJPEGMCUController.h"
#include "ZETextureFileJPEGMainController.h"
#include "ZETextureFileJPEGCommonDefinitions.h"
#include "ZETextureFileJPEGColorSpaceConverter.h"


bool ZEJpegMainController::SetInitialParameters()
{
	// Check image size
	if (Info.ImageWidth > ZE_JPEG_MAX_IMAGE_WIDTH || Info.ImageWidth > ZE_JPEG_MAX_IMAGE_HEIGHT)
	{
		zeError("Image is larger than supported width / height");
		return false;
	}


	if (Info.ComponentCount > ZE_JPEG_MAX_COMPONENT_COUNT)
	{
		zeError("There are more components than the standard");
		return false;
	}

	// Internal sampling factors, find max h and v sampling factors by checking all components
	Info.MaxHorzSampleFactor = ZE_JPEG_MIN_SAMPLING_FACTOR;
	Info.MaxVertSampleFactor = ZE_JPEG_MIN_SAMPLING_FACTOR;
	Info.MinHorzSampleFactor = ZE_JPEG_MAX_SAMPLING_FACTOR;
	Info.MinVertSampleFactor = ZE_JPEG_MAX_SAMPLING_FACTOR;

	for (ZESize I = 0; I < Info.ComponentCount; ++I)
	{
		if (Info.ComponentInfo[I].HorizontalFreq > ZE_JPEG_MAX_SAMPLE_FREQUENCY || Info.ComponentInfo[I].VerticalFreq > ZE_JPEG_MAX_SAMPLE_FREQUENCY)
		{
			zeError("Sampling frequency of one of the components exceeds max possible frequency.");
			return false;
		}

		Info.MaxHorzSampleFactor = ZEMath::Max(Info.MaxHorzSampleFactor, Info.ComponentInfo[I].HorizontalFreq);
		Info.MaxVertSampleFactor = ZEMath::Max(Info.MaxVertSampleFactor, Info.ComponentInfo[I].VerticalFreq);

		Info.MinHorzSampleFactor = ZEMath::Min(Info.MinHorzSampleFactor, Info.ComponentInfo[I].HorizontalFreq);
		Info.MinVertSampleFactor = ZEMath::Min(Info.MinVertSampleFactor, Info.ComponentInfo[I].VerticalFreq);
	}


	// Decide block size, natural order and Spectral selection end limit

	// No pseudo SOS marker
	if (Info.IsBaseLine || (Info.IsProgressive && Info.ComponentsInScan)) 
	{
		Info.DctBlockSize = (ZEInt)ZE_JPEG_DCT_BLOCK_DIMENSION;
		Info.SpectralSelectionEndLimit = (ZEInt)ZE_JPEG_DCT_BLOCK_COEFF_COUNT - 1;
	}
	else
	{
		switch (Info.SpectralSelectionEnd)
		{
			case 0: // 1 * 1 - 1
				Info.DctBlockSize = 1;
				Info.NaturalOrderPosArray = ZeJpegZigZagToNaturalOrder8x8;
				Info.SpectralSelectionEndLimit = Info.SpectralSelectionEnd;
				break;
			case 3: // 2 * 2 - 1
				Info.DctBlockSize = 2;
				Info.NaturalOrderPosArray = ZeJpegZigZagToNaturalOrder2x2;
				Info.SpectralSelectionEndLimit = Info.SpectralSelectionEnd;
				break;
			case 8: // 3 * 3 - 1
				Info.DctBlockSize = 3;
				Info.NaturalOrderPosArray = ZeJpegZigZagToNaturalOrder3x3;
				Info.SpectralSelectionEndLimit = Info.SpectralSelectionEnd;
				break;
			case 15: // 4 * 4 - 1
				Info.DctBlockSize = 4;
				Info.NaturalOrderPosArray = ZeJpegZigZagToNaturalOrder4x4;
				Info.SpectralSelectionEndLimit = Info.SpectralSelectionEnd;
				break;
			case 24: // 5 * 5 - 1
				Info.DctBlockSize = 5;
				Info.NaturalOrderPosArray = ZeJpegZigZagToNaturalOrder5x5;
				Info.SpectralSelectionEndLimit = Info.SpectralSelectionEnd;
				break;
			case 35: // 6 * 6 - 1
				Info.DctBlockSize = 6;
				Info.NaturalOrderPosArray = ZeJpegZigZagToNaturalOrder6x6;
				Info.SpectralSelectionEndLimit = Info.SpectralSelectionEnd;
				break;
			case 48: // 7 * 7 - 1
				Info.DctBlockSize = 7;
				Info.NaturalOrderPosArray = ZeJpegZigZagToNaturalOrder7x7;
				Info.SpectralSelectionEndLimit = Info.SpectralSelectionEnd;
				break;
			case 63: // 8 * 8 - 1
				Info.DctBlockSize = 8;
				Info.NaturalOrderPosArray = ZeJpegZigZagToNaturalOrder8x8;
				Info.SpectralSelectionEndLimit = Info.SpectralSelectionEnd;
				break;
			case 80: // 9 * 9 - 1
				Info.DctBlockSize = 9;
				Info.NaturalOrderPosArray = ZeJpegZigZagToNaturalOrder8x8;
				Info.SpectralSelectionEndLimit = ZE_JPEG_DCT_BLOCK_COEFF_COUNT - 1;
				break;
			case 99: // 10 * 10 - 1
				Info.DctBlockSize = 10;
				Info.NaturalOrderPosArray = ZeJpegZigZagToNaturalOrder8x8;
				Info.SpectralSelectionEndLimit = ZE_JPEG_DCT_BLOCK_COEFF_COUNT - 1;
				break;
			case 120: // 11 * 11 - 1
				Info.DctBlockSize = 11;
				Info.NaturalOrderPosArray = ZeJpegZigZagToNaturalOrder8x8;
				Info.SpectralSelectionEndLimit = ZE_JPEG_DCT_BLOCK_COEFF_COUNT - 1;
				break;
			case 143: // 12 * 12 - 1
				Info.DctBlockSize = 12;
				Info.NaturalOrderPosArray = ZeJpegZigZagToNaturalOrder8x8;
				Info.SpectralSelectionEndLimit = ZE_JPEG_DCT_BLOCK_COEFF_COUNT - 1;
				break;
			case 168: // 13 * 13 - 1
				Info.DctBlockSize = 13;
				Info.NaturalOrderPosArray = ZeJpegZigZagToNaturalOrder8x8;
				Info.SpectralSelectionEndLimit = ZE_JPEG_DCT_BLOCK_COEFF_COUNT - 1;
				break;
			case 195: // 14 * 14 - 1
				Info.DctBlockSize = 14;
				Info.NaturalOrderPosArray = ZeJpegZigZagToNaturalOrder8x8;
				Info.SpectralSelectionEndLimit = ZE_JPEG_DCT_BLOCK_COEFF_COUNT - 1;
				break;
			case 224: // 15 * 15 - 1
				Info.DctBlockSize = 15;
				Info.NaturalOrderPosArray = ZeJpegZigZagToNaturalOrder8x8;
				Info.SpectralSelectionEndLimit = ZE_JPEG_DCT_BLOCK_COEFF_COUNT - 1;
				break;
			case 255: // 16 * 16 - 1
				Info.DctBlockSize = 16;
				Info.NaturalOrderPosArray = ZeJpegZigZagToNaturalOrder8x8;
				Info.SpectralSelectionEndLimit = ZE_JPEG_DCT_BLOCK_COEFF_COUNT - 1;
				break;
			default:
				zeError("Cannot decide dct block size");
				return false;
				break;
		}
	}

	Info.MCUWidthInPixels = Info.MaxHorzSampleFactor / Info.MinHorzSampleFactor * Info.DctBlockSize;
	Info.MCUHeightInPixels = Info.MaxVertSampleFactor / Info.MinVertSampleFactor * Info.DctBlockSize;

	// Compute component dimensions
	for (ZESize I = 0; I < Info.ComponentCount; ++I)
	{
		Info.ComponentInfo[I].ComponentValueNeeded = true;

		Info.ComponentInfo[I].DctHorzScaledSize = Info.DctBlockSize;
		Info.ComponentInfo[I].DctVertScaledSize = Info.DctBlockSize;

		Info.ComponentInfo[I].MCUHeightInBlocks = (ZEUInt)ZEMath::Ceil((float)(Info.ImageHeight * Info.ComponentInfo[I].VerticalFreq) / (float)(Info.MaxVertSampleFactor * Info.DctBlockSize));
		Info.ComponentInfo[I].MCUWidthInBlocks  = (ZEUInt)ZEMath::Ceil((float)(Info.ImageWidth * Info.ComponentInfo[I].HorizontalFreq) / (float)(Info.MaxHorzSampleFactor * Info.DctBlockSize));

		// Set the quantization table pointer for the component
		Info.ComponentInfo[I].QuantizationTable = &Info.QuantizationTables[Info.ComponentInfo[I].QuantizationTableNo];
	}

	/* Decide whether file contains multiple scans */
	if ((Info.ComponentsInScan < Info.ComponentCount) || Info.IsProgressive)
		Info.HasMultipleScan = true;
	else
		Info.HasMultipleScan = false;

	// Not Supported yet
	if (Info.HasMultipleScan)
	{
		zeError("Multiple scan jpeg files are not supported.");
		return false;
	}

	return true;
}

bool ZEJpegMainController::CreateRangeLimitTable()
{
	memset(Info.SampleRangeLimit, 0, 5 * (ZE_JPEG_MAX_SAMPLE_VALUE + 1) + ZE_JPEG_MID_SAMPLE_VALUE);
	
	ZEUInt8* Table = Info.SampleRangeLimit + ZE_JPEG_MAX_SAMPLE_VALUE + 1;

	for (ZESize I = 0; I <= ZE_JPEG_MAX_SAMPLE_VALUE; ++I)
	{
		Table[I] = (ZEUInt8)I;
	}

	Table += ZE_JPEG_MID_SAMPLE_VALUE;

	for (ZESize I = ZE_JPEG_MID_SAMPLE_VALUE; I < 2 * (ZE_JPEG_MAX_SAMPLE_VALUE + 1); ++I)
	{
		Table[I] = ZE_JPEG_MAX_SAMPLE_VALUE;
	}

	memcpy(Table + (4 * (ZE_JPEG_MAX_SAMPLE_VALUE + 1) - ZE_JPEG_MID_SAMPLE_VALUE), Info.SampleRangeLimit + ZE_JPEG_MAX_SAMPLE_VALUE + 1, ZE_JPEG_MID_SAMPLE_VALUE * sizeof(ZEUInt8));
	return true;
}

// Default decompression parameters
bool ZEJpegMainController::SetDefaultParameters()
{
	// Set color space
	switch (Info.ComponentCount)
	{
		case 1:
			// If input is greyscale output is also greyscale
			Info.InputColorSpace = ZE_JPEG_CS_GRAYSCALE;
			Info.OutputColorSpace = ZE_JPEG_CS_GRAYSCALE;
			Info.OutputPixelSize = 1;
			break;

		case 2:
			zeError("Two component color spaces are not supported");
			break;

		case 3:
			Info.OutputPixelSize = 4;
			if (Info.JFIFMarkerFound)
			{
				Info.InputColorSpace = ZE_JPEG_CS_YCbCr;
				Info.OutputColorSpace = ZE_JPEG_CS_RGB;
			}
			else if (Info.AdobeMarkerFound)
			{
				switch (Info.AdobeTransform)
				{
					case 0:
						Info.InputColorSpace = ZE_JPEG_CS_RGB;
						Info.OutputColorSpace = ZE_JPEG_CS_RGB;
						break;
					case 1:
						Info.InputColorSpace = ZE_JPEG_CS_YCbCr;
						Info.OutputColorSpace = ZE_JPEG_CS_RGB;
						break;
					default:
						zeWarning("Cannot decide input color space. Assuming YCbCr.");
						Info.InputColorSpace = ZE_JPEG_CS_YCbCr;
						Info.OutputColorSpace = ZE_JPEG_CS_RGB;
						break;
				}
			}
			else // If there are no special markers try to guess
			{
				ZEUInt8 Comp0Id = Info.ComponentInfo[0].ComponentId;
				ZEUInt8 Comp1Id = Info.ComponentInfo[0].ComponentId;
				ZEUInt8 Comp2Id = Info.ComponentInfo[0].ComponentId;

				if (Comp0Id == 1 && Comp1Id == 2 && Comp2Id == 3)
				{
					// if components are ordered assume it is YCbCr
					Info.InputColorSpace = ZE_JPEG_CS_YCbCr;
					Info.OutputColorSpace = ZE_JPEG_CS_RGB;
				}
				else if (Comp0Id == 82 && Comp1Id == 71 && Comp2Id == 66)
				{
					// AscII 'R' 'G' 'B' case
					Info.InputColorSpace = ZE_JPEG_CS_RGB;
					Info.OutputColorSpace = ZE_JPEG_CS_RGB;
				}
				else
				{
					zeWarning("Cannot decide input color space. Assuming YCbCr.");
					Info.InputColorSpace = ZE_JPEG_CS_YCbCr;
					Info.OutputColorSpace = ZE_JPEG_CS_RGB;
				}
			}
			break;

		case 4:
			zeError("Four component color spaces are not supported");
			break;

		default:
			zeError("Cannot decide input/output color space");
			return false;
			break;
	}

	return true;

}

bool ZEJpegMainController::SetScanParameters()
{

	// Noninterleaved (single-component) scan
	if (Info.ComponentsInScan == 1)
	{
		// Only component 0 is used
		Info.MCUCountPerRow = Info.OrederedCompInfo[0]->MCUWidthInBlocks;
		Info.MCURowCountInImage = Info.OrederedCompInfo[0]->MCUHeightInBlocks;

		// For Noninterleaved scan there is always one block per MCU
		Info.OrederedCompInfo[0]->McuWidth = 1;
		Info.OrederedCompInfo[0]->McuHeight = 1;
		Info.OrederedCompInfo[0]->McuBlocks = 1;
		Info.OrederedCompInfo[0]->McuSampleWidth = Info.OrederedCompInfo[0]->DctHorzScaledSize;
		Info.OrederedCompInfo[0]->LastColumtWidth = 1;

		ZEUInt Temp = Info.OrederedCompInfo[0]->MCUHeightInBlocks % Info.OrederedCompInfo[0]->VerticalFreq;
		Info.OrederedCompInfo[0]->LastRowHeight = (Temp == 0) ? Info.OrederedCompInfo[0]->VerticalFreq : Temp;

		// Prepare array describing MCU composition
		Info.DctBlocksPerMCU = 1;
		Info.MCUMembership[0] = 0;

	}
	else
	{
		// Interleaved (multi-component) scan

		if (Info.ComponentsInScan > ZE_JPEG_MAX_COMPONENTS_IN_SCAN)
		{
			zeError("Component count in current scan exceeds the limits.");
			return false;
		}

		Info.MCUCountPerRow = (ZEUInt)ZEMath::Ceil((float)Info.ImageWidth / (float)(Info.MaxHorzSampleFactor * Info.DctBlockSize));
		Info.MCURowCountInImage = (ZEUInt)ZEMath::Ceil((float)Info.ImageHeight / (float)(Info.MaxVertSampleFactor * Info.DctBlockSize));

		Info.DctBlocksPerMCU = 0;
		for (ZESize I = 0; I < Info.ComponentsInScan; ++I)
		{
			ZEJpegComponentInfo* Component = Info.OrederedCompInfo[I];

			// Sampling factors give # of blocks of component in each MCU
			Component->McuWidth = Component->HorizontalFreq;
			Component->McuHeight = Component->VerticalFreq;
			Component->McuBlocks = Component->McuWidth * Component->McuHeight;
			Component->McuSampleWidth = Component->McuWidth * Component->DctHorzScaledSize;

			// Figure number of non-dummy blocks in last MCU column & row
			ZEUInt Temp = Component->MCUWidthInBlocks % Component->McuWidth;
			Component->LastColumtWidth = (Temp == 0) ? Component->McuWidth : Temp;

			Temp = Component->MCUHeightInBlocks % Component->McuHeight;
			Component->LastRowHeight = (Temp == 0) ? Component->McuHeight : Temp;

			// Prepare array describing MCU composition
			ZEUInt MCUBlocks = Component->McuBlocks;

			while (MCUBlocks-- > 0)
			{
				Info.MCUMembership[(ZESize)Info.DctBlocksPerMCU++] = (ZEInt)I;
			}
		}
	}

	return true;
}

bool ZEJpegMainController::ProcessScan(ZETextureData* TextureData)
{
	bool Result = false;
	ZESize I, J;
	ZEUInt OutputCoordX, OutputCoordY;

	Result = this->SetScanParameters();
	STOPPROCESS(Result);
	
	// For each MCU Row
	for (J = 0, OutputCoordY = 0; J < (ZESize)Info.MCURowCountInImage; ++J, OutputCoordY += Info.MCUHeightInPixels)
	{
		// For each MCU Column
		for (I = 0, OutputCoordX = 0; I < (ZESize)Info.MCUCountPerRow; ++I, OutputCoordX += Info.MCUWidthInPixels)
		{
			// Process MCU and give destination offsets in terms of MCU count
			Result = McuController->ProcessMCU(OutputCoordX, OutputCoordY);
			STOPPROCESS(Result);
		}
	}

	return true;
}

bool ZEJpegMainController::LoadHeader(ZETextureDataInfo* TextureDataInfo)
{
	bool NoError = false;

	ZEJpegFileMarker StoppedAt = ZE_JPG_FM_UNKNOWN;
	NoError = MarkerReader->ReadMarkers(StoppedAt);
	
	if (!NoError || (StoppedAt != ZE_JPG_FM_SOS))
	{
		MarkerReader->Destroy();
		MarkerReader = NULL;	
		return false;
	}
	else
	{
		TextureDataInfo->Type = ZEGR_TT_2D;
		TextureDataInfo->LevelCount = 1;
		TextureDataInfo->SurfaceCount = 1;
		TextureDataInfo->Width = Info.ImageWidth;
		TextureDataInfo->Height = Info.ImageHeight;
		
		switch (Info.ComponentCount)
		{
			case 1:
				TextureDataInfo->PixelFormat = ZEGR_TF_I8;
				break;
			
			case 3:
			case 4:
			default:
				TextureDataInfo->PixelFormat = ZEGR_TF_I8_4;
				break;
		}

		return true;
	}
}

ZETextureData* ZEJpegMainController::LoadJpeg()
{
	bool Done = false;
	bool Result = false;
	bool NoError = false;
	ZETextureData* TextureData = NULL;
	ZEJpegFileMarker StoppedAt = ZE_JPG_FM_UNKNOWN;

	// Read the markers and fetch the appropriate data from markers
	NoError = MarkerReader->ReadMarkers(StoppedAt);
	if(!NoError)
	{
		zeError("Cannot read image info.");
		return NULL;
	}

	if (!SetDefaultParameters())	return NULL;
	if (!SetInitialParameters())	return NULL;
	if (!SetScanParameters())		return NULL;
	if (!CreateRangeLimitTable())	return NULL;

	McuController = ZEJpegMCUController::CreateInstance();
	McuController->Initialize(&Info, MarkerReader);

	ColorConverter = ZEJpegColorSpaceConverter::CreateInstance();
	ColorConverter->Initialize(&Info);
	
	// Image info should be read till now
	TextureData = new ZETextureData();
	ZEGRFormat PixelFormat = Info.ComponentCount == 1 ? ZEGR_TF_I8 : ZEGR_TF_I8_4;
	TextureData->Create(ZEGR_TT_2D, PixelFormat, 1, 1, Info.ImageWidth, Info.ImageHeight);
	Info.OutputData = TextureData->GetSurfaces().GetItem(0).GetLevels().GetItem(0).GetData();
	Info.OutputPitch = TextureData->GetSurfaces().GetItem(0).GetLevels().GetItem(0).GetPitch();

	while (NoError && !Done)
	{
		// Process the last read marker, must be SOS in the first iteration of loop.
		switch (StoppedAt)
		{
			case ZE_JPG_FM_SOS:	// If start of scan is found
				NoError = ProcessScan(TextureData);
				if (!NoError)
				{
					zeError("Cannot process image data.");
					return NULL;
				}
				break;

			case ZE_JPG_FM_EOI:	// If end of image
				Done = true;
				break;

			default:
			case ZE_JPG_FM_UNKNOWN:	// Unknown marker
				zeError("Unknown marker found in image file");
				break;
		}

		// Read the markers and fetch the appropriate data from markers
		if (!Done)
		{
			NoError  = MarkerReader->ReadMarkers(StoppedAt);
			if (!NoError)
			{
				zeError("Cannot process image data.");
				return NULL;
			}
		}
	}

	// Color convert
	(ColorConverter->*(ColorConverter->Convert))(Info.OutputData, Info.OutputData);

	return TextureData;
}



ZEJpegMainController::ZEJpegMainController(ZEFile* ImageFile)
{
	McuController = NULL;
	MarkerReader = NULL;
	ColorConverter = NULL;

	ZEJpegDeCompressionInfo::ZeroData(&Info);

	MarkerReader = ZEJpegFileMarkerReader::CreateInstance();
	MarkerReader->Initialize(ImageFile, &Info);
	
}

ZEJpegMainController::~ZEJpegMainController()
{
	if (ColorConverter != NULL)
	{
		ColorConverter->Destroy();
		ColorConverter = NULL;
	}

	if (McuController != NULL)
	{
		McuController->Destroy();
		McuController = NULL;
	}

	if (MarkerReader != NULL)
	{
		MarkerReader->Destroy();
		MarkerReader = NULL;
	}
	
}
