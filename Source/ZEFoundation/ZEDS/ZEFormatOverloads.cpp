//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFormatOverloads.cpp
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

#include "ZEFormat.h"

ZEString ZEFormat::Format(const char* Format)
{
	return ZEFormat::FormatInner(Format, NULL, 0);
}

ZEString ZEFormat::Format(const char* Format, const ZEValue& Arg0)
{
	const ZEValue* ArgumentArray[1] = { &Arg0 };
	return ZEFormat::FormatInner(Format, ArgumentArray, 1);
}

ZEString ZEFormat::Format(const char* Format, const ZEValue& Arg0, const ZEValue& Arg1)
{
	const ZEValue* ArgumentArray[2] =	{ &Arg0, &Arg1 };
	return ZEFormat::FormatInner(Format, ArgumentArray, 2);
}

ZEString ZEFormat::Format(const char* Format, const ZEValue& Arg0, const ZEValue& Arg1, const ZEValue& Arg2)
{
	const ZEValue* ArgumentArray[3] = { &Arg0, &Arg1, &Arg2 };
	return ZEFormat::FormatInner(Format, ArgumentArray, 3);
}

ZEString ZEFormat::Format(const char* Format, const ZEValue& Arg0, const ZEValue& Arg1, const ZEValue& Arg2, const ZEValue& Arg3)
{
	const ZEValue* ArgumentArray[4] =	{ &Arg0, &Arg1,	&Arg2, &Arg3 };
	return ZEFormat::FormatInner(Format, ArgumentArray, 4);
}

ZEString ZEFormat::Format(const char* Format, const ZEValue& Arg0, const ZEValue& Arg1, const ZEValue& Arg2, const ZEValue& Arg3, const ZEValue& Arg4)
{
	const ZEValue* ArgumentArray[5] =	{ &Arg0, &Arg1, &Arg2, &Arg3, &Arg4	};
	return ZEFormat::FormatInner(Format, ArgumentArray, 5);
}

ZEString ZEFormat::Format(const char* Format, const ZEValue& Arg0, const ZEValue& Arg1, const ZEValue& Arg2, const ZEValue& Arg3, const ZEValue& Arg4, const ZEValue& Arg5)
{
	const ZEValue* ArgumentArray[6] =	{ &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5 };
	return ZEFormat::FormatInner(Format, ArgumentArray, 6);
}

ZEString ZEFormat::Format(const char* Format, const ZEValue& Arg0, const ZEValue& Arg1, const ZEValue& Arg2, const ZEValue& Arg3, const ZEValue& Arg4, const ZEValue& Arg5, const ZEValue& Arg6)
{
	const ZEValue* ArgumentArray[7] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6 };
	return ZEFormat::FormatInner(Format, ArgumentArray, 7);
}

ZEString ZEFormat::Format(const char* Format, const ZEValue& Arg0, const ZEValue& Arg1, const ZEValue& Arg2, const ZEValue& Arg3, const ZEValue& Arg4, const ZEValue& Arg5, const ZEValue& Arg6, const ZEValue& Arg7)
{
	const ZEValue* ArgumentArray[8] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7 };
	return ZEFormat::FormatInner(Format, ArgumentArray, 8);
}

ZEString ZEFormat::Format(const char* Format, const ZEValue& Arg0, const ZEValue& Arg1, const ZEValue& Arg2, const ZEValue& Arg3, const ZEValue& Arg4, const ZEValue& Arg5, const ZEValue& Arg6, const ZEValue& Arg7, const ZEValue& Arg8)
{
	const ZEValue* ArgumentArray[9] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7, &Arg8 };
	return ZEFormat::FormatInner(Format, ArgumentArray, 9);
}

ZEString ZEFormat::Format(const char* Format, const ZEValue& Arg0, const ZEValue& Arg1, const ZEValue& Arg2, const ZEValue& Arg3, const ZEValue& Arg4, const ZEValue& Arg5, const ZEValue& Arg6, const ZEValue& Arg7, const ZEValue& Arg8, const ZEValue& Arg9)
{
	const ZEValue* ArgumentArray[10] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7, &Arg8, &Arg9 };
	return ZEFormat::FormatInner(Format, ArgumentArray, 10);
}

