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

#include "ZEMeta/ZEObject.h"
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

struct ZEDOFScreenAlignedQuad
{
	float Position[3];
};

class ZED3D9DOFProcessor : public ZED3D9ComponentBase, public ZEObject
{
	ZE_OBJECT

	private:
		static ZEDOFScreenAlignedQuad	Vertices[4];

		ZESize							BlurPassCount;

		float							FarClamp;				// Max blur amount at FarDistance point. 0 < FarClamp < 1
		float							NearClamp;				// Max blur amount at NearDistance point. 0 < NearClamp < 1
		float							FarDistance;			// Point where max far distance blur will be achieved. FarDistance < FarZ
		float							NearDistance;			// Point where max near distance blur will be achieved. NearDistance > NearZ
		float							FocusDistance;			// Focus point. NearDistance < FocusDistance < FarDistance
		float							FullFocusRange;			// Centered on FoucsDistance point and full focus is achieved on this range.

		ZED3D9FrameRenderer*			Renderer;				// Renderer

		ZED3D9Texture2D*				BlurBuffers[9];

		ZED3D9Texture2D*				InputColorBuffer;		// Input color texture
		ZED3D9Texture2D*				InputDepthBuffer;		// Input depth texture
		ZED3D9ViewPort*					OutputBuffer;			// DOF applied output texture

		ZED3D9VertexShader*				VertexShaderCommon;		// Common vertex shader for all passes

		ZED3D9PixelShader*				PixelShaderDOF;			// DOF pixel shader
		ZED3D9PixelShader*				PixelShaderUsDsBlur;	// Up/Down Sampling + Blur pixel shader

		LPDIRECT3DVERTEXDECLARATION9	VertexDeclaration;		// Vertex deceleration for quadiliteral

		void							CreateRenderTargets();

		void							UpDownSampleBlur(ZED3D9Texture2D* Input, ZED3D9Texture2D* Output);

	public:
		void							SetBlurPassCount(ZEUInt Count);
		ZEUInt							GetBlurPassCount() const;

		void							SetFocusDistance(float Value);
		float							GetFocusDistance() const;

		void							SetFullFocusRange(float Value);
		float							GetFullFocusRange() const;

		void							SetFarDistance(float Value);
		float							GetFarDistance() const;

		void							SetNearDistance(float Value);
		float							GetNearDistance() const;

		void							SetFarClamp(float Value);
		float							GetFarClamp() const;

		void							SetNearClamp(float Value);
		float							GetNearClamp() const;

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

#endif
