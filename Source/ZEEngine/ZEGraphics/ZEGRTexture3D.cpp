//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRTexture3D.cpp
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

#include "ZEGRTexture3D.h"

#include "ZEGRGraphicsModule.h"
#include "ZEMath/ZEMath.h"

bool ZEGRTexture3D::Initialize(ZEUInt Width, ZEUInt Height, ZEUInt Depth, ZEUInt LevelCount, ZEGRFormat Format, ZEGRResourceUsage Usage, ZEFlags BindFlags)
{
	this->Width = Width;
	this->Height = Height;
	this->Depth = Depth;

	SetLevelCount(LevelCount);
	SetFormat(Format);
	SetResourceUsage(Usage);
	SetResourceBindFlags(BindFlags);

	SetSize(Depth * CalculateSize(Width, Height, LevelCount, Format));
	ZEGR_COUNTER_RESOURCE_INCREASE(this, Texture3D, Texture);

	return true;
}

void ZEGRTexture3D::Deinitialize()
{
	SetSize(0);
	ZEGR_COUNTER_RESOURCE_DECREASE(this, Texture3D, Texture);
}

ZEGRResourceType ZEGRTexture3D::GetResourceType() const
{
	return ZEGR_RT_TEXTURE;
}

ZEGRTextureType ZEGRTexture3D::GetTextureType() const
{
	return ZEGR_TT_3D;
}

ZEUInt ZEGRTexture3D::GetWidth() const
{
	return Width;
}

ZEUInt ZEGRTexture3D::GetHeight() const
{
	return Height;
}

ZEUInt ZEGRTexture3D::GetDepth() const
{
	return Depth;
}

ZEVector3 ZEGRTexture3D::GetPixelSize() const
{
	return ZEVector3(1.0f / (float)Width, 1.0f / (float)Height, 1.0f / (float)Depth);
}

ZEGRTexture3D::ZEGRTexture3D()
{
	Width = 0;
	Height = 0;
	Depth = 0;
}

ZEHolder<ZEGRTexture3D> ZEGRTexture3D::Create(ZEUInt Width, ZEUInt Height, ZEUInt Depth, ZEUInt LevelCount, ZEGRFormat Format, ZEGRResourceUsage Usage, ZEFlags BindFlags)
{
	zeCheckError(Width == 0, NULL, "Width cannot be 0.");
	zeCheckError(Height == 0, NULL, "Height cannot be 0.");
	zeCheckError(Depth == 0, NULL, "Depth cannot be 0.");
	zeCheckError(LevelCount == 0, NULL, "Level cannot be 0.");
	zeCheckError(LevelCount > 1 && (!ZEMath::IsPowerOfTwo(Width) || !ZEMath::IsPowerOfTwo(Height) || !ZEMath::IsPowerOfTwo(Depth)), NULL, "Level must be 1 for non-power of two textures.");

	ZEGRTexture3D* Texture = ZEGRGraphicsModule::GetInstance()->CreateTexture3D();
	if (Texture == NULL)
		return NULL;

	if (!Texture->Initialize(Width, Height, Depth, LevelCount, Format, Usage, BindFlags))
	{
		Texture->Destroy();
		return NULL;
	}

	return Texture;
}
