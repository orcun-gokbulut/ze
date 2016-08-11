//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFrustum.cpp
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

#include "ZEFrustum.h"

#include "ZEError.h"
#include "ZEVector.h"
#include "ZEQuaternion.h"
#include "ZEMath.h"
#include "ZEAngle.h"
#include "ZEBSphere.h"
#include "ZEAABBox.h"
#include "ZEOBBox.h"
#include "ZELineSegment.h"
#include "ZERectangle3D.h"

// ZEFrustum
//////////////////////////////////////////////////////////////////////////////////////

ZEVector3 ZEFrustum::GetPosition() const
{
	return LeftPlane.p;
}

ZEVector3 ZEFrustum::GetLook() const
{
	return NearPlane.n;
}

ZEVector3 ZEFrustum::GetRight() const
{
	ZEVector3 Temp;
	ZEVector3::CrossProduct(Temp, NearPlane.n, DownPlane.n);
	Temp.NormalizeSelf();
	return Temp;
}

ZEVector3 ZEFrustum::GetUp() const
{
	ZEVector3 Temp;
	ZEVector3::CrossProduct(Temp, NearPlane.n, LeftPlane.n);
	Temp.NormalizeSelf();
	return Temp;
}

const ZEPlane& ZEFrustum::GetPlane(ZEFrustumPlane Plane) const
{
	switch(Plane)
	{
		default:
			zeDebugCheck(true, "Wrong frustum plane");
			return ZEPlane(ZEVector3::Zero, ZEVector3::Zero);

		case ZE_FP_LEFT:
			return LeftPlane;
		
		case ZE_FP_RIGHT:
			return RightPlane;

		case ZE_FP_DOWN:
			return DownPlane;

		case ZE_FP_UP:
			return UpPlane;

		case ZE_FP_NEAR:
			return NearPlane;

		case ZE_FP_FAR:
			return FarPlane;
	}
}

ZELineSegment ZEFrustum::GetEdge(ZEFrustumEdge Edge) const
{
	ZELine Line;
	float t0, t1;
	switch(Edge)
	{
		case ZE_FE_LEFT_DOWN:
			ZEPlane::IntersectionTest(LeftPlane, DownPlane, Line);
			ZEPlane::IntersectionTest(NearPlane, Line, t0);
			ZEPlane::IntersectionTest(FarPlane, Line, t1);
			break;
		
		case ZE_FE_LEFT_UP:
			ZEPlane::IntersectionTest(LeftPlane, UpPlane, Line);
			ZEPlane::IntersectionTest(NearPlane, Line, t0);
			ZEPlane::IntersectionTest(FarPlane, Line, t1);
			break;

		case ZE_FE_LEFT_NEAR:
			ZEPlane::IntersectionTest(LeftPlane, NearPlane, Line);
			ZEPlane::IntersectionTest(DownPlane, Line, t0);
			ZEPlane::IntersectionTest(UpPlane, Line, t1);
			break;

		case ZE_FE_LEFT_FAR:
			ZEPlane::IntersectionTest(LeftPlane, FarPlane, Line);
			ZEPlane::IntersectionTest(DownPlane, Line, t0);
			ZEPlane::IntersectionTest(UpPlane, Line, t1);
			break;

		case ZE_FE_RIGHT_DOWN:
			ZEPlane::IntersectionTest(RightPlane, DownPlane, Line);
			ZEPlane::IntersectionTest(NearPlane, Line, t0);
			ZEPlane::IntersectionTest(FarPlane, Line, t1);
			break;

		case ZE_FE_RIGHT_UP:
			ZEPlane::IntersectionTest(RightPlane, UpPlane, Line);
			ZEPlane::IntersectionTest(NearPlane, Line, t0);
			ZEPlane::IntersectionTest(FarPlane, Line, t1);
			break;


		case ZE_FE_RIGHT_NEAR:
			ZEPlane::IntersectionTest(RightPlane, NearPlane, Line);
			ZEPlane::IntersectionTest(DownPlane, Line, t0);
			ZEPlane::IntersectionTest(UpPlane, Line, t1);
			break;

		case ZE_FE_RIGHT_FAR:
			ZEPlane::IntersectionTest(RightPlane, FarPlane, Line);
			ZEPlane::IntersectionTest(DownPlane, Line, t0);
			ZEPlane::IntersectionTest(UpPlane, Line, t1);
			break;

		case ZE_FE_DOWN_NEAR:
			ZEPlane::IntersectionTest(DownPlane, NearPlane, Line);
			ZEPlane::IntersectionTest(LeftPlane, Line, t0);
			ZEPlane::IntersectionTest(RightPlane, Line, t1);
			break;

		case ZE_FE_DOWN_FAR:
			ZEPlane::IntersectionTest(DownPlane, FarPlane, Line);
			ZEPlane::IntersectionTest(LeftPlane, Line, t0);
			ZEPlane::IntersectionTest(RightPlane, Line, t1);
			break;
			
		case ZE_FE_UP_NEAR:
			ZEPlane::IntersectionTest(UpPlane, NearPlane, Line);
			ZEPlane::IntersectionTest(LeftPlane, Line, t0);
			ZEPlane::IntersectionTest(RightPlane, Line, t1);
			break;

		case ZE_FE_UP_FAR:
			ZEPlane::IntersectionTest(UpPlane, FarPlane, Line);
			ZEPlane::IntersectionTest(LeftPlane, Line, t0);
			ZEPlane::IntersectionTest(RightPlane, Line, t1);
			break;

		default:
			zeDebugCheck(true, "Wrong frustum edge");
			t0 = 0.0f;
			t1 = 0.0f;
			break;
	}

	return ZELineSegment(Line.GetPointOn(t0), Line.GetPointOn(t1));
}

