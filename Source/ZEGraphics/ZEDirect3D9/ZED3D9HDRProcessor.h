//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9HDRProcessor.h
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

#include <d3d9.h>
#include "ZED3D9CommonTools.h"
#include "ZED3D9ComponentBase.h"
#include "ZED3D9Module.h"
#include "ZECore\ZECore.h"


#include "ZECore\ZEConsole.h"
class ZED3D9HDRProcessor : public ZED3D9ComponentBase
{
	public:
		LPDIRECT3DTEXTURE9 Input;
		LPDIRECT3DSURFACE9 Target;
		size_t ScreenWidth, ScreenHeight;

		LPDIRECT3DVERTEXDECLARATION9	VertexDeclaration;

		struct 
		{
			LPDIRECT3DTEXTURE9			Luminance5;
			LPDIRECT3DTEXTURE9			Luminance4;
			LPDIRECT3DTEXTURE9			Luminance3;
			LPDIRECT3DTEXTURE9			Luminance2;
			LPDIRECT3DTEXTURE9			Luminance1;
			LPDIRECT3DTEXTURE9			Luminance;
			LPDIRECT3DTEXTURE9			OldLuminance;
			LPDIRECT3DTEXTURE9			DownSampled2x;
			LPDIRECT3DTEXTURE9			DownSampled8xA;
			LPDIRECT3DTEXTURE9			DownSampled8xB;
		} Textures;

		struct
		{
			LPDIRECT3DVERTEXSHADER9		VertexShader;
			LPDIRECT3DPIXELSHADER9		LumMeasureStart;
			LPDIRECT3DPIXELSHADER9		LumDownSample3x;
			LPDIRECT3DPIXELSHADER9		LumMeasureEnd;
			LPDIRECT3DPIXELSHADER9		BrightPass; 
			LPDIRECT3DPIXELSHADER9		ColorDownSample4x;
			LPDIRECT3DPIXELSHADER9		VerticalBloom;
			LPDIRECT3DPIXELSHADER9		HorizontalBloom;
			LPDIRECT3DPIXELSHADER9		ToneMap;
		} Shaders;

		ZED3D9HDRProcessor()
		{
			memset(&Textures, 0, sizeof(Textures));
			memset(&Shaders, 0, sizeof(Shaders));
		}

		void CreateRenderTargets()
		{
			ZED3D9CommonTools::CreateRenderTarget(&Textures.DownSampled2x, ScreenWidth / 2, ScreenHeight / 2, ZE_TPF_RGBA_HDR);
			ZED3D9CommonTools::CreateRenderTarget(&Textures.DownSampled8xA, ScreenWidth / 8, ScreenHeight / 8, ZE_TPF_RGBA_HDR);
			ZED3D9CommonTools::CreateRenderTarget(&Textures.DownSampled8xB, ScreenWidth / 8, ScreenHeight / 8, ZE_TPF_RGBA_HDR);

			ZED3D9CommonTools::CreateRenderTarget(&Textures.Luminance5, 243, 243, ZE_TPF_LUM_HDR);
			ZED3D9CommonTools::CreateRenderTarget(&Textures.Luminance4, 81, 81, ZE_TPF_LUM_HDR);
			ZED3D9CommonTools::CreateRenderTarget(&Textures.Luminance3, 27, 27, ZE_TPF_LUM_HDR);
			ZED3D9CommonTools::CreateRenderTarget(&Textures.Luminance2, 9, 9, ZE_TPF_LUM_HDR);
			ZED3D9CommonTools::CreateRenderTarget(&Textures.Luminance1, 3, 3, ZE_TPF_LUM_HDR);
			ZED3D9CommonTools::CreateRenderTarget(&Textures.Luminance, 1, 1, ZE_TPF_LUM_HDR);
			ZED3D9CommonTools::CreateRenderTarget(&Textures.OldLuminance, 1, 1, ZE_TPF_LUM_HDR);
		}

		void ReleaseRenderTargets()
		{
			ZED3D_RELEASE(Textures.DownSampled2x);
			ZED3D_RELEASE(Textures.DownSampled8xA);
			ZED3D_RELEASE(Textures.DownSampled8xB);
			ZED3D_RELEASE(Textures.Luminance5);
			ZED3D_RELEASE(Textures.Luminance4);
			ZED3D_RELEASE(Textures.Luminance3);
			ZED3D_RELEASE(Textures.Luminance2);
			ZED3D_RELEASE(Textures.Luminance1);
			ZED3D_RELEASE(Textures.Luminance);
			ZED3D_RELEASE(Textures.OldLuminance);
		}

