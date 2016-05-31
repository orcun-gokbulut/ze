//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEITRecord.cpp
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

#include "ZEITRecord.h"
#include "ZEML\ZEMLReader.h"
#include "ZEML\ZEMLWriter.h"
#include "ZETypes.h"
#include <sha.h>
#include "ZEBase64.h"
#include "ZEFile\ZEFileInfo.h"

ZEString ZEITRecord::CalculateChecksum(ZEFile* File)
{
	CryptoPP::SHA SHA;
	ZEBYTE Digest[CryptoPP::SHA::DIGESTSIZE];

	const ZESize BufferSize = 16 * ZE_SIZE_KB;
	ZEBYTE Buffer[BufferSize];
	ZESize ReadedBytes;
	do
	{
		ReadedBytes = File->Read(Buffer, 1, BufferSize);
		SHA.Update(Buffer, ReadedBytes);
	} while(ReadedBytes == BufferSize);

	SHA.Final(Digest);

	char Hex[64];
	char* Current = Hex;
	for (ZESize I = 0; I < CryptoPP::SHA::DIGESTSIZE; I++)
		Current += sprintf(Current, "%02X", Digest[I]);

	return Hex;
}

void ZEITRecord::SetPath(const ZEString& Path)
{
	this->Path = Path;
}

const ZEString& ZEITRecord::GetPath() const
{
	return Path;
}

void ZEITRecord::SetType(ZEITRecordType Type)
{
	this->Type = Type;
}

ZEITRecordType ZEITRecord::GetType() const
{
	return Type;
}

void ZEITRecord::SetEnabled(bool Enabled)
{
	this->Enabled = Enabled;
}

bool ZEITRecord::GetEnabled() const
{
	return Enabled;
}

void ZEITRecord::SetRequired(bool Required)
{
	this->Required = Required;
}

bool ZEITRecord::GetRequired() const
{
	return Required;
}

void ZEITRecord::SetFileSize(ZESize Size)
{
	FileSize = Size;
}

ZESize ZEITRecord::GetFileSize() const
{
	return FileSize;
}

void ZEITRecord::SetChecksum(const ZEString& CheckSum)
{
	this->Checksum = CheckSum;
}

const ZEString& ZEITRecord::GetChecksum() const
{
	return Checksum;
}

ZEITCheckResult ZEITRecord::GetResult() const
{
	return Result;
}

ZEITCheckProblem ZEITRecord::GetProblem() const
{
	return Problem;
}

bool ZEITRecord::Check()
{
	if (!GetEnabled())
		return true;

	ZEFile File;
	if (!File.Open(Path, ZE_FOM_READ, ZE_FCM_NONE))
	{
		Problem = ZEIT_CP_MISSING;

		if (GetRequired())
		{
			Result = ZEIT_CR_ERROR;
			return false;
		}
		else
		{
			Result = ZEIT_CR_WARNING;
			return true;
		}
	}

	if (File.GetSize() != FileSize)
	{
		File.Close();

		Problem = ZEIT_CP_FILE_SIZE;

		if (GetRequired())
		{
			Result = ZEIT_CR_ERROR;
			return false;
		}
		else
		{
			Result = ZEIT_CR_WARNING;
			return true;
		}
	}

	ZEString CheckSumFile = CalculateChecksum(&File);
	File.Close();

	if (!CheckSumFile.EqualsIncase(GetChecksum()))
	{
		Problem = ZEIT_CP_CHECKSUM;

		if (GetRequired())
		{
			Result = ZEIT_CR_ERROR;
			return false;
		}
		else
		{
			Result = ZEIT_CR_WARNING;
			return true;
		}
	}

	Result = ZEIT_CR_SUCCESS;
	Problem = ZEIT_CP_NONE;
	
	return true;
}

bool ZEITRecord::CheckExists()
{
	if (!ZEFileInfo(GetPath()).IsExists())
	{
		Problem = ZEIT_CP_MISSING;
		if (Required)
		{
			Result = ZEIT_CR_ERROR;
			return false;
		}
		else
		{
			Result = ZEIT_CR_WARNING;
			return true;
		}
	}

	return true;
}

bool ZEITRecord::Generate()
{
	if (!GetEnabled())
		return true;

	ZEFileInfo Info(GetPath());
	Path = Info.Normalize();

	ZEFile File;
	if (!File.Open(Path, ZE_FOM_READ, ZE_FCM_NONE))
	{
		Result = ZEIT_CR_ERROR;
		Problem = ZEIT_CP_MISSING;
		return false;
	}

	FileSize = File.GetSize();
	Checksum = CalculateChecksum(&File);

	File.Close();

	Result = ZEIT_CR_SUCCESS;
	Problem = ZEIT_CP_NONE;

	return true;
}

void ZEITRecord::Reset()
{
	Result = ZEIT_CR_NOT_CHECKED;
	Problem = ZEIT_CP_NONE;
}

bool ZEITRecord::Load(ZEMLReaderNode* RecordNode)
{
	SetPath(RecordNode->ReadString("Path"));
	SetType((ZEITRecordType)RecordNode->ReadUInt8("Type", ZEIT_RT_FILE));
	SetEnabled(RecordNode->ReadBoolean("Enabled", true));
	SetRequired(RecordNode->ReadBoolean("Required", true));
	SetFileSize(RecordNode->ReadUInt64("FileSize"));
	SetChecksum(RecordNode->ReadString("Checksum"));
	Reset();

	return true;
}

bool ZEITRecord::Save(ZEMLWriterNode* RecordsNode) const
{
	ZEMLWriterNode RecordNode;
	if (!RecordsNode->OpenNode("Record", RecordNode))
		return false;

	RecordNode.WriteString("Path", GetPath());
	RecordNode.WriteUInt8("Type", GetType());
	RecordNode.WriteBool("Enabled", GetEnabled());
	RecordNode.WriteBool("Required", GetRequired());
	RecordNode.WriteUInt64("FileSize", GetFileSize());
	RecordNode.WriteString("Checksum", GetChecksum());

	RecordNode.CloseNode();

	return true;
}

ZEITRecord::ZEITRecord()
{
	Type = ZEIT_RT_NONE;
	Required = true;
	Enabled = true;
	FileSize = 0;
	Result = ZEIT_CR_NOT_CHECKED;
	Problem = ZEIT_CP_NONE;
}
