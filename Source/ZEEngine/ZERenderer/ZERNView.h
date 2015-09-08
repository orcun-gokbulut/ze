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

#include "ZETypes.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEGraphics/ZEGRHolder.h"
#include "ZEPacking.h"

class ZEViewVolume;
class ZEEntity;
class ZEGROutput;
class ZEGRViewport;
class ZEGRConstantBuffer;

enum ZERNProjectionType
{
	ZERN_PT_PERSPECTIVE,
	ZERN_PT_ORTHAGONAL,
	ZERN_PT_PARABOLOID,
};

enum ZERNViewType
{
	ZERN_VT_NONE,
	ZERN_VT_CAMERA,
	ZERN_VT_SHADOW_CASTER,
	ZERN_VT_PROBE
};

struct ZERNView
{
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
	float						Width;
	float						Height;
	float						VerticalFOV;		
	float						HorizontalFOV;
	float						AspectRatio;
	float						NearZ;
	float						FarZ;

	// Orientation
	ZEVector3					Position;
	ZEQuaternion				Rotation;
	ZEVector3					Direction;
	ZEVector3					U, V, N;

	// Others
	ZEGROutput*					Output;
	const ZEGRViewport*			Viewport;
	const ZEViewVolume*			ViewVolume;
};

struct ZERNViewConstantBuffer
{
	ZEMatrix4x4				ViewTransform;
	ZEMatrix4x4				ProjectionTransform;		
	ZEMatrix4x4				ViewProjectionTransform;
	ZEMatrix4x4				InvViewTransform;
	ZEMatrix4x4				InvProjectionTransform;			
	ZEMatrix4x4				InvViewProjectionTransform;

	float					Width;
	float					Height;
	float					VerticalFOV;
	float					HorizontalFOV;
	float					AspectRatio;
	float					NearZ;
	float					FarZ;
	float					Reserved0;

	ZEVector3				Position; float	Reserved1;
	ZEQuaternion			RotationQuaternion;
	ZEVector3				RotationEuler; float Reserved2;
	ZEVector3				U; float Reserved3;
	ZEVector3				V; float Reserved4;
	ZEVector3				N; float Reserved5;
};
