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

#include "ZEGraphicsModule.h"
#include "ZECore/ZEOptionManager.h"
#include "ZECore/ZEOption.h"
#include "ZECore/ZECore.h"
#include "ZETexture/ZETexture2DResource.h"

#include <freeimage.h>

ZEOptionSection ZEGraphicsModule::GraphicsOptions;

ZE_MODULE_DESCRIPTION_ABSTRACT(ZEGraphicsModule, ZEModule, &ZEGraphicsModule::GraphicsOptions)

ZETextureOptions* ZEGraphicsModule::GetTextureOptions()
{
	static ZETextureOptions VeryHigh	= {ZE_TCT_NONE,		ZE_TCQ_HIGH,    ZE_TDS_NONE,	ZE_TFC_DISABLED,	ZE_TMM_ENABLED,		25};
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
		
		default:
		case ZE_TQ_HIGH:
			return &High;
			break;
		
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

ZESize ZEGraphicsModule::GetCurrentFrameId()
{
	return CurrentFrameId;
}

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
	GraphicsOptions.AddOption(new ZEOption("Fullscreen", false, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("VerticalSync", false, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("Antialiasing", 0, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("AnisotropicFilter", 0, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("AnisotropicFilterLevel", 8, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("ShaderQuality", 5, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("TextureQuality", ZE_TQ_HIGH, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("ModelQuality", 5, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("PostEffectQuality", 5, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("HDRQuality", 5, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("ShadowQuality", 1, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("LightQuantity", 1, ZE_OA_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("NearZ", 10.0f, ZE_OA_INTERNAL));
	GraphicsOptions.AddOption(new ZEOption("FarZ", 20000.0f, ZE_OA_INTERNAL));
	ZEOptionManager::GetInstance()->RegisterSection(&GraphicsOptions);
}

void ZEGraphicsModule::BaseDeinitialize()
{
	FreeImage_DeInitialise();
	ZEOptionManager::GetInstance()->UnregisterSection(&GraphicsOptions);
}

void ZEGraphicsModule::SetScreenSize(ZEInt Width, ZEInt Height)
{
	ScreenWidth = Width;
	ScreenHeight = Height;
}

void ZEGraphicsModule::GetScreenSize(ZEInt& Width, ZEInt& Height)
{
	Width = ScreenWidth;
	Height = ScreenHeight;
}

ZEInt ZEGraphicsModule::GetScreenWidth()
{
	return ScreenWidth;
}

ZEInt ZEGraphicsModule::GetScreenHeight()
{
	return ScreenHeight;
}

float ZEGraphicsModule::GetAspectRatio()
{
	return (float)ScreenWidth / (float)ScreenHeight;
}

void ZEGraphicsModule::SetNearZ(float NearZ)
{
	this->NearZ = NearZ;
}

float ZEGraphicsModule::GetNearZ()
{
	return NearZ;
}

void ZEGraphicsModule::SetFarZ(float FarZ)
{
	this->FarZ = FarZ;
}

float ZEGraphicsModule::GetFarZ()
{
	return FarZ;
}

void ZEGraphicsModule::SetVerticalSync(bool Enabled)
{
	VerticalSync = Enabled;
}

bool ZEGraphicsModule::GetVerticalSync()
{
	return VerticalSync;
}

void ZEGraphicsModule::SetShaderQuality(ZEInt Quality)
{
	ShaderQuality = Quality;
}

ZEInt ZEGraphicsModule::GetShaderQuality()
{
	return ShaderQuality;
}

void ZEGraphicsModule::SetTextureQuality(ZETextureQuality Quality)
{
	TextureQuality = Quality;
}

ZETextureQuality ZEGraphicsModule::GetTextureQuality()
{
	return TextureQuality;
}

void ZEGraphicsModule::SetModelQuality(ZEInt Quality)
{
	ModelQuality = Quality;
}

ZEInt ZEGraphicsModule::GetModelQuality()
{
	return ModelQuality;
}

void ZEGraphicsModule::SetShadowQuality(ZEInt Quality)
{
	ShadowQuality = Quality;
}

ZEInt ZEGraphicsModule::GetShadowQuality()
{
	return ShadowQuality;
}

void ZEGraphicsModule::SetPostEffectQuality(ZEInt Quality)
{
	PostEffectQuality = Quality;
}

ZEInt ZEGraphicsModule::GetPostEffectQuality()
{
	return PostEffectQuality;
}

void ZEGraphicsModule::SetHDRQuality(ZEInt Quality)
{
	HDRQuality = Quality;
}

ZEInt ZEGraphicsModule::GetHDRQuality()
{
	return HDRQuality;
}

void ZEGraphicsModule::SetAntiAliasing(ZEInt Level)
{
	AntiAliasing = Level;
}

ZEInt ZEGraphicsModule::GetAntiAliasing()
{
	return AntiAliasing;
}

void ZEGraphicsModule::SetAnisotropicFilter(ZEUInt Level)
{
	AnisotropicFilter = Level;
}

ZEUInt ZEGraphicsModule::GetAnisotropicFilter()
{
	return AnisotropicFilter;
}


ZEGraphicsModule* ZEGraphicsModule::GetInstance()
{
	return ZECore::GetInstance()->GetGraphicsModule();
}
