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
#include "ZESectorSelector.h"
#include "ZEGeographicEntity.h"
#include "ZEMath\ZEMath.h"
#include "ZEScene.h"
#include "ZEML\ZEMLWriter.h"
#include "ZEML\ZEMLReader.h"
#include "ZEMeta\ZEMTProvider.h"

#define ZE_GEDF_LOCAL_TRANSFORM 0x0004

void ZESectorManager::ActivateSector(ZESector* Sector)
{
	zeDebugCheck(Sector->GetScene() != NULL, "Sector is already activated.");

	zeDebugLog("ZESectorManager updated. Sector added to scene. Sector: \"%s\"", Sector->GetName().ToCString());

	Scene->AddEntity(Sector);
	ActiveSectors.Add(Sector);
}

void ZESectorManager::DeactivateSector(ZESector* Sector)
{
	zeDebugCheck(Sector->GetScene() != Scene, "Sector is not added to the scene.");

	Scene->RemoveEntity(Sector);
	ActiveSectors.RemoveValue(Sector);
}

void ZESectorManager::AddToCache(ZESector* Sector)
{
	zeDebugCheck(Sector == NULL, "Sector is NULL.");
	
	if (SectorCache.Exists(Sector))
		return;

	zeDebugLog("ZESectorManager Cache updated. Sector added. Sector: \"%s\"", Sector->GetName().ToCString());

	Sector->Load();
	SectorCache.Add(Sector);
}

void ZESectorManager::RemoveFromCache(ZESector* Sector)
{
	zeDebugCheck(Sector == NULL, "Sector is NULL.");

	zeDebugLog("ZESectorManager Cache updated. Sector removed. Sector: \"%s\"", Sector->GetName().ToCString());

	Sector->Unload();
	SectorCache.RemoveValue(Sector);
}

void ZESectorManager::FollowLinks(ZEArray<ZESector*>& ActivateList, ZEArray<ZESector*>& CacheList, ZESector* Sector, ZESSize ActivateDepth, ZESSize CacheDepth)
{
	if (CacheDepth == 0)
		return;

	CacheDepth--;
	ActivateDepth--;

	ze_for_each(Link, Sector->GetSectorLinks())
	{
		ZESector* LinkedSector = GetSector(Link->Id);
		if (LinkedSector == NULL)
			continue;

		if (ActivateDepth > 0)
		{
			if (ActivateDepth > Link->Depth)
				ActivateDepth = Link->Depth;

			if (!ActivateList.Exists(LinkedSector))
				ActivateList.Add(LinkedSector);
		}
		
		if (!CacheList.Exists(LinkedSector))
			CacheList.Add(LinkedSector);

		FollowLinks(ActivateList, CacheList, LinkedSector, ActivateDepth, CacheDepth);
	}
}

void ZESectorManager::UpdateTransformations()
{
	ZEVector3d GeographicRelativeOrigin = ZEVector3d::Zero;

	ze_for_each(Selector, Selectors)
		GeographicRelativeOrigin += Selector->GetGeographicPosition();

	GeographicRelativeOrigin = GeographicRelativeOrigin / (double)Selectors.GetCount();

	ZESector* ResultSector = GetSector(GeographicRelativeOrigin, true);
	if (OriginSector != ResultSector)
	{
		OriginSector = ResultSector;

		UpdateActiveSectors();

		ze_for_each(Sector, Sectors)
			Sector->GeographicTransformChanged();

		ze_for_each(Selector, Selectors)
			Selector->GeographicTransformChanged();

		ze_for_each(Entity, Scene->GetEntities())
		{
			ZEGeographicEntity* GeographicEntity = ZEClass::Cast<ZEGeographicEntity>(Entity.GetItem());
			if (GeographicEntity == NULL)
				continue;

			GeographicEntity->GeographicTransformChanged();
		}
	}
}

