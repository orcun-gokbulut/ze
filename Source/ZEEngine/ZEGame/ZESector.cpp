//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESector.cpp
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

#include "ZESector.h"
#include "ZEGame.h"
#include "ZESectorSelector.h"
#include "ZEML\ZEMLWriter.h"
#include "ZEML\ZEMLReader.h"
#include "ZEFile\ZEPathInfo.h"
#include "ZEDS\ZEFormat.h"
#include "ZEFile\ZEPathManager.h"

bool ZESector::SaveSector(ZEMLWriterNode* Serializer)
{
	if (Serializer == NULL)
		return false;

	Serializer->WriteString("GUID", GetGUID().ToString());

	ZEMLWriterNode BoundingBoxNode;
	Serializer->OpenNode("BoundingBox", BoundingBoxNode);
	BoundingBoxNode.WriteVector3("Max", GetBoundingBox().Max);
	BoundingBoxNode.WriteVector3("Min", GetBoundingBox().Min);
	BoundingBoxNode.CloseNode();

	ZEMLWriterNode AdjacentSectorsNode;
	Serializer->OpenNode("AdjacentSectors", AdjacentSectorsNode);
	
	for (ZESize I = 0; I < AdjacentSectorIds.GetCount(); I++)
	{
		ZEMLWriterNode AdjSectorNode;
		AdjacentSectorsNode.OpenNode("Sector", AdjSectorNode);
		AdjSectorNode.WriteString("GUID", AdjacentSectorIds[I].ToString());
		AdjSectorNode.CloseNode();
	}

	AdjacentSectorsNode.CloseNode();

	return ZEGeographicEntity::Save(Serializer);
}

bool ZESector::RestoreSector(ZEMLReaderNode* Unserializer)
{
	if (Unserializer == NULL)
		return false;

	if (!ZEGeographicEntity::Restore(Unserializer))
		return false;

	ZEGUID Value;
	Value.FromString(Unserializer->ReadString("GUID"));

	if (Value == ZEGUID())
		SetGUID(ZEGUID::Generate());
	else
		SetGUID(Value);

	ZEMLReaderNode BoundingBoxNode = Unserializer->GetNode("BoundingBox");

	if (!BoundingBoxNode.IsValid())
		return false;

	ZEAABBox BoundingBox(BoundingBoxNode.ReadVector3("Min"), BoundingBoxNode.ReadVector3("Max"));
	SetBoundingBox(BoundingBox);

	ZEMLReaderNode AdjacentSectorsNode = Unserializer->GetNode("AdjacentSectors");

	if (!AdjacentSectorsNode.IsValid())
		return false;

	ZESize AdjacentSectorCount = AdjacentSectorsNode.GetNodeCount();
	AdjacentSectorIds.SetCount(AdjacentSectorCount);

	for (ZESize I = 0; I < AdjacentSectorCount; I++)
		AdjacentSectorIds[I].FromString(AdjacentSectorsNode.GetNode(I).ReadString("GUID"));

	return true;
}

bool ZESector::SetOwner(ZEEntity* Owner)
{
	if (Owner != NULL)
		return false;

	return ZEGeographicEntity::SetOwner(Owner);
}

bool ZESector::InitializeSelf()
{
// 	if (!SectorFile.IsEmpty())
// 	{
// 		ZESector* ReferenceSector = ZESectorSelector::GetInstance()->GetReferenceSector();
// 
// 		if (ReferenceSector == NULL)
// 			return false;
// 
// 		if (!CheckAdjacency(ReferenceSector, ReferenceSector->GetAdjacencyDepth()))
// 			return false;
// 	}

	return ZEGeographicEntity::InitializeSelf();
}

bool ZESector::CheckAdjacency(ZESector* TargetSector, ZEInt8 Depth)
{
	if (TargetSector == NULL)
		return false;

	if (this == TargetSector)
		return true;

	if (Depth == 0)
		return false;

	const ZEArray<ZEGUID>& SectorIds = TargetSector->GetAdjacentSectorIds();
	ZESector* CurrentSector = NULL;

	for (ZESize I = 0; I < SectorIds.GetCount(); I++)
	{
		CurrentSector = ZESector::FindSector(SectorIds[I]);

		if (CheckAdjacency(CurrentSector, (Depth - 1)))
			return true;
	}

	return false;
}

