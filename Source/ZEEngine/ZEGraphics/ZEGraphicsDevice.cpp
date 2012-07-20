//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGraphicsDevice.cpp
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

#include "ZEGraphicsDevice.h"


void ZEGraphicsDevice::SetBlendState(const ZEBlendState &NewBlendState)
{
	RequestedBlendState = NewBlendState;
}

ZEBlendState& ZEGraphicsDevice::GetBlendStateEdit()
{
	return RequestedBlendState;
}

void ZEGraphicsDevice::SetRasterizerState(const ZERasterizerState &NewRasterizerState)
{
	RequestedRasterizerState = NewRasterizerState;
}

ZERasterizerState& ZEGraphicsDevice::GetRasterizerStateEdit()
{
	return RequestedRasterizerState;
}

void ZEGraphicsDevice::SetSamplerState(int index, const ZESamplerState &NewSamplerState)
{
	RequestedSamplerStates[index] = NewSamplerState;
}

ZESamplerState& ZEGraphicsDevice::GetSamplerStateEdit(int index)
{
	return RequestedSamplerStates[index];
}

void ZEGraphicsDevice::SetStencilZState(ZEStencilZState NewStencilZState)
{
	RequestedStencilZState = NewStencilZState;
}

ZEStencilZState& ZEGraphicsDevice::GetStencilZStateEdit()
{
	return RequestedStencilZState;
}

void ZEGraphicsDevice::SetVertexDecleration(ZEVertexDeclaration* Declaration)
{
	RequestedVertexDeclaration = Declaration;
}

ZEVertexDeclaration* ZEGraphicsDevice::GetVertexDeclaration()
{
	return RequestedVertexDeclaration;
}

void ZEGraphicsDevice::SetVertexShader(ZEShader* VertexShader)
{
	RequestedVertexShader = VertexShader;
}

ZEShader* ZEGraphicsDevice::GetVertexShader()
{
	return RequestedVertexShader;
}

void ZEGraphicsDevice::SetPixelShader(ZEShader* PixelShader)
{
	RequestedVertexShader = PixelShader;
}

ZEShader* ZEGraphicsDevice::GetPixelShader()
{
	return RequestedPixelShader;
}

void ZEGraphicsDevice::SetRenderTarget(int index, ZERenderTarget* RenderTarget)
{
	RequestedRenderTargets[index] = RenderTarget;
}

ZERenderTarget* ZEGraphicsDevice::GetRenderTarget(int index)
{
	return RequestedRenderTargets[index];
}

void ZEGraphicsDevice::CommitRequestedState()
{
	ApplyAllRequestedStates();
}

ZEGraphicsDevice::ZEGraphicsDevice()
{

}

ZEGraphicsDevice::~ZEGraphicsDevice()
{

}
