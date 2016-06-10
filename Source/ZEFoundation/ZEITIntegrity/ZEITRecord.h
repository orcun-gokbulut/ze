//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEITRecord.h
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

#include "ZEDS/ZEString.h"

class ZEFile;
class ZEMLReaderNode;
class ZEMLWriterNode;

enum ZEITRecordType
{
	ZEIT_RT_NONE,
	ZEIT_RT_FILE,
	ZEIT_RT_DIRECTORY
};

enum ZEITCheckResult
{
	ZEIT_CR_NOT_CHECKED,
	ZEIT_CR_SUCCESS,
	ZEIT_CR_WARNING,
	ZEIT_CR_ERROR
};

enum ZEITCheckProblem
{
	ZEIT_CP_NONE,
	ZEIT_CP_MISSING,
	ZEIT_CP_FILE_SIZE,
	ZEIT_CP_CHECKSUM
};

class ZEITRecord
{
	private:
		ZEString						Path;
		ZEITRecordType					Type;
		bool							Enabled;
		bool							Required;
		ZEUInt64						FileSize;
		ZEString						Checksum;
		ZEITCheckResult					Result;
		ZEITCheckProblem				Problem;
	
		ZEString						CalculateChecksum(ZEFile* File);

	public:
		void							SetPath(const ZEString& Path);
		const ZEString&					GetPath() const;

		void							SetType(ZEITRecordType Type);
		ZEITRecordType					GetType() const;

		void							SetEnabled(bool Enabled);
		bool							GetEnabled() const;

		void							SetRequired(bool Required);
		bool							GetRequired() const;

		void							SetFileSize(ZEUInt64 Size);
		ZEUInt64						GetFileSize() const;

		void							SetChecksum(const ZEString& CheckSum);
		const ZEString&					GetChecksum() const;

		ZEITCheckResult					GetResult() const;
		ZEITCheckProblem				GetProblem() const;

		bool							Check();
		bool							CheckExists();
		bool							Generate();
		void							Reset();

		bool							Load(ZEMLReaderNode* RecordNode);
		bool							Save(ZEMLWriterNode* RecordsNode) const;

										ZEITRecord();
};