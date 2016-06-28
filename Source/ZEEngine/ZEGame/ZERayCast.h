//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERayCast.h
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
#ifndef __ZE_RAY_CAST_H__
#define __ZE_RAY_CAST_H__

#include "ZETypes.h"
#include "ZEDS/ZEFlags.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEDelegate.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZERay.h"
#include "ZEMath/ZETriangle.h"
#include "ZEMath/ZEAABBox.h"

class ZEObject;

typedef ZEFlags ZERayCastReportComponents;
#define ZE_RCRE_ALL					0xFFFFFFFF
#define ZE_RCRE_NONE				0x0
#define ZE_RCRE_MULTI_COLLISIONS	0x1
#define ZE_RCRE_BOUNDING_BOX_ENTER	0x2
#define ZE_RCRE_BOUNDING_BOX_EXIT	0x4
#define ZE_RCRE_POLYGONS			0x8

enum ZERayCastCollisionType
{
	ZE_RCCT_NONE = 0,
	ZE_RCCT_BOUNDING_BOX,
	ZE_RCCT_BOUNDING_BOX_ENTER = ZE_RCCT_BOUNDING_BOX,
	ZE_RCCT_BOUNDING_BOX_EXIT,
	ZE_RCCT_POLGON,
};

enum ZERayCastMatch
{
	ZE_RCM_NONE = 0,
	ZE_RCM_FIRST,
	ZE_RCM_NEAREST,
	ZE_RCM_FURTHEST
};

class ZERayCastCollision;

typedef ZEDelegate<bool (ZEObject*, void*)> ZERayCastFilterFunction;
typedef ZEDelegate<bool (ZERayCastCollision&, const void*)> ZERayCastModifierFunction;

class ZERayCastParameters
{
	public:
		ZERay								Ray;
		ZERayCastReportComponents			Components;
		ZERayCastMatch						Match;

		ZEClass*							FilterClass;
		ZEArray<ZEClass*>					FilterIncludedClasses;
		ZEArray<ZEClass*>					FilterExludedClasses;

		ZEArray<ZEObject*>					FilterIncludedObjects;
		ZEArray<ZEObject*>					FilterExcludedObjects;

		ZERayCastFilterFunction				FilterFunction;
		void*								FilterFunctionParameter;

		float								MinimumDistance;
		float								MaximumDistance;

		void*								ExtraParameters;

		bool								Filter(ZEObject* Object) const;

											ZERayCastParameters();
};

class ZERayCastCollision
{
	public:
		ZERayCastCollisionType				Type;

		ZEObject*							Object;
		ZEObject*							SubObject;

		float								Distance;
		ZEVector3							Position;
		ZETriangle							Polygon;

		const void*							CustomValue;

											ZERayCastCollision();
};

class ZERayCastReport
{
	private:
		const ZERayCastParameters*			Parameters;
		ZERayCastCollision					Collision;
		ZEArray<ZERayCastCollision>			Collisions;
		float								MinimumDistance;
		float								MaximumDistance;

		const void*							ModifierParameter;
		ZERayCastModifierFunction			ModifierFunction;

	public:
		bool								GetResult() const;
		const ZERayCastCollision&			GetCollision() const;
		const ZEArray<ZERayCastCollision>	GetCollisions() const;

		float								GetMinimumDistance();
		float								GetMaximumDistance();

		void								SetParameters(const ZERayCastParameters* Parameters);
		const ZERayCastParameters*			GetParameters() const;

		void								SetModifierFunction(const ZERayCastModifierFunction& Function);
		const ZERayCastModifierFunction&	GetModifierFunction();

		void								SetModifierParameter(const void* Parameter);
		const void*							GetModifierParameter();

		virtual void						AddCollision(const ZERayCastCollision& Collision);

		bool								CheckDistance(float Distance) const;
		bool								CheckDone() const;

											ZERayCastReport();
};


class ZERayCastHelper
{
	private:
		mutable bool						Dirty;
		mutable ZERay						LocalRay;
		const ZEMatrix4x4*					WorldTransform;
		const ZEMatrix4x4*					InvWorldTransform;

		ZEObject*							Object;
		ZEObject*							SubObject;
		ZERayCastReport*					Report;
		const void*							CustomValue;

		void								InitializeCollision(ZERayCastCollision& Collision, float RayT) const;

		void								Update()  const;

	public:
		void								SetReport(ZERayCastReport* Parameters);
		ZERayCastReport*					GetReport() const;

		void								SetWorldTransform(const ZEMatrix4x4* WorldTransform);
		const ZEMatrix4x4*					GetWorldTransform() const;

		void								SetInvWorldTransform(const ZEMatrix4x4* InvWorldTransform);
		const ZEMatrix4x4*					GetInvWorldTransform() const;

		void								SetObject(ZEObject* Object);
		ZEObject*							GetObject();

		void								SetSubObject(ZEObject* Object);
		ZEObject*							SetSubObject();

		void								SetCustomValue(const void* Value);
		const void*							GetCustomValue();

		const ZERay&						GetLocalRay() const;

		bool								TestBoundingBox(const ZEAABBox& WorldBoundingBox) const;

		bool								RayCastPolygon(const ZEVector3& Vertex0, const ZEVector3& Vertex1, const ZEVector3& Vertex3) const;

		bool								RayCastBoundingBox(const ZEAABBox& WorldBoundingBox) const;
		bool								RayCastBoundingBox(const ZEAABBox& WorldBoundingBox, const ZEAABBox& LocalBoundingBox) const;

		bool								RayCastMesh(const void* VertexBuffer, ZESize VertexCount, ZESize VertexStride) const;
		bool								RayCastMesh(const void* PolygonBuffer, ZESize PolgonCount, ZESize PolygonStride, 
														ZESize Vertex0Offset, ZESize Vertex1Offset, ZESize Vertex2Offset) const;

											ZERayCastHelper();
};

#endif
