//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEATAstronomy.cpp
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

#include "ZEATAstronomy.h"

#include "ZEMath/ZEMath.h"
#include "ZEMath/ZEAngle.h"
#include "ZEATPeriodicTerms.h"

static inline double LimitDegreeTo_0_360(double Degree)
{
	ZEInt Multiplier = Degree / 360;

	if (Degree < 0 && Multiplier == 0)
		return (Degree + 360.0);

	return (Degree - Multiplier * 360);
}

static inline double LimitDegreeTo_0_180(double Degree)
{
	ZEInt Multiplier = Degree / 180;

	if (Degree < 0 && Multiplier == 0)
		return (Degree + 180.0);

	return (Degree - Multiplier * 180);
}

static inline double Limit_0_1(double Value)
{
	Value = Value - ZEMath::Floord(Value);
	if (Value < 0.0f)
		Value += 1.0;

	return Value;
}

static inline double LimitDegreeTo_0_180M(double Degree)
{
	ZEInt Multiplier = Degree / 360;
	double Value = (Degree - Multiplier * 360);
	if (Value <= -180.0)
		Value += 360.0;
	else if (Value >= 180.0)
		Value -= 360.0;

	return Value;
}

static double CalculateJulianDay(const ZEATObserver& Observer)
{
	ZEInt Hour = Observer.Time.Hour;
	ZEInt Minute = Observer.Time.Minute;
	ZEInt Day = Observer.Time.Day;
	ZEInt Month = Observer.Time.Month;
	ZEInt Year = Observer.Time.Year;
	ZEInt TimeZone = Observer.Time.TimeZone;

	if(Month <= 2)
	{
		Year -= 1;
		Month += 12;
	}

	double HourInDay = (Hour - TimeZone) / 24.0;
	double MinuteInDay = Minute / 1440.0;

	double JulianDay = (ZEInt)(365.25 * (Year + 4716.0)) + (ZEInt)(30.6001 * (Month + 1)) + Day + HourInDay + MinuteInDay - 1524.5;
	if(JulianDay > 2299160.0)
	{
		ZEInt A = Year / 100;
		JulianDay += (2 - A + (A / 4));
	}

	return JulianDay;
}

static ZEATJulian CalculateJulianTerms(double JulianDay, double DeltaT)
{
	ZEATJulian Julian;
	Julian.Day = JulianDay;
	Julian.Century = (Julian.Day - 2451545.0) / 36525.0;
	Julian.EphemerisDay = Julian.Day + (DeltaT / 86400.0);
	Julian.EphemerisCentury = (Julian.EphemerisDay - 2451545.0) / 36525.0;
	Julian.EphemerisMillenium = Julian.EphemerisCentury / 10.0;

	return Julian;
}

