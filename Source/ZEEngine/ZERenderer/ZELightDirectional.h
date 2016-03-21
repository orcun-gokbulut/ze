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

#include "ZELight.h"

#include "ZEMath/ZEViewCuboid.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEVector.h"

class ZEGRTexture2D;
class ZEGRConstantBuffer;
class ZEScene;

class ZELightDirectional : public ZELight
{
	ZE_OBJECT

	private:
		struct ZECascade
		{
			ZEMatrix4x4						ProjectionTransform;
			ZEVector4						Borders;
			ZEVector4						Band;
		};

		struct ZECascadeConstants
		{
			ZECascade						Cascades[4];
			ZEUInt							CascadeCount;
			ZEVector3						Reserved;
		}CascadeConstants;

		ZEHolder<ZEGRConstantBuffer>		CascadeConstantBuffer;
		ZEHolder<ZEGRTexture2D>				CascadeShadowMaps;
		ZEArray<ZEViewCuboid>				CascadeVolumes;

		ZEHolder<const ZEGRTexture2D>		DensityBuffer;
		ZEHolder<const ZEGRTexture2D>		AmbientBuffer;
		ZEHolder<const ZEGRTexture2D>		Scatteringbuffer;

		float								CascadeDistanceFactor;
		bool								UseSunLight;
		bool								UseMoonLight;

		void								UpdateCascadeTransforms(const ZERNView& View);
		void								UpdateCascadeShadowMaps();

											ZELightDirectional();
		virtual								~ZELightDirectional();

		virtual bool						InitializeSelf();
		virtual bool						DeinitializeSelf();

	public:
		virtual ZEDrawFlags					GetDrawFlags() const;
		virtual ZELightType					GetLightType() const;
		virtual ZESize						GetViewCount() const;

		void								SetCascadeCount(ZEUInt CascadeCount);
		ZEUInt								GetCascadeCount() const;

		void								SetCascadeDistanceFactor(float CascadeDistanceFactor);
		float								GetCascadeDistanceFactor() const;

		void								SetUseSunLight(bool UseSunLight);
		bool								GetUseSunLight() const;

		void								SetUseMoonLight(bool UseMoonLight);
		bool								GetUseMoonLight() const;

		void								SetDensityBuffer(const ZEGRTexture2D* DensityBuffer);
		const ZEGRTexture2D*				GetDensityBuffer() const;

		void								SetAmbientBuffer(const ZEGRTexture2D* AmbientBuffer);
		const ZEGRTexture2D*				GetAmbientBuffer() const;

		void								BindCascades(ZERNRenderer* Renderer, ZEGRContext* Context);

		virtual ZEGRTexture*				GetShadowMap(ZESize Index = 0) const;
		virtual const ZEViewVolume&			GetViewVolume(ZESize Index = 0) const;
		virtual const ZEMatrix4x4&			GetViewTransform(ZESize Index = 0) const;
		virtual const ZEMatrix4x4&			GetProjectionTransform(ZESize Index = 0) const;

		virtual bool						PreRender(const ZERNCullParameters* CullParameters);
		virtual void						Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command);

		static ZELightDirectional*			CreateInstance();
};
