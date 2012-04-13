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
enum ZETextureCubeFace;

class ZED3D9ShadowRenderer : public ZEShadowRenderer, public ZED3D9ComponentBase
{
	friend ZED3D9Module;
	private:
		ZED3D9ViewPort*							ViewPort;

		ZESmartArray<ZERenderCommand>			CommandList;

		ZEArray<ZEPostProcessor*>				PostProcessors;

		ZELight*								Light;
		
		bool									Face;

		bool									InitializeShaders();
		void									DeinitializeShaders();

		void									RenderProjectiveLight();
		void									RenderPointLight();
		void									RenderOmniProjectiveLight();
		void									RenderDirectionalLight();

		LPDIRECT3DSURFACE9						ShadowMapFrameBuffer;
		LPDIRECT3DSURFACE9						ShadowMapZBuffer;

	protected:
												ZED3D9ShadowRenderer();
		virtual									~ZED3D9ShadowRenderer();

		void									DrawRenderCommand(ZERenderCommand* RenderCommand);

	public:	
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

		virtual void							AddToRenderList(ZERenderCommand* RenderCommand);
		virtual void							ClearRenderList();

		virtual void							Render(float ElaspedTime);
};

#endif



