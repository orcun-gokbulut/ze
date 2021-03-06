//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRGraphicsModule.cpp
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

#include "ZEGRGraphicsModule.h"

#include "ZECore/ZECore.h"
#include "ZECore/ZEOption.h"
#include "ZECore/ZEOptionManager.h"

#include <DirectXTex.h>
#include <FreeImage.h>

using namespace DirectX;

ZEUInt ZEGRGraphicsModule::SAMPLE_COUNT = 4;

static bool IsComInitialized = false;

void FreeImageOutput(FREE_IMAGE_FORMAT Bitmap, const char* Message)
{
	zeLog("%s", Message);
}

ZETextureOptions* ZEGRGraphicsModule::GetTextureOptions()
{
	static ZETextureOptions VeryHigh	= {ZE_TCT_NONE,		ZE_TCQ_HIGH,    ZE_TDS_NONE,	ZE_TFC_ENABLED,		ZE_TMM_ENABLED,		25};
	static ZETextureOptions High		= {ZE_TCT_NONE/*ZE_TCT_DXT5*/,		ZE_TCQ_HIGH,	ZE_TDS_NONE,	ZE_TFC_ENABLED,		ZE_TMM_ENABLED,		25};
	static ZETextureOptions Normal		= {ZE_TCT_NONE,		ZE_TCQ_NORMAL,	ZE_TDS_2X,		ZE_TFC_ENABLED,		ZE_TMM_ENABLED,		25};
	static ZETextureOptions Low			= {ZE_TCT_DXT5,		ZE_TCQ_NORMAL,	ZE_TDS_2X,		ZE_TFC_ENABLED,		ZE_TMM_ENABLED,		25};
	static ZETextureOptions VeryLow		= {ZE_TCT_DXT5,		ZE_TCQ_LOW,		ZE_TDS_4X,		ZE_TFC_ENABLED,		ZE_TMM_ENABLED,		25};
	static ZETextureOptions UltraLow	= {ZE_TCT_DXT5,		ZE_TCQ_LOW,		ZE_TDS_8X,		ZE_TFC_ENABLED,		ZE_TMM_ENABLED,		25};

	switch(TextureQuality)
	{
		case ZE_TQ_VERY_HIGH:
			return &VeryHigh;
			break;

		case ZE_TQ_HIGH:
			return &High;
			break;

		default:
		case ZE_TQ_NORMAL:
			return &Normal;
			break;
		
		case ZE_TQ_LOW:
			return &Low;
			break;
		
		case ZE_TQ_VERY_LOW:
			return &VeryLow;
			break;
			
		case ZE_TQ_ULTRA_LOW:
			return &UltraLow;
			break;
	}
}

ZEGRCounter& ZEGRGraphicsModule::GetCounter()
{
	return Counter;
}

ZEGRGraphicsModule* ZEGRGraphicsModule::GetInstance()
{
	return ZECore::GetInstance()->GetGraphicsModule();
}

bool ZEGRGraphicsModule::InitializeInternal()
{
	HRESULT HR = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (HR != RPC_E_CHANGED_MODE)
	{
		IsComInitialized = true;

		bool IsWic2 = false;
		IWICImagingFactory* WICFactory = GetWICFactory(IsWic2);
		if (WICFactory == NULL)
		{
			zeError("WIC Factory creation failed.");
			return false;
		}

		SetWICFactory(WICFactory);
	}

	return ZEModule::InitializeInternal();
}

bool ZEGRGraphicsModule::DeinitializeInternal()
{
	extern ZEHolder<ZEGRBuffer> InstanceVertexBuffer;
	InstanceVertexBuffer.Release();
	
	if (IsComInitialized)
		CoUninitialize();

	return ZEModule::DeinitializeInternal();
}

ZEGRGraphicsModule::ZEGRGraphicsModule()
{
	TextureQuality = ZE_TQ_HIGH;
}

ZEGRGraphicsModule::~ZEGRGraphicsModule()
{
	Deinitialize();
}
