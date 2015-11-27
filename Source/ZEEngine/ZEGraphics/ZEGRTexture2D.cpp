//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRTexture2D.cpp
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

#include "ZEGRTexture2D.h"

#include "ZEGRGraphicsModule.h"
#include "ZETexture/ZETextureData.h"
#include "ZEMath/ZEMath.h"

ZEGRResourceType ZEGRTexture2D::GetResourceType()
{
	return ZEGR_RT_TEXTURE;
}

ZEGRTextureType ZEGRTexture2D::GetTextureType()
{
	return ZEGR_TT_2D;
}

ZEUInt ZEGRTexture2D::GetWidth()
{
	return Width;
}

ZEUInt ZEGRTexture2D::GetHeight()
{
	return Height;
}

ZEVector2 ZEGRTexture2D::GetPixelSize()
{
	return ZEVector2(1.0f / Width, 1.0f / Height);
}

bool ZEGRTexture2D::Initialize(ZEUInt Width, ZEUInt Height, ZEUInt LevelCount, ZEGRFormat Format, bool RenderTarget)
{

	this->Width = Width;
	this->Height = Height;
	SetLevelCount(LevelCount);
	SetFormat(Format);
	SetIsRenderTarget(RenderTarget);

	SetSize(CalculateSize(Width, Height, LevelCount, Format));
	ZEGR_COUNTER_RESOURCE_INCREASE(this, Texture2D, Texture);

	return true;
}

void ZEGRTexture2D::Deinitialize()
{
	SetSize(0);
	ZEGR_COUNTER_RESOURCE_DECREASE(this, Texture2D, Texture);
}

ZEGRTexture2D::ZEGRTexture2D()
{
	Width = 0;
	Height = 0;
};

ZEGRTexture2D* ZEGRTexture2D::CreateInstance(ZEUInt Width, ZEUInt Height, ZEUInt LevelCount, ZEGRFormat Format, bool RenderTarget)
{
	zeCheckError(Width == 0, false, "Width cannot be 0.");
	zeCheckError(Height == 0, false, "Height cannot be 0.");
	zeCheckError(Width > ZEGR_MAX_TEXTURE_DIMENSION, false, "Width is too big.")
	zeCheckError(Height > ZEGR_MAX_TEXTURE_DIMENSION, false, "Width is too big.")
	zeCheckError(LevelCount == 0, false, "Level cannot be 0.");
	zeCheckError(LevelCount > 1 && (!ZEMath::IsPowerOfTwo(Width) || !ZEMath::IsPowerOfTwo(Height)), false, "Level must be 1 for non-power of two textures.");

	ZEGRTexture2D* Texture = ZEGRGraphicsModule::GetInstance()->CreateTexture2D();
	if (Texture == NULL)
		return NULL;

	if (!Texture->Initialize(Width, Height, LevelCount, Format, RenderTarget))
	{
		Texture->Destroy();
		return NULL;
	}

	return Texture;
}
