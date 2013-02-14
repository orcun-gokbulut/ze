//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEOrbit.cpp
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

#include "ZEOrbit.h"

#define SecondsInAYear 31557600

void ZEOrbitalMotion::SetOrbitRadius(float OrbitRadius)
{
	this->OrbitRadius = OrbitRadius;
}

float ZEOrbitalMotion::GetOrbitRadius() const
{
	return this->OrbitRadius;
}

void ZEOrbitalMotion::SetObserverLatitude(float Latitude)
{
	ObserverLatitude = Latitude;
}

float ZEOrbitalMotion::GetObserverLatitude() const
{
	return ObserverLatitude;
}

void ZEOrbitalMotion::SetObserverLongitude(float Longitude)
{
	ObserverLongitude = Longitude;
}

float ZEOrbitalMotion::GetObserverLongitude() const
{
	return ObserverLongitude;
}

void ZEOrbitalMotion::SetOrbitalVelocityAnglePerSec(double OrbitalVelocity_AnglePerSec)
{
	this->OrbitalVelocity_AnglePerSec = OrbitalVelocity_AnglePerSec * ZE_PI / 180; //In Radians
}

double ZEOrbitalMotion::GetOrbitalVelocityAnglePerSec() const
{
	return this->OrbitalVelocity_AnglePerSec;
}

void ZEOrbitalMotion::SetAngleBetweenEquatorAndOrbit(float AngleInRadians)
{
	AngleBetweenEquatorAndOrbit = AngleInRadians;

	//Normalize to Orbit Radius
	//AngleBetweenEquatorAndOrbit /= OrbitRadius;
}

float ZEOrbitalMotion::GetAngleBetweenEquatorAndOrbit() const
{
	return AngleBetweenEquatorAndOrbit;
}

void ZEOrbitalMotion::SetStartingTime(JulianDate Time)
{
	this->Time = Time;
	this->Time.ReCalculate();
	OrbitalDay = OrbitalVelocity_AnglePerSec * this->Time.GetDayPercentageAsSeconds();
	AngularYear = (2 * ZE_PI ) * (this->Time.GetDayOfYear() + this->Time.GetDayPercentageAsSeconds() / SecondsInAYear);
}

JulianDate ZEOrbitalMotion::GetTime() const
{
	return Time;
}

void ZEOrbitalMotion::SetSpeedFactor(int SpeedFactor)
{
	this->SpeedFactor = SpeedFactor;

	if(this->SpeedFactor < 1)
		this->SpeedFactor = 1;
}

int ZEOrbitalMotion::GetSpeedFactor() const
{
	return this->SpeedFactor;
}

void ZEOrbitalMotion::SetNorthDirection(ZEVector3 NorthDirection)
{
	this->NorthDirection = NorthDirection;
}

ZEVector3 ZEOrbitalMotion::GetNorthDirection() const
{
	return this->NorthDirection;
}

ZEVector3 ZEOrbitalMotion::GetPosition() const
{
	return this->Position;
}

void ZEOrbitalMotion::Process(float ElapsedTime)
{
	OrbitalDay += OrbitalVelocity_AnglePerSec  * SpeedFactor * ElapsedTime;

	if(OrbitalDay > ZE_PI * 2.0f)
		OrbitalDay = ZEMath::Mod((float)OrbitalDay, ZE_PI * 2.0f);

	AngularYear += (2 * ZE_PI / SecondsInAYear) * SpeedFactor * ElapsedTime;

	if(AngularYear > ZE_PI * 2)
		AngularYear = ZEMath::Mod(OrbitalDay,ZE_PI * 2);

	Time.AddSeconds(ElapsedTime * SpeedFactor);

	Position = ZEVector3(OrbitRadius * ZEAngle::Sin(OrbitalDay),
						OrbitRadius * ZEAngle::Cos(OrbitalDay + ZE_PI),
						AngleBetweenEquatorAndOrbit * ZEAngle::Sin(AngularYear));

	ZEQuaternion TempQuat;
	ZEQuaternion::CreateFromAngleAxis(TempQuat, ObserverLatitude * ZE_PI / 180, ZEVector3::UnitX);
	ZEQuaternion::VectorProduct(Position,TempQuat,Position);
}