void ZESectorManager::UpdateActiveSectors()
{
	ZEArray<ZESector*> ActivateList;
	ZEArray<ZESector*> CacheList;

	if (!ActivateList.Exists(OriginSector))
		ActivateList.Add(OriginSector);

	if (!CacheList.Exists(OriginSector))
		CacheList.Add(OriginSector);

	FollowLinks(ActivateList, CacheList, OriginSector, CacheDepth, CacheDepth);

	// Update Active Sectors
	for (ZESize I = 0; I < ActiveSectors.GetCount(); I++)
	{
		if (!ActivateList.Exists(ActiveSectors[I]))
		{
			DeactivateSector(ActiveSectors[I]);
			I--;
		}
	}

	for (ZESize I = 0; I < ActivateList.GetCount(); I++)
	{
		if (!ActiveSectors.Exists(ActivateList[I]))
			ActivateSector(ActivateList[I]);
	}


	// Update Cache
	for (ZESize I = 0; I < SectorCache.GetCount(); I++)
	{
		if (!CacheList.Exists(SectorCache[I]))
		{
			RemoveFromCache(SectorCache[I]);
			I--;
		}
	}
	
	for (ZESize I = 0; I < CacheList.GetCount(); I++)
	{
		if (!SectorCache.Exists(CacheList[I]))
			AddToCache(CacheList[I]);
	}
}

ZESectorManager::ZESectorManager()
{
	Scene = NULL;
	OriginSector = NULL;
	CacheDepth = 8;
}

ZESectorManager::~ZESectorManager()
{
	if (Scene != NULL)
		Scene->SetSectorManager(NULL);
}

ZEScene* ZESectorManager::GetScene() const
{
	return Scene;
}

void ZESectorManager::SetCacheDepth(ZEUInt32 Depth)
{
	zeCheckError(Depth < 8, ZE_VOID, "ZESectorManager Cache Depth must be greater than 8. Value : \"%d\".", Depth);

	CacheDepth = Depth;
}

ZESize ZESectorManager::GetCacheDepth() const
{
	return CacheDepth;
}

const ZEList2<ZEGeographicEntity>& ZESectorManager::GetSectors() const
{
	return Sectors;
}

ZEArray<ZESector*> ZESectorManager::GetSectors(const ZEVector3d& Position) const
{
	ZEArray<ZESector*> Results;
	ZEVector3 LocalizedTestPosition;

	ze_for_each(Sector, Sectors)
	{
		LocalizedTestPosition = (Position - Sector->GetGeographicPosition()).ToVector3();

		if (ZEAABBox::IntersectionTest(Sector->GetBoundingBox(), LocalizedTestPosition))
			Results.Add(static_cast<ZESector*>(Sector.GetPointer()));
	}

	return Results;
}

ZESector* ZESectorManager::GetSector(const ZEGUID& Id) const
{
	ze_for_each(Sector, Sectors)
	{
		ZESector* Temp = static_cast<ZESector*>(Sector.GetPointer());
		if (Temp->GetGUID() == Id)
			return Temp;
	}

	return NULL;
}

ZESector* ZESectorManager::GetSector(const ZEVector3d& Position, bool Proximity) const
{
	ZEGeographicEntity* ResultSector = NULL;
	ZEGeographicEntity* ClosestSector = NULL;
	double DistanceToResult = 0.0;
	double DistanceToCurrent = 0.0;
	double DistanceToClosest = ZE_DOUBLE_MAX;
	ZEVector3 LocalizedTestPosition;

	ze_for_each(Sector, Sectors)
	{
		LocalizedTestPosition = (Position - Sector->GetGeographicPosition()).ToVector3();

		DistanceToCurrent = ZEVector3d::DistanceSquare(Sector->GetGeographicPosition(), Position);

		if (DistanceToCurrent < DistanceToClosest)
		{
			DistanceToClosest = DistanceToCurrent;
			ClosestSector = Sector.GetPointer();
		}

		if (ZEAABBox::IntersectionTest(Sector->GetBoundingBox(), LocalizedTestPosition))
		{
			if (ResultSector != NULL)
			{
				DistanceToResult = ZEVector3d::DistanceSquare(ResultSector->GetGeographicPosition(), Position);

				if (DistanceToCurrent < DistanceToResult)
					ResultSector = Sector.GetPointer();
			}
			else
			{
				ResultSector = Sector.GetPointer();
			}
		}
	}

//	zeDebugLog("Closest Sector is not current Sector.", (ResultSector != NULL) && (ResultSector != ClosestSector));

	if (ResultSector == NULL && Proximity)
		ResultSector = ClosestSector;

	return static_cast<ZESector*>(ResultSector);
}

