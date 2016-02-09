//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMath.cpp
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

#include "ZEMath.h"
#include "ZEPlatform.h"

#include <math.h>

float ZEMath::Sqrt(float Value)
{
	return sqrtf(Value);
}

double ZEMath::Sqrtd(double Value)
{
	return sqrt(Value);
}

float ZEMath::Mod(float Value, float Modulus)
{
	return fmodf(Value, Modulus);
}

double ZEMath::Modd(double Value, double Modulus)
{
	return fmod(Value, Modulus);
}

float ZEMath::Log(float Value)
{
	return logf(Value);
}

double ZEMath::Logd(double Value)
{
	return log(Value);
}

float ZEMath::Log10(float Value)
{
	return log10f(Value);
}

double ZEMath::Log10d(double Value)
{
	return log10(Value);
}

float ZEMath::Power(float Base, float Exponent)
{
	return powf(Base, Exponent);
}

double ZEMath::Powerd(double Base, double Exponent)
{
	return pow(Base, Exponent);
}

float ZEMath::Exp(float Value)
{
	return expf(Value);
}

double ZEMath::Expd(double Value)
{
	return exp(Value);
}

float ZEMath::Floor(float Value)
{
	return floorf(Value);
}

double ZEMath::Floord(double Value)
{
	return floor(Value);
}

float ZEMath::Ceil(float Value)
{
	return ceilf(Value);
}

double ZEMath::Ceild(double Value)
{
	return ceil(Value);
}

float ZEMath::Round(float Value)
{
	return (float)(int)(Value + 0.5f);
}

double ZEMath::Roundd(double Value)
{
	return (double)(int)(Value + 0.5);
}

bool ZEMath::IsValid(float Value)
{
	return Value == Value;
}

bool ZEMath::IsValidd(double Value)
{
	return Value == Value;
}

bool ZEMath::IsPowerOfTwo(ZEUInt Value)
{
	return ((Value & (Value - 1)) != 0)  ? false : true;
}

ZEUInt ZEMath::NextPowerOfTwo(ZEUInt Value)
{
	if(Value <= 1)
		return 1;

	Value = (Value >> 1)  | Value;
	Value = (Value >> 2)  | Value;
	Value = (Value >> 4)  | Value;
	Value = (Value >> 8)  | Value;
	Value = (Value >> 16) | Value;

	return ((Value << 1) + 1) - Value;
}

ZEUInt ZEMath::PreviousPowerOfTwo(ZEUInt Value)
{
	if(Value <= 1)
		return 1;

	Value = (Value >> 1)  | Value;
	Value = (Value >> 2)  | Value;
	Value = (Value >> 4)  | Value;
	Value = (Value >> 8)  | Value;
	Value = (Value >> 16) | Value;

	return Value - (Value >> 1);
}

float ZEMath::Lerp(float A, float B, float Factor)
{
	return A + (B - A) * Factor;
}

double ZEMath::Lerpd(double A, double B, double Factor)
{
	return A + (B - A) * Factor;
}

float ZEMath::CopySign(float Value, float Sign)
{
    #ifdef ZE_PLATFORM_COMPILER_MSVC
        return (float)_copysign(Value, Sign);
    #else
        return copysignf(Value, Sign);
    #endif
}

double ZEMath::CopySignd(double Value, double Sign)
{
#ifdef ZE_PLATFORM_COMPILER_MSVC
	return _copysign(Value, Sign);
#else
	return copysign(Value, Sign);
#endif
}