ZEString ZEFormat::Format(const char* Format, const ZEValue& Arg0, const ZEValue& Arg1, const ZEValue& Arg2, const ZEValue& Arg3, const ZEValue& Arg4, const ZEValue& Arg5, const ZEValue& Arg6, const ZEValue& Arg7, const ZEValue& Arg8, const ZEValue& Arg9, const ZEValue& Arg10)
{
	const ZEValue* ArgumentArray[11] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7, &Arg8, &Arg9, &Arg10 };
	return ZEFormat::FormatInner(Format, ArgumentArray, 11);
}

ZEString ZEFormat::Format(const char* Format, const ZEValue& Arg0, const ZEValue& Arg1, const ZEValue& Arg2, const ZEValue& Arg3, const ZEValue& Arg4, const ZEValue& Arg5, const ZEValue& Arg6, const ZEValue& Arg7, const ZEValue& Arg8, const ZEValue& Arg9, const ZEValue& Arg10, const ZEValue& Arg11)
{
	const ZEValue* ArgumentArray[12] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7, &Arg8, &Arg9, &Arg10, &Arg11 };
	return ZEFormat::FormatInner(Format, ArgumentArray, 12);
}

ZEString ZEFormat::Format(const char* Format, const ZEValue& Arg0, const ZEValue& Arg1, const ZEValue& Arg2, const ZEValue& Arg3, const ZEValue& Arg4, const ZEValue& Arg5, const ZEValue& Arg6, const ZEValue& Arg7, const ZEValue& Arg8, const ZEValue& Arg9, const ZEValue& Arg10, const ZEValue& Arg11, const ZEValue& Arg12)
{
	const ZEValue* ArgumentArray[13] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7, &Arg8, &Arg9, &Arg10, &Arg11, &Arg12 };
	return ZEFormat::FormatInner(Format, ArgumentArray, 13);
}

ZEString ZEFormat::Format(const char* Format, const ZEValue& Arg0, const ZEValue& Arg1, const ZEValue& Arg2, const ZEValue& Arg3, const ZEValue& Arg4, const ZEValue& Arg5, const ZEValue& Arg6, const ZEValue& Arg7, const ZEValue& Arg8, const ZEValue& Arg9, const ZEValue& Arg10, const ZEValue& Arg11, const ZEValue& Arg12, const ZEValue& Arg13)
{
	const ZEValue* ArgumentArray[14] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7, &Arg8, &Arg9, &Arg10, &Arg11, &Arg12, &Arg13 };
	return ZEFormat::FormatInner(Format, ArgumentArray, 14);
}

ZEString ZEFormat::Format(const char* Format, const ZEValue& Arg0, const ZEValue& Arg1, const ZEValue& Arg2, const ZEValue& Arg3, const ZEValue& Arg4, const ZEValue& Arg5, const ZEValue& Arg6, const ZEValue& Arg7, const ZEValue& Arg8, const ZEValue& Arg9, const ZEValue& Arg10, const ZEValue& Arg11, const ZEValue& Arg12, const ZEValue& Arg13, const ZEValue& Arg14)
{
	const ZEValue* ArgumentArray[15] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7, &Arg8, &Arg9, &Arg10, &Arg11, &Arg12, &Arg13, &Arg14 };
	return ZEFormat::FormatInner(Format, ArgumentArray, 15);
}

ZEString ZEFormat::Format(const char* Format, const ZEValue& Arg0, const ZEValue& Arg1, const ZEValue& Arg2, const ZEValue& Arg3, const ZEValue& Arg4, const ZEValue& Arg5, const ZEValue& Arg6, const ZEValue& Arg7, const ZEValue& Arg8, const ZEValue& Arg9, const ZEValue& Arg10, const ZEValue& Arg11, const ZEValue& Arg12, const ZEValue& Arg13, const ZEValue& Arg14, const ZEValue& Arg15)
{
	const ZEValue* ArgumentArray[16] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7, &Arg8, &Arg9, &Arg10, &Arg11, &Arg12, &Arg13, &Arg14, &Arg15 };
	return ZEFormat::FormatInner(Format, ArgumentArray, 16);
}

ZEString ZEFormat::Format(const char* Format, const ZEValue& Arg0, const ZEValue& Arg1, const ZEValue& Arg2, const ZEValue& Arg3, const ZEValue& Arg4, const ZEValue& Arg5, const ZEValue& Arg6, const ZEValue& Arg7, const ZEValue& Arg8, const ZEValue& Arg9, const ZEValue& Arg10, const ZEValue& Arg11, const ZEValue& Arg12, const ZEValue& Arg13, const ZEValue& Arg14, const ZEValue& Arg15, const ZEValue& Arg16)
{
	const ZEValue* ArgumentArray[17] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7, &Arg8, &Arg9, &Arg10, &Arg11, &Arg12, &Arg13, &Arg14, &Arg15, &Arg16 };
	return ZEFormat::FormatInner(Format, ArgumentArray, 17);
}

