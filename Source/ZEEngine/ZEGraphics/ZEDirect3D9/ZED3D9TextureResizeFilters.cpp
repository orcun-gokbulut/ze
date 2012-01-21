//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9TextureResizeFilters.cpp
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

#include "ZED3D9TextureResizeFilters.h"
#include "ZECore\ZEConsole.h"
#include "ZEMath\ZEVector.h"
#include "ZEMath/ZEMath.h"
#include "ZEMath/ZEAngle.h"

// Includes
#include <cassert>

#include <memory.h>

// Definitions
#define PI  		(float)3.14159265358979323846
#define EPSILON		(float)0.0001


// Sinc Function
float Sincf(float x)
{
	if (ZEMath::Abs(x) < EPSILON) 
	{
		float Square = x * x;
		return 1.0f + Square * (-1.0f / 6.0f + Square * 1.0f / 120.0f);
	}
	else 
		return ZEAngle::Sin(x) / x;
}

// Bessel Function
float Bessel0(float x)
{
	const float EPSILON_RATIO = (float)0.000001;
	float Xh, Sum, Pow, Ds;
	ZEInt K;

	Xh = 0.5f * x;
	Sum = 1.0f;
	Pow = 1.0f;
	K = 0;
	Ds = 1.0;

	while (Ds > Sum * EPSILON_RATIO) 
	{
		++K;
		Pow = Pow * (Xh / K);
		Ds = Pow * Pow;
		Sum = Sum + Ds;
	}

	return Sum;
}

// ZEFilter Class
ZEFilter::ZEFilter(float Width) : FilterWidth(Width)
{

}

ZEFilter::~ZEFilter()
{

}

float ZEFilter::GetFilterWidth() const 
{
	return FilterWidth;
}

float ZEFilter::SampleBox(float x, float Scale, ZEInt Samples) const
{
	float Sum = 0;
	float InverseSamples = 1.0f / (float)Samples;
	float Increment = 1.0f / (Samples + 1);
	

	for(ZEInt s = 0; s < Samples; s++)
	{
		// this equation is wrong
		//float p = (x + (float(s) + 0.5f) * InverseSamples) * Scale;
		float p = (x + Increment * ((float)s + 1)) * Scale; 

		float Value = this->Process(p);
		Sum += Value;
	}

	return Sum * InverseSamples;
}


// Box(Mean) ZEFilter
BoxFilter::BoxFilter() : ZEFilter(0.5f)
{

}

BoxFilter::BoxFilter(float Width) : ZEFilter(Width)
{

}

BoxFilter::~BoxFilter()
{

}

float BoxFilter::Process(float x) const
{
	x = ZEMath::Abs(x);
	if(x <= GetFilterWidth())
		return 1.0f;
	return 0.0f;
}


// Triangle(Bilinear) ZEFilter
TriangleFilter::TriangleFilter() : ZEFilter(1.0f)
{

}

TriangleFilter::TriangleFilter(float Width) : ZEFilter(Width)
{

}

TriangleFilter::~TriangleFilter()
{

}

float TriangleFilter::Process(float x) const
{
	x = ZEMath::Abs(x);
	if(x < GetFilterWidth())
		return 1.0f;
	return 0.0f;
}



// Cubic(Hermite) ZEFilter
CubicFilter::CubicFilter() : ZEFilter(1.0f)
{

}

CubicFilter::CubicFilter(float Width) : ZEFilter(Width)
{

}

CubicFilter::~CubicFilter()
{

}

float CubicFilter::Process(float x) const
{
	x = ZEMath::Abs(x);
	if(x < 1.0f)
		return (2.0f * x - 3.0f) * x * x + 1;
	
	return 0.0f;
}


// Gaussian ZEFilter
GaussianFilter::GaussianFilter() : ZEFilter(2.0f)
{
	Alpha = 2.0f;
}

GaussianFilter::GaussianFilter(float Width) : ZEFilter(Width)
{

}

GaussianFilter::GaussianFilter(float Width, float Alpha) : ZEFilter(Width)
{
	this->Alpha = Alpha;
}

GaussianFilter::~GaussianFilter()
{

}

float GaussianFilter::Process(float x) const
{
	x = ZEMath::Abs(x);
	return ZEMath::Exp(-Alpha * x * x) * ZEMath::Sqrt(Alpha / PI);
}


