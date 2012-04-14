//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEWeather.h
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
#ifndef __ZE_WEATHER_H__
#define __ZE_WEATHER_H__

#include "ZEEntity.h"
#include "ZEMath\ZEVector.h"
#include "ZEDrawParameters.h"


class ZEDirectionalLight;
class ZEDirectionalLight;	
class ZESkyBrush;	
class ZESkyDome;			
class ZECloud;		
class ZEMoon;				

class ZEWeather : public ZEEntity
{
	protected:

		ZEMoon*						Moon;
		ZECloud*					Cloud;
		ZESkyDome*					SkyDome;
		ZESkyBrush*					StarMap;
		ZEDirectionalLight*			SunLight;
		ZEDirectionalLight*			MoonLight;

		float						SunHeight;
		float						MoonHeight;

		float						FogFactor;
		float						MoonPhase;
		float						CloudCover;
		float						SunLightIntensity;
		float						MoonLightIntensity;

		ZEVector3					SunDirection;
		ZEVector3					MoonDirection;
		ZEVector3					SunMoonRotation;

		ZEVector3					SunLightColor;
		ZEVector3					MoonLightColor;

									ZEWeather();
		virtual						~ZEWeather();

		virtual void				AdjustHDR(float DayTime);

	private:

	public:
		ZEMoon*						GetMoon() const;
		ZECloud*					GetCloud() const;
		ZESkyDome*					GetSkyDome() const;
		ZESkyBrush*					GetStarMap() const;
		ZEDirectionalLight*			GetSunLight() const;
		ZEDirectionalLight*			GetMoonLight() const;

		float						GetSunHeight() const;
		float						GetMoonHeight() const;

		void						SetFogFactor(const float Value);
		float						GetFogFactor() const;

		void						SetMoonPhase(const float Value);
		float						GetMoonPhase() const;

		void						SetCloudCover(const float Value);
		float						GetCloudCover() const;

		void						SetMoonLightIntensity(const float Value);
		float						GetMoonLightIntensity() const;

		void						SetSunLightIntensity(const float Value);
		float						GetSunLightIntensity() const;

		void						SetSunDirection(const ZEVector3& Value);
		const ZEVector3&			GetSunDirection() const;
		
		void						SetMoonDirection(const ZEVector3& Value);
		const ZEVector3&			GetMoonDirection() const;

		void						SetSunMoonRotation(const ZEVector3& Value);
		const ZEVector3&			GetSunMoonRotation() const;

		void						SetSunLightColor(const ZEVector3& Value);
		const ZEVector3&			GetSunLightColor() const;

		void						SetMoonLightColor(const ZEVector3& Value);
		const ZEVector3&			GetMoonLightColor() const;


		virtual ZEDrawFlags			GetDrawFlags() const;

		virtual bool				Initialize();
		virtual void				Deinitialize();

		virtual void				Draw(ZEDrawParameters* DrawParameters);
		virtual void				Tick(float Time);

		static ZEWeather*			CreateInstance();
};



#endif // __ZE_WEATHER_H__