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

#include "ZEMath/ZEMath.h"
#include "ZEDS/ZEHashGenerator.h"
#include "ZEGraphicsDeviceState.h"

/************************************************************************/
/*                          ZEGraphicsDeviceState                       */
/************************************************************************/
void ZEGraphicsDeviceState::SetToDefault()
{
	IndexBuffer = NULL;
	VertexLayout = NULL;
	PixelShader = NULL;
	VertexShader = NULL;
	GeometryShader = NULL;
	DepthStencilBuffer = NULL;

	StencilReferance = 0;
	ComponentBlendMask = ZEGR_CM_ALL;
	ComponentBlendFactors = ZEVector4::One;
	ScreenWriteEnable = false;

	BlendState.SetToDefault();
	RasterizerState.SetToDefault();
	DepthStencilState.SetToDefault();

	for (ZESize I = 0; I < ZEGR_MAX_SAMPLER_SLOT; ++I)
	{
		VertexShaderSamplers[I].SetToDefault();
		GeometryShaderSamplers[I].SetToDefault();
		PixelShaderSamplers[I].SetToDefault();
	}

	memset(ViewPorts, 0, sizeof(ZEGRScissorRectangle) * ZEGR_MAX_SCISSOR_SLOT);
	memset(ScissorRects, 0, sizeof(ZEViewport) * ZEGR_MAX_VIEWPORT_SLOT);

	memset(PixelShaderTextures, 0, sizeof(const ZEGRTexture*) * ZEGR_MAX_TEXTURE_SLOT);
	memset(VertexShaderTextures, 0, sizeof(const ZEGRTexture*) * ZEGR_MAX_TEXTURE_SLOT);
	memset(GeometryShaderTextures, 0, sizeof(const ZEGRTexture*) * ZEGR_MAX_TEXTURE_SLOT);

	memset(RenderTargets, 0, sizeof(const ZEGRRenderTarget*) * ZEGR_MAX_RENDER_TARGET_SLOT);

	memset(VertexBuffers, 0, sizeof(ZEVertexBuffer*) * ZEGR_MAX_VERTEX_BUFFER_SLOT);

	memset(PixelShaderBuffers, 0, sizeof(ZEGRConstantBuffer*) * ZEGR_MAX_CONSTANT_BUFFER_SLOT);
	memset(VertexShaderBuffers, 0, sizeof(ZEGRConstantBuffer*) * ZEGR_MAX_CONSTANT_BUFFER_SLOT);
	memset(GeometryShaderBuffers, 0, sizeof(ZEGRConstantBuffer*) * ZEGR_MAX_CONSTANT_BUFFER_SLOT);
}

ZEGraphicsDeviceState::ZEGraphicsDeviceState()
{
	SetToDefault();
}

ZEGraphicsDeviceState::~ZEGraphicsDeviceState()
{

}

/************************************************************************/
/*                      ZEGraphicsDeviceHashState                       */
/************************************************************************/

void ZEGraphicsDeviceHashState::SetToDefault()
{
	VertexLayoutHash = 0;

	IndexBuffer = NULL;
	memset(VertexBuffers, NULL, sizeof(ZEVertexBuffer*) * ZEGR_MAX_VERTEX_BUFFER_SLOT);
	
	VertexShader = NULL;
	memset(VertexShaderBuffers, NULL, sizeof(ZEGRConstantBuffer*) * ZEGR_MAX_CONSTANT_BUFFER_SLOT);
	memset(VertexShaderTextures, NULL, sizeof(const ZEGRTexture*) * ZEGR_MAX_TEXTURE_SLOT);
	memset(VertexShaderSamplerHashes, 0, sizeof(ZESize) * ZEGR_MAX_SAMPLER_SLOT);

	GeometryShader = NULL;
	memset(GeometryShaderBuffers, NULL, sizeof(ZEGRConstantBuffer*) * ZEGR_MAX_CONSTANT_BUFFER_SLOT);
	memset(GeometryShaderTextures, NULL, sizeof(const ZEGRTexture*) * ZEGR_MAX_TEXTURE_SLOT);
	memset(GeometryShaderSamplerHashes, 0, sizeof(ZESize) * ZEGR_MAX_SAMPLER_SLOT);

	RasterizerStateHash = 0;
	memset(ViewPortHashes, 0, sizeof(ZESize) * ZEGR_MAX_VIEWPORT_SLOT);
	memset(ScissorRectHashes, 0, sizeof(ZESize) * ZEGR_MAX_SCISSOR_SLOT);
	
	PixelShader = NULL;
	memset(PixelShaderBuffers, NULL, sizeof(ZEGRConstantBuffer*) * ZEGR_MAX_CONSTANT_BUFFER_SLOT);
	memset(PixelShaderTextures, NULL, sizeof(const ZEGRTexture*) * ZEGR_MAX_TEXTURE_SLOT);
	memset(PixelShaderSamplerHashes, 0, sizeof(ZESize) * ZEGR_MAX_SAMPLER_SLOT);

	BlendStateHash = 0;
	ComponentBlendMask = ZEGR_CM_ALL;
	ComponentBlendFactors = ZEVector4::One;
	
	DepthStencilStateHash = 0;
	StencilReferance = 0;
	
	DepthStencilBuffer = NULL;
	ScreenWriteEnable = false;
	memset(RenderTargets, NULL, sizeof(const ZEGRRenderTarget*) * ZEGR_MAX_RENDER_TARGET_SLOT);
}

ZEGraphicsDeviceHashState::ZEGraphicsDeviceHashState()
{
	SetToDefault();
}

ZEGraphicsDeviceHashState::~ZEGraphicsDeviceHashState()
{

}