ZEString ZEFormat::Format(const char* Format, const ZEValue& Arg0, const ZEValue& Arg1, const ZEValue& Arg2, const ZEValue& Arg3, const ZEValue& Arg4, const ZEValue& Arg5, const ZEValue& Arg6, const ZEValue& Arg7, const ZEValue& Arg8, const ZEValue& Arg9, const ZEValue& Arg10, const ZEValue& Arg11, const ZEValue& Arg12, const ZEValue& Arg13, const ZEValue& Arg14, const ZEValue& Arg15, const ZEValue& Arg16, const ZEValue& Arg17)
{
	const ZEValue* ArgumentArray[18] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7, &Arg8, &Arg9, &Arg10, &Arg11, &Arg12, &Arg13, &Arg14, &Arg15, &Arg16, &Arg17 };
	return ZEFormat::FormatInner(Format, ArgumentArray, 18);
}

ZEString ZEFormat::Format(const char* Format, const ZEValue& Arg0, const ZEValue& Arg1, const ZEValue& Arg2, const ZEValue& Arg3, const ZEValue& Arg4, const ZEValue& Arg5, const ZEValue& Arg6, const ZEValue& Arg7, const ZEValue& Arg8, const ZEValue& Arg9, const ZEValue& Arg10, const ZEValue& Arg11, const ZEValue& Arg12, const ZEValue& Arg13, const ZEValue& Arg14, const ZEValue& Arg15, const ZEValue& Arg16, const ZEValue& Arg17, const ZEValue& Arg18)
{
	const ZEValue* ArgumentArray[19] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7, &Arg8, &Arg9, &Arg10, &Arg11, &Arg12, &Arg13, &Arg14, &Arg15, &Arg16, &Arg17, &Arg18 };
	return ZEFormat::FormatInner(Format, ArgumentArray, 19);
}

ZEString ZEFormat::Format(const char* Format, const ZEValue& Arg0, const ZEValue& Arg1, const ZEValue& Arg2, const ZEValue& Arg3, const ZEValue& Arg4, const ZEValue& Arg5, const ZEValue& Arg6, const ZEValue& Arg7, const ZEValue& Arg8, const ZEValue& Arg9, const ZEValue& Arg10, const ZEValue& Arg11, const ZEValue& Arg12, const ZEValue& Arg13, const ZEValue& Arg14, const ZEValue& Arg15, const ZEValue& Arg16, const ZEValue& Arg17, const ZEValue& Arg18, const ZEValue& Arg19)
{
	const ZEValue* ArgumentArray[20] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7, &Arg8, &Arg9, &Arg10, &Arg11, &Arg12, &Arg13, &Arg14, &Arg15, &Arg16, &Arg17, &Arg18, &Arg19 };
	return ZEFormat::FormatInner(Format, ArgumentArray, 20);
}

ZEString ZEFormat::Format(const char* Format, const ZEValue& Arg0, const ZEValue& Arg1, const ZEValue& Arg2, const ZEValue& Arg3, const ZEValue& Arg4, const ZEValue& Arg5, const ZEValue& Arg6, const ZEValue& Arg7, const ZEValue& Arg8, const ZEValue& Arg9, const ZEValue& Arg10, const ZEValue& Arg11, const ZEValue& Arg12, const ZEValue& Arg13, const ZEValue& Arg14, const ZEValue& Arg15, const ZEValue& Arg16, const ZEValue& Arg17, const ZEValue& Arg18, const ZEValue& Arg19, const ZEValue& Arg20)
{
	const ZEValue* ArgumentArray[21] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7, &Arg8, &Arg9, &Arg10, &Arg11, &Arg12, &Arg13, &Arg14, &Arg15, &Arg16, &Arg17, &Arg18, &Arg19, &Arg20 };
	return ZEFormat::FormatInner(Format, ArgumentArray, 21);
}

