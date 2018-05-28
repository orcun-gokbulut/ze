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
#include "ZEMeta\ZEMTProvider.h"

void ZESector::SetManager(ZESectorManager* Manager)
{
	this->Manager = Manager;
}

bool ZESector::CheckLinkInternal(ZESector* TargetSector, ZEUInt32 Depth, bool DepthCheck)
{
	if (TargetSector == NULL)
		return false;

	if (Manager == NULL)
		return false;

	if (this == TargetSector)
		return true;

	if (Depth == 0)
		return false;

	ZEUInt32 NextDepth = Depth - 1;

	for (ZESize I = 0; I < SectorLinks.GetCount(); I++)
	{
		ZESector* CurrentLinkedSector = Manager->GetSector(SectorLinks[I].Id);

		if (CurrentLinkedSector == NULL)
			continue;

		if (DepthCheck && SectorLinks[I].Depth < NextDepth)
			NextDepth = SectorLinks[I].Depth;
			
		if (CurrentLinkedSector->CheckLinkInternal(TargetSector, NextDepth, DepthCheck))
			return true;
	}

	return false;
}

ZEEntityResult ZESector::LoadInternal()
{
	static ZEUInt32 VegetationReductionCount = 0;

	ZE_ENTITY_LOAD_CHAIN(ZEGeographicEntity);

	if (SectorFile.IsEmpty())
		return ZE_ER_DONE;

	if (IsSectorFileLoaded)
		return ZE_ER_DONE;

	ZEMLReader SectorReader;
	zeCheckError(!SectorReader.Open(SectorFile), ZE_ER_FAILED, "ZESector Load failed. Cannot open sector file. File Name: \"%s\".", SectorFile.ToCString());
	ZEMLReaderNode SectorNode = SectorReader.GetRootNode();

	zeCheckError(!SectorNode.IsValid(), ZE_ER_FAILED, "ZESector Load failed. Corrupt ZESector file. File Name: \"%s\".", SectorFile.ToCString());

	ZEMLReaderNode SubEntitiesNode = SectorNode.GetNode("ChildEntities");
	zeCheckError(!SubEntitiesNode.IsValid(), ZE_ER_FAILED, "ZESector Load failed. Corrupt ZESector file. File Name: \"%s\".", SectorFile.ToCString());

	ZESize ChildNodeCount = SubEntitiesNode.GetNodeCount("Entity");

	for (ZESize I = 0; I < ChildNodeCount; I++)
	{
		ZEMLReaderNode ChildEntityNode = SubEntitiesNode.GetNode("Entity", I);

		ZEClass* NewChildEntityClass = ZEMTProvider::GetInstance()->GetClass(ChildEntityNode.ReadString("Class"));
		zeCheckError(NewChildEntityClass == NULL, ZE_ER_FAILED, "ZESector Load failed. ZESector child entity class is unknown. Sector Name: \"%s\", Class Name: \"%s\".", 
			GetName().ToCString(), ChildEntityNode.ReadString("Class").ToCString());

		ZEString VegetationClassName = "ZETIVegetation";

		if (NewChildEntityClass->GetName() == VegetationClassName)
		{
			ZEMLReaderNode PropertiesNode = ChildEntityNode.GetNode("Properties");
			ZEInt32 VegetationType = PropertiesNode.ReadInt32("Type");

			if (VegetationType == 7) //Grass
				continue;

			VegetationReductionCount++;

			if (VegetationReductionCount % 4 != 0)
				continue;

		}

		ZEEntity* NewChildEntity = static_cast<ZEEntity*>(NewChildEntityClass->CreateInstance());

		if (NewChildEntity == NULL)
		{
			zeError("ZESector Load failed. Cannot create instance of a child entity. Class Name: \"%s\".", ChildEntityNode.ReadString("Class").ToCString());
			NewChildEntity->Destroy();
			return ZE_ER_FAILED;
		}

		if (!NewChildEntity->Unserialize(&ChildEntityNode))
		{
			zeWarning("Unserialization of child entity has failed. Class Name: \"%s\".", ChildEntityNode.ReadString("Class").ToCString());
			NewChildEntity->Destroy();
			continue;
		}

		if (!AddChildEntity(NewChildEntity))
		{
			zeError("ZESector Load failed. Cannot add child entity. Class Name: \"%s\".", ChildEntityNode.ReadString("Class").ToCString());
			NewChildEntity->Destroy();
			return ZE_ER_FAILED;
		}
	}

	IsSectorFileLoaded = true;
	return ZE_ER_DONE;
}

ZEEntityResult ZESector::UnloadInternal()
{
	ZE_ENTITY_UNLOAD_CHAIN(ZEGeographicEntity);

	ClearChildEntities();
	IsSectorFileLoaded = false;

	return ZE_ER_DONE;
}

ZESectorManager* ZESector::GetManager() const
{
	return Manager;
}

