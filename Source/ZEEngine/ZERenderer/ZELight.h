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

#include "ZETypes.h"
#include "ZEGame/ZEEntity.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEViewVolume.h"

enum ZELightType
{
	ZE_LT_NONE				= 0,
	ZE_LT_POINT				= 1,
	ZE_LT_DIRECTIONAL		= 2,
	ZE_LT_PROJECTIVE		= 3,
	ZE_LT_OMNIPROJECTIVE	= 4
};

class ZEShadowRenderer;
class ZEScene;

class ZELight : public ZEEntity
{
	ZE_OBJECT

	private:
		bool							Enabled;

		float							AttenuationFunction(float RootToTry);

	protected:
		bool							CastsShadows;
		bool							UpdateViewVolume;
		
		float							Range;
		float							Intensity;

		ZEVector3						Color;
		ZEVector3						Attenuation;

		virtual void					OnTransformChanged();

	public:
		virtual ZEDrawFlags				GetDrawFlags() const;

		virtual	void					SetPosition(const ZEVector3& NewPosition);
		virtual void					SetRotation(const ZEQuaternion& NewRotation);

		void							SetIntensity(float NewValue);
		float							GetIntensity() const;

		void							SetAttenuation(const ZEVector3& Attenuation);
		void							SetAttenuation(float DistanceSquare, float Distance, float Constant);
		const ZEVector3&				GetAttenuation() const;

		void							SetColor(const ZEVector3& NewColor);
		const ZEVector3&				GetColor() const;

		void							SetRange(float NewValue);
		float							GetRange() const;		

		virtual void					SetCastsShadow(bool NewValue);
		bool							GetCastsShadow() const;

		virtual ZELightType				GetLightType() = 0;
		virtual ZESize					GetViewCount() = 0;
		virtual const ZEViewVolume&		GetViewVolume(ZESize Index = 0) = 0;
		virtual const ZEMatrix4x4&		GetViewTransform(ZESize CascadeIndex = 0) = 0;

		virtual void					Draw(ZERNDrawParameters* DrawParameters);

										ZELight();
		virtual							~ZELight();
};

#endif
