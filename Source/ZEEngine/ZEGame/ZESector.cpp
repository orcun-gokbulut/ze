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
#include "ZESectorManager.h"
#include "ZEML\ZEMLWriter.h"
#include "ZEML\ZEMLReader.h"
#include "ZEFile\ZEPathInfo.h"
#include "ZEFile\ZEPathManager.h"
#include "ZEDS\ZEFormat.h"
#include "ZEDS\ZEVariant.h"
#include "ZEMeta\ZEProvider.h"

ZEEntityResult ZESector::LoadInternal()
{
	if (SectorFile.IsEmpty())
		return ZE_ER_DONE;

	ZEFile CurrentSectorFile;
	if (!CurrentSectorFile.Open(SectorFile, ZE_FOM_READ, ZE_FCM_NONE))
	{
		zeError("ZESector Load failed. Cannot open sector file. File Name: \"%s\".", SectorFile.ToCString());
		return ZE_ER_FAILED;
	}

	ZEMLReader SectorReader;
	SectorReader.Open(&CurrentSectorFile);
	ZEMLReaderNode SectorNode = SectorReader.GetRootNode();

	zeCheckError(!SectorNode.IsValid(), ZE_ER_FAILED, "ZESector Load failed. Corrupt ZESector file. File Name: \"%s\".", SectorFile.ToCString());

	if (LoadingIndex == 0) // On initial load pass
	{
		ZEGUID Value;
		Value.FromString(SectorNode.ReadString("GUID"));

		if (Value == ZEGUID())
			SetGUID(ZEGUID::Generate());
		else
			SetGUID(Value);

		ZEMLReaderNode BoundingBoxNode = SectorNode.GetNode("BoundingBox");

		zeCheckError(!BoundingBoxNode.IsValid(), ZE_ER_FAILED, "ZESector Load failed. Corrupt ZESector file. File Name: \"%s\".", SectorFile.ToCString());

		ZEAABBox BoundingBox(BoundingBoxNode.ReadVector3("Min"), BoundingBoxNode.ReadVector3("Max"));
		SetBoundingBox(BoundingBox);

		ZEMLReaderNode AdjacentSectorsNode = SectorNode.GetNode("AdjacentSectors");

		zeCheckError(!AdjacentSectorsNode.IsValid(), ZE_ER_FAILED, "ZESector Load failed. Corrupt ZESector file. File Name: \"%s\".", SectorFile.ToCString());

		ZESize AdjacentSectorCount = AdjacentSectorsNode.GetNodeCount();
		AdjacentSectorIds.SetCount(AdjacentSectorCount);

		for (ZESize I = 0; I < AdjacentSectorCount; I++)
			AdjacentSectorIds[I].FromString(AdjacentSectorsNode.GetNode(I).ReadString("GUID"));
	
		ZEMLReaderNode PropertiesNode = SectorNode.GetNode("Properties");

		zeCheckError(!PropertiesNode.IsValid(), ZE_ER_FAILED, "ZESector Load failed. Corrupt ZESector file. File Name: \"%s\".", SectorFile.ToCString());

		const ZEArray<ZEMLFormatElement>& Elements = PropertiesNode.GetElements();

		for (ZESize I = 0; I < Elements.GetCount(); I++)
		{
			if (Elements[I].ElementType != ZEML_ET_PROPERTY)
				continue;

			if (!GetClass()->SetProperty(this, Elements[I].Name, ZEVariant(Elements[I].Value)))
				zeWarning("Cannot restore property. Entity: \"%s\", Property: \"%s\".", GetClass()->GetName(), Elements[I].Name.ToCString());
		}

	}

	ZEMLReaderNode SubEntitiesNode = SectorNode.GetNode("ChildEntities");
	zeCheckError(!SubEntitiesNode.IsValid(), ZE_ER_FAILED, "ZESector Load failed. Corrupt ZESector file. File Name: \"%s\".", SectorFile.ToCString());

	ZESize ChildNodeCount = SubEntitiesNode.GetNodeCount("Entity");
	zeCheckError(LoadingIndex >= ChildNodeCount, ZE_ER_FAILED, "ZESector Load failed. ZESector child loading index greater than child count. Sector Name: \"%s\".", GetName());

	ZESize ChildLoadPassCount = 5;
	ZEClass* NewChildEntityClass = NULL;
	ZEEntity* NewChildEntity = NULL;

	while (ChildLoadPassCount != 0)
	{
		if (LoadingIndex < ChildNodeCount)
		{
			ZEMLReaderNode ChildEntityNode = SubEntitiesNode.GetNode("Entity", LoadingIndex);

			NewChildEntityClass = ZEProvider::GetInstance()->GetClass(ChildEntityNode.ReadString("Class"));
			zeCheckError(NewChildEntityClass == NULL, ZE_ER_FAILED, "ZESector Load failed. ZESector child entity class is unknown. Sector Name: \"%s\".", GetName());

			NewChildEntity = (ZEEntity*)NewChildEntityClass->CreateInstance();

			if (NewChildEntity == NULL)
			{
				zeError("ZESector Load failed. Cannot create instance of a child entity. Class Name: \"%s\".", ChildEntityNode.ReadString("Class").ToCString());
				NewChildEntity->Destroy();
				return ZE_ER_FAILED;
			}

			if (!NewChildEntity->Unserialize(&ChildEntityNode))
			{
				zeError("ZESector Load failed. Unserialization of child entity has failed. Class Name: \"%s\".", ChildEntityNode.ReadString("Class").ToCString());
				NewChildEntity->Destroy();
				return ZE_ER_FAILED;
			}

			if (!AddChildEntity(NewChildEntity))
			{
				zeError("ZESector Load failed. Cannot add child entity. Class Name: \"%s\".", ChildEntityNode.ReadString("Class").ToCString());
				NewChildEntity->Destroy();
				return ZE_ER_FAILED;
			}
		
			LoadingIndex++;
			ChildLoadPassCount--;
		}
		else
		{
			LoadingIndex++;
			return ZE_ER_DONE;
		}
	}

	return ZE_ER_WAIT;
}

