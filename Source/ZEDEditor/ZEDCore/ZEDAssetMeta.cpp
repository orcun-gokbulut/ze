//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDAssetMeta.cpp
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


#include "ZEDAssetMeta.h"
#include "ZEML\ZEMLReader.h"
#include "ZEML\ZEMLWriter.h"

void ZEDAssetMetaData::SetGUID(const ZEGUID& GUID)
{
	this->GUID = GUID;
}

const ZEGUID& ZEDAssetMetaData::GetGUID() const
{
	return GUID;
}

void ZEDAssetMetaData::SetName(const ZEString& Name)
{
	this->Name = Name;
}

const ZEString& ZEDAssetMetaData::GetName() const
{
	return Name;
}

void ZEDAssetMetaData::SetPackageName(const ZEString& PackageName)
{
	this->PackageName = PackageName;
}

const ZEString& ZEDAssetMetaData::GetPackageName() const
{
	return PackageName;
}

void ZEDAssetMetaData::SetVersionMajor(ZEUInt Major)
{
	this->VersionMajor = Major;
}

ZEUInt ZEDAssetMetaData::GetVersionMajor() const
{
	return VersionMajor;
}

void ZEDAssetMetaData::SetVersionMinor(ZEUInt Minor)
{
	this->VersionMinor = Minor;
}

ZEUInt ZEDAssetMetaData::GetVersionMinor() const
{
	return VersionMinor;
}

void ZEDAssetMetaData::SetVersionRevision(ZEUInt Revision)
{
	this->VersionRevision = Revision;
}

ZEUInt ZEDAssetMetaData::GetVersionRevision() const
{
	return VersionRevision;
}

void ZEDAssetMetaData::SetCreationTime(ZETimeStamp TimeStamp)
{
	CreationTime = TimeStamp;
}

ZETimeStamp ZEDAssetMetaData::GetCreationTime() const
{
	return CreationTime;
}

void ZEDAssetMetaData::SetModificationTime(ZETimeStamp TimeStamp)
{
	ModificationTime = TimeStamp;
}

ZETimeStamp ZEDAssetMetaData::GetModificationTime() const
{
	return ModificationTime;
}

void ZEDAssetMetaData::SetCategory(const ZEString& Category)
{
	this->Category = Category;
}

const ZEString& ZEDAssetMetaData::GetCategory() const
{
	return Category;
}

void ZEDAssetMetaData::SetTags(const ZEArray<ZEString>& Tags)
{
	this->Tags = Tags;
}

const ZEArray<ZEString>& ZEDAssetMetaData::GetTags() const
{
	return Tags;
}

void ZEDAssetMetaData::SetDescription(const ZEString& Description)
{
	this->Description = Description;
}

const ZEString& ZEDAssetMetaData::GetDescription() const
{
	return Description;
}

void ZEDAssetMetaData::SetAuthor(const ZEString& Author)
{
	this->Author = Author;
}

const ZEString& ZEDAssetMetaData::GetAuthor() const
{
	return Author;
}

void ZEDAssetMetaData::SetWebsite(const ZEString& Website)
{
	this->Website = Website;
}

const ZEString& ZEDAssetMetaData::GetWebsite() const
{
	return Website;
}

void ZEDAssetMetaData::SetCopyright(const ZEString& Copyright)
{
	this->Copyright = Copyright;
}

const ZEString& ZEDAssetMetaData::GetCopyright() const
{
	return Copyright;
}

void ZEDAssetMetaData::SetLicense(const ZEString& License)
{
	this->License = License;
}

const ZEString& ZEDAssetMetaData::GetLicense() const
{
	return License;
}

void ZEDAssetMetaData::SetLicenseURL(const ZEString& URL)
{
	this->LicenseURL = URL;
}

const ZEString& ZEDAssetMetaData::GetLicenseURL() const
{
	return LicenseURL;
}

void ZEDAssetMetaData::SetApplicationName(const ZEString& Name)
{
	this->ApplicationName = Name;
}

const ZEString& ZEDAssetMetaData::GetApplicationName() const
{
	return ApplicationName;
}

void ZEDAssetMetaData::SetDependencies(const ZEArray<ZEDAssetDependency> Dependencies)
{
	this->Dependencies = Dependencies;
}

const ZEArray<ZEDAssetDependency>& ZEDAssetMetaData::GetDependencies()
{
	return Dependencies;
}

