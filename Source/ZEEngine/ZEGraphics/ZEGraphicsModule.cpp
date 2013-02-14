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


#include "ZECore/ZECore.h"
#include "ZECore/ZEOption.h"
#include "ZEGraphicsModule.h"
#include "ZECore/ZEOptionManager.h"


ZEOptionSection ZEGraphicsModule::GraphicsOptions;

ZE_MODULE_DESCRIPTION_ABSTRACT(ZEGraphicsModule, ZEModule, &ZEGraphicsModule::GraphicsOptions)

ZEGraphicsModule::ZEGraphicsModule()
{
	ScreenWidth = 0;
	ScreenHeight = 0;
	VerticalSync = false;
	FullScreen = false;
	SampleCount = 1;
	SampleQuality = 0;
	AnisotropicFilter = 16;
	ScreenCount = 0;
	TextureQuality = ZE_TQ_HIGH;
}

ZEGraphicsModule::~ZEGraphicsModule()
{

}

void ZEGraphicsModule::BaseInitialize()
{
	GraphicsOptions.SetName("Graphics");
	GraphicsOptions.AddOption(new ZEOption("ScreenWidth", 640, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("ScreenHeight", 480, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("FullScreen", false, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("VerticalSync", false, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("SampleCount", 1, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("SampleQuality", 0, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("AnisotropicFilter", 0, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("ShaderQuality", 5, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("TextureQuality", ZE_TQ_HIGH, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("ModelQuality", 5, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("PostEffectQuality", 5, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("HDRQuality", 5, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("ShadowQuality", 1, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("LightQuantity", 1, ZE_OA_NORMAL));
	ZEOptionManager::GetInstance()->RegisterSection(&GraphicsOptions);
}

void ZEGraphicsModule::BaseDeinitialize()
{
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

bool ZEGraphicsModule::GetVerticalSync() const
{
	return VerticalSync;
}

ZEUInt ZEGraphicsModule::GetAnisotropicFilter() const
{
	return AnisotropicFilter;
}

ZEUInt ZEGraphicsModule::GetSampleCount() const
{
	return SampleCount;
}

ZEUInt ZEGraphicsModule::GetSampleQuality() const
{
	return SampleQuality;
}

bool ZEGraphicsModule::GetFullScreen() const
{
	return FullScreen;
}

ZEUInt ZEGraphicsModule::GetScreenWidth() const
{
	return ScreenWidth;
}

ZEUInt ZEGraphicsModule::GetScreenHeight() const
{
	return ScreenHeight;
}

ZEUInt ZEGraphicsModule::GetScreenCount() const
{
	return ScreenCount;
}

const ZEViewport& ZEGraphicsModule::GetViewport(ZESize Index) const
{
	zeDebugCheck(Index >= ScreenCount, "Index out of range");

	return Viewports[Index];
}

const ZEScissorRectangle& ZEGraphicsModule::GetScissorRectangle(ZESize Index) const
{
	zeDebugCheck(Index >= ScreenCount, "Index out of range");

	return ScissorRects[Index];
}

const ZERenderTarget* ZEGraphicsModule::GetFrameBuffer(ZESize Index) const
{
	zeDebugCheck(Index >= ScreenCount, "Index out of range");

	return FrameBuffers[Index];
}

const ZEDepthStencilBuffer* ZEGraphicsModule::GetDepthBuffer(ZESize Index) const
{
	zeDebugCheck(Index >= ScreenCount, "Index out of range");

	return DepthBuffers[Index];
}

ZEGraphicsModule* ZEGraphicsModule::GetInstance()
{
	return ZECore::GetInstance()->GetGraphicsModule();
}
