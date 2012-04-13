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

#include "ZETypes.h"

float Sincf(float x);	
float Bessel0(float x); 
class ZEVector4;

class ZEFilter
{
	private:
		const float		FilterWidth;

	public:
						ZEFilter(float Width);
		virtual			~ZEFilter();

		float			GetFilterWidth() const;
		float			SampleBox(float x, float Scale, ZEInt Samples) const;
		virtual float	Process(float x) const = 0;
};


class BoxFilter : public ZEFilter
{
	public:
						BoxFilter();
						BoxFilter(float Width);
						~BoxFilter();

		virtual float	Process(float x) const;
};


class TriangleFilter : public ZEFilter
{
	public:
						TriangleFilter();
						TriangleFilter(float Width);
		virtual			~TriangleFilter();

		virtual float	Process(float x) const;
};


class CubicFilter : public ZEFilter
{
	public:
						CubicFilter();
						CubicFilter(float Width);
		virtual			~CubicFilter();

		virtual float	Process(float x) const;
};


class GaussianFilter : public ZEFilter
{
	private:
		float			Alpha;

	public:
						GaussianFilter();
						GaussianFilter(float Width);
						GaussianFilter(float Width, float Alpha = 2);
		virtual			~GaussianFilter();

		virtual float	Process(float x) const;
};


class QuadraticFilter : public ZEFilter
{
	public:
						QuadraticFilter();
						QuadraticFilter(float Width);
		virtual			~QuadraticFilter();

		virtual float	Process(float x) const;
};


class KaiserFilter : public ZEFilter
{
	private:
		float 			Alpha;
		float 			Stretch;

	public:
						KaiserFilter(float Width);
		virtual			~KaiserFilter();

		virtual float	Process(float x) const;
};


class MitchellFilter : public ZEFilter
{
	private:
		float 			P0, P2, P3;
		float 			Q0, Q1, Q2, Q3;

	public:
						MitchellFilter(float B, float C);
		virtual			~MitchellFilter();

		virtual float	Process(float x) const;
};


class ZEKernel
{
	protected:
		ZESize			KernelWindowSize;
		float			KernelWidth;
		ZEVector4*		KernelWeights;
		float			Center;

	public:
						ZEKernel() {};
						ZEKernel(const ZEFilter* Filt, ZEUInt SrcLength, ZEUInt DestLength, ZEInt Samples, float PixelSize);
		virtual			~ZEKernel();

		ZEVector4*		GetKernel() const;
		ZESize			GetKernelWindowSize() const;
		float			GetKernelWidth() const;
		float			GetKernelCenter() const;
};

#endif
