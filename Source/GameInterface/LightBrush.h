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

#include "GameInterface/Entity.h"
#include "Graphics/Light.h"
#include "Graphics/RenderList.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/Material.h"

ZE_ENTITY_DESCRIPTION(ZELightBrush, ZEEntity);

class ZELightBrush : public ZEEntity
{
	ZE_ENTITY_CLASS(ZELightBrush)
	private:
		ZELightType							LightType;
		ZELight*							Light;
		ZETextureType						Type;

		ZERenderList						RenderList;
		ZEDefaultMaterial					Material;

		float								Intensity;
		ZEVector3							Color;
		float								Range;
		ZEVector3							Attenuation;
		bool								CastsShadow;
		ZETextureResource*					ProjectionTexture;
		ZECubeTextureResource*				CubeProjectionTexture;
		float								ProjectionFOV;
		float								ProjectionAspectRatio;

	public:
		virtual bool						IsDrawable();

		void								SetLightType(ZELightType LightType);
		ZELight*							GetLight();

		void								SetColor(const ZEVector3& NewColor);
		const ZEVector3&					GetColor();

		void								SetIntensity(float NewValue);
		float								GetIntensity();

		void								SetRange(float NewValue);
		float								GetRange();

		void								SetEnabled(bool NewValue);
		
		void								SetAttenuation(float DistanceSquare, float Distance, float Constant);
		const ZEVector3&					GetAttenuation();

		bool								IsCastingShadows();
		void								SetCastShadows(bool NewValue);

		void								SetProjectionTexture(const char* TextureFile);
		const char*							GetProjectionTexture();

		void								SetProjectionFOV(float NewValue);
		float								GetProjectionFOV();

		void								SetProjectionAspectRatio(float NewValue);
		float								GetProjectionAspectRatio();

		virtual void						Initialize();
		virtual void						Deinitialize();
		virtual void						Reset();

		virtual void						Tick(float Time);
		virtual void						Update();
		virtual void						Draw(ZERenderer* Renderer, const ZESmartArray<const ZERLLight*>& Lights);

		virtual bool						SetAttribute(const char* AttributeName, const ZEVariant& Value);
		virtual bool						GetAttribute(const char* AttributeName, ZEVariant& Value);

											ZELightBrush();
		virtual								~ZELightBrush();
};
#endif
