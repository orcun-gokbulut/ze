//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - D3D9PPBlurNode.cpp
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

#include "D3D9PPBlurNode.h"
#include "Core/Error.h"
#include "../D3D9CommonTools.h"
#include "../D3D9Texture2D.h"
#include "../D3D9Module.h"

LPDIRECT3DVERTEXSHADER9 ZED3D9PPBlurNode::VertexShader = NULL;
LPDIRECT3DPIXELSHADER9 ZED3D9PPBlurNode::VerticalPassPixelShader = NULL;
LPDIRECT3DPIXELSHADER9 ZED3D9PPBlurNode::HorizontalPassPixelShader = NULL;

ZED3D9PPBlurNode::ZED3D9PPBlurNode()
{

}

ZED3D9PPBlurNode::~ZED3D9PPBlurNode()
{
	Deinitialize();
}

bool ZED3D9PPBlurNode::Initialize()
{
	if (GetModule()->GetVertexShaderVersion() != ZE_D3D9_VSV_NONE)
	{
		// Create general vertex shader
		if (VertexShader == NULL)
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
					"}", "Blur Pass", "vs_2_0", NULL)
				)
				return false;
	}
	else
	{
		zeError("D3D9 Post Processor - Blur Node", "Vertex shader is not supported.");
		return false;
	}

	switch(GetModule()->GetPixelShaderVersion())
	{
		case ZE_D3D9_PSV_2_0_B:
		case ZE_D3D9_PSV_2_0_0:
			// Create horizontal blur pass pixel shader
			if (HorizontalPassPixelShader == NULL)
				if (!ZED3D9CommonTools::CompilePixelShader(&HorizontalPassPixelShader, 
						"sampler Input : register(s0);"
						"const float2 Kernel[7] : register(c0);"
						"float4 ps_main(float2 TexCoord : TEXCOORD0) : COLOR0"
						"{"
							"float4 Color = 0.0f;"
							"for (int I = 0; I < KernelSize; I++)"
								"Color += Kernel[I].x * tex2D(Input, float2(TexCoord.x, TexCoord.y + Kernel[I].y));"
							"return Color;"
						"}", "Horizantal Blur Pass", "ps_2_0", NULL)
					)
					return false;

			// Create vertical blur pass pixel shader
			if (VerticalPassPixelShader == NULL)
				if (!ZED3D9CommonTools::CompilePixelShader(&VerticalPassPixelShader, 
						"sampler Input : register(s0);"
						"const float2 Kernel[7] : register(c0);"
						"float4 ps_main(float2 TexCoord : TEXCOORD0) : COLOR0"
						"{"
							"float4 Color = 0.0f;"
							"for (int I = 0; I < KernelSize; I++)"
								"Color += Kernel[I].x * tex2D(Input, float2(TexCoord.x + Kernel[I].y * PixelSize, TexCoord.y));"
							"return Color;"
						"}", "Vertical Blur Pass", "ps_2_0", NULL)
					)
					return false;
			break;
		case ZE_D3D9_PSV_2_0_A:
			// Create horizontal blur pass pixel shader
			if (HorizontalPassPixelShader == NULL)
				if (!ZED3D9CommonTools::CompilePixelShader(&HorizontalPassPixelShader, 
						"sampler Input : register(s0);"
						"const int KernelSize : register(i0);"
						"const float2 Kernel[16] : register(c0);"
						"float4 ps_main(float2 TexCoord : TEXCOORD0) : COLOR0"
						"{"
							"float4 Color = 0.0f;"
							"for (int I = 0; I < KernelSize; I++)"
								"Color += Kernel[I].x * tex2D(Input, float2(TexCoord.x, TexCoord.y + Kernel[I].y * PixelSize));"
							"return Color;"
						"}", "Horizantal Blur Pass", "ps_2_a", NULL)
					)
					return false;

			// Create vertical blur pass pixel shader
			if (VerticalPassPixelShader == NULL)
				if (!ZED3D9CommonTools::CompilePixelShader(&VerticalPassPixelShader, 
						"sampler Input : register(s0);"
						"const int KernelSize : register(i0);"
						"const float PixelSize : register(c0);"
						"const float2 Kernel[16] : register(c1);"
						"float4 ps_main(float2 TexCoord : TEXCOORD0) : COLOR0"
						"{"
							"float4 Color = 0.0f;"
							"for (int I = 0; I < KernelSize; I++)"
								"Color += Kernel[I].x * tex2D(Input, float2(TexCoord.x + Kernel[I].y, TexCoord.y));"
							"return Color;"
						"}", "Vertical Blur Pass", "ps_2_a", NULL)
					)
					return false;
			break;
		case ZE_D3D9_PSV_3_0_0:
			// Create horizontal blur pass pixel shader
			if (HorizontalPassPixelShader == NULL)
				if (!ZED3D9CommonTools::CompilePixelShader(&HorizontalPassPixelShader, 
						"sampler Input : register(s0);"
						"int KernelSize : register(i0);"
						"float PixelSize : register(c0);"
						"float2 Kernel[16] : register(c1);"
						"float4 ps_main(float2 TexCoord : TEXCOORD0) : COLOR0"
						"{"
							"float4 Color = 0.0f;"
							"for (int I = 0; I < KernelSize; I++)"
								"Color += Kernel[I].x * tex2D(Input, float2(TexCoord.x, TexCoord.y + Kernel[I].y * PixelSize));"
							"return Color;"
						"}", "Horizantal Blur Pass", "ps_3_0", NULL)
					)
					return false;

			// Create vertical blur pass pixel shader
			if (VerticalPassPixelShader == NULL)
				if (!ZED3D9CommonTools::CompilePixelShader(&VerticalPassPixelShader, 
						"sampler Input : register(s0);"
						"int KernelSize : register(i0);"
						"float PixelSize : register(c0);"
						"float2 Kernel[16] : register(c1);"
						"float4 ps_main(float2 TexCoord : TEXCOORD0) : COLOR0"
						"{"
							"float4 Color = 0.0f;"
							"for (int I = 0; I < KernelSize; I++)"
								"Color += Kernel[I].x * tex2D(Input, float2(TexCoord.x + Kernel[I].y, TexCoord.y));"
							"return Color;"
						"}", "Vertical Blur Pass", "ps_3_0", NULL)
					)
					return false;
			break;
		default:
			zeError("D3D9 Post Processor - Blur Node", "Pixel shader is not supported.");
			return false;
	}
}

