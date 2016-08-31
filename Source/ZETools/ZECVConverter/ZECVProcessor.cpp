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
#include "ZECVTestAsset.h"

ZECVAsset* const* ZECVProcessor::GetAssets() const
{
	static ZECVModelAsset Model;
	static ZECVTestAsset Test;
	static ZECVAsset* Assets[] =
	{
		&Model,
		&Test
	};

	return Assets;
}

ZESize ZECVProcessor::GetAssetCount() const
{
	return 2;
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
	zeLog("Processing file. \n"
		"  Source File Name: \"%s\"\n"
		"  Destination File Name: \"%s\"", SourceFileName.ToCString(), DestinationFileName.ToCString());

	ZEFileInfo SourceFileInfo(SourceFileName);
	ZEString Extension = SourceFileInfo.GetExtension();

	ZECVAsset* Asset = FindAsset(Extension);
	if (Asset == NULL)
	{
		zeLog("Unknown asset type. Ignoring file. File Name: \"%s\"", SourceFileName.ToCString());
		return false;
	}

	zeLog("Asset type found. Processing. Extension: \"%s\", Asset Type: \"%s\".", Extension.ToCString(), Asset->GetName());
	
	zeLog("Processing...");
	ZECVResult Result = Asset->Convert(SourceFileName, DestinationFileName);
	switch (Result)
	{
		case ZECV_R_DONE:
			zeLog("File converted. File Name: \"%s\"", SourceFileName.ToCString());
			return true;

		case ZECV_R_LATEST_VERSION:
			zeLog("Cannot convert file. File format is unknown. File Name: \"%s\"", SourceFileName.ToCString());
			return true;

		case ZECV_R_UNKNOWN_FORMAT:
			zeLog("Cannot convert file. File format is unknown. File Name: \"%s\"", SourceFileName.ToCString());
			return false;

		default:
		case ZECV_R_FAILED:
			zeError("Processing failed. File Name: \"%s\"", SourceFileName.ToCString());
			return false;
	}
}

static bool OperationWarpper(const char* Path, ZEPathOperationElement Element)
{
	if (Element != ZE_POE_FILE)
		return true;

	ZECVProcessor::GetInstance()->Convert(Path, Path);
	return true;
}

void ZECVProcessor::ConvertDirectory(const ZEString& DirectoryName) const
{
	ZEPathInfo::Operate(DirectoryName, ZEPathOperationFunction::Create<OperationWarpper>(), ZE_POE_FILE, true);
}

ZECVProcessor* ZECVProcessor::GetInstance()
{
	static ZECVProcessor Processor;
	return &Processor;
}
