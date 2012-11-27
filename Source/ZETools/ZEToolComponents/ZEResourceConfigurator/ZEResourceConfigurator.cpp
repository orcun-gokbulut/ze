//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEResourceConfigurator.cpp
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

#include "ZEResourceConfigurator.h"
#include "ZEFile\ZEFileUtils.h"
#include "ZEFile\ZEPathUtils.h"
#include "ZEML\ZEMLProperty.h"
#include "ZEFile\ZEFile.h"
#include "ZEError.h"
#include <time.h>

inline static time_t GetTime(ZEFileTime* Date)
{
	struct tm CDate;
	memset(&CDate, 0, sizeof(struct tm));

	CDate.tm_year = Date->Year - 1900;
	CDate.tm_mon = Date->Month;
	CDate.tm_mday = Date->Day;
	CDate.tm_hour = Date->Hour;
	CDate.tm_min = Date->Minute;
	CDate.tm_sec = Date->Second;

	return mktime(&CDate);
}

ZEResourceConfigurator::ZEResourceConfigurator()
{
	WorkingDirectory.Clear();
	Options.Clear();
	Preset = new ZEMLNode("Preset");
}

ZEResourceConfigurator::~ZEResourceConfigurator()
{
	delete Preset;
	Preset = NULL;
	WorkingDirectory.Clear();
	Options.Clear();
}

bool ZEResourceConfigurator::GetResourceCopyState(ZEResourceOption Option)
{
	return GetResourceCopyState(Option.Identifier);
}

bool ZEResourceConfigurator::GetResourceCopyState(const ZEString& FileName)
{
	ZEResourceOption* TempOption;
	bool IsAvailableAtExportLocation = false;

	if(!GetOption(FileName, TempOption))
	{
		zeError("Resource option not found. Resource identifier : %s", FileName.ToCString());
		return false;
	}

	if(ZEFileUtils::IsFile(TempOption->ExportPath + "\\" + TempOption->Identifier))
		IsAvailableAtExportLocation = true;

	if(TempOption->Action == ZE_ROAA_DO_NOTHING)
	{
		return false;
	}
	else if(TempOption->Action == ZE_ROAA_COPY_OVERWRITE)
	{
		return true;
	}
	else if(TempOption->Action == ZE_ROAA_COPY_IF_NEWER)
	{
		if(TempOption->IsAutoGenerated)
			return true;
		else
		{
			if(!IsAvailableAtExportLocation)
				return true;

			ZEFileTime* ExportedFileTime = new ZEFileTime();
			ZEFileTime* SourceFileTime = new ZEFileTime();
			ZEFileUtils::GetModificationTime(ExportedFileTime, TempOption->ExportPath + "\\" + TempOption->Identifier);
			ZEFileUtils::GetModificationTime(SourceFileTime, TempOption->PhysicalPath);

			if(GetTime(ExportedFileTime) >= GetTime(SourceFileTime))
				return true;

			return false;
		}
	}
	else if(TempOption->Action == ZE_ROAA_COPY_IF_MISSING)
	{
		if(IsAvailableAtExportLocation)
			return false;
		else
			return true;
	}
}

bool ZEResourceConfigurator::AddResource(const ZEString& FileName, const ZEString& Type, const ZEString& ExportDir, const ZEString& PhysicalDir, ZEResourceOptionAvailableAction Action, bool IsAutoGenerated)
{
	for(ZESize I = 0; I < Options.GetCount(); I++)
	{
		if(Options[I]->Identifier == FileName)
		{
			zeWarning("Resource already exists. File name : %s", FileName.ToCString());
			return false;
		}
	}

	ZEResourceOption* TempOption = new ZEResourceOption();
	TempOption->Identifier = FileName;
	TempOption->ExportPath = ExportDir;
	TempOption->PhysicalPath = PhysicalDir;
	TempOption->Action = Action;
	TempOption->IsAutoGenerated = IsAutoGenerated;
	TempOption->Type = Type;
	Options.Add(TempOption);

	return true;
}

bool ZEResourceConfigurator::AddResource(const ZEString& FileName, const ZEString& Type, const ZEString& ExportDir, ZEResourceOptionAvailableAction Action, bool IsAutoGenerated)
{
	return AddResource(FileName, ExportDir, ZEString(), Action, IsAutoGenerated);
}

