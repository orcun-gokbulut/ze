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
	return ZEFormat::Format(Format, NULL, 0);
}

ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0)
{
	const ZEVariant* ArgumentArray[1] = { &Arg0 };
	return ZEFormat::Format(Format, ArgumentArray, 1);
}

ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1)
{
	const ZEVariant* ArgumentArray[2] =	{ &Arg0, &Arg1 };
	return ZEFormat::Format(Format, ArgumentArray, 2);
}

ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2)
{
	const ZEVariant* ArgumentArray[3] = { &Arg0, &Arg1, &Arg2 };
	return ZEFormat::Format(Format, ArgumentArray, 3);
}

ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3)
{
	const ZEVariant* ArgumentArray[4] =	{ &Arg0, &Arg1,	&Arg2, &Arg3 };
	return ZEFormat::Format(Format, ArgumentArray, 4);
}

ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4)
{
	const ZEVariant* ArgumentArray[5] =	{ &Arg0, &Arg1, &Arg2, &Arg3, &Arg4	};
	return ZEFormat::Format(Format, ArgumentArray, 5);
}

ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5)
{
	const ZEVariant* ArgumentArray[6] =	{ &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5 };
	return ZEFormat::Format(Format, ArgumentArray, 6);
}

ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6)
{
	const ZEVariant* ArgumentArray[7] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6 };
	return ZEFormat::Format(Format, ArgumentArray, 7);
}

ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7)
{
	const ZEVariant* ArgumentArray[8] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7 };
	return ZEFormat::Format(Format, ArgumentArray, 8);
}

ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8)
{
	const ZEVariant* ArgumentArray[9] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7, &Arg8 };
	return ZEFormat::Format(Format, ArgumentArray, 9);
}

ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9)
{
	const ZEVariant* ArgumentArray[10] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7, &Arg8, &Arg9 };
	return ZEFormat::Format(Format, ArgumentArray, 10);
}

ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10)
{
	const ZEVariant* ArgumentArray[11] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7, &Arg8, &Arg9, &Arg10 };
	return ZEFormat::Format(Format, ArgumentArray, 11);
}

ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11)
{
	const ZEVariant* ArgumentArray[12] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7, &Arg8, &Arg9, &Arg10, &Arg11 };
	return ZEFormat::Format(Format, ArgumentArray, 12);
}

ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12)
{
	const ZEVariant* ArgumentArray[13] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7, &Arg8, &Arg9, &Arg10, &Arg11, &Arg12 };
	return ZEFormat::Format(Format, ArgumentArray, 13);
}

ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12, const ZEVariant& Arg13)
{
	const ZEVariant* ArgumentArray[14] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7, &Arg8, &Arg9, &Arg10, &Arg11, &Arg12, &Arg13 };
	return ZEFormat::Format(Format, ArgumentArray, 14);
}

ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12, const ZEVariant& Arg13, const ZEVariant& Arg14)
{
	const ZEVariant* ArgumentArray[15] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7, &Arg8, &Arg9, &Arg10, &Arg11, &Arg12, &Arg13, &Arg14 };
	return ZEFormat::Format(Format, ArgumentArray, 15);
}

ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12, const ZEVariant& Arg13, const ZEVariant& Arg14, const ZEVariant& Arg15)
{
	const ZEVariant* ArgumentArray[16] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7, &Arg8, &Arg9, &Arg10, &Arg11, &Arg12, &Arg13, &Arg14, &Arg15 };
	return ZEFormat::Format(Format, ArgumentArray, 16);
}

ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12, const ZEVariant& Arg13, const ZEVariant& Arg14, const ZEVariant& Arg15, const ZEVariant& Arg16)
{
	const ZEVariant* ArgumentArray[17] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7, &Arg8, &Arg9, &Arg10, &Arg11, &Arg12, &Arg13, &Arg14, &Arg15, &Arg16 };
	return ZEFormat::Format(Format, ArgumentArray, 17);
}

ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12, const ZEVariant& Arg13, const ZEVariant& Arg14, const ZEVariant& Arg15, const ZEVariant& Arg16, const ZEVariant& Arg17)
{
	const ZEVariant* ArgumentArray[18] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7, &Arg8, &Arg9, &Arg10, &Arg11, &Arg12, &Arg13, &Arg14, &Arg15, &Arg16, &Arg17 };
	return ZEFormat::Format(Format, ArgumentArray, 18);
}

ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12, const ZEVariant& Arg13, const ZEVariant& Arg14, const ZEVariant& Arg15, const ZEVariant& Arg16, const ZEVariant& Arg17, const ZEVariant& Arg18)
{
	const ZEVariant* ArgumentArray[19] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7, &Arg8, &Arg9, &Arg10, &Arg11, &Arg12, &Arg13, &Arg14, &Arg15, &Arg16, &Arg17, &Arg18 };
	return ZEFormat::Format(Format, ArgumentArray, 19);
}

ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12, const ZEVariant& Arg13, const ZEVariant& Arg14, const ZEVariant& Arg15, const ZEVariant& Arg16, const ZEVariant& Arg17, const ZEVariant& Arg18, const ZEVariant& Arg19)
{
	const ZEVariant* ArgumentArray[20] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7, &Arg8, &Arg9, &Arg10, &Arg11, &Arg12, &Arg13, &Arg14, &Arg15, &Arg16, &Arg17, &Arg18, &Arg19 };
	return ZEFormat::Format(Format, ArgumentArray, 20);
}

ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12, const ZEVariant& Arg13, const ZEVariant& Arg14, const ZEVariant& Arg15, const ZEVariant& Arg16, const ZEVariant& Arg17, const ZEVariant& Arg18, const ZEVariant& Arg19, const ZEVariant& Arg20)
{
	const ZEVariant* ArgumentArray[21] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7, &Arg8, &Arg9, &Arg10, &Arg11, &Arg12, &Arg13, &Arg14, &Arg15, &Arg16, &Arg17, &Arg18, &Arg19, &Arg20 };
	return ZEFormat::Format(Format, ArgumentArray, 21);
}

ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12, const ZEVariant& Arg13, const ZEVariant& Arg14, const ZEVariant& Arg15, const ZEVariant& Arg16, const ZEVariant& Arg17, const ZEVariant& Arg18, const ZEVariant& Arg19, const ZEVariant& Arg20, const ZEVariant& Arg21)
{
	const ZEVariant* ArgumentArray[22] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7, &Arg8, &Arg9, &Arg10, &Arg11, &Arg12, &Arg13, &Arg14, &Arg15, &Arg16, &Arg17, &Arg18, &Arg19, &Arg20, &Arg21 };
	return ZEFormat::Format(Format, ArgumentArray, 22);
}

ZEString ZEFormat::Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12, const ZEVariant& Arg13, const ZEVariant& Arg14, const ZEVariant& Arg15, const ZEVariant& Arg16, const ZEVariant& Arg17, const ZEVariant& Arg18, const ZEVariant& Arg19, const ZEVariant& Arg20, const ZEVariant& Arg21, const ZEVariant& Arg22)
{
	const ZEVariant* ArgumentArray[23] = { &Arg0, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7, &Arg8, &Arg9, &Arg10, &Arg11, &Arg12, &Arg13, &Arg14, &Arg15, &Arg16, &Arg17, &Arg18, &Arg19, &Arg20, &Arg21, &Arg22 };
	return ZEFormat::Format(Format, ArgumentArray, 23);
}
