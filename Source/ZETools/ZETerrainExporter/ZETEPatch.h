//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETEPatch.h
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
#include "ZETEBlock.h"
#include "ZEMath\ZEVector.h"

class ZETEPatchDatabase;

class ZETEPatch
{
	friend class ZETEPatchDatabase;
	private:
		ZETEPatchDatabase*		Database;

		ZEUInt					Priority;
		ZEInt					Level;
		double					StartX, StartY;
		double					EndX, EndY;

		double					LevelScalingX, LevelScalingY;
		double					PixelScaleX, PixelScaleY;

		ZEString				Source;

		bool					Loaded;

	protected:
		ZESize					Width, Height;
		ZETEPixelType			PixelType;

		void					UpdateLevelAndScaling();

	public:
		void					SetPriority(ZEUInt Priority);
		ZEUInt					GetPriority();

		ZEInt					GetLevel();

		void					SetPositionX(double x);
		double					GetPositionX();

		void					SetPositionY(double y);
		double					GetPositionY();

		void					SetEndX(double EndX);
		double					GetEndX();

		void					SetEndY(double EndY);
		double					GetEndY();

		double					GetPixelScaleX();
		double					GetPixelScaleY();

		double					GetLevelScaleX();
		double					GetLevelScaleY();

		ZESize					GetWidth();
		ZESize					GetHeight();

		ZETEPixelType			GetPixelType();
		ZESize					GetPixelSize();
		
		virtual void			SetSource(const ZEString& Source);
		const ZEString&			GetSource();

		bool					Intersect(double PositionX, double PositionY, double Width, double Height);
		
		virtual bool			GetData(void* Output, ZEUInt64 x, ZEUInt64 y, ZESize Width, ZESize Height) = 0;

		bool					IsLoaded();
		virtual bool			Load();
		virtual void			Unload();

								ZETEPatch();
								~ZETEPatch();
};
