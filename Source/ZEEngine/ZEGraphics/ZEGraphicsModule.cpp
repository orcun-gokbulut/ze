//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGraphicsModule.cpp
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

#include "ZEShader.h"
#include "ZEStatePool.h"
#include "ZETexture2D.h"
#include "ZETexture3D.h"
#include "ZECore/ZECore.h"
#include "ZEIndexBuffer.h"
#include "ZETextureCube.h"
#include "ZERenderTarget.h"
#include "ZEVertexBuffer.h"
#include "ZECore/ZEOption.h"
#include "ZEGraphicsModule.h"
#include "ZEConstantBuffer.h"
#include "ZEGraphicsWindow.h"
#include "ZEDepthStencilBuffer.h"
#include "ZECore/ZEOptionManager.h"

#include <FreeImage.h>

ZEOptionSection ZEGraphicsModule::GraphicsOptions;

ZE_MODULE_DESCRIPTION_ABSTRACT(ZEGraphicsModule, ZEModule, &ZEGraphicsModule::GraphicsOptions)

void FreeImageOutput(FREE_IMAGE_FORMAT Bitmap, const char* Message)
{
	zeLog("%s", Message);
}

void ZEGraphicsModule::BaseInitialize()
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

void ZEGraphicsModule::BaseDeinitialize()
{
	FreeImage_DeInitialise();

	ZEOptionManager::GetInstance()->UnregisterSection(&GraphicsOptions);
}

ZETextureOptions* ZEGraphicsModule::GetTextureOptions()
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

ZEGraphicsWindow* ZEGraphicsModule::GetWindow(ZEUInt WindowId) const
{
	ZESize Count = Windows.GetCount();
	for (ZESize I = 0; I < Count; ++I)
	{
		if (Windows[I]->GetId() == WindowId)
		{
			return Windows[I];
		}
	}
	return NULL;
}

const ZEArray<ZEGraphicsWindow*>& ZEGraphicsModule::GetWindows() const
{
	return Windows;
}

const ZEGraphicsMonitor* ZEGraphicsModule::GetMonitor(ZEUInt MonitorId) const
{
	ZESize Count = Monitors.GetCount();
	for (ZESize I = 0; I < Count; ++I)
	{
		if (Monitors[I]->GetId() == MonitorId)
		{
			return Monitors[I];
		}
	}
	return NULL;
}

const ZEArray<ZEGraphicsMonitor*>& ZEGraphicsModule::GetMonitors() const
{
	return Monitors;
}

ZEGraphicsDevice* ZEGraphicsModule::GetDevice(ZESize Index) const
{
	zeDebugCheck(Index >= Devices.GetCount(), "Index out of range");
	return Devices[Index];
}

const ZEArray<ZEGraphicsDevice*>& ZEGraphicsModule::GetDevices() const
{
	return Devices;
}

void ZEGraphicsModule::GetStatistics(ZEGraphicsStatistics& Statistics) const
{
	Statistics.BlendStateCount = ZEStatePool::BlendStateCount;
	Statistics.SamplerStateCount = ZEStatePool::SamplerStateCount;
	Statistics.DepthStencilCount = ZEStatePool::DepthStencilStateCount;
	Statistics.RasterizerCount = ZEStatePool::RasterizerStateCount;
	Statistics.VertexLayoutCount = ZEStatePool::VertexLayoutCount;

	Statistics.ShaderCount = ZEShader::TotalCount;
	Statistics.ShaderSize = ZEShader::TotalSize;

	Statistics.Texture2DCount = ZETexture2D::TotalCount;
	Statistics.Texture2DSize = ZETexture2D::TotalSize;
	
	Statistics.Texture3DCount = ZETexture3D::TotalCount;
	Statistics.Texture3DSize = ZETexture3D::TotalSize;
	
	Statistics.TextureCubeCount	= ZETextureCube::TotalCount;
	Statistics.TextureCubeSize = ZETextureCube::TotalSize;
	
	Statistics.DepthStancilBufferCount = ZEDepthStencilBuffer::TotalCount;
	Statistics.DepthStancilBufferSize = ZEDepthStencilBuffer::TotalSize;
	
	Statistics.IndexBufferCount	= ZEIndexBuffer::TotalCount;
	Statistics.IndexBufferSize = ZEIndexBuffer::TotalSize;
	
	Statistics.VertexBufferCount = ZEVertexBuffer::TotalCount;
	Statistics.VertexBufferSize	= ZEVertexBuffer::TotalSize;
	
	Statistics.ConstantBufferCount = ZEConstantBuffer::TotalCount;
	Statistics.ConstantBufferSize = ZEConstantBuffer::TotalSize;	
	
	Statistics.RenderTargetCount = ZERenderTarget::TotalCount;	
}

ZEGraphicsModule* ZEGraphicsModule::GetInstance()
{
	return ZECore::GetInstance()->GetGraphicsModule();
}

bool ZEGraphicsModule::InitializeSelf()
{
	if (!ZEModule::InitializeSelf())
		return false;

	ZEGraphicsWindow* MainWindow = ZEGraphicsWindow::CreateInstance();
	MainWindow->Initialize();

	return true;
}

bool ZEGraphicsModule::DeinitializeSelf()
{
	Devices.Clear();
	Monitors.Clear(false);

	ZESize WindowCount = Windows.GetCount();
	for (ZESize I = 0; I < WindowCount; ++I)
	{
		ZE_DESTROY(Windows[I]);
	}
	Windows.Clear(false);

	return ZEModule::Deinitialize();
}

ZEGraphicsModule::ZEGraphicsModule()
{
	TextureQuality = ZE_TQ_HIGH;
}

ZEGraphicsModule::~ZEGraphicsModule()
{

}
