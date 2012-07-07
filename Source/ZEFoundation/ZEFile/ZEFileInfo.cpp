//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFileInfo.cpp
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

#include "ZEDS/ZEArray.h"
#include "ZEFileInfo.h"

#include <memory.h>


ZEFileInfo::ZEFileInfo()
{
	Size = 0;
	memset((void*)&Creation, 0, sizeof(ZEFileTime));
	memset((void*)&Modification, 0, sizeof(ZEFileTime));
}

ZEFileInfo::ZEFileInfo(const ZEString& FilePath)
{
	Size = 0;
	Path = ZEPathManager::GetFinalPath(FilePath, &Root);
	Extension = GetFileExtension(Path);
	Name = GetFileName(Path);

	memset((void*)&Creation, 0, sizeof(ZEFileTime));
	memset((void*)&Modification, 0, sizeof(ZEFileTime));
}

ZEFileInfo::~ZEFileInfo()
{

}

ZESize ZEFileInfo::GetSize()
{
	Size = ZEFileUtils::GetFileSize(Path);

	return Size;
}

const ZEString& ZEFileInfo::GetName() const
{
	return Name;
}

const ZEString&	ZEFileInfo::GetPath() const
{
	return Path;
}

const ZEString&	ZEFileInfo::GetExtension() const
{
	return Extension;
}

bool ZEFileInfo::GetCreationDate(ZEFileTime& Time)
{
    bool Result;

	Result = ZEFileUtils::GetCreationTime(&Creation, Path);
    memcpy((void*)&Time, (void*)&Creation, sizeof(ZEFileTime));

	return Result;
}

bool ZEFileInfo::GetModificationDate(ZEFileTime& Time)
{
    bool Result;

    Result = ZEFileUtils::GetModificationTime(&Modification, Path);
    memcpy((void*)&Time, (void*)&Modification, sizeof(ZEFileTime));

    return Result;
}

bool ZEFileInfo::IsFile(const ZEString& FilePath)
{
	return ZEFileUtils::IsFile(FilePath);
}

ZEString ZEFileInfo::GetFileName(const ZEString& FilePath)
{
	ZESize Length = FilePath.GetLength();

	if (Length == 0)
		return "";

	for (ZESSize I = Length - 1; I >= 0; I--)
	{
		if (FilePath[I] == '\\' || FilePath[I] == '/')
			return FilePath.Right(Length - 1 - I);
	}

	return FilePath;
}

ZEString ZEFileInfo::GetFileExtension(const ZEString& FilePath)
{
	ZESize Length = FilePath.GetLength();

	if (Length == 0)
		return "";

	for (ZESSize I = Length - 1; I >= 0; I--)
	{
		if(FilePath[I] == '.')
			return FilePath.Right(Length - I);
	}

	return "";
}

ZEString ZEFileInfo::GetParentFolder(const ZEString& Path)
{
	ZESize Length = Path.GetLength();

	if (Length == 0)
		return "";

	for (ZESSize I = Length - 1; I >= 0; I--)
	{
		if (Path[I] == '\\' || Path[I] == '/')
			return Path.Left(I);
	}

	return "";
}
