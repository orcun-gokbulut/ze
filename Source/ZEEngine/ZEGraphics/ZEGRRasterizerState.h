//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRRasterizerState.h
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

enum ZEGRPrimitiveType
{
	ZEGR_PT_NONE			= 0,
	ZEGR_PT_POINT_LIST		= 1,
	ZEGR_PT_LINE_LIST		= 2,
	ZEGR_PT_TRIANGLE_LIST	= 3,
	ZEGR_PT_TRIANGLE_STRIPT	= 4
};

enum ZEGRFillMode
{
	ZEGR_FM_SOLID			= 0,
	ZEGR_FM_WIREFRAME		= 1,
};

enum ZEGRCullDirection
{
	ZEGR_CD_NONE				= 0,
	ZEGR_CD_CLOCKWISE 			= 1,
	ZEGR_CD_COUNTER_CLOCKWISE	= 2
};

class ZEGRRasterizerState : public ZEGRState
{
	private:
		struct ZERasterizerStateData	
		{
			ZEGRFillMode			FillMode : 2;
			ZEGRCullDirection		CullDirection : 3;
			bool					FrontIsCounterClockwise : 1;	
		} StateData;

	public:
		virtual const void*			GetData() const;
		virtual ZESize				GetDataSize() const;

		void						SetFillMode(ZEGRFillMode Mode);
		ZEGRFillMode				GetFillMode() const;
	
		void						SetCullDirection(ZEGRCullDirection Direction);
		ZEGRCullDirection			GetCullDirection() const;
	
		void						SetFrontIsCounterClockwise(bool IsCounterClockwise);
		bool						GetFrontIsCounterClockwise() const;

		void						SetToDefault();

									ZEGRRasterizerState();
		virtual						~ZEGRRasterizerState();
};
