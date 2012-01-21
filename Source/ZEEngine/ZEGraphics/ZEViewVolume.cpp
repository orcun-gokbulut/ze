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
#include "ZEError.h"
#include "ZEGame/ZEEntity.h"
#include "ZEGraphics/ZELight.h"
#include "ZEMath/ZEAngle.h"

/*bool ZEViewFrustum::ConeCullTest(const ZEBSphere& BoundingSphere) const
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
}*/

bool ZEViewFrustum::CullTest(const ZEBSphere& BoundingSphere) const
{
	if ((ZEBSphere::IntersectionTest(BoundingSphere, NearClippingPlane)		== ZE_HS_NEGATIVE_SIDE) ||
		(ZEBSphere::IntersectionTest(BoundingSphere, LeftClippingPlane)		== ZE_HS_NEGATIVE_SIDE) ||
		(ZEBSphere::IntersectionTest(BoundingSphere, RightClippingPlane)		== ZE_HS_NEGATIVE_SIDE) ||
		(ZEBSphere::IntersectionTest(BoundingSphere, TopClippingPlane)		== ZE_HS_NEGATIVE_SIDE) ||
		(ZEBSphere::IntersectionTest(BoundingSphere, FarClippingPlane)		== ZE_HS_NEGATIVE_SIDE) ||
		(ZEBSphere::IntersectionTest(BoundingSphere, BottomClippingPlane)	== ZE_HS_NEGATIVE_SIDE))
		return true;

	return false;
}

bool ZEViewFrustum::CullTest(const ZEAABBox& BoundingBox) const
{
	if ((ZEAABBox::IntersectionTest(BoundingBox, NearClippingPlane)		== ZE_HS_NEGATIVE_SIDE) ||
		(ZEAABBox::IntersectionTest(BoundingBox, LeftClippingPlane)		== ZE_HS_NEGATIVE_SIDE) ||
		(ZEAABBox::IntersectionTest(BoundingBox, RightClippingPlane)	== ZE_HS_NEGATIVE_SIDE) ||
		(ZEAABBox::IntersectionTest(BoundingBox, TopClippingPlane)		== ZE_HS_NEGATIVE_SIDE) ||
		(ZEAABBox::IntersectionTest(BoundingBox, FarClippingPlane)		== ZE_HS_NEGATIVE_SIDE) ||
		(ZEAABBox::IntersectionTest(BoundingBox, BottomClippingPlane)	== ZE_HS_NEGATIVE_SIDE))
		return true;

	return false;
}

bool ZEViewFrustum::CullTest(const ZEOBBox& BoundingBox) const
{
	if ((ZEOBBox::IntersectionTest(BoundingBox, NearClippingPlane)		== ZE_HS_NEGATIVE_SIDE) ||
		(ZEOBBox::IntersectionTest(BoundingBox, LeftClippingPlane)		== ZE_HS_NEGATIVE_SIDE) ||
		(ZEOBBox::IntersectionTest(BoundingBox, RightClippingPlane)		== ZE_HS_NEGATIVE_SIDE) ||
		(ZEOBBox::IntersectionTest(BoundingBox, TopClippingPlane)		== ZE_HS_NEGATIVE_SIDE) ||
		(ZEOBBox::IntersectionTest(BoundingBox, FarClippingPlane)		== ZE_HS_NEGATIVE_SIDE) ||
		(ZEOBBox::IntersectionTest(BoundingBox, BottomClippingPlane)	== ZE_HS_NEGATIVE_SIDE))
		return true;

	return false;
}

ZEViewVolumeType ZEViewFrustum::GetViewVolumeType() const
{
	return ZE_VVT_FRUSTUM;
}

bool ZEViewFrustum::CullTest(ZELight* Light) const
{
	if (Light->GetLightType() == ZE_LT_DIRECTIONAL)
		return false;

	ZEBSphere BoundingSphere;
	BoundingSphere.Position = Light->GetWorldPosition();
	BoundingSphere.Radius = Light->GetRange();

	return CullTest(BoundingSphere);
}

bool ZEViewFrustum::CullTest(ZEEntity* Entity) const
{
	return CullTest(Entity->GetWorldBoundingBox());
}

ZEDoorViewTest ZEViewFrustum::CullTest(const ZERectangle3D& PortalDoor) const
{
	return ZE_DVT_INSIDE;
}

