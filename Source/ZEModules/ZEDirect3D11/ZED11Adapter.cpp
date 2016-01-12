//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11Adapter.cpp
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

#include "ZED11Adapter.h"

#include "ZED11Module.h"
#include "ZEError.h"

#include <dxgi1_2.h>

ZED11Monitor::ZED11Monitor(ZED11Adapter* Adapter, IDXGIOutput1* Output)
{
	DXGI_OUTPUT_DESC Descriptor;
	memset(&Descriptor, 0, sizeof(DXGI_OUTPUT_DESC));
	
	HRESULT Result = Output->GetDesc(&Descriptor);
	if (FAILED(Result))
	{
		zeCriticalError("Cannot get monitor info.");
		return;
	}

	Area.LeftUp.x = (float)Descriptor.DesktopCoordinates.left;
	Area.LeftUp.y = (float)Descriptor.DesktopCoordinates.top;
	Area.RightDown.x = (float)Descriptor.DesktopCoordinates.right;
	Area.RightDown.y = (float)Descriptor.DesktopCoordinates.bottom;

	this->Adapter = Adapter;
	this->Output = Output;
	Name = Descriptor.DeviceName;
	Handle = Descriptor.Monitor;
}

ZED11Monitor::~ZED11Monitor()
{
	
}

IDXGIOutput1* ZED11Monitor::GetOutput()
{
	return Output;
}

const ZEArray<ZEGRMonitorMode>& ZED11Monitor::GetModes()
{
	if (Modes.GetCount() != 0)
	{
		ZEArray<DXGI_MODE_DESC1> ModeDesc;
		ZEUInt DisplayModeCount = 0;
		ZEUInt DisplayModeFlags = DXGI_ENUM_MODES_INTERLACED | DXGI_ENUM_MODES_SCALING;
		HRESULT Result = Output->GetDisplayModeList1(DXGI_FORMAT_R8G8B8A8_UNORM, DisplayModeFlags, &DisplayModeCount, NULL);
		if(FAILED(Result))
		{
			zeCriticalError("Cannot get display mode count. Error: %d", Result);
			return Modes;
		}

		ModeDesc.SetCount(DisplayModeCount);
		Result = Output->GetDisplayModeList1(DXGI_FORMAT_R8G8B8A8_UNORM, DisplayModeFlags, &DisplayModeCount, ModeDesc.GetCArray());
		if(FAILED(Result))
		{
			zeCriticalError("Cannot get display modes. Error: %d", Result);
			return Modes;
		}

		Modes.SetCount(ModeDesc.GetCount());
		for (ZESize I = 0; I < ModeDesc.GetCount(); I++)
		{
			Modes[I].Width = ModeDesc[I].Width;
			Modes[I].Height = ModeDesc[I].Height;
			Modes[I].Format = ZEGR_TF_R8G8B8A8_UNORM;
			Modes[I].RefreshRate.Numerator = ModeDesc[I].RefreshRate.Numerator;
			Modes[I].RefreshRate.Denominator = ModeDesc[I].RefreshRate.Denominator;
		}
	}

	return Modes;
}

ZED11Adapter::ZED11Adapter(IDXGIAdapter2* Adapter)
{
	this->Adapter = Adapter;
	DXGI_ADAPTER_DESC Desc;
	Adapter->GetDesc(&Desc);

	Name = Desc.Description;
	Id = *(ZEUInt64*)&Desc.AdapterLuid;
}

ZED11Adapter::~ZED11Adapter()
{
	for (ZESize I = 0; I < Monitors.GetCount(); I++)
		delete (ZED11Monitor*)Monitors[I];
	
	Monitors.Clear();
}


IDXGIAdapter2* ZED11Adapter::GetAdapter()
{
	return Adapter;
}

const ZEArray<ZEGRMonitor*>& ZED11Adapter::GetMonitors()
{
	ZEUInt OutputId = 0;
	if (Monitors.GetCount() == 0)
	{
		IDXGIOutput1* Output = NULL;
		while (Adapter->EnumOutputs(OutputId, (IDXGIOutput**)&Output) != DXGI_ERROR_NOT_FOUND)
		{
			ZED11Monitor* Monitor = new ZED11Monitor(this, Output);
			Monitors.Add(Monitor);
			OutputId++;
		}

		if (Monitors.GetCount() == 0)
			zeCriticalError("Cannot enumerate monitors. Error: %d", DXGI_ERROR_NOT_FOUND);
	}

	return Monitors;
}
