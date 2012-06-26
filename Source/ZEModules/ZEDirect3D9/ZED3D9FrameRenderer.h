//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9FrameRenderer.h
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
#ifndef __ZE_D3D9_FRAME_RENDERER_H__
#define __ZE_D3D9_FRAME_RENDERER_H__


#include "ZED3D9EDProcessor.h"
#include "ZED3D9HDRProcessor.h"
#include "ZED3D9DOFProcessor.h"
#include "ZED3D9FogProcessor.h"
#include "ZED3D9ComponentBase.h"
#include "ZED3D9BlurProcessor.h"
#include "ZED3D9SSAOProcessor.h"
#include "ZED3D9SSAAProcessor.h"
#include "ZEGraphics/ZEFrameRenderer.h"
#include "ZED3D9ColorTransformProcessor.h"
#include "ZED3D9MLAAProcessor.h"
#include "ZED3D9GrainProcessor.h"
#include "ZED3D9ChannelDisorientationProcessor.h"
#include "ZED3D9TextureMaskProcessor.h"

class ZELight;
class ZEPointLight;
class ZED3D9ViewPort;
class ZED3D9Texture2D;
class ZED3D9PixelShader;
class ZEProjectiveLight;
class ZEDirectionalLight;
class ZED3D9VertexShader;
class ZEOmniProjectiveLight;
class ZED3D9StaticVertexBuffer;

class ZED3D9FrameRenderer : public ZEFrameRenderer, public ZED3D9ComponentBase
{
	friend class ZED3D9Module;
	friend class ZED3D9ShadowRenderer;

	private:
		ZEUInt32							LightStencilMaskValue;

		ZED3D9Texture2D*					ABuffer;
		ZED3D9ViewPort*						ViewPort;

		//ZED3D9Texture2D*					ABuffer;
		ZED3D9Texture2D* 					GBuffer1;
		ZED3D9Texture2D*					GBuffer2;
		ZED3D9Texture2D*					GBuffer3;
		ZED3D9Texture2D*					LBuffer1;
		ZED3D9Texture2D*					LBuffer2;
		ZED3D9Texture2D*					SSAOBuffer;
		ZED3D9Texture2D*					CTInputBuffer;
		//ZED3D9Texture2D*					EDInputBuffer;
		ZED3D9Texture2D*					HDRInputBuffer;
		ZED3D9Texture2D*					FogInputBuffer;
		ZED3D9Texture2D*					DOFInputBuffer;
		ZED3D9Texture2D*					SSAAInputBuffer;
		ZED3D9Texture2D*					BlurInputBuffer;
		ZED3D9Texture2D*					GrainInputBuffer;
		ZED3D9Texture2D*					CDInputBuffer;
		ZED3D9Texture2D*					TMInput;

		ZED3D9SSAOProcessor					SSAOProcessor;
		ZED3D9SSAAProcessor					SSAAProcessor;
		ZED3D9MLAAProcessor					MLAAProcessor;
		
		ZED3D9EDProcessor					EDProcessor;
		ZESmartArray<ZERenderCommand>		CommandList;

		ZECamera*							Camera;

		ZESmartArray<ZELight*>				Lights;
		ZEArray<ZEPostProcessor*>			PostProcessors;

		struct 
		{
			ZED3D9StaticVertexBuffer*		LightMeshVB;
			ZED3D9VertexShader*				PointLightVS;
			ZED3D9PixelShader*				PointLightPS;
			ZED3D9VertexShader*				DirectionalLightVS;
			ZED3D9PixelShader*				DirectionalLightPS;
			ZED3D9VertexShader*				ProjectiveLightVS;
			ZED3D9PixelShader*				ProjectiveLightPS;
			ZED3D9VertexShader*				OmniProjectiveLightVS;
			ZED3D9PixelShader*				OmniProjectiveLightPS;
		} LightningComponents;


		static bool							CheckRenderCommand(ZERenderCommand* RenderCommand);
		static void							PumpStreams(ZERenderCommand* RenderCommand);

		void								InitializeLightning();
		void								DeinitializeLightning();

		void								InitializeRenderTargets();
		void								DeinitializeRenderTargets();

		void								DrawPointLight(ZEPointLight* Light);
		void								DrawDirectionalLight(ZEDirectionalLight* Light);
		void								DrawProjectiveLight(ZEProjectiveLight* Light);
		void								DrawOmniProjectiveLight(ZEOmniProjectiveLight* Light);

		void								DoPreZPass();
		void								DoGBufferPass();
		void								DoLightningPass();
		void								DoForwardPass();
		void								Do2DPass();
		void								DoPostProcess();

	protected:
											ZED3D9FrameRenderer();
		virtual								~ZED3D9FrameRenderer();					

	public:
		ZED3D9FogProcessor					FogProcessor;
		ZED3D9HDRProcessor					HDRProcessor;
		ZED3D9DOFProcessor					DOFProcessor;
		ZED3D9BlurProcessor					BlurProcessor;
		ZED3D9GrainProcessor				GrainProcessor;
		ZED3D9ColorTransformProcessor		CTProcessor;
		ZED3D9ChannelDisorientationProcessor CDProcessor;
		ZED3D9TextureMaskProcessor			TMProcessor;
		

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

		virtual void						SetLights(ZESmartArray<ZELight*>& Lights);

		virtual void						AddToLightList(ZELight* Light);
		virtual void						ClearLightList();

		virtual void						AddToRenderList(ZERenderCommand* RenderCommand);
		virtual void						ClearRenderList();

		virtual void						Render(float ElaspedTime);
};

#endif
