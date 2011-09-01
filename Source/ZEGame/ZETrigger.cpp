//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETrigger.cpp
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

#include "ZETrigger.h"
#include "ZEScene.h"
#include "ZEGame/ZEEntityProvider.h"

ZE_META_REGISTER_CLASS(ZEEntityProvider, ZETrigger);

ZEDWORD ZETrigger::GetDrawFlags()
{
	return ZE_DF_NONE;
}

void ZETrigger::SetScale(const ZEVector3& Scale)
{
	// Scale bounding box

	SetLocalBoundingBox(ZEAABBox(ZEVector3(-0.5f * Scale.x, -0.5f * Scale.y, -0.5f * Scale.z), ZEVector3(0.5f * Scale.x, 0.5f * Scale.y, 0.5f * Scale.z)));
	ZEEntity::SetScale(Scale);
}

void ZETrigger::SetCallback(ZETriggerCallback Callback)
{
	this->Callback = Callback;
}

ZETriggerCallback ZETrigger::GetCallback()
{
	return this->Callback;
}

void ZETrigger::SetTriggerShape(ZETriggerShape TriggerShape)
{
	this->TriggerShape = TriggerShape;
}

ZETriggerShape ZETrigger::GetTriggerShape() const
{
	return this->TriggerShape;
}

void ZETrigger::SetActivateTriggerOn(ZEActivateTriggerOn ActivateTriggerOn)
{
	this->ActivateTriggerOn = ActivateTriggerOn;
}

ZEActivateTriggerOn ZETrigger::GetActivateTriggerOn() const
{
	return this->ActivateTriggerOn;
}

void ZETrigger::SetFilterObjectsBy(ZEFilterObjectsBy FilterObjectsBy)
{
	this->FilterObjectsBy = FilterObjectsBy;
}

ZEFilterObjectsBy ZETrigger::GetFilterObjectsBy() const
{
	return this->FilterObjectsBy;
}

void ZETrigger::SetReportBehaviour(ZEReportBehaviour ReportBehaviour)
{
	this->ReportBehaviour = ReportBehaviour;
}

ZEReportBehaviour ZETrigger::GetReportBehaviour() const
{
	return this->ReportBehaviour;
}

void ZETrigger::Tick(float ElapsedTime)
{
	if (Callback != NULL && ActivateTriggerOn != ZE_ATO_NONE)
		for (size_t I = 0; I < zeScene->GetEntities().GetCount(); I++)
		{
			ZEEntity* CurrentEntity = zeScene->GetEntities()[I];

			if (CurrentEntity == this)
				continue;

			if (ZEAABBox::IntersectionTest(GetWorldBoundingBox(), CurrentEntity->GetPosition()) && IsTriggerActive)
			{
				switch (ActivateTriggerOn)
				{
				case ZE_ATO_ENTER:
					if(!IsTriggerFired)
					{
						Callback(this,CurrentEntity);
						IsTriggerFired = true;
					}
					break;

				case ZE_ATO_LEAVE:
					if(!IsTriggerFired)
					{
						InsideEntityIds.Add(CurrentEntity->GetEntityId());
						IsTriggerFired = true;
					}
					break;

				case ZE_ATO_INSIDE:
					Callback(this, CurrentEntity);
					break;
				}
			}

			if(!ZEAABBox::IntersectionTest(GetWorldBoundingBox(), CurrentEntity->GetPosition()))
				if (InsideEntityIds.GetCount() != 0 && ActivateTriggerOn == ZE_ATO_LEAVE)
				{
					for(size_t N = 0; N < InsideEntityIds.GetCount(); N++)
					{
						Callback(this,CurrentEntity);
					}

					ActivateTriggerOn = ZE_ATO_NONE;
				}
		}
}

ZETrigger::ZETrigger()
{
	//Trigger Options:
	TriggerShape		= ZE_TS_BOX;
	FilterObjectsBy		= ZE_FOB_NONE;
	ActivateTriggerOn	= ZE_ATO_INSIDE;
	ReportBehaviour		= ZE_RB_REPORT_NONE;

	IsTriggerActive		= true;
	IsTriggerFired		= false;
}

ZETrigger* ZETrigger::CreateInstance()
{
	return new ZETrigger();
}

#include "ZETrigger.h.zpp"

ZEEntityRunAt ZETriggerDescription::GetRunAt() const
{
	return ZE_ERA_BOTH;
}

