//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9MLAAProcessor.h
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
#ifndef __ZE_D3D9_MLAA_PROCESSOR_H__
#define __ZE_D3D9_MLAA_PROCESSOR_H__

#include "ZEMeta/ZEObject.h"
#include "ZED3D9ComponentBase.h"

class ZETexture2D;
class ZED3D9ViewPort;
class ZEFrameRenderer;
class ZED3D9Texture2D;
class ZED3D9PixelShader;
class ZED3D9VertexShader;
class ZED3D9FrameRenderer;
class ZETexture2DResource;

struct ZEMLAAScreenAlignedQuad
{
	float Position[3];
};

ZE_META_OBJECT_DESCRIPTION(ZED3D9MLAAProcessor);

class ZED3D9MLAAProcessor : public ZED3D9ComponentBase, public ZEObject
{
	ZE_META_OBJECT(ZED3D9MLAAProcessor)

	private:
		bool							VisualizeEdges;
		bool							VisualizeWeights;

		static ZEMLAAScreenAlignedQuad	Vertices[4];

		float							Treshold;
		float							SearchStep;

		ZED3D9Texture2D*				EdgeBuffer;
		ZED3D9Texture2D*				AreaBuffer;
		ZED3D9Texture2D*				BlendWeightBuffer;

		ZED3D9FrameRenderer*			Renderer;

		LPDIRECT3DVERTEXDECLARATION9	VertexDeclaration;

		ZED3D9VertexShader*				VertexShaderCommon;
		ZED3D9PixelShader*				PixelShaderEdgeDetection;
		ZED3D9PixelShader*				PixelShaderWeightBlending;
		ZED3D9PixelShader*				PixelShaderColorBlending;

		ZED3D9Texture2D*				InputColorBuffer;
		ZED3D9Texture2D*				InputDepthBuffer;
		ZED3D9Texture2D*				InputNormalBuffer;
		
		ZED3D9ViewPort*					OutputBuffer;

		void							CreateRenderTargets();
		void							DestroyRenderTargets();

		void							EdgeDetectionPass(ZED3D9Texture2D* Depth, ZED3D9Texture2D* Normal, ZED3D9Texture2D* Output);
		void							WeightBlendingPass(ZED3D9Texture2D* Endges, ZED3D9Texture2D* AreaTexture, ZED3D9Texture2D* Output);
		void							ColorBlendingPass(ZED3D9Texture2D* Color, ZED3D9Texture2D* Weights, ZED3D9ViewPort* Output);

	public:
		void							SetVisualizeEdges(bool Enabled);
		bool							GetVisualizeEdges() const;

		void							SetVisualizeWeights(bool Enabled);
		bool							GetVisualizeWeights() const;

		void							SetTreshold(float Value);
		float							GetTreshold() const;
		
		void							SetSearchStep(float Value);
		float							GetSearchStep() const;

		void							SetRenderer(ZEFrameRenderer* Renderer);
		ZEFrameRenderer*				GetRenderer();

		void							SetInputColor(ZETexture2D* Texture);
		ZETexture2D*					GetInputColor();

		void							SetInputDepth(ZETexture2D* Texture);
		ZETexture2D*					GetInputDepth();

		void							SetInputNormal(ZETexture2D* Texture);
		ZETexture2D*					GetInputNormal();

		void							SetOutput(ZED3D9ViewPort* Texture);
		ZED3D9ViewPort*					GetOutput();

		void							Initialize();
		void							Deinitialize();

		void							OnDeviceLost();
		void							OnDeviceRestored();

		void							Process();

										ZED3D9MLAAProcessor();
		virtual							~ZED3D9MLAAProcessor();

}; // class ZED3D9MLAAProcessor

/*
ZE_POST_PROCESSOR_START(Meta)
	<zinek>
		<meta> 
			<class name="ZED3D9MLAAProcessor">
				<noinstance>true</noinstance>
				<description>ZED3D9MLAAProcessor</description>
				
				<property name="VisualizeEdges" type="boolean" autogetset="yes" description="..."/>
				<property name="VisualizeWeights" type="boolean" autogetset="yes" description="..."/>
				
				<property name="Treshold" type="float" autogetset="yes" description="..."/>
				<property name="SearchStep" type="float" autogetset="yes" description="..."/>
				
			</class>
		</meta>
	</zinek>
ZE_POST_PROCESSOR_END()
*/

#endif // __ZE_D3D9_MLAA_PROCESSOR_H__
