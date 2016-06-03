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

enum ZEGRBlend 
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
	ZEGR_BO_BLEND_FACTOR		= 14,
	ZEGR_BO_INV_BLEND_FACTOR	= 15,
	ZEGR_BO_SRC1_COLOR			= 16,
	ZEGR_BO_INV_SRC1_COLOR		= 17,
	ZEGR_BO_SRC1_ALPHA			= 18,
	ZEGR_BO_INV_SRC1_ALPHA		= 19
};

enum ZEGRBlendOperation 
{
	ZEGR_BE_ADD					= 1,
	ZEGR_BE_SUBTRACT			= 2,
	ZEGR_BE_REV_SUBTRACT		= 3,
	ZEGR_BE_MIN					= 4,
	ZEGR_BE_MAX					= 5 
};

class ZEGRBlendRenderTarget
{
	friend class ZEGRBlendState;
	private:
		struct ZEBlendStateData
		{
			bool						BlendEnable;
			ZEGRBlendOperation			Operation;
			ZEGRBlend					Source;
			ZEGRBlend					Destination;
			ZEGRBlendOperation			AlphaOperation;
			ZEGRBlend					SourceAlpha;
			ZEGRBlend					DestinationAlpha;
			ZEGRColorMask				WriteMask;
		} StateData;

	public:
		void							SetBlendEnable(bool Enabled);
		bool							GetBlendEnable() const;

		void							SetOperation(ZEGRBlendOperation Equation);
		ZEGRBlendOperation				GetOperation() const;

		void							SetSource(ZEGRBlend Option);
		ZEGRBlend						GetSource() const;

		void							SetDestination(ZEGRBlend Option);
		ZEGRBlend						GetDestination() const;

		void							SetAlphaOperation(ZEGRBlendOperation Equation);
		ZEGRBlendOperation				GetAlphaOperation() const;

		void							SetSourceAlpha(ZEGRBlend Option);
		ZEGRBlend						GetSourceAlpha() const;

		void							SetDestinationAlpha(ZEGRBlend Option);
		ZEGRBlend						GetDestinationAlpha() const;

		void							SetWriteMask(ZEGRColorMask Mask);
		ZEGRColorMask					GetWriteMask() const;

		void							SetToDefault();

										ZEGRBlendRenderTarget();
										~ZEGRBlendRenderTarget();
};


class ZEGRBlendState : public ZEGRState
{
	private:
		struct ZEGRBlendStateData
		{
			bool						BlendEnable;
			bool						IndividualBlendEnable;
			bool						AlphaToCoverageEnable;
			ZEGRBlendRenderTarget		RenderTargets[ZEGR_MAX_RENDER_TARGET_SLOT];
		} StateData;
		
	public:
		virtual ZEGRStateType			GetStateType() const;
		virtual const void*				GetData() const;
		virtual ZESize					GetDataSize() const;

		void							SetBlendEnable(bool Enable);
		bool							GetBlendEnable() const;

		void							SetIndividualBlendEnable(bool Enable);
		bool							GetIndividualBlendEnable() const;

		void							SetAlphaToCoverageEnable(bool Enable);
		bool							GetAlphaToCoverageEnable() const;

		void							SetRenderTargetBlend(ZEUInt Index, const ZEGRBlendRenderTarget& RenderTargetState);
		const ZEGRBlendRenderTarget&	GetRenderTarget(ZEUInt Index) const;

		void							SetToDefault();

										ZEGRBlendState();
		virtual							~ZEGRBlendState();
};
