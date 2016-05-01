//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERayCast.cpp
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

#include "ZERayCast.h"

#include "ZEMeta/ZEObject.h"
#include "ZEMeta/ZEClass.h"
#include "ZEMath/ZEMath.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZETriangle.h"


// ZERayCastParameters
//////////////////////////////////////////////////////////////////////////////

ZERayCastParameters::ZERayCastParameters()
{
	Ray.p = ZEVector3::Zero;
	Ray.v = ZEVector3::Zero;
	Match = ZE_RCM_NEAREST;

	FilterFunctionParameter = NULL;
	Components = ZE_RCRE_BOUNDING_BOX_ENTER;

	FilterClass = NULL;
	FilterFunctionParameter = NULL;

	MinimumDistance = 0.0f;
	MaximumDistance = ZE_FLOAT_MAX;

	ExtraParameters = NULL;
}

bool ZERayCastParameters::Filter(ZEObject* Object) const
{
	if (FilterClass != NULL)
	{
		if (!ZEClass::IsDerivedFrom(FilterClass, Object->GetClass()))
			return false;
	}

	if (FilterIncludedClasses.GetCount() != 0)
	{
		for (ZESize I = 0; I < FilterExludedClasses.GetCount(); I++)
		{
			if (ZEClass::IsDerivedFrom(FilterExludedClasses[I], Object->GetClass()))
				return false;
		}
	}

	for (ZESize I = 0; I < FilterExludedClasses.GetCount(); I++)
	{
		if (!ZEClass::IsDerivedFrom(FilterExludedClasses[I], Object->GetClass()))
			return false;
	}

	if (FilterIncludedObjects.GetCount() != 0 && !FilterIncludedObjects.Exists(Object))
		return false;
	
	if (FilterExcludedObjects.Exists(Object))
		return false;


	if (!FilterFunction.IsNull())
	{
		if (!FilterFunction(Object, FilterFunctionParameter))
			return false;
	}

	return true;
}


// ZERayCastCollision
//////////////////////////////////////////////////////////////////////////////

ZERayCastCollision::ZERayCastCollision()
{
	Type = ZE_RCCT_NONE;

	Object = NULL;
	SubObject = NULL;

	Distance = 0.0f;
	Position = ZEVector3::Zero;
	Polygon.V0 = ZEVector3::Zero;
	Polygon.V1 = ZEVector3::Zero;
	Polygon.V2 = ZEVector3::Zero;

	CustomValue = NULL;
};


// ZERayCastReport
//////////////////////////////////////////////////////////////////////////////

ZERayCastReport::ZERayCastReport()
{
	Parameters = NULL;
	MinimumDistance = ZE_FLOAT_MAX;
	MaximumDistance = ZE_FLOAT_MIN;
}

bool ZERayCastReport::GetResult() const
{
	return (Collision.Type != ZE_RCCT_NONE);
}

const ZERayCastCollision& ZERayCastReport::GetCollision() const
{
	return Collision;
}

const ZEArray<ZERayCastCollision> ZERayCastReport::GetCollisions() const
{
	return Collisions;
}

float ZERayCastReport::GetMinimumDistance()
{
	return MinimumDistance;
}

float ZERayCastReport::GetMaximumDistance()
{
	return MaximumDistance;
}

void ZERayCastReport::SetParameters(const ZERayCastParameters* Parameters)
{
	this->Parameters = Parameters;
}

const ZERayCastParameters* ZERayCastReport::GetParameters() const
{
	return Parameters;
}

void ZERayCastReport::SetModifierFunction(const ZERayCastModifierFunction& Function)
{
	ModifierFunction = Function;
}

const ZERayCastModifierFunction& ZERayCastReport::GetModifierFunction()
{
	return ModifierFunction;
}

void ZERayCastReport::SetModifierParameter(const void* Parameter)
{
	ModifierParameter = Parameter;
}

const void* ZERayCastReport::GetModifierParameter()
{
	return ModifierParameter;
}

void ZERayCastReport::AddCollision(const ZERayCastCollision& NewCollision)
{
	if (NewCollision.Type == ZE_RCCT_NONE)
		return;

	if (!ModifierFunction.IsNull())
	{
		if (!ModifierFunction(Collision, ModifierParameter))
			return;
	}

	if (Collision.Type == ZE_RCCT_NONE)
	{
		Collision = NewCollision;
		MinimumDistance = Collision.Distance;
		MaximumDistance = Collision.Distance;
	}
	else if (Parameters->Match == ZE_RCM_NEAREST)
	{
		if (NewCollision.Distance < MinimumDistance)
			Collision = NewCollision;
	}
	else if (Parameters->Match == ZE_RCM_FURTHEST)
	{
		if (Collision.Distance > MaximumDistance)
			Collision = NewCollision;
	}

	if (Collision.Distance < MinimumDistance)
		MinimumDistance = Collision.Distance;

	if (Collision.Distance > MaximumDistance)
		MaximumDistance = Collision.Distance;

	if (Parameters->Components.GetFlags(ZE_RCRE_MULTI_COLLISIONS))
	{
		if (Parameters->Match == ZE_RCM_NEAREST)
		{
			for (ZESize I = 0; I < Collisions.GetCount(); I++)
			{
				if (NewCollision.Distance < Collisions[I].Distance)
				{
					Collisions.Insert(I, NewCollision);
					return;
				}
			}
		}
		else if (Parameters->Match == ZE_RCM_FURTHEST)
		{
			for (ZESize I = 0; I < Collisions.GetCount(); I++)
			{
				if (NewCollision.Distance > Collisions[I].Distance)
				{
					Collisions.Insert(I, NewCollision);
					return;
				}
			}
		}

		Collisions.Add(NewCollision);
	}
}

