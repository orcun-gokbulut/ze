//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEATSun.cpp
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

#include "ZEATSun.h"
#include "ZEATPeriodicTerms.h"
#include "ZEMath/ZEAngle.h"
#include "ZEMath/ZEMath.h"

ZEATJulian ZEATSun::CalculateJulians()
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

ZEATHeliocentric ZEATSun::CalculateHeliocentrics(double JulianEphemerisMillennium)
{
	double Longtitude[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	double Latitude[2] = {0.0, 0.0};
	double Radius[5] = {0.0, 0.0, 0.0, 0.0, 0.0};

	ZEUInt Start = 0;
	ZEUInt End = 0;

	for(ZEUInt I = 0; I < 6; ++I)
	{
		End = Start + LongtitudeRowCounts[I];
		for(ZEUInt J = Start; J < End; ++J)
		{
			Longtitude[I] += ZEATEarthPeriodicTerms[J][0] * ZEAngle::Cos(ZEATEarthPeriodicTerms[J][1] + ZEATEarthPeriodicTerms[J][2] * JulianEphemerisMillennium);
		}

		Start = End;
	}

	for(ZEUInt I = 0; I < 2; ++I)
	{
		End = Start + LatitudeRowCounts[I];
		for(ZEUInt J = Start; J < End; ++J)
		{
			Latitude[I] += ZEATEarthPeriodicTerms[J][0] * ZEAngle::Cos(ZEATEarthPeriodicTerms[J][1] + ZEATEarthPeriodicTerms[J][2] * JulianEphemerisMillennium);
		}

		Start = End;
	}

	for(ZEUInt I = 0; I < 5; ++I)
	{
		End = Start + RadiusRowCounts[I];
		for(ZEUInt J = Start; J < End; ++J)
		{
			Radius[I] += ZEATEarthPeriodicTerms[J][0] * ZEAngle::Cos(ZEATEarthPeriodicTerms[J][1] + ZEATEarthPeriodicTerms[J][2] * JulianEphemerisMillennium);
		}

		Start = End;
	}

	ZEATHeliocentric Heliocentric;
	Heliocentric.Longtitude = (
								Longtitude[0] + 
								Longtitude[1] * JulianEphemerisMillennium + 
								Longtitude[2] * ZEMath::Power(JulianEphemerisMillennium, 2) +
								Longtitude[3] * ZEMath::Power(JulianEphemerisMillennium, 3) + 
								Longtitude[4] * ZEMath::Power(JulianEphemerisMillennium, 4) +
								Longtitude[5] * ZEMath::Power(JulianEphemerisMillennium, 5)
								) / 100000000.0;

	Heliocentric.Latitude = (
							Latitude[0] + 
							Latitude[1] * JulianEphemerisMillennium
							) / 100000000.0;

	Heliocentric.Radius = (
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

ZEATNutation ZEATSun::CalculateNutations(double JulianEphemerisCentury)
{
	double MeanElongationOfMoonromSun = 297.85036 + 
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
											MeanElongationOfMoonromSun * ZEATLongtitudeAndObliquityPeriodicTerms[I][0] +
											MeanAnomalyOfSun * ZEATLongtitudeAndObliquityPeriodicTerms[I][1] +
											MeanAnomalyOfMoon * ZEATLongtitudeAndObliquityPeriodicTerms[I][2] +
											MoonLatitudeArgument * ZEATLongtitudeAndObliquityPeriodicTerms[I][3] +
											MoonEclipticLongtitude * ZEATLongtitudeAndObliquityPeriodicTerms[I][4]));

		ObliquityNutation += (ZEATLongtitudeAndObliquityPeriodicTerms[I][7] + ZEATLongtitudeAndObliquityPeriodicTerms[I][8] * JulianEphemerisCentury) *
							ZEAngle::Cos(ZEAngle::ToRadian(
															MeanElongationOfMoonromSun * ZEATLongtitudeAndObliquityPeriodicTerms[I][0] +
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

double ZEATSun::CalculateTrueObliquityOfEcliptic(double JulianEphemerisMillennium, double ObliquityNutation)
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

ZEATGeocentric ZEATSun::CalculateGeocentrics(ZEATHeliocentric Heliocentric, double LongtitudeNutation, double TrueObliquity)
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

	Geocentric.SunRightAscension = ZEAngle::ArcTan2(ZEAngle::Sin(RadianApparentSunLongtitude) * ZEAngle::Cos(RadianTrueObliquity) - 
													ZEAngle::Tan(RadianGeocentricLatitude) * ZEAngle::Sin(RadianTrueObliquity),
													ZEAngle::Cos(RadianApparentSunLongtitude));

	Geocentric.SunRightAscension = ZEAngle::ToDegree(Geocentric.SunRightAscension);
	Geocentric.SunRightAscension = LimitDegreeTo_0_360(Geocentric.SunRightAscension);

	Geocentric.SunDeclination = ZEAngle::ArcSin(ZEAngle::Sin(RadianGeocentricLatitude) * ZEAngle::Cos(RadianTrueObliquity) +
												ZEAngle::Cos(RadianGeocentricLatitude) * ZEAngle::Sin(RadianTrueObliquity) * 
												ZEAngle::Sin(RadianApparentSunLongtitude));

	Geocentric.SunDeclination = ZEAngle::ToDegree(Geocentric.SunDeclination);

	return Geocentric;
}

ZEATTopocentric ZEATSun::CalculateTopocentrics(ZEATGeocentric Geocentric, ZEATJulian Julian, double LongtitudeNutation, double TrueObliquity, double HeliocentricRadius)
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

	double EquatorialHorizontalParallaxOfSun = 8.794 / (3600.0 * HeliocentricRadius);
	double RadianEquatorialHorizontalParallaxOfSun = ZEAngle::ToRadian(EquatorialHorizontalParallaxOfSun);

	double RadianGeocentricSunDeclination = ZEAngle::ToRadian(Geocentric.SunDeclination);

	double ParallaxSunRightAscension = ZEAngle::ArcTan2(-X * ZEAngle::Sin(RadianEquatorialHorizontalParallaxOfSun) * ZEAngle::Sin(RadianObserverLocalHourAngle),
														ZEAngle::Cos(RadianGeocentricSunDeclination) - 
														X * ZEAngle::Sin(RadianEquatorialHorizontalParallaxOfSun) * ZEAngle::Cos(RadianObserverLocalHourAngle));

	ParallaxSunRightAscension = ZEAngle::ToDegree(ParallaxSunRightAscension);

	ZEATTopocentric Topocentric;

	Topocentric.SunRightAscension = Geocentric.SunRightAscension + ParallaxSunRightAscension;
	Topocentric.SunDeclination = ZEAngle::ArcTan2((ZEAngle::Sin(RadianGeocentricSunDeclination) - 
												Y * ZEAngle::Sin(RadianEquatorialHorizontalParallaxOfSun)) * ZEAngle::Cos(ZEAngle::ToRadian(ParallaxSunRightAscension)),
												ZEAngle::Cos(RadianGeocentricSunDeclination) - 
												X * ZEAngle::Sin(RadianEquatorialHorizontalParallaxOfSun) * ZEAngle::Cos(RadianObserverLocalHourAngle));

	Topocentric.LocalHourAngle = ObserverLocalHourAngle - ParallaxSunRightAscension;
	double RadianTopocentricLocalHourAngle = ZEAngle::ToRadian(Topocentric.LocalHourAngle);

	double TopocentricElevationAngle = ZEAngle::ArcSin(ZEAngle::Sin(RadianObserverLatitude) * ZEAngle::Sin(Topocentric.SunDeclination) +
													ZEAngle::Cos(RadianObserverLatitude) * ZEAngle::Cos(Topocentric.SunDeclination) * ZEAngle::Cos(RadianTopocentricLocalHourAngle));

	TopocentricElevationAngle = ZEAngle::ToDegree(TopocentricElevationAngle);

	double AtmosphericRefractionCorrection = (913.1 / 1010.0) * (283.0 / (273.0 + 11.7)) * 
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

void ZEATSun::SetColor(const ZEVector3& Color)
{
	this->Color = Color;
}

const ZEVector3& ZEATSun::GetColor() const
{
	return Color;
}

void ZEATSun::SetDirection(const ZEVector3& Direction)
{
	this->Direction = Direction;
}

const ZEVector3& ZEATSun::GetDirection() const
{
	return Direction;
}

void ZEATSun::SetIntensity(float Intensity)
{
	this->Intensity = Intensity;
}

float ZEATSun::GetIntensity() const
{
	return Intensity;
}

void ZEATSun::SetObserver(const ZEATObserver& Observer)
{
	this->Observer = Observer;
}

const ZEATObserver& ZEATSun::GetObserver() const
{
	return Observer;
}

ZEDrawFlags ZEATSun::GetDrawFlags() const
{
	return ZE_DF_NONE;
}

void ZEATSun::Tick(float ElapsedTime)
{
	ZEATJulian Julian = CalculateJulians();
	ZEATHeliocentric Heliocentric = CalculateHeliocentrics(Julian.EphemerisMillenium);
	ZEATNutation Nutation = CalculateNutations(Julian.EphemerisCentury);
	double TrueObliquity = CalculateTrueObliquityOfEcliptic(Julian.EphemerisMillenium, Nutation.Obliquity);
	ZEATGeocentric Geocentric = CalculateGeocentrics(Heliocentric, Nutation.Longtitude, TrueObliquity);
	ZEATTopocentric Topocentric = CalculateTopocentrics(Geocentric, Julian, Nutation.Longtitude, TrueObliquity, Heliocentric.Radius);

	double RadianTopocentricZenith = ZEAngle::ToRadian(Topocentric.Zenith);
	double RadianTopocentricAzimuth = ZEAngle::ToRadian(Topocentric.Azimuth);

	double ProjectionXZ = ZEAngle::Sin(RadianTopocentricZenith);

	Direction = -ZEVector3(ProjectionXZ * ZEAngle::Sin(RadianTopocentricAzimuth), ZEAngle::Cos(RadianTopocentricZenith), ProjectionXZ * ZEAngle::Cos(RadianTopocentricAzimuth));
}

ZEATSun::ZEATSun()
{
	Color = ZEVector3::One;
	Direction = ZEVector3(0.0f, -1.0f, 0.0f);
	Intensity = 1.0f;
}
