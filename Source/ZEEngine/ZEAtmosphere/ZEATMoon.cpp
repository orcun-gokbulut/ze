//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEATMoon.cpp
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

#include "ZEATMoon.h"
#include "ZEATPeriodicTerms.h"
#include "ZEMath/ZEAngle.h"
#include "ZEMath/ZEMath.h"

ZEATJulian ZEATMoon::CalculateJulians()
{
	if(Observer.Time.Month <= 2)
	{
		Observer.Time.Year -= 1;
		Observer.Time.Month += 12;
	}

	double HourInDay = (Observer.Time.Hour - Observer.Time.TimeZone) / 24.0;
	double MinuteInDay = Observer.Time.Minute / 1440.0;

	ZEATJulian Julian;

	Julian.Day = (ZEUInt)(365.25 * (Observer.Time.Year + 4716.0)) + (ZEUInt)(30.6001 * (Observer.Time.Month + 1)) + Observer.Time.Day + HourInDay + MinuteInDay - 1524.5;
	if(Julian.Day > 2299160.0)
	{
		ZEInt A = Observer.Time.Year / 100;
		Julian.Day += (2 - A + (A / 4));
	}

	Julian.EphemerisDay = Julian.Day + (Observer.Time.DeltaT / 86400.0);
	Julian.Century = (Julian.Day - 2451545.0) / 36525.0;
	Julian.EphemerisCentury = (Julian.EphemerisDay - 2451545.0) / 36525.0;
	Julian.EphemerisMillenium = Julian.EphemerisCentury / 10.0;

	return Julian;
}

ZEATGeocentric ZEATMoon::CalculateGeocentrics(double JulianEphemerisCentury, double LongtitudeNutation, double TrueObliquity)
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
	for(ZEUInt I = 0; I < 60; ++I)
	{
		double Multiplier = 1.0;
		ZEInt M = MoonPeriodicTermsForLongtitudeAndDistance[I][1];

		if(M & 0x01)
			Multiplier = E;
		else if(M & 0x02)
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
	for(ZEUInt I = 0; I < 60; ++I)
	{
		double Multiplier = 1.0;
		ZEInt M = MoonPeriodicTermsForLatitude[I][1];

		if(M & 0x01)
			Multiplier = E;
		else if(M & 0x02)
			Multiplier = Esquare;

		TermB += MoonPeriodicTermsForLatitude[I][4] * Multiplier *
			ZEAngle::Sin(ZEAngle::ToRadian(MoonPeriodicTermsForLatitude[I][0] * MoonMeanElongation +
											M								   * SunMeanAnomaly +
											MoonPeriodicTermsForLatitude[I][2] * MoonMeanAnomaly +
											MoonPeriodicTermsForLatitude[I][3] * MoonLatitudeArgument));
	}

	double A1 = ZEAngle::ToRadian(119.75 + 131.849 * JulianEphemerisCentury);
	double A2 = ZEAngle::ToRadian(53.09 + 479264.29 * JulianEphemerisCentury);
	double A3 = ZEAngle::ToRadian(313.45 + 481266.484 * JulianEphemerisCentury);

	double RadianMoonMeanLongtitude = ZEAngle::ToRadian(MoonMeanLongtitude);
	double RadianMoonLatitudeArgument = ZEAngle::ToRadian(MoonLatitudeArgument);
	double RadianMoonMeanAnomaly = ZEAngle::ToRadian(MoonMeanAnomaly);

	double DeltaI = 3958.0 * ZEAngle::Sin(A1) + 
					1962.0 * ZEAngle::Sin(RadianMoonMeanLongtitude - RadianMoonLatitudeArgument) +
					318.0 * ZEAngle::Sin(A2);

	double DeltaB = -2235.0 * ZEAngle::Sin(RadianMoonMeanLongtitude) + 
					382.0 * ZEAngle::Sin(A3) +
					175.0 * ZEAngle::Sin(A1 - RadianMoonLatitudeArgument) +
					175.0 * ZEAngle::Sin(A1 + RadianMoonLatitudeArgument) +
					127.0 * ZEAngle::Sin(RadianMoonMeanLongtitude - RadianMoonMeanAnomaly) -
					115.0 * ZEAngle::Sin(RadianMoonMeanLongtitude + RadianMoonMeanAnomaly);

	double MoonLongtitude = MoonMeanLongtitude + ((TermI + DeltaI) / 1000000.0);
	double MoonLatitude = (TermB + DeltaB) / 1000000.0;

	MoonLongtitude = LimitDegreeTo_0_360(MoonLongtitude);
	MoonLatitude = LimitDegreeTo_0_360(MoonLatitude);

	double MoonDistance = 385000.56 + TermR / 1000.0;

	double ApparentMoonLongtitude = MoonLongtitude + LongtitudeNutation;

	double RadianApparentMoonLongtitude = ZEAngle::ToRadian(ApparentMoonLongtitude);
	double RadianTrueObliquity = ZEAngle::ToRadian(TrueObliquity);
	double RadianMoonLatitude = ZEAngle::ToRadian(MoonLatitude);

	double MoonGeocentricRightAscension = ZEAngle::ArcTan2(ZEAngle::Sin(RadianApparentMoonLongtitude) * ZEAngle::Cos(RadianTrueObliquity) -
														ZEAngle::Tan(RadianMoonLatitude) * ZEAngle::Sin(RadianTrueObliquity),
														ZEAngle::Cos(RadianApparentMoonLongtitude));

	MoonGeocentricRightAscension = ZEAngle::ToDegree(MoonGeocentricRightAscension);
	MoonGeocentricRightAscension = LimitDegreeTo_0_360(MoonGeocentricRightAscension);

	double MoonGeocentricDeclination = ZEAngle::ArcSin(ZEAngle::Sin(RadianMoonLatitude) * ZEAngle::Cos(RadianTrueObliquity) +
													ZEAngle::Cos(RadianMoonLatitude) * ZEAngle::Sin(RadianTrueObliquity) * ZEAngle::Sin(RadianApparentMoonLongtitude));

	MoonGeocentricDeclination = ZEAngle::ToDegree(MoonGeocentricDeclination);
	MoonGeocentricDeclination = LimitDegreeTo_0_360(MoonGeocentricDeclination);

	ZEATGeocentric Geocentric;
	Geocentric.Distance = MoonDistance;
	Geocentric.Longtitude= MoonLongtitude;
	Geocentric.Latitude = MoonLatitude;
	Geocentric.SunRightAscension = MoonGeocentricRightAscension;
	Geocentric.SunDeclination = MoonGeocentricDeclination;

	return Geocentric;
}

