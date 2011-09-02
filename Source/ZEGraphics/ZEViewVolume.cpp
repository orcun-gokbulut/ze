//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEViewVolume.cpp
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

#include "ZEViewVolume.h"
#include "ZEGame/ZEComponent.h"
#include "ZECore/ZEError.h"
#include "ZEGame/ZEEntity.h"
#include "ZEGraphics/ZELight.h"

bool ZEViewFrustum::ConeCullTest(const ZEBSphere& BoundingSphere) const
{
	// it is assumed that CenterVector is normalized
	ZEVector3 SphereMinusCone;
	ZEVector3 SphereCenterProj;
	ZEVector3 SphereCenterNormal;

	ZEVector3::Sub(SphereMinusCone, BoundingSphere.Position, Position);
	float ProjLength = ZEVector3::DotProduct(SphereMinusCone, CenterVector);
	ZEVector3::Scale(SphereCenterProj, CenterVector,ProjLength);
	ZEVector3::Sub(SphereCenterNormal, SphereMinusCone,SphereCenterProj);
	
	float NormalLength = ZEVector3::Length(SphereCenterNormal);
	float ExtensionLength = TanRatio * NormalLength;
	float TotalLength = NormalLength * (1/SinRatio);
	
	ExtensionLength = (ProjLength + ExtensionLength) * SinRatio; // ConeRadius

	return TotalLength > BoundingSphere.Radius + ExtensionLength;
}

bool ZEViewFrustum::CullTest(const ZEOBBox& BoundingBox) const
{
	if ((ZEOBBox::IntersectionTest(BoundingBox, NearClippingPlane)	== ZE_HS_NEGATIVE_SIDE) ||
		(ZEOBBox::IntersectionTest(BoundingBox, FarClippingPlane)		== ZE_HS_NEGATIVE_SIDE) ||
		(ZEOBBox::IntersectionTest(BoundingBox, LeftClippingPlane)	== ZE_HS_NEGATIVE_SIDE) ||
		(ZEOBBox::IntersectionTest(BoundingBox, RightClippingPlane)	== ZE_HS_NEGATIVE_SIDE) ||
		(ZEOBBox::IntersectionTest(BoundingBox, TopClippingPlane)		== ZE_HS_NEGATIVE_SIDE) ||
		(ZEOBBox::IntersectionTest(BoundingBox, BottomClippingPlane)	== ZE_HS_NEGATIVE_SIDE))
		return true;

	return false;
}

bool ZEViewFrustum::CullTest(const ZEAABBox& BoundingBox) const
{
	if ((ZEAABBox::IntersectionTest(BoundingBox, NearClippingPlane)	== ZE_HS_NEGATIVE_SIDE) ||
		(ZEAABBox::IntersectionTest(BoundingBox, FarClippingPlane)		== ZE_HS_NEGATIVE_SIDE) ||
		(ZEAABBox::IntersectionTest(BoundingBox, LeftClippingPlane)	== ZE_HS_NEGATIVE_SIDE) ||
		(ZEAABBox::IntersectionTest(BoundingBox, RightClippingPlane)	== ZE_HS_NEGATIVE_SIDE) ||
		(ZEAABBox::IntersectionTest(BoundingBox, TopClippingPlane)		== ZE_HS_NEGATIVE_SIDE) ||
		(ZEAABBox::IntersectionTest(BoundingBox, BottomClippingPlane)	== ZE_HS_NEGATIVE_SIDE))
		return true;

	return false;
}

