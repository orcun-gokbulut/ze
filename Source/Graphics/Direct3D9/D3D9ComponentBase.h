//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - D3D9ComponentBase.h
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
#ifndef	__ZE_D3DCOMPONENTBASE_H_
#define __ZE_D3DCOMPONENTBASE_H_
#include "ZEDS/ZEDS.h"
#include "Graphics/Texture.h"
#ifdef ZEDEBUG_ENABLED
#define D3D_DEBUG_INFO
#endif
#include <D3D9.h>
#include <D3DX9.h>

class ZEDirect3D9Module;
class ZED3D9ComponentBase
{
	protected:
		static ZEDirect3D9Module*		Module;
		static LPDIRECT3DDEVICE9		Device;

		static D3DFORMAT				ConvertPixelFormat(ZETexturePixelFormat Format);
		static bool						CompileVertexShader(const char* Source, LPDIRECT3DVERTEXSHADER9* VertexShader, ZEArray<D3DXMACRO>* Macros = NULL);
		static bool						CompilePixelShader(const char* Source, LPDIRECT3DPIXELSHADER9* PixelShader, ZEArray<D3DXMACRO>* Macros = NULL);
		static bool						CreateDepthRenderTarget(LPDIRECT3DSURFACE9* Target, int Width, int Height);
		static bool						CreateRenderTarget(LPDIRECT3DTEXTURE9* Target, int Width, int Height, ZETexturePixelFormat Format);
		static bool						CreateRenderTarget(LPDIRECT3DSURFACE9* Target, int Width, int Height, ZETexturePixelFormat Format);

	public:
		static bool						BaseInitialize(ZEDirect3D9Module* D3DModule);

};
#endif
