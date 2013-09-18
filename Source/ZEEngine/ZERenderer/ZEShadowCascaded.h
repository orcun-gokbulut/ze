//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEShadowCascaded.h
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

#ifndef __ZE_SHADOW_CASCADED_H__
#define __ZE_SHADOW_CASCADED_H__

#include "ZEView.h"
#include "ZETypes.h"
#include "ZEShadow.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEMath/ZEViewCuboid.h"

class ZEShadowCascaded;

class ZEViewCascaded : public ZEView
{
	protected:
		ZEUInt							CascadeIndex;
		const ZEShadowCascaded*			OwnerShadow;

		ZEViewCuboid					ShadowVolume;

		ZEMatrix4x4						ViewTransform;
		ZEMatrix4x4						ProjectionTransform;
		ZEMatrix4x4						ViewProjectionTransform;

	public:
										ZEViewCascaded();
		virtual							~ZEViewCascaded();

		void							SetCascadeIndex(ZEUInt Index);
		ZEUInt							GetCascadeIndex() const;

		void							SetOwnerShadow(const ZEShadowCascaded* Owner);
		const ZEShadowCascaded*			GetOwnerShadow() const;

		virtual void					Update(ZECamera* Camera, ZEUInt CascadeCount, float SplitBias, float ViewDepth);
		
		virtual ZEViewType				GetViewType() const;
		virtual const ZEViewVolume*		GetViewVolume() const;
		
		virtual const ZEMatrix4x4&		GetViewTransform() const;
		virtual const ZEMatrix4x4&		GetProjectionTransform() const;
		virtual const ZEMatrix4x4&		GetViewProjectionTransform() const;
};

class ZELight;
class ZECamera;
class ZETexture2D;
class ZERenderTarget;
class ZEDrawParameters;

#define ZE_SHADOW_MAX_CASCADE_COUNT		4

class ZEShadowCascaded : public ZEShadow
{
	friend class ZELightDirectional;

	protected:
		float							SplitBias;
		float							ViewDepth;
		ZEUInt							CascadeCount;
		
		ZETexture2D*					ShadowMaps[ZE_SHADOW_MAX_CASCADE_COUNT];
		ZERenderTarget*					RenderTargets[ZE_SHADOW_MAX_CASCADE_COUNT];
		ZEViewCascaded					CascadeViews[ZE_SHADOW_MAX_CASCADE_COUNT];

		float							CameraOldFarZ;
		float							CameraOldNearZ;
		float							CameraOldFov;
		float							CameraOldShadowRange;
		float							CameraOldAspectRatio;
		ZEVector3						CameraOldPosition;
		ZEQuaternion					CameraOldRotation;
		ZEQuaternion					LightOldRotation;

		void							UpdateBuffers();
		void							DestroyBuffers();

	public:
										ZEShadowCascaded();
		virtual							~ZEShadowCascaded();

		void							SetCascadeCount(ZEUInt Value);
		ZEUInt							GetCascadeCount() const;

		void							SetSplitBias(float Value);
		float							GetSplitBias() const;

		void							SetViewDepth(float Value);
		float							GetViewDepth() const;

		virtual ZEUInt					GetViewCount() const;
		virtual const ZEView*			GetView(ZESize Index) const;

		virtual bool					Initialize(ZELight* Light);
		virtual void					Deinitialize();

		virtual void					Update(ZECamera* Camera);
		virtual void					Draw(ZEDrawParameters* LightDrawParameters);
};

#endif
