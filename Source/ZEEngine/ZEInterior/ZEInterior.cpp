//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEInterior.cpp
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

#include "ZEInterior.h"
#include "ZEInteriorResource.h"
#include "ZEInteriorRoom.h"
#include "ZEInteriorDoor.h"
#include "ZEInteriorHelper.h"
#include "ZEError.h"
#include "ZECore/ZEConsole.h"
#include "ZEGame/ZEDrawParameters.h"
#include "ZEMath/ZETriangle.h"
#include "ZEMath/ZERay.h"
#include "ZEMath/ZEViewFrustum.h"
#include "ZEMath/ZEAngle.h"
#include "ZEPhysics/ZEPhysicalMesh.h"
#include "ZEGame/ZEEntityProvider.h"

ZE_META_REGISTER_CLASS(ZEEntityProvider, ZEInterior);

ZEDrawFlags ZEInterior::GetDrawFlags() const
{
	return ZE_DF_DRAW | ZE_DF_LIGHT_RECIVER;
}

void ZEInterior::LoadInteriorResource()
{
	for (ZESize I = 0; I < Rooms.GetCount(); I++)
		Rooms[I]->Deinitialize();

	Rooms.SetCount(0);

	for (ZESize I = 0; I < Doors.GetCount(); I++)
		Doors[I]->Deinitialize();

	Doors.SetCount(0);

	for (ZESize I = 0; I < Helpers.GetCount(); I++)
		Helpers[I]->Deinitialize();

	Helpers.SetCount(0);

	if (InteriorResource == NULL)
		return;

	Rooms.SetCount(InteriorResource->GetRooms().GetCount());
	for (ZESize I = 0; I < Rooms.GetCount(); I++)
	{
		Rooms[I] = ZEInteriorRoom::CreateInstance();
		Rooms[I]->Initialize(this, (ZEInteriorResourceRoom*)&InteriorResource->GetRooms()[I]);
	}

	Doors.SetCount(InteriorResource->GetDoors().GetCount());
	for (ZESize I = 0; I < Doors.GetCount(); I++)
	{
		Doors[I] = ZEInteriorDoor::CreateInstance();
		Doors[I]->Initialize(this, &InteriorResource->GetDoors()[I]);
	}

	Helpers.SetCount(InteriorResource->GetHelpers().GetCount());
	for (ZESize I = 0; I < Helpers.GetCount(); I++)
	{
		Helpers[I] = ZEInteriorHelper::CreateInstance();
		Helpers[I]->Initialize(this, &InteriorResource->GetHelpers()[I]);
	}
}

ZEInterior::ZEInterior()
{
	InteriorResource = NULL;
	CullMode = ZE_ICM_FULL;
	memset(&Statistics, 0, sizeof(ZEInteriorStatistics));
}

ZEInterior::~ZEInterior()
{
	if (InteriorResource != NULL)
		((ZEInteriorResource*)InteriorResource)->Release();
}

const ZEArray<ZEInteriorRoom*>& ZEInterior::GetRooms()
{
	return Rooms;
}

const ZEArray<ZEInteriorDoor*>& ZEInterior::GetDoors()
{
	return Doors;
}

const ZEArray<ZEInteriorHelper*>& ZEInterior::GetHelpers()
{
	return Helpers;
}

ZEInteriorRoom* ZEInterior::GetRoom(const char* Name)
{
	for (ZESize I = 0; I < Rooms.GetCount(); I++)
		if (strcmp(Rooms[I]->GetName(), Name) == 0)
			return Rooms[I];

	return NULL;
}

ZEInteriorDoor* ZEInterior::GetDoor(const char* Name)
{
	for (ZESize I = 0; I < Doors.GetCount(); I++)
		if (strcmp(Doors[I]->GetName(), Name) == 0)
			return Doors[I];

	return NULL;
}

ZEInteriorHelper* ZEInterior::GetHelper(const char* Name)
{
	for (ZESize I = 0; I < Helpers.GetCount(); I++)
		if (strcmp(Helpers[I]->GetName(), Name) == 0)
			return Helpers[I];

	return NULL;
}
const ZEInteriorStatistics& ZEInterior::GetStatistics() const
{
	return Statistics;
}

bool ZEInterior::InitializeSelf()
{
	if (!ZEEntity::InitializeSelf())
		return false;

	LoadInteriorResource();

	return true;
}

bool ZEInterior::DeinitializeSelf()
{
	return ZEEntity::DeinitializeSelf();
}