ZEArray<ZELineSegment> ZEViewFrustum::GetFrustumLineSegments()
{
	ZEArray<ZELineSegment> LineSegments;
	float Fov_2 = Fov / 2;

	ZEVector3 NearPlaneCenter = Position + (Look * NearZ);
	ZEVector3 FarPlaneCenter = Position + (Look * FarZ);

	float NearPlaneHeight = NearZ * tan(Fov_2);
	float FarPlaneHeight = FarZ * tan(Fov_2);

	float NearPlaneWidth = NearPlaneHeight * AspectRatio;
	float FarPlaneWidth = FarPlaneHeight * AspectRatio;

	ZEVector3 PN1, PN2, PN3, PN4, PF1, PF2, PF3, PF4;

	PN1 = NearPlaneCenter - (Up * NearPlaneHeight) + (Right * NearPlaneWidth);
	PN2 = NearPlaneCenter + (Up * NearPlaneHeight) + (Right * NearPlaneWidth);
	PN3 = NearPlaneCenter + (Up * NearPlaneHeight) - (Right * NearPlaneWidth);
	PN4 = NearPlaneCenter - (Up * NearPlaneHeight) - (Right * NearPlaneWidth);

	PF1 = FarPlaneCenter - (Up * FarPlaneHeight) + (Right * FarPlaneWidth);
	PF2 = FarPlaneCenter + (Up * FarPlaneHeight) + (Right * FarPlaneWidth);
	PF3 = FarPlaneCenter + (Up * FarPlaneHeight) - (Right * FarPlaneWidth);
	PF4 = FarPlaneCenter - (Up * FarPlaneHeight) - (Right * FarPlaneWidth);

	LineSegments.Add(ZELineSegment(PN1, PN2));
	LineSegments.Add(ZELineSegment(PN2, PN3));
	LineSegments.Add(ZELineSegment(PN3, PN4));
	LineSegments.Add(ZELineSegment(PN4, PN1));

	LineSegments.Add(ZELineSegment(PF1, PF2));
	LineSegments.Add(ZELineSegment(PF2, PF3));
	LineSegments.Add(ZELineSegment(PF3, PF4));
	LineSegments.Add(ZELineSegment(PF4, PF1));

	LineSegments.Add(ZELineSegment(PN1, PF1));
	LineSegments.Add(ZELineSegment(PN2, PF2));
	LineSegments.Add(ZELineSegment(PN3, PF3));
	LineSegments.Add(ZELineSegment(PN4, PF4));

	LineSegments.Add(ZELineSegment(Position, PN2));
	LineSegments.Add(ZELineSegment(Position, PN3));
	LineSegments.Add(ZELineSegment(Position, PN4));
	LineSegments.Add(ZELineSegment(Position, PN1));

	return LineSegments;
}

void ZEViewFrustum::Create(const ZEVector3& Position, const ZEQuaternion& Rotation, float FOV, float AspectRatio, float NearZ, float FarZ)
{
	float SinFov = ZEAngle::Sin(FOV/2);
	float SinFovAT = SinFov * AspectRatio;
	float CosFov = ZEAngle::Cos(FOV/2);

	Look = Rotation * ZEVector3::UnitZ;
	Up = Rotation * ZEVector3::UnitY;
	Right = Rotation * ZEVector3::UnitX;

	this->NearZ = NearZ;
	this->FarZ = FarZ;
	this->Fov = FOV;
	this->AspectRatio = AspectRatio;

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

	float tanfov = ZEAngle::Tan(FOV / 2.0f);
	float tHeight = tanfov * NearZ * 2.0f;
	float tWidth = AspectRatio * tHeight;

	float radius = ZEMath::Sqrt(tWidth * tWidth + tHeight * tHeight) / 2.0f;
	TanRatio = radius / NearZ;
	SinRatio = radius / ZEMath::Sqrt(radius * radius + NearZ * NearZ);
}

// ZEViewSphere
ZEViewVolumeType ZEViewSphere::GetViewVolumeType() const
{
	return ZE_VVT_SPHERE;
}

bool ZEViewSphere::CullTest(const ZEBSphere& BoundingSphere) const
{
	return !ZEBSphere::IntersectionTest(Sphere, BoundingSphere);
}

bool ZEViewSphere::CullTest(const ZEAABBox& BoundingBox) const
{
	return !ZEBSphere::IntersectionTest(Sphere, BoundingBox);
}

bool ZEViewSphere::CullTest(const ZEOBBox& BoundingBox) const
{
	return !ZEBSphere::IntersectionTest(Sphere, BoundingBox);
}

bool ZEViewSphere::CullTest(ZEEntity* Entity) const
{
	return !ZEBSphere::IntersectionTest(Sphere, Entity->GetWorldBoundingBox());
}

bool ZEViewSphere::CullTest(ZELight* Light) const
{
	if (Light->GetLightType() == ZE_LT_DIRECTIONAL)
		return false;

	ZEBSphere BoundingSphere;
	BoundingSphere.Position = Light->GetWorldPosition();
	BoundingSphere.Radius = Light->GetRange();

	return CullTest(BoundingSphere);
}

ZEDoorViewTest ZEViewSphere::CullTest(const ZERectangle3D& PortalDoor) const
{
	zeAssert(true, "NOT IMPLAMENTED");
	return ZE_DVT_INSIDE;
}

