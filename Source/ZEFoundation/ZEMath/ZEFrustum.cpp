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

const ZEPlane& ZEFrustum::GetClippingPlane(ZEFrustumPlane Plane) const
{
	switch(Plane)
	{
		case ZE_FP_RIGHT:
			return RightClippingPlane;

		case ZE_FP_LEFT:
			return LeftClippingPlane;

		case ZE_FP_UP:
			return UpClippingPlane;

		case ZE_FP_DOWN:
			return DownClippingPlane;

		case ZE_FP_FAR:
			return FarClippingPlane;

		case ZE_FP_NEAR:
			return NearClippingPlane;

		default:
			return LeftClippingPlane;
	}
}

ZELineSegment ZEFrustum::GetEdge(ZEFrustumEdge Edge) const
{
	ZEArray<ZELineSegment> LineSegments;
	float Fov_2 = FOV / 2;

	ZEVector3 NearPlaneCenter = Position + (Look * NearZ);
	ZEVector3 FarPlaneCenter = Position + (Look * FarZ);

	float NearPlaneHeight = NearZ * ZEAngle::Tan(Fov_2);
	float FarPlaneHeight = FarZ * ZEAngle::Tan(Fov_2);

	float NearPlaneWidth = NearPlaneHeight * AspectRatio;
	float FarPlaneWidth = FarPlaneHeight * AspectRatio;

	ZEVector3 P1, P2;
	switch(Edge)
	{
		case ZE_FE_RIGHT_FAR:
			P1 = FarPlaneCenter + (Up * FarPlaneHeight) + (Right * FarPlaneWidth);
			P2 = FarPlaneCenter - (Up * FarPlaneHeight) + (Right * FarPlaneWidth);
			return ZELineSegment(P1, P2);
		
		case ZE_FE_BOTTOM_FAR:
			P1 = FarPlaneCenter - (Up * FarPlaneHeight) + (Right * FarPlaneWidth);
			P2 = FarPlaneCenter - (Up * FarPlaneHeight) - (Right * FarPlaneWidth);
			return ZELineSegment(P1, P2);
		
		case ZE_FE_LEFT_FAR:
			P1 = FarPlaneCenter - (Up * FarPlaneHeight) - (Right * FarPlaneWidth);
			P2 = FarPlaneCenter + (Up * FarPlaneHeight) - (Right * FarPlaneWidth);
			return ZELineSegment(P1, P2);
		
		case ZE_FE_TOP_FAR:
			P1 = FarPlaneCenter + (Up * FarPlaneHeight) - (Right * FarPlaneWidth);
			P2 = FarPlaneCenter + (Up * FarPlaneHeight) + (Right * FarPlaneWidth);
			return ZELineSegment(P1, P2);
		
		case ZE_FE_RIGHT_UP:
			P1 = NearPlaneCenter + (Up * NearPlaneHeight) + (Right * NearPlaneWidth);
			P2 = FarPlaneCenter + (Up * FarPlaneHeight) + (Right * FarPlaneWidth);
			return ZELineSegment(P1, P2);
		
		case ZE_FE_RIGHT_DOWN:
			P1 = NearPlaneCenter - (Up * NearPlaneHeight) + (Right * NearPlaneWidth);
			P2 = FarPlaneCenter - (Up * FarPlaneHeight) + (Right * FarPlaneWidth);
			return ZELineSegment(P1, P2);
		
		case ZE_FE_LEFT_DOWN:
			P1 = NearPlaneCenter - (Up * NearPlaneHeight) - (Right * NearPlaneWidth);
			P2 = FarPlaneCenter - (Up * FarPlaneHeight) - (Right * FarPlaneWidth);
			return ZELineSegment(P1, P2);
		
		case ZE_FE_LEFT_UP:
			P1 = NearPlaneCenter + (Up * NearPlaneHeight) - (Right * NearPlaneWidth);
			P2 = FarPlaneCenter + (Up * FarPlaneHeight) - (Right * FarPlaneWidth);
			return ZELineSegment(P1, P2);
		
		case ZE_FE_RIGHT_NEAR:
			P1 = NearPlaneCenter + (Up * NearPlaneHeight) + (Right * NearPlaneWidth);
			P2 = NearPlaneCenter - (Up * NearPlaneHeight) + (Right * NearPlaneWidth);
			return ZELineSegment(P1, P2);
		
		case ZE_FE_BOTTOM_NEAR:
			P1 = NearPlaneCenter - (Up * NearPlaneHeight) + (Right * NearPlaneWidth);
			P2 = NearPlaneCenter - (Up * NearPlaneHeight) - (Right * NearPlaneWidth);
			return ZELineSegment(P1, P2);
		
		case ZE_FE_LEFT_NEAR:
			P1 = NearPlaneCenter - (Up * NearPlaneHeight) - (Right * NearPlaneWidth);
			P2 = NearPlaneCenter + (Up * NearPlaneHeight) - (Right * NearPlaneWidth);
			return ZELineSegment(P1, P2);
		
		case ZE_FE_TOP_NEAR:
			P1 = NearPlaneCenter + (Up * NearPlaneHeight) - (Right * NearPlaneWidth);
			P2 = NearPlaneCenter + (Up * NearPlaneHeight) + (Right * NearPlaneWidth);
			return ZELineSegment(P1, P2);
		
		default:
			P1 = FarPlaneCenter + (Up * FarPlaneHeight) + (Right * FarPlaneWidth);
			P2 = FarPlaneCenter - (Up * FarPlaneHeight) + (Right * FarPlaneWidth);
			return ZELineSegment(P1, P2);
	}
}

