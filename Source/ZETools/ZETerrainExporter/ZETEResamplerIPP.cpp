//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETEResamplerIPP.cpp
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

#include "ZETEResamplerIPP.h"

#include "ZETEBlock.h"
#include "ZETEPatch.h"
#include "ZEError.h"

#include <ippi.h>
#include <ippcore.h>
#include <ipps.h>

void ZETEResamplerIPP::Resample(ZETEPatch* Patch, ZETEBlock* Block)
{
	double SourcePositionX = (Block->GetPositionX() - Patch->GetPositionX()) / Patch->GetPixelScaleX();
	double SourcePositionY = (Block->GetPositionY() - Patch->GetPositionY()) / Patch->GetPixelScaleY();

	double SourceEndPositionX = (Block->GetEndX() - Patch->GetPositionX()) / Patch->GetPixelScaleX();
	double SourceEndPositionY = (Block->GetEndY() - Patch->GetPositionY()) / Patch->GetPixelScaleY();

	// Clamp Front
	double ClampedSourcePositionX = floor(SourcePositionX);
	if (ClampedSourcePositionX < 0)
		ClampedSourcePositionX = 0;
	double ClampedSourcePositionY = floor(SourcePositionY);
	if (ClampedSourcePositionY < 0)
		ClampedSourcePositionY = 0;

	// Clamp End
	double ClampedSourceEndPositionX = ceil(SourceEndPositionX);
	if (ClampedSourceEndPositionX >= Patch->GetWidth())
		ClampedSourceEndPositionX = Patch->GetWidth() - 1;
	double ClampedSourceEndPositionY = ceil(SourceEndPositionY);
	if (ClampedSourceEndPositionY >= Patch->GetHeight())
		ClampedSourceEndPositionY = Patch->GetHeight() - 1;

	IppiSize SourceSize;
	SourceSize.width = ClampedSourceEndPositionX - ClampedSourcePositionX;
	SourceSize.height = ClampedSourceEndPositionY - ClampedSourcePositionY;

	IppiRect SourceROI;
	SourceROI.x = 0;
	SourceROI.y = 0;
	SourceROI.width = SourceSize.width;
	SourceROI.height = SourceSize.height;

	IppiRect DestROI;
	DestROI.x = 0;
	DestROI.y = 0;
	DestROI.width = Block->GetSize();
	DestROI.height = Block->GetSize();
	
	int CurrentBufferSize = 0;
	ippiResizeGetBufSize(SourceROI, DestROI, 4, IPPI_INTER_LINEAR, &CurrentBufferSize);
	if (CurrentBufferSize > ResampleData.BufferSize)
	{
		if (ResampleData.Buffer != NULL)
			ippsFree(ResampleData.Buffer);

		ResampleData.Buffer = ippsMalloc_8u(CurrentBufferSize);	
		ResampleData.BufferSize = CurrentBufferSize;
	}

	ZESize CurrentPatchBufferSize = SourceSize.width * SourceSize.height * Patch->GetPixelSize();
	if (CurrentPatchBufferSize > ResampleData.PatchBufferSize)
	{
		if (ResampleData.PatchBuffer != NULL)
			delete ResampleData.PatchBuffer;

		ResampleData.PatchBufferSize = CurrentPatchBufferSize;
		ResampleData.PatchBuffer = new ZEBYTE[ResampleData.PatchBufferSize];
	}

	Patch->GetData(ResampleData.PatchBuffer, (ZEUInt64)ClampedSourcePositionX, (ZEUInt64)ClampedSourcePositionY, SourceSize.width, SourceSize.height);

	if (Patch->GetPixelType() == ZETE_PT_ELEVATION)
	{
		IppStatus Result = ippiResizeSqrPixel_16u_C1R(
			(const Ipp16u*)ResampleData.PatchBuffer, SourceSize, 
			SourceSize.width * Patch->GetPixelSize(), SourceROI, 
			(Ipp16u*)Block->GetData(), Block->GetPitch(), DestROI, 
			Patch->GetLevelScaleX(), Patch->GetLevelScaleY(), 
			(ClampedSourcePositionX - SourcePositionX) * Patch->GetLevelScaleX(), 
			(ClampedSourcePositionY - SourcePositionY) * Patch->GetLevelScaleY(), 
			IPPI_INTER_LINEAR, (Ipp8u*)ResampleData.Buffer);
	}
	else if (Patch->GetPixelType() == ZETE_PT_COLOR)
	{
		IppStatus Result = ippiResizeSqrPixel_8u_C4R(
			(const Ipp8u*)ResampleData.PatchBuffer, SourceSize, 
			SourceSize.width * Patch->GetPixelSize(), SourceROI, 
			(Ipp8u*)Block->GetData(), Block->GetPitch(), DestROI, 
			Patch->GetLevelScaleX(), Patch->GetLevelScaleY(), 
			(ClampedSourcePositionX - SourcePositionX) * Patch->GetLevelScaleX(), 
			(ClampedSourcePositionY - SourcePositionY) * Patch->GetLevelScaleY(), 
			IPPI_INTER_LINEAR, (Ipp8u*)ResampleData.Buffer);
	}
	else if (Patch->GetPixelType() == ZETE_PT_GRAYSCALE)
	{
		IppStatus Result = ippiResizeSqrPixel_8u_C1R(
			(const Ipp8u*)ResampleData.PatchBuffer, SourceSize, 
			SourceSize.width * Patch->GetPixelSize(), SourceROI, 
			(Ipp8u*)Block->GetData(), Block->GetPitch(), DestROI, 
			Patch->GetLevelScaleX(), Patch->GetLevelScaleY(), 
			(ClampedSourcePositionX - SourcePositionX) * Patch->GetLevelScaleX(), 
			(ClampedSourcePositionY - SourcePositionY) * Patch->GetLevelScaleY(), 
			IPPI_INTER_LINEAR, (Ipp8u*)ResampleData.Buffer);
	}
}

