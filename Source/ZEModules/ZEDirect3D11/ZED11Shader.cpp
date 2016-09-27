//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11Shader.cpp
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

#include "ZED11Shader.h"

#include "ZED11Module.h"

bool ZED11Shader::Initialize(ZEGRShaderType ShaderType, const void* ShaderBinary, ZESize Size)
{
	HRESULT Result;
	switch (ShaderType)
	{
		case ZEGR_ST_VERTEX:
			Result = GetDevice()->CreateVertexShader(ShaderBinary, Size, NULL, &VertexShader);
			ByteCode.AddMultiple((ZEBYTE*)ShaderBinary, Size);
			break;

		case ZEGR_ST_DOMAIN:
			Result = GetDevice()->CreateDomainShader(ShaderBinary, Size, NULL, &DomainShader);
			ByteCode.AddMultiple((ZEBYTE*)ShaderBinary, Size);
			break;

		case ZEGR_ST_HULL:
			Result = GetDevice()->CreateHullShader(ShaderBinary, Size, NULL, &HullShader);
			ByteCode.AddMultiple((ZEBYTE*)ShaderBinary, Size);
			break;

		case ZEGR_ST_GEOMETRY:
			Result = GetDevice()->CreateGeometryShader(ShaderBinary, Size, NULL, &GeometryShader);
			ByteCode.AddMultiple((ZEBYTE*)ShaderBinary, Size);
			break;

		case ZEGR_ST_PIXEL:
			Result = GetDevice()->CreatePixelShader(ShaderBinary, Size, NULL, &PixelShader);
			ByteCode.AddMultiple((ZEBYTE*)ShaderBinary, Size);
			break;

		case ZEGR_ST_COMPUTE:
			Result = GetDevice()->CreateComputeShader(ShaderBinary, Size, NULL, &ComputeShader);
			ByteCode.AddMultiple((ZEBYTE*)ShaderBinary, Size);
			break;

		default:
			return NULL;
	}
	
	if (FAILED(Result))
		return NULL;

	return ZEGRShader::Initialize(ShaderType, ShaderBinary, Size);
}

void ZED11Shader::Deinitialize()
{
	ZEGR_RELEASE(VertexShader);

	ZEGRShader::Deinitialize();
}

ZED11Shader::ZED11Shader()
{
	VertexShader = NULL;
}

const ZEArray<ZEBYTE>& ZED11Shader::GetByteCode() const
{
	return ByteCode;
}

ID3D11VertexShader* ZED11Shader::GetVertexShader() const
{
	return VertexShader;
}

ID3D11GeometryShader* ZED11Shader::GetGeometryShader() const
{
	return GeometryShader;
}

ID3D11DomainShader* ZED11Shader::GetDomainShader() const
{
	return DomainShader;
}

ID3D11HullShader* ZED11Shader::GetHullShader() const
{
	return HullShader;
}

ID3D11PixelShader* ZED11Shader::GetPixelShader() const
{
	return PixelShader;
}

ID3D11ComputeShader* ZED11Shader::GetComputeShader() const
{
	return ComputeShader;
}

ZED11Shader::~ZED11Shader()
{
	Deinitialize();
}
