//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEVersion.cpp
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

#include "ZEVersion.h"

#include "ZEVersionData.h"
#include "ZEDS/ZEFormat.h"
#include "ZEPlatform.h"

ZEString ZEVersion::GetShortString() 
{
	return ZEFormat::Format("{0}.{1}.{2}-Rev:{3}", Major, Minor, Internal, Revision);
}

ZEString ZEVersion::GetLongString()	
{
	return ZEFormat::Format("{0}.{1}.{2}-Rev:{3} (Branch:{4}, Platform:{5}-{6})", Major, Minor, Internal, Revision, Branch, Platform, Architecture);
}

ZEVersion ZEVersion::GetZinekVersion()
{
	ZEVersion Temp;
	
	Temp.Major = ZE_VERSION_MAJOR;
	Temp.Minor = ZE_VERSION_MINOR;
	Temp.Internal = ZE_VERSION_INTERNAL;
	Temp.Revision = ZE_VERSION_REVISION;
	strcpy(Temp.Branch, ZE_VERSION_BRANCH);
	strcpy(Temp.Platform, ZE_PLATFORM);
	strcpy(Temp.Architecture, ZE_PLATFORM_ARCHITECTURE);

	return Temp;
}

bool ZEVersion::Check(const ZEVersion& A, const ZEVersion& B, ZEVersionCheckLevel Level)
{
	switch(Level)
	{
		case ZE_VCL_MAJOR:
			return A.Major == B.Major;

		case ZE_VCL_MINOR:
			return A.Major == B.Major && A.Minor >= B.Minor;

		case ZE_VCL_INTERNAL:
			return A.Major == B.Major && A.Minor >= B.Minor && A.Internal >= B.Internal;
		
		default:
			return false;
	}
}

ZEVersion::ZEVersion()
{
	this->Major = 0;
	this->Minor = 0;
	this->Internal = 0;
	this->Revision = 0;
	strcpy(this->Branch, "");
	strcpy(this->Platform, "");
	strcpy(this->Architecture, "");
}

ZEVersion::ZEVersion(ZEUInt Major, ZEUInt Minor, ZEUInt Internal, ZEUInt Revision, const char* Branch, const char* Platform, const char* Architecture)
{
	this->Major = Major;
	this->Minor = Minor;
	this->Internal = Internal;
	this->Revision = 0;
	strcpy(this->Branch, Branch);
	strcpy(this->Platform, Platform);
	strcpy(this->Architecture, Architecture);
}
