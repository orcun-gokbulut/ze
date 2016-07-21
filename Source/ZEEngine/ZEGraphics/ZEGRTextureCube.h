//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRTextureCube.h
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

#include "ZEGRTexture.h"

#include "ZETypes.h"

enum ZEGRTextureCubeFace
{
	ZEGR_TCF_POSITIVEX		= 0,
	ZEGR_TCF_NEGATIVEX		= 1,
	ZEGR_TCF_POSITIVEY		= 2,
	ZEGR_TCF_NEGATIVEY		= 3,
	ZEGR_TCF_POSITIVEZ		= 4,
	ZEGR_TCF_NEGATIVEZ		= 5
};

class ZEGRRenderTarget;

class ZEGRTextureCube : public ZEGRTexture
{
	private:
		ZEUInt										Length;

	protected:
		virtual	bool								Initialize(ZEUInt Length, ZEUInt LevelCount, ZEGRFormat Format, ZEGRResourceUsage Usage, ZEFlags BindFlags, const void* Data) = 0;
		virtual void								Deinitialize();

													ZEGRTextureCube();

	public:
		ZEGRResourceType							GetResourceType() const;
		ZEGRTextureType								GetTextureType() const;

		ZEUInt										GetLength() const;
		float										GetPixelSize() const;

		virtual void								UpdateSubResource(ZEGRTextureCubeFace DestFace, ZEUInt DestLevel, const void* SrcData, ZESize SrcRowPitch) = 0;
		
		virtual const ZEGRRenderTarget*				GetRenderTarget(ZEGRTextureCubeFace Face, ZEUInt Level = 0) const = 0;

		static ZEHolder<ZEGRTextureCube>			CreateInstance(
																	ZEUInt Length, 
																	ZEUInt LevelCount, 
																	ZEGRFormat Format, 
																	ZEGRResourceUsage Usage = ZEGR_RU_GPU_READ_WRITE_CPU_WRITE, 
																	ZEFlags BindFlags = ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_RENDER_TARGET, 
																	const void* Data = NULL);

		static ZEHolder<ZEGRTextureCube>			CreateFromFile(const ZEString& Filename, const ZEGRTextureOptions& TextureOptions);
};
