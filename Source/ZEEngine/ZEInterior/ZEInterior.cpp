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

#include "ZEError.h"
#include "ZEMath/ZETriangle.h"
#include "ZEMath/ZERay.h"
#include "ZEMath/ZEFrustum.h"
#include "ZEMath/ZEAngle.h"

#include "ZEInteriorResource.h"
#include "ZEInteriorRoom.h"
#include "ZEInteriorDoor.h"
#include "ZEInteriorHelper.h"
#include "ZECore/ZEConsole.h"
#include "ZEPhysics/ZEPhysicalMesh.h"
#include "ZERenderer/ZERNView.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRBuffer.h"
#include "ZERenderer/ZERNRenderParameters.h"

void ZEInterior::LoadInteriorResource()
{
	if (InteriorResource == NULL)
		return;

	ZEAABBox TempBoundingBox(ZEVector3(FLT_MAX), ZEVector3(FLT_MIN));

	ZESize RoomCount = InteriorResource->GetRooms().GetCount();
	Rooms.SetCount(RoomCount);
	for (ZESize I = 0; I < RoomCount; I++)
	{
		Rooms[I] = ZEInteriorRoom::CreateInstance();
		Rooms[I]->Initialize(this, (ZEInteriorResourceRoom*)&InteriorResource->GetRooms()[I]);
		ZEAABBox::Combine(TempBoundingBox, TempBoundingBox, Rooms[I]->GetBoundingBox());
	}

	this->SetBoundingBox(TempBoundingBox);

	ZESize DoorCount = InteriorResource->GetDoors().GetCount();
	Doors.SetCount(DoorCount);
	for (ZESize I = 0; I < DoorCount; I++)
	{
		Doors[I] = ZEInteriorDoor::CreateInstance();
		Doors[I]->Initialize(this, &InteriorResource->GetDoors()[I]);
	}

	ZESize HelperCount = InteriorResource->GetHelpers().GetCount();
	Helpers.SetCount(HelperCount);
	for (ZESize I = 0; I < HelperCount; I++)
	{
		Helpers[I] = ZEInteriorHelper::CreateInstance();
		Helpers[I]->Initialize(this, &InteriorResource->GetHelpers()[I]);
	}
}

ZEInterior::ZEInterior()
{
	InteriorResource = NULL;
	CullMode = ZE_ICM_NONE;

	SetEntityFlags(ZE_EF_RENDERABLE |  ZE_EF_CULLABLE);
}

