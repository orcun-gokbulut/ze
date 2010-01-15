//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - D3D9FixedMaterialShader.h
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
#ifndef	__ZE_D3D9_SHADER_H__
#define __ZE_D3D9_SHADER_H__

#include "D3D9ComponentBase.h"
#include "ZEDS/Array.h"
#include <d3dx9.h>

enum ZED3D9FixedMateriaShaderPassType
{
	ZE_D3D9SPT_SHADOW_PASS = -2,
	ZE_D3D9SPT_DEPTH_PASS = -1,
	ZE_D3D9SPT_PRELIGHT_PASS = 0,
	ZE_D3D9SPT_POINT_LIGHT_PASS = 1,
	ZE_D3D9SPT_DIRECTIONAL_LIGHT_PASS = 2,
	ZE_D3D9SPT_PROJECTIVE_LIGHT_PASS = 3,
};

class ZED3D9FixedMateriaShaderPass : public ZED3D9ComponentBase
{
	public:
		LPDIRECT3DVERTEXSHADER9					VertexShader;
		LPDIRECT3DVERTEXSHADER9					SkinnedVertexShader;
		LPDIRECT3DPIXELSHADER9					PixelShader;
		bool									Shared;

		bool									Compile(const char* VertexShaderSource, const char* PixelShaderSource, const char* ShaderName, const char* ShaderProfile, D3DXMACRO* Macros);
		void									Release();

												ZED3D9FixedMateriaShaderPass();
												~ZED3D9FixedMateriaShaderPass();
};

class ZED3D9FixedMaterialShader : public ZED3D9ComponentBase
{	
	private:
		int											ShaderComponents;
		int											ShaderInstanceIndex;
		
		static const char*							GetMaterialComponentName(unsigned ShaderComponent);

													ZED3D9FixedMaterialShader();
													~ZED3D9FixedMaterialShader();
	public:

		ZED3D9FixedMateriaShaderPass				PreLightPass;
		ZED3D9FixedMateriaShaderPass				PointLightPass;
		ZED3D9FixedMateriaShaderPass				ShadowedPointLightPass;
		ZED3D9FixedMateriaShaderPass				DirectionalLightPass;
		ZED3D9FixedMateriaShaderPass				ShadowedDirectionalLightPass;
		ZED3D9FixedMateriaShaderPass				ProjectiveLightPass;
		ZED3D9FixedMateriaShaderPass				ShadowedProjectiveLightPass;
		ZED3D9FixedMateriaShaderPass				OmniProjectiveLightPass;
		ZED3D9FixedMateriaShaderPass				ShadowedOmniProjectiveLightPass;

		ZED3D9FixedMateriaShaderPass				DepthPass;
		ZED3D9FixedMateriaShaderPass				VelocityPass;
		ZED3D9FixedMateriaShaderPass				PointLightShadowPass;
		ZED3D9FixedMateriaShaderPass				DirectionalLightShadowPass;
		ZED3D9FixedMateriaShaderPass				ProjectiveLightShadowPass;
		ZED3D9FixedMateriaShaderPass				OmniDirectionalLightShadowPass;

		static bool									BaseInitialize();
		static void									BaseDeinitialize();

		void										Release();
		void										Destroy();

		static ZED3D9FixedMaterialShader*			CreateInstance(unsigned int MaterialComponents);
};

class ZEFixedMaterialShaderManager
{
	private:
		ZEArray<ZED3D9FixedMaterialShader*>		Shaders;
	
	public:
		void									CreateNewShader();
};

#endif
