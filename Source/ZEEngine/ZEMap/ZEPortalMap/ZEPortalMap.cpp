//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPortalMap.cpp
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

#include "ZEPortalMap.h"
#include "ZEPortalMapResource.h"
#include "ZEPortalMapPortal.h"
#include "ZEPortalMapDoor.h"
#include "ZEError.h"
#include "ZECore/ZEConsole.h"
#include "ZEGame/ZEDrawParameters.h"
#include "ZEMath/ZETriangle.h"
#include "ZEMath/ZERay.h"
#include "ZEMath/ZEViewFrustum.h"
#include "ZEMath/ZEAngle.h"
#include "ZEPhysics/ZEPhysicalMesh.h"

ZEDrawFlags ZEPortalMap::GetDrawFlags() const
{
	return ZE_DF_DRAW | ZE_DF_LIGHT_RECIVER;
}

void ZEPortalMap::LoadPortalResource(ZEPortalMapResource* NewResource)
{
	if (Resource != NULL)
	{
		Resource->Release();
		Resource = NULL;
	}

	if (NewResource == NULL)
	{
		for (ZESize I = 0; I < Portals.GetCount(); I++)
			Portals[I]->Deinitialize();

		Portals.SetCount(0);

		for (ZESize I = 0; I < Doors.GetCount(); I++)
			Doors[I]->Deinitialize();

		Doors.SetCount(0);
	}

	this->Resource = NewResource;

	Portals.SetCount(Resource->GetPortals().GetCount());
	for (ZESize I = 0; I < Portals.GetCount(); I++)
	{
		Portals[I] = ZEPortalMapPortal::CreateInstance();
		Portals[I]->Initialize(this, (ZEPortalMapResourcePortal*)&Resource->GetPortals()[I]);
	}

	Doors.SetCount(Resource->GetDoors().GetCount());
	for (ZESize I = 0; I < Doors.GetCount(); I++)
	{
		Doors[I] = ZEPortalMapDoor::CreateInstance();
		Doors[I]->Initialize(this, &Resource->GetDoors()[I]);
	}
}

ZEPortalMap::ZEPortalMap()
{
	Resource = NULL;
	CullMode = ZE_MCM_FULL;
}

ZEPortalMap::~ZEPortalMap()
{
	Deinitialize();
}

const ZEArray<ZEPortalMapPortal*>& ZEPortalMap::GetPortals()
{
	return Portals;
}

const ZEArray<ZEPortalMapDoor*>& ZEPortalMap::GetDoors()
{
	return Doors;
}

const ZEPortalMapCullStatistics& ZEPortalMap::GetCullStatistics()
{
	return Statistics;
}

bool ZEPortalMap::Initialize()
{
	if (GetInitialized())
		return false;

	if (PortalMapFile != "")
	{
		ZEPortalMapResource* NewResource = ZEPortalMapResource::LoadSharedResource(ZEString("Resources\\") + PortalMapFile);
		if (NewResource != NULL)
			LoadPortalResource(NewResource);
		else
		{
			zeError("Can not load ZEPortalMap file.");
			return false;
		}
	}

	return ZEEntity::Initialize();
}

void ZEPortalMap::Deinitialize()
{
	if (Resource != NULL)
	{
		Resource->Release();
		Resource = NULL;
	}

	ZEEntity::Deinitialize();
}

bool ZEPortalMap::SetMapFile(const ZEString& FileName)
{
	PortalMapFile = FileName;

	if (!GetInitialized())
	{
		PortalMapFile = FileName;
		return true;
	}

	const ZEPortalMapResource* NewResource = ZEPortalMapResource::LoadSharedResource(PortalMapFile);
	if (NewResource != NULL)
	{
		PortalMapFile = FileName;
		LoadPortalResource(Resource);
	}
	else
	{
		zeError("Can not load ZEPortalMap file.");
		return false;
	}

	return false;
}

const ZEString& ZEPortalMap::GetMapFile() const
{
	return PortalMapFile;
}

void ZEPortalMap::Draw(ZEDrawParameters* DrawParameters)
{
	if (!GetVisible())
		return;

	memset(&Statistics, 0, sizeof(ZEPortalMapCullStatistics));

	for (size_t I = 0; I < Portals.GetCount(); I++)
	{
		Portals[I]->CullPass = false;
		Portals[I]->IsDrawn = false;
	}

	CullPortals(DrawParameters);
}

