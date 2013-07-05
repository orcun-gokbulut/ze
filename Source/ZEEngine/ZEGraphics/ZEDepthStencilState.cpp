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

#include "ZEStatePool.h"
#include "ZEFoundation/ZELog.h"
#include "ZEDepthStencilState.h"
#include "ZEDS/ZEHashGenerator.h"

#include <string.h>

ZESize ZEDepthStencilState::GetHash()
{
	if (Dirty)
	{
		Hash = ZEHashGenerator::Hash(&StateData, sizeof(ZEDepthStencilStateData));;
		Dirty = false;
	}
	return Hash;
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

#include "ZEFoundation/ZELog.h"
#include "ZEFoundation/ZEError.h"

void ZEDepthStencilState::DebugPrint() const
{
	static const char* ComparisonFunctionStrings[] =
	{		
		"NULL",
		"ZE_CF_NEVER",
		"ZE_CF_LESS",
		"ZE_CF_EQUAL",
		"ZE_CF_LESS_EQUAL",
		"ZE_CF_GREATER",
		"ZE_CF_NOT_EQUAL",
		"ZE_CF_GREATER_EQUAL",
		"ZE_CF_ALWAYS"
	};

	static const char* StencilOperationStrings[] = 
	{ 
		"NULL",
		"ZE_SO_KEEP",
		"ZE_SO_ZERO",
		"ZE_SO_REPLACE",
		"ZE_SO_INCR_SAT",
		"ZE_SO_DECR_SAT",
		"ZE_SO_INVERT",
		"ZE_SO_INCR",
		"ZE_SO_DECR",
	};

	ZEString ZTestEnable = StateData.ZTestEnable ? "ZTestEnable: TRUE" : "ZTestEnable: FALSE";
	ZEString ZWriteEnable = StateData.ZWriteEnable ? "ZWriteEnable: TRUE" : "ZWriteEnable: FALSE";
	ZEString ZFunction = ZEString("ZFunction: ") + ComparisonFunctionStrings[StateData.ZFunction];
	ZEString StencilTestEnable = StateData.StencilTestEnable ? "StencilTestEnable: TRUE" : "StencilTestEnable: FALSE";
	ZEString StencilWriteMask = ZEString("StencilWriteMask: ") + ZEString::FromUInt8(StateData.StencilWriteMask);
	ZEString StencilReadMask = ZEString("StencilReadMask: ") + ZEString::FromUInt8(StateData.StencilReadMask);
	ZEString FrontStencilFailOperation = ZEString("FrontStencilFailOperation: ") + StencilOperationStrings[StateData.FrontStencilFailOperation];
	ZEString FrontZFailOperation = ZEString("FrontZFailOperation: ") + StencilOperationStrings[StateData.FrontZFailOperation];
	ZEString FrontStencilPassOperation = ZEString("FrontStencilPassOperation: ") + StencilOperationStrings[StateData.FrontStencilPassOperation];
	ZEString FrontStencilFunction = ZEString("FrontStencilFunction: ") + ComparisonFunctionStrings[StateData.FrontStencilFunction];
	ZEString BackStencilFailOperation = ZEString("BackStencilFailOperation: ") + StencilOperationStrings[StateData.BackStencilFailOperation];
	ZEString BackZFailOperation = ZEString("BackZFailOperation: ") + StencilOperationStrings[StateData.BackZFailOperation];
	ZEString BackStencilPassOperation = ZEString("BackStencilPassOperation: ") + StencilOperationStrings[StateData.BackStencilPassOperation];
	ZEString BackStencilFunction = ZEString("BackStencilFunction: ") + ComparisonFunctionStrings[StateData.BackStencilFunction];

	zeLog(ZTestEnable.ToCString());
	zeLog(ZWriteEnable.ToCString());
	zeLog(ZFunction.ToCString());
	zeLog(StencilTestEnable.ToCString());
	zeLog(StencilWriteMask.ToCString());
	zeLog(StencilReadMask.ToCString());
	zeLog(FrontStencilFailOperation.ToCString());
	zeLog(FrontZFailOperation.ToCString());
	zeLog(FrontStencilPassOperation.ToCString());
	zeLog(FrontStencilFunction.ToCString());
	zeLog(BackStencilFailOperation.ToCString());
	zeLog(BackZFailOperation.ToCString());
	zeLog(BackStencilPassOperation.ToCString());
	zeLog(BackStencilFunction.ToCString());
}

void ZEDepthStencilState::SetToDefault()
{
	Hash = 0;
	Dirty = false;

	memset(&StateData, 0, sizeof(ZEDepthStencilStateData));
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
