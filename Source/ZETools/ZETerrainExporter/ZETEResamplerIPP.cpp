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

#include <ippi.h>
#include <ippcore.h>
#include <ipps.h>

void ZETEResamplerIPP::Resample(ZETEPatch* Patch, ZETEBlock* Block)
{
	double PositionX = (Block->GetPositionX() - Patch->GetStartX()) / Patch->GetPixelScaleX();
	double PositionY = (Block->GetPositionY() - Patch->GetStartY()) / Patch->GetPixelScaleY();

	IppiSize SourceSize;
	SourceSize.width = Patch->GetWidth();
	SourceSize.height = Patch->GetHeight();

	IppiRect SourceROI;
	SourceROI.x = 0;
	SourceROI.y = 0;
	SourceROI.width = Patch->GetWidth();
	SourceROI.height = Patch->GetHeight();

	IppiRect DestROI;
	DestROI.x = 0;
	DestROI.y = 0;
	DestROI.width = Block->GetSize();
	DestROI.height = Block->GetSize();

	int CurrentBufferSize = 0;
	ippiResizeGetBufSize(SourceROI, DestROI, 4, IPPI_INTER_LINEAR, &CurrentBufferSize);
	if (CurrentBufferSize != BufferSize)
	{
		if (Buffer != NULL)
			ippsFree(Buffer);

		Buffer = ippsMalloc_8u(CurrentBufferSize);	
		BufferSize = CurrentBufferSize;
	}

	IppStatus Result = ippiResizeSqrPixel_8u_C4R(
		(const Ipp8u*)Patch->GetData(), SourceSize, Patch->GetPitch(), SourceROI, 
		(Ipp8u*)Block->GetData(), Block->GetPitch(), DestROI, 
		Patch->GetLevelScaleX(), Patch->GetLevelScaleY(), 
		-PositionX, -PositionY, 
		IPPI_INTER_LINEAR, (Ipp8u*)Buffer);
}

ZETEResamplerIPP::ZETEResamplerIPP()
{
	Buffer = NULL;
	BufferSize = 0;
}

ZETEResamplerIPP::~ZETEResamplerIPP()
{
	if (Buffer != NULL)
	{
		ippsFree(Buffer);
		Buffer = NULL;
		BufferSize = 0;
	}
}
