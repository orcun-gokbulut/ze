//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureFileJPEGIDCTProcessor.cpp
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


#include "ZETextureFileJPEGIDCTProcessor.h"
#include "ZEError.h"
#include "ZETextureFileJPEGCommonDefinitions.h"


ZEJpegIDCTProcessor::ZEJpegIDCTProcessor()
{
	Info = NULL;
}

ZEJpegIDCTProcessor::~ZEJpegIDCTProcessor()
{
	this->Deinitialize();
}

void ZEJpegIDCTProcessor::ApplyIdct(ZEJpegComponentInfo* Component, ZEJpegDataBlock* Block)
{
	float	Z5, Z10, Z11, Z12, Z13;

	float	Temp0, Temp1, Temp2, Temp3, Temp4, 
			Temp5, Temp6, Temp7, Temp8, Temp9,
			Temp10, Temp11, Temp12, Temp13;

	float* DctTable = Component->DCTTable;
	ZEInt* SrcBlockData = Block->BlockData.OneDim;
	float TempBuffer[ZE_JPEG_DCT_BLOCK_COEFF_COUNT];
	float* TempBufferPtr = TempBuffer;
	memset(TempBuffer, 0, sizeof(float) * ZE_JPEG_DCT_BLOCK_COEFF_COUNT);
	ZEUInt8* RangeLimit = Info->SampleRangeLimit + ZE_JPEG_MAX_SAMPLE_VALUE + 1;

	// Process Columns
	for (ZESize ColumnN =  ZE_JPEG_DCT_BLOCK_DIMENSION; ColumnN > 0; --ColumnN)
	{
		
		// If coefficient is not zero do IDCT
		if (SrcBlockData[ZE_JPEG_DCT_BLOCK_DIMENSION * 1] == 0 && SrcBlockData[ZE_JPEG_DCT_BLOCK_DIMENSION * 2] == 0 && 
			SrcBlockData[ZE_JPEG_DCT_BLOCK_DIMENSION * 3] == 0 && SrcBlockData[ZE_JPEG_DCT_BLOCK_DIMENSION * 4] == 0 && 
			SrcBlockData[ZE_JPEG_DCT_BLOCK_DIMENSION * 5] == 0 && SrcBlockData[ZE_JPEG_DCT_BLOCK_DIMENSION * 6] == 0 && 
			SrcBlockData[ZE_JPEG_DCT_BLOCK_DIMENSION * 7] == 0 )
		{
			float DCValue = SrcBlockData[ZE_JPEG_DCT_BLOCK_DIMENSION * 0] * DctTable[ZE_JPEG_DCT_BLOCK_DIMENSION * 0];
			TempBufferPtr[ZE_JPEG_DCT_BLOCK_DIMENSION * 0] = DCValue;
			TempBufferPtr[ZE_JPEG_DCT_BLOCK_DIMENSION * 1] = DCValue;
			TempBufferPtr[ZE_JPEG_DCT_BLOCK_DIMENSION * 2] = DCValue;
			TempBufferPtr[ZE_JPEG_DCT_BLOCK_DIMENSION * 3] = DCValue;
			TempBufferPtr[ZE_JPEG_DCT_BLOCK_DIMENSION * 4] = DCValue;
			TempBufferPtr[ZE_JPEG_DCT_BLOCK_DIMENSION * 5] = DCValue;
			TempBufferPtr[ZE_JPEG_DCT_BLOCK_DIMENSION * 6] = DCValue;
			TempBufferPtr[ZE_JPEG_DCT_BLOCK_DIMENSION * 7] = DCValue;

			TempBufferPtr++;
			SrcBlockData++;
			DctTable++;
			continue;
		}
		
		// If not zero
		// Even part

		Temp0 = SrcBlockData[ZE_JPEG_DCT_BLOCK_DIMENSION * 0] * DctTable[ZE_JPEG_DCT_BLOCK_DIMENSION * 0];
		Temp1 = SrcBlockData[ZE_JPEG_DCT_BLOCK_DIMENSION * 2] * DctTable[ZE_JPEG_DCT_BLOCK_DIMENSION * 2];
		Temp2 = SrcBlockData[ZE_JPEG_DCT_BLOCK_DIMENSION * 4] * DctTable[ZE_JPEG_DCT_BLOCK_DIMENSION * 4];
		Temp3 = SrcBlockData[ZE_JPEG_DCT_BLOCK_DIMENSION * 6] * DctTable[ZE_JPEG_DCT_BLOCK_DIMENSION * 6];

		Temp10 = Temp0 + Temp2;
		Temp11 = Temp0 - Temp2;

		Temp13 = Temp1 + Temp3;
		Temp12 = (Temp1 - Temp3) * 1.414213562f - Temp13;

		Temp0 = Temp10 + Temp13;
		Temp3 = Temp10 - Temp13;
		Temp1 = Temp11 + Temp12;
		Temp2 = Temp11 - Temp12;

		// Odd Part
		Temp4 = SrcBlockData[ZE_JPEG_DCT_BLOCK_DIMENSION * 1] * DctTable[ZE_JPEG_DCT_BLOCK_DIMENSION * 1];
		Temp5 = SrcBlockData[ZE_JPEG_DCT_BLOCK_DIMENSION * 3] * DctTable[ZE_JPEG_DCT_BLOCK_DIMENSION * 3];
		Temp6 = SrcBlockData[ZE_JPEG_DCT_BLOCK_DIMENSION * 5] * DctTable[ZE_JPEG_DCT_BLOCK_DIMENSION * 5];
		Temp7 = SrcBlockData[ZE_JPEG_DCT_BLOCK_DIMENSION * 7] * DctTable[ZE_JPEG_DCT_BLOCK_DIMENSION * 7];

		Z13 = Temp6 + Temp5;
		Z10 = Temp6 - Temp5;
		Z11 = Temp4 + Temp7;
		Z12 = Temp4 - Temp7;

		Temp7	= Z11 + Z13;
		Temp11	= (Z11 - Z13) *  1.414213562f;

		Z5		= (Z10 + Z12) * 1.847759065f;
		Temp10	= Z5 - Z12 * 1.08239220f;
		Temp12	= Z5 - Z10 * 2.61312593f;

		Temp6 = Temp12 - Temp7;
		Temp5 = Temp11 - Temp6;
		Temp4 = Temp10 - Temp5;

		TempBufferPtr[ZE_JPEG_DCT_BLOCK_DIMENSION * 0] = Temp0 + Temp7;
		TempBufferPtr[ZE_JPEG_DCT_BLOCK_DIMENSION * 7] = Temp0 - Temp7;
		TempBufferPtr[ZE_JPEG_DCT_BLOCK_DIMENSION * 1] = Temp1 + Temp6;
		TempBufferPtr[ZE_JPEG_DCT_BLOCK_DIMENSION * 6] = Temp1 - Temp6;
		TempBufferPtr[ZE_JPEG_DCT_BLOCK_DIMENSION * 2] = Temp2 + Temp5;
		TempBufferPtr[ZE_JPEG_DCT_BLOCK_DIMENSION * 5] = Temp2 - Temp5;
		TempBufferPtr[ZE_JPEG_DCT_BLOCK_DIMENSION * 3] = Temp3 + Temp4;
		TempBufferPtr[ZE_JPEG_DCT_BLOCK_DIMENSION * 4] = Temp3 - Temp4;

		TempBufferPtr++;
		SrcBlockData++;
		DctTable++;
	}

	
	TempBufferPtr = TempBuffer;

	// Process rows
	for (ZESize RowN = 0; RowN < ZE_JPEG_DCT_BLOCK_DIMENSION; ++RowN)
	{
		// Set output row
		ZEInt* OutputPtr = Block->BlockData.OneDim + RowN * ZE_JPEG_DCT_BLOCK_DIMENSION;

		// Even part
		Z5 = TempBufferPtr[0] + 128.0f + 0.5f;
		Temp10 = Z5 + TempBufferPtr[4];
		Temp11 = Z5 - TempBufferPtr[4];

		Temp13 = TempBufferPtr[2] + TempBufferPtr[6];
		Temp12 = (TempBufferPtr[2] - TempBufferPtr[6]) *  1.414213562f - Temp13;

		Temp0 = Temp10 + Temp13;
		Temp3 = Temp10 - Temp13;
		Temp1 = Temp11 + Temp12;
		Temp2 = Temp11 - Temp12;

		// Odd part

		Z13 = TempBufferPtr[5] + TempBufferPtr[3];
		Z10 = TempBufferPtr[5] - TempBufferPtr[3];
		Z11 = TempBufferPtr[1] + TempBufferPtr[7];
		Z12 = TempBufferPtr[1] - TempBufferPtr[7];

		Temp7	= Z11 + Z13;
		Temp11	= (Z11 - Z13) * 1.414213562f;

		Z5		= (Z10 + Z12) * 1.847759065f;
		Temp10	= Z5 - Z12 * 1.082392200f;
		Temp12	= Z5 - Z10 * 2.613125930f;

		Temp6 = Temp12 - Temp7;
		Temp5 = Temp11 - Temp6;
		Temp4 = Temp10 - Temp5;
		
		OutputPtr[0] = RangeLimit[((ZESize)(Temp0 + Temp7)) & ZE_JPEG_RANGE_MASK];
		OutputPtr[7] = RangeLimit[((ZESize)(Temp0 - Temp7)) & ZE_JPEG_RANGE_MASK];
		OutputPtr[1] = RangeLimit[((ZESize)(Temp1 + Temp6)) & ZE_JPEG_RANGE_MASK];
		OutputPtr[6] = RangeLimit[((ZESize)(Temp1 - Temp6)) & ZE_JPEG_RANGE_MASK];
		OutputPtr[2] = RangeLimit[((ZESize)(Temp2 + Temp5)) & ZE_JPEG_RANGE_MASK];
		OutputPtr[5] = RangeLimit[((ZESize)(Temp2 - Temp5)) & ZE_JPEG_RANGE_MASK];
		OutputPtr[3] = RangeLimit[((ZESize)(Temp3 + Temp4)) & ZE_JPEG_RANGE_MASK];
		OutputPtr[4] = RangeLimit[((ZESize)(Temp3 - Temp4)) & ZE_JPEG_RANGE_MASK];

		TempBufferPtr += ZE_JPEG_DCT_BLOCK_DIMENSION;
	}

}

