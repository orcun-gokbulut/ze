//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9HBAOProcessor.h
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

#ifndef __ZE_HBAO_PROCESSOR_H__
#define __ZE_HBAO_PROCESSOR_H__

#include "ZETypes.h"
#include "ZEMeta/ZEObject.h"
#include "ZED3D9ComponentBase.h"

class ZETexture2D;
class ZED3D9ViewPort;
class ZED3D9Texture2D;
class ZED3D9PixelShader;
class ZED3D9VertexShader;
class ZED3D9FrameRenderer;

struct ZEHBAOScreenAlignedQuad
{
	float	Position[3];
	float	TexCoord[2];
};

ZE_CLASS(ZED3D9HBAOProcessor)

class ZED3D9HBAOProcessor : public ZED3D9ComponentBase, public ZEObject
{
	ZE_OBJECT

	private:
		ZED3D9FrameRenderer*			Renderer;
		ZED3D9Texture2D*				Output;
		ZED3D9Texture2D*				InputDepth;
		ZED3D9Texture2D*				InputNormal;
		
		ZED3D9Texture2D*				TempBuffer;
		ZED3D9Texture2D*				RandomAngles;
		ZED3D9Texture2D*				HalfResDepth;
		ZED3D9Texture2D*				HalfResNormal;

		struct
		{
			ZED3D9VertexShader*			Vertex;
			ZED3D9PixelShader*			DownSampleDepth;
			ZED3D9PixelShader*			DownSampleNormal;
			ZED3D9PixelShader*			AmbientOcclusion;
			ZED3D9PixelShader*			BiliteralVertical;
			ZED3D9PixelShader*			BiliteralHorizontal;

		} Shaders;

		static ZEHBAOScreenAlignedQuad	Vertices[4];
		LPDIRECT3DVERTEXDECLARATION9	VertexDeclaration;

		float							OcclusionRadius;
		float							RadiusMultiplier;
		ZEUInt							StepCount;
		ZEUInt							DirectionCount;
		float							AngleBias;
		float							Attenuation;
		float							Contrast;

		float							BlurRadius;
		float							BlurSharpness;
		float							BlurEdgeThreshold;

		void							UpdateBuffers();
		void							DestroyBuffers();

		void							UpdateShaders();
		void							DestroyShaders();

		void							UpdateStates();

		void							DownSampleDepth(const ZED3D9Texture2D* Input, ZED3D9ViewPort* Output);
		void							DownSampleNormal(const ZED3D9Texture2D* Input, ZED3D9ViewPort* Output);

		void							AmbientOcclusion(const ZED3D9Texture2D* InputDepth, const ZED3D9Texture2D* InputNormal, ZED3D9ViewPort* OutputBuffer);

		void							BiliteralFilterVertical(const ZED3D9Texture2D* Input, ZED3D9ViewPort* Output);
		void							BiliteralFilterHorizontal(const ZED3D9Texture2D* Input, ZED3D9ViewPort* Output);

	public:
		void							SetOcclusionRadius(float Value);
		float							GetOcclusionRadius() const;

		void							SetRadiusMultiplier(float Value);
		float							GetRadiusMultiplier() const;

		void							SetStepCount(ZEUInt Value);
		ZEUInt							GetStepCount() const;

		void							SetDirectionCount(ZEUInt Value);
		ZEUInt							GetDirectionCount() const;

		void							SetAngleBias(float Value);
		float							GetAngleBias() const;

		void							SetAttenuation(float Value);
		float							GetAttenuation() const;

		void							SetContrast(float Value);
		float							GetContrast() const;

		void							SetBlurRadius(float Value);
		float							GetBlurRadius() const;

		void							SetBlurSharpness(float Value);
		float							GetBlurSharpness() const;

		void							SetBlurEdgeThreshold(float Value);
		float							GetBlurEdgeThreshold() const;

		void							SetInputDepth(ZED3D9Texture2D* InputBuffer);
		ZED3D9Texture2D*				GetInputDepth();

		void							SetInputNormal(ZED3D9Texture2D* InputBuffer);
		ZED3D9Texture2D*				GetInputNormal();

		void							SetOutput(ZED3D9Texture2D* OutputBuffer);
		ZED3D9Texture2D*				GetOutput();

		void							SetRenderer(ZED3D9FrameRenderer* FrameRenderer);
		ZED3D9FrameRenderer*			SetRenderer() const;

		void							Process();

		void							Initialize();
		void							Deinitialize();

										ZED3D9HBAOProcessor();
										~ZED3D9HBAOProcessor();
};

#endif
