//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEAngle.h
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
#ifndef __ZE_ANGLE_H__
#define __ZE_ANGLE_H__

#define ZE_ZERO_TRESHOLD		0.00000001 

#define ZE_PIx2				6.28318530717958647692f
#define ZE_E				2.71828182845904523536f
#define ZE_LOG2E			1.44269504088896340736f
#define ZE_LOG10E			0.434294481903251827651f
#define ZE_LN2				0.693147180559945309417f
#define ZE_LN10				2.30258509299404568402f
#define ZE_PI				3.14159265358979323846f		// 180  Degree
#define ZE_PI_2				1.57079632679489661923f		// 90   Degree
#define ZE_PI_4				0.78539816339744830962f		// 45   Degree 
#define ZE_PI_8				0.39269908169872415481f		// 22.5 Degree 
#define ZE_PI_12			0.26179938779914943654f		// 15	Degree 
#define ZE_1_PI				0.318309886183790671538f
#define ZE_2_PI				0.636619772367581343076f
#define ZE_2_SQRTPI			1.12837916709551257390f
#define ZE_SQRT2			1.41421356237309504880f
#define ZE_SQRT1_2			0.707106781186547524401f

class ZEAngle
{
	public:
		static float ConvertToDegree(float Radian);
		static float ConvertToRadian(float Degree);
		static float RangeDegree(float Angle);
		static float RangeRadian(float Angle);
		static float DistanceDegree(float Angle1, float Angle2);
		static float DistanceRadian(float Angle1, float Angle2);
};


#endif




