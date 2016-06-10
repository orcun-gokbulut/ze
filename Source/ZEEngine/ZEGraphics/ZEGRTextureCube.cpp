//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRTextureCube.cpp
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

#include "ZEGRTextureCube.h"

#include "ZEGRGraphicsModule.h"
#include "ZEError.h"
#include "ZEMath/ZEMath.h"

bool ZEGRTextureCube::Initialize(ZEUInt Length, ZEUInt LevelCount, ZEGRFormat Format, ZEGRResourceUsage Usage, ZEFlags BindFlags, const void* Data)
{
	this->Length = Length;
	SetLevelCount(LevelCount);
	SetFormat(Format);
	SetResourceUsage(Usage);
	SetResourceBindFlags(BindFlags);

	SetSize(CalculateSize(Length, Length, LevelCount, Format));
	ZEGR_COUNTER_RESOURCE_INCREASE(this, TextureCube, Texture);

	return true;
}

void ZEGRTextureCube::Deinitialize()
{
	SetSize(0);
	ZEGR_COUNTER_RESOURCE_DECREASE(this, TextureCube, Texture);
}


ZEGRResourceType ZEGRTextureCube::GetResourceType() const
{
	return ZEGR_RT_TEXTURE;
}

ZEGRTextureType ZEGRTextureCube::GetTextureType() const
{
	return ZEGR_TT_CUBE;
}

ZEUInt ZEGRTextureCube::GetLength() const
{
	return Length;
}

float ZEGRTextureCube::GetPixelSize() const
{
	return 1.0f / (float)GetLength();
}

ZEGRTextureCube::ZEGRTextureCube()
{
	Length = 0;
}

ZEHolder<ZEGRTextureCube> ZEGRTextureCube::CreateInstance(ZEUInt Length, ZEUInt LevelCount, ZEGRFormat Format, ZEGRResourceUsage Usage, ZEFlags BindFlags, const void* Data)
{	
	zeDebugCheck(Length == 0, "length cannot be 0.");
	zeDebugCheck(Length > ZEGR_MAX_TEXTURECUBE_DIMENSION, "length is too big.");
	zeDebugCheck(LevelCount == 0, "Level cannot be 0.");
	zeDebugCheck(LevelCount > 1 && !ZEMath::IsPowerOfTwo(Length), NULL, "Level must be 1 for non-power of two textures.");
	zeDebugCheck(Usage == ZEGR_RU_GPU_READ_ONLY && Data == NULL, "Immutable textures must be initialized with data");
	zeDebugCheck(Usage == ZEGR_RU_GPU_READ_ONLY && BindFlags.GetFlags(ZEGR_RBF_RENDER_TARGET | ZEGR_RBF_DEPTH_STENCIL), "Immutable textures cannot be bound as render target or depth-stencil");
	zeDebugCheck(Usage == ZEGR_RU_GPU_READ_CPU_WRITE && BindFlags.GetFlags(ZEGR_RBF_RENDER_TARGET | ZEGR_RBF_DEPTH_STENCIL),  "Dynamic textures cannot be bound as render target or depth-stencil");
	zeDebugCheck(Usage == ZEGR_RU_CPU_READ_WRITE && !BindFlags.GetFlags(ZEGR_RBF_NONE), "Staging textures cannot be bound to gpu");
	zeDebugCheck(BindFlags.GetFlags(ZEGR_RBF_RENDER_TARGET) && BindFlags.GetFlags(ZEGR_RBF_DEPTH_STENCIL), "Both render target and depth-stencil bind flags cannot be set");

	ZEGRTextureCube* Texture = ZEGRGraphicsModule::GetInstance()->CreateTextureCube();
	if (Texture == NULL)
		return NULL;

	if (!Texture->Initialize(Length, LevelCount, Format, Usage, BindFlags, Data))
	{
		Texture->Destroy();
		return NULL;
	}

	return Texture;
}