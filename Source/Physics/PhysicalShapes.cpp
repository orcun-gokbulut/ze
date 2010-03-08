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

// Physical Shape
/////////////////////////////////////////////////////////////////////////////////////////
void  ZEPhysicalShape::SetPosition(const ZEVector3& NewPosition)
{
	Position = NewPosition;
}

const ZEVector3&  ZEPhysicalShape::GetPosition()
{
	return Position;
}

void  ZEPhysicalShape::SetOrientation(const ZEVector3& NewOrientation)
{
	Rotation = NewRotation;
}

const ZEQuaternion&  ZEPhysicalShape::GetOrientation()
{
	return Rotation;
}

void  ZEPhysicalShape::SetMaterial(const ZEPhysicalMaterial& NewMaterial)
{
	Material = NewMaterial;
}

const ZEPhysicalMaterial&  ZEPhysicalShape::GetMaterial()
{
	return Material;
}

ZEPhysicalShape::ZEPhysicsShapeInfo()
{
	Material.DynamicFriction = 1.0f;
	Material.StaticFriction = 1.0f;
	Material.Restitioun = 1.0f;
}

ZEPhysicalShape::~ZEPhysicsShapeInfo()
{
}

// Physical Box Shape
/////////////////////////////////////////////////////////////////////////////////////////
virtual ZEPhysicalBoxShape::GetType()
{
	return ZE_PST_BOX;
}

void  ZEPhysicalBoxShape::SetWidth(float NewWidth)
{
	Width = NewWidth;
}

float  ZEPhysicalBoxShape::GetWidth()
{
	return Width;
}

void  ZEPhysicalBoxShape::SetHeight(float NewHeight)
{
	Height = NewHeight;
}

float  ZEPhysicalBoxShape::GetHeight()
{
	return Height;
}

void  ZEPhysicalBoxShape::SetLength(float NewLength)
{
	Lenght = NewLength;
}

float  ZEPhysicalBoxShape::GetLength()
{
	return Length;
}

// Physical Sphere Shape
/////////////////////////////////////////////////////////////////////////////////////////
void  ZEPhysicalSphereShape::SetRadious(float NewRadious)
{
	Radious = NewRadious;
}

float  ZEPhysicalSphereShape::GetRadious()
{
	return Radious;
}

// Physical Capsule Shape
/////////////////////////////////////////////////////////////////////////////////////////
void  ZEPhysicalCapsuleShape::SetRadious(float NewRadious)
{
	Radious = NewRadious;
}

float  ZEPhysicalCapsuleShape::GetRadious()
{
	return Radious;
}

void  ZEPhysicalCapsuleShape::SetHeight(float NewHeight)
{
	Height = NewHeight;
}

float  ZEPhysicalCapsuleShape::GetHeight()
{
	return Height;
}

// Physical Trimesh Shape
/////////////////////////////////////////////////////////////////////////////////////////
void  ZEPhysicalTrimeshShape::SetVertices(ZEArray<ZEVector3>& Vertices, ZEArray<unsigned int[3]> Indices)
{
	// UNDEFINED
}

// Physical Convex Shape
/////////////////////////////////////////////////////////////////////////////////////////
void  ZEPhysicalConvexShape::SetVertices(ZEArray<ZEVector3>& Vertices)
{
	// UNDEFINED
}

#endif
