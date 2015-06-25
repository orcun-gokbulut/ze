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

ZED11Direct3D11Module* ZED11ComponentBase::Module = NULL;
ID3D11Device* ZED11ComponentBase::Device = NULL;

ZED11Direct3D11Module* ZED11ComponentBase::GetModule()
{
	return Module;
}

ID3D11Device* ZED11ComponentBase::GetDevice()
{
	return Device;
}

DXGI_FORMAT ZED11ComponentBase::ConvertFormat(ZEGRFormat Format)
{
	switch (Format)
	{
		default:
		case ZEGR_TF_NULL:
		case ZEGR_TF_NONE:
			return DXGI_FORMAT_UNKNOWN;

		case ZEGR_TF_R8:
			return DXGI_FORMAT_R8_UINT;

		case ZEGR_TF_R8G8:
			return DXGI_FORMAT_R8G8_UINT;

		case ZEGR_TF_R8G8B8A8:
			return DXGI_FORMAT_R8G8B8A8_UINT;

		case ZEGR_TF_R16:
			return DXGI_FORMAT_R16_UINT;

		case ZEGR_TF_R16G16:
			return DXGI_FORMAT_R16G16_UINT;

		case ZEGR_TF_R16G16B16A16:
			return DXGI_FORMAT_R16G16B16A16_UINT;
	
		case ZEGR_TF_R32:
			return DXGI_FORMAT_R32_UINT;

		case ZEGR_TF_R32G32:
			return DXGI_FORMAT_R32G32_UINT;

		case ZEGR_TF_R32G32B32A32:
			return DXGI_FORMAT_R32G32B32A32_UINT;

		case ZEGR_TF_R16F:
			return DXGI_FORMAT_R16_FLOAT;

		case ZEGR_TF_R16FG16F:
			return DXGI_FORMAT_R16G16_FLOAT;

		case ZEGR_TF_R16FG16FB16FA16F:
			return DXGI_FORMAT_R16G16B16A16_FLOAT;

		case ZEGR_TF_R32F:
			return DXGI_FORMAT_R32_FLOAT;

		case ZEGR_TF_R32FG32F:
			return DXGI_FORMAT_R32G32_FLOAT;

		case ZEGR_TF_R32FG32FB32FA32F:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;

		case ZEGR_TF_DXT1:
			return DXGI_FORMAT_BC1_UNORM;

		case ZEGR_TF_DXT3:
			return DXGI_FORMAT_BC2_UNORM;

		case ZEGR_TF_DXT5:
			return DXGI_FORMAT_BC3_UNORM;
	}
}
