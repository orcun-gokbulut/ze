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
#include "ZEDS/ZEHashGenerator.h"

#include <memory.h>
#include "ZEMath/ZEVector.h"

void ZEBlendState::UpdateHash()
{
	if (Dirty)
	{
		Hash = 0;
		Dirty = false;
		ZEHashGenerator::Hash(Hash, &StateData, sizeof(ZEBlendStateData));
	}
}

void ZEBlendState::SetAlphaToCoverageEnable(bool Enable)
{
	if (StateData.AlphaToCoverageEnable != Enable)
	{
		StateData.AlphaToCoverageEnable = Enable;
		Dirty = true;
	}
}

bool ZEBlendState::GetAlphaToCoverageEnable() const
{
	return StateData.AlphaToCoverageEnable;
}

void ZEBlendState::SetBlendEnable(ZESize Target, bool Enable)
{
	zeDebugCheck(Target >= ZE_MAX_RENDER_TARGET_SLOT, "Index out of range");

	if (StateData.BlendEnable[Target] != Enable)
	{
		StateData.BlendEnable[Target] = Enable;
		Dirty = true;
	}
}

bool ZEBlendState::GetBlendEnable(ZESize Target) const
{
	zeDebugCheck(Target >= ZE_MAX_RENDER_TARGET_SLOT, "Index out of range");

	return StateData.BlendEnable[Target];
}
		
void ZEBlendState::SetSourceBlendOption(ZEBlendOption Option)
{
	if (StateData.SourceBlendOption != Option)
	{
		StateData.SourceBlendOption = Option;
		Dirty = true;
	}
}

ZEBlendOption ZEBlendState::GetSourceBlendOption() const
{
	return StateData.SourceBlendOption;
}

void ZEBlendState::SetDestinationBlendOption(ZEBlendOption Option)
{
	if (StateData.DestinationBlendOption != Option)
	{
		StateData.DestinationBlendOption = Option;
		Dirty = true;
	}
}

ZEBlendOption ZEBlendState::GetDestinationBlendOption() const
{
	return StateData.DestinationBlendOption;
}

void ZEBlendState::SetBlendEquation(ZEBlendEquation Equation)
{
	if (StateData.BlendEquation != Equation)
	{
		StateData.BlendEquation = Equation;
		Dirty = true;
	}
}

ZEBlendEquation ZEBlendState::GetBlendEquation() const
{
	return StateData.BlendEquation;
}

void ZEBlendState::SetSourceBlendAlphaOption(ZEBlendOption Option)
{
	if (StateData.SourceBlendAlphaOption != Option)
	{
		StateData.SourceBlendAlphaOption = Option;
		Dirty = true;
	}
}

ZEBlendOption ZEBlendState::GetSourceBlendAlphaOption() const
{
	return StateData.SourceBlendAlphaOption;
}
		
void ZEBlendState::SetDestinationBlendAlphaOption(ZEBlendOption Option)
{
	if (StateData.DestinationBlendAlphaOption != Option)
	{
		StateData.DestinationBlendAlphaOption = Option;
		Dirty = true;
	}
}

ZEBlendOption ZEBlendState::GetDestinationBlendAlphaOption() const
{
	return StateData.DestinationBlendAlphaOption;
}		

void ZEBlendState::SetBlendAlphaEquation(ZEBlendEquation Equation)
{
	if (StateData.BlendAlphaEquation != Equation)
	{
		StateData.BlendAlphaEquation= Equation;
		Dirty = true;
	}
}

ZEBlendEquation ZEBlendState::GetBlendAlphaEquation() const
{
	return StateData.BlendAlphaEquation;
}

void ZEBlendState::SetComponentWriteMask(ZESize Target, ZEComponentMask Mask)
{
	zeDebugCheck(Target >= ZE_MAX_RENDER_TARGET_SLOT, "Index out of range");
	
	if (StateData.ComponentWriteMask[Target] != Mask)
	{
		StateData.ComponentWriteMask[Target] = Mask;
		Dirty = true;
	}
}

ZEComponentMask ZEBlendState::GetComponentWriteMask(ZESize Target) const
{
	zeDebugCheck(Target >= ZE_MAX_RENDER_TARGET_SLOT, "Index out of range");

	return StateData.ComponentWriteMask[Target];
}

void ZEBlendState::SetToDefault()
{
	Hash = 0;
	Dirty = false;
	
	StateData.AlphaToCoverageEnable = false;
	memset(StateData.BlendEnable, 0, sizeof(bool) * 8);
	StateData.SourceBlendOption = ZE_BO_ONE;
	StateData.DestinationBlendOption = ZE_BO_ZERO;
	StateData.BlendEquation = ZE_BE_ADD;
	StateData.SourceBlendAlphaOption = ZE_BO_ONE;
	StateData.DestinationBlendAlphaOption = ZE_BO_ZERO;
	StateData.BlendAlphaEquation = ZE_BE_ADD;
	memset(StateData.ComponentWriteMask, ZE_CM_ALL, sizeof(ZEComponentMask) * 8);
}

const ZEBlendState& ZEBlendState::operator=(const ZEBlendState& State)
{
	Hash = State.Hash;
	Dirty = State.Dirty;

	memcpy(&StateData, &State.StateData, sizeof(ZEBlendStateData));
	return *this;
}

bool ZEBlendState::operator==(const ZEBlendState& State)
{
	return memcmp(&StateData, &State.StateData, sizeof(ZEBlendStateData)) == 0 ? true : false;
}

bool ZEBlendState::operator!=(const ZEBlendState& State)
{
	return !operator==(State);
}

ZEBlendState::ZEBlendState()					
{
	SetToDefault();
}

ZEBlendState::~ZEBlendState()
{

}
