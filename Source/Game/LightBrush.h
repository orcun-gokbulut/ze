//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - LightBrush.h
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
#ifndef __ZE_LIGHTBRUSH_H__
#define __ZE_LIGHTBRUSH_H__

#include "Meta/Class.h"
#include "Game/Entity.h"
#include "Graphics/RenderOrder.h"
#include "Graphics/Light.h"

ZE_META_CLASS_DESCRIPTION(ZELightBrush);

class ZETexture2DResource;
class ZETextureCubeResource;
class ZEFixedMaterial;
class ZELight;

class ZELightBrush : public ZEEntity
{
	ZE_META_CLASS()
	private:
		ZELightType							LightType;
		ZELight*							Light;

		ZERenderOrder						RenderOrder;
		ZEFixedMaterial*					Material;

		float								Intensity;
		ZEVector3							Color;
		float								Range;
		ZEVector3							Attenuation;
		bool								CastsShadow;
		ZETexture2DResource*				ProjectionTexture;
		ZETextureCubeResource*				CubeProjectionTexture;
		float								ProjectionFOV;
		float								ProjectionAspectRatio;

	public:
		virtual ZEDWORD						GetDrawFlags() const;

		ZELight*							GetLight();

		void								SetLightType(ZELightType LightType);
		ZELightType							GetLightType() const;

		void								SetColor(const ZEVector3& NewColor);
		const ZEVector3&					GetColor() const;

		void								SetIntensity(float NewValue);
		float								GetIntensity() const;

		void								SetRange(float NewValue);
		float								GetRange() const;

		void								SetEnabled(bool NewValue);
		
		void								SetAttenuation(const ZEVector3& NewValue);
		const ZEVector3&					GetAttenuation() const;

		void								SetCastsShadow(bool NewValue);
		bool								GetCastsShadow() const;

		void								SetProjectionTexture(const char* TextureFile);
		const char*							GetProjectionTexture() const;

		void								SetProjectionFOV(float NewValue);
		float								GetProjectionFOV() const;

		void								SetProjectionAspectRatio(float NewValue);
		float								GetProjectionAspectRatio() const;

		virtual bool						Initialize();
		virtual void						Deinitialize();
		virtual void						Reset();

		virtual void						Tick(float Time);
		virtual void						Update();

		virtual void						Draw(ZEDrawParameters* DrawParameters);

											ZELightBrush();
		virtual								~ZELightBrush();
};
/*
ZE_POST_PROCESSOR_START(Meta)
<zinek>
	<meta>
		<class name="ZELightBrush" parent="ZEEntity" icon="" description="Light brush">
			<property name="LightType" type="integer" autogetset="true" description="Type of the light">
				<enumurator name="ZELightType">
					<item name="None" value="ZE_LT_NONE"/>  
					<item name="Point Light" value="ZE_LT_POINT"/>
					<item name="Directional Light" value="ZE_LT_DIRECTIONAL"/>
					<item name="Projective Light" value="ZE_LT_PROJECTIVE"/>
					<item name="Omni Projective Light" value="ZE_LT_OMNIPROJECTIVE"/>
				</enumurator>
			</property>
			<property name="Intensity" type="float" autogetset="true" description="Intesity of the light"/>
			<property name="Color" type="ZEVector3" autogetset="true" semantic="ZE_PS_COLOR" description= "Color of the light"/>
			<property name="Range" type="float" autogetset="true" description="Range of the light"/>
			<property name="Attenuation" type="ZEVector3" autogetset="true" description="Attenuation of the light"/>
			<property name="CastsShadow" type="boolean" autogetset="true" description="Does light casts shadows ?"/>
			<property name="ProjectionTexture" type="string" autogetset="true" semantic="ZE_PS_FILENAME" description="Projection texture"/>
			<property name="ProjectionFOV" type="float" autogetset="true" description="Field of view of the projection"/>
			<property name="ProjectionAspectRatio" type="float" autogetset="true" description="Aspect Ratio of the projection"/>
		</class>
	</meta>
</zinek>
ZE_POST_PROCESSOR_END() 
*/
#endif
