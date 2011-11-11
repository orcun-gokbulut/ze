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

// ZEFilter Base Class
class ZEFilter
{
	private:
		const float		FilterWidth;

	public:
						ZEFilter(float Width);
		virtual			~ZEFilter();

		float			GetFilterWidth() const;
		float			SampleBox(float x, float Scale, int Samples) const;
		virtual float	Process(float x) const = 0;
};

// Box(Mean) ZEFilter
class BoxFilter : public ZEFilter
{
	public:
				BoxFilter();
				BoxFilter(float Width);
				~BoxFilter();

		float	Process(float x) const;
};

// Triangle(Bilinear) ZEFilter
class TriangleFilter : public ZEFilter
{
	public:
				TriangleFilter();
				TriangleFilter(float Width);
				~TriangleFilter();

		float	Process(float x) const;
};

// Cubic(Hermite) ZEFilter
class CubicFilter : public ZEFilter
{
	public:
				CubicFilter();
				CubicFilter(float Width);
				~CubicFilter();

		float	Process(float x) const;
};

// Gaussian ZEFilter
class GaussianFilter : public ZEFilter
{
	private:
		float Alpha;

	public:
				GaussianFilter();
				GaussianFilter(float Width);
				~GaussianFilter();

		float	Process(float x) const;
};

// Quadratic ZEFilter
class QuadraticFilter : public ZEFilter
{
	public:
				QuadraticFilter();
				QuadraticFilter(float Width);
				~QuadraticFilter();

		float	Process(float x) const;
};

// Kaiser ZEFilter
class KaiserFilter : public ZEFilter
{
	private:
		float Alpha;
		float Stretch;

	public:
				KaiserFilter(float Width);
				~KaiserFilter();

		virtual float	Process(float x) const;
};

// Mitchell ZEFilter
class MitchellFilter : public ZEFilter
{
	private:
		float P0, P2, P3;
		float Q0, Q1, Q2, Q3;

	public:
				MitchellFilter(float B, float C);
				~MitchellFilter();

		float	Process(float x) const;
};


class ZEVector4;

// 1D Kernel
class ZEKernel
{
	private:
		int				KernelWindowSize;
		float			KernelWidth;
		ZEVector4*		KernelWeights;
		float			Center;

	public:
					ZEKernel(const ZEFilter* Filt, unsigned int SrcLength, unsigned int DestLength, int Samples, float PixelSize);
					~ZEKernel();

		ZEVector4*	GetKernel() const;
		int			GetKernelWindowSize() const;
		float		GetKernelWidth() const;
		float		GetKernelCenter() const;
	
		// Also add precalculated filters
};

#endif /* __ZE_D3D9_FILTERS_H__ */