ZEString ZEResourceConfigurator::GetResourceRelativePathToWorkingDir(ZEString& Filename)
{
	return GetResourceRelativePath(WorkingDirectory, Filename);
}

ZEString ZEResourceConfigurator::GetResourceRelativePathToWorkingDir(ZEResourceOption& Option)
{
	return GetResourceRelativePath(WorkingDirectory, Option.Identifier);
}

ZEString ZEResourceConfigurator::GetResourceRelativePath(ZEString& RelativeTo, const ZEString& FileName)
{
	ZEResourceOption* Option;

	if(!GetOption(FileName, Option))
	{
		zeWarning("Resource not found in options. Name : %s", FileName.ToCString());
		return ZEString();
	}

	return ZEPathUtils::GetRelativePath(RelativeTo, Option->ExportPath + "\\" + Option->Identifier);
}

ZEString ZEResourceConfigurator::GetResourceRelativePath(ZEString& RelativeTo, const ZEResourceOption& Option)
{
	return GetResourceRelativePath(RelativeTo, Option.Identifier);
}

const ZEArray<ZEResourceOption*>& ZEResourceConfigurator::GetOptions()
{
	return Options;
}

bool ZEResourceConfigurator::GetOption(const ZEString& FileName, ZEResourceOption*& Option)
{
	for(ZESize I = 0; I < Options.GetCount(); I++)
	{
		if(Options[I]->Identifier == FileName)
		{
			Option = Options[I];
			return true;
		}
	}

	Option = NULL;
	return false;
}

void ZEResourceConfigurator::SetWorkingDirectory(const ZEString& WorkingDirectory)
{
	this->WorkingDirectory = WorkingDirectory;
}

const ZEString&	ZEResourceConfigurator::GetWorkingDirectory()
{
	return WorkingDirectory;
}

void ZEResourceConfigurator::LoadPresetFromFile(ZEFile* File)
{
	if(!File->IsOpen())
		zeError("Given preset file is not open.");

	delete Preset;
	Preset = new ZEMLNode("Preset");
	Preset->Read(File, false);

	ZEArray<ZEMLNode*> Resources = Preset->GetSubNodes("Resource");

	for(ZESize I = 0; I < Resources.GetCount(); I++)
	{
		ZEMLNode* CurrentResource = Resources[I];
		ZEResourceOption* TempOption = new ZEResourceOption;
		TempOption->Identifier		= ((ZEMLProperty*)CurrentResource->GetProperty("FileName"))->GetValue().GetString();
		TempOption->Type				= ((ZEMLProperty*)CurrentResource->GetProperty("Type"))->GetValue().GetString();
		TempOption->ExportPath		= ((ZEMLProperty*)CurrentResource->GetProperty("ExportDirectory"))->GetValue().GetString();
		TempOption->PhysicalPath		= ((ZEMLProperty*)CurrentResource->GetProperty("PhysicalDirectory"))->GetValue().GetString();
		TempOption->Action			= (ZEResourceOptionAvailableAction)((ZEMLProperty*)CurrentResource->GetProperty("Action"))->GetValue().GetInt32();
		TempOption->IsAutoGenerated	= ((ZEMLProperty*)CurrentResource->GetProperty("IsAutoGenerated"))->GetValue().GetBoolean();
		Options.Add(TempOption);
	}
}

void ZEResourceConfigurator::SavePresetToFile(ZEFile* File)
{
	if(!File->IsOpen())
		zeError("Given preset file is not open.");

	if(File->GetSize() != 0)
		zeError("To save preset file, given file must be empty.");

	delete Preset;
	Preset = new ZEMLNode("Preset");

	for (ZESize I = 0; I < Options.GetCount(); I++)
	{
		ZEMLNode* CurrentResource = Preset->AddSubNode("Resource");
		CurrentResource->AddProperty("FileName", Options[I]->Identifier);
		CurrentResource->AddProperty("Type", Options[I]->Type);
		CurrentResource->AddProperty("ExportDirectory", Options[I]->ExportPath);
		CurrentResource->AddProperty("PhysicalDirectory", Options[I]->PhysicalPath);
		CurrentResource->AddProperty("Action", Options[I]->Action);
		CurrentResource->AddProperty("IsAutoGenerated", Options[I]->IsAutoGenerated);
	}

	Preset->Write(File);
}
