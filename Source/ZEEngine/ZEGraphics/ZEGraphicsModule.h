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

#pragma once
#ifndef	__ZE_GRAPHICS_MODULE_H__
#define __ZE_GRAPHICS_MODULE_H__

#include "ZETypes.h"
#include "ZECore/ZEModule.h"
#include "ZECore/ZEOptionSection.h"

#define zeGraphics ZEGraphicsModule::GetInstance()

class ZETexture2D;
class ZETextureCube;
class ZETexture3D;
class ZEMaterial;
class ZEFixedMaterial;
class ZETerrainMaterial;
class ZEStaticIndexBuffer;
class ZEDynamicIndexBuffer;
class ZEStaticVertexBuffer;
class ZEVertexDeclaration;
class ZERenderCommand;
class ZEFrameRenderer;
class ZEShadowRenderer;
class ZEViewPort;
class ZECloudMaterial;
class ZEPostProcessor;
class ZEUIMaterial;
class ZESimpleMaterial;
class ZESkyBoxMaterial;
class ZEMoonMaterial;
class ZESkyDomeMaterial;
enum ZETextureQuality;
struct ZETextureOptions;





class ZEGraphicsModule : public ZEModule
{	
	ZE_MODULE(ZEGraphicsModule)
	protected:
		ZESize								CurrentFrameId;
		ZEInt								ScreenWidth, ScreenHeight;
		bool								Fullscreen;
		float								NearZ, FarZ;
		bool								VerticalSync;
		ZEInt								ShaderQuality;
		ZETextureQuality					TextureQuality;
		ZEInt								ModelQuality;
		ZEInt								ShadowQuality;
		ZEInt								PostEffectQuality;
		ZEInt								HDRQuality;
		ZEInt								AntiAliasing;
		bool								ToneMappingEnabled;
		ZEUInt								AnisotropicFilter;


	public:
		static ZEOptionSection				GraphicsOptions;
		static void							BaseInitialize();
		static void							BaseDeinitialize();

		// Options
		virtual void						SetScreenSize(ZEInt Width, ZEInt Height);
		void								GetScreenSize(ZEInt& Width, ZEInt& Height);
		ZEInt								GetScreenWidth();
		ZEInt								GetScreenHeight();

		float								GetAspectRatio();
		void								SetNearZ(float NearZ);
		float								GetNearZ();
		void								SetFarZ(float FarZ);
		float								GetFarZ();
		virtual void						SetVerticalSync(bool Enabled);
		bool								GetVerticalSync();
		virtual void						SetShaderQuality(ZEInt Quality);
		ZEInt								GetShaderQuality();
		virtual void						SetTextureQuality(ZETextureQuality Quality);
		ZETextureQuality					GetTextureQuality();
		virtual void						SetModelQuality(ZEInt Quality);
		ZEInt								GetModelQuality();
		virtual void						SetShadowQuality(ZEInt Quality);
		ZEInt								GetShadowQuality();
		virtual void						SetPostEffectQuality(ZEInt Quality);
		ZEInt								GetPostEffectQuality();
		virtual void						SetHDRQuality(ZEInt Quality);
		ZEInt								GetHDRQuality();
		virtual void						SetAntiAliasing(ZEInt Level);
		ZEInt								GetAntiAliasing();
		virtual void						SetAnisotropicFilter(ZEUInt Level);
		ZEUInt								GetAnisotropicFilter();

		virtual ZETextureOptions*			GetTextureOptions();

		ZESize								GetCurrentFrameId();

		virtual void						SetMaterialComponentMask(ZEUInt Mask) = 0;
		virtual ZEUInt						GetMaterialComponentMask() = 0;

		virtual void						UpdateScreen() = 0;
		virtual void						ClearFrameBuffer() = 0;

		virtual ZEViewPort*					GetFrameBufferViewPort() = 0;

		virtual ZEPostProcessor*			CreatePostProcessor() = 0;

		virtual ZEFrameRenderer*			CreateFrameRenderer() = 0;
		virtual ZEShadowRenderer*			CreateShadowRenderer() = 0;

		virtual ZEStaticIndexBuffer*		CreateStaticIndexBuffer() = 0;
		virtual ZEStaticVertexBuffer*		CreateStaticVertexBuffer() = 0;
		virtual ZEVertexDeclaration*		CreateVertexDeclaration() = 0;

		virtual ZETexture2D*				CreateTexture2D() = 0;
		virtual ZETexture3D*				CreateTexture3D() = 0;
		virtual ZETextureCube*				CreateTextureCube() = 0;

		virtual ZEUIMaterial*				CreateUIMaterial() = 0;
		virtual ZECloudMaterial*			CreateCloudMaterial() = 0;
		virtual ZEFixedMaterial*			CreateFixedMaterial() = 0;
		virtual ZEMoonMaterial*				CreateMoonMaterial() = 0;
		virtual ZETerrainMaterial*			CreateTerrainMaterial() = 0;
		virtual ZESimpleMaterial*			CreateSimpleMaterial() = 0;
		virtual ZESkyBoxMaterial*			CreateSkyBoxMaterial() = 0;
		virtual ZESkyDomeMaterial*			CreateSkyDomeMaterial() = 0;
		virtual ZEFixedMaterial*			CreateCustomMaterial() = 0;
		virtual ZEFixedMaterial*			CreateCGFXMaterial() = 0;

		static ZEGraphicsModule*			GetInstance();
};


#endif
