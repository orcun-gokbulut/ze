//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D10Shader.h
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

#ifndef	__ZE_D3D10_SHADER_H__
#define __ZE_D3D10_SHADER_H__

#include "ZETypes.h"
#include "ZEGraphics/ZEShader.h"
#include "ZED3D10ComponentBase.h"

struct ID3D10Blob;

class ZED3D10Shader : public ZEShader
{
	friend class ZED3D10StatePool;
	friend class ZED3D10ShaderCompiler;
	friend class ZED3D10GraphicsDevice;
	friend class ZED3D10GraphicsModule;

	protected:
		static ZESize				GlobalSize;
		static ZEUInt16				GlobalCount;
	
		ID3D10Blob*					D3D10ByteCode;

									ZED3D10Shader(ID3D10Blob* ByteCode);
		virtual						~ZED3D10Shader();

	public:
		const ID3D10Blob*			GetD3D10ByteCode();
};

struct ID3D10PixelShader;

class ZED3D10PixelShader : public ZED3D10Shader, private ZED3D10ComponentBase
{
	friend class ZED3D10ShaderCompiler;
	friend class ZED3D10GraphicsDevice;

	protected:
		ID3D10PixelShader*			D3D10PixelShader;
	
									ZED3D10PixelShader(ID3D10Blob* ByteCode, ID3D10PixelShader*	PixelShader);
		virtual						~ZED3D10PixelShader();

	public:
		ZEShaderType				GetShaderType() const;

		const ID3D10PixelShader*	GetD3D10PixelShader() const;

};

struct ID3D10VertexShader;

class ZED3D10VertexShader : public ZED3D10Shader, private ZED3D10ComponentBase
{
	friend class ZED3D10ShaderCompiler;
	friend class ZED3D10GraphicsDevice;

	protected:
		ID3D10VertexShader*			D3D10VertexShader;

									ZED3D10VertexShader(ID3D10Blob* ByteCode, ID3D10VertexShader* VertexShader);
		virtual						~ZED3D10VertexShader();

	public:
		ZEShaderType				GetShaderType() const;

		const ID3D10VertexShader*	GetD3D10VertexShader() const;
};

struct ID3D10GeometryShader;

class ZED3D10GeometryShader : public ZED3D10Shader, private ZED3D10ComponentBase
{
	friend class ZED3D10ShaderCompiler;
	friend class ZED3D10GraphicsDevice;

	protected:
		ID3D10GeometryShader*		D3D10GeometryShader;

									ZED3D10GeometryShader(ID3D10Blob* ByteCode, ID3D10GeometryShader* GeometryShader);
		virtual						~ZED3D10GeometryShader();

	public:
		ZEShaderType				GetShaderType() const;

		const ID3D10GeometryShader*	GetD3D10GeometryShader() const;
};

#endif
