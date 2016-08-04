//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEVolumeCast.cpp
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

#include "ZEVolumeCast.h"

#include "ZEMeta/ZEObject.h"
#include "ZEMeta/ZEClass.h"
#include "ZEMath/ZEMath.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZETriangle.h"


// ZEVolumeCastParameters
//////////////////////////////////////////////////////////////////////////////////////

ZEVolumeCastParameters::ZEVolumeCastParameters()
{
	Ray.p = ZEVector3::Zero;
	Ray.v = ZEVector3::Zero;
	Match = ZE_VCM_NEAREST;

	FilterFunctionParameter = NULL;
	Components = ZE_VCRE_BOUNDING_BOX_ENTER;

	FilterClass = NULL;
	FilterFunctionParameter = NULL;

	MinimumDistance = 0.0f;
	MaximumDistance = ZE_FLOAT_MAX;

	ExtraParameters = NULL;
}

bool ZEVolumeCastParameters::Filter(ZEObject* Object) const
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


// ZEVolumeCastCollision
//////////////////////////////////////////////////////////////////////////////////////

ZEVolumeCastCollision::ZEVolumeCastCollision()
{
	Type = ZE_VCCT_NONE;

	Object = NULL;
	SubObject = NULL;

	Distance = 0.0f;
	Position = ZEVector3::Zero;
	Polygon.V0 = ZEVector3::Zero;
	Polygon.V1 = ZEVector3::Zero;
	Polygon.V2 = ZEVector3::Zero;

	CustomValue = NULL;
};


// ZEVolumeCastReport
//////////////////////////////////////////////////////////////////////////////////////

ZEVolumeCastReport::ZEVolumeCastReport()
{
	Parameters = NULL;
	MinimumDistance = ZE_FLOAT_MAX;
	MaximumDistance = ZE_FLOAT_MIN;
	ModifierParameter = NULL;
}

bool ZEVolumeCastReport::GetResult() const
{
	return (Collision.Type != ZE_VCCT_NONE);
}

const ZEVolumeCastCollision& ZEVolumeCastReport::GetCollision() const
{
	return Collision;
}

const ZEArray<ZEVolumeCastCollision> ZEVolumeCastReport::GetCollisions() const
{
	return Collisions;
}

float ZEVolumeCastReport::GetMinimumDistance()
{
	return MinimumDistance;
}

float ZEVolumeCastReport::GetMaximumDistance()
{
	return MaximumDistance;
}

void ZEVolumeCastReport::SetParameters(const ZEVolumeCastParameters* Parameters)
{
	this->Parameters = Parameters;
}

const ZEVolumeCastParameters* ZEVolumeCastReport::GetParameters() const
{
	return Parameters;
}

void ZEVolumeCastReport::SetModifierFunction(const ZEVolumeCastModifierFunction& Function)
{
	ModifierFunction = Function;
}

const ZEVolumeCastModifierFunction& ZEVolumeCastReport::GetModifierFunction()
{
	return ModifierFunction;
}

void ZEVolumeCastReport::SetModifierParameter(const void* Parameter)
{
	ModifierParameter = Parameter;
}

const void* ZEVolumeCastReport::GetModifierParameter()
{
	return ModifierParameter;
}

void ZEVolumeCastReport::AddCollision(const ZEVolumeCastCollision& NewCollision)
{
	if (NewCollision.Type == ZE_VCCT_NONE)
		return;

	if (Collision.Type == ZE_VCCT_NONE)
	{
		Collision = NewCollision;
		ModifierFunction.CheckAndCall(Collision, ModifierParameter);
		MinimumDistance = Collision.Distance;
		MaximumDistance = Collision.Distance;
	}
	else if (Parameters->Match == ZE_VCM_NEAREST)
	{
		if (NewCollision.Distance < MinimumDistance)
		{
			Collision = NewCollision;
			ModifierFunction.CheckAndCall(Collision, ModifierParameter);
		}
	}
	else if (Parameters->Match == ZE_VCM_FURTHEST)
	{
		if (Collision.Distance > MaximumDistance)
		{
			Collision = NewCollision;
			ModifierFunction.CheckAndCall(Collision, ModifierParameter);
		}
	}

	if (Collision.Distance < MinimumDistance)
		MinimumDistance = Collision.Distance;

	if (Collision.Distance > MaximumDistance)
		MaximumDistance = Collision.Distance;

	if (Parameters->Components.GetFlags(ZE_VCRE_MULTI_COLLISIONS))
	{
		if (Parameters->Match == ZE_VCM_NEAREST)
		{
			for (ZESize I = 0; I < Collisions.GetCount(); I++)
			{
				if (NewCollision.Distance < Collisions[I].Distance)
				{
					Collisions.Insert(I, NewCollision);
					ModifierFunction.CheckAndCall(Collisions[I], ModifierParameter);
					return;
				}
			}
		}
		else if (Parameters->Match == ZE_VCM_FURTHEST)
		{
			for (ZESize I = 0; I < Collisions.GetCount(); I++)
			{
				if (NewCollision.Distance > Collisions[I].Distance)
				{
					Collisions.Insert(I, NewCollision);
					ModifierFunction.CheckAndCall(Collisions[I], ModifierParameter);
					return;
				}
			}
		}

		Collisions.Add(NewCollision);
		ModifierFunction.CheckAndCall(Collisions.GetLastItem(), ModifierParameter);
	}
}

