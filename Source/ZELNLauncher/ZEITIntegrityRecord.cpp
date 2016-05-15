//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEITIntegrityRecord.cpp
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

#include "ZEITIntegrityRecord.h"
#include "ZEML\ZEMLReader.h"
#include "ZEML\ZEMLWriter.h"
#include "ZETypes.h"
#include <sha.h>
#include "ZEBase64.h"
#include "ZEFile\ZEFileInfo.h"

ZEString ZEITIntegrityRecord::CalculateChecksum(ZEFile* File)
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

void ZEITIntegrityRecord::SetPath(const ZEString& Path)
{
	this->Path = Path;
}

const ZEString& ZEITIntegrityRecord::GetPath() const
{
	return Path;
}

void ZEITIntegrityRecord::SetType(ZEITIntegrityRecordType Type)
{
	this->Type = Type;
}

ZEITIntegrityRecordType ZEITIntegrityRecord::GetType() const
{
	return Type;
}

void ZEITIntegrityRecord::SetRequired(bool Required)
{
	this->Required = Required;
}

bool ZEITIntegrityRecord::GetRequired() const
{
	return Required;
}


void ZEITIntegrityRecord::SetChecksum(const ZEString& CheckSum)
{
	this->Checksum = CheckSum;
}

const ZEString& ZEITIntegrityRecord::GetChecksum() const
{
	return Checksum;
}

ZEITIntegrityResult ZEITIntegrityRecord::GetResult() const
{
	return Result;
}

bool ZEITIntegrityRecord::Check()
{
	ZEFile File;
	if (!File.Open(Path, ZE_FOM_READ, ZE_FCM_NONE))
	{
		Result = ZEIT_CR_MISSING;
		if (GetRequired())
			return true;
		else
			return false;
	}
	
	ZEString CheckSumFile = CalculateChecksum(&File);
	if (!CheckSumFile.EqualsIncase(GetChecksum()))
	{
		File.Close();
		Result = ZEIT_CR_CHECKSUM_FAILED;
		if (GetRequired())
			return false;
		else
			return true;
	}

	Result = ZEIT_CR_SUCCESS;
	File.Close();
}

bool ZEITIntegrityRecord::Generate()
{
	ZEFileInfo Info(GetPath());
	Path = Info.Normalize();

	ZEFile File;
	if (!File.Open(Path, ZE_FOM_READ, ZE_FCM_NONE))
			return false;

	Checksum = CalculateChecksum(&File);

	File.Close();
}

void ZEITIntegrityRecord::Reset()
{
	Type = ZEIT_RT_NONE;
}

bool ZEITIntegrityRecord::Load(ZEMLReaderNode* RecordNode)
{
	SetPath(RecordNode->ReadString("Path"));
	SetType((ZEITIntegrityRecordType)RecordNode->ReadUInt8("Type"));
	SetRequired(RecordNode->ReadBoolean("Required"));
	SetChecksum(RecordNode->ReadString("Checksum"));
	Reset();

	return true;
}

bool ZEITIntegrityRecord::Save(ZEMLWriterNode* RecordsNode)
{
	ZEMLWriterNode RecordNode;
	if (!RecordsNode->OpenNode("Record", RecordNode))
		return false;

	RecordNode.WriteString("Path", GetPath());
	RecordNode.WriteUInt8("Type", GetType());
	RecordNode.WriteBool("Required", GetRequired());
	RecordNode.WriteString("Checksum", GetChecksum());

	RecordNode.CloseNode();

	return true;
}

ZEITIntegrityRecord::ZEITIntegrityRecord()
{
	Type = ZEIT_RT_NONE;
	Required = false;
	Result = ZETI_CR_NOT_CHECKED;
}
