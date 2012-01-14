//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9Module.h
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
#ifndef	__ZE_D3D9_MODULE_H__
#define __ZE_D3D9_MODULE_H__

#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZED3D9ViewPort.h"
#include "ZEDS/ZEArray.h"

#ifdef ZE_DEBUG_ENABLE
#define D3D_DEBUG_INFO
#endif
#define INIT_GUID
#include <d3d9.h>

enum ZEDirect3D9ShaderVersion
{
	ZE_D3D9SV_NO_SHADER_SUPPORT	= 0,
	ZE_D3D9SV_SHADER_MODEL_2	= 1,
	ZE_D3D9SV_SHADER_MODEL_3	= 2
};

class ZED3D9Texture2D;
class ZED3D9Texture3D;
class ZED3D9TextureCube;
class ZED3D9VertexDeclaration;
class ZED3D9StaticVertexBuffer;
class ZED3D9FrameRenderer;
class ZED3D9ShadowRenderer;
class ZED3D9PostProcessor;
class ZED3D9FixedMaterialShader;
class ZED3D9ShaderManager;

enum ZED3D9PixelShaderVersion
{
	ZE_D3D9_PSV_3_0_0	= 4,
	ZE_D3D9_PSV_2_0_A	= 3,
	ZE_D3D9_PSV_2_0_B	= 2,
	ZE_D3D9_PSV_2_0_0	= 1,
	ZE_D3D9_PSV_NONE	= 0
};

enum ZED3D9VertexShaderVersion
{
	ZE_D3D9_VSV_3_0_0	= 3,
	ZE_D3D9_VSV_2_0_A	= 2,
	ZE_D3D9_VSV_2_0_0	= 1,
	ZE_D3D9_VSV_NONE	= 0
};

enum ZED3D9PipelineType
{
	ZE_D3D9_PT_FIXED_FUNCTION,
	ZE_D3D9_PT_PROGRAMABLE
};

class ZED3D9Module : public ZEGraphicsModule
{
	ZE_MODULE(ZED3D9Module)
	friend class										ZED3D9ModuleDescription;

	private:
		bool											DeviceLostState;
		bool											Enabled;

		ZED3D9PixelShaderVersion						PixelShaderVersion;
		ZED3D9VertexShaderVersion						VertexShaderVersion;
		ZED3D9PipelineType								PipelineType;

		D3DCAPS9										DeviceCaps;

		LPDIRECT3D9										D3D;
		LPDIRECT3DDEVICE9								Device;

		ZED3D9ViewPort									FrameBufferViewPort;

		ZED3D9ShaderManager*							ShaderManager;

	protected:
														ZED3D9Module();
											 			~ZED3D9Module();

	public:
		ZEChunkArray<ZED3D9FrameRenderer*, 50>			Renderers;
		ZEChunkArray<ZED3D9ShadowRenderer*, 50>			ShadowRenderers;
		ZEChunkArray<ZED3D9Texture2D*, 50>				Texture2Ds;
		ZEChunkArray<ZED3D9Texture3D*, 50>				Texture3Ds;
		ZEChunkArray<ZED3D9TextureCube*, 50>			TextureCubes;
		ZEChunkArray<ZED3D9FixedMaterialShader*, 50>	Shaders;
		ZEChunkArray<ZED3D9StaticVertexBuffer*, 50>		VertexBuffers;
		ZEChunkArray<ZED3D9PostProcessor*, 50>			PostProcessors;
		ZEChunkArray<ZED3D9VertexDeclaration*, 50>		VertexDeclaration;

		LPDIRECT3D9										GetDirect3D();
		LPDIRECT3DDEVICE9								GetDevice();
		D3DCAPS9*										GetDeviceCaps();

		ZED3D9PixelShaderVersion						GetPixelShaderVersion();
		ZED3D9VertexShaderVersion						GetVertexShaderVersion();
		ZED3D9PipelineType								GetPipelineType();

		virtual bool									GetEnabled();
		virtual void									SetEnabled(bool Enabled);

		virtual bool									Initialize();
		virtual void									Deinitialize();

		bool											IsDeviceLost();
		void											DeviceLost();
		void											DeviceRestored();
		void											RestoreDevice(bool ForceReset = false);

		virtual void									ClearFrameBuffer();
		virtual void									UpdateScreen();
	
		virtual void									SetScreenSize(int Width, int Height);
		virtual void									SetVerticalSync(bool Enabled);
		virtual void									SetShaderQuality(int Quality);
		virtual void									SetTextureQuality(int Quality);
		virtual void									SetModelQuality(int Quality);
		virtual void									SetShadowQuality(int Quality);
		virtual void									SetPostEffectQuality(int Quality);
		virtual void									SetHDRQuality(int Quality);
		virtual void									SetAntiAliasing(int Level);
		virtual void									SetAnisotropicFilter(int Level);

		virtual void									SetMaterialComponentMask(unsigned int Mask);
		virtual unsigned int							GetMaterialComponentMask();
		
		virtual ZEViewPort*								GetFrameBufferViewPort();

		virtual ZED3D9ShaderManager*					GetShaderManager();

		virtual ZEFrameRenderer*						CreateFrameRenderer();
		virtual ZEShadowRenderer*						CreateShadowRenderer();

		virtual ZEPostProcessor*						CreatePostProcessor();

		virtual ZEVertexDeclaration*					CreateVertexDeclaration();
		virtual ZEStaticVertexBuffer*					CreateStaticVertexBuffer();

		virtual ZETexture2D*							CreateTexture2D();
		virtual ZETexture3D*							CreateTexture3D();
		virtual ZETextureCube*							CreateTextureCube();

		virtual ZEFixedMaterial*						CreateFixedMaterial();
		virtual ZEUIMaterial*							CreateUIMaterial();
		virtual ZESimpleMaterial*						CreateSimpleMaterial();
		virtual ZETerrainMaterial*						CreateTerrainMaterial();
		virtual ZESkyBoxMaterial*						CreateSkyBoxMaterial();
		virtual ZEFixedMaterial*						CreateCustomMaterial();
		virtual ZEFixedMaterial*						CreateCGFXMaterial();

		static LPDIRECT3DDEVICE9						GetD3D9Device();
		static ZED3D9Module*							GetD3D9Module();
};

#endif





