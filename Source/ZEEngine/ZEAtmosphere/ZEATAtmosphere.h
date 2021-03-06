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
class ZEGRBuffer;
class ZEGRTexture;
class ZEATSun;
class ZEATMoon;
class ZEATFog;
class ZEATCloud;
class ZEATSkyBox;

class ZEATAtmosphere : public ZEEntity
{
	ZE_OBJECT
	private:
		ZEFlags							DirtyFlags;
		ZERNCommand						Command;
		ZEATObserver					Observer;
		//ZEVector4						SkyAmbient[1024];
		ZEVector3						TerrestrialSunColor;
		ZEVector3						TerrestrialSunAmbientColor;
		ZEVector3						TerrestrialMoonColor;
		ZEVector3						TerrestrialMoonAmbientColor;

		ZEATSun*						Sun;
		ZEATMoon*						Moon;
		ZEATFog*						Fog;
		ZEATCloud*						Cloud;
		ZEATSkyBox*						Stars;

		ZEHolder<ZEGRShader>			ScreenCoverVertexShader;

		ZEHolder<ZEGRShader>			SkyPixelShader;
		ZEHolder<ZEGRRenderStateData>	SkyRenderStateData;
		ZEHolder<ZEGRBuffer>			SkyConstantBuffer;

		ZEHolder<ZEGRShader>			PrecomputeExtinctionPixelShader;
		ZEHolder<ZEGRShader>			PrecomputeSingleScatteringPixelShader;
		ZEHolder<ZEGRShader>			PrecomputeHighOrderScatteringPixelShader;
		ZEHolder<ZEGRShader>			PrecomputeHighOrderInScatteringPixelShader;
		ZEHolder<ZEGRShader>			AddOrdersPixelShader;
		ZEHolder<ZEGRShader>			PrecomputeSkyAmbientPixelShader;

		ZEHolder<ZEGRRenderStateData>	PrecomputeExtinctionRenderStateData;
		ZEHolder<ZEGRRenderStateData>	PrecomputeSingleScatteringRenderStateData;
		ZEHolder<ZEGRRenderStateData>	PrecomputeHighOrderScatteringRenderStateData;
		ZEHolder<ZEGRRenderStateData>	PrecomputeHighOrderInScatteringRenderStateData;
		ZEHolder<ZEGRRenderStateData>	AddOrdersRenderStateData;
		ZEHolder<ZEGRRenderStateData>	PrecomputeSkyAmbientRenderStateData;

		ZEHolder<ZEGRTexture>			PrecomputedSingleScatteringBuffer;
		ZEHolder<ZEGRTexture>			PrecomputedMultipleScatteringBuffer;
		ZEHolder<ZEGRTexture>			PrecomputedExtinctionBuffer;

		ZEHolder<ZEGRTexture>			RandomVectorsTexture;

		ZEUInt							OrderCount;
		bool							UseMultipleScattering;

		struct
		{
			ZEVector3					SunDirection;
			float						Reserved0;
			ZEVector3					SunColor;
			float						Reserved1;

			ZEVector3					MoonDirection;
			float						Reserved2;
			ZEVector3					MoonColor;
			float						Reserved3;
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

		ZEVector3						GetAmbientColorFromLUT(float CosLightZenith);

		void							PrecomputeBuffers(ZEGRContext* Context);
		void							ComputeTerrestrialColors(const ZEVector3& SunDirection, const ZEVector3& MoonDirection);
		void							ComputeAmbientColors(float CosSunZenith, float CosMoonZenith);

		virtual ZEEntityResult			LoadInternal();
		virtual ZEEntityResult			UnloadInternal();

										ZEATAtmosphere();
		virtual							~ZEATAtmosphere();

	public:
		void							SetObserver(const ZEATObserver& Observer);
		const ZEATObserver&				GetObserver() const;

		void							SetMultipleScattering(bool MultipleScattering);
		bool							GetMultipleScattering();

		ZELightDirectional*				GetSunLight();

		ZELightDirectional*				GetMoonLight();

		void							SetCloudCoverage(float CloudCoverage);
		float							GetCloudCoverage() const;

		void							SetCloudDensity(float CloudDensity);
		float							GetCloudDensity() const;

		void							SetFogDensity(float FogDensity);
		float							GetFogDensity() const;

		ZEVector3						GetTerrestrialSunColor();
		ZEVector3						GetTerrestrialSunAmbientColor();
		ZEVector3						GetTerrestrialMoonColor();
		ZEVector3						GetTerrestrialMoonAmbientColor();

		virtual void					Tick(float Time);
		virtual bool					PreRender(const ZERNPreRenderParameters* Parameters);
		virtual void					Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command);

		static ZEATAtmosphere*			CreateInstance();
}
ZEMT_ATTRIBUTE(ZEDEditor.ObjectWrapper.Icon, "#R:/ZEDEditor/Icons/ZEDObjectWrapper/ZEATAtmosphere.png")
ZEMT_ATTRIBUTE(ZEDEditor.ObjectWrapper3D.Pickable, false)
ZEMT_ATTRIBUTE(ZEDEditor.ObjectWrapper3D.NameplateVisible, false)
ZEMT_ATTRIBUTE(ZEDEditor.ObjectWrapper3D.BoundingBoxVisible, false);
