//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11Texture.h
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

#include "ZEGraphics\ZEGRTexture.h"
#include "ZED11ComponentBase.h"

template<typename Type>
class ZED11TextureView : public ZEGRTextureView
{
	friend class ZED11Texture;
	friend class ZED11Context;
	private:
		Type*				View;
		
							ZED11TextureView();
							ZED11TextureView(ZEGRFormat Format, ZEUInt LevelMin, ZEUInt LevelCount, ZEUInt LayerMin, ZEUInt LayerCount);

		virtual				~ZED11TextureView();
};

typedef ZED11TextureView<ID3D11ShaderResourceView>	ZED11ShaderResourceView;
typedef ZED11TextureView<ID3D11UnorderedAccessView>	ZED11UnorderedAccessView;

class ZED11Texture : public ZEGRTexture, public ZED11ComponentBase
{
	friend class ZED11Context;
	friend class ZED11Module;
	friend class ZED11Output;
	private:
		ID3D11Resource*								Resource;

		bool										Initialize2D(ZEUInt Width, ZEUInt Height, ZEUInt LevelCount, ZEGRFormat Format, ZEGRResourceUsage Usage, ZEGRResourceBindFlags BindFlags, ZEUInt ArrayCount, ZEUInt SampleCount, const void* Data);
		bool										Initialize3D(ZEUInt Width, ZEUInt Height, ZEUInt LevelCount, ZEGRFormat Format, ZEGRResourceUsage Usage, ZEGRResourceBindFlags BindFlags, ZEUInt Depth, ZEUInt SampleCount, const void* Data);

		virtual bool								Initialize(ZEGRTextureType TextureType, ZEUInt Width, ZEUInt Height, ZEUInt LevelCount, ZEGRFormat Format, ZEGRResourceUsage Usage, ZEGRResourceBindFlags BindFlags, ZEUInt DepthOrArrayCount, ZEUInt SampleCount, const void* Data);
		virtual void								Deinitialize();

		virtual const ZEGRTextureView*				GetView(ZEGRFormat Format = ZEGR_TF_NONE, ZEUInt LevelMin = 0, ZEUInt LevelCount = 0, ZEUInt LayerMin = 0, ZEUInt LayerCount = 0) const;
		virtual const ZEGRTextureView*				GetRWView(ZEGRFormat Format = ZEGR_TF_NONE, ZEUInt Level = 0, ZEUInt LayerMin = 0, ZEUInt LayerCount = 0) const;

		virtual const ZEGRRenderTarget*				GetRenderTarget(ZEUInt ArrayIndex = 0, ZEUInt Level = 0) const;
		virtual const ZEGRDepthStencilBuffer*		GetDepthStencilBuffer(bool ReadOnly, ZEUInt DepthOrArrayIndex) const;

		virtual bool								Map(ZEGRResourceMapType MapType, void** Buffer, ZESize* RowPitch = NULL, ZESize* DepthPitch = NULL, ZEUInt ArrayIndex = 0, ZEUInt Level = 0);
		virtual void								Unmap(ZEUInt ArrayIndex = 0, ZEUInt Level = 0);

		virtual void								Update(const void* SrcData, ZESize SrcRowPitch, ZESize SrcDepthPitch = 0, ZEUInt DestArrayIndex = 0, ZEUInt DestLevel = 0, const ZERect* DestRect = NULL);

													ZED11Texture();
		virtual										~ZED11Texture();
};
