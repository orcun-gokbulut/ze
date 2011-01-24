//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETrigger.h
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

#pragma once
#ifndef __ZE_TRIGGER_H__
#define __ZE_TRIGGER_H__

#include "ZEMeta/ZEClass.h"
#include "ZEEntity.h"
#include "ZEFastDelegate.h"

ZE_META_ENTITY_DESCRIPTION(ZETrigger);

class ZETrigger;

enum ZEActivateTriggerOn
{
	ZE_ATO_NONE,
	ZE_ATO_ENTER,
	ZE_ATO_LEAVE,
	ZE_ATO_INSIDE
};

enum ZETriggerShape
{
	ZE_TS_SPHERE,
	ZE_TS_BOX,
};

enum ZEFilterObjectsBy
{
	ZE_FOB_NONE,
	ZE_FOB_ENTITY_INSTANCE,
	ZE_FOB_ENTITY_TYPE
};

enum ZEReportBehaviour
{
	ZE_RB_REPORT_NONE,
	ZE_RB_REPORT_TRIGGER_STATUS,
	ZE_RB_REPORT_ENTITY_STATUS
};

typedef fastdelegate::FastDelegate2<ZETrigger*, ZEEntity*, void> ZETriggerCallback;

class ZETrigger : public ZEEntity
{
	ZE_META_ENTITY(ZETrigger)
	private:
		ZETriggerCallback			Callback;

		bool						IsTriggerActive;
		bool						IsTriggerFired;

		ZESmartArray<int>			InsideEntityIds;

		ZETriggerShape				TriggerShape;
		ZEFilterObjectsBy			FilterObjectsBy;
		ZEActivateTriggerOn			ActivateTriggerOn;
		ZEReportBehaviour			ReportBehaviour;

	public:
		virtual	ZEDWORD				GetDrawFlags();

		virtual void				SetScale(const ZEVector3& Scale);

		void						SetCallback(ZETriggerCallback Callback);
		ZETriggerCallback			GetCallback();

		void						SetTriggerShape(ZETriggerShape TriggerShape);
		ZETriggerShape				GetTriggerShape() const;

		void						SetFilterObjectsBy(ZEFilterObjectsBy FilterObjectsBy);
		ZEFilterObjectsBy			GetFilterObjectsBy() const;

		void						SetActivateTriggerOn(ZEActivateTriggerOn ActivateTriggerOn);
		ZEActivateTriggerOn			GetActivateTriggerOn() const;

		void						SetReportBehaviour(ZEReportBehaviour ReportBehaviour);
		ZEReportBehaviour			GetReportBehaviour() const;

		virtual void				Tick(float ElapsedTime);

									ZETrigger();
};
/*
ZE_POST_PROCESSOR_START(Meta)
<zinek>
	<meta>
		<class name="ZETrigger" parent="ZEEntity" icon="" description="Trigger">
			<property name="TriggerShape" type="integer" autogetset="true" description="Shape of trigger">
				<enumurator name="ZETriggerShape">
					<item name="Sphere" value="ZE_TS_SPHERE"/>
					<item name="Box" value="ZE_TS_BOX"/>
				</enumurator>
			</property>
			<property name="ActivateTriggerOn" type="integer" autogetset="true" description="Activation type of trigger">
				<enumurator name="ZEActivateTriggerOn">
					<item name="Never Activate" value="ZE_ATO_NONE"/>  
					<item name="Activate on enter" value="ZE_ATO_ENTER"/>
					<item name="Activate on leave" value="ZE_ATO_LEAVE"/>
					<item name="Activate on inside" value="ZE_ATO_INSIDE"/>
				</enumurator>
			</property>
			<property name="FilterObjectsBy" type="integer" autogetset="true" description= "Filter objects for activate">
				<enumurator name="ZEFilterObjectsBy">
					<item name="Filter None" value="ZE_FOB_NONE"/>
					<item name="Filter by entity instance" value="ZE_FOB_ENTITY_INSTANCE"/>
					<item name="Filter by entity type" value="ZE_FOB_ENTITY_TYPE"/>
				</enumurator>
			</property>
			<property name="ReportBehaviour" type="integer" autogetset="true" description="Report behaviour">
				<enumurator name="ZEReportBehaviour">
					<item name="Report None" value="ZE_RB_REPORT_NONE"/>
					<item name="Report Trigger Status" value="ZE_RB_REPORT_TRIGGER_STATUS"/>
					<item name="Report Entity Status" value="ZE_RB_REPORT_ENTITY_STATUS"/>
				</enumurator>
			</property>
		</class>
	</meta>
</zinek>
ZE_POST_PROCESSOR_END() 
*/
#endif

