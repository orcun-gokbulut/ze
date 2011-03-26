//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPhysicalTrigger.h
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
#ifndef	__ZE_PHYSICAL_TRIGGER_H__
#define __ZE_PHYSICAL_TRIGGER_H__

#include "ZEPhysicsModule.h"
#include "ZEPhysicalObject.h"

class	ZEPhysicalWorld;
class	ZEPhysicalObject;
class	ZEPhysicalShape;

enum ZEPhysicalTriggerType
{
	ZE_PTT_DYNAMIC			= 0,
	ZE_PTT_KINEMATIC		= 1
};

enum ZEPhysicalTriggerActivationType
{
	ZE_PTAT_ON_ENTER		= 0,
	ZE_PTAT_ON_LEAVE		= 1,
	ZE_PTAT_ON_INSIDE		= 2
};

class ZEPhysicalTrigger: public ZEPhysicalObject
{
	protected:
		ZEPhysicalTriggerEvent				TriggerEvent;
											ZEPhysicalTrigger();
		virtual								~ZEPhysicalTrigger();

	public:
		virtual	void						SetTriggerShape(ZEPhysicalShape* TriggerShape) = 0;
		virtual const ZEPhysicalShape*		GetTriggerShape() const = 0;

		virtual void						SetTriggerType(ZEPhysicalTriggerType TriggerType) = 0;
		virtual ZEPhysicalTriggerType		GetTriggerType() const = 0;

		virtual void						SetIsTriggerActive(bool IsTriggerActive) = 0;
		virtual bool						GetIsTriggerActive() const = 0;

		virtual void						SetIsTriggerFired(bool IsTriggerFired) = 0;
		virtual bool						GetIsTriggerFired() const = 0;

		virtual void						SetTriggerActivationType(ZEPhysicalTriggerActivationType ActivationType) = 0;
		virtual 
		ZEPhysicalTriggerActivationType		GetTriggerActivationType() const = 0;

		virtual void						SetPosition(ZEVector3 TriggerPosition) = 0;
		virtual ZEVector3					GetPosition() const = 0;

		virtual void						SetRotation(ZEQuaternion TriggerRotation) = 0;
		virtual ZEQuaternion				GetRotation() const = 0;

		virtual void						SetCollisionEnabled(bool IsCollisionEnabled) = 0;
		virtual bool						GetCollisionEnabled() const = 0;

		virtual void						SetTriggerEvent(const ZEPhysicalTriggerEvent& Event);
		virtual const 
		ZEPhysicalTriggerEvent&				GetTriggerEvent();

		virtual bool						Initialize() = 0;
		virtual void						Deinitialize() = 0;

		virtual void						Destroy();

		static ZEPhysicalTrigger*			CreateInstance();
};

#endif




