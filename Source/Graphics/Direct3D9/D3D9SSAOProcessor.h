//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - D3D9SSAOProcessor.h
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

#include "D3D9ComponentBase.h"
#include <d3d9.h>
#include <stdlib.h>
#include <freeimage.h>
class ZED3D9SSAOProcessor : public ZED3D9ComponentBase
{
	public:
		LPDIRECT3DTEXTURE9				InputColor;
		LPDIRECT3DTEXTURE9				InputDepth;
		LPDIRECT3DSURFACE9				Output;

		LPDIRECT3DTEXTURE9				RandomTexture;

		LPDIRECT3DVERTEXSHADER9			VertexShader;
		LPDIRECT3DPIXELSHADER9			SSOAShader;
		LPDIRECT3DVERTEXDECLARATION9	VertexDeclaration;

		void Initialize()
		{
			// Vertex Declaration
			/*D3DVERTEXELEMENT9 Declaration[] = 
			{
				{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
				{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
				D3DDECL_END()
			};
			GetDevice()->CreateVertexDeclaration(Declaration, &VertexDeclaration);*/

			srand(GetTickCount());
			// RandomTexture
			FIBITMAP* Bitmap = FreeImage_Allocate(64, 64, 32);
			BYTE* Data = FreeImage_GetBits(Bitmap);
			for (size_t Y = 0; Y < 64; Y++)
				for (size_t X = 0; X < 64; X++)
				{
					RGBQUAD* Quad = (RGBQUAD*)Data + Y * 64 +X;
					float Value = (float)((rand() % (int)(ZE_PIx2 * 10000.0f)) / 10000.0f);
					Quad->rgbRed = (int)((0.5f * cosf(Value) - 0.5f) * 256.0f);
					Quad->rgbGreen = (int)((0.5f * sinf(Value) - 0.5f) * 256.0f);
					Quad->rgbBlue = 0;
					Quad->rgbReserved = 0;				
				}
			FreeImage_Save(FIF_BMP, Bitmap, "c:\\Random.bmp");
			FreeImage_Unload(Bitmap);

		/*	// Compile Shaders
			ZED3D9CommonTools::CompileVertexShaderFromFile(&VertexShader, "Resources/Shaders/SSOAProcessor.hlsl", "VS_Main", "SSOA", "vs_2_0", NULL);
			ZED3D9CommonTools::CompilePixelShaderFromFile(&SSOAShader, "Resources/Shaders/SSOAProcessor.hlsl", "PS_LumMeasureStart", "SSOA", "ps_2_0", NULL);
		*/
		}


		void Deinitialize()
		{
			ZED3D_RELEASE(VertexShader);
			ZED3D_RELEASE(SSOAShader);
			ZED3D_RELEASE(RandomTexture);
		}

		void OnDeviceLost()
		{
		
		}

		void OnDeviceRestored()
		{

		}

		void SetRenderTarget(LPDIRECT3DTEXTURE9 Texture)
		{
			LPDIRECT3DSURFACE9 Surface;
			Texture->GetSurfaceLevel(0, &Surface);
			GetDevice()->SetRenderTarget(0, Surface);
			Surface->Release();
		}

		void Process()
		{
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

			static const float KernelDisc[12][4] =
			{
				{ 0.6487,		-0.111764669,	0.0f, 0.0f},
				{ 0.0923,		 0.586844921,	0.0f, 0.0f},
				{ 0.4026,		-0.185264587,	0.0f, 0.0f},
				{-0.5019,		 0.563529253,	0.0f, 0.0f},
				{-0.6044,		-0.310119152,	0.0f, 0.0f},
				{-0.1893,		 0.311908484,	0.0f, 0.0f},
				{ 0.5923,		-0.726073265,	0.0f, 0.0f},
				{ 0.035603881,	-0.793983221,	0.0f, 0.0f},
				{-0.296554923,	-0.939840794,	0.0f, 0.0f},
				{-0.181980252,	 0.935429335,	0.0f, 0.0f},
				{-0.214773059,	-0.49524498,	0.0f, 0.0f},
				{ 0.895886064,	 0.058031559,	0.0f, 0.0f},
			};

			GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
			GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			GetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
			SetRenderTarget(InputColor);

			GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
			GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			GetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
			SetRenderTarget(InputDepth);

			GetDevice()->SetVertexShader(VertexShader);
			GetDevice()->SetPixelShader(SSOAShader);
			GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(Vert));
		}

		ZED3D9SSAOProcessor()
		{
			VertexShader = NULL;
			SSOAShader = NULL;
			SSOAShader = NULL;
		}

		~ZED3D9SSAOProcessor()
		{
			Deinitialize();
		}

};
