//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9CloudMaterial.h
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
#ifndef __ZE_D3D9_CLOUD_MATERIAL_H__
#define __ZE_D3D9_CLOUD_MATERIAL_H__

#include "ZED3D9ComponentBase.h"
#include "..\ZECloudMaterial.h"

class ZEFrameRenderer;
class ZERenderCommand;
class ZED3D9VertexShader;
class ZED3D9PixelShader;
class ZED3D9Texture2D;
class ZED3D9StaticIndexBuffer;
class ZED3D9StaticVertexBuffer;
class ZED3D9VertexDeclaration;



class ZED3D9CloudMaterial : public ZECloudMaterial, public ZED3D9ComponentBase
{
	friend class ZED3D9Module;
	friend class ZECould;

	protected:
		struct Vertex0	// Vertex type 0
		{
			float X; // x index
			float Z; // z index
			float U; // texture coordinate u 
			float V; // texture coordinate v 
		};

		struct Vertex1	// Vertex type 1
		{
			float Position[4];
			float UV[2];

		};

		// Geometry creation parameters
		ZEVector2						StartXZ;
		ZEVector2						CellCountXZ;
		ZEVector2						CellSizeXZ;

		// Transformation matrices for shadow pass
		ZEMatrix4x4						WorldToShadowTexProjMatrix;
		ZEMatrix4x4						WorldToShadowTexCoordMatrix;
		
		// Shaders
		ZED3D9VertexShader*				VertexShaderCloudShadow;
		ZED3D9PixelShader*				PixelShaderCloudShadow;

		ZED3D9VertexShader*				VertexShaderCloudDensity;
		ZED3D9PixelShader*				PixelShaderCloudDensity;

		ZED3D9VertexShader*				VertexShaderDensityBlur;
		ZED3D9PixelShader*				PixelShaderDensityBlur;

		ZED3D9VertexShader*				VertexShaderDrawCloud;
		ZED3D9PixelShader*				PixelShaderDrawCloud;

		// BUffers
		ZEUInt							CloudShadowVertexCount;
		ZEUInt							CloudShadowTriangleCount;
		ZED3D9StaticIndexBuffer*		CloudShadowIndexBuffer;
		ZED3D9StaticVertexBuffer*		CloudShadowVertexBuffer;
		ZED3D9VertexDeclaration*		CloudShadowVertexDeclaration;

		ZEUInt							DensityBlurTriangleCount;
		ZED3D9StaticVertexBuffer*		DensityBlurVertexBuffer;
		ZED3D9VertexDeclaration*		DensityBlurVertexDeclaration;

		ZEUInt							RenderCloudVertexCount;
		ZEUInt							RenderCloudTriangleCount;
		ZED3D9StaticIndexBuffer*		RenderCloudIndexBuffer;
		ZED3D9StaticVertexBuffer*		RenderCloudVertexBuffer;
		ZED3D9VertexDeclaration*		RenderCloudVertexDeclaration;

		// Textures / Render targets
		ZED3D9Texture2D*				CloudTexture;
		ZED3D9Texture2D*				CloudShadowBuffer;
		ZED3D9Texture2D*				CloudDensityBuffer;
		ZED3D9Texture2D*				CloudDensityBlurBuffer;

		void							UpdateShadowTransformations();
		void							CreateBuffers();
		void							ReleaseBuffers();

		void							CreateRenderTargets();
		void							ReleaseRenderTargets();

		void							CreateShaders();
		void							ReleaseShaders();

	private:

	public:
										ZED3D9CloudMaterial();
		virtual							~ZED3D9CloudMaterial();

		virtual void					UpdateMaterial();
		virtual bool					SetupForwardPass(ZEFrameRenderer* Renderer, ZERenderCommand* RenderCommand) const;
		virtual void					Release();

};

#endif // __ZE_D3D9_CLOUD_MATERIAL_H__
