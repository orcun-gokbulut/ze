//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETexture.h
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

#pragma once

#include "ZEGraphicsResource.h"

#include "ZETypes.h"

enum ZEGRTextureType
{
	ZEGR_TT_NONE	= 0,
	ZEGR_TT_2D		= 1,
	ZEGR_TT_3D		= 2,
	ZEGR_TT_CUBE	= 3
};

enum ZEGRTextureFormat
{
	ZEGR_TF_NONE,

	ZEGR_TF_R8,
	ZEGR_TF_R8G8,
	ZEGR_TF_R8G8B8A8,

	ZEGR_TF_R16,
	ZEGR_TF_R16G16,
	ZEGR_TF_R16G16B16A16,

	ZEGR_TF_R32,
	ZEGR_TF_R32G32,
	ZEGR_TF_R32G32B32A32,

	ZEGR_TF_R16F,
	ZEGR_TF_R16FG16F,
	ZEGR_TF_R16FG16FB16FA16F,

	ZEGR_TF_R32F,
	ZEGR_TF_R32FG32F,
	ZEGR_TF_R32FG32FB32FA32F,

	ZEGR_TF_DXT1,
	ZEGR_TF_DXT3,
	ZEGR_TF_DXT5,

	// Special formats for shadow mapping
	ZEGR_TF_NULL,
	ZEGR_TF_INTZ,
};

class ZEGRTexture : public ZEGRResource
{
	protected:
		ZEGRTextureFormat		Format;
		ZEUInt					LevelCount;
		bool					IsRenderTarget;

								ZEGRTexture();

	public:
		virtual ZEGRTextureType	GetTextureType() const = 0;

		ZEGRTextureFormat		GetFormat() const;
		ZEUInt					GetLevelCount() const;
		bool					GetIsRenderTarget() const;

		static ZESize			GetBlockSize(ZEGRTextureFormat PixelFormat);
};
