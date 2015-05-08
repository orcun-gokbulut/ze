//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETEBlock.h
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
#include "ZEDS\ZEString.h"

class ZEFile;

enum ZETEPixelType
{
	ZETE_PT_NONE		= 0,
	ZETE_PT_ELEVATION	= 1,	// Float
	ZETE_PT_COLOR		= 2,	// ZEUInt8[4]
	ZETE_PT_GRAYSCALE	= 3		// ZEUInt8
};

class ZETEBlock
{
	private:
		ZEInt64					PositionX;
		ZEInt64					PositionY;
		ZEUInt					Level;
		ZESize					Size;

		void*					Data;
		ZESize					DataSize;
		ZETEPixelType			PixelType;

		void					Configure();

	public:
		void					SetPositionX(ZEInt64 x);
		ZEInt64					GetPositionX();

		void					SetPositionY(ZEInt64 y);
		ZEInt64					GetPositionY();

		void					SetLevel(ZEUInt Level);
		ZEUInt					GetLevel();

		void					SetSize(ZESize BlockSize);
		ZESize					GetSize();

		void					SetPixelType(ZETEPixelType Type);
		ZETEPixelType			GetPixelType();

		ZESize					GetPixelSize();

		void*					GetData();
		ZESize					GetPitch();
		ZESize					GetDataSize();

		bool					Load(const ZEString& FileName);
		bool					Load(ZEFile* File);

		bool					Save(const ZEString& FileName);
		bool					Save(ZEFile* File);
		
		bool					Dump(const ZEString& Directory);

		void					Clean();

								ZETEBlock();
								~ZETEBlock();
};
