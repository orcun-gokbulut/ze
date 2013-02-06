//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECloud.h
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
#ifndef __ZE_CLOUD_H__
#define __ZE_CLOUD_H__

#include "ZEEntity.h"
#include "ZERenderer/ZECanvas.h"
#include "ZERenderer/ZERenderCommand.h"
#include "ZETypes.h"
#include "ZEMath/ZEAABBox.h"

class ZETexture2DResource;
class ZECloudMaterial;
class ZEMaterial;
class ZEScene;
class ZECamera;

class ZECloud : public ZEEntity
{
	protected:
		float					CloudCover;
		float					CloudPlaneHeight;
		
		ZEVector2				WindVelocity;
		ZEVector3				SunLightDirection;
		ZEVector3				SunLightColor;
		ZEVector3				AmbientColor;
		ZECamera*				Camera;

		ZEVector3				Rayleigh;
		ZEVector3				Mie;
		float					G;
		float					LightScale;
		float					AmbientScale;
		float					EarthRadius;
		float					AtmosphereHeight;

		ZETexture2DResource*	CloudFormationTexture;

		ZECloudMaterial*		CloudMaterial;
		ZERenderCommand			CloudRenderCommand;

								ZECloud();
		virtual					~ZECloud();

	private:


	public:

		void					SetAmbientColor(ZEVector3 Color);
		ZEVector3				GetAmbientColor();

		void					SetSunLightColor(ZEVector3 Color);
		ZEVector3				GetSunLightColor();
		
		void					SetRayleigh(ZEVector3  Vector);
		ZEVector3				GetRayleigh();
		
		void					SetMie(ZEVector3 Vector);
		ZEVector3				GetMie();
		
		void					SetG(float Value);
		float					GetG();

		void					SetLightScale(float Value);
		float					GetLightScale();
		
		void					SetAmbientScale(float Value);
		float					GetAmbientScale();
		
		void					SetEarthRadius(float Value);
		float					GetEarthRadius();
		
		void					SetAtmosphereHeight(float Value);
		float					GetAtmosphereHeight();

		void					SetCloudCover(float Value);
		float					GetCloudCover();

		void					SetCloudPlaneHeight(float Value);
		float					GetCloudPlaneHeight();

		void					SetWindVelocity(ZEVector2 Value);
		ZEVector2				GetWindVelocity();

		void					SetSunLightDirection(ZEVector3 Value);
		ZEVector3				GetSunLightDirection();

		void					SetCamera(ZECamera* Camera);
		ZECamera*				GetCamera();

		virtual void			SetCloudFormationTexture(const ZEString& FileName);
		const ZEString			GetCloudFormationTexture() const;

		virtual ZEDrawFlags		GetDrawFlags() const;

		virtual bool			Initialize();
		virtual void			Deinitialize();

		virtual void			Draw(ZEDrawParameters* DrawParameters);
		virtual void			Tick(float Time);

		static ZECloud*			CreateInstance();

}; // class ZECloud


#endif
