//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEViewFrustum.cpp
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

#include "ZEViewFrustum.h"
#include "ZEGame/ZEComponent.h"
#include "ZEError.h"
#include "ZEGame/ZEEntity.h"
#include "ZEGraphics/ZELight.h"
#include "ZEMath.h"
#include "ZEAngle.h"

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
		(ZEBSphere::IntersectionTest(BoundingSphere, RightClippingPlane)	== ZE_HS_NEGATIVE_SIDE) ||
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

ZEVector3 ZEViewFrustum::GetPosition() const
{
	return Position;
}


void ZEViewFrustum::SetPosition( ZEVector3 Value )
{
	Position = Value;
}

const ZEPlane& ZEViewFrustum::GetClippingPlane(ZEViewFrustumPlane Plane) const
{
	switch(Plane)
	{
		case ZE_VFP_RIGHT:
			return RightClippingPlane;
		case ZE_VFP_LEFT:
			return LeftClippingPlane;
		case ZE_VFP_TOP:
			return TopClippingPlane;
		case ZE_VFP_BOTTOM:
			return BottomClippingPlane;
		case ZE_VFP_FAR:
			return FarClippingPlane;
		case ZE_VFP_NEAR:
			return NearClippingPlane;
		default:
			return LeftClippingPlane;
	}
}

const ZELineSegment& ZEViewFrustum::GetEdge(ZEViewFrustumEdge Edge) const
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
		case ZE_VFE_FARRIGHT:
			P1 = FarPlaneCenter + (Up * FarPlaneHeight) + (Right * FarPlaneWidth);
			P2 = FarPlaneCenter - (Up * FarPlaneHeight) + (Right * FarPlaneWidth);
			return ZELineSegment(P1, P2);
		case ZE_VFE_FARBOTTOM:
			P1 = FarPlaneCenter - (Up * FarPlaneHeight) + (Right * FarPlaneWidth);
			P2 = FarPlaneCenter - (Up * FarPlaneHeight) - (Right * FarPlaneWidth);
			return ZELineSegment(P1, P2);
		case ZE_VFE_FARLEFT:
			P1 = FarPlaneCenter - (Up * FarPlaneHeight) - (Right * FarPlaneWidth);
			P2 = FarPlaneCenter + (Up * FarPlaneHeight) - (Right * FarPlaneWidth);
			return ZELineSegment(P1, P2);
		case ZE_VFE_FARTOP:
			P1 = FarPlaneCenter + (Up * FarPlaneHeight) - (Right * FarPlaneWidth);
			P2 = FarPlaneCenter + (Up * FarPlaneHeight) + (Right * FarPlaneWidth);
			return ZELineSegment(P1, P2);
		case ZE_VFE_RIGHTUP:
			P1 = NearPlaneCenter + (Up * NearPlaneHeight) + (Right * NearPlaneWidth);
			P2 = FarPlaneCenter + (Up * FarPlaneHeight) + (Right * FarPlaneWidth);
			return ZELineSegment(P1, P2);
		case ZE_VFE_RIGHTDOWN:
			P1 = NearPlaneCenter - (Up * NearPlaneHeight) + (Right * NearPlaneWidth);
			P2 = FarPlaneCenter - (Up * FarPlaneHeight) + (Right * FarPlaneWidth);
			return ZELineSegment(P1, P2);
		case ZE_VFE_LEFTDOWN:
			P1 = NearPlaneCenter - (Up * NearPlaneHeight) - (Right * NearPlaneWidth);
			P2 = FarPlaneCenter - (Up * FarPlaneHeight) - (Right * FarPlaneWidth);
			return ZELineSegment(P1, P2);
		case ZE_VFE_LEFTUP:
			P1 = NearPlaneCenter + (Up * NearPlaneHeight) - (Right * NearPlaneWidth);
			P2 = FarPlaneCenter + (Up * FarPlaneHeight) - (Right * FarPlaneWidth);
			return ZELineSegment(P1, P2);
		case ZE_VFE_NEARRIGHT:
			P1 = NearPlaneCenter + (Up * NearPlaneHeight) + (Right * NearPlaneWidth);
			P2 = NearPlaneCenter - (Up * NearPlaneHeight) + (Right * NearPlaneWidth);
			return ZELineSegment(P1, P2);
		case ZE_VFE_NEARBOTTOM:
			P1 = NearPlaneCenter - (Up * NearPlaneHeight) + (Right * NearPlaneWidth);
			P2 = NearPlaneCenter - (Up * NearPlaneHeight) - (Right * NearPlaneWidth);
			return ZELineSegment(P1, P2);
		case ZE_VFE_NEARLEFT:
			P1 = NearPlaneCenter - (Up * NearPlaneHeight) - (Right * NearPlaneWidth);
			P2 = NearPlaneCenter + (Up * NearPlaneHeight) - (Right * NearPlaneWidth);
			return ZELineSegment(P1, P2);
		case ZE_VFE_NEARTOP:
			P1 = NearPlaneCenter + (Up * NearPlaneHeight) - (Right * NearPlaneWidth);
			P2 = NearPlaneCenter + (Up * NearPlaneHeight) + (Right * NearPlaneWidth);
			return ZELineSegment(P1, P2);
		default:
			P1 = FarPlaneCenter + (Up * FarPlaneHeight) + (Right * FarPlaneWidth);
			P2 = FarPlaneCenter - (Up * FarPlaneHeight) + (Right * FarPlaneWidth);
			return ZELineSegment(P1, P2);
	}
}

