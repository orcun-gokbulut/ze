//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECVProcessor.cpp
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

#include "ZECVProcessor.h"

#include "ZEFile/ZEFileInfo.h"

#include "ZECVModelAsset.h"
#include "ZECVMaterialAsset.h"
#include "ZECVTestAsset.h"
#include "ZECVSceneAsset.h"
#include "ZECVConverter.h"
#include "ZEDS/ZEFormat.h"
#include "ZELogSession.h"

ZECVAsset* const* ZECVProcessor::GetAssets() const
{
	static ZECVModelAsset Model;
	static ZECVMaterialAsset Material;
	static ZECVSceneAsset Scene;
	static ZECVTestAsset Test;

	static ZECVAsset* Assets[] =
	{
		&Model,
		&Material,
		&Scene,
		&Test
	};

	return Assets;
}

ZESize ZECVProcessor::GetAssetCount() const
{
	return 4;
}

ZECVAsset* ZECVProcessor::FindAsset(const ZEString& Extension) const
{
	ZECVAsset* const* Assets = GetAssets();
	ZESize AssetCount = GetAssetCount();
	for (ZESize I = 0; I < AssetCount; I++)
	{
		ZECVAsset* CurrentAsset = Assets[I];

		const char* const* Extensions = CurrentAsset->GetFileExtensions();
		ZESize ExtensionCount = CurrentAsset->GetFileExtensionCount();
		for (ZESize N = 0; N < ExtensionCount; N++)
		{
			if (Extension.EqualsIncase(Extensions[N]))
				return CurrentAsset;
		}
	}

	return NULL;
}

bool ZECVProcessor::Convert(const ZEString& SourceFileName, const ZEString& DestinationFileName) const
{
	ZELogSession ItemLogSession;
	ItemLogSession.BeginSession();

	zeLog("Processing file. Source File Name: \"%s\". Destination File Name: \"%s\".", 
		SourceFileName.ToCString(), DestinationFileName.ToCString());

	ZEFileInfo SourceFileInfo(SourceFileName);
	ZEString Extension = SourceFileInfo.GetExtension();

	ZECVAsset* Asset = FindAsset(Extension);
	if (Asset == NULL)
	{
		zeLog("Unknown asset type. Ignoring file. File Name: \"%s\"", SourceFileName.ToCString());
		return false;
	}

	zeLog("Asset type found. Processing. Extension: \"%s\", Asset Type: \"%s\".", Extension.ToCString(), Asset->GetName());

	ZECVVersion FileVersion;
	ZECVResult Result = Asset->Check(SourceFileName, FileVersion);
	switch(Result)
	{
		case  ZECV_R_DONE:
			zeLog("File check completed. File Version: %d.%d.", FileVersion.Major, FileVersion.Minor);
			break;

		case ZECV_R_LATEST_VERSION:
			if (FileVersion.Major > Asset->GetCurrentVersion().Major ||
				FileVersion.Major == Asset->GetCurrentVersion().Major && FileVersion.Minor > Asset->GetCurrentVersion().Minor)
			{
				zeWarning("Newer file version detected. Skipping file. This indicates your ZECVConverter needs to be updated. File Version: %d.%d. Latest Known Version: %d.%d.",
					FileVersion.Major, FileVersion.Minor,
					Asset->GetCurrentVersion().Major, Asset->GetCurrentVersion().Minor);
			}
			zeLog("File's version is already latest version. No conversion done. File Name: \"%s\".", SourceFileName.ToCString());
			return true;

		case ZECV_R_UNKNOWN_FORMAT:
			zeError("File's format is unknown. File Name: \"%s\".", SourceFileName.ToCString());
			return false;

		default:
		case ZECV_R_FAILED:
			zeError("File check failed. Skipping. File Name: \"%s\".", SourceFileName.ToCString());
			return false;
	}

	ZEString LogFileName = ZEFormat::Format("{0}.ZECVConvert-{1}.log", SourceFileName, ZETimeStamp::Now().ToString("%Y%m%d%H%M"));
	ZEString LogFileHeader = ZEFormat::Format(
		" ZECVConvert v1.0 Log File\n"
		"----------------------------------------------------\n"
		"   Source File      : {0}\n"
		"   Destination File : {1}\n"
		"   File Version     : {2}.{3}\n"
		"   Target Version   : {4}.{5}\n"
		"   Time Stamp       : {6}\n"
		"----------------------------------------------------", 
		SourceFileName,
		DestinationFileName,
		FileVersion.Major, FileVersion.Minor,
		Asset->GetCurrentVersion().Major, Asset->GetCurrentVersion().Minor,
		ZETimeStamp::Now().ToString("%d-%m-%Y %H:%M:%S"));

	ZELogSession ConvertLogSession;
	ConvertLogSession.BeginSession();
	ConvertLogSession.OpenLogFile(LogFileName, LogFileHeader, false);

	zeLog("Converting %s asset. File Name: \"%s\".", Asset->GetName(), SourceFileName.ToCString());

	ZEFileInfo DestinationFileInfo(DestinationFileName);
	bool SourceDestinationSame = false;
	if (SourceFileInfo.Normalize().EqualsIncase(DestinationFileInfo.Normalize()))
		SourceDestinationSame = true;
	
	ZECVVersion OriginalVersion = FileVersion;

	ZEString InputFileName = SourceFileName;
	ZEString OutputFileName = SourceFileName;
	for (ZESize I = 0 ; I < Asset->GetConverterCount(); I++)
	{
		ZECVConverter* Converter = Asset->GetConverters()[I];
		ZECVVersion SourceVersion = Converter->GetSourceVersion();
		ZECVVersion DestinationVersion = Converter->GetDestinationVersion();

		if (SourceVersion.Major != FileVersion.Major || 
			SourceVersion.Minor != FileVersion.Minor)
			continue;

		zeLog("Found converter. Source Version: %d.%d. Destination Version: %d.%d.", 
			SourceVersion.Major, SourceVersion.Minor,
			DestinationVersion.Major, DestinationVersion.Minor);

		bool LastLink = I + 1 == Asset->GetConverterCount();
		if (LastLink)
		{
			// Backup File
			if (SourceDestinationSame)
			{
				ZEString BackupFileName = ZEFormat::Format("{0}.ZECVConvert.{1}.{2}.bak", SourceFileName, OriginalVersion.Major, OriginalVersion.Minor);
				zeLog("Creating backup. Version: %d.%d. Backup File Name: \"%s\".", OriginalVersion.Major, OriginalVersion.Minor, BackupFileName.ToCString());
				SourceFileInfo.Copy(BackupFileName);
			}

			InputFileName = OutputFileName;
			OutputFileName = DestinationFileName;
		}
		else
		{
			InputFileName = OutputFileName;
			OutputFileName = ZEFormat::Format("{0}.{1}.{2}.ZECVConvert.tmp", SourceFileName, Converter->GetDestinationVersion().Major, Converter->GetDestinationVersion().Minor);
		}

		zeLog("Converting to %d.%d...", DestinationVersion.Major, DestinationVersion.Minor);


		if (InputFileName == OutputFileName) // Direct Convert
		{
			OutputFileName = ZEFormat::Format("{0}.{1}.{2}.ZECVConvert.tmp", SourceFileName, Converter->GetDestinationVersion().Major, Converter->GetDestinationVersion().Minor);
			
			ZECVResult Result = Converter->Convert(InputFileName, OutputFileName);
			if (Result != ZECV_R_DONE)
			{
				zeError("Conversion failed. File Name: \"%s\".", SourceFileName.ToCString());
				return false;
			}
			
			if (!ZEFileInfo(InputFileName).Delete())
			{
				zeError("Conversion failed. Cannot delete source file to replace it with temporary file. File Name: \"%s\".", SourceFileName.ToCString());
				return false;
			}

			if (!ZEFileInfo(OutputFileName).Move(InputFileName))
			{
				zeError("Conversion failed. Cannot rename temporary file. File Name: \"%s\".", SourceFileName.ToCString());	
				return false;
			}
		}
		else
		{
			ZECVResult Result = Converter->Convert(InputFileName, OutputFileName);

			// Remove Temp File
			if (InputFileName != SourceFileName)
				ZEFileInfo(InputFileName).Delete();

			if (Result != ZECV_R_DONE)
			{
				zeError("Conversion failed. File Name: \"%s\".", SourceFileName.ToCString());
				return false;
			}
		}

		zeLog("Conversation %d.%d -> %d.%d successfully completed.", FileVersion.Major, FileVersion.Minor, DestinationVersion.Major, DestinationVersion.Minor);

		FileVersion = DestinationVersion;
	}

	zeLog("Conversion of %s asset to newest version (%d.%d -> %d.%d) has been completed succesfuly. File Name: \"%s\".", Asset->GetName(), 
		OriginalVersion.Major, OriginalVersion.Minor, FileVersion.Major, FileVersion.Minor, SourceFileName.ToCString());

	ConvertLogSession.EndSession();
	ItemLogSession.EndSession();
	return true;
}


