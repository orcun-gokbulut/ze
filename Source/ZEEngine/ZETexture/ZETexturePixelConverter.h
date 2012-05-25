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

struct ZEBGRA32;

class ZETexturePixelConverter
{
	public:
		static void ConvertIndexed(ZEBGRA32* Destination, void* Source, ZESize Count, ZEBGRA32* Palette);

		static void ConvertL8(ZEBGRA32* Destination, void* Source, ZESize Count);
		static void ConvertL16(ZEBGRA32* Destination, void* Source, ZESize Count);
		
		static void ConvertLA8(ZEBGRA32* Destination, void* Source, ZESize Count);
		static void ConvertAL8(ZEBGRA32* Destination, void* Source, ZESize Count);

		static void ConvertLA16(ZEBGRA32* Destination, void* Source, ZESize Count);
		static void ConvertAL16(ZEBGRA32* Destination, void* Source, ZESize Count);

		static void ConvertBGR555(ZEBGRA32* Destination, void* Source, ZESize Count);
		static void ConvertRGB555(ZEBGRA32* Destination, void* Source, ZESize Count);

		static void ConvertBGRA1555(ZEBGRA32* Destination, void* Source, ZESize Count);
		static void ConvertARGB1555(ZEBGRA32* Destination, void* Source, ZESize Count);

		static void ConvertBGR565(ZEBGRA32* Destination, void* Source, ZESize Count);
		static void ConvertRGB565(ZEBGRA32* Destination, void* Source, ZESize Count);

		static void ConvertBGR8(ZEBGRA32* Destination, void* Source, ZESize Count);
		static void ConvertRGB8(ZEBGRA32* Destination, void* Source, ZESize Count);
		
		static void ConvertBGRA8(ZEBGRA32* Destination, void* Source, ZESize Count);
		static void ConvertARGB8(ZEBGRA32* Destination, void* Source, ZESize Count);

		static void ConvertABGR8(ZEBGRA32* Destination, void* Source, ZESize Count);
		static void ConvertRGBA8(ZEBGRA32* Destination, void* Source, ZESize Count);
};

#endif
