//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESkyDome.h
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
#ifndef __ZE_SKY_DOME_H__
#define __ZE_SKY_DOME_H__

#include "ZEEntity.h"
#include "ZEGraphics\ZECanvas.h"
#include "ZEGraphics\ZERenderCommand.h"
#include "ZETypes.h"

ZE_META_FORWARD_DECLARE(ZESkyDomeMaterial, "ZEGraphics/ZESkyDomeMaterial.h")

ZE_CLASS(ZESkyDome)

class ZESkyDome : public ZEEntity
{
	ZE_OBJECT

	protected:
		float					G;

		float					AmbientFactor;
		ZEVector3				MiddayAmbientColor;		// Ambient color for the midday
		ZEVector3				SunsetAmbientColor;		// Ambient color for sunset or sundown

		float					MieConstant;
		float					RayleighConstant;

		float					MieScaleDepth;
		float					RayleighScaleDepth;

		float					OuterRadius;			// = 61500.0f. Try to maintain Inner / Outer radius ratio 
		float					InnerRadius;			// = 60000.0f. Try to maintain Inner / Outer radius ratio

		float					SunIntensity;

		ZEVector3				SunLightDirection;
		ZEVector3				SunLightWaveLenght;

		ZEVector3				CameraPosition;			// This parameter is the real time camera position for when the ground is round. No need to use offset parameter.
		ZEVector3				CameraPositionOffset;	// = 60000.0f. This parameter is for when the ground is flat and calculations are done 
														// with a fixed camera height. No need to use real time camera position.
		
		ZECanvas				SkyDomeGeometry;
		ZESkyDomeMaterial*		SkyDomeMaterial;
		ZERenderCommand			SkyDomeRenderCommand;

		virtual bool			InitializeSelf();
		virtual bool			DeinitializeSelf();

								ZESkyDome();
		virtual					~ZESkyDome();

	private:
		
	public:
		void					SetAmbientFactor(float Value);
		float					GetAmbientFactor() const;

		void					SetMiddayAmbientColor(ZEVector3& Color);
		ZEVector3				GetMiddayAmbientColor() const;

		void					SetSunsetAmbientColor(ZEVector3& Color);
		ZEVector3				GetSunsetAmbientColor() const;

		void					SetSetMieConstant(float Value);
		float					GetGetMieConstant() const;

		void					SetRayleighConstant(float Value);
		float					GetRayleighConstant() const;

		void					SetMieScaleDepth(float Value);
		float					GetMieScaleDepth() const;

		void					SetRayleighScaleDepth(float Value);
		float					GetRayleighScaleDepth() const;

		void					SetSunIntensity(float Value);
		float					GetSunIntensity() const;

		void					SetSunLightDirection(const ZEVector3& Value);
		ZEVector3				GetSunLightDirection() const;

		void					SetSunLightWaveLenght(ZEVector3& Value);
		ZEVector3				GetSunLightWaveLenght() const;

		void					SetOuterRadius(float Value);
		float					GetOuterRadius() const;

		void					SetInnerRadius(float Value);
		float					GetInnerRadius() const;

		void					SetCameraPosition(ZEVector3 Value);
		ZEVector3				GetCameraPosition() const;

		void					SetCameraPositionOffset(ZEVector3 Value);
		ZEVector3				GetCameraPositionOffset() const;
		
		void					SetG(float Value);
		float					GetG() const;

		virtual ZEDrawFlags		GetDrawFlags() const;

		virtual void			Draw(ZEDrawParameters* DrawParameters);
		virtual void			Tick(float Time);

		static ZESkyDome*		CreateInstance();
};

#endif // __ZE_SKY_DOME_H__
