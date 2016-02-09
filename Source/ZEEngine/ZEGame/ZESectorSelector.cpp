//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESectorSelector.cpp
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

#include "ZESectorSelector.h"
#include "ZEGame/ZESector.h"
#include "ZEGame/ZEScene.h"
#include "ZEGame/ZEGame.h"
#include "ZEMath/ZEMath.h"

void ZESectorSelector::UpdateSectors()
{
	if (!IsInitialized())
		return;

	if (IsProcessing)
		return;

	ZESector* TargetSector = DetermineSector(this);
	ZESector* OwnerSector = (ZESector*)GetOwner();

	if (TargetSector == NULL)
		return;

	if (TargetSector != OwnerSector)
	{
		IsProcessing = true;
		ZEVector3d CurrentGeographicPosition = GetGeographicPosition();
		ZEQuaternion CurrentGeographicRotation = GetGeographicRotation();
		ZEVector3d CurrentGeographicScale = GetGeographicScale();

		if (OwnerSector != NULL)
			OwnerSector->RemoveChildEntity(this);
		else
			ZEGame::GetInstance()->GetScene()->RemoveEntity(this);

		if (TargetSector != NULL)
			TargetSector->AddChildEntity(this);
		else
			ZEGame::GetInstance()->GetScene()->AddEntity(this);

		SetReferenceSector(TargetSector);
		SetGeographicPosition(CurrentGeographicPosition);
		SetGeographicRotation(CurrentGeographicRotation);
		SetGeographicScale(CurrentGeographicScale);
		IsProcessing = false;
	}
}

void ZESectorSelector::LocalTransformChanged()
{
	ZEEntity::LocalTransformChanged();
	UpdateSectors();
}

void ZESectorSelector::ParentTransformChanged()
{
	ZEEntity::ParentTransformChanged();
	UpdateSectors();
}

bool ZESectorSelector::SetOwner(ZEEntity* Owner)
{
	if (Owner != NULL)
		if (!ZEClass::IsDerivedFrom(ZESector::Class(), Owner->GetClass()))
			return false;

	return ZEGeographicEntity::SetOwner(Owner);
}

ZESectorSelector::ZESectorSelector()
{
	IsProcessing = false;
}

ZESector* ZESectorSelector::DetermineSector(ZEGeographicEntity* Entity)
{
	if (Entity == NULL)
		return NULL;

	if (ZEClass::IsDerivedFrom(ZESector::Class(), Entity->GetClass()))
		return NULL;

	ZESector* ResultSector = NULL;
	ZESector* ClosestSector = NULL;
	ZESector* CurrentSector = NULL;
	double DistanceToResult = 0.0;
	double DistanceToCurrent = 0.0;
	double DistanceToClosest = ZE_DOUBLE_MAX;
	ZEVector3 LocalizedPosition;

	ZEArray<ZEEntity*> Sectors = ZEScene::GetInstance()->GetEntities(ZESector::Class());

	for (ZESize I = 0; I < Sectors.GetCount(); I++)
	{
		CurrentSector = (ZESector*)Sectors[I];
		LocalizedPosition = (CurrentSector->GetInvGeographicTransform() * Entity->GetGeographicPosition()).ToVector3();

		DistanceToCurrent = ZEVector3d::DistanceSquare(CurrentSector->GetGeographicPosition(), Entity->GetGeographicPosition());

		if (DistanceToCurrent < DistanceToClosest)
		{
			DistanceToClosest = DistanceToCurrent;
			ClosestSector = CurrentSector;
		}

		if (ZEAABBox::IntersectionTest(CurrentSector->GetBoundingBox(), LocalizedPosition))
		{
			if (ResultSector != NULL)
			{
				DistanceToResult = ZEVector3d::DistanceSquare(ResultSector->GetGeographicPosition(), Entity->GetGeographicPosition());

				if (DistanceToCurrent < DistanceToResult)
					ResultSector = CurrentSector;
			}
			else
			{
				ResultSector = CurrentSector;
			}
		}
	}

//	zeBreak((ResultSector != NULL) && (ResultSector != ClosestSector));

	if (ResultSector == NULL)
		ResultSector = ClosestSector;

	return ResultSector;
}

void ZESectorSelector::SetReferenceSector(ZESector* ReferenceSector)
{
	ZEArray<ZEEntity*> Sectors = ZEScene::GetInstance()->GetEntities(ZESector::Class());
	ZESector* CurrentSector = NULL;

	for (ZESize I = 0; I < Sectors.GetCount(); I++)
	{
		CurrentSector = (ZESector*)Sectors[I];

		if (ReferenceSector != NULL)
		{
			if (CurrentSector->CheckAdjacency(ReferenceSector, ReferenceSector->GetAdjacencyDepth()))
			{
				if (!CurrentSector->GetEnabled())
				{
					CurrentSector->SetEnabled(true);
					CurrentSector->SetVisible(true);
				}

				const ZEMatrix4x4d& InvGeoTransform = ReferenceSector->GetInvGeographicTransform();
				ZEMatrix4x4d Result;
				ZEMatrix4x4d::Multiply(Result, ReferenceSector->GetInvGeographicTransform(), CurrentSector->GetGeographicTransform());
				CurrentSector->SetPosition(Result.GetTranslation().ToVector3());
				CurrentSector->SetRotation(Result.GetRotation());
				CurrentSector->SetScale(Result.GetScale().ToVector3());
			}
			else
			{
				CurrentSector->SetEnabled(false);
				CurrentSector->SetVisible(false);
			}
		}
		else
		{
			CurrentSector->SetGeographicPosition(CurrentSector->GetGeographicPosition());
			CurrentSector->SetGeographicRotation(CurrentSector->GetGeographicRotation());
			CurrentSector->SetGeographicScale(CurrentSector->GetGeographicScale());
		}
	}
}

ZESector* ZESectorSelector::GetReferenceSector()
{
	return (ZESector*)GetOwner();
}

ZESectorSelector* ZESectorSelector::CreateInstance()
{
	return new ZESectorSelector();
}
