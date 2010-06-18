//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETrigonometry.cpp
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

#include "ZEtrigonometry.h"
#include <math.h>
#include "ZEdefinitions.h"
float SinTable[451];

void InitTrigTables()
{
	float Value;
	for(int I = 0; I < 181; I++)
	{
		Value = sinf((float)I * ZE_PI/180);//12.02.2010 tarihinde PI ==> ZE_PI düzeltmesi yapıldı.
		SinTable[I] = Value;
		SinTable[I + 180] = -Value;
	}

	for(int I = 360; I < 451; I++)
		SinTable[I] = sinf((float)I * ZE_PI/180);//12.02.2010 tarihinde PI ==> ZE_PI düzeltmesi yapıldı.
}

float FastSin(float Angle)
{
	InitTrigTables();//Bu satır 12.02.2010 tarihinde eklendi.
	Angle = fmodf(Angle, 360);
	
	if (Angle < 0) Angle = Angle + 360;
	
	int IntAngle = (int)Angle;
	
	return SinTable[IntAngle] + (SinTable[IntAngle + 1] - SinTable[IntAngle]) * (Angle - IntAngle);
}

float FastCos(float Angle)
{
	InitTrigTables();//Bu satır 12.02.2010 tarihinde eklendi.
	Angle = fmodf(Angle, 360);

	if (Angle < 0) 
		Angle = Angle + 450;
	else
		Angle = Angle + 90;	

	int IntAngle = (int)Angle;

	return SinTable[IntAngle] + (SinTable[IntAngle + 1] - SinTable[IntAngle]) * (Angle - IntAngle);
}


float FastTan(float Angle)
{
	return FastSin(Angle) / FastCos(Angle);
}