bool ZEViewFrustum::CullTest(const ZEBSphere& BoundingSphere) const
{
	float distanceSquare = 0;
	float SphereDotN, NDotN;
	ZEVector3 Subs;

	ZEVector3::Sub(Subs,BoundingSphere.Position, NearClippingPlane.p);
	SphereDotN = ZEVector3::DotProduct(Subs,NearClippingPlane.n);
	if ( SphereDotN > 0)
	{
		ZEVector3::Sub(Subs,BoundingSphere.Position, FarClippingPlane.p);
		SphereDotN = ZEVector3::DotProduct(Subs, FarClippingPlane.n);
		if ( SphereDotN < 0)
		{
			NDotN = ZEVector3::DotProduct(FarClippingPlane.n, FarClippingPlane.n);
			distanceSquare += (SphereDotN * SphereDotN) / NDotN;
		}
	}
	else
	{
		NDotN = ZEVector3::DotProduct(NearClippingPlane.n, NearClippingPlane.n);
		distanceSquare += (SphereDotN * SphereDotN) / NDotN;
	}

	ZEVector3::Sub(Subs,BoundingSphere.Position, LeftClippingPlane.p);
	SphereDotN = ZEVector3::DotProduct(Subs, LeftClippingPlane.n);
	if ( SphereDotN > 0)
	{
		ZEVector3::Sub(Subs,BoundingSphere.Position, RightClippingPlane.p);
		SphereDotN = ZEVector3::DotProduct(Subs, RightClippingPlane.n);
		if ( SphereDotN < 0)
		{
			NDotN = ZEVector3::DotProduct(RightClippingPlane.n, RightClippingPlane.n);
			distanceSquare += (SphereDotN * SphereDotN) / NDotN;
		}
	}
	else
	{
		NDotN = ZEVector3::DotProduct(LeftClippingPlane.n, LeftClippingPlane.n);
		distanceSquare += (SphereDotN * SphereDotN) / NDotN;
	}

	ZEVector3::Sub(Subs,BoundingSphere.Position, TopClippingPlane.p);
	SphereDotN = ZEVector3::DotProduct(Subs, TopClippingPlane.n);
	if ( SphereDotN > 0)
	{
		ZEVector3::Sub(Subs,BoundingSphere.Position, BottomClippingPlane.p);
		SphereDotN = ZEVector3::DotProduct(Subs, BottomClippingPlane.n);
		if ( SphereDotN < 0)
		{
			NDotN = ZEVector3::DotProduct(BottomClippingPlane.n, BottomClippingPlane.n);
			distanceSquare += (SphereDotN * SphereDotN) / NDotN;
		}
	}
	else
	{
		NDotN = ZEVector3::DotProduct(TopClippingPlane.n, TopClippingPlane.n);
		distanceSquare += (SphereDotN * SphereDotN) / NDotN;
	}

	return sqrt(distanceSquare) <= BoundingSphere.Radius;
}

ZEViewVolumeType ZEViewFrustum::GetViewVolumeType() const
{
	return ZE_VVT_FRUSTUM;
}

bool ZEViewFrustum::LightCullTest(ZELight* Light) const
{
	ZEBSphere BoundingSphere;
	BoundingSphere.Position = Light->GetWorldPosition();
	BoundingSphere.Radius = Light->GetRange();
	//return ConeCullTest(BoundingSphere);
	return false;
}

bool ZEViewFrustum::CullTest(ZEEntity* Entity) const
{
	return CullTest(Entity->GetWorldBoundingBox());
}

ZEDoorViewTest ZEViewFrustum::CullTest(const ZERectangle3D& PortalDoor) const
{
	return ZE_DVT_INSIDE;
}

