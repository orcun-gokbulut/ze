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

static inline ZEIntersectionResult SeparatingAxisTest(const ZEVector3& Axis, const ZEFrustum& Frustum, const ZEVector3* Vertices, const ZEOBBox& BoundingBox)
{
	zeDebugCheck(!BoundingBox.Right.IsNormalized(), "BoundingBox Right vector is not normalized.");
	zeDebugCheck(!BoundingBox.Up.IsNormalized(), "BoundingBox Up vector is not normalized.");
	zeDebugCheck(!BoundingBox.Front.IsNormalized(), "BoundingBox Front vector is not normalized.");

	float FrustumMin = ZE_FLOAT_MAX;
	float FrustumMax = ZE_FLOAT_MIN;
	for (ZESize I = 0; I < 8; I++)
	{
		float Projection = ZEVector3::DotProduct(Axis, Vertices[I]);

		if (Projection > FrustumMax)
			FrustumMax = Projection;

		if (Projection < FrustumMin)
			FrustumMin = Projection;
	}

	float BoundingBoxCenter = ZEVector3::DotProduct(BoundingBox.Center, Axis);
	float BoundingBoxExtent = 
		BoundingBox.HalfSize.x * ZEMath::Abs(ZEVector3::DotProduct(BoundingBox.Right, Axis)) +
		BoundingBox.HalfSize.y * ZEMath::Abs(ZEVector3::DotProduct(BoundingBox.Up, Axis)) +
		BoundingBox.HalfSize.z * ZEMath::Abs(ZEVector3::DotProduct(BoundingBox.Front, Axis));
	float BoundingBoxExtentMin = BoundingBoxCenter - BoundingBoxExtent;
	float BoundingBoxExtentMax = BoundingBoxCenter + BoundingBoxExtent;

	// Covers
	if (FrustumMin < BoundingBoxExtentMin && FrustumMax > BoundingBoxExtentMax)
		return ZE_IR_COVERS;
	else if (FrustumMin < BoundingBoxExtentMin && BoundingBoxExtentMax < FrustumMax)
		return ZE_IR_INSIDE;
	else if ((FrustumMin > BoundingBoxExtentMin) && (FrustumMin > BoundingBoxExtentMax) || ((FrustumMax < BoundingBoxExtentMin) && (FrustumMax < BoundingBoxExtentMax))) 
		return ZE_IR_NONE;
	else
		return ZE_IR_INTERSECT;
}


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
	ZEVector3::CrossProduct(Temp, NearPlane.n, BottomPlane.n);
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

ZEPlane& ZEFrustum::GetPlane(ZEFrustumPlane Plane)
{
	static ZEPlane None(ZEVector3::Zero, ZEVector3::Zero);
	switch(Plane)
	{
		default:
			zeDebugCheck(true, "Wrong frustum plane");
			return None;

		case ZE_FP_LEFT:
			return LeftPlane;
		
		case ZE_FP_RIGHT:
			return RightPlane;

		case ZE_FP_BOTTOM:
			return BottomPlane;

		case ZE_FP_TOP:
			return TopPlane;

		case ZE_FP_NEAR:
			return NearPlane;

		case ZE_FP_FAR:
			return FarPlane;
	}
}

