//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEBlendState.h
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

#ifndef __ZE_DEVICE_STATE_BLEND_H__
#define __ZE_DEVICE_STATE_BLEND_H__

#include "ZETypes.h"
#include "ZEFoundation/ZEDS/ZEFlags.h"


#define ZE_MAX_RENDER_TARGET_SLOT		8

enum ZEBlendOption 
{	
	ZE_BO_ZERO					= 1,
	ZE_BO_ONE					= 2,
	ZE_BO_SRC_COLOR				= 3,
	ZE_BO_INV_SRC_COLOR			= 4,
	ZE_BO_SRC_ALPHA				= 5,
	ZE_BO_INV_SRC_ALPHA			= 6,
	ZE_BO_DEST_ALPHA			= 7,
	ZE_BO_INV_DEST_ALPHA		= 8,
	ZE_BO_DEST_COLOR			= 9,
	ZE_BO_INV_DEST_COLOR		= 10,
	ZE_BO_SRC_ALPHA_SAT			= 11,
	ZE_BO_BLEND_FACTOR			= 12,
	ZE_BO_INV_BLEND_FACTOR		= 13,
	ZE_BO_SRC1_COLOR			= 14,
	ZE_BO_INV_SRC1_COLOR		= 15,
	ZE_BO_SRC1_ALPHA			= 16,
	ZE_BO_INV_SRC1_ALPHA		= 17
};

enum ZEBlendEquation 
{ 
	ZE_BE_ADD					= 1,
	ZE_BE_SUBTRACT				= 2,
	ZE_BE_REV_SUBTRACT			= 3,
	ZE_BE_MIN					= 4,
	ZE_BE_MAX					= 5 
};


#define ZE_CCM_NONE				0x0
#define	ZE_CCM_RED				0x1
#define	ZE_CCM_GREEN			0x2
#define	ZE_CCM_BLUE				0x4
#define	ZE_CCM_ALPHA			0x8
#define ZE_CCM_ALL				ZE_CCM_RED | ZE_CCM_GREEN | ZE_CCM_BLUE | ZE_CCM_ALPHA
typedef ZEFlagsBase<ZEUInt8>	ZEColorChannelMask;

class ZEBlendState
{
	friend class ZEGraphicsDevice;
	friend class ZEDeviceStageOutput;

	// Should be public for only internal usage
	public:
		ZEUInt64					Hash;
		bool						Dirty;
		
		struct ZEBlendStateData
		{
			bool					AlphaToCoverageEnable;
			ZEBlendOption			SourceBlendOption;
			ZEBlendOption			DestinationBlendOption;
			ZEBlendEquation			BlendEquation;
			ZEBlendOption			SourceBlendAlphaOption;
			ZEBlendOption			DestinationBlendAlphaOption;
			ZEBlendEquation			BlendAlphaEquation;
			bool					BlendEnable[ZE_MAX_RENDER_TARGET_SLOT];
			ZEColorChannelMask		ColorChannelMask[ZE_MAX_RENDER_TARGET_SLOT];

		} StateData;

		void						UpdateHash();
		
	public:
		void						SetAlphaToCoverageEnable(bool Enable);
		bool						GetAlphaToCoverageEnable() const;

		void						SetBlendEnable(ZESize Target, bool Enable);
		bool						GetBlendEnable(ZESize Target) const;
		
		void						SetSourceBlendOption(ZEBlendOption Option);
		ZEBlendOption				GetSourceBlendOption() const;
		
		void						SetDestinationBlendOption(ZEBlendOption Option);
		ZEBlendOption				GetDestinationBlendOption() const;

		void						SetBlendEquation(ZEBlendEquation Equation);
		ZEBlendEquation				GetBlendEquation() const;

		void						SetSourceBlendAlphaOption(ZEBlendOption Option);
		ZEBlendOption				GetSourceBlendAlphaOption() const;
		
		void						SetDestinationBlendAlphaOption(ZEBlendOption Option);
		ZEBlendOption				GetDestinationBlendAlphaOption() const;
		
		void						SetBlendAlphaEquation(ZEBlendEquation Equation);
		ZEBlendEquation				GetBlendAlphaEquation() const;

		void						SetColorChannelMask(ZESize Target, ZEColorChannelMask Mask);
		ZEColorChannelMask			GetColorChannelMask(ZESize Target) const;

		void						SetToDefault();
		
		const ZEBlendState&	operator=(const ZEBlendState& State);
		bool						operator==(const ZEBlendState& State);
		bool						operator!=(const ZEBlendState& State);

									ZEBlendState();
		virtual						~ZEBlendState();

};
#endif
