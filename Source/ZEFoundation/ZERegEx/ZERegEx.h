//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERegEx.h
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
#ifndef ZE_REG_EX
#define ZE_REG_EX

#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEFlags.h"
#include "ZETypes.h"

typedef ZEFlags ZERegExFlags;
#define ZE_REF_NONE						0
#define ZE_REF_IN_CASE_SENSITIVE		1
#define ZE_REF_NEW_LINE					2

class ZEString;

class ZERegExMatch
{
	public:
		ZESize		Offset;
		ZESize		Size;
};

class ZERegEx
{
	private:
		void*			Code;

	public:
		bool			Compile(const ZEString& RegEx, ZERegExFlags Flags = ZE_REF_NONE);

		bool			Match(const ZEString& String);
		bool			Match(const ZEString& String, ZEArray<ZERegExMatch>& Matches);

						ZERegEx();
						ZERegEx(const ZEString& RegEx, ZERegExFlags Flags = ZE_REF_NONE);
						~ZERegEx();

		static bool		Match(const ZEString& RegEx, const ZEString& String, ZERegExFlags Flags = ZE_REF_NONE);
		static bool		Match(const ZEString& RegEx, const ZEString& String, ZEArray<ZERegExMatch>& Matches, ZERegExFlags Flags = ZE_REF_NONE);
};

#endif
