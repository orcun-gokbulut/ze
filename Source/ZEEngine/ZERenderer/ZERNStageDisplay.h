//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageDisplay.h
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

#include "ZEMeta/ZEObject.h"
#include "ZEInitializable.h"

#include "ZEMath/ZEMatrix.h"
#include "ZEPointer/ZEHolder.h"
#include "ZEGraphics/ZEGRSampler.h"
#include "ZEGraphics/ZEGRViewport.h"

ZE_ENUM(ZERNStageDisplayScaleMode)
{
	ZERN_SDSM_NONE,
	ZERN_SDSM_FILL,
	ZERN_SDSM_FIT_AUTO,
	ZERN_SDSM_FIT_HORIZONTAL,
	ZERN_SDSM_FIT_VERTICAL
};

ZE_ENUM(ZERNStageDisplayFlipMode)
{
	ZERN_SDMF_NONE,
	ZERN_SDMF_VERTICAL,
	ZERN_SDMF_HORIZONTAL,
	ZERN_SDMF_BOTH
};

class ZEGRContext;
class ZEGRTexture;
class ZEGRBuffer;
class ZEGRRenderTarget;
class ZERNStageMultiplexer;

class ZERNStageDisplay : public ZEObject, public ZEInitializable
{
	ZE_OBJECT
	friend class ZERNStageMultiplexer;
	private:
		ZEFlags									DirtyFlags;
												
		ZERNStageMultiplexer*					Owner;
		ZEString								InputName;
												
		ZEVector2								InputOffset;
		ZEVector2								InputSize;
												
		ZEVector2								OutputOffset;
		ZEVector2								OutputSize;
		
		ZEGRViewport							Viewport;
												
		ZERNStageDisplayScaleMode				ScaleMode;
		ZERNStageDisplayFlipMode				FlipMode;
		ZEVector2								Zoom;
		float									Rotation;
		ZEGRTextureFilter						Filter;
												
		struct  								
		{										
			ZEVector4							Transform0;
			ZEVector4							Transform1;
			ZEVector4							Transform2;
			ZEVector2							InputSize;
			ZEVector2							Reserved0;
		} Constants;							
												
		ZEHolder<ZEGRBuffer>					ConstantBuffer;
		ZEHolder<ZEGRSampler>					Sampler;

		ZEHolder<const ZEGRTexture>				InputTexture;

		ZELink<ZERNStageDisplay>				Link;

		virtual bool							InitializeInternal();
		virtual bool							DeinitializeInternal();
												
		bool									UpdateConstantBuffer();
		bool									UpdateSampler();
		bool									Update();
												
		bool									Draw(ZEGRContext* Context);
												
												ZERNStageDisplay();
		virtual									~ZERNStageDisplay();

	public:											
		void									SetInput(const ZEString& InputName);
		const ZEString&							GetInput() const;

		void									SetInputOffset(const ZEVector2& Offset);
		const ZEVector2&						GetInputOffset() const;
												
		void									SetInputSize(const ZEVector2& Size);
		const ZEVector2&						GetInputSize() const;
												
		void									SetOutputOffset(const ZEVector2& Offset);
		const ZEVector2&						GetOutputOffset() const;
												
		void									SetOutputSize(const ZEVector2& Size);
		const ZEVector2&						GetOutputSize() const;
												
		void									SetScaleMode(ZERNStageDisplayScaleMode Mode);
		ZERNStageDisplayScaleMode				GetScaleMode() const;
												
		void									SetZoom(const ZEVector2& Zoom);
		const ZEVector2&						GetZoom() const;
												
		void									SetRotation(float Rotation);
		float									GetRotation()const;
												
		void									SetFlipMode(ZERNStageDisplayFlipMode Mode);
		ZERNStageDisplayFlipMode				GetFlipMode() const;
												
		void									SetFilter(ZEGRTextureFilter Filter);
		ZEGRTextureFilter						GetFilter() const;

		void									SetViewport(const ZEGRViewport& Viewport);
		const ZEGRViewport&						GetViewport() const;

		static ZERNStageDisplay*				CreateInstance();
};
