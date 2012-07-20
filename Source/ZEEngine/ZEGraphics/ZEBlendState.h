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

#ifndef __ZE_BLEND_STATE_H__
#define __ZE_BLEND_STATE_H__

#include "ZEFoundation/ZEDS/ZEFlags.h"

enum ZEBlendOption 
{	
	// In OpenGL 'ONE_MINUS' is the equivalent of 'INV'
	ZE_BO_CURRENT				= 0,
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
//	ZE_BO_SRC1_COLOR			= 16,
//	ZE_BO_INV_SRC1_COLOR		= 17,
//	ZE_BO_SRC1_ALPHA			= 18,
//	ZE_BO_INV_SRC1_ALPHA		= 19 
};

enum ZEBlendEquation 
{ 
	ZE_BE_CURRENT			= 0, 
	ZE_BE_ADD				= 1,
	ZE_BE_SUBTRACT			= 2,
	ZE_BE_REV_SUBTRACT		= 3,
	ZE_BE_MIN				= 4,
	ZE_BE_MAX				= 5 
};

typedef ZEFlags ZEColorChannelMask;
#define	ZE_CCM_RED		0x1
#define	ZE_CCM_GREEN	0x2
#define	ZE_CCM_BLUE		0x4
#define	ZE_CCM_ALPHA	0x8

class ZEBlendState
{
protected:
	bool						BlendEnable;
	ZEBlendOption				SourceColorBlendOption;
	ZEBlendOption				DestinationColorBlendOption;
	ZEBlendOption				SourceAlphaBlendOption;
	ZEBlendOption				DestinationAlphaBlendOption;
	ZEBlendEquation				BlendEquation;
	ZEColorChannelMask			ColorChannelMask;

	bool						Changed;

public:
	void						SetBlendEnable(bool Enable);	
	bool						GetBlendEnable() const;
	void						SetSourceColorBlendOption(ZEBlendOption Option);
	ZEBlendOption				GetSourceColorBlendOption() const;
	void						SetDestinationColorBlendOption(ZEBlendOption Option);
	ZEBlendOption				GetDestinationColorBlendOption() const;
	void						SetSourceAlphaBlendOption(ZEBlendOption Option);
	ZEBlendOption				GetSourceAlphaBlendOption() const;
	void						SetDestinationAlphaBlendOption(ZEBlendOption Option);
	ZEBlendOption				GetDestinationAlphaBlendOption() const;
	void						SetBlendEquation(ZEBlendEquation Equation);
	ZEBlendEquation				GetBlendEquation() const;
	void						SetColorChannelMask(ZEUInt Mask);
	ZEUInt						GetColorChannelMask() const;
	void						SetChanged(bool Change);
	bool						GetChanged() const;

	const ZEBlendState&			operator=(const ZEBlendState& State);

								ZEBlendState();
	virtual						~ZEBlendState();

};
#endif