void ZEJpegIDCTProcessor::Initialize(ZEJpegDeCompressionInfo* Info)
{
	zeAssert(Info == NULL, "NULL Pointer");

	this->Info = Info;

	static const float AANScaleFactor[ZE_JPEG_DCT_BLOCK_DIMENSION] =
	{
		1.000000000f,
		1.387039845f,
		1.306562965f,
		1.175875602f,
		1.000000000f,
		0.785694958f,
		0.541196100f,
		0.275899379f
	};

	// Create precalculated IDCT tables per component
	for (ZESize ComponentN = 0; ComponentN < Info->ComponentCount; ++ComponentN)
	{
		ZEJpegComponentInfo* CurComp = &Info->ComponentInfo[ComponentN];

		float*	DCTTable = Info->ComponentInfo[ComponentN].DCTTable;

		ZESize Index = 0;
		for (ZESize RowN = 0; RowN < ZE_JPEG_DCT_BLOCK_DIMENSION; ++RowN)
		{
			for (ZESize ColumnN = 0; ColumnN < ZE_JPEG_DCT_BLOCK_DIMENSION; ++ColumnN)
			{
				DCTTable[Index] = AANScaleFactor[RowN] * AANScaleFactor[ColumnN] * 0.125f;
				Index++;
			}
		}
	}
}

void ZEJpegIDCTProcessor::Deinitialize()
{
	Info = NULL;
}

void ZEJpegIDCTProcessor::Destroy()
{
	delete this;
}

ZEJpegIDCTProcessor* ZEJpegIDCTProcessor::CreateInstance()
{
	return new ZEJpegIDCTProcessor();
}
