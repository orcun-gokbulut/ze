//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGraphicsDeviceState.h
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

#ifndef __ZE_GRAPHICS_DEVICE_STATE_H__
#define __ZE_GRAPHICS_DEVICE_STATE_H__

#include "ZETypes.h"
#include "ZEBlendState.h"
#include "ZEVertexLayout.h"
#include "ZESamplerState.h"
#include "ZERasterizerState.h"
#include "ZEDepthStencilState.h"
#include "ZEGraphicsDefinitions.h"
#include "ZEScissorRectangle.h"
#include "ZEViewPort.h"

class ZEShader;
class ZETexture;
class ZEIndexBuffer;
class ZEVertexBuffer;
class ZERenderTarget;
class ZEConstantBuffer;
class ZEDepthStencilBuffer;

typedef ZEComponentMask ColorBlendMask;

class ZEGraphicsDeviceState
{
	friend class ZEGraphicsDevice;

	public:
		ZEVertexLayout*				VertexLayout;
		ZEIndexBuffer*				IndexBuffer;
		ZEVertexBuffer*				VertexBuffers[ZE_MAX_VERTEX_BUFFER_SLOT];

		ZEShader*					VertexShader;
		ZEConstantBuffer*			VertexShaderBuffers[ZE_MAX_CONSTANT_BUFFER_SLOT];
		const ZETexture*			VertexShaderTextures[ZE_MAX_TEXTURE_SLOT];
		ZESamplerState				VertexShaderSamplers[ZE_MAX_SAMPLER_SLOT];

		ZEShader*					GeometryShader;
		ZEConstantBuffer*			GeometryShaderBuffers[ZE_MAX_CONSTANT_BUFFER_SLOT];
		const ZETexture*			GeometryShaderTextures[ZE_MAX_TEXTURE_SLOT];
		ZESamplerState				GeometryShaderSamplers[ZE_MAX_SAMPLER_SLOT];

		ZERasterizerState			RasterizerState;
		ZEViewport					ViewPorts[ZE_MAX_VIEWPORT_SLOT];
		ZEScissorRectangle			ScissorRects[ZE_MAX_SCISSOR_SLOT];
		
		ZEShader*					PixelShader;
		ZEConstantBuffer*			PixelShaderBuffers[ZE_MAX_CONSTANT_BUFFER_SLOT];
		const ZETexture*			PixelShaderTextures[ZE_MAX_TEXTURE_SLOT];
		ZESamplerState				PixelShaderSamplers[ZE_MAX_SAMPLER_SLOT];

		ZEBlendState				BlendState;
		ColorBlendMask				ComponentBlendMask;
		ZEVector4					ComponentBlendFactors;

		ZEDepthStencilState			DepthStencilState;
		ZEUInt32					StencilReferance;

		const ZERenderTarget*		RenderTargets[ZE_MAX_RENDER_TARGET_SLOT];
		const ZEDepthStencilBuffer*	DepthStencilBuffer;
		bool						ScreenWriteEnable;
		
		void						SetToDefault();

									ZEGraphicsDeviceState();
									~ZEGraphicsDeviceState();
};

class ZEGraphicsDeviceHashState
{
	friend class ZEGraphicsDevice;

	public:
		ZESize						VertexLayoutHash;
		ZEIndexBuffer*				IndexBuffer;
		ZEVertexBuffer*				VertexBuffers[ZE_MAX_VERTEX_BUFFER_SLOT];

		ZEShader*					VertexShader;
		ZEConstantBuffer*			VertexShaderBuffers[ZE_MAX_CONSTANT_BUFFER_SLOT];
		const ZETexture*			VertexShaderTextures[ZE_MAX_TEXTURE_SLOT];
		ZESize						VertexShaderSamplerHashes[ZE_MAX_SAMPLER_SLOT];

		ZEShader*					GeometryShader;
		ZEConstantBuffer*			GeometryShaderBuffers[ZE_MAX_CONSTANT_BUFFER_SLOT];
		const ZETexture*			GeometryShaderTextures[ZE_MAX_TEXTURE_SLOT];
		ZESize						GeometryShaderSamplerHashes[ZE_MAX_SAMPLER_SLOT];

		ZESize						RasterizerStateHash;
		ZESize						ViewPortHashes[ZE_MAX_VIEWPORT_SLOT];
		ZESize						ScissorRectHashes[ZE_MAX_SCISSOR_SLOT];
		
		ZEShader*					PixelShader;
		ZEConstantBuffer*			PixelShaderBuffers[ZE_MAX_CONSTANT_BUFFER_SLOT];
		const ZETexture*			PixelShaderTextures[ZE_MAX_TEXTURE_SLOT];
		ZESize						PixelShaderSamplerHashes[ZE_MAX_SAMPLER_SLOT];

		ZESize						BlendStateHash;
		ColorBlendMask				ComponentBlendMask;
		ZEVector4					ComponentBlendFactors;

		ZESize						DepthStencilStateHash;
		ZEUInt32					StencilReferance;

		const ZERenderTarget*		RenderTargets[ZE_MAX_RENDER_TARGET_SLOT];
		const ZEDepthStencilBuffer*	DepthStencilBuffer;
		bool						ScreenWriteEnable;
		
		void						SetToDefault();

									ZEGraphicsDeviceHashState();
									~ZEGraphicsDeviceHashState();
};

#endif