ZEVector3 ZEFrustum::GetVertex(ZEFrustumVertex Vertex) const
{
	float Fov_2 = FOV / 2;

	ZEVector3 NearPlaneCenter = Position + (Look * NearZ);
	ZEVector3 FarPlaneCenter = Position + (Look * FarZ);

	float NearPlaneHeight = NearZ * ZEAngle::Tan(Fov_2);
	float FarPlaneHeight = FarZ * ZEAngle::Tan(Fov_2);

	float NearPlaneWidth = NearPlaneHeight * AspectRatio;
	float FarPlaneWidth = FarPlaneHeight * AspectRatio;

	switch(Vertex)
	{
		case ZE_FV_LEFT_DOWN_NEAR:
			return (NearPlaneCenter - (Up * NearPlaneHeight) - (Right * NearPlaneWidth));

		case ZE_FV_LEFT_DOWN_FAR:
			return (FarPlaneCenter - (Up * FarPlaneHeight) - (Right * FarPlaneWidth));

		case ZE_FV_LEFT_UP_NEAR:
			return (NearPlaneCenter + (Up * NearPlaneHeight) + (Right * NearPlaneWidth));

		case ZE_FV_LEFT_UP_FAR:
			return (FarPlaneCenter + (Up * FarPlaneHeight) - (Right * FarPlaneWidth));

		case ZE_FV_RIGHT_DOWN_NEAR:
			return (NearPlaneCenter - (Up * NearPlaneHeight) + (Right * NearPlaneWidth));

		case ZE_FV_RIGHT_DOWN_FAR:
			return (FarPlaneCenter - (Up * FarPlaneHeight) + (Right * FarPlaneWidth));

		case ZE_FV_RIGHT_UP_NEAR:
			return (NearPlaneCenter + (Up * NearPlaneHeight) + (Right * NearPlaneWidth));

		case ZE_FV_RIGHT_UP_FAR:
			return (FarPlaneCenter + (Up * FarPlaneHeight) + (Right * FarPlaneWidth));

		default:
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

ZEFrustum::ZEFrustum(const ZEPlane& LeftClippingPlane, const ZEPlane& RightClippingPlane, const ZEPlane& DownClippingPlane, const ZEPlane& UpClippingPlane, const ZEPlane& NearClippingPlane, const ZEPlane& FarClippingPlane)
{
	Create(*this, LeftClippingPlane, RightClippingPlane, DownClippingPlane, UpClippingPlane, NearClippingPlane, FarClippingPlane);
}

void ZEFrustum::Create(ZEFrustum& Frustum, const ZEVector3& Position, const ZEQuaternion& Rotation, float FOV, float AspectRatio, float NearZ, float FarZ)
{
	float SinFov = ZEAngle::Sin(FOV/2);
	float SinFovAT = SinFov * AspectRatio;
	float CosFov = ZEAngle::Cos(FOV/2);

	Frustum.Look = Rotation * ZEVector3::UnitZ;
	Frustum.Up = Rotation * ZEVector3::UnitY;
	Frustum.Right = Rotation * ZEVector3::UnitX;

	Frustum.NearZ = NearZ;
	Frustum.FarZ = FarZ;
	Frustum.FOV = FOV;
	Frustum.AspectRatio = AspectRatio;

	ZEQuaternion::VectorProduct(Frustum.UpClippingPlane.n, Rotation, ZEVector3(0.0f, -CosFov, SinFov));
	ZEQuaternion::VectorProduct(Frustum.DownClippingPlane.n, Rotation, ZEVector3(0.0f, CosFov, SinFov));

	ZEQuaternion::VectorProduct(Frustum.RightClippingPlane.n, Rotation, ZEVector3(-CosFov, 0.0f, SinFovAT));
	Frustum.RightClippingPlane.n.NormalizeSelf();
	ZEQuaternion::VectorProduct(Frustum.LeftClippingPlane.n, Rotation, ZEVector3(CosFov, 0.0f, SinFovAT));
	Frustum.LeftClippingPlane.n.NormalizeSelf();

	ZEQuaternion::VectorProduct(Frustum.NearClippingPlane.n, Rotation, ZEVector3(0.0f, 0.0f, 1.0f));
	ZEQuaternion::VectorProduct(Frustum.FarClippingPlane.n, Rotation, ZEVector3(0.0f, 0.0f, -1.0f));

	Frustum.Position = Position;
	Frustum.LeftClippingPlane.p = Frustum.Position;
	Frustum.RightClippingPlane.p = Frustum.Position;
	Frustum.UpClippingPlane.p = Frustum.Position;
	Frustum.DownClippingPlane.p = Frustum.Position;

	ZEVector3 Temp;
	ZEVector3::Scale(Temp, Frustum.NearClippingPlane.n, NearZ);
	ZEVector3::Add(Frustum.NearClippingPlane.p, Position, Temp);

	ZEVector3::Scale(Temp, Frustum.FarClippingPlane.n, -FarZ);
	ZEVector3::Add(Frustum.FarClippingPlane.p, Position, Temp);

	// View cone
	Frustum.Position = Position;
	ZEQuaternion::VectorProduct(Frustum.CenterVector, Rotation, ZEVector3(0.0f, 0.0f, 1.0f));

	float tanfov = ZEAngle::Tan(FOV / 2.0f);
	float tHeight = tanfov * NearZ * 2.0f;
	float tWidth = AspectRatio * tHeight;

	float radius = ZEMath::Sqrt(tWidth * tWidth + tHeight * tHeight) / 2.0f;
	Frustum.TanRatio = radius / NearZ;
	Frustum.SinRatio = radius / ZEMath::Sqrt(radius * radius + NearZ * NearZ);
}

void ZEFrustum::Create(ZEFrustum& Frustum, const ZEPlane& LeftClippingPlane, const ZEPlane& RightClippingPlane, const ZEPlane& DownClippingPlane, const ZEPlane& UpClippingPlane, const ZEPlane& NearClippingPlane, const ZEPlane& FarClippingPlane)
{
	Frustum.LeftClippingPlane = LeftClippingPlane;
	Frustum.RightClippingPlane = RightClippingPlane;
	Frustum.DownClippingPlane = DownClippingPlane;
	Frustum.UpClippingPlane = UpClippingPlane;
	Frustum.NearClippingPlane = NearClippingPlane;
	Frustum.FarClippingPlane = FarClippingPlane;
}

bool ZEFrustum::IntersectionTest(const ZEFrustum& Frustum, const ZEBSphere& BoundingSphere)
{
	return 
		!((ZEBSphere::IntersectionTest(BoundingSphere, Frustum.NearClippingPlane)	== ZE_HS_NEGATIVE_SIDE) ||
		(ZEBSphere::IntersectionTest(BoundingSphere, Frustum.LeftClippingPlane)		== ZE_HS_NEGATIVE_SIDE) ||
		(ZEBSphere::IntersectionTest(BoundingSphere, Frustum.RightClippingPlane)	== ZE_HS_NEGATIVE_SIDE) ||
		(ZEBSphere::IntersectionTest(BoundingSphere, Frustum.UpClippingPlane)		== ZE_HS_NEGATIVE_SIDE) ||
		(ZEBSphere::IntersectionTest(BoundingSphere, Frustum.FarClippingPlane)		== ZE_HS_NEGATIVE_SIDE) ||
		(ZEBSphere::IntersectionTest(BoundingSphere, Frustum.DownClippingPlane)		== ZE_HS_NEGATIVE_SIDE));
}

bool ZEFrustum::IntersectionTest(const ZEFrustum& Frustum, const ZEAABBox& BoundingBox)
{
	return 
		!((ZEAABBox::IntersectionTest(BoundingBox, Frustum.NearClippingPlane)	== ZE_HS_NEGATIVE_SIDE) ||
		(ZEAABBox::IntersectionTest(BoundingBox, Frustum.LeftClippingPlane)		== ZE_HS_NEGATIVE_SIDE) ||
		(ZEAABBox::IntersectionTest(BoundingBox, Frustum.RightClippingPlane)	== ZE_HS_NEGATIVE_SIDE) ||
		(ZEAABBox::IntersectionTest(BoundingBox, Frustum.UpClippingPlane)		== ZE_HS_NEGATIVE_SIDE) ||
		(ZEAABBox::IntersectionTest(BoundingBox, Frustum.FarClippingPlane)		== ZE_HS_NEGATIVE_SIDE) ||
		(ZEAABBox::IntersectionTest(BoundingBox, Frustum.DownClippingPlane)		== ZE_HS_NEGATIVE_SIDE));

	/*return 
		((ZEAABBox::IntersectionTest(BoundingBox, Frustum.NearClippingPlane)	!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEAABBox::IntersectionTest(BoundingBox, Frustum.LeftClippingPlane)		!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEAABBox::IntersectionTest(BoundingBox, Frustum.RightClippingPlane)	!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEAABBox::IntersectionTest(BoundingBox, Frustum.UpClippingPlane)		!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEAABBox::IntersectionTest(BoundingBox, Frustum.FarClippingPlane)		!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEAABBox::IntersectionTest(BoundingBox, Frustum.DownClippingPlane)		!= ZE_HS_NEGATIVE_SIDE));*/
}

bool ZEFrustum::IntersectionTest(const ZEFrustum& Frustum, const ZEOBBox& BoundingBox)
{
	return 
		!((ZEOBBox::IntersectionTest(BoundingBox, Frustum.NearClippingPlane)		== ZE_HS_NEGATIVE_SIDE) ||
		(ZEOBBox::IntersectionTest(BoundingBox, Frustum.LeftClippingPlane)		== ZE_HS_NEGATIVE_SIDE) ||
		(ZEOBBox::IntersectionTest(BoundingBox, Frustum.RightClippingPlane)		== ZE_HS_NEGATIVE_SIDE) ||
		(ZEOBBox::IntersectionTest(BoundingBox, Frustum.UpClippingPlane)		== ZE_HS_NEGATIVE_SIDE) ||
		(ZEOBBox::IntersectionTest(BoundingBox, Frustum.FarClippingPlane)		== ZE_HS_NEGATIVE_SIDE) ||
		(ZEOBBox::IntersectionTest(BoundingBox, Frustum.DownClippingPlane)		== ZE_HS_NEGATIVE_SIDE));

	/*return 
		((ZEOBBox::IntersectionTest(BoundingBox, Frustum.NearClippingPlane)		!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEOBBox::IntersectionTest(BoundingBox, Frustum.LeftClippingPlane)		!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEOBBox::IntersectionTest(BoundingBox, Frustum.RightClippingPlane)		!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEOBBox::IntersectionTest(BoundingBox, Frustum.UpClippingPlane)		!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEOBBox::IntersectionTest(BoundingBox, Frustum.FarClippingPlane)		!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEOBBox::IntersectionTest(BoundingBox, Frustum.DownClippingPlane)		!= ZE_HS_NEGATIVE_SIDE));*/
}

bool ZEFrustum::IntersectionTest(const ZEFrustum& Frustum, const ZERectangle3D& Rectangle)
{
	return 
		!((ZERectangle3D::IntersectionTest(Rectangle, Frustum.NearClippingPlane)	== ZE_HS_NEGATIVE_SIDE) ||
		(ZERectangle3D::IntersectionTest(Rectangle, Frustum.LeftClippingPlane)	== ZE_HS_NEGATIVE_SIDE) ||
		(ZERectangle3D::IntersectionTest(Rectangle, Frustum.RightClippingPlane)	== ZE_HS_NEGATIVE_SIDE) ||
		(ZERectangle3D::IntersectionTest(Rectangle, Frustum.UpClippingPlane)	== ZE_HS_NEGATIVE_SIDE) ||
		(ZERectangle3D::IntersectionTest(Rectangle, Frustum.FarClippingPlane)	== ZE_HS_NEGATIVE_SIDE) ||
		(ZERectangle3D::IntersectionTest(Rectangle, Frustum.DownClippingPlane)	== ZE_HS_NEGATIVE_SIDE));

	/*return 
		((ZERectangle3D::IntersectionTest(Rectangle, Frustum.NearClippingPlane)	!= ZE_HS_NEGATIVE_SIDE) &&
		(ZERectangle3D::IntersectionTest(Rectangle, Frustum.LeftClippingPlane)	!= ZE_HS_NEGATIVE_SIDE) &&
		(ZERectangle3D::IntersectionTest(Rectangle, Frustum.RightClippingPlane)	!= ZE_HS_NEGATIVE_SIDE) &&
		(ZERectangle3D::IntersectionTest(Rectangle, Frustum.UpClippingPlane)	!= ZE_HS_NEGATIVE_SIDE) &&
		(ZERectangle3D::IntersectionTest(Rectangle, Frustum.FarClippingPlane)	!= ZE_HS_NEGATIVE_SIDE) &&
		(ZERectangle3D::IntersectionTest(Rectangle, Frustum.DownClippingPlane)	!= ZE_HS_NEGATIVE_SIDE));*/
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

ZEViewFrustum::ZEViewFrustum(const ZEPlane& LeftClippingPlane, const ZEPlane& RightClippingPlane, const ZEPlane& DownClippingPlane, const ZEPlane& UpClippingPlane, const ZEPlane& NearClippingPlane, const ZEPlane& FarClippingPlane)
{
	Create(*this, LeftClippingPlane, RightClippingPlane, DownClippingPlane, UpClippingPlane, NearClippingPlane, FarClippingPlane);
}
