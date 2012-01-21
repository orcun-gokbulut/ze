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

#define ZE_ZERO_THRESHOLD	0.00000001 
#define ZE_INT_MIN			-2147483647
#define ZE_INT_MAX			2147483647
#define ZE_UINT_MAX			4294967295
#define ZE_FLOAT_MAX		3.402823466e+38F 
#define ZE_FLOAT_MIN		-3.402823466e+38F
#define ZE_E				2.71828182845904523536f
#define ZE_LOG2E			1.44269504088896340736f
#define ZE_LOG10E			0.434294481903251827651f
#define ZE_LN2				0.693147180559945309417f
#define ZE_LN10				2.30258509299404568402f
#define ZE_SQRT2			1.41421356237309504880f
#define ZE_SQRT1_2			0.707106781186547524401f

class ZEMath
{
	public:
		static float		Sqrt(float Value);
		static float		Abs(float Value);
		static ZEInt		Abs(ZEInt Value);
		static float		Sign(float Value);
		static ZEInt		Sign(ZEInt Value);
		static float		Mod(float Value, float Modulus);

		static float		Min(float A, float B);
		static float		Max(float A, float B);
		static ZEInt		Min(ZEInt A, ZEInt B);
		static ZEInt		Max(ZEInt A, ZEInt B);

		static float		Log(float Value);
		static float		Log10(float Value);
		static float		Power(float Base, float Exponent);
		static float		Exp(float Value);

		static float		Floor(float Value);
		static float		Ceil(float Value);
		static float		Round(float Value);

		static float		CopySign(float Value, float Sign);

		static float		Lerp(float A, float B, float Factor);

		static float		Clamp(float A, float MinValue, float MaxValue);
		static float		ClampLower(float Value, float MinValue);
		static float		ClampUpper(float Value, float MaxValue);
		static float		Saturate(float Value);
};

#endif




