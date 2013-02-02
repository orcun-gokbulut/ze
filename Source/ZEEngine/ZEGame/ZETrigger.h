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

#include "ZEEntity.h"
#include "ZEDS/ZEFastDelegate.h"

#include "ZENewMeta/ZEObject.h"

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

ZE_CLASS(ZETrigger)

class ZETrigger : public ZEEntity
{
	ZE_OBJECT

	private:
		ZETriggerCallback			Callback;

		bool						IsTriggerActive;
		bool						IsTriggerFired;

		ZESmartArray<ZEInt>			InsideEntityIds;

		ZETriggerShape				TriggerShape;
		ZEFilterObjectsBy			FilterObjectsBy;
		ZEActivateTriggerOn			ActivateTriggerOn;
		ZEReportBehaviour			ReportBehaviour;

	protected:
									ZETrigger();

	public:
		virtual	ZEUInt32			GetDrawFlags();

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

		static ZETrigger*			CreateInstance();
};
#endif