static ZEATHeliocentric CalculateHeliocentrics(double JulianEphemerisMillennium)
{
	double Longtitude[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	double Latitude[2] = {0.0, 0.0};
	double Radius[5] = {0.0, 0.0, 0.0, 0.0, 0.0};

	ZEUInt Start = 0;
	ZEUInt End = 0;

	for(ZEUInt I = 0; I < 6; I++)
	{
		End = Start + LongtitudeRowCounts[I];
		for(ZEUInt J = Start; J < End; J++)
		{
			Longtitude[I] += ZEATEarthPeriodicTerms[J][0] * ZEAngle::Cos(ZEATEarthPeriodicTerms[J][1] + ZEATEarthPeriodicTerms[J][2] * JulianEphemerisMillennium);
		}

		Start = End;
	}

	for(ZEUInt I = 0; I < 2; I++)
	{
		End = Start + LatitudeRowCounts[I];
		for(ZEUInt J = Start; J < End; J++)
		{
			Latitude[I] += ZEATEarthPeriodicTerms[J][0] * ZEAngle::Cos(ZEATEarthPeriodicTerms[J][1] + ZEATEarthPeriodicTerms[J][2] * JulianEphemerisMillennium);
		}

		Start = End;
	}

	for(ZEUInt I = 0; I < 5; I++)
	{
		End = Start + RadiusRowCounts[I];
		for(ZEUInt J = Start; J < End; J++)
		{
			Radius[I] += ZEATEarthPeriodicTerms[J][0] * ZEAngle::Cos(ZEATEarthPeriodicTerms[J][1] + ZEATEarthPeriodicTerms[J][2] * JulianEphemerisMillennium);
		}

		Start = End;
	}

	ZEATHeliocentric Heliocentric;
	Heliocentric.Longtitude = 
		(
		Longtitude[0] + 
		Longtitude[1] * JulianEphemerisMillennium + 
		Longtitude[2] * ZEMath::Power(JulianEphemerisMillennium, 2) +
		Longtitude[3] * ZEMath::Power(JulianEphemerisMillennium, 3) + 
		Longtitude[4] * ZEMath::Power(JulianEphemerisMillennium, 4) +
		Longtitude[5] * ZEMath::Power(JulianEphemerisMillennium, 5)
		) / 100000000.0;

	Heliocentric.Latitude = 
		(
		Latitude[0] + 
		Latitude[1] * JulianEphemerisMillennium
		) / 100000000.0;

	Heliocentric.Radius = 
		(
		Radius[0] + 
		Radius[1] * JulianEphemerisMillennium + 
		Radius[2] * ZEMath::Power(JulianEphemerisMillennium, 2) +
		Radius[3] * ZEMath::Power(JulianEphemerisMillennium, 3) + 
		Radius[4] * ZEMath::Power(JulianEphemerisMillennium, 4)
		) / 100000000.0;

	Heliocentric.Longtitude = ZEAngle::ToDegree(Heliocentric.Longtitude);
	Heliocentric.Latitude = ZEAngle::ToDegree(Heliocentric.Latitude);

	Heliocentric.Longtitude = LimitDegreeTo_0_360(Heliocentric.Longtitude);

	return Heliocentric;
}

static ZEATNutation CalculateNutations(double JulianEphemerisCentury)
{
	double MeanElongationOfMoonfromSun = 
		297.85036 + 
		445267.111480 * JulianEphemerisCentury - 
		ZEMath::Power(JulianEphemerisCentury, 2) * 0.0019142 +
		ZEMath::Power(JulianEphemerisCentury, 3) / 189474.0;

	double MeanAnomalyOfSun = 
		357.52772 + 
		35999.050340 * JulianEphemerisCentury -
		ZEMath::Power(JulianEphemerisCentury, 2) * 0.0001603 -
		ZEMath::Power(JulianEphemerisCentury, 3) / 300000.0;

	double MeanAnomalyOfMoon = 
		134.96298 + 
		477198.867398 * JulianEphemerisCentury +
		ZEMath::Power(JulianEphemerisCentury, 2) * 0.0086972 +
		ZEMath::Power(JulianEphemerisCentury, 3) / 56250.0;

	double MoonLatitudeArgument = 
		93.27191 + 
		483202.017538 * JulianEphemerisCentury -
		ZEMath::Power(JulianEphemerisCentury, 2) * 0.0036825 +
		ZEMath::Power(JulianEphemerisCentury, 3) / 327270.0;

	double MoonEclipticLongtitude = 
		125.04452 - 
		1934.136261 * JulianEphemerisCentury +
		ZEMath::Power(JulianEphemerisCentury, 2) * 0.0020708 +
		ZEMath::Power(JulianEphemerisCentury, 3) / 450000.0;

	double LongitudeNutation = 0.0;
	double ObliquityNutation = 0.0;
	for(ZEUInt I = 0; I < 63; I++)
	{
		LongitudeNutation += (ZEATLongtitudeAndObliquityPeriodicTerms[I][5] + ZEATLongtitudeAndObliquityPeriodicTerms[I][6] * JulianEphemerisCentury) *
			ZEAngle::Sin(ZEAngle::ToRadian(
			MeanElongationOfMoonfromSun * ZEATLongtitudeAndObliquityPeriodicTerms[I][0] +
			MeanAnomalyOfSun * ZEATLongtitudeAndObliquityPeriodicTerms[I][1] +
			MeanAnomalyOfMoon * ZEATLongtitudeAndObliquityPeriodicTerms[I][2] +
			MoonLatitudeArgument * ZEATLongtitudeAndObliquityPeriodicTerms[I][3] +
			MoonEclipticLongtitude * ZEATLongtitudeAndObliquityPeriodicTerms[I][4]));

		ObliquityNutation += (ZEATLongtitudeAndObliquityPeriodicTerms[I][7] + ZEATLongtitudeAndObliquityPeriodicTerms[I][8] * JulianEphemerisCentury) *
			ZEAngle::Cos(ZEAngle::ToRadian(
			MeanElongationOfMoonfromSun * ZEATLongtitudeAndObliquityPeriodicTerms[I][0] +
			MeanAnomalyOfSun * ZEATLongtitudeAndObliquityPeriodicTerms[I][1] +
			MeanAnomalyOfMoon * ZEATLongtitudeAndObliquityPeriodicTerms[I][2] +
			MoonLatitudeArgument * ZEATLongtitudeAndObliquityPeriodicTerms[I][3] +
			MoonEclipticLongtitude * ZEATLongtitudeAndObliquityPeriodicTerms[I][4]));
	}

	LongitudeNutation /= 36000000.0;
	ObliquityNutation /= 36000000.0;

	ZEATNutation Nutation;
	Nutation.Longtitude = LongitudeNutation;
	Nutation.Obliquity = ObliquityNutation;

	return Nutation;
}

static double CalculateTrueObliquityOfEcliptic(double JulianEphemerisMillennium, double ObliquityNutation)
{
	double U = JulianEphemerisMillennium / 10.0;

	double MeanObliquityOEcliptic = 
		84381.448 - 
		4680.93 * U - 
		1.55 * ZEMath::Power(U, 2) + 
		1999.25 * ZEMath::Power(U, 3) -
		51.38 * ZEMath::Power(U, 4) - 
		249.67 * ZEMath::Power(U, 5) - 
		39.05 * ZEMath::Power(U, 6) +
		7.12 * ZEMath::Power(U, 7) + 
		27.87 * ZEMath::Power(U, 8) + 
		5.79 * ZEMath::Power(U, 9) +
		2.45 * ZEMath::Power(U, 10);

	return (MeanObliquityOEcliptic / 3600.0 + ObliquityNutation);
}

static double CalculateApparentSiderealTimeAtGreenwich(double JulianDay, double JulianCentury, double LongtitudeNutation, double TrueObliquity)
{
	double MeanSiderealtimeGreenwich = 
		280.46061837 + 
		360.98564736629 * (JulianDay - 2451545.0) +
		ZEMath::Power(JulianCentury, 2) * 0.000387933 -
		ZEMath::Power(JulianCentury, 3) / 38710000.0;

	MeanSiderealtimeGreenwich = LimitDegreeTo_0_360(MeanSiderealtimeGreenwich);

	return MeanSiderealtimeGreenwich + LongtitudeNutation * ZEAngle::Cos(ZEAngle::ToRadian(TrueObliquity));
}

static ZEATGeocentric CalculateSunGeocentrics(const ZEATHeliocentric& Heliocentric, double LongtitudeNutation, double TrueObliquity)
{
	ZEATGeocentric Geocentric;
	Geocentric.Longtitude = Heliocentric.Longtitude + 180.0;
	Geocentric.Longtitude = LimitDegreeTo_0_360(Geocentric.Longtitude);

	Geocentric.Latitude = -Heliocentric.Latitude;

	double AberrationCorrection = -20.4898 / (3600.0 * Heliocentric.Radius);
	double ApparentSunLongtitude = Geocentric.Longtitude + LongtitudeNutation + AberrationCorrection;

	double RadianApparentSunLongtitude = ZEAngle::ToRadian(ApparentSunLongtitude);
	double RadianGeocentricLatitude = ZEAngle::ToRadian(Geocentric.Latitude);
	double RadianTrueObliquity = ZEAngle::ToRadian(TrueObliquity);

	Geocentric.RightAscension = 
		ZEAngle::ArcTan2
		(
		ZEAngle::Sin(RadianApparentSunLongtitude) * ZEAngle::Cos(RadianTrueObliquity) - 
		ZEAngle::Tan(RadianGeocentricLatitude) * ZEAngle::Sin(RadianTrueObliquity),
		ZEAngle::Cos(RadianApparentSunLongtitude)
		);

	Geocentric.RightAscension = ZEAngle::ToDegree(Geocentric.RightAscension);
	Geocentric.RightAscension = LimitDegreeTo_0_360(Geocentric.RightAscension);

	Geocentric.Declination = 
		ZEAngle::ArcSin
		(
		ZEAngle::Sin(RadianGeocentricLatitude) * ZEAngle::Cos(RadianTrueObliquity) +
		ZEAngle::Cos(RadianGeocentricLatitude) * ZEAngle::Sin(RadianTrueObliquity) * ZEAngle::Sin(RadianApparentSunLongtitude)
		);

	Geocentric.Declination = ZEAngle::ToDegree(Geocentric.Declination);

	return Geocentric;
}

static ZEATTopocentric CalculateSunTopocentrics(const ZEATObserver& Observer, const ZEATGeocentric& Geocentric, const ZEATJulian& Julian, double LongtitudeNutation, double TrueObliquity, double HeliocentricRadius)
{
	double RadianObserverLatitude = ZEAngle::ToRadian(Observer.Space.Latitude);
	double ElevationOverEarthRadius = Observer.Space.Elevation / 6378140.0;
	double SinLatitude = ZEAngle::Sin(RadianObserverLatitude);
	double CosLatitude = ZEAngle::Cos(RadianObserverLatitude);

	double U = ZEAngle::ArcTan(0.99664719 * (SinLatitude / CosLatitude));
	double X = ZEAngle::Cos(U) + ElevationOverEarthRadius * CosLatitude;
	double Y = 0.99664719 * ZEAngle::Sin(U) + ElevationOverEarthRadius * SinLatitude;

	double ApparentSiderealtimeGreenwich = CalculateApparentSiderealTimeAtGreenwich(Julian.Day, Julian.Century, LongtitudeNutation, TrueObliquity);

	double ObserverLocalHourAngle = ApparentSiderealtimeGreenwich + Observer.Space.Longtitude - Geocentric.RightAscension;

	ObserverLocalHourAngle = LimitDegreeTo_0_360(ObserverLocalHourAngle);
	double RadianObserverLocalHourAngle = ZEAngle::ToRadian(ObserverLocalHourAngle);

	double EquatorialHorizontalParallaxOfSun = 8.794 / (3600.0 * HeliocentricRadius);
	double RadianEquatorialHorizontalParallaxOfSun = ZEAngle::ToRadian(EquatorialHorizontalParallaxOfSun);

	double RadianGeocentricSunDeclination = ZEAngle::ToRadian(Geocentric.Declination);

	double ParallaxSunRightAscension = 
		ZEAngle::ArcTan2(-X * ZEAngle::Sin(RadianEquatorialHorizontalParallaxOfSun) * ZEAngle::Sin(RadianObserverLocalHourAngle),
		ZEAngle::Cos(RadianGeocentricSunDeclination) - 
		X * ZEAngle::Sin(RadianEquatorialHorizontalParallaxOfSun) * ZEAngle::Cos(RadianObserverLocalHourAngle));

	ParallaxSunRightAscension = ZEAngle::ToDegree(ParallaxSunRightAscension);

	ZEATTopocentric Topocentric;

	Topocentric.RightAscension = Geocentric.RightAscension + ParallaxSunRightAscension;
	Topocentric.Declination = 
		ZEAngle::ArcTan2((ZEAngle::Sin(RadianGeocentricSunDeclination) - 
		Y * ZEAngle::Sin(RadianEquatorialHorizontalParallaxOfSun)) * ZEAngle::Cos(ZEAngle::ToRadian(ParallaxSunRightAscension)),
		ZEAngle::Cos(RadianGeocentricSunDeclination) - 
		X * ZEAngle::Sin(RadianEquatorialHorizontalParallaxOfSun) * ZEAngle::Cos(RadianObserverLocalHourAngle));

	Topocentric.LocalHourAngle = ObserverLocalHourAngle - ParallaxSunRightAscension;
	double RadianTopocentricLocalHourAngle = ZEAngle::ToRadian(Topocentric.LocalHourAngle);

	double TopocentricElevationAngle = 
		ZEAngle::ArcSin(ZEAngle::Sin(RadianObserverLatitude) * ZEAngle::Sin(Topocentric.Declination) +
		ZEAngle::Cos(RadianObserverLatitude) * ZEAngle::Cos(Topocentric.Declination) * ZEAngle::Cos(RadianTopocentricLocalHourAngle));

	TopocentricElevationAngle = ZEAngle::ToDegree(TopocentricElevationAngle);

	double AtmosphericRefractionCorrection = 
		(913.1 / 1010.0) * (283.0 / (273.0 + 11.7)) * 
		(1.02 / (60.0 * ZEAngle::Tan(ZEAngle::ToRadian(TopocentricElevationAngle + (10.3 / (TopocentricElevationAngle + 5.11))))));

	double ResultTopocentricElevationAngle = TopocentricElevationAngle + AtmosphericRefractionCorrection;

	Topocentric.Zenith = 90.0 - ResultTopocentricElevationAngle;

	double AzimuthAngle = 
		ZEAngle::ArcTan2(ZEAngle::Sin(RadianTopocentricLocalHourAngle), 
		ZEAngle::Cos(RadianTopocentricLocalHourAngle) * ZEAngle::Sin(RadianObserverLatitude) -
		ZEAngle::Tan(Topocentric.Declination) * ZEAngle::Cos(RadianObserverLatitude));

	AzimuthAngle = ZEAngle::ToDegree(AzimuthAngle);
	AzimuthAngle = LimitDegreeTo_0_360(AzimuthAngle);
	AzimuthAngle += 180.0;
	AzimuthAngle = LimitDegreeTo_0_360(AzimuthAngle);

	Topocentric.Azimuth = AzimuthAngle;
	Topocentric.Declination = ZEAngle::ToDegree(Topocentric.Declination);

	return Topocentric;
}

static ZEATGeocentric CalculateMoonGeocentrics(double JulianEphemerisCentury, double LongtitudeNutation, double TrueObliquity)
{
	double MoonMeanLongtitude = 218.3164477 +
		481267.88123421 * JulianEphemerisCentury -
		ZEMath::Power(JulianEphemerisCentury, 2) * 0.0015786 +
		ZEMath::Power(JulianEphemerisCentury, 3) / 538841.0 -
		ZEMath::Power(JulianEphemerisCentury, 4) / 65194000.0;

	double MoonMeanElongation = 297.8501921 +
		445267.1114034 * JulianEphemerisCentury -
		ZEMath::Power(JulianEphemerisCentury, 2) * 0.0018819 +
		ZEMath::Power(JulianEphemerisCentury, 3) / 545868.0 -
		ZEMath::Power(JulianEphemerisCentury, 4) / 113065000.0;

	double SunMeanAnomaly = 357.5291092 +
		35999.0502909 * JulianEphemerisCentury -
		ZEMath::Power(JulianEphemerisCentury, 2) * 0.0001536 +
		ZEMath::Power(JulianEphemerisCentury, 3) / 24490000.0;

	double MoonMeanAnomaly = 134.9633964 +
		477198.8675055 * JulianEphemerisCentury +
		ZEMath::Power(JulianEphemerisCentury, 2) * 0.0087414 +
		ZEMath::Power(JulianEphemerisCentury, 3) / 69699.0 -
		ZEMath::Power(JulianEphemerisCentury, 4) / 14712000.0;

	double MoonLatitudeArgument = 93.2720950 +
		483202.0175233 * JulianEphemerisCentury -
		ZEMath::Power(JulianEphemerisCentury, 2) * 0.0036539 -
		ZEMath::Power(JulianEphemerisCentury, 3) / 3526000.0 +
		ZEMath::Power(JulianEphemerisCentury, 4) / 863310000.0;

	double E = 1.0 - 0.002516 * JulianEphemerisCentury - 0.0000074 * JulianEphemerisCentury * JulianEphemerisCentury;
	double Esquare = E * E;

	double TermI = 0.0;
	double TermR = 0.0;
	for(ZEUInt I = 0; I < 60; I++)
	{
		double Multiplier = 1.0;
		ZEInt M = MoonPeriodicTermsForLongtitudeAndDistance[I][1];

		if(M == -1 || M == 1)
			Multiplier = E;
		else if(M == -2 || M == 2)
			Multiplier = Esquare;

		TermI += MoonPeriodicTermsForLongtitudeAndDistance[I][4] * Multiplier *
			ZEAngle::Sin(ZEAngle::ToRadian(MoonPeriodicTermsForLongtitudeAndDistance[I][0] * MoonMeanElongation +
			M												* SunMeanAnomaly +
			MoonPeriodicTermsForLongtitudeAndDistance[I][2] * MoonMeanAnomaly +
			MoonPeriodicTermsForLongtitudeAndDistance[I][3] * MoonLatitudeArgument));

		TermR += MoonPeriodicTermsForLongtitudeAndDistance[I][5] * Multiplier *
			ZEAngle::Cos(ZEAngle::ToRadian(MoonPeriodicTermsForLongtitudeAndDistance[I][0] * MoonMeanElongation +
			M												* SunMeanAnomaly +
			MoonPeriodicTermsForLongtitudeAndDistance[I][2] * MoonMeanAnomaly +
			MoonPeriodicTermsForLongtitudeAndDistance[I][3] * MoonLatitudeArgument));
	}

	double TermB = 0.0;
	for(ZEUInt I = 0; I < 60; I++)
	{
		double Multiplier = 1.0;
		ZEInt M = MoonPeriodicTermsForLatitude[I][1];

		if(M == -1 || M == 1)
			Multiplier = E;
		else if(M == -2 || M == 2)
			Multiplier = Esquare;

		TermB += MoonPeriodicTermsForLatitude[I][4] * Multiplier *
			ZEAngle::Sin(ZEAngle::ToRadian(MoonPeriodicTermsForLatitude[I][0] * MoonMeanElongation +
			M								   * SunMeanAnomaly +
			MoonPeriodicTermsForLatitude[I][2] * MoonMeanAnomaly +
			MoonPeriodicTermsForLatitude[I][3] * MoonLatitudeArgument));
	}

	double A1 = 119.75 + 131.849 * JulianEphemerisCentury;
	double A2 = 53.09 + 479264.29 * JulianEphemerisCentury;
	double A3 = 313.45 + 481266.484 * JulianEphemerisCentury;

	double DeltaI = 
		3958.0 * ZEAngle::Sin(ZEAngle::ToRadian(A1)) + 
		1962.0 * ZEAngle::Sin(ZEAngle::ToRadian(MoonMeanLongtitude - MoonLatitudeArgument)) +
		318.0 * ZEAngle::Sin(A2);

	double DeltaB = 
		-2235.0 * ZEAngle::Sin(ZEAngle::ToRadian(MoonMeanLongtitude)) + 
		382.0 * ZEAngle::Sin(ZEAngle::ToRadian(A3)) +
		175.0 * ZEAngle::Sin(ZEAngle::ToRadian(A1 - MoonLatitudeArgument)) +
		175.0 * ZEAngle::Sin(ZEAngle::ToRadian(A1 + MoonLatitudeArgument)) +
		127.0 * ZEAngle::Sin(ZEAngle::ToRadian(MoonMeanLongtitude - MoonMeanAnomaly)) -
		115.0 * ZEAngle::Sin(ZEAngle::ToRadian(MoonMeanLongtitude + MoonMeanAnomaly));

	double MoonLongtitude = MoonMeanLongtitude + ((TermI + DeltaI) / 1000000.0);
	double MoonLatitude = (TermB + DeltaB) / 1000000.0;

	MoonLongtitude = LimitDegreeTo_0_360(MoonLongtitude);
	MoonLatitude = LimitDegreeTo_0_360(MoonLatitude);

	double MoonDistance = 385000.56 + TermR / 1000.0;

	double ApparentMoonLongtitude = MoonLongtitude + LongtitudeNutation;

	double RadianApparentMoonLongtitude = ZEAngle::ToRadian(ApparentMoonLongtitude);
	double RadianTrueObliquity = ZEAngle::ToRadian(TrueObliquity);
	double RadianMoonLatitude = ZEAngle::ToRadian(MoonLatitude);

	double MoonGeocentricRightAscension = ZEAngle::ArcTan2
		(
		ZEAngle::Sin(RadianApparentMoonLongtitude) * ZEAngle::Cos(RadianTrueObliquity) -
		ZEAngle::Tan(RadianMoonLatitude) * ZEAngle::Sin(RadianTrueObliquity),
		ZEAngle::Cos(RadianApparentMoonLongtitude)
		);

	MoonGeocentricRightAscension = ZEAngle::ToDegree(MoonGeocentricRightAscension);
	MoonGeocentricRightAscension = LimitDegreeTo_0_360(MoonGeocentricRightAscension);

	double MoonGeocentricDeclination = ZEAngle::ArcSin
		(
		ZEAngle::Sin(RadianMoonLatitude) * ZEAngle::Cos(RadianTrueObliquity) +
		ZEAngle::Cos(RadianMoonLatitude) * ZEAngle::Sin(RadianTrueObliquity) * ZEAngle::Sin(RadianApparentMoonLongtitude)
		);

	MoonGeocentricDeclination = ZEAngle::ToDegree(MoonGeocentricDeclination);
	MoonGeocentricDeclination = LimitDegreeTo_0_360(MoonGeocentricDeclination);

	ZEATGeocentric Geocentric;
	Geocentric.Distance = MoonDistance;
	Geocentric.Longtitude= MoonLongtitude;
	Geocentric.Latitude = MoonLatitude;
	Geocentric.RightAscension = MoonGeocentricRightAscension;
	Geocentric.Declination = MoonGeocentricDeclination;

	return Geocentric;
}

ZEATTopocentric CalculateMoonTopocentrics(const ZEATObserver& Observer, ZEATGeocentric Geocentric, ZEATJulian Julian, double LongtitudeNutation, double TrueObliquity)
{
	double RadianObserverLatitude = ZEAngle::ToRadian(Observer.Space.Latitude);
	double ElevationOverEarthRadius = Observer.Space.Elevation / 6378140.0;
	double SinLatitude = ZEAngle::Sin(RadianObserverLatitude);
	double CosLatitude = ZEAngle::Cos(RadianObserverLatitude);

	double U = ZEAngle::ArcTan(0.99664719 * (SinLatitude / CosLatitude));
	double X = ZEAngle::Cos(U) + ElevationOverEarthRadius * CosLatitude;
	double Y = 0.99664719 * ZEAngle::Sin(U) + ElevationOverEarthRadius * SinLatitude;

	double ApparentSiderealtimeGreenwich = CalculateApparentSiderealTimeAtGreenwich(Julian.Day, Julian.Century, LongtitudeNutation, TrueObliquity);
	ApparentSiderealtimeGreenwich = LimitDegreeTo_0_360(ApparentSiderealtimeGreenwich);

	double ObserverLocalHourAngleDegree = ApparentSiderealtimeGreenwich + Observer.Space.Longtitude - Geocentric.RightAscension;
	ObserverLocalHourAngleDegree = LimitDegreeTo_0_360(ObserverLocalHourAngleDegree);
	double RadianObserverLocalHourAngle = ZEAngle::ToRadian(ObserverLocalHourAngleDegree);

	double MoonEquatorialHorizontalParallaxRadian = ZEAngle::ArcSin(6378.14 / Geocentric.Distance);
	double RadianGeocentricSunDeclination = ZEAngle::ToRadian(Geocentric.Declination);

	double ParallaxMoonRightAscensionRadian = ZEAngle::ArcTan2
		(
		-X * ZEAngle::Sin(MoonEquatorialHorizontalParallaxRadian) * ZEAngle::Sin(RadianObserverLocalHourAngle),
		ZEAngle::Cos(RadianGeocentricSunDeclination) - 
		X * ZEAngle::Sin(MoonEquatorialHorizontalParallaxRadian) * ZEAngle::Cos(RadianObserverLocalHourAngle)
		);

	double ParallaxMoonRightAscensionDegree = ZEAngle::ToDegree(ParallaxMoonRightAscensionRadian);

	ZEATTopocentric Topocentric;
	Topocentric.RightAscension = Geocentric.RightAscension + ParallaxMoonRightAscensionDegree;
	Topocentric.Declination = ZEAngle::ArcTan2
		(
		(ZEAngle::Sin(RadianGeocentricSunDeclination) - Y * ZEAngle::Sin(MoonEquatorialHorizontalParallaxRadian)) * 
		ZEAngle::Cos(ParallaxMoonRightAscensionRadian),
		ZEAngle::Cos(RadianGeocentricSunDeclination) - 
		Y * ZEAngle::Sin(MoonEquatorialHorizontalParallaxRadian) * ZEAngle::Cos(RadianObserverLocalHourAngle)
		);

	Topocentric.LocalHourAngle = ObserverLocalHourAngleDegree - ParallaxMoonRightAscensionDegree;
	double RadianTopocentricLocalHourAngle = ZEAngle::ToRadian(Topocentric.LocalHourAngle);

	double TopocentricElevationAngle = ZEAngle::ArcSin
		(
		ZEAngle::Sin(RadianObserverLatitude) * ZEAngle::Sin(Topocentric.Declination) +
		ZEAngle::Cos(RadianObserverLatitude) * ZEAngle::Cos(Topocentric.Declination) * ZEAngle::Cos(RadianTopocentricLocalHourAngle)
		);

	TopocentricElevationAngle = ZEAngle::ToDegree(TopocentricElevationAngle);

	double AtmosphericRefractionCorrection = 
		(Observer.Space.AnnualPressure / 1010.0) * (283.0 / (273.0 + Observer.Space.AnnualTemperature)) * 
		(1.02 / (60.0 * ZEAngle::Tan(ZEAngle::ToRadian(TopocentricElevationAngle + (10.3 / (TopocentricElevationAngle + 5.11))))));

	double TopocentricElevationAngleWithAtmosphericRefractionCorrection = TopocentricElevationAngle + AtmosphericRefractionCorrection;

	Topocentric.Zenith = 90.0 - TopocentricElevationAngleWithAtmosphericRefractionCorrection;

	double AzimuthAngle = ZEAngle::ArcTan2
		(
		ZEAngle::Sin(RadianTopocentricLocalHourAngle), 
		ZEAngle::Cos(RadianTopocentricLocalHourAngle) * ZEAngle::Sin(RadianObserverLatitude) -
		ZEAngle::Tan(Topocentric.Declination) * ZEAngle::Cos(RadianObserverLatitude)
		);

	AzimuthAngle = ZEAngle::ToDegree(AzimuthAngle);
	AzimuthAngle = LimitDegreeTo_0_360(AzimuthAngle);
	AzimuthAngle += 180.0;
	AzimuthAngle = LimitDegreeTo_0_360(AzimuthAngle);

	Topocentric.Azimuth = AzimuthAngle;
	Topocentric.Declination = ZEAngle::ToDegree(Topocentric.Declination);

	return Topocentric;
}

ZEVector3 ZEATAstronomy::GetSunDirection(const ZEATObserver& Observer)
{
	double JulianDay = CalculateJulianDay(Observer);
	ZEATJulian Julian = CalculateJulianTerms(JulianDay, Observer.Time.DeltaT);
	ZEATHeliocentric Heliocentric = CalculateHeliocentrics(Julian.EphemerisMillenium);
	ZEATNutation Nutation = CalculateNutations(Julian.EphemerisCentury);
	double TrueObliquity = CalculateTrueObliquityOfEcliptic(Julian.EphemerisMillenium, Nutation.Obliquity);
	ZEATGeocentric Geocentric = CalculateSunGeocentrics(Heliocentric, Nutation.Longtitude, TrueObliquity);
	ZEATTopocentric Topocentric = CalculateSunTopocentrics(Observer, Geocentric, Julian, Nutation.Longtitude, TrueObliquity, Heliocentric.Radius);

	double RadianTopocentricZenith = ZEAngle::ToRadian(Topocentric.Zenith);
	double RadianTopocentricAzimuth = ZEAngle::ToRadian(Topocentric.Azimuth);

	double ProjectionXZ = ZEAngle::Sin(RadianTopocentricZenith);

	return ZEVector3(ProjectionXZ * ZEAngle::Sin(RadianTopocentricAzimuth), ZEAngle::Cos(RadianTopocentricZenith), ProjectionXZ * ZEAngle::Cos(RadianTopocentricAzimuth));
}

ZEVector3 ZEATAstronomy::GetMoonDirection(const ZEATObserver& Observer)
{
	double JulianDay = CalculateJulianDay(Observer);
	ZEATJulian Julian = CalculateJulianTerms(JulianDay, Observer.Time.DeltaT);
	ZEATNutation Nutation = CalculateNutations(Julian.EphemerisCentury);
	double TrueObliquity = CalculateTrueObliquityOfEcliptic(Julian.EphemerisMillenium, Nutation.Obliquity);
	ZEATGeocentric Geocentric = CalculateMoonGeocentrics(Julian.EphemerisCentury, Nutation.Longtitude, TrueObliquity);
	ZEATTopocentric Topocentric = CalculateMoonTopocentrics(Observer, Geocentric, Julian, Nutation.Longtitude, TrueObliquity);

	double RadianTopocentricZenith = ZEAngle::ToRadian(Topocentric.Zenith);
	double RadianTopocentricAzimuth = ZEAngle::ToRadian(Topocentric.Azimuth);

	double ProjectionXZ = ZEAngle::Sin(RadianTopocentricZenith);

	return ZEVector3(ProjectionXZ * ZEAngle::Sin(RadianTopocentricAzimuth), ZEAngle::Cos(RadianTopocentricZenith), ProjectionXZ * ZEAngle::Cos(RadianTopocentricAzimuth));
}

void ZEATAstronomy::GetSunTransitSunriseSunsetTime(double& SunTransitTime, double& SunriseTime, double& SunsetTime, const ZEATObserver& Observer)
{
	ZEATObserver ModifiedObserver = Observer;
	ModifiedObserver.Time.Hour = 0;
	ModifiedObserver.Time.Minute = 0;
	ModifiedObserver.Time.TimeZone = 0;

	double JulianDay = CalculateJulianDay(ModifiedObserver);
	ZEATJulian Julian = CalculateJulianTerms(JulianDay, ModifiedObserver.Time.DeltaT);
	ZEATNutation Nutation = CalculateNutations(Julian.EphemerisCentury);
	double TrueObliquity = CalculateTrueObliquityOfEcliptic(Julian.EphemerisMillenium, Nutation.Obliquity);
	double ApparentSiderealTimeAtGreenwich = CalculateApparentSiderealTimeAtGreenwich(Julian.Day, Julian.Century, Nutation.Longtitude, TrueObliquity);

	ZEATGeocentric Geocentrics[3];
	ModifiedObserver.Time.DeltaT = 0;
	JulianDay--;
	for (ZEUInt I = 0; I < 3; I++)
	{
		ZEATJulian Julian = CalculateJulianTerms(JulianDay, ModifiedObserver.Time.DeltaT);
		ZEATHeliocentric Heliocentric = CalculateHeliocentrics(Julian.EphemerisMillenium);
		ZEATNutation Nutation = CalculateNutations(Julian.EphemerisCentury);
		double TrueObliquity = CalculateTrueObliquityOfEcliptic(Julian.EphemerisMillenium, Nutation.Obliquity);
		Geocentrics[I] = CalculateSunGeocentrics(Heliocentric, Nutation.Longtitude, TrueObliquity);

		JulianDay++;
	}

	double GeocentricSunDeclinationRadian1 = ZEAngle::ToRadian(Geocentrics[1].Declination);
	double ObserverLatitudeRadian = ZEAngle::ToRadian(Observer.Space.Latitude);

	double Argument =
		(ZEAngle::Sin(ZEAngle::ToRadian(-0.8333)) - ZEAngle::Sin(ObserverLatitudeRadian) * ZEAngle::Sin(GeocentricSunDeclinationRadian1)) /
		(ZEAngle::Cos(ObserverLatitudeRadian) * ZEAngle::Cos(GeocentricSunDeclinationRadian1));

	if (Argument < -1.0 || Argument > 1.0)
		return;

	double LocalHourAngleRadian = ZEAngle::ArcCos(Argument);
	double LocalHourAngleDegree = ZEAngle::ToDegree(LocalHourAngleRadian);
	LocalHourAngleDegree = LimitDegreeTo_0_180(LocalHourAngleDegree);
	
	double ApproximateSunTransitTime = (Geocentrics[1].RightAscension - Observer.Space.Longtitude - ApparentSiderealTimeAtGreenwich) / 360.0;
	double ApproximateSunriseTime = ApproximateSunTransitTime - (LocalHourAngleDegree / 360.0);
	double ApproximateSunsetTime = ApproximateSunTransitTime + (LocalHourAngleDegree / 360.0);

	ApproximateSunTransitTime = Limit_0_1(ApproximateSunTransitTime);
	ApproximateSunriseTime = Limit_0_1(ApproximateSunriseTime);
	ApproximateSunsetTime = Limit_0_1(ApproximateSunsetTime);

	double SiderealTimeAtGreenwichSunTransit = ApparentSiderealTimeAtGreenwich + 360.985647 * ApproximateSunTransitTime;
	double SiderealTimeAtGreenwichSunrise = ApparentSiderealTimeAtGreenwich + 360.985647 * ApproximateSunriseTime;
	double SiderealTimeAtGreenwichSunset = ApparentSiderealTimeAtGreenwich + 360.985647 * ApproximateSunsetTime;

	double A = Geocentrics[1].RightAscension - Geocentrics[0].RightAscension;
	double B = Geocentrics[2].RightAscension - Geocentrics[1].RightAscension;
	A = (ZEMath::Abs(A) > 2.0) ? Limit_0_1(A) : A;
	B = (ZEMath::Abs(B) > 2.0) ? Limit_0_1(B) : B;
	double C = B - A;

	double AA = Geocentrics[1].Declination - Geocentrics[0].Declination;
	double BB = Geocentrics[2].Declination - Geocentrics[1].Declination;
	AA = (ZEMath::Abs(AA) > 2.0) ? Limit_0_1(AA) : AA;
	BB = (ZEMath::Abs(BB) > 2.0) ? Limit_0_1(BB) : BB;
	double CC = BB - AA;

	double SunRightAscensionSunTransitDegree = Geocentrics[1].RightAscension + (ApproximateSunTransitTime * (A + B + C * ApproximateSunTransitTime) * 0.5);
	double SunRightAscensionSunriseDegree = Geocentrics[1].RightAscension + (ApproximateSunriseTime * (A + B + C * ApproximateSunriseTime) * 0.5);
	double SunRightAscensionSunsetDegree = Geocentrics[1].RightAscension + (ApproximateSunsetTime * (A + B + C * ApproximateSunsetTime) * 0.5);

	double SunDeclinationSunTransitDegree = Geocentrics[1].Declination + (ApproximateSunTransitTime * (AA + BB + CC * ApproximateSunTransitTime) * 0.5);
	double SunDeclinationSunriseDegree = Geocentrics[1].Declination + (ApproximateSunriseTime * (AA + BB + CC * ApproximateSunriseTime) * 0.5);
	double SunDeclinationSunsetDegree = Geocentrics[1].Declination + (ApproximateSunsetTime * (AA + BB + CC * ApproximateSunsetTime) * 0.5);

	double LocalHourAngleSunTransitDegree = SiderealTimeAtGreenwichSunTransit + Observer.Space.Longtitude - SunRightAscensionSunTransitDegree;
	double LocalHourAngleSunriseDegree = SiderealTimeAtGreenwichSunrise + Observer.Space.Longtitude - SunRightAscensionSunriseDegree;
	double LocalHourAngleSunsetDegree = SiderealTimeAtGreenwichSunset + Observer.Space.Longtitude - SunRightAscensionSunsetDegree;

	LocalHourAngleSunTransitDegree = LimitDegreeTo_0_180M(LocalHourAngleSunTransitDegree);
	LocalHourAngleSunriseDegree = LimitDegreeTo_0_180M(LocalHourAngleSunriseDegree);
	LocalHourAngleSunsetDegree = LimitDegreeTo_0_180M(LocalHourAngleSunsetDegree);

	double SunRightAscensionSunTransitRadian = ZEAngle::ToRadian(SunRightAscensionSunTransitDegree);
	double SunRightAscensionSunriseRadian = ZEAngle::ToRadian(SunRightAscensionSunriseDegree);
	double SunRightAscensionSunsetRadian = ZEAngle::ToRadian(SunRightAscensionSunsetDegree);

	double SunDeclinationSunTransitRadian = ZEAngle::ToRadian(SunDeclinationSunTransitDegree);
	double SunDeclinationSunriseRadian = ZEAngle::ToRadian(SunDeclinationSunriseDegree);
	double SunDeclinationSunsetRadian = ZEAngle::ToRadian(SunDeclinationSunsetDegree);

	double LocalHourAngleSunTransitRadian = ZEAngle::ToRadian(LocalHourAngleSunTransitDegree);
	double LocalHourAngleSunriseRadian = ZEAngle::ToRadian(LocalHourAngleSunriseDegree);
	double LocalHourAngleSunsetRadian = ZEAngle::ToRadian(LocalHourAngleSunsetDegree);

	double SunAltitudeSunTransitRadian = ZEAngle::ArcSin
		(
		ZEAngle::Sin(ObserverLatitudeRadian) * ZEAngle::Sin(SunDeclinationSunTransitRadian) + 
		ZEAngle::Cos(ObserverLatitudeRadian) * ZEAngle::Cos(SunDeclinationSunTransitRadian) * ZEAngle::Cos(LocalHourAngleSunTransitRadian)
		);

	double SunAltitudeSunriseRadian = ZEAngle::ArcSin
		(
		ZEAngle::Sin(ObserverLatitudeRadian) * ZEAngle::Sin(SunDeclinationSunriseRadian) + 
		ZEAngle::Cos(ObserverLatitudeRadian) * ZEAngle::Cos(SunDeclinationSunriseRadian) * ZEAngle::Cos(LocalHourAngleSunriseRadian)
		);

	double SunAltitudeSunsetRadian = ZEAngle::ArcSin
		(
		ZEAngle::Sin(ObserverLatitudeRadian) * ZEAngle::Sin(SunDeclinationSunsetRadian) + 
		ZEAngle::Cos(ObserverLatitudeRadian) * ZEAngle::Cos(SunDeclinationSunsetRadian) * ZEAngle::Cos(LocalHourAngleSunsetRadian)
		);

	double SunAltitudeSunTransitDegree = ZEAngle::ToDegree(SunAltitudeSunTransitRadian);
	double SunAltitudeSunriseDegree = ZEAngle::ToDegree(SunAltitudeSunriseRadian);
	double SunAltitudeSunsetDegree = ZEAngle::ToDegree(SunAltitudeSunsetRadian);

	double SunTransitDayFraction = ApproximateSunTransitTime - (LocalHourAngleSunTransitDegree / 360.0);
	double SunriseDayFraction = 
		ApproximateSunriseTime + 
		(
		(SunAltitudeSunriseDegree + 0.8333) / 
		(360.0 * ZEAngle::Cos(SunDeclinationSunriseRadian) * ZEAngle::Cos(ObserverLatitudeRadian) * ZEAngle::Sin(LocalHourAngleSunriseRadian))
		);
	double SunsetDayFraction = 
		ApproximateSunsetTime + 
		(
		(SunAltitudeSunsetDegree + 0.8333) / 
		(360.0 * ZEAngle::Cos(SunDeclinationSunsetRadian) * ZEAngle::Cos(ObserverLatitudeRadian) * ZEAngle::Sin(LocalHourAngleSunsetRadian))
		);

	SunTransitTime = 24.0 * Limit_0_1(SunTransitDayFraction + Observer.Time.TimeZone / 24.0);
	SunriseTime = 24.0 * Limit_0_1(SunriseDayFraction + Observer.Time.TimeZone / 24.0);
	SunsetTime = 24.0 * Limit_0_1(SunsetDayFraction + Observer.Time.TimeZone / 24.0);
}

float ZEATAstronomy::GetSunDiskRadius(const ZEATObserver& Observer)
{
	double JulianDay = CalculateJulianDay(Observer);
	ZEATJulian Julian = CalculateJulianTerms(JulianDay, Observer.Time.DeltaT);
	ZEATHeliocentric Heliocentric = CalculateHeliocentrics(Julian.EphemerisMillenium);

	return 959.63 / (3600.0 * Heliocentric.Radius);
}

float ZEATAstronomy::GetMoonDiskRadius(const ZEATObserver& Observer)
{
	double JulianDay = CalculateJulianDay(Observer);
	ZEATJulian Julian = CalculateJulianTerms(JulianDay, Observer.Time.DeltaT);
	ZEATNutation Nutation = CalculateNutations(Julian.EphemerisCentury);
	double TrueObliquity = CalculateTrueObliquityOfEcliptic(Julian.EphemerisMillenium, Nutation.Obliquity);
	ZEATGeocentric Geocentric = CalculateMoonGeocentrics(Julian.EphemerisCentury, Nutation.Longtitude, TrueObliquity);

	double RadianObserverLatitude = ZEAngle::ToRadian(Observer.Space.Latitude);
	double ElevationOverEarthRadius = Observer.Space.Elevation / 6378140.0;
	double SinLatitude = ZEAngle::Sin(RadianObserverLatitude);
	double CosLatitude = ZEAngle::Cos(RadianObserverLatitude);

	double U = ZEAngle::ArcTan(0.99664719 * (SinLatitude / CosLatitude));
	double X = ZEAngle::Cos(U) + ElevationOverEarthRadius * CosLatitude;
	double Y = 0.99664719 * ZEAngle::Sin(U) + ElevationOverEarthRadius * SinLatitude;

	double ApparentSiderealtimeGreenwich = CalculateApparentSiderealTimeAtGreenwich(Julian.Day, Julian.Century, Nutation.Longtitude, TrueObliquity);
	ApparentSiderealtimeGreenwich = LimitDegreeTo_0_360(ApparentSiderealtimeGreenwich);

	double ObserverLocalHourAngleDegree = ApparentSiderealtimeGreenwich + Observer.Space.Longtitude - Geocentric.RightAscension;
	ObserverLocalHourAngleDegree = LimitDegreeTo_0_360(ObserverLocalHourAngleDegree);
	double RadianObserverLocalHourAngle = ZEAngle::ToRadian(ObserverLocalHourAngleDegree);

	double MoonEquatorialHorizontalParallaxRadian = ZEAngle::ArcSin(6378.14 / Geocentric.Distance);
	double RadianGeocentricSunDeclination = ZEAngle::ToRadian(Geocentric.Declination);

	double ParallaxMoonRightAscensionRadian = ZEAngle::ArcTan2
		(
		-X * ZEAngle::Sin(MoonEquatorialHorizontalParallaxRadian) * ZEAngle::Sin(RadianObserverLocalHourAngle),
		ZEAngle::Cos(RadianGeocentricSunDeclination) - 
		X * ZEAngle::Sin(MoonEquatorialHorizontalParallaxRadian) * ZEAngle::Cos(RadianObserverLocalHourAngle)
		);

	double ParallaxMoonRightAscensionDegree = ZEAngle::ToDegree(ParallaxMoonRightAscensionRadian);

	ZEATTopocentric Topocentric;
	Topocentric.RightAscension = Geocentric.RightAscension + ParallaxMoonRightAscensionDegree;
	Topocentric.Declination = ZEAngle::ArcTan2
		(
		(ZEAngle::Sin(RadianGeocentricSunDeclination) - Y * ZEAngle::Sin(MoonEquatorialHorizontalParallaxRadian)) * 
		ZEAngle::Cos(ParallaxMoonRightAscensionRadian),
		ZEAngle::Cos(RadianGeocentricSunDeclination) - 
		Y * ZEAngle::Sin(MoonEquatorialHorizontalParallaxRadian) * ZEAngle::Cos(RadianObserverLocalHourAngle)
		);

	Topocentric.LocalHourAngle = ObserverLocalHourAngleDegree - ParallaxMoonRightAscensionDegree;
	double RadianTopocentricLocalHourAngle = ZEAngle::ToRadian(Topocentric.LocalHourAngle);

	double TopocentricElevationAngle = ZEAngle::ArcSin
		(
		ZEAngle::Sin(RadianObserverLatitude) * ZEAngle::Sin(Topocentric.Declination) +
		ZEAngle::Cos(RadianObserverLatitude) * ZEAngle::Cos(Topocentric.Declination) * ZEAngle::Cos(RadianTopocentricLocalHourAngle)
		);

	TopocentricElevationAngle = ZEAngle::ToDegree(TopocentricElevationAngle);

	double AtmosphericRefractionCorrection = 
		(Observer.Space.AnnualPressure / 1010.0) * (283.0 / (273.0 + Observer.Space.AnnualTemperature)) * 
		(1.02 / (60.0 * ZEAngle::Tan(ZEAngle::ToRadian(TopocentricElevationAngle + (10.3 / (TopocentricElevationAngle + 5.11))))));

	double TopocentricElevationAngleWithAtmosphericRefractionCorrection = TopocentricElevationAngle + AtmosphericRefractionCorrection;

	return (358473400.0 * (1.0 + ZEAngle::Sin(ZEAngle::ToRadian(TopocentricElevationAngleWithAtmosphericRefractionCorrection)) * ZEAngle::Sin(MoonEquatorialHorizontalParallaxRadian))) / 
		(3600.0 * Geocentric.Distance);
}
