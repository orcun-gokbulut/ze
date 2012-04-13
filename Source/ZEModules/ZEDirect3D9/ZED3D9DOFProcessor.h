//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9DOFProcessor.h
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
#ifndef __ZE_D3D9_DOF_PROCESSOR_H__
#define __ZE_D3D9_DOF_PROCESSOR_H__

#include "ZED3D9ComponentBase.h"
#include "ZED3D9BlurProcessor.h"

class ZED3D9PixelShader;
class ZED3D9VertexShader;
class ZED3D9Texture2D;
class ZETexture2D;
class ZED3D9ViewPort;
class ZEFrameRenderer;
class ZED3D9FrameRenderer;
class ZETexture2DResource;


class ZED3D9DOFProcessor : public ZED3D9ComponentBase
{
	private:
		float							FarClamp;			// Max blur between FarDistance and FocusDistance [Range: 0-1]
		float							NearClamp;			// Max blur between FocusDistance and NearDistance [Range: 0-1]
		float							FarDistance;		// Far distance [FocusDistance < FarDistance < ZFar where FarDistance is in meters]
		float							NearDistance;		// Near distance [ZNear < NearDistance < FocusDistance where NearDistance is in meters]
		float							FocusDistance;		// Focus Distance [NearDistance < FocusDistance < FarDistance where FocusDistance is in meters]

		ZED3D9BlurKernel*				HorizontalKernel;	// Horizontal blur kernel
		ZED3D9BlurKernel*				VerticalKernel;		// Vertical blur kernel

		ZED3D9FrameRenderer*			Renderer;			// Renderer

		ZED3D9Texture2D*				ColorBufferDS2xBlur;// 1/4 sized color buffer
		ZED3D9Texture2D*				ColorBufferDS2xTmp; // 1/4 sized temp buffer

		ZED3D9Texture2D*				InputColorBuffer;	// Input color texture
		ZED3D9Texture2D*				InputDepthBuffer;	// Input depth texture
		ZED3D9ViewPort*					OutputBuffer;		// DOF applied output texture

		ZED3D9VertexShader*				VertexShaderCommon;	// Common vertex shader for all passes

		ZED3D9PixelShader*				PixelShaderDOF;		// DOF pixel shader
		ZED3D9PixelShader*				PixelShaderDS2x;	// Down Sampling pixel shader
		ZED3D9PixelShader*				PixelShaderBlurH;	// Horizontal blur pixel shader
		ZED3D9PixelShader*				PixelShaderBlurV;	// Vertical blur pixel shader


		LPDIRECT3DVERTEXDECLARATION9	VertexDeclaration;	// Vertex deceleration for quadiliteral

		void							CreateBlurKernels();
		void							CreateRenderTargets();

	public:
		float							GetFocusDistance();
		void							SetFocusDistance(float Value);

		float							GetFarDistance();
		void							SetFarDistance(float Value);

		float							GetNearDistance();
		void							SetNearDistance(float Value);

		float							GetFarClamp();
		void							SetFarClamp(float Value);

		float							GetNearClamp();
		void							SetNearClamp(float Value);

		void							SetRenderer(ZEFrameRenderer* Renderer);
		ZEFrameRenderer*				GetRenderer();

		void							SetInputDepth(ZETexture2D* Texture);
		ZETexture2D*					GetInputDepth();

		void							SetInputColor(ZETexture2D* Texture);
		ZETexture2D*					GetInputColor();

		void							SetOutput(ZED3D9ViewPort* Texture);
		ZED3D9ViewPort*					GetOutput();

		void							OnDeviceLost();
		void							OnDeviceRestored();

		void							Process();

		void							Initialize();
		void							Deinitialize();

										ZED3D9DOFProcessor();
		virtual							~ZED3D9DOFProcessor();
};

#endif	/* __ZE_D3D9_DOF_PROCESSOR_H__ */
