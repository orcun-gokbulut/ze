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
#ifndef	__ZE_PHYSICS_SHAPE_INFO_H__
#define __ZE_PHYSICS_SHAPE_INFO_H__

class ZEPhysicsMaterial;
class ZEVector3;
#include "Physics/PhysicsCollisionMask.h"

enum  ZEPhysicalShapeType
{
	ZE_PBST_PLANE,
	ZE_PBST_BOX,
	ZE_PBST_SPHERE,
	ZE_PBST_CAPSULE,
	ZE_PBST_CONVEX,
	ZE_PBST_TRIMESH
};

struct ZEPhysicalMaterial
{
	float		StaticFriction;
	float		DynamicFriction;
	float		Restitution;
};

class  ZEPhysicalShape
{
	private:
		ZEVector3					Positon;
		ZEQuaternion				Orientation;
		bool						Trigger;
		 ZEPhysicalShapeMaterial	Material;

	public:
		virtual ZEPhysicsShapeType	GetType() = 0;

		void						SetPosition(const ZEVector3& NewPosition);
		const ZEVector3&			GetPosition();

		void						SetRotation(const ZEVector3& NewOrientation);
		const ZEQuaternion&			GetRotation();

		void						SetMaterial(const ZEPhysicalMaterial& NewMaterial);
		const ZEPhysicalMaterial&	GetMaterial();

									ZEPhysicsShapeInfo();
		virtual						~ZEPhysicsShapeInfo();
};

class  ZEPhysicalBoxShape : public  ZEPhysicalShape
{
	private:
		float						Width;
		float						Height;
		float						Length;

	public:
		virtual ZEPhysicsShapeType	GetType();

		void						SetWidth(float NewWidth);
		float						GetLength();

		void						SetHeight(float NewHeight);
		float						GetHeight();

		void						SetLength(float NewLength);
		float						GetLength();
};

class  ZEPhysicalSphereShape : public  ZEPhysicalSShape
{
	private:
		float						Radious;

	public:
		virtual ZEPhysicsShapeType	GetType();

		void						SetRadious(float NewRadious);
		float						GetRadious();
};

class  ZEPhysicalCapsuleShape : public  ZEPhysicalShape
{
	private:
		float						Radious1;
		float						Height;

	public:
		virtual ZEPhysicsShapeType	GetType();

		void						SetRadious(float NewRadious);
		float						GetRadious();

		void						SetHeight(float NewHeight);
		float						GetHeight();
};

class  ZEPhysicalTrimeshShape : public  ZEPhysicalShape
{
	public:
		virtual ZEPhysicsShapeType	GetType();

		void						SetVertices(ZEArray<ZEVector3>& Vertices, ZEArray<unsigned int[3]> Indices);
	
};

class  ZEPhysicalConvexShape : public  ZEPhysicalShape
{
	public:	
		virtual ZEPhysicsShapeType	GetType();

		void						SetVertices(ZEArray<ZEVector3>& Vertices);	
};
#endif
