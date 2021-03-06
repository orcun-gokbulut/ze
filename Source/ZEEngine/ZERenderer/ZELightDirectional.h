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

#include "ZEMath/ZEOBBox.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEVector.h"

class ZEGRTexture;
class ZEGRBuffer;

struct ZERNCascade
{
	ZEMatrix4x4								ViewTransform;
	ZEMatrix4x4								ProjectionTransform;
	ZEViewOBBox								ViewVolume;
	ZEVector4								Borders;
	ZEUInt									SampleCount;
	float									SampleLength;
	float									DepthBias;
	float									NormalBias;

											ZERNCascade();
};

class ZERNCommandDirectionalLight : public ZERNCommand
{
	ZE_OBJECT
	public:
		ZEQuaternion						RotationWorld;
		ZEVector3							Color;
		ZEBool32							CastShadow;
		ZEArray<ZERNCascade>				Cascades;
};

class ZELightDirectional : public ZELight
{
	ZE_OBJECT
	private:
		ZERNCommandDirectionalLight			Command;
		ZEArray<ZERNCascade>				Cascades;

		float								CascadeDistanceFactor;

		bool								IsTerrestrial;
		ZEVector3							TerrestrialColor;
		float								TerrestrialIntensity;

		void								UpdateCascadeTransforms(const ZERNView& View, ZEUInt ShadowMapSize);

		virtual ZEEntityResult				LoadInternal();
		virtual ZEEntityResult				UnloadInternal();

											ZELightDirectional();
		virtual								~ZELightDirectional();

	public:
		virtual ZELightType					GetLightType() const;
		virtual ZESize						GetViewCount() const;

		void								SetCascadeCount(ZEUInt CascadeCount);
		ZEUInt								GetCascadeCount() const;

		void								SetCascadeDistanceFactor(float CascadeDistanceFactor);
		float								GetCascadeDistanceFactor() const;

		void								SetCascadeDepthBias(ZEUInt CascadeIndex, float CascadeDepthBias);
		float								GetCascadeDepthBias(ZEUInt CascadeIndex) const;

		void								SetCascadeNormalBias(ZEUInt CascadeIndex, float CascadeNormalBias);
		float								GetCascadeNormalBias(ZEUInt CascadeIndex) const;

		void								SetIsTerrestrial(bool Value);
		bool								GetIsTerrestrial() const;

		void								SetTerrestrialColor(const ZEVector3& TerrestrialColor);
		const ZEVector3&					GetTerrestrialColor() const;

		void								SetTerrestrialIntensity(float TerrestrialIntensity);
		float								GetTerrestrialIntensity() const;

		virtual const ZEViewVolume&			GetViewVolume(ZESize Index = 0) const;
		virtual const ZEMatrix4x4&			GetViewTransform(ZESize Index = 0) const;
		virtual const ZEMatrix4x4&			GetProjectionTransform(ZESize Index = 0) const;

		virtual bool						PreRender(const ZERNPreRenderParameters* Parameters);

		static ZELightDirectional*			CreateInstance();
}
ZEMT_ATTRIBUTE(ZEDEditor.ObjectWrapper.Icon, "#R:/ZEDEditor/Icons/ZEDObjectWrapper/ZELightDirectional.png")
ZEMT_ATTRIBUTE(ZEDEditor.ObjectWrapper3D.NameplateVisible, false)
ZEMT_ATTRIBUTE(ZEDEditor.ObjectWrapper3D.BoundingBoxVisible, false);
