//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9Renderer.h
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
#ifndef __ZE_D3D9_RENDERER_H__
#define __ZE_D3D9_RENDERER_H__

#include "ZEGraphics\ZERenderer.h"
#include "ZED3D9ComponentBase.h"
#include "ZED3D9HDRProcessor.h"
#include "ZED3D9SSAOProcessor.h"

class ZED3D9ViewPort;

class ZED3D9Renderer : public ZERenderer, public ZED3D9ComponentBase
{
	friend class ZED3D9Module;
	friend class ZED3D9ShadowRenderer;

	private:
		ZED3D9ViewPort*						ViewPort;

		LPDIRECT3DTEXTURE9					AmbientColorRenderTarget;
		LPDIRECT3DTEXTURE9					NormalDepthRenderTarget;
		LPDIRECT3DTEXTURE9					PositionRenderTarget;
		LPDIRECT3DTEXTURE9					TempRenderTarget;
		ZED3D9HDRProcessor					HDRProcessor;
		ZED3D9SSAOProcessor					SSAOProcessor;

		ZESmartArray<ZERenderOrder>			NonTransparent;
		ZESmartArray<ZERenderOrder>			Transparent;
		ZESmartArray<ZERenderOrder>			Imposter;
		
		ZECamera*							Camera;
		
		ZEArray<ZEPostProcessor*>			PostProcessors;

		static bool							CheckRenderOrder(ZERenderOrder* RenderOrder);
		static void							PumpStreams(ZERenderOrder* RenderOrder);
		static void							DrawAmbientPass(ZERenderOrder* RenderOrder, ZECamera* Camera);
		static void							DrawLightPasses(ZERenderOrder* RenderOrder, ZECamera* Camera);

		void								CreateRenderTargets();
		void								DestroyRenderTargets();

	protected:
											ZED3D9Renderer();
		virtual								~ZED3D9Renderer();					

	public:
		virtual void						SetViewPort(ZEViewPort* ViewPort);
		virtual ZEViewPort*					GetViewPort();

		virtual void						SetCamera(ZECamera* Camera);
		virtual ZECamera*					GetCamera();

		virtual bool						Initialize();
		virtual void						Deinitialize();
		virtual void						Destroy();

		virtual void						DeviceLost();
		virtual bool						DeviceRestored();

		virtual ZEArray<ZEPostProcessor*>&	GetPostProcessors();
		virtual void						AddPostProcessor(ZEPostProcessor* PostProcessor);
		virtual void						RemovePostProcessor(ZEPostProcessor* PostProcessor);

		virtual void						AddToRenderList(ZERenderOrder* RenderOrder);
		virtual void						ClearList();

		virtual void						Render(float ElaspedTime);
};

#endif




