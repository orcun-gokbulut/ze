//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRGraphicsModule.h
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

#include "ZECore/ZEModule.h"

#include "ZETypes.h"
#include "ZECore/ZEOptionSection.h"
#include "ZETexture/ZETextureOptions.h"
#include "ZEGRAdapter.h"
#include "ZEGRRenderState.h"
#include "ZEGRCounter.h"

class ZEGRContext;
class ZEGRShader;
class ZEGRSampler;
class ZEGRTracer;
class ZEGROutput;
class ZEGRBuffer;
class ZEGRTexture;
class ZEGRRenderState;
class ZEGRVertexLayout;
class ZEGRShaderCompiler;

class ZEGRGraphicsModule : public ZEModule
{
	ZE_OBJECT
	friend class ZEGRContext;
	protected:
		ZEGRCounter								Counter;

		virtual bool							InitializeInternal();
		virtual bool							DeinitializeInternal();

												ZEGRGraphicsModule();
		virtual									~ZEGRGraphicsModule();

	public:
		// NOTE: These methods should be here ?
		// -------------------------------------------
		ZETextureQuality						TextureQuality;
		virtual ZETextureOptions*				GetTextureOptions();
		// -------------------------------------------

		ZEGRCounter&							GetCounter();
		virtual const ZEArray<ZEGRAdapter*>&	GetAdapters() = 0;
		virtual ZEGRTracer*						GetTracer() = 0;
		virtual ZEGRAdapter*					GetCurrentAdapter() = 0;
		virtual ZEGRContext*					GetMainContext() = 0;

		virtual ZEGRSampler*					CreateSamplerState() = 0;
		virtual ZEGROutput*						CreateOutput() = 0;
		virtual ZEGRContext*					CreateContext() = 0;
		virtual ZEGRShader*						CreateShader() = 0;
		virtual ZEGRBuffer*						CreateBuffer() = 0;
		virtual ZEGRTexture*					CreateTexture() = 0;
		virtual ZEGRShaderCompiler*				CreateShaderCompiler() = 0;
		virtual ZEGRRenderStateData*			CreateRenderStateData() = 0;
		virtual ZEGRComputeRenderStateData*		CreateComputeRenderStateData() = 0;

		static ZEUInt							SAMPLE_COUNT;

		static ZEGRGraphicsModule*				GetInstance();
};
