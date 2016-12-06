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
#include "ZEMeta\ZEProvider.h"

#define ZE_GEDF_LOCAL_TRANSFORM 0x0004

void ZESectorManager::AddToCache(ZESector* Sector)
{
	if (Sector == NULL)
		return;

	if (SectorCache.Exists(Sector))
		return;

	zeDebugLog("ZESectorManager Cache updated. Sector added. Sector: \"%s\"", Sector->GetName().ToCString());

	Sector->Load();
	SectorCache.Add(Sector);
}

void ZESectorManager::RemoveFromCache(ZESector* Sector)
{
	if (Sector == NULL)
		return;

	if (Sector->GetScene() != NULL && Sector->GetScene() != Scene)
		return;

	if (!SectorCache.Exists(Sector))
		return;

	zeDebugLog("ZESectorManager Cache updated. Sector removed. Sector: \"%s\"", Sector->GetName().ToCString());
	zeDebugLog("ZESectorManager updated. Sector removed from Scene. Sector: \"%s\"", Sector->GetName().ToCString());

	SectorCache.RemoveValue(Sector);

	if (Sector->GetScene() != NULL)
		Scene->RemoveEntity(Sector);

	Sector->Unload();
}

void ZESectorManager::UpdateTransformation(ZEGeographicEntity* Entity, bool Forced)
{
	if (OriginSector == NULL)
		return;

	if (Entity == NULL)
		return;

	if (Entity->GetScene() == NULL)
		return;

	if (Forced || Entity->GeographicEntityDirtyFlags.GetFlags(ZE_GEDF_LOCAL_TRANSFORM))
	{
		ZEMatrix4x4d EntityLocalTransform;
		ZEMatrix4x4d::Multiply(EntityLocalTransform, OriginSector->GetInvGeographicTransform(), Entity->GetGeographicTransform());
		Entity->SetPosition(EntityLocalTransform.GetTranslation().ToVector3());
		Entity->SetRotation(EntityLocalTransform.GetRotation());
		Entity->SetScale(EntityLocalTransform.GetScale().ToVector3());
		Entity->GeographicEntityDirtyFlags.UnraiseFlags(ZE_GEDF_LOCAL_TRANSFORM);
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

		ze_for_each(Sector, Sectors)
			UpdateTransformation(Sector.GetPointer(), true);

		ze_for_each(Selector, Selectors)
			UpdateTransformation(Selector.GetPointer(), true);

		ze_for_each(Entity, GeographicEntities)
			UpdateTransformation(Entity.GetPointer(), true);
	}
	else
	{
		ze_for_each(Selector, Selectors)
			UpdateTransformation(Selector.GetPointer());

		ze_for_each(Entity, GeographicEntities)
			UpdateTransformation(Entity.GetPointer());
	}
}

void ZESectorManager::UpdateActiveSectors()
{
	ZESector* CurrentSelectedSector = NULL;

	ze_for_each(Selector, Selectors)
	{
		CurrentSelectedSector = GetSector(Selector->GetGeographicPosition(), true);

		if (CurrentSelectedSector == NULL)
			continue;

		ze_for_each(Sector, Sectors)
		{
			ZESector* CurrentSector = static_cast<ZESector*>(Sector.GetPointer());
			bool NormalResult = CurrentSelectedSector->CheckLink(CurrentSector);
			bool CacheResult = CurrentSelectedSector->CheckLinkInternal(CurrentSector, CacheDepth, false);

			if (CacheResult)
			{
				AddToCache(CurrentSector);
			}
			else
			{
				RemoveFromCache(CurrentSector);
			}

			if (NormalResult)
			{
				if (CurrentSector->GetScene() == NULL)
				{
					zeDebugLog("ZESectorManager updated. Sector added to scene. Sector: \"%s\"", Sector->GetName().ToCString());
					Scene->AddEntity(CurrentSector);
					UpdateTransformation(CurrentSector, true);
				}

				if (!CurrentSector->GetVisible())
				{
					CurrentSector->SetVisible(true);
					zeDebugLog("ZESectorManager updated. Sector set as visible. Sector: \"%s\"", Sector->GetName().ToCString());
					UpdateTransformation(CurrentSector, true);
				}
			}
			else
			{
				if (CurrentSector->GetVisible())
				{
					CurrentSector->SetVisible(false);
					zeDebugLog("ZESectorManager updated. Sector set as invisible. Sector: \"%s\"", Sector->GetName().ToCString());
				}
			}
		}
	}
}

ZESectorManager::ZESectorManager()
{
	OriginSector = NULL;
	CacheDepth = 8;
}

