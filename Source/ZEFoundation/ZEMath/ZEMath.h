//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMath.h
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
#ifndef __ZE_MATH_H__
#define __ZE_MATH_H__

#include "ZETypes.h"

#define ZE_ZERO_THRESHOLD	 0.00000001 
#define ZE_INT_MIN			-2147483647
#define ZE_INT_MAX			 2147483647
#define ZE_UINT_MAX			 4294967295
#define ZE_FLOAT_MAX		 3.402823466e+38F 
#define ZE_FLOAT_MIN		-3.402823466e+38F
#define ZE_E				 2.71828182845904523536f
#define ZE_LOG2E			 1.44269504088896340736f
#define ZE_LOG10E			 0.434294481903251827651f
#define ZE_LN2				 0.693147180559945309417f
#define ZE_LN10				 2.30258509299404568402f
#define ZE_SQRT2			 1.41421356237309504880f
#define ZE_SQRT1_2			 0.707106781186547524401f

class ZEMath
{
	public:
		static float		Sqrt(float Value);

		template <typename Type>
		static Type			Abs(Type Value);

		template <typename Type>
		static Type			Sign(Type Value);

		static float		Mod(float Value, float Modulus);

		template <typename Type>
		static Type			Min(Type A, Type B);
		template <typename Type>
		static Type			Max(Type A, Type B);

		static float		Log(float Value);
		static float		Log10(float Value);
		static float		Power(float Base, float Exponent);
		static float		Exp(float Value);

		static float		Floor(float Value);
		static float		Ceil(float Value);
		static float		Round(float Value);

		static bool			IsValid(float Value);
		static bool			IsPowerOfTwo(ZEUInt Value);
		
		static ZEUInt		NextPowerOfTwo(ZEUInt Value);
		static ZEUInt		PreviousPowerOfTwo(ZEUInt Value);
		
		static float		CopySign(float Value, float Sign);

		static float		Lerp(float A, float B, float Factor);

		template <typename Type>
		static Type			Circular(Type Value, Type MinValue, Type MaxValue);

		template <typename Type>
		static Type			Clamp(Type Value, Type MinValue, Type MaxValue);
		
		template <typename Type>
		static Type			ClampLower(Type Value, Type MinValue);
		
		template <typename Type>
		static Type			ClampUpper(Type Value, Type MaxValue);
		
		template <typename Type>
		static Type			Saturate(Type Value);
};

template <typename Type>
Type ZEMath::Abs(Type Value)
{
	if (Value < (Type)0)
		return -Value;
	else
		return Value;
}

template <typename Type>
Type ZEMath::Sign(Type Value)
{
	if (Value < (Type)0)
		return (Type)-1;
	else if (Value > (Type)0)
		return (Type)1;
	else
		return (Type)0;
}

template <typename Type>
Type ZEMath::Min(Type A, Type B)
{
	return (A < B ? A : B);
}

template <typename Type>
Type ZEMath::Max(Type A, Type B)
{
	return (A > B ? A : B);
}

template <typename Type>
Type ZEMath::Circular(Type Value, Type MinValue, Type MaxValue)
{
	if (Value < MinValue)
		return MaxValue - (MinValue - Value);
	else if (Value > MaxValue)
		return MinValue + (Value - MaxValue);
	else
		return Value;
}

template <typename Type>
Type ZEMath::Clamp(Type Value, Type MinValue, Type MaxValue)
{
	if (Value < MinValue)
		return MinValue;
	else if (Value > MaxValue)
		return MaxValue;
	else
		return Value;
}

template <typename Type>
Type ZEMath::ClampLower(Type Value, Type MinValue)
{
	if (Value < MinValue)
		return MinValue;
	else
		return Value;
}

template <typename Type>
Type ZEMath::ClampUpper(Type Value, Type MaxValue)
{
	if (Value > MaxValue)
		return MaxValue;
	else
		return Value;
}

template <typename Type>
Type ZEMath::Saturate(Type Value)
{
	if (Value < (Type)0)
		return (Type)0;
	else if (Value > (Type)1)
		return (Type)1;
	else
		return Value;
}

#endif
