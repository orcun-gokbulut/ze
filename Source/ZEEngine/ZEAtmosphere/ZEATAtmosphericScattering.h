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

#include "ZEInitializable.h"

#include "ZEDS/ZEFlags.h"
#include "ZEMath/ZEVector.h"
#include "ZEPointer/ZEHolder.h"
#include "ZEPointer/ZESharedPointer.h"

class ZEGRRenderTarget;
class ZEGRRenderStateData;
class ZEGRShader;
class ZEGRSampler;
class ZEGRContext;
class ZEGRConstantBuffer;
class ZEGRTexture2D;
class ZEGRTexture3D;
class ZERNRenderer;

class ZEATAtmosphericScattering : public ZEInitializable
{
	private:
		ZEFlags							DirtyFlags;
		
		ZEHolder<ZEGRShader>			VertexShader;
		ZEHolder<ZEGRShader>			PixelShader;
		ZEHolder<ZEGRRenderStateData>	RenderStateData;
		ZEHolder<ZEGRConstantBuffer>	ConstantBuffer;

		ZEHolder<ZEGRShader>			PrecomputeDensityPixelShader;
		ZEHolder<ZEGRShader>			PrecomputeSingleScatteringPixelShader;
		ZEHolder<ZEGRShader>			PrecomputeHighOrderScatteringPixelShader;
		ZEHolder<ZEGRShader>			PrecomputeHighOrderInScatteringPixelShader;
		ZEHolder<ZEGRShader>			AddOrdersPixelShader;
		ZEHolder<ZEGRShader>			PrecomputeSkyAmbientPixelShader;

		ZEHolder<ZEGRRenderStateData>	PrecomputeDensityRenderStateData;
		ZEHolder<ZEGRRenderStateData>	PrecomputeSingleScatteringRenderStateData;
		ZEHolder<ZEGRRenderStateData>	PrecomputeHighOrderScatteringRenderStateData;
		ZEHolder<ZEGRRenderStateData>	PrecomputeHighOrderInScatteringRenderStateData;
		ZEHolder<ZEGRRenderStateData>	AddOrdersRenderStateData;
		ZEHolder<ZEGRRenderStateData>	PrecomputeSkyAmbientRenderStateData;

		ZEHolder<ZEGRTexture2D>			PrecomputedDensityBuffer;
		ZEHolder<ZEGRTexture3D>			PrecomputedSingleScatteringBuffer;
		ZEHolder<ZEGRTexture3D>			PrecomputedHighOrderScatteringBuffer;
		ZEHolder<ZEGRTexture3D>			PrecomputedHighOrderInScatteringBuffer;
		ZEHolder<ZEGRTexture3D>			PrecomputedMultipleScatteringBuffer;
		ZEHolder<ZEGRTexture2D>			PrecomputedSkyAmbientBuffer;

		ZEHolder<ZEGRConstantBuffer>	PrecomputeConstantBuffer;

		ZEHolder<ZEGRTexture2D>			RandomVectorsTexture;

		ZESharedPointer<ZEGRSampler>	SamplerLinearClamp;

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