void ZEInterior::SetInteriorFile(const char* InteriorFile)
{
	ZEInteriorResource* InteriorResource = ZEInteriorResource::LoadSharedResource(InteriorFile);

	if (InteriorResource == NULL)
	{
		zeError("Can not load interior file. File Name : \"%s\"", InteriorFile);
		return;
	}

	SetInteriorResource(InteriorResource);
}

const char* ZEInterior::GetInteriorFile() const
{
	if (InteriorResource != NULL)
		return InteriorResource->GetFileName();
	else
		return "";
}

void ZEInterior::SetInteriorResource(const ZEInteriorResource* InteriorResource)
{
	if (this->InteriorResource != NULL)
		((ZEInteriorResource*)this->InteriorResource)->Release();

	InteriorResource->AddReferance();

	this->InteriorResource = InteriorResource;

	if (IsInitialized())
		LoadInteriorResource();
}

const ZEInteriorResource* ZEInterior::GetInteriorResource()
{
	return InteriorResource;
}

void ZEInterior::Draw(ZEDrawParameters* DrawParameters)
{
	if (!GetVisible())
		return;

	if (DrawParameters->Pass == ZE_RP_COLOR)
		memset(&Statistics, 0, sizeof(ZEInteriorStatistics));

	for (size_t I = 0; I < Rooms.GetCount(); I++)
	{
		Rooms[I]->CullPass = false;
		Rooms[I]->IsDrawn = false;
	}

	CullRooms(DrawParameters);

	if (DrawParameters->Pass == ZE_RP_COLOR)
	{
		Statistics.TotalDoorCount = GetDoors().GetCount();

		DrawParameters->Statistics.InteriorStatistics.TotalRoomCount += Statistics.TotalRoomCount;
		DrawParameters->Statistics.InteriorStatistics.CulledRoomCount += Statistics.CulledRoomCount;
		DrawParameters->Statistics.InteriorStatistics.DrawedRoomCount += Statistics.DrawedRoomCount;
		DrawParameters->Statistics.InteriorStatistics.TotalDoorCount += Statistics.TotalDoorCount;
		DrawParameters->Statistics.InteriorStatistics.SeenDoorCount += Statistics.SeenDoorCount;
		DrawParameters->Statistics.InteriorStatistics.TotalInteriorPolygonCount += Statistics.TotalInteriorPolygonCount;
		DrawParameters->Statistics.InteriorStatistics.CulledInteriorPolygonCount += Statistics.CulledInteriorPolygonCount;
		DrawParameters->Statistics.InteriorStatistics.DrawedInteriorPolygonCount += Statistics.DrawedInteriorPolygonCount;
	}
}

