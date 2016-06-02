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

#include "ZEGame/ZEEntity.h"

#include "ZETypes.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEViewVolume.h"
#include "ZERNRenderer.h"
#include "ZERNView.h"
#include "ZERNCommand.h"

enum ZELightType				: ZEUInt8
{
	ZE_LT_NONE				= 0,
	ZE_LT_POINT				= 1,
	ZE_LT_DIRECTIONAL		= 2,
	ZE_LT_PROJECTIVE		= 3,
	ZE_LT_OMNIPROJECTIVE	= 4
};

enum ZELightShadowResolution	: ZEUInt8
{
	ZE_LSR_LOW				= 0,
	ZE_LSR_MEDIUM			= 1,
	ZE_LSR_HIGH				= 2,
	ZE_LSR_VERY_HIGH		= 3
};

enum ZELightShadowSampleCount	: ZEUInt8
{
	ZE_LSC_LOW				= 0,
	ZE_LSC_MEDIUM			= 1,
	ZE_LSC_HIGH				= 2,
	ZE_LSC_VERY_HIGH		= 3
};

class ZEGRTexture;

class ZELight : public ZEEntity
{
	ZE_OBJECT
	private:
		float							AttenuationFunction(float RootToTry);

	protected:
		mutable ZEFlags					DirtyFlags;

		bool							CastsShadows;

		ZELightShadowResolution			ShadowResolution;
		ZELightShadowSampleCount		ShadowSampleCount;
		float							ShadowSampleLength;
		float							ShadowDepthBias;
		float							ShadowNormalBias;

		float							Range;
		float							Intensity;

		ZEVector3						Color;
		ZEVector3						Attenuation;

		mutable ZEMatrix4x4				ViewTransform;
		mutable ZEMatrix4x4				ProjectionTransform;

		ZERNRenderer					ShadowRenderer;
		ZERNCommand						Command;

		virtual bool					InitializeSelf();
		virtual bool					DeinitializeSelf();

		void							CalculateBoundingBox() const;

		virtual void					LocalTransformChanged();
		virtual void					ParentTransformChanged();
		virtual void					BoundingBoxChanged();

										ZELight();
		virtual							~ZELight();

	public:
		virtual ZEDrawFlags				GetDrawFlags() const;

		virtual ZELightType				GetLightType() const = 0;
		virtual ZESize					GetViewCount() const = 0;

		void							SetCastsShadow(bool NewValue);
		bool							GetCastsShadow() const;

		void							SetRange(float NewValue);
		float							GetRange() const;

		void							SetIntensity(float NewValue);
		float							GetIntensity() const;

		void							SetShadowResolution(ZELightShadowResolution ShadowResolution);
		ZELightShadowResolution			GetShadowResolution() const;

		void							SetShadowSampleCount(ZELightShadowSampleCount ShadowSampleCount);
		ZELightShadowSampleCount		GetShadowSampleCount() const;

		void							SetShadowSampleLength(float ShadowSampleLength);
		float							GetShadowSampleLength() const;

		void							SetShadowDepthBias(float ShadowDepthBias);
		float							GetShadowDepthBias() const;

		void							SetShadowNormalBias(float ShadowNormalBias);
		float							GetShadowNormalBias() const;

		void							SetColor(const ZEVector3& NewColor);
		const ZEVector3&				GetColor() const;

		void							SetAttenuation(const ZEVector3& Attenuation);
		void							SetAttenuation(float DistanceSquare, float Distance, float Constant);
		const ZEVector3&				GetAttenuation() const;

		virtual const ZEAABBox&			GetBoundingBox() const;

		virtual ZEGRTexture*			GetShadowMap(ZESize Index = 0) const = 0;
		virtual const ZEViewVolume&		GetViewVolume(ZESize Index = 0) const = 0;
		virtual const ZEMatrix4x4&		GetViewTransform(ZESize Index = 0) const = 0;
		virtual const ZEMatrix4x4&		GetProjectionTransform(ZESize Index = 0) const = 0;
		
		virtual bool					PreRender(const ZERNCullParameters* CullParameters);

		static ZEUInt					ConvertShadowResolution(ZELightShadowResolution ShadowResolution);
		static ZEUInt					ConvertShadowSampleCount(ZELightShadowSampleCount ShadowSampleCount);
};