ZESector* ZESectorManager::GetSector(const ZEString& Name)
{
	ze_for_each(Sector, Sectors)
	{
		if (Sector->GetName() == Name)
			return static_cast<ZESector*>(Sector.GetPointer());
	}

	return NULL;
}

ZESector* ZESectorManager::GetOriginSector()
{
	return OriginSector;
}

bool ZESectorManager::AddSector(ZESector* Sector)
{
	zeCheckError(Sector == NULL, false, "Cannot add sector. Sector is NULL.");
	zeCheckError(Sectors.Exists(&Sector->GeoLink), false, 
		"Can not add sector. Sector already registered to this Manager. Sector Name: \"%s\".", Sector->GetName().ToCString());

	Sectors.AddEnd(&Sector->GeoLink);
	Sector->SetManager(this);

	return true;
}

void ZESectorManager::RemoveSector(ZESector* Sector)
{
	zeCheckError(Sector == NULL, ZE_VOID, "Cannot remove sector. Sector is NULL.");
	zeCheckError(!Sectors.Exists(&Sector->GeoLink), ZE_VOID, 
		"Can not remove sector. Sector is not registered to this manager. Sector Name: \"%s\".", Sector->GetName().ToCString());

	Sector->SetManager(NULL);
	Sectors.Remove(&Sector->GeoLink);
}

void ZESectorManager::ClearSectors()
{
	ze_for_each(Sector, Sectors)
		static_cast<ZESector*>(Sector.GetPointer())->SetManager(NULL);

	Sectors.Clear();
}

const ZEList2<ZEGeographicEntity>& ZESectorManager::GetSelectors() const
{
	return Selectors;
}

bool ZESectorManager::AddSelector(ZESectorSelector* Selector)
{
	zeCheckError(Selector == NULL, false, "Cannot add selector. Selector is NULL.");
	zeCheckError(Selectors.Exists(&Selector->GeoLink), false, 
		"Can not add selector. Selector already registered to this manager. Selector Name: \"%s\".", Selector->GetName().ToCString());

	Selectors.AddEnd(&Selector->GeoLink);

	return true;
}

void ZESectorManager::RemoveSelector(ZESectorSelector* Selector)
{
	zeCheckError(Selector == NULL, ZE_VOID, "Cannot remove selector. Selector is NULL.");
	zeCheckError(!Selectors.Exists(&Selector->GeoLink), ZE_VOID, 
		"Can not remove selector. Selector is not registered to this manager. Selector Name: \"%s\".", Selector->GetName().ToCString());

	Selectors.Remove(&Selector->GeoLink);
}

void ZESectorManager::ClearSelectors()
{
	Selectors.Clear();
}

void ZESectorManager::Process(float Time)
{
	UpdateTransformations();
}

