//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERegEx.cpp
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

#include "ZERegEx.h"
#include "ZEDS/ZEString.h"

#include "ZERegEx.h"
#include "TRE/regex.h"

bool ZERegEx::Compile(const ZEString& RegEx, ZERegExFlags Flags)
{
	if (Code == NULL)
		Code = new regex_t;

	if (regcomp((regex_t*)Code, RegEx, 0) != 0)
	{
		delete Code;
		return false;
	}

	return true;
}

bool ZERegEx::Match(const ZEString& String)
{
	if (Code == NULL)
		return false;

	return regexec((regex_t*)Code, String.ToCString(), 0, NULL, NULL) == 0;
}

bool ZERegEx::Match(const ZEString& String, ZEArray<ZERegExMatch>& Matches)
{
	if (Code == NULL)
		return false;

	Matches.SetCount(((regex_t*)Code)->re_nsub);
	regmatch_t Temp[64];

	if (regexec((regex_t*)Code, String.ToCString(), ((regex_t*)Code)->re_nsub, Temp, NULL) != 0)
		return false;

	for (int I = 0; I < ((regex_t*)Code)->re_nsub; I++)
	{
		Matches[I].Offset = Temp[I].rm_so;
		Matches[I].Size = Temp[I].rm_eo - Temp[I].rm_so;
	}

	return true;
}

bool ZERegEx::Match(const ZEString& RegEx, const ZEString& String, ZERegExFlags Flags)
{
	regex_t Code;

	if (regcomp(&Code, RegEx, 0) != 0)
		return false;

	return regexec(&Code, String.ToCString(), 0, NULL, NULL) == 0;
}

bool ZERegEx::Match(const ZEString& RegEx, const ZEString& String, ZEArray<ZERegExMatch>& Matches, ZERegExFlags Flags)
{
	regex_t Code;

	if (regcomp(&Code, RegEx, 0) != 0)
		return false;

	Matches.SetCount(Code.re_nsub);
	regmatch_t Temp[64];

	if (regexec(&Code, String.ToCString(), Code.re_nsub, Temp, NULL) != 0)
		return false;

	for (int I = 0; I < Code.re_nsub; I++)
	{
		Matches[I].Offset = Temp[I].rm_so;
		Matches[I].Size = Temp[I].rm_eo - Temp[I].rm_so;
	}

	return true;
}

ZERegEx::ZERegEx()
{
	Code = NULL;
}

ZERegEx::ZERegEx(const ZEString& RegEx, ZERegExFlags Flags)
{
	Compile(RegEx, Flags);
}

ZERegEx::~ZERegEx()
{
	if (Code != NULL)
		delete Code;
}
