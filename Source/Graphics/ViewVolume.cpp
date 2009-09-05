//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ViewVolume.cpp
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

#include "ViewVolume.h"
#include "Core/Component.h"
#include "Core/Error.h"
#include "GameInterface/Entity.h"
#include "Graphics/Light.h"

bool ViewFrustumPlaneVSBox(const ZEAABoundingBox & Box, const ZEPlane &Plane)
{
	for (int I = 0; I < 8; I++)
		if (ZEPlane::TestHalfSpace(Plane, Box.GetVertex(I)) != ZEHALFSPACE_NEGATIVESIDE) 
			return true;
	return false;
}

bool ViewFrustumPlaneVSBox(const ZEOBoundingBox & Box, const ZEPlane &Plane)
{
	for (int I = 0; I < 8; I++)
		if (ZEPlane::TestHalfSpace(Plane,Box.GetVertex(I)) != ZEHALFSPACE_NEGATIVESIDE) 
			return true;
	return false;
}


bool ZEViewFrustum::ConeCullTest(const ZEBoundingSphere& BoundingSphere) const
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

	return TotalLength <= BoundingSphere.Radius + ExtensionLength;
}

bool ZEViewFrustum::CullTest(const ZEAABoundingBox& BoundingBox) const
{
	if (!ViewFrustumPlaneVSBox(BoundingBox, NearClippingPlane)
		|| !ViewFrustumPlaneVSBox(BoundingBox, FarClippingPlane)
		|| !ViewFrustumPlaneVSBox(BoundingBox, LeftClippingPlane)
		|| !ViewFrustumPlaneVSBox(BoundingBox, RightClippingPlane)
		|| !ViewFrustumPlaneVSBox(BoundingBox, TopClippingPlane)
		|| !ViewFrustumPlaneVSBox(BoundingBox, BottomClippingPlane))
		return false;

	return true;
}

bool ZEViewFrustum::CullTest(const ZEOBoundingBox& BoundingBox) const 
{
	if (!ViewFrustumPlaneVSBox(BoundingBox, NearClippingPlane)
		|| !ViewFrustumPlaneVSBox(BoundingBox, FarClippingPlane)
		|| !ViewFrustumPlaneVSBox(BoundingBox, LeftClippingPlane)
		|| !ViewFrustumPlaneVSBox(BoundingBox, RightClippingPlane)
		|| !ViewFrustumPlaneVSBox(BoundingBox, TopClippingPlane)
		|| !ViewFrustumPlaneVSBox(BoundingBox, BottomClippingPlane))
		return false;

	return true;
}

bool ZEViewFrustum::CullTest(const ZEBoundingSphere& BoundingSphere) const
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
	ZEBoundingSphere BoundingSphere;
	BoundingSphere.Position = Light->GetWorldPosition();
	BoundingSphere.Radius = Light->GetRange();
	return ConeCullTest(BoundingSphere);
}

bool ZEViewFrustum::CullTest(ZEEntity* Entity) const
{
	return /*ConeCullTest(Entity->GetWorldBoundingSphere() &&*/ CullTest(Entity->GetWorldBoundingBox());
}

bool ZEViewFrustum::CullTest(ZEComponent* Component) const
{
	return ConeCullTest(Component->GetWorldBoundingSphere())&& CullTest(Component->GetWorldBoundingBox());
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

	ZEQuaternion::Transform(TopClippingPlane.n, ZEVector3(0.0f, -CosFov, SinFov), Rotation);
	ZEQuaternion::Transform(BottomClippingPlane.n, ZEVector3(0.0f, CosFov, SinFov), Rotation);

	ZEQuaternion::Transform(RightClippingPlane.n, ZEVector3(-CosFov, 0.0f, SinFovAT), Rotation);
	ZEQuaternion::Transform(LeftClippingPlane.n, ZEVector3(CosFov, 0.0f, SinFovAT), Rotation);

	ZEQuaternion::Transform(NearClippingPlane.n, ZEVector3(0.0f, 0.0f, 1.0f), Rotation);
	ZEQuaternion::Transform(FarClippingPlane.n, ZEVector3(0.0f, 0.0f, -1.0f), Rotation);


	LeftClippingPlane.p = RightClippingPlane.p = TopClippingPlane.p = BottomClippingPlane.p = Position;

	ZEVector3 Temp;
	ZEVector3::Scale(Temp, NearClippingPlane.n, NearZ);
	ZEVector3::Add(NearClippingPlane.p, Position, Temp);

	ZEVector3::Scale(Temp, FarClippingPlane.n, -FarZ);
	ZEVector3::Add(FarClippingPlane.p, Position, Temp);

	// View cone

	this->Position = Position;
	ZEQuaternion::Transform(CenterVector, ZEVector3(0.0f, 0.0f, 1.0f), Rotation);

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
	ZEASSERT(true, "Not implamented");
	return false;
}

