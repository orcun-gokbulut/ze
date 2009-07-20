//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Direct3D9Module.h
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
#ifndef	__ZE_DIRECT3D9MODULE_H__
#define __ZE_DIRECT3D9MODULE_H__

#include "Graphics/GraphicsModule.h"
#include "D3D9Renderer.h"
#include "D3D9Texture.h"
#include "D3D9Shader.h"
#include "D3D9VertexBuffer.h"
#include "D3D9PostProcessor.h"

#ifdef ZEDEBUG_ENABLED
#define D3D_DEBUG_INFO
#endif
#define INIT_GUID
#include <d3d9.h>

class ZEDirect3D9Module : public ZEGraphicsModule
{
	public:
		bool									IsDeviceLost;
		bool									Enabled;
		bool									UseOnlyShaderModel2;

		D3DCAPS9								D3DCaps;

		LPDIRECT3D9								D3D;
		LPDIRECT3DDEVICE9						Device;
		LPDIRECT3DSURFACE9						FrameColorBuffer;
		LPDIRECT3DSURFACE9						FrameZBuffer;

		ZEChunkArray<ZED3D9RendererBase*, 50>			Renderers;
		ZEChunkArray<ZED3D9Texture*, 50>				Textures;
		ZEChunkArray<ZED3D9VolumeTexture*, 50>			VolumeTextures;
		ZEChunkArray<ZED3D9CubeTexture*, 50>			CubeTextures;
		ZEChunkArray<ZED3D9Shader*, 50>					Shaders;
		ZEChunkArray<ZED3D9StaticVertexBuffer*, 50>		VertexBuffers;
		ZEChunkArray<ZED3D9PostProcessor*, 50>			PostProcessors;


		int										ShaderModel;

		ZEModuleDescription*					GetModuleDescription();

		virtual bool							IsEnabled();
		virtual void							SetEnabled(bool Enabled);

		virtual bool							Initialize();
		virtual void							Deinitialize();

		void									DeviceLost();
		void									DeviceRestored();
		void									RestoreDevice(bool ForceReset = false);

		virtual void							ClearFrameBuffer();
		virtual void							UpdateScreen();

		virtual void							SetScreenSize(int Width, int Height);
		virtual void							SetVerticalSync(bool Enabled);
		virtual void							SetShaderQuality(int Quality);
		virtual void							SetTextureQuality(int Quality);
		virtual void							SetModelQuality(int Quality);
		virtual void							SetShadowQuality(int Quality);
		virtual void							SetPostEffectQuality(int Quality);
		virtual void							SetHDRQuality(int Quality);
		virtual void							SetAntiAliasing(int Level);
		virtual void							SetAnisotropicFilter(int Level);

		virtual ZERenderer*						CreateFrameRenderer();
		virtual ZERenderer*						CreateShadowRenderer();
		virtual ZERenderer*						CreateTextureRenderer();
		virtual ZEPostProcessor*				CreatePostProcessor();

		virtual ZEStaticVertexBuffer*			CreateStaticVertexBuffer();
		virtual ZETexture*						CreateTexture();
		virtual ZEVolumeTexture*				CreateVolumeTexture();
		virtual ZECubeTexture*					CreateCubeTexture();
		virtual ZEShader*						CreateShader(unsigned int ShaderComponents);

		static LPDIRECT3DDEVICE9				GetD3D9Device();
		static ZEDirect3D9Module*				GetD3D9Module();

												ZEDirect3D9Module();
											 	~ZEDirect3D9Module();
};

#endif

