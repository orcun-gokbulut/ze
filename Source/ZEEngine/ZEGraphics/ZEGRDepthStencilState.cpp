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

ZEGRStateType ZEGRDepthStencilState::GetStateType() const
{
	return ZEGRStateType::ZEGR_ST_DEPTH_STENCIL;
}

const void* ZEGRDepthStencilState::GetData() const
{
	return &StateData;
}

ZESize ZEGRDepthStencilState::GetDataSize() const
{
	return sizeof(StateData);
}

void ZEGRDepthStencilState::SetDepthTestEnable(bool Enable)
{
	if (StateData.DepthTestEnable == Enable)
		return;

	StateData.DepthTestEnable = Enable;
	MarkDirty();
}

bool ZEGRDepthStencilState::GetDepthTestEnable() const
{
	return StateData.DepthTestEnable;
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

void ZEGRDepthStencilState::SetDepthWriteEnable(bool Enable)
{
	if (StateData.DepthWriteEnable == Enable)
		return;

	StateData.DepthWriteEnable = Enable;
	MarkDirty();
}

bool ZEGRDepthStencilState::GetDepthWriteEnable() const
{
	return StateData.DepthWriteEnable;
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

void ZEGRDepthStencilState::SetDepthFunction(ZEGRComparisonFunction Function)
{
	if (StateData.DepthFunction == Function)
		return;

	StateData.DepthFunction = Function;
	MarkDirty();
}

ZEGRComparisonFunction ZEGRDepthStencilState::GetDepthFunction() const
{
	return StateData.DepthFunction;
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

void ZEGRDepthStencilState::SetFrontStencilDepthFail(ZEGRStencilOperation Operation)
{
	if (StateData.FrontStencilDepthFailOperation == Operation)
		return;

	StateData.FrontStencilDepthFailOperation = Operation;
	MarkDirty();
}

ZEGRStencilOperation ZEGRDepthStencilState::GetFrontStencilDepthFail() const
{
	return StateData.FrontStencilDepthFailOperation;
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
	if (StateData.BackStencilDepthFailOperation == Operation)
		return;

	StateData.BackStencilDepthFailOperation = Operation;
	MarkDirty();
}

ZEGRStencilOperation ZEGRDepthStencilState::GetBackZFail() const
{
	return StateData.BackStencilDepthFailOperation;
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

	StateData.DepthTestEnable = true;
	StateData.DepthWriteEnable = true;
	StateData.DepthFunction = ZEGR_CF_LESS;

	StateData.StencilTestEnable = false;
	StateData.StencilWriteMask = 0xFF;
	StateData.StencilReadMask = 0xFF;

	StateData.FrontStencilFunction = ZEGR_CF_ALWAYS;
	StateData.FrontStencilPassOperation = ZEGR_SO_KEEP;
	StateData.FrontStencilFailOperation = ZEGR_SO_KEEP;
	StateData.FrontStencilDepthFailOperation = ZEGR_SO_KEEP;

	StateData.BackStencilFunction = ZEGR_CF_ALWAYS;
	StateData.BackStencilPassOperation = ZEGR_SO_KEEP;
	StateData.BackStencilFailOperation = ZEGR_SO_KEEP;
	StateData.BackStencilDepthFailOperation = ZEGR_SO_KEEP;
}

ZEGRDepthStencilState::ZEGRDepthStencilState()
{
	SetToDefault();
}

ZEGRDepthStencilState::~ZEGRDepthStencilState()
{

}
