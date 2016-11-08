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
	ZE_FV_LEFT_BOTTOM_NEAR = 0,
	ZE_FV_LEFT_BOTTOM_FAR,
	ZE_FV_LEFT_TOP_NEAR,
	ZE_FV_LEFT_TOP_FAR,
	ZE_FV_RIGHT_BOTTOM_NEAR,
	ZE_FV_RIGHT_BOTTOM_FAR,
	ZE_FV_RIGHT_TOP_NEAR,
	ZE_FV_RIGHT_TOP_FAR,
};

enum ZEFrustumEdge
{
	ZE_FE_LEFT_BOTTOM = 0,
	ZE_FE_LEFT_TOP,
	ZE_FE_LEFT_NEAR,
	ZE_FE_LEFT_FAR,
	ZE_FE_RIGHT_BOTTOM,
	ZE_FE_RIGHT_TOP,
	ZE_FE_RIGHT_NEAR,
	ZE_FE_RIGHT_FAR,
	ZE_FE_BOTTOM_NEAR,
	ZE_FE_BOTTOM_FAR,
	ZE_FE_TOP_NEAR,
	ZE_FE_TOP_FAR,

};

enum ZEFrustumPlane
{
	ZE_FP_LEFT = 0,
	ZE_FP_RIGHT,
	ZE_FP_BOTTOM,
	ZE_FP_TOP,
	ZE_FP_NEAR,
	ZE_FP_FAR,
};

enum ZEIntersectionResult
{
	ZE_IR_NONE,
	ZE_IR_INTERSECT,
	ZE_IR_INSIDE,
	ZE_IR_COVERS,
};

class ZEFrustum
{
	public:
		ZEPlane								LeftPlane; 
		ZEPlane								RightPlane;
		ZEPlane								BottomPlane;
		ZEPlane								TopPlane;
		ZEPlane								NearPlane;
		ZEPlane								FarPlane;

		ZEVector3							GetPosition() const;

		ZEVector3							GetLook() const;
		ZEVector3							GetRight() const;
		ZEVector3							GetUp() const;

		ZEPlane&							GetPlane(ZEFrustumPlane Plane);
		const ZEPlane&						GetPlane(ZEFrustumPlane Plane) const;
		ZELineSegment						GetEdge(ZEFrustumEdge Edge) const;
		ZEVector3							GetVertex(ZEFrustumVertex Vertex) const;

											ZEFrustum();
											ZEFrustum(const ZEVector3& Position, const ZEQuaternion& Rotation, float FOV, float AspectRatio, float NearZ, float FarZ);
											ZEFrustum(const ZEPlane& LeftClippingPlane, const ZEPlane& RightClippingPlane, 
												const ZEPlane& DownClippingPlane, const ZEPlane& UpClippingPlane, 
												const ZEPlane& NearClippingPlane, const ZEPlane& FarClippingPlane);

		static bool							IntersectionTest(const ZEFrustum& Frustum, const ZEVector3& Point);
		static bool							IntersectionTest(const ZEFrustum& Frustum, const ZEBSphere& BoundingSphere);
		static bool							IntersectionTest(const ZEFrustum& Frustum, const ZEAABBox& BoundingBox);
		static bool							IntersectionTest(const ZEFrustum& Frustum, const ZEOBBox& BoundingBox);
		static bool							IntersectionTest(const ZEFrustum& Frustum, const ZERectangle3D& Rectangle);
		static ZEIntersectionResult			IntersectionTestExact(const ZEFrustum& Frustum, const ZEAABBox& BoundingBox);
		static ZEIntersectionResult			IntersectionTestExact(const ZEFrustum& Frustum, const ZEOBBox& BoundingBox);

		static void							Create(ZEFrustum& Frustum, const ZEVector3& Position, const ZEQuaternion& Rotation, float FOV, float AspectRatio, float NearZ, float FarZ);
		static void							Create(ZEFrustum& Frustum,
												const ZEPlane& LeftPlane, const ZEPlane& RightPlane, 
												const ZEPlane& BottomPlane, const ZEPlane& TopPlane, 
												const ZEPlane& NearPlane, const ZEPlane& FarPlane);
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
