//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRFormat.h
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
#include "ZETypes.h"

#pragma once

#include "ZEMeta\ZEObject.h"
#include "ZEMeta\ZEEnumerator.h"

ZE_ENUM(ZEGRFormatStructure)
{
	ZEGR_FS_NONE,
	ZEGR_FS_R8,
	ZEGR_FS_R8G8,
	ZEGR_FS_R8G8B8A8,
	ZEGR_FS_R16,
	ZEGR_FS_R16G16,
	ZEGR_FS_R16G16B16A16,
	ZEGR_FS_R32,
	ZEGR_FS_R32G32,
	ZEGR_FS_R32G32B32A32,
	ZEGR_FS_R10G10B10A2,
	ZEGR_FS_R11G11B10,
	ZEGR_FS_BC1,
	ZEGR_FS_BC3,
	ZEGR_FS_BC4,
	ZEGR_FS_BC5,
	ZEGR_FS_BC7,
	ZEGR_FS_D16,
	ZEGR_FS_D24_S8,
	ZEGR_FS_D32,
	ZEGR_FS_D32_S8X24
};

ZE_ENUM(ZEGRFormatType)
{
	ZEGR_FT_NONE,
	ZEGR_FT_SINT,
	ZEGR_FT_UINT,
	ZEGR_FT_SNORM,
	ZEGR_FT_UNORM,
	ZEGR_FT_UNORM_UINT,
	ZEGR_FT_UNORM_SRGB,
	ZEGR_FT_FLOAT,
	ZEGR_FT_FLOAT_UINT
};

ZE_ENUM(ZEGRFormatEncoding)
{
	ZEGR_FE_NONE,
	ZEGR_FE_PIXEL,
	ZEGR_FE_BLOCK
};

ZE_ENUM(ZEGRFormat)
{
	ZEGR_TF_NONE = 0,

	ZEGR_TF_R8_UINT,
	ZEGR_TF_R8_SINT,
	ZEGR_TF_R8_UNORM,
	ZEGR_TF_R8_SNORM,		

	ZEGR_TF_R8G8_UINT,			
	ZEGR_TF_R8G8_SINT,			
	ZEGR_TF_R8G8_UNORM,		
	ZEGR_TF_R8G8_SNORM,

	ZEGR_TF_R8G8B8A8_UINT,
	ZEGR_TF_R8G8B8A8_SINT,
	ZEGR_TF_R8G8B8A8_UNORM,
	ZEGR_TF_R8G8B8A8_UNORM_SRGB,
	ZEGR_TF_R8G8B8A8_SNORM,

	ZEGR_TF_R16_UINT,
	ZEGR_TF_R16_SINT,
	ZEGR_TF_R16_UNORM,
	ZEGR_TF_R16_SNORM,
	ZEGR_TF_R16_FLOAT,

	ZEGR_TF_R16G16_UINT,
	ZEGR_TF_R16G16_SINT,
	ZEGR_TF_R16G16_UNORM,
	ZEGR_TF_R16G16_SNORM,
	ZEGR_TF_R16G16_FLOAT,

	ZEGR_TF_R16G16B16A16_UINT,
	ZEGR_TF_R16G16B16A16_SINT,
	ZEGR_TF_R16G16B16A16_UNORM,
	ZEGR_TF_R16G16B16A16_SNORM,
	ZEGR_TF_R16G16B16A16_FLOAT,

	ZEGR_TF_R32_UINT,
	ZEGR_TF_R32_SINT,
	ZEGR_TF_R32_FLOAT,

	ZEGR_TF_R32G32_UINT,
	ZEGR_TF_R32G32_SINT,
	ZEGR_TF_R32G32_FLOAT,

	ZEGR_TF_R32G32B32A32_UINT,
	ZEGR_TF_R32G32B32A32_SINT,
	ZEGR_TF_R32G32B32A32_FLOAT,

	ZEGR_TF_R10G10B10A2_UINT,
	ZEGR_TF_R10G10B10A2_UNORM,

	ZEGR_TF_R11G11B10_FLOAT,

	ZEGR_TF_BC1_UNORM,
	ZEGR_TF_BC1_UNORM_SRGB,

	ZEGR_TF_BC3_UNORM,
	ZEGR_TF_BC3_UNORM_SRGB,

	ZEGR_TF_BC4_UNORM,
	ZEGR_TF_BC4_SNORM,

	ZEGR_TF_BC5_UNORM,
	ZEGR_TF_BC5_SNORM,

	ZEGR_TF_BC7_UNORM,
	ZEGR_TF_BC7_UNORM_SRGB,

	ZEGR_TF_D16_UNORM,
	ZEGR_TF_D24_UNORM_S8_UINT,
	ZEGR_TF_D32_FLOAT,
	ZEGR_TF_D32_FLOAT_S8X24_UINT,

	ZEGR_TF_RESERVED
};

class ZEGRFormatDefinition
{
	public:
		ZEGRFormat							Format;
		ZEGRFormatStructure					Structure;
		ZEGRFormatType						Type;
		ZEGRFormatEncoding					Encoding;
		ZESize								BlockDimension;
		ZESize								BlockSize;
		bool								Compressed;

		static ZESize						GetFormatCount();
		static const ZEGRFormatDefinition*	GetDefinitions();
		static const ZEGRFormatDefinition*	GetDefinition(ZEGRFormat Format);
};
