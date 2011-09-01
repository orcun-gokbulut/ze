//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPortalMap.cpp
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

#include "ZEPortalMap.h"
#include "ZEPortalMapResource.h"
#include "ZEPortalMapPortal.h"
#include "ZEPortalMapDoor.h"
#include "ZECore/ZEError.h"
#include "ZECore/ZEConsole.h"
#include "ZEGame/ZEDrawParameters.h"
#include "ZEMath/ZETriangle.h"
#include "ZEMath/ZERay.h"
#include <string.h>

ZEDrawFlags ZEPortalMap::GetDrawFlags() const
{
	return ZE_DF_DRAW | ZE_DF_LIGHT_RECIVER;
}

ZERayCastFlags ZEPortalMap::GetRayCastFlags() const
{
	return ZE_RCF_INTERNAL;
}

void ZEPortalMap::LoadPortalResource(ZEPortalMapResource* NewResource)
{
	if (Resource != NULL)
	{
		Resource->Release();
		Resource = NULL;
	}

	if (NewResource == NULL)
	{
		for (size_t I = 0; I < Portals.GetCount(); I++)
			Portals[I].Deinitialize();

		Portals.SetCount(0);

		for (size_t I = 0; I < Doors.GetCount(); I++)
			Doors[I].Deinitialize();

		Doors.SetCount(0);
	}

	this->Resource = NewResource;

	Portals.SetCount(Resource->GetPortals().GetCount());
	for (size_t I = 0; I < Portals.GetCount(); I++)
		Portals[I].Initialize(this, (ZEPortalMapResourcePortal*)&Resource->GetPortals()[I]);

	Doors.SetCount(Resource->GetDoors().GetCount());
	for (size_t I = 0; I < Doors.GetCount(); I++)
		Doors[I].Initialize(this, &Resource->GetDoors()[I]);
}

ZEPortalMap::ZEPortalMap()
{
	Resource = NULL;
}

ZEPortalMap::~ZEPortalMap()
{
	Deinitialize();
}

const ZEArray<ZEPortalMapPortal>& ZEPortalMap::GetPortals()
{
	return Portals;
}

const ZEArray<ZEPortalMapDoor>& ZEPortalMap::GetDoors()
{
	return Doors;
}

bool ZEPortalMap::Initialize()
{
	if (GetInitialized())
		return false;

	if (!PortalMapFile.IsEmpty())
	{
		ZEPortalMapResource* NewResource = ZEPortalMapResource::LoadSharedResource(PortalMapFile);
		if (NewResource != NULL)
			LoadPortalResource(NewResource);
		else
		{
			zeError("ZEPortalMap", "Can not load ZEPortalMap file.");
			return false;
		}
	}

	return ZEEntity::Initialize();
}

void ZEPortalMap::Deinitialize()
{
	if (Resource != NULL)
	{
		Resource->Release();
		Resource = NULL;
	}
}

bool ZEPortalMap::SetMapFile(const ZEString& FileName)
{
	PortalMapFile = FileName;

	if (!GetInitialized())
		return false;

	const ZEPortalMapResource* NewResource = ZEPortalMapResource::LoadSharedResource(PortalMapFile);
	if (NewResource != NULL)
		LoadPortalResource(Resource);
	else
	{
		zeError("ZEPortalMap", "Can not load ZEPortalMap file.");
		return false;
	}

	return false;
}

const ZEString& ZEPortalMap::GetMapFile() const
{
	return PortalMapFile;
}

void ZEPortalMap::Draw(ZEDrawParameters* DrawParameters)
{
	for (size_t I = 0; I < Portals.GetCount(); I++)
		Portals[I].Draw(DrawParameters);
}

bool ZEPortalMap::CastRay(const ZERay& Ray, ZEVector3& Position, ZEVector3& Normal, float& MinT)
{
	if (Resource == NULL)
		return false;

	float T, CurMinT = 100000000000000.0f;
	bool Found = false;
	for (size_t I = 0; I < Resource->GetPortals().GetCount(); I++)
	{
		const ZEPortalMapResourcePortal* CurrentPortal = &Resource->GetPortals()[I];
		//if (ZEAABBox::IntersectionTest(CurrentPortal->BoundingBox,Ray))
		for (size_t N = 0; N < CurrentPortal->Polygons.GetCount(); N++)
		{
			const ZEPortalMapPolygon& MapPolygon = CurrentPortal->Polygons[N];
			ZETriangle Triangle(MapPolygon.Vertices[0].Position, MapPolygon.Vertices[1].Position, MapPolygon.Vertices[2].Position);
			if (ZETriangle::IntersectionTest(Triangle, Ray, T) && (!Found || MinT > T))
			{
				MinT = T;
				Ray.GetPointOn(Position, T);
				ZETriangle::GetNormal(Triangle, Normal);
				Found = true;
			}
		}
	}

	return Found;
}

ZEPortalMapResource* ZEPortalMap::GetResource() const
{
	return Resource;
}

ZEPortalMap* ZEPortalMap::CreateInstance()
{
	return new ZEPortalMap();
}

ZEEntityRunAt ZEPortalMapDescription::GetRunAt() const
{
	return ZE_ERA_BOTH;
}

#include "ZEPortalMap.h.zpp"
