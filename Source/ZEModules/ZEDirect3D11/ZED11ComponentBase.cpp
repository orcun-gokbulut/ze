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
ID3D11Device* ZED11ComponentBase::Device = NULL;
ID3D11DeviceContext* ZED11ComponentBase::Context = NULL;
ZELock ZED11ComponentBase::ContextLock;

ZED11Module* ZED11ComponentBase::GetModule()
{
	return Module;
}

ID3D11Device* ZED11ComponentBase::GetDevice() const
{
	return Device;
}

ID3D11DeviceContext* ZED11ComponentBase::GetMainContext() const
{
	return Context;
}

void ZED11ComponentBase::LockContext()
{
	ContextLock.Lock();
}

void ZED11ComponentBase::UnlockContext()
{
	ContextLock.Unlock();
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



		case ZEGR_TF_BC1_UNORM:
			return DXGI_FORMAT_BC1_UNORM;

		case ZEGR_TF_BC1_UNORM_SRGB:
			return DXGI_FORMAT_BC1_UNORM_SRGB;

		case ZEGR_TF_BC3_UNORM:
			return DXGI_FORMAT_BC3_UNORM;

		case ZEGR_TF_BC3_UNORM_SRGB:
			return DXGI_FORMAT_BC3_UNORM_SRGB;

		case ZEGR_TF_BC4_UNORM:
			return DXGI_FORMAT_BC4_UNORM;

		case ZEGR_TF_BC4_SNORM:
			return DXGI_FORMAT_BC4_SNORM;

		case ZEGR_TF_BC5_UNORM:
			return DXGI_FORMAT_BC5_UNORM;

		case ZEGR_TF_BC5_SNORM:
			return DXGI_FORMAT_BC5_SNORM;

		case ZEGR_TF_BC7_UNORM:
			return DXGI_FORMAT_BC7_UNORM;

		case ZEGR_TF_BC7_UNORM_SRGB:
			return DXGI_FORMAT_BC7_UNORM_SRGB;
	}
}

ZEGRFormat ZED11ComponentBase::ConvertDXGIFormat(DXGI_FORMAT Format)
{
	switch (Format)
	{
		default:
		case DXGI_FORMAT_UNKNOWN:
			return ZEGR_TF_NONE;

		case DXGI_FORMAT_R8_SINT:
			return ZEGR_TF_R8_SINT;

		case DXGI_FORMAT_R8_UINT:
			return ZEGR_TF_R8_UINT;

		case DXGI_FORMAT_R8_UNORM:
			return ZEGR_TF_R8_UNORM;

		case DXGI_FORMAT_R8_SNORM:
			return ZEGR_TF_R8_SNORM;



		case DXGI_FORMAT_R8G8_UINT:
			return ZEGR_TF_R8G8_UINT;

		case DXGI_FORMAT_R8G8_SINT:
			return ZEGR_TF_R8G8_SINT;

		case DXGI_FORMAT_R8G8_UNORM:
			return ZEGR_TF_R8G8_UNORM;

		case DXGI_FORMAT_R8G8_SNORM:
			return ZEGR_TF_R8G8_SNORM;



		case DXGI_FORMAT_R8G8B8A8_UINT:
			return ZEGR_TF_R8G8B8A8_UINT;

		case DXGI_FORMAT_R8G8B8A8_SINT:
			return ZEGR_TF_R8G8B8A8_SINT;

		case DXGI_FORMAT_R8G8B8A8_UNORM:
			return ZEGR_TF_R8G8B8A8_UNORM;

		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			return ZEGR_TF_R8G8B8A8_UNORM_SRGB;

		case DXGI_FORMAT_R8G8B8A8_SNORM:
			return ZEGR_TF_R8G8B8A8_SNORM;



		case DXGI_FORMAT_R16_UINT:
			return ZEGR_TF_R16_UINT;

		case DXGI_FORMAT_R16_SINT:
			return ZEGR_TF_R16_SINT;

		case DXGI_FORMAT_R16_UNORM:
			return ZEGR_TF_R16_UNORM;

		case DXGI_FORMAT_R16_SNORM:
			return ZEGR_TF_R16_SNORM;

		case DXGI_FORMAT_R16_FLOAT:
			return ZEGR_TF_R16_FLOAT;


		case DXGI_FORMAT_R16G16_UINT:
			return ZEGR_TF_R16G16_UINT;

		case DXGI_FORMAT_R16G16_SINT:
			return ZEGR_TF_R16G16_SINT;

		case DXGI_FORMAT_R16G16_UNORM:
			return ZEGR_TF_R16G16_UNORM;

		case DXGI_FORMAT_R16G16_SNORM:
			return ZEGR_TF_R16G16_SNORM;

		case DXGI_FORMAT_R16G16_FLOAT:
			return ZEGR_TF_R16G16_FLOAT;



		case DXGI_FORMAT_R16G16B16A16_UINT:
			return ZEGR_TF_R16G16B16A16_UINT;

		case DXGI_FORMAT_R16G16B16A16_SINT:
			return ZEGR_TF_R16G16B16A16_SINT;

		case DXGI_FORMAT_R16G16B16A16_UNORM:
			return ZEGR_TF_R16G16B16A16_UNORM;

		case DXGI_FORMAT_R16G16B16A16_SNORM:
			return ZEGR_TF_R16G16B16A16_SNORM;

		case DXGI_FORMAT_R16G16B16A16_FLOAT:
			return ZEGR_TF_R16G16B16A16_FLOAT;


		case DXGI_FORMAT_R32_FLOAT:
			return ZEGR_TF_R32_FLOAT;

		case DXGI_FORMAT_R32_SINT:
			return ZEGR_TF_R32_SINT;

		case DXGI_FORMAT_R32_UINT:
			return ZEGR_TF_R32_UINT;


		case DXGI_FORMAT_R32G32_FLOAT:
			return ZEGR_TF_R32G32_FLOAT;

		case DXGI_FORMAT_R32G32_SINT:
			return ZEGR_TF_R32G32_SINT;

		case DXGI_FORMAT_R32G32_UINT:
			return ZEGR_TF_R32G32_UINT;


		case DXGI_FORMAT_R32G32B32A32_SINT:
			return ZEGR_TF_R32G32B32A32_SINT;

		case DXGI_FORMAT_R32G32B32A32_UINT:
			return ZEGR_TF_R32G32B32A32_UINT;

		case DXGI_FORMAT_R32G32B32A32_FLOAT:
			return ZEGR_TF_R32G32B32A32_FLOAT;



		case DXGI_FORMAT_R10G10B10A2_UINT:
			return ZEGR_TF_R10G10B10A2_UINT;

		case DXGI_FORMAT_R10G10B10A2_UNORM:
			return ZEGR_TF_R10G10B10A2_UNORM;

		case DXGI_FORMAT_R11G11B10_FLOAT:
			return ZEGR_TF_R11G11B10_FLOAT;



		case DXGI_FORMAT_BC1_UNORM:
			return ZEGR_TF_BC1_UNORM;

		case DXGI_FORMAT_BC1_UNORM_SRGB:
			return ZEGR_TF_BC1_UNORM_SRGB;

		case DXGI_FORMAT_BC3_UNORM:
			return ZEGR_TF_BC3_UNORM;

		case DXGI_FORMAT_BC3_UNORM_SRGB:
			return ZEGR_TF_BC3_UNORM_SRGB;

		case DXGI_FORMAT_BC4_UNORM:
			return ZEGR_TF_BC4_UNORM;

		case DXGI_FORMAT_BC4_SNORM:
			return ZEGR_TF_BC4_SNORM;

		case DXGI_FORMAT_BC5_UNORM:
			return ZEGR_TF_BC5_UNORM;

		case DXGI_FORMAT_BC5_SNORM:
			return ZEGR_TF_BC5_SNORM;

		case DXGI_FORMAT_BC7_UNORM:
			return ZEGR_TF_BC7_UNORM;

		case DXGI_FORMAT_BC7_UNORM_SRGB:
			return ZEGR_TF_BC7_UNORM_SRGB;
	}
}

