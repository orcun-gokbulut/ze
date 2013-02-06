//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGraphicsDeviceState.cpp
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


#include "ZEGraphicsDeviceState.h"
#include "ZEMath/ZEMath.h"
#include "ZEDS/ZEHashGenerator.h"

/************************************************************************/
/*                         ZEViewport                                   */
/************************************************************************/
void ZEViewport::UpdateHash()
{
	Hash = 0;
	ZEHashGenerator::Hash(Hash, &ViewportData, sizeof(ZEViewportData));
}

void ZEViewport::SetZero()
{
	memset(&ViewportData, 0, sizeof(ZEViewportData));

// 	ViewportData.Width		= 0;
// 	ViewportData.Height		= 0;
// 	ViewportData.TopLeftX	= 0;
// 	ViewportData.TopLeftY	= 0;
// 	ViewportData.MinDepth	= 0.0f;
// 	ViewportData.MaxDepth	= 0.0f;
}

const ZEViewport& ZEViewport::operator =(const ZEViewport& Other)
{
	Hash = Other.Hash;
	memcpy(&ViewportData, &Other.ViewportData, sizeof(ZEViewportData));

// 	ViewportData.Width		= Other.ViewportData.Width;
// 	ViewportData.Height		= Other.ViewportData.Height;
// 	ViewportData.TopLeftX	= Other.ViewportData.TopLeftX;
// 	ViewportData.TopLeftY	= Other.ViewportData.TopLeftY;
// 	ViewportData.MinDepth	= Other.ViewportData.MinDepth;
// 	ViewportData.MaxDepth	= Other.ViewportData.MaxDepth;

	return *this;
}

bool ZEViewport::operator ==(const ZEViewport& Other)
{
	if (ViewportData.Width != Other.ViewportData.Width)
		return false;
	if (ViewportData.Height != Other.ViewportData.Height)
		return false;
	if (ViewportData.TopLeftX != Other.ViewportData.TopLeftX)
		return false;
	if (ViewportData.TopLeftY != Other.ViewportData.TopLeftY)
		return false;
	if (ZEMath::Abs(ViewportData.MinDepth - Other.ViewportData.MinDepth) > ZE_ZERO_THRESHOLD)
		return false;
	if (ZEMath::Abs(ViewportData.MaxDepth - Other.ViewportData.MaxDepth) > ZE_ZERO_THRESHOLD)
		return false;

	return true;
}

bool ZEViewport::operator !=(const ZEViewport& Other)
{
	return !operator==(Other);
}

ZEViewport::ZEViewport()
{
	Hash = 0;

	SetZero();
}

ZEViewport::~ZEViewport()
{

}

/************************************************************************/
/*                       ZEScissorRectangle                             */
/************************************************************************/
void ZEScissorRectangle::UpdateHash()
{
	Hash = 0;
	ZEHashGenerator::Hash(Hash, &ScissorRectangleData, sizeof(ZEScissorRectangleData));
}

void ZEScissorRectangle::SetZero()
{
	memset(&ScissorRectangleData, 0, sizeof(ZEScissorRectangleData));

// 	ScissorRectangleData.Bottom = 0;
// 	ScissorRectangleData.Left = 0;
// 	ScissorRectangleData.Right = 0;
// 	ScissorRectangleData.Top = 0;
}

const ZEScissorRectangle& ZEScissorRectangle::operator =(const ZEScissorRectangle& Other)
{
	Hash = Other.Hash;
	memcpy(&ScissorRectangleData, &Other.ScissorRectangleData, sizeof(ZEScissorRectangleData));

// 	ScissorRectangleData.Bottom = Other.ScissorRectangleData.Bottom;
// 	ScissorRectangleData.Left	= Other.ScissorRectangleData.Left;
// 	ScissorRectangleData.Right	= Other.ScissorRectangleData.Right;
// 	ScissorRectangleData.Top	= Other.ScissorRectangleData.Top;

	return *this;
}

bool ZEScissorRectangle::operator ==(const ZEScissorRectangle& Other)
{
	if (ScissorRectangleData.Bottom != Other.ScissorRectangleData.Bottom)
		return false;
	if (ScissorRectangleData.Right != Other.ScissorRectangleData.Right)
		return false;
	if (ScissorRectangleData.Left != Other.ScissorRectangleData.Left)
		return false;
	if (ScissorRectangleData.Top != Other.ScissorRectangleData.Top)
		return false;

	return true;
}

bool ZEScissorRectangle::operator !=(const ZEScissorRectangle& Other)
{
	return !operator==(Other);
}

ZEScissorRectangle::ZEScissorRectangle()
{
	Hash = 0;
	SetZero();
}

ZEScissorRectangle::~ZEScissorRectangle()
{

}

/************************************************************************/
/*                          ZEDeviceState                               */
/************************************************************************/

void ZEDeviceState::SetToDefault()
{
	IndexBuffer = NULL;
	VertexLayout.SetToDefault();
	
	for (ZESize I = 0; I < ZE_MAX_VERTEX_BUFFER_SLOT; ++I)
		VertexBuffers[I] = NULL;

	VertexShader = NULL;
	
	for (ZESize I = 0; I < ZE_MAX_BUFFER_SLOT; ++I)
	{
		VertexShaderBuffers[I] = NULL;
		GeometryShaderBuffers[I] = NULL;
		PixelShaderBuffers[I] = NULL;
	}
	
	for (ZESize I = 0; I < ZE_MAX_TEXTURE_SLOT; ++I)
	{
		VertexShaderTextures[I] = NULL;
		GeometryShaderTextures[I] = NULL;
		PixelShaderTextures[I] = NULL;
	}

	for (ZESize I = 0; I < ZE_MAX_SAMPLER_SLOT; ++I)
	{
		VertexShaderSamplers[I].SetToDefault();
		GeometryShaderSamplers[I].SetToDefault();
		PixelShaderSamplers[I].SetToDefault();
	}

	RasterizerState.SetToDefault();

	for (ZESize I = 0; I < ZE_MAX_VIEWPORT_SLOT; ++I)
		ViewPorts[I].SetZero();

	for (ZESize I = 0; I < ZE_MAX_SCISSOR_SLOT; ++I)
		ScissorRectangles[I].SetZero();

	BlendState.SetToDefault();
	DepthStencilState.SetToDefault();

	for (ZESize I = 0; I < ZE_MAX_RENDER_TARGET_SLOT; ++I)
		RenderTargets[I] = NULL;

	DepthStencilBuffer = NULL;

	ScreenWriteEnable = false;
}

ZEDeviceState::ZEDeviceState()
{
	SetToDefault();
}

ZEDeviceState::~ZEDeviceState()
{

}