ZEInterior::~ZEInterior()
{
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

ZEInteriorRoom* ZEInterior::GetRoom(const ZEString& Name)
{
	ZESize RoomCount = Rooms.GetCount();
	for (ZESize I = 0; I < RoomCount; I++)
		if (strcmp(Rooms[I]->GetName(), Name) == 0)
			return Rooms[I];

	return NULL;
}

ZEInteriorDoor* ZEInterior::GetDoor(const ZEString& Name)
{
	ZESize DoorCount = Doors.GetCount();
	for (ZESize I = 0; I < DoorCount; I++)
		if (strcmp(Doors[I]->GetName(), Name) == 0)
			return Doors[I];

	return NULL;
}

ZEInteriorHelper* ZEInterior::GetHelper(const ZEString& Name)
{
	ZESize HelperCount = Helpers.GetCount();
	for (ZESize I = 0; I < HelperCount; I++)
		if (strcmp(Helpers[I]->GetName(), Name) == 0)
			return Helpers[I];

	return NULL;
}

ZEEntityResult ZEInterior::LoadInternal()
{
	ZE_ENTITY_LOAD_CHAIN(ZEEntity);
	LoadInteriorResource();
	return ZE_ER_DONE;
}

ZEEntityResult ZEInterior::UnloadInternal()
{
	for (ZESize I = 0; I < Rooms.GetCount(); I++)
		Rooms[I]->Deinitialize();
	Rooms.Clear();

	for (ZESize I = 0; I < Doors.GetCount(); I++)
		Doors[I]->Deinitialize();
	Doors.Clear();

	for (ZESize I = 0; I < Helpers.GetCount(); I++)
		Helpers[I]->Deinitialize();
	Helpers.Clear();


	ZE_ENTITY_INITIALIZE_CHAIN(ZEEntity);
	return ZE_ER_DONE;
}

void ZEInterior::SetInteriorFile(const ZEString& InteriorFile)
{
	ZEInteriorResource* InteriorResource = ZEInteriorResource::LoadSharedResource(InteriorFile);

	if (InteriorResource == NULL)
	{
		zeError("Can not load interior file. File Name : \"%s\"", InteriorFile);
		return;
	}

	SetInteriorResource(InteriorResource);
}

const ZEString& ZEInterior::GetInteriorFile() const
{
	if (InteriorResource != NULL)
		return InteriorResource->GetFileName();
	else
		return ZEString::Empty;
}

void ZEInterior::SetInteriorResource(const ZEInteriorResource* InteriorResource)
{
	if (this->InteriorResource != NULL)
		((ZEInteriorResource*)this->InteriorResource)->Release();

	InteriorResource->AddReferance();

	this->InteriorResource = InteriorResource;

	if (IsLoaded())
		Reload();
}

const ZEInteriorResource* ZEInterior::GetInteriorResource() const
{
	return InteriorResource;
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

	return Index % Count;
}

static void IntersectionTest(ZEVector3* IntersectedPoints, ZESize& IntersectedPointCount, const ZEPlane& Plane, const ZEVector3* Points, ZESize PointCount)
{
	IntersectedPointCount = 0;
	if (PointCount == 0)
		return;

	bool CurrentResult = ZEPlane::TestHalfSpace(Plane, Points[0]) != ZE_HS_NEGATIVE_SIDE;
	for (ZESize I = 0; I < PointCount; I++)
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
		IntersectionTest(PointsTemp, PointTempCount,	Frustum->GetPlane(ZE_FP_LEFT),		Points, PointsCount);
		IntersectionTest(Points, PointsCount,			Frustum->GetPlane(ZE_FP_RIGHT),		PointsTemp, PointTempCount);
		IntersectionTest(PointsTemp, PointTempCount,	Frustum->GetPlane(ZE_FP_BOTTOM),		Points, PointsCount);
		IntersectionTest(Points, PointsCount,			Frustum->GetPlane(ZE_FP_TOP),		PointsTemp, PointTempCount);

		if (PointsCount == 0)
			return false;

		float TempDotProduct = 0.0f;
		ZEPlane VerticalPlane, HorizontalPlane;

 		float LeftDotProduct = -1.0f; float RightDotProduct = -1.0f;
 		float TopDotProduct = -1.0f; float BottomDotProduct = -1.0f;
		ZEPlane LeftPlane, RightPlane, TopPlane, BottomPlane;

		for (ZESize I = 0; I < PointsCount; I++)
		{
			ZEPlane::Create(VerticalPlane, Frustum->GetPosition(), Points[I], Points[I] - Frustum->GetUp());

			TempDotProduct = ZEVector3::DotProduct(Frustum->GetPlane(ZE_FP_LEFT).n, VerticalPlane.n);

			if (TempDotProduct > LeftDotProduct)
			{
				LeftDotProduct = TempDotProduct;
				LeftPlane.n = VerticalPlane.n;
				LeftPlane.p = Frustum->GetPosition();
			}

			TempDotProduct = ZEVector3::DotProduct(Frustum->GetPlane(ZE_FP_RIGHT).n, -VerticalPlane.n);

			if (TempDotProduct > RightDotProduct)
			{
				RightDotProduct = TempDotProduct;
				RightPlane.n = -VerticalPlane.n;
				RightPlane.p = Frustum->GetPosition();
			}

			ZEPlane::Create(HorizontalPlane, Frustum->GetPosition(), Points[I], Points[I] - Frustum->GetRight());

			TempDotProduct = ZEVector3::DotProduct(Frustum->GetPlane(ZE_FP_TOP).n, HorizontalPlane.n);

			if (TempDotProduct > TopDotProduct)
			{
				TopDotProduct = TempDotProduct;
				TopPlane.n = HorizontalPlane.n;
				TopPlane.p = Frustum->GetPosition();
			}

			TempDotProduct = ZEVector3::DotProduct(Frustum->GetPlane(ZE_FP_BOTTOM).n, -HorizontalPlane.n);

			if (TempDotProduct > BottomDotProduct)
			{
				BottomDotProduct = TempDotProduct;
				BottomPlane.n = -HorizontalPlane.n;
				BottomPlane.p = Frustum->GetPosition();
			}
		}

		ZEViewFrustum::Create(NewViewVolume, LeftPlane, RightPlane, BottomPlane, TopPlane, Frustum->GetPlane(ZE_FP_NEAR), Frustum->GetPlane(ZE_FP_FAR));
		return true;
	}

	return false;
}

