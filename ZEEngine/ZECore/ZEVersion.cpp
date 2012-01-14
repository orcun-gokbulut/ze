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

#include <stdio.h>

void ZEVersion::GetShortString(char* Buffer) 
{
	sprintf(Buffer, "%02d.%02d.%02d", Major, Minor, Internal);
}

void ZEVersion::GetLongString(char* Buffer)	
{
	char* PlatformString;
	switch(Platform)
	{
		case ZE_VP_WIN32:
			PlatformString = "WIN32";
			break;

		default:
			PlatformString = "UNKNOWN";
			break;

	}
	sprintf(Buffer, "%02d.%02d.%02d (%s) - Build %06d", Major, Minor, Internal, PlatformString, Build);
}

ZEVersion ZEVersion::GetZinekVersion()
{
	ZEVersion Temp;
	
	Temp.Major = ZE_ZINEK_VERSION_MAJOR;
	Temp.Minor = ZE_ZINEK_VERSION_MINOR;
	Temp.Internal = ZE_ZINEK_VERSION_INTERNAL;
	Temp.Build = ZE_ZINEK_VERSION_BUILD;
	Temp.Platform = ZE_VP_WIN32;

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
	this->Build = 0;
	this->Platform = ZE_VP_WIN32;
}

ZEVersion::ZEVersion(unsigned int Major, unsigned int Minor, unsigned int Internal)
{
	this->Major = Major;
	this->Minor = Minor;
	this->Internal = Internal;
	this->Build = 0;
	this->Platform = ZE_VP_WIN32;
}

ZEVersion::ZEVersion(unsigned int Major, unsigned int Minor, unsigned int Internal, unsigned int Build)
{
	this->Major = Major;
	this->Minor = Minor;
	this->Internal = Internal;
	this->Build = 0;
	this->Platform = ZE_VP_WIN32;
}

ZEVersion::ZEVersion(unsigned int Major, unsigned int Minor, unsigned int Internal, unsigned int Build, ZEVersionPlatform Platform)
{
	this->Major = Major;
	this->Minor = Minor;
	this->Internal = Internal;
	this->Build = Build;
	this->Platform = ZE_VP_WIN32;
}
