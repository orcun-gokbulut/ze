//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9PPBlurNode.cpp
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

#include "ZED3D9PPBlurNode.h"
#include "ZEError.h"
#include "../ZED3D9CommonTools.h"
#include "../ZED3D9Texture2D.h"
#include "../ZED3D9Module.h"

LPDIRECT3DVERTEXSHADER9 ZED3D9PPBlurNode::VertexShader = NULL;
LPDIRECT3DPIXELSHADER9 ZED3D9PPBlurNode::VerticalPassPixelShader = NULL;
LPDIRECT3DPIXELSHADER9 ZED3D9PPBlurNode::HorizontalPassPixelShader = NULL;
/*
ZED3D9PPBlurNode::ZED3D9PPBlurNode()
{
	Internal = NULL;
	Output = NULL;
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
					"struct VSOutput"
					"{"
						"float4 Position : POSITION0;"
						"float2 TexCoord : TEXCOORD0;"
					"};"
					"VSOutput vs_main(float4 Position : POSITION0, float2 TexCoord : TEXCOORD0)"
					"{"
						"VSOutput Output;"
						"Output.Position = float4(Position.xy, 0.0f, 1.0f);"
						"Output.TexCoord = TexCoord;"
						"return Output;"
					"}", "Blur Pass", "vs_2_0", NULL)
				)
				return false;
	}
	else
	{
		zeError("Vertex shader is not supported.");
		return false;
	}

	if (GetModule()->GetPixelShaderVersion() == ZE_D3D9_PSV_NONE)
	{
		zeError("Pixel shader is not supported.");
		return false;
	}

 	// Create horizontal blur pass pixel shader
	if (HorizontalPassPixelShader == NULL)
		if (!ZED3D9CommonTools::CompilePixelShader(&HorizontalPassPixelShader, 
				"sampler Input : register(s0);"
				"const float2 Kernel[7] : register(c0);"
				"float4 ps_main(float2 TexCoord : TEXCOORD0) : COLOR0"
				"{"
					"float4 Color = 0.0f;"
					"for (ZEInt I = 0; I < 7; I++)"
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
					"for (ZEInt I = 0; I < 7; I++)"
						"Color += Kernel[I].x * tex2D(Input, float2(TexCoord.x + Kernel[I].y, TexCoord.y));"
					"return Color;"
				"}", "Vertical Blur Pass", "ps_2_0", NULL)
			)
			return false;
	
	return true;
}

void ZED3D9PPBlurNode::Deinitialize()
{
	// If temporary render target created, release it
	if (Internal != NULL)
	{
		Internal->Release();
		Internal = NULL;
	}

	if (Output != NULL)
	{
		Output->Destroy();
		Output = NULL;
	}
}

bool ZED3D9PPBlurNode::Process()
{	
	// Check pass count
	if (PassCount == 0)
		return true;

	// Check shaders
	if (VertexShader == NULL || (HorizontalPass == NULL && VerticalPassPixelShader == NULL))
	{
		zeError("Vertex or/and Pixel shaders are not available");
		return false;
	}

	// Check input post processor is available
	if (Input == NULL)
	{
		zeError("Post processor node does not have any input.");
		return false;
	}

	// Check input node provides valid texture or not
	if (Input->GetOutput() == NULL || Input->GetOutput()->IsEmpty())
	{
		zeError("Post processor input node output texture is not available.");
		return false;
	}

	// Generate output texture
	if (Output ==  NULL)
	{
		Output = ZETexture2D::CreateInstance();
		if (!Output->Create(Input->GetOutput()->GetWidth(), Input->GetOutput()->GetHeight(), Input->GetOutput()->GetPixelFormat(), true))
		{
			zeError("Can not create output texture.");
			return false;
		}
	}
	else
	{
		if (!Output->Create(Input->GetOutput()->GetWidth(), Input->GetOutput()->GetHeight(), Input->GetOutput()->GetPixelFormat(), true))
		{
			zeError("Can not create output texture.");
			return false;
		}
	}

	// Update kernel values. UpdateKernel() function will detect changed properties of Blur Post Effect and 
	// if necessery will regenerate kernel values.
	UpdateKernel();

	// Calculate pixel sizes for calculating texel to pixel conversion
	float PixelWidth_2 = 0.5f / (Input->GetOutput()->GetWidth() * DownSample);
	float PixelHeight_2 = 0.5f / (Input->GetOutput()->GetHeight() * DownSample);
	
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
	GetDevice()->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);							// Vertex Format
	GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);						// No Z-Culling
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);					// No Z-Write
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);				// No Alpha Test
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);				// No Alpha Blending
	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, TRUE);				// Enable Color Write
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);				// No Culling

	GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);		// No Filtering
	GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);		// No filtering
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);	// Texture Adressing in Clamp Mode
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);	// Texture Adressing in Clamp Mode

	// Set Vertex Shader
	GetDevice()->SetVertexShader(VertexShader);

	// Kernel values
	ZEVector4 KernelValues[7];

	LPDIRECT3DSURFACE9 InternalSurface = NULL;
	if ((HorizontalPass && VerticalPass) || PassCount > 1)
	{
		// ZED3D9CommonTools::CreateRenderTarget will check previous render target is available or has correct dimensions. If not it will create one or modify current one.
		if (!ZED3D9CommonTools::CreateRenderTarget(&Internal, Input->GetOutput()->GetWidth() * DownSample, Input->GetOutput()->GetHeight() * DownSample, Input->GetOutput()->GetPixelFormat()))
		{
			zeError("Can not create internal texture.");
			return false;
		}
		Internal->GetSurfaceLevel(0, &InternalSurface);
	}
	else
	{
		Internal->Release();
		Internal = NULL;
	}

	LPDIRECT3DTEXTURE9 InputTexture = ((ZED3D9Texture2D*)Input->GetOutput())->Texture;
	LPDIRECT3DTEXTURE9 OutputTexture = ((ZED3D9Texture2D*)Output)->Texture;
	LPDIRECT3DSURFACE9 OutputSurface;
	OutputTexture->GetSurfaceLevel(0, &OutputSurface);

	// Source and destination
	LPDIRECT3DTEXTURE9 Source;
	LPDIRECT3DSURFACE9 Destination;

	if (HorizontalPass && VerticalPass)
	{
		// Initial source and target asignment
		Source = InputTexture;
		Destination = InternalSurface;

		// HORIZONTAL PASS
		// Set horizontal pixel shader
		GetDevice()->SetPixelShader(HorizontalPassPixelShader);
		
		// Pixel shader constants
		for (ZEInt I = 0; I <= 7; I++)
		{
			KernelValues[I].x = (float)(I - 3) * PixelWidth_2;
			KernelValues[I].y = Kernel[I];
		}
		GetDevice()->SetPixelShaderConstantF(0, (float*)KernelValues, 7);

		GetDevice()->BeginScene();
		// Do the passes
		for (ZEUInt I = 0; I < PassCount; I++)
		{
			// Set source as texture input
			GetDevice()->SetTexture(0, Source);
			// Set destination as render target
			GetDevice()->SetRenderTarget(0, Destination);
			// Draw the pass
			GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, ScreenAlignedQuad, 5 * 4);

			// Change source and destination according to the chain
			Source = (I + 1) % 2 == 1 ? Internal : OutputTexture;
			Destination = (I + 1) % 2 == 1 ? OutputSurface : InternalSurface;
		}

		// VERTICAL PASS
		// Set horizontal pixel shader
		GetDevice()->SetPixelShader(HorizontalPassPixelShader);
		// Pixel shader constants
		for (ZEInt I = 0; I <= 7; I++)
		{
			KernelValues[I].x =  (I - 3) * PixelHeight_2;
			KernelValues[I].y = Kernel[I];
		}
		GetDevice()->SetPixelShaderConstantF(0, (float*)KernelValues, 7);

		// Do the passes
		for (ZEUInt I = 0; I < PassCount; I++)
		{
			// Set source as texture input
			GetDevice()->SetTexture(0, Source);
			// Set destionation as render target
			GetDevice()->SetRenderTarget(0, Destination);
			// Draw the pass
			GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, ScreenAlignedQuad, 5 * 4);

			// Change source and destination according to the chain
			Source = I % 2 == 1 ? Internal : OutputTexture;
			Destination = I % 2 == 1 ? OutputSurface : InternalSurface;
		}
		GetDevice()->EndScene();
	}
	else
	{
		// If making multiple pass create a internal texture
		if (PassCount > 1)
		{
			// Create temporary render target. 		
			// ZED3D9CommonTools::CreateRenderTarget will check previous render target is available or has correct dimensions. If not it will create one or modify current one.
			ZED3D9CommonTools::CreateRenderTarget(&Internal, Input->GetOutput()->GetWidth() * DownSample, Input->GetOutput()->GetHeight() * DownSample, Input->GetOutput()->GetPixelFormat());
		}

		// Set source and destination according to pass count
		Source = InputTexture;
		Destination = PassCount % 2 == 1 ? OutputSurface : InternalSurface;

		if (HorizontalPass)
		{
			// Set horizantal pixel shader
			GetDevice()->SetPixelShader(VerticalPassPixelShader);
			// Set kernel values;
			for (ZEInt I = 0; I <= 7; I++)
			{
				KernelValues[I].x =  (I - 3) * PixelWidth_2;
				KernelValues[I].y = Kernel[I];
			}
			GetDevice()->SetPixelShaderConstantF(0, (float*)KernelValues, 7);
		}
		else
		{
			// Set vertical pixel shader
			GetDevice()->SetPixelShader(VerticalPassPixelShader);
			// Set kernel values;
			for (ZEInt I = 0; I <= 7; I++)
				KernelValues[I].x =  (I - 3) * PixelHeight_2;
			GetDevice()->SetPixelShaderConstantF(0, (float*)KernelValues, 7);
		}

		// Do passes
		GetDevice()->BeginScene();
		for (ZEUInt I = 0; I < PassCount; I++)
		{
			// Set input as texture
			GetDevice()->SetTexture(0, Source);
			// Set output as render target
			GetDevice()->SetRenderTarget(0, Destination);
			// Draw the pass
			GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, ScreenAlignedQuad, 5 * 4);
	

			// Setup next render target and source
			if (PassCount % 2 == 1)
			{
				// If pass count is odd passes
				Source = (I + 1) % 2 == 1 ? OutputTexture : Internal;
				Destination = (I + 1) % 2 == 1 ? InternalSurface : OutputSurface;
			}
			else
			{
				// If pass count is even
				Source = (I + 1) % 2 == 1 ? Internal : OutputTexture;
				Destination  = (I + 1) % 2 == 1 ? OutputSurface : InternalSurface;
			}
		}
		GetDevice()->EndScene();
	}

	if (OutputSurface != NULL)
		OutputSurface->Release();

	if (InternalSurface != NULL)
		InternalSurface->Release();
}

ZETexture2D* ZED3D9PPBlurNode::GetOutput()
{
	return Output;
}*/
