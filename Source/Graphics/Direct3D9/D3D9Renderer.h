//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - D3D9Renderer.h
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
#ifndef __ZE_D3D9RENDERER_H__
#define __ZE_D3D9RENDERER_H__

#include "Graphics/Renderer.h"
#include "Graphics/Texture.h"
#include "D3D9Shader.h"
#include "D3D9ComponentBase.h"
#include <d3d9.h>

class ZED3D9RendererBase : public ZERenderer, public ZED3D9ComponentBase
{
	friend class ZEDirect3D9Module;
	protected:
		ZESmartArray<ZERenderOrder>				NonTransparent;
		ZESmartArray<ZERenderOrder>				Transparent;
		ZESmartArray<ZERenderOrder>				Imposter;
		ZECamera*								Camera;
		LPDIRECT3DSURFACE9						RenderColorBufferRT;
		LPDIRECT3DSURFACE9						RenderZBufferRT;

												ZED3D9RendererBase();
		virtual									~ZED3D9RendererBase();

	protected:						
		void									PumpStreams(ZERenderOrder* RenderOrder);
		void									SetupRenderOrder(ZERenderOrder* RenderOrder);

	public:
		virtual bool							Initialize();
		virtual void							Deinitialize();
		virtual void							Destroy();

		virtual void							DeviceLost() = 0;
		virtual bool							DeviceRestored() = 0;

		virtual void							SetCamera(ZECamera* Camera);
		virtual void							AddToRenderOrder(ZERenderOrder* RenderOrder);
		virtual void							ClearList();

		virtual void							Render(float ElaspedTime);

};

class ZED3D9FrameBufferRenderer : public ZED3D9RendererBase
{
	friend class ZEDirect3D9Module;
	public:
		virtual bool							SetOutput(ZETextureCube* TextureResource, ZETextureCubeFace Face);
		virtual bool							SetOutput(ZETexture2D* TextureResource);

		virtual bool							Initialize();

		virtual void							DeviceLost();
		virtual bool							DeviceRestored();
};

class ZED3D9TextureRenderer : public ZED3D9RendererBase
{
	friend class ZEDirect3D9Module;
	private:
		ZETexture2D*								OutputTexture;
		ZETextureCube*							OutputCubeTexture;
		ZETextureCubeFace						OutputCubeTextureFace;

	protected:
												ZED3D9TextureRenderer();

	public:	
		virtual bool							SetOutput(ZETextureCube* Texture, ZETextureCubeFace Face);
		virtual bool							SetOutput(ZETexture2D* Texture);

		virtual void							DeviceLost();
		virtual bool							DeviceRestored();
};

class ZED3D9ShadowRenderer : public ZED3D9RendererBase
{
	friend class ZEDirect3D9Module;
	protected:
		void									DrawSM2(ZERenderOrder* RenderOrder);
		ZETexture2D*								OutputTexture;
		ZETextureCube*							OutputCubeTexture;
		ZETextureCubeFace						OutputCubeTextureFace;

												ZED3D9ShadowRenderer();

	public:	
		static bool								BaseInitialize();
		static void								BaseDeinitialize();

		virtual bool							SetOutput(ZETextureCube* Texture, ZETextureCubeFace Face);
		virtual bool							SetOutput(ZETexture2D* Texture);

		virtual void							DeviceLost();
		virtual bool							DeviceRestored();

		virtual void							Render(float ElaspedTime);
};

#endif