static bool OperationWarpper(const char* Path, ZEPathOperationElement Element, void* Parameters)
{
	ZESmartArray<ZEString>* FileList = reinterpret_cast<ZESmartArray<ZEString>*>(Parameters);
	FileList->Add(Path);
	return true;
}

void ZECVProcessor::ConvertDirectory(const ZEString& DirectoryName) const
{
	ZEString LogFileName = ZEFormat::Format("{0}/ZECVConvert-{1}.log", DirectoryName, ZETimeStamp::Now().ToString("%Y%m%d%H%M"));
	ZEString Header = ZEFormat::Format(
		" ZECVConvert v1.0 Log File\n"
		"----------------------------------------------------\n"
		"   Directory  : {0}\n"
		"   Time Stamp : {1}\n"
		"----------------------------------------------------", 
		DirectoryName,
		ZETimeStamp::Now().ToString("%d-%m-%Y %H:%M:%S"));
	
	ZELogSession LogSession;
	LogSession.SetSink(true);
	LogSession.BeginSession();
	LogSession.OpenLogFile(LogFileName, Header, false);

	zeLog("Converting Directory... Directory Name: \"%s\".", DirectoryName.ToCString());
	
	zeLog("Creating list of files. Directory Name: \"%s\".", DirectoryName.ToCString());
	ZESmartArray<ZEString> FileNames;
	ZEPathInfo::Operate(DirectoryName, ZEPathOperationFunction::Create<OperationWarpper>(), ZE_POE_FILE, true, &FileNames);

	zeLog("Processing files...");
	#pragma omp parallel
	{
		#pragma omp for schedule(dynamic)
		for (ZESSize I = 0; I < FileNames.GetCount(); I++)
			Convert(FileNames[I], FileNames[I]);
	}

	LogSession.EndSession();
}

ZECVProcessor* ZECVProcessor::GetInstance()
{
	static ZECVProcessor Processor;
	return &Processor;
}
