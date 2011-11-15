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

float ZEAngle::ConvertToDegree(float Radian)
{
	return (Radian * 180/ZE_PI);
}

float ZEAngle::ConvertToRadian(float Degree)
{
	return (Degree * ZE_PI/180);
}

float ZEAngle::RangeDegree(float Angle)
{
	float ModAngle = fmod(Angle, 360);
	if(ModAngle > 180)
		return ModAngle - 360;
	else if(ModAngle < -180)
		return ModAngle + 360;

	return ModAngle;
}

float ZEAngle::RangeRadian(float Angle)
{
	float ModAngle = fmod(Angle, ZE_PIx2);
	if(ModAngle > ZE_PI)
		return ModAngle - ZE_PIx2;
	else if(ModAngle < -ZE_PI)
		return ModAngle + ZE_PIx2;

	return ModAngle;
}

float ZEAngle::DistanceDegree(float Angle1, float Angle2)
{
	return RangeDegree(fabs(Angle1 - Angle2));
}

float ZEAngle::DistanceRadian(float Angle1, float Angle2)
{
	return RangeRadian(fabs(Angle1 - Angle2));
}
