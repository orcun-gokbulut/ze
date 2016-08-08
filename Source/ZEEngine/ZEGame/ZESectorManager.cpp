//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESectorManager.cpp
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

#include "ZESectorManager.h"
#include "ZESectorSelector.h"
#include "ZEGeographicEntity.h"
#include "ZEMath\ZEMath.h"

#define ZE_GEDF_LOCAL_TRANSFORM 0x0004

void ZESectorManager::UpdateTransformation(ZEGeographicEntity* Entity)
{
	if (OriginSector == NULL)
		return;

	if (Entity->GeographicEntityDirtyFlags.GetFlags(ZE_GEDF_LOCAL_TRANSFORM))
	{
		ZEMatrix4x4d EntityLocalTransform;
		ZEMatrix4x4d::Multiply(EntityLocalTransform, OriginSector->GetInvGeographicTransform(), Entity->GetGeographicTransform());
		Entity->SetPosition(EntityLocalTransform.GetTranslation().ToVector3());
		Entity->SetRotation(EntityLocalTransform.GetRotation());
		Entity->SetScale(EntityLocalTransform.GetScale().ToVector3());
		Entity->GeographicEntityDirtyFlags.UnraiseFlags(ZE_GEDF_LOCAL_TRANSFORM);
	}
}

void ZESectorManager::UpdateTransformations()
{
	ZEVector3d GeographicRelativeOrigin = ZEVector3d::Zero;

	ze_for_each(Selector, Selectors)
		GeographicRelativeOrigin += Selector->GetGeographicPosition();

	GeographicRelativeOrigin = GeographicRelativeOrigin / (double)Selectors.GetCount();

	ZESector* ResultSector = GetSector(GeographicRelativeOrigin, true);

	if (OriginSector == ResultSector)
		return;
	
	OriginSector = ResultSector;

	ze_for_each(Sector, Sectors)
		UpdateTransformation(Sector.GetPointer());

	ze_for_each(Selector, Selectors)
		UpdateTransformation(Selector.GetPointer());

	ze_for_each(Entity, GeographicEntities)
		UpdateTransformation(Entity.GetPointer());
	
}

void ZESectorManager::UpdateActiveSectors()
{
	ZESector* CurrentSelectedSector = NULL;

	ze_for_each(Selector, Selectors)
	{
		CurrentSelectedSector = GetSector(Selector->GetGeographicPosition(), true);

		if (CurrentSelectedSector == NULL)
			continue;

		ze_for_each(Sector, Sectors)
		{
			ZESector* Temp = static_cast<ZESector*>(Sector.GetPointer());
			bool Result = Temp->CheckAdjacency(CurrentSelectedSector, CurrentSelectedSector->GetAdjacencyDepth());

			if (!Temp->GetEnabled()) //Investigate Possible bug
				UpdateTransformation(Temp);

			Temp->SetEnabled(Result);
			Temp->SetVisible(Result);
		}
	}
}

ZESectorManager::ZESectorManager()
{
	OriginSector = NULL;
}

const ZEList2<ZEGeographicEntity>& ZESectorManager::GetSectors() const
{
	return Sectors;
}

ZEArray<ZESector*> ZESectorManager::GetSectors(const ZEVector3d& Position) const
{
	ZEArray<ZESector*> Results;
	ZEVector3 LocalizedTestPosition;

	ze_for_each(Sector, Sectors)
	{
		LocalizedTestPosition = (Position - Sector->GetGeographicPosition()).ToVector3();

		if (ZEAABBox::IntersectionTest(Sector->GetBoundingBox(), LocalizedTestPosition))
			Results.Add(static_cast<ZESector*>(Sector.GetPointer()));
	}

	return Results;
}

ZESector* ZESectorManager::GetSector(const ZEGUID& Id) const
{
	ze_for_each(Sector, Sectors)
	{
		ZESector* Temp = static_cast<ZESector*>(Sector.GetPointer());
		if (Temp->GetGUID() == Id)
			return Temp;
	}

	return NULL;
}