ZESector::ZESector()
{
	AdjacencyDepth = 1;
}

void ZESector::SetGUID(const ZEGUID& GUID)
{
	this->GUID = GUID;
}

const ZEGUID& ZESector::GetGUID()
{
	return GUID;
}

void ZESector::SetSectorFile(const ZEString& FileName)
{
	SectorFile = FileName;
}

const ZEString& ZESector::GetSectorFile() const
{
	return SectorFile;
}

const ZEArray<ZEGUID>& ZESector::GetAdjacentSectorIds() const
{
	return AdjacentSectorIds;
}

bool ZESector::AddAdjacentSector(ZESector* Sector)
{
	if (IsInitialized())
		return false;

	if (Sector == this)
		return false;

	if (Sector == NULL)
		return false;

	if (AdjacentSectorIds.Exists(Sector->GetGUID()))
		return false;

	AdjacentSectorIds.Add(Sector->GetGUID());

	return true;
}

bool ZESector::RemoveAdjacentSector(ZESector* Sector)
{
	if (IsInitialized())
		return false;

	if (Sector == this)
		return false;

	if (Sector == NULL)
		return false;

	if (!AdjacentSectorIds.Exists(Sector->GetGUID()))
		return false;

	AdjacentSectorIds.RemoveValue(Sector->GetGUID());

	return true;
}

ZESector* ZESector::FindSector(const ZEGUID& Id)
{
	const ZEArray<ZEEntity*>& Sectors = ZEGame::GetInstance()->GetScene()->GetEntities(ZESector::Class());

	for (ZESize I = 0; I < Sectors.GetCount(); I++)
		if (((ZESector*)Sectors[I])->GetGUID() == Id)
			return (ZESector*)Sectors[I];

	return NULL;
}

void ZESector::SetAdjacencyDepth(ZEInt8 Value)
{
	AdjacencyDepth = Value;
}

ZEInt8 ZESector::GetAdjacencyDepth() const
{
	return AdjacencyDepth;
}

bool ZESector::Save(ZEMLWriterNode* Serializer)
{
	if (SectorFile.IsEmpty())
	{
		ZEPathInfo SceneFilePath(Serializer->GetFile()->GetPath());
		ZEPathInfo SceneFolderPath(SceneFilePath.GetParentDirectory());
		SetSectorFile("#R:" + SceneFolderPath.GetRelativeTo(ZEPathManager::GetInstance()->GetResourcePath().ToCString()) + ZEFormat::Format("/{0}.ZESector", GetName()));
	}

	ZEFile CurrentSectorFile;
	if (!CurrentSectorFile.Open(SectorFile, ZE_FOM_WRITE, ZE_FCM_OVERWRITE))
	{
		zeError("Serialization of entity \"%s\" has failed. Cannot write to sector file. File Name: \"%s\".", GetName(), SectorFile.ToCString());
		return false;
	}

	ZEMLWriterNode PropertiesNode;
	Serializer->OpenNode("Properties", PropertiesNode);
	PropertiesNode.WriteString("SectorFile", SectorFile.ToCString());
	PropertiesNode.CloseNode();

	ZEMLWriter SectorWriter;
	SectorWriter.Open(&CurrentSectorFile);
	ZEMLWriterNode SectorNode;
	SectorWriter.OpenRootNode("Sector", SectorNode);

	bool Result = SaveSector(&SectorNode);

	SectorNode.CloseNode();

	return Result;
}

bool ZESector::Restore(ZEMLReaderNode* Unserializer)
{
	ZEMLReaderNode PropertiesNode = Unserializer->GetNode("Properties");
	SetSectorFile(PropertiesNode.ReadString("SectorFile"));

	ZEFile CurrentSectorFile;
	if (!CurrentSectorFile.Open(SectorFile, ZE_FOM_READ, ZE_FCM_NONE))
	{
		zeError("Unserialization of entity \"%s\" has failed. Cannot open sector file. File Name: \"%s\".", GetName(), SectorFile.ToCString());
		return false;
	}

	ZEMLReader SectorReader;
	SectorReader.Open(&CurrentSectorFile);
	ZEMLReaderNode SectorNode = SectorReader.GetRootNode();

	return RestoreSector(&SectorNode);
}

void ZESector::Destroy()
{
	delete this;
}

ZESector* ZESector::CreateInstance()
{
	return new ZESector();
}



