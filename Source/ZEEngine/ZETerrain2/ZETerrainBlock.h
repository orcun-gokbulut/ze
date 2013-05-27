//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETerrainBlock.h
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
class ZEFile;

enum ZETerrainPixelType
{
	ZE_TPT_NONE			= 0,
	ZE_TPT_ELEVATION	= 1,
	ZE_TPT_COLOR		= 2,
	ZE_TPT_GRAYSCALE	= 3	
};

enum ZETerrainBlockState
{
	ZE_TBS_EMPTY,
	ZE_TBS_NORMAL,
	ZE_TBS_LOADING,
	ZE_TBS_NOT_AVAILABLE
};

struct ZETerrainBlockInfo
{
	ZESize						BlockSize;
	ZETerrainPixelType			PixelType;
	ZESize						PixelSize;
	ZESize						MipmapCount;
};

class ZETerrainBlock
{
	private:
		ZEUInt64				PositionX;
		ZEUInt64				PositionY;
		ZEUInt					Level;
		ZESize					Size;

		void*					Data;
		ZESize					TotalSize;
		ZESize					Pitch;
		ZEUInt					PixelSize;
		ZETerrainPixelType		PixelType;

		ZEUInt					MipmapCount;
		float					MinValue;
		float					MaxValue;
		
		ZETerrainBlockState		State;

	public:
		void					SetState(ZETerrainBlockState State);
		bool					GetState();

		void					SetPositionX(ZEUInt64 x);
		ZEUInt64				GetPositionX();

		void					SetPositionY(ZEUInt64 y);
		ZEUInt64				GetPositionY();

		ZEUInt64				GetEndX();
		ZEUInt64				GetEndY();

		void					SetLevel(ZEUInt Level);
		ZEUInt					GetLevel();

		ZETerrainPixelType		GetPixelType();
		ZESize					GetPixelSize();

		ZEUInt					GetMipmapCount();

		void*					GetData(ZEUInt MipmapLevel = 0);
		ZESize					GetPitch(ZEUInt MipmapLevel = 0);
		ZESize					GetSize(ZEUInt MipmapLevel = 0);
		ZESize					GetTotalSize();

		float					GetMinValue();
		float					GetMaxValue();
		
		void					Sample(void* Output, float x, float y);

		bool					Create(ZETerrainPixelType PixelType, ZESize Size);
		void					Clean();

		bool					Load(ZEFile* File);
		bool					Save(ZEFile* File);
		
								ZETerrainBlock();
								~ZETerrainBlock();
};
