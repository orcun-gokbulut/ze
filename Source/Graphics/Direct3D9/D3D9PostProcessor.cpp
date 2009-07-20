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
#include "Direct3D9Module.h"
#include "Core/Error.h"
#include "D3D9Texture.h"
#include "D3D9Common.h"

LPDIRECT3DVERTEXSHADER9 ZED3D9PostProcessor::GeneralVS;
LPDIRECT3DPIXELSHADER9 ZED3D9PostProcessor::ColorTransformPS = NULL;
LPDIRECT3DPIXELSHADER9 ZED3D9PostProcessor::InversePS = NULL;
LPDIRECT3DPIXELSHADER9 ZED3D9PostProcessor::GrayscalePS = NULL;
LPDIRECT3DPIXELSHADER9 ZED3D9PostProcessor::DownSample4xPS = NULL;
LPDIRECT3DPIXELSHADER9 ZED3D9PostProcessor::UpSample4xPS = NULL;
LPDIRECT3DPIXELSHADER9 ZED3D9PostProcessor::HBlurPS = NULL; 
LPDIRECT3DPIXELSHADER9 ZED3D9PostProcessor::VBlurPS = NULL;
LPDIRECT3DPIXELSHADER9 ZED3D9PostProcessor::SharpenPS = NULL; 
LPDIRECT3DPIXELSHADER9 ZED3D9PostProcessor::HEdgePS = NULL;
LPDIRECT3DPIXELSHADER9 ZED3D9PostProcessor::VEdgePS = NULL;
LPDIRECT3DPIXELSHADER9 ZED3D9PostProcessor::DirectPS = NULL;
LPDIRECT3DVERTEXBUFFER9 ZED3D9PostProcessor::ScreenQuadVB = NULL;
LPDIRECT3DVERTEXDECLARATION9 ZED3D9PostProcessor::ScreenQuadVD = NULL;

