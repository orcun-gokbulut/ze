//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETexture3D.cpp
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

#include "ZETexture3D.h"
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

static ZESize CalculateTexture3DSize(ZEUInt Width, ZEUInt Height, ZEUInt Depth, ZEUInt LevelCount, ZETexturePixelFormat PixelFormat)
{
	float TotalSize = 0;
	float PixelSize = GetAveragePixelSize(PixelFormat);
	
	for (ZESize LevelN = LevelCount; LevelCount > 0; --LevelN)
	{
		ZEUInt32 LevelIndex = (ZEUInt32)LevelN - 1;

		float PixelCount = (float)((Width >> LevelIndex) * (Height >> LevelIndex) * (Depth >> LevelIndex));
		TotalSize += PixelCount * PixelSize;
	}
	return (ZESize)TotalSize;
}

bool ZETexture3D::UpdateWith(ZEUInt ShadowIndex)
{
	ShadowCopy.SetChanged(ShadowIndex, false);
	return true;
}

ZEGraphicsResourceType ZETexture3D::GetResourceType() const
{
	return ZE_GRT_TEXTURE;
}

ZESize ZETexture3D::GetSize() const
{
	return Size;
}

ZEUInt ZETexture3D::GetWidth() const
{
	return Width;
}

ZEUInt ZETexture3D::GetHeight() const
{
	return Height;
}

ZEUInt ZETexture3D::GetDepth() const
{
	return Depth;
}

ZEUInt ZETexture3D::GetLevelCount() const
{
	return LevelCount;
}

const ZEVector3& ZETexture3D::GetPixelSize() const
{
	return PixelSize;
}

bool ZETexture3D::Unlock()
{
	zeDebugCheck(!GetIsCreated(), "Buffer not created.");
	zeDebugCheck(State.IsStatic, "Static buffer not be locked/unlocked.");

	State.IsLocked = false;

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Dynamic 3D texture unlocked. Texture3D: %p", this);
#endif

	return true;
}

bool ZETexture3D::Lock(void** Buffer, ZESize* RowPitch, ZESize* SlicePitch)
{
	zeDebugCheck(!GetIsCreated(), "Texture not created.");
	zeDebugCheck(Buffer == NULL, "NULL Pointer.");
	zeDebugCheck(RowPitch == NULL, "NULL Pointer.");
	zeDebugCheck(SlicePitch == NULL, "SlicePitch cannot be null");
	zeDebugCheck(State.IsLocked, "Already locked.");
	zeDebugCheck(State.IsStatic, "Static buffer not be locked/unlocked.");

	*Buffer = ShadowCopy.GetData();
	*RowPitch = ShadowCopy.GetRowSize();
	*SlicePitch = ShadowCopy.GetSliceSize();

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Dynamic 3D texture locked. Texture3D: %p, Buffer: %p , RowPitch: %u , SlicePitch: %u", 
			this, *Buffer, *RowPitch, *SlicePitch);
#endif

	return true;
}

bool ZETexture3D::CreateDynamic(ZEUInt Width, ZEUInt Height, ZEUInt Depth, ZETexturePixelFormat PixelFormat, ZETextureData* Data)
{
	zeDebugCheck(GetIsCreated(), "Texture already created.");
	zeDebugCheck(Width == 0, "Width cannot be zero");
	zeDebugCheck(Height == 0, "Height cannot be zero");
	zeDebugCheck(Depth == 0, "Depth cannot be zero");
	zeDebugCheck(PixelFormat == ZE_TPF_NOTSET, "PixelFormat must be set");
	zeDebugCheck(Width > 2048 || Height > 2048 || Depth > 2048, "Texture dimensions exceeds the limits.");

	ZESize RowSize = ZETextureLevel::GetRowSize(Width, PixelFormat);
	ZEUInt RowCount = ZETextureLevel::GetRowCount(Height, PixelFormat);
	ZESize SliceSize = ZETextureSurface::GetSurfaceSize(RowCount, RowSize);
	ShadowCopy.CreateTexture(RowCount, RowSize, Depth, SliceSize, Data);

	this->Width = Width;
	this->Height = Height;
	this->Depth	= Depth;
	this->LevelCount = 1;
	this->PixelFormat = PixelFormat;
	this->PixelSize = ZEVector3::One / ZEVector3((float)Width, (float)Height, (float)Depth);
	this->Size = CalculateTexture3DSize(Width, Height, Depth, 1, PixelFormat);

	this->State.IsCreated = true;
	this->State.IsStatic = false;
	this->IsRenderTarget = false;

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Dynamic 3D texture created. Texture3D: %p, Width: %u, Height: %u, Depth: %u, LevelCount: %u, PixelFormat: %u", 
			this, Width, Height, Depth, 1, PixelFormat);
#endif

	TotalSize += Size;
	TotalCount++;

	return true;
}

bool ZETexture3D::CreateStatic(ZEUInt Width, ZEUInt Height, ZEUInt Depth, ZEUInt LevelCount, ZETexturePixelFormat PixelFormat, bool RenderTarget, ZETextureData* Data)
{
	zeDebugCheck(GetIsCreated(), "Texture already created.");
	zeDebugCheck(Width == 0, "Width cannot be zero");
	zeDebugCheck(Height == 0, "Height cannot be zero");
	zeDebugCheck(Depth == 0, "Depth cannot be zero");
	zeDebugCheck(LevelCount == 0, "LevelCount cannot be zero");
	zeDebugCheck(PixelFormat == ZE_TPF_NOTSET, "PixelFormat must be valid");
	zeDebugCheck(RenderTarget && LevelCount != 1, "Render target's LevelCount must be one ");
	zeDebugCheck(Width > 2048 || Height > 2048 || Depth > 2048, "Texture dimensions exceeds the limits.");

	this->Width = Width;
	this->Height = Height;
	this->Depth	= Depth;
	this->LevelCount = LevelCount;
	this->PixelFormat = PixelFormat;
	this->PixelSize = ZEVector3(1.0f / Width, 1.0f / Height, 1.0f / Depth);
	this->Size = CalculateTexture3DSize(Width, Height, Depth, LevelCount, PixelFormat);

	this->IsRenderTarget = RenderTarget;
	this->State.IsCreated = true;
	this->State.IsStatic = true;
	
#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Static 3D texture created. Texture3D: %p, Width: %u, Height: %u, Depth: %u, LevelCount: %u, PixelFormat: %u", 
			this, Width, Height, Depth, LevelCount, PixelFormat);
#endif

	TotalSize += Size;
	TotalCount++;

	return true;
}

ZESize		ZETexture3D::TotalSize = 0;
ZEUInt16	ZETexture3D::TotalCount = 0;

ZETexture3D::ZETexture3D()
{
	Size = 0;
	Width = 0;
	Height = 0;
	Depth = 0;
	LevelCount = 0;
	TextureType = ZE_TT_3D;
	PixelSize = ZEVector3::Zero;
};

ZETexture3D::~ZETexture3D()
{
	if (GetIsCreated())
	{
		TotalSize -= Size;
		TotalCount--;
	}
};

ZETexture3D* ZETexture3D::CreateInstance()
{
	return zeGraphics->CreateTexture3D();
}
