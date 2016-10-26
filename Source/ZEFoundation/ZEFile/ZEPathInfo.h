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
#include "ZETimeStamp.h"
#include "ZEFileCommon.h"
#include "ZEDS/ZEString.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEDelegate.h"

class ZEPathTokenizer;

enum ZEPathOperationElement
{
	ZE_POE_NONE			= 0,
	ZE_POE_FILE			= 1,
	ZE_POE_DIRECTORY	= 2,
	ZE_POE_ALL			= 3
};

typedef ZEDelegate<bool (const char*, ZEPathOperationElement, void*)> ZEPathOperationFunction;

class ZEPathInfo
{
	private:
		ZEString					Path;

	public:
		void						SetPath(const char* Path);

		const ZEString&				GetPath() const;
		ZEString					GetFileName() const;
		ZEString					GetName() const;
		ZEString					GetExtension() const;
		ZEString					GetParentDirectory() const;
		ZEPathRoot					GetRoot() const;
		ZEPathAccess				GetAccess() const;
		ZERealPath					GetRealPath() const;

		bool						IsExists() const;
		bool						IsFile() const;
		bool						IsDirectory() const;
		bool						IsInsidePackage() const;

		bool						IsParent(const char* ParentPath) const;
		ZEString					GetRelativeTo(const char* ParentPath) const;

		ZETimeStamp					GetCreationDate() const;
		ZETimeStamp					GetModificationTime() const;

		ZEString					Normalize();

		bool						Equals(const ZEPathInfo& OtherPath) const;

		static bool					Operate(const char* TargetDirectory, ZEPathOperationFunction Function, ZEPathOperationElement Elements = ZE_POE_FILE, bool Recursive = true, void* Parameters = NULL);

									ZEPathInfo();
									ZEPathInfo(const char* Path);

		static bool					Compare(const ZEString& PathA, const ZEString& PathB);
		static bool					Normalize(ZEPathTokenizer& Tokenizer);
		static ZEString				CombineRelativePath(const char* ParentFilePath, const char* Path);
};

#endif
