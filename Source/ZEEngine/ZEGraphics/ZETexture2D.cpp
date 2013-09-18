//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETexture2D.cpp
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

#include "ZETexture2D.h"
#include "ZEGraphicsModule.h"
#include "ZETexture/ZETextureData.h"

inline static float GetSizePerPixel(ZETexturePixelFormat PixelFormat)
{
	float Size = 0.0f;
	switch (PixelFormat)
	{
		case ZE_TPF_R32F: // 4 byte
		case ZE_TPF_RGBA8:
		case ZE_TPF_RG16F:
		case ZE_TPF_RG16:
		case ZE_TPF_INTZ:
			Size = 4.0f;
			break;

		case ZE_TPF_R16F: // 2 byte
		case ZE_TPF_R16:	
		case ZE_TPF_LA8:
			Size = 2.0f;
			break;

		case ZE_TPF_RG32F: // 8 byte
		case ZE_TPF_RGBA16F:
		case ZE_TPF_RGBA16:	
			Size = 8.0f;
			break;

		case ZE_TPF_L8: // 1 byte
			Size = 1.0f;
			break;
			
		case ZE_TPF_NULL: // 0 byte
			Size = 0.0f;
			break;

		case ZE_TPF_DXT1: // 8 byte per 4x4 block
			Size = 8.0f / 16.0f;
			break;
		case ZE_TPF_DXT3: // 16 byte per 4x4 block
			Size = 16.0f / 16.0f;
			break;
		case ZE_TPF_DXT5: // 16 byte per 4x4 block
			Size = 16.0f / 16.0f;
			break;

	};
	return Size;
}

static ZESize CalculateTexture2DSize(ZEUInt Width, ZEUInt Height, ZEUInt LevelCount, ZETexturePixelFormat PixelFormat)
{
	float TotalSize = 0;
	float PixelSize = GetSizePerPixel(PixelFormat);
	
	for (ZESize LevelN = LevelCount; LevelN > 0; --LevelN)
	{
		ZEUInt32 LevelIndex = (ZEUInt32)LevelN - 1;

		float PixelCount = (float)((Width >> LevelIndex) * (Height >> LevelIndex));
		TotalSize += PixelCount * PixelSize;
	}
	return (ZESize)TotalSize;
}

bool ZETexture2D::UpdateWith(ZEUInt ShadowIndex)
{
	ShadowCopy.SetChanged(ShadowIndex, false);
	return true;
}

ZEGraphicsResourceType ZETexture2D::GetResourceType() const
{
	return ZE_GRT_TEXTURE;
}

ZESize ZETexture2D::GetSize() const
{
	return Size;
}

ZEUInt ZETexture2D::GetWidth() const
{
	return Width;
}

ZEUInt ZETexture2D::GetHeight() const
{
	return Height;
}

ZEUInt ZETexture2D::GetLevelCount() const
{
	return LevelCount;
}

const ZEVector2& ZETexture2D::GetPixelSize() const
{
	return PixelSize;
}

bool ZETexture2D::Unlock()
{
	zeDebugCheck(!GetIsCreated(), "Buffer not created.");
	zeDebugCheck(GetIsStatic(), "Static buffer not be locked/unlocked.");

	State.IsLocked = false;

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Dynamic 2D texture unlocked. Texture2D: %p", this);
#endif

	return true;
}

bool ZETexture2D::Lock(void** Buffer, ZESize* Pitch)
{
	zeDebugCheck(!GetIsCreated(), "Texture not created.");
	zeDebugCheck(Buffer == NULL, "Null pointer.");
	zeDebugCheck(Pitch == NULL, "Null pointer.");
	zeDebugCheck(GetIsLocked(), "Already locked.");
	zeDebugCheck(GetIsStatic(), "Static buffer not be locked/unlocked.");

	*Buffer = ShadowCopy.GetData();
	*Pitch = ShadowCopy.GetRowSize();

	State.IsLocked = true;

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Dynamic 2D texture locked. Texture2D: %p, Buffer: %p, Pitch: %u", 
			this, *Buffer, *Pitch);
