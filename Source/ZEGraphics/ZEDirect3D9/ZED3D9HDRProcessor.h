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

#pragma once
#ifndef __ZE_D3D9_HDR_PROCESSOR_H__
#define __ZE_D3D9_HDR_PROCESSOR_H__

#include "ZED3D9ComponentBase.h"
#include <d3d9.h>

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
			LPDIRECT3DTEXTURE9			DownSampled4xA;
			LPDIRECT3DTEXTURE9			DownSampled4xB;
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

										ZED3D9HDRProcessor();

		void							SetRenderTarget(LPDIRECT3DTEXTURE9 Texture);
		void							CreateRenderTargets();
		void							ReleaseRenderTargets();

	public:
		void							OnDeviceLost();
		void							OnDeviceRestored();

		void							Initialize();
		void							Deinitialize();


		void							DoHDR();
};

#endif
