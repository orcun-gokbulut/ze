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

void ZEDeviceStateDepthStencil::UpdateHash()
{
	if (Dirty)
	{
		Hash = 0;
		Dirty = false;
		ZEHashGenerator::Hash(Hash, &StateData, sizeof(ZEDepthStencilStateData));
	}
}

void ZEDeviceStateDepthStencil::SetZTestEnable(bool Enable)
{
	if (StateData.ZTestEnable != Enable)
	{
		StateData.ZTestEnable = Enable;
		Dirty = true;
	}
}

bool ZEDeviceStateDepthStencil::GetZTestEnable() const
{
	return StateData.ZTestEnable;
}

void ZEDeviceStateDepthStencil::SetStencilTestEnable(bool Enable)
{
	if (StateData.StencilTestEnable != Enable)
	{
		StateData.StencilTestEnable = Enable;
		Dirty = true;
	}
}

bool ZEDeviceStateDepthStencil::GetStencilTestEnable() const
{
	return StateData.StencilTestEnable;
}

void ZEDeviceStateDepthStencil::SetZWriteEnable(bool Enable)
{
	if (StateData.ZWriteEnable != Enable)
	{
		StateData.ZWriteEnable = Enable;
		Dirty = true;
	}
}

bool ZEDeviceStateDepthStencil::GetZWriteEnable() const
{
	return StateData.ZWriteEnable;
}

void ZEDeviceStateDepthStencil::SetStencilWriteMask(ZEUInt8 Mask)
{
	if (StateData.StencilWriteMask != Mask)
	{
		StateData.StencilWriteMask = Mask;
		Dirty = true;
	}
}

ZEUInt8 ZEDeviceStateDepthStencil::GetStencilWriteMask() const
{
	return StateData.StencilWriteMask;
}

void ZEDeviceStateDepthStencil::SetStencilReadMask(ZEUInt8 Mask)
{
	if (StateData.StencilReadMask != Mask)
	{
		StateData.StencilReadMask = Mask;
		Dirty = true;
	}
}

ZEUInt8 ZEDeviceStateDepthStencil::GetStencilReadMask() const
{
	return StateData.StencilReadMask;
}

void ZEDeviceStateDepthStencil::SetStencilReferance(ZEUInt32 Value)
{
	if (StateData.StencilReferance != Value)
	{
		StateData.StencilReferance = Value;
		Dirty = true;
	}
}

ZEUInt32 ZEDeviceStateDepthStencil::GetStencilReferance() const
{
	return StateData.StencilReferance;
}

void ZEDeviceStateDepthStencil::SetZFunction(ZEComparisonFunction Function)
{
	if (StateData.ZFunction != Function)
	{
		StateData.ZFunction = Function;
		Dirty = true;
	}
}

ZEComparisonFunction ZEDeviceStateDepthStencil::GetZFunction() const
{
	return StateData.ZFunction;
}

void ZEDeviceStateDepthStencil::SetFrontStencilFail(ZEStencilOperation Operation)
{
	if (StateData.FrontStencilFailOperation != Operation)
	{
		StateData.FrontStencilFailOperation = Operation;
		Dirty = true;
	}
}

ZEStencilOperation ZEDeviceStateDepthStencil::GetFrontStencilFail() const
{
	return StateData.FrontStencilFailOperation;
}

void ZEDeviceStateDepthStencil::SetFrontZFail(ZEStencilOperation Operation)
{
	if (StateData.FrontZFailOperation != Operation)
	{
		StateData.FrontZFailOperation = Operation;
		Dirty = true;
	}
}

ZEStencilOperation ZEDeviceStateDepthStencil::GetFrontZFail() const
{
	return StateData.FrontZFailOperation;
}

void ZEDeviceStateDepthStencil::SetFrontStencilPass(ZEStencilOperation Operation)
{
	if (StateData.FrontStencilPassOperation != Operation)
	{
		StateData.FrontStencilPassOperation = Operation;
		Dirty = true;
	}
}

ZEStencilOperation ZEDeviceStateDepthStencil::GetFrontStencilPass() const
{
	return StateData.FrontStencilPassOperation;
}

void ZEDeviceStateDepthStencil::SetFrontStencilFunction(ZEComparisonFunction Function)
{
	if (StateData.FrontStencilFunction != Function)
	{
		StateData.FrontStencilFunction = Function;
		Dirty = true;
	}
}

ZEComparisonFunction ZEDeviceStateDepthStencil::GetFrontStencilFunction() const
{
	return StateData.FrontStencilFunction;
}

void ZEDeviceStateDepthStencil::SetBackStencilFail(ZEStencilOperation Operation)
{
	if (StateData.BackStencilFailOperation != Operation)
	{
		StateData.BackStencilFailOperation = Operation;
		Dirty = true;
	}
}

ZEStencilOperation ZEDeviceStateDepthStencil::GetBackStencilFail() const
{
	return StateData.BackStencilFailOperation;
}

void ZEDeviceStateDepthStencil::SetBackZFail(ZEStencilOperation Operation)
{
	if (StateData.BackZFailOperation != Operation)
	{
		StateData.BackZFailOperation = Operation;
		Dirty = true;
	}
}

ZEStencilOperation ZEDeviceStateDepthStencil::GetBackZFail() const
{
	return StateData.BackZFailOperation;
}

void ZEDeviceStateDepthStencil::SetBackStencilPass(ZEStencilOperation Operation)
{
	if (StateData.BackStencilPassOperation != Operation)
	{
		StateData.BackStencilPassOperation = Operation;
		Dirty = true;
	}
}

ZEStencilOperation ZEDeviceStateDepthStencil::GetBackStencilPass() const
{
	return StateData.BackStencilPassOperation;
}

void ZEDeviceStateDepthStencil::SetBackStencilFunction(ZEComparisonFunction Function)
{
	if (StateData.BackStencilFunction != Function)
	{
		StateData.BackStencilFunction = Function;
		Dirty = true;
	}
}

ZEComparisonFunction ZEDeviceStateDepthStencil::GetBackStencilFunction() const
{
	return StateData.BackStencilFunction;
}

void ZEDeviceStateDepthStencil::SetToDefault()
{
	Hash = 0;
	Dirty = false;

	StateData.ZTestEnable = true;
	StateData.StencilTestEnable = false;
	StateData.ZWriteEnable = false;
	StateData.StencilWriteMask = 0xFF;
	StateData.StencilReadMask = 0xFF;
	StateData.StencilReferance = 0;
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

const ZEDeviceStateDepthStencil& ZEDeviceStateDepthStencil::operator=(const ZEDeviceStateDepthStencil& State)
{
	Hash = State.Hash;
	Dirty = State.Dirty;
	memcpy(&StateData, &State.StateData, sizeof(ZEDepthStencilStateData));
	return *this;
}

bool ZEDeviceStateDepthStencil::operator==(const ZEDeviceStateDepthStencil& State)
{
	return memcmp(&StateData, &State.StateData, sizeof(ZEDepthStencilStateData)) == 0 ? true : false;
}

bool ZEDeviceStateDepthStencil::operator!=(const ZEDeviceStateDepthStencil& State)
{
	return !operator==(State);
}

ZEDeviceStateDepthStencil::ZEDeviceStateDepthStencil()
{
	memset(&StateData, 0, sizeof(ZEDepthStencilStateData));
	SetToDefault();
}

ZEDeviceStateDepthStencil::~ZEDeviceStateDepthStencil()
{

}