		void OnDeviceLost()
		{
			ReleaseRenderTargets();
		}

		void OnDeviceRestored()
		{

		}

		static float GausianFunction(float x, float StandartDeviation)
		{
			return (1.0f / (sqrtf(2.0f * ZE_PI) * StandartDeviation)) * powf(ZE_E, -((x * x) / (2.0f * StandartDeviation * StandartDeviation)));
		}

		static float GausianFunction(float x, float y, float StandartDeviation)
		{
			return (1.0f / (2.0f * ZE_PI * StandartDeviation * StandartDeviation)) * powf(ZE_E, -((x * x + y * y) / (2.0f * StandartDeviation * StandartDeviation))); 
		}

		static void FillGaussianKernel1D(float* Kernel, size_t KernelSize, float StandartDeviation)
		{
			int HalfKernelSize = (KernelSize - 1) / 2;
			for (int x = 0; x < KernelSize; x++)
			{
				Kernel[4 * x] = x - HalfKernelSize;
				Kernel[4 * x + 1] = GausianFunction(x - HalfKernelSize, StandartDeviation);
				zeOutput("{%lff, %lff}, \r\n", Kernel[4 * x], Kernel[4 * x + 1]);
			}
		}

		static void FillGaussianKernel2D(float* Kernel, size_t KernelSize, float StandartDeviation)
		{
			int HalfKernelSize = (KernelSize - 1) / 2;
			for (int y = 0; y < KernelSize; y++)
			{
				for (int x = 0; x < KernelSize; x++)
				{
					float* CurrentSample = &Kernel[4 * KernelSize * y + x];
					CurrentSample[0] = x - HalfKernelSize;
					CurrentSample[1] = y - HalfKernelSize;
					CurrentSample[2] = GausianFunction(x - HalfKernelSize, y - HalfKernelSize, StandartDeviation);
					zeOutput("%lf ", CurrentSample[2]);
				}
				zeOutput("\r\n");
			}
		}

