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

#include <math.h>

float ZEMath::Sqrt(float Value)
{
	return sqrtf(Value);
}

float ZEMath::Abs(float Value)
{
	if (Value < 0.0f)
		return -Value;
	else
		return Value;
}

float ZEMath::Mod(float Value, float Modulus)
{
	return fmodf(Value, Modulus);
}

float ZEMath::Sign(float Value)
{
	if (Value == 0.0f)
		return 0.0f;
	else if (Value < 0.0f)
		return -1.0f;
	else
		return 1.0f;
}

float ZEMath::Min(float A, float B)
{
	return (A < B ? A : B);
}

float ZEMath::Max(float A, float B)
{
	return (A > B ? A : B);
}

float ZEMath::Log(float Value)
{
	return logf(Value);
}

float ZEMath::Log10(float Value)
{
	return log10f(Value);
}

float ZEMath::Power(float Base, float Exponent)
{
	return powf(Base, Exponent);
}

float ZEMath::Exp(float Exponent)
{
	return expf(Exponent);
}

float ZEMath::Floor(float Value)
{
	return floorf(Value);
}

float ZEMath::Ceil(float Value)
{
	return ceilf(Value);
}

float ZEMath::Round(float Value)
{
	return (float)(int)(Value + 0.5f);
}


float ZEMath::Lerp(float A, float B, float Factor)
{
	return A + (B - A) * Factor;
}

float ZEMath::CopySign(float Value, float Sign)
{
	return _copysign(Value, Sign);
}

float ZEMath::Clamp(float A, float MinValue, float MaxValue)
{
	if (A > MaxValue)
		return MaxValue;
	else if (A < MinValue)
		return MinValue;
	else
		return A;
}

float ZEMath::ClampLower(float Value, float MinValue)
{
	if (Value < MinValue)
		return MinValue;
	else
		return Value;
}

float ZEMath::ClampUpper(float Value, float MaxValue)
{
	if (Value > MaxValue)
		return MaxValue;
	else
		return Value;
}

float ZEMath::Saturate(float Value)
{
	if (Value > 1.0f)
		return 1.0f;
	else if (Value < 0.0f)
		return 0.0f;
	else
		return Value;
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
