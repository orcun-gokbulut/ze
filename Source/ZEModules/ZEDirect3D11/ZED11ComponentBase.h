//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11ComponentBase.h
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

#include <d3d11_1.h>

#include "ZEDS/ZEFlags.h"
#include "ZEThread/ZELock.h"

class ZED11Module;
enum ZEGRFormat;
enum ZEGRResourceUsage		: ZEUInt8;
enum ZEGRResourceMapType	: ZEUInt8;

class ZED11ComponentBase
{
	friend class ZED11Module;
	private:
		static ZED11Module*				Module;
		static ID3D11Device1*			Device;
		static ID3D11DeviceContext1*	Context;
		static ZELock					ContextLock;

	public:
		static ZED11Module*				GetModule();
		static ID3D11Device1*			GetDevice();
		static ID3D11DeviceContext1*	GetMainContext();

		static void						LockContext();
		static void						UnlockContext();

		static DXGI_FORMAT				ConvertFormat(ZEGRFormat Format);
		static ZEGRFormat				ConvertDXGIFormat(DXGI_FORMAT Format);
		static D3D11_USAGE				ConvertUsage(ZEGRResourceUsage Usage);
		static UINT						ConvertBindFlags(ZEFlags BindFlags);
		static UINT						ConvertUsageToCpuAccessFlags(ZEGRResourceUsage Usage);
		static D3D11_MAP				ConvertMapType(ZEGRResourceMapType MapType);
		static DXGI_FORMAT				ConvertToNonSRGB(ZEGRFormat Format);
};