ZESector* ZESectorManager::GetSector(const ZEVector3d& Position, bool Proximity) const
{
	ZEGeographicEntity* ResultSector = NULL;
	ZEGeographicEntity* ClosestSector = NULL;
	double DistanceToResult = 0.0;
	double DistanceToCurrent = 0.0;
	double DistanceToClosest = ZE_DOUBLE_MAX;
	ZEVector3 LocalizedTestPosition;

	ze_for_each(Sector, Sectors)
	{
		LocalizedTestPosition = (Position - Sector->GetGeographicPosition()).ToVector3();

		DistanceToCurrent = ZEVector3d::DistanceSquare(Sector->GetGeographicPosition(), Position);

		if (DistanceToCurrent < DistanceToClosest)
		{
			DistanceToClosest = DistanceToCurrent;
			ClosestSector = Sector.GetPointer();
		}

		if (ZEAABBox::IntersectionTest(Sector->GetBoundingBox(), LocalizedTestPosition))
		{
			if (ResultSector != NULL)
			{
				DistanceToResult = ZEVector3d::DistanceSquare(ResultSector->GetGeographicPosition(), Position);

				if (DistanceToCurrent < DistanceToResult)
					ResultSector = Sector.GetPointer();
			}
			else
			{
				ResultSector = Sector.GetPointer();
			}
		}
	}

//	zeDebugLog("Closest Sector is not current Sector.", (ResultSector != NULL) && (ResultSector != ClosestSector));

	if (ResultSector == NULL && Proximity)
		ResultSector = ClosestSector;

	return static_cast<ZESector*>(ResultSector);
}

ZESector* ZESectorManager::GetOriginSector()
{
	return OriginSector;
}

bool ZESectorManager::AddSector(ZESector* Sector)
{
	zeCheckError(Sector == NULL, false, "Cannot add sector. Sector is NULL.");
	zeCheckError(Sectors.Exists(&Sector->GeoLink), false, 
		"Can not add sector. Sector already registered to this Manager. Sector Name: \"%s\".", Sector->GetName().ToCString());

	Sectors.AddEnd(&Sector->GeoLink);
	Sector->SetManager(this);

	return true;
}

void ZESectorManager::RemoveSector(ZESector* Sector)
{
	zeCheckError(Sector == NULL, ZE_VOID, "Cannot remove sector. Sector is NULL.");
	zeCheckError(!Sectors.Exists(&Sector->GeoLink), ZE_VOID, 
		"Can not remove sector. Sector is not registered to this manager. Sector Name: \"%s\".", Sector->GetName().ToCString());

	Sector->SetManager(NULL);
	Sectors.Remove(&Sector->GeoLink);
}

const ZEList2<ZEGeographicEntity>& ZESectorManager::GetSelectors() const
{
	return Selectors;
}

bool ZESectorManager::AddSelector(ZESectorSelector* Selector)
{
	zeCheckError(Selector == NULL, false, "Cannot add selector. Selector is NULL.");
	zeCheckError(Selectors.Exists(&Selector->GeoLink), false, 
		"Can not add selector. Selector already registered to this manager. Selector Name: \"%s\".", Selector->GetName().ToCString());

	Selectors.AddEnd(&Selector->GeoLink);

	return true;
}

void ZESectorManager::RemoveSelector(ZESectorSelector* Selector)
{
	zeCheckError(Selector == NULL, ZE_VOID, "Cannot remove selector. Selector is NULL.");
	zeCheckError(!Selectors.Exists(&Selector->GeoLink), ZE_VOID, 
		"Can not remove selector. Selector is not registered to this manager. Selector Name: \"%s\".", Selector->GetName().ToCString());

	Selectors.Remove(&Selector->GeoLink);
}

const ZEList2<ZEGeographicEntity>& ZESectorManager::GetGeographicEntities() const
{
	return GeographicEntities;
}

bool ZESectorManager::AddGeographicEntity(ZEGeographicEntity* Entity)
{
	zeCheckError(Entity == NULL, false, "Cannot add entity. Entity is NULL.");
	zeCheckError(GeographicEntities.Exists(&Entity->GeoLink), false, 
		"Can not add entity. Entity already registered to this manager. Entity Name: \"%s\".", Entity->GetName().ToCString());

	GeographicEntities.AddEnd(&Entity->GeoLink);

	return true;
}

void ZESectorManager::RemoveGeographicEntity(ZEGeographicEntity* Entity)
{
	zeCheckError(Entity == NULL, ZE_VOID, "Cannot remove entity. Entity is NULL.");
	zeCheckError(!GeographicEntities.Exists(&Entity->GeoLink), ZE_VOID, 
		"Can not remove entity. Entity is not registered to this manager. Entity Name: \"%s\".", Entity->GetName().ToCString());

	GeographicEntities.Remove(&Entity->GeoLink);
}

void ZESectorManager::Process(float Time)
{
	UpdateTransformations();

	UpdateActiveSectors();
}

ZESectorManager* ZESectorManager::CreateInstance()
{
	return new ZESectorManager();
}