bool ZED3D9PostProcessor::BaseInitialize()
{	
	D3DVERTEXELEMENT9 VertexDecl[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		D3DDECL_END()
	};
	Device->CreateVertexDeclaration(VertexDecl, &ScreenQuadVD);

	if (Device->CreateVertexBuffer(32, NULL, NULL, D3DPOOL_MANAGED, &ScreenQuadVB, NULL) != D3D_OK)
	{
		zeError("D3D9 Post Processor", "Can not create post effect vertex buffer.");
		return false;
	}

	ZEVector2* Vertices;
	ScreenQuadVB->Lock(0, NULL, (void**)&Vertices, NULL);
	Vertices[0].Create(-1.0f, -1.0f);
	Vertices[1].Create(-1.0f, 1.0f);
	Vertices[2].Create(1.0f, -1.0f);
	Vertices[3].Create(1.0f, 1.0f);
	ScreenQuadVB->Unlock();

	CompileVertexShader(
		"float2 InverseViewportDimentsions : register(c0);"
		"struct VS_OUTPUT"
		"{"
			"float4 Position : POSITION0;"
			"float2 TexCoord : TEXCOORD0;"
		"};"
		"VS_OUTPUT vs_main(float4 Position : POSITION0)"
		"{"
			"VS_OUTPUT Output;"
			"Output.Position = float4(Position.xy, 0.0f, 1.0f);"
			"Output.TexCoord.x = 0.5f * (1.0f + Output.Position.x + InverseViewportDimentsions.x);"
			"Output.TexCoord.y = 0.5f * (1.0f - Output.Position.y + InverseViewportDimentsions.y);"
			"return Output;"
		"}",
		&GeneralVS);

	CompilePixelShader(
		"sampler Screen : register(s0);"
		"float4 ps_main(float2 TexCoord : TEXCOORD0) : COLOR0"
		"{" 
		"float4 Color = tex2D(Screen, TexCoord);"
			"return float4(dot(float3(0.299f, 0.587f, 0.114f), Color.xyz).xxx, Color.w);"
		"}",
		&GrayscalePS);

	CompilePixelShader(
		"sampler Screen : register(s0);"
		"float4 ps_main(float2 TexCoord : TEXCOORD0) : COLOR0"
		"{"  
			"return tex2D(Screen, TexCoord);"
		"}",
		&DirectPS);
	
	CompilePixelShader(
		"sampler Screen : register(s0);"
		"float4 ps_main(float2 TexCoord : TEXCOORD0) : COLOR0"
		"{"  
			"return 1.0f - tex2D(Screen, TexCoord);"
		"}",
		&InversePS);

	#pragma message("Task : Modify post processor downsample.")
	/*CompilePixelShader(
		"sampler Screen : register(s0);"
		"float3x3 Transformation : register(s1);"
		"float4 ps_main(float2 TexCoord : TEXCOORD0) : COLOR0"
		"{"  
			"return mul(Transformation, tex2D(Screen, TexCoord));"
		"}",
		&ColorTransformPS);*/

	CompilePixelShader(
		"sampler Screen : register(s0);"
		"float2 InverseViewportDimensions : register(c0);"
		"static const int KernelSize = 13;"
		"static const float2 Kernel[KernelSize] ="
		"{"
			"{-6.0f, 0.002216f},"
			"{-5.0f, 0.008764f},"
			"{-4.0f, 0.026995f},"
			"{-3.0f, 0.064759f},"
			"{-2.0f, 0.120985f},"
			"{-1.0f, 0.176033f},"
			"{0.0f, 0.199471f},"
			"{1.0f, 0.176033f},"
			"{2.0f, 0.120985f},"
			"{3.0f, 0.064759f},"
			"{4.0f, 0.026995f},"
			"{5.0f, 0.008764f},"
			"{6.0f, 0.002216f},"
		"};"
		"float4 ps_main(float2 TexCoord : TEXCOORD0) : COLOR0"
		"{"
			"float4 Color = 0.0f;"
			"for (int I = 0; I < KernelSize; I++)"
				"Color += Kernel[I].y * tex2D(Screen, float2(TexCoord.x, TexCoord.y + Kernel[I].x * InverseViewportDimensions.y));"
			"return Color;"
		"}", &VBlurPS);

	CompilePixelShader(
		"sampler Screen : register(s0);"
		"float2 InverseViewportDimensions : register(c0);"
		"static const int KernelSize = 13;"
		"static const float2 Kernel[KernelSize] ="
		"{"
			"{-6.0f, 0.002216f},"
			"{-5.0f, 0.008764f},"
			"{-4.0f, 0.026995f},"
			"{-3.0f, 0.064759f},"
			"{-2.0f, 0.120985f},"
			"{-1.0f, 0.176033f},"
			"{0.0f, 0.199471f},"
			"{1.0f, 0.176033f},"
			"{2.0f, 0.120985f},"
			"{3.0f, 0.064759f},"
			"{4.0f, 0.026995f},"
			"{5.0f, 0.008764f},"
			"{6.0f, 0.002216f},"
		"};"
		"float4 ps_main(float2 TexCoord : TEXCOORD0) : COLOR0"
		"{"
			"float4 Color = 0.0f;"
			"for (int I = 0; I < KernelSize; I++)"
				"Color += Kernel[I].y * tex2D(Screen, float2(TexCoord.x + Kernel[I].x * InverseViewportDimensions.x, TexCoord.y));"
			"return Color;"
		"}", &HBlurPS);

	CompilePixelShader(
		"sampler Screen : register(s0);"
		"float2 InverseViewportDimensions : register(c0);"
		"static const int KernelSize = 6;"
		"static const float3 Kernel[KernelSize] ="
		"{"
			"{-1.0f, 1.0f, 1.0f},"
			"{ 0.0f, 1.0f, 2.0f},"
			"{ 1.0f, 1.0f, 1.0f},"
			"{-1.0f, -1.0f, -1.0f},"
			"{ 0.0f, -1.0f, -2.0f},"
			"{ 1.0f, -1.0f, -1.0f},"
		"};"
		"float4 ps_main(float2 Texcoord : TEXCOORD0) : COLOR0"
		"{"
			"float4 Color = 0.0f;"
			"for (int I = 0; I < KernelSize; I++)"
				"Color += Kernel[I].z * tex2D(Screen, float2(Texcoord.x + Kernel[I].x * InverseViewportDimensions.x, Texcoord.y + Kernel[I].y * InverseViewportDimensions.y));"
			"return Color;"
		"}", &HEdgePS);

	CompilePixelShader(
		"sampler Screen : register(s0);"
		"float2 InverseViewportDimensions : register(c0);"
		"static const int KernelSize = 6;"
		"static const float3 Kernel[KernelSize] ="
		"{"
			"{-1.0f, 1.0f, 1.0f},"
			"{ 0.0f, 1.0f, 2.0f},"
			"{ 1.0f, 1.0f, 1.0f},"
			"{-1.0f, -1.0f, -1.0f},"
			"{ 0.0f, -1.0f, -2.0f},"
			"{ 1.0f, -1.0f, -1.0f},"
		"};"
		"float4 ps_main(float2 Texcoord : TEXCOORD0) : COLOR0"
		"{"
			"float4 Color = 0.0f;"
			"for (int I = 0; I < KernelSize; I++)"
				"Color += Kernel[I].z * tex2D(Screen, float2(Texcoord.x + Kernel[I].y * InverseViewportDimensions.x, Texcoord.y + Kernel[I].x * InverseViewportDimensions.y));"
			"return Color;"
		"}", &VEdgePS);

	
	CompilePixelShader(
		"sampler Screen : register(s0);"
		"float2 InverseViewportDimensions : register(c0);"
		"static const int KernelSize = 5;"
		"static const float3 Kernel[KernelSize] ="
		"{"
			"{ 0.0f, 0.0f, 11.0f / 3.0f},"
			"{-1.0f, -1.0f, -2.0f  / 3.0f},"
			"{-1.0f, 1.0f, -2.0f  / 3.0f},"
			"{ 1.0f, -1.0f, -2.0f  / 3.0f},"
			"{ 1.0f, 1.0f, -2.0f  / 3.0f},"
		"};"
		"float4 ps_main(float2 Texcoord : TEXCOORD0) : COLOR0"
		"{"
			"float4 Color = 0.0f;"
			"for (int I = 0; I < KernelSize; I++)"
				"Color += Kernel[I].z * tex2D(Screen, float2(Texcoord.x + Kernel[I].x * InverseViewportDimensions.x, Texcoord.y + Kernel[I].y * InverseViewportDimensions.y));"
			"return Color;"
		"}", &SharpenPS);

#pragma message("Task : Modify post processor downsample.")

/*	CompilePixelShader(
		"sampler Screen : register(s0);"
		"float2 InverseViewportDimensions : register(c0);"
		"static const int KernelSize = 16;"
		"static float2 Kernel[16] ="
		"{"
			"{1.5f, -1.5f},"
			"{1.5f, -0.5f},"
			"{1.5f, 0.5f},"
			"{1.5f, 1.5f},"
			"{0.5f, -1.5f},"
			"{0.5f, -0.5f},"
			"{0.5f, 0.5f},"
			"{0.5f, 1.5f},"
			"{-0.5f, -1.5f},"
			"{-0.5f, -0.5f},"
			"{-0.5f, 0.5f},"
			"{-0.5f, 1.5f},"
			"{-1.5f, -1.5f},"
			"{-1.5f, -0.5f},"
			"{-1.5f, 0.5f},"
			"{-1.5f, 1.5f},"
		"};"
		"float4 ps_main(float2 Texcoord : TEXCOORD0 ) : COLOR0"
		"{"
			"float4 Color = 0;"
			"for (int i = 0; i < KernelSize; i++)"
				"Color += tex2D( Screen, Tex + TexelCoordsDownFilter[i].xy );"
			"return Color / KernelSize;"
		"}", &DownSample4xPS);*/

	CompilePixelShader(
		"sampler Screen : register(s0);"
		"float2 InverseViewportDimensions : register(c0);"
		"float4 ps_main(float2 Texcoord : TEXCOORD0) : COLOR0"
		"{"
			"return tex2D(Screen, Texcoord);"
		"}", &UpSample4xPS);

	return true;
}

