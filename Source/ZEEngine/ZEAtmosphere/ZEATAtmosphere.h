//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEATAtmosphere.h
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

#include "ZEDS/ZEFlags.h"
#include "ZEPointer/ZEHolder.h"
#include "ZERenderer/ZERNCommand.h"
#include "ZEATCommon.h"

class ZELightDirectional;
class ZEGRRenderStateData;
class ZEGRShader;
class ZEGRSampler;
class ZEGRContext;
class ZEGRConstantBuffer;
class ZEGRTexture2D;
class ZEGRTexture3D;
class ZEATSun;
class ZEATMoon;
class ZEATFog;
class ZEATCloud;
class ZEATSkyBox;

class ZEATAtmosphere : public ZEEntity
{
	private:
		ZEFlags							DirtyFlags;
		ZERNCommand						Command;
		ZEATObserver					Observer;

		ZEATSun*						Sun;
		ZEATMoon*						Moon;
		ZEATFog*						Fog;
		ZEATCloud*						Cloud;
		ZEATSkyBox*						Stars;

		ZEHolder<ZEGRShader>			ScreenCoverVertexShader;

		ZEHolder<ZEGRShader>			SkyPixelShader;
		ZEHolder<ZEGRRenderStateData>	SkyRenderStateData;
		ZEHolder<ZEGRConstantBuffer>	SkyConstantBuffer;

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

		ZEHolder<ZEGRSampler>			SamplerLinearClamp;

		ZEUInt							OrderCount;
		bool							UseMultipleScattering;

		ZELightDirectional*				SunLight;
		ZELightDirectional*				MoonLight;

		struct
		{
			ZEVector3					SunDirection;
			float						SunIntensity;
			ZEVector3					SunColor;
			float						Reserved0;

			ZEVector3					MoonDirection;
			float						MoonIntensity;
			ZEVector3					MoonColor;
			float						Reserved1;
		} Constants;

		struct
		{
			float						IndexZ;
			float						IndexW;
			ZEVector2					Reserved0;
		} PrecomputeConstants;

		void							CreateRandomVectors();

		bool							UpdateShaders();
		bool							UpdateRenderState();
		bool							UpdateConstantBuffers();
		bool							Update();

		void							PrecomputeBuffers(ZEGRContext* Context);

		virtual bool					InitializeSelf();
		virtual bool					DeinitializeSelf();

										ZEATAtmosphere();
		virtual							~ZEATAtmosphere();

	public:
		virtual ZEDrawFlags				GetDrawFlags() const;

		void							SetObserver(const ZEATObserver& Observer);
		const ZEATObserver&				GetObserver() const;

		void							SetMultipleScattering(bool MultipleScattering);
		bool							GetMultipleScattering();

		void							SetSunLight(ZELightDirectional* SunLight);
		ZELightDirectional*				GetSunLight();

		void							SetMoonLight(ZELightDirectional* MoonLight);
		ZELightDirectional*				GetMoonLight();

		void							SetCloudCoverage(float CloudCoverage);
		float							GetCloudCoverage() const;

		void							SetCloudDensity(float CloudDensity);
		float							GetCloudDensity() const;

		void							SetFogDensity(float FogDensity);
		float							GetFogDensity() const;

		virtual void					Tick(float Time);
		virtual bool					PreRender(const ZERNCullParameters* CullParameters);
		virtual void					Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command);

		static ZEATAtmosphere*			CreateInstance();
};
