//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFormat.h
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

#pragma  once
#ifndef __ZE_FORMAT_H__
#define __ZE_FORMAT_H__

#include "ZEDS/ZEString.h"
#include "ZEDS/ZEVariant.h"
#include "ZETypes.h"

class ZEStringWriter;
class ZEVector2;
class ZEVector3;
class ZEVector4;
class ZEQuaternion;
class ZEMatrix3x3;
class ZEMatrix4x4;
class ZEObject;

class ZEFormat
{
	private:
		static ZEString		FormatInner(const char* Input, const ZEVariant** Arguments, ZESize Count);

	public :
		static ZEString		Format(const char* Format);
		static ZEString		Format(const char* Format, const ZEVariant& Arg0);
		static ZEString		Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1);
		static ZEString		Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2);
		static ZEString		Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3);
		static ZEString		Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4);
		static ZEString		Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5);
		static ZEString		Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6);
		static ZEString		Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7);
		static ZEString		Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8);
		static ZEString		Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9);
		static ZEString		Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10);
		static ZEString		Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11);
		static ZEString		Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12);
		static ZEString		Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12, const ZEVariant& Arg13);
		static ZEString		Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12, const ZEVariant& Arg13, const ZEVariant& Arg14);
		static ZEString		Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12, const ZEVariant& Arg13, const ZEVariant& Arg14, const ZEVariant& Arg15);
		static ZEString		Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12, const ZEVariant& Arg13, const ZEVariant& Arg14, const ZEVariant& Arg15, const ZEVariant& Arg16);
		static ZEString		Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12, const ZEVariant& Arg13, const ZEVariant& Arg14, const ZEVariant& Arg15, const ZEVariant& Arg16, const ZEVariant& Arg17);
		static ZEString		Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12, const ZEVariant& Arg13, const ZEVariant& Arg14, const ZEVariant& Arg15, const ZEVariant& Arg16, const ZEVariant& Arg17, const ZEVariant& Arg18);
		static ZEString		Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12, const ZEVariant& Arg13, const ZEVariant& Arg14, const ZEVariant& Arg15, const ZEVariant& Arg16, const ZEVariant& Arg17, const ZEVariant& Arg18, const ZEVariant& Arg19);
		static ZEString		Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12, const ZEVariant& Arg13, const ZEVariant& Arg14, const ZEVariant& Arg15, const ZEVariant& Arg16, const ZEVariant& Arg17, const ZEVariant& Arg18, const ZEVariant& Arg19, const ZEVariant& Arg20);
		static ZEString		Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12, const ZEVariant& Arg13, const ZEVariant& Arg14, const ZEVariant& Arg15, const ZEVariant& Arg16, const ZEVariant& Arg17, const ZEVariant& Arg18, const ZEVariant& Arg19, const ZEVariant& Arg20, const ZEVariant& Arg21);
		static ZEString		Format(const char* Format, const ZEVariant& Arg0, const ZEVariant& Arg1, const ZEVariant& Arg2, const ZEVariant& Arg3, const ZEVariant& Arg4, const ZEVariant& Arg5, const ZEVariant& Arg6, const ZEVariant& Arg7, const ZEVariant& Arg8, const ZEVariant& Arg9, const ZEVariant& Arg10, const ZEVariant& Arg11, const ZEVariant& Arg12, const ZEVariant& Arg13, const ZEVariant& Arg14, const ZEVariant& Arg15, const ZEVariant& Arg16, const ZEVariant& Arg17, const ZEVariant& Arg18, const ZEVariant& Arg19, const ZEVariant& Arg20, const ZEVariant& Arg21, const ZEVariant& Arg22);
};

#endif
