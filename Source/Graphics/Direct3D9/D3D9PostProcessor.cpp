//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - D3D9PostProcessor.cpp
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

#include "D3D9PostProcessor.h"
#include "Core/Error.h"
#include "D3D9CommonTools.h"
#include "D3D9Texture2D.h"
#include "D3D9Module.h"

LPDIRECT3DVERTEXSHADER9 ZED3D9BlurPostEffect::VertexShader = NULL;
LPDIRECT3DPIXELSHADER9 ZED3D9BlurPostEffect::VerticalPassPixelShader = NULL;
LPDIRECT3DPIXELSHADER9 ZED3D9BlurPostEffect::HorizontalPassPixelShader = NULL;

ZED3D9BlurPostEffect::ZED3D9BlurPostEffect()
{
	Temporary = NULL;
}

ZED3D9BlurPostEffect::~ZED3D9BlurPostEffect()
{
	Deinitialize();
}

bool ZED3D9BlurPostEffect::Initialize()
{
	// Create general vertex shader
	if (VertexShader == NULL)
		if (!ZED3D9CommonTools::CompileVertexShader(&VertexShader,
				"struct VS_OUTPUT"
				"{"
					"float4 Position : POSITION0;"
					"float2 TexCoord : TEXCOORD0;"
				"};"
				"VS_OUTPUT vs_main(float4 Position : POSITION0)"
				"{"
					"VS_OUTPUT Output;"
					"Output.Position = float4(Position.xy, 0.0f, 1.0f);"
					"Output.TexCoord.x = Output.TexCoord.y;"
					"return Output;"
				"}", "Blur Pass", "VS_2_0", NULL)
			)
			return false;

	// Create horizontal blur pass pixel shader
	if (HorizontalPassPixelShader == NULL)
		if (!ZED3D9CommonTools::CompilePixelShader(&HorizontalPassPixelShader, 
				"sampler Input : register(s0);"
				"const int KernelSize : register(i0);"
				"const float2 Kernel : register(c0);"
				"const float PixelSize : register(1);"
				"float4 ps_main(float2 TexCoord : TEXCOORD0) : COLOR0"
				"{"
					"float4 Color = 0.0f;"
					"for (int I = 0; I < KernelSize; I++)"
						"Color += Kernel[I].y * tex2D(Input, float2(TexCoord.x, TexCoord.y + Kernel[I].x * PixelSize));"
					"return Color;"
				"}", "Horizantal Blur Pass", "PS_3_0", NULL)
			)
			return false;

	// Create vertical blur pass pixel shader
	if (VerticalPassPixelShader == NULL)
		if (!ZED3D9CommonTools::CompilePixelShader(&VerticalPassPixelShader, 
				"sampler Input : register(s0);"
				"const int KernelSize : register(i0);"
				"const float2 Kernel : register(c0);"
				"const float PixelSize : register(1);"
				"float4 ps_main(float2 TexCoord : TEXCOORD0) : COLOR0"
				"{"
					"float4 Color = 0.0f;"
					"for (int I = 0; I < KernelSize; I++)"
						"Color += Kernel[I].y * tex2D(Input, float2(TexCoord.x + Kernel[I].x * PixelSize, TexCoord.y));"
					"return Color;"
				"}", "Vertical Blur Pass", "PS_3_0", NULL)
			)
			return false;

	return true;
}

void ZED3D9BlurPostEffect::Deinitialize()
{
	// If temporary render target created, release it
	if (Temporary != NULL)
		Temporary->Release();
}

