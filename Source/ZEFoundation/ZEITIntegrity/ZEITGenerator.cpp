//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEITGenerator.cpp
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

#include "ZEITGenerator.h"

#include "ZEDS\ZEFormat.h"
#include "ZEFile\ZEDirectoryInfo.h"
#include "ZEML\ZEMLWriter.h"
#include "ZEML\ZEMLFormat.h"
#include "ZEML\ZEMLReader.h"


// ZEITScannerEntry
//////////////////////////////////////////////////////////////////////////////////////

void ZEITScannerEntry::SetPath(const ZEString& Path)
{
	this->Path = Path;
}

const ZEString& ZEITScannerEntry::GetPath() const
{
	return Path;
}

void  ZEITScannerEntry::SetEnabled(bool Enabled)
{
	this->Enabled = Enabled;
}

bool ZEITScannerEntry::GetEnabled() const
{
	return Enabled;
}

void  ZEITScannerEntry::SetRecursive(bool Recursive)
{
	this->Recursive = Recursive;
}

bool ZEITScannerEntry::GetRecursive() const
{
	return Recursive;
}

ZEITScannerEntry::ZEITScannerEntry()
{
	Enabled = true;
	Recursive = true;
}


// ZEITIntegrityGenerator
//////////////////////////////////////////////////////////////////////////////////////

int WildcardCompare(const char *wild, const char *string) 
{
	// Written by Jack Handy - <A href="mailto:jakkhandy@hotmail.com">jakkhandy@hotmail.com</A>
	// Thanks Jack !!! Or�un.

	const char *cp = NULL, *mp = NULL;

	while ((*string) && (*wild != '*')) 
	{
		if ((*wild != *string) && (*wild != '?')) 
		{
			return 0;
		}

		wild++;
		string++;
	}

	while (*string) 
	{
		if (*wild == '*') 
		{
			if (!*++wild) 
			{
				return 1;
			}
			mp = wild;
			cp = string+1;
		} 
		else if ((*wild == *string) || (*wild == '?')) 
		{
			wild++;
			string++;
		} 
		else 
		{
			wild = mp;
			string = cp++;
		}
	}

	while (*wild == '*') 
	{
		wild++;
	}

	return !*wild;
}

bool ZEITGenerator::CheckExcluded(const ZEString& Path) const
{
	bool Result = WildcardCompare("*.pdb", "#E:/ZEDSHShaderEditorDLL.pdb");

	for (ZESize I = 0; I < Excludes.GetCount(); I++)
	{
		if (!Excludes[I].GetEnabled())
			continue;

		if (WildcardCompare(Excludes[I].GetPath(), Path) || 
			strncmp(Excludes[I].GetPath(), Path, strlen(Excludes[I].GetPath())) == 0)
		{
			return true;
		}
	}

	return false;
}

void ZEITGenerator::ScanDirectory(const ZEString& Path, bool Recursive)
{
	ZEDirectoryInfo Info(Path);
	ZEArray<ZEString> Files = Info.GetFiles();

	if (CheckExcluded(Path))
		return;
	
	for (ZESSize I = 0; I < Files.GetCount(); I++) 
	{
		ZEPathInfo Info(ZEFormat::Format("{0}/{1}", Path, Files[I]));
		ZEString NormalizedPath = Info.Normalize();

		if (CheckExcluded(NormalizedPath))
			continue;

		bool Found = false;
		for (ZESize I = 0; I < Records.GetCount(); I++)
		{
			if (Records[I].GetPath() == NormalizedPath)
			{
				Found = true;
				break;
			}
			
		}
		
		if (Found)
			continue;

		ZEITRecord Record;
		Record.SetPath(NormalizedPath);
		Record.SetRequired(true);
		Record.SetType(ZEIT_RT_FILE);
		AddRecord(Record);
	}

	if (!Recursive)
		return;

	ZEArray<ZEString> SubDirectories = Info.GetSubDirectories();
	for (ZESize I = 0; I < SubDirectories.GetCount(); I++) 
		ScanDirectory(ZEFormat::Format("{0}/{1}", Path, SubDirectories[I]), true);
}

const ZEArray<ZEITRecord>& ZEITGenerator::GetRecords() const
{
	return Records;
}

ZEITRecord& ZEITGenerator::GetRecord(ZESize Index)
{
	return Records[Index];
}

const ZEITRecord& ZEITGenerator::GetRecord(ZESize Index) const
{
	return Records[Index];
}