ZEATNutation ZEATMoon::CalculateNutations(double JulianEphemerisCentury)
{
	double MeanElongationOfMoonfromSun = 297.85036 + 
										445267.111480 * JulianEphemerisCentury - 
										ZEMath::Power(JulianEphemerisCentury, 2) * 0.0019142 +
										ZEMath::Power(JulianEphemerisCentury, 3) / 189474.0;

	double MeanAnomalyOfSun = 357.52772 + 
							35999.050340 * JulianEphemerisCentury -
							ZEMath::Power(JulianEphemerisCentury, 2) * 0.0001603 -
							ZEMath::Power(JulianEphemerisCentury, 3) / 300000.0;

	double MeanAnomalyOfMoon = 134.96298 + 
							477198.867398 * JulianEphemerisCentury +
							ZEMath::Power(JulianEphemerisCentury, 2) * 0.0086972 +
							ZEMath::Power(JulianEphemerisCentury, 3) / 56250.0;

	double MoonLatitudeArgument = 93.27191 + 
								483202.017538 * JulianEphemerisCentury -
								ZEMath::Power(JulianEphemerisCentury, 2) * 0.0036825 +
								ZEMath::Power(JulianEphemerisCentury, 3) / 327270.0;

	double MoonEclipticLongtitude = 125.04452 - 
								1934.136261 * JulianEphemerisCentury +
								ZEMath::Power(JulianEphemerisCentury, 2) * 0.0020708 +
								ZEMath::Power(JulianEphemerisCentury, 3) / 450000.0;

	double LongitudeNutation = 0.0;
	double ObliquityNutation = 0.0;
	for(ZEUInt I = 0; I < 63; ++I)
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

double ZEATMoon::CalculateTrueObliquityOfEcliptic(double JulianEphemerisMillennium, double ObliquityNutation)
{
	double U = JulianEphemerisMillennium / 10.0;

	double MeanObliquityOEcliptic = 84381.448 - 
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

ZEATTopocentric ZEATMoon::CalculateTopocentrics(ZEATGeocentric Geocentric, ZEATJulian Julian, double LongtitudeNutation, double TrueObliquity)
{
	double RadianObserverLatitude = ZEAngle::ToRadian(Observer.Space.Latitude);
	double ElevationOverEarthRadius = Observer.Space.Elevation / 6378140.0;
	double SinLatitude = ZEAngle::Sin(RadianObserverLatitude);
	double CosLatitude = ZEAngle::Cos(RadianObserverLatitude);

	double U = ZEAngle::ArcTan(0.99664719 * (SinLatitude / CosLatitude));
	double X = ZEAngle::Cos(U) + (ElevationOverEarthRadius) * CosLatitude;
	double Y = 0.99664719 * ZEAngle::Sin(U) + (ElevationOverEarthRadius) * SinLatitude;

	double MeanSiderealtimeGreenwich = 280.46061837 + 
									360.98564736629 * (Julian.Day - 2451545.0) +
									ZEMath::Power(Julian.Century, 2) * 0.000387933 -
									ZEMath::Power(Julian.Century, 3) / 38710000.0;

	MeanSiderealtimeGreenwich = LimitDegreeTo_0_360(MeanSiderealtimeGreenwich);

	double ApparentSiderealtimeGreenwich = MeanSiderealtimeGreenwich + LongtitudeNutation * ZEAngle::Cos(ZEAngle::ToRadian(TrueObliquity));

	double ObserverLocalHourAngle = ApparentSiderealtimeGreenwich + Observer.Space.Longtitude - Geocentric.SunRightAscension;

	ObserverLocalHourAngle = LimitDegreeTo_0_360(ObserverLocalHourAngle);
	double RadianObserverLocalHourAngle = ZEAngle::ToRadian(ObserverLocalHourAngle);

	double MoonEquatorialHorizontalParallax = ZEAngle::ArcSin(6378.14 / Geocentric.Distance);
	double RadianMoonEquatorialHorizontalParallax = ZEAngle::ToRadian(MoonEquatorialHorizontalParallax);

	double RadianGeocentricSunDeclination = ZEAngle::ToRadian(Geocentric.SunDeclination);

	double ParallaxMoonRightAscension = ZEAngle::ArcTan2(-X * ZEAngle::Sin(RadianMoonEquatorialHorizontalParallax) * ZEAngle::Sin(RadianObserverLocalHourAngle),
														ZEAngle::Cos(RadianGeocentricSunDeclination) - 
														X * ZEAngle::Sin(RadianMoonEquatorialHorizontalParallax) * ZEAngle::Cos(RadianObserverLocalHourAngle));

	ParallaxMoonRightAscension = ZEAngle::ToDegree(ParallaxMoonRightAscension);

	ZEATTopocentric Topocentric;

	Topocentric.SunRightAscension = Geocentric.SunRightAscension + ParallaxMoonRightAscension;
	Topocentric.SunDeclination = ZEAngle::ArcTan2((ZEAngle::Sin(RadianGeocentricSunDeclination) - 
												Y * ZEAngle::Sin(RadianMoonEquatorialHorizontalParallax)) * ZEAngle::Cos(ZEAngle::ToRadian(ParallaxMoonRightAscension)),
												ZEAngle::Cos(RadianGeocentricSunDeclination) - 
												X * ZEAngle::Sin(RadianMoonEquatorialHorizontalParallax) * ZEAngle::Cos(RadianObserverLocalHourAngle));

	Topocentric.LocalHourAngle = ObserverLocalHourAngle - ParallaxMoonRightAscension;
	double RadianTopocentricLocalHourAngle = ZEAngle::ToRadian(Topocentric.LocalHourAngle);

	double TopocentricElevationAngle = ZEAngle::ArcSin(ZEAngle::Sin(RadianObserverLatitude) * ZEAngle::Sin(Topocentric.SunDeclination) +
													ZEAngle::Cos(RadianObserverLatitude) * ZEAngle::Cos(Topocentric.SunDeclination) * ZEAngle::Cos(RadianTopocentricLocalHourAngle));

	TopocentricElevationAngle = ZEAngle::ToDegree(TopocentricElevationAngle);

	double AtmosphericRefractionCorrection = (Observer.Space.AnnualPressure / 1010.0) * (283.0 / (273.0 + Observer.Space.AnnualTemperature)) * 
											(1.02 / (60.0 * ZEAngle::Tan(ZEAngle::ToRadian(TopocentricElevationAngle + (10.3 / (TopocentricElevationAngle + 5.11))))));
	
	double ResultTopocentricElevationAngle = TopocentricElevationAngle + AtmosphericRefractionCorrection;

	Topocentric.Zenith = 90.0 - ResultTopocentricElevationAngle;

	double AzimuthAngle = ZEAngle::ArcTan2(ZEAngle::Sin(RadianTopocentricLocalHourAngle), 
										ZEAngle::Cos(RadianTopocentricLocalHourAngle) * ZEAngle::Sin(RadianObserverLatitude) -
										ZEAngle::Tan(Topocentric.SunDeclination) * ZEAngle::Cos(RadianObserverLatitude));

	AzimuthAngle = ZEAngle::ToDegree(AzimuthAngle);
	AzimuthAngle = LimitDegreeTo_0_360(AzimuthAngle);
	AzimuthAngle += 180.0;
	AzimuthAngle = LimitDegreeTo_0_360(AzimuthAngle);

	Topocentric.Azimuth = AzimuthAngle;
	Topocentric.SunDeclination = ZEAngle::ToDegree(Topocentric.SunDeclination);

	return Topocentric;
}

void ZEATMoon::SetColor(const ZEVector3& Color)
{
	this->Color = Color;
}

const ZEVector3& ZEATMoon::GetColor() const
{
	return Color;
}

void ZEATMoon::SetDirection(const ZEVector3& Direction)
{
	this->Direction = Direction;
}

const ZEVector3& ZEATMoon::GetDirection() const
{
	return Direction;
}

void ZEATMoon::SetIntensity(float Intensity)
{
	this->Intensity = Intensity;
}

float ZEATMoon::GetIntensity() const
{
	return Intensity;
}

void ZEATMoon::SetObserver(const ZEATObserver& Observer)
{
	this->Observer = Observer;
}

const ZEATObserver& ZEATMoon::GetObserver() const
{
	return Observer;
}

void ZEATMoon::Tick(float ElapsedTime)
{
	/*ZEATJulian Julian = CalculateJulians();
	ZEATNutation Nutation = CalculateNutations(Julian.EphemerisCentury);
	double TrueObliquity = CalculateTrueObliquityOfEcliptic(Julian.EphemerisMillenium, Nutation.Obliquity);
	ZEATGeocentric Geocentric = CalculateGeocentrics(Julian.EphemerisCentury, Nutation.Longtitude, TrueObliquity);
	ZEATTopocentric Topocentric = CalculateTopocentrics(Geocentric, Julian, Nutation.Longtitude, TrueObliquity);

	double RadianTopocentricZenith = ZEAngle::ToRadian(Topocentric.Zenith);
	double RadianTopocentricAzimuth = ZEAngle::ToRadian(Topocentric.Azimuth);

	double ProjectionXZ = ZEAngle::Sin(RadianTopocentricZenith);

	Direction = -ZEVector3(ProjectionXZ * ZEAngle::Sin(RadianTopocentricAzimuth), ZEAngle::Cos(RadianTopocentricZenith), ProjectionXZ * ZEAngle::Cos(RadianTopocentricAzimuth));*/
}

ZEATMoon::ZEATMoon()
{
	Color = ZEVector3::One;
	Direction = ZEVector3(0.0f, -1.0f, 0.0f);
	Intensity = 1.0f;
}
