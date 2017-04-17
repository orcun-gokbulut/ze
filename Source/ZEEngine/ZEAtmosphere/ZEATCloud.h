//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEATCloud.h
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
#include "ZEDS/ZEArray.h"
#include "ZEMath/ZEVector.h"
#include "ZEPointer/ZEHolder.h"
#include "ZERenderer/ZERNCommand.h"
#include "ZEExport.ZEEngine.h"

class ZEGRShader;
class ZEGRRenderStateData;
class ZEGRBuffer;
class ZEGRSampler;
class ZEGRContext;
class ZEGRDepthStencilBuffer;
class ZEGRRenderTarget;
class ZEGRTexture;

class ZE_EXPORT_ZEENGINE ZEATCloud : public ZEEntity
{
	ZE_OBJECT
	private:
		ZEFlags									DirtyFlags;
		ZERNCommand								RenderCommand;

		ZEHolder<ZEGRShader>					PlaneVertexShader;
		ZEHolder<ZEGRShader>					PlaneHullShader;
		ZEHolder<ZEGRShader>					PlaneDomainShader;
		ZEHolder<ZEGRShader>					PlanePixelShader;
		ZEHolder<ZEGRRenderStateData>			PlaneRenderStateData;
		ZEHolder<ZEGRBuffer>					PlaneVertexBuffer;

		ZEHolder<ZEGRBuffer>					ConstantBuffer;

		ZEHolder<const ZEGRTexture>				CloudTexture;

		struct  
		{
			ZEVector3							LightColor;
			float								PlaneSubdivision;

			ZEVector3							Inscattering;
			ZEUInt								CloudCoverage;

			ZEVector3							LightDirection;
			float								CloudDensity;

			ZEVector2							Translation;
			ZEVector2							TextureTileFactor;
		} Constants;

		void									CreatePlane();

		bool									UpdateShaders();
		bool									UpdateRenderStates();
		bool									UpdateConstantBuffers();
		bool									Update();

		virtual ZEEntityResult					LoadInternal();
		virtual ZEEntityResult					UnloadInternal();

												ZEATCloud();
		virtual									~ZEATCloud();

	public:
		void									SetCloudTexture(const ZEString& FileName);
		const ZEString&							GetCloudTexture() const;

		void									SetCloudCoverage(float CloudCoverage);
		float									GetCloudCoverage() const;

		void									SetCloudDensity(float CloudDensity);
		float									GetCloudDensity() const;

		void									SetLightColor(const ZEVector3& LightColor);
		const ZEVector3&						GetLightColor() const;

		void									SetInscattering(const ZEVector3& Inscattering);
		const ZEVector3&						GetInscattering() const;

		void									SetLightDirection(const ZEVector3& SunDirection);
		const ZEVector3&						GetLightDirection() const;

		void									SetTranslation(const ZEVector2& Translation);
		const ZEVector2&						GetTranslation() const;

		virtual bool							PreRender(const ZERNPreRenderParameters* Parameters);
		virtual void							Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command);

		static ZEATCloud*						CreateInstance();
}
ZEMT_ATTRIBUTE(ZEDEditor.ObjectWrapper.NameplateVisible, false)
ZEMT_ATTRIBUTE(ZEDEditor.ObjectWrapper3D.Pickable, false)
ZEMT_ATTRIBUTE(ZEDEditor.EntityWrapper3D.BoundingBoxVisible, false);
