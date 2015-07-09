//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRBlendState.h
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
#pragma  once

#include "ZEGRState.h"

#include "ZETypes.h"
#include "ZEGRDefinitions.h"

enum ZEGRBlendOption 
{	
	ZEGR_BO_ZERO				= 1,
	ZEGR_BO_ONE					= 2,
	ZEGR_BO_SRC_COLOR			= 3,
	ZEGR_BO_INV_SRC_COLOR		= 4,
	ZEGR_BO_SRC_ALPHA			= 5,
	ZEGR_BO_INV_SRC_ALPHA		= 6,
	ZEGR_BO_DEST_ALPHA			= 7,
	ZEGR_BO_INV_DEST_ALPHA		= 8,
	ZEGR_BO_DEST_COLOR			= 9,
	ZEGR_BO_INV_DEST_COLOR		= 10,
	ZEGR_BO_SRC_ALPHA_SAT		= 11,
	ZEGR_BO_BLEND_FACTOR		= 12,
	ZEGR_BO_INV_BLEND_FACTOR	= 13,
	ZEGR_BO_SRC1_COLOR			= 14,
	ZEGR_BO_INV_SRC1_COLOR		= 15,
	ZEGR_BO_SRC1_ALPHA			= 16,
	ZEGR_BO_INV_SRC1_ALPHA		= 17
};

enum ZEGRBlendEquation 
{ 
	ZEGR_BE_ADD					= 1,
	ZEGR_BE_SUBTRACT			= 2,
	ZEGR_BE_REV_SUBTRACT		= 3,
	ZEGR_BE_MIN					= 4,
	ZEGR_BE_MAX					= 5 
};

typedef ZEGRColorMask ZEGRColorWriteMask;

class ZEGRBlendState : public ZEGRState
{
	private:
		struct ZEBlendStateData
		{
			bool					AlphaToCoverageEnable : 2;
			ZEGRBlendOption			SourceBlendOption : 6;
			ZEGRBlendOption			DestinationBlendOption : 6;
			ZEGRBlendEquation		BlendEquation : 4;
			ZEGRBlendOption			SourceBlendAlphaOption : 6;
			ZEGRBlendOption			DestinationBlendAlphaOption : 6;
			ZEGRBlendEquation		BlendAlphaEquation : 4;
			bool					BlendEnable[ZEGR_MAX_RENDER_TARGET_SLOT];
			ZEGRColorWriteMask		ComponentWriteMask[ZEGR_MAX_RENDER_TARGET_SLOT];

		} StateData;
		
	public:
		virtual const void*			GetData() const;
		virtual ZESize				GetDataSize() const;

		void						SetAlphaToCoverageEnable(bool Enable);
		bool						GetAlphaToCoverageEnable() const;

		void						SetBlendEnable(ZEUInt RenderTarget, bool Enable);
		bool						GetBlendEnable(ZEUInt RenderTarget) const;
		
		void						SetSourceBlendOption(ZEGRBlendOption Option);
		ZEGRBlendOption				GetSourceBlendOption() const;
		
		void						SetDestinationBlendOption(ZEGRBlendOption Option);
		ZEGRBlendOption				GetDestinationBlendOption() const;

		void						SetBlendEquation(ZEGRBlendEquation Equation);
		ZEGRBlendEquation			GetBlendEquation() const;

		void						SetSourceBlendAlphaOption(ZEGRBlendOption Option);
		ZEGRBlendOption				GetSourceBlendAlphaOption() const;
		
		void						SetDestinationBlendAlphaOption(ZEGRBlendOption Option);
		ZEGRBlendOption				GetDestinationBlendAlphaOption() const;
		
		void						SetBlendAlphaEquation(ZEGRBlendEquation Equation);
		ZEGRBlendEquation			GetBlendAlphaEquation() const;

		void						SetComponentWriteMask(ZEUInt RenderTarget, ZEGRColorWriteMask Mask);
		ZEGRColorWriteMask			GetComponentWriteMask(ZEUInt RenderTarget) const;

		void						SetToDefault();

									ZEGRBlendState();
									~ZEGRBlendState();
};
