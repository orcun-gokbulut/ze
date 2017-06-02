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
#include <stddef.h>

void ZEGRBlendRenderTarget::SetBlendEnable(bool Enabled)
{
	StateData.BlendEnable = Enabled;
}

bool ZEGRBlendRenderTarget::GetBlendEnable() const
{
	return StateData.BlendEnable;
}

void ZEGRBlendRenderTarget::SetOperation(ZEGRBlendOperation Equation)
{
	StateData.Operation = Equation;
}

ZEGRBlendOperation ZEGRBlendRenderTarget::GetOperation() const
{
	return StateData.Operation;
}

void ZEGRBlendRenderTarget::SetSource(ZEGRBlend Blend)
{
	StateData.Source = Blend;
}

ZEGRBlend ZEGRBlendRenderTarget::GetSource() const
{
	return StateData.Source;
}

void ZEGRBlendRenderTarget::SetDestination(ZEGRBlend Blend)
{
	StateData.Destination = Blend;
}

ZEGRBlend ZEGRBlendRenderTarget::GetDestination() const
{
	return StateData.Destination;
}

void ZEGRBlendRenderTarget::SetAlphaOperation(ZEGRBlendOperation Equation)
{
	StateData.AlphaOperation = Equation;
}

ZEGRBlendOperation ZEGRBlendRenderTarget::GetAlphaOperation() const
{
	return StateData.AlphaOperation;
}

void ZEGRBlendRenderTarget::SetSourceAlpha(ZEGRBlend Blend)
{
	StateData.SourceAlpha = Blend;
}

ZEGRBlend ZEGRBlendRenderTarget::GetSourceAlpha() const
{
	return StateData.SourceAlpha;
}
		
void ZEGRBlendRenderTarget::SetDestinationAlpha(ZEGRBlend Blend)
{
	StateData.DestinationAlpha = Blend;
}

ZEGRBlend ZEGRBlendRenderTarget::GetDestinationAlpha() const
{
	return StateData.DestinationAlpha;
}		

void ZEGRBlendRenderTarget::SetWriteMask(ZEGRColorMask Mask)
{
	StateData.WriteMask = Mask;
}

ZEGRColorMask ZEGRBlendRenderTarget::GetWriteMask() const
{
	return StateData.WriteMask;
}

void ZEGRBlendRenderTarget::SetToDefault()
{
	memset(&StateData, 0, sizeof(ZEBlendStateData));
	StateData.BlendEnable = false;
	StateData.Operation = ZEGR_BE_ADD;
	StateData.Source = ZEGR_BO_ONE;
	StateData.Destination = ZEGR_BO_ZERO;
	StateData.AlphaOperation = ZEGR_BE_ADD;
	StateData.SourceAlpha = ZEGR_BO_ONE;
	StateData.DestinationAlpha = ZEGR_BO_ZERO;
	StateData.WriteMask = ZEGR_CM_ALL;
}

ZEGRBlendRenderTarget::ZEGRBlendRenderTarget()					
{
	SetToDefault();
}

ZEGRBlendRenderTarget::~ZEGRBlendRenderTarget()
{

}

ZEGRStateType ZEGRBlendState::GetStateType() const
{
	return ZEGRStateType::ZEGR_ST_BLEND;
}

const void* ZEGRBlendState::GetData() const
{
	return &StateData;
}

ZESize ZEGRBlendState::GetDataSize() const
{
	return sizeof(StateData);
}

void ZEGRBlendState::SetBlendEnable(bool Enable)
{
	if (StateData.BlendEnable == Enable)
		return;

	StateData.BlendEnable = Enable;
	MarkDirty();
}

bool ZEGRBlendState::GetBlendEnable() const
{
	return StateData.BlendEnable;
}


void ZEGRBlendState::SetIndividualBlendEnable(bool Enable)
{
	if (StateData.IndividualBlendEnable == Enable)
		return;

	StateData.IndividualBlendEnable = Enable;
	MarkDirty();
}

bool ZEGRBlendState::GetIndividualBlendEnable() const
{
	return StateData.IndividualBlendEnable;
}

void ZEGRBlendState::SetAlphaToCoverageEnable(bool Enable)
{
	StateData.AlphaToCoverageEnable = true;
}

bool ZEGRBlendState::GetAlphaToCoverageEnable() const
{
	return StateData.AlphaToCoverageEnable;
}

void ZEGRBlendState::SetRenderTargetBlend(ZEUInt Index, const ZEGRBlendRenderTarget& RenderTargetState)
{
	zeCheckError(Index >= ZEGR_MAX_RENDER_TARGET_SLOT, ZE_VOID, "Index out of range.");

	if (memcmp(&StateData.RenderTargets[Index].StateData, &RenderTargetState.StateData, sizeof(RenderTargetState.StateData)) == 0)
		return;

	StateData.RenderTargets[Index] = RenderTargetState;
	MarkDirty();
}

const ZEGRBlendRenderTarget& ZEGRBlendState::GetRenderTarget(ZEUInt Index) const
{
	return StateData.RenderTargets[Index];
}

void ZEGRBlendState::SetToDefault()
{
	memset(&StateData, 0, sizeof(ZEGRBlendStateData));

	for (int I = 0; I < ZEGR_MAX_RENDER_TARGET_SLOT; I++)
	{	
		StateData.RenderTargets[I].SetToDefault();
	}
}

ZEGRBlendState::ZEGRBlendState()					
{
	SetToDefault();
}

ZEGRBlendState::~ZEGRBlendState()
{

}