bool ZEVolumeCastReport::CheckDistance(float Distance) const
{
	if (Distance < Parameters->MinimumDistance && Distance > Parameters->MaximumDistance)
		return false;

	if (!Parameters->Components.GetFlags(ZE_VCRE_MULTI_COLLISIONS))
	{
		if (Parameters->Match == ZE_VCM_NEAREST)
		{
			if (Distance > MinimumDistance)
				return false;
		}
		else if (Parameters->Match == ZE_VCM_FURTHEST)
		{
			if (Distance < MaximumDistance)
				return false;
		}
	}

	return true;
}

bool ZEVolumeCastReport::CheckDone() const
{
	return (!Parameters->Components.GetFlags(ZE_VCRE_MULTI_COLLISIONS) && Collision.Type != ZE_VCCT_NONE && Collision.Type != ZE_VCM_FIRST);
}


// ZEVolumeCastHelper
//////////////////////////////////////////////////////////////////////////////////////

void ZEVolumeCastHelper::InitializeCollision(ZEVolumeCastCollision& Collision, float RayT) const
{
	Collision.Object = Object;
	Collision.SubObject = SubObject;
	Collision.Distance = RayT;
	Collision.Position = Report->GetParameters()->Ray.GetPointOn(RayT);
	Collision.CustomValue = CustomValue;
}

void ZEVolumeCastHelper::Update() const
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

void ZEVolumeCastHelper::SetReport(ZEVolumeCastReport* Report)
{
	this->Report = Report;
}

ZEVolumeCastReport* ZEVolumeCastHelper::GetReport() const
{
	return Report;
}

void ZEVolumeCastHelper::SetWorldTransform(const ZEMatrix4x4* WorldTransform)
{
	this->WorldTransform = WorldTransform;
}

const ZEMatrix4x4* ZEVolumeCastHelper::GetWorldTransform() const
{
	return WorldTransform;
}

void ZEVolumeCastHelper::SetInvWorldTransform(const ZEMatrix4x4* InvWorldTransform)
{
	this->InvWorldTransform = InvWorldTransform;
	Dirty = true;
}

const ZEMatrix4x4* ZEVolumeCastHelper::GetInvWorldTransform() const
{
	return InvWorldTransform;
}

void ZEVolumeCastHelper::SetObject(ZEObject* Object)
{
	this->Object = Object;
}

ZEObject* ZEVolumeCastHelper::GetObject()
{
	return Object;
}

void ZEVolumeCastHelper::SetSubObject(ZEObject* Object)
{
	SubObject = Object;
}

ZEObject* ZEVolumeCastHelper::SetSubObject()
{
	return SubObject;
}

void ZEVolumeCastHelper::SetCustomValue(const void* Value)
{
	CustomValue = Value;
}

const void* ZEVolumeCastHelper::GetCustomValue()
{
	return CustomValue;
}

const ZERay& ZEVolumeCastHelper::GetLocalRay() const
{
	Update();
	return LocalRay;
}

bool ZEVolumeCastHelper::TestBoundingBox(const ZEAABBox& WorldBoundingBox) const
{
	float EnteranceT, ExitT;
	if (!ZEAABBox::IntersectionTest(WorldBoundingBox, Report->GetParameters()->Ray, EnteranceT, ExitT))
		return false;

	if (!Report->CheckDistance(EnteranceT) && !Report->CheckDistance(ExitT))
		return false;

	return true;
}

