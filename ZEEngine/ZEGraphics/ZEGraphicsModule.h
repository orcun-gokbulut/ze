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

#include "ZECore/ZEModule.h"
#include "ZECore/ZEOptionSection.h"

#define zeGraphics ZEGraphicsModule::GetInstance()

class ZETexture2D;
class ZETextureCube;
class ZETexture3D;
class ZEMaterial;
class ZEFixedMaterial;
class ZETerrainMaterial;
class ZEStaticVertexBuffer;
class ZEVertexDeclaration;
class ZERenderCommand;
class ZEFrameRenderer;
class ZEShadowRenderer;
class ZEViewPort;
class ZEPostProcessor;
class ZEUIMaterial;
class ZESimpleMaterial;
class ZESkyBoxMaterial;
enum ZETextureQuality;
struct ZETextureOptions;


class ZEGraphicsModule : public ZEModule
{	
	ZE_MODULE(ZEGraphicsModule)
	protected:
		size_t								CurrentFrameId;
		int									ScreenWidth, ScreenHeight;
		bool								Fullscreen;
		float								NearZ, FarZ;
		bool								VerticalSync;
		int									ShaderQuality;
		ZETextureQuality					TextureQuality;
		int									ModelQuality;
		int									ShadowQuality;
		int									PostEffectQuality;
		int									HDRQuality;
		int									AntiAliasing;
		bool								ToneMappingEnabled;
		int									AnisotropicFilter;


	public:
		static ZEOptionSection				GraphicsOptions;
		static void							BaseInitialize();
		static void							BaseDeinitialize();

		// Options
		virtual void						SetScreenSize(int Width, int Height);
		void								GetScreenSize(int& Width, int& Height);
		int									GetScreenWidth();
		int									GetScreenHeight();

		float								GetAspectRatio();
		void								SetNearZ(float NearZ);
		float								GetNearZ();
		void								SetFarZ(float FarZ);
		float								GetFarZ();
		virtual void						SetVerticalSync(bool Enabled);
		bool								GetVerticalSync();
		virtual void						SetShaderQuality(int Quality);
		int									GetShaderQuality();
		virtual void						SetTextureQuality(ZETextureQuality Quality);
		ZETextureQuality					GetTextureQuality();
		virtual void						SetModelQuality(int Quality);
		int									GetModelQuality();
		virtual void						SetShadowQuality(int Quality);
		int									GetShadowQuality();
		virtual void						SetPostEffectQuality(int Quality);
		int									GetPostEffectQuality();
		virtual void						SetHDRQuality(int Quality);
		int									GetHDRQuality();
		virtual void						SetAntiAliasing(int Level);
		int									GetAntiAliasing();
		virtual void						SetAnisotropicFilter(int Level);
		int									GetAnisotropicFilter();

		virtual ZETextureOptions*			GetTextureOptions();

		size_t								GetCurrentFrameId();

		virtual void						SetMaterialComponentMask(unsigned int Mask) = 0;
		virtual unsigned int				GetMaterialComponentMask() = 0;

		virtual void						UpdateScreen() = 0;
		virtual void						ClearFrameBuffer() = 0;

		virtual ZEViewPort*					GetFrameBufferViewPort() = 0;

		virtual ZEPostProcessor*			CreatePostProcessor() = 0;

		virtual ZEFrameRenderer*			CreateFrameRenderer() = 0;
		virtual ZEShadowRenderer*			CreateShadowRenderer() = 0;

		virtual ZEStaticVertexBuffer*		CreateStaticVertexBuffer() = 0;
		virtual ZEVertexDeclaration*		CreateVertexDeclaration() = 0;

		virtual ZETexture2D*				CreateTexture2D() = 0;
		virtual ZETexture3D*				CreateTexture3D() = 0;
		virtual ZETextureCube*				CreateTextureCube() = 0;

		virtual ZEUIMaterial*				CreateUIMaterial() = 0;
		virtual ZEFixedMaterial*			CreateFixedMaterial() = 0;
		virtual ZETerrainMaterial*			CreateTerrainMaterial() = 0;
		virtual ZESimpleMaterial*			CreateSimpleMaterial() = 0;
		virtual ZESkyBoxMaterial*			CreateSkyBoxMaterial() = 0;
		virtual ZEFixedMaterial*			CreateCustomMaterial() = 0;
		virtual ZEFixedMaterial*			CreateCGFXMaterial() = 0;

		static ZEGraphicsModule*			GetInstance();
};
#endif