void ZEViewSphere::Create(const ZEVector3& Position, float Radius, float NearZ)
{
	Sphere.Position = Position;
	Sphere.Radius = Radius;
	this->NearZ = NearZ;
}


// ZEViewHemiSphere
ZEViewVolumeType ZEViewHemiSphere::GetViewVolumeType() const
{
	return ZE_VVT_HEMISPHERE;
}

bool ZEViewHemiSphere::CullTest(const ZEBSphere& BoundingSphere) const
{
	if(ZEBSphere::IntersectionTest(BoundingSphere, HalfPlane) != ZE_HS_NEGATIVE_SIDE)
		return !ZEBSphere::IntersectionTest(Sphere, BoundingSphere);

	return true;
}

bool ZEViewHemiSphere::CullTest(const ZEAABBox& BoundingBox) const
{
	if(ZEAABBox::IntersectionTest(BoundingBox, HalfPlane) != ZE_HS_NEGATIVE_SIDE)
		return !ZEBSphere::IntersectionTest(Sphere, BoundingBox);

	return true;
}

bool ZEViewHemiSphere::CullTest(const ZEOBBox& BoundingBox) const
{
	if (ZEOBBox::IntersectionTest(BoundingBox, HalfPlane) != ZE_HS_NEGATIVE_SIDE)
		return !ZEBSphere::IntersectionTest(Sphere, BoundingBox);

	return true;
}

bool ZEViewHemiSphere::CullTest(ZEEntity* Entity) const
{
	return CullTest(Entity->GetWorldBoundingBox());
}

bool ZEViewHemiSphere::CullTest(ZELight* Light) const
{
	if (Light->GetLightType() == ZE_LT_DIRECTIONAL)
		return false;

	ZEBSphere BoundingSphere;
	BoundingSphere.Position = Light->GetWorldPosition();
	BoundingSphere.Radius = Light->GetRange();

	return CullTest(BoundingSphere);
}

ZEDoorViewTest ZEViewHemiSphere::CullTest(const ZERectangle3D& PortalDoor) const
{
	zeAssert(true, "NOT IMPLAMENTED");
	return ZE_DVT_INSIDE;
}

void ZEViewHemiSphere::Create(const ZEVector3& Position, const ZEVector3& Direction, float Radius, float NearZ)
{
	Sphere.Position = Position;
	Sphere.Radius = Radius;
	HalfPlane.p = Position;
	HalfPlane.n = Direction;
}

// ZEViewCuboid
ZEViewVolumeType ZEViewCuboid::GetViewVolumeType() const
{
	return ZE_VVT_CUBOID;
}


bool ZEViewCuboid::CullTest(const ZEBSphere& BoundingSphere) const
{
	return !ZEOBBox::IntersectionTest(Box, BoundingSphere);
}

bool ZEViewCuboid::CullTest(const ZEAABBox& BoundingBox) const
{
	return !ZEOBBox::IntersectionTest(Box, BoundingBox);

}

bool ZEViewCuboid::CullTest(const ZEOBBox& BoundingBox) const
{
	return !ZEOBBox::IntersectionTest(Box, BoundingBox);
}

bool ZEViewCuboid::CullTest(ZEEntity* Entity) const
{
	return CullTest(Entity->GetWorldBoundingBox());
}

bool ZEViewCuboid::CullTest(ZELight* Light) const
{
	if (Light->GetLightType() == ZE_LT_DIRECTIONAL)
		return false;

	ZEBSphere BoundingSphere;
	BoundingSphere.Position = Light->GetWorldPosition();
	BoundingSphere.Radius = Light->GetRange();

	return CullTest(BoundingSphere);
}

ZEDoorViewTest ZEViewCuboid::CullTest(const ZERectangle3D& PortalDoor) const
{
	zeAssert(true, "Not implamented");
	return ZE_DVT_INSIDE;
}

void ZEViewCuboid::Create(const ZEVector3& Position, const ZEQuaternion& Rotation, float Width, float Height, float NearZ, float FarZ)
{
	ZEQuaternion::VectorProduct(Box.Right, Rotation, ZEVector3::UnitX);
	ZEQuaternion::VectorProduct(Box.Up, Rotation, ZEVector3::UnitY);
	ZEQuaternion::VectorProduct(Box.Front, Rotation, ZEVector3::UnitZ);
	
	Box.HalfSize = ZEVector3(Width * 0.5f, Height * 0.5f, (FarZ - NearZ) * 0.5f);

	ZEVector3 Center;
	ZEQuaternion::VectorProduct(Center, Rotation, ZEVector3(0.0f, 0.0f, FarZ - NearZ));
	
	ZEVector3::Multiply(Box.Center, Position, Center);
}