bool ZESectorManager::Serialize(const ZEString& FileName)
{
	zeLog("Saving ZESectorManager file \"%s\".", FileName.GetValue());

	ZEMLWriter Writer;
	zeCheckError(!Writer.Open(FileName), false, "Saving ZESectorManager failed. Cannot write to file. File Name: \"%s\".", FileName.ToCString());

	ZEMLWriterNode SectorManagerNode;
	Writer.OpenRootNode("ZESectorManager", SectorManagerNode);

	SectorManagerNode.WriteUInt8("VersionMajor", 1);
	SectorManagerNode.WriteUInt8("VersionMinor", 0);

	ZEMLWriterNode PropertiesNode;
	SectorManagerNode.OpenNode("Properties", PropertiesNode);
	PropertiesNode.WriteUInt32("CacheDepth", CacheDepth);
	PropertiesNode.CloseNode();

	ZEMLWriterNode SelectorsNode;
	SectorManagerNode.OpenNode("Selectors", SelectorsNode);

	ze_for_each(Selector, Selectors)
	{
		zeCheckError(Selector->GetClass() == NULL, false, "Saving ZESectorManager file failed. Class for selector is not registered. Selector Name: \"%s\".", Selector->GetName().ToCString());

		ZEMLWriterNode SelectorNode;
		SelectorsNode.OpenNode("Selector", SelectorNode);
		SelectorNode.WriteString("Class", Selector->GetClass()->GetName());

		if (!Selector->Serialize(&SelectorNode))
		{
			zeError("Saving ZESectorManager file failed. Selector serialization failed. Selector Name: \"%s\".", Selector->GetName().ToCString());
			SelectorNode.CloseNode();
			SelectorsNode.CloseNode();
			SectorManagerNode.CloseNode();
			Writer.Close();
			return false;
		}

		SelectorNode.CloseNode();
	}

	SelectorsNode.CloseNode();

	ZEMLWriterNode SectorsNode;
	SectorManagerNode.OpenNode("Sectors", SectorsNode);

	ze_for_each(Sector, Sectors)
	{
		zeCheckError(Sector->GetClass() == NULL, false, "Saving ZESectorManager file failed. Class for sector is not registered. Sector Name: \"%s\".", Sector->GetName().ToCString());

		ZEMLWriterNode SectorNode;
		SectorsNode.OpenNode("Sector", SectorNode);
		SectorNode.WriteString("Class", Sector->GetClass()->GetName());

		if (!Sector->Serialize(&SectorNode))
		{
			zeError("Saving ZESectorManager file failed. Sector serialization failed. Sector Name: \"%s\".", Sector->GetName().ToCString());
			SectorNode.CloseNode();
			SectorsNode.CloseNode();
			SectorManagerNode.CloseNode();
			Writer.Close();
			return false;
		}

		SectorNode.CloseNode();
	}

	SectorsNode.CloseNode();

	ZEMLWriterNode GeoEntitiesNode;
	SectorManagerNode.OpenNode("GeographicEntities", GeoEntitiesNode);

	/*ze_for_each(GeographicEntity, GeographicEntities)
	{
		zeCheckError(GeographicEntity->GetClass() == NULL, false, "Saving ZESectorManager file failed. Class for geographic entity is not registered. Entity Name: \"%s\".", GeographicEntity->GetName().ToCString());

		ZEMLWriterNode GeoEntityNode;
		SelectorsNode.OpenNode("GeographicEntity", GeoEntityNode);
		GeoEntityNode.WriteString("Class", GeographicEntity->GetClass()->GetName());

		if (!GeographicEntity->Serialize(&GeoEntityNode))
		{
			zeError("Saving ZESectorManager file failed. Geographic entity serialization failed. Entity Name: \"%s\".", GeographicEntity->GetName().ToCString());
			GeoEntityNode.CloseNode();
			GeoEntitiesNode.CloseNode();
			SectorManagerNode.CloseNode();
			Writer.Close();
			return false;
		}

		GeoEntityNode.CloseNode();
	}*/

	GeoEntitiesNode.CloseNode();
	SectorManagerNode.CloseNode();
	Writer.Close();	
	zeLog("ZESectorManager file \"%s\" has been saved.", FileName.GetValue());

	return true;
}

