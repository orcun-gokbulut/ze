//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELightDirectional.h
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
#ifndef	__ZE_LIGHT_DIRECTIONAL_H__
#define __ZE_LIGHT_DIRECTIONAL_H__

#include "ZELight.h"
#include "ZETypes.h"
#include "ZERenderCommand.h"
#include "ZEShadowCascaded.h"

class ZEVertexBuffer;
class ZEDrawParameters;
class ZEMaterialLightDirectional;

class ZELightDirectional : public ZELight
{
	protected:
		bool							PropertyChanged;
		bool							TransformChanged;

		ZEVector3						Color;
		float							Intensity;
		
		float							PenumbraSize;
		float							BiasDepthScaled;
		float							BiasSlopeScaled;

		ZEShadowCascaded				CascadedShadow;

		ZEMaterialLightDirectional*		Material;
		ZEVertexBuffer*					VertexBuffer;
		ZEVertexLayout					VertexLayout;
		ZERenderCommandDefault			RenderCommand;

		virtual void					UpdateMaterial(const ZEDrawParameters* DrawParameters);

		virtual void					OnTransformChanged();

		virtual bool					InitializeSelf();
		virtual bool					DeinitializeSelf();

										ZELightDirectional();
		virtual							~ZELightDirectional();

	public:
		void							SetColor(const ZEVector3& NewColor);
		const ZEVector3&				GetColor() const;
		
		void							SetIntensity(float NewValue);
		float							GetIntensity() const;

		void							SetPenumbraSize(float Value);
		float							GetPenumbraSize() const;

		void							SetSlopeScaledBias(float Value);
		float							GetSlopeScaledBias() const;

		void							SetDepthScaledBias(float Value);
		float							GetDepthScaledBias() const;

		virtual ZELightType				GetLightType() const;

		virtual void					Tick(float Time);
		virtual void					Draw(ZEDrawParameters* DrawParameters);

		static ZELightDirectional*		CreateInstance();
};

#endif
