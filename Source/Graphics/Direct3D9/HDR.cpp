//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - HDR.cpp
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
class HDR
{
	LPDIRECT3DDEVICE9 Device;
	LPDIRECT3DTEXTURE9 Input;
	LPDIRECT3DSURFACE9 Target;
	size_t ScreenWidth, ScreenHeight;

	struct 
	{
		LPDIRECT3DTEXTURE9			DownSampled2x;
		LPDIRECT3DTEXTURE9			DownSampled8xA;
		LPDIRECT3DTEXTURE9			DownSampled8xB;
		LPDIRECT3DTEXTURE9			Luminance4;
		LPDIRECT3DTEXTURE9			Luminance3;
		LPDIRECT3DTEXTURE9			Lumunance2;
		LPDIRECT3DTEXTURE9			Luminance1;
		LPDIRECT3DTEXTURE9			Luminance;
		LPDIRECT3DTEXTURE9			OldLuminance;
	} Textures;

	struct
	{
		LPDIRECT3DVERTEXSHADER9		VertexShader;
		LPDIRECT3DPIXELSHADER9		ColorDownSample4x;
		LPDIRECT3DPIXELSHADER9		LumMeasureStart;
		LPDIRECT3DPIXELSHADER9		LumDownSample4x;
		LPDIRECT3DPIXELSHADER9		LumMeasureEnd;
		LPDIRECT3DPIXELSHADER9		BrightPass; 
		LPDIRECT3DPIXELSHADER9		VerticalBlur;
		LPDIRECT3DPIXELSHADER9		HorizontalBlur;
		LPDIRECT3DPIXELSHADER9		ToneMap;
	} Shaders;