D3D11_USAGE ZED11ComponentBase::ConvertUsage(ZEGRResourceUsage Usage)
{
	switch (Usage)
	{
		case ZEGR_RU_GPU_READ_ONLY:
			return D3D11_USAGE_IMMUTABLE;
			
		default:
		case ZEGR_RU_GPU_READ_WRITE_CPU_WRITE:
			return D3D11_USAGE_DEFAULT;

		case ZEGR_RU_GPU_READ_CPU_WRITE:
			return D3D11_USAGE_DYNAMIC;

		case ZEGR_RU_CPU_READ_WRITE:
			return D3D11_USAGE_STAGING;
	}
}

UINT ZED11ComponentBase::ConvertBindFlags(ZEFlags BindFlags)
{
	if (BindFlags == ZEGR_RBF_NONE)
		return 0;

	UINT Flags = 0;

	if (BindFlags.GetFlags(ZEGR_RBF_SHADER_RESOURCE))
		Flags |= D3D11_BIND_SHADER_RESOURCE;

	if (BindFlags.GetFlags(ZEGR_RBF_RENDER_TARGET))
		Flags |= D3D11_BIND_RENDER_TARGET;

	if (BindFlags.GetFlags(ZEGR_RBF_DEPTH_STENCIL))
		Flags |= D3D11_BIND_DEPTH_STENCIL;

	if (BindFlags.GetFlags(ZEGR_RBF_UNORDERED_ACCESS))
		Flags |= D3D11_BIND_UNORDERED_ACCESS;

	return Flags;
}

UINT ZED11ComponentBase::ConvertUsageToCpuAccessFlags(ZEGRResourceUsage Usage)
{
	switch (Usage)
	{
		case ZEGR_RU_GPU_READ_ONLY:
		case ZEGR_RU_GPU_READ_WRITE_CPU_WRITE:
			return 0;

		case ZEGR_RU_GPU_READ_CPU_WRITE:
			return D3D11_CPU_ACCESS_WRITE;

		case ZEGR_RU_CPU_READ_WRITE:
			return D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	}
}
