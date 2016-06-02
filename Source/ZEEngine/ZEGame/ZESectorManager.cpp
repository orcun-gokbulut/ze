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
#include "ZESector.h"
#include "ZESectorSelector.h"
#include "ZEMath\ZEMath.h"

void ZESectorManager::UpdateTransformation(ZEGeographicEntity* Entity)
{
	if (OriginSector == NULL)
		return;

	ZEMatrix4x4d EntityLocalTransform;
	ZEMatrix4x4d::Multiply(EntityLocalTransform, OriginSector->GetInvGeographicTransform(), Entity->GetGeographicTransform());
	Entity->SetPosition(EntityLocalTransform.GetTranslation().ToVector3());
	Entity->SetRotation(EntityLocalTransform.GetRotation());
	Entity->SetScale(EntityLocalTransform.GetScale().ToVector3());
}

void ZESectorManager::UpdateTransformations()
{
	ZEVector3d GeographicRelativeOrigin = ZEVector3d::Zero;

	for (ZESize I = 0; I < Selectors.GetCount(); I++)
		GeographicRelativeOrigin += Selectors[I]->GetGeographicPosition();

	GeographicRelativeOrigin = GeographicRelativeOrigin / Selectors.GetCount();

	ZESector* ResultSector = GetSector(GeographicRelativeOrigin, true);

	if (OriginSector == ResultSector)
		return;
	
	OriginSector = ResultSector;

	for (ZESize I = 0; I < Sectors.GetCount(); I++)
		UpdateTransformation(Sectors[I]);

	for (ZESize I = 0; I < Selectors.GetCount(); I++)
		UpdateTransformation(Selectors[I]);

	for (ZESize I = 0; I < GeographicEntities.GetCount(); I++)
		UpdateTransformation(GeographicEntities[I]);
	
}

void ZESectorManager::UpdateActiveSectors()
{
	ZESector* CurrentSelectedSector = NULL;

	for (ZESize I = 0; I < Selectors.GetCount(); I++)
	{
		CurrentSelectedSector = GetSector(Selectors[I]->GetGeographicPosition(), true);

		if (CurrentSelectedSector == NULL)
			continue;

		for (ZESize J = 0; J < Sectors.GetCount(); J++)
		{
			bool Result = Sectors[J]->CheckAdjacency(CurrentSelectedSector, CurrentSelectedSector->GetAdjacencyDepth());

			if (!Sectors[J]->GetEnabled())
				UpdateTransformation(Sectors[J]);

			Sectors[J]->SetEnabled(Result);
			Sectors[J]->SetVisible(Result);
		}
	}
}

ZESectorManager::ZESectorManager()
{
	OriginSector = NULL;
}

bool ZESectorManager::InitializeSelf()
{
	if (!ZEEntity::InitializeSelf())
		return false;

	OriginSector = NULL;
	Sectors.Clear();
	Selectors.Clear();
	GeographicEntities.Clear();

	const ZEArray<ZEEntity*>& ChildEntities = GetChildEntities();
	ZEEntity* CurrentEntity = NULL;

	for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
	{
		CurrentEntity = ChildEntities[I];

		if (ZEClass::IsDerivedFrom(ZESector::Class(), CurrentEntity->GetClass()))
			Sectors.Add((ZESector*)CurrentEntity);
		else if (ZEClass::IsDerivedFrom(ZESectorSelector::Class(), CurrentEntity->GetClass()))
			Selectors.Add((ZESectorSelector*)CurrentEntity);
		else if (ZEClass::IsDerivedFrom(ZEGeographicEntity::Class(), CurrentEntity->GetClass()))
			GeographicEntities.Add((ZEGeographicEntity*)CurrentEntity);
		else
			continue;
	}

	return true;
}

const ZEArray<ZESector*>& ZESectorManager::GetSectors() const
{
	return Sectors;
}

