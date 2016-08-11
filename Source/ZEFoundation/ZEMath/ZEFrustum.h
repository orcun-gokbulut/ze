//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFrustum.h
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
#ifndef __ZE_VIEW_FRUSTUM_H__
#define __ZE_VIEW_FRUSTUM_H__

#include "ZEViewVolume.h"

#include "ZEDS/ZEArray.h"
#include "ZEVector.h"
#include "ZEPlane.h"

class ZEQuaternion;

enum ZEFrustumVertex
{
	ZE_FV_LEFT_DOWN_NEAR = 0,
	ZE_FV_LEFT_DOWN_FAR,
	ZE_FV_LEFT_UP_NEAR,
	ZE_FV_LEFT_UP_FAR,
	ZE_FV_RIGHT_DOWN_FAR,
	ZE_FV_RIGHT_DOWN_NEAR,
	ZE_FV_RIGHT_UP_NEAR,
	ZE_FV_RIGHT_UP_FAR
};

enum ZEFrustumEdge
{
	ZE_FE_LEFT_DOWN = 0,
	ZE_FE_LEFT_UP,
	ZE_FE_LEFT_NEAR,
	ZE_FE_LEFT_FAR,
	ZE_FE_RIGHT_DOWN,
	ZE_FE_RIGHT_UP,
	ZE_FE_RIGHT_NEAR,
	ZE_FE_RIGHT_FAR,
	ZE_FE_DOWN_NEAR,
	ZE_FE_DOWN_FAR,
	ZE_FE_UP_NEAR,
	ZE_FE_UP_FAR

};

enum ZEFrustumPlane
{
	ZE_FP_LEFT = 0,
	ZE_FP_RIGHT,
	ZE_FP_DOWN,
	ZE_FP_UP,
	ZE_FP_NEAR,
	ZE_FP_FAR,
};

class ZEFrustum
{
	public:
		ZEPlane								LeftPlane; 
		ZEPlane								RightPlane;
		ZEPlane								DownPlane;
		ZEPlane								UpPlane;
		ZEPlane								NearPlane;
		ZEPlane								FarPlane;

		ZEVector3							GetPosition() const;

		ZEVector3							GetLook() const;
		ZEVector3							GetRight() const;
		ZEVector3							GetUp() const;

		const ZEPlane&						GetPlane(ZEFrustumPlane Plane) const;
		ZELineSegment						GetEdge(ZEFrustumEdge Edge) const;
		ZEVector3							GetVertex(ZEFrustumVertex Vertex) const;

											ZEFrustum();
											ZEFrustum(const ZEVector3& Position, const ZEQuaternion& Rotation, float FOV, float AspectRatio, float NearZ, float FarZ);
											ZEFrustum(const ZEPlane& LeftClippingPlane, const ZEPlane& RightClippingPlane, 
												const ZEPlane& DownClippingPlane, const ZEPlane& UpClippingPlane, 
												const ZEPlane& NearClippingPlane, const ZEPlane& FarClippingPlane);

		static void							Create(ZEFrustum& Frustum, const ZEVector3& Position, const ZEQuaternion& Rotation, float FOV, float AspectRatio, float NearZ, float FarZ);
		static void							Create(ZEFrustum& Frustum,
												const ZEPlane& LeftPlane, const ZEPlane& RightPlane, 
												const ZEPlane& DownPlane, const ZEPlane& UpPlane, 
												const ZEPlane& NearPlane, const ZEPlane& FarPlane);

		static bool							IntersectionTest(const ZEFrustum& Frustum, const ZEBSphere& BoundingSphere);
		static bool							IntersectionTest(const ZEFrustum& Frustum, const ZEAABBox& BoundingBox);
		static bool							IntersectionTest(const ZEFrustum& Frustum, const ZEOBBox& BoundingBox);
		static bool							IntersectionTest(const ZEFrustum& Frustum, const ZERectangle3D& Rectangle);

};

class ZEViewFrustum : public ZEFrustum, public ZEViewVolume
{
public:
		virtual ZEViewVolumeType			GetViewVolumeType() const;

		virtual bool						IntersectionTest(const ZEBSphere& BoundingSphere) const;
		virtual bool						IntersectionTest(const ZEAABBox& BoundingBox) const;
		virtual bool						IntersectionTest(const ZEOBBox& BoundingBox) const;
		virtual bool						IntersectionTest(const ZERectangle3D& Rectangle) const;

											ZEViewFrustum();
											ZEViewFrustum(const ZEVector3& Position, const ZEQuaternion& Rotation, float FOV, float AspectRatio, float NearZ, float FarZ);
											ZEViewFrustum(const ZEPlane& LeftPlane, const ZEPlane& RightPlane, 
												const ZEPlane& DownPlane, const ZEPlane& UpPlane, 
												const ZEPlane& NearPlane, const ZEPlane& FarPlane);
};

#endif