bool ZEPortalMap::CastRay(const ZERay& Ray, ZEVector3& Position, ZEVector3& Normal, float& MinT)
{
	if (Resource == NULL)
		return false;

	float T;
	bool Found = false;
	for (ZESize I = 0; I < Resource->GetPortals().GetCount(); I++)
	{
		const ZEPortalMapResourcePortal* CurrentPortal = &Resource->GetPortals()[I];
		//if (ZEAABBox::IntersectionTest(CurrentPortal->BoundingBox,Ray))
		for (ZESize N = 0; N < CurrentPortal->Polygons.GetCount(); N++)
		{
			const ZEPortalMapPolygon& MapPolygon = CurrentPortal->Polygons[N];
			ZETriangle Triangle(MapPolygon.Vertices[0].Position, MapPolygon.Vertices[1].Position, MapPolygon.Vertices[2].Position);
			if (ZETriangle::IntersectionTest(Triangle, Ray, T) && (!Found || MinT > T))
			{
				MinT = T;
				ZERay::GetPointOn(Position, Ray, T);
				ZETriangle::GetNormal(Triangle, Normal);
				Found = true;
			}
		}
	}

	return Found;
}

ZEPortalMapResource* ZEPortalMap::GetResource() const
{
	return Resource;
}

ZEPortalMap* ZEPortalMap::CreateInstance()
{
	return new ZEPortalMap();
}

static inline ZESize Circular(ZESSize Index, ZESize Count) 
{
	if (Index < 0)
	{
		ZESSize CircularIndex = ((-Index) % Count);
		return (CircularIndex == 0 ? 0 : Count - CircularIndex);
	}
	else
		return Index % Count;
}