const ZEVector3& ZEViewFrustum::GetVertex(ZEViewFrustumVertex Vertex) const
{
	float Fov_2 = FOV / 2;

	ZEVector3 NearPlaneCenter = Position + (Look * NearZ);
	ZEVector3 FarPlaneCenter = Position + (Look * FarZ);

	float NearPlaneHeight = NearZ * tan(Fov_2);
	float FarPlaneHeight = FarZ * tan(Fov_2);

	float NearPlaneWidth = NearPlaneHeight * AspectRatio;
	float FarPlaneWidth = FarPlaneHeight * AspectRatio;

	switch(Vertex)
	{
		case ZE_VFV_FARRIGHTUP:
			return (FarPlaneCenter + (Up * FarPlaneHeight) + (Right * FarPlaneWidth));
		case ZE_VFV_FARRIGHTDOWN:
			return (FarPlaneCenter - (Up * FarPlaneHeight) + (Right * FarPlaneWidth));
		case ZE_VFV_FARLEFTDOWN:
			return (FarPlaneCenter - (Up * FarPlaneHeight) - (Right * FarPlaneWidth));
		case ZE_VFV_FARLEFTUP:
			return (FarPlaneCenter + (Up * FarPlaneHeight) - (Right * FarPlaneWidth));
		case ZE_VFV_NEARRIGHTUP:
			return (NearPlaneCenter + (Up * NearPlaneHeight) + (Right * NearPlaneWidth));
		case ZE_VFV_NEARRIGHTDOWN:
			return (NearPlaneCenter - (Up * NearPlaneHeight) + (Right * NearPlaneWidth));
		case ZE_VFV_NEARLEFTDOWN:
			return (NearPlaneCenter - (Up * NearPlaneHeight) - (Right * NearPlaneWidth));
		case ZE_VFV_NEARLEFTUP:
			return (NearPlaneCenter + (Up * NearPlaneHeight) + (Right * NearPlaneWidth));
		default:
			return ZEVector3::Zero;
	}
}

float ZEViewFrustum::GetAspectRatio() const
{
	return AspectRatio;
}

float ZEViewFrustum::GetFOV() const
{
	return FOV;
}

float ZEViewFrustum::GetNearZ() const
{
	return NearZ;
}

float ZEViewFrustum::GetFarZ() const
{
	return FarZ;
}

const ZEVector3& ZEViewFrustum::GetDirection() const
{
	return Look;
}

const ZEVector3& ZEViewFrustum::GetRight() const
{
	return Right;
}