void ZETEResamplerIPP::DownsampleElevation(ZETEBlock* Output, ZETEBlock* Block00, ZETEBlock* Block01, ZETEBlock* Block10, ZETEBlock* Block11)
{
	ZESize HalfBlockSize = Block00->GetSize() / 2;
	IppiSize SourceSize = {Block00->GetSize(), Block00->GetSize()};
	IppiSize DestinationSize = {HalfBlockSize, HalfBlockSize};

	IppStatus Result;

	int CurrentSpecSize = 0;
	int CurrentInitBufferSize = 0;
	Result = ippiResizeGetSize_16u(SourceSize, DestinationSize, ippLinear, 0, &CurrentSpecSize, &CurrentInitBufferSize);
	if (Result != ippStsNoErr) 
		return;

	if (CurrentInitBufferSize != DownsampleData.InitBufferSize)
	{
		if (DownsampleData.InitBuffer != NULL)
		{
			ippsFree(DownsampleData.InitBuffer);
			DownsampleData.InitBuffer = NULL;
		}

		DownsampleData.InitBuffer = ippsMalloc_8u(CurrentInitBufferSize);
		DownsampleData.InitBufferSize = CurrentInitBufferSize;
	}

	if (CurrentSpecSize > DownsampleData.SpecSize)
	{
		if (DownsampleData.Spec != NULL)
		{
			ippsFree(DownsampleData.Spec);
			DownsampleData.Spec = NULL;
		}

		DownsampleData.Spec = ippsMalloc_8u(CurrentSpecSize);
		DownsampleData.SpecSize = CurrentSpecSize;

		ippiResizeLinearInit_16u(SourceSize, DestinationSize, (IppiResizeSpec_32f*)DownsampleData.Spec);
	}

	int CurrentBufferSize = 0;
	Result = ippiResizeGetBufferSize_16u((IppiResizeSpec_32f*)DownsampleData.Spec, DestinationSize, 1, &CurrentBufferSize);
	if (Result != ippStsNoErr) 
		return;

	if (CurrentBufferSize > DownsampleData.BufferSize)
	{
		if (DownsampleData.Buffer != NULL)
		{
			ippsFree(DownsampleData.Buffer);
			DownsampleData.Buffer = NULL;
		}

		DownsampleData.Buffer = ippsMalloc_8u(CurrentBufferSize);
		DownsampleData.BufferSize = CurrentBufferSize;
	}

	IppiPoint DestinationOffset;
	DestinationOffset.x = 0;
	DestinationOffset.y = 0;

	ZEBYTE* Destination = (ZEBYTE*)Output->GetData();
	Result = ippiResizeLinear_16u_C1R(
		(Ipp16u*)Block00->GetData(), Block00->GetPitch(), 
		(Ipp16u*)Destination, Output->GetPitch(), 
		DestinationOffset, DestinationSize, ippBorderRepl, 0, 
		(IppiResizeSpec_32f*)DownsampleData.Spec, (Ipp8u*)DownsampleData.Buffer);


	Destination = (ZEBYTE*)Output->GetData() + HalfBlockSize * Output->GetPixelSize();
	Result = ippiResizeLinear_16u_C1R(
		(Ipp16u*)Block01->GetData(), Block01->GetPitch(), 
		(Ipp16u*)Destination, Output->GetPitch(), 
		DestinationOffset, DestinationSize, ippBorderRepl, 0, 
		(IppiResizeSpec_32f*)DownsampleData.Spec, (Ipp8u*)DownsampleData.Buffer);


	Destination = (ZEBYTE*)Output->GetData() + HalfBlockSize * Output->GetPitch();
	Result = ippiResizeLinear_16u_C1R(
		(Ipp16u*)Block10->GetData(), Block10->GetPitch(), 
		(Ipp16u*)Destination, Output->GetPitch(), 
		DestinationOffset, DestinationSize, ippBorderRepl, 0, 
		(IppiResizeSpec_32f*)DownsampleData.Spec, (Ipp8u*)DownsampleData.Buffer);

	Destination = (ZEBYTE*)Output->GetData()  + HalfBlockSize * Output->GetPitch() + HalfBlockSize * Output->GetPixelSize();
	Result = ippiResizeLinear_16u_C1R(
		(Ipp16u*)Block11->GetData(), Block11->GetPitch(), 
		(Ipp16u*)Destination, Output->GetPitch(), 
		DestinationOffset, DestinationSize, ippBorderRepl, 0, 
		(IppiResizeSpec_32f*)DownsampleData.Spec, (Ipp8u*)DownsampleData.Buffer);
}