void ZED3D9PostProcessor::SetInput(ZETexture* Texture)
{
	Source = ((ZED3D9Texture*)Texture)->Texture;
}

void ZED3D9PostProcessor::SetOutput(ZETexture* Texture)
{
	Destination = ((ZED3D9Texture*)Texture)->Texture;
}

void ZED3D9PostProcessor::DeviceLost()
{

}

bool ZED3D9PostProcessor::DeviceRestored()
{
	return false;
}

void ZED3D9PostProcessor::BaseDeinitialize()
{
	ZED3D_RELEASE(GeneralVS);
	ZED3D_RELEASE(ColorTransformPS);
	ZED3D_RELEASE(InversePS);
	ZED3D_RELEASE(GrayscalePS);
	ZED3D_RELEASE(DownSample4xPS);
	ZED3D_RELEASE(UpSample4xPS);
	ZED3D_RELEASE(HBlurPS);
	ZED3D_RELEASE(VBlurPS);
	ZED3D_RELEASE(SharpenPS);
	ZED3D_RELEASE(HEdgePS);
	ZED3D_RELEASE(VEdgePS);
	ZED3D_RELEASE(GeneralVS);
	ZED3D_RELEASE(ScreenQuadVD);
	ZED3D_RELEASE(ScreenQuadVB);
}

