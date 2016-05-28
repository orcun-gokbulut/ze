//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEITGenerator.h
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

#include "ZEDS/ZEArray.h"
#include "ZEITRecord.h"
#include "ZEThread/ZELock.h"

class ZEITScannerEntry
{
	private:
		ZEString							Path;
		bool								Enabled;
		bool								Recursive;

	public:
		void								SetPath(const ZEString& Path);
		const ZEString&						GetPath() const;

		void								SetEnabled(bool Enabled);
		bool								GetEnabled() const;

		void								SetRecursive(bool Recursive);
		bool								GetRecursive() const;

											ZEITScannerEntry();
};

class ZEITGenerator
{
	private:
		ZELock								Lock;
		ZEArray<ZEITRecord>					Records;
		ZEArray<ZEITScannerEntry>			Includes;
		ZEArray<ZEITScannerEntry>			Excludes;
		bool								DiscardDisabled;

		bool								CheckExcluded(const ZEString& Path) const;
		void								ScanDirectory(const ZEString& Path, bool Recursive);

	public:
		const ZEArray<ZEITRecord>&			GetRecords() const;
		ZEITRecord&							GetRecord(ZESize Index);
		const ZEITRecord&					GetRecord(ZESize Index) const;
		void								AddRecord(const ZEITRecord& Entry);
		void								RemoveRecord(ZESize Index);

		const ZEArray<ZEITScannerEntry>&	GetIncludes() const;
		ZEITScannerEntry&					GetInclude(ZESize Index);
		const ZEITScannerEntry&				GetInclude(ZESize Index) const;
		void								AddInclude(const ZEITScannerEntry& Entry);
		void								RemoveInclude(ZESize Index);

		const ZEArray<ZEITScannerEntry>&	GetExcludes() const;
		ZEITScannerEntry&					GetExclude(ZESize Index);
		const ZEITScannerEntry&				GetExclude(ZESize Index) const;
		void								AddExclude(const ZEITScannerEntry& Entry);
		void								RemoveExclude(ZESize Index);

		void								SetDiscardDisabled(bool Discard);
		bool								GetDiscardDisabled() const;

		void								Scan();
		void								Generate();

		bool								LoadGeneratorFile(const ZEString& FileName);
		bool								SaveGeneratorFile(const ZEString& FileName) const;

		bool								LoadIntegrityFile(const ZEString& FileName);
		bool								SaveIntegrityFile(const ZEString& FileName) const;

											ZEITGenerator();
};
