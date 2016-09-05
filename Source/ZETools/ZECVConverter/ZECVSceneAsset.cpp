//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECVSceneAsset.cpp
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

#include "ZECVSceneAsset.h"

#include "ZEML/ZEMLReader.h"
#include "ZECVSceneConverterV1.h"


const char* ZECVSceneAsset::GetName() const
{
	return "ZEScene";
}

const char* const* ZECVSceneAsset::GetFileExtensions() const
{
	static const char* Extensions[] =
	{
		".ZEScene"
	};

	return Extensions;
}

ZESize ZECVSceneAsset::GetFileExtensionCount() const
{
	return 1;
}

 ZECVConverter* const* ZECVSceneAsset::GetConverters() const
{
	static ZECVSceneConverterV1 Version1;
	static ZECVConverter* Converters[] =
	{
		&Version1
	};

	return Converters;
}

ZESize ZECVSceneAsset::GetConverterCount() const
{
	return 1;
}

ZECVResult ZECVSceneAsset::Check(const ZEString& SourceFileName, ZECVVersion& Version) const
{
	ZEFile SceneFile;
	if (!SceneFile.Open(SourceFileName, ZE_FOM_READ, ZE_FCM_NONE))
	{
		zeError("Cannot open source ZEScene file. File Name: \"%s\"", SourceFileName.ToCString());
		return ZECV_R_FAILED;
	}

	char Identifier[4];
	if (SceneFile.Read(Identifier, 4, 1) == 1)
	{
		if (Identifier[0] != 'Z' &&
			Identifier[1] != 'E' &&
			Identifier[2] != 'M' &&
			Identifier[3] != 'L')
		{
			Version.Minor = 0;
			Version.Major = 0;
			return ZECV_R_DONE;
		}
	}

	SceneFile.Seek(0, ZE_SF_BEGINING);

	ZEMLReader Reader;
	if (!Reader.Open(&SceneFile))
		return ZECV_R_UNKNOWN_FORMAT;

	ZEMLReaderNode SourceSceneNode = Reader.GetRootNode();
	if (SourceSceneNode.GetName() != "ZEScene")
		return ZECV_R_UNKNOWN_FORMAT;

	Version.Major = SourceSceneNode.ReadUInt8("MajorVersion", 1);
	Version.Minor = SourceSceneNode.ReadUInt8("MinorVersion", 0);

	if (Version.Major > 1 ||
		Version.Major == 1 && Version.Minor >= 0)
		return ZECV_R_LATEST_VERSION;

	SceneFile.Close();

	return ZECV_R_DONE;
}