void ZETEResamplerIPP::DownsampleColor(ZETEBlock* Output, ZETEBlock* Block00, ZETEBlock* Block01, ZETEBlock* Block10, ZETEBlock* Block11)
{
	ZESize HalfBlockSize = Block00->GetSize() / 2;
	IppiSize SourceSize = {Block00->GetSize(), Block00->GetSize()};
	IppiSize DestinationSize = {HalfBlockSize, HalfBlockSize};

	IppStatus Result;

	int CurrentSpecSize = 0;
	int CurrentInitBufferSize = 0;
	Result = ippiResizeGetSize_8u(SourceSize, DestinationSize, ippLinear, 0, &CurrentSpecSize, &CurrentInitBufferSize);
	if (Result != ippStsNoErr) 
		return;

	if (CurrentInitBufferSize != DownsampleData.InitBufferSize)
	{
		if (DownsampleData.InitBuffer != NULL)
		{
			ippsFree(DownsampleData.InitBuffer);
			DownsampleData.InitBuffer = NULL;
		}

		DownsampleData.InitBuffer = ippsMalloc_8u(CurrentInitBufferSize);
		DownsampleData.InitBufferSize = CurrentInitBufferSize;
	}

	if (CurrentSpecSize > DownsampleData.SpecSize)
	{
		if (DownsampleData.Spec != NULL)
		{
			ippsFree(DownsampleData.Spec);
			DownsampleData.Spec = NULL;
		}

		DownsampleData.Spec = ippsMalloc_8u(CurrentSpecSize);
		DownsampleData.SpecSize = CurrentSpecSize;

		ippiResizeLinearInit_8u(SourceSize, DestinationSize, (IppiResizeSpec_32f*)DownsampleData.Spec);
	}

	int CurrentBufferSize = 0;
	Result = ippiResizeGetBufferSize_8u((IppiResizeSpec_32f*)DownsampleData.Spec, DestinationSize, 4, &CurrentBufferSize);
	if (Result != ippStsNoErr) 
		return;

	if (CurrentBufferSize > DownsampleData.BufferSize)
	{
		if (DownsampleData.Buffer != NULL)
		{
			ippsFree(DownsampleData.Buffer);
			DownsampleData.Buffer = NULL;
		}

		DownsampleData.Buffer = ippsMalloc_8u(CurrentBufferSize);
		DownsampleData.BufferSize = CurrentBufferSize;
	}

	IppiPoint DestinationOffset;
	DestinationOffset.x = 0;
	DestinationOffset.y = 0;

	Ipp8u* Destination = (Ipp8u*)Output->GetData();
	Result = ippiResizeLinear_8u_C4R(
		(Ipp8u*)Block00->GetData(), Block00->GetPitch(), 
		Destination, Output->GetPitch(), 
		DestinationOffset, DestinationSize, ippBorderRepl, 0, 
		(IppiResizeSpec_32f*)DownsampleData.Spec, (Ipp8u*)DownsampleData.Buffer);


	Destination = (Ipp8u*)Output->GetData() + HalfBlockSize * Output->GetPixelSize();
	Result = ippiResizeLinear_8u_C4R(
		(Ipp8u*)Block01->GetData(), Block01->GetPitch(), 
		Destination, Output->GetPitch(), 
		DestinationOffset, DestinationSize, ippBorderRepl, 0, 
		(IppiResizeSpec_32f*)DownsampleData.Spec, (Ipp8u*)DownsampleData.Buffer);


	Destination = (Ipp8u*)Output->GetData() + HalfBlockSize * Output->GetPitch();
	Result = ippiResizeLinear_8u_C4R(
		(Ipp8u*)Block10->GetData(), Block10->GetPitch(), 
		Destination, Output->GetPitch(), 
		DestinationOffset, DestinationSize, ippBorderRepl, 0, 
		(IppiResizeSpec_32f*)DownsampleData.Spec, (Ipp8u*)DownsampleData.Buffer);

	Destination = (Ipp8u*)Output->GetData()  + HalfBlockSize * Output->GetPitch() + HalfBlockSize * Output->GetPixelSize();
	Result = ippiResizeLinear_8u_C4R(
		(Ipp8u*)Block11->GetData(), Block11->GetPitch(), 
		Destination, Output->GetPitch(), 
		DestinationOffset, DestinationSize, ippBorderRepl, 0, 
		(IppiResizeSpec_32f*)DownsampleData.Spec, (Ipp8u*)DownsampleData.Buffer);
}

