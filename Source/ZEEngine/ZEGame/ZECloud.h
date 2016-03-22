//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECloud.h
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

#include "ZEEntity.h"

#include "ZETypes.h"
#include "ZEDS/ZEArray.h"
#include "ZEMath/ZEVector.h"
#include "ZEPointer/ZEHolder.h"
#include "ZERenderer/ZERNCommand.h"

ZE_META_FORWARD_DECLARE(ZETexture2DResource, "ZETexture/ZETexture2DResource.h")

class ZEGRShader;
class ZEGRRenderStateData;
class ZEGRConstantBuffer;
class ZEGRVertexBuffer;
class ZEGRSampler;
class ZEGRContext;
class ZEGRTexture2D;
class ZEGRDepthStencilBuffer;
class ZEGRRenderTarget;

class ZECloud : public ZEEntity
{
	ZE_OBJECT

	private:
		ZEFlags							DirtyFlags;
		ZERNCommand						RenderCommand;

		ZEHolder<ZEGRShader>			PlaneVertexShader;
		ZEHolder<ZEGRShader>			PlaneHullShader;
		ZEHolder<ZEGRShader>			PlaneDomainShader;
		ZEHolder<ZEGRShader>			PlanePixelShader;
		ZEHolder<ZEGRRenderStateData>	PlaneRenderStateData;
		ZEHolder<ZEGRVertexBuffer>		PlaneVertexBuffer;
		ZEHolder<ZEGRConstantBuffer>	PlaneConstantBuffer;

		ZEHolder<ZEGRShader>			BlurVertexShader;
		ZEHolder<ZEGRShader>			BlurPixelShader;
		ZEHolder<ZEGRRenderStateData>	BlurRenderStateData;

		ZEHolder<ZEGRShader>			LightingPixelShader;
		ZEHolder<ZEGRRenderStateData>	LightingRenderStateData;

		ZEHolder<ZEGRConstantBuffer>	ConstantBuffer;

		ZEHolder<ZEGRSampler>			SamplerLinearWrap;
		
		ZEHolder<ZEGRConstantBuffer>	VerticalConstantBuffer;
		ZEHolder<ZEGRConstantBuffer>	HorizontalConstantBuffer;

		ZEArray<ZEVector4>				HorizontalValues;
		ZEArray<ZEVector4>				VerticalValues;

		ZEHolder<ZEGRTexture2D>			TempTexture;
		ZEHolder<ZEGRTexture2D>			BlurredTexture;
		ZETexture2DResource*			CloudTexture;

		struct  
		{
			float						PlaneSubdivision;
			float						CloudCoverage;
			float						CloudDensity;
			float						Reserved;

			ZEVector3					SunDirection;
			float						SunIntensity;

			ZEVector2					Translation;
			ZEVector2					Reserved1;
		} Constants;

		void							CreatePlane();

		bool							UpdateShaders();
		bool							UpdateRenderStates();
		bool							UpdateConstantBuffers();
		bool							Update();

		void							RenderClouds(ZEGRContext* Context, const ZEGRRenderTarget* RenderTarget, const ZEGRDepthStencilBuffer* DepthStencilBuffer);
		void							ApplyBlur(ZEGRContext* Context, ZEGRTexture2D* OutputTexture, ZEGRDepthStencilBuffer* DepthStencilBuffer);
		void							LightingClouds(ZEGRContext* Context, ZEGRTexture2D* OutputTexture, ZEGRDepthStencilBuffer* DepthStencilBuffer);

		virtual bool					InitializeSelf();
		virtual bool					DeinitializeSelf();

										ZECloud();

	public:
		virtual ZEDrawFlags				GetDrawFlags() const;

		void							SetCloudTexture(const ZEString& FileName);
		const ZEString&					GetCloudTexture() const;

		void							SetSunDirection(const ZEVector3& SunDirection);
		const ZEVector3&				GetSunDirection() const;

		void							SetCloudCoverage(float CloudCoverage);
		float							GetCloudCoverage() const;

		void							SetCloudDensity(float CloudDensity);
		float							GetCloudDensity() const;

		void							SetTranslation(const ZEVector2& Translation);
		const ZEVector2&				GetTranslation() const;

		virtual void					Tick(float Time);
		virtual bool					PreRender(const ZERNCullParameters* CullParameters);
		virtual void					Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command);

		virtual							~ZECloud();

		static ZECloud*					CreateInstance();
};