#endif

	return true;
}

bool ZETexture2D::CreateDynamic(ZEUInt Width, ZEUInt Height, ZETexturePixelFormat PixelFormat, ZETextureData* Data)
{
	zeDebugCheck(GetIsCreated(), "Texture already created.");
	zeDebugCheck(Width == 0, "Width cannot be zero");
	zeDebugCheck(Height == 0, "Height cannot be zero");
	zeDebugCheck(PixelFormat == ZE_TPF_NOTSET, "PixelFormat must be set");
	zeDebugCheck(Width > 8192 || Height > 8192, "Texture2D dimensions exceeds the limits");

	ZESize RowSize = ZETextureLevel::GetRowSize(Width, PixelFormat);
	ZEUInt RowCount = ZETextureLevel::GetRowCount(Height, PixelFormat);
	ZESize SliceSize = ZETextureSurface::GetSurfaceSize(RowCount, RowSize);
	ShadowCopy.CreateTexture(RowCount, RowSize, 1, SliceSize, Data);

	this->Width = Width;
	this->Height = Height;
	this->LevelCount = 1;
	this->PixelFormat = PixelFormat;
	this->Size = CalculateTexture2DSize(Width, Height, LevelCount, PixelFormat);
	this->PixelSize = ZEVector2::One / ZEVector2((float)Width, (float)Height);

	this->IsRenderTarget = false;
	this->State.IsCreated = true;
	this->State.IsStatic = false;

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Dynamic 2D texture created. Texture2D: %p, Width: %u, Height: %u, LevelCount: %u, PixelFormat: %u", 
			this, Width, Height, 1, PixelFormat);
#endif

	TotalSize += Size;
	TotalCount++;

	return true;
}

bool ZETexture2D::CreateStatic(ZEUInt Width, ZEUInt Height, ZEUInt LevelCount, ZETexturePixelFormat PixelFormat, bool RenderTarget, ZETextureData* Data)
{
	zeDebugCheck(GetIsCreated(), "Texture already created.");
	zeDebugCheck(Width == 0, "Width cannot be zero");
	zeDebugCheck(Height == 0, "Height cannot be zero");
	zeDebugCheck(LevelCount == 0, "LevelCount cannot be zero");
	zeDebugCheck(PixelFormat == ZE_TPF_NOTSET, "PixelFormat must be valid");
	zeDebugCheck(RenderTarget && LevelCount != 1, "Render target's LevelCount must be one ");
	zeDebugCheck(Width > 8192 || Height > 8192, "Texture2D dimensions exceeds the limits.");
	
	this->Width = Width;
	this->Height = Height;
	this->LevelCount = LevelCount;
	this->PixelFormat = PixelFormat;
	this->Size = CalculateTexture2DSize(Width, Height, LevelCount, PixelFormat);
	this->PixelSize = ZEVector2(1.0f / Width, 1.0f / Height);

	this->IsRenderTarget = RenderTarget;
	this->State.IsStatic = true;
	this->State.IsCreated = true;

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Static 2D texture created. Texture2D: %p, Width: %u, Height: %u, LevelCount: %u, PixelFormat: %u", 
			this, Width, Height, LevelCount, PixelFormat);
#endif

	TotalSize += Size;
	TotalCount++;

	return true;
}

ZESize		ZETexture2D::TotalSize = 0;
ZEUInt16	ZETexture2D::TotalCount = 0;

ZETexture2D::ZETexture2D()
{
	Width = 0;
	Height = 0;
	LevelCount = 0;
	TextureType = ZE_TT_2D;
};

ZETexture2D::~ZETexture2D()
{
	if (GetIsCreated())
	{
		TotalSize -= Size;
		TotalCount--;
	}
}

ZETexture2D* ZETexture2D::CreateInstance()
{
	return zeGraphics->CreateTexture2D();
}
