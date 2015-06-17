//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETexture.cpp
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

#include "ZETexture.h"

#include "ZEGraphicsModule.h"

bool ZEGRTexture::GetIsRenderTarget() const
{
	return IsRenderTarget;
}

ZEGRTextureFormat ZEGRTexture::GetFormat() const
{
	return Format;
}

ZEGRTexture::ZEGRTexture()
{
	IsRenderTarget = false;
	LevelCount = 0;
	Format = ZEGR_TF_NONE;
}

ZESize ZEGRTexture::GetBlockSize(ZEGRTextureFormat PixelFormat)
{
	ZESize Size = 0;
	switch (PixelFormat)
	{
		case ZEGR_TF_R8:
			Size = 1;
			break;

		case ZEGR_TF_R16F:
		case ZEGR_TF_R16:	
		case ZEGR_TF_R8G8:
			Size = 2;
			break;

		case ZEGR_TF_R32F:
		case ZEGR_TF_R8G8B8A8:
		case ZEGR_TF_R16G16:
		case ZEGR_TF_R16FG16F:
		case ZEGR_TF_INTZ:
			Size = 4;
			break;

		case ZEGR_TF_R16G16B16A16: // 8 byte
		case ZEGR_TF_R16FG16FB16FA16F:
		case ZEGR_TF_R32G32:
		case ZEGR_TF_R32FG32F:	
			Size = 8;
			break;

		case ZEGR_TF_R32G32B32A32:
		case ZEGR_TF_R32FG32FB32FA32F:	
			Size = 16;
				
		case ZEGR_TF_DXT1:
			Size = 8;
			break;
		case ZEGR_TF_DXT3:
			Size = 16;
			break;
		case ZEGR_TF_DXT5:
			Size = 16;
			break;
	};

	return Size;
}
