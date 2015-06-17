//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERasterizerState.h
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

#include "ZEGRState.h"

#include "ZETypes.h"

enum ZEPrimitiveType
{
	ZEGR_PT_NONE			= 0,
	ZEGR_PT_POINT_LIST		= 1,
	ZEGR_PT_LINE_LIST		= 2,
	ZEGR_PT_TRIANGLE_LIST	= 3,
	ZEGR_PT_TRIANGLE_STRIPT	= 4
};

enum ZEFillMode
{
	ZEGR_FM_WIREFRAME		= 1,
	ZEGR_FM_SOLID			= 2
};

enum ZECullDirection
{
	ZEGR_CD_NONE				= 1,
	ZEGR_CD_CLOCKWISE 			= 2,
	ZEGR_CD_COUNTER_CLOCKWISE	= 3
};

class ZEGRRasterizerState : public ZEGRState
{
	private:
		struct ZERasterizerStateData	
		{
			ZEFillMode				FillMode : 3;
			ZECullDirection			CullDirection : 3;
			bool					FrontIsCounterClockwise : 2;	
		} StateData;

	public:
		void						SetFillMode(ZEFillMode Mode);
		ZEFillMode					GetFillMode() const;
	
		void						SetCullDirection(ZECullDirection Direction);
		ZECullDirection				GetCullDirection() const;
	
		void						SetFrontIsCounterClockwise(bool IsCounterClockwise);
		bool						GetFrontIsCounterClockwise() const;

		void						SetToDefault();

									ZEGRRasterizerState();
		virtual						~ZEGRRasterizerState();
};