bool ZESectorManager::Unserialize(const ZEString& FileName)
{
	zeLog("Loading ZESectorManager file \"%s\".", FileName.GetValue());

	ZEMLReader Reader;
	zeCheckError(!Reader.Open(FileName), false, "Loading ZESectorManager failed. Cannot read from file. File Name: \"%s\".", FileName.ToCString());
	ZEMLReaderNode SectorManagerNode = Reader.GetRootNode();

	zeCheckError(SectorManagerNode.GetName() != "ZESectorManager", false, "Loading ZESectorManager failed. Corrupted ZESectorManager file. File Name: \"%s\".", FileName.ToCString());

	ClearSelectors();
	ClearSectors();

	ZEMLReaderNode PropertiesNode = SectorManagerNode.GetNode("Properties");
	SetCacheDepth(PropertiesNode.ReadUInt32("CacheDepth", 8));

	ZEMLReaderNode SelectorsNode = SectorManagerNode.GetNode("Selectors");
	ZESize SelectorCount = SelectorsNode.GetNodeCount("Selector");

	for (ZESize I = 0; I < SelectorCount; I++)
	{
		ZEMLReaderNode SelectorNode = SelectorsNode.GetNode("Selector", I);
		zeCheckError(!SelectorNode.IsValid(), false, "Loading ZESectorManager failed. Corrupted ZESectorManager file. File Name: \"%s\".", FileName.ToCString());


		ZEClass* SelectorClass = ZEMTProvider::GetInstance()->GetClass(SelectorNode.ReadString("Class"));
		if (SelectorClass == NULL)
		{
			zeWarning("Problem in loading ZESectorManager. Selector class is not registered. Class Name: \"%s\".", SelectorNode.ReadString("Class").ToCString());
			continue;
		}

		ZESectorSelector* NewSelector = (ZESectorSelector*)SelectorClass->CreateInstance();
		zeCheckError(NewSelector == NULL, false, "Loading ZESectorManager failed. Cannot create instance of a selector. Class Name: \"%s\".", SelectorNode.ReadString("Class").ToCString());
		zeCheckError(!NewSelector->Unserialize(&SelectorNode), false, "Loading ZESectorManager failed. Unserialization of selector has failed. Class Name: \"%s\".", SelectorNode.ReadString("Class").ToCString());

		AddSelector(NewSelector);
	}

	ZEMLReaderNode SectorsNode = SectorManagerNode.GetNode("Sectors");
	ZESize SectorCount = SectorsNode.GetNodeCount("Sector");

	for (ZESize I = 0; I < SectorCount; I++)
	{
		ZEMLReaderNode SectorNode = SectorsNode.GetNode("Sector", I);
		zeCheckError(!SectorNode.IsValid(), false, "Loading ZESectorManager failed. Corrupted ZESectorManager file. File Name: \"%s\".", FileName.ToCString());

		ZEClass* SectorClass = ZEMTProvider::GetInstance()->GetClass(SectorNode.ReadString("Class"));

		if (SectorClass == NULL)
		{
			zeWarning("Problem in loading ZESectorManager. Sector class is not registered. Class Name: \"%s\".", SectorNode.ReadString("Class").ToCString());
			continue;
		}

		ZESector* NewSector = (ZESector*)SectorClass->CreateInstance();
		zeCheckError(NewSector == NULL, false, "Loading ZESectorManager failed. Cannot create instance of a sector. Class Name: \"%s\".", SectorNode.ReadString("Class").ToCString());
		zeCheckError(!NewSector->Unserialize(&SectorNode), false, "Loading ZESectorManager failed. Unserialization of sector has failed. Class Name: \"%s\".", SectorNode.ReadString("Class").ToCString());

		AddSector(NewSector);
	}

	/*ZEMLReaderNode GeographicEntitiesNode = SectorManagerNode.GetNode("GeographicEntities");
	ZESize GeographicEntityCount = GeographicEntitiesNode.GetNodeCount("GeographicEntity");
	for (ZESize I = 0; I < GeographicEntityCount; I++)
	{
		ZEMLReaderNode GeographicEntityNode = GeographicEntitiesNode.GetNode("GeographicEntity", I);
		zeCheckError(!GeographicEntityNode.IsValid(), false, "Loading ZESectorManager failed. Corrupted ZESectorManager file. File Name: \"%s\".", FileName.ToCString());

		ZEClass* GeographicEntityClass = ZEMTProvider::GetInstance()->GetClass(GeographicEntityNode.ReadString("Class"));

		if (GeographicEntityClass == NULL)
		{
			zeWarning("Problem in loading ZESectorManager. GeographicEntity class is not registered. Class Name: \"%s\".", GeographicEntityNode.ReadString("Class").ToCString());
			continue;
		}

		ZEGeographicEntity* NewGeographicEntity = (ZEGeographicEntity*)GeographicEntityClass->CreateInstance();
		zeCheckError(NewGeographicEntity == NULL, false, "Loading ZESectorManager failed. Cannot create instance of a GeographicEntity. Class Name: \"%s\".", GeographicEntityNode.ReadString("Class").ToCString());
		zeCheckError(!NewGeographicEntity->Unserialize(&GeographicEntityNode), false, "Loading ZESectorManager failed. Unserialization of GeographicEntity has failed. Class Name: \"%s\".", GeographicEntityNode.ReadString("Class").ToCString());

		AddGeographicEntity(NewGeographicEntity);
	}*/

	zeLog("ZESectorManager file \"%s\" has been loaded.", FileName.GetValue());
	return true;
}

void ZESectorManager::Destroy()
{
	delete this;
}

ZESectorManager* ZESectorManager::CreateInstance()
{
	return new ZESectorManager();
}
