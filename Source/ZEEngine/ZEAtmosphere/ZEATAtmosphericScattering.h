//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEATAtmosphericScattering.h
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

#include "ZEFoundation/ZEInitializable.h"
#include "ZEGraphics/ZEGRSamplerState.h"
#include "ZEGraphics/ZEGRHolder.h"
#include "ZEMath/ZEVector.h"

class ZEGRRenderTarget;
class ZEGRRenderStateData;
class ZEGRShader;
class ZEGRContext;
class ZEGRConstantBuffer;
class ZEGRTexture2D;
class ZEGRTexture3D;
class ZERNRenderer;

class ZEATAtmosphericScattering : public ZEInitializable
{
	private:
		ZEFlags							DirtyFlags;
		
		ZEGRHolder<ZEGRShader>			VertexShader;
		ZEGRHolder<ZEGRShader>			PixelShader;
		ZEGRHolder<ZEGRRenderStateData>	RenderStateData;
		ZEGRHolder<ZEGRConstantBuffer>	ConstantBuffer;

		ZEGRHolder<ZEGRShader>			PrecomputeDensityPixelShader;
		ZEGRHolder<ZEGRShader>			PrecomputeSingleScatteringPixelShader;
		ZEGRHolder<ZEGRShader>			PrecomputeHighOrderScatteringPixelShader;
		ZEGRHolder<ZEGRShader>			PrecomputeHighOrderInScatteringPixelShader;
		ZEGRHolder<ZEGRShader>			AddOrdersPixelShader;
		ZEGRHolder<ZEGRShader>			PrecomputeSkyAmbientPixelShader;

		ZEGRHolder<ZEGRRenderStateData>	PrecomputeDensityRenderStateData;
		ZEGRHolder<ZEGRRenderStateData> PrecomputeSingleScatteringRenderStateData;
		ZEGRHolder<ZEGRRenderStateData> PrecomputeHighOrderScatteringRenderStateData;
		ZEGRHolder<ZEGRRenderStateData> PrecomputeHighOrderInScatteringRenderStateData;
		ZEGRHolder<ZEGRRenderStateData>	AddOrdersRenderStateData;
		ZEGRHolder<ZEGRRenderStateData>	PrecomputeSkyAmbientRenderStateData;

		ZEGRHolder<ZEGRTexture2D>		PrecomputedDensityBuffer;
		ZEGRHolder<ZEGRTexture3D>		PrecomputedSingleScatteringBuffer;
		ZEGRHolder<ZEGRTexture3D>		PrecomputedHighOrderScatteringBuffer;
		ZEGRHolder<ZEGRTexture3D>		PrecomputedHighOrderInScatteringBuffer;
		ZEGRHolder<ZEGRTexture3D>		PrecomputedMultipleScatteringBuffer;
		ZEGRHolder<ZEGRTexture2D>		PrecomputedSkyAmbientBuffer;

		ZEGRHolder<ZEGRConstantBuffer>	PrecomputeConstantBuffer;

		ZEGRHolder<ZEGRTexture2D>		RandomVectorsTexture;

		ZEGRSamplerState				SamplerLinearClamp;

		ZEUInt							OrderCount;
		bool							UseMultipleScattering;

		struct
		{
			ZEVector3					LightDirection;
			float						LightIntensity;
			ZEVector3					LightColor;
			float						Reserved;
		}Constants;

		struct
		{
			float						IndexZ;
			float						IndexW;
			ZEVector2					Reserved;

			ZEVector4					CornetteShanksG;

			ZEVector4					RayleighAngularFactor;
			ZEVector4					MieAngularFactor;

			ZEVector4					RayleighScatteringFactor;
			ZEVector4					MieScatteringFactor;
		}PrecomputeConstants;

		void							CreateRandomVectors();

		bool							UpdateShaders();
		bool							UpdateRenderState();
		bool							UpdateConstantBuffers();
		bool							Update();

		void							ComputeRayleighMiePhaseFactors();
		void							PrecomputeWithPixelShader(ZEGRContext* Context);
		void							ReleasePrecomputeResources();

		virtual bool					InitializeSelf();
		virtual void					DeinitializeSelf();

	public:
		void							SetLightDirection(const ZEVector3& LightDirection);
		const ZEVector3&				GetLightDirection() const;

		void							SetLightIntensity(float LightIntensity);
		float							GetLightIntensity() const;

		void							SetLightColor(const ZEVector3& LightColor);
		const ZEVector3&				GetLightColor() const;

		void							SetOrderCount(ZEUInt OrderCount);
		ZEUInt							GetOrderCount() const;

		void							SetMultipleScattering(bool UseMultipleScattering);
		float							GetMultipleScattering() const;

		void							Process(ZEGRContext* Context);

										ZEATAtmosphericScattering();
		virtual							~ZEATAtmosphericScattering();
};
