//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECVAsset.cpp
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

#include "ZECVAsset.h"
#include "ZELog.h"
#include "ZECVConverter.h"
#include "ZEFile\ZEFileInfo.h"
#include "ZEDS\ZEFormat.h"

ZECVVersion ZECVAsset::GetMinimumVersion() const
{
	ZECVVersion Version;

	if (GetConverterCount() == 0)
	{
		Version.Major = -1;
		Version.Minor = -1;
		return Version;
	}

	return GetConverters()[0]->GetSourceVersion();
}

ZECVVersion ZECVAsset::GetCurrentVersion() const
{
	ZECVVersion Version;

	if (GetConverterCount() == 0)
	{
		Version.Major = -1;
		Version.Minor = -1;
		return Version;
	}

	return GetConverters()[GetConverterCount() - 1]->GetDestinationVersion();
}

ZECVResult ZECVAsset::Convert(const ZEString& SourceFileName, const ZEString& DestinationFileName)
{
	zeLog("Processing %s asset. File Name: \"%s\".", GetName(), SourceFileName.ToCString());
	
	ZECVVersion FileVersion;
	ZECVResult Result = Check(SourceFileName, FileVersion);
	switch(Result)
	{
		case  ZECV_R_DONE:
			zeLog("File check completed. File Version: %d.%d.", FileVersion.Major, FileVersion.Minor);
			break;

		case ZECV_R_LATEST_VERSION:
			if (FileVersion.Major > GetCurrentVersion().Major ||
				FileVersion.Major == GetCurrentVersion().Major && FileVersion.Minor > GetCurrentVersion().Minor)
			{
				zeWarning("Newer file version detected. Skipping file. This indicates your ZECVConverter needs to be updated. File Version: %d.%d. Latest Known Version: %d.%d.",
					FileVersion.Major, FileVersion.Minor,
					GetCurrentVersion().Major, GetCurrentVersion().Minor);
			}
			zeLog("File's version is already latest version. No conversion done. File Name: \"%s\".", SourceFileName.ToCString());
			return ZECV_R_DONE;

		case ZECV_R_UNKNOWN_FORMAT:
			zeError("File's format is unknown. File Name: \"%s\".", SourceFileName.ToCString());
			return ZECV_R_FAILED;

		default:
		case ZECV_R_FAILED:
			zeError("File check failed. Skipping. File Name: \"%s\".", SourceFileName.ToCString());
			return ZECV_R_FAILED;
	}

	ZELog::GetInstance()->BeginSession(ZE_LOG_INFO, SourceFileName + ".ZECVConver.log", "", false);

	zeLog("Converting %s asset. File Name: \"%s\".", GetName(), SourceFileName.ToCString());

	ZEFileInfo SourceFileInfo(SourceFileName);
	ZEFileInfo DestinationFileInfo(DestinationFileName);

	bool SourceDestinationSame = false;
	if (SourceFileInfo.Normalize().EqualsIncase(DestinationFileInfo.Normalize()))
		SourceDestinationSame = true;
	
	ZECVVersion OriginalVersion = FileVersion;

	ZEString InputFileName = SourceFileName;
	ZEString OutputFileName = SourceFileName;
	for (ZESize I = 0 ; I < GetConverterCount(); I++)
	{
		ZECVConverter* Converter = GetConverters()[I];
		ZECVVersion SourceVersion = Converter->GetSourceVersion();
		ZECVVersion DestinationVersion = Converter->GetDestinationVersion();

		if (SourceVersion.Major != FileVersion.Major || 
			SourceVersion.Minor != FileVersion.Minor)
			continue;

		zeLog("Found converter. Source Version: %d.%d. Destination Version: %d.%d.", 
			SourceVersion.Major, SourceVersion.Minor,
			DestinationVersion.Major, DestinationVersion.Minor);

		bool LastLink = I + 1 == GetConverterCount();
		if (LastLink)
		{
			// Backup File
			if (SourceDestinationSame)
			{
				ZEString BackupFileName = ZEFormat::Format("{0}.{1}.{2}.ZECVConvert.bak", SourceFileName, OriginalVersion.Major, OriginalVersion.Minor);
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
				return ZECV_R_FAILED;
			}
			
			if (!ZEFileInfo(InputFileName).Delete())
			{
				zeError("Conversion failed. Cannot delete source file to replace it with temporary file. File Name: \"%s\".", SourceFileName.ToCString());
				return ZECV_R_FAILED;
			}

			if (!ZEFileInfo(OutputFileName).Copy(InputFileName))
			{
				zeError("Conversion failed. Cannot rename temporary file. File Name: \"%s\".", SourceFileName.ToCString());
				return ZECV_R_FAILED;
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
				return ZECV_R_FAILED;
			}
		}

		zeLog("Conversation %d.%d -> %d.%d successfully completed.", FileVersion.Major, FileVersion.Minor, DestinationVersion.Major, DestinationVersion.Minor);

		FileVersion = DestinationVersion;
	}

	zeLog("Conversion of %s asset to newest version (%d.%d ->%d.%d) has been completed succesfuly. File Name: \"%s\".", GetName(), 
		OriginalVersion.Major, OriginalVersion.Minor, FileVersion.Major, FileVersion.Minor, SourceFileName.ToCString());

	ZELog::GetInstance()->EndSession();

	return ZECV_R_DONE;
}
