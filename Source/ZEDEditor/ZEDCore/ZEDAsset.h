//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDAsset.h
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

#include "ZEMeta\ZEObject.h"

#include "ZEDS\ZEArray.h"
#include "ZEDS\ZEString.h"
#include "ZEDS\ZELink.h"
#include "ZETimeStamp.h"


class ZEDAssetType;
class ZEDAssetManager;

class ZEDAssetMetaData
{
	public:
		ZEGUID							GUID;
		ZEUInt							VersionMinor;
		ZEUInt							VersionMajor;
		ZEUInt							VersionRevision;
		ZETimeStamp						CreationDate;
		ZEString						Description;
		ZEString						Author;
		ZEString						Copyright;
		ZEString						WebSite;
		ZEString						ProgramName;
};

class ZEDAsset : public ZEObject
{
	ZE_OBJECT
	friend class ZEDAssetManager;
	private:
		ZEDAssetType*					Type;
		ZEString						Path;
		ZEInt							Hash;
		ZEString						Category;
		ZEArray<ZEString>				Tags;
		ZETimeStamp						ModificationTime;

	public:
		void							SetPath(const ZEString& Path);
		ZEString						GetPath() const;
		ZEUInt							GetHash() const;

		void							SetType(ZEDAssetType* Type);
		ZEDAssetType*					GetType() const;
	
		void							SetCategory(const ZEString& Category);
		const ZEString&					GetCategory() const;

		void							SetTags(const ZEArray<ZEString>& Tags);
		const ZEArray<ZEString>&		GetTags() const;

		ZETimeStamp						GetModificationTime();

		ZEDAssetMetaData				ReadMetaData();

										ZEDAsset();
										~ZEDAsset();
};