bool ZED3D9PostProcessor::ManageSourceDestination(ZEPostProcessorSource Source_, ZEPostProcessorDestination Destination_, float Scale)
{
	D3DSURFACE_DESC SrcDesc, DestDesc;
	LPDIRECT3DSURFACE9 Surface;
	LPDIRECT3DTEXTURE9 InternalSwap;

	switch(Source_)
	{
		case ZE_PPS_INPUT:
			if (Source == NULL)
			{
				zeError("D3D9 Post Processor", "Source is not valid. There is no input texture.");
				return false;
			}

			Source->GetLevelDesc(0, &SrcDesc);
			Device->SetTexture(0, Source);
			break;

		case ZE_PPS_INTERNAL:
			InternalSwap = InternalSource;
			InternalSource = InternalDestination;
			InternalDestination = InternalSwap;

			if (InternalSource == NULL)
			{
				zeError("D3D9 Post Processor", "Destination is not valid. There is no internal texture.");
				return false;
			}

			InternalSource->GetLevelDesc(0, &SrcDesc);
			Device->SetTexture(0, InternalSource);
			break;

		default:
			zeError("D3D9 Post Processor", "Wrong source.");
			return false;
			break;
	}

	switch(Destination_)
	{
		case ZE_PPD_OUTPUT:
			if (Destination == NULL)
			{
				zeError("D3D9 Post Processor", "Destination is not valid.");
				return false;
			}
			
			Destination->GetLevelDesc(0, &DestDesc);
			Destination->GetSurfaceLevel(0, &Surface);
			Device->SetRenderTarget(0, Surface);		
			Device->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / DestDesc.Width, 1.0f / DestDesc.Height, 0.0f, 0.0f), 1);
			break;

		case ZE_PPD_INTERNAL:
			if (InternalDestination == NULL)
				Device->CreateTexture((UINT)(SrcDesc.Width * Scale), (UINT)(SrcDesc.Height * Scale), 0, D3DUSAGE_RENDERTARGET, SrcDesc.Format, D3DPOOL_DEFAULT, &InternalDestination, NULL);
			else 
			{
				InternalDestination->GetLevelDesc(0, &DestDesc);
				if (DestDesc.Width != SrcDesc.Width * Scale || DestDesc.Height!= SrcDesc.Height * Scale ||DestDesc.Format != SrcDesc.Format)
				{
					InternalDestination->Release();
					Device->CreateTexture(SrcDesc.Width * Scale, SrcDesc.Height * Scale, 0, D3DUSAGE_RENDERTARGET, SrcDesc.Format, D3DPOOL_DEFAULT, &InternalDestination, NULL);
				}
			}

			Device->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / (SrcDesc.Width * Scale), 1.0f / (SrcDesc.Height * Scale), 0.0f, 0.0f), 1);
			Device->SetPixelShaderConstantF(0, (const float*)&ZEVector4(1.0f / (SrcDesc.Width * Scale), 1.0f / (SrcDesc.Height * Scale), 0.0f, 0.0f), 1);
			InternalDestination->GetSurfaceLevel(0, &Surface);
			Device->SetRenderTarget(0, Surface);
			break;

		case ZE_PPD_FRAMEBUFFER:
			Device->SetRenderTarget(0, Module->FrameColorBuffer); //  ZEDirect3D9Module::GetD3D9Module()->FrameColorBuffer
			Device->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / Module->GetScreenWidth(), 1.0f / Module->GetScreenHeight(), 0.0f, 0.0f), 1);
			Device->SetPixelShaderConstantF(0, (const float*)&ZEVector4(1.0f / Module->GetScreenWidth(), 1.0f / Module->GetScreenHeight(), 0.0f, 0.0f), 1);
			break;

		default:
			zeError("D3D9 Post Processor", "Wrong source.");
			return false;
			break;
	}
	return true;
};

