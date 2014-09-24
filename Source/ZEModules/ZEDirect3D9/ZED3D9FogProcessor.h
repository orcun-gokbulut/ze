//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9FogProcessor.h
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
#ifndef __ZE_D3D9_FOG_PROCESSOR_H__
#define __ZE_D3D9_FOG_PROCESSOR_H__

#include "ZEMeta/ZEObject.h"
#include "ZED3D9ComponentBase.h"
#include "ZED3D9BlurProcessor.h"
#include "ZEMath\ZEVector.h"

class ZETexture2D;
class ZED3D9ViewPort;
class ZEFrameRenderer;
class ZED3D9Texture2D;
class ZED3D9PixelShader;
class ZED3D9VertexShader;
class ZED3D9FrameRenderer;
class ZETexture2DResource;

enum ZED3D9FogModel
{
	ZE_D3D9_FM_LINEAR				= 0,
	ZE_D3D9_FM_EXPONENTIAL			= 1,
	ZE_D3D9_FM_EXPONENTIAL_SQUARE	= 2
};

struct ZEFOGScreenAlignedQuad
{
	float Position[3];
};

class ZED3D9FogProcessor : public ZED3D9ComponentBase, public ZEObject
{
	ZE_OBJECT

	protected:
		static ZEFOGScreenAlignedQuad	Vertices[4];

		float							FogHeight;
		float							FogDistanceFar;
		float							FogDistanceNear;
		float							FogFactor;
		
		ZEVector3						FogColor;
		ZEVector3						OutScatterFactor;
		ZEVector3						InScatterFactor;

		ZED3D9FogModel					FogModel;

		ZED3D9FrameRenderer*			Renderer;

		ZED3D9Texture2D*				InputColorBuffer;
		ZED3D9Texture2D*				InputDepthBuffer;
		ZED3D9ViewPort*					OutputBuffer;

		ZED3D9VertexShader*				VertexShaderFog;
		ZED3D9PixelShader*				PixelShaderExpFog;
		ZED3D9PixelShader*				PixelShaderLinearFog;
		ZED3D9PixelShader*				PixelShaderExpSquareFog;

		LPDIRECT3DVERTEXDECLARATION9	VertexDeclaration;
	
	private:
		
	public:
		void							SetFogModel(ZED3D9FogModel Model);
		ZED3D9FogModel					GetFogModel() const;

		void							SetFogColor(ZEVector3 Color);
		ZEVector3						GetFogColor() const;

		void							SetOutScatterFactor(ZEVector3 Color);
		ZEVector3						GetOutScatterFactor() const;

		void							SetInScatterFactor(ZEVector3 Color);
		ZEVector3						GetInScatterFactor() const;
		
		void							SetFogHeight(float Value);
		float							GetFogHeight() const;

		void							SetFogDistanceFar(float Value);
		float							GetFogDistanceFar() const;
		
		void							SetFogDistanceNear(float Value);
		float							GetFogDistanceNear() const;
		
		void							SetFogFactor(float Value);
		float							GetFogFactor() const;

		void							SetRenderer(ZEFrameRenderer* Renderer);
		ZEFrameRenderer*				GetRenderer();

		void							SetInputDepth(ZETexture2D* Texture);
		ZETexture2D*					GetInputDepth();

		void							SetInputColor(ZETexture2D* Texture);
		ZETexture2D*					GetInputColor();

		void							SetOutput(ZED3D9ViewPort* Texture);
		ZED3D9ViewPort*					GetOutput();

		void							Initialize();
		void							Deinitialize();

		void							OnDeviceLost();
		void							OnDeviceRestored();

		void							Process();

										ZED3D9FogProcessor();
		virtual							~ZED3D9FogProcessor();
};

#endif