bool ZESector::CheckLink(ZESector* TargetSector)
{
	zeCheckError(TargetSector == NULL, false, "Link check failed. Target Sector is NULL.");
	zeCheckError(Manager == NULL, false, "Link check failed. Sector Manager is NULL. Sector Name: \"%s\".", GetName());

	if (TargetSector == this)
		return true;

	for (ZESize I = 0; I < SectorLinks.GetCount(); I++)
	{
		if (SectorLinks[I].Depth == 0)
			continue;

		ZESector* CurrentLinkedSector = Manager->GetSector(SectorLinks[I].Id);

		if (CurrentLinkedSector == NULL)
			continue;

		if (CurrentLinkedSector->CheckLinkInternal(TargetSector, SectorLinks[I].Depth - 1, true))
			return true;
	}

	return false;
}

ZESector::ZESector()
{
	GUID = ZEGUID::Zero;
	Manager = NULL;
	IsSectorFileLoaded = false;
}

ZESector::~ZESector()
{
	if (Manager != NULL)
		Manager->RemoveSector(this);

	Manager = NULL;
	SectorLinks.Clear();
}

ZESectorManager* ZESector::GetSectorManager() const
{
	return Manager;
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

	SectorFile = FilePath;

	if (IsLoaded())
		Reload();
}

const ZEString& ZESector::GetSectorFile() const
{
	return SectorFile;
}

const ZEArray<ZESectorLink>& ZESector::GetSectorLinks() const
{
	return SectorLinks;
}

bool ZESector::AddLink(const ZESector* Sector, ZEUInt32 Depth)
{
	zeCheckError(Sector == NULL, false, "Cannot add sector link. Sector is NULL.");
	zeCheckError(Sector == this, false, "Cannot add sector link. Sector is same sector. Sector Name: \"%s\".", Sector->GetName());
	zeCheckError(Depth == 0, false, "Cannot add sector link. Depth must be greater than 0. Sector Name: \"%s\".", Sector->GetName());

	for (ZESize I = 0; I < SectorLinks.GetCount(); I++)
	{
		if (SectorLinks[I].Id == Sector->GetGUID())
		{
			zeError("Cannot add sector link. Sector is already added as link. Sector Name: \"%s\".", Sector->GetName());
			return false;
		}
	}

	ZE_LOCK_SECTION(SectorLock)
	{
		ZESectorLink Link;
		Link.Id = Sector->GetGUID();
		Link.Depth = Depth;
		SectorLinks.Add(Link);
	}

	return true;
}

bool ZESector::AddLink(const ZEGUID& SectorId, ZEUInt32 Depth)
{
	zeCheckError(SectorId == ZEGUID::Zero, false, "Cannot add sector link. Sector Id is NULL.");
	zeCheckError(SectorId == GetGUID(), false, "Cannot add sector link. Sector is same sector. Sector Id: \"%s\".", SectorId.ToString());
	zeCheckError(Depth == 0, false, "Cannot add sector link. Depth must be greater than 0. Sector Id: \"%s\".", SectorId.ToString());

	for (ZESize I = 0; I < SectorLinks.GetCount(); I++)
	{
		if (SectorLinks[I].Id == SectorId)
		{
			zeError("Cannot add sector link. Sector is already added as link. Sector Id: \"%s\".", SectorId.ToString());
			return false;
		}
	}

	ZE_LOCK_SECTION(SectorLock)
	{
		ZESectorLink Link;
		Link.Id = SectorId;
		Link.Depth = Depth;
		SectorLinks.Add(Link);
	}

	return true;
}

bool ZESector::RemoveLink(const ZESector* Sector)
{
	zeCheckError(Sector == NULL, false, "Cannot remove sector link. Sector is NULL.");
	zeCheckError(Sector == this, false, "Cannot remove sector link. Sector is same sector. Sector Name: \"%s\".", Sector->GetName());

	bool IsLinkExists = false;
	ZESize LinkIndex = 0;

	for (ZESize I = 0; I < SectorLinks.GetCount(); I++)
	{
		if (SectorLinks[I].Id == Sector->GetGUID())
		{
			IsLinkExists = true;
			LinkIndex = I;
		}
	}

	zeCheckError(!IsLinkExists, false, "Cannot remove sector link. Sector is not a link. Sector Name: \"%s\".", Sector->GetName());

	ZE_LOCK_SECTION(SectorLock)
	{
		SectorLinks.Remove(LinkIndex);
	}

	return true;
}

bool ZESector::RemoveLink(const ZEGUID& SectorId)
{
	zeCheckError(SectorId == ZEGUID::Zero, false, "Cannot remove sector link. Sector Id is NULL.");
	zeCheckError(SectorId == GetGUID(), false, "Cannot remove sector link. Sector is same sector. Sector Id: \"%s\".", SectorId.ToString());

	bool IsLinkExists = false;
	ZESize LinkIndex = 0;

	for (ZESize I = 0; I < SectorLinks.GetCount(); I++)
	{
		if (SectorLinks[I].Id == SectorId)
		{
			IsLinkExists = true;
			LinkIndex = I;
		}
	}

	zeCheckError(!IsLinkExists, false, "Cannot remove sector link. Sector is not a link. Sector Id: \"%s\".", SectorId.ToString());

	ZE_LOCK_SECTION(SectorLock)
	{
		SectorLinks.Remove(LinkIndex);
	}

	return true;
}

