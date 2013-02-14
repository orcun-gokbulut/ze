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
#ifndef	__ZE_DIRECTIONAL_LIGHT_H__
#define __ZE_DIRECTIONAL_LIGHT_H__

#include "ZELight.h"
#include "ZETypes.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEViewCuboid.h"

class ZETexture2D;
class ZERenderTarget;
struct ZEDrawParameters;

struct ZEDirectionalLightCascade
{
	ZEUInt				Index;
	float				FarZ;
	float				NearZ;
	float				Depth;
	ZEUInt				UpdateInterval;

	ZETexture2D*		ShadowMap;
	ZERenderTarget*		ShadowMapRenderTarget;
	ZEViewCuboid		ViewVolume;
	ZEMatrix4x4			ShadowTransform;
};

#define	ZE_DL_MAX_CASCADE_COUNT		5

class ZELightDirectional : public ZELight
{
	protected:
		ZESize							CascadeCount;
		ZEDirectionalLightCascade		Cascades[ZE_DL_MAX_CASCADE_COUNT];

		float							CascadeSplitBias;

		void							UpdateCascades();
		void							UpdateRenderTargets();
		void							DestroyRenderTargets();
		
										ZELightDirectional();
		virtual							~ZELightDirectional();

	public:
		void							SetCascadeCount(ZESize Value);
		ZESize							GetCascadeCount() const;
		
		void							SetSplitBias(float Value);
		float							GetSplitBias() const;

		float							GetFarZ(ZESize Index) const;
		float							GetNearZ(ZESize Index) const;
		float							GetDepth(ZESize Index) const;

		void							SetUpdateInterval(ZESize Index, ZEUInt Value);
		ZEUInt							GetUpdateInterval(ZESize Index) const;

		const ZETexture2D*				GetShadowMap(ZESize Index) const;
		const ZEMatrix4x4&				GetShadowTransform(ZESize Index) const;

		virtual const ZEViewVolume&		GetViewVolume(ZESize Index);

		virtual void					Draw(ZEDrawParameters* DrawParameters);
		
		virtual bool					Initialize();
		virtual void					Deinitialize();

		static ZELightDirectional*		CreateInstance();
};

#endif
