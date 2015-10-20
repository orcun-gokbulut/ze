//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11RenderStateData.cpp
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

#include "ZED11RenderStateData.h"

#include "ZEGraphics\ZEGRRenderState.h"
#include "ZED11Context.h"
#include "ZED11VertexBuffer.h"
#include "ZED11IndexBuffer.h"
#include "ZED11ConstantBuffer.h"
#include "ZED11RenderTarget.h"
#include "ZED11Shader.h"
#include "ZED11StatePool.h"
#include "ZED11DepthStencilBuffer.h"
#include "ZED11Module.h"
#include "ZEGraphics/ZEGRTexture.h"

#include <d3d11.h>

static D3D11_PRIMITIVE_TOPOLOGY ConvertPrimitveType(ZEGRPrimitiveType Type)
{
	switch (Type)
	{
		default:
		case ZEGR_PT_NONE:
			return (D3D11_PRIMITIVE_TOPOLOGY)0;

		case ZEGR_PT_POINT_LIST:
			return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

		case ZEGR_PT_LINE_LIST:
			return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

		case ZEGR_PT_TRIANGLE_LIST:
			return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		case ZEGR_PT_TRIANGLE_STRIPT:
			return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	}
}

bool ZED11RenderStateData::Initialize(const ZEGRRenderState& RenderState)
{
	ZED11StatePool* StatePool = GetModule()->GetStatePool();
	
	//if (RenderState.GetVertexLayout().GetElementCount() == 0)
	//	return false;

	VertexLayout = StatePool->GetVertexLayout(RenderState.GetVertexLayout(), RenderState.GetShader(ZEGR_ST_VERTEX));
	if (VertexLayout == NULL)
		return false;

	RasterizerState = StatePool->GetRasterizerState(RenderState.GetRasterizerState());
	if (RasterizerState == NULL)
		return false;
	NativeRasterizerState = RasterizerState->GetInterface();

	DepthStencilState = StatePool->GetDepthStencilState(RenderState.GetDepthStencilState());
	if (DepthStencilState == NULL)
		return false;
	NativeDepthStencilState = DepthStencilState->GetInterface();

	BlendState = StatePool->GetBlendState(RenderState.GetBlendState());
	if (BlendState == NULL)
		return false;
	NativeBlendState = BlendState->GetInterface();

	PrimitiveTopology = ConvertPrimitveType(RenderState.GetPrimitiveType());
	
	for (ZESize I = 0; I < ZEGR_SHADER_TYPE_COUNT; I++)
		Shaders[I] = RenderState.GetShader((ZEGRShaderType)I);
	
	return true;
}