const ZEPlane& ZEFrustum::GetPlane(ZEFrustumPlane Plane) const
{
	static ZEPlane None(ZEVector3::Zero, ZEVector3::Zero);
	switch(Plane)
	{
		default:
			zeDebugCheck(true, "Wrong frustum plane");
			return None;

		case ZE_FP_LEFT:
			return LeftPlane;
		
		case ZE_FP_RIGHT:
			return RightPlane;

		case ZE_FP_BOTTOM:
			return BottomPlane;

		case ZE_FP_TOP:
			return TopPlane;

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
		case ZE_FE_LEFT_BOTTOM:
			ZEPlane::IntersectionTest(LeftPlane, BottomPlane, Line);
			ZEPlane::IntersectionTest(NearPlane, Line, t0);
			ZEPlane::IntersectionTest(FarPlane, Line, t1);
			break;
		
		case ZE_FE_LEFT_TOP:
			ZEPlane::IntersectionTest(LeftPlane, TopPlane, Line);
			ZEPlane::IntersectionTest(NearPlane, Line, t0);
			ZEPlane::IntersectionTest(FarPlane, Line, t1);
			break;

		case ZE_FE_LEFT_NEAR:
			ZEPlane::IntersectionTest(LeftPlane, NearPlane, Line);
			ZEPlane::IntersectionTest(BottomPlane, Line, t0);
			ZEPlane::IntersectionTest(TopPlane, Line, t1);
			break;

		case ZE_FE_LEFT_FAR:
			ZEPlane::IntersectionTest(LeftPlane, FarPlane, Line);
			ZEPlane::IntersectionTest(BottomPlane, Line, t0);
			ZEPlane::IntersectionTest(TopPlane, Line, t1);
			break;

		case ZE_FE_RIGHT_BOTTOM:
			ZEPlane::IntersectionTest(RightPlane, BottomPlane, Line);
			ZEPlane::IntersectionTest(NearPlane, Line, t0);
			ZEPlane::IntersectionTest(FarPlane, Line, t1);
			break;

		case ZE_FE_RIGHT_TOP:
			ZEPlane::IntersectionTest(RightPlane, TopPlane, Line);
			ZEPlane::IntersectionTest(NearPlane, Line, t0);
			ZEPlane::IntersectionTest(FarPlane, Line, t1);
			break;


		case ZE_FE_RIGHT_NEAR:
			ZEPlane::IntersectionTest(RightPlane, NearPlane, Line);
			ZEPlane::IntersectionTest(BottomPlane, Line, t0);
			ZEPlane::IntersectionTest(TopPlane, Line, t1);
			break;

		case ZE_FE_RIGHT_FAR:
			ZEPlane::IntersectionTest(RightPlane, FarPlane, Line);
			ZEPlane::IntersectionTest(BottomPlane, Line, t0);
			ZEPlane::IntersectionTest(TopPlane, Line, t1);
			break;

		case ZE_FE_BOTTOM_NEAR:
			ZEPlane::IntersectionTest(BottomPlane, NearPlane, Line);
			ZEPlane::IntersectionTest(LeftPlane, Line, t0);
			ZEPlane::IntersectionTest(RightPlane, Line, t1);
			break;

		case ZE_FE_BOTTOM_FAR:
			ZEPlane::IntersectionTest(BottomPlane, FarPlane, Line);
			ZEPlane::IntersectionTest(LeftPlane, Line, t0);
			ZEPlane::IntersectionTest(RightPlane, Line, t1);
			break;
			
		case ZE_FE_TOP_NEAR:
			ZEPlane::IntersectionTest(TopPlane, NearPlane, Line);
			ZEPlane::IntersectionTest(LeftPlane, Line, t0);
			ZEPlane::IntersectionTest(RightPlane, Line, t1);
			break;

		case ZE_FE_TOP_FAR:
			ZEPlane::IntersectionTest(TopPlane, FarPlane, Line);
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
		case ZE_FV_LEFT_BOTTOM_NEAR:
			return GetEdge(ZE_FE_LEFT_BOTTOM).GetStartPoint();

		case ZE_FV_LEFT_BOTTOM_FAR:
			return GetEdge(ZE_FE_LEFT_BOTTOM).GetEndPoint();

		case ZE_FV_LEFT_TOP_NEAR:
			return GetEdge(ZE_FE_LEFT_TOP).GetStartPoint();

		case ZE_FV_LEFT_TOP_FAR:
			return GetEdge(ZE_FE_LEFT_TOP).GetEndPoint();

		case ZE_FV_RIGHT_BOTTOM_NEAR:
			return GetEdge(ZE_FE_RIGHT_BOTTOM).GetStartPoint();

		case ZE_FV_RIGHT_BOTTOM_FAR:
			return GetEdge(ZE_FE_RIGHT_BOTTOM).GetEndPoint();

		case ZE_FV_RIGHT_TOP_NEAR:
			return GetEdge(ZE_FE_RIGHT_TOP).GetStartPoint();

		case ZE_FV_RIGHT_TOP_FAR:
			return GetEdge(ZE_FE_RIGHT_TOP).GetEndPoint();

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

bool ZEFrustum::IntersectionTest(const ZEFrustum& Frustum, const ZEVector3& Point)
{
	return (
		(ZEPlane::TestHalfSpace(Frustum.NearPlane, Point) 	!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEPlane::TestHalfSpace(Frustum.LeftPlane, Point) 	!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEPlane::TestHalfSpace(Frustum.RightPlane, Point)	!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEPlane::TestHalfSpace(Frustum.TopPlane, Point) 	!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEPlane::TestHalfSpace(Frustum.FarPlane, Point) 	!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEPlane::TestHalfSpace(Frustum.BottomPlane, Point) != ZE_HS_NEGATIVE_SIDE));
}

bool ZEFrustum::IntersectionTest(const ZEFrustum& Frustum, const ZEBSphere& BoundingSphere)
{
	return (
		(ZEBSphere::IntersectionTest(BoundingSphere, Frustum.NearPlane)		!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEBSphere::IntersectionTest(BoundingSphere, Frustum.LeftPlane)		!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEBSphere::IntersectionTest(BoundingSphere, Frustum.RightPlane)	!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEBSphere::IntersectionTest(BoundingSphere, Frustum.TopPlane)		!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEBSphere::IntersectionTest(BoundingSphere, Frustum.FarPlane)		!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEBSphere::IntersectionTest(BoundingSphere, Frustum.BottomPlane)	!= ZE_HS_NEGATIVE_SIDE));
}

bool ZEFrustum::IntersectionTest(const ZEFrustum& Frustum, const ZEAABBox& BoundingBox)
{
	return (
		(ZEAABBox::IntersectionTest(BoundingBox, Frustum.NearPlane)		!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEAABBox::IntersectionTest(BoundingBox, Frustum.LeftPlane)		!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEAABBox::IntersectionTest(BoundingBox, Frustum.RightPlane)	!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEAABBox::IntersectionTest(BoundingBox, Frustum.TopPlane)		!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEAABBox::IntersectionTest(BoundingBox, Frustum.FarPlane)		!= ZE_HS_NEGATIVE_SIDE) &&
		(ZEAABBox::IntersectionTest(BoundingBox, Frustum.BottomPlane)	!= ZE_HS_NEGATIVE_SIDE));
}

bool ZEFrustum::IntersectionTest(const ZEFrustum& Frustum, const ZEOBBox& BoundingBox)
{
	bool Result1 = (ZEOBBox::IntersectionTest(BoundingBox, Frustum.NearPlane)		!= ZE_HS_NEGATIVE_SIDE);
	bool Result2 = 	(ZEOBBox::IntersectionTest(BoundingBox, Frustum.LeftPlane)		!= ZE_HS_NEGATIVE_SIDE);
	bool Result3 = 	(ZEOBBox::IntersectionTest(BoundingBox, Frustum.RightPlane)		!= ZE_HS_NEGATIVE_SIDE);
	bool Result4 = 	(ZEOBBox::IntersectionTest(BoundingBox, Frustum.TopPlane)		!= ZE_HS_NEGATIVE_SIDE);
	bool Result5 = (ZEOBBox::IntersectionTest(BoundingBox, Frustum.FarPlane)		!= ZE_HS_NEGATIVE_SIDE);
	bool Result6 = (ZEOBBox::IntersectionTest(BoundingBox, Frustum.BottomPlane)		!= ZE_HS_NEGATIVE_SIDE);
	return Result1 && Result2 && Result3 && Result4 && Result5 && Result6; 
}

bool ZEFrustum::IntersectionTest(const ZEFrustum& Frustum, const ZERectangle3D& Rectangle)
{
	return (
		(ZERectangle3D::IntersectionTest(Rectangle, Frustum.NearPlane)		!= ZE_HS_NEGATIVE_SIDE) &&
		(ZERectangle3D::IntersectionTest(Rectangle, Frustum.LeftPlane)		!= ZE_HS_NEGATIVE_SIDE) &&
		(ZERectangle3D::IntersectionTest(Rectangle, Frustum.RightPlane)		!= ZE_HS_NEGATIVE_SIDE) &&
		(ZERectangle3D::IntersectionTest(Rectangle, Frustum.TopPlane)		!= ZE_HS_NEGATIVE_SIDE) &&
		(ZERectangle3D::IntersectionTest(Rectangle, Frustum.FarPlane)		!= ZE_HS_NEGATIVE_SIDE) &&
		(ZERectangle3D::IntersectionTest(Rectangle, Frustum.BottomPlane)	!= ZE_HS_NEGATIVE_SIDE));
}

ZEIntersectionResult ZEFrustum::IntersectionTestExact(const ZEFrustum& Frustum, const ZEAABBox& BoundingBox)
{
	ZEOBBox TempBoundingBox;
	ZEAABBox::GenerateOBoundingBox(TempBoundingBox, BoundingBox);
	return IntersectionTestExact(Frustum, TempBoundingBox);
}

#define CHECK_RESULT() do { if (Result == ZE_IR_NONE) return ZE_IR_NONE; else if (TotalResult != Result) TotalResult = ZE_IR_INTERSECT; } while(false)

ZEIntersectionResult ZEFrustum::IntersectionTestExact(const ZEFrustum& Frustum, const ZEOBBox& BoundingBox)
{
	if (!IntersectionTest(Frustum, BoundingBox))
		return ZE_IR_NONE;

	ZEIntersectionResult TotalResult = ZE_IR_NONE;
	ZEIntersectionResult Result;

	ZEVector3 FrustumVertices[8];
	FrustumVertices[0] = Frustum.GetVertex(ZE_FV_LEFT_BOTTOM_NEAR);
	FrustumVertices[1] = Frustum.GetVertex(ZE_FV_LEFT_BOTTOM_FAR);
	FrustumVertices[2] = Frustum.GetVertex(ZE_FV_LEFT_TOP_NEAR);
	FrustumVertices[3] = Frustum.GetVertex(ZE_FV_LEFT_TOP_FAR);
	FrustumVertices[4] = Frustum.GetVertex(ZE_FV_RIGHT_BOTTOM_NEAR);
	FrustumVertices[5] = Frustum.GetVertex(ZE_FV_RIGHT_BOTTOM_FAR);
	FrustumVertices[6] = Frustum.GetVertex(ZE_FV_RIGHT_TOP_NEAR);
	FrustumVertices[7] = Frustum.GetVertex(ZE_FV_RIGHT_TOP_FAR);

	// Frustum SAT Tests
	////////////////////////////////////////////////

	Result = SeparatingAxisTest(Frustum.NearPlane.n, Frustum, FrustumVertices, BoundingBox);
	if (Result == ZE_IR_NONE)
		return ZE_IR_NONE;
	TotalResult = Result;

	Result = SeparatingAxisTest(Frustum.RightPlane.n, Frustum, FrustumVertices, BoundingBox);
	CHECK_RESULT();
	
	Result = SeparatingAxisTest(Frustum.LeftPlane.n, Frustum, FrustumVertices, BoundingBox);
	CHECK_RESULT();

	Result = SeparatingAxisTest(Frustum.TopPlane.n, Frustum, FrustumVertices, BoundingBox);
	CHECK_RESULT();

	Result = SeparatingAxisTest(Frustum.BottomPlane.n, Frustum, FrustumVertices, BoundingBox);
	CHECK_RESULT();

	Result = SeparatingAxisTest(Frustum.FarPlane.n, Frustum, FrustumVertices, BoundingBox);
	CHECK_RESULT();


	// BoundingBox SAT Tests
	////////////////////////////////////////////////

	Result = SeparatingAxisTest(BoundingBox.Right, Frustum, FrustumVertices, BoundingBox);
	CHECK_RESULT();

	Result = SeparatingAxisTest(BoundingBox.Up, Frustum, FrustumVertices, BoundingBox);
	CHECK_RESULT();
	
	Result = SeparatingAxisTest(BoundingBox.Front, Frustum, FrustumVertices, BoundingBox);
	CHECK_RESULT();

	
	const float Epsilon = 0.001f;

	// BoundingBox.Right Cross Products
	////////////////////////////////////////////////

	ZEVector3 CrossProductOutput;

	ZEVector3 FrustumLeftNearEdge = Frustum.GetEdge(ZE_FE_LEFT_NEAR).v;
	ZEVector3::CrossProduct(CrossProductOutput, BoundingBox.Right, FrustumLeftNearEdge);
	if (ZEMath::Abs(CrossProductOutput.LengthSquare()) > Epsilon)
	{
		Result = SeparatingAxisTest(CrossProductOutput, Frustum, FrustumVertices, BoundingBox);
		CHECK_RESULT();
	}

	ZEVector3 FrustumTopNearEdge = Frustum.GetEdge(ZE_FE_TOP_NEAR).v;
	ZEVector3::CrossProduct(CrossProductOutput, BoundingBox.Right, FrustumTopNearEdge);
	if (ZEMath::Abs(CrossProductOutput.LengthSquare()) > Epsilon)
	{
		Result = SeparatingAxisTest(CrossProductOutput, Frustum, FrustumVertices, BoundingBox);
		CHECK_RESULT();
	}

	ZEVector3 FrustumLeftBottomEdge = Frustum.GetEdge(ZE_FE_LEFT_BOTTOM).v;
	ZEVector3::CrossProduct(CrossProductOutput, BoundingBox.Right, FrustumLeftBottomEdge);
	if (ZEMath::Abs(CrossProductOutput.LengthSquare()) > Epsilon)
	{
		Result = SeparatingAxisTest(CrossProductOutput, Frustum, FrustumVertices, BoundingBox);
		CHECK_RESULT();
	}

	ZEVector3 FrustumLeftTopEdge = Frustum.GetEdge(ZE_FE_LEFT_TOP).v;
	ZEVector3::CrossProduct(CrossProductOutput, BoundingBox.Right, FrustumLeftTopEdge);
	if (ZEMath::Abs(CrossProductOutput.LengthSquare()) > Epsilon)
	{
		Result = SeparatingAxisTest(CrossProductOutput, Frustum, FrustumVertices, BoundingBox);
		CHECK_RESULT();
	}

	ZEVector3 FrustumRightBottomEdge = Frustum.GetEdge(ZE_FE_RIGHT_BOTTOM).v;
	CrossProductOutput.NormalizeSelf();
	if (ZEMath::Abs(CrossProductOutput.LengthSquare()) > Epsilon)
	{
		Result = SeparatingAxisTest(CrossProductOutput, Frustum, FrustumVertices, BoundingBox);
		CHECK_RESULT();
	}

	ZEVector3 FrustumRightTopEdge = Frustum.GetEdge(ZE_FE_RIGHT_TOP).v;
	ZEVector3::CrossProduct(CrossProductOutput, BoundingBox.Right, FrustumRightTopEdge);
	if (ZEMath::Abs(CrossProductOutput.LengthSquare()) > Epsilon)
	{
		Result = SeparatingAxisTest(CrossProductOutput, Frustum, FrustumVertices, BoundingBox);
		CHECK_RESULT();
	}

	ZEVector3 FrustumLeftFarEdge = Frustum.GetEdge(ZE_FE_LEFT_FAR).v;
	ZEVector3::CrossProduct(CrossProductOutput, BoundingBox.Right, FrustumLeftFarEdge);
	if (ZEMath::Abs(CrossProductOutput.LengthSquare()) > Epsilon)
	{
		Result = SeparatingAxisTest(CrossProductOutput, Frustum, FrustumVertices, BoundingBox);
		CHECK_RESULT();
	}

	ZEVector3 FrustumTopFarEdge = Frustum.GetEdge(ZE_FE_TOP_FAR).v;
	ZEVector3::CrossProduct(CrossProductOutput, BoundingBox.Right, FrustumTopFarEdge);
	if (ZEMath::Abs(CrossProductOutput.LengthSquare()) > Epsilon)
	{
		Result = SeparatingAxisTest(CrossProductOutput, Frustum, FrustumVertices, BoundingBox);
		CHECK_RESULT();
	}

	// BoundingBox.Up Cross Products
	////////////////////////////////////////////////

	ZEVector3::CrossProduct(CrossProductOutput, BoundingBox.Up, FrustumLeftNearEdge);
	if (ZEMath::Abs(CrossProductOutput.LengthSquare()) > Epsilon)
	{
		Result = SeparatingAxisTest(CrossProductOutput, Frustum, FrustumVertices, BoundingBox);
		CHECK_RESULT();
	}

	ZEVector3::CrossProduct(CrossProductOutput, BoundingBox.Up,FrustumTopNearEdge);
	if (ZEMath::Abs(CrossProductOutput.LengthSquare()) > Epsilon)
	{
		Result = SeparatingAxisTest(CrossProductOutput, Frustum, FrustumVertices, BoundingBox);
		CHECK_RESULT();
	}

	ZEVector3::CrossProduct(CrossProductOutput, BoundingBox.Up, FrustumLeftBottomEdge);
	if (ZEMath::Abs(CrossProductOutput.LengthSquare()) > Epsilon)
	{
		Result = SeparatingAxisTest(CrossProductOutput, Frustum, FrustumVertices, BoundingBox);
		CHECK_RESULT();
	}

	ZEVector3::CrossProduct(CrossProductOutput, BoundingBox.Up, FrustumLeftTopEdge);
	if (ZEMath::Abs(CrossProductOutput.LengthSquare()) > Epsilon)
	{
		Result = SeparatingAxisTest(CrossProductOutput, Frustum, FrustumVertices, BoundingBox);
		CHECK_RESULT();
	}

	ZEVector3::CrossProduct(CrossProductOutput, BoundingBox.Up, FrustumRightBottomEdge);
	if (ZEMath::Abs(CrossProductOutput.LengthSquare()) > Epsilon)
	{
		Result = SeparatingAxisTest(CrossProductOutput, Frustum, FrustumVertices, BoundingBox);
		CHECK_RESULT();
	}

	ZEVector3::CrossProduct(CrossProductOutput, BoundingBox.Up, FrustumRightTopEdge);
	if (ZEMath::Abs(CrossProductOutput.LengthSquare()) > Epsilon)
	{
		Result = SeparatingAxisTest(CrossProductOutput, Frustum, FrustumVertices, BoundingBox);
		CHECK_RESULT();
	}
	
	ZEVector3::CrossProduct(CrossProductOutput, BoundingBox.Up, FrustumLeftFarEdge);
	if (ZEMath::Abs(CrossProductOutput.LengthSquare()) > Epsilon)
	{
		Result = SeparatingAxisTest(CrossProductOutput, Frustum, FrustumVertices, BoundingBox);
		CHECK_RESULT();
	}

	ZEVector3::CrossProduct(CrossProductOutput, BoundingBox.Up, FrustumLeftTopEdge);
	if (ZEMath::Abs(CrossProductOutput.LengthSquare()) > Epsilon)
	{
		Result = SeparatingAxisTest(CrossProductOutput, Frustum, FrustumVertices, BoundingBox);
		CHECK_RESULT();
	}

	// BoundingBox.Front Cross Products
	////////////////////////////////////////////////

	ZEVector3::CrossProduct(CrossProductOutput, BoundingBox.Front, FrustumLeftNearEdge);
	if (ZEMath::Abs(CrossProductOutput.LengthSquare()) > Epsilon)
	{
		Result = SeparatingAxisTest(CrossProductOutput, Frustum, FrustumVertices, BoundingBox);
		CHECK_RESULT();
	}

	ZEVector3::CrossProduct(CrossProductOutput, BoundingBox.Front, FrustumTopNearEdge);
	if (ZEMath::Abs(CrossProductOutput.LengthSquare()) > Epsilon)
	{
		Result = SeparatingAxisTest(CrossProductOutput, Frustum, FrustumVertices, BoundingBox);
		CHECK_RESULT();
	}

	ZEVector3::CrossProduct(CrossProductOutput, BoundingBox.Front, FrustumLeftBottomEdge);
	if (ZEMath::Abs(CrossProductOutput.LengthSquare()) > Epsilon)
	{
		Result = SeparatingAxisTest(CrossProductOutput, Frustum, FrustumVertices, BoundingBox);
		CHECK_RESULT();
	}
	
	ZEVector3::CrossProduct(CrossProductOutput, BoundingBox.Front, FrustumLeftTopEdge);
	if (ZEMath::Abs(CrossProductOutput.LengthSquare()) > Epsilon)
	{
		Result = SeparatingAxisTest(CrossProductOutput, Frustum, FrustumVertices, BoundingBox);
		CHECK_RESULT();
	}

	ZEVector3::CrossProduct(CrossProductOutput, BoundingBox.Front, FrustumRightBottomEdge);
	if (ZEMath::Abs(CrossProductOutput.LengthSquare()) > Epsilon)
	{
		Result = SeparatingAxisTest(CrossProductOutput, Frustum, FrustumVertices, BoundingBox);
		CHECK_RESULT();
	}

	ZEVector3::CrossProduct(CrossProductOutput, BoundingBox.Front, FrustumRightTopEdge);
	if (ZEMath::Abs(CrossProductOutput.LengthSquare()) > Epsilon)
	{
		Result = SeparatingAxisTest(CrossProductOutput, Frustum, FrustumVertices, BoundingBox);
		CHECK_RESULT();
	}

	ZEVector3::CrossProduct(CrossProductOutput, BoundingBox.Front, FrustumLeftFarEdge);
	if (ZEMath::Abs(CrossProductOutput.LengthSquare()) > Epsilon)
	{
		Result = SeparatingAxisTest(CrossProductOutput, Frustum, FrustumVertices, BoundingBox);
		CHECK_RESULT();
	}

	ZEVector3::CrossProduct(CrossProductOutput, BoundingBox.Front, FrustumTopFarEdge);
	if (ZEMath::Abs(CrossProductOutput.LengthSquare()) > Epsilon)
	{
		Result = SeparatingAxisTest(CrossProductOutput, Frustum, FrustumVertices, BoundingBox);
		CHECK_RESULT();
	}

	return TotalResult;
}

void ZEFrustum::Create(ZEFrustum& Frustum, const ZEVector3& Position, const ZEQuaternion& Rotation, float FOV, float AspectRatio, float NearZ, float FarZ)
{
	float SinFov = ZEAngle::Sin(FOV/2);
	float SinFovAT = SinFov * AspectRatio;
	float CosFov = ZEAngle::Cos(FOV/2);

	ZEQuaternion::VectorProduct(Frustum.TopPlane.n, Rotation, ZEVector3(0.0f, -CosFov, SinFov));
	ZEQuaternion::VectorProduct(Frustum.BottomPlane.n, Rotation, ZEVector3(0.0f, CosFov, SinFov));

	ZEQuaternion::VectorProduct(Frustum.RightPlane.n, Rotation, ZEVector3(-CosFov, 0.0f, SinFovAT));
	Frustum.RightPlane.n.NormalizeSelf();
	ZEQuaternion::VectorProduct(Frustum.LeftPlane.n, Rotation, ZEVector3(CosFov, 0.0f, SinFovAT));
	Frustum.LeftPlane.n.NormalizeSelf();

	ZEQuaternion::VectorProduct(Frustum.NearPlane.n, Rotation, ZEVector3(0.0f, 0.0f, 1.0f));
	ZEQuaternion::VectorProduct(Frustum.FarPlane.n, Rotation, ZEVector3(0.0f, 0.0f, -1.0f));

	Frustum.LeftPlane.p = Position;
	Frustum.RightPlane.p = Position;
	Frustum.TopPlane.p = Position;
	Frustum.BottomPlane.p = Position;

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
	Frustum.BottomPlane = DownPlane;
	Frustum.TopPlane = UpPlane;
	Frustum.NearPlane = NearPlane;
	Frustum.FarPlane = FarPlane;
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
