//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEITChecker.cpp
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

#include "ZEITChecker.h"
#include "ZEML\ZEMLReader.h"

ZEITChecker::ZEITChecker()
{
	Result = ZEIT_CR_NOT_CHECKED;
}

const ZEArray<ZEITRecord>& ZEITChecker::GetRecords() const
{
	return Records;
}

void ZEITChecker::SetIntegrityFile(const ZEString& FileName)
{
	IntegrityFile = FileName;
}

const ZEString& ZEITChecker::GetIntegrityFile() const
{
	return IntegrityFile;
}

ZEITCheckResult ZEITChecker::GetResult() const
{
	return Result;
}

void ZEITChecker::CheckStart()
{
	Result = ZEIT_CR_NOT_CHECKED;
	for (ZESize I = 0; I < Records.GetCount(); I++)
		Records[I].Reset();
}

bool ZEITChecker::Check(ZESize Index)
{
	if (Index >= Records.GetCount())
		return false;

	bool CheckResult = Records[Index].Check();
	
	if (Records[Index].GetResult() > Result)
		Result = Records[Index].GetResult();

	return CheckResult;
}

bool ZEITChecker::Load()
{
	ZEMLReader Reader;
	if (!Reader.Open(IntegrityFile))
	{
		zeError("Cannot read Integrity file. File Name: \"%s\".", IntegrityFile.ToCString());
		return false;
	}

	ZEMLReaderNode RootNode = Reader.GetRootNode();
	if (RootNode.GetName() != "ZEITIntegrity")
	{
		zeError("Unknown Integrity file format. File Name: \"%s\".", IntegrityFile.ToCString());
		return false;
	}

	ZEMLReaderNode RecordsNode = RootNode.GetNode("Records");

	ZESize RecordCount = RecordsNode.GetNodeCount("Record");
	Records.SetCount(RecordCount);
	for (ZESize I = 0; I < RecordCount; I++)
	{
		ZEMLReaderNode RecordNode = RecordsNode.GetNode("Record", I);
		Records[I].Load(&RecordNode);
	}

	return true;
}