ZEVector3 ZEFrustum::GetVertex(ZEFrustumVertex Vertex) const
{
	switch(Vertex)
	{
		case ZE_FV_LEFT_DOWN_NEAR:
			return GetEdge(ZE_FE_LEFT_DOWN).GetStartPoint();

		case ZE_FV_LEFT_DOWN_FAR:
			return GetEdge(ZE_FE_LEFT_DOWN).GetEndPoint();

		case ZE_FV_LEFT_UP_NEAR:
			return GetEdge(ZE_FE_LEFT_UP).GetStartPoint();

		case ZE_FV_LEFT_UP_FAR:
			return GetEdge(ZE_FE_LEFT_UP).GetEndPoint();

		case ZE_FV_RIGHT_DOWN_NEAR:
			return GetEdge(ZE_FE_RIGHT_DOWN).GetStartPoint();

		case ZE_FV_RIGHT_DOWN_FAR:
			return GetEdge(ZE_FE_RIGHT_DOWN).GetEndPoint();

		case ZE_FV_RIGHT_UP_NEAR:
			return GetEdge(ZE_FE_RIGHT_UP).GetStartPoint();

		case ZE_FV_RIGHT_UP_FAR:
			return GetEdge(ZE_FE_RIGHT_DOWN).GetEndPoint();

		default:
			zeDebugCheck(true, "Wrong frustum vertex");
			return ZEVector3::Zero;
	}
}

ZEFrustum::ZEFrustum()
{

}

ZEFrustum::ZEFrustum(const ZEVector3& Position, const ZEQuaternion& Rotation, float FOV, float AspectRatio, float NearZ, float FarZ)
{
	Create(*this, Position, Rotation, FOV, AspectRatio, NearZ, FarZ);
}

ZEFrustum::ZEFrustum(const ZEPlane& LeftPlane, const ZEPlane& RightPlane, const ZEPlane& DownPlane, const ZEPlane& UpPlane, const ZEPlane& NearPlane, const ZEPlane& FarPlane)
{
	Create(*this, LeftPlane, RightPlane, DownPlane, UpPlane, NearPlane, FarPlane);
}

void ZEFrustum::Create(ZEFrustum& Frustum, const ZEVector3& Position, const ZEQuaternion& Rotation, float FOV, float AspectRatio, float NearZ, float FarZ)
{
	float SinFov = ZEAngle::Sin(FOV/2);
	float SinFovAT = SinFov * AspectRatio;
	float CosFov = ZEAngle::Cos(FOV/2);

	ZEQuaternion::VectorProduct(Frustum.UpPlane.n, Rotation, ZEVector3(0.0f, -CosFov, SinFov));
	ZEQuaternion::VectorProduct(Frustum.DownPlane.n, Rotation, ZEVector3(0.0f, CosFov, SinFov));

	ZEQuaternion::VectorProduct(Frustum.RightPlane.n, Rotation, ZEVector3(-CosFov, 0.0f, SinFovAT));
	Frustum.RightPlane.n.NormalizeSelf();
	ZEQuaternion::VectorProduct(Frustum.LeftPlane.n, Rotation, ZEVector3(CosFov, 0.0f, SinFovAT));
	Frustum.LeftPlane.n.NormalizeSelf();

	ZEQuaternion::VectorProduct(Frustum.NearPlane.n, Rotation, ZEVector3(0.0f, 0.0f, 1.0f));
	ZEQuaternion::VectorProduct(Frustum.FarPlane.n, Rotation, ZEVector3(0.0f, 0.0f, -1.0f));

	Frustum.LeftPlane.p = Position;
	Frustum.RightPlane.p = Position;
	Frustum.UpPlane.p = Position;
	Frustum.DownPlane.p = Position;

	ZEVector3 Temp;
	ZEVector3::Scale(Temp, Frustum.NearPlane.n, NearZ);
	ZEVector3::Add(Frustum.NearPlane.p, Position, Temp);

	ZEVector3::Scale(Temp, Frustum.FarPlane.n, -FarZ);
	ZEVector3::Add(Frustum.FarPlane.p, Position, Temp);
}

void ZEFrustum::Create(ZEFrustum& Frustum, const ZEPlane& LeftPlane, const ZEPlane& RightPlane, const ZEPlane& DownPlane, const ZEPlane& UpPlane, const ZEPlane& NearPlane, const ZEPlane& FarPlane)
{
	Frustum.LeftPlane = LeftPlane;
	Frustum.RightPlane = RightPlane;
	Frustum.DownPlane = DownPlane;
	Frustum.UpPlane = UpPlane;
	Frustum.NearPlane = NearPlane;
	Frustum.FarPlane = FarPlane;
}

