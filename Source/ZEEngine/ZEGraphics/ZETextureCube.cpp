//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureCube.cpp
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

#include "ZETextureCube.h"
#include "ZEGraphicsModule.h"
#include "ZETexture/ZETextureData.h"

inline static float GetAveragePixelSize(ZETexturePixelFormat PixelFormat)
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

static ZESize CalculateTextureCubeSize(ZEUInt EdgeLength, ZEUInt LevelCount, ZETexturePixelFormat PixelFormat)
{
	float TotalSize = 0;
	float PixelSize = GetAveragePixelSize(PixelFormat);
	
	for (ZESize LevelN = LevelCount; LevelN > 0; --LevelN)
	{
		ZEUInt32 LevelIndex = (ZEUInt32)LevelN - 1;

		float PixelCount = ZEMath::Power((float)(EdgeLength >> LevelIndex), 2.0f);
		TotalSize += PixelCount * PixelSize;
	}
	return (ZESize)(TotalSize * 6.0f);
}

bool ZETextureCube::UpdateWith(ZEUInt ShadowIndex)
{
	ShadowCopy.SetChanged(ShadowIndex, false);
	return true;
}

ZEGraphicsResourceType ZETextureCube::GetResourceType() const
{
	return ZE_GRT_TEXTURE;
}

ZESize ZETextureCube::GetSize() const
{
	return Size;
}

ZEUInt ZETextureCube::GetEdgeLenght() const
{
	return EdgeLength;
}

ZEUInt ZETextureCube::GetLevelCount() const
{
	return LevelCount;
}

const ZEVector2& ZETextureCube::GetPixelSize() const
{
	return PixelSize;
}

bool ZETextureCube::Unlock(ZETextureCubeFace Face)
{
	zeDebugCheck(!GetIsCreated(), "Buffer not created.");
	zeDebugCheck(GetIsStatic(), "Static buffer not be locked/unlocked.");

	State.IsLocked = false;

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Dynamic cube texture unlocked. Texture2D: %p, Face: %u", this, (ZEUInt)Face);
#endif

	return true;
}

bool ZETextureCube::Lock(void** Buffer, ZESize* Pitch, ZETextureCubeFace Face)
{
	zeDebugCheck(GetIsCreated(), "Texture not created.");
	zeDebugCheck(Pitch == NULL, "Pitch cannot be null");
	zeDebugCheck(Buffer == NULL, "Buffer cannot be null");
	zeDebugCheck(GetIsLocked(), "Already locked.");
	zeDebugCheck(GetIsStatic(), "Static buffer not be locked/unlocked.");

	void* Data = ShadowCopy.GetData();
	ZESize RowSize = ShadowCopy.GetRowSize();

	*Buffer = (ZEUInt8*)Data + Face * RowSize;
	*Pitch = RowSize * 6;

	State.IsLocked = true;

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Dynamic cube texture locked. TextureCube: %p, Buffer: %p, Pitch: %u, Face: %u", 
			this, *Buffer, *Pitch, (ZEUInt)Face);
#endif

	return true;
}

bool ZETextureCube::CreateDynamic(ZEUInt EdgeLength, ZETexturePixelFormat PixelFormat, ZETextureData* InitialData)
{
	zeDebugCheck(GetIsCreated(), "Texture already created.");
	zeDebugCheck(EdgeLength == 0, "EdgeLength cannot be zero");
	zeDebugCheck(EdgeLength > 8192, "EdgeLength exceeds the limits.");
	zeDebugCheck(PixelFormat == ZE_TPF_NOTSET, "PixelFormat must be set");
	
	ZESize RowSize = ZETextureLevel::GetRowSize(EdgeLength, PixelFormat);
	ZEUInt RowCount = ZETextureLevel::GetRowCount(EdgeLength, PixelFormat);
	ZESize SliceSize = ZETextureSurface::GetSurfaceSize(RowCount, RowSize);
	ShadowCopy.CreateTexture(RowCount, RowSize, 6, SliceSize, InitialData);

	this->LevelCount = 1;
	this->EdgeLength = EdgeLength;
	this->PixelFormat = PixelFormat;
	this->Size = CalculateTextureCubeSize(EdgeLength, 1, PixelFormat);
	this->PixelSize = ZEVector2::One / ZEVector2((float)EdgeLength, (float)EdgeLength);

	this->IsRenderTarget = false;
	this->State.IsStatic = false;
	this->State.IsCreated = true;

	TotalSize += Size;
	TotalCount++;

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Dynamic cube texture created. TextureCube: %p, EdgeLength: %u, LevelCount: %u, PixelFormat: %u", 
			this, EdgeLength, 1, PixelFormat);
#endif

	return true;
}

bool ZETextureCube::CreateStatic(ZEUInt EdgeLength, ZEUInt LevelCount, ZETexturePixelFormat PixelFormat, bool RenderTarget, ZETextureData* InitialData)
{
	zeDebugCheck(GetIsCreated(), "Texture already created.");
	zeDebugCheck(EdgeLength == 0, "EdgeLength cannot be zero");
	zeDebugCheck(LevelCount == 0, "LevelCount cannot be zero");
	zeDebugCheck(EdgeLength > 8192, "EdgeLength exceeds the limits.");
	zeDebugCheck(PixelFormat == ZE_TPF_NOTSET, "PixelFormat must be set");
	zeDebugCheck(RenderTarget && LevelCount != 1, "Render target's LevelCount must be one.");

	this->EdgeLength = EdgeLength;
	this->LevelCount = LevelCount;
	this->PixelFormat = PixelFormat;
	this->PixelSize = ZEVector2::One / ZEVector2((float)EdgeLength, (float)EdgeLength);
	this->Size = CalculateTextureCubeSize(EdgeLength, LevelCount, PixelFormat);

	this->IsRenderTarget = RenderTarget;
	this->State.IsStatic = true;
	this->State.IsCreated = true;

	TotalSize += Size;
	TotalCount++;

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Static cube texture created. TextureCube: %p, EdgeLength: %u, LevelCount: %u, PixelFormat: %u, RenderTarget: %u",
			this, EdgeLength, LevelCount, PixelFormat, RenderTarget);
#endif

	return true;
}

ZESize		ZETextureCube::TotalSize = 0;
ZEUInt16	ZETextureCube::TotalCount = 0;

ZETextureCube::ZETextureCube()
{
	Size = 0;
	EdgeLength = 0;
	LevelCount = 0;
	TextureType = ZE_TT_CUBE;
	PixelSize = ZEVector2::Zero;
}

ZETextureCube::~ZETextureCube()
{
	if (GetIsCreated())
	{
		TotalSize -= Size;
		TotalCount--;
	}
}

ZETextureCube* ZETextureCube::CreateInstance()
{
	return zeGraphics->CreateTextureCube();
}
