//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - PostProcessor.h
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
#ifndef __ZE_POSTPROCESSOR_H__
#define __ZE_POSTPROCESSOR_H__

#include "Graphics/TextureResource.h"
#include "ZEMath/ZEMath.h"

enum ZEPostProcessorDestination
{
	ZE_PPD_INTERNAL			= 0,
	ZE_PPD_OUTPUT			= 1,
	ZE_PPD_FRAMEBUFFER		= 2
};

enum ZEPostProcessorSource
{
	ZE_PPS_INTERNAL			= 0,
	ZE_PPS_INPUT			= 1,
};

enum ZEPostEffectMultiplePass
{
	ZE_PEMP_HORIZANTAL,
	ZE_PEMP_VERTICAL,
	ZE_PEMP_BOTH
};

class ZEPostEffect
{
	virtual const char*					GetPostEffectName();

	virtual void						SetInput(ZERenderer* Renderer) = 0;
	virtual void						SetInput(ZEPostEffect* Renderer) = 0;
	virtual void						SetInput(ZETexture*	Texture) = 0;
	virtual void						SetOutputToScreen() = 0;
	virtual void						SetOutput(ZETexture* Texture) = 0;
	virtual void						SetOutput(ZEPostEffect* Renderer) = 0;

	virtual void						Apply() = 0;

};

class ZE2x2KernelFilterPostEffect : public ZEPostEffect
{
	private:
		float							Kernel[2][2];

	public:
		void							SetKernel(float** Elements);
		float***						GetKernel();
};

class ZE3x3KernelFilterPostEffect : public ZEPostEffect
{
	private:
		float							Kernel[3][3];
	
	public:
		void							SetKernel(float** Elements);
		float***						GetKernel();
};

class ZE4x4KernelFilterPostEffect : public ZEPostEffect
{
	private:
		float							Kernel[4][4];

	public:
		void							SetKernel(float** Elements);
		float***						GetKernel();
};

class ZE5x5KernelFilterPostEffect : public ZEPostEffect
{
	private:
		float							Kernel[5][5];

	public:
		void							SetKernel(float** Elements);
		float***						GetKernel();
};

class ZELineerlySperableKernel : public ZEPostEffect
{
	private:
		ZEArray<float>					Horizantal;
		ZEArray<float>					Vertical;

	public:
		void							SetHorizantalLineerKernel(const ZEArray<float>& Kernel);
		const ZEArray<float>&			GetHorizantalLineerKernel();

		void							SetKernel(float* Horizantal, size_t Count, float* Vertical, size_tCount);
		float***						GetKernel();
};

class ZEInversePostEffect : public ZEPostEffect
{
};

class ZEGrayScale : public ZEPostEffect
{
	private:
		float							RedFactor;
		float							BlueFactor;
		float							GreenFactor;
	
	public:
		void							SetRedFactor(float Red);
		float							GetRedFactor();
		void							SetGreenFactor(float Green);
		float							GetGreenFactor();
		void							SetBlueFactor(float Blue);
		float							GetBlueFactor();
										
										ZEGrayScale();
};

class ZEBlurPostEffect : public ZEPostEffect
{
	private:
		float							Radious;
		float							ro;
		float							GaussianFilter(unsigned int x, unsigned int y);

	public:
		ZEPostEffectMultiplePass		GetPasses();
		void							SetPasses(ZEPostEffectMultiplePass Passes);


		float							GetRadius();
		void							SetRadius(unsigned int Radius);		
		float							GetStandarDeviation();
		void							SetStandartDeviation(float ro);
};

class ZEColorTransformPostEffect : public ZEPostEffect
{
	private:
		ZEMatrix4x4						ColorTransform;
	
	public:
		void							SetTransformation(const ZEMatrix4x4& Transformation);
		const ZEMatrix4x4&				GetTransformation();
};



class ZEEdgeDetectionPostEffect : public ZEPostEffect
{
	private:
		ZEPostEffectMultiplePass		MultiplePass;

	public:
		ZEPostEffectMultiplePass		GetPasses();
		void							SetPasses(ZEPostEffectMultiplePass Passes);
		

};

class ZEHDRPostEffect
{
};

class ZEDOFPostProcessor

class ZEPostProcessor
{
	public:

		virtual ZEBlurPostEffect*			CreateBlurPostEffect();
		virtual ZEBlurPostEffect*			CreateBlurPostEffect();
		virtual ZEBlurPostEffect*			CreateBlurPostEffect();
		virtual ZEBlurPostEffect*			CreateBlurPostEffect();
		virtual ZEBlurPostEffect*			CreateBlurPostEffect();
		virtual ZEBlurPostEffect*			CreateBlurPostEffect();
		virtual ZEBlurPostEffect*			CreateBlurPostEffect();
		virtual ZEBlurPostEffect*			CreateBlurPostEffect();
		
		virtual ZEPostEffectResource*		CreatePostEffectResource();
		virtual ZEPostEffectChainResource*	CreatePostEffectChainResource();

		virtual void						SetInput(ZETexture* Texture) = 0;
		virtual void						SetOutput(ZETexture* Texture) = 0;

		virtual void						DirectOutput(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination) = 0;

		virtual void						ApplyUpSample4x(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination) = 0;
		virtual void						ApplyDownSample4x(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination) = 0;

		virtual void						ApplyColorTransform(ZEPostProcessorSource Source, ZEMatrix3x3 Matrix, ZEPostProcessorDestination Destination) = 0;
		virtual void						ApplyInverse(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination) = 0;
		virtual void						ApplyGrayscale(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination) = 0;
	
		virtual void						ApplyBlurH(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination) = 0;
		virtual void						ApplyBlurV(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination) = 0;
	
		virtual void						ApplySharpen(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination) = 0;

		virtual void						ApplyEdgeDetectionH(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination) = 0;
		virtual void						ApplyEdgeDetectionV(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination) = 0;
};

#endif
