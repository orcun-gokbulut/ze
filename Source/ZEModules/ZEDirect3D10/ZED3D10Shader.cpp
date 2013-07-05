//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D10Shader.cpp
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

#include "ZED3D10Shader.h"
#include "ZED3D10GraphicsModule.h"

#include <d3d10.h>

/************************************************************************/
/*							ZED3D10Shader                               */
/************************************************************************/

const ID3D10Blob* ZED3D10Shader::GetD3D10ByteCode()
{
	return D3D10ByteCode;
}

ZESize		ZED3D10Shader::GlobalSize = 0;
ZEUInt16	ZED3D10Shader::GlobalCount = 0;

ZED3D10Shader::ZED3D10Shader(ID3D10Blob* ByteCode)
{
	D3D10ByteCode = ByteCode;

	GlobalSize += ByteCode->GetBufferSize();
	GlobalCount++;
}

ZED3D10Shader::~ZED3D10Shader()
{
	ZED3D_RELEASE(D3D10ByteCode);

	GlobalSize -= D3D10ByteCode->GetBufferSize();
	GlobalCount--;
}

/************************************************************************/
/*					ZED3D10PixelShader									*/
/************************************************************************/

ZEShaderType ZED3D10PixelShader::GetShaderType() const
{
	return ZE_ST_PIXEL;
}

const ID3D10PixelShader* ZED3D10PixelShader::GetD3D10PixelShader() const
{
	return D3D10PixelShader;
}

ZED3D10PixelShader::ZED3D10PixelShader(ID3D10Blob* ByteCode, ID3D10PixelShader*	PixelShader) : ZED3D10Shader(ByteCode)
{
	D3D10PixelShader = PixelShader;
}

ZED3D10PixelShader::~ZED3D10PixelShader()
{
	ZED3D_RELEASE(D3D10PixelShader);
}

/************************************************************************/
/*					ZED3D10VertexShader									*/
/************************************************************************/

ZEShaderType ZED3D10VertexShader::GetShaderType() const
{
	return ZE_ST_VERTEX;
}

const ID3D10VertexShader* ZED3D10VertexShader::GetD3D10VertexShader() const
{
	return D3D10VertexShader;
}

ZED3D10VertexShader::ZED3D10VertexShader(ID3D10Blob* ByteCode, ID3D10VertexShader* VertexShader) : ZED3D10Shader(ByteCode)
{
	D3D10VertexShader = VertexShader;
}

ZED3D10VertexShader::~ZED3D10VertexShader()
{
	ZED3D_RELEASE(D3D10VertexShader);
}

/************************************************************************/
/*					ZED3D10GeometryShader								*/
/************************************************************************/

ZEShaderType ZED3D10GeometryShader::GetShaderType() const
{
	return ZE_ST_GEOMETRY;
}

const ID3D10GeometryShader*	ZED3D10GeometryShader::GetD3D10GeometryShader() const
{
	return D3D10GeometryShader;
}

ZED3D10GeometryShader::ZED3D10GeometryShader(ID3D10Blob* ByteCode, ID3D10GeometryShader* GeometryShader) : ZED3D10Shader(ByteCode)
{
	D3D10GeometryShader = GeometryShader;
}

ZED3D10GeometryShader::~ZED3D10GeometryShader()
{
	ZED3D_RELEASE(D3D10GeometryShader);
}
