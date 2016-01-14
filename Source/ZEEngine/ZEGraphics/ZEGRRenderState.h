//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRRenderState.h
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

#pragma once

#include "ZETypes.h"
#include "ZECommon.h"
#include "ZEPointer/ZEHolder.h"
#include "ZEGRVertexLayout.h"
#include "ZEGRDefinitions.h"
#include "ZEGRBlendState.h"
#include "ZEGRSamplerState.h"
#include "ZEGRRasterizerState.h"
#include "ZEGRDepthStencilState.h"
#include "ZEGRScissorRect.h"
#include "ZEGRViewPort.h"
#include "ZEGRDepthStencilBuffer.h"
#include "ZEGRFormat.h"

class ZEGRVertexLayout;
class ZEGRVertexBuffer;
class ZEGRIndexBuffer;
class ZEGRShader;
class ZEGRTexture;
class ZEGRRenderTarget;
class ZEGRConstantBuffer;
class ZEGRDepthStencilBuffer;
class ZEGRContext;

typedef ZEGRColorMask ColorBlendMask;

class ZEGRRenderState;

class ZEGRRenderStateData : public ZEGRResource
{
	friend class ZEGRRenderState;
	protected:
		virtual bool							Initialize(const ZEGRRenderState& RenderState) = 0;

	public:
		virtual ZEGRResourceType				GetResourceType();

		static ZEGRRenderStateData*				Create(const ZEGRRenderState& RenderState);
};

class ZEGRRenderState
{
	friend class ZEGRContext;

	private:
		ZEGRPrimitiveType						PrimitiveType;
		ZEGRVertexLayout						VertexLayout;
		ZEHolder<ZEGRShader>					Shaders[ZEGR_SHADER_TYPE_COUNT];
		ZEGRRasterizerState						RasterizerState;
		ZEGRDepthStencilState					DepthStencilState;
		ZEGRFormat								DepthStencilFormat;
		ZEGRFormat								RenderTargetFormats[ZEGR_MAX_RENDER_TARGET_SLOT];
		ZEGRBlendState							BlendState;

	public:
		static ZEGRRenderState					Default;

		void									SetVertexLayout(const ZEGRVertexLayout& Layout);
		const ZEGRVertexLayout&					GetVertexLayout() const;

		void									SetPrimitiveType(ZEGRPrimitiveType Type);
		ZEGRPrimitiveType						GetPrimitiveType() const;

		void									SetShader(ZEGRShaderType Type, ZEGRShader* Shader);
		ZEGRShader*								GetShader(ZEGRShaderType Type) const;

		void									SetRenderTargetFormat(ZEUInt Index, ZEGRFormat Format);
		ZEGRFormat								GetRenderTargetFormat(ZEUInt Index) const;

		void									SetDepthStencilFormat(ZEGRFormat Format);
		ZEGRFormat								GetDepthStencilFormat() const;

		void									SetRasterizerState(const ZEGRRasterizerState& State);
		const ZEGRRasterizerState&				GetRasterizerState() const;

		void									SetBlendState(const ZEGRBlendState& State);
		const ZEGRBlendState&					GetBlendState() const;

		void									SetDepthStencilState(const ZEGRDepthStencilState& State);
		const ZEGRDepthStencilState&			GetDepthStencilState() const;
		
		void									SetToDefault();

		ZEGRRenderStateData*					Compile();

		ZEGRRenderState&						operator=(const ZEGRRenderState& RenderState);

												ZEGRRenderState();
												ZEGRRenderState(const ZEGRRenderState& RenderState);
		virtual									~ZEGRRenderState();
};