bool ZEViewSphere::CullTest(const ZEAABoundingBox& BoundingBox) const
{
	return ZEBoundingSphere::CollisionTest(BoundingSphere, BoundingBox);
}


bool ZEViewSphere::CullTest(ZEEntity* Entity) const
{
	if (ZEBoundingSphere::CollisionTest(Entity->GetWorldBoundingSphere(), BoundingSphere) 
		&& ZEAABoundingBox::CollisionTest(Entity->GetWorldBoundingBox(), BoundingSphere))
			return true;
	return false;
}

bool ZEViewSphere::CullTest(ZEComponent* Component) const
{
	ZEBoundingSphere ComponentBoundingSphere;
	ZEAABoundingBox ComponentBoundingBox;

	ZEWARNINGASSERT(true, "NearZ cull test not implamented");

	return ZEBoundingSphere::CollisionTest(Component->GetWorldBoundingSphere(), BoundingSphere) 
		&& ZEAABoundingBox::CollisionTest(Component->GetWorldBoundingBox(), BoundingSphere);
}

ZEDoorViewTest ZEViewSphere::CullTest(const ZERectangle3D& PortalDoor) const
{
	ZEASSERT(true, "NOT IMPLAMENTED");
	return ZE_DVT_INSIDE;
}

void ZEViewSphere::Create(const ZEVector3& Position, float Radious, float NearZ)
{
	BoundingSphere.Position = Position;
	BoundingSphere.Radius = Radious;
	this->NearZ = NearZ;
}


// ZEViewHemiSphere
ZEViewVolumeType ZEViewHemiSphere::GetViewVolumeType() const
{
	return ZE_VVT_HEMISPHERE;
}

bool ZEViewHemiSphere::LightCullTest(ZELight* Light) const
{
	ZEASSERT(true, "Not implamented");
	return false;
}

bool ZEViewHemiSphere::CullTest(const ZEAABoundingBox& BoundingBox) const
{
	return ZEAABoundingBox::PlaneHalfSpaceTest(BoundingBox, HalfPlane) != ZEHALFSPACE_NEGATIVESIDE 
		&& ZEBoundingSphere::CollisionTest(BoundingSphere, BoundingBox);
}

bool ZEViewHemiSphere::CullTest(ZEEntity* Entity) const
{
	return ZEBoundingSphere::PlaneHalfSpaceTest(Entity->GetWorldBoundingSphere(), HalfPlane) != ZEHALFSPACE_NEGATIVESIDE 
		&& ZEBoundingSphere::CollisionTest(BoundingSphere, Entity->GetWorldBoundingSphere())
		&& ZEAABoundingBox::PlaneHalfSpaceTest(Entity->GetWorldBoundingBox(), HalfPlane) != ZEHALFSPACE_NEGATIVESIDE 
		&& ZEAABoundingBox::CollisionTest(Entity->GetWorldBoundingBox(), BoundingSphere);
}

bool ZEViewHemiSphere::CullTest(ZEComponent* Component) const
{
	return ZEBoundingSphere::PlaneHalfSpaceTest(Component->GetWorldBoundingSphere(), HalfPlane) != ZEHALFSPACE_NEGATIVESIDE 
		&& ZEBoundingSphere::CollisionTest(BoundingSphere, Component->GetWorldBoundingSphere())
		&& ZEAABoundingBox::PlaneHalfSpaceTest(Component->GetWorldBoundingBox(), HalfPlane) != ZEHALFSPACE_NEGATIVESIDE 
		&& ZEAABoundingBox::CollisionTest(Component->GetWorldBoundingBox(), BoundingSphere);
}

