//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11Texture2D.h
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

#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZED11ComponentBase.h"

#include "ZETypes.h"
#include "ZEPointer/ZEHolder.h"

class ZEGRRenderTarget;
class ZEGRDepthStencilBuffer;

class ZED11Texture2D : public ZEGRTexture2D, public ZED11ComponentBase
{
	friend class ZED11Module;
	friend class ZED11Context;

	private:
		ID3D11Texture2D*										Texture2D;
		ID3D11ShaderResourceView*								ShaderResourceView;
		ID3D11UnorderedAccessView*								UnorderedAccessView;
		mutable ZEHolder<ZEGRRenderTarget>						RenderTargets[15];			//maximum mip count is 15 in directx 11
		mutable ZEHolder<ZEGRDepthStencilBuffer>				DepthStencilBuffers[4][2];

		virtual bool											Initialize(ZEUInt Width, ZEUInt Height, ZEUInt LevelCount, ZEGRFormat Format, ZEGRResourceUsage Usage = ZEGR_RU_GPU_READ_ONLY, ZEGRResourceBindFlag BindFlag = ZEGR_RBF_SHADER_RESOURCE, ZEUInt ArrayCount = 1, ZEUInt SampleCount = 1);
		virtual void											Deinitialize();

		ID3D11Resource*											GetResource() const;
		ID3D11Texture2D*										GetTexture() const;
		ID3D11ShaderResourceView*								GetShaderResourceView() const;
		ID3D11UnorderedAccessView*								GetUnorderedAccessView() const;

																ZED11Texture2D();
		virtual													~ZED11Texture2D();

	public:
		virtual ZEHolder<const ZEGRRenderTarget>				GetRenderTarget(ZEUInt Level) const;
		virtual ZEHolder<const ZEGRDepthStencilBuffer>			GetDepthStencilBuffer(bool ReadOnly = false, ZEUInt ArrayIndex = 0) const;

		virtual void											GenerateMipMaps();
		virtual bool											UpdateSubResource(ZEUInt DestArrayIndex, ZEUInt DestLevel, const void* SrcData, ZESize SrcRowPitch);
		virtual bool											Lock(void** Buffer, ZESize* RowPitch);
		virtual void											Unlock();
};
