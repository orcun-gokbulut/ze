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


#include "ZECore/ZECore.h"
#include "ZEGRIndexBuffer.h"
#include "ZEGRTextureCube.h"
#include "ZEGRRenderTarget.h"
#include "ZEGRVertexBuffer.h"
#include "ZECore/ZEOption.h"
#include "ZEGRGraphicsModule.h"
#include "ZEGRConstantBuffer.h"
#include "ZEGRWindow.h"
#include "ZEGRDepthStencilBuffer.h"
#include "ZECore/ZEOptionManager.h"

#include <FreeImage.h>

ZEOptionSection ZEGRGraphicsModule::GraphicsOptions;

ZE_MODULE_DESCRIPTION_ABSTRACT(ZEGRGraphicsModule, ZEModule, &ZEGRGraphicsModule::GraphicsOptions)

void FreeImageOutput(FREE_IMAGE_FORMAT Bitmap, const char* Message)
{
	zeLog("%s", Message);
}

void ZEGRGraphicsModule::BaseInitialize()
{
	FreeImage_Initialise();
	FreeImage_SetOutputMessage(FreeImageOutput);

	GraphicsOptions.SetName("Graphics");
	GraphicsOptions.AddOption(new ZEOption("ScreenWidth", 640, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("ScreenHeight", 480, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("FullScreen", false, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("VerticalSync", false, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("SampleCount", 1, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("AnisotropicFilter", 1, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("ShaderQuality", 5, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("TextureQuality", ZE_TQ_HIGH, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("ModelQuality", 5, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("PostEffectQuality", 5, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("HDRQuality", 5, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("ShadowQuality", 1, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("LightQuantity", 1, ZE_OA_NORMAL));
	// ZEOptionManager::GetInstance()->RegisterSection(&GraphicsOptions);
}

void ZEGRGraphicsModule::BaseDeinitialize()
{
	FreeImage_DeInitialise();

	ZEOptionManager::GetInstance()->UnregisterSection(&GraphicsOptions);
}

ZETextureOptions* ZEGRGraphicsModule::GetTextureOptions()
{
	static ZETextureOptions VeryHigh	= {ZE_TCT_NONE,		ZE_TCQ_HIGH,    ZE_TDS_NONE,	ZE_TFC_ENABLED,		ZE_TMM_ENABLED,		25};
	static ZETextureOptions High		= {ZE_TCT_DXT5,		ZE_TCQ_HIGH,	ZE_TDS_NONE,	ZE_TFC_ENABLED,		ZE_TMM_ENABLED,		25};
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

const ZEArray<ZEGRWindow*>& ZEGRGraphicsModule::GetWindows() const
{
	return Windows;
}

const ZEArray<ZEGRMonitor*>& ZEGRGraphicsModule::GetMonitors() const
{
	return Monitors;
}

const ZEArray<ZEGRDevice*>& ZEGRGraphicsModule::GetDevices() const
{
	return Devices;
}

ZEGRCounter& ZEGRGraphicsModule::GetCounter()
{
	return Counter;
}

ZEGRGraphicsModule* ZEGRGraphicsModule::GetInstance()
{
	return ZECore::GetInstance()->GetGraphicsModule();
}

bool ZEGRGraphicsModule::InitializeSelf()
{
	if (!ZEModule::InitializeSelf())
		return false;

	ZEGRWindow* MainWindow = ZEGRWindow::CreateInstance();
	MainWindow->Initialize();

	return true;
}

bool ZEGRGraphicsModule::DeinitializeSelf()
{
	Devices.Clear();
	Monitors.Clear(false);

	ZESize WindowCount = Windows.GetCount();
	for (ZESize I = 0; I < WindowCount; ++I)
	{
		if (Windows[I] != NULL)
		{
			Windows[I]->Destroy();
			Windows[I] = NULL;
		}
	}
	Windows.Clear(false);

	return ZEModule::Deinitialize();
}

ZEGRGraphicsModule::ZEGRGraphicsModule()
{
	TextureQuality = ZE_TQ_HIGH;
}

ZEGRGraphicsModule::~ZEGRGraphicsModule()
{

}
