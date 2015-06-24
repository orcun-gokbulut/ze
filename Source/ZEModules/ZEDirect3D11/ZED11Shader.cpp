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
#include "ZED3D11GraphicsModule.h"

bool ZED11Shader::Initialize(ZEGRShaderType ShaderType, void* ShaderBinary, ZESize Size)
{
	HRESULT Result;
	switch (ShaderType)
	{
		case ZEGR_ST_VERTEX:
			Result = GetDevice()->CreateVertexShader(ShaderBinary, Size, NULL, &VertexShader);
			break;

		case ZEGR_ST_DOMAIN:
			Result = GetDevice()->CreateDomainShader(ShaderBinary, Size, NULL, &DomainShader);
			break;

		case ZEGR_ST_HULL:
			Result = GetDevice()->CreateHullShader(ShaderBinary, Size, NULL, &HullShader);
			break;

		case ZEGR_ST_GEOMETRY:
			Result = GetDevice()->CreateGeometryShader(ShaderBinary, Size, NULL, &GeometryShader);
			break;

		case ZEGR_ST_PIXEL:
			Result = GetDevice()->CreatePixelShader(ShaderBinary, Size, NULL, &PixelShader);
			break;

		case ZEGR_ST_COMPUTE:
			Result = GetDevice()->CreateComputeShader(ShaderBinary, Size, NULL, &ComputeShader);
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
	switch (GetShaderType())
	{
		case ZEGR_ST_VERTEX:
			ZEGR_RELEASE(ComputeShader);
			break;

		case ZEGR_ST_DOMAIN:
			ZEGR_RELEASE(DomainShader);
			break;

		case ZEGR_ST_HULL:
			ZEGR_RELEASE(HullShader);
			break;

		case ZEGR_ST_GEOMETRY:
			ZEGR_RELEASE(GeometryShader);
			break;

		case ZEGR_ST_PIXEL:
			ZEGR_RELEASE(PixelShader);
			break;

		case ZEGR_ST_COMPUTE:
			ZEGR_RELEASE(ComputeShader);
			break;
	}

	ZEGRShader::Deinitialize();
}

ZED11Shader::ZED11Shader()
{
	VertexShader = NULL;
}

ID3D11VertexShader* ZED11Shader::GetVertexShader()
{
	return VertexShader;
}

ID3D11GeometryShader* ZED11Shader::GetGeometryShader()
{
	return GeometryShader;
}

ID3D11DomainShader* ZED11Shader::GetDomainShader()
{
	return DomainShader;
}

ID3D11HullShader* ZED11Shader::GetHullShader()
{
	return HullShader;
}

ID3D11PixelShader* ZED11Shader::GetPixelShader()
{
	return PixelShader;
}

ID3D11ComputeShader* ZED11Shader::GetComputeShader()
{
	return ComputeShader;
}