bool ZED3D9BlurPostEffect::Process()
{
	// Check input post processor is available
	if (Input == NULL && this->InputTexture == NULL)
	{
		zeError("Blur Post Effect", "Post effect does not have any input.");
		return false;
	}

	// Check wheater we are getting input from a texture or output of a post processor
	ZETexture2D* InputTexture;
	if (Input != NULL)
	{
		InputTexture = Input->GetOutput();

		// Check input postprocessor has valid output
		if (InputTexture == NULL)
		{
			zeError("Blur Post Effect", "Output of input post effect does not exist.");
			return false;
		}
	}
	else
		InputTexture = this->InputTexture;


	if (!OutputToFrameBuffer)
	{
		// If post effect will not output directly frame buffer then create output render target. 
		if (Output == NULL)
			Output = ZETexture2D::CreateInstance();

		if (Output->GetWidth() != InputTexture->GetWidth() || Output->GetHeight() != InputTexture->GetHeight() || Output->GetPixelFormat() != InputTexture->GetPixelFormat())
			Output->Create(InputTexture->GetWidth(), InputTexture->GetHeight(), InputTexture->GetPixelFormat(), true);
	}
	else
	{
		// If it will then release output render targer in case of old configurations of this post effect did not output frame buffer
		if (Output != NULL)
			Output->Release();
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
	ScreenAlignedQuad[] =	{
		{{-1.0f + PixelWidth_2, 1.0f + PixelHeight_2, 0.0f},  {0.0f, 0.0f}},
		{{ 1.0f + PixelWidth_2, 1.0f + PixelHeight_2, 0.0f},  {1.0f, 0.0f}},
		{{ 1.0f + PixelWidth_2, -1.0f + PixelHeight_2, 0.0f}, {1.0f, 1.0f}},
		{{-1.0f + PixelWidth_2, -1.0f + PixelHeight_2, 0.0f}, {0.0f, 1.0f}}};

	// Set proper render states for drawing post effect
	Device->SetFVF(D3DFVF_XYZ |D3DFVF_TEX0);							// Vertex Format
	Device->SetRenderState(D3DRS_ZENABLE, FALSE);						// No Z-Culling
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);				// No Alpha Test
	Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);				// No Alpha Blending
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);				// No Culling
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);		// No Filtering
	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);		// No filtering
	Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);	// Texture Adressing in Clamp Mode
	Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);	// Texture Adressing in Clamp Mode

	// Set Vertex Shader
	Device->SetVertexShader(VertexShader);

	// Update kernel values. UpdateKernel() function will detect changed properties of Blur Post Effect and 
	// if necessery will regenerate kernel values.
	UpdateKernel();

	// Set Kernel values
	Device->SetPixelShaderConstantF(0, Kernel.GetCArray(), Kernel.GetCount());
	Device->SetPixelShaderConstantI(0, &KernelSize, 1);

	if (HorizontalPass && VerticalPass)
	{
		// Do both vertical and horizontal passes. 
		// First do vertical pass and put output in temporary render target
		// then do second pass read values from temporary render target and put
		// output in output texture.

		// Create temporary render target. 
		// ZED3D9CommonTools::CreateRenderTarget will check previous render target is available or has correct dimensions. If not it will create one or modify current one.
		ZED3D9CommonTools::CreateRenderTarget(&Temporary, InputTexture->GetWidth(), InputTexture->GetHeight(), InputTexture->GetPixelFormat());


		// HORIZONTAL PASS
		// Set horizontal pixel shader
		Device->SetPixelShader(HorizontalPassPixelShader);
		// Set input as texture
		Device->SetTexture(0, ((ZED3D9Texture2D*)InputTexture)->Texture);
		// Set output as render target
		LPDIRECT3DSURFACE9 RenderTarget;
		((ZED3D9Texture2D*)InputTexture)->Texture->GetSurfaceLevel(0, &RenderTarget);
		Device->SetRenderTarget(0, RenderTarget);
		// Set Pixel Size
		PixelWidth_2 *= 2;
		Device->SetPixelShaderConstantF(1, &PixelWidth_2, 1);
		// Draw the pass
		Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, ScreenAlignedQuad, 0);

		
		// VERTICAL PASS
		// Set horizontal pixel shader
		Device->SetPixelShader(VerticalPassPixelShader);
		// Set input as texture
		Device->SetTexture(0, ((ZED3D9Texture2D*)Temporary)->Texture);
		// Set output as render target
		if (OutputToFrameBuffer)
			Device->SetRenderTarget(0, Module->FrameColorBuffer);
		else
		{
			((ZED3D9Texture2D*)Output)->Texture->GetSurfaceLevel(0, &RenderTarget);
			Device->SetRenderTarget(0, RenderTarget);
		}
		// Set Pixel Size
		PixelWidth_2 *= 2;
		Device->SetPixelShaderConstantF(1, &PixelHeight_2, 1);
		// Draw the pass
		Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, ScreenAlignedQuad, 0);
	}
	else
		if (VerticalPass)
		{
			// VERTICAL PASS
			// Set vertical pixel shader
			Device->SetPixelShader(VerticalPassPixelShader);
			// Set input as texture
			Device->SetTexture(0, ((ZED3D9Texture2D*)InputTexture)->Texture);
			// Set output as render target
			LPDIRECT3DSURFACE9 RenderTarget;
			((ZED3D9Texture2D*)Output)->Texture->GetSurfaceLevel(0, &RenderTarget);
			Device->SetRenderTarget(0, RenderTarget);
			// Set Pixel Size
			PixelWidth_2 *= 2;
			Device->SetPixelShaderConstantF(1, &PixelHeight_2, 1);
			// Draw the pass
			Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, ScreenAlignedQuad, 0);
		}
		else
		{
			// HORIZONTAL PASS
			// Set vertical pixel shader
			Device->SetPixelShader(HorizontalPassPixelShader);
			// Set input as texture
			Device->SetTexture(0, ((ZED3D9Texture2D*)InputTexture)->Texture);
			// Set output as render target
			LPDIRECT3DSURFACE9 RenderTarget;
			((ZED3D9Texture2D*)Output)->Texture->GetSurfaceLevel(0, &RenderTarget);
			Device->SetRenderTarget(0, RenderTarget);
			// Set Pixel Size
			PixelWidth_2 *= 2;
			Device->SetPixelShaderConstantF(1, &PixelWidth_2, 1);
			// Draw the pass
			Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, ScreenAlignedQuad, 0);
		}
}