bool ZEFrustum::IntersectionTest(const ZEFrustum& Frustum, const ZEBSphere& BoundingSphere)
{
	return 
		((ZEBSphere::IntersectionTest(BoundingSphere, Frustum.NearPlane)	!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEBSphere::IntersectionTest(BoundingSphere, Frustum.LeftPlane)		!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEBSphere::IntersectionTest(BoundingSphere, Frustum.RightPlane)	!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEBSphere::IntersectionTest(BoundingSphere, Frustum.UpPlane)		!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEBSphere::IntersectionTest(BoundingSphere, Frustum.FarPlane)		!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEBSphere::IntersectionTest(BoundingSphere, Frustum.DownPlane)		!= ZE_HS_NEGATIVE_SIDE));
}

bool ZEFrustum::IntersectionTest(const ZEFrustum& Frustum, const ZEAABBox& BoundingBox)
{
	return 
		((ZEAABBox::IntersectionTest(BoundingBox, Frustum.NearPlane)	!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEAABBox::IntersectionTest(BoundingBox, Frustum.LeftPlane)		!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEAABBox::IntersectionTest(BoundingBox, Frustum.RightPlane)	!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEAABBox::IntersectionTest(BoundingBox, Frustum.UpPlane)		!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEAABBox::IntersectionTest(BoundingBox, Frustum.FarPlane)		!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEAABBox::IntersectionTest(BoundingBox, Frustum.DownPlane)		!= ZE_HS_NEGATIVE_SIDE));
}

bool ZEFrustum::IntersectionTest(const ZEFrustum& Frustum, const ZEOBBox& BoundingBox)
{
	return 
		((ZEOBBox::IntersectionTest(BoundingBox, Frustum.NearPlane)		!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEOBBox::IntersectionTest(BoundingBox, Frustum.LeftPlane)		!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEOBBox::IntersectionTest(BoundingBox, Frustum.RightPlane)		!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEOBBox::IntersectionTest(BoundingBox, Frustum.UpPlane)		!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEOBBox::IntersectionTest(BoundingBox, Frustum.FarPlane)		!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEOBBox::IntersectionTest(BoundingBox, Frustum.DownPlane)		!= ZE_HS_NEGATIVE_SIDE));
}

bool ZEFrustum::IntersectionTest(const ZEFrustum& Frustum, const ZERectangle3D& Rectangle)
{
	return 
		((ZERectangle3D::IntersectionTest(Rectangle, Frustum.NearPlane)	!= ZE_HS_NEGATIVE_SIDE) &&
		(ZERectangle3D::IntersectionTest(Rectangle, Frustum.LeftPlane)	!= ZE_HS_NEGATIVE_SIDE) &&
		(ZERectangle3D::IntersectionTest(Rectangle, Frustum.RightPlane)	!= ZE_HS_NEGATIVE_SIDE) &&
		(ZERectangle3D::IntersectionTest(Rectangle, Frustum.UpPlane)	!= ZE_HS_NEGATIVE_SIDE) &&
		(ZERectangle3D::IntersectionTest(Rectangle, Frustum.FarPlane)	!= ZE_HS_NEGATIVE_SIDE) &&
		(ZERectangle3D::IntersectionTest(Rectangle, Frustum.DownPlane)	!= ZE_HS_NEGATIVE_SIDE));
}


// ZEViewFrustum
//////////////////////////////////////////////////////////////////////////////////////

ZEViewVolumeType ZEViewFrustum::GetViewVolumeType() const
{
	return ZE_VVT_FRUSTUM;
}

bool ZEViewFrustum::IntersectionTest(const ZEBSphere& BoundingSphere) const
{
	return ZEFrustum::IntersectionTest(*this, BoundingSphere);
}

bool ZEViewFrustum::IntersectionTest(const ZEAABBox& BoundingBox) const
{
	return ZEFrustum::IntersectionTest(*this, BoundingBox);
}

bool ZEViewFrustum::IntersectionTest(const ZEOBBox& BoundingBox) const
{
	return ZEFrustum::IntersectionTest(*this, BoundingBox);
}

bool ZEViewFrustum::IntersectionTest(const ZERectangle3D& Rectangle) const
{
	return ZEFrustum::IntersectionTest(*this, Rectangle);
}

ZEViewFrustum::ZEViewFrustum()
{

}

ZEViewFrustum::ZEViewFrustum(const ZEVector3& Position, const ZEQuaternion& Rotation, float FOV, float AspectRatio, float NearZ, float FarZ)
{
	Create(*this, Position, Rotation, FOV, AspectRatio, NearZ, FarZ);
}

ZEViewFrustum::ZEViewFrustum(const ZEPlane& LeftPlane, const ZEPlane& RightPlane, const ZEPlane& DownPlane, const ZEPlane& UpPlane, const ZEPlane& NearPlane, const ZEPlane& FarPlane)
{
	Create(*this, LeftPlane, RightPlane, DownPlane, UpPlane, NearPlane, FarPlane);
}