void ZED3D9PostProcessor::DrawPostEffect(LPDIRECT3DPIXELSHADER9 PixelShader)
{
	Device->SetRenderState(D3DRS_ZENABLE, FALSE);
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	Device->SetVertexShader(GeneralVS);
	Device->SetPixelShader(PixelShader);

	Device->SetStreamSource(0, ScreenQuadVB, 0, 8);
	Device->SetVertexDeclaration(ScreenQuadVD);

	Device->BeginScene();
	Device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	Device->EndScene();
}

void ZED3D9PostProcessor::ApplyInverse(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination)
{
	if (ManageSourceDestination(Source, Destination, 1.0f))
		DrawPostEffect(InversePS);
}

void ZED3D9PostProcessor::ApplyGrayscale(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination)
{
	if (ManageSourceDestination(Source, Destination, 1.0f))
		DrawPostEffect(GrayscalePS);
}

void ZED3D9PostProcessor::ApplyColorTransform(ZEPostProcessorSource Source, ZEMatrix3x3 Matrix, ZEPostProcessorDestination Destination)
{
	if (ManageSourceDestination(Source, Destination, 1.0f));
}

void ZED3D9PostProcessor::DirectOutput(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination)
{
	if (ManageSourceDestination(Source, Destination, 1.0f))
		DrawPostEffect(DirectPS);
}

void ZED3D9PostProcessor::ApplyDownSample4x(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination)
{
}

void ZED3D9PostProcessor::ApplyUpSample4x(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination)
{
	if (ManageSourceDestination(Source, Destination, 1.0f))
		DrawPostEffect(DirectPS);
}

void ZED3D9PostProcessor::ApplyBlurV(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination)
{
	if (ManageSourceDestination(Source, Destination, 1.0f))
		DrawPostEffect(VBlurPS);
}

void ZED3D9PostProcessor::ApplyBlurH(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination)
{
	if (ManageSourceDestination(Source, Destination, 1.0f))
		DrawPostEffect(HBlurPS);
}

void ZED3D9PostProcessor::ApplySharpen(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination)
{
	if (ManageSourceDestination(Source, Destination, 1.0f))
		DrawPostEffect(SharpenPS);
}

void ZED3D9PostProcessor::ApplyEdgeDetectionH(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination)
{
	if (ManageSourceDestination(Source, Destination, 1.0f))
		DrawPostEffect(HEdgePS);
}

void ZED3D9PostProcessor::ApplyEdgeDetectionV(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination)
{
	if (ManageSourceDestination(Source, Destination, 1.0f))
		DrawPostEffect(VEdgePS);
}

ZED3D9PostProcessor::ZED3D9PostProcessor()
{
	Source = NULL;
	InternalSource = NULL;
	InternalDestination = NULL;
	Destination = NULL;
}

ZED3D9PostProcessor::~ZED3D9PostProcessor()
{
	ZED3D_RELEASE(Source);
	ZED3D_RELEASE(InternalSource);
	ZED3D_RELEASE(InternalDestination);
	ZED3D_RELEASE(Destination);
}