void ZEViewFrustum::Create(const ZEVector3& Position, const ZEQuaternion& Rotation, float FOV, float AspectRatio, float NearZ, float FarZ)
{
	float SinFov = sinf(FOV/2);
	float SinFovAT = SinFov * AspectRatio;
	float CosFov = cosf(FOV/2);

	ZEQuaternion::VectorProduct(TopClippingPlane.n, Rotation, ZEVector3(0.0f, -CosFov, SinFov));
	ZEQuaternion::VectorProduct(BottomClippingPlane.n, Rotation, ZEVector3(0.0f, CosFov, SinFov));

	ZEQuaternion::VectorProduct(RightClippingPlane.n, Rotation, ZEVector3(-CosFov, 0.0f, SinFovAT));
	ZEQuaternion::VectorProduct(LeftClippingPlane.n, Rotation, ZEVector3(CosFov, 0.0f, SinFovAT));

	ZEQuaternion::VectorProduct(NearClippingPlane.n, Rotation, ZEVector3(0.0f, 0.0f, 1.0f));
	ZEQuaternion::VectorProduct(FarClippingPlane.n, Rotation, ZEVector3(0.0f, 0.0f, -1.0f));


	LeftClippingPlane.p = RightClippingPlane.p = TopClippingPlane.p = BottomClippingPlane.p = Position;

	ZEVector3 Temp;
	ZEVector3::Scale(Temp, NearClippingPlane.n, NearZ);
	ZEVector3::Add(NearClippingPlane.p, Position, Temp);

	ZEVector3::Scale(Temp, FarClippingPlane.n, -FarZ);
	ZEVector3::Add(FarClippingPlane.p, Position, Temp);

	// View cone

	this->Position = Position;
	ZEQuaternion::VectorProduct(CenterVector, Rotation, ZEVector3(0.0f, 0.0f, 1.0f));

	float tanfov = tanf(FOV / 2.0f);
	float tHeight = tanfov * NearZ * 2.0f;
	float tWidth = AspectRatio * tHeight;

	float radius = sqrt(tWidth * tWidth + tHeight * tHeight) / 2.0f;
	TanRatio = radius / NearZ;
	SinRatio = radius / sqrt(radius * radius + NearZ * NearZ);
}

// ZEViewSphere
ZEViewVolumeType ZEViewSphere::GetViewVolumeType() const
{
	return ZE_VVT_SPHERE;
}

bool ZEViewSphere::LightCullTest(ZELight* Light) const
{
	zeAssert(true, "Not implamented");
	return false;
}

bool ZEViewSphere::CullTest(const ZEAABBox& BoundingBox) const
{
	return false; //!ZEBSphere::CollisionTest(BoundingSphere, BoundingBox);
}

bool ZEViewSphere::CullTest(ZEEntity* Entity) const
{
	if (!ZEAABBox::IntersectionTest(Entity->GetWorldBoundingBox(), BoundingSphere))
			return true;
	return false;
}

ZEDoorViewTest ZEViewSphere::CullTest(const ZERectangle3D& PortalDoor) const
{
	zeAssert(true, "NOT IMPLAMENTED");
	return ZE_DVT_INSIDE;
}

void ZEViewSphere::Create(const ZEVector3& Position, float Radius, float NearZ)
{
	BoundingSphere.Position = Position;
	BoundingSphere.Radius = Radius;
	this->NearZ = NearZ;
}


// ZEViewHemiSphere
ZEViewVolumeType ZEViewHemiSphere::GetViewVolumeType() const
{
	return ZE_VVT_HEMISPHERE;
}

bool ZEViewHemiSphere::LightCullTest(ZELight* Light) const
{
	zeAssert(true, "Not implamented");
	return false;
}

bool ZEViewHemiSphere::CullTest(const ZEAABBox& BoundingBox) const
{
	return false; /*ZEAABBox::PlaneHalfSpaceTest(BoundingBox, HalfPlane) == ZE_HS_NEGATIVE_SIDE 
		|| !ZEBSphere::CollisionTest(BoundingSphere, BoundingBox);*/
}

bool ZEViewHemiSphere::CullTest(ZEEntity* Entity) const
{
	return 
//		ZEBSphere::PlaneHalfSpaceTest(Entity->GetWorldBoundingSphere(), HalfPlane) == ZE_HS_NEGATIVESIDE ||
//		!ZEBSphere::CollisionTest(BoundingSphere, Entity->GetWorldBoundingSphere()) ||
		ZEAABBox::IntersectionTest(Entity->GetWorldBoundingBox(), HalfPlane) == ZE_HS_NEGATIVE_SIDE ||
		!ZEAABBox::IntersectionTest(Entity->GetWorldBoundingBox(), BoundingSphere);
}

