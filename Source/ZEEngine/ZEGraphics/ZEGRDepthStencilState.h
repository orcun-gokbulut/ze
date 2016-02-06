//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRDepthStencilState.h
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

enum ZEGRStencilOperation 
{ 
	ZEGR_SO_KEEP				= 0,
	ZEGR_SO_ZERO				= 1,
	ZEGR_SO_REPLACE				= 2,
	ZEGR_SO_INCREASE_SATURATE	= 3,
	ZEGR_SO_DECREASE_SATURATE	= 4,
	ZEGR_SO_INVERT				= 5,
	ZEGR_SO_INCREASE			= 6,
	ZEGR_SO_DECREASE			= 7 
};

class ZEGRDepthStencilState : public ZEGRState
{
	private:
		struct ZEDepthStencilStateData
		{
			bool						DepthTestEnable : 1;
			bool						DepthWriteEnable : 1;
			ZEGRComparisonFunction		DepthFunction : 5;

			bool						StencilTestEnable : 1;
			ZEUInt8						StencilWriteMask : 8;
			ZEUInt8						StencilReadMask : 8;

			ZEGRComparisonFunction		FrontStencilFunction : 5;
			ZEGRStencilOperation		FrontStencilPassOperation : 5;
			ZEGRStencilOperation		FrontStencilFailOperation : 5;
			ZEGRStencilOperation		FrontStencilDepthFailOperation : 5;

			ZEGRComparisonFunction		BackStencilFunction : 5;
			ZEGRStencilOperation		BackStencilPassOperation : 5;
			ZEGRStencilOperation		BackStencilFailOperation : 5;
			ZEGRStencilOperation		BackStencilDepthFailOperation : 5;
		} StateData;
	
	public:
		virtual ZEGRStateType			GetStateType() const;
		virtual const void*				GetData() const;
		virtual ZESize					GetDataSize() const;

		void							SetDepthTestEnable(bool Enable);
		bool							GetDepthTestEnable() const;
	
		void							SetDepthWriteEnable(bool Enable);
		bool							GetDepthWriteEnable() const;

		void							SetDepthFunction(ZEGRComparisonFunction Function);
		ZEGRComparisonFunction			GetDepthFunction() const;

		void							SetStencilTestEnable(bool Enable);
		bool							GetStencilTestEnable() const;

		void							SetStencilWriteMask(ZEUInt8 Mask);
		ZEUInt8							GetStencilWriteMask() const;
	
		void							SetStencilReadMask(ZEUInt8 Mask);
		ZEUInt8							GetStencilReadMask() const;

		void							SetFrontStencilFunction(ZEGRComparisonFunction Function);
		ZEGRComparisonFunction			GetFrontStencilFunction() const;

		void							SetFrontStencilPass(ZEGRStencilOperation Operation);
		ZEGRStencilOperation			GetFrontStencilPass() const;

		void							SetFrontStencilFail(ZEGRStencilOperation Operation);
		ZEGRStencilOperation			GetFrontStencilFail() const;
	
		void							SetFrontStencilDepthFail(ZEGRStencilOperation Operation);
		ZEGRStencilOperation			GetFrontStencilDepthFail() const;

		void							SetBackStencilFunction(ZEGRComparisonFunction Function);		
		ZEGRComparisonFunction			GetBackStencilFunction() const;

		void							SetBackStencilPass(ZEGRStencilOperation Operation);
		ZEGRStencilOperation			GetBackStencilPass() const;

		void							SetBackStencilFail(ZEGRStencilOperation Operation);
		ZEGRStencilOperation			GetBackStencilFail() const;

		void							SetBackZFail(ZEGRStencilOperation Operation);
		ZEGRStencilOperation			GetBackZFail() const;
	
		void							SetToDefault();

										ZEGRDepthStencilState();
		virtual							~ZEGRDepthStencilState();
};
