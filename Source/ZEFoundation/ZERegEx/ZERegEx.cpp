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
#include "TRE/regex.h"

bool ZERegEx::Compile(const ZEString& RegEx, ZERegExFlags Flags)
{
	if (Code != NULL)
	{
		delete Code;
		Code = NULL;
	}

	int NativeFlags = REG_EXTENDED;
	if (Flags.GetFlags(ZE_REF_NEW_LINE))
		NativeFlags |= REG_NEWLINE;
	
	if (Flags.GetFlags(ZE_REF_CASE_INSENSITIVE))
		NativeFlags |= REG_ICASE;

	regex_t Temp;
	if (regcomp(&Temp, RegEx, NativeFlags) != REG_OK)
		return false;

	Code = new regex_t;
	*(regex_t*)Code = Temp;

	return true;
}

bool ZERegEx::IsValid()
{
	return Code != NULL;
}

bool ZERegEx::Match(const ZEString& String, ZERegExMatch& Match, ZERegExFlags Flags, ZERegExMatch* OldMatch)
{
	if (Code == NULL)
		return false;

	regmatch_t NativeMatches[256];

	ZESize OldOffset = 0;
	if (OldMatch != NULL)
		OldOffset = OldMatch->Offset + OldMatch->Size;

	int Result = 
		regexec((regex_t*)Code, 
			String.ToCString() + OldOffset, 
			((regex_t*)Code)->re_nsub + 1, 
			NativeMatches, 
			(OldMatch != NULL && OldMatch->Offset != 0 ? REG_NOTBOL : 0));

	if (Result != REG_OK)
		return false;
	
	Match.Offset = OldOffset + NativeMatches[0].rm_so;
	Match.Size = NativeMatches[0].rm_eo - NativeMatches[0].rm_so;

	if (Flags.GetFlags(ZE_REF_NO_MATCH_STRING) == 0)
		Match.String.SetValue(String.ToCString() + Match.Offset, Match.Size);

	if (Flags.GetFlags(ZE_REF_NO_SUBMATCH) == 0)
	{
		Match.SubMatches.SetCount(((regex_t*)Code)->re_nsub);
		for (ZESize I = 0; I < ((regex_t*)Code)->re_nsub; I++)
		{
			ZERegExSubMatch& SubMatch = Match.SubMatches[I];
			SubMatch.Offset = OldOffset + NativeMatches[I + 1].rm_so;
			SubMatch.Size = NativeMatches[I + 1].rm_eo - NativeMatches[I + 1].rm_so;
			if (Flags.GetFlags(ZE_REF_NO_MATCH_STRING) == 0)
				SubMatch.String.SetValue(String.ToCString() + SubMatch.Offset, SubMatch.Size);
		}
	}

	return true;
}

ZERegEx::ZERegEx()
{
	Code = NULL;
}

ZERegEx::ZERegEx(const ZEString& RegEx, ZERegExFlags Flags)
{
	Code = NULL;
	Compile(RegEx, Flags);
}

ZERegEx::~ZERegEx()
{
	if (Code != NULL)
		delete Code;
}
