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

class ZEGRShader;
class ZEGRTexture;
class ZEGRIndexBuffer;
class ZEVertexBuffer;
class ZEGRRenderTarget;
class ZEGRConstantBuffer;
class ZEGRDepthStencilBuffer;

typedef ZEGRColorMask ColorBlendMask;

class ZEGraphicsDeviceState
{
	friend class ZEGraphicsDevice;

	public:
		ZEGRVertexLayout*					VertexLayout;
		ZEGRIndexBuffer*					IndexBuffer;
		ZEVertexBuffer*					VertexBuffers[ZEGR_MAX_VERTEX_BUFFER_SLOT];

		ZEGRShader*						VertexShader;
		ZEGRConstantBuffer*				VertexShaderBuffers[ZEGR_MAX_CONSTANT_BUFFER_SLOT];
		const ZEGRTexture*				VertexShaderTextures[ZEGR_MAX_TEXTURE_SLOT];
		ZEGRSamplerState					VertexShaderSamplers[ZEGR_MAX_SAMPLER_SLOT];

		ZEGRShader*						GeometryShader;
		ZEGRConstantBuffer*				GeometryShaderBuffers[ZEGR_MAX_CONSTANT_BUFFER_SLOT];
		const ZEGRTexture*				GeometryShaderTextures[ZEGR_MAX_TEXTURE_SLOT];
		ZEGRSamplerState					GeometryShaderSamplers[ZEGR_MAX_SAMPLER_SLOT];

		ZEGRRasterizerState				RasterizerState;
		ZEViewport						ViewPorts[ZEGR_MAX_VIEWPORT_SLOT];
		ZEGRScissorRectangle				ScissorRects[ZEGR_MAX_SCISSOR_SLOT];
		
		ZEGRShader*						PixelShader;
		ZEGRConstantBuffer*				PixelShaderBuffers[ZEGR_MAX_CONSTANT_BUFFER_SLOT];
		const ZEGRTexture*				PixelShaderTextures[ZEGR_MAX_TEXTURE_SLOT];
		ZEGRSamplerState					PixelShaderSamplers[ZEGR_MAX_SAMPLER_SLOT];

		ZEGRBlendState					BlendState;
		ColorBlendMask					ComponentBlendMask;
		ZEVector4						ComponentBlendFactors;

		ZEGRDepthStencilState			DepthStencilState;
		ZEUInt32						StencilReferance;

		const ZEGRRenderTarget*			RenderTargets[ZEGR_MAX_RENDER_TARGET_SLOT];
		const ZEGRDepthStencilBuffer*	DepthStencilBuffer;
		bool							ScreenWriteEnable;
		
		void							SetToDefault();

										ZEGraphicsDeviceState();
										~ZEGraphicsDeviceState();
};

class ZEGraphicsDeviceHashState
{
	friend class ZEGraphicsDevice;

	public:
		ZESize						VertexLayoutHash;
		ZEGRIndexBuffer*				IndexBuffer;
		ZEVertexBuffer*				VertexBuffers[ZEGR_MAX_VERTEX_BUFFER_SLOT];

		ZEGRShader*					VertexShader;
		ZEGRConstantBuffer*			VertexShaderBuffers[ZEGR_MAX_CONSTANT_BUFFER_SLOT];
		const ZEGRTexture*			VertexShaderTextures[ZEGR_MAX_TEXTURE_SLOT];
		ZESize						VertexShaderSamplerHashes[ZEGR_MAX_SAMPLER_SLOT];

		ZEGRShader*					GeometryShader;
		ZEGRConstantBuffer*			GeometryShaderBuffers[ZEGR_MAX_CONSTANT_BUFFER_SLOT];
		const ZEGRTexture*			GeometryShaderTextures[ZEGR_MAX_TEXTURE_SLOT];
		ZESize						GeometryShaderSamplerHashes[ZEGR_MAX_SAMPLER_SLOT];

		ZESize						RasterizerStateHash;
		ZESize						ViewPortHashes[ZEGR_MAX_VIEWPORT_SLOT];
		ZESize						ScissorRectHashes[ZEGR_MAX_SCISSOR_SLOT];
		
		ZEGRShader*					PixelShader;
		ZEGRConstantBuffer*			PixelShaderBuffers[ZEGR_MAX_CONSTANT_BUFFER_SLOT];
		const ZEGRTexture*			PixelShaderTextures[ZEGR_MAX_TEXTURE_SLOT];
		ZESize						PixelShaderSamplerHashes[ZEGR_MAX_SAMPLER_SLOT];

		ZESize						BlendStateHash;
		ColorBlendMask				ComponentBlendMask;
		ZEVector4					ComponentBlendFactors;

		ZESize						DepthStencilStateHash;
		ZEUInt32					StencilReferance;

		const ZEGRRenderTarget*		RenderTargets[ZEGR_MAX_RENDER_TARGET_SLOT];
		const ZEGRDepthStencilBuffer*	DepthStencilBuffer;
		bool						ScreenWriteEnable;
		
		void						SetToDefault();

									ZEGraphicsDeviceHashState();
									~ZEGraphicsDeviceHashState();
};

#endif
