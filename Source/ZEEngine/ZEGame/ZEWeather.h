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

ZE_META_ENTITY_DESCRIPTION(ZEWeather)

class ZEWeather : public ZEEntity
{
	ZE_META_ENTITY(ZEModel)

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

		virtual bool				InitializeSelf();
		virtual bool				DeinitializeSelf();

									ZEWeather();
		virtual						~ZEWeather();

		virtual void				AdjustHDR(float DayTime);
		virtual void				UpdateCloudColor();

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

		void						SetSunCastsShadows(bool Value);
		bool						GetSunCastsShadows() const;

		void						SetMoonCastsShadows(bool Value);
		bool						GetMoonCastsShadows() const;
		
		void						SetMoonDirection(const ZEVector3& Value);
		const ZEVector3&			GetMoonDirection() const;

		void						SetSunLightColor(const ZEVector3& Value);
		const ZEVector3&			GetSunLightColor() const;

		void						SetMoonLightColor(const ZEVector3& Value);
		const ZEVector3&			GetMoonLightColor() const;

		virtual ZEDrawFlags			GetDrawFlags() const;

		virtual void				Draw(ZEDrawParameters* DrawParameters);
		virtual void				Tick(float Time);

		static ZEWeather*			CreateInstance();
};

/*
ZE_POST_PROCESSOR_START(Meta)
<zinek>
	<meta>
		<class name="ZEWeather" parent="ZEEntity" description="Weather">
			<property name="SunDirection" type="ZEVector3" autogetset="true" description="Sun's Direction"/>
			<property name="MoonDirection" type="ZEVector3" autogetset="true" description="Moon's Direction"/>
			<property name="SunLightIntensity" type="float" autogetset="true" description="Sun Light's Intensity"/>
			<property name="MoonLightIntensity" type="float" autogetset="true" description="Moon  Light's Intensity"/>
			<property name="MoonPhase" type="float" autogetset="true" description="Moon's Phase"/>
			<property name="FogFactor" type="float" autogetset="true" description="Fog Density"/>
			<property name="CloudCover" type="float" autogetset="true" description="Cloud Cover"/>
			<property name="SunCastsShadows" type="boolean" autogetset="true" description="Sun shadow casting option"/>
			<property name="MoonCastsShadows" type="boolean" autogetset="true" description="Moon shadow casting option"/>
			<property name="SunLightColor" type="ZEVector3" autogetset="true" description="Sun Light's Color" semantic="ZE_PS_COLOR"/>
			<property name="MoonLightColor" type="ZEVector3" autogetset="true" description="Moon Light's Color" semantic="ZE_PS_COLOR"/>
		</class>
	</meta>
</zinek>
ZE_POST_PROCESSOR_END()
*/

#endif // __ZE_WEATHER_H__