ZEOrbitalMotion::ZEOrbitalMotion()
{
	OrbitalVelocity_AnglePerSec = 0.0f;
	AngleBetweenEquatorAndOrbit = 0.0f;

	OrbitalDay = 0.0;
	AngularYear = 0.0;
	SpeedFactor = 1;

	Time.SetDate(2012,9,22,20,44,0);

	ObserverLatitude = 90.0f; // equador
	ObserverLongitude = 0.0f; // sea level

	Position = ZEVector3::Zero;
}

//////////////////////////////////////////////////////////////////////////
//								JULIAN IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////

void JulianDate::SetDate(int Year, int Month, int Day, int Hour, int Minute, int Second)
{
	this->Year = Year;
	this->Month = Month;
	this->Day = Day;
	this->Hour = Hour;
	this->Minute = Minute;
	this->Second = Second;

	this->DayPercentage = ((float)Hour / 24.0f) + ((float)Minute / (24.0f * 60.0f)) + ((float)Second / ( 24.0f * 3600.0f));
	Calculate();
}

void JulianDate::SetDate(int Year, int Month, int Day, float DayPercentage)
{
	this->Year = Year;
	this->Month = Month;
	this->Day = Day;
	this->DayPercentage = DayPercentage;

	this->Hour = (int)(DayPercentage * 24);
	this->Minute = (int)(((DayPercentage * 24) - this->Hour) * 60);
	this->Second = (int)(((((DayPercentage * 24) - this->Hour) * 60) - this->Minute) * 60);
	Calculate();
}

double JulianDate::GetJulianDate() const
{
	return this->JDate;
}

double JulianDate::GetJulianDateFraction() const
{
	return this->JDateFraction;
}

double JulianDate::GetDayOfYear() const
{
	return ZEMath::Mod(this->JDate,365.25f) + 4;
}

double JulianDate::GetDayPercentageAsSeconds() const
{
	return this->Hour * 3600 + this->Minute * 60 + this->Second;
}

void JulianDate::SetYear(int Year)
{
	this->Year = Year;
}

void JulianDate::SetMonth(int Month)
{
	this->Month = Month;
}

void JulianDate::SetDay(int Day)
{
	this->Day = Day;
	ReCalculate();
}

int JulianDate::GetYear() const
{
	return this->Year;
}

int JulianDate::GetMonth() const
{
	return this->Month;
}

int JulianDate::GetDay() const
{
	return this->Day;
}

void JulianDate::SetHour(int Hour)
{
	this->Hour = Hour;
	ReCalculate();
}

void JulianDate::SetMinute(int Minute)
{
	this->Minute = Minute;
	ReCalculate();
}

void JulianDate::SetSecond(int Second)
{
	this->Second = Second;
	ReCalculate();
}

void JulianDate::AddHours(double Hour)
{
	JDateFraction += Hour / 24;

	if(JDateFraction > 1)
	{
		JDateFraction--;
		JDate++;
	}
	ReCalculate();
}

void JulianDate::AddMinutes(double Minute)
{
	JDateFraction += Minute / (24 * 60);

	if(JDateFraction > 1)
	{
		JDateFraction--;
		JDate++;
	}
	ReCalculate();
}

void JulianDate::AddSeconds(double Second)
{
	JDateFraction += Second / (24 * 60 * 60);

	if(JDateFraction > 1)
	{
		JDateFraction--;
		JDate++;
	}
	ReCalculate();
}

int JulianDate::GetHour() const
{
	return this->Hour;
}

int JulianDate::GetMinute() const
{
	return this->Minute;
}

int JulianDate::GetSecond() const
{
	return this->Second;
}

