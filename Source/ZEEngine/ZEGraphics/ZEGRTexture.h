//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRTexture.h
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

#include "ZEGRResource.h"

#include "ZETypes.h"
#include "ZEDS/ZEArray.h"
#include "ZEPointer/ZEHolder.h"

class ZEGRRenderTarget;
class ZEGRDepthStencilBuffer;
class ZEGRTexture;

ZE_EXPORT_ZEENGINE ZE_ENUM_TYPED(ZEGRTextureType, ZEUInt8)
{
	ZEGR_TT_NONE	= 0,
	ZEGR_TT_2D		= 1,
	ZEGR_TT_3D		= 2,
	ZEGR_TT_CUBE	= 3
};

struct ZERect
{
	ZEInt x, y;
	ZEInt Width, Height;
};

class ZE_EXPORT_ZEENGINE ZEGRTextureOptions : public ZEObject
{
	ZE_OBJECT
	public:
		ZEGRTextureType				Type;
		ZEUInt						HorizontalTileCount;
		ZEUInt						VerticalTileCount;
		ZEGRFormat					CompressionFormat;
		ZEUInt						MaximumMipmapLevel;
		bool						GenerateMipMaps;
		bool						sRGB;

									ZEGRTextureOptions();
};

class ZE_EXPORT_ZEENGINE ZEGRTextureView : public ZEObject
{
	ZE_OBJECT
	ZE_DISALLOW_COPY(ZEGRTextureView)
	friend class ZEGRTexture;
	protected:
		const ZEGRTexture*		Texture;
		ZEGRFormat				Format;
		ZEUInt					LevelMin;
		ZEUInt					LevelCount;
		ZEUInt					LayerMin;
		ZEUInt					LayerCount;

								ZEGRTextureView();
								ZEGRTextureView(ZEGRFormat Format, ZEUInt LevelMin, ZEUInt LevelCount, ZEUInt LayerMin, ZEUInt LayerCount);
		virtual					~ZEGRTextureView();

	public:
		const ZEGRTexture*		GetTexture() const;
		ZEGRFormat				GetFormat() const;
		ZEUInt					GetLevelMin() const;
		ZEUInt					GetLevelCount() const;
		ZEUInt					GetLayerMin() const;
		ZEUInt					GetLayerCount() const;
};

class ZE_EXPORT_ZEENGINE ZEGRTexture : public ZEGRResource
{
	ZE_OBJECT
	ZE_DISALLOW_COPY(ZEGRTexture)
	friend class ZEGRContext;
	friend class ZEGRGraphicsModule;
	friend class ZERSTemplates;
	private:
		ZESize												CalculateSize();

	protected:
		ZEUInt												Width;
		ZEUInt												Height;
		ZEUInt												LevelCount;
		ZEUInt												SampleCount;
		ZEUInt												DepthOrArrayCount;
		ZEGRTextureType										Type;
		ZEGRTextureOptions									TextureOptions;
		
		mutable ZEArray<ZEGRTextureView*>					ShaderResourceViews;
		mutable ZEArray<ZEGRTextureView*>					UnorderedAccessViews;

		mutable ZEArray<ZEHolder<ZEGRRenderTarget>>			RenderTargets;
		mutable ZEArray<ZEHolder<ZEGRDepthStencilBuffer>>	DepthStencilBuffers;

		const ZEArray<ZEHolder<ZEGRRenderTarget>>&			GetRenderTargets() const;
		const ZEArray<ZEHolder<ZEGRDepthStencilBuffer>>&	GetDepthStencilBuffers() const;

		virtual bool										Initialize(ZEGRTextureType TextureType, ZEUInt Width, ZEUInt Height, ZEUInt LevelCount, ZEGRFormat Format, ZEGRResourceUsage Usage, ZEGRResourceBindFlags BindFlags, ZEUInt DepthOrArrayCount, ZEUInt SampleCount, const void* Data);
		virtual void										Deinitialize();

		ZETaskResult										LoadInternal();
		ZETaskResult										UnloadInternal();

															ZEGRTexture();
		virtual												~ZEGRTexture();

		static ZERSResource*								Instanciator(const void* Parameters);

	public:
		ZEUInt												GetWidth() const;
		ZEUInt												GetHeight() const;
		ZEUInt												GetLevelCount() const;
		ZEUInt												GetSampleCount() const;
		ZEUInt												GetDepthOrArrayCount() const;
		ZEGRTextureType										GetTextureType() const;

		virtual ZEGRResourceType							GetResourceType() const;

		virtual const ZEGRTextureView*						GetView(ZEGRFormat Format = ZEGR_TF_NONE, ZEUInt LevelMin = 0, ZEUInt LevelCount = 0, ZEUInt LayerMin = 0, ZEUInt LayerCount = 0) const = 0;
		virtual const ZEGRTextureView*						GetRWView(ZEGRFormat Format = ZEGR_TF_NONE, ZEUInt Level = 0, ZEUInt LayerMin = 0, ZEUInt LayerCount = 0) const = 0;

		virtual const ZEGRRenderTarget*						GetRenderTarget(ZEUInt DepthOrArrayIndex = 0, ZEUInt Level = 0) const = 0;
		virtual const ZEGRDepthStencilBuffer*				GetDepthStencilBuffer(bool ReadOnly = false, ZEUInt ArrayIndex = 0) const = 0;

		virtual bool										Map(ZEGRResourceMapType MapType, void** Buffer, ZESize* RowPitch = NULL, ZESize* DepthPitch = NULL, ZEUInt ArrayIndex = 0, ZEUInt Level = 0) = 0;
		virtual void										Unmap(ZEUInt ArrayIndex = 0, ZEUInt Level = 0) = 0;
		
		virtual void										Update(const void* SrcData, ZESize SrcRowPitch, ZESize SrcDepthPitch = 0, ZEUInt DestArrayIndex = 0, ZEUInt DestLevel = 0, const ZERect* DestRect = NULL) = 0;

		static ZEHolder<ZEGRTexture>						CreateResource(ZEGRTextureType TextureType, ZEUInt Width, ZEUInt Height, ZEUInt LevelCount, ZEGRFormat Format, ZEGRResourceUsage Usage = ZEGR_RU_STATIC, ZEGRResourceBindFlags BindFlags = ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_RENDER_TARGET, ZEUInt DepthOrArrayCount = 1, ZEUInt SampleCount = 1, const void* Data = NULL);
		static ZEHolder<const ZEGRTexture>					CreateResourceShared(const ZEGUID& GUID, ZEGRTextureType TextureType, ZEUInt Width, ZEUInt Height, ZEUInt LevelCount, ZEGRFormat Format, ZEGRResourceUsage Usage, ZEGRResourceBindFlags BindFlags, ZEUInt DepthOrArrayCount = 1, ZEUInt SampleCount = 1, const void* Data = NULL, ZEGRTexture** StagingResource = NULL);

		static ZEHolder<ZEGRTexture>						LoadResource(const ZEString& FileName, const ZEGRTextureOptions& TextureOptions);
		static ZEHolder<const ZEGRTexture>					LoadResourceShared(const ZEString& FileName, const ZEGRTextureOptions& TextureOptions);
};
