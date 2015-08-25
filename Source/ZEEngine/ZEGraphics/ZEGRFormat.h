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

enum ZEGRFormatStructure
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
	ZEGR_FS_R11FG11FB10F,
	ZEGR_FS_DXT1,	
	ZEGR_FS_DXT3,			
	ZEGR_FS_DXT5
};

enum ZEGRFormatType
{
	ZEGR_FT_NONE,
	ZEGR_FT_SINT,
	ZEGR_FT_UINT,
	ZEGR_FT_SNORM,
	ZEGR_FT_UNORM,
	ZEGR_FT_FLOAT
};

enum ZEGRFormatEncoding
{
	ZEGR_FE_NONE,
	ZEGR_FE_PIXEL,
	ZEGR_FE_BLOCK
};

enum ZEGRFormat
{
	ZEGR_TF_NONE					= 0,
	ZEGR_TF_R8_UINT					= 1,
	ZEGR_TF_R8_SINT					= 2,
	ZEGR_TF_R8_UNORM				= 3,
	ZEGR_TF_R8_SNORM				= 4,

	ZEGR_TF_R8G8_UINT				= 5,
	ZEGR_TF_R8G8_SINT				= 6,
	ZEGR_TF_R8G8_UNORM				= 7,
	ZEGR_TF_R8G8_SNORM				= 8,

	ZEGR_TF_R8G8B8A8_UINT			= 9,
	ZEGR_TF_R8G8B8A8_SINT			= 10,
	ZEGR_TF_R8G8B8A8_UNORM			= 11,
	ZEGR_TF_R8G8B8A8_SNORM			= 12,

	ZEGR_TF_R16_UINT				= 13,
	ZEGR_TF_R16_SINT				= 14,
	ZEGR_TF_R16_UNORM				= 15,
	ZEGR_TF_R16_SNORM				= 16,
	ZEGR_TF_R16_FLOAT				= 17,

	ZEGR_TF_R16G16_UINT				= 18,
	ZEGR_TF_R16G16_SINT				= 19,
	ZEGR_TF_R16G16_UNORM			= 20,
	ZEGR_TF_R16G16_SNORM			= 21,
	ZEGR_TF_R16G16_FLOAT			= 22,

	ZEGR_TF_R16G16B16A16_UINT		= 23,
	ZEGR_TF_R16G16B16A16_SINT		= 24,
	ZEGR_TF_R16G16B16A16_UNORM		= 25,
	ZEGR_TF_R16G16B16A16_SNORM		= 26,
	ZEGR_TF_R16G16B16A16_FLOAT		= 27,

	ZEGR_TF_R32_UINT				= 28,
	ZEGR_TF_R32_SINT				= 29,
	ZEGR_TF_R32_UNORM				= 30,
	ZEGR_TF_R32_SNORM				= 31,
	ZEGR_TF_R32_FLOAT				= 32,

	ZEGR_TF_R32G32_UINT				= 33,
	ZEGR_TF_R32G32_SINT				= 34,
	ZEGR_TF_R32G32_UNORM			= 35,
	ZEGR_TF_R32G32_SNORM			= 36,
	ZEGR_TF_R32G32_FLOAT			= 37,

	ZEGR_TF_R32G32B32A32_UINT		= 38,
	ZEGR_TF_R32G32B32A32_SINT		= 39,
	ZEGR_TF_R32G32B32A32_FLOAT		= 40,

	ZEGR_TF_R10G10B10A2_UINT		= 41,
	ZEGR_TF_R10G10B10A2_UNORM		= 42,

	ZEGR_TF_R11FG11FB10F_FLOAT		= 43,

	ZEGR_TF_DXT1_UNORM				= 44,
	ZEGR_TF_DXT3_UNORM				= 45,
	ZEGR_TF_DXT5_UNORM				= 46,

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
