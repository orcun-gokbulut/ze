//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETexturePixelConverter.h
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
#ifndef __ZE_TEXTURE_PIXEL_CONVERTER_H__
#define __ZE_TEXTURE_PIXEL_CONVERTER_H__

#include "ZETypes.h"
#include "ZEPixel.h"

class ZETexturePixelConverter
{
	public:
		static void ConvertIndexed(ZEPixelRGBA8* Destination, void* Source, ZESize Count, ZEPixelRGBA8* Palette);
		static void ConvertIndexed16(ZEPixelRGBA8* Destination, void* Source, ZESize Count, ZEPixelRGBA8* Palette);

		static void ConvertL8_INV(ZEPixelL8* Destination, void* Source, ZESize Count);
		static void ConvertLA8_INV(ZEPixelLA8* Destination, void* Source, ZESize Count);

		static void ConvertL16_INV(ZEPixelL16* Destination, void* Source, ZESize Count);
		static void ConvertLA16_INV(ZEPixelLA16* Destination, void* Source, ZESize Count);

		static void ConvertL16_INV_BE(ZEPixelL16* Destination, void* Source, ZESize Count);
		static void ConvertLA16_INV_BE(ZEPixelLA16* Destination, void* Source, ZESize Count);

		static void ConvertL16_BE(ZEPixelL16* Destination, void* Source, ZESize Count);
		static void ConvertLA16_BE(ZEPixelLA16* Destination, void* Source, ZESize Count);

		static void ConvertAL8(ZEPixelLA8* Destination, void* Source, ZESize Count);
		static void ConvertAL16(ZEPixelLA16* Destination, void* Source, ZESize Count);

		static void ConvertBGRX5551(ZEPixelRGBA8* Destination, void* Source, ZESize Count);
		static void ConvertBGRA5551(ZEPixelRGBA8* Destination, void* Source, ZESize Count);
		static void ConvertBGRX1555(ZEPixelRGBA8* Destination, void* Source, ZESize Count);
		static void ConvertBGR565(ZEPixelRGBA8* Destination, void* Source, ZESize Count);

		static void ConvertRGB8(ZEPixelRGBA8* Destination, void* Source, ZESize Count);
		static void ConvertBGR8(ZEPixelRGBA8* Destination, void* Source, ZESize Count);
		static void ConvertBGRA8(ZEPixelRGBA8* Destination, void* Source, ZESize Count);
		static void ConvertBGRX8(ZEPixelRGBA8* Destination, void* Source, ZESize Count);

		static void ConvertRGB16(ZEPixelRGBA16* Destination, void* Source, ZESize Count);
		static void ConvertRGB16_BE(ZEPixelRGBA16* Destination, void* Source, ZESize Count);
		static void ConvertRGBA16_BE(ZEPixelRGBA16* Destination, void* Source, ZESize Count);
};

#endif