ZEDoorViewTest ZEViewHemiSphere::CullTest(const ZERectangle3D& PortalDoor) const
{
	ZEASSERT(true, "NOT IMPLAMENTED");
	return ZE_DVT_INSIDE;
}

void ZEViewHemiSphere::Create(const ZEVector3& Position, const ZEVector3& Direction, float Radious, float NearZ)
{
	BoundingSphere.Position = Position;
	BoundingSphere.Radius = Radious;
	HalfPlane.p = Position;
	HalfPlane.n = Direction;
}


// ZEViewCuboid
ZEViewVolumeType ZEViewCuboid::GetViewVolumeType() const
{
	ZEASSERT(true, "Not implamented");
	return ZE_VVT_CUBOID;
}

bool ZEViewCuboid::CullTest(const ZEAABoundingBox& BoundingBox) const
{
	ZEWARNINGASSERT(true, "NOT IMPLAMENTED");
	//return ZEBoundingSphere::CollisionTest(Entity->GetWorldBoundingSphere(), BoundingBox);
	return true;
}

bool ZEViewCuboid::CullTest(ZEEntity* Entity) const
{
	ZEWARNINGASSERT(true, "NOT IMPLAMENTED");
	//return ZEBoundingSphere::CollisionTest(Entity->GetWorldBoundingSphere(), BoundingBox);
	return true;
}

bool ZEViewCuboid::CullTest(ZEComponent* Component) const
{
	ZEWARNINGASSERT(true, "NOT IMPLAMENTED");
	//return ZEBoundingSphere::CollisionTest(Component->GetWorldBoundingSphere(), BoundingBox);
	return true;
}

ZEDoorViewTest ZEViewCuboid::CullTest(const ZERectangle3D& PortalDoor) const
{
	ZEASSERT(true, "Not implamented");
	return ZE_DVT_INSIDE;
}

void ZEViewCuboid::Create(const ZEPosition3& Position, const ZEQuaternion& Rotation, float Width, float Height, float NearZ, float FarZ)
{
	ZEQuaternion::Transform(BoundingBox.U, ZEVector3(0.0f, Height, 0.0f), Rotation);
	ZEQuaternion::Transform(BoundingBox.V, ZEVector3(Width, 0.0f, 0.0f), Rotation);
	ZEQuaternion::Transform(BoundingBox.N, ZEVector3(0.0f, 0.0f, FarZ - NearZ), Rotation);
	
	ZEVector3 Near;
	ZEQuaternion::Transform(Near, ZEVector3(0.0f, 0.0f, NearZ), Rotation);
	
	BoundingBox.Position.x = Position.x - (BoundingBox.U.x + BoundingBox.V.x) * 0.5f + Near.x;
	BoundingBox.Position.y = Position.y - (BoundingBox.U.y + BoundingBox.V.y) * 0.5f + Near.y;
	BoundingBox.Position.z = Position.z - (BoundingBox.U.z + BoundingBox.V.z) * 0.5f + Near.z;
}

// ZEViewPlane
ZEViewVolumeType ZEViewPlane::GetViewVolumeType() const
{
	return ZE_VVT_PLANE;
}

bool ZEViewPlane::LightCullTest(ZELight* Light) const
{

	ZEASSERT(true, "Not implamented");
	return false;
}


bool ZEViewPlane::CullTest(const ZEAABoundingBox& BoundingBox) const
{
	ZEWARNINGASSERT(true, "NOT IMPLAMENTED");
	return true;
}

bool ZEViewPlane::CullTest(ZEEntity* Entity) const
{
	const ZEBoundingSphere& BoundingSphere = Entity->GetWorldBoundingSphere();
	if (ZEBoundingSphere::PlaneHalfSpaceTest(BoundingSphere, Plane) == ZEHALFSPACE_NEGATIVESIDE 
		|| ZEPlane::Distance(Plane, BoundingSphere.Position) - BoundingSphere.Radius > MaxDistance)
		return false;

	return true;
}

bool ZEViewPlane::CullTest(ZEComponent* Component) const
{
	const ZEBoundingSphere& BoundingSphere = Component->GetWorldBoundingSphere();
	if (ZEBoundingSphere::PlaneHalfSpaceTest(BoundingSphere, Plane) == ZEHALFSPACE_NEGATIVESIDE 
		|| ZEPlane::Distance(Plane, BoundingSphere.Position) - BoundingSphere.Radius > MaxDistance)
		return false;

	return true;
}