bool ZEVolumeCastHelper::VolumeCastBoundingBox(const ZEAABBox& WorldBoundingBox) const
{
	const ZEVolumeCastParameters& Parameters = *Report->GetParameters();
	float EnteranceT, ExitT;
	if (!ZEAABBox::IntersectionTest(WorldBoundingBox, Parameters.Ray, EnteranceT, ExitT))
		return false;

	if (Parameters.Components.GetFlags(ZE_VCRE_BOUNDING_BOX_ENTER) && Report->CheckDistance(EnteranceT))
	{
		ZEVolumeCastCollision Enterance;
		InitializeCollision(Enterance, EnteranceT);
		Enterance.Type = ZE_VCCT_BOUNDING_BOX_ENTER;
		Report->AddCollision(Enterance);
	}

	if (Parameters.Components.GetFlags(ZE_VCRE_BOUNDING_BOX_EXIT) && Report->CheckDistance(ExitT))
	{
		ZEVolumeCastCollision Exit;
		InitializeCollision(Exit, ExitT);
		Exit.Type = ZE_VCCT_BOUNDING_BOX_EXIT;
		Report->AddCollision(Exit);
	}

	return true;
}

bool ZEVolumeCastHelper::VolumeCastBoundingBox(const ZEAABBox& WorldBoundingBox, const ZEAABBox& LocalBoundingBox) const
{
	const ZEVolumeCastParameters& Parameters = *Report->GetParameters();
	float EnteranceT, ExitT;
	if (!ZEAABBox::IntersectionTest(WorldBoundingBox, Parameters.Ray, EnteranceT, ExitT))
		return false;

	Update();

	if (!ZEAABBox::IntersectionTest(LocalBoundingBox, LocalRay, EnteranceT, ExitT))
		return false;

	if (!Report->CheckDistance(EnteranceT) && !Report->CheckDistance(ExitT))
		return false;

	if (Parameters.Components.GetFlags(ZE_VCRE_BOUNDING_BOX_ENTER) && Report->CheckDistance(EnteranceT))
	{
		ZEVolumeCastCollision Enterance;
		InitializeCollision(Enterance, EnteranceT);
		Enterance.Type = ZE_VCCT_BOUNDING_BOX_ENTER;
		Report->AddCollision(Enterance);
	}

	if (Parameters.Components.GetFlags(ZE_VCRE_BOUNDING_BOX_EXIT) && Report->CheckDistance(ExitT))
	{
		ZEVolumeCastCollision Exit;
		InitializeCollision(Exit, ExitT);
		Exit.Type = ZE_VCCT_BOUNDING_BOX_EXIT;
		Report->AddCollision(Exit);
	}

	return true;
}

bool ZEVolumeCastHelper::VolumeCastPolygon(const ZEVector3& Vertex0, const ZEVector3& Vertex1, const ZEVector3& Vertex2) const
{
	const ZEVolumeCastParameters& Parameters = *Report->GetParameters();
	ZETriangle Triangle(Vertex0, Vertex1, Vertex2);
	float RayT;
	if (!ZETriangle::IntersectionTest(Triangle, LocalRay, RayT))
		return false;

	if (!Report->CheckDistance(RayT))
		return false;

	ZEVolumeCastCollision Collision;
	InitializeCollision(Collision, RayT);
	Collision.Type = ZE_VCCT_POLGON;
	ZEMatrix4x4::Transform(Collision.Polygon.V0, *WorldTransform, Triangle.V0);
	ZEMatrix4x4::Transform(Collision.Polygon.V1, *WorldTransform, Triangle.V1);
	ZEMatrix4x4::Transform(Collision.Polygon.V2, *WorldTransform, Triangle.V2);
	Report->AddCollision(Collision);

	return true;
}


bool ZEVolumeCastHelper::VolumeCastMesh(const void* VertexBuffer, ZESize VertexCount, ZESize VertexStride) const
{
	return VolumeCastMesh(VertexBuffer, VertexCount / 3, 3 * VertexStride, 0, VertexStride, 2 * VertexStride);
}

bool ZEVolumeCastHelper::VolumeCastMesh(const void* PolygonBuffer,  ZESize PolgonCount, ZESize PolygonStride, 
								  ZESize Vertex0Offset, ZESize Vertex1Offset, ZESize Vertex2Offset) const
{
	const ZEVolumeCastParameters& Parameters = *Report->GetParameters();
	if (!Parameters.Components.GetFlags(ZE_VCRE_POLYGONS))
		return false;

	Update();

	bool Result = false;
	ZEBYTE* PolgonPointer = (ZEBYTE*)PolygonBuffer;
	for (ZESize I = 0; I < PolgonCount; I++)
	{
		ZEVector3* Vertex0 = (ZEVector3*)(PolgonPointer + Vertex0Offset);
		ZEVector3* Vertex1 = (ZEVector3*)(PolgonPointer + Vertex1Offset);
		ZEVector3* Vertex2 = (ZEVector3*)(PolgonPointer + Vertex2Offset);
		PolgonPointer += PolygonStride;

		Result |= VolumeCastPolygon(*Vertex0, *Vertex1, *Vertex2);

		if (Report->CheckDone())
			break;
	}

	return Result;
}

ZEVolumeCastHelper::ZEVolumeCastHelper()
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