// Quadratic ZEFilter
QuadraticFilter::QuadraticFilter() : ZEFilter(1.5f)
{

}
QuadraticFilter::QuadraticFilter(float Width) : ZEFilter(Width)
{

}

QuadraticFilter::~QuadraticFilter()
{

}

float QuadraticFilter::Process(float x) const
{
	x = ZEMath::Abs(x);
	if(x < 0.5f)
		return (float)(0.75 - x * x);
	if(x < 1.5f)
	{
		float t = x - 1.5f;
		return 0.5f * t * t;
	}
	return 0.0f;
}


// Kaiser ZEFilter
KaiserFilter::KaiserFilter(float Width) : ZEFilter(Width)
{
	Alpha = 4.0f; 
	Stretch = 1.0f;
}

KaiserFilter::~KaiserFilter()
{

}

float KaiserFilter::Process(float x) const
{
	float SincValue = Sincf(PI * x * Stretch);
	float T = x / GetFilterWidth();
	float T2 = 1 - T * T;

	if(T2 >= 0.0f)
		return SincValue * Bessel0(Alpha * ZEMath::Sqrt(T2)) / Bessel0(Alpha);
	else
		return 0.0f;
}


// Mitchell ZEFilter
MitchellFilter::MitchellFilter(float B, float C) : ZEFilter(2.0f)
{
	P0 = (6.0f -  2.0f * B) / 6.0f;
	P2 = (-18.0f + 12.0f * B + 6.0f * C) / 6.0f;
	P3 = (12.0f - 9.0f * B - 6.0f * C) / 6.0f;
	Q0 = (8.0f * B + 24.0f * C) / 6.0f;
	Q1 = (-12.0f * B - 48.0f * C) / 6.0f;
	Q2 = (6.0f * B + 30.0f * C) / 6.0f;
	Q3 = (-B - 6.0f * C) / 6.0f;
}

MitchellFilter::~MitchellFilter()
{

}

float MitchellFilter::Process(float x) const
{
	x = ZEMath::Abs(x);
	if( x < 1.0f )
		return P0 + x * x * (P2 + x * P3);
	if( x < 2.0f )
		return Q0 + x * (Q1 + x * (Q2 + x * Q3));
	return 0.0f;
}

// 1D Polyphase kernel
ZEKernel::ZEKernel(const ZEFilter* Filt, ZEUInt SrcLength, ZEUInt DestLength, ZEInt Samples, float PixelSize)
{
	float Scale = (float)DestLength / (float)SrcLength;
	float InverseScale = 1.0f / Scale;

	// Upsampling Case
	if (Scale > 1) 
	{
		Samples = 1;
		Scale = 1;
	}

	KernelWidth = Filt->GetFilterWidth() * InverseScale;
	KernelWindowSize = (ZEInt)ZEMath::Ceil(KernelWidth * 2);
	// allocation is fixed since we pass it to graphics device
	KernelWeights = new ZEVector4[KernelWindowSize];
	memset(KernelWeights, 0, sizeof(ZEVector4) * KernelWindowSize);

	Center = float(KernelWindowSize) / 2.0f;

	float PixelCoord = -1.0f * (ZEMath::Floor(Center) - 0.5f);
	float Total = 0.0f;
	for(ZEInt I = 0; I < KernelWindowSize; I++)
	{
		const float Sample = Filt->SampleBox(I - Center, Scale, Samples);
		KernelWeights[I] = ZEVector4(Sample, PixelCoord * PixelSize, 0.0f, 0.0f);
		Total += Sample;
		PixelCoord += 1.0f;
	}

	float InverseTotal = 1.0f / Total;
	// Normalize the weight of the WindowSize
	for(ZEInt I = 0; I < KernelWindowSize; I++)
	{
		KernelWeights[I].x *= InverseTotal;
	}
}
ZEKernel::~ZEKernel()
{
	// Delete weights
	if (KernelWeights)
	{
		delete KernelWeights;
		KernelWeights = NULL;
	}

}

ZEVector4* ZEKernel::GetKernel() const
{
	return KernelWeights;
}

ZEInt ZEKernel::GetKernelWindowSize() const
{
	return KernelWindowSize;
}

float ZEKernel::GetKernelWidth() const
{
	return KernelWidth;
}

// Works as the center of the array
float ZEKernel::GetKernelCenter() const
{
	return KernelWidth;
}
