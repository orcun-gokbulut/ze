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

#ifndef	__ZE_D3D11_SHADER_H__
#define __ZE_D3D11_SHADER_H__

#include <d3d11.h>

#include "ZETypes.h"
#include "ZEGraphics/ZEShader.h"
#include "ZED3D10ComponentBase.h"

class ZED3D10Shader : public ZEShader
{
	friend class ZED3D10StatePool;
	friend class ZED3D10ShaderCompiler;
	friend class ZED3D10GraphicsDevice;
	friend class ZED3D10GraphicsModule;

	protected:	
		ID3DBlob*					D3D10ByteCode;

									ZED3D10Shader(ID3DBlob* ByteCode);
		virtual						~ZED3D10Shader();

	public:
		const ID3DBlob*				GetD3D10ByteCode();
};

class ZED3D10PixelShader : public ZED3D10Shader, private ZED3D10ComponentBase
{
	friend class ZED3D10ShaderCompiler;
	friend class ZED3D10GraphicsDevice;

	protected:
		ID3D11PixelShader*			D3D10PixelShader;
	
									ZED3D10PixelShader(ID3DBlob* ByteCode, ID3D11PixelShader*	PixelShader);
		virtual						~ZED3D10PixelShader();

	public:
		ZEShaderType				GetShaderType() const;

		const ID3D11PixelShader*	GetD3D10PixelShader() const;

};

class ZED3D10VertexShader : public ZED3D10Shader, private ZED3D10ComponentBase
{
	friend class ZED3D10ShaderCompiler;
	friend class ZED3D10GraphicsDevice;

	protected:
		ID3D11VertexShader*			D3D10VertexShader;

									ZED3D10VertexShader(ID3DBlob* ByteCode, ID3D11VertexShader* VertexShader);
		virtual						~ZED3D10VertexShader();

	public:
		ZEShaderType				GetShaderType() const;

		const ID3D11VertexShader*	GetD3D10VertexShader() const;
};

class ZED3D10GeometryShader : public ZED3D10Shader, private ZED3D10ComponentBase
{
	friend class ZED3D10ShaderCompiler;
	friend class ZED3D10GraphicsDevice;

	protected:
		ID3D11GeometryShader*		D3D10GeometryShader;

									ZED3D10GeometryShader(ID3DBlob* ByteCode, ID3D11GeometryShader* GeometryShader);
		virtual						~ZED3D10GeometryShader();

	public:
		ZEShaderType				GetShaderType() const;

		const ID3D11GeometryShader*	GetD3D10GeometryShader() const;
};

class ZED3D10DomainShader : public ZED3D10Shader, private ZED3D10ComponentBase
{
	friend class ZED3D10ShaderCompiler;
	friend class ZED3D10GraphicsDevice;

	protected:
		ID3D11DomainShader*			D3D10DomainShader;

									ZED3D10DomainShader(ID3DBlob* ByteCode, ID3D11DomainShader* DomainShader);
		virtual						~ZED3D10DomainShader();

	public:
		ZEShaderType				GetShaderType() const;

		const ID3D11DomainShader*	GetD3D10DomainShader() const;
};

class ZED3D10HullShader : public ZED3D10Shader, private ZED3D10ComponentBase
{
	friend class ZED3D10ShaderCompiler;
	friend class ZED3D10GraphicsDevice;

	protected:
		ID3D11HullShader*			D3D10HullShader;

									ZED3D10HullShader(ID3DBlob* ByteCode, ID3D11HullShader* HullShader);
		virtual						~ZED3D10HullShader();

	public:
		ZEShaderType				GetShaderType() const;

		const ID3D11HullShader*		GetD3D10HullShader() const;
};

class ZED3D10ComputeShader : public ZED3D10Shader, private ZED3D10ComponentBase
{
	friend class ZED3D10ShaderCompiler;
	friend class ZED3D10GraphicsDevice;

	protected:
		ID3D11ComputeShader*		D3D10ComputeShader;

									ZED3D10ComputeShader(ID3DBlob* ByteCode, ID3D11ComputeShader* ComputeShader);
		virtual						~ZED3D10ComputeShader();

	public:
		ZEShaderType				GetShaderType() const;

		const ID3D11ComputeShader*	GetD3D10ComputeShader() const;
};

#endif
