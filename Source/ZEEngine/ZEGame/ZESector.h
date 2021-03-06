//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESector.h
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

#include "ZEGame/ZEGeographicEntity.h"
#include "ZEFoundation/ZEGUID.h"

class ZESectorManager;

struct ZESectorLink
{
	ZEGUID		Id;
	ZEUInt32	Depth;
};

class ZESector : public ZEGeographicEntity
{
	friend class ZESectorManager;
	ZE_OBJECT
	protected:
		ZEGUID								GUID;
		ZEString							SectorFile;
		ZESectorManager*					Manager;
		ZELock								SectorLock;
		ZEArray<ZESectorLink>				SectorLinks;

		bool								IsSectorFileLoaded;

		bool								CheckLinkInternal(ZESector* TargetSector, ZEUInt32 Depth, bool DepthCheck);
		void								SetManager(ZESectorManager* Manager);				

		virtual ZEEntityResult				LoadInternal();
		virtual ZEEntityResult				UnloadInternal();

											ZESector();
		virtual								~ZESector();
	
	public:
		ZESectorManager*					GetSectorManager() const;

		void								SetGUID(const ZEGUID& GUID);
		const ZEGUID&						GetGUID() const;

		void								SetSectorFile(const ZEString& FilePath);
		const ZEString&						GetSectorFile() const;

		ZESectorManager*					GetManager() const;
		bool								CheckLink(ZESector* TargetSector);

		const ZEArray<ZESectorLink>&		GetSectorLinks() const;
		bool								AddLink(const ZESector* Sector, ZEUInt32 Depth);
		bool								AddLink(const ZEGUID& SectorId, ZEUInt32 Depth);
		bool								RemoveLink(const ZESector* Sector);
		bool								RemoveLink(const ZEGUID& SectorId);
		
		bool								Save();
		bool								Save(const ZEString& FileName);

		virtual bool						Serialize(ZEMLWriterNode* Serializer);
		virtual bool						Unserialize(ZEMLReaderNode* Unserializer);

		static ZESector*					CreateInstance();
};
