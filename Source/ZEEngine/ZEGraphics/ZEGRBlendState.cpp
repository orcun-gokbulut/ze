//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRBlendState.cpp
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

#include "ZEGRBlendState.h"

#include <memory.h>

void ZEGRBlendState::SetAlphaToCoverageEnable(bool Enable)
{
	if (StateData.AlphaToCoverageEnable != Enable)
	{
		StateData.AlphaToCoverageEnable = Enable;
		MarkDirty();
	}
}

bool ZEGRBlendState::GetAlphaToCoverageEnable() const
{
	return StateData.AlphaToCoverageEnable;
}

void ZEGRBlendState::SetBlendEnable(ZEUInt Target, bool Enable)
{
	zeDebugCheck(Target >= ZEGR_MAX_RENDER_TARGET_SLOT, "Index out of range");

	if (StateData.BlendEnable[(ZESize)Target] != Enable)
	{
		StateData.BlendEnable[(ZESize)Target] = Enable;
		MarkDirty();
	}
}

bool ZEGRBlendState::GetBlendEnable(ZEUInt Target) const
{
	zeDebugCheck(Target >= ZEGR_MAX_RENDER_TARGET_SLOT, "Index out of range");

	return StateData.BlendEnable[(ZESize)Target];
}
		
void ZEGRBlendState::SetSourceBlendOption(ZEGRBlendOption Option)
{
	if (StateData.SourceBlendOption != Option)
	{
		StateData.SourceBlendOption = Option;
		MarkDirty();
	}
}

ZEGRBlendOption ZEGRBlendState::GetSourceBlendOption() const
{
	return StateData.SourceBlendOption;
}

void ZEGRBlendState::SetDestinationBlendOption(ZEGRBlendOption Option)
{
	if (StateData.DestinationBlendOption != Option)
	{
		StateData.DestinationBlendOption = Option;
		MarkDirty();
	}
}

ZEGRBlendOption ZEGRBlendState::GetDestinationBlendOption() const
{
	return StateData.DestinationBlendOption;
}

void ZEGRBlendState::SetBlendEquation(ZEGRBlendEquation Equation)
{
	if (StateData.BlendEquation != Equation)
	{
		StateData.BlendEquation = Equation;
		MarkDirty();
	}
}

ZEGRBlendEquation ZEGRBlendState::GetBlendEquation() const
{
	return StateData.BlendEquation;
}

void ZEGRBlendState::SetSourceBlendAlphaOption(ZEGRBlendOption Option)
{
	if (StateData.SourceBlendAlphaOption != Option)
	{
		StateData.SourceBlendAlphaOption = Option;
		MarkDirty();
	}
}

ZEGRBlendOption ZEGRBlendState::GetSourceBlendAlphaOption() const
{
	return StateData.SourceBlendAlphaOption;
}
		
void ZEGRBlendState::SetDestinationBlendAlphaOption(ZEGRBlendOption Option)
{
	if (StateData.DestinationBlendAlphaOption != Option)
	{
		StateData.DestinationBlendAlphaOption = Option;
		MarkDirty();
	}
}

ZEGRBlendOption ZEGRBlendState::GetDestinationBlendAlphaOption() const
{
	return StateData.DestinationBlendAlphaOption;
}		

void ZEGRBlendState::SetBlendAlphaEquation(ZEGRBlendEquation Equation)
{
	if (StateData.BlendAlphaEquation != Equation)
	{
		StateData.BlendAlphaEquation= Equation;
		MarkDirty();
	}
}

ZEGRBlendEquation ZEGRBlendState::GetBlendAlphaEquation() const
{
	return StateData.BlendAlphaEquation;
}

void ZEGRBlendState::SetComponentWriteMask(ZEUInt Target, ZEGRColorWriteMask Mask)
{
	zeDebugCheck(Target >= ZEGR_MAX_RENDER_TARGET_SLOT, "Index out of range");
	
	if (StateData.ComponentWriteMask[(ZESize)Target] != Mask)
	{
		StateData.ComponentWriteMask[(ZESize)Target] = Mask;
		MarkDirty();
	}
}

ZEGRColorWriteMask ZEGRBlendState::GetComponentWriteMask(ZEUInt Target) const
{
	zeDebugCheck(Target >= ZEGR_MAX_RENDER_TARGET_SLOT, "Index out of range");

	return StateData.ComponentWriteMask[(ZESize)Target];
}

void ZEGRBlendState::SetToDefault()
{
	memset(&StateData, 0, sizeof(ZEBlendStateData));
	
	StateData.AlphaToCoverageEnable = false;
	memset(StateData.BlendEnable, 0, sizeof(bool) * 8);
	StateData.SourceBlendOption = ZEGR_BO_ONE;
	StateData.DestinationBlendOption = ZEGR_BO_ZERO;
	StateData.BlendEquation = ZEGR_BE_ADD;
	StateData.SourceBlendAlphaOption = ZEGR_BO_ONE;
	StateData.DestinationBlendAlphaOption = ZEGR_BO_ZERO;
	StateData.BlendAlphaEquation = ZEGR_BE_ADD;
	memset(StateData.ComponentWriteMask, ZEGR_CM_ALL, sizeof(ZEGRColorMask) * 8);
}

ZEGRBlendState::ZEGRBlendState()					
{
	SetToDefault();
}

ZEGRBlendState::~ZEGRBlendState()
{

}