void ZEInterior::CullRoom(ZEInteriorDoor* Door, const ZERNPreRenderParameters* Parameters, ZEViewVolume* ViewVolume)
{
	if(!Door->GetOpened())
		return;
	
	if (!ViewVolume->IntersectionTest(Door->GetRectangle()))
		return;

	ZEInteriorRoom** DoorRooms = Door->GetRooms();
	for (ZEInt I = 0; I < 2; I++)
	{
		if(DoorRooms[I]->CullPass == true)
			continue;

		ZEViewFrustum NewViewVolume;
		GenerateViewVolume(NewViewVolume, Door, ViewVolume);

		DoorRooms[I]->CullPass = true;
		DoorRooms[I]->PreRender(Parameters);

		const ZEArray<ZEInteriorDoor*>& NextDoors = DoorRooms[I]->GetDoors();
		ZESize NextDoorCount = NextDoors.GetCount();
		for (ZESize J = 0; J < NextDoorCount; J++)
		{
			if(NextDoors[J] == Door)
				continue;

			CullRoom(NextDoors[J], Parameters, (ZEViewVolume*)&NewViewVolume);
		}
	}
}

void ZEInterior::CullRooms(const ZERNPreRenderParameters* Parameters)
{
	ZESize RoomCount = Rooms.GetCount();

	if (CullMode == ZE_ICM_NONE)
	{
		for (ZESize I = 0; I < RoomCount; I++)
			Rooms[I]->PreRender(Parameters);
	}
	else
	{
		bool NoClip = true;

		if (Parameters->View->ViewVolume->GetViewVolumeType() == ZE_VVT_FRUSTUM)
		{
			ZEViewFrustum* Frustum = (ZEViewFrustum*)Parameters->View->ViewVolume;
			ZEVector3 FrustumPosition = Frustum->GetPosition();

			for (ZESize I = 0; I < RoomCount; I++)
			{
				if (CullMode == ZE_ICM_VIEW)
					continue;

				if (ZEAABBox::IntersectionTest(Rooms[I]->GetWorldBoundingBox(), FrustumPosition))
				{
					NoClip = false;
					Rooms[I]->CullPass = true;
					Rooms[I]->PreRender(Parameters);

					const ZEArray<ZEInteriorDoor*>& Doors = Rooms[I]->GetDoors();
					ZESize DoorCount = Doors.GetCount();
					for(ZESize J = 0; J < DoorCount; J++)
						CullRoom(Doors[J], Parameters, Frustum);
				}
			}
		}

		if (NoClip)
		{
			for (ZESize I = 0; I < RoomCount; I++)
			{
				if (Parameters->View->ViewVolume->IntersectionTest(Rooms[I]->GetWorldBoundingBox()))
					Rooms[I]->PreRender(Parameters);
			}
		}

		for (ZESize I = 0; I < RoomCount; I++)
		{
			if (Rooms[I]->IsPersistentDraw)
			{
				if (!Rooms[I]->IsDrawn)
					Rooms[I]->PreRender(Parameters);
			}
		}
	}
}

void ZEInterior::ParentTransformChanged()
{
	ZEEntity::ParentTransformChanged();

	for (ZESize I = 0; I < Rooms.GetCount(); I++)
		Rooms[I]->ParentTransformChanged();

	ZESize DoorCount = Doors.GetCount();
	for (ZESize I = 0; I < DoorCount; I++)
		Doors[I]->TransformChanged = true;
}

void ZEInterior::SetCullMode(ZEInteriorCullMode Value)
{
	CullMode = Value;
}

bool ZEInterior::PreRender(const ZERNPreRenderParameters* Parameters)
{
	if (!GetVisible())
		return false;

	ZESize RoomCount = Rooms.GetCount();
	for (ZESize I = 0; I < RoomCount; I++)
	{
		Rooms[I]->CullPass = false;
		Rooms[I]->IsDrawn = false;
	}

	CullRooms(Parameters);

	return true;
}

void ZEInterior::RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters)
{
	ZESize RoomCount = Rooms.GetCount();
	for (ZESize I = 0; I < RoomCount; I++)
	{
		if (!Parameters.Filter(Rooms[I]) || Report.CheckDone())
			return;

		Rooms[I]->RayCast(Report, Parameters);
	}
}

ZEInteriorCullMode ZEInterior::GetCullMode() const
{
	return CullMode;
}
