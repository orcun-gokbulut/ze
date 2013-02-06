//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9GraphicsModule.h
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
#ifndef	__ZE_D3D9_GRAPHICS_MODULE_H__
#define __ZE_D3D9_GRAPHICS_MODULE_H__

#ifdef ZE_DEBUG_ENABLE
	#define D3D_DEBUG_INFO
#endif
#define INIT_GUID

#include <d3d9.h>


#include "ZETypes.h"
#include "ZEDS/ZEArray.h"
#include "ZED3D9RenderTarget.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZED3D9DepthBuffer.h"

class ZED3D9Texture2D;
class ZED3D9Texture3D;
class ZED3D9TextureCube;
class ZEShaderCompiler;
class ZED3D9VertexDeclaration;
class ZED3D9StaticVertexBuffer;
class ZED3D9FrameRenderer;
class ZED3D9ShadowRenderer;
class ZED3D9PostProcessor;
class ZED3D9FixedMaterialShader;
class ZED3D9ShaderManager;
class ZED3D9StaticIndexBuffer;
class ZED3D9DynamicIndexBuffer;


class ZED3D9GraphicsModule : public ZEGraphicsModule
{
	ZE_MODULE(ZED3D9GraphicsModule)
	
	friend class									ZED3D9GraphicsModuleDescription;

	private:
		bool										Enabled;
		bool										IsDeviceLost;
		
		D3DCAPS9									Caps;
		LPDIRECT3D9									D3D;
		LPDIRECT3DDEVICE9							Device;
		D3DPRESENT_PARAMETERS						D3DPP;

		ZED3D9RenderTarget							FrameBuffer;
		ZED3D9DepthBuffer							DepthStencilBuffer;

		ZEChunkArray<ZED3D9Texture2D*, 50>			RenderTarget2Ds;
		ZEChunkArray<ZED3D9TextureCube*, 50>		RenderTargetCubes;

	protected:
													ZED3D9GraphicsModule();
											 		~ZED3D9GraphicsModule();

	public:
		virtual bool								GetEnabled();
		virtual void								SetEnabled(bool Enabled);

		virtual bool								GetDeviceLost();
		virtual bool								RestoreDevice(bool ForceReset = false);

		virtual bool								Initialize();
		virtual void								Deinitialize();
		
		virtual ZEGraphicsDevice*					GetDevice();
		virtual ZEGraphicsEventTracer*				GetEventTracer();
		virtual ZEShaderCompiler*					GetShaderCompiler();

		virtual ZERenderTarget*						GetFrameBuffer();
		virtual ZEDepthBuffer*						GetDepthStencilBuffer();

		virtual ZED3D9ShaderManager*				GetShaderManager();

		virtual void								RegisterRenderTarget(ZED3D9Texture2D* RenderTarget);
		virtual void								RegisterRenderTarget(ZED3D9TextureCube* RenderTarget);
		virtual void								UnregisterRenderTarget(ZED3D9Texture2D* RenderTarget);
		virtual void								UnregisterRenderTarget(ZED3D9TextureCube* RenderTarget);

		virtual void								SetVerticalSync(bool Enabled);
		virtual void								SetAntiAliasing(ZEUInt Level);
		virtual void								SetAnisotropicFilter(ZEUInt Level);
		virtual void								SetScreenSize(ZEUInt Width, ZEUInt Height);

		virtual ZEShader*							CreatePixelShader();
		virtual ZEShader*							CreateVertexShader();
		virtual ZERenderTarget*						CreateRenderTarget();
		virtual ZETexture2D*						CreateTexture2D();
		virtual ZETexture3D*						CreateTexture3D();
		virtual ZETextureCube*						CreateTextureCube();
		virtual ZEVertexDeclaration*				CreateVertexDeclaration();
		virtual ZEStaticVertexBuffer*				CreateStaticVertexBuffer();
		virtual ZEStaticIndexBuffer*				CreateStaticIndexBuffer();

		LPDIRECT3D9									GetDirect3D();
		LPDIRECT3DDEVICE9							GetDirect3DDevice();
		
};

#endif