void ZETEResamplerIPP::DownsampleGrayscale(ZETEBlock* Output, ZETEBlock* Block00, ZETEBlock* Block01, ZETEBlock* Block10, ZETEBlock* Block11)
{
	ZESize HalfBlockSize = Block00->GetSize() / 2;
	IppiSize SourceSize = {Block00->GetSize(), Block00->GetSize()};
	IppiSize DestinationSize = {HalfBlockSize, HalfBlockSize};

	IppStatus Result;

	int CurrentSpecSize = 0;
	int CurrentInitBufferSize = 0;
	Result = ippiResizeGetSize_8u(SourceSize, DestinationSize, ippLinear, 0, &CurrentSpecSize, &CurrentInitBufferSize);
	if (Result != ippStsNoErr) 
		return;

	if (CurrentInitBufferSize != DownsampleData.InitBufferSize)
	{
		if (DownsampleData.InitBuffer != NULL)
		{
			ippsFree(DownsampleData.InitBuffer);
			DownsampleData.InitBuffer = NULL;
		}

		DownsampleData.InitBuffer = ippsMalloc_8u(CurrentInitBufferSize);
		DownsampleData.InitBufferSize = CurrentInitBufferSize;
	}

	if (CurrentSpecSize > DownsampleData.SpecSize)
	{
		if (DownsampleData.Spec != NULL)
		{
			ippsFree(DownsampleData.Spec);
			DownsampleData.Spec = NULL;
		}

		DownsampleData.Spec = ippsMalloc_8u(CurrentSpecSize);
		DownsampleData.SpecSize = CurrentSpecSize;

		ippiResizeLinearInit_8u(SourceSize, DestinationSize, (IppiResizeSpec_32f*)DownsampleData.Spec);
	}

	int CurrentBufferSize = 0;
	Result = ippiResizeGetBufferSize_8u((IppiResizeSpec_32f*)DownsampleData.Spec, DestinationSize, 1, &CurrentBufferSize);
	if (Result != ippStsNoErr) 
		return;

	if (CurrentBufferSize > DownsampleData.BufferSize)
	{
		if (DownsampleData.Buffer != NULL)
		{
			ippsFree(DownsampleData.Buffer);
			DownsampleData.Buffer = NULL;
		}

		DownsampleData.Buffer = ippsMalloc_8u(CurrentBufferSize);
		DownsampleData.BufferSize = CurrentBufferSize;
	}

	IppiPoint DestinationOffset;
	DestinationOffset.x = 0;
	DestinationOffset.y = 0;

	Ipp8u* Destination = (Ipp8u*)Output->GetData();
	Result = ippiResizeLinear_8u_C1R(
		(Ipp8u*)Block00->GetData(), Block00->GetPitch(), 
		Destination, Output->GetPitch(), 
		DestinationOffset, DestinationSize, ippBorderRepl, 0, 
		(IppiResizeSpec_32f*)DownsampleData.Spec, (Ipp8u*)DownsampleData.Buffer);


	Destination = (Ipp8u*)Output->GetData() + HalfBlockSize * Output->GetPixelSize();
	Result = ippiResizeLinear_8u_C1R(
		(Ipp8u*)Block01->GetData(), Block01->GetPitch(), 
		Destination, Output->GetPitch(), 
		DestinationOffset, DestinationSize, ippBorderRepl, 0, 
		(IppiResizeSpec_32f*)DownsampleData.Spec, (Ipp8u*)DownsampleData.Buffer);


	Destination = (Ipp8u*)Output->GetData() + HalfBlockSize * Output->GetPitch();
	Result = ippiResizeLinear_8u_C1R(
		(Ipp8u*)Block10->GetData(), Block10->GetPitch(), 
		Destination, Output->GetPitch(), 
		DestinationOffset, DestinationSize, ippBorderRepl, 0, 
		(IppiResizeSpec_32f*)DownsampleData.Spec, (Ipp8u*)DownsampleData.Buffer);

	Destination = (Ipp8u*)Output->GetData()  + HalfBlockSize * Output->GetPitch() + HalfBlockSize * Output->GetPixelSize();
	Result = ippiResizeLinear_8u_C1R(
		(Ipp8u*)Block11->GetData(), Block11->GetPitch(), 
		Destination, Output->GetPitch(), 
		DestinationOffset, DestinationSize, ippBorderRepl, 0, 
		(IppiResizeSpec_32f*)DownsampleData.Spec, (Ipp8u*)DownsampleData.Buffer);
}


