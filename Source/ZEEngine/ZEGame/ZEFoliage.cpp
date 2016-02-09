//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFoliage.cpp
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

#include "ZEFoliage.h"
#include "ZEModel\ZEModelResource.h"
#include "ZERandom.h"
#include "ZEModel\ZEModel.h"
#include "ZEMath\ZEAngle.h"
#include "ZEEntityProvider.h"
#include "ZEScene.h"
#include "ZEInterior\ZEInterior.h"

bool ZEFoliage::InitializeSelf()
{
	if (!ZEEntity::InitializeSelf())
		return false;



	Trees.Add(ZEModelResource::LoadSharedResource("CustTPayloadSim\\Trees\\Goknar\\Goknar.ZEMODEL"));
	Trees.Add(ZEModelResource::LoadSharedResource("CustTPayloadSim\\Trees\\Karacam\\Karacam.ZEMODEL"));
	Trees.Add(ZEModelResource::LoadSharedResource("CustTPayloadSim\\Trees\\Mese\\Mese.ZEMODEL"));
	Trees.Add(ZEModelResource::LoadSharedResource("CustTPayloadSim\\Trees\\Zeytin\\Zeytin.ZEMODEL"));

// 	Tree1 = ZEModelResource::LoadSharedResource("CustTPayloadSim\\Trees\\Goknar\\Goknar.ZEMODEL");
// 	Tree2 = ZEModelResource::LoadSharedResource("CustTPayloadSim\\Trees\\Karacam\\Karacam.ZEMODEL");
// 	Tree3 = ZEModelResource::LoadSharedResource("CustTPayloadSim\\Trees\\Mese\\Mese.ZEMODEL");
// 	Tree4 = ZEModelResource::LoadSharedResource("CustTPayloadSim\\Trees\\Zeytin\\Zeytin.ZEMODEL");


	return true;
}

bool ZEFoliage::DeinitializeSelf()
{
	return ZEEntity::DeinitializeSelf();
}

ZEFoliage::ZEFoliage()
{
	BrushSize = ZEVector2(10.0f, 10.0f);
	BrushDensity = 10.0f;

	MaxScale = 1.0f;
	MinScale = 1.0f;

	Ground = NULL;

// 	Tree1 = NULL;
// 	Tree2 = NULL;
// 	Tree3 = NULL;
// 	Tree4 = NULL;

}

ZEFoliage::~ZEFoliage()
{

}

void ZEFoliage::SetSurfaceEntityName(const char* EntityName)
{
	ZEArray<ZEEntity*>& Entities = ZEScene::GetInstance()->GetEntities(ZEInterior::Class());

	for (ZESize I = 0; I < Entities.GetCount(); I++)
	{
		if (Entities[I]->GetName() == EntityName)
		{
			SetGround((ZEInterior*)Entities[I]);
			break;
		}
	}
}

const char* ZEFoliage::GetSurfaceEntityName() const
{
	if(Ground == NULL)
		return "";
	else
		return Ground->GetName();
}

void ZEFoliage::SetBrushSize(const ZEVector2& Size)
{
	BrushSize = Size;

	Tile();
}

ZEVector2 ZEFoliage::GetBrushSize() const
{
	return BrushSize;
}

void ZEFoliage::SetBrushDensity(float Density)
{
	BrushDensity = Density;

	Tile();
}

float ZEFoliage::GetBrushDensity() const
{
	return BrushDensity;
}

void ZEFoliage::SetGround(ZEInterior* Ground)
{
	this->Ground = Ground;
}

ZEInterior* ZEFoliage::GetGround() const
{
	return Ground;
}

void ZEFoliage::SetMinScale(float MinScale)
{
	this->MinScale = MinScale;

	Tile();
}

float ZEFoliage::GetMinScale() const
{
	return MinScale;
}

void ZEFoliage::SetMaxScale(float MaxScale)
{
	this->MaxScale = MaxScale;

	Tile();
}

float ZEFoliage::GetMaxScale() const
{
	return MaxScale;
}

void ZEFoliage::Tile()
{
	for (ZESize I = 0; I < GetChildEntities().GetCount(); I++)
	{
		ZEEntity* CurrentChild = GetChildEntities()[I];
		RemoveChildEntity(CurrentChild);
		CurrentChild->Destroy();
		I--;
	}

	if (Ground == NULL)
		return;

	HaltonSequence HaltonSeqX((((10.0f) - (2.0f)) * ZERandom::GetFloatPositive() + (2.0f)));
	HaltonSequence HaltonSeqZ((((10.0f) - (2.0f)) * ZERandom::GetFloatPositive() + (2.0f)));

	ZERayCastParameters Parameters;
	ZERayCastReport		Report;

	for (ZESize I = 0; I < BrushDensity; I++)
	{
		ZEModel* Tree = ZEModel::CreateInstance();
		Tree->SetModelResource(Trees[ZERandom::GetUInt8() % 4]);
		AddChildEntity(Tree);

		float PositionX = (HaltonSeqX.GetNext() - 0.5f) * BrushSize.x;
		float PositionZ = (HaltonSeqZ.GetNext() - 0.5f) * BrushSize.y;

		float ScalarValue = (((MaxScale) - (MinScale)) * ZERandom::GetFloatPositive() + (MinScale));

		Parameters.MaximumDistance = 10000.0f;
		Parameters.MinimumDistance = 0.1f;
		Parameters.Ray.p = ZEVector3(PositionX, GetWorldPosition().y, PositionZ);
		Parameters.Ray.v = -ZEVector3::UnitY;
		Parameters.Extras = ZE_RCRE_NONE;

		Ground->RayCast(Report, Parameters);

		Tree->SetWorldPosition(ZEVector3(PositionX, Report.Position.y, PositionZ));
		Tree->SetWorldRotation(ZEQuaternion((((ZE_PI) - (0.0f)) * ZERandom::GetFloatPositive() + (0.0f)), ZEVector3::UnitY));
		Tree->SetWorldScale(ZEVector3(ScalarValue, ScalarValue, ScalarValue));
	}
}

void ZEFoliage::Realign()
{
	if (Ground == NULL)
		return;

	ZERayCastParameters Parameters;
	ZERayCastReport		Report;

	for (ZESize I = 0; I < GetChildEntities().GetCount(); I++)
	{
		ZEVector3 OldPosition = GetChildEntities()[I]->GetWorldPosition();

		Parameters.MaximumDistance = 10000.0f;
		Parameters.MinimumDistance = 0.1f;
		Parameters.Ray.p = ZEVector3(OldPosition.x, GetWorldPosition().y, OldPosition.z);
		Parameters.Ray.v = -ZEVector3::UnitY;
		Parameters.Extras = ZE_RCRE_NONE;

		Ground->RayCast(Report, Parameters);

		GetChildEntities()[I]->SetWorldPosition(ZEVector3(OldPosition.x, Report.Position.y, OldPosition.z));
	}
}

void ZEFoliage::SetPosition(const ZEVector3& NewPosition)
{
	ZEEntity::SetPosition(NewPosition);

	Realign();
}

ZEFoliage* ZEFoliage::CreateInstance()
{
	return new ZEFoliage();
}
