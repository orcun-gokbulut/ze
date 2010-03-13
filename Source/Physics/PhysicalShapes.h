//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - PhysicalShapes.h
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
#ifndef	__ZE_PHYSICAL_SHAPES_H__
#define __ZE_PHYSICAL_SHAPES_H__

#include "ZEDS/Array.h"
#include "ZEMath/Vector.h"
#include "ZEMath/Quaternion.h"

enum  ZEPhysicalShapeType
{
	ZE_PST_PLANE,
	ZE_PST_BOX,
	ZE_PST_SPHERE,
	ZE_PST_CYLINDER,
	ZE_PST_CAPSULE,
	ZE_PST_CONVEX,
	ZE_PST_TRIMESH
};

struct ZEPhysicalTriangle
{
	unsigned int Indices[3];
	unsigned int MaterialIndex;
};

struct ZEPhysicalMaterial
{
	float		StaticFriction;
	float		DynamicFriction;
	float		Restitution;
};

class ZEPhysicalObject;

class ZEPhysicalShape
{
	private:
		ZEPhysicalObject*				Owner;
		ZEVector3						Position;
		ZEQuaternion					Rotation;
		bool							Trigger;
		ZEPhysicalMaterial				Material;

	public:
		virtual ZEPhysicalShapeType		GetPhysicalShapeType() = 0;

		void							SetOwner(ZEPhysicalObject* Owner);
		ZEPhysicalObject*				GetOwner();

		void							SetPosition(const ZEVector3& NewPosition);
		const ZEVector3&				GetPosition();

		void							SetRotation(const ZEQuaternion& NewRotation);
		const ZEQuaternion&				GetRotation();

		void							SetMaterial(const ZEPhysicalMaterial& NewMaterial);
		const ZEPhysicalMaterial&		GetMaterial();

										ZEPhysicalShape();
};

class  ZEPhysicalBoxShape : public  ZEPhysicalShape
{
	private:
		float							Width;
		float							Height;
		float							Length;

	public:
		virtual ZEPhysicalShapeType		GetPhysicalShapeType();

		void							SetWidth(float NewWidth);
		float							GetWidth();

		void							SetHeight(float NewHeight);
		float							GetHeight();

		void							SetLength(float NewLength);
		float							GetLength();

										ZEPhysicalBoxShape();
};

class  ZEPhysicalSphereShape : public  ZEPhysicalShape
{
	private:
		float							Radius;

	public:
		virtual ZEPhysicalShapeType		GetPhysicalShapeType();

		void							SetRadius(float NewRadius);
		float							GetRadius();

										ZEPhysicalSphereShape();
};

class  ZEPhysicalCapsuleShape : public  ZEPhysicalShape
{
	private:
		float							Radius;
		float							Height;

	public:
		virtual ZEPhysicalShapeType		GetPhysicalShapeType();

		void							SetRadius(float NewRadius);
		float							GetRadius();

		void							SetHeight(float NewHeight);
		float							GetHeight();

										ZEPhysicalCapsuleShape();
};

class ZEPhysicalCylinderShape
{
	private:
		float							Radius;
		float							Height;

	public:
		virtual ZEPhysicalShapeType		GetPhysicalShapeType();

		void							SetRadius(float NewRadius);
		float							GetRadius();

		void							SetHeight(float NewHeight);
		float							GetHeight();

										ZEPhysicalCylinderShape();
};

class  ZEPhysicalTrimeshShape : public  ZEPhysicalShape
{
	private:
		ZEArray<ZEVector3>				TriangleVertices;
		ZEArray<ZEPhysicalTriangle>		Triangles;
		ZEArray<ZEPhysicalMaterial>		TriangleMaterials;
		bool							MaterialsPerTriangle;

	public:
		virtual ZEPhysicalShapeType		GetPhysicalShapeType();

		void							SetMaterialsPerTriangle(bool Enabled);
		bool							GetMaterialsPerTriangle();

		void							SetTriangleVertices(const ZEArray<ZEVector3>& Vertices);
		const ZEArray<ZEVector3>&		GetTriangleVertices();

		void							SetTriangles(const ZEArray<ZEPhysicalTriangle>& Triangles);
		const ZEArray<ZEPhysicalTriangle>& GetTriangles();

		void							SetTriangleMaterials(const ZEArray<ZEPhysicalMaterial>& Materials);
		const ZEArray<ZEPhysicalMaterial>& GetTriangleMaterials();

										ZEPhysicalTrimeshShape();
};

class  ZEPhysicalConvexShape : public  ZEPhysicalShape
{
	private:
		ZEArray<ZEVector3>				ConvexVertices;

	public:	
		virtual ZEPhysicalShapeType		GetPhysicalShapeType();

		void							SetConvexVertices(const ZEArray<ZEVector3>& Vertices);	
		const ZEArray<ZEVector3>&		GetConvexVertices();
};
#endif
