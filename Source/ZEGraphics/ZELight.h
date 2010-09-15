//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELight.h
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

#include "ZEGame\ZEComponent.h"
#include "ZEViewVolume.h"

enum ZELightType
{
	ZE_LT_NONE = 0,
	ZE_LT_POINT = 1,
	ZE_LT_DIRECTIONAL = 2,
	ZE_LT_PROJECTIVE = 3,
	ZE_LT_OMNIPROJECTIVE = 4
};

class ZEShadowRenderer;
class ZEScene;
class ZELight : public ZEComponent
{
	private:
		bool							Enabled;

	protected:
		bool							UpdateViewVolume;
		bool							CastsShadows;
		ZEVector3						Color;
		float							Intensity;
		float							Range;
		ZEVector3						Attenuation;

	public:
		virtual ZELightType				GetLightType() = 0;

		virtual ZEDWORD					GetDrawFlags() const;

		virtual	void					SetPosition(const ZEVector3& NewPosition);
		virtual void					SetRotation(const ZEQuaternion& NewRotation);

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
		
		virtual void					RenderShadowMap(ZEScene* Scene, ZEShadowRenderer* ShadowRenderer) = 0;

		virtual const ZEViewVolume&		GetViewVolume() = 0;

										ZELight();
};	
#endif