bool ZERayCastReport::CheckDistance(float Distance) const
{
	if (Distance < Parameters->MinimumDistance && Distance > Parameters->MaximumDistance)
		return false;

	if (!Parameters->Components.GetFlags(ZE_RCRE_MULTI_COLLISIONS))
	{
		if (Parameters->Match == ZE_RCM_NEAREST)
		{
			if (Distance > MinimumDistance)
				return false;
		}
		else if (Parameters->Match == ZE_RCM_FURTHEST)
		{
			if (Distance < MaximumDistance)
				return false;
		}
	}

	return true;
}

bool ZERayCastReport::CheckDone() const
{
	return (!Parameters->Components.GetFlags(ZE_RCRE_MULTI_COLLISIONS) && Collision.Type != ZE_RCCT_NONE && Collision.Type != ZE_RCM_FIRST);
}


// ZERayCastHelper
//////////////////////////////////////////////////////////////////////////////

void ZERayCastHelper::InitializeCollision(ZERayCastCollision& Collision, float RayT) const
{
	Collision.Object = Object;
	Collision.SubObject = SubObject;
	Collision.Distance = RayT;
	Collision.Position = Report->GetParameters()->Ray.GetPointOn(RayT);
	Collision.CustomValue = CustomValue;
}

void ZERayCastHelper::Update() const
{
	zeDebugCheck(Report == NULL, "Report is NULL.");
	zeDebugCheck(WorldTransform == NULL, "WorldTransform is NULL.");
	zeDebugCheck(InvWorldTransform == NULL, "InvWorldTransform is NULL.");

	if (!Dirty)
		return;

	ZEMatrix4x4::Transform(LocalRay.p, *InvWorldTransform, Report->GetParameters()->Ray.p);
	ZEMatrix4x4::Transform3x3(LocalRay.v, *InvWorldTransform, Report->GetParameters()->Ray.v);

	Dirty = false;
}

void ZERayCastHelper::SetReport(ZERayCastReport* Report)
{
	this->Report = Report;
}

ZERayCastReport* ZERayCastHelper::GetReport() const
{
	return Report;
}

void ZERayCastHelper::SetWorldTransform(const ZEMatrix4x4* WorldTransform)
{
	this->WorldTransform = WorldTransform;
}

const ZEMatrix4x4* ZERayCastHelper::GetWorldTransform() const
{
	return WorldTransform;
}

void ZERayCastHelper::SetInvWorldTransform(const ZEMatrix4x4* InvWorldTransform)
{
	this->InvWorldTransform = InvWorldTransform;
	Dirty = true;
}

const ZEMatrix4x4* ZERayCastHelper::GetInvWorldTransform() const
{
	return InvWorldTransform;
}

void ZERayCastHelper::SetObject(ZEObject* Object)
{
	this->Object = Object;
}

ZEObject* ZERayCastHelper::GetObject()
{
	return Object;
}

void ZERayCastHelper::SetSubObject(ZEObject* Object)
{
	SubObject = Object;
}

ZEObject* ZERayCastHelper::SetSubObject()
{
	return SubObject;
}

void ZERayCastHelper::SetCustomValue(const void* Value)
{
	CustomValue = Value;
}

const void* ZERayCastHelper::GetCustomValue()
{
	return CustomValue;
}

const ZERay& ZERayCastHelper::GetLocalRay() const
{
	Update();
	return LocalRay;
}

bool ZERayCastHelper::TestBoundingBox(const ZEAABBox& WorldBoundingBox) const
{
	float EnteranceT, ExitT;
	if (!ZEAABBox::IntersectionTest(WorldBoundingBox, Report->GetParameters()->Ray, EnteranceT, ExitT))
		return false;

	if (!Report->CheckDistance(EnteranceT) && !Report->CheckDistance(ExitT))
		return false;

	return true;
}

