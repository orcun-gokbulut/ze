//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRFormat.cpp
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

#include "ZEGRFormat.h"

static ZEGRFormatDefinition FormatDefinitions[] =
{
	{ZEGR_TF_NONE,					ZEGR_FS_NONE,			ZEGR_FT_NONE,	ZEGR_FE_NONE,	0,	0,	false},		
	{ZEGR_TF_R8_UINT,				ZEGR_FS_R8,				ZEGR_FT_UINT,	ZEGR_FE_PIXEL,	1,	1,	false},
	{ZEGR_TF_R8_SINT,				ZEGR_FS_R8,				ZEGR_FT_SINT,	ZEGR_FE_PIXEL,	1,	1,	false},
	{ZEGR_TF_R8_UNORM,				ZEGR_FS_R8,				ZEGR_FT_UNORM,	ZEGR_FE_PIXEL,	1,	1,	false},
	{ZEGR_TF_R8_SNORM,				ZEGR_FS_R8,				ZEGR_FT_SNORM,	ZEGR_FE_PIXEL,	1,	1,	false},

	{ZEGR_TF_R8G8_UINT,				ZEGR_FS_R8G8,			ZEGR_FT_UINT,	ZEGR_FE_PIXEL,	1,	2,	false},
	{ZEGR_TF_R8G8_SINT,				ZEGR_FS_R8G8,			ZEGR_FT_SINT,	ZEGR_FE_PIXEL,	1,	2,	false},
	{ZEGR_TF_R8G8_UNORM,			ZEGR_FS_R8G8,			ZEGR_FT_UNORM,	ZEGR_FE_PIXEL,	1,	2,	false},
	{ZEGR_TF_R8G8_SNORM,			ZEGR_FS_R8G8,			ZEGR_FT_SNORM,	ZEGR_FE_PIXEL,	1,	2,	false},

	{ZEGR_TF_R8G8B8A8_UINT,			ZEGR_FS_R8G8B8A8,		ZEGR_FT_UINT,	ZEGR_FE_PIXEL,	1,	4,	false},
	{ZEGR_TF_R8G8B8A8_SINT,			ZEGR_FS_R8G8B8A8,		ZEGR_FT_SINT,	ZEGR_FE_PIXEL,	1,	4,	false},
	{ZEGR_TF_R8G8B8A8_UNORM,		ZEGR_FS_R8G8B8A8,		ZEGR_FT_UNORM,	ZEGR_FE_PIXEL,	1,	4,	false},
	{ZEGR_TF_R8G8B8A8_UNORM_SRGB,	ZEGR_FS_R8G8B8A8,		ZEGR_FT_UNORM,	ZEGR_FE_PIXEL,	1,	4,	false},
	{ZEGR_TF_R8G8B8A8_SNORM,		ZEGR_FS_R8G8B8A8,		ZEGR_FT_SNORM,	ZEGR_FE_PIXEL,	1,	4,	false},

	{ZEGR_TF_R16_UINT,				ZEGR_FS_R16,			ZEGR_FT_UINT,	ZEGR_FE_PIXEL,	1,	2,	false},
	{ZEGR_TF_R16_SINT,				ZEGR_FS_R16,			ZEGR_FT_SINT,	ZEGR_FE_PIXEL,	1,	2,	false},
	{ZEGR_TF_R16_UNORM,				ZEGR_FS_R16,			ZEGR_FT_UNORM,	ZEGR_FE_PIXEL,	1,	2,	false},
	{ZEGR_TF_R16_SNORM,				ZEGR_FS_R16,			ZEGR_FT_SNORM,	ZEGR_FE_PIXEL,	1,	2,	false},
	{ZEGR_TF_R16_FLOAT,				ZEGR_FS_R16,			ZEGR_FT_FLOAT,	ZEGR_FE_PIXEL,	1,	2,	false},

	{ZEGR_TF_R16G16_UINT,			ZEGR_FS_R16G16,			ZEGR_FT_UINT,	ZEGR_FE_PIXEL,	1,	4,	false},
	{ZEGR_TF_R16G16_SINT,			ZEGR_FS_R16G16,			ZEGR_FT_SINT,	ZEGR_FE_PIXEL,	1,	4,	false},
	{ZEGR_TF_R16G16_UNORM,			ZEGR_FS_R16G16,			ZEGR_FT_UNORM,	ZEGR_FE_PIXEL,	1,	4,	false},
	{ZEGR_TF_R16G16_SNORM,			ZEGR_FS_R16G16,			ZEGR_FT_SNORM,	ZEGR_FE_PIXEL,	1,	4,	false},
	{ZEGR_TF_R16G16_FLOAT,			ZEGR_FS_R16G16,			ZEGR_FT_FLOAT,	ZEGR_FE_PIXEL,	1,	4,	false},

	{ZEGR_TF_R16G16B16A16_UINT,		ZEGR_FS_R16G16B16A16,	ZEGR_FT_UINT,	ZEGR_FE_PIXEL,	1,	8,	false},
	{ZEGR_TF_R16G16B16A16_SINT,		ZEGR_FS_R16G16B16A16,	ZEGR_FT_SINT,	ZEGR_FE_PIXEL,	1,	8,	false},
	{ZEGR_TF_R16G16B16A16_UNORM,	ZEGR_FS_R16G16B16A16,	ZEGR_FT_UNORM,	ZEGR_FE_PIXEL,	1,	8,	false},
	{ZEGR_TF_R16G16B16A16_SNORM,	ZEGR_FS_R16G16B16A16,	ZEGR_FT_SNORM,	ZEGR_FE_PIXEL,	1,	8,	false},
	{ZEGR_TF_R16G16B16A16_FLOAT,	ZEGR_FS_R16G16B16A16,	ZEGR_FT_FLOAT,	ZEGR_FE_PIXEL,	1,	8,	false},

	{ZEGR_TF_R32_UINT,				ZEGR_FS_R32,			ZEGR_FT_UINT,	ZEGR_FE_PIXEL,	1,	4,	false},
	{ZEGR_TF_R32_SINT,				ZEGR_FS_R32,			ZEGR_FT_SINT,	ZEGR_FE_PIXEL,	1,	4,	false},
	{ZEGR_TF_R32_FLOAT,				ZEGR_FS_R32,			ZEGR_FT_FLOAT,	ZEGR_FE_PIXEL,	1,	4,	false},

	{ZEGR_TF_R32G32_UINT,			ZEGR_FS_R32G32,			ZEGR_FT_UINT,	ZEGR_FE_PIXEL,	1,	8,	false},
	{ZEGR_TF_R32G32_SINT,			ZEGR_FS_R32G32,			ZEGR_FT_SINT,	ZEGR_FE_PIXEL,	1,	8,	false},
	{ZEGR_TF_R32G32_FLOAT,			ZEGR_FS_R32G32,			ZEGR_FT_FLOAT,	ZEGR_FE_PIXEL,	1,	8,	false},

	{ZEGR_TF_R32G32B32A32_UINT,		ZEGR_FS_R32G32B32A32,	ZEGR_FT_UINT,	ZEGR_FE_PIXEL,	1,	16,	false},
	{ZEGR_TF_R32G32B32A32_SINT,		ZEGR_FS_R32G32B32A32,	ZEGR_FT_SINT,	ZEGR_FE_PIXEL,	1,	16,	false},
	{ZEGR_TF_R32G32B32A32_FLOAT,	ZEGR_FS_R32G32B32A32,	ZEGR_FT_FLOAT,	ZEGR_FE_PIXEL,	1,	16,	false},

	{ZEGR_TF_R10G10B10A2_UINT,		ZEGR_FS_R10G10B10A2,	ZEGR_FT_UINT,	ZEGR_FE_PIXEL,	1,	4,	false},
	{ZEGR_TF_R10G10B10A2_UNORM,		ZEGR_FS_R10G10B10A2,	ZEGR_FT_UNORM,	ZEGR_FE_PIXEL,	1,	4,	false},
	{ZEGR_TF_R11G11B10_FLOAT,		ZEGR_FS_R10G10B10A2,	ZEGR_FT_FLOAT,	ZEGR_FE_PIXEL,	1,	4,	false},

	{ZEGR_TF_DXT1_UNORM,			ZEGR_FS_DXT1,			ZEGR_FT_UNORM,	ZEGR_FE_PIXEL,	4,	8,	true},
	{ZEGR_TF_DXT3_UNORM,			ZEGR_FS_DXT3,			ZEGR_FT_UNORM,	ZEGR_FE_PIXEL,	4,	16,	true},
	{ZEGR_TF_DXT5_UNORM,			ZEGR_FS_DXT5,			ZEGR_FT_UNORM,	ZEGR_FE_PIXEL,	4,	16,	true}
};

ZESize ZEGRFormatDefinition::GetFormatCount()
{
	return sizeof(FormatDefinitions) / sizeof(ZEGRFormatDefinition);
}

const ZEGRFormatDefinition* ZEGRFormatDefinition::GetDefinitions()
{

	return FormatDefinitions;
}

const ZEGRFormatDefinition* ZEGRFormatDefinition::GetDefinition(ZEGRFormat Format)
{
	const ZEGRFormatDefinition* Definitions = GetDefinitions();
	if (Format >= GetFormatCount())
		return NULL;

	return &Definitions[Format];

}
