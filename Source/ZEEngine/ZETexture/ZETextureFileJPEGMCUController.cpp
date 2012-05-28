//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureFileJPEGMCUController.cpp
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
#include "ZETextureFileJPEGMCUController.h"
#include "ZETextureFileJPEGMarkerReader.h"
#include "ZETextureFileJPEGHuffmanDecoder.h"
#include "ZETextureFileJPEGDequantizator.h"
#include "ZETextureFileJPEGIDCTProcessor.h"
#include "ZETextureFileJPEGChromaUpsampler.h"
#include "ZETextureFileJPEGColorSpaceConverter.h"
#include <stdio.h>



bool ZEJpegMCUController::ProcessMCU(ZEUInt McuWidth, ZEUInt McuHeight, ZEUInt McuCoordX, ZEUInt McuCoordY)
{
	bool Result = false;
	
	ZEUInt CopyWidth = McuWidth; 
	ZEUInt CopyHeight = McuHeight;

	if (McuCoordX + McuWidth > Info->ImageWidth)
		CopyWidth = Info->ImageWidth - McuCoordX;

	if (McuCoordY + McuHeight > Info->ImageHeight)
		CopyHeight = Info->ImageHeight - McuCoordY;

	void* Destination = (ZEUInt8*)Info->OutputData + (Info->OutputPitch * (ZESize)McuCoordY + (ZESize)McuCoordX * (ZESize)Info->OutputPixelSize);

	// Decode MCU with huffman decoder
	Result = (HuffmanDecoder->*(HuffmanDecoder->DecodeFunction))(&McuBuffer);
	STOPPROCESS(Result); 

	// For each component
	ZEJpegDataBlock* TargetBlock = McuBuffer.McuData;
	ZEJpegDataBlock* ComponentStart = McuBuffer.McuData;	
	
	for (ZESize ComponentN = 0; ComponentN < (ZESize)Info->ComponentsInScan; ++ComponentN)
	{
		// Process the decoded MCU block by block
		ZEJpegComponentInfo* CurComp = Info->CurrentCompInfo[ComponentN];
		ZESize ComponentBlockCount = (ZESize)CurComp->McuBlocks;

		// For each block that component allocates
		for (ZESize BlockN = 0; BlockN < ComponentBlockCount; ++BlockN)
		{
			Dequantizator->Dequantize(CurComp, TargetBlock);
			IDCTProcessor->ApplyIdct(CurComp, TargetBlock);
			
			++TargetBlock;
		}

		// Up Sample the component
		(ChromaUpsampler->*(ChromaUpsampler->UpSamplePointers[ComponentN]))(Destination, CurComp, ComponentStart, CopyWidth, CopyHeight, McuCoordX, McuCoordY);
		ComponentStart += ComponentBlockCount;
	}

	++ProcessedMcuCount;
	return true;

}

ZEUInt ZEJpegMCUController::GetProcessedMcuCount()
{
	return ProcessedMcuCount;
}

void ZEJpegMCUController::Initialize(ZEJpegDeCompressionInfo* Info, ZEJpegFileMarkerReader* MarkerReader)
{
	zeAssert(Info == NULL, "NULL Pointer");

	this->Info = Info;

	// Initialize sub modules
	HuffmanDecoder = ZEJpegHuffmanDecoder::CreateInstance();
	HuffmanDecoder->Initialize(Info, MarkerReader);

	Dequantizator = ZEJpegDequantizator::CreateInstance();
	Dequantizator->Initialize(Info);

	IDCTProcessor = ZEJpegIDCTProcessor::CreateInstance();
	IDCTProcessor->Initialize(Info);

	ChromaUpsampler = ZEJpegChromaUpsampler::CreateInstance();
	ChromaUpsampler->Initialize(Info);

	ProcessedMcuCount = 0;
}

void ZEJpegMCUController::Deinitialize()
{
	Info = NULL;
	ProcessedMcuCount = 0;
	
	if (HuffmanDecoder != NULL)
	{
		HuffmanDecoder->Destroy();
		HuffmanDecoder = NULL;
	}
	
	if (Dequantizator != NULL)
	{
		Dequantizator->Destroy();
		Dequantizator = NULL;
	}

	if (IDCTProcessor != NULL)
	{
		IDCTProcessor->Destroy();
		IDCTProcessor	= NULL;
	}

	if (ChromaUpsampler != NULL)
	{
		ChromaUpsampler->Destroy();
		ChromaUpsampler	= NULL;
	}
}

void ZEJpegMCUController::Destroy()
{
	delete this;
}

ZEJpegMCUController* ZEJpegMCUController::CreateInstance()
{
	return new ZEJpegMCUController();
}

ZEJpegMCUController::ZEJpegMCUController()
{
	Info = NULL;
	ChromaUpsampler = NULL;
	HuffmanDecoder = NULL;
	Dequantizator = NULL;
	IDCTProcessor = NULL;

	ProcessedMcuCount = 0;
}

ZEJpegMCUController::~ZEJpegMCUController()
{
	this->Deinitialize();
}
