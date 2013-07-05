//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGraphicsModule.h
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

#ifndef	__ZE_GRAPHICS_MODULE_H__
#define __ZE_GRAPHICS_MODULE_H__

#include "ZETypes.h"
#include "ZECore/ZEModule.h"
#include "ZEGraphicsDeviceState.h"
#include "ZECore/ZEOptionSection.h"
#include "ZETexture/ZETextureOptions.h"

#define ZE_DESTROY(x)		\
{							\
	if ((x) != NULL)		\
	{						\
		(x)->Destroy();		\
		(x) = NULL;			\
	}						\
}

#define zeGraphics ZEGraphicsModule::GetInstance()

struct ZEGraphicsStatistics
{
	ZEUInt16	ShaderCount;
	ZESize		ShaderSize;

	ZEUInt16	Texture2DCount;
	ZESize		Texture2DSize;

	ZEUInt16	Texture3DCount;
	ZESize		Texture3DSize;

	ZEUInt16	TextureCubeCount;
	ZESize		TextureCubeSize;

	ZEUInt16	DepthStancilBufferCount;
	ZESize		DepthStancilBufferSize;

	ZEUInt16	IndexBufferCount;
	ZESize		IndexBufferSize;

	ZEUInt16	VertexBufferCount;
	ZESize		VertexBufferSize;

	ZEUInt16	ConstantBufferCount;
	ZESize		ConstantBufferSize;	

	ZEUInt16	RenderTargetCount;

	ZEUInt16	BlendStateCount;
	ZEUInt16	SamplerStateCount;
	ZEUInt16	DepthStencilCount;
	ZEUInt16	RasterizerCount;
	ZEUInt16	VertexLayoutCount;
	ZEUInt16	StatesPerSecond;
};

class ZEShader;
class ZEStatePool;
class ZETexture2D;
class ZETexture3D;
class ZETextureCube;
class ZEIndexBuffer;
class ZEVertexLayout;
class ZEVertexBuffer;
class ZERenderTarget;
class ZERenderTarget;
class ZEShaderCompiler;
class ZEGraphicsDevice;
class ZEConstantBuffer;
class ZEDepthStencilBuffer;
class ZEGraphicsEventTracer;

class ZEGraphicsModule : public ZEModule
{
	ZE_MODULE(ZEGraphicsModule)

	friend class ZEGraphicsDevice;

	protected:
		bool							FullScreen;
		ZEUInt							ScreenWidth;
		ZEUInt							ScreenHeight;
		bool							VerticalSync;
		ZEUInt							SampleCount;
		ZEUInt							SampleQuality;
		ZEUInt							AnisotropicFilter;

		ZEUInt							ScreenCount;
		ZEArray<ZEViewport>				Viewports;
		ZEArray<ZEScissorRectangle>		ScissorRects;
		ZEArray<ZERenderTarget*>		FrameBuffers;
		ZEArray<ZEDepthStencilBuffer*>	DepthBuffers;

										ZEGraphicsModule();
		virtual							~ZEGraphicsModule();

	public:
		// NOTE: These methods should not be here ?
		// -------------------------------------------
		ZETextureQuality				TextureQuality;
		static ZEOptionSection			GraphicsOptions;
		static void						BaseInitialize();
		static void						BaseDeinitialize();
		virtual ZETextureOptions*		GetTextureOptions();
		// -------------------------------------------

		virtual bool					SetFullScreen(bool Enabled) = 0;
		bool							GetFullScreen() const;

		virtual bool					SetSampleCount(ZEUInt Count) = 0;
		ZEUInt							GetSampleCount() const;
		ZEUInt							GetSampleQuality() const;

		virtual void					SetVerticalSync(bool Enabled) = 0;
		bool							GetVerticalSync() const;

		virtual bool					SetScreenCount(ZEUInt Count) = 0;
		ZEUInt							GetScreenCount() const;

		virtual void					SetAnisotropicFilter(ZEUInt Anisotropy) = 0;
		ZEUInt							GetAnisotropicFilter() const;
		
		virtual bool					SetScreenSize(ZEUInt WindowWidth, ZEUInt WindowHeight) = 0;
		ZEVector2						GetScreenSize() const;
		ZEUInt							GetScreenWidth() const;
		ZEUInt							GetScreenHeight() const;

		const ZEViewport&				GetViewport(ZESize Index = 0) const;
		const ZEScissorRectangle&		GetScissorRectangle(ZESize Index = 0) const;
		const ZERenderTarget*			GetFrameBuffer(ZESize Index = 0) const;
		const ZEDepthStencilBuffer*		GetDepthBuffer(ZESize Index = 0) const;

		virtual void					GetStatistics(ZEGraphicsStatistics& Statistics) const = 0;

		virtual ZEGraphicsDevice*		GetDevice() const = 0;
		virtual ZEStatePool*			GetStatePool() const = 0;
		virtual ZEGraphicsEventTracer*	GetEventTracer() const = 0;
		virtual ZEShaderCompiler*		GetShaderCompiler() const = 0;
		
		virtual ZETexture2D*			CreateTexture2D() const = 0;
		virtual ZETexture3D*			CreateTexture3D() const = 0;
		virtual ZETextureCube*			CreateTextureCube() const = 0;
		virtual ZEIndexBuffer*			CreateIndexBuffer() const = 0;
		virtual ZEVertexBuffer*			CreateVertexBuffer() const = 0;
		virtual ZEConstantBuffer*		CreateConstantBuffer() const = 0;
		virtual ZEDepthStencilBuffer*	CreateDepthStencilBuffer() const = 0;

		static ZEGraphicsModule*		GetInstance();
};

#endif
