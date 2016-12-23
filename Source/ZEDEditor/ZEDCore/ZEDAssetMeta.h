//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDAssetMeta.h
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

#include "ZEGUID.h"
#include "ZETimeStamp.h"
#include "ZEDS/ZEString.h"
#include "ZEDS/ZEArray.h"
#include "ZEDAssetDependency.h"

class ZEMLNode;
class ZEMLReaderNode;
class ZEMLWriterNode;

class ZEDAssetMetaData : public ZEObject
{
	private:
		ZEGUID									GUID;
		ZEString								Name;
		ZEString								PackageName;
		ZEUInt									VersionMajor;
		ZEUInt									VersionMinor;
		ZEUInt									VersionRevision;
		ZETimeStamp								CreationTime;
		ZETimeStamp								ModificationTime;
		ZEString								Category;
		ZEArray<ZEString>						Tags;
		ZEString								Description;
		ZEString								Author;
		ZEString								Website;
		ZEString								Copyright;
		ZEString								License;
		ZEString								LicenseURL;
		ZEString								ApplicationName;
		ZEArray<ZEDAssetDependency>				Dependencies;

	public:
		void									SetGUID(const ZEGUID& GUID);
		const ZEGUID&							GetGUID() const;
		void									SetName(const ZEString& Name);
		const ZEString&							GetName() const;
		void									SetPackageName(const ZEString& PackageName);
		const ZEString&							GetPackageName() const;

		void									SetVersionMajor(ZEUInt Major);
		ZEUInt									GetVersionMajor() const;
		void									SetVersionMinor(ZEUInt Minor);
		ZEUInt									GetVersionMinor() const;
		void									SetVersionRevision(ZEUInt Revision);
		ZEUInt									GetVersionRevision() const;

		void									SetCreationTime(ZETimeStamp TimeStamp);
		ZETimeStamp								GetCreationTime() const;
		void									SetModificationTime(ZETimeStamp TimeStamp);
		ZETimeStamp								GetModificationTime() const;

		void									SetCategory(const ZEString& Category);
		const ZEString&							GetCategory() const;
		void									SetTags(const ZEArray<ZEString>& Tags);
		const ZEArray<ZEString>&				GetTags() const;
		void									SetDescription(const ZEString& Description);
		const ZEString&							GetDescription() const;
		void									SetAuthor(const ZEString& Author);
		const ZEString&							GetAuthor() const;
		void									SetWebsite(const ZEString& Website);
		const ZEString&							GetWebsite() const;
		void									SetCopyright(const ZEString& Copyright);
		const ZEString&							GetCopyright() const;
		void									SetLicense(const ZEString& License);
		const ZEString&							GetLicense() const;
		void									SetLicenseURL(const ZEString& URL);
		const ZEString&							GetLicenseURL() const;
		void									SetApplicationName(const ZEString& Name);
		const ZEString&							GetApplicationName() const;

		void									SetDependencies(const ZEArray<ZEDAssetDependency> Dependencies);
		const ZEArray<ZEDAssetDependency>&		GetDependencies();
		
		bool									Load(ZEMLReaderNode* AssetNode);
		bool									Load(const ZEMLNode* AssetNode);
		bool									Save(ZEMLWriterNode* RootNode) const;
		bool									Save(ZEMLNode* AssetNode);

												ZEDAssetMetaData();
};