ZEEntityResult ZESector::UnloadInternal()
{
	ClearChildEntities();
	LoadingIndex = 0;
	return ZE_ER_DONE;
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
		CurrentSector = static_cast<ZESectorManager*>(GetParent())->GetSector(SectorIds[I]);

		if (CheckAdjacency(CurrentSector, (Depth - 1)))
			return true;
	}

	return false;
}

ZESector::ZESector()
{
	LoadingIndex = 0;
	AdjacencyDepth = 1;
}

void ZESector::SetGUID(const ZEGUID& GUID)
{
	this->GUID = GUID;
}

const ZEGUID& ZESector::GetGUID() const
{
	return GUID;
}

void ZESector::SetSectorFile(const ZEString& FilePath)
{
	if (SectorFile.Hash() == FilePath.Hash())
		return;

	if (IsLoaded())
		Unload();

	SectorFile = FilePath;

	Load();
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
	if (Sector == this)
		return false;

	if (Sector == NULL)
		return false;

	if (AdjacentSectorIds.Exists(Sector->GetGUID()))
		return false;

	ZE_LOCK_SECTION(SectorLock)
	{
		AdjacentSectorIds.Add(Sector->GetGUID());
	}

	return true;
}

bool ZESector::RemoveAdjacentSector(ZESector* Sector)
{
	if (Sector == this)
		return false;

	if (Sector == NULL)
		return false;

	if (!AdjacentSectorIds.Exists(Sector->GetGUID()))
		return false;

	ZE_LOCK_SECTION(SectorLock)
	{
		AdjacentSectorIds.RemoveValue(Sector->GetGUID());
	}

	return true;
}

void ZESector::SetAdjacencyDepth(ZEInt8 Value)
{
	AdjacencyDepth = Value;
}

ZEInt8 ZESector::GetAdjacencyDepth() const
{
	return AdjacencyDepth;
}