void ZETEResamplerIPP::Downsample(ZETEBlock* Output, ZETEBlock* Block00, ZETEBlock* Block01, ZETEBlock* Block10, ZETEBlock* Block11)
{
	if (Block00->GetPixelType() == ZETE_PT_ELEVATION)
		DownsampleElevation(Output, Block00, Block01, Block10, Block11);
	else if (Block00->GetPixelType() == ZETE_PT_COLOR)
		DownsampleColor(Output, Block00, Block01, Block10, Block11);
	else if (Block00->GetPixelType() == ZETE_PT_GRAYSCALE)
		DownsampleGrayscale(Output, Block00, Block01, Block10, Block11);
}

void ZETEResamplerIPP::CleanUp()
{
	ResampleData.BufferSize = 0;
	if (ResampleData.Buffer != NULL)
	{
		ippsFree(ResampleData.Buffer);
		ResampleData.Buffer = NULL;
	}

	ResampleData.PatchBufferSize = 0;
	if (ResampleData.PatchBuffer != NULL)
	{
		delete ResampleData.PatchBuffer;
		ResampleData.PatchBuffer = NULL;
	}

	DownsampleData.BufferSize = 0;
	if (DownsampleData.Buffer != NULL)
	{
		ippsFree(DownsampleData.Buffer);
		DownsampleData.Buffer = NULL;
	}

	DownsampleData.SpecSize = 0;
	if (DownsampleData.Spec != NULL)
	{
		ippsFree(DownsampleData.Spec);
		DownsampleData.Spec = NULL;
	}

	DownsampleData.InitBufferSize = 0;
	if (DownsampleData.InitBuffer != NULL)
	{
		ippsFree(DownsampleData.InitBuffer);
		DownsampleData.InitBuffer = NULL;
	}
}
ZETEResamplerIPP::ZETEResamplerIPP()
{
	ResampleData.Buffer = NULL;
	ResampleData.BufferSize = 0;
	ResampleData.PatchBuffer = NULL;
	ResampleData.PatchBufferSize = 0;
	DownsampleData.Spec = NULL;
	DownsampleData.SpecSize = 0;
	DownsampleData.InitBuffer = NULL;
	DownsampleData.InitBufferSize = 0;
	DownsampleData.Buffer = NULL;
	DownsampleData.BufferSize = 0;
}

ZETEResamplerIPP::~ZETEResamplerIPP()
{
	CleanUp();
}
