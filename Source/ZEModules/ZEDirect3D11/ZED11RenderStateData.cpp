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

#include "ZED11Context.h"
#include "ZED11Shader.h"
#include "ZED11StatePool.h"
#include "ZED11Module.h"
#include "ZEGraphics/ZEGRTexture.h"
#include "ZEGraphics/ZEGRRenderState.h"

static D3D11_PRIMITIVE_TOPOLOGY ConvertPrimitiveType(ZEGRPrimitiveType Type)
{
	switch (Type)
	{
		default:
		case ZEGR_PT_NONE:
			return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;

		case ZEGR_PT_POINT_LIST:
			return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

		case ZEGR_PT_LINE_LIST:
			return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

		case ZEGR_PT_TRIANGLE_LIST:
			return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		case ZEGR_PT_TRIANGLE_STRIPT:
			return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

		case ZEGR_PT_4_CONTROL_POINT_PATCHLIST:
			return D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;

		case ZEGR_PT_16_CONTROL_POINT_PATCHLIST:
			return D3D11_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST;
	}
}

bool ZED11RenderStateData::Initialize(const ZEGRRenderState& RenderState)
{
	ZED11StatePool* StatePool = GetModule()->GetStatePool();

	for (ZESize I = 0; I < ZEGR_SHADER_TYPE_COUNT; I++)
		Shaders[I] = static_cast<const ZED11Shader*>(RenderState.GetShader((ZEGRShaderType)I));

	BlendState = StatePool->GetBlendState(RenderState.GetBlendState());
	RasterizerState = StatePool->GetRasterizerState(RenderState.GetRasterizerState());
	DepthStencilState = StatePool->GetDepthStencilState(RenderState.GetDepthStencilState());
	VertexLayout = StatePool->GetVertexLayout(RenderState.GetVertexLayout(), Shaders[ZEGR_ST_VERTEX]);
	PrimitiveTopology = ConvertPrimitiveType(RenderState.GetPrimitiveType());
	
	return true;
}

bool ZED11ComputeRenderStateData::Initialize(const ZEGRComputeRenderState& RenderState)
{
	this->ComputeShader = static_cast<const ZED11Shader*>(RenderState.GetComputeShader());

	return true;
}
