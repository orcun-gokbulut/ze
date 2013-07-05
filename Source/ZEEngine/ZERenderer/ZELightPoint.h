//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELightPoint.h
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

#ifndef	__ZE_LIGHT_POINT_H__
#define __ZE_LIGHT_POINT_H__

#include "ZELight.h"
#include "ZERenderCommand.h"
#include "ZEMath/ZEViewSphere.h"
#include "ZEGraphics/ZEConstantBuffer.h"

class ZETexture2D;
class ZEMaterialLightPoint;

class ZELightPoint  : public ZELight
{
	protected:
		float							Range;
		ZEVector3						Color;
		float							Intensity;
		ZEVector3						Attenuation;

		float							PenumbraSize;
		float							SlopeScaledBias;
		float							DepthScaledBias;

		ZEViewSphere					ViewVolume;
		ZEVertexLayout					VertexLayout;
		ZEVertexBuffer*					Vertices;
		ZEMaterialLightPoint*			Material;
		ZERenderCommandDefault			RenderCommand;
		
		virtual void					UpdateMaterial(const ZEDrawParameters* DrawParameters);

		virtual bool					InitializeSelf();
		virtual bool					DeinitializeSelf();

										ZELightPoint();
		virtual							~ZELightPoint();

	public:
		void							SetRange(float NewValue);
		float							GetRange() const;

		void							SetColor(const ZEVector3& NewColor);
		const ZEVector3&				GetColor() const;
		
		void							SetIntensity(float NewValue);
		float							GetIntensity() const;

		void							SetAttenuation(const ZEVector3& Attenuation);
		void							SetAttenuation(float DistanceSquare, float Distance, float Constant);
		const ZEVector3&				GetAttenuation() const;

		void							SetPenumbraSize(float Value);
		float							GetPenumbraSize() const;

		void							SetSlopeScaledBias(float Value);
		float							GetSlopeScaledBias() const;

		void							SetDepthScaledBias(float Value);
		float							GetDepthScaledBias() const;

		virtual void					Tick(float Time);
		virtual void					Draw(ZEDrawParameters* DrawParameters);
		
		virtual const ZEViewVolume*		GetLightVolume();

		static ZELightPoint*			CreateInstance();
};

#endif
