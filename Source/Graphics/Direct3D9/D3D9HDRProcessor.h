//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - D3D9HDRProcessor.h
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
#include "D3D9CommonTools.h"
#include "D3D9ComponentBase.h"
#include "D3D9Module.h"

class ZED3D9HDRProcessor : public ZED3D9ComponentBase
{
	public:
		LPDIRECT3DTEXTURE9 Input;
		LPDIRECT3DSURFACE9 Target;
		size_t ScreenWidth, ScreenHeight;

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
			LPDIRECT3DPIXELSHADER9		VerticalBlur;
			LPDIRECT3DPIXELSHADER9		HorizontalBlur;
			LPDIRECT3DPIXELSHADER9		ToneMap;
		} Shaders;

		ZED3D9HDRProcessor()
		{
			memset(&Textures, 0, sizeof(Textures));
			memset(&Shaders, 0, sizeof(Shaders));
		}

		void DoPass(LPDIRECT3DPIXELSHADER9 Shader, size_t Width, size_t Height)
		{
			float W = 0.5f / Width;
			float H = 0.5f / Height;

			struct  
			{
				float Position[3];
				float Texcoord[2];
			} 
			Vertices[] =
			{
				{{-1.0f - W,  1.0f - H, 0.0f}, {0.0f, 0.0f}},
				{{ 1.0f - W,  1.0f - H, 0.0f}, {1.0f, 0.0f}},
				{{-1.0f - W, -1.0f - H, 0.0f}, {1.0f, 1.0f}},
				{{-1.0f - W, -1.0f - H, 0.0f}, {0.0f, 1.0f}}
			};

			GetDevice()->SetPixelShader(Shader);
			GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, 0);
		}



		void ManageTextures()
		{
			ZED3D9CommonTools::CreateRenderTarget(&Textures.DownSampled2x, 243, 243, ZE_TPF_LUM_HDR);
			ZED3D9CommonTools::CreateRenderTarget(&Textures.DownSampled8xA, 243, 243, ZE_TPF_LUM_HDR);
			ZED3D9CommonTools::CreateRenderTarget(&Textures.DownSampled8xB, 243, 243, ZE_TPF_LUM_HDR);

			ZED3D9CommonTools::CreateRenderTarget(&Textures.Luminance5, 243, 243, ZE_TPF_LUM_HDR);
			ZED3D9CommonTools::CreateRenderTarget(&Textures.Luminance4, 81, 81, ZE_TPF_LUM_HDR);
			ZED3D9CommonTools::CreateRenderTarget(&Textures.Luminance3, 27, 27, ZE_TPF_LUM_HDR);
			ZED3D9CommonTools::CreateRenderTarget(&Textures.Luminance2, 9, 9, ZE_TPF_LUM_HDR);
			ZED3D9CommonTools::CreateRenderTarget(&Textures.Luminance1, 3, 3, ZE_TPF_LUM_HDR);
			ZED3D9CommonTools::CreateRenderTarget(&Textures.Luminance, 1, 1, ZE_TPF_LUM_HDR);
			ZED3D9CommonTools::CreateRenderTarget(&Textures.OldLuminance, 1, 1, ZE_TPF_LUM_HDR);
		}

		void Initialize()
		{
			size_t ScreenWidth, ScreenHeight;

			ZED3D9CommonTools::CompileVertexShaderFromFile(&Shaders.VertexShader, "Resources/Shaders/PostProcessor/VertexShader.vs", "HDR - Pass Vertex Shader", "vs_2_0", NULL);
			ZED3D9CommonTools::CompilePixelShaderFromFile(&Shaders.LumMeasureStart, "Resources/Shaders/PostProcessor/LumMeasureStart.ps", "HDR - Luminance Measure (Start)", "ps_2_0", NULL);
			ZED3D9CommonTools::CompilePixelShaderFromFile(&Shaders.LumDownSample3x, "Resources/Shaders/PostProcessor/LumDownSample4x.ps", "HDR - Luminance Down Sample 4x", "ps_2_0", NULL);
			ZED3D9CommonTools::CompilePixelShaderFromFile(&Shaders.LumMeasureEnd, "Resources/Shaders/PostProcessor/LumMeasureEnd.ps", "HDR - Luminance Measure (End)", "ps_2_0", NULL);
			/*ZED3D9CommonTools::CompilePixelShaderFromFile(&Shaders.BrightPass, "Resources/Shaders/PostProcessor/BrightPass.ps", "HDR - BrightPass", "ps_2_0", NULL);
			ZED3D9CommonTools::CompilePixelShaderFromFile(&Shaders.ColorDownSample4x, "Resources/Shaders/PostProcessor/ColorDownSample4x.ps", "HDR - Color Down Sample 4x", "ps_2_0", NULL);
			ZED3D9CommonTools::CompilePixelShaderFromFile(&Shaders.HorizontalBlur, "Resources/Shaders/PostProcessor/HorizontalBlur.ps", "HDR - Horizontal Blur", "ps_2_0", NULL);
			ZED3D9CommonTools::CompilePixelShaderFromFile(&Shaders.VerticalBlur, "Resources/Shaders/PostProcessor/VerticalBlur.ps", "HDR - Vertical Blur", "ps_2_0", NULL);
			ZED3D9CommonTools::CompilePixelShaderFromFile(&Shaders.ToneMap, "Resources/Shaders/PostProcessor/ToneMap.ps", "HDR - Tone Map", "ps_2_0", NULL);*/

		}

		void Deinitialize()
		{
			#define ZED3D9_RELEASE(Resource) if (Resource != NULL) {Resource->Release(); Resource = NULL;}
				ZED3D9_RELEASE(Shaders.VertexShader);
				ZED3D9_RELEASE(Shaders.ColorDownSample4x);
				ZED3D9_RELEASE(Shaders.LumMeasureStart); 
				ZED3D9_RELEASE(Shaders.LumDownSample3x); 
				ZED3D9_RELEASE(Shaders.LumMeasureEnd);
				ZED3D9_RELEASE(Shaders.BrightPass);
				ZED3D9_RELEASE(Shaders.HorizontalBlur);
				ZED3D9_RELEASE(Shaders.VerticalBlur);
				ZED3D9_RELEASE(Shaders.ToneMap);

				ZED3D9_RELEASE(Textures.DownSampled2x);
				ZED3D9_RELEASE(Textures.DownSampled8xA);
				ZED3D9_RELEASE(Textures.DownSampled8xB);
				ZED3D9_RELEASE(Textures.Luminance5);
				ZED3D9_RELEASE(Textures.Luminance4);
				ZED3D9_RELEASE(Textures.Luminance3);
				ZED3D9_RELEASE(Textures.Luminance2);
				ZED3D9_RELEASE(Textures.Luminance1);
				ZED3D9_RELEASE(Textures.Luminance);
				ZED3D9_RELEASE(Textures.OldLuminance);
			#undef ZED3D9_RELEASE
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

			ManageTextures();

			GetDevice()->SetFVF(D3DFVF_XYZ | D3DFVF_TEX0);
			GetDevice()->SetVertexShader(Shaders.VertexShader);

			// Luminance Measure Start Pass
			GetDevice()->SetTexture(0, Input);
			SetRenderTarget(Textures.Luminance4);
			DoPass(Shaders.LumMeasureStart, 243, 243);

			// Luminance Measure Down Sample 3x (From : 243, To: 81)
			GetDevice()->SetTexture(0, Textures.Luminance3);
			SetRenderTarget(Textures.Luminance3);
			DoPass(Shaders.LumDownSample3x, 81, 81);


			// Luminance Measure Down Sample 3x (From : 81, To: 27)
			GetDevice()->SetTexture(0, Textures.Luminance2);
			SetRenderTarget(Textures.Luminance2);
			DoPass(Shaders.LumDownSample3x, 27, 27);

			// Luminance Measure Down Sample 3x (From : 27, To: 9)
			GetDevice()->SetTexture(0, Textures.Luminance2);
			SetRenderTarget(Textures.Luminance1);
			DoPass(Shaders.LumDownSample3x, 9, 9);

			// Luminance Measure Down Sample 3x (From : 9, To: 3)
			GetDevice()->SetTexture(0, Textures.Luminance2);
			SetRenderTarget(Textures.Luminance1);
			DoPass(Shaders.LumDownSample3x, 3, 3);

			// Luminance Measure End Pass
			GetDevice()->SetTexture(0, Textures.Luminance1);
			GetDevice()->SetTexture(0, Textures.OldLuminance);
			SetRenderTarget(Textures.Luminance);
			DoPass(Shaders.LumMeasureEnd, 1, 1);

			/*// Bright Pass
			GetDevice()->SetTexture(0, Input);
			GetDevice()->SetTexture(1, Textures.Luminance);
			SetRenderTarget(Textures.DownSampled2x);
			DoPass(Shaders.BrightPass, ScreenWidth / 2, ScreenHeight / 2);
			
			// Down Sample 4x
			GetDevice()->SetTexture(0, Textures.DownSampled2x);
			SetRenderTarget(Textures.DownSampled8xA);
			DoPass(Shaders.ColorDownSample4x, ScreenWidth / 8, ScreenHeight / 8);

			// Vertical Blur Pass
			GetDevice()->SetTexture(0, Textures.DownSampled8xA);
			SetRenderTarget(Textures.DownSampled8xB);
			DoPass(Shaders.VerticalBlur, ScreenWidth / 8, ScreenHeight / 8);

			// Horizontal Blur Pass
			GetDevice()->SetTexture(0, Textures.DownSampled8xB);
			SetRenderTarget(Textures.DownSampled8xA);
			DoPass(Shaders.HorizontalBlur, ScreenWidth / 8, ScreenHeight / 8);

			// Tone Mapping Pass
			GetDevice()->SetTexture(0, Textures.Luminance);
			GetDevice()->SetTexture(1, Textures.DownSampled8xB);
			GetDevice()->SetRenderTarget(0, Target);
			DoPass(Shaders.ToneMap, ScreenWidth, ScreenHeight);*/
		}
};
