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
	ZEGR_PT_NONE						= 0,
	ZEGR_PT_POINT_LIST					= 1,
	ZEGR_PT_LINE_LIST					= 2,
	ZEGR_PT_TRIANGLE_LIST				= 3,
	ZEGR_PT_TRIANGLE_STRIPT				= 4,
	ZEGR_PT_1_CONTROL_POINT_PATCHLIST	= 5,
	ZEGR_PT_2_CONTROL_POINT_PATCHLIST	= 6,
	ZEGR_PT_3_CONTROL_POINT_PATCHLIST	= 7,
	ZEGR_PT_4_CONTROL_POINT_PATCHLIST	= 8,
	ZEGR_PT_5_CONTROL_POINT_PATCHLIST	= 9,
	ZEGR_PT_6_CONTROL_POINT_PATCHLIST	= 10,
	ZEGR_PT_7_CONTROL_POINT_PATCHLIST	= 11,
	ZEGR_PT_8_CONTROL_POINT_PATCHLIST	= 12,
	ZEGR_PT_9_CONTROL_POINT_PATCHLIST	= 13,
	ZEGR_PT_10_CONTROL_POINT_PATCHLIST	= 14,
	ZEGR_PT_11_CONTROL_POINT_PATCHLIST	= 15,
	ZEGR_PT_12_CONTROL_POINT_PATCHLIST	= 16,
	ZEGR_PT_13_CONTROL_POINT_PATCHLIST	= 17,
	ZEGR_PT_14_CONTROL_POINT_PATCHLIST	= 18,
	ZEGR_PT_15_CONTROL_POINT_PATCHLIST	= 19,
	ZEGR_PT_16_CONTROL_POINT_PATCHLIST	= 20,
	ZEGR_PT_17_CONTROL_POINT_PATCHLIST	= 21,
	ZEGR_PT_18_CONTROL_POINT_PATCHLIST	= 22,
	ZEGR_PT_19_CONTROL_POINT_PATCHLIST	= 23,
	ZEGR_PT_20_CONTROL_POINT_PATCHLIST	= 24,
	ZEGR_PT_21_CONTROL_POINT_PATCHLIST	= 25,
	ZEGR_PT_22_CONTROL_POINT_PATCHLIST	= 26,
	ZEGR_PT_23_CONTROL_POINT_PATCHLIST	= 27,
	ZEGR_PT_24_CONTROL_POINT_PATCHLIST	= 28,
	ZEGR_PT_25_CONTROL_POINT_PATCHLIST	= 29,
	ZEGR_PT_26_CONTROL_POINT_PATCHLIST	= 30,
	ZEGR_PT_27_CONTROL_POINT_PATCHLIST	= 31,
	ZEGR_PT_28_CONTROL_POINT_PATCHLIST	= 32,
	ZEGR_PT_29_CONTROL_POINT_PATCHLIST	= 33,
	ZEGR_PT_30_CONTROL_POINT_PATCHLIST	= 34,
	ZEGR_PT_31_CONTROL_POINT_PATCHLIST	= 35,
	ZEGR_PT_32_CONTROL_POINT_PATCHLIST	= 36
};

enum ZEGRFillMode : ZEUInt8
{
	ZEGR_FM_SOLID						= 0,
	ZEGR_FM_WIREFRAME					= 1
};

enum ZEGRCullMode : ZEUInt8
{
	ZEGR_CMD_NONE						= 0,
	ZEGR_CMD_FRONT						= 1,
	ZEGR_CMD_BACK						= 2
};

class ZEGRRasterizerState : public ZEGRState
{
	private:
		struct ZERasterizerStateData	
		{
			ZEGRFillMode			FillMode;
			ZEGRCullMode			CullMode;
			bool					FrontIsCounterClockwise;
			bool					DepthClipEnable;
			ZEInt					DepthBias;
			float					DepthBiasClamp;
			float					SlopeScaledDepthBias;
		} StateData;

	public:
		virtual ZEGRStateType		GetStateType() const;
		virtual const void*			GetData() const;
		virtual ZESize				GetDataSize() const;

		void						SetFillMode(ZEGRFillMode Mode);
		ZEGRFillMode				GetFillMode() const;
	
		void						SetCullMode(ZEGRCullMode CullMode);
		ZEGRCullMode				GetCullMode() const;
	
		void						SetFrontIsCounterClockwise(bool IsCounterClockwise);
		bool						GetFrontIsCounterClockwise() const;

		void						SetDepthBias(ZEInt DepthBias);
		ZEInt						GetDepthBias() const;

		void						SetDepthBiasClamp(float DepthBiasClamp);
		float						GetDepthBiasClamp() const;

		void						SetSlopeScaledDepthBias(float SlopeScaledDepthBias);
		float						GetSlopeScaledDepthBias() const;

		void						SetDepthClipEnable(bool DepthClipEnable);
		bool						GetDepthClipEnable() const;

		void						SetToDefault();

									ZEGRRasterizerState();
		virtual						~ZEGRRasterizerState();
};