	void DoPass(LPDIRECT3DPIXELSHADER9 Shader, size_t Width, size_t Height)
	{
		float W = 0.5f / Width;
		float H = 0.5f / Height;

		struct  
		{
			float Position[3];
			float Texcoord[2];
		} 
		Vertices =
		{
			{{-1.0f - W,  1.0f - H, 0.0f}, {0.0f, 0.0f}},
			{{ 1.0f - W,  1.0f - H, 0.0f}, {1.0f, 0.0f}},
			{{-1.0f - W, -1.0f - H, 0.0f}, {1.0f, 1.0f}},
			{{-1.0f - W, -1.0f - H, 0.0f}, {0.0f, 1.0f}}
		};

		Device->SetPixelShader(Shader);
		Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, 0);
	}



	void Initialize()
	{
		size_t ScreenWidth, ScreenHeight;

		ZED3D9CommonTools::CompileVertexShader(Shaders.VertexShader, "Shaders/PostProcessor/VertexShader.vs", "HDR - Pass Vertex Shader", "VS_2_0", NULL);
		ZED3D9CommonTools::CompilePixelShader(Shaders.ColorDownSample4x, "Shaders/PostProcessor/ColorDownSample4x.ps", "HDR - Color Down Sample 4x", NULL);
		ZED3D9CommonTools::CompilePixelShader(Shaders.LumMeasureStart, "Shaders/PostProcessor/LumMeasureStart.ps", "HDR - Luminance Measure (Start)", "PS_2_0", NULL);
		ZED3D9CommonTools::CompilePixelShader(Shaders.LumDownSample4x, "Shaders/PostProcessor/LumDownSample4x.ps", "HDR - Luminance Down Sample 4x", "PS_2_0", NULL);
		ZED3D9CommonTools::CompilePixelShader(Shaders.LumMeasureEnd, "Shaders/PostProcessor/LumMeasureEnd.ps", "HDR - Luminance Measure (End)", "PS_2_0", NULL);
		ZED3D9CommonTools::CompilePixelShader(Shaders.BrightPass, "Shaders/PostProcessor/BrightPass.ps", "HDR - BrightPass", "PS_2_0", NULL);
		ZED3D9CommonTools::CompilePixelShader(Shaders.HorizontalBlur, "Shaders/PostProcessor/HorizontalBlur.ps", "HDR - Horizontal Blur", "PS_2_0", NULL);
		ZED3D9CommonTools::CompilePixelShader(Shaders.VerticalBlur, "Shaders/PostProcessor/VerticalBlur.ps", "HDR - Vertical Blur", "PS_2_0", NULL);
		ZED3D9CommonTools::CompilePixelShader(Shaders.ToneMap, "Shaders/PostProcessor/ToneMap.ps", "HDR - Tone Map", "PS_2_0", NULL);

		ZED3D9CommonTools::CreateRenderTarget(DownSampled8xA, ScreenWidth / 8, ScreenHeight / 8, ZE_TPF_RGBA_HDR);
		ZED3D9CommonTools::CreateRenderTarget(DownSampled8xA, ScreenWidth / 4, ScreenHeight / 4, ZE_TPF_RGBA_HDR);
		ZED3D9CommonTools::CreateRenderTarget(DownSampled8xB, ScreenWidth / 4, ScreenHeight / 4, ZE_TPF_RGBA_HDR);
		ZED3D9CommonTools::CreateRenderTarget(Luminance4, 256, 256, ZE_TPF_LUM_HDR);
		ZED3D9CommonTools::CreateRenderTarget(Luminance3, 64, 64, ZE_TPF_LUM_HDR);
		ZED3D9CommonTools::CreateRenderTarget(Lumunance2, 16, 16, ZE_TPF_LUM_HDR);
		ZED3D9CommonTools::CreateRenderTarget(Luminance1, 4, 4, ZE_TPF_LUM_HDR);
		ZED3D9CommonTools::CreateRenderTarget(Luminance, 1, 1, ZE_TPF_LUM_HDR);
		ZED3D9CommonTools::CreateRenderTarget(OldLuminance, 1, 1, ZE_TPF_LUM_HDR);
	}

	void SetRenderTarget(LPDIRECT3DTEXTURE9* Texture)
	{
		LPDIRECT3DSURFACE9* Surface;
		Texture->GetSurfaceLevel(0, &Surface);
		Device->SetRenderTarget(0, Surface);
		Surface->Release();
	}

	void DoHDR()
	{
		Device->SetFVF(D3DFVF_XYZ | D3DFVF_TEX0);
		Device->SetVertexShader(Shaders.VertexShader);

		// Luminance Measure Start Pass
		Device->SetTexture(0, Input);
		SetRenderTarget(Textures.Luminance4);
		DoPass(Shaders.LumMeasureStart, 256, 256);

		// Luminance Measure Down Sample 4x (From : 256, To: 64)
		Device->SetTexture(0, Textures.Luminance4);
		SetRenderTarget(Textures.Luminance3);
		DoPass(Shaders.LumMeasureStart, 64, 64);

		// Luminance Measure Down Sample 4x (From : 64, To: 16)
		Device->SetTexture(0, Textures.Luminance3);
		SetRenderTarget(Textures.Luminance2);
		DoPass(Shaders.LumMeasureStart, 16, 16);


		// Luminance Measure Down Sample 4x (From : 16, To: 4)
		Device->SetTexture(0, Textures.Luminance2);
		SetRenderTarget(Textures.Luminance1);
		DoPass(Shaders.LumMeasureStart, 4, 4);

		// Luminance Measure End Pass
		Device->SetTexture(0, Textures.Luminance1);
		SetRenderTarget(Textures.Luminance);
		DoPass(Shaders.LumMeasureStart, 1, 1);

		// Bright Pass
		Device->SetTexture(0, Input);
		Device->SetTexture(1, Textures.Luminance);
		SetRenderTarget(Textures.BrightPass);
		DoPass(Shaders.LumMeasureStart, ScreenWidth / 2, ScreenHeight / 2);

		// Bright Pass
		Device->SetTexture(0, Input);
		Device->SetTexture(1, Textures.Luminance);
		SetRenderTarget(Textures.BrightPass);
		DoPass(Shaders.Down, ScreenWidth / 2, ScreenHeight / 2);

	}
};