bool ZESector::Save()
{
	ZEFile CurrentSectorFile;
	if (!CurrentSectorFile.Open(SectorFile, ZE_FOM_WRITE, ZE_FCM_OVERWRITE))
	{
		zeError("Cannot write to sector file. File Name: \"%s\".", SectorFile.ToCString());
		return false;
	}

	ZEMLWriter SectorWriter;
	SectorWriter.Open(&CurrentSectorFile);
	ZEMLWriterNode SectorNode;
	SectorWriter.OpenRootNode("Sector", SectorNode);

	SectorNode.WriteString("GUID", GetGUID().ToString());

	ZEMLWriterNode BoundingBoxNode;
	SectorNode.OpenNode("BoundingBox", BoundingBoxNode);
	BoundingBoxNode.WriteVector3("Max", GetBoundingBox().Max);
	BoundingBoxNode.WriteVector3("Min", GetBoundingBox().Min);
	BoundingBoxNode.CloseNode();

	ZEMLWriterNode AdjacentSectorsNode;
	SectorNode.OpenNode("AdjacentSectors", AdjacentSectorsNode);

	for (ZESize I = 0; I < AdjacentSectorIds.GetCount(); I++)
	{
		ZEMLWriterNode AdjSectorNode;
		AdjacentSectorsNode.OpenNode("Sector", AdjSectorNode);
		AdjSectorNode.WriteString("GUID", AdjacentSectorIds[I].ToString());
		AdjSectorNode.CloseNode();
	}

	AdjacentSectorsNode.CloseNode();

	ZEMLWriterNode PropertiesNode; 
	SectorNode.OpenNode("Properties", PropertiesNode);

	const ZEProperty* Properties = GetClass()->GetProperties();

	for (ZESize I = 0; I < GetClass()->GetPropertyCount(); I++)
	{
		const ZEProperty* Current = &Properties[I];
		if (Current->Type.ContainerType != ZE_CT_NONE)
			continue;

		if (Current->Type.TypeQualifier != ZE_TQ_VALUE)
			continue;

		if (Current->Type.Type == ZE_TT_OBJECT || Current->Type.Type == ZE_TT_OBJECT_PTR)
			continue;

		if ((Current->Access & ZEMT_PA_READ_WRITE) != ZEMT_PA_READ_WRITE)
			continue;

		ZEVariant Variant;
		GetClass()->GetProperty(this, Current->ID, Variant);

		ZEValue Value = Variant.GetValue();
		if (Value.IsNull())
			continue;

		PropertiesNode.WriteValue(Current->Name, Value);
	}

	PropertiesNode.CloseNode();

	const ZEArray<ZEEntity*>& SectorChildEntities = GetChildEntities();

	if (SectorChildEntities.GetCount() != 0)
	{
		ZEMLWriterNode SubEntitiesNode, EntityNode;
		SectorNode.OpenNode("ChildEntities", SubEntitiesNode);

		for (ZESize I = 0; I < SectorChildEntities.GetCount(); I++)
		{
			SubEntitiesNode.OpenNode("Entity", EntityNode);
			EntityNode.WriteString("Class", SectorChildEntities[I]->GetClass()->GetName());
			SectorChildEntities[I]->Serialize(&EntityNode);
			EntityNode.CloseNode();
		}

		SubEntitiesNode.CloseNode();
	}

	SectorNode.CloseNode();

	return true;
}

bool ZESector::Serialize(ZEMLWriterNode* Serializer)
{
	ZEMLWriterNode PropertiesNode;
	Serializer->OpenNode("Properties", PropertiesNode);
	PropertiesNode.WriteString("SectorFile", SectorFile.ToCString());
	PropertiesNode.CloseNode();

	return true;
}

bool ZESector::Unserialize(ZEMLReaderNode* Unserializer)
{
	ZEMLReaderNode PropertiesNode = Unserializer->GetNode("Properties");
	SetSectorFile(PropertiesNode.ReadString("SectorFile"));

	return true;
}

void ZESector::Destroy()
{
	delete this;
}

ZESector* ZESector::CreateInstance()
{
	return new ZESector();
}



