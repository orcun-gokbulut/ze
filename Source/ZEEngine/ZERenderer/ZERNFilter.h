//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNFilter.h
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

#include "ZEMeta/ZEObject.h"
#include "ZEInitializable.h"

#include "ZEDS/ZEArray.h"
#include "ZEDS/ZELink.h"
#include "ZEDS/ZEList2.h"
#include "ZEDS/ZEFlags.h"
#include "ZEMath/ZEVector.h"
#include "ZEPointer/ZEHolder.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZERNFilterCommon.h"
#include "ZEExport.ZEEngine.h"

class ZEGRShader;
class ZEGRSampler;
class ZEGRContext;
class ZEGRTexture;
class ZEGRBuffer;
class ZEGRRenderStateData;
class ZEGRComputeRenderStateData;
class ZEGRRenderTarget;
enum ZEGRFormat;

class ZE_EXPORT_ZEENGINE ZERNFilter : public ZEObject, public ZEInitializable
{
	ZE_OBJECT
	friend class ZERNStagePostProcess;
	private:
		ZEArray<ZEHolder<ZEGRTexture>>			TempTextures;

	protected:
		ZEFlags									DirtyFlags;
		ZEGRViewport							Viewport;
		ZELink<ZERNFilter>						Link;

		ZEHolder<ZEGRShader>					ScreenCoverVertexShader;
		
		const ZEGRTexture*						InputTexture;
		ZEUInt									InputSubresource;

		const ZEGRTexture*						OutputTexture;
		ZEUInt									OutputSubresource;

		virtual bool							InitializeInternal();
		virtual bool							DeinitializeInternal();

		virtual bool							UpdateShaders();
		virtual bool							UpdateRenderStates();
		virtual bool							UpdateConstantBuffers();

		bool									Update();

		ZEHolder<const ZEGRTexture>				GetTempTexture(ZEUInt Width, ZEUInt Height, ZEGRFormat Format);

	public:
		virtual void							SetInputTexture(const ZEGRTexture* InputTexture);
		virtual const ZEGRTexture*				GetInputTexture() const;

		virtual void							SetOutputTexture(const ZEGRTexture* OutputTexture);
		virtual const ZEGRTexture*				GetOutputTexture() const;

		virtual void							Apply(ZEGRContext* Context);

												ZERNFilter();
		virtual									~ZERNFilter();

		static void								GenerateGaussianKernel(ZEArray<ZEVector4>& Values, ZEInt Size, float StandartDeviation, bool Horizontal = true);
};

class ZE_EXPORT_ZEENGINE ZERNFilterGaussianBlur : public ZERNFilter
{
	ZE_OBJECT
	private:
		ZEHolder<ZEGRShader>					BlurHorizontalPixelShader;
		ZEHolder<ZEGRShader>					BlurVerticalPixelShader;
		ZEHolder<ZEGRShader>					BlurHorizontalComputeShader;
		ZEHolder<ZEGRShader>					BlurVerticalComputeShader;

		ZEHolder<ZEGRRenderStateData>			BlurHorizontalGraphicsRenderStateData;
		ZEHolder<ZEGRRenderStateData>			BlurVerticalGraphicsRenderStateData;

		ZEHolder<ZEGRComputeRenderStateData>	BlurHorizontalComputeRenderStateData;
		ZEHolder<ZEGRComputeRenderStateData>	BlurVerticalComputeRenderStateData;

		ZEHolder<ZEGRBuffer>					ConstantBuffer;

		bool									UseComputeShader;

		struct
		{
			ZEUInt								KernelRadius;
			float								StandartDeviation;
			ZEVector2							Reserved;
		} Constants;

		virtual bool							InitializeInternal() override;
		virtual bool							DeinitializeInternal() override;

		virtual bool							UpdateShaders() override;
		virtual bool							UpdateRenderStates() override;
		virtual bool							UpdateConstantBuffers() override;

	public:
		virtual const ZEGRTexture*				GetOutputTexture() const override;

		void									SetUseComputeShader(bool UseComputeShader);
		bool									GetUseComputeShader() const;

		void									SetKernelRadius(ZEUInt KernelRadius);
		ZEUInt									GetKernelRadius() const;

		void									SetStandartDeviation(float StandartDeviation);
		float									GetStandartDeviation() const;

