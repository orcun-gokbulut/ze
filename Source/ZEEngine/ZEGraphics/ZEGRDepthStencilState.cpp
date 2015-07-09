//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRDepthStencilState.cpp
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

#include "ZEGRDepthStencilState.h"

#include <memory.h>

const void* ZEGRDepthStencilState::GetData() const
{
	return &StateData;
}

ZESize ZEGRDepthStencilState::GetDataSize() const
{
	return sizeof(StateData);
}

void ZEGRDepthStencilState::SetZTestEnable(bool Enable)
{
	if (StateData.ZTestEnable == Enable)
		return;

	StateData.ZTestEnable = Enable;
	MarkDirty();
}

bool ZEGRDepthStencilState::GetZTestEnable() const
{
	return StateData.ZTestEnable;
}

void ZEGRDepthStencilState::SetStencilTestEnable(bool Enable)
{
	if (StateData.StencilTestEnable == Enable)
		return;

	StateData.StencilTestEnable = Enable;
	MarkDirty();
}

bool ZEGRDepthStencilState::GetStencilTestEnable() const
{
	return StateData.StencilTestEnable;
}

void ZEGRDepthStencilState::SetZWriteEnable(bool Enable)
{
	if (StateData.ZWriteEnable == Enable)
		return;

	StateData.ZWriteEnable = Enable;
	MarkDirty();
}

bool ZEGRDepthStencilState::GetZWriteEnable() const
{
	return StateData.ZWriteEnable;
}

void ZEGRDepthStencilState::SetStencilWriteMask(ZEUInt8 Mask)
{
	if (StateData.StencilWriteMask == Mask)
		return;

	StateData.StencilWriteMask = Mask;
	MarkDirty();
}

ZEUInt8 ZEGRDepthStencilState::GetStencilWriteMask() const
{
	return StateData.StencilWriteMask;
}

void ZEGRDepthStencilState::SetStencilReadMask(ZEUInt8 Mask)
{
	if (StateData.StencilReadMask == Mask)
		return;

	StateData.StencilReadMask = Mask;
	MarkDirty();
}

ZEUInt8 ZEGRDepthStencilState::GetStencilReadMask() const
{
	return StateData.StencilReadMask;
}

void ZEGRDepthStencilState::SetZFunction(ZEGRComparisonFunction Function)
{
	if (StateData.ZFunction == Function)
		return;

	StateData.ZFunction = Function;
	MarkDirty();
}

ZEGRComparisonFunction ZEGRDepthStencilState::GetZFunction() const
{
	return StateData.ZFunction;
}

void ZEGRDepthStencilState::SetFrontStencilFail(ZEGRStencilOperation Operation)
{
	if (StateData.FrontStencilFailOperation == Operation)
		return;

	StateData.FrontStencilFailOperation = Operation;
	MarkDirty();
}

ZEGRStencilOperation ZEGRDepthStencilState::GetFrontStencilFail() const
{
	return StateData.FrontStencilFailOperation;
}

void ZEGRDepthStencilState::SetFrontZFail(ZEGRStencilOperation Operation)
{
	if (StateData.FrontZFailOperation == Operation)
		return;

	StateData.FrontZFailOperation = Operation;
	MarkDirty();
}

ZEGRStencilOperation ZEGRDepthStencilState::GetFrontZFail() const
{
	return StateData.FrontZFailOperation;
}

void ZEGRDepthStencilState::SetFrontStencilPass(ZEGRStencilOperation Operation)
{
	if (StateData.FrontStencilPassOperation == Operation)
		return;

	StateData.FrontStencilPassOperation = Operation;
	MarkDirty();
}

ZEGRStencilOperation ZEGRDepthStencilState::GetFrontStencilPass() const
{
	return StateData.FrontStencilPassOperation;
}

void ZEGRDepthStencilState::SetFrontStencilFunction(ZEGRComparisonFunction Function)
{
	if (StateData.FrontStencilFunction == Function)
		return;

	StateData.FrontStencilFunction = Function;
	MarkDirty();
}

ZEGRComparisonFunction ZEGRDepthStencilState::GetFrontStencilFunction() const
{
	return StateData.FrontStencilFunction;
}

void ZEGRDepthStencilState::SetBackStencilFail(ZEGRStencilOperation Operation)
{
	if (StateData.BackStencilFailOperation == Operation)
		return;

	StateData.BackStencilFailOperation = Operation;
	MarkDirty();
}

ZEGRStencilOperation ZEGRDepthStencilState::GetBackStencilFail() const
{
	return StateData.BackStencilFailOperation;
}

void ZEGRDepthStencilState::SetBackZFail(ZEGRStencilOperation Operation)
{
	if (StateData.BackZFailOperation == Operation)
		return;

	StateData.BackZFailOperation = Operation;
	MarkDirty();
}

ZEGRStencilOperation ZEGRDepthStencilState::GetBackZFail() const
{
	return StateData.BackZFailOperation;
}

void ZEGRDepthStencilState::SetBackStencilPass(ZEGRStencilOperation Operation)
{
	if (StateData.BackStencilPassOperation == Operation)
		return;

	StateData.BackStencilPassOperation = Operation;
	MarkDirty();
}

ZEGRStencilOperation ZEGRDepthStencilState::GetBackStencilPass() const
{
	return StateData.BackStencilPassOperation;
}

void ZEGRDepthStencilState::SetBackStencilFunction(ZEGRComparisonFunction Function)
{
	if (StateData.BackStencilFunction == Function)
		return;

	StateData.BackStencilFunction = Function;
	MarkDirty();
}

ZEGRComparisonFunction ZEGRDepthStencilState::GetBackStencilFunction() const
{
	return StateData.BackStencilFunction;
}

void ZEGRDepthStencilState::SetToDefault()
{
	memset(&StateData, 0, sizeof(ZEDepthStencilStateData));
	StateData.ZTestEnable = true;
	StateData.StencilTestEnable = false;
	StateData.ZWriteEnable = false;
	StateData.StencilWriteMask = 0xFF;
	StateData.StencilReadMask = 0xFF;
	StateData.ZFunction = ZEGR_CF_LESS;
	StateData.FrontStencilFailOperation = ZEGR_SO_KEEP;
	StateData.FrontZFailOperation = ZEGR_SO_KEEP;
	StateData.FrontStencilPassOperation = ZEGR_SO_KEEP;
	StateData.FrontStencilFunction = ZEGR_CF_ALWAYS;
	StateData.BackStencilFailOperation = ZEGR_SO_KEEP;
	StateData.BackZFailOperation = ZEGR_SO_KEEP;
	StateData.BackStencilPassOperation = ZEGR_SO_KEEP;
	StateData.BackStencilFunction = ZEGR_CF_ALWAYS;
}

ZEGRDepthStencilState::ZEGRDepthStencilState()
{
	SetToDefault();
}

ZEGRDepthStencilState::~ZEGRDepthStencilState()
{

}
