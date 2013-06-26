//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9ShadowRenderer.h
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
#ifndef __ZE_D3D9_SHADOW_RENDERER_H__
#define __ZE_D3D9_SHADOW_RENDERER_H__

#include "ZEGraphics/ZEShadowRenderer.h"
#include "ZED3D9ComponentBase.h"

#include <d3d9.h>


class ZETexture2D;
class ZETextureCube;
class ZERenderCommand;
class ZEViewPort;
class ZED3D9ViewPort;
struct ZEDrawParameters;
enum ZETextureCubeFace;

enum ZEShadowMapFilterType
{
	ZE_SMFT_PCF_1_TAP			= 0,
	ZE_SMFT_PCF_4_TAP			= 1,
	ZE_SMFT_PCF_POISSON_4_TAP	= 2,
	ZE_SMFT_PCF_POISSON_5_TAP	= 3,
	ZE_SMFT_PCF_POISSON_6_TAP	= 4,
	ZE_SMFT_PCF_POISSON_8_TAP	= 5,
	ZE_SMFT_PCF_POISSON_12_TAP	= 6
};

class ZED3D9ShadowRenderer : public ZEShadowRenderer, public ZED3D9ComponentBase
{
	friend class ZED3D9Module;
	private:
		ZETexture2D*							RandomRotationMap;
		ZEShadowMapFilterType					FilterType;

		ZEDrawParameters*						DrawParameters;

		ZELight*								Light;
		ZED3D9ViewPort*							ViewPort;
		ZEUInt									ShadowResolution;
		ZESmartArray<ZERenderCommand>			CommandList;

		LPDIRECT3DSURFACE9						DepthSurface;
		ZED3D9Texture2D*						NULLRenderTarget;

		ZEArray<ZEPostProcessor*>				PostProcessors;
		bool									Face;

		IDirect3DQuery9*						EventQuery;

		bool									InitializeShaders();
		void									DeinitializeShaders();

		void									RenderPointLight();
		void									RenderProjectiveLight();
		void									RenderDirectionalLight();
		void									RenderOmniProjectiveLight();
		
	protected:
												ZED3D9ShadowRenderer();
		virtual									~ZED3D9ShadowRenderer();

	public:
		void									SetDrawParameters(ZEDrawParameters* Parameters);
		ZEDrawParameters*						GetDrawParameters();

		void									SetShadowResolution(ZEUInt Resolution);
		ZEUInt									GetShadowResolution() const;
		
		const ZETexture2D*						GetRandomRotationMap() const;

		virtual ZEArray<ZEPostProcessor*>&		GetPostProcessors();
		virtual void							AddPostProcessor(ZEPostProcessor* PostProcessor);
		virtual void							RemovePostProcessor(ZEPostProcessor* PostProcessor);

		virtual bool							Initialize();
		virtual void							Deinitialize();
		virtual void							Destroy();

		virtual void							DeviceLost();
		virtual bool							DeviceRestored();

		virtual void							SetLight(ZELight* Light);
		virtual ZELight*						GetLight();

		virtual	void							SetFace(bool Front);
		virtual bool							GetFace();

		virtual void							SetViewPort(ZEViewPort* ViewPort);
		virtual ZEViewPort*						GetViewPort();

		virtual const ZESmartArray<ZERenderCommand>& GetRenderList() const;
		virtual void							AddToRenderList(ZERenderCommand* RenderCommand);
		virtual void							ClearRenderList();

		virtual void							Render(float ElaspedTime);
		virtual bool							IsGPUBusy();
};

#endif
