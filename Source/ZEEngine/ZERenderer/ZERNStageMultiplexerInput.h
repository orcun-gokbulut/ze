//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageMultiplexerInput.h
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

#include "ZEDS/ZEArray.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEPointer/ZEHolder.h"
#include "ZERNStage.h"
#include "ZEGraphics/ZEGRSampler.h"
#include "ZEGraphics/ZEGRViewport.h"

ZE_ENUM(ZERNStageMultiplexerInputScaleMode)
{
	ZERN_SMIM_NONE,
	ZERN_SMIM_FILL,
	ZERN_SMIM_FIT_AUTO,
	ZERN_SMIM_FIT_HORIZONTAL,
	ZERN_SMIM_FIT_VERTICAL
};

ZE_ENUM(ZERNStageMultiplexerInputFlipMode)
{
	ZERN_SMF_NONE,
	ZERN_SMF_VERTICAL,
	ZERN_SMF_HORIZONTAL,
	ZERN_SMF_BOTH,
};

class ZEGRTexture2D;
class ZEGRConstantBuffer;
class ZEGRViewport;
class ZEGRRenderTarget;

class ZERNStageMultiplexerInput : public ZEObject, public ZEInitializable
{
	ZE_OBJECT
	friend class ZERNStageMultiplexer;
	private:
		ZEFlags								DirtyFlags;

		ZERNStageMultiplexer*				Owner;
		ZERNStageBuffer						Input;
		ZEString							GlobalInput;

		ZEVector2							InputOffset;
		ZEVector2							InputSize;

		ZEHolder<const ZEGRRenderTarget>	OutputRenderTarget;
		ZEVector2							OutputOffset;
		ZEVector2							OutputSize;

		ZEGRViewport						Viewport;

		ZERNStageMultiplexerInputScaleMode	ScaleMode;
		ZERNStageMultiplexerInputFlipMode	FlipMode;
		ZEVector2							Zoom;
		float								Rotation;
		ZEGRTextureFilter					Filter;

		struct  
		{
			ZEVector4						Transform0;
			ZEVector4						Transform1;
			ZEVector4						Transform2;
			ZEVector2						InputSize;
			ZEVector2						Reserved0;
		} Constants;
		
		ZEHolder<ZEGRConstantBuffer>		ConstantBuffer;
		ZEHolder<ZEGRSampler>				Sampler;
		ZEHolder<const ZEGRTexture2D>		InputTexture;

		ZELink<ZERNStageMultiplexerInput>	StageLink;

		virtual void						DeinitializeSelf();

		bool								UpdateInputOutput();
		bool								UpdateConstantBuffer();
		bool								UpdateSampler();

	public:
		void								SetInput(ZERNStageBuffer Input);
		ZERNStageBuffer						GetInput() const;

		void								SetInputGlobal(const ZEString& Name);
		const ZEString&						GetInputGlobal();

		void								SetTexture(const ZEGRTexture2D* Texture);
		const ZEGRTexture2D*				GetTexture() const;

		void								SetInputOffset(const ZEVector2& Offset);
		const ZEVector2&					GetInputOffset() const;

		void								SetInputSize(const ZEVector2& Size);
		const ZEVector2&					GetInputSize() const;

		void								SetOutputOffset(const ZEVector2& Offset);
		const ZEVector2&					GetOutputOffset() const;

		void								SetOutputSize(const ZEVector2& Size);
		const ZEVector2&					GetOutputSize() const;

		void								SetScaleMode(ZERNStageMultiplexerInputScaleMode Mode);
		ZERNStageMultiplexerInputScaleMode	GetScaleMode() const;

		void								SetZoom(const ZEVector2& Zoom);
		const ZEVector2&					GetZoom() const;

		void								SetRotate(float Rotation);
		float								GetRotation()const;

		void								SetFlipMode(ZERNStageMultiplexerInputFlipMode Mode);
		ZERNStageMultiplexerInputFlipMode	GetFlipMode() const;

		void								SetFilter(ZEGRTextureFilter Filter);
		ZEGRTextureFilter					GetFilter() const;

		bool								Draw(ZEGRContext* Context);
		bool								Draw(ZEGRContext* Context, const ZEGRViewport& Viewport);

											ZERNStageMultiplexerInput();
};