		virtual void							Apply(ZEGRContext* Context) override;

												ZERNFilterGaussianBlur();
};

class ZE_EXPORT_ZEENGINE ZERNFilterImageTransform : public ZERNFilter
{
	ZE_OBJECT
	private:
		ZEHolder<ZEGRShader>					ScalePixelShader;

		ZEHolder<ZEGRRenderStateData>			ScaleGraphicsRenderStateData;

		ZEHolder<const ZEGRSampler>				Sampler;
		ZEUInt									OutputWidth;
		ZEUInt									OutputHeight;

		virtual bool							InitializeInternal() override;
		virtual bool							DeinitializeInternal() override;

		virtual bool							UpdateShaders() override;
		virtual bool							UpdateRenderStates() override;

	public:
		void									SetOutputWidth(ZEUInt Width);
		ZEUInt									GetOutputWidth() const;

		void									SetOutputHeight(ZEUInt Height);
		ZEUInt									GetOutputHeight() const;

		void									SetSampler(const ZEGRSampler* Sampler);
		const ZEGRSampler*						GetSampler() const;

		virtual void							Apply(ZEGRContext* Context) override;

												ZERNFilterImageTransform();
};

class ZE_EXPORT_ZEENGINE ZERNFilterEdgeDetection : public ZERNFilter
{
	ZE_OBJECT
	private:
		ZEHolder<ZEGRShader>					EdgeDetectionPixelShader;

		ZEHolder<ZEGRRenderStateData>			EdgeDetectionGraphicsRenderStateData;

		virtual bool							InitializeInternal() override;
		virtual bool							DeinitializeInternal() override;

		virtual bool							UpdateShaders() override;
		virtual bool							UpdateRenderStates() override;

	public:
		virtual void							Apply(ZEGRContext* Context) override;

												ZERNFilterEdgeDetection();
};

class ZE_EXPORT_ZEENGINE ZERNFilterColorTransform : public ZERNFilter
{
	ZE_OBJECT
	private:
		ZEHolder<ZEGRShader>					ColorTransformPixelShader;
	
		ZEHolder<ZEGRRenderStateData>			ColorTransformGraphicsRenderStateData;

		ZEHolder<ZEGRBuffer>					ConstantBuffer;

		struct
		{
			ZEMatrix4x4							ColorMatrix;
		} Constants;

		virtual bool							InitializeInternal() override;
		virtual bool							DeinitializeInternal() override;
	
		virtual bool							UpdateShaders() override;
		virtual bool							UpdateRenderStates() override;
		virtual bool							UpdateConstantBuffers() override;
	
	public:
		void									SetColorMatrix(const ZEMatrix4x4& ColorMatrix);
		const ZEMatrix4x4&						GetColorMatrix() const;

		virtual void							Apply(ZEGRContext* Context) override;
	
												ZERNFilterColorTransform();
};

class ZE_EXPORT_ZEENGINE ZERNFilterNoise : public ZERNFilter
{
	ZE_OBJECT
	private:
		ZEHolder<ZEGRShader>					GenerateNoiseComputeShader;
		ZEHolder<ZEGRShader>					NoisePixelShader;
	
		ZEHolder<ZEGRComputeRenderStateData>	GenerateNoiseComputeRenderStateData;
		ZEHolder<ZEGRRenderStateData>			NoiseGraphicsRenderStateData;
	
		ZEHolder<ZEGRBuffer>					ConstantBuffer;
		ZEHolder<ZEGRTexture>					NoiseTexture;
	
		struct
		{
			float								Weight;
			ZEUInt								Granularity;
			ZEVector2							Size;
		} Constants;
	
		virtual bool							InitializeInternal() override;
		virtual bool							DeinitializeInternal() override;
	
		virtual bool							UpdateShaders() override;
		virtual bool							UpdateRenderStates() override;
		virtual bool							UpdateConstantBuffers() override;

	public:
		virtual void							SetInputTexture(const ZEGRTexture* InputTexture) override;

		void									SetWeight(float Weight);
		float									GetWeight() const;
	
		void									SetGranularity(ZEUInt Granularity);
		ZEUInt									GetGranularity() const;
	
		virtual void							Apply(ZEGRContext* Context) override;
		
												ZERNFilterNoise();
};