bool ZESector::Save()
{
	return Save(SectorFile);
}

bool ZESector::Save(const ZEString& FileName)
{
	ZEMLWriter SectorWriter;
	zeCheckError(!SectorWriter.Open(FileName), false, "Cannot write to sector file. File Name: \"%s\".", FileName.ToCString());
	ZEMLWriterNode SectorNode;
	SectorWriter.OpenRootNode("Sector", SectorNode);

	SectorNode.WriteUInt8("VersionMajor", 1);
	SectorNode.WriteUInt8("VersionMinor", 0);

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

	PropertiesNode.WriteString("GUID", GetGUID().ToString());

	ZEMLWriterNode BoundingBoxNode;
	PropertiesNode.OpenNode("BoundingBox", BoundingBoxNode);
	BoundingBoxNode.WriteVector3("Max", GetBoundingBox().Max);
	BoundingBoxNode.WriteVector3("Min", GetBoundingBox().Min);
	BoundingBoxNode.CloseNode();

	const ZEMTProperty* Properties = GetClass()->GetProperties();

	for (ZESize I = 0; I < GetClass()->GetPropertyCount(); I++)
	{
		const ZEMTProperty* Current = &Properties[I];
		if (Current->Type.GetCollectionType() != ZEMT_CT_NONE)
			continue;

		if (Current->Type.GetBaseQualifier() != ZEMT_TQ_VALUE)
			continue;

		if (Current->Type.GetBaseType() == ZEMT_BT_OBJECT || Current->Type.GetBaseType() == ZEMT_BT_OBJECT_PTR)
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

	ZEMLWriterNode LinksNode;
	PropertiesNode.OpenNode("Links", LinksNode);

	for (ZESize I = 0; I < SectorLinks.GetCount(); I++)
	{
		ZEMLWriterNode LinkNode;
		LinksNode.OpenNode("Link", LinkNode);
		LinkNode.WriteString("GUID", SectorLinks[I].Id.ToString());
		LinkNode.WriteUInt32("Depth", SectorLinks[I].Depth);
		LinkNode.CloseNode();
	}

	LinksNode.CloseNode();
	PropertiesNode.CloseNode();

	return true;
}

bool ZESector::Unserialize(ZEMLReaderNode* Unserializer)
{
	ZEMLReaderNode PropertiesNode = Unserializer->GetNode("Properties");

	ZEGUID Value = ZEGUID::FromString(PropertiesNode.ReadString("GUID"));

	if (Value == ZEGUID())
		SetGUID(ZEGUID::Generate());
	else
		SetGUID(Value);

	ZEMLReaderNode BoundingBoxNode = PropertiesNode.GetNode("BoundingBox");

	zeCheckError(!BoundingBoxNode.IsValid(), false, "ZESector Load failed. Corrupt ZESector file. File Name: \"%s\".", SectorFile.ToCString());

	ZEAABBox BoundingBox(BoundingBoxNode.ReadVector3("Min"), BoundingBoxNode.ReadVector3("Max"));
	SetBoundingBox(BoundingBox);

	const ZEArray<ZEMLFormatElement>& Elements = PropertiesNode.GetElements();

	for (ZESize I = 0; I < Elements.GetCount(); I++)
	{
		if (Elements[I].ElementType != ZEML_ET_PROPERTY)
			continue;

		if (!GetClass()->SetProperty(this, Elements[I].Name, ZEVariant(Elements[I].Value)))
			zeWarning("Cannot restore property. Entity: \"%s\", Property: \"%s\".", GetClass()->GetName(), Elements[I].Name.ToCString());
	}

	ZEMLReaderNode LinksNode = PropertiesNode.GetNode("Links");

	zeCheckError(!LinksNode.IsValid(), false, "ZESector Load failed. Corrupt ZESector file. File Name: \"%s\".", SectorFile.ToCString());

	ZESize LinkCount = LinksNode.GetNodeCount();
	SectorLinks.SetCount(LinkCount);

	for (ZESize I = 0; I < LinkCount; I++)
	{
		ZESectorLink Link;
		ZEMLReaderNode LinkNode = LinksNode.GetNode(I);

		Link.Id = ZEGUID::FromString(LinkNode.ReadString("GUID"));
		Link.Depth = LinkNode.ReadUInt32("Depth", 1);
		SectorLinks[I] = Link;
	}

	return true;
}

ZESector* ZESector::CreateInstance()
{
	return new ZESector();
}


