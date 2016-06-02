//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureOptions.h
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

#include "ZETypes.h"

enum ZETextureQuality
{
	ZE_TQ_ULTRA_LOW = 0,
	ZE_TQ_VERY_LOW	= 1,
	ZE_TQ_LOW		= 2,
	ZE_TQ_NORMAL	= 3,
	ZE_TQ_HIGH		= 4,
	ZE_TQ_VERY_HIGH	= 5,
	ZE_TQ_AUTO		= 6,

};

enum ZETextureCompressionType
{
	ZE_TCT_NONE		= 0,
	ZE_TCT_DXT1		= 1,	//BC1
	ZE_TCT_DXT3		= 2,	//BC2
	ZE_TCT_DXT5		= 3,	//BC3
	ZE_TCT_3DC		= 4,
	ZE_TCT_AUTO		= 5,

};

enum ZETextureCompressionQuality
{
	ZE_TCQ_LOW		= 1,
	ZE_TCQ_NORMAL	= 2,
	ZE_TCQ_HIGH		= 3,
	ZE_TCQ_AUTO		= 4,

};

enum ZETextureDownSampling
{
	ZE_TDS_NONE		= 0,
	ZE_TDS_2X		= 1,
	ZE_TDS_4X		= 2,
	ZE_TDS_8X		= 3,
	ZE_TDS_AUTO		= 4,

};

enum ZETextureFileCaching
{
	ZE_TFC_DISABLED = 0,
	ZE_TFC_ENABLED	= 1,
	ZE_TFC_AUTO		= 2,

};

enum ZETextureMipMapping
{
	ZE_TMM_DISABLED = 0,
	ZE_TMM_ENABLED	= 1,
	ZE_TMM_AUTO		= 2,

};

struct ZETextureOptions
{
	ZETextureCompressionType		CompressionType;
	ZETextureCompressionQuality		CompressionQuality;
	ZETextureDownSampling			DownSample;
	ZETextureFileCaching			FileCaching;
	ZETextureMipMapping				MipMapping;
	ZEUInt32						MaximumMipmapLevel;
	bool							sRGB;
};