void JulianDate::Calculate()
{
	int Sign;

	if (Month < 3) 
	{
		Year = Year - 1;
		Month = Month + 12;
	}

	YYYYMMDDdd = Year + (float)Month / 100 + (float)Day / 10000 + DayPercentage / 1000000;

	if (Year < 0)
	{
		Year *= -1;
		Sign = -1;
	}
	else
	{
		Year *= 1;
		Sign = 1;
	}

	A = (int)(Year / 100);
	A *= Sign;
	B = 2 - A + (int)(A / 4);

	if(YYYYMMDDdd < 1582.1015)
	{
		JDateFraction = 0.5 + DayPercentage;
		JDate = 1720994 +(int)(365.25 * Year) + (int)(30.6001 * (Month + 1)) + Day;
	}
	else
	{
		JDateFraction = 0.5 + DayPercentage;
		JDate += Day;
		JDate += (int)(30.6001 * (Month + 1));
		JDate += (int)(365.25 * Year);
		JDate += 1720994;
		JDate += B;
	}

	if(JDateFraction > 1)
	{
		JDateFraction--;
		JDate++;
	}
}

void JulianDate::ReCalculate()
{
	//Jean Meeus' Algorithm - "Astronomical Formulae for Calculators" book

	double tempJD, tempJDFraction;

	tempJDFraction = JDateFraction;
	tempJD = JDate;

	tempJDFraction += 0.5;
	if(tempJDFraction > 1)
	{
		tempJDFraction--;
		tempJD++;
	}

	int A = 0, Alpha = 0;

	if(tempJD < 2299161)
	{
		A = tempJD;
	}
	else
	{
		Alpha = (int)((tempJD - 1867216.25) / 36524.25);
		A = tempJD + 1 + Alpha - (int)(Alpha / 4);
	}

	int B = 0, C = 0, D = 0, E = 0;

	B = A + 1524;
	C = (int)((B - 122.1) / 365.25);
	D = (int)(365.25 * C);
	E = (int)((B - D) / 30.6001);

	double DDdd = B - D - (int)(30.6001 * E) + tempJDFraction;

	if(E < 13.5)
		Month = E - 1;
	else
		Month = E - 13;

	if(Month > 2.5)
		Year = C - 4716;
	else
		Year = C - 4715;

	Day = (int)DDdd;
	float dayPercent = (DDdd - Day) * 24;
	Hour = (int)dayPercent;
	float hourPercent = (dayPercent - Hour) * 60;
	Minute = (int)hourPercent;
	float secondPercent = (hourPercent - Minute) * 60;
	Second = (int)secondPercent;
}

JulianDate::JulianDate()
{
	this->Year = 0;
	this->Month = 0;
	this->Day = 0;
	this->Hour = 0;
	this->Minute = 0;
	this->Second = 0;
	this->DayPercentage = 0;
	this->YYYYMMDDdd = 0;
	this->JDate = 0;
	this->JDateFraction = 0;
	this->A = 0;
	this->B = 0;
}

JulianDate::JulianDate(double JDate, double JDateFraction)
{
	this->Year = 0;
	this->Month = 0;
	this->Day = 0;
	this->Hour = 0;
	this->Minute = 0;
	this->Second = 0;
	this->DayPercentage = 0;
	this->YYYYMMDDdd = 0;
	this->JDate = 0;
	this->JDateFraction = 0;
	this->A = 0;
	this->B = 0;

	this->JDate = JDate;
	this->JDateFraction = JDateFraction;

	ReCalculate();
}

JulianDate::JulianDate(int Year, int Month, int Day, float DayPercentage)
{
	this->Year = Year;
	this->Month = Month;
	this->Day = Day;
	this->DayPercentage = DayPercentage;

	this->Hour = (int)(DayPercentage * 24);
	this->Minute = (int)(((DayPercentage * 24) - this->Hour) * 60);
	this->Second = (int)(((((DayPercentage * 24) - this->Hour) * 60) - this->Minute) * 60);
	Calculate();
}

JulianDate::JulianDate(int Year, int Month, int Day, int Hour, int Minute, int Second)
{
	this->Year = Year;
	this->Month = Month;
	this->Day = Day;
	this->Hour = Hour;
	this->Minute = Minute;
	this->Second = Second;

	this->DayPercentage = ((float)Hour / 24.0f) + ((float)Minute / (24.0f * 60.0f)) + ((float)Second / ( 24.0f * 3600.0f));
	Calculate();
}