void ZESectorManager::SetScene(ZEScene* Scene)
{
	zeCheckError(Scene == NULL, ZE_VOID, "Cannot set scene to sector manager. Scene is NULL.");
	this->Scene = Scene;
}

ZEScene* ZESectorManager::GetScene() const
{
	return Scene;
}

void ZESectorManager::SetCacheDepth(ZESize Depth)
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

const ZEList2<ZEGeographicEntity>& ZESectorManager::GetGeographicEntities() const
{
	return GeographicEntities;
}

bool ZESectorManager::AddGeographicEntity(ZEGeographicEntity* Entity)
{
	zeCheckError(Entity == NULL, false, "Cannot add entity. Entity is NULL.");
	zeCheckError(GeographicEntities.Exists(&Entity->GeoLink), false, 
		"Can not add entity. Entity already registered to this manager. Entity Name: \"%s\".", Entity->GetName().ToCString());

	GeographicEntities.AddEnd(&Entity->GeoLink);

	return true;
}

void ZESectorManager::RemoveGeographicEntity(ZEGeographicEntity* Entity)
{
	zeCheckError(Entity == NULL, ZE_VOID, "Cannot remove entity. Entity is NULL.");
	zeCheckError(!GeographicEntities.Exists(&Entity->GeoLink), ZE_VOID, 
		"Can not remove entity. Entity is not registered to this manager. Entity Name: \"%s\".", Entity->GetName().ToCString());

	GeographicEntities.Remove(&Entity->GeoLink);
}

void ZESectorManager::ClearGeographicEntities()
{
	GeographicEntities.Clear();
}

void ZESectorManager::Process(float Time)
{
	UpdateTransformations();

	UpdateActiveSectors();
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

	ze_for_each(GeographicEntity, GeographicEntities)
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
	}

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
	ClearGeographicEntities();

	ZEMLReaderNode PropertiesNode = SectorManagerNode.GetNode("Properties");
	SetCacheDepth(PropertiesNode.ReadUInt32("CacheDepth", 8));

	ZEMLReaderNode SelectorsNode = SectorManagerNode.GetNode("Selectors");
	ZESize SelectorCount = SelectorsNode.GetNodeCount("Selector");

	for (ZESize I = 0; I < SelectorCount; I++)
	{
		ZEMLReaderNode SelectorNode = SelectorsNode.GetNode("Selector", I);
		zeCheckError(!SelectorNode.IsValid(), false, "Loading ZESectorManager failed. Corrupted ZESectorManager file. File Name: \"%s\".", FileName.ToCString());


		ZEClass* SelectorClass = ZEProvider::GetInstance()->GetClass(SelectorNode.ReadString("Class"));
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

		ZEClass* SectorClass = ZEProvider::GetInstance()->GetClass(SectorNode.ReadString("Class"));

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

	ZEMLReaderNode GeographicEntitiesNode = SectorManagerNode.GetNode("GeographicEntities");
	ZESize GeographicEntityCount = GeographicEntitiesNode.GetNodeCount("GeographicEntity");

	for (ZESize I = 0; I < GeographicEntityCount; I++)
	{
		ZEMLReaderNode GeographicEntityNode = GeographicEntitiesNode.GetNode("GeographicEntity", I);
		zeCheckError(!GeographicEntityNode.IsValid(), false, "Loading ZESectorManager failed. Corrupted ZESectorManager file. File Name: \"%s\".", FileName.ToCString());

		ZEClass* GeographicEntityClass = ZEProvider::GetInstance()->GetClass(GeographicEntityNode.ReadString("Class"));

		if (GeographicEntityClass == NULL)
		{
			zeWarning("Problem in loading ZESectorManager. GeographicEntity class is not registered. Class Name: \"%s\".", GeographicEntityNode.ReadString("Class").ToCString());
			continue;
		}

		ZEGeographicEntity* NewGeographicEntity = (ZEGeographicEntity*)GeographicEntityClass->CreateInstance();
		zeCheckError(NewGeographicEntity == NULL, false, "Loading ZESectorManager failed. Cannot create instance of a GeographicEntity. Class Name: \"%s\".", GeographicEntityNode.ReadString("Class").ToCString());
		zeCheckError(!NewGeographicEntity->Unserialize(&GeographicEntityNode), false, "Loading ZESectorManager failed. Unserialization of GeographicEntity has failed. Class Name: \"%s\".", GeographicEntityNode.ReadString("Class").ToCString());

		AddGeographicEntity(NewGeographicEntity);
	}

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