ZEArray<ZESector*> ZESectorManager::GetSectors(const ZEVector3d& Position) const
{
	ZESector* CurrentSector = NULL;
	ZEArray<ZESector*> Results;
	ZEVector3 LocalizedTestPosition;

	for (ZESize I = 0; I < Sectors.GetCount(); I++)
	{
		CurrentSector = Sectors[I];

		LocalizedTestPosition = (Position - CurrentSector->GetGeographicPosition()).ToVector3();

		if (ZEAABBox::IntersectionTest(CurrentSector->GetBoundingBox(), LocalizedTestPosition))
			Results.Add(CurrentSector);
	}

	return Results;
}

ZESector* ZESectorManager::GetSector(const ZEGUID& Id) const
{
	for (ZESize I = 0; I < Sectors.GetCount(); I++)
		if (((ZESector*)Sectors[I])->GetGUID() == Id)
			return (ZESector*)Sectors[I];

	return NULL;
}

ZESector* ZESectorManager::GetSector(const ZEVector3d& Position, bool Proximity) const
{
	ZESector* ResultSector = NULL;
	ZESector* ClosestSector = NULL;
	ZESector* CurrentSector = NULL;
	double DistanceToResult = 0.0;
	double DistanceToCurrent = 0.0;
	double DistanceToClosest = ZE_DOUBLE_MAX;
	ZEVector3 LocalizedTestPosition;

	for (ZESize I = 0; I < Sectors.GetCount(); I++)
	{
		CurrentSector = Sectors[I];

		LocalizedTestPosition = (Position - CurrentSector->GetGeographicPosition()).ToVector3();

		DistanceToCurrent = ZEVector3d::DistanceSquare(CurrentSector->GetGeographicPosition(), Position);

		if (DistanceToCurrent < DistanceToClosest)
		{
			DistanceToClosest = DistanceToCurrent;
			ClosestSector = CurrentSector;
		}

		if (ZEAABBox::IntersectionTest(CurrentSector->GetBoundingBox(), LocalizedTestPosition))
		{
			if (ResultSector != NULL)
			{
				DistanceToResult = ZEVector3d::DistanceSquare(ResultSector->GetGeographicPosition(), Position);

				if (DistanceToCurrent < DistanceToResult)
					ResultSector = CurrentSector;
			}
			else
			{
				ResultSector = CurrentSector;
			}
		}
	}

//	zeDebugLog("Closest Sector is not current Sector.", (ResultSector != NULL) && (ResultSector != ClosestSector));

	if (ResultSector == NULL && Proximity)
		ResultSector = ClosestSector;

	return ResultSector;
}

ZESector* ZESectorManager::GetOriginSector()
{
	return OriginSector;
}

bool ZESectorManager::AddSector(ZESector* Sector)
{
	if (!AddChildEntity(Sector))
		return false;

	Sectors.Add(Sector);

	return true;
}

void ZESectorManager::RemoveSector(ZESector* Sector)
{
	Sectors.RemoveValue(Sector);

	RemoveChildEntity(Sector);
}

const ZEArray<ZESectorSelector*>& ZESectorManager::GetSelectors() const
{
	return Selectors;
}

bool ZESectorManager::AddSelector(ZESectorSelector* Selector)
{
	if (!AddChildEntity(Selector))
		return false;

	Selectors.Add(Selector);

	return true;
}

void ZESectorManager::RemoveSelector(ZESectorSelector* Selector)
{
	Selectors.RemoveValue(Selector);

	RemoveChildEntity(Selector);
}

const ZEArray<ZEGeographicEntity*>& ZESectorManager::GetGeographicEntities() const
{
	return GeographicEntities;
}

bool ZESectorManager::AddGeographicEntity(ZEGeographicEntity* Entity)
{
	if (!AddChildEntity(Entity))
		return false;

	GeographicEntities.Add(Entity);

	return true;
}

void ZESectorManager::RemoveGeographicEntity(ZEGeographicEntity* Entity)
{
	GeographicEntities.RemoveValue(Entity);

	RemoveChildEntity(Entity);
}

void ZESectorManager::Tick(float Time)
{
	UpdateTransformations();

	UpdateActiveSectors();
}

ZESectorManager* ZESectorManager::CreateInstance()
{
	return new ZESectorManager();
}
