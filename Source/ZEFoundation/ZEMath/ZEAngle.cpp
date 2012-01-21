//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEAngle.cpp
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

#include "ZEAngle.h"
#include <math.h>
#include "ZEMath/ZEMath.h"

float ZEAngle::Range(float Angle)
{
	float ModAngle = ZEMath::Mod(Angle, ZE_PIx2);
	if(ModAngle > ZE_PI)
		return ModAngle - ZE_PIx2;
	else if(ModAngle < -ZE_PI)
		return ModAngle + ZE_PIx2;

	return ModAngle;
}

float ZEAngle::Distance(float Angle1, float Angle2)
{
	return Range(ZEMath::Abs(Angle1 - Angle2));
}

float ZEAngle::Cos(float Value)
{
	return cosf(Value);
}

float ZEAngle::Sin(float Value)
{
	return sinf(Value);
}

float ZEAngle::Tan(float Value)
{
	return tanf(Value);
}

float ZEAngle::Cot(float Value)
{
	return 1.0f / tanf(Value);
}

float ZEAngle::ArcSin(float Value)
{
	return asinf(Value);
}

float ZEAngle::ArcCos(float Value)
{
	return acosf(Value);
}

float ZEAngle::ArcTan(float Value)
{
	return atanf(Value);
}

float ZEAngle::ArcTan2(float x, float y)
{
	return atan2f(x, y);
}

float ZEAngle::ToRadian(float Angle)
{
	return (Angle * ZE_PI/180);
}

float ZEAngle::ToDegree(float Angle)
{
	return (Angle * 180/ZE_PI);
}

ZEVector2 ZEAngle::ToVector(float Angle)
{
	ZEVector2 Output;
	Output.x = sinf(Angle);
	Output.y = cosf(Angle);

	return Output;
}
