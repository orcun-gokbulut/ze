//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEVersion.h
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

#pragma once
#ifndef	__ZE_VERSION_H__
#define __ZE_VERSION_H__

#include "ZETypes.h"
#include "ZEDS/ZEString.h"

enum ZEVersionCheckLevel
{
	ZE_VCL_MAJOR,
	ZE_VCL_MINOR,
	ZE_VCL_INTERNAL
};

#define ZE_VERSION_GENERATE() ZEVersion(ZE_VERSION_MAJOR, ZE_VERSION_MINOR, ZE_VERSION_INTERNAL, ZE_VERSION_REVISION, ZE_VERSION_BRANCH, ZE_PLATFORM, ZE_PLATFORM_ARCHITECTURE)

class ZEVersion
{
	public:
		ZEUInt					Major;
		ZEUInt					Minor;
		ZEUInt					Internal;
		ZEUInt					Revision;		
		char					Branch[50];
		char					Platform[50];
		char					Architecture[50];

		ZEString				GetShortString();
		ZEString				GetLongString();

								ZEVersion();
								ZEVersion(ZEUInt Major, ZEUInt Minor, ZEUInt Internal = 0, ZEUInt Revision = 0, const char* Branch = "", const char* Platform = "", const char* Architecture = "");

		static ZEVersion		GetZinekVersion();
		static bool				Check(const ZEVersion& A, const ZEVersion& B, ZEVersionCheckLevel Level);
};

#endif