static void IntersectionTest(ZEVector3* IntersectedPoints, ZESize& IntersectedPointCount, const ZEPlane& Plane, const ZEVector3* Points, ZESize PointCount)
{
	IntersectedPointCount = 0;
	if (PointCount == 0)
		return;

	bool CurrentResult = ZEPlane::TestHalfSpace(Plane, Points[0]) != ZE_HS_NEGATIVE_SIDE;
	for (size_t I = 0; I < PointCount; I++)
	{
		bool NextResult = ZEPlane::TestHalfSpace(Plane, Points[Circular(I + 1, PointCount)]) != ZE_HS_NEGATIVE_SIDE;
		if (CurrentResult)
		{
			IntersectedPoints[IntersectedPointCount++] = Points[Circular(I, PointCount)];

			if (!NextResult)
			{
				ZELineSegment Edge(Points[Circular(I, PointCount)], Points[Circular(I + 1, PointCount)]);
				float t;
				ZEPlane::IntersectionTest(Plane, Edge, t);
				IntersectedPoints[IntersectedPointCount++] = Edge.GetPointOn(t);
			}
		}
		else if (!CurrentResult && NextResult)
		{
			ZELineSegment Edge(Points[Circular(I, PointCount)], Points[Circular(I + 1, PointCount)]);
			float t;
			ZEPlane::IntersectionTest(Plane, Edge, t);
			IntersectedPoints[IntersectedPointCount++] = Edge.GetPointOn(t);
			IntersectedPoints[IntersectedPointCount++] = Points[Circular(I + 1, PointCount)];
		}

		CurrentResult = NextResult;
	}
}
bool ZEPortalMap::GenerateViewVolume(ZEViewFrustum& NewViewVolume, ZEPortalMapDoor* Door, const ZEViewVolume* OldViewVolume)
{
	ZERectangle3D DoorRectangle = Door->GetRectangle();

	if (OldViewVolume->GetViewVolumeType() == ZE_VVT_FRUSTUM)
	{
		ZEViewFrustum* Frustum = (ZEViewFrustum*)OldViewVolume;

		ZEVector3 Points[32];
		ZESize PointsCount = 4;
		for (ZEInt I = 0;  I < 4; I++)
			Points[I] = DoorRectangle.GetPoint(I);

		ZEVector3 PointsTemp[32];
		ZESize PointTempCount;
		IntersectionTest(PointsTemp, PointTempCount,	Frustum->GetClippingPlane(ZE_VFP_LEFT),		Points, PointsCount);
		IntersectionTest(Points, PointsCount,			Frustum->GetClippingPlane(ZE_VFP_RIGHT),	PointsTemp, PointTempCount);
		IntersectionTest(PointsTemp, PointTempCount,	Frustum->GetClippingPlane(ZE_VFP_BOTTOM),	Points, PointsCount);
		IntersectionTest(Points, PointsCount,			Frustum->GetClippingPlane(ZE_VFP_TOP),		PointsTemp, PointTempCount);

		if (PointsCount == 0)
			return false;

		float TempDotProduct = 0.0f;
		ZEPlane VerticalPlane, HorizontalPlane;

 		float LeftDotProduct = -1.0f; float RightDotProduct = -1.0f;
 		float TopDotProduct = -1.0f; float BottomDotProduct = -1.0f;
		//ZEVector3 LeftPoint, RightPoint, TopPoint, BottomPoint;
		ZEPlane LeftPlane, RightPlane, TopPlane, BottomPlane;

		for (ZESize I = 0; I < PointsCount; I++)
		{
			ZEPlane::Create(VerticalPlane, Frustum->GetPosition(), Points[I], Points[I] - Frustum->GetUp());

			TempDotProduct = ZEVector3::DotProduct(Frustum->GetClippingPlane(ZE_VFP_LEFT).n, VerticalPlane.n);

			if (TempDotProduct > LeftDotProduct)
			{
				LeftDotProduct = TempDotProduct;
				LeftPlane.n = VerticalPlane.n;
				LeftPlane.p = Frustum->GetPosition();
				//LeftPoint = Points[I];
			}

			TempDotProduct = ZEVector3::DotProduct(Frustum->GetClippingPlane(ZE_VFP_RIGHT).n, -VerticalPlane.n);

			if (TempDotProduct > RightDotProduct)
			{
				RightDotProduct = TempDotProduct;
				RightPlane.n = -VerticalPlane.n;
				RightPlane.p = Frustum->GetPosition();
				//RightPoint = Points[I];
			}

			ZEPlane::Create(HorizontalPlane, Frustum->GetPosition(), Points[I], Points[I] - Frustum->GetRight());

			TempDotProduct = ZEVector3::DotProduct(Frustum->GetClippingPlane(ZE_VFP_TOP).n, HorizontalPlane.n);

			if (TempDotProduct > TopDotProduct)
			{
				TopDotProduct = TempDotProduct;
				TopPlane.n = HorizontalPlane.n;
				TopPlane.p = Frustum->GetPosition();
				//TopPoint = Points[I];
			}

			TempDotProduct = ZEVector3::DotProduct(Frustum->GetClippingPlane(ZE_VFP_BOTTOM).n, -HorizontalPlane.n);

			if (TempDotProduct > BottomDotProduct)
			{
				BottomDotProduct = TempDotProduct;
				BottomPlane.n = -HorizontalPlane.n;
				BottomPlane.p = Frustum->GetPosition();
				//BottomPoint = Points[I];
			}
		}

		NewViewVolume.Create(RightPlane, BottomPlane, LeftPlane, TopPlane, Frustum->GetClippingPlane(ZE_VFP_FAR), Frustum->GetClippingPlane(ZE_VFP_NEAR));
		NewViewVolume.SetPosition(Frustum->GetPosition());
		NewViewVolume.Right = Frustum->GetRight();
		NewViewVolume.Up = Frustum->GetUp();
		NewViewVolume.Look = Frustum->GetDirection();

		return true;
	}

	return false;
}

void ZEPortalMap::CullPortal(ZEPortalMapDoor* Door, ZEDrawParameters* DrawParameters, ZEViewVolume* ViewVolume)
{
	if(!Door->GetOpen())
		return;
	
	if (ViewVolume->CullTest(Door->GetRectangle()))
		return;

	ZEPortalMapPortal** DoorPortals = Door->GetPortals();
	for (ZEInt I = 0; I < 2; I++)
	{
		if(DoorPortals[I]->CullPass == true)
			continue;

		ZEViewFrustum NewViewVolume;
		GenerateViewVolume(NewViewVolume, Door, ViewVolume);

		DoorPortals[I]->CullPass = true;
		DoorPortals[I]->Draw(DrawParameters);

		Statistics.DrawedPortalCount++;
		Statistics.DrawedMapPolygonCount += DoorPortals[I]->GetPolygonCount();

		const ZEArray<ZEPortalMapDoor*>& NextDoors = DoorPortals[I]->GetDoors();

		for (size_t J = 0; J < NextDoors.GetCount(); J++)
		{
			if(NextDoors[J] == Door)
				continue;

			CullPortal(NextDoors[J], DrawParameters, (ZEViewVolume*)&NewViewVolume);
		}
	}
}

