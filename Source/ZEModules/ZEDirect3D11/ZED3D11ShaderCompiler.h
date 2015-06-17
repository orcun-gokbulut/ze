//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D11ShaderCompiler.h
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
#ifndef __ZE_D3D11_SHADER_COMPILER_H__
#define __ZE_D3D11_SHADER_COMPILER_H__

#include <d3d11.h>

#include "ZED3D11ComponentBase.h"
#include "ZEGraphics/ZEGRShaderCompiler.h"

class ZEGRShader;
class ZED3D11Shader;
class ZED3D11PixelShader;
class ZED3D11VertexShader;
class ZED3D11GeometryShader;

class ZED3D11ShaderCompiler : public ZEGRShaderCompiler, public ZED3D11ComponentBase
{
	friend class ZED3D11GraphicsModule;

	protected:
		ZED3D11VertexShader*		CreateVertexShader(ID3DBlob* ByteCode);
		ZED3D11GeometryShader*		CreateGeometryShader(ID3DBlob* ByteCode);
		ZED3D11DomainShader*		CreateDomainShader(ID3DBlob* ByteCode);
		ZED3D11HullShader*			CreateHullShader(ID3DBlob* ByteCode);
		ZED3D11PixelShader*			CreatePixelShader(ID3DBlob* ByteCode);
		ZED3D11ComputeShader*		CreateComputeShader(ID3DBlob* ByteCode);

		bool						CreateMetaTable(ZED3D11Shader* Shader, ID3DBlob* ByteCode);

		ZEGRShader*					CompileShader(ZEGRShaderCompileOptions* Options);

									ZED3D11ShaderCompiler();
		virtual						~ZED3D11ShaderCompiler();
};

#endif
