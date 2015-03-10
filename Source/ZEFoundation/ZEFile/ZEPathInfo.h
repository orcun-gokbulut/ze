//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPathInfo.h
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
#ifndef __ZE_PATH_INFO_H__
#define __ZE_PATH_INFO_H__

#include "ZETypes.h"
#include "ZEFileCommon.h"
#include "ZEDS/ZEString.h"
#include "ZEDS/ZEArray.h"

class ZEPathInfo
{
	protected:
		ZEString				Path;

	public:
		void					SetPath(const char* Path);
		void					SetRelativePath(const char* ParentPath, const char* RelativePath);

		const ZEString&			GetPath();
		ZEString				GetFileName();
		ZEString				GetName();
		ZEString				GetExtension();
		ZEString				GetParentDirectory();
		ZEPathRoot				GetRoot();
		ZEPathAccess			GetAccess();
		ZERealPath				GetRealPath();

		bool					IsExists();
		bool					IsFile();
		bool					IsDirectory();
		bool					IsInsidePackage();

		bool					IsParent(const char* ParentPath);
		ZEString				GetRelativeTo(const char* ParentPath);

		ZEFileTime				GetCreationDate();
		ZEFileTime				GetModificationTime();

		ZEString				Normalize();

								ZEPathInfo();
								ZEPathInfo(const char* Path);
								ZEPathInfo(const char* ParentPath, const char* RelativePath);

		static ZEArray<ZEString>	Divide(const char* Path);
		static bool					Normalize(ZEArray<ZEString>& PathElements);
		static ZEString				Construct(const ZEArray<ZEString>& PathElements);
		static bool					CheckParent(const ZEArray<ZEString>& ParentPathElemens, const ZEArray<ZEString>& ChildrenPathElements);
		static ZEArray<ZEString>	RelativeTo(const ZEArray<ZEString>& ParentPathElemens, const ZEArray<ZEString>& ChildrenPathElements);
};

#endif