void ZEPortalMap::CullPortals(ZEDrawParameters* DrawParameters)
{
	if (CullMode == ZE_MCM_NONE)
	{
		for (ZESize I = 0; I < Portals.GetCount(); I++)
		{
			Portals[I]->Draw(DrawParameters);

			Statistics.TotalPortalCount++;
			Statistics.DrawedPortalCount++;
			Statistics.TotalMapPolygonCount += Portals[I]->GetPolygonCount();
			Statistics.DrawedMapPolygonCount += Portals[I]->GetPolygonCount();
		}
	}
	else
	{
		bool NoClip = true;

		if (DrawParameters->ViewVolume->GetViewVolumeType() == ZE_VVT_FRUSTUM)
		{
			ZEViewFrustum* Frustum = (ZEViewFrustum*)DrawParameters->ViewVolume;
			ZEVector3 FrustumPosition = Frustum->GetPosition();

			for (size_t I = 0; I < Portals.GetCount(); I++)
			{
				Statistics.TotalPortalCount++;
				Statistics.TotalMapPolygonCount += Portals[I]->GetPolygonCount();

				if (CullMode == ZE_MCM_VIEW)
					continue;

				if (ZEAABBox::IntersectionTest(Portals[I]->GetWorldBoundingBox(), FrustumPosition))
				{
					NoClip = false;
					Portals[I]->CullPass = true;
					Portals[I]->Draw(DrawParameters);

					Statistics.DrawedPortalCount++;
					Statistics.DrawedMapPolygonCount += Portals[I]->GetPolygonCount();

					const ZEArray<ZEPortalMapDoor*>& Doors = Portals[I]->GetDoors();
					for(size_t J = 0; J < Doors.GetCount(); J++)
					{
						CullPortal(Doors[J], DrawParameters, Frustum);
					}
				}
			}
		}

		if (NoClip)
		{
			for (ZESize I = 0; I < Portals.GetCount(); I++)
			{
				if (!DrawParameters->ViewVolume->CullTest(Portals[I]->GetWorldBoundingBox()))
				{
					Portals[I]->Draw(DrawParameters);

					Statistics.DrawedPortalCount++;
					Statistics.DrawedMapPolygonCount += Portals[I]->GetPolygonCount();
				}
			}
		}

		for (ZESize I = 0; I < Portals.GetCount(); I++)
		{
			if (Portals[I]->IsPersistentDraw)
			{
				if (!Portals[I]->IsDrawn)
				{
					Portals[I]->Draw(DrawParameters);

					Statistics.DrawedPortalCount++;
					Statistics.DrawedMapPolygonCount += Portals[I]->GetPolygonCount();
				}
			}
		}
	}

	Statistics.CulledPortalCount = Statistics.TotalPortalCount - Statistics.DrawedPortalCount;
	Statistics.CulledMapPolygonCount = Statistics.TotalMapPolygonCount - Statistics.DrawedMapPolygonCount;
}

void ZEPortalMap::OnTransformChanged()
{
	for (ZESize I = 0; I < Portals.GetCount(); I++)
	{
		Portals[I]->TransformChanged = true;
		Portals[I]->PhysicalMesh->SetPosition(GetWorldPosition() + Portals[I]->Position);
		Portals[I]->PhysicalMesh->SetRotation(GetWorldRotation() * Portals[I]->Rotation);
		Portals[I]->PhysicalMesh->SetScale(GetWorldScale() * Portals[I]->Scale);
	}

	for (ZESize I = 0; I < Doors.GetCount(); I++)
		Doors[I]->TransformChanged = true;

	ZEEntity::OnTransformChanged();
}

void ZEPortalMap::SetCullMode(ZEMapCullMode Value)
{
	CullMode = Value;
}

ZEEntityRunAt ZEPortalMapDescription::GetRunAt() const
{
	return ZE_ERA_BOTH;
}
