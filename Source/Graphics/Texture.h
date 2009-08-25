//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Texture.h
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
#ifndef __ZE_TEXTURE_H__
#define __ZE_TEXTURE_H__

enum ZETextureType
{
	ZE_TT_SURFACE,
	ZE_TT_VOLUME,
	ZE_TT_CUBE
};

enum ZECubeTextureFace
{
	ZE_CTF_POSITIVEX	= 0,
	ZE_CTF_NEGATIVEX	= 1,
	ZE_CTF_POSITIVEY	= 2,
	ZE_CTF_NEGATIVEY	= 3,
	ZE_CTF_POSITIVEZ	= 4,
	ZE_CTF_NEGATIVEZ	= 5
};

enum ZETexturePixelFormat
{
	ZE_TPF_NOTSET,
	ZE_TPF_ARGB32,
	ZE_TPF_HDR,
	ZE_TPF_SHADOWMAP,
	ZE_TPF_DEPTH
};

class ZETextureBase
{
	public:
		virtual ZETextureType			GetTextureType() const = 0;

		virtual bool					IsEmpty() const = 0;

		virtual void					Release() = 0;
		virtual void					Destroy() = 0;
};

class ZETexture : public ZETextureBase
{
	protected:
		int								Width;
		int								Height;
		ZETexturePixelFormat			PixelFormat;
		bool							RenderTarget;

	public:
		virtual ZETextureType			GetTextureType() const;

		int								GetWidth() const;
		int								GetHeight() const;
		ZETexturePixelFormat			GetPixelFormat() const;
		bool							IsRenderTarget() const;

		virtual bool					IsEmpty() const = 0;

		virtual bool					Create(int Width, int Height, ZETexturePixelFormat PixelFormat, bool RenderTarget = false) = 0;
		virtual void					Lock(void** Buffer, int* Pitch) = 0;
		virtual void					Unlock() = 0;

										ZETexture();
};

class ZEVolumeTexture : public ZETextureBase
{
	protected:
		int								Width;
		int								Height;
		int								Depth;
		ZETexturePixelFormat			PixelFormat;

	public:
		virtual ZETextureType			GetTextureType() const;

		int								GetWidth() const;
		int								GetHeight() const;
		int								GetDepth() const;
		ZETexturePixelFormat			GetPixelFormat() const;
		bool							IsRenderTarget() const;

		virtual bool					Create(int Width, int Height, int Depth, ZETexturePixelFormat PixelFormat) = 0;
		virtual void					Lock(void** Buffer, int* RowPitch, int* SlicePitch) = 0;
		virtual void					Unlock() = 0;

										ZEVolumeTexture();
};

class ZECubeTexture : public ZETextureBase
{
	protected:
		int								EdgeLength;
		ZETexturePixelFormat			PixelFormat;
		bool							RenderTarget;

	public:
		virtual ZETextureType			GetTextureType() const;
		int								GetEdgeLength() const;
		ZETexturePixelFormat			GetPixelFormat() const;
		bool							IsRenderTarget() const;

		virtual	bool					Create(int EdgeLength, ZETexturePixelFormat PixelFormat, bool RenderTarget = false) = 0;
		virtual void					Lock(ZECubeTextureFace Face, void** Buffer, int* Pitch) = 0;
		virtual void					Unlock(ZECubeTextureFace Face) = 0;

										ZECubeTexture();
};
#endif
