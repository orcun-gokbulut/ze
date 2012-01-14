//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9CommonTools.h
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
#ifndef __ZE_D3D9_COMMON_TOOLS_H__
#define __ZE_D3D9_COMMON_TOOLS_H__

#include <d3d9.h>
#include <d3dx9.h>
#include "ZEGraphics/ZETexture.h"
#include "ZED3D9ComponentBase.h"
#include "ZETypes.h"

#define ZED3D_RELEASE(x) {if ((x) != NULL) {(x)->Release(); (x) = NULL;}}
#define ZED3D_DESTROY(x) {if ((x) != NULL) {(x)->Destroy(); (x) = NULL;}}

class ZETexture2D;
class ZETexture2DResource;
class ZETextureCube;
class ZETextureCubeResource;
class ZEViewPort;

class ZED3D9CommonTools : public ZED3D9ComponentBase
{
	public:
		static void				SetRenderTarget(DWORD RenderTarget, ZEViewPort* Texture);
		static void				SetRenderTarget(DWORD RenderTarget, ZETexture2D* Texture);
		static void				SetTexture(DWORD Stage, ZETexture2D* Texture, DWORD Filter, DWORD MipMappingFilter, DWORD Addressing);
		static void				SetTexture(DWORD Stage, ZETexture2DResource* Texture, DWORD Filter, DWORD MipMappingFilter, DWORD Addressing);
		static void				SetTexture(DWORD Stage, ZETextureCube* Texture, DWORD Filter, DWORD MipMappingFilter, DWORD Addressing);
		static void				SetTexture(DWORD Stage, ZETextureCubeResource* Texture, DWORD Filter, DWORD MipMappingFilter, DWORD Addressing);

		static D3DFORMAT		ConvertPixelFormat(ZETexturePixelFormat Format);
		static bool				CompileVertexShaderFromMemory(LPDIRECT3DVERTEXSHADER9* VertexShader, const char* Source, const char* ShaderName, const char* ShaderProfile, ZEUInt32 Components = 0);
		static bool				CompilePixelShaderFromMemory(LPDIRECT3DPIXELSHADER9* PixelShader, const char* Source, const char* ShaderName, const char* ShaderProfile, ZEUInt32 Components = 0);
		static bool				CompileVertexShader(LPDIRECT3DVERTEXSHADER9* VertexShader, const char* FileName, const char* MainFunction, const char* ShaderProfile, ZEUInt32 Components = 0);
		static bool				CompilePixelShader(LPDIRECT3DPIXELSHADER9* PixelShader, const char* FileNamee, const char* MainFunction, const char* ShaderProfile, ZEUInt32 Components = 0);
		/*static bool				CreateDepthRenderTarget(LPDIRECT3DSURFACE9* Target, int Width, int Height);
		static bool				CreateDepthRenderTarget(LPDIRECT3DTEXTURE9* Target, int Width, int Height);
		static bool				CreateRenderTarget(LPDIRECT3DTEXTURE9* Target, int Width, int Height, ZETexturePixelFormat Format);
		static bool				CreateRenderTarget(LPDIRECT3DSURFACE9* Target, int Width, int Height, ZETexturePixelFormat Format);*/
};
#endif




