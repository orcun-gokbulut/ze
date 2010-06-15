//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - D3D9PPScreenOutputNode.cpp
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

#include "D3D9PPScreenOutputNode.h"
#include "Core/Error.h"
#include "Graphics/Texture2D.h"
#include "../D3D9CommonTools.h"
#include "../D3D9Module.h"

LPDIRECT3DPIXELSHADER9 ZED3D9PPScreenOutputNode::PixelShader = NULL;
LPDIRECT3DVERTEXSHADER9 ZED3D9PPScreenOutputNode::VertexShader = NULL;

bool ZED3D9PPScreenOutputNode::Initialize()
{
	if (VertexShader == NULL)
	{
		if (GetModule()->GetVertexShaderVersion() < ZE_D3D9_VSV_2_0_0)
		{
			zeError("D3D9 Post Processor - Screen Output Node", "Vertex shader is not supported.");
			return false;
		}

		if (!ZED3D9CommonTools::CompileVertexShader(&VertexShader,
			"struct VS_OUTPUT"
			"{"
				"float4 Position : POSITION0;"
				"float2 TexCoord : TEXCOORD0;"
			"};"
			"VS_OUTPUT vs_main(float4 Position : POSITION0, float2 TexCoord : TEXCOORD0)"
			"{"
				"VS_OUTPUT Output;"
				"Output.Position = float4(Position.xy, 0.0f, 1.0f);"
				"Output.TexCoord = TexCoord;"
				"return Output;"
			"}", "Screen copy", "vs_2_0", NULL)
			)
		{
			zeError("D3D9 Post Processor - Screen Output Node", "Can not compile vertex shader.");
			return false;
		}
	}

	if (PixelShader == NULL)
	{
		if (GetModule()->GetVertexShaderVersion() < ZE_D3D9_PSV_2_0_0)
		{
			zeError("D3D9 Post Processor - Screen Output Node", "Pixel shader is not supported.");
			return false;
		}

		// Create horizontal blur pass pixel shader
		if (!ZED3D9CommonTools::CompilePixelShader(&PixelShader, 
				"sampler Input : register(s0);"
				"float4 ps_main(float2 TexCoord : TEXCOORD0) : COLOR0"
				"{"
					"return tex2D(Input, TexCoord);"
				"}", "Screen copy", "ps_2_0", NULL)
			)
			return false;
	}

	return true;
}

void ZED3D9PPScreenOutputNode::Deinitialize()
{
	
}

bool ZED3D9PPScreenOutputNode::Proces()
{
	if (PixelShader == NULL || VertexShader == NULL)
	{
		zeError("D3D9 Post Processor - Screen Output Node", "Pixel or/and vertex shader is not available.");
		return false;
	}

	// Check input post processor is available
	if (Input == NULL)
	{
		zeError("D3D9 Post Processor - Screen Output Node", "Post processor node does not have any input.");
		return false;
	}
	
	ZETexture2D* InputTexture = Input->GetOutput();

	// Check input node provides valid texture or not
	if (InputTexture == NULL || InputTexture->IsEmpty())
	{
		zeError("D3D9 Post Processor - Screen Output Node", "Post processor input node output texture is not available.");
		return false;
	}

	// Calculate pixel sizes for calculating texel to pixel conversion
	float PixelWidth_2 = 0.5f / InputTexture->GetWidth();
	float PixelHeight_2 = 0.5f / InputTexture->GetHeight();
	
	// Generate screen aligned quad in the memory
	struct
	{
		float Position[3];
		float UV[2];
	} 
	ScreenAlignedQuad[] = {
		{{-1.0f + PixelWidth_2, 1.0f + PixelHeight_2, 0.0f},  {0.0f, 0.0f}},
		{{ 1.0f + PixelWidth_2, 1.0f + PixelHeight_2, 0.0f},  {1.0f, 0.0f}},
		{{ 1.0f + PixelWidth_2, -1.0f + PixelHeight_2, 0.0f}, {1.0f, 1.0f}},
		{{-1.0f + PixelWidth_2, -1.0f + PixelHeight_2, 0.0f}, {0.0f, 1.0f}}};

	GetDevice()->SetFVF(D3DFVF_XYZ | D3DFVF_TEX0);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, TRUE);

	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	GetDevice()->SetRenderTarget(0, GetModule()->GetFrameColorBuffer());
	GetDevice()->SetPixelShader(PixelShader);
	GetDevice()->SetVertexShader(VertexShader);
	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, ScreenAlignedQuad, 5 * 4);
	
	return true;
}

ZETexture2D* ZED3D9PPScreenOutputNode::GetOutput()
{
	return NULL;
}