void ZEITGenerator::AddRecord(const ZEITRecord& Record)
{
	Lock.Lock();
	Records.Add(Record);
	Lock.Unlock();
}

void ZEITGenerator::RemoveRecord(ZESize Index)
{
	Lock.Lock();
	Records.Remove(Index);
	Lock.Unlock();
}

void ZEITGenerator::ClearRecords()
{
	Records.Clear();
}

const ZEArray<ZEITScannerEntry>& ZEITGenerator::GetIncludes() const
{
	return Includes;
}

ZEITScannerEntry& ZEITGenerator::GetInclude(ZESize Index)
{
	return Includes[Index];
}

const ZEITScannerEntry& ZEITGenerator::GetInclude(ZESize Index) const
{
	return Includes[Index];
}

void ZEITGenerator::AddInclude(const ZEITScannerEntry& Entry)
{
	Includes.Add(Entry);
}

void ZEITGenerator::RemoveInclude(ZESize Index)
{
	Includes.Remove(Index);
}

const ZEArray<ZEITScannerEntry>& ZEITGenerator::GetExcludes() const
{
	return Excludes;
}

ZEITScannerEntry& ZEITGenerator::GetExclude(ZESize Index)
{
	return Excludes[Index];
}

const ZEITScannerEntry& ZEITGenerator::GetExclude(ZESize Index) const
{
	return Excludes[Index];
}

void ZEITGenerator::AddExclude(const ZEITScannerEntry& Entry)
{
	Excludes.Add(Entry);
}

void ZEITGenerator::RemoveExclude(ZESize Index)
{
	Excludes.Remove(Index);
}

void ZEITGenerator::SetIntegrityFileName(const ZEString& FileName)
{
	IntegrityFileName = FileName;
}

const ZEString& ZEITGenerator::GetIntegrityFileName() const
{
	return IntegrityFileName;
}

void ZEITGenerator::SetDiscardDisabled(bool Discard)
{
	DiscardDisabled = Discard;
}

bool ZEITGenerator::GetDiscardDisabled() const
{
	return DiscardDisabled;
}

void ZEITGenerator::Scan()
{
	for (ZESize I = 0; I < Includes.GetCount(); I++)
	{
		if (!Includes[I].GetEnabled())
			continue;

		ScanDirectory(Includes[I].GetPath(), Includes[I].GetRecursive());
	}
}

void ZEITGenerator::GenerateStart()
{

}

bool ZEITGenerator::Generate(ZESize Index)
{
	if (Index >= Records.GetCount())
		return false;

	if (!Records[Index].GetEnabled())
		return true;

	if (!Records[Index].Generate())
		return false;

	return true;
}

void ZEITGenerator::Clear()
{
	Includes.Clear();
	Excludes.Clear();
	Records.Clear();
}

bool ZEITGenerator::LoadGeneratorFile(const ZEString& FileName)
{
	ZEMLReader Reader;
	if (!Reader.Open(FileName))
	{
		zeError("Cannot read Integrity Generator file. File Name: \"%s\".", FileName.ToCString());
		return false;
	}

	ZEMLReaderNode RootNode = Reader.GetRootNode();
	if (RootNode.GetName() != "ZEITIntegrityGenerator")
	{
		zeError("Unknown Integrity Generator file format. File Name: \"%s\".", FileName.ToCString());
		return false;
	}

	IntegrityFileName = RootNode.ReadString("IntegrityFileName");
	
	ZEMLReaderNode ScannerNode = RootNode.GetNode("Scanner");
	if (ScannerNode.IsValid())
	{
		ZEMLReaderNode IncludesNode = ScannerNode.GetNode("Includes");
		ZESize IncludesCount = IncludesNode.GetNodeCount("Include");
		Includes.SetCount(IncludesCount);
		for (ZESize I = 0; I < IncludesCount; I++)
		{
			ZEMLReaderNode IncludeNode = IncludesNode.GetNode("Include", I);
			Includes[I].SetPath(IncludeNode.ReadString("Path"));
			Includes[I].SetEnabled(IncludeNode.ReadBoolean("Enabled", true));
			Includes[I].SetRecursive(IncludeNode.ReadBoolean("Recursive", true));
		}

		ZEMLReaderNode ExcludesNode = ScannerNode.GetNode("Excludes");
		ZESize ExcludesCount = ExcludesNode.GetNodeCount("Exclude");
		Excludes.SetCount(ExcludesCount);
		for (ZESize I = 0; I < ExcludesCount; I++)
		{
			ZEMLReaderNode IncludeNode = ExcludesNode.GetNode("Exclude", I);
			Excludes[I].SetPath(IncludeNode.ReadString("Path"));
			Excludes[I].SetEnabled(IncludeNode.ReadBoolean("Enabled", true));
			Excludes[I].SetRecursive(IncludeNode.ReadBoolean("Recursive", true));
		}
	}

	ZEMLReaderNode RecordsNode = RootNode.GetNode("Records");
	ZESize RecordCount = RecordsNode.GetNodeCount("Record");
	Records.SetCount(RecordCount);
	for (ZESize I = 0; I < RecordCount; I++)
	{
		ZEMLReaderNode RecordNode = RecordsNode.GetNode("Record", I);
		if (!Records[I].Load(&RecordNode))
			return false;
	}

	return true;
}