ZEDoorViewTest ZEViewHemiSphere::CullTest(const ZERectangle3D& PortalDoor) const
{
	zeAssert(true, "NOT IMPLAMENTED");
	return ZE_DVT_INSIDE;
}

void ZEViewHemiSphere::Create(const ZEVector3& Position, const ZEVector3& Direction, float Radius, float NearZ)
{
	BoundingSphere.Position = Position;
	BoundingSphere.Radius = Radius;
	HalfPlane.p = Position;
	HalfPlane.n = Direction;
}


// ZEViewCuboid
ZEViewVolumeType ZEViewCuboid::GetViewVolumeType() const
{
	zeAssert(true, "Not implamented");
	return ZE_VVT_CUBOID;
}

bool ZEViewCuboid::CullTest(const ZEAABBox& BoundingBox) const
{
	zeWarningAssert(true, "NOT IMPLAMENTED");
	//return ZEBSphere::CollisionTest(Entity->GetWorldBoundingSphere(), BoundingBox);
	return true;
}

bool ZEViewCuboid::CullTest(ZEEntity* Entity) const
{
	zeWarningAssert(true, "NOT IMPLAMENTED");
	//return ZEBSphere::CollisionTest(Entity->GetWorldBoundingSphere(), BoundingBox);
	return true;
}

ZEDoorViewTest ZEViewCuboid::CullTest(const ZERectangle3D& PortalDoor) const
{
	zeAssert(true, "Not implamented");
	return ZE_DVT_INSIDE;
}

void ZEViewCuboid::Create(const ZEVector3& Position, const ZEQuaternion& Rotation, float Width, float Height, float NearZ, float FarZ)
{
	ZEQuaternion::VectorProduct(BoundingBox.Right, Rotation, ZEVector3::UnitX);
	ZEQuaternion::VectorProduct(BoundingBox.Up, Rotation, ZEVector3::UnitY);
	ZEQuaternion::VectorProduct(BoundingBox.Front, Rotation, ZEVector3::UnitZ);
	
	BoundingBox.HalfSize = ZEVector3(Width * 0.5f, Height * 0.5f, (FarZ - NearZ) * 0.5f);

	ZEVector3 Center;
	ZEQuaternion::VectorProduct(Center, Rotation, ZEVector3(0.0f, 0.0f, FarZ - NearZ));
	
	ZEVector3::Multiply(BoundingBox.Center, Position, Center);
}

// ZEViewPlane
ZEViewVolumeType ZEViewPlane::GetViewVolumeType() const
{
	return ZE_VVT_PLANE;
}

bool ZEViewPlane::LightCullTest(ZELight* Light) const
{

	zeAssert(true, "Not implamented");
	return false;
}


bool ZEViewPlane::CullTest(const ZEAABBox& BoundingBox) const
{
	zeWarningAssert(true, "NOT IMPLAMENTED");
	return true;
}

bool ZEViewPlane::CullTest(ZEEntity* Entity) const
{
/*	const ZEBSphere& BoundingSphere = Entity->GetWorldBoundingSphere();
	if (ZEBSphere::PlaneHalfSpaceTest(BoundingSphere, Plane) == ZE_HS_NEGATIVESIDE ||
		ZEPlane::Distance(Plane, BoundingSphere.Position) - BoundingSphere.Radius > MaxDistance)
		return true;*/

	return false;
}

ZEDoorViewTest ZEViewPlane::CullTest(const ZERectangle3D& PortalDoor) const
{
	bool Visible = false;
	for (int I = 0; I < 4; I++)
	{
		const ZEVector3& Point = PortalDoor.GetPoint(I);
		if (ZEPlane::TestHalfSpace(Plane, Point) == ZE_HS_NEGATIVE_SIDE || ZEPlane::Distance(Plane, Point) > MaxDistance)
			Visible = true;
		else
			if (Visible)
				return ZE_DVT_INTERSECTS;

	}

	return (Visible ? ZE_DVT_INSIDE: ZE_DVT_OUTSIDE);
}

