//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageAO.h
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

#include "ZERNStage.h"
#include "ZEPointer/ZEHolder.h"
#include "ZEDS/ZEList2.h"
#include "ZEGraphics/ZEGRSamplerState.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEFlags.h"

class ZEGRTexture2D;
class ZEGRRenderTarget;
class ZEGRRenderStateData;
class ZEGRShader;
class ZEGRContext;
class ZEGRConstantBuffer;
class ZERNRenderer;

class ZERNStageAO : public ZERNStage
{
private:
	ZEFlags								DirtyFlags;

	ZEHolder<ZEGRTexture2D>			OcclusionMap;
	ZEHolder<ZEGRTexture2D>			BlurTexture;
	ZEHolder<ZEGRTexture2D>			RandomVectorsTexture;

	ZEHolder<ZEGRConstantBuffer>		ConstantBuffer;
	ZEHolder<ZEGRConstantBuffer>		BilateralConstantBuffer;

	ZEHolder<ZEGRShader>				VertexShader;
	ZEHolder<ZEGRShader>				BlendVertexShader;

	ZEHolder<ZEGRShader>				PixelShader;
	ZEHolder<ZEGRShader>				BlendPixelShader;
	ZEHolder<ZEGRShader>				BilateralPixelShader;

	ZEHolder<ZEGRRenderStateData>		RenderStateData;
	ZEHolder<ZEGRRenderStateData>		BlendRenderStateData;
	ZEHolder<ZEGRRenderStateData>		BilateralRenderStateData;

	ZEGRSamplerState					SamplerPointBorder;
	ZEGRSamplerState					SamplerPointWrap;
	ZEGRSamplerState					SamplerPointClamp;
	ZEGRSamplerState					SamplerLinearClamp;

	ZEArray<ZEVector4>					HorizontalValues;
	ZEArray<ZEVector4>					VerticalValues;

	ZEUInt								Width;
	ZEUInt								Height;

	struct SSAOConstants
	{
		ZEVector4						OffsetVectors[14];
		float							OcclusionRadius;
		float							DistanceMax;
		float							DistanceMin;
		float							Intensity;
		float							DownScale;
		ZEVector3						Reserved;
	}Constants;

	struct SSAOBilateralConstants
	{
		float							DepthThreshold;
		float							IntensityThreshold;
		float							Reserved;
		ZEUInt							KernelSize;
		ZEVector4						KernelValues[32];
	}BilateralConstants;

	void								CreateRandomVectors();
	void								CreateOffsetVectors();

	bool								Update();
	bool								UpdateConstantBuffers();
	bool								UpdateShaders();
	bool								UpdateRenderStates();
	bool								UpdateTextures();

	void								GenerateOcclusionMap(ZERNRenderer* Renderer, ZEGRContext* Context);
	void								ApplyBlur(ZEGRContext* Context);
	void								BlendWithAccumulationBuffer(ZERNRenderer* Renderer, ZEGRContext* Context);

	virtual bool						InitializeSelf();						
	virtual void						DeinitializeSelf();

public:
	ZEGRTexture2D*						GetOcclusionMap();

	void								SetDistanceMin(float DistanceMin);
	float								GetDistanceMin() const;

	void								SetDistanceMax(float DistanceMax);
	float								GetDistanceMax() const;

	void								SetIntensity(float Intensity);
	float								GetIntensity() const;

	void								SetOcclusionRadius(float Radius);
	float								GetOcclusionRadius() const;

	void								SetBilateralDepthThreshold(float DepthThreshold);
	float								GetBilateralDepthThreshold() const;

	void								SetBilateralIntensityThreshold(float IntensityThreshold);
	float								GetBilateralIntensityThreshold() const;

	void								SetBilateralKernelValues(const ZEVector4* Values, ZEUInt KernelSize);
	const ZEVector4* 					GetBilateralKernelValues();

	void								SetOcclusionMapDownScale(float DownScale);
	float								GetOcclusionMapDownScale() const;

	virtual ZEInt						GetId();
	virtual const ZEString&				GetName();

	virtual bool						Setup(ZERNRenderer* Renderer, ZEGRContext* Context, ZEList2<ZERNCommand>& Commands);
	virtual void						CleanUp(ZERNRenderer* Renderer, ZEGRContext* Context);

										ZERNStageAO();
};