const ZEVector3& ZEViewFrustum::GetUp() const
{
	return Up;
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

bool ZEViewFrustum::CullTest(const ZERectangle3D& PortalDoor) const
{
	if ((ZERectangle3D::IntersectionTest(PortalDoor, NearClippingPlane)		== ZE_HS_NEGATIVE_SIDE) ||
		(ZERectangle3D::IntersectionTest(PortalDoor, LeftClippingPlane)		== ZE_HS_NEGATIVE_SIDE) ||
		(ZERectangle3D::IntersectionTest(PortalDoor, RightClippingPlane)	== ZE_HS_NEGATIVE_SIDE) ||
		(ZERectangle3D::IntersectionTest(PortalDoor, TopClippingPlane)		== ZE_HS_NEGATIVE_SIDE) ||
		(ZERectangle3D::IntersectionTest(PortalDoor, FarClippingPlane)		== ZE_HS_NEGATIVE_SIDE) ||
		(ZERectangle3D::IntersectionTest(PortalDoor, BottomClippingPlane)	== ZE_HS_NEGATIVE_SIDE))
		return true;

	return false;
}

ZEArray<ZELineSegment> ZEViewFrustum::GetFrustumLineSegments()
{
	ZEArray<ZELineSegment> LineSegments;
	float Fov_2 = FOV / 2;

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
	float SinFov = sinf(FOV/2);
	float SinFovAT = SinFov * AspectRatio;
	float CosFov = cosf(FOV/2);

	Look = Rotation * ZEVector3::UnitZ;
	Up = Rotation * ZEVector3::UnitY;
	Right = Rotation * ZEVector3::UnitX;

	this->NearZ = NearZ;
	this->FarZ = FarZ;
	this->FOV = FOV;
	this->AspectRatio = AspectRatio;

	ZEQuaternion::VectorProduct(TopClippingPlane.n, Rotation, ZEVector3(0.0f, -CosFov, SinFov));
	ZEQuaternion::VectorProduct(BottomClippingPlane.n, Rotation, ZEVector3(0.0f, CosFov, SinFov));

	ZEQuaternion::VectorProduct(RightClippingPlane.n, Rotation, ZEVector3(-CosFov, 0.0f, SinFovAT));
	RightClippingPlane.n.NormalizeSelf();
	ZEQuaternion::VectorProduct(LeftClippingPlane.n, Rotation, ZEVector3(CosFov, 0.0f, SinFovAT));
	LeftClippingPlane.n.NormalizeSelf();

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

void ZEViewFrustum::Create( const ZEPlane& RightClippingPlane, const ZEPlane& BottomClippingPlane, const ZEPlane& LeftClippingPlane, const ZEPlane& TopClippingPlane, const ZEPlane& FarClippingPlane, const ZEPlane& NearClippingPlane )
{
	this->RightClippingPlane = RightClippingPlane;
	this->BottomClippingPlane = BottomClippingPlane;
	this->LeftClippingPlane = LeftClippingPlane;
	this->TopClippingPlane = TopClippingPlane;
	this->FarClippingPlane = FarClippingPlane;
	this->NearClippingPlane = NearClippingPlane;

// 	//FOV Calculation
// 
// 	float TempDotProduct = ZEVector3::DotProduct(this->RightClippingPlane.n, this->LeftClippingPlane.n);
// 	float TempAngle = acos(TempDotProduct);
// 
// 	if (TempAngle < 0.0f)
// 		TempAngle = (ZE_PI * 2.0f) + TempAngle;
// 
// 	FOV = ZEMath::Abs(ZE_PI - TempAngle);
// 
// 	//Position Calculation
// 
// 	ZELine TempLine1;
// 	float TempT1;
// 	ZEPlane::IntersectionTest(this->TopClippingPlane, this->RightClippingPlane, TempLine1);
// 
// 	ZELine TempLine2;
// 	float TempT2;
// 	ZEPlane::IntersectionTest(this->BottomClippingPlane, this->LeftClippingPlane, TempLine2);
// 
// 	ZELine::MinimumDistance(TempLine1, TempLine2, TempT1, TempT2);
// 
// 	if(ZEMath::Abs(TempT1 - TempT2) <= 0.00000f)
// 		Position = TempLine1.GetPointOn(TempT1);
// 	else
// 		return; // Should give assert

}

ZEArray<ZELineSegment> ZEViewFrustum::GetCustomFrustumLineSegments()
{
	ZEArray<ZELineSegment> LineSegments;

	ZEVector3 NearRU, FarRU, NearRD, FarRD, NearLD, FarLD, NearLU, FarLU;
	
	// Near Lines

	ZELine NearRight;
	float NearRightTT = 0.0f;
	float NearRightTB = 0.0f;
	ZEPlane::IntersectionTest(RightClippingPlane, NearClippingPlane, NearRight);

	ZEPlane::IntersectionTest(TopClippingPlane, NearRight, NearRightTT);
	NearRU = NearRight.GetPointOn(NearRightTT);

	ZEPlane::IntersectionTest(BottomClippingPlane, NearRight, NearRightTB);
	NearRD = NearRight.GetPointOn(NearRightTB);

// 	ZELine NearTop;
// 	float NearTopTR = 0.0f;
// 	float NearTopTL = 0.0f;
// 	ZEPlane::IntersectionTest(TopClippingPlane, NearClippingPlane, NearTop);

	ZELine NearLeft;
	float NearLeftTT = 0.0f;
	float NearLeftTB = 0.0f;
	ZEPlane::IntersectionTest(LeftClippingPlane, NearClippingPlane, NearLeft);

	ZEPlane::IntersectionTest(TopClippingPlane, NearLeft, NearLeftTT);
	NearLU = NearLeft.GetPointOn(NearLeftTT);

	ZEPlane::IntersectionTest(BottomClippingPlane, NearLeft, NearLeftTB);
	NearLD = NearLeft.GetPointOn(NearLeftTB);

// 	ZELine NearBottom;
// 	float NearLeftTR = 0.0f;
// 	float NearLeftTL = 0.0f;
// 	ZEPlane::IntersectionTest(BottomClippingPlane, NearClippingPlane, NearBottom);

	// Far Lines

	ZELine FarRight;
	float FarRightTT = 0.0f;
	float FarRightTB = 0.0f;
	ZEPlane::IntersectionTest(RightClippingPlane, FarClippingPlane, FarRight);

	ZEPlane::IntersectionTest(TopClippingPlane, FarRight, FarRightTT);
	FarRU = FarRight.GetPointOn(FarRightTT);

	ZEPlane::IntersectionTest(BottomClippingPlane, FarRight, FarRightTB);
	FarRD = FarRight.GetPointOn(FarRightTB);

// 	ZELine FarTop;
// 	float FarTopTR = 0.0f;
// 	float FarTopTL = 0.0f;
// 	ZEPlane::IntersectionTest(TopClippingPlane, FarClippingPlane, FarTop);

	ZELine FarLeft;
	float FarLeftTT = 0.0f;
	float FarLeftTB = 0.0f;
	ZEPlane::IntersectionTest(LeftClippingPlane, FarClippingPlane, FarLeft);

	ZEPlane::IntersectionTest(TopClippingPlane, FarLeft, FarLeftTT);
	FarLU = FarLeft.GetPointOn(FarLeftTT);

	ZEPlane::IntersectionTest(BottomClippingPlane, FarLeft, FarLeftTB);
	FarLD = FarLeft.GetPointOn(FarLeftTB);

// 	ZELine FarBottom;
// 	float FarBottomTR = 0.0f;
// 	float FarBottomTL = 0.0f;
// 	ZEPlane::IntersectionTest(BottomClippingPlane, FarClippingPlane, FarBottom);

	// Remaining Lines

// 	ZELine TopRight;
// 	float TopRightTF = 0.0f;
// 	float TopRightTN = 0.0f;
// 	ZEPlane::IntersectionTest(RightClippingPlane, TopClippingPlane, TopRight);
// 
// 	ZELine TopLeft;
// 	float TopLeftTF = 0.0f;
// 	float TopLeftTN = 0.0f;
// 	ZEPlane::IntersectionTest(LeftClippingPlane, TopClippingPlane, TopLeft);
// 
// 	ZELine BottomRight;
// 	float BottomRightTF = 0.0f;
// 	float BottomRightTN = 0.0f;
// 	ZEPlane::IntersectionTest(RightClippingPlane, BottomClippingPlane, BottomRight);
// 
// 	ZELine BottomLeft;
// 	float BottomLeftTF = 0.0f;
// 	float BottomLeftTN = 0.0f;
// 	ZEPlane::IntersectionTest(LeftClippingPlane, BottomClippingPlane, BottomLeft);


	//Near Rectangle
	LineSegments.Add(ZELineSegment(NearRU, NearRD));
	LineSegments.Add(ZELineSegment(NearLU, NearRU));
	LineSegments.Add(ZELineSegment(NearLD, NearLU));
	LineSegments.Add(ZELineSegment(NearRD, NearLD));

	//Far Rectangle
	LineSegments.Add(ZELineSegment(FarRU, FarRD));
	LineSegments.Add(ZELineSegment(FarLU, FarRU));
	LineSegments.Add(ZELineSegment(FarLD, FarLU));
	LineSegments.Add(ZELineSegment(FarRD, FarLD));

	//Other LineSegments
	LineSegments.Add(ZELineSegment(NearRU, FarRU));
	LineSegments.Add(ZELineSegment(NearRD, FarRD));
	LineSegments.Add(ZELineSegment(NearLD, FarLD));
	LineSegments.Add(ZELineSegment(NearLU, FarLU));

	return LineSegments;
}
