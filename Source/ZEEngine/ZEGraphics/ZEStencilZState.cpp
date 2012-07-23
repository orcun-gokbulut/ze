//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEStencilZState.cpp
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

#include "ZEStencilZState.h"

void ZEStencilZState::SetZTestEnable(bool Enable)
{
	ZTestEnable = Enable;
	Changed = true;
}

bool ZEStencilZState::GetZTestEnable() const
{
	return ZTestEnable;
}

void ZEStencilZState::SetStencilTestEnable(bool Enable)
{
	StencilTestEnable = Enable;
	Changed = true;
}

bool ZEStencilZState::GetStencilTestEnable() const
{
	return StencilTestEnable;
}

void ZEStencilZState::SetZWriteEnable(bool Enable)
{
	ZWriteEnable = Enable;
	Changed = true;
}

bool ZEStencilZState::GetZWriteEnable() const
{
	return ZWriteEnable;
}

void ZEStencilZState::SetStencilWriteMask(unsigned char Mask)
{
	StencilWriteMask = Mask;
	Changed = true;
}

unsigned char ZEStencilZState::GetStencilWriteMask() const
{
	return StencilWriteMask;
}

void ZEStencilZState::SetZFunction(ZEComparisonFunction Function)
{
	ZFunction = Function;
	Changed = true;
}

ZEComparisonFunction ZEStencilZState::GetZFunction() const
{
	return ZFunction;
}

void ZEStencilZState::SetFrontStencilFail(ZEStencilOperation Operation)
{
	FrontStencilFailOperation = Operation;
	Changed = true;
}

ZEStencilOperation ZEStencilZState::GetFrontStencilFail() const
{
	return FrontStencilFailOperation;
}

void ZEStencilZState::SetFrontZFail(ZEStencilOperation Operation)
{
	FrontZFailOperation = Operation;
	Changed = true;
}

ZEStencilOperation ZEStencilZState::GetFrontZFail() const
{
	return FrontZFailOperation;
}

void ZEStencilZState::SetFrontStencilPass(ZEStencilOperation Operation)
{
	FrontStencilPassOperation = Operation;
	Changed = true;
}

ZEStencilOperation ZEStencilZState::GetFrontStencilPass() const
{
	return FrontStencilPassOperation;
}

void ZEStencilZState::SetFrontStencilFunction(ZEComparisonFunction Function)
{
	FrontStencilFunction = Function;
	Changed = true;
}

ZEComparisonFunction ZEStencilZState::GetFrontStencilFunction() const
{
	return FrontStencilFunction;
}

void ZEStencilZState::SetBackStencilFail(ZEStencilOperation Operation)
{
	BackStencilFailOperation = Operation;
	Changed = true;
}

ZEStencilOperation ZEStencilZState::GetBackStencilFail() const
{
	return BackStencilFailOperation;
}

void ZEStencilZState::SetBackZFail(ZEStencilOperation Operation)
{
	BackZFailOperation = Operation;
	Changed = true;
}

ZEStencilOperation ZEStencilZState::GetBackZFail() const
{
	return BackZFailOperation;
}

void ZEStencilZState::SetBackStencilPass(ZEStencilOperation Operation)
{
	BackStencilPassOperation = Operation;
	Changed = true;
}

ZEStencilOperation ZEStencilZState::GetBackStencilPass() const
{
	return BackStencilPassOperation;
}

void ZEStencilZState::SetBackStencilFunction(ZEComparisonFunction Function)
{
	BackStencilFunction = Function;
	Changed = true;
}

ZEComparisonFunction ZEStencilZState::GetBackStencilFunction() const
{
	return BackStencilFunction;
}

void ZEStencilZState::SetChanged(bool Change)
{
	Changed = Change;
}

bool ZEStencilZState::GetChanged() const
{
	return Changed;
}

const ZEStencilZState& ZEStencilZState::operator=(const ZEStencilZState& State)
{
	ZTestEnable = State.ZTestEnable;
	StencilTestEnable = State.StencilTestEnable;
	ZWriteEnable = State.ZWriteEnable;
	StencilWriteMask = State.StencilWriteMask;
	ZFunction = State.ZFunction;

	FrontStencilFailOperation = State.FrontStencilFailOperation;
	FrontZFailOperation = State.FrontZFailOperation;
	FrontStencilPassOperation = State.FrontStencilPassOperation;
	FrontStencilFunction = State.FrontStencilFunction;

	BackStencilFailOperation = State.BackStencilFailOperation;
	BackZFailOperation = State.BackZFailOperation;
	BackStencilPassOperation = State.BackStencilPassOperation;
	BackStencilFunction = State.BackStencilFunction;

	Changed = true;

	return *this;
}

ZEStencilZState::ZEStencilZState() :	ZTestEnable(false),
										StencilTestEnable(false),
										ZWriteEnable(false),
										StencilWriteMask(0xFF),
										ZFunction(ZE_CF_CURRENT),
										FrontStencilFailOperation(ZE_SO_CURRENT),
										FrontZFailOperation(ZE_SO_CURRENT),
										FrontStencilPassOperation(ZE_SO_CURRENT),
										FrontStencilFunction(ZE_CF_CURRENT),
										BackStencilFailOperation(ZE_SO_CURRENT),
										BackZFailOperation(ZE_SO_CURRENT),
										BackStencilPassOperation(ZE_SO_CURRENT),
										BackStencilFunction(ZE_CF_CURRENT),
										Changed(false)
{

}

ZEStencilZState::~ZEStencilZState()
{

}