void ZED3D9PPBlurNode::Deinitialize()
{
	// If temporary render target created, release it
	if (InternalSurface != NULL)
	{
		InternalSurface->Release();
		InternalSurface = NULL;
	}

	if (Output != NULL)
	{
		Output->Destroy();
		Output = NULL;
	}
}

bool ZED3D9PPBlurNode::Process()
{
	// Check shaders
	if (VertexShader == NULL || (HorizontalPass == NULL && VerticalPassPixelShader == NULL))
	{
		zeError("D3D9 Post Processor - Blur Node", "Vertex or/and Pixel shaders are not available");
		return false;
	}

	// Check input post processor is available
	if (Input == NULL)
	{
		zeError("D3D9 Post Processor - Blur Node", "Post processor node does not have any input.");
		return false;
	}
	
	ZETexture2D* InputTexture = Input->GetOutput();

	// Check input node provides valid texture or not
	if (InputTexture == NULL || InputTexture->IsEmpty())
	{
		zeError("D3D9 Post Processor - Blur Node", "Post processor input node output texture is not available.");
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

	// Set proper render states for drawing post effect
	GetDevice()->SetFVF(D3DFVF_XYZ | D3DFVF_TEX0);							// Vertex Format
	GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);						// No Z-Culling
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);					// No Z-Write
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);				// No Alpha Test
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);				// No Alpha Blending
	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, TRUE);				// Enable Color Write
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);				// No Culling

	GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);		// No Filtering
	GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);		// No filtering
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);	// Texture Adressing in Clamp Mode
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);	// Texture Adressing in Clamp Mode

	// Set Vertex Shader
	GetDevice()->SetVertexShader(VertexShader);

	// Update kernel values. UpdateKernel() function will detect changed properties of Blur Post Effect and 
	// if necessery will regenerate kernel values.
	UpdateKernel();

	
	// Set shader constants
	// Check shader versions and mechanisms
	if (GetModule()->GetPixelShaderVersion() > ZE_D3D9_PSV_2_0_A || GetModule()->GetPixelShaderVersion() == ZE_D3D9_PSV_3_0_0)
	{
		int KernelSize = Kernel.GetCount();
		GetDevice()->SetPixelShaderConstantF(0, (float*)Kernel.GetCArray(), Kernel.GetCount());
		GetDevice()->SetPixelShaderConstantI(0, &KernelSize, 1);
	}
	else
		GetDevice()->SetPixelShaderConstantF(0, (float*)Kernel.GetCArray(), 7);

	if (HorizontalPass && VerticalPass)
	{
		// Do both vertical and horizontal passes. 
		// First do vertical pass and put output in temporary render target
		// then do second pass read values from temporary render target and put
		// output in output texture.

		// Create temporary render target. 
		// ZED3D9CommonTools::CreateRenderTarget will check previous render target is available or has correct dimensions. If not it will create one or modify current one.
		ZED3D9CommonTools::CreateRenderTarget(&InternalSurface, InputTexture->GetWidth(), InputTexture->GetHeight(), InputTexture->GetPixelFormat());


		// HORIZONTAL PASS
		// Set horizontal pixel shader
		GetDevice()->SetPixelShader(HorizontalPassPixelShader);
		// Set input as texture
		GetDevice()->SetTexture(0, ((ZED3D9Texture2D*)InputTexture)->Texture);
		// Set output as render target
		LPDIRECT3DSURFACE9 RenderTarget;
		((ZED3D9Texture2D*)InputTexture)->Texture->GetSurfaceLevel(0, &RenderTarget);
		GetDevice()->SetRenderTarget(0, RenderTarget);
		// Set Pixel Size
		PixelWidth_2 *= 2;
		GetDevice()->SetPixelShaderConstantF(1, &PixelWidth_2, 1);
		// Draw the pass
		GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, ScreenAlignedQuad, 5 * 4);

		
		// VERTICAL PASS
		// Set horizontal pixel shader
		GetDevice()->SetPixelShader(VerticalPassPixelShader);
		// Set input as texture
		GetDevice()->SetTexture(0, ((ZED3D9Texture2D*)InternalSurface)->Texture);
		// Set output as render target
		((ZED3D9Texture2D*)Output)->Texture->GetSurfaceLevel(0, &RenderTarget);
		GetDevice()->SetRenderTarget(0, RenderTarget);

		// Set Pixel Size
		PixelWidth_2 *= 2;
		GetDevice()->SetPixelShaderConstantF(1, &PixelHeight_2, 1);
		// Draw the pass
		GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, ScreenAlignedQuad, 5 * 4);
	}
	else
		if (VerticalPass)
		{
			// VERTICAL PASS
			// Set vertical pixel shader
			GetDevice()->SetPixelShader(VerticalPassPixelShader);
			// Set input as texture
			GetDevice()->SetTexture(0, ((ZED3D9Texture2D*)InputTexture)->Texture);
			// Set output as render target
			LPDIRECT3DSURFACE9 RenderTarget;
			((ZED3D9Texture2D*)Output)->Texture->GetSurfaceLevel(0, &RenderTarget);
			GetDevice()->SetRenderTarget(0, RenderTarget);
			// Set Pixel Size
			PixelWidth_2 *= 2;
			GetDevice()->SetPixelShaderConstantF(1, &PixelHeight_2, 1);
			// Draw the pass
			GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, ScreenAlignedQuad, 5 * 4);
		}
		else
		{
			// HORIZONTAL PASS
			// Set vertical pixel shader
			GetDevice()->SetPixelShader(HorizontalPassPixelShader);
			// Set input as texture
			GetDevice()->SetTexture(0, ((ZED3D9Texture2D*)InputTexture)->Texture);
			// Set output as render target
			LPDIRECT3DSURFACE9 RenderTarget;
			((ZED3D9Texture2D*)Output)->Texture->GetSurfaceLevel(0, &RenderTarget);
			GetDevice()->SetRenderTarget(0, RenderTarget);
			// Set Pixel Size
			PixelWidth_2 *= 2;
			GetDevice()->SetPixelShaderConstantF(1, &PixelWidth_2, 1);
			// Draw the pass
			GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, ScreenAlignedQuad, 5 * 4);
		}
}

ZETexture2D* ZED3D9PPBlurNode::GetOutput()
{
	return Output;
}
