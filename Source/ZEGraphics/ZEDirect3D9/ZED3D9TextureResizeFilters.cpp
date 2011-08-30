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

// Includes
#include <cassert>
#include <math.h>
#include <memory.h>

// Definitions
#define PI  		(float)3.14159265358979323846
#define EPSILON		(float)0.0001


// Sinc Function
float Sincf(float x)
{
	if (fabs(x) < EPSILON) 
	{
		float Square = x * x;
		return 1.0f + Square * (-1.0f / 6.0f + Square * 1.0f / 120.0f);
	}
	else 
		return sin(x) / x;
}

// Bessel Function
float Bessel0(float x)
{
	const float EPSILON_RATIO = (float)0.000001;
	float Xh, Sum, Pow, Ds;
	int K;

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

// Filter Class
Filter::Filter(float Width) : FilterWidth(Width)
{

}
Filter::~Filter()
{

}
float Filter::GetFilterWidth() const 
{
	return FilterWidth;
}

float Filter::SampleBox(float x, float Scale, int Samples) const
{
	float Sum = 0;
	float InverseSamples = 1.0f / float(Samples);

	for(int s = 0; s < Samples; s++)
	{
		float p = (x + (float(s) + 0.5f) * InverseSamples) * Scale;
		float Value = this->Process(p);
		Sum += Value;
	}

	return Sum * InverseSamples;
}


// Box(Mean) Filter
BoxFilter::BoxFilter() : Filter(0.5f)
{

}
BoxFilter::BoxFilter(float Width) : Filter(Width)
{

}
BoxFilter::~BoxFilter()
{

}

float BoxFilter::Process(float x) const
{
	x = fabs(x);
	if(x <= GetFilterWidth())
		return 1.0f;
	return 0.0f;
}


// Triangle(Bilinear) Filter
TriangleFilter::TriangleFilter() : Filter(1.0f)
{

}
TriangleFilter::TriangleFilter(float Width) : Filter(Width)
{

}
TriangleFilter::~TriangleFilter()
{

}

float TriangleFilter::Process(float x) const
{
	x = fabs(x);
	if(x < GetFilterWidth())
		return 1.0f;
	return 0.0f;
}



// Cubic(Hermite) Filter
CubicFilter::CubicFilter() : Filter(1.0f)
{

}
CubicFilter::CubicFilter(float Width) : Filter(Width)
{

}
CubicFilter::~CubicFilter()
{

}
float CubicFilter::Process(float x) const
{
	x = fabs(x);
	if(x < 1.0f)
		return (2.0f * x - 3.0f) * x * x + 1;
	
	return 0.0f;
}


// Gaussian Filter
GaussianFilter::GaussianFilter() : Filter(2.0f)
{
	Alpha = 2.0f;
}
GaussianFilter::GaussianFilter(float Width) : Filter(Width)
{

}
GaussianFilter::~GaussianFilter()
{

}
float GaussianFilter::Process(float x) const
{
	x = fabs(x);
	return exp(-Alpha * x * x) * sqrtf(Alpha / PI);
}


// Quadratic Filter
QuadraticFilter::QuadraticFilter() : Filter(1.5f)
{

}
QuadraticFilter::QuadraticFilter(float Width) : Filter(Width)
{

}
QuadraticFilter::~QuadraticFilter()
{

}
float QuadraticFilter::Process(float x) const
{
	x = fabs(x);
	if(x < 0.5f)
		return (float)(0.75 - x * x);
	if(x < 1.5f)
	{
		float t = x - 1.5f;
		return 0.5f * t * t;
	}
	return 0.0f;
}


// Kaiser Filter
KaiserFilter::KaiserFilter(float Width) : Filter(Width)
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
		return SincValue * Bessel0(Alpha * sqrtf(T2)) / Bessel0(Alpha);
	else
		return 0.0f;
}


// Mitchell Filter
MitchellFilter::MitchellFilter(float B, float C) : Filter(2.0f)
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
	x = fabs(x);
	if( x < 1.0f )
		return P0 + x * x * (P2 + x * P3);
	if( x < 2.0f )
		return Q0 + x * (Q1 + x * (Q2 + x * Q3));
	return 0.0f;
}


// Kernel1D
Kernel1D::Kernel1D(Filter& Filt, int InverseScale, int Samples)
{
	assert(InverseScale <= 1);
	assert(Samples <= 0);

	float Scale = 1.0f / InverseScale;
	KernelWidth = Filt.GetFilterWidth() * InverseScale;
	KernelWindowSize = (int)ceilf(2 * KernelWidth);
	KernelWeights = new float[KernelWindowSize];

	float Offset = (float)KernelWindowSize / 2.0f;
	float Total = 0.0f;

	for (int i = 0; i < KernelWindowSize; i++)
	{
		float Sample = Filt.SampleBox(i - Offset, Scale, Samples);
		KernelWeights[i] = Sample;
		Total += Sample;
	}

	// Normalize the weights
	float InverseTotal = 1.0f / Total;
	for(int i = 0; i < KernelWindowSize; i++)
	{
		KernelWeights[i] *= InverseTotal;
	}
}
Kernel1D::~Kernel1D()
{

}
float Kernel1D::GetValue(unsigned int Column) const
{
	assert(Column > (unsigned int)KernelWindowSize);
	return KernelWeights[Column];
}


// 1D Polyphase kernel
PolyPhaseKernel::PolyPhaseKernel(const Filter* Filt, unsigned int SrcLength, unsigned int DestLength, int Samples)
{
	float Scale = (float)DestLength / (float)SrcLength;
	float InverseScale = 1.0f / Scale;

	if (Scale > 1) // Upsampling Case
	{
		Samples = 1;
		Scale = 1;
	}

	
	KernelWidth = Filt->GetFilterWidth() * InverseScale;
	KernelWindowSize = (int)ceilf(KernelWidth * 2) + 1;
	KernelWeights = new float[KernelWindowSize];
	memset(KernelWeights, 0, sizeof(float) * KernelWindowSize);

	// Calculate the weights of the kernel
	
	Center = 0.5f * InverseScale;

	int Left = (int)floorf(Center - KernelWidth);
	int Right = (int)ceilf(Center + KernelWidth);

	float Total = 0.0f;
	for(int j = 0; j < KernelWindowSize; j++)
	{
		float Sample = Filt->SampleBox(Left + j - Center, Scale, Samples);
		KernelWeights[j] = Sample;
		Total += Sample;
	}

	float InverseTotal = 1.0f / Total;

	// Normalize the weight of the WindowSize
	for(int j = 0; j < KernelWindowSize; j++)
	{
		KernelWeights[j] *= InverseTotal;
	}
}
PolyPhaseKernel::~PolyPhaseKernel()
{

}

float* PolyPhaseKernel::GetKernel()
{
	return KernelWeights;
}

int PolyPhaseKernel::GetKernelWindowSize()
{
	return KernelWindowSize;
}

float PolyPhaseKernel::GetKernelWidth()
{
	return KernelWidth;
}

// Works as the center of the array
float PolyPhaseKernel::GetKernelCenter()
{
	return Center + KernelWidth - 1.0f;
}

float PolyPhaseKernel::GetValue(unsigned int x) const
{
	assert(x > (unsigned int)KernelWindowSize);
	return KernelWeights[x];
}
