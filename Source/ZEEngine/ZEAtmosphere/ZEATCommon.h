//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEATCommon.h
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

#include "ZETypes.h"
#include "ZEMath/ZEVector.h"

#define EARTH_RADIUS			6360000.0f
#define ATMOSPHERE_HEIGHT		80000.0f
#define TOTAL_RADIUS			(EARTH_RADIUS + ATMOSPHERE_HEIGHT)
#define RAYLEIGH_MIE_HEIGHT 	ZEVector2(7994.0f, 1200.0f)

static const ZEVector3 ZEATRayleighScatteringFactor	= ZEVector3(5.8e-6, 13.5e-6, 33.1e-6);
static const ZEVector3 ZEATMieScatteringFactor		= ZEVector3(2.0e-5, 2.0e-5, 2.0e-5);

struct ZEATObserver
{
	struct
	{
		ZEInt8	Hour;
		ZEInt8	Minute;
		ZEInt8	Day;
		ZEInt8	Month;
		ZEInt	Year;
		ZEInt	TimeZone;
		double	DeltaT;
	} Time;

	struct
	{
		double	Elevation;
		double	Longtitude;
		double	Latitude;
		double	AnnualPressure;
		double	AnnualTemperature;
	} Space;

	ZEATObserver();
};

struct ZEATJulian
{
	double Day;
	double EphemerisDay;
	double Century;
	double EphemerisCentury;
	double EphemerisMillenium;
};

struct ZEATHeliocentric
{
	double Longtitude;
	double Latitude;
	double Radius;
};

struct ZEATGeocentric
{
	double Longtitude;
	double Latitude;
	double RightAscension;
	double Declination;
	double Distance;
};

struct ZEATTopocentric
{
	double Zenith;
	double Azimuth;
	double RightAscension;
	double Declination;
	double LocalHourAngle;
};

struct ZEATNutation
{
	double Longtitude;
	double Obliquity;
};
