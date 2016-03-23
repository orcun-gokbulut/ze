//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11ComponentBase.cpp
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

#include "ZED11ComponentBase.h"
#include "ZEGraphics\ZEGRTexture.h"

ZED11Module* ZED11ComponentBase::Module = NULL;
ID3D11Device1* ZED11ComponentBase::Device = NULL;
ID3D11DeviceContext1* ZED11ComponentBase::Context = NULL;

ZED11Module* ZED11ComponentBase::GetModule()
{
	return Module;
}

ID3D11Device1* ZED11ComponentBase::GetDevice() const
{
	return Device;
}

ID3D11DeviceContext1* ZED11ComponentBase::GetMainContext() const
{
	return Context;
}

DXGI_FORMAT ZED11ComponentBase::ConvertFormat(ZEGRFormat Format)
{
	switch (Format)
	{
		default:
		case ZEGR_TF_NONE:
			return DXGI_FORMAT_UNKNOWN;

		case ZEGR_TF_R8_SINT:
			return DXGI_FORMAT_R8_SINT;
		
		case ZEGR_TF_R8_UINT:
			return DXGI_FORMAT_R8_UINT;
		
		case ZEGR_TF_R8_UNORM:
			return DXGI_FORMAT_R8_UNORM;
		
		case ZEGR_TF_R8_SNORM:
			return DXGI_FORMAT_R8_SNORM;



		case ZEGR_TF_R8G8_UINT:
			return DXGI_FORMAT_R8G8_UINT;

		case ZEGR_TF_R8G8_SINT:
			return DXGI_FORMAT_R8G8_SINT;

		case ZEGR_TF_R8G8_UNORM:
			return DXGI_FORMAT_R8G8_UNORM;

		case ZEGR_TF_R8G8_SNORM:
			return DXGI_FORMAT_R8G8_SNORM;



		case ZEGR_TF_R8G8B8A8_UINT:
			return DXGI_FORMAT_R8G8B8A8_UINT;

		case ZEGR_TF_R8G8B8A8_SINT:
			return DXGI_FORMAT_R8G8B8A8_SINT;

		case ZEGR_TF_R8G8B8A8_UNORM:
			return DXGI_FORMAT_R8G8B8A8_UNORM;

		case ZEGR_TF_R8G8B8A8_UNORM_SRGB:
			return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

		case ZEGR_TF_R8G8B8A8_SNORM:
			return DXGI_FORMAT_R8G8B8A8_SNORM;



		case ZEGR_TF_R16_UINT:
			return DXGI_FORMAT_R16_UINT;

		case ZEGR_TF_R16_SINT:
			return DXGI_FORMAT_R16_SINT;

		case ZEGR_TF_R16_UNORM:
			return DXGI_FORMAT_R16_UNORM;

		case ZEGR_TF_R16_SNORM:
			return DXGI_FORMAT_R16_SNORM;

		case ZEGR_TF_R16_FLOAT:
			return DXGI_FORMAT_R16_FLOAT;


		case ZEGR_TF_R16G16_UINT:
			return DXGI_FORMAT_R16G16_UINT;

		case ZEGR_TF_R16G16_SINT:
			return DXGI_FORMAT_R16G16_SINT;

		case ZEGR_TF_R16G16_UNORM:
			return DXGI_FORMAT_R16G16_UNORM;

		case ZEGR_TF_R16G16_SNORM:
			return DXGI_FORMAT_R16G16_SNORM;

		case ZEGR_TF_R16G16_FLOAT:
			return DXGI_FORMAT_R16G16_FLOAT;



		case ZEGR_TF_R16G16B16A16_UINT:
			return DXGI_FORMAT_R16G16B16A16_UINT;

		case ZEGR_TF_R16G16B16A16_SINT:
			return DXGI_FORMAT_R16G16B16A16_SINT;

		case ZEGR_TF_R16G16B16A16_UNORM:
			return DXGI_FORMAT_R16G16B16A16_UNORM;

		case ZEGR_TF_R16G16B16A16_SNORM:
			return DXGI_FORMAT_R16G16B16A16_SNORM;

		case ZEGR_TF_R16G16B16A16_FLOAT:
			return DXGI_FORMAT_R16G16B16A16_FLOAT;


		case ZEGR_TF_R32_FLOAT:
			return DXGI_FORMAT_R32_FLOAT;

		case ZEGR_TF_R32_SINT:
			return DXGI_FORMAT_R32_SINT;

		case ZEGR_TF_R32_UINT:
			return DXGI_FORMAT_R32_UINT;


		case ZEGR_TF_R32G32_FLOAT:
			return DXGI_FORMAT_R32G32_FLOAT;

		case ZEGR_TF_R32G32_SINT:
			return DXGI_FORMAT_R32G32_SINT;

		case ZEGR_TF_R32G32_UINT:
			return DXGI_FORMAT_R32G32_UINT;


		case ZEGR_TF_R32G32B32A32_SINT:
			return DXGI_FORMAT_R32G32B32A32_SINT;

		case ZEGR_TF_R32G32B32A32_UINT:
			return DXGI_FORMAT_R32G32B32A32_UINT;

		case ZEGR_TF_R32G32B32A32_FLOAT:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;



		case ZEGR_TF_R10G10B10A2_UINT:
			return DXGI_FORMAT_R10G10B10A2_UINT;

		case ZEGR_TF_R10G10B10A2_UNORM:
			return DXGI_FORMAT_R10G10B10A2_UNORM;

		case ZEGR_TF_R11G11B10_FLOAT:
			return DXGI_FORMAT_R11G11B10_FLOAT;



		case ZEGR_TF_DXT1_UNORM:
			return DXGI_FORMAT_BC1_UNORM;

		case ZEGR_TF_DXT3_UNORM:
			return DXGI_FORMAT_BC2_UNORM;

		case ZEGR_TF_DXT5_UNORM:
			return DXGI_FORMAT_BC3_UNORM;
	}
}