bool ZERayCastHelper::RayCastBoundingBox(const ZEAABBox& WorldBoundingBox) const
{
	const ZERayCastParameters& Parameters = *Report->GetParameters();
	float EnteranceT, ExitT;
	if (!ZEAABBox::IntersectionTest(WorldBoundingBox, Parameters.Ray, EnteranceT, ExitT))
		return false;

	if (Parameters.Components.GetFlags(ZE_RCRE_BOUNDING_BOX_ENTER) && Report->CheckDistance(EnteranceT))
	{
		ZERayCastCollision Enterance;
		InitializeCollision(Enterance, EnteranceT);
		Enterance.Type = ZE_RCCT_BOUNDING_BOX_ENTER;
		Report->AddCollision(Enterance);
	}

	if (Parameters.Components.GetFlags(ZE_RCRE_BOUNDING_BOX_EXIT) && Report->CheckDistance(ExitT))
	{
		ZERayCastCollision Exit;
		InitializeCollision(Exit, ExitT);
		Exit.Type = ZE_RCCT_BOUNDING_BOX_EXIT;
		Report->AddCollision(Exit);
	}

	return true;
}

bool ZERayCastHelper::RayCastBoundingBox(const ZEAABBox& WorldBoundingBox, const ZEAABBox& LocalBoundingBox) const
{
	const ZERayCastParameters& Parameters = *Report->GetParameters();
	float EnteranceT, ExitT;
	if (!ZEAABBox::IntersectionTest(WorldBoundingBox, Parameters.Ray, EnteranceT, ExitT))
		return false;

	Update();

	if (!ZEAABBox::IntersectionTest(LocalBoundingBox, LocalRay, EnteranceT, ExitT))
		return false;

	if (!Report->CheckDistance(EnteranceT) && !Report->CheckDistance(ExitT))
		return false;

	if (Parameters.Components.GetFlags(ZE_RCRE_BOUNDING_BOX_ENTER) && Report->CheckDistance(EnteranceT))
	{
		ZERayCastCollision Enterance;
		InitializeCollision(Enterance, EnteranceT);
		Enterance.Type = ZE_RCCT_BOUNDING_BOX_ENTER;
		Report->AddCollision(Enterance);
	}

	if (Parameters.Components.GetFlags(ZE_RCRE_BOUNDING_BOX_EXIT) && Report->CheckDistance(ExitT))
	{
		ZERayCastCollision Exit;
		InitializeCollision(Exit, ExitT);
		Exit.Type = ZE_RCCT_BOUNDING_BOX_EXIT;
		Report->AddCollision(Exit);
	}

	return true;
}

bool ZERayCastHelper::RayCastPolygon(const ZEVector3& Vertex0, const ZEVector3& Vertex1, const ZEVector3& Vertex2) const
{
	const ZERayCastParameters& Parameters = *Report->GetParameters();
	ZETriangle Triangle(Vertex0, Vertex1, Vertex2);
	float RayT;
	if (!ZETriangle::IntersectionTest(Triangle, LocalRay, RayT))
		return false;

	if (Report->CheckDistance(RayT))
		return false;

	ZERayCastCollision Collision;
	InitializeCollision(Collision, RayT);
	Collision.Type = ZE_RCCT_POLGON;
	ZEMatrix4x4::Transform(Collision.Polygon.V0, *WorldTransform, Triangle.V0);
	ZEMatrix4x4::Transform(Collision.Polygon.V1, *WorldTransform, Triangle.V1);
	ZEMatrix4x4::Transform(Collision.Polygon.V2, *WorldTransform, Triangle.V2);
	Report->AddCollision(Collision);

	return true;
}


bool ZERayCastHelper::RayCastMesh(const void* VertexBuffer, ZESize VertexCount, ZESize VertexStride) const
{
	return RayCastMesh(VertexBuffer, VertexCount / 3, 3 * VertexStride, 0, VertexStride, 2 * VertexStride);
}

bool ZERayCastHelper::RayCastMesh(const void* PolygonBuffer,  ZESize PolgonCount, ZESize PolygonStride, 
								  ZESize Vertex0Offset, ZESize Vertex1Offset, ZESize Vertex2Offset) const
{
	const ZERayCastParameters& Parameters = *Report->GetParameters();
	if (!Parameters.Components.GetFlags(ZE_RCRE_POLYGONS))
		return false;

	Update();

	bool Result = false;
	ZEBYTE* PolgonPointer = (ZEBYTE*)PolygonBuffer;
	for (ZESize I = 0; I < PolgonCount; I++)
	{
		ZEVector3* Vertex0 = (ZEVector3*)(PolgonPointer + Vertex0Offset);
		ZEVector3* Vertex1 = (ZEVector3*)(PolgonPointer + Vertex1Offset);
		ZEVector3* Vertex2 = (ZEVector3*)(PolgonPointer + Vertex1Offset);
		PolgonPointer += PolygonStride;

		Result |= RayCastPolygon(*Vertex0, *Vertex1, *Vertex2);

		if (Report->CheckDone())
			break;
	}

	return Result;
}

ZERayCastHelper::ZERayCastHelper()
{
	Dirty = true;
	LocalRay.p = ZEVector3::Zero;
	LocalRay.v = ZEVector3::Zero;
	Report = NULL;
	WorldTransform = NULL;
	InvWorldTransform = NULL;

	Object = NULL;
	SubObject = NULL;
	CustomValue = NULL;
}
