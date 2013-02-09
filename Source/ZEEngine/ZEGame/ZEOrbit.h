//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEOrbit.h
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
#ifndef __ZE_ORBIT_H__
#define __ZE_ORBIT_H__

#include "ZEFoundation/ZEMath/ZEVector.h"
#include "ZEFoundation/ZEMath/ZEQuaternion.h"
#include "ZEFoundation/ZEMath/ZEAngle.h"
#include "ZEMath/ZEMath.h"

class JulianDate
{
private:
	double						JDate, JDateFraction;
	int							Year, Month, Day;
	int							Hour, Minute, Second;
	float						DayPercentage;
	float						YYYYMMDDdd;
	int							A,B;

public:
	void						SetDate(int Year, int Month, int Day, int Hour, int Minute, int Second);
	void						SetDate(int Year, int Month, int Day, float DayPercentage);

	double						GetJulianDate() const;
	double						GetJulianDateFraction() const;

	double						GetDayOfYear() const;

	double						GetDayPercentageAsSeconds() const;

	void						SetYear(int Year);
	int							GetYear() const;

	void						SetMonth(int Month);
	int							GetMonth() const;

	void						SetDay(int Day);
	int							GetDay() const;

	void						SetHour(int Hour);
	int							GetHour() const;

	void						SetMinute(int Minute);
	int							GetMinute() const;

	void						SetSecond(int Second);
	int							GetSecond() const;

	void						AddHours(double Hour);
	void						AddMinutes(double Minute);
	void						AddSeconds(double Second);

	void						Calculate();
	void						ReCalculate();

								JulianDate();
								JulianDate(double JDate, double JDateFraction);
								JulianDate(int Year, int Month, int Day, float DayPercentage);
								JulianDate(int Year, int Month, int Day, int Hour, int Minute, int Second);
};

class ZEOrbitalMotion
{
	private:
		float						OrbitRadius;

		double						OrbitalDay;
		double						AngularYear;

		JulianDate					Time;

		float						ObserverLatitude;
		float						ObserverLongitude;

		float						AngleBetweenEquatorAndOrbit;
		double						OrbitalVelocity_AnglePerSec;
		int							SpeedFactor;

		ZEVector3					Position;

		ZEVector3					NorthDirection;

	public:
		void						SetOrbitRadius(float OrbitRadius);
		float						GetOrbitRadius() const;

		void						SetObserverLatitude(float Latitude);
		float						GetObserverLatitude() const;

		void						SetObserverLongitude(float Longitude);
		float						GetObserverLongitude() const;

		void						SetOrbitalVelocityAnglePerSec(double OrbitalVelocity_AnglePerSec);
		double						GetOrbitalVelocityAnglePerSec() const;

		void						SetAngleBetweenEquatorAndOrbit(float AngleInRadians);
		float						GetAngleBetweenEquatorAndOrbit() const;

		void						SetSpeedFactor(int SpeedFactor);
		int							GetSpeedFactor() const;

		void						SetStartingTime(JulianDate Time);
		JulianDate					GetTime() const;

		void						SetNorthDirection(ZEVector3 NorthDirection);
		ZEVector3					GetNorthDirection() const;

		ZEVector3					GetPosition() const;

		void						Process(float ElapsedTime);

									ZEOrbitalMotion();
};

#endif
