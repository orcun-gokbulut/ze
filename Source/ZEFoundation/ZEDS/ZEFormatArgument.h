//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFormatArgument.h
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
#ifndef __ZE_FORMAT_ARGUMENT_H__
#define __ZE_FORMAT_ARGUMENT_H__

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

class ZEFormatArgument
{
	public :
		static bool			Format(ZEStringWriter& Output, ZEInt Argument, const char* ArgumentFormat);
		static bool			Format(ZEStringWriter& Output, ZEUInt Argument, const char* ArgumentFormat);
		static bool			Format(ZEStringWriter& Output, ZEInt64 Argument,	const char* ArgumentFormat);
		static bool			Format(ZEStringWriter& Output, ZEUInt64 Argument, const char* ArgumentFormat);
		static bool			Format(ZEStringWriter& Output, float Argument, const char* ArgumentFormat);
		static bool			Format(ZEStringWriter& Output, bool Argument, const char* ArgumentFormat);
		static bool			Format(ZEStringWriter& Output, const ZEVector2& Argument, const char* ArgumentFormat);
		static bool			Format(ZEStringWriter& Output, const ZEVector3& Argument, const char* ArgumentFormat);
		static bool			Format(ZEStringWriter& Output, const ZEVector4& Argument, const char* ArgumentFormat);
		static bool			Format(ZEStringWriter& Output, const ZEQuaternion& Argument, const char* ArgumentFormat);
		static bool			Format(ZEStringWriter& Output, const ZEMatrix3x3& Argument, const char* ArgumentFormat);
		static bool			Format(ZEStringWriter& Output, const ZEMatrix4x4& Argument,const char* ArgumentFormat);
		static bool			Format(ZEStringWriter& Output, const ZEString& Argument, const char* ArgumentFormat);
		static bool			Format(ZEStringWriter& Output, ZEObject* Argument,	const char* ArgumentFormat);
		static bool			FormatVariant(ZEStringWriter& Output, const ZEVariant& Argument, const char* ArgumentFormat);
};

#endif
