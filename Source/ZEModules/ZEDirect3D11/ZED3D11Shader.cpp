//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D11Shader.cpp
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

#include "ZED3D11Shader.h"
#include "ZED3D11GraphicsModule.h"

/************************************************************************/
/*							ZED3D11Shader                               */
/************************************************************************/

const ID3DBlob* ZED3D11Shader::GetD3D10ByteCode()
{
	return D3D10ByteCode;
}

ZED3D11Shader::ZED3D11Shader(ID3DBlob* ByteCode)
{
	D3D10ByteCode = ByteCode;

	TotalSize += ByteCode->GetBufferSize();
	TotalCount++;
}

ZED3D11Shader::~ZED3D11Shader()
{
	TotalSize -= D3D10ByteCode->GetBufferSize();
	TotalCount--;

	ZEGR_RELEASE(D3D10ByteCode);
}

/************************************************************************/
/*					ZED3D11PixelShader									*/
/************************************************************************/

ZEGRShaderType ZED3D11PixelShader::GetShaderType() const
{
	return ZE_ST_PIXEL;
}

const ID3D11PixelShader* ZED3D11PixelShader::GetD3D10PixelShader() const
{
	return D3D10PixelShader;
}

ZED3D11PixelShader::ZED3D11PixelShader(ID3DBlob* ByteCode, ID3D11PixelShader*	PixelShader) : ZED3D11Shader(ByteCode)
{
	D3D10PixelShader = PixelShader;
}

ZED3D11PixelShader::~ZED3D11PixelShader()
{
	ZEGR_RELEASE(D3D10PixelShader);
}

/************************************************************************/
/*					ZED3D11VertexShader									*/
/************************************************************************/

ZEGRShaderType ZED3D11VertexShader::GetShaderType() const
{
	return ZE_ST_VERTEX;
}

const ID3D11VertexShader* ZED3D11VertexShader::GetD3D10VertexShader() const
{
	return D3D10VertexShader;
}

ZED3D11VertexShader::ZED3D11VertexShader(ID3DBlob* ByteCode, ID3D11VertexShader* VertexShader) : ZED3D11Shader(ByteCode)
{
	D3D10VertexShader = VertexShader;
}

ZED3D11VertexShader::~ZED3D11VertexShader()
{
	ZEGR_RELEASE(D3D10VertexShader);
}

/************************************************************************/
/*						ZED3D11GeometryShader							*/
/************************************************************************/

ZEGRShaderType ZED3D11GeometryShader::GetShaderType() const
{
	return ZE_ST_GEOMETRY;
}

const ID3D11GeometryShader*	ZED3D11GeometryShader::GetD3D10GeometryShader() const
{
	return D3D10GeometryShader;
}

ZED3D11GeometryShader::ZED3D11GeometryShader(ID3DBlob* ByteCode, ID3D11GeometryShader* GeometryShader) : ZED3D11Shader(ByteCode)
{
	D3D10GeometryShader = GeometryShader;
}

ZED3D11GeometryShader::~ZED3D11GeometryShader()
{
	ZEGR_RELEASE(D3D10GeometryShader);
}

/************************************************************************/
/*						ZED3D11DomainShader								*/
/************************************************************************/

ZEGRShaderType ZED3D11DomainShader::GetShaderType() const
{
	return ZE_ST_DOMAIN;
}

const ID3D11DomainShader* ZED3D11DomainShader::GetD3D10DomainShader() const
{
	return D3D10DomainShader;
}

ZED3D11DomainShader::ZED3D11DomainShader(ID3DBlob* ByteCode, ID3D11DomainShader* DomainShader) : ZED3D11Shader(ByteCode)
{
	D3D10DomainShader = DomainShader;
}

ZED3D11DomainShader::~ZED3D11DomainShader()
{
	ZEGR_RELEASE(D3D10DomainShader);
}

/************************************************************************/
/*						ZED3D11HullShader								*/
/************************************************************************/

ZEGRShaderType ZED3D11HullShader::GetShaderType() const
{
	return ZE_ST_HULL;
}

const ID3D11HullShader* ZED3D11HullShader::GetD3D10HullShader() const
{
	return D3D10HullShader;
}

ZED3D11HullShader::ZED3D11HullShader(ID3DBlob* ByteCode, ID3D11HullShader* HullShader) : ZED3D11Shader(ByteCode)
{
	D3D10HullShader = HullShader;
}

ZED3D11HullShader::~ZED3D11HullShader()
{
	ZEGR_RELEASE(D3D10HullShader);
}

/************************************************************************/
/*						ZED3D11ComputeShader							*/
/************************************************************************/

ZEGRShaderType ZED3D11ComputeShader::GetShaderType() const
{
	return ZE_ST_COMPUTE;
}

const ID3D11ComputeShader* ZED3D11ComputeShader::GetD3D10ComputeShader() const
{
	return D3D10ComputeShader;
}

ZED3D11ComputeShader::ZED3D11ComputeShader(ID3DBlob* ByteCode, ID3D11ComputeShader* ComputeShader) : ZED3D11Shader(ByteCode)
{
	D3D10ComputeShader = ComputeShader;
}

ZED3D11ComputeShader::~ZED3D11ComputeShader()
{
	ZEGR_RELEASE(D3D10ComputeShader);
}


