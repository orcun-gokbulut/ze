//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETerrainPatch.h
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
#include "ZETerrainBlock.h"
#include "ZEMath\ZEVector.h"

class ZETerrainPatch
{
	private:
		ZEUInt					Priority;
		ZEUInt					Level;
		double					StartX, StartY;
		double					EndX, EndY;
		double					ScaleX, ScaleY;

		void*					Data;
		ZESize					Width, Height;
		ZESize					Pitch;
		ZESize					PixelSize;
		ZETerrainPixelType		PixelType;

		void					UpdateScale();

	public:
		void					SetStartX(double x);
		double					GetStartX();

		void					SetStartY(double y);
		double					GetStartY();

		void					SetEndX(double EndX);
		double					GetEndX();

		void					SetEndY(double EndY);
		double					GetEndY();

		double					GetScaleX();
		double					GetScaleY();

		ZESize					GetWidth();
		ZESize					GetHeight();

		void*					GetData();
		ZESize					GetPitch();

		ZETerrainPixelType		GetPixelType();
		ZESize					GetPixelSize();

		ZEUInt					GetLevel();

		void					SetPriority(ZEUInt Priority);
		ZEUInt					GetPriority();
		
		bool					BoundaryCheck(double x, double y);

		void*					SampleLocalRaw(ZESSize x, ZESSize y);
		void					SampleLocal(void* Output, double x, double y);
		void					Sample(void* Output, double x, double y);

		bool					Create(ZESize Width, ZESize Height, ZETerrainPixelType PixelType);
		void					Clean();

		bool					Load(const char* FileName, ZETerrainPixelType PixelType);

								ZETerrainPatch();
								~ZETerrainPatch();
};
