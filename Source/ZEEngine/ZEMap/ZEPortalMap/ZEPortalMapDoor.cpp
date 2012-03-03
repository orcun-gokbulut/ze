//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPortalMapDoor.cpp
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

#include "ZEPortalMapDoor.h"
#include "ZEPortalMapResource.h"
#include "ZEPortalMap.h"
#include "ZEPortalMapPortal.h"

ZEPortalMap* ZEPortalMapDoor::GetOwner()
{
	return Owner;
}

const char* ZEPortalMapDoor::GetName()
{
	if (Resource != NULL)
		return Resource->Name;
	else
		return "";
}

ZEPortalMapPortal** ZEPortalMapDoor::GetPortals()
{
	return Portals;
}

const ZERectangle3D& ZEPortalMapDoor::GetRectangle() const
{
	return Rectangle;
}

void ZEPortalMapDoor::SetSeenThrough(bool Value)
{
	SeenThrough = Value;
}

bool ZEPortalMapDoor::GetSeenThrough()
{
	return SeenThrough;
}

void ZEPortalMapDoor::Initialize(ZEPortalMap* Owner, const ZEPortalMapResourceDoor* Resource)
{
	this->Owner = Owner;
	this->Resource = Resource;
	Rectangle = Resource->Rectangle;
	Open = Resource->IsOpen;
	
	Portals[0] = Owner->Portals[Resource->PortalIds[0]];
	Portals[0]->Doors.Add(this);

	Portals[1] = Owner->Portals[Resource->PortalIds[1]];
	Portals[1]->Doors.Add(this);
}

void ZEPortalMapDoor::Deinitialize()
{
	Owner = NULL;
	Resource = NULL;
}

void ZEPortalMapDoor::SetOpen(bool Open)
{
	this->Open = Open;
}

bool ZEPortalMapDoor::GetOpen()
{
	return Open;
}

ZEPortalMapDoor::ZEPortalMapDoor()
{
	Owner = NULL;
	Resource = NULL;
	Portals[0] = NULL;
	Portals[1] = NULL;
	Open = true;
}

ZEPortalMapDoor* ZEPortalMapDoor::CreateInstance()
{
	return new ZEPortalMapDoor();
}


