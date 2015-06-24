//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRTexture.cpp
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

#include "ZEGRTexture.h"

#include "ZEGRGraphicsModule.h"
#include "ZEMath\ZEMath.h"

void ZEGRTexture::SetFormat(ZEGRTextureFormat Format)
{
	this->Format = Format;
}

void ZEGRTexture::SetLevelCount(ZEUInt LevelCount)
{
	this->LevelCount = LevelCount;
}

void ZEGRTexture::SetIsRenderTarget(bool RenderTarget)
{
	this->IsRenderTarget = RenderTarget;
}

ZESize ZEGRTexture::CalculateSize(ZEUInt Width, ZEUInt Height, ZEUInt LevelCount, ZESize BlockSize, ZESize BlockDimension)
{
	ZESize Output = 0;

	Width /= (ZEUInt)BlockDimension;
	Height /= (ZEUInt)BlockDimension;

	for (ZESize I = 0; I < LevelCount; I++)
		Output += (Width << LevelCount) * (Height << LevelCount);

	return Output * BlockSize;
}

ZESize ZEGRTexture::CalculateLevelCount(ZEUInt Width, ZEUInt Height, ZESize BlockDimension)
{
	Width /= (ZEUInt)BlockDimension;
	Height /= (ZEUInt)BlockDimension;

	if (!ZEMath::IsPowerOfTwo(Width) || ZEMath::IsPowerOfTwo(Height))
		return 1;

	ZEUInt Level = 1;
	while(Width != 1 && Height != 1)
	{
		Width <<= 1;
		Height <<= 1;
		Level++;
	}

	return Level;
}

ZESize ZEGRTexture::GetBlockSize(ZEGRTextureFormat Format)
{
	switch (Format)
	{
		default:
		case ZEGR_TF_NONE:
		case ZEGR_TF_NULL:
			return 0;

		case ZEGR_TF_R8:
			return 1;

		case ZEGR_TF_R8G8:
		case ZEGR_TF_R16:
		case ZEGR_TF_R16F:
			return 2;

		case ZEGR_TF_R8G8B8A8:
		case ZEGR_TF_R16G16:
		case ZEGR_TF_R16FG16F:
		case ZEGR_TF_R32:
		case ZEGR_TF_R32F:
			return 4;

		case ZEGR_TF_R16G16B16A16:
		case ZEGR_TF_R16FG16FB16FA16F:
		case ZEGR_TF_R32G32:
		case ZEGR_TF_R32FG32F:
			return 8;

		case ZEGR_TF_R32G32B32A32:
		case ZEGR_TF_R32FG32FB32FA32F:
			return 16;

		case ZEGR_TF_DXT1:
			return 8;

		case ZEGR_TF_DXT3:
		case ZEGR_TF_DXT5:
			return 16;
	}
}

ZESize ZEGRTexture::GetBlockDimension(ZEGRTextureFormat Format)
{
	switch (Format)
	{
		default:
		case ZEGR_TF_NONE:
		case ZEGR_TF_NULL:
		case ZEGR_TF_R8:
		case ZEGR_TF_R8G8:
		case ZEGR_TF_R16:
		case ZEGR_TF_R16F:
		case ZEGR_TF_R8G8B8A8:
		case ZEGR_TF_R16G16:
		case ZEGR_TF_R16FG16F:
		case ZEGR_TF_R32:
		case ZEGR_TF_R32F:
		case ZEGR_TF_R16G16B16A16:
		case ZEGR_TF_R16FG16FB16FA16F:
		case ZEGR_TF_R32G32:
		case ZEGR_TF_R32FG32F:
		case ZEGR_TF_R32G32B32A32:
		case ZEGR_TF_R32FG32FB32FA32F:
			return 1;

		case ZEGR_TF_DXT1:
			return 8;

		case ZEGR_TF_DXT3:
		case ZEGR_TF_DXT5:
			return 4;
	}
}

ZEGRTexture::ZEGRTexture()
{
	Format = ZEGR_TF_NONE;
	LevelCount = 0;
	IsRenderTarget = false;
}

ZEGRTextureFormat ZEGRTexture::GetFormat()
{
	return Format;
}

ZESize ZEGRTexture::GetBlockSize()
{
	return GetBlockSize(Format);
}

ZESize ZEGRTexture::GetBlockDimension()
{
	return GetBlockDimension(Format);
}

bool ZEGRTexture::GetIsRenderTarget()
{
	return IsRenderTarget;
}
