//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPathUtils.h
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
#ifndef __ZE_PATH_UTILS_H__
#define __ZE_PATH_UTILS_H__


#include "ZEDS/ZEString.h"
#include "ZEPathManager.h"


#define ISSEPERATOR(Char)		(((Char) == '\\') || ((Char) == '/'))
#define ISDRIVELETTER(Char)		((('a' < (Char)) && ((Char) < 'z')) || (('A' < (Char)) && ((Char) < 'Z')))

class ZEPathUtils
{
	private:
									ZEPathUtils();
									~ZEPathUtils();

	public:
		static const ZEString&		GetDot();
		static const ZEString&		GetDotDot();
		static const ZEString&		GetSeperator();
		static const ZEString&		GetEmptyPath();

		static ZEString				GetSimplifiedPath(const ZEString& Path);

		static ZEKnownPath			SearchForSymbol(ZEString* RelativePart, const ZEString& SymbolicPath);

		static bool					CheckForRestriction(const ZEString& RootPath, const ZEString& Path);
		
		static bool					IsAbsolutePath(const ZEString& Path);
		static ZEString				GetAbsolutePath(const ZEString& RootPath, const ZEString& RelativePath);

		static bool					IsRelativePath(const ZEString& Path);
		static ZEString				GetRelativePath(const ZEString& RootPath, const ZEString& AbsolutePath);
};


#endif
