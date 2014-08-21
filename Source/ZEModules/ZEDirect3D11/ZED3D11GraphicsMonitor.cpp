//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D11GraphicsMonitor.cpp
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

#include "ZEError.h"
#include "ZED3D11GraphicsMonitor.h"
#include "ZED3D11GraphicsModule.h"

ZED3D11GraphicsMonitor::ZED3D11GraphicsMonitor(IDXGIOutput1* Output, IDXGIAdapter2* OwnerAdapter)
{
	HRESULT Result = S_FALSE;
	DXGI_OUTPUT_DESC Descriptor = {0};
	
	Result = Output->GetDesc(&Descriptor);
	if (FAILED(Result))
	{
		zeCriticalError("Cannot get monitor info.");
		return;
	}
	
	DXGI_FORMAT Format = ((ZED3D11GraphicsModule*)zeGraphics)->GetDXGIDisplayFormat();

	ZEUInt DisplayModeCount = 0;
	ZEUInt DisplayModeFlags = DXGI_ENUM_MODES_INTERLACED | DXGI_ENUM_MODES_SCALING;
	Result = Output->GetDisplayModeList1(Format, DisplayModeFlags, &DisplayModeCount, NULL);
	if(FAILED(Result))
	{
		zeCriticalError("Cannot get display mode count. Error: %d", Result);
		return;
	}

	// Get display modes
	DXGIModes.SetCount(DisplayModeCount);
	Result = Output->GetDisplayModeList1(Format, DisplayModeFlags, &DisplayModeCount, DXGIModes.GetCArray());
	if(FAILED(Result))
	{
		zeCriticalError("Cannot get display modes. Error: %d", Result);
		return;
	}

	Area.LeftUp.x = (float)Descriptor.DesktopCoordinates.left;
	Area.LeftUp.y = (float)Descriptor.DesktopCoordinates.top;
	Area.RightDown.x = (float)Descriptor.DesktopCoordinates.right;
	Area.RightDown.y = (float)Descriptor.DesktopCoordinates.bottom;

	Name = ZEString::FromWCString(Descriptor.DeviceName);

	Handle = Descriptor.Monitor;
	DXGIAdapter = OwnerAdapter;
	DXGIOutput = Output;
}

ZED3D11GraphicsMonitor::~ZED3D11GraphicsMonitor()
{
	
}

IDXGIAdapter2* ZED3D11GraphicsMonitor::GetDXGIAdapter() const
{
	return DXGIAdapter;
}

IDXGIOutput1* ZED3D11GraphicsMonitor::GetDXGIOutput() const
{
	return DXGIOutput;
}

const ZEArray<DXGI_MODE_DESC1>& ZED3D11GraphicsMonitor::GetDXGIModes() const
{
	return DXGIModes;
}
