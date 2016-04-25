//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNView.h
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

#include "ZEMeta/ZEObject.h"
#include "ZEMeta/ZEEnumerator.h"

#include "ZETypes.h"
#include "ZEPacking.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEPointer/ZEHolder.h"

#include "ZEGraphics/ZEGRViewport.h"

class ZEEntity;
class ZEViewVolume;

ZE_META_FORWARD_DECLARE(ZEGRViewport, "ZEGRViewport.h");

ZE_ENUM(ZERNViewType)
{
	ZERN_VT_NONE,
	ZERN_VT_CAMERA,
	ZERN_VT_SHADOW_CASTER,
	ZERN_VT_PROBE,
	ZERN_VT_VIEWPORT
};

ZE_ENUM(ZERNProjectionType)
{
	ZERN_PT_NONE,
	ZERN_PT_PERSPECTIVE,
	ZERN_PT_PERSPECTIVE_OFFCENTER,
	ZERN_PT_ORTHOGONAL,
	ZERN_PT_ORTHOGONAL_OFFCENTER,
	ZERN_PT_PARABOLOID
};

class ZERNView : public ZEObject
{
	ZE_OBJECT
	public:
		ZERNViewType				Type;
		ZEEntity*					Entity;

		// Transforms
		ZEMatrix4x4					ViewTransform;
		ZEMatrix4x4					ProjectionTransform;		
		ZEMatrix4x4					ViewProjectionTransform;
		ZEMatrix4x4					InvViewTransform;
		ZEMatrix4x4					InvProjectionTransform;			
		ZEMatrix4x4					InvViewProjectionTransform;

		// Parameters
		ZERNProjectionType			ProjectionType;
		float						VerticalFOV;
		float						VerticalFOVTop;
		float						VerticalFOVBottom;
		float						HorizontalFOV;
		float						HorizontalFOVRight;
		float						HorizontalFOVLeft;
		float						AspectRatio;
		float						NearZ;
		float						FarZ;
		float						ShadowDistance;
		float						ShadowFadeDistance;

		// Orientation
		ZEVector3					Position;
		ZEQuaternion				Rotation;
		ZEVector3					Direction;
		ZEVector3					U, V, N;

		// Others
		ZEGRViewport				Viewport;
		const ZEViewVolume*			ViewVolume;

									ZERNView();
};

struct ZERNViewConstantBuffer
{
	ZEMatrix4x4						ViewTransform;
	ZEMatrix4x4						ProjectionTransform;		
	ZEMatrix4x4						ViewProjectionTransform;
	ZEMatrix4x4						InvViewTransform;
	ZEMatrix4x4						InvProjectionTransform;			
	ZEMatrix4x4						InvViewProjectionTransform;

	ZEVector3						Position;
	float							Width;
	ZEQuaternion					RotationQuaternion;
	ZEVector3						RotationEuler;
	float							Height;

	ZEVector3						RightVector;
	float							VerticalFOV;
	ZEVector3						UpVector;
	float							HorizontalFOV;
	ZEVector3						FrontVector;
	float							AspectRatio;

	float							ShadowDistance;
	float							ShadowFadeDistance;
	float							NearZ;
	float							FarZ;
};
