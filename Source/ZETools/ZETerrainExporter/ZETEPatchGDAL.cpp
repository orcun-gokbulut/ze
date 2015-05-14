//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETEPatchGDAL.cpp
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

#include "ZETEPatchGDAL.h"
#include "ZEError.h"

#include <gdal_priv.h>
#include "ippi.h"

struct ZETEThreadData
{
	GDALDataset* Dataset;
};

void* ZETEPatchGDAL::ThreadBegin()
{
	GDALDataset* Dataset = (GDALDataset*)GDALOpen(GetSource(), GA_ReadOnly);
	if (Dataset == NULL)
		return NULL;

	ZETEThreadData* ThreadData = new ZETEThreadData();
	ThreadData->Dataset = Dataset;

	return ThreadData;
}

bool ZETEPatchGDAL::GetData(void* Output, ZEUInt64 x, ZEUInt64 y, ZESize Width, ZESize Height, void* ThreadData)
{
	GDALDataset* Dataset = NULL;
	if (ThreadData == NULL)
		Dataset = this->Dataset;
	else
		Dataset = static_cast<ZETEThreadData*>(ThreadData)->Dataset;

	if (Dataset == NULL)
		return false;

	if (x + Width >= this->Width)
		return false;

	if (y + Height >= this->Height)
		return false;
	
	memset(Output, 0xFF, Width * Height * GetPixelSize());
	Lock.Lock();
	CPLErr Result = Dataset->RasterIO(GF_Read, x, y, Width, Height, (ZEBYTE*)Output, Width, Height, GDT_Byte, Dataset->GetRasterCount(), NULL, GetPixelSize(),  Width * GetPixelSize(), 1);
	Lock.Unlock();
	if (Result != CE_None)
		return false;

	IppiSize Size;
	Size.width = Width;
	Size.height = Height;
	int Order[4] = {2, 1, 0, 3};
	ippiSwapChannels_8u_C4IR((Ipp8u*)Output, Width * GetPixelSize(), Size, Order);

	return true;
}

void ZETEPatchGDAL::ThreadEnd(void* ThreadData)
{
	if (ThreadData == NULL)
		return;
	
	ZETEThreadData* Data = static_cast<ZETEThreadData*>(ThreadData);
	if (Data->Dataset != NULL)
	{
		GDALClose(Data->Dataset);
		Data->Dataset = NULL;
	}

	delete Data;
}

bool ZETEPatchGDAL::Load()
{
	Unload();

	Dataset = (GDALDataset*)GDALOpen(GetSource(), GA_ReadOnly);
	if (Dataset == NULL)
		return false;

	Width = Dataset->GetRasterXSize();
	Height = Dataset->GetRasterYSize();
	PixelType = ZETE_PT_COLOR;
}

void ZETEPatchGDAL::Unload()
{
	if (Dataset != NULL)
	{
		GDALClose(Dataset);
		Dataset = NULL;
	}
}

ZETEPatchGDAL::ZETEPatchGDAL()
{
	Dataset = NULL;
}

ZETEPatchGDAL::~ZETEPatchGDAL()
{
	Unload();
}
