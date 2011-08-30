//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9TextureResizeFilters.h
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
#ifndef __ZE_D3D9_FILTERS_H__
#define __ZE_D3D9_FILTERS_H__

// Common Helper Functions
float Sincf(float x);	// Sinc Function
float Bessel0(float x);  // Bessel Function

// Filter Base Class
class Filter
{
	private:
		const float		FilterWidth;

	public:
						Filter(float Width);
		virtual			~Filter();

		float			GetFilterWidth() const;
		float			SampleBox(float x, float Scale, int Samples) const;
		virtual float	Process(float x) const = 0;
};

// Box(Mean) Filter
class BoxFilter : public Filter
{
	public:
				BoxFilter();
				BoxFilter(float Width);
				~BoxFilter();

		float	Process(float x) const;
};

// Triangle(Bilinear) Filter
class TriangleFilter : public Filter
{
	public:
				TriangleFilter();
				TriangleFilter(float Width);
				~TriangleFilter();

		float	Process(float x) const;
};

// Cubic(Hermite) Filter
class CubicFilter : public Filter
{
	public:
				CubicFilter();
				CubicFilter(float Width);
				~CubicFilter();

		float	Process(float x) const;
};

// Gaussian Filter
class GaussianFilter : public Filter
{
	private:
		float Alpha;

	public:
				GaussianFilter();
				GaussianFilter(float Width);
				~GaussianFilter();

		float	Process(float x) const;
};

// Quadratic Filter
class QuadraticFilter : public Filter
{
	public:
				QuadraticFilter();
				QuadraticFilter(float Width);
				~QuadraticFilter();

		float	Process(float x) const;
};

// Kaiser Filter
class KaiserFilter : public Filter
{
	private:
		float Alpha;
		float Stretch;

	public:
				KaiserFilter(float Width);
				~KaiserFilter();

		virtual float	Process(float x) const;
};

// Mitchell Filter
class MitchellFilter : public Filter
{
	private:
		float P0, P2, P3;
		float Q0, Q1, Q2, Q3;

	public:
				MitchellFilter(float B, float C);
				~MitchellFilter();

		float	Process(float x) const;
};

// 1D kernel which is used to precompute filter weights
class Kernel1D
{
	private:
		int		KernelWindowSize;
		float	KernelWidth;
		float*	KernelWeights;

	public:
		// Construct the kernel
				Kernel1D(Filter& Filt, int InverseScale, int Samples = 32);
				~Kernel1D();

		float	GetValue(unsigned int Column) const;
};

// 1D Polyphase kernel
class PolyPhaseKernel
{
	private:
		int				KernelWindowSize;
		float			KernelWidth;
		float*			KernelWeights;
		float			Center;

	public:
		// Construct the kernel
				PolyPhaseKernel(const Filter* Filt, unsigned int SrcLength, unsigned int DestLength, int Samples = 32);
				~PolyPhaseKernel();

		float*	GetKernel();
		int		GetKernelWindowSize();
		float	GetKernelWidth();
		float	GetKernelCenter();
		float	GetValue(unsigned int x) const;
};

#endif /* __ZE_D3D9_FILTERS_H__ */
