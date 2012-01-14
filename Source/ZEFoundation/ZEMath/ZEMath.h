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

#define ZE_ZERO_THRESHOLD	0.00000001 
#define ZE_INT_MIN			-2147483647
#define ZE_INT_MAX			2147483647
#define ZE_UINT_MAX			4294967295
#define ZE_FLOAT_MAX		3.402823466e+38F 
#define ZE_FLOAT_MIN		-3.402823466e+38F

class ZEMath
{
	public:
		static float		Sqrt(float Value);
		static float		Abs(float Value);
		static int			Abs(int Value);
		static float		Sign(float Value);
		static int			Sign(int Value);

		static float		Min(float A, float B);
		static float		Max(float A, float B);
		static int			Min(int A, int B);
		static int			Max(int A, int B);

		static float		Loge(float Value);
		static float		Log10(float Value);
		static float		Power(float Base, float Exponent);
		static float		Exp(float Value);

		static float		Lerp(float A, float B, float Factor);

		static float		Clamp(float A, float MinValue, float MaxValue);
		static float		ClampLower(float Value, float MinValue);
		static float		ClampUpper(float Value, float MaxValue);
		static float		Saturate(float Value);
};

#endif




