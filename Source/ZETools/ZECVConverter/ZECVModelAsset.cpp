//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECVModelAsset.cpp
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

#include "ZECVModelAsset.h"

#include "ZEML/ZEMLReader.h"
#include "ZECVModelConverterV2.h"


const char* ZECVModelAsset::GetName() const
{
	return "ZEModel";
}

const char* const* ZECVModelAsset::GetFileExtensions() const
{
	static const char* Extensions[] =
	{
		".ZEModel",
		".ZEMDModel"
	};

	return Extensions;
}

ZESize ZECVModelAsset::GetFileExtensionCount() const
{
	return 2;
}

 ZECVConverter* const* ZECVModelAsset::GetConverters() const
{
	static ZECVModelConverterV2 Version2;
	static ZECVConverter* Converters[] =
	{
		&Version2
	};

	return Converters;
}

ZESize ZECVModelAsset::GetConverterCount() const
{
	return 1;
}

ZECVResult ZECVModelAsset::Check(const ZEString& SourceFileName, ZECVVersion& Version) const
{
	ZEMLReader Unserializer;
	if (!Unserializer.Open(SourceFileName))
		return ZECV_R_UNKNOWN_FORMAT;

	ZEMLReaderNode SourceModelNode = Unserializer.GetRootNode();
	if (SourceModelNode.GetName() != "ZEModel")
		return ZECV_R_UNKNOWN_FORMAT;

	Version.Major = SourceModelNode.ReadUInt8("MajorVersion");
	Version.Minor = SourceModelNode.ReadUInt8("MinorVersion");

	if (Version.Major > 2 ||
		Version.Major == 2 && Version.Minor >= 0)
		return ZECV_R_LATEST_VERSION;

	Unserializer.Close();

	return ZECV_R_DONE;
}
