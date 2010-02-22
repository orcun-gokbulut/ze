//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - GraphicsModule.cpp
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
#include "GraphicsModule.h"
#include "Core/Core.h"
#include <freeimage.h>

ZEOptionSection ZEGraphicsModule::GraphicsOptions;

size_t ZEGraphicsModule::GetCurrentFrameId()
{
	return CurrentFrameId;
}

void ZEGraphicsModule::BaseInitialize()
{
	FreeImage_Initialise();
	GraphicsOptions.SetName("Graphics");
	GraphicsOptions.AddOption(new ZEOption("ScreenWidth", 640, ZEOPTIONATTRIBUTE_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("ScreenHeight", 480, ZEOPTIONATTRIBUTE_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("Fullscreen", false, ZEOPTIONATTRIBUTE_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("VerticalSync", false, ZEOPTIONATTRIBUTE_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("Antialiasing", 0, ZEOPTIONATTRIBUTE_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("AnisotropicFilter", 0, ZEOPTIONATTRIBUTE_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("AnisotropicFilterLevel", 8, ZEOPTIONATTRIBUTE_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("ShaderQuality", 5, ZEOPTIONATTRIBUTE_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("TextureQuality", 5, ZEOPTIONATTRIBUTE_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("ModelQuality", 5, ZEOPTIONATTRIBUTE_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("PostEffectQuality", 5, ZEOPTIONATTRIBUTE_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("HDRQuality", 5, ZEOPTIONATTRIBUTE_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("ShadowQuality", 1, ZEOPTIONATTRIBUTE_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("LightQuantity", 1, ZEOPTIONATTRIBUTE_NORMAL));
	GraphicsOptions.AddOption(new ZEOption("NearZ", 5.0f, ZEOPTIONATTRIBUTE_INTERNAL));
	GraphicsOptions.AddOption(new ZEOption("FarZ", 1024.f, ZEOPTIONATTRIBUTE_INTERNAL));
	zeOptions->RegisterSection(&GraphicsOptions);
}

void ZEGraphicsModule::BaseDeinitialize()
{
	FreeImage_DeInitialise();
	zeOptions->UnregisterSection(&GraphicsOptions);
}

void ZEGraphicsModule::SetScreenSize(int Width, int Height)
{
	ScreenWidth = Width;
	ScreenHeight = Height;
}

void ZEGraphicsModule::GetScreenSize(int& Width, int& Height)
{
	Width = ScreenWidth;
	Height = ScreenHeight;
}

int ZEGraphicsModule::GetScreenWidth()
{
	return ScreenWidth;
}

int ZEGraphicsModule::GetScreenHeight()
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

void ZEGraphicsModule::SetShaderQuality(int Quality)
{
	ShaderQuality = Quality;
}

int ZEGraphicsModule::GetShaderQuality()
{
	return ShaderQuality;
}

void ZEGraphicsModule::SetTextureQuality(int Quality)
{
	ShaderQuality = Quality;
}

int ZEGraphicsModule::GetTextureQuality()
{
	return TextureQuality;
}

void ZEGraphicsModule::SetModelQuality(int Quality)
{
	ShaderQuality = Quality;
}

int ZEGraphicsModule::GetModelQuality()
{
	return ModelQuality;
}

void ZEGraphicsModule::SetShadowQuality(int Quality)
{
	ShadowQuality = Quality;
}

int ZEGraphicsModule::GetShadowQuality()
{
	return ShadowQuality;
}

void ZEGraphicsModule::SetPostEffectQuality(int Quality)
{
	PostEffectQuality = Quality;
}

int ZEGraphicsModule::GetPostEffectQuality()
{
	return PostEffectQuality;
}

void ZEGraphicsModule::SetHDRQuality(int Quality)
{
	HDRQuality = Quality;
}

int ZEGraphicsModule::GetHDRQuality()
{
	return HDRQuality;
}

void ZEGraphicsModule::SetAntiAliasing(int Level)
{
	AntiAliasing = Level;
}

int ZEGraphicsModule::GetAntiAliasing()
{
	return AntiAliasing;
}

void ZEGraphicsModule::SetAnisotropicFilter(int Level)
{
	AnisotropicFilter = Level;
}

bool ZEGraphicsModule::GetAnisotropicFilter()
{
	return AnisotropicFilter;
}


ZEGraphicsModule* ZEGraphicsModule::GetInstance()
{
	return ZECore::GetInstance()->GetGraphicsModule();
}