bool ZEITGenerator::SaveGeneratorFile(const ZEString& FileName) const
{
	ZEMLWriter Writer;
	Writer.SetFormat(ZEMLFormat::GetDefaultTextFormat()->CreateInstance());
	if (!Writer.Open(FileName))
	{
		zeError("Cannot write Integrity Generator file. File Name: \"%s\".", FileName.ToCString());
		return false;
	}

	ZEMLWriterNode RootNode;
	Writer.OpenRootNode("ZEITIntegrityGenerator", RootNode);
	RootNode.WriteString("IntegrityFileName", IntegrityFileName);

	ZEMLWriterNode ScannerNode;
	RootNode.OpenNode("Scanner", ScannerNode);

	ZEMLWriterNode IncludesNode;
	ScannerNode.OpenNode("Includes", IncludesNode);
	for (ZESize I = 0; I < Includes.GetCount(); I++)
	{
		ZEMLWriterNode IncludeNode;
		IncludesNode.OpenNode("Include", IncludeNode);
		IncludeNode.WriteString("Path", Includes[I].GetPath());
		IncludeNode.WriteBool("Enabled", Includes[I].GetEnabled());
		IncludeNode.WriteBool("Recursive", Includes[I].GetRecursive());

		IncludeNode.WriteBool("Enabled", true);
		IncludeNode.CloseNode();
	}
	IncludesNode.CloseNode();

	ZEMLWriterNode ExcludesNode;
	ScannerNode.OpenNode("Excludes", ExcludesNode);
	for (ZESize I = 0; I < Excludes.GetCount(); I++)
	{
		ZEMLWriterNode ExcludeNode;
		ExcludesNode.OpenNode("Exclude", ExcludeNode);
		ExcludeNode.WriteString("Path", Excludes[I].GetPath());
		ExcludeNode.WriteBool("Enabled", Excludes[I].GetEnabled());
		ExcludeNode.WriteBool("Recursive", Excludes[I].GetRecursive());
		ExcludeNode.CloseNode();
	}
	ExcludesNode.CloseNode();

	ScannerNode.CloseNode();

	ZEMLWriterNode RecordsNode;
	RootNode.OpenNode("Records", RecordsNode);

	for (ZESize I = 0; I < Records.GetCount(); I++)
		Records[I].Save(&RecordsNode);

	RecordsNode.CloseNode();
	RootNode.CloseNode();
	Writer.Close();

	return true;
}

bool ZEITGenerator::GenerateIntegrityFile() const
{
	ZEMLWriter Writer;
	Writer.SetFormat(ZEMLFormat::GetDefaultTextFormat()->CreateInstance());
	if (!Writer.Open(IntegrityFileName))
	{
		zeError("Cannot write Integrity file. File Name: \"%s\".", IntegrityFileName.ToCString());
		return false;
	}

	ZEMLWriterNode RootNode;
	Writer.OpenRootNode("ZEITIntegrity", RootNode);

	ZEMLWriterNode RecordsNode;
	RootNode.OpenNode("Records", RecordsNode);

	for (ZESize I = 0; I < Records.GetCount(); I++)
	{
		if (DiscardDisabled && !Records[I].GetEnabled())
			continue;

		Records[I].Save(&RecordsNode);
	}

	RecordsNode.CloseNode();
	RootNode.CloseNode();
	Writer.Close();

	return true;
}

ZEITGenerator::ZEITGenerator()
{
	DiscardDisabled = true;
}
