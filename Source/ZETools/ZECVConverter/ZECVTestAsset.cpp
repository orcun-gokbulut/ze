//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECVTestAsset.cpp
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

#include "ZECVTestAsset.h"
#include "ZECVTestConverter.h"

const char* ZECVTestAsset::GetName() const
{
	return "ZEVTestAsset";
}

const char* const* ZECVTestAsset::GetFileExtensions() const
{
	static const char* Extensions[] =
	{
		".ZECVTestAsset",
		".ZECVTestAsset0",
		".ZECVTestAsset1"
	};

	return Extensions;
}

ZESize ZECVTestAsset::GetFileExtensionCount() const
{
	return 3;
}

ZECVConverter* const* ZECVTestAsset::GetConverters() const
{
	ZECVVersion Temp;
	
	static ZECVTestConverter ConverterV1;
	Temp.Major = 0;
	Temp.Minor = 0;
	ConverterV1.SetSourceVersion(Temp);
	Temp.Major = 1;
	Temp.Minor = 0;
	ConverterV1.SetDestinationVersion(Temp);

	static ZECVTestConverter ConverterV1_2;
	ConverterV1_2.SetSourceVersion(Temp);
	Temp.Major = 1;
	Temp.Minor = 2;
	ConverterV1_2.SetDestinationVersion(Temp);

	static ZECVTestConverter ConverterV2_0;
	ConverterV2_0.SetSourceVersion(Temp);
	Temp.Major = 2;
	Temp.Minor = 0;
	ConverterV2_0.SetDestinationVersion(Temp);
	
	static ZECVTestConverter ConverterV2_5;
	ConverterV2_5.SetSourceVersion(Temp);
	Temp.Major = 2;
	Temp.Minor = 5;
	ConverterV2_5.SetDestinationVersion(Temp);

	static ZECVTestConverter ConverterV4;
	ConverterV4.SetSourceVersion(Temp);
	Temp.Major = 4;
	Temp.Minor = 0;
	ConverterV4.SetDestinationVersion(Temp);

	static ZECVConverter* Converters[] =
	{
		&ConverterV1,
		&ConverterV1_2,
		&ConverterV2_0,
		&ConverterV2_5,
		&ConverterV4
	};

	return Converters;
}

ZESize ZECVTestAsset::GetConverterCount() const
{
	return 5;
}

ZECVResult ZECVTestAsset::Check(const ZEString& SourceFileName, ZECVVersion& Version) const
{
	FILE* File = fopen(SourceFileName, "r");
	if (File == NULL)
		return ZECV_R_FAILED;

	char Buffer[255];
	if (!fscanf(File, "%s", Buffer))
	{
		fclose(File);
		return ZECV_R_UNKNOWN_FORMAT;
	}
	
	if (strcmp(Buffer, "ZECVTestAsset") != 0)
	{
		fclose(File);
		return ZECV_R_UNKNOWN_FORMAT;
	}

	if (fscanf(File, "%d.%d", &Version.Major, &Version.Minor) != 2)
	{
		fclose(File);
		return ZECV_R_FAILED;
	}

	fclose(File);

	ZECVVersion LatestVersion = GetCurrentVersion();
	if (Version.Major > LatestVersion.Major ||
		Version.Major == LatestVersion.Major && Version.Minor >= LatestVersion.Minor)
	{
		return ZECV_R_LATEST_VERSION;
	}

	return ZECV_R_DONE;
}
