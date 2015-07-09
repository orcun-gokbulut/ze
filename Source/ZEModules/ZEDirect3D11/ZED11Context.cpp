//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11Context.cpp
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

#include "ZED11Context.h"

#include "ZED11Shader.h"
#include "ZED11Texture2D.h"
#include "ZED11Texture3D.h"
#include "ZED11TextureCube.h"
#include "ZED11IndexBuffer.h"
#include "ZED11RenderTarget.h"
#include "ZED11VertexBuffer.h"
#include "ZED11Direct3D11Module.h"
#include "ZED11ConstantBuffer.h"
#include "ZED11DepthStencilBuffer.h"
#include "ZEGraphics/ZEGRDefinitions.h"

#include <d3d11.h>

inline DXGI_FORMAT ZEIndexBufferFormatToD3D10(ZEGRIndexBufferFormat BufferFormat)
{
	static const DXGI_FORMAT Values[] = 
	{
		DXGI_FORMAT_R16_UINT,
		DXGI_FORMAT_R32_UINT
	};
	return Values[BufferFormat];
}

inline D3D11_PRIMITIVE_TOPOLOGY ZEPrimitiveTypeToD3D10(ZEGRPrimitiveType PrimitiveType)
{
	static const D3D11_PRIMITIVE_TOPOLOGY Values[] = 
	{
		D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED,
		D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
		D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	};

	return Values[PrimitiveType];
}

void ZED11Context::Draw(ZEGRRenderStateData* State, ZEGRPrimitiveType PrimitiveType, ZEUInt VertexCount, ZEUInt FirstVertex)
{
	State->Setup(this);
	GetMainContext()->IASetPrimitiveTopology(ZEPrimitiveTypeToD3D10(PrimitiveType));
	GetMainContext()->Draw(VertexCount, FirstVertex);
}

void ZED11Context::DrawInstanced(ZEGRRenderStateData* State, ZEGRPrimitiveType PrimitiveType, ZEUInt VertexCount, ZEUInt FirstVertex, ZEUInt InstanceCount, ZEUInt FirstInstance)
{
	State->Setup(this);
	GetMainContext()->IASetPrimitiveTopology(ZEPrimitiveTypeToD3D10(PrimitiveType));
	GetMainContext()->DrawInstanced(VertexCount, InstanceCount, FirstVertex, FirstInstance);
}


void ZED11Context::ClearRenderTarget(ZEGRRenderTarget* RenderTarget, const ZEVector4& ClearColor)
{
	GetMainContext()->ClearRenderTargetView(((ZED11RenderTarget*)RenderTarget)->GetView(), ClearColor.M);
}

void ZED11Context::ClearDepthStencilBuffer(ZEGRDepthStencilBuffer* DepthStencil, bool Depth, bool Stencil, float DepthValue, ZEUInt8 StencilValue)
{
	UINT ClearFlag = 0;
	ClearFlag |= Depth ? D3D11_CLEAR_DEPTH : 0;
	ClearFlag |= Stencil ? D3D11_CLEAR_STENCIL : 0;

	GetMainContext()->ClearDepthStencilView(((ZED11DepthStencilBuffer*)DepthStencil)->GetView(), ClearFlag, DepthValue, StencilValue);
}

ZED11Context::ZED11Context()
{

}

ZED11Context::~ZED11Context()
{
	ZEGR_RELEASE(Context);
}
