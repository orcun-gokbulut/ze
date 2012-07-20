//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEBlendState.cpp
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

#include "ZEBlendState.h"

void ZEBlendState::SetBlendEnable(bool Enable)
{
	BlendEnable = Enable;
	Changed = true;
}

bool ZEBlendState::GetBlendEnable() const
{
	return BlendEnable;
}

void ZEBlendState::SetSourceColorBlendOption(ZEBlendOption Option)
{
	SourceColorBlendOption = Option;
	Changed = true;
}

ZEBlendOption ZEBlendState::GetSourceColorBlendOption() const
{
	return SourceColorBlendOption;
}

void ZEBlendState::SetDestinationColorBlendOption(ZEBlendOption Option)
{
	DestinationColorBlendOption = Option;
	Changed = true;
}

ZEBlendOption ZEBlendState::GetDestinationColorBlendOption() const
{
	return DestinationColorBlendOption;
}

void ZEBlendState::SetSourceAlphaBlendOption(ZEBlendOption Option)
{
	SourceAlphaBlendOption = Option;
	Changed = true;
}

ZEBlendOption ZEBlendState::GetSourceAlphaBlendOption() const
{
	return SourceAlphaBlendOption;
}

void ZEBlendState::SetDestinationAlphaBlendOption(ZEBlendOption Option)
{
	DestinationAlphaBlendOption = Option;
	Changed = true;
}

ZEBlendOption ZEBlendState::GetDestinationAlphaBlendOption() const
{
	return DestinationAlphaBlendOption;
}

void ZEBlendState::SetBlendEquation(ZEBlendEquation Equation)
{
	BlendEquation = Equation;
	Changed = true;
}

ZEBlendEquation ZEBlendState::GetBlendEquation() const
{
	return BlendEquation;
}

void ZEBlendState::SetColorChannelMask(ZEUInt Mask)
{
	ColorChannelMask.Value = Mask;
	Changed = true;
}

ZEUInt ZEBlendState::GetColorChannelMask() const
{
	return ColorChannelMask.Value;
}

void ZEBlendState::SetChanged(bool Change)
{
	Changed = Change;
}

bool ZEBlendState::GetChanged() const
{
	return Changed;
}

const ZEBlendState& ZEBlendState::operator=(const ZEBlendState& State)
{
	BlendEnable = State.BlendEnable;
	SourceColorBlendOption = State.SourceColorBlendOption;
	DestinationColorBlendOption = State.DestinationColorBlendOption;
	SourceAlphaBlendOption = State.SourceAlphaBlendOption;
	DestinationAlphaBlendOption = State.DestinationAlphaBlendOption;
	BlendEquation = State.BlendEquation;
	ColorChannelMask = State.ColorChannelMask;
	Changed = State.Changed;
	return *this;
}

ZEBlendState::ZEBlendState() :	BlendEnable(false),
								SourceColorBlendOption(ZE_BO_CURRENT),
								DestinationColorBlendOption(ZE_BO_CURRENT),
								SourceAlphaBlendOption(ZE_BO_CURRENT),
								DestinationAlphaBlendOption(ZE_BO_CURRENT),
								BlendEquation(ZE_BE_CURRENT),
								ColorChannelMask(ZE_CCM_RED | ZE_CCM_GREEN | ZE_CCM_BLUE | ZE_CCM_ALPHA),
								Changed(false)
{

}

ZEBlendState::~ZEBlendState()
{

}