		void Initialize()
		{
			size_t ScreenWidth, ScreenHeight;
			D3DVERTEXELEMENT9 Declaration[] = 
			{
				{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
				{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
				D3DDECL_END()
			};
			GetDevice()->CreateVertexDeclaration(Declaration, &VertexDeclaration);

			ZED3D9CommonTools::CompileVertexShaderFromFile(&Shaders.VertexShader, "Resources/Shaders/HDRProcessor.hlsl", "VS_Main", "HDR - Pass Vertex Shader", "vs_2_0", NULL);
			ZED3D9CommonTools::CompilePixelShaderFromFile(&Shaders.LumMeasureStart, "Resources/Shaders/HDRProcessor.hlsl", "PS_LumMeasureStart", "HDR - Luminance Measure (Start)", "ps_2_0", NULL);
			ZED3D9CommonTools::CompilePixelShaderFromFile(&Shaders.LumDownSample3x, "Resources/Shaders/HDRProcessor.hlsl", "PS_LumDownSample3x", "HDR - Luminance Down Sample 4x", "ps_2_0", NULL);
			ZED3D9CommonTools::CompilePixelShaderFromFile(&Shaders.LumMeasureEnd, "Resources/Shaders/HDRProcessor.hlsl", "PS_LumMeasureEnd", "HDR - Luminance Measure (End)", "ps_2_0", NULL);
			ZED3D9CommonTools::CompilePixelShaderFromFile(&Shaders.BrightPass, "Resources/Shaders/HDRProcessor.hlsl", "PS_BrightPass", "HDR - BrightPass", "ps_2_0", NULL);
			ZED3D9CommonTools::CompilePixelShaderFromFile(&Shaders.ColorDownSample4x, "Resources/Shaders/HDRProcessor.hlsl", "PS_ColorDownSample4x", "HDR - Color Down Sample 4x", "ps_2_0", NULL);
			ZED3D9CommonTools::CompilePixelShaderFromFile(&Shaders.HorizontalBloom, "Resources/Shaders/HDRProcessor.hlsl", "PS_HorizontalBloom", "HDR - Horizontal Bloom", "ps_3_0", NULL);
			ZED3D9CommonTools::CompilePixelShaderFromFile(&Shaders.VerticalBloom, "Resources/Shaders/HDRProcessor.hlsl", "PS_VerticalBloom", "HDR - Vertical Bloom", "ps_3_0", NULL);
			ZED3D9CommonTools::CompilePixelShaderFromFile(&Shaders.ToneMap, "Resources/Shaders/HDRProcessor.hlsl", "PS_ToneMap", "HDR - Tone Map", "ps_2_0", NULL);
			


		}	
		
		void Deinitialize()
		{
			ZED3D_RELEASE(VertexDeclaration);
			ZED3D_RELEASE(Shaders.VertexShader);
			ZED3D_RELEASE(Shaders.ColorDownSample4x);
			ZED3D_RELEASE(Shaders.LumMeasureStart); 
			ZED3D_RELEASE(Shaders.LumDownSample3x); 
			ZED3D_RELEASE(Shaders.LumMeasureEnd);
			ZED3D_RELEASE(Shaders.BrightPass);
			ZED3D_RELEASE(Shaders.HorizontalBloom);
			ZED3D_RELEASE(Shaders.VerticalBloom);
			ZED3D_RELEASE(Shaders.ToneMap);

			ReleaseRenderTargets();
		}

		void SetRenderTarget(LPDIRECT3DTEXTURE9 Texture)
		{
			LPDIRECT3DSURFACE9 Surface;
			Texture->GetSurfaceLevel(0, &Surface);
			GetDevice()->SetRenderTarget(0, Surface);
			Surface->Release();
		}

		void DoHDR()
		{
			ScreenWidth = GetModule()->GetScreenWidth();
			ScreenHeight = GetModule()->GetScreenHeight();

			CreateRenderTargets();
			static struct Vert  
			{
				float Position[3];
				float Texcoord[2];
			} 
			Vertices[] =
			{
				{{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f}},
				{{ 1.0f,  1.0f, 0.0f}, {1.0f, 0.0f}},
				{{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
				{{ 1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}
			};

			static const float Kernel2x2[4][4] = 
			{
				{-0.5f, -0.5f, 0.0f, 0.0f},
				{-0.5f,  0.5f, 0.0f, 0.0f},

				{ 0.5f, -0.5f, 0.0f, 0.0f},
				{ 0.5f,  0.5f, 0.0f, 0.0f}
			};

			static const float Kernel3x3[9][4] = 
			{
				{-1.0f, -1.0f, 0.0f, 0.0f},
				{-1.0f,  0.0f, 0.0f, 0.0f},
				{-1.0f,  1.0f, 0.0f, 0.0f},

				{ 0.0f, -1.0f, 0.0f, 0.0f},
				{ 0.0f,  0.0f, 0.0f, 0.0f},
				{ 0.0f,  1.0f, 0.0f, 0.0f},

				{ 1.0f, -1.0f, 0.0f, 0.0f},
				{ 1.0f,  0.0f, 0.0f, 0.0f},
				{ 1.0f,  1.0f, 0.0f, 0.0f}
			};

			static const float Kernel4x4[16][4] = 
			{
				{-1.5f, -1.5f, 0.0f, 0.0f},
				{-1.5f, -0.5f, 0.0f, 0.0f},
				{-1.5f,  0.5f, 0.0f, 0.0f},
				{-1.5f,  1.5f, 0.0f, 0.0f},

				{-0.5f, -1.5f, 0.0f, 0.0f},
				{-0.5f, -0.5f, 0.0f, 0.0f},
				{-0.5f,  0.5f, 0.0f, 0.0f},
				{-0.5f,  1.5f, 0.0f, 0.0f},

				{ 0.5f, -1.5f, 0.0f, 0.0f},
				{ 0.5f, -0.5f, 0.0f, 0.0f},
				{ 0.5f,  0.5f, 0.0f, 0.0f},
				{ 0.5f,  1.5f, 0.0f, 0.0f},

				{ 1.5f, -1.5f, 0.0f, 0.0f},
				{ 1.5f, -0.5f, 0.0f, 0.0f},
				{ 1.5f,  0.5f, 0.0f, 0.0f},
				{ 1.5f,  1.5f, 0.0f, 0.0f}
			};

			static const float BloomKernel[15][4] = 
			{
				{-7.000000f, 0.008741f, 0.0f, 0.0f}, 
				{-6.000000f, 0.017997f, 0.0f, 0.0f}, 
				{-5.000000f, 0.033159f, 0.0f, 0.0f}, 
				{-4.000000f, 0.054670f, 0.0f, 0.0f}, 
				{-3.000000f, 0.080657f, 0.0f, 0.0f}, 
				{-2.000000f, 0.106483f, 0.0f, 0.0f}, 
				{-1.000000f, 0.125794f, 0.0f, 0.0f}, 
				{0.000000f, 0.132981f, 0.0f, 0.0f}, 
				{1.000000f, 0.125794f, 0.0f, 0.0f}, 
				{2.000000f, 0.106483f, 0.0f, 0.0f}, 
				{3.000000f, 0.080657f, 0.0f, 0.0f}, 
				{4.000000f, 0.054670f, 0.0f, 0.0f}, 
				{5.000000f, 0.033159f, 0.0f, 0.0f}, 
				{6.000000f, 0.017997f, 0.0f, 0.0f}, 
				{7.000000f, 0.008741f, 0.0f, 0.0f}, 
			};
			struct 
			{
				float Exposure;
				float BrightPassTreshold;
				float BrightPassOffset;
				float BloomFactor;
				float MaxLimunance;
				float MinLimunance;
				float ElapsedTime;
				float Reserved0; 
			} Parameters;

			Parameters.Exposure = 0.78f;
			Parameters.BrightPassTreshold = 0.5f;
			Parameters.BrightPassOffset = 0.2f;
			Parameters.BloomFactor = 1.5f;
			Parameters.MaxLimunance = 2.0f;

			Parameters.MinLimunance = 0.2f;
			Parameters.ElapsedTime = zeCore->GetFrameTime();

			GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
			GetDevice()->SetVertexDeclaration(VertexDeclaration);
			GetDevice()->SetVertexShader(Shaders.VertexShader);

			GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

			GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			GetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);



			GetDevice()->SetPixelShaderConstantF(1, (const float*)&Parameters, 2);
			GetDevice()->SetPixelShaderConstantF(10, (const float*) Kernel2x2, 4);
			GetDevice()->SetPixelShaderConstantF(15, (const float*) Kernel3x3, 9);

			// Luminance Measure Start Pass
			SetRenderTarget(Textures.Luminance5);
			GetDevice()->SetTexture(0, Input);
			GetDevice()->SetPixelShader(Shaders.LumMeasureStart);
			GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / 243.0f, 1.0f / 243.0f, 0.0f, 0.0f), 1);
			GetDevice()->SetPixelShaderConstantF(0, (const float*)&ZEVector4(1.0f / ScreenWidth, 1.0f / ScreenHeight, 0.0f, 0.0f), 1);
			GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));
			
			// Luminance Measure Down Sample 3x (From : 243, To: 81)
			GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
			GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);

