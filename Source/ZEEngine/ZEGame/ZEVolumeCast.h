//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEVolumeCast.h
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

#include "ZEMeta/ZEObject.h"

#include "ZETypes.h"
#include "ZEDS/ZEFlags.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEDelegate.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZERay.h"
#include "ZEMath/ZETriangle.h"
#include "ZEMath/ZEAABBox.h"
#include "ZEMeta/ZEEnumerator.h"

class ZEObject;

typedef ZEFlags ZEVolumeCastReportComponents;
ZE_ENUM(ZEVolumeCastReportComponent)
{
	ZE_VCRE_ALL					= 0xFFFFFFFF,
	ZE_VCRE_NONE				= 0x0,
	ZE_VCRE_MULTI_COLLISIONS	= 0x1,
	ZE_VCRE_BOUNDING_BOX_ENTER	= 0x2,
	ZE_VCRE_BOUNDING_BOX_EXIT	= 0x4,
	ZE_VCRE_POLYGONS			= 0x8
};

ZE_ENUM(ZEVolumeCastCollisionType)
{
	ZE_VCCT_NONE				= 0,
	ZE_VCCT_BOUNDING_BOX		= 1,
	ZE_VCCT_BOUNDING_BOX_ENTER	= ZE_VCCT_BOUNDING_BOX,
	ZE_VCCT_BOUNDING_BOX_EXIT	= 2,
	ZE_VCCT_POLGON				= 3,
};

enum ZEVolumeCastMatch
{
	ZE_VCM_NONE					= 0,
	ZE_VCM_FIRST				= 1,
	ZE_VCM_NEAREST				= 2,
	ZE_VCM_FURTHEST				= 3
};

class ZEVolumeCastCollision;

typedef ZEDelegate<bool (ZEObject*, void*)> ZEVolumeCastFilterFunction;
typedef ZEDelegate<bool (ZEVolumeCastCollision&, const void*)> ZEVolumeCastModifierFunction;

class ZEVolumeCastParameters : public ZEObject
{
	public:
		ZERay									Ray;
		ZEVolumeCastReportComponents			Components;
		ZEVolumeCastMatch						Match;

		ZEClass*								FilterClass;
		ZEArray<ZEClass*>						FilterIncludedClasses;
		ZEArray<ZEClass*>						FilterExludedClasses;

		ZEArray<ZEObject*>						FilterIncludedObjects;
		ZEArray<ZEObject*>						FilterExcludedObjects;

		ZEVolumeCastFilterFunction				FilterFunction;
		void*									FilterFunctionParameter;

		float									MinimumDistance;
		float									MaximumDistance;

		void*									ExtraParameters;

		bool									Filter(ZEObject* Object) const;

												ZEVolumeCastParameters();
};

class ZEVolumeCastCollision : public ZEObject
{
	public:
		ZEVolumeCastCollisionType				Type;

		ZEObject*								Object;
		ZEObject*								SubObject;

		float									Distance;
		ZEVector3								Position;
		ZETriangle								Polygon;

		const void*								CustomValue;

												ZEVolumeCastCollision();
};

class ZEVolumeCastReport : public ZEObject
{
	private:
		const ZEVolumeCastParameters*			Parameters;
		ZEVolumeCastCollision					Collision;
		ZEArray<ZEVolumeCastCollision>			Collisions;
		float									MinimumDistance;
		float									MaximumDistance;

		const void*								ModifierParameter;
		ZEVolumeCastModifierFunction			ModifierFunction;

	public:
		bool									GetResult() const;
		const ZEVolumeCastCollision&			GetCollision() const;
		const ZEArray<ZEVolumeCastCollision>	GetCollisions() const;

		float									GetMinimumDistance();
		float									GetMaximumDistance();

		void									SetParameters(const ZEVolumeCastParameters* Parameters);
		const ZEVolumeCastParameters*			GetParameters() const;

		void									SetModifierFunction(const ZEVolumeCastModifierFunction& Function);
		const ZEVolumeCastModifierFunction&		GetModifierFunction();

		void									SetModifierParameter(const void* Parameter);
		const void*								GetModifierParameter();

		virtual void							AddCollision(const ZEVolumeCastCollision& Collision);

		bool									CheckDistance(float Distance) const;
		bool									CheckDone() const;

												ZEVolumeCastReport();
};


class ZEVolumeCastHelper : public ZEObject
{
	private:
		mutable bool							Dirty;
		mutable ZERay							LocalRay;
		const ZEMatrix4x4*						WorldTransform;
		const ZEMatrix4x4*						InvWorldTransform;

		ZEObject*								Object;
		ZEObject*								SubObject;
		ZEVolumeCastReport*						Report;
		const void*								CustomValue;

		void									InitializeCollision(ZEVolumeCastCollision& Collision, float RayT) const;

		void									Update()  const;

	public:
		void									SetReport(ZEVolumeCastReport* Parameters);
		ZEVolumeCastReport*						GetReport() const;

		void									SetWorldTransform(const ZEMatrix4x4* WorldTransform);
		const ZEMatrix4x4*						GetWorldTransform() const;

		void									SetInvWorldTransform(const ZEMatrix4x4* InvWorldTransform);
		const ZEMatrix4x4*						GetInvWorldTransform() const;

		void									SetObject(ZEObject* Object);
		ZEObject*								GetObject();

		void									SetSubObject(ZEObject* Object);
		ZEObject*								SetSubObject();

		void									SetCustomValue(const void* Value);
		const void*								GetCustomValue();

		const ZERay&							GetLocalRay() const;

		bool									TestBoundingBox(const ZEAABBox& WorldBoundingBox) const;

		bool									VolumeCastPolygon(const ZEVector3& Vertex0, const ZEVector3& Vertex1, const ZEVector3& Vertex3) const;

		bool									VolumeCastBoundingBox(const ZEAABBox& WorldBoundingBox) const;
		bool									VolumeCastBoundingBox(const ZEAABBox& WorldBoundingBox, const ZEAABBox& LocalBoundingBox) const;

		bool									VolumeCastMesh(const void* VertexBuffer, ZESize VertexCount, ZESize VertexStride) const;
		bool									VolumeCastMesh(const void* PolygonBuffer, ZESize PolgonCount, ZESize PolygonStride, 
															ZESize Vertex0Offset, ZESize Vertex1Offset, ZESize Vertex2Offset) const;

												ZEVolumeCastHelper();
};