void ZEViewPlane::Create(const ZEVector3& Position, const ZEVector3& Direction, float MaxDistance)
{
	Plane.p = Position;
	Plane.n = Direction;
	this->MaxDistance = MaxDistance;
}

/*
//----------------------------------CULLING----------------------------------//
//----------------------------------CULLING----------------------------------//
//----------------------------------CULLING----------------------------------//
//----------------------------------CULLING----------------------------------//


enum ZECullTestResult
{
	ZE_COVERABLE,
	ZE_OUTSIDE_BY_FIRST_PLANE,
	ZE_OUTSIDE_BY_SECOND_PLANE,
	ZE_INTERSECTION
};
ZECullTestResult LineSegmentAgainstBordersTest(const ZEVector3 &P1,const ZEVector3 &P2,const ZEPlane & Border1,const ZEPlane & Border2);
ZEHalfSpace DoorAgainstPlanePreTest(const ZERectangle3D & Door,const ZEPlane & Plane);
void CutPortalDoorWithLine(ZEVector3 & Out1,ZEVector3 & Out2,const ZERectangle3D & Door,const ZELine & Line);

ZEDoorViewTest ZECamera::ViewFrustumCullTest(const ZERectangle3D& PortalDoor) 
{
	ZEPlane DoorPlane;
	PortalDoor.GetPlane(DoorPlane);
	ZEHalfSpace pretests[6];

	ZEDoorViewTest status = ZE_DVT_INSIDE;
	bool Coverable = 1;

	int i = 0;
	while (1)
	{
		if (i == 6) break;
		
		if ((pretests[i] = DoorAgainstPlanePreTest(PortalDoor,GetClippingPlane(i))) == ZE_HS_NEGATIVESIDE)
			return ZE_DVT_OUTSIDE;
		if (pretests[i] == ZE_HS_INTERSECTS)
			status = ZE_DVT_DOORCOVERS;
		else
			Coverable = 0;
		i++;
	}

	if (status == ZE_DVT_INSIDE) return status;

	ZEVector3 Point1,Point2;
	ZECullTestResult Result1 = ZE_COVERABLE,Result2;
	ZELine Intersection;

	if (pretests[0] = ZE_HS_INTERSECTS)
	{
		ZEGeometryUtils::IntersectPlane_Plane(Intersection,DoorPlane,RightClippingPlane);

		CutPortalDoorWithLine(Point1,Point2,PortalDoor,Intersection);
		Result1 = LineSegmentAgainstBordersTest(Point1,Point2,TopClippingPlane,BottomClippingPlane);

		if (Result1 == ZE_INTERSECTION)
			return ZE_DVT_INTERSECTS;
		if (Result1 == ZE_COVERABLE) 
		{
			if (Coverable != 1)
				return ZE_DVT_INTERSECTS;
		}
		else
			Coverable = 0;
	}

	if (pretests[1] = ZE_HS_INTERSECTS)
	{
		ZEGeometryUtils::IntersectPlane_Plane(Intersection,DoorPlane,LeftClippingPlane);

		CutPortalDoorWithLine(Point1,Point2,PortalDoor,Intersection);
		Result2 = LineSegmentAgainstBordersTest(Point1,Point2,TopClippingPlane,BottomClippingPlane);

		if (Coverable == 1)
		{
			if (Result2 != ZE_COVERABLE)
				return ZE_DVT_INTERSECTS;
		}
		else if (Result2 == ZE_COVERABLE || Result2 == ZE_INTERSECTION)
			return ZE_DVT_INTERSECTS;
		else if (Result2 != Result1 && Result1 != ZE_COVERABLE)
			return ZE_DVT_INTERSECTS;
	}

	if (pretests[2] = ZE_HS_INTERSECTS)
	{
		ZEGeometryUtils::IntersectPlane_Plane(Intersection,DoorPlane,TopClippingPlane);

		CutPortalDoorWithLine(Point1,Point2,PortalDoor,Intersection);
		Result1 = LineSegmentAgainstBordersTest(Point1,Point2,RightClippingPlane,LeftClippingPlane);

		if (Coverable == 1)
		{
			if (Result1 != ZE_COVERABLE)
				return ZE_DVT_INTERSECTS;
		}
		else if (Result1 == ZE_COVERABLE || Result1 == ZE_INTERSECTION)
			return ZE_DVT_INTERSECTS;
	}

	if (pretests[3] = ZE_HS_INTERSECTS)
	{
		ZEGeometryUtils::IntersectPlane_Plane(Intersection,DoorPlane,BottomClippingPlane);

		CutPortalDoorWithLine(Point1,Point2,PortalDoor,Intersection);
		Result1 = LineSegmentAgainstBordersTest(Point1,Point2,RightClippingPlane,LeftClippingPlane);

		if (Coverable == 1)
		{
			if (Result1 != ZE_COVERABLE)
				return ZE_DVT_INTERSECTS;
		}
		else if (Result1 == ZE_COVERABLE || Result1 == ZE_INTERSECTION)
			return ZE_DVT_INTERSECTS;
	}

	if (Coverable == 1) return ZE_DVT_DOORCOVERS;
	return ZE_DVT_OUTSIDE;

}


void ZECamera::GenerateCameraFromDoor(ZEViewCone & Cone,ZECamera & Camera,const ZERectangle3D & Door)
{
	// Just Clipping Plane

	Camera.NearClippingPlane = NearClippingPlane;
	Camera.FarClippingPlane = FarClippingPlane;

	ZEVector3 WorldPosition = GetWorldPosition();

	ZEPlane::Create(RightClippingPlane,WorldPosition,Door.P2,Door.P1);
	ZEPlane::Create(TopClippingPlane,WorldPosition,Door.P3,Door.P2);
	ZEPlane::Create(LeftClippingPlane,WorldPosition,Door.P4,Door.P3);
	ZEPlane::Create(BottomClippingPlane,WorldPosition,Door.P1,Door.P4);

	Cone.Position = WorldPosition;

	ZEVector3 DoorCenterPosition;
	ZEVector3::Add(DoorCenterPosition,Door.P3,Door.P1);
	ZEVector3::Scale(DoorCenterPosition,DoorCenterPosition,0.5);
	ZEVector3::Sub(Cone.CenterVector,DoorCenterPosition,WorldPosition);
	
	ZEVector3::Sub(DoorCenterPosition,Door.P1,WorldPosition);
	
	float P1DotP1 = ZEVector3::DotProduct(DoorCenterPosition,DoorCenterPosition);
	float CDotC = ZEVector3::DotProduct(Cone.CenterVector,Cone.CenterVector);
	float Both = ZEVector3::DotProduct(Cone.CenterVector,DoorCenterPosition);

	float cosTeta = (Both * Both)/(P1DotP1 * CDotC);
	Cone.SinRatio = sqrt(1 - cosTeta);
	Cone.TanRatio = Cone.SinRatio / sqrt(cosTeta);
}




//-----------------PORTALDOOR CULLING HELPERS--------------------//
//-----------------PORTALDOOR CULLING HELPERS--------------------//

ZECullTestResult LineSegmentAgainstBordersTest(const ZEVector3 &P1,const ZEVector3 &P2,const ZEPlane & Border1,const ZEPlane & Border2)
{
	if (ZEPlane::TestHalfSpace(Border1,P1) == ZE_HS_NEGATIVESIDE)
	{
		if (ZEPlane::TestHalfSpace(Border2,P2) == ZE_HS_NEGATIVESIDE)
			return ZE_COVERABLE;
		else if (ZEPlane::TestHalfSpace(Border1,P2) == ZE_HS_NEGATIVESIDE)
			return ZE_OUTSIDE_BY_FIRST_PLANE;
		else 
			return ZE_INTERSECTION;
	}
	else if (ZEPlane::TestHalfSpace(Border1,P2) == ZE_HS_NEGATIVESIDE)
	{
		if (ZEPlane::TestHalfSpace(Border2,P1) == ZE_HS_NEGATIVESIDE)
			return ZE_COVERABLE;
		else
			return ZE_INTERSECTION;
	}
	else if (ZEPlane::TestHalfSpace(Border2,P1) == ZE_HS_POSITIVESIDE ||
			 ZEPlane::TestHalfSpace(Border2,P2) == ZE_HS_POSITIVESIDE)
		return ZE_INTERSECTION;
	return ZE_OUTSIDE_BY_SECOND_PLANE;
}

ZEHalfSpace DoorAgainstPlanePreTest(const ZERectangle3D & Door,const ZEPlane & Plane)
{
	ZEHalfSpace test = ZEPlane::TestHalfSpace(Plane,Door.P1);

	if (test == ZEPlane::TestHalfSpace(Plane,Door.P2))
		if (test == ZEPlane::TestHalfSpace(Plane,Door.P3))
			if (test == ZEPlane::TestHalfSpace(Plane,Door.P4))
				return test;

	return ZE_HS_INTERSECTS;
}

bool CutLineWithLine(ZEVector3 & Out,const ZELine & Line1,const ZELine & Line2) // Line 2 is PortalDoor Line
{
	float V1dotV2 = ZEVector3::DotProduct(Line2.v,Line1.v);
	if (V1dotV2 == 0) return 0;
	float V1dotV1 = ZEVector3::DotProduct(Line1.v,Line1.v);

	ZEVector3 V2C,P1C;
	ZEVector3 temp;
	ZEVector3::Sub(temp,Line2.p,Line1.p);
	ZEVector3::Scale(P1C,Line1.v,(ZEVector3::DotProduct(Line1.v,temp) / V1dotV1));
	ZEVector3::Scale(V2C,Line1.v,V1dotV2 / V1dotV1);

	ZEVector3::Sub(P1C,P1C,Line2.p);
	ZEVector3::Sub(V2C,Line2.v,V2C);
	float t2;
	if (V2C.x != 0) 
		t2 = P1C.x / V2C.x;
	else if (V2C.y != 0) 
		t2 = P1C.y / V2C.y;
	else 
		t2 = P1C.z / V2C.z;

	if (t2 > 1 || t2 < 0) return 0;

	ZEVector3::Scale(Out,Line2.v,t2);
	ZEVector3::Add(Out,Out,Line2.p);
	return 1;
}

void CutPortalDoorWithLine(ZEVector3 & Out1,ZEVector3 & Out2,const ZERectangle3D & Door,const ZELine & Line)
{
	bool cut = 0;
	ZEVector3 point;
	ZELine DoorLine;
	
	for (unsinged int I = 0; I < 4; I++)
	{
		switch(I)
		{
			case 0:
				DoorLine = ZELine(Door.P1, Door.P2);
				break;
			case 1:
				DoorLine = ZELine(Door.P2, Door.P3);
				break;
			case 2:
				DoorLine = ZELine(Door.P3, Door.P4);
				break;
			case 3:
				DoorLine = ZELine(Door.P4, Door.P1);
				break;
		}

		if (CutLineWithLine(point, Line, DoorLine))
			if (cut == 1)
			{
				Out2 = point;
				return;
			}
			else
			{
				cut++;
				Out1 = point;
			}
	}

	zeAssert(true, "PortalDoorVSFrustumPlaneIntersection test error, Ozan should control it");
}*/