ZEInterior* ZEInterior::CreateInstance()
{
	return new ZEInterior();
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

bool ZEInterior::GenerateViewVolume(ZEViewFrustum& NewViewVolume, ZEInteriorDoor* Door, const ZEViewVolume* OldViewVolume)
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

void ZEInterior::CullRoom(ZEInteriorDoor* Door, ZEDrawParameters* DrawParameters, ZEViewVolume* ViewVolume)
{
	if(!Door->GetOpen())
		return;
	
	if (ViewVolume->CullTest(Door->GetRectangle()))
		return;

	if (DrawParameters->Pass == ZE_RP_COLOR)
		Statistics.SeenDoorCount++;

	ZEInteriorRoom** DoorRooms = Door->GetRooms();
	for (ZEInt I = 0; I < 2; I++)
	{
		if(DoorRooms[I]->CullPass == true)
			continue;

		ZEViewFrustum NewViewVolume;
		GenerateViewVolume(NewViewVolume, Door, ViewVolume);

		DoorRooms[I]->CullPass = true;
		DoorRooms[I]->Draw(DrawParameters);

		if (DrawParameters->Pass == ZE_RP_COLOR)
		{
			Statistics.DrawedRoomCount++;
			Statistics.DrawedInteriorPolygonCount += DoorRooms[I]->GetPolygonCount();
		}

		const ZEArray<ZEInteriorDoor*>& NextDoors = DoorRooms[I]->GetDoors();

		for (size_t J = 0; J < NextDoors.GetCount(); J++)
		{
			if(NextDoors[J] == Door)
				continue;

			CullRoom(NextDoors[J], DrawParameters, (ZEViewVolume*)&NewViewVolume);
		}
	}
}

void ZEInterior::CullRooms(ZEDrawParameters* DrawParameters)
{
	if (CullMode == ZE_ICM_NONE)
	{
		for (ZESize I = 0; I < Rooms.GetCount(); I++)
		{
			Rooms[I]->Draw(DrawParameters);

			if (DrawParameters->Pass == ZE_RP_COLOR)
			{
				Statistics.TotalRoomCount++;
				Statistics.DrawedRoomCount++;
				Statistics.TotalInteriorPolygonCount += Rooms[I]->GetPolygonCount();
				Statistics.DrawedInteriorPolygonCount += Rooms[I]->GetPolygonCount();
			}
		}
	}
	else
	{
		bool NoClip = true;

		if (DrawParameters->ViewVolume->GetViewVolumeType() == ZE_VVT_FRUSTUM)
		{
			ZEViewFrustum* Frustum = (ZEViewFrustum*)DrawParameters->ViewVolume;
			ZEVector3 FrustumPosition = Frustum->GetPosition();

			for (size_t I = 0; I < Rooms.GetCount(); I++)
			{

				if (DrawParameters->Pass == ZE_RP_COLOR)
				{
					Statistics.TotalRoomCount++;
					Statistics.TotalInteriorPolygonCount += Rooms[I]->GetPolygonCount();
				}

				if (CullMode == ZE_ICM_VIEW)
					continue;

				if (ZEAABBox::IntersectionTest(Rooms[I]->GetWorldBoundingBox(), FrustumPosition))
				{
					NoClip = false;
					Rooms[I]->CullPass = true;
					Rooms[I]->Draw(DrawParameters);


					if (DrawParameters->Pass == ZE_RP_COLOR)
					{
						Statistics.DrawedRoomCount++;
						Statistics.DrawedInteriorPolygonCount += Rooms[I]->GetPolygonCount();
					}

					const ZEArray<ZEInteriorDoor*>& Doors = Rooms[I]->GetDoors();
					for(size_t J = 0; J < Doors.GetCount(); J++)
					{
						CullRoom(Doors[J], DrawParameters, Frustum);
					}
				}
			}
		}

		if (NoClip)
		{
			for (ZESize I = 0; I < Rooms.GetCount(); I++)
			{
				if (!DrawParameters->ViewVolume->CullTest(Rooms[I]->GetWorldBoundingBox()))
				{
					Rooms[I]->Draw(DrawParameters);


					if (DrawParameters->Pass == ZE_RP_COLOR)
					{
						Statistics.DrawedRoomCount++;
						Statistics.DrawedInteriorPolygonCount += Rooms[I]->GetPolygonCount();
					}
				}
			}
		}

		for (ZESize I = 0; I < Rooms.GetCount(); I++)
		{
			if (Rooms[I]->IsPersistentDraw)
			{
				if (!Rooms[I]->IsDrawn)
				{
					Rooms[I]->Draw(DrawParameters);


					if (DrawParameters->Pass == ZE_RP_COLOR)
					{
						Statistics.DrawedRoomCount++;
						Statistics.DrawedInteriorPolygonCount += Rooms[I]->GetPolygonCount();
					}

				}
			}
		}
	}

	if (DrawParameters->Pass == ZE_RP_COLOR)
	{
		Statistics.CulledRoomCount = Statistics.TotalRoomCount - Statistics.DrawedRoomCount;
		Statistics.CulledInteriorPolygonCount = Statistics.TotalInteriorPolygonCount - Statistics.DrawedInteriorPolygonCount;
	}
}

void ZEInterior::OnTransformChanged()
{
	for (ZESize I = 0; I < Rooms.GetCount(); I++)
		Rooms[I]->OnTransformChanged();

	for (ZESize I = 0; I < Doors.GetCount(); I++)
		Doors[I]->TransformChanged = true;

	ZEEntity::OnTransformChanged();
}

void ZEInterior::SetCullMode(ZEInteriorCullMode Value)
{
	CullMode = Value;
}

bool ZEInterior::RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters)
{
	bool Result = false;
	for (ZESize I = 0; I < Rooms.GetCount(); I++)
		Result |= Rooms[I]->RayCast(Report, Parameters);
	
	if (Result)
		Report.Entity = this;

	return Result;
}

ZEInteriorCullMode ZEInterior::GetCullMode() const
{
	return CullMode;
}

ZEEntityRunAt ZEInteriorDescription::GetRunAt() const
{
	return ZE_ERA_BOTH;
}
