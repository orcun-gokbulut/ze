//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRTexture3D.h
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
#include "ZEMath/ZEVector.h"

class ZEGRRenderTarget;

class ZEGRTexture3DOptions : public ZEGRTextureOptions
{
	public:
		ZEUInt									HorizontalTileCount;
		ZEUInt									VerticalTileCount;

												ZEGRTexture3DOptions();
};

class ZEGRTexture3D : public ZEGRTexture
{
	ZE_OBJECT
	ZE_DISALLOW_COPY(ZEGRTexture3D)
	friend class ZERSTemplates;
	private:
		ZEUInt									Width;
		ZEUInt									Height;
		ZEUInt									Depth;
		ZEGRTexture3DOptions					TextureOptions;

		static ZERSResource*					Instanciator(const void* Parameter);

	protected:
		static bool								CheckParameters(ZEUInt Width, ZEUInt Height, ZEUInt Depth, ZEUInt LevelCount, ZEGRFormat Format, ZEGRResourceUsage Usage, ZEFlags BindFlags, const void* Data);

		virtual bool							Initialize(ZEUInt Width, ZEUInt Height, ZEUInt Depth, ZEUInt LevelCount, ZEGRFormat Format, ZEGRResourceUsage Usage, ZEFlags BindFlag, const void* Data);
		virtual void							Deinitialize();	

		ZETaskResult							LoadInternal();
		ZETaskResult							UnloadInternal();

												ZEGRTexture3D();
		virtual									~ZEGRTexture3D();

	public:
		virtual ZEGRResourceType				GetResourceType() const;
		virtual ZEGRTextureType					GetTextureType() const;

		ZEUInt									GetWidth() const;
		ZEUInt									GetHeight() const;
		ZEUInt									GetDepth() const;
		ZEVector3								GetPixelSize() const;

		virtual void							UpdateSubResource(ZEUInt DestLevel, const void* SrcData, ZESize SrcRowPitch, ZESize SrcDepthPitch) = 0;

		virtual const ZEGRRenderTarget*			GetRenderTarget(ZEUInt Depth, ZEUInt MipLevel = 0) const = 0;

		static ZEHolder<ZEGRTexture3D>			CreateResource(ZEUInt Width, ZEUInt Height, ZEUInt Depth, ZEUInt LevelCount, ZEGRFormat Format, 
													ZEGRResourceUsage Usage = ZEGR_RU_GPU_READ_WRITE_CPU_WRITE, ZEFlags BindFlags = ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_RENDER_TARGET, 
													const void* Data = NULL);
		static ZEHolder<const ZEGRTexture3D>	CreateResourceShared(const ZEGUID& GUID, ZEUInt Width, ZEUInt Height, ZEUInt Depth, ZEUInt LevelCount, ZEGRFormat Format, 
													ZEGRResourceUsage Usage = ZEGR_RU_GPU_READ_WRITE_CPU_WRITE, ZEFlags BindFlags = ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_RENDER_TARGET, 
													const void* Data = NULL, ZEGRTexture3D** StagingResource = NULL);

		static ZEHolder<ZEGRTexture3D>			LoadResource(const ZEString& FileName, const ZEGRTexture3DOptions& TextureOptions);
		static ZEHolder<const ZEGRTexture3D>	LoadResourceShared(const ZEString& FileName, const ZEGRTexture3DOptions& TextureOptions);
};
