//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECamera.h
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
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZECanvas.h"
#include "ZEMath/ZEViewFrustum.h"
#include "ZEMath/ZEPlane.h"
#include "ZEMath/ZERectangle.h"
#include "ZEGraphics/ZEGRHolder.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEDS/ZEFlags.h"

typedef ZEFlags ZECameraDirtyFlags;
typedef ZEFlags ZECameraSettingFlags;

class ZECamera : public ZEEntity
{
	ZE_OBJECT

	private:
		mutable ZECameraDirtyFlags		CameraDirtyFlags;
		mutable ZERNView				View;

		ZEGRViewport					Viewport;
		ZEViewFrustum					ViewFrustum;
		bool							AutoAspectRatio;

										ZECamera();

		virtual bool					InitializeSelf();
		virtual bool					DeinitializeSelf();

		virtual void					OnTransformChanged();

	public:
		void							SetViewport(const ZEGRViewport& Viewport);
		const ZEGRViewport&				GetViewport() const;

		void							SetNearZ(float NearZ);
		float							GetNearZ() const;

		void							SetFarZ(float FarZ);
		float							GetFarZ() const;

		void							SetHorizontalFOV(float FOV);
		float							GetHorizontalFOV() const;

		void							SetVerticalFOV(float FOV);
		float							GetVerticalFOV() const;

		void							SetAutoAspectRatio(bool Enabled);
		bool							GetAutoAspectRatio() const;

		void							SetAspectRatio(float AspectRatio);
		float							GetAspectRatio() const;

		void							SetShadowDistance(float Value);
		float							GetShadowDistance() const;

		void							SetShadowFadeDistance(float Value);
		float							GetShadowFadeDistance() const;

		const ZEMatrix4x4&				GetViewTransform();
		const ZEMatrix4x4&				GetProjectionTransform();
		const ZEMatrix4x4&				GetViewProjectionTransform();

		const ZEMatrix4x4&				GetInvViewTransform();
		const ZEMatrix4x4&				GetInvProjectionTransform();
		const ZEMatrix4x4&				GetInvViewProjectionTransform();

		const ZERNView&					GetView();
		const ZEViewVolume&				GetViewVolume();

		void							GetScreenRay(ZERay& Ray, ZEInt ScreenX, ZEInt ScreenY);
		ZEVector2						GetScreenPosition(const ZEVector3& WorldPosition);

		virtual void					SetPosition(const ZEVector3& NewPosition);	
		virtual void					SetRotation(const ZEQuaternion& NewRotation);

		static ZECamera*				CreateInstance();
};
