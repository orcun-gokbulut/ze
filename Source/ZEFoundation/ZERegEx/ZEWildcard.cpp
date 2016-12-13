//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEWildcard.cpp
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

#include "ZEWildcard.h"

const ZEString& ZEWildcard::GetPattern() const
{
	return Pattern;
}

bool ZEWildcard::Compile(const ZEString& Pattern)
{
	this->Pattern = Pattern;
	return true;
}

bool ZEWildcard::Match(const ZEString& Input) const
{
	if (Pattern.IsEmpty())
		return false;

	if (Input.IsEmpty())
		return false;

	const char* pat = Pattern.GetValue();
	const char* str = Input.GetValue();
	const char* s;
	const char* p;
	bool star = false;

	loopStart:
		for (s = str, p = pat; *s; ++s, ++p) 
		{
			switch (*p) 
			{
			case '?':
				if (*s == '.') goto starCheck;
				break;

			case '*':
				star = true;
				str = s, pat = p;
				if (!*++pat) 
					return true;
				goto loopStart;

			default:
				if (toupper(*s) != toupper(*p))
					goto starCheck;
				break;
			}
		}

		if (*p == '*') 
			++p;

		return (!*p);

	starCheck:
		if (!star)
			return false;

	str++;
	goto loopStart;
}

ZEWildcard::ZEWildcard()
{

}

ZEWildcard::ZEWildcard(const ZEString& Pattern)
{
	Compile(Pattern);
}

ZEWildcard::~ZEWildcard()
{

}
