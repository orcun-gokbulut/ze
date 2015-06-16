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
#ifndef	__ZE_CAMERA_H__
#define __ZE_CAMERA_H__

#include "ZEGame/ZEEntity.h"
#include "ZEGame/ZEDrawParameters.h"
#include "ZERenderer/ZERenderer.h"
#include "ZERenderer/ZERenderCommand.h"
#include "ZERenderer/ZECanvas.h"
#include "ZEMath/ZEViewFrustum.h"
#include "ZEMath/ZEPlane.h"
#include "ZEMath/ZERectangle.h"
#include "ZEDS/ZEFlags.h"

typedef ZEFlags ZECameraDirtyFlags;
typedef ZEFlags ZECameraSettingFlags;

enum ZECameraSettingType
{
	ZE_CST_NONE			= 0,
	ZE_CST_ASPECT_RATIO = 1,
	ZE_CST_NEAR_Z		= 2,
	ZE_CST_FAR_Z		= 3,
	ZE_CST_ALL			= 0xFFFFFFFF
};

//#define ZE_CSF_NONE			0
//#define ZE_CSF_ASPECT_RATIO	1
//#define ZE_CSF_NEAR_Z		2
//#define ZE_CSF_FAR_Z		4
//#define ZE_CSF_ALL			0xFFFFFFFF

enum ZECameraProjectionType
{
	ZE_CPT_NONE,
	ZE_CPT_ORTHOGRAPHICAL,
	ZE_CPT_PERSPECTIVE
};

class ZEViewPort;

class ZECamera : public ZEEntity
{
	ZE_OBJECT

	private:
		mutable ZECameraDirtyFlags		CameraDirtyFlags;

		float							NearZ, FarZ;
		float							VerticalFOV;
		float							AspectRatio;
		mutable float					EffectiveNearZ, EffectiveFarZ,  EffectiveAspectRatio;
		float							Width, Height;

		bool							AutoZ;
		bool							AutoAspectRatio;

		float							ShadowDistance;
		float							ShadowFadeDistance;

		ZEMatrix4x4						ViewTransform;
		ZEMatrix4x4						ProjectionTransform;
		ZEMatrix4x4						ViewProjectionTransform;

		ZEViewFrustum					ViewFrustum;
		ZEView							View;

										ZECamera();

		virtual void					OnTransformChanged();
		virtual void					UpdateAutoParameters();

	public:

		void							SetAutoZ(bool Enabled);
		bool							GetAutoZ();

		void							SetNearZ(float NearZ);
		float							GetNearZ() const;

		void							SetFarZ(float FarZ);
		float							GetFarZ() const;

		void							SetHorizontalFOV(float FOV);
		float							GetHorizontalFOV() const;

		void							SetVerticalFOV(float FOV);
		float							GetVerticalFOV() const;

		void							SetAutoAspectRatio(bool Enabled);
		bool							GetAutoAspectRatio();

		void							SetAspectRatio(float AspectRatio);
		float							GetAspectRatio() const;

		void							SetShadowDistance(float Value);
		float							GetShadowDistance() const;

		void							SetShadowFadeDistance(float Value);
		float							GetShadowFadeDistance() const;

		const ZEMatrix4x4&				GetViewTransform();
		const ZEMatrix4x4&				GetProjectionTransform();
		const ZEMatrix4x4&				GetViewProjectionTransform();

		virtual void					SetPosition(const ZEVector3& NewPosition);	
		virtual void					SetRotation(const ZEQuaternion& NewRotation);

		const ZEView&					GetView();
		const ZEViewVolume&				GetViewVolume();

		void							GetScreenRay(ZERay& Ray, ZEInt ScreenX, ZEInt ScreenY);
		//ZEVector2						GetScreenPosition(ZEVector3& WorldPosition);

		static ZECamera*				CreateInstance();
};
#endif
