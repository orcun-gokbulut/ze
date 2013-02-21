//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDepthStencilState.cpp
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

#include "ZEDepthStencilState.h"
#include "ZEDS/ZEHashGenerator.h"

#include <string.h>

void ZEDepthStencilState::UpdateHash()
{
	if (Dirty)
	{
		Hash = 0;
		Dirty = false;
		ZEHashGenerator::Hash(Hash, &StateData, sizeof(ZEDepthStencilStateData));
	}
}

void ZEDepthStencilState::SetZTestEnable(bool Enable)
{
	if (StateData.ZTestEnable != Enable)
	{
		StateData.ZTestEnable = Enable;
		Dirty = true;
	}
}

bool ZEDepthStencilState::GetZTestEnable() const
{
	return StateData.ZTestEnable;
}

void ZEDepthStencilState::SetStencilTestEnable(bool Enable)
{
	if (StateData.StencilTestEnable != Enable)
	{
		StateData.StencilTestEnable = Enable;
		Dirty = true;
	}
}

bool ZEDepthStencilState::GetStencilTestEnable() const
{
	return StateData.StencilTestEnable;
}

void ZEDepthStencilState::SetZWriteEnable(bool Enable)
{
	if (StateData.ZWriteEnable != Enable)
	{
		StateData.ZWriteEnable = Enable;
		Dirty = true;
	}
}

bool ZEDepthStencilState::GetZWriteEnable() const
{
	return StateData.ZWriteEnable;
}

void ZEDepthStencilState::SetStencilWriteMask(ZEUInt8 Mask)
{
	if (StateData.StencilWriteMask != Mask)
	{
		StateData.StencilWriteMask = Mask;
		Dirty = true;
	}
}

ZEUInt8 ZEDepthStencilState::GetStencilWriteMask() const
{
	return StateData.StencilWriteMask;
}

void ZEDepthStencilState::SetStencilReadMask(ZEUInt8 Mask)
{
	if (StateData.StencilReadMask != Mask)
	{
		StateData.StencilReadMask = Mask;
		Dirty = true;
	}
}

ZEUInt8 ZEDepthStencilState::GetStencilReadMask() const
{
	return StateData.StencilReadMask;
}

void ZEDepthStencilState::SetZFunction(ZEComparisonFunction Function)
{
	if (StateData.ZFunction != Function)
	{
		StateData.ZFunction = Function;
		Dirty = true;
	}
}

ZEComparisonFunction ZEDepthStencilState::GetZFunction() const
{
	return StateData.ZFunction;
}

void ZEDepthStencilState::SetFrontStencilFail(ZEStencilOperation Operation)
{
	if (StateData.FrontStencilFailOperation != Operation)
	{
		StateData.FrontStencilFailOperation = Operation;
		Dirty = true;
	}
}

ZEStencilOperation ZEDepthStencilState::GetFrontStencilFail() const
{
	return StateData.FrontStencilFailOperation;
}

void ZEDepthStencilState::SetFrontZFail(ZEStencilOperation Operation)
{
	if (StateData.FrontZFailOperation != Operation)
	{
		StateData.FrontZFailOperation = Operation;
		Dirty = true;
	}
}

ZEStencilOperation ZEDepthStencilState::GetFrontZFail() const
{
	return StateData.FrontZFailOperation;
}

void ZEDepthStencilState::SetFrontStencilPass(ZEStencilOperation Operation)
{
	if (StateData.FrontStencilPassOperation != Operation)
	{
		StateData.FrontStencilPassOperation = Operation;
		Dirty = true;
	}
}

ZEStencilOperation ZEDepthStencilState::GetFrontStencilPass() const
{
	return StateData.FrontStencilPassOperation;
}

void ZEDepthStencilState::SetFrontStencilFunction(ZEComparisonFunction Function)
{
	if (StateData.FrontStencilFunction != Function)
	{
		StateData.FrontStencilFunction = Function;
		Dirty = true;
	}
}

ZEComparisonFunction ZEDepthStencilState::GetFrontStencilFunction() const
{
	return StateData.FrontStencilFunction;
}

void ZEDepthStencilState::SetBackStencilFail(ZEStencilOperation Operation)
{
	if (StateData.BackStencilFailOperation != Operation)
	{
		StateData.BackStencilFailOperation = Operation;
		Dirty = true;
	}
}

ZEStencilOperation ZEDepthStencilState::GetBackStencilFail() const
{
	return StateData.BackStencilFailOperation;
}

void ZEDepthStencilState::SetBackZFail(ZEStencilOperation Operation)
{
	if (StateData.BackZFailOperation != Operation)
	{
		StateData.BackZFailOperation = Operation;
		Dirty = true;
	}
}

ZEStencilOperation ZEDepthStencilState::GetBackZFail() const
{
	return StateData.BackZFailOperation;
}

void ZEDepthStencilState::SetBackStencilPass(ZEStencilOperation Operation)
{
	if (StateData.BackStencilPassOperation != Operation)
	{
		StateData.BackStencilPassOperation = Operation;
		Dirty = true;
	}
}

ZEStencilOperation ZEDepthStencilState::GetBackStencilPass() const
{
	return StateData.BackStencilPassOperation;
}

void ZEDepthStencilState::SetBackStencilFunction(ZEComparisonFunction Function)
{
	if (StateData.BackStencilFunction != Function)
	{
		StateData.BackStencilFunction = Function;
		Dirty = true;
	}
}

ZEComparisonFunction ZEDepthStencilState::GetBackStencilFunction() const
{
	return StateData.BackStencilFunction;
}

void ZEDepthStencilState::SetToDefault()
{
	Hash = 0;
	Dirty = false;

	StateData.ZTestEnable = true;
	StateData.StencilTestEnable = false;
	StateData.ZWriteEnable = false;
	StateData.StencilWriteMask = 0xFF;
	StateData.StencilReadMask = 0xFF;
	StateData.ZFunction = ZE_CF_LESS;
	StateData.FrontStencilFailOperation = ZE_SO_KEEP;
	StateData.FrontZFailOperation = ZE_SO_KEEP;
	StateData.FrontStencilPassOperation = ZE_SO_KEEP;
	StateData.FrontStencilFunction = ZE_CF_ALWAYS;
	StateData.BackStencilFailOperation = ZE_SO_KEEP;
	StateData.BackZFailOperation = ZE_SO_KEEP;
	StateData.BackStencilPassOperation = ZE_SO_KEEP;
	StateData.BackStencilFunction = ZE_CF_ALWAYS;

	UpdateHash();
}

const ZEDepthStencilState& ZEDepthStencilState::operator=(const ZEDepthStencilState& State)
{
	Hash = State.Hash;
	Dirty = State.Dirty;
	memcpy(&StateData, &State.StateData, sizeof(ZEDepthStencilStateData));
	return *this;
}

bool ZEDepthStencilState::operator==(const ZEDepthStencilState& State)
{
	return memcmp(&StateData, &State.StateData, sizeof(ZEDepthStencilStateData)) == 0 ? true : false;
}

bool ZEDepthStencilState::operator!=(const ZEDepthStencilState& State)
{
	return !operator==(State);
}

ZEDepthStencilState::ZEDepthStencilState()
{
	SetToDefault();
}

ZEDepthStencilState::~ZEDepthStencilState()
{

}