			SetRenderTarget(Textures.Luminance4);
			GetDevice()->SetTexture(0, Textures.Luminance5);
			GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / 81.0f, 1.0f / 81.0f, 0.0f, 0.0f), 1);
			GetDevice()->SetPixelShaderConstantF(0, (const float*)&ZEVector4(1.0f / 243.0f, 1.0f / 243.0f, 0.0f, 0.0f), 1);
			GetDevice()->SetPixelShader(Shaders.LumDownSample3x);
			GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));

			// Luminance Measure Down Sample 3x (From : 81, To: 27)
			SetRenderTarget(Textures.Luminance3);
			GetDevice()->SetTexture(0, Textures.Luminance4);
			GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / 27.0f, 1.0f / 27.0f, 0.0f, 0.0f), 1);
			GetDevice()->SetPixelShaderConstantF(0, (const float*)&ZEVector4(1.0f / 81.0f, 1.0f / 81.0f, 0.0f, 0.0f), 1);
			GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));

			// Luminance Measure Down Sample 3x (From : 27, To: 9)
			SetRenderTarget(Textures.Luminance2);
			GetDevice()->SetTexture(0, Textures.Luminance3);
			GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / 9.0f, 1.0f / 9.0f, 0.0f, 0.0f), 1);
			GetDevice()->SetPixelShaderConstantF(0, (const float*)&ZEVector4(1.0f / 27.0f, 1.0f / 27.0f, 0.0f, 0.0f), 1);
			GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));

			// Luminance Measure Down Sample 3x (From : 9, To: 3)
			SetRenderTarget(Textures.Luminance1);
			GetDevice()->SetTexture(0, Textures.Luminance2);
			GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / 3.0f, 1.0f / 3.0f, 0.0f, 0.0f), 1);
			GetDevice()->SetPixelShaderConstantF(0, (const float*)&ZEVector4(1.0f / 9.0f, 1.0f / 9.0f, 0.0f, 0.0f), 1);
			GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));

			LPDIRECT3DTEXTURE9 TempTexture;
			TempTexture = Textures.OldLuminance;
			Textures.OldLuminance = Textures.Luminance;
			Textures.Luminance = TempTexture;

			// Luminance Measure End Pass
			SetRenderTarget(Textures.Luminance);
			GetDevice()->SetTexture(0, Textures.Luminance1);
			GetDevice()->SetTexture(3, Textures.OldLuminance);
			GetDevice()->SetPixelShader(Shaders.LumMeasureEnd);
			GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f, 1.0f, 0.0f, 0.0f), 1);
			GetDevice()->SetPixelShaderConstantF(0, (const float*)&ZEVector4(1.0f / 3.0f, 1.0f / 3.0f, 0.0f, 0.0f), 1);
			GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));

			// Bright Pass
			SetRenderTarget(Textures.DownSampled2x);
			GetDevice()->SetTexture(0, Input);
			GetDevice()->SetTexture(1, Textures.Luminance);
			GetDevice()->SetPixelShader(Shaders.BrightPass);
			GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / ScreenWidth, 1.0f / ScreenHeight, 0.0f, 0.0f), 1);
			GetDevice()->SetPixelShaderConstantF(0, (const float*)&ZEVector4(1.0f / (ScreenHeight / 2.0f), 1.0f / (ScreenHeight / 2.0f), 0.0f, 0.0f), 1);
			GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));

			// Down Sample 4x
			SetRenderTarget(Textures.DownSampled8xA);
			GetDevice()->SetPixelShaderConstantF(15, (const float*) Kernel4x4, 16);
			GetDevice()->SetTexture(0, Textures.DownSampled2x);
			GetDevice()->SetPixelShader(Shaders.ColorDownSample4x);
			GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / (ScreenWidth / 2.0f), 1.0f / (ScreenWidth / 2.0f), 0.0f, 0.0f), 1);
			GetDevice()->SetPixelShaderConstantF(0, (const float*)&ZEVector4 (1.0f / (ScreenWidth / 8.0f), 1.0f / (ScreenHeight / 8.0f), 0.0f, 0.0f), 1);
			GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));

			// Vertical Bloom Pass
			SetRenderTarget(Textures.DownSampled8xB);
			GetDevice()->SetPixelShaderConstantF(15, (const float*)BloomKernel, 15);
			GetDevice()->SetTexture(0, Textures.DownSampled8xA);
			GetDevice()->SetPixelShader(Shaders.VerticalBloom); 
			GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4(1.0f / (ScreenWidth / 8.0f), 1.0f / (ScreenWidth / 8.0f), 0.0f, 0.0f), 1);
			GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));

			// Horizontal Bloom Pass
			SetRenderTarget(Textures.DownSampled8xA);
			GetDevice()->SetTexture(0, Textures.DownSampled8xB);
			GetDevice()->SetPixelShader(Shaders.HorizontalBloom);
			GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));

			// Tone Mapping Pass
			GetDevice()->SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			GetDevice()->SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
			GetDevice()->SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			GetDevice()->SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			GetDevice()->SetSamplerState(2, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

			GetDevice()->SetRenderTarget(0, Target);
			GetDevice()->SetTexture(0, Input);
			GetDevice()->SetTexture(1, Textures.Luminance);
			GetDevice()->SetTexture(2, Textures.DownSampled8xB);
			GetDevice()->SetPixelShader(Shaders.ToneMap);
			GetDevice()->SetVertexShaderConstantF(0, (const float*)&ZEVector4 (1.0f / ScreenWidth, 1.0f / ScreenHeight, 0.0f, 0.0f), 1);
			GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));
		}
};
