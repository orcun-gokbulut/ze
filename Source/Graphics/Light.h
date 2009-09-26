//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Light.h
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
#ifndef	__ZE_LIGHT_H__
#define __ZE_LIGHT_H__

#include "Core/Component.h"
#include "RenderOrder.h"
#include "ViewVolume.h"

enum ZELightType
{
	ZE_LT_NONE = 0,
	ZE_LT_POINT = 1,
	ZE_LT_DIRECTIONAL = 2,
	ZE_LT_PROJECTIVE = 3,
	ZE_LT_OMNIPROJECTIVE = 4
};

class ZEScene;
class ZELight : public ZEComponent
{
	private:
		bool							Enabled;

	protected:
		bool							UpdateViewVolume;
		bool							CastsShadows;
		ZERLLight						RenderOrderLight;

	public:
		virtual ZELightType				GetLightType() = 0;

		virtual bool					IsLight();

		virtual	void					SetLocalPosition(const ZEVector3& NewPosition);
		virtual void					SetLocalRotation(const ZEQuaternion& NewRotation);

		virtual void					OwnerWorldTransformChanged();

		void							SetColor(const ZEVector3& NewColor);
		const ZEVector3&				GetColor() const;

		void							SetIntensity(float NewValue);
		float							GetIntensity() const;

		void							SetRange(float NewValue);
		float							GetRange() const;

		void							SetEnabled(bool NewValue);
		bool							GetEnabled() const;
		
		void							SetAttenuation(float DistanceSquare, float Distance, float Constant);
		const ZEVector3&				GetAttenuation() const;

		virtual void					SetCastsShadows(bool NewValue);
		bool							GetCastsShadows() const;
		
		virtual void					RenderShadowMap(ZEScene* Scene, ZERenderer* ShadowRenderer) = 0;
		virtual const ZERLLight*		GetRenderOrderLight();

		virtual const ZEViewVolume&		GetViewVolume() = 0;

										ZELight();

};
		
class ZEPointLight  : public ZELight
{
	private:
		ZETextureCube*					ShadowMap;
		ZEViewSphere					ViewVolume;

	public:
		ZELightType						GetLightType();

		virtual void					SetCastShadows(bool NewValue);

		const ZETextureCube*			GetShadowMap();

		virtual void					RenderShadowMap(ZEScene* Scene, ZERenderer* ShadowRenderer);
		virtual const ZEViewVolume&		GetViewVolume();

										ZEPointLight();

};

class ZEDirectionalLight : public ZELight
{
	private:
		ZETexture2D*						ShadowMap;
		ZEViewPlane						ViewVolume;

	public:
		ZELightType						GetLightType();

		const ZETexture2D*				GetShadowMap();

		virtual void					RenderShadowMap(ZEScene* Scene, ZERenderer* ShadowRenderer);
		virtual const ZEViewVolume&		GetViewVolume();

										ZEDirectionalLight();
};

class ZEProjectiveLight : public ZELight
{
	private:
		float							FOV;
		float							AspectRatio;
		ZEViewFrustum					ViewVolume;

	public:
		ZELightType						GetLightType();

		void							SetFOV(float FOV);
		float							GetFOV();

		void							SetAspectRatio(float AspectRatio);
		float							GetAspectRatio();
 
		const ZETexture2D*				GetShadowMap();
		void							SetShadowMap(int Width, int Height);

		void							SetProjectionTexture(const ZETexture2D* Texture);
		const ZETexture2D*				GetProjectionTexture();

		virtual void					RenderShadowMap(ZEScene* Scene, ZERenderer* ShadowRenderer);
		virtual const ZERLLight*		GetRenderOrderLight();
		virtual const ZEViewVolume&		GetViewVolume();

										ZEProjectiveLight();
										~ZEProjectiveLight();

};

class ZEOmniProjectiveLight : public ZELight
{
	private:
		ZETextureCube*					ShadowMap;
		ZETextureCube*					ProjectionTexture;
		ZEViewSphere					ViewVolume;

	public:
		ZELightType						GetLightType();

		const ZETextureCube*			GetShadowMap();

		void							SetProjectionTexture(const ZETextureCube* Texture);
		const ZETextureCube*			GetProjectionTexture();

		virtual void					RenderShadowMap(ZEScene* Scene, ZERenderer* ShadowRenderer);
		virtual const ZEViewVolume&		GetViewVolume();

										ZEOmniProjectiveLight();

};

#endif