ZEDoorViewTest ZEViewPlane::CullTest(const ZERectangle3D& PortalDoor) const
{
	bool Visible = false;
	for (int I = 0; I < 4; I++)
	{
		const ZEVector3& Point = PortalDoor.GetPoint(I);
		if (ZEPlane::TestHalfSpace(Plane, Point) == ZEHALFSPACE_NEGATIVESIDE || ZEPlane::Distance(Plane, Point) > MaxDistance)
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
ZECullTestResult LineSegmentAgainstBordersTest(const ZEPoint3 &P1,const ZEPoint3 &P2,const ZEPlane & Border1,const ZEPlane & Border2);
ZEHalfSpace DoorAgainstPlanePreTest(const ZERectangle3D & Door,const ZEPlane & Plane);
void CutPortalDoorWithLine(ZEPoint3 & Out1,ZEPoint3 & Out2,const ZERectangle3D & Door,const ZELine & Line);

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
		
		if ((pretests[i] = DoorAgainstPlanePreTest(PortalDoor,GetClippingPlane(i))) == ZEHALFSPACE_NEGATIVESIDE)
			return ZE_DVT_OUTSIDE;
		if (pretests[i] == ZEHALFSPACE_INTERSECTS)
			status = ZE_DVT_DOORCOVERS;
		else
			Coverable = 0;
		i++;
	}

	if (status == ZE_DVT_INSIDE) return status;

	ZEPoint3 Point1,Point2;
	ZECullTestResult Result1 = ZE_COVERABLE,Result2;
	ZELine Intersection;

	if (pretests[0] = ZEHALFSPACE_INTERSECTS)
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

	if (pretests[1] = ZEHALFSPACE_INTERSECTS)
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

	if (pretests[2] = ZEHALFSPACE_INTERSECTS)
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

	if (pretests[3] = ZEHALFSPACE_INTERSECTS)
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

	ZEPosition3 WorldPosition = GetWorldPosition();

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

ZECullTestResult LineSegmentAgainstBordersTest(const ZEPoint3 &P1,const ZEPoint3 &P2,const ZEPlane & Border1,const ZEPlane & Border2)
{
	if (ZEPlane::TestHalfSpace(Border1,P1) == ZEHALFSPACE_NEGATIVESIDE)
	{
		if (ZEPlane::TestHalfSpace(Border2,P2) == ZEHALFSPACE_NEGATIVESIDE)
			return ZE_COVERABLE;
		else if (ZEPlane::TestHalfSpace(Border1,P2) == ZEHALFSPACE_NEGATIVESIDE)
			return ZE_OUTSIDE_BY_FIRST_PLANE;
		else 
			return ZE_INTERSECTION;
	}
	else if (ZEPlane::TestHalfSpace(Border1,P2) == ZEHALFSPACE_NEGATIVESIDE)
	{
		if (ZEPlane::TestHalfSpace(Border2,P1) == ZEHALFSPACE_NEGATIVESIDE)
			return ZE_COVERABLE;
		else
			return ZE_INTERSECTION;
	}
	else if (ZEPlane::TestHalfSpace(Border2,P1) == ZEHALFSPACE_POSITIVESIDE ||
			 ZEPlane::TestHalfSpace(Border2,P2) == ZEHALFSPACE_POSITIVESIDE)
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

	return ZEHALFSPACE_INTERSECTS;
}

bool CutLineWithLine(ZEVector3 & Out,const ZELine & Line1,const ZELine & Line2) // Line 2 is PortalDoor Line
{
	float V1dotV2 = ZEVector3::DotProduct(Line2.v,Line1.v);
	if (V1dotV2 == 0) return 0;
	float V1dotV1 = ZEVector3::DotProduct(Line1.v,Line1.v);

	ZEPoint3 V2C,P1C;
	ZEPoint3 temp;
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

void CutPortalDoorWithLine(ZEPoint3 & Out1,ZEPoint3 & Out2,const ZERectangle3D & Door,const ZELine & Line)
{
	bool cut = 0;
	ZEPoint3 point;
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

	ZEASSERT(true, "PortalDoorVSFrustumPlaneIntersection test error, Ozan should control it");
}*/
