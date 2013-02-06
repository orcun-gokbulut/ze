//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureFilters.cpp
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

#include "ZETextureFilters.h"
#include "ZEMath/ZEMath.h"
#include "ZEMath/ZEAngle.h"


// Sinc Function
inline static float Sincf(float x)
{
	if (ZEMath::Abs(x) < 0.0001f) 
	{
		float Square = x * x;
		return 1.0f + Square * (-1.0f / 6.0f + Square * 1.0f / 120.0f);
	}
	else
	{
		return ZEAngle::Sin(x) / x;
	}
}

// Bessel Function
inline static float Bessel0(float x)
{
	float Xh, Sum, Pow, Ds;
	ZEInt K;

	Xh = 0.5f * x;
	Sum = 1.0f;
	Pow = 1.0f;
	K = 0;
	Ds = 1.0;

	while (Ds > Sum * 0.000001f) 
	{
		++K;
		Pow = Pow * (Xh / K);
		Ds = Pow * Pow;
		Sum = Sum + Ds;
	}

	return Sum;
}


ZEFilter::~ZEFilter()
{

}

ZEFilter::ZEFilter()
{
	FilterWidth = 0;
}

float ZEFilter::GetFilterWidth() const
{
	return FilterWidth;
}

float ZEFilter::SampleBox(float Position, float Scale, ZEInt SampleCount) const
{
	float Sum = 0;
	float InverseSampleCount = 1.0f / (float)SampleCount;
	float Increment = 1.0f / (SampleCount + 1);
	
	for(ZESize SampleN = 0; SampleN < (ZESize)SampleCount; SampleN++)
	{
		float SamplePoint = (Position + Increment * ((float)SampleN + 1)) * Scale; 
		Sum += Sample(SamplePoint);
	}

	return Sum * InverseSampleCount;
}

ZEMeanFilter::ZEMeanFilter()
{
	FilterWidth = 0.5f;
}

ZEMeanFilter::ZEMeanFilter(float FilterWidth)
{
	this->FilterWidth = FilterWidth;
}

ZEMeanFilter::~ZEMeanFilter()
{

}

float ZEMeanFilter::Sample(float Position) const
{
	Position = ZEMath::Abs(Position);

	if(Position <= FilterWidth)
		return 1.0f;

	return 0.0f;
}

ZEBilinearFilter::ZEBilinearFilter()
{
	FilterWidth = 1.0f;
}

ZEBilinearFilter::ZEBilinearFilter(float FilterWidth)
{
	this->FilterWidth = FilterWidth;
}

ZEBilinearFilter::~ZEBilinearFilter()
{

}

float ZEBilinearFilter::Sample(float Position) const
{
	Position = ZEMath::Abs(Position);

	if(Position < FilterWidth)
		return 1.0f;

	return 0.0f;
}

ZEGaussianFilter::ZEGaussianFilter()
{
	FilterWidth = 2.0f;
	Alpha = 2.0f;
}

ZEGaussianFilter::ZEGaussianFilter(float FilterWidth, float Alpha)
{
	this->FilterWidth = FilterWidth;
	this->Alpha = Alpha;
}

ZEGaussianFilter::~ZEGaussianFilter()
{

}

float ZEGaussianFilter::GetAlpha() const
{
	return Alpha;
}

float ZEGaussianFilter::Sample(float Position) const
{
	Position = ZEMath::Abs(Position);

	return ZEMath::Exp(-Alpha * Position * Position) * ZEMath::Sqrt(Alpha / ZE_PI);
}

ZEKaiserFilter::ZEKaiserFilter()
{
	FilterWidth = 5.0f;
	Alpha = 4.0f;
	Stretch = 1.0f;
}

ZEKaiserFilter::ZEKaiserFilter(float FilterWidth, float Alpha, float Stretch)
{
	this->FilterWidth = FilterWidth;
	this->Alpha = Alpha;
	this->Stretch = Stretch;
}

ZEKaiserFilter::~ZEKaiserFilter()
{

}

float ZEKaiserFilter::GetAlpha() const
{
	return Alpha;
}

float ZEKaiserFilter::GetStretch() const
{
	return Stretch;
}

float ZEKaiserFilter::Sample(float Position) const
{
	float SincValue = Sincf(ZE_PI * Position * Stretch);
	float T = Position / FilterWidth;
	float T2 = 1 - T * T;

	if(T2 >= 0.0f)
		return SincValue * Bessel0(Alpha * ZEMath::Sqrt(T2)) / Bessel0(Alpha);
	else
		return 0.0f;
}



ZEFilterCoefficients::ZEFilterCoefficients(const ZEFilter& Filter, ZEUInt SourceLength, ZEUInt TargetLength, ZEInt SampleCount)
{
	zeDebugCheck(SampleCount == 0, "SampleCount cannot be zero");
	zeDebugCheck(SourceLength == 0, "SourceLength cannot be zero");
	zeDebugCheck(TargetLength == 0, "TargetLength cannot be zero");

	float PixelSize = 1.0f / (float)SourceLength;

	float Scale = (float)TargetLength / (float)SourceLength;
	//float InverseScale = 1.0f / Scale;
	
	// Upsampling Case
	//if (Scale > 1)
	//{
	//	SampleCount = 1;
	//	Scale = 1;
	//}

	//Width = Filter.GetFilterWidth() * InverseScale;
	//WindowSize = (ZEUInt)ZEMath::Ceil(Width * 2.0f);
	WindowSize = Filter.GetFilterWidth();
	
	/*
	if (SourceLength == TargetLength)
	{
		// Ensure that windows size is odd
		WindowSize = WindowSize >> 1;
		WindowSize = WindowSize << 1;
		WindowSize += 1;
	}
	*/

	Coefficients.Resize(WindowSize);
	Center = (float)WindowSize / 2.0f;
	
	float Total = 0.0f;
	float PixelCoord = -1.0f * ZEMath::Floor(Center);
	PixelCoord += (WindowSize % 2 == 0) ? 0.5f : 0.0f;

	for(ZESize I = 0; I < WindowSize; I++)
	{
		const float Weight = Filter.SampleBox(I - Center, Scale, SampleCount);
		Coefficients[I] = ZEVector2(Weight, PixelCoord * PixelSize);
		Total += Weight;
		PixelCoord += 1.0f;
	}

	// Normalize the weight of the WindowSize
	float InverseTotal = 1.0f / Total;
	for(ZESize I = 0; I < WindowSize; I++)
		Coefficients[I].x *= InverseTotal;
}

ZEFilterCoefficients::~ZEFilterCoefficients()
{

}

const ZEVector2* ZEFilterCoefficients::GetCoefficients() const
{
	return Coefficients.GetConstCArray();
}

ZEUInt ZEFilterCoefficients::GetWindowSize() const
{
	return WindowSize;
}

float ZEFilterCoefficients::GetWidth() const
{
	return Width;
}

float ZEFilterCoefficients::GetCenter() const
{
	return Center;
}
