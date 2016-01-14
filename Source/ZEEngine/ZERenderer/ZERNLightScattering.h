//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNLightScattering.h
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

#include "ZEPointer/ZEHolder.h"
#include "ZEFoundation/ZEInitializable.h"
#include "ZEMath/ZEVector.h"
#include "ZEGraphics/ZEGRSamplerState.h"

class ZEGRRenderTarget;
class ZEGRRenderStateData;
class ZEGRShader;
class ZEGRContext;
class ZEGRConstantBuffer;
class ZEGRTexture2D;
class ZEGRTexture3D;

class ZERNLightScattering : public ZEInitializable
{
	private:
		ZEGRTexture2D*					InputTexture;
		ZEGRTexture2D*					DepthTexture;
		ZEGRRenderTarget*				OutputRenderTarget;
		
		ZEHolder<ZEGRShader>			VertexShader;
		ZEHolder<ZEGRShader>			PixelShader;
		ZEHolder<ZEGRRenderStateData>	RenderStateData;
		ZEHolder<ZEGRConstantBuffer>	ConstantBuffer;

		ZEHolder<ZEGRShader>			PrecomputeSingleScatteringPixelShader;
		ZEHolder<ZEGRShader>			PrecomputeHighOrderScatteringPixelShader;
		ZEHolder<ZEGRShader>			PrecomputeHighOrderInScatteringPixelShader;
		ZEHolder<ZEGRShader>			AddOrdersPixelShader;

		ZEHolder<ZEGRRenderStateData> PrecomputeSingleScatteringRenderStateData;
		ZEHolder<ZEGRRenderStateData> PrecomputeHighOrderScatteringRenderStateData;
		ZEHolder<ZEGRRenderStateData> PrecomputeHighOrderInScatteringRenderStateData;
		ZEHolder<ZEGRRenderStateData>	AddOrdersRenderStateData;

		ZEHolder<ZEGRTexture3D>		PrecomputedSingleScatteringBuffer;
		ZEHolder<ZEGRTexture3D>		PrecomputedHighOrderScatteringBuffer;
		ZEHolder<ZEGRTexture3D>		PrecomputedHighOrderInScatteringBuffer;
		ZEHolder<ZEGRTexture3D>		PrecomputedMultipleScatteringBuffer;

		ZEHolder<ZEGRConstantBuffer>	PrecomputeConstantBuffer;

		ZEGRSamplerState				SamplerLinearClamp;

		struct LightScatteringConstants
		{
			ZEVector3					LightDirection;
			float						LightIntensity;
			ZEVector3					LightColor;
			float						MieScatteringStrength;
		}Constants;

		struct PrecomputeScatteringConstants
		{
			float Index;
			float Index2;
			ZEVector2 Reserved;
		}PrecomputeConstants;

		void							CreateShaders();
		void							CreateRenderState();

		virtual bool					InitializeSelf();
		virtual void					DeinitializeSelf();

		void							PrecomputeWithPixelShader(ZEGRContext* Context);
		void							ReleasePrecomputeResources();

	public:
		void							SetInputTexture(ZEGRTexture2D* InputTexture);
		const ZEGRTexture2D*			GetInputTexture() const;

		void							SetDepthTexture(ZEGRTexture2D* DepthTexture);
		const ZEGRTexture2D*			GetDepthTexture() const;

		void							SetOutputRenderTarget(ZEGRRenderTarget* OutputRenderTarget);
		const ZEGRRenderTarget*			GetOutputRenderTarget() const;

		void							SetLightDirection(const ZEVector3& LightDirection);
		const ZEVector3&				GetLightDirection() const;

		void							SetLightIntensity(float LightIntensity);
		float							GetLightIntensity() const;

		void							SetLightColor(const ZEVector3& LightColor);
		const ZEVector3&				GetLightColor() const;

		void							SetMieScatteringStrengh(float MieScatteringStrength);
		float							GetMieScatteringStrengh() const;

		void							Process(ZEGRContext* Context, bool MultipleScattering);

										ZERNLightScattering();
		virtual							~ZERNLightScattering();
};