ZEString ZEFormat::Format(const char* Format, const ZEValue& Arg0, const ZEValue& Arg1, const ZEValue& Arg2, const ZEValue& Arg3, const ZEValue& Arg4, const ZEValue& Arg5, const ZEValue& Arg6, const ZEValue& Arg7, const ZEValue& Arg8, const ZEValue& Arg9, const ZEValue& Arg10, const ZEValue& Arg11, const ZEValue& Arg12, const ZEValue& Arg13, const ZEValue& Arg14, const ZEValue& Arg15, const ZEValue& Arg16, const ZEValue& Arg17, const ZEValue& Arg18, const ZEValue& Arg19, const ZEValue& Arg20, const ZEValue& Arg21)
{
	const ZEValue* ArgumentArray[22] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7, &Arg8, &Arg9, &Arg10, &Arg11, &Arg12, &Arg13, &Arg14, &Arg15, &Arg16, &Arg17, &Arg18, &Arg19, &Arg20, &Arg21 };
	return ZEFormat::FormatInner(Format, ArgumentArray, 22);
}

ZEString ZEFormat::Format(const char* Format, const ZEValue& Arg0, const ZEValue& Arg1, const ZEValue& Arg2, const ZEValue& Arg3, const ZEValue& Arg4, const ZEValue& Arg5, const ZEValue& Arg6, const ZEValue& Arg7, const ZEValue& Arg8, const ZEValue& Arg9, const ZEValue& Arg10, const ZEValue& Arg11, const ZEValue& Arg12, const ZEValue& Arg13, const ZEValue& Arg14, const ZEValue& Arg15, const ZEValue& Arg16, const ZEValue& Arg17, const ZEValue& Arg18, const ZEValue& Arg19, const ZEValue& Arg20, const ZEValue& Arg21, const ZEValue& Arg22)
{
	const ZEValue* ArgumentArray[23] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7, &Arg8, &Arg9, &Arg10, &Arg11, &Arg12, &Arg13, &Arg14, &Arg15, &Arg16, &Arg17, &Arg18, &Arg19, &Arg20, &Arg21, &Arg22 };
	return ZEFormat::FormatInner(Format, ArgumentArray, 23);
}

ZEString ZEFormat::Format(const char* Format, const ZEValue& Arg0, const ZEValue& Arg1, const ZEValue& Arg2, const ZEValue& Arg3, const ZEValue& Arg4, const ZEValue& Arg5, const ZEValue& Arg6, const ZEValue& Arg7, const ZEValue& Arg8, const ZEValue& Arg9, const ZEValue& Arg10, const ZEValue& Arg11, const ZEValue& Arg12, const ZEValue& Arg13, const ZEValue& Arg14, const ZEValue& Arg15, const ZEValue& Arg16, const ZEValue& Arg17, const ZEValue& Arg18, const ZEValue& Arg19, const ZEValue& Arg20, const ZEValue& Arg21, const ZEValue& Arg22, const ZEValue& Arg23)
{
	const ZEValue* ArgumentArray[24] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7, &Arg8, &Arg9, &Arg10, &Arg11, &Arg12, &Arg13, &Arg14, &Arg15, &Arg16, &Arg17, &Arg18, &Arg19, &Arg20, &Arg21, &Arg22, &Arg23 };
	return ZEFormat::FormatInner(Format, ArgumentArray, 24);
}

ZEString ZEFormat::Format(const char* Format, const ZEValue& Arg0, const ZEValue& Arg1, const ZEValue& Arg2, const ZEValue& Arg3, const ZEValue& Arg4, const ZEValue& Arg5, const ZEValue& Arg6, const ZEValue& Arg7, const ZEValue& Arg8, const ZEValue& Arg9, const ZEValue& Arg10, const ZEValue& Arg11, const ZEValue& Arg12, const ZEValue& Arg13, const ZEValue& Arg14, const ZEValue& Arg15, const ZEValue& Arg16, const ZEValue& Arg17, const ZEValue& Arg18, const ZEValue& Arg19, const ZEValue& Arg20, const ZEValue& Arg21, const ZEValue& Arg22, const ZEValue& Arg23, const ZEValue& Arg24)
{
	const ZEValue* ArgumentArray[25] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7, &Arg8, &Arg9, &Arg10, &Arg11, &Arg12, &Arg13, &Arg14, &Arg15, &Arg16, &Arg17, &Arg18, &Arg19, &Arg20, &Arg21, &Arg22, &Arg23, &Arg24 };
	return ZEFormat::FormatInner(Format, ArgumentArray, 25);
}
