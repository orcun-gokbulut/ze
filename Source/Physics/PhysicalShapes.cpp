//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - PhysicalShapes.cpp
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

#include "PhysicalShapes.h"

// Physical Shape
/////////////////////////////////////////////////////////////////////////////////////////
void ZEPhysicalShape::SetOwner(ZEPhysicalObject* Owner)
{
	this->Owner = Owner;
}

ZEPhysicalObject* ZEPhysicalShape::GetOwner()
{
	return Owner;
}

void ZEPhysicalShape::SetPosition(const ZEVector3& NewPosition)
{
	Position = NewPosition;
}

const ZEVector3& ZEPhysicalShape::GetPosition()
{
	return Position;
}

void ZEPhysicalShape::SetRotation(const ZEQuaternion& NewRotation)
{
	Rotation = NewRotation;
}

const ZEQuaternion& ZEPhysicalShape::GetRotation()
{
	return Rotation;
}

void ZEPhysicalShape::SetMaterial(const ZEPhysicalMaterial& NewMaterial)
{
	Material = NewMaterial;
}

const ZEPhysicalMaterial&  ZEPhysicalShape::GetMaterial()
{
	return Material;
}

ZEPhysicalShape::ZEPhysicalShape()
{
	Position = ZEVector3::UnitX;
	Rotation = ZEQuaternion::Identity;
	Material.DynamicFriction = 1.0f;
	Material.StaticFriction = 1.0f;
	Material.Restitution = 1.0f;
}

// Physical Box Shape
/////////////////////////////////////////////////////////////////////////////////////////
ZEPhysicalShapeType ZEPhysicalBoxShape::GetPhysicalShapeType()
{
	return ZE_PST_BOX;
}

void ZEPhysicalBoxShape::SetWidth(float NewWidth)
{
	Width = NewWidth;
}

float ZEPhysicalBoxShape::GetWidth()
{
	return Width;
}

void ZEPhysicalBoxShape::SetHeight(float NewHeight)
{
	Height = NewHeight;
}

float ZEPhysicalBoxShape::GetHeight()
{
	return Height;
}

void ZEPhysicalBoxShape::SetLength(float NewLength)
{
	Length = NewLength;
}

float ZEPhysicalBoxShape::GetLength()
{
	return Length;
}

ZEPhysicalBoxShape::ZEPhysicalBoxShape()
{
	Width = 1.0f;
	Height = 1.0f;
	Length = 1.0f;
}


// Physical Sphere Shape
/////////////////////////////////////////////////////////////////////////////////////////
ZEPhysicalShapeType ZEPhysicalSphereShape::GetPhysicalShapeType()
{
	return ZE_PST_SPHERE;
}

void ZEPhysicalSphereShape::SetRadius(float NewRadius)
{
	Radius = NewRadius;
}

float ZEPhysicalSphereShape::GetRadius()
{
	return Radius;
}

ZEPhysicalSphereShape::ZEPhysicalSphereShape()
{
	Radius = 1.0f;
}


// Physical Capsule Shape
/////////////////////////////////////////////////////////////////////////////////////////
ZEPhysicalShapeType ZEPhysicalCapsuleShape::GetPhysicalShapeType()
{
	return ZE_PST_CAPSULE;
}

void  ZEPhysicalCapsuleShape::SetRadius(float NewRadius)
{
	Radius = NewRadius;
}

float  ZEPhysicalCapsuleShape::GetRadius()
{
	return Radius;
}

void  ZEPhysicalCapsuleShape::SetHeight(float NewHeight)
{
	Height = NewHeight;
}

float  ZEPhysicalCapsuleShape::GetHeight()
{
	return Height;
}

ZEPhysicalCapsuleShape::ZEPhysicalCapsuleShape()
{
	Radius = 1.0f;
	Height = 1.0f;
}


// Physical Trimesh Shape
/////////////////////////////////////////////////////////////////////////////////////////
ZEPhysicalShapeType ZEPhysicalCylinderShape::GetPhysicalShapeType()
{
	return ZE_PST_CYLINDER;
}

void ZEPhysicalCylinderShape::SetRadius(float NewRadius)
{
	Radius = NewRadius;
}

float ZEPhysicalCylinderShape::GetRadius()
{
	return Radius;
}


void ZEPhysicalCylinderShape::SetHeight(float NewHeight)
{
	Height = NewHeight;
}

float ZEPhysicalCylinderShape::GetHeight()
{
	return Height;
}

 ZEPhysicalCylinderShape::ZEPhysicalCylinderShape()
 {
	Radius = 1.0f;
	Height = 1.0f;
 }


// Physical Trimesh Shape
/////////////////////////////////////////////////////////////////////////////////////////
ZEPhysicalShapeType ZEPhysicalTrimeshShape::GetPhysicalShapeType()
{
	return ZE_PST_TRIMESH;
}

void ZEPhysicalTrimeshShape::SetMaterialsPerTriangle(bool Enabled)
{
	MaterialsPerTriangle = Enabled;
}

bool ZEPhysicalTrimeshShape::GetMaterialsPerTriangle()
{
	return MaterialsPerTriangle;
}

void  ZEPhysicalTrimeshShape::SetTriangleVertices(const ZEArray<ZEVector3>& Vertices)
{
	TriangleVertices = Vertices;
}

const ZEArray<ZEVector3>& ZEPhysicalTrimeshShape::GetTriangleVertices()
{
	return TriangleVertices;
}

void ZEPhysicalTrimeshShape::SetTriangles(const ZEArray<ZEPhysicalTriangle>& Triangles)
{
	this->Triangles = Triangles;
}

const ZEArray<ZEPhysicalTriangle>& ZEPhysicalTrimeshShape::GetTriangles()
{
	return Triangles;
}

void ZEPhysicalTrimeshShape::SetTriangleMaterials(const ZEArray<ZEPhysicalMaterial>& Materials)
{
	TriangleMaterials = Materials;
}

const ZEArray<ZEPhysicalMaterial>& ZEPhysicalTrimeshShape::GetTriangleMaterials()
{
	return TriangleMaterials;
}

ZEPhysicalTrimeshShape::ZEPhysicalTrimeshShape()
{
	MaterialsPerTriangle = false;
}


// Physical Convex Shape
/////////////////////////////////////////////////////////////////////////////////////////
ZEPhysicalShapeType ZEPhysicalConvexShape::GetPhysicalShapeType()
{
	return ZE_PST_CONVEX;
}

void ZEPhysicalConvexShape::SetConvexVertices(const ZEArray<ZEVector3>& Vertices)
{
	ConvexVertices= Vertices;
}

const ZEArray<ZEVector3>& ZEPhysicalConvexShape::GetConvexVertices()
{
	return ConvexVertices;
}
