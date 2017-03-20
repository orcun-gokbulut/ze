//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEInteriorHelper.h
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

#include "ZEMeta/ZEObject.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"

ZEMT_FORWARD_DECLARE(ZEInterior);
ZEMT_FORWARD_DECLARE(ZEInteriorRoom);

struct ZEInteriorResourceHelper;

enum ZEInteriorHelperOwnerType
{
	ZE_IHOT_INTERIOR		= 0,
	ZE_IHOT_ROOM			= 1
};

class ZEInteriorHelper : public ZEObject
{
	ZE_OBJECT
	private:
		ZEInterior*							OwnerInterior;
		ZEInteriorHelperOwnerType			OwnerType;
		ZEInteriorRoom*						OwnerRoom;

		const ZEInteriorResourceHelper*		HelperResource;

		ZEVector3							Position;
		ZEVector3							Scale;
		ZEQuaternion						Rotation;

											ZEInteriorHelper();

	public:
		const char*							GetName();
		ZEInteriorHelperOwnerType			GetOwnerType();
		ZEInteriorRoom*						GetRoomOwner();

		void								SetPosition(const ZEVector3& LocalPosition);
		const ZEVector3&					GetPosition();
		ZEVector3							GetInteriorPosition();
		ZEVector3							GetWorldPosition();

		void								SetRotation(const ZEQuaternion& LocalRotation);
		const ZEQuaternion&					GetRotation();
		ZEQuaternion						GetInteriorRotation();
		ZEQuaternion						GetWorldRotation();

		void								SetScale(const ZEVector3& LocalScale);
		const ZEVector3&					GetScale();
		ZEVector3							GetInteriorScale();
		ZEVector3							GetWorldScale();

		ZEVector3							GetFront();
		ZEVector3							GetRight();
		ZEVector3							GetUp();

		ZEVector3							GetInteriorFront();
		ZEVector3							GetInteriorRight();
		ZEVector3							GetInteriorUp();

		ZEVector3							GetWorldFront();
		ZEVector3							GetWorldRight();
		ZEVector3							GetWorldUp();

		void								Initialize(ZEInterior* Interior, const ZEInteriorResourceHelper* HelperResource);
		void								Deinitialize();

		static ZEInteriorHelper*			CreateInstance();
};