bool ZEDAssetMetaData::Load(ZEMLReaderNode* AssetNode)
{
	zeCheckError(AssetNode == NULL, false, "Cannot load asset meta data. AssetNode is NULL");
	zeCheckError(AssetNode->GetName() != "Asset", false, "Cannot load asset meta data. Unknown AssetNode name.");

	SetGUID(AssetNode->ReadString("GUID"));
	SetName(AssetNode->ReadString("Name"));
	SetPackageName(AssetNode->ReadString("PackageName"));
	SetVersionMajor(AssetNode->ReadUInt8("VersionMajor"));
	SetVersionMinor(AssetNode->ReadUInt8("VersionMinor"));
	SetVersionRevision(AssetNode->ReadUInt32("VersionRevision"));
	SetCreationTime((ZETimeStamp)AssetNode->ReadUInt32("CreationTime"));
	SetModificationTime(AssetNode->ReadUInt32("ModificationTime"));
	SetCategory(AssetNode->ReadString("Category"));
	SetDescription(AssetNode->ReadString("Description"));
	SetAuthor(AssetNode->ReadString("Author"));
	SetWebsite(AssetNode->ReadString("Website"));
	SetCopyright(AssetNode->ReadString("Copyright"));
	SetLicense(AssetNode->ReadString("License"));
	SetLicenseURL(AssetNode->ReadString("LicenseURL"));
	SetApplicationName(AssetNode->ReadString("ApplicationName"));

	ZEMLReaderNode TagsNode = AssetNode->GetNode("Tags");
	if (TagsNode.IsValid())
	{
		ZESize TagCount = TagsNode.GetNodeCount("Tag");
		Tags.SetCount(TagCount);
		for (ZESize I = 0; I < TagCount; I++)
			Tags[I] = TagsNode.ReadString("Name");
	}

	ZEMLReaderNode DependenciesNode = AssetNode->GetNode("Dependencies");
	if (DependenciesNode.IsValid())
	{
		ZESize DependencyCount = DependenciesNode.GetNodeCount("Dependency");
		Dependencies.SetCount(DependencyCount);
		for (ZESize I = 0; I < DependencyCount; I++)
			Dependencies[I].Load(&DependenciesNode.GetNode("Dependency" , I));
	}

	return true;
}

bool ZEDAssetMetaData::Load(const ZEMLNode* AssetNode)
{
	return false;
}

bool ZEDAssetMetaData::Save(ZEMLWriterNode* RootNode) const
{
	zeCheckError(RootNode == NULL, false, "Cannot save asset meta data. RootNode is NULL");

	ZEMLWriterNode AssetNode;
	if (!RootNode->OpenNode("Asset", AssetNode))
	{
		zeError("Cannot save asset meta data. Cannot open asset node.");
		return false;
	}

	AssetNode.WriteString("GUID", GetGUID().ToString());
	AssetNode.WriteString("Name", GetName());
	AssetNode.WriteString("PackageName", GetPackageName());
	AssetNode.WriteUInt8("VersionMajor", GetVersionMajor());
	AssetNode.WriteUInt8("VersionMinor", GetVersionMinor());
	AssetNode.WriteUInt32("VersionRevision", GetVersionRevision());
	AssetNode.WriteUInt64("CreationTime", GetCreationTime());
	AssetNode.WriteUInt64("ModificationTim", GetModificationTime());
	AssetNode.WriteString("Category", GetCategory());
	AssetNode.WriteString("Description", GetDescription());
	AssetNode.WriteString("Author", GetAuthor());
	AssetNode.WriteString("Website", GetWebsite());
	AssetNode.WriteString("Copyright", GetCopyright());
	AssetNode.WriteString("License", GetLicense());
	AssetNode.WriteString("LicenseURL", GetLicenseURL());
	AssetNode.WriteString("ApplicationName", GetApplicationName());

	if (Tags.GetCount() != 0)
	{
		ZEMLWriterNode TagsNode;
		if (!AssetNode.OpenNode("Tags", TagsNode))
		{
			zeError("Cannot save asset meta data. Cannot open tags node.");
			return false;
		}
		
		for (ZESize I = 0; I < Tags.GetCount(); I++)
		{
			ZEMLWriterNode TagNode;
			TagsNode.OpenNode("Tag", TagNode);
			TagNode.WriteString("Name", Tags[I]);
			TagNode.CloseNode();
		}
		TagsNode.CloseNode();

	}

	if (Dependencies.GetCount() != 0)
	{
		ZEMLWriterNode DependenciesNode;
		if (!AssetNode.OpenNode("Dependencies", DependenciesNode))
		{
			zeError("Cannot save asset meta data. Cannot open tags node.");
			return false;
		}

		for (ZESize I = 0; I < Dependencies.GetCount(); I++)
		{
			ZEMLWriterNode DependencyNode;
			DependenciesNode.OpenNode("Dependency", DependencyNode);
			DependencyNode.WriteString("Name", Tags[I]);
			DependencyNode.CloseNode();
		}
		DependenciesNode.CloseNode();
	}

	AssetNode.CloseNode();

	return true;
}

bool ZEDAssetMetaData::Save(ZEMLNode* AssetNode)
{
	return false;
}

ZEDAssetMetaData::ZEDAssetMetaData()
{
	GUID = ZEGUID::Zero;
	VersionMajor = 0;
	VersionMinor = 0;
	VersionRevision = 0;
	CreationTime = 0;
	ModificationTime = 0;
}
