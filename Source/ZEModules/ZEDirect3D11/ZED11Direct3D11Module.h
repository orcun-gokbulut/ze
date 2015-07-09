//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11Direct3D11Module.h
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

#include "ZEGraphics/ZEGRGraphicsModule.h"

#include "ZETypes.h"
#include "ZEDS/ZEArray.h"
#include "ZED11StatePool.h"
#include "ZED11Tracer.h"
#include "ZED11Context.h"

class ZEGRTexture2D;
class ZEGRTexture3D;
class ZEGRTextureCube;
class ZEGRIndexBuffer;
class ZEGRVertexBuffer;
class ZEGRShaderCompiler;
class ZEGRContext;
class ZEGRDepthStencilBuffer;

#define ZE_MAX_MULTI_SAMPLE_COUNT		D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT
#define ZE_MAX_ANISOTROPY_LEVEL			D3D11_MAX_MAXANISOTROPY

class ZED11Direct3D11Module : public ZEGRGraphicsModule
{
	ZE_MODULE(ZED11Direct3D11Module)
	protected:
		ID3D11Device*							Device;
		ZED11Context							MainContext;
		ZEArray<ZEGRAdapter*>					Adapters;
		ZED11StatePool							StatePool;
		ZED11Tracer								Tracer;

		bool									CreateDevice(ZEGRAdapter* Adapter);

		virtual bool							InitializeSelf();
		virtual bool							DeinitializeSelf();

												ZED11Direct3D11Module();
		virtual									~ZED11Direct3D11Module();

	public:
		ZED11StatePool*							GetStatePool();
		virtual ZEGRTracer*						GetTracer();
		virtual const ZEArray<ZEGRAdapter*>&	GetAdapters();
		virtual ZEGRContext*					GetMainContext();

		virtual ZEGRContext*					CreateContext();
		virtual ZEGRVertexBuffer*				CreateVertexBuffer();
		virtual ZEGRIndexBuffer*				CreateIndexBuffer();
		virtual ZEGRShader*						CreateShader();
		virtual ZEGRConstantBuffer*				CreateConstantBuffer();
		virtual ZEGRTexture2D*					CreateTexture2D();
		virtual ZEGRTexture3D*					CreateTexture3D();
		virtual ZEGRTextureCube*				CreateTextureCube();
		virtual ZEGRDepthStencilBuffer*			CreateDepthStencilBuffer();
		virtual ZEGRShaderCompiler*				CreateShaderCompiler();
		virtual ZEGRWindow*						CreateGraphicsWindow();
		virtual ZEGRRenderStateData*			CreateRenderStateData(const ZEGRRenderState& RenderState);
};
