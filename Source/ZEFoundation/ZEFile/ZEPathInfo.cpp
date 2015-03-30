//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPathInfo.cpp
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

#include "ZEPathInfo.h"

#include "ZEDS/ZEFormat.h"
#include "ZEPathManager.h"
#include "ZEFileInfo.h"
#include "ZEDirectoryInfo.h"



void ZEPathInfo::SetPath(const char* Path)
{
	this->Path = Path;
}

void ZEPathInfo::SetRelativePath(const char* ParentPath, const char* RelativePath)
{
	ZEPathInfo ParentPathInfo = ZEPathInfo(ParentPath);
	if (ParentPathInfo.IsFile()) // Relative To File
	{
		ZEString ParentDirectory = ParentPathInfo.GetParentDirectory();
		Path = ZEFormat::Format("{0}/{1}", ParentDirectory, RelativePath);
	}
	else
	{
		Path = ZEFormat::Format("{0}/{1}", ParentPath, RelativePath);
	}
}

const ZEString&	ZEPathInfo::GetPath()
{
	return Path;
}

ZEString ZEPathInfo::GetFileName()
{
	ZESSize Length = Path.GetLength();
	for (ZESSize I = Length - 1; I >= 0; I--)
	{
		if (Path[I] == '\\' || Path[I] == '/')
			return Path.Right(Length - 1 - I).Trim();
	}

	return Path;
}

ZEString ZEPathInfo::GetName()
{
	ZESize Length = Path.GetLength();
	
	if (Length == 0)
		return "";
	

	ZESize Start = 0;
	for (ZESSize I = Length - 1; I >= 0; I--)
	{
		if (Path[I] == '\\' || Path[I] == '/')
		{
			Start = I + 1;
			break;
		}
	}

	ZESSize ExtensionStart = Length;
	for (ZESSize I = Start;  I < Length; I++)
	{
		if(Path[I] == '.')
			ExtensionStart = I;
	}

	Length = ExtensionStart - Start;
	return Path.Middle(Start, Length).Trim();
}

ZEString ZEPathInfo::GetExtension()
{
	ZESSize Length = Path.GetLength();

	for (ZESSize I = Length - 1; I >= 0; I--)
	{
		if (Path[I] == '.')
			return Path.Right(Length - I).Trim();
		else if (Path[I] == '/' || Path[I] == '\\')
			return "";
	}

	return "";
}

ZEString ZEPathInfo::GetParentDirectory()
{
	ZESSize Length = Path.GetLength();

	for (ZESSize I = Length - 1; I >= 0; I--)
	{
		if (Path[I] == '\\' || Path[I] == '/')
			return Path.Left(I).Trim();
	}

	return "";
}

ZERealPath ZEPathInfo::GetRealPath()
{
	return ZEPathManager::GetInstance()->TranslateToRealPath(Path);
}

ZEPathRoot ZEPathInfo::GetRoot()
{
	return ZEPathManager::GetInstance()->GetRoot(Path);
}

ZEPathAccess ZEPathInfo::GetAccess()
{
	ZERealPath RealPath = GetRealPath();
	return RealPath.Access;
}

bool ZEPathInfo::IsInsidePackage()
{
	return false;
}

bool ZEPathInfo::IsParent(const char* ParentPath)
{
	ZEArray<ZEString> ParentPathElements = Divide(ParentPath);
	if (!Normalize(ParentPathElements))
		return false;

	ZEArray<ZEString> PathElements = Divide(Path);
	if (!Normalize(PathElements))
		return false;

	return CheckParent(ParentPathElements, PathElements);
}

ZEString ZEPathInfo::GetRelativeTo(const char* ParentPath)
{
	ZEArray<ZEString> ParentPathElements = Divide(ParentPath);
	if (!Normalize(ParentPathElements))
		return false;

	ZEArray<ZEString> PathElements = Divide(Path);
	if (!Normalize(PathElements))
		return false;

	return Construct(RelativeTo(ParentPathElements, PathElements));
}

#ifdef ZE_PLATFORM_WINDOWS

#include <Windows.h>
#include <shlwapi.h>

bool ZEPathInfo::IsExists()
{
	if ((GetAccess() & ZE_PA_READ) == 0)
		return false;

	DWORD PathAttribute = GetFileAttributesA(GetRealPath().Path);
	if (PathAttribute == INVALID_FILE_ATTRIBUTES)
		return false; 
	else
		return true;
}

bool ZEPathInfo::IsFile()
{
	if ((GetAccess() & ZE_PA_READ) == 0)
		return false;

	DWORD PathAttribute = GetFileAttributesA(GetRealPath().Path);
	if (PathAttribute == INVALID_FILE_ATTRIBUTES)
		return false; 
	else if ((PathAttribute & FILE_ATTRIBUTE_DIRECTORY) == 0)
		return true;
	else
		return false;
}

bool ZEPathInfo::IsDirectory()
{
	if ((GetAccess() & ZE_PA_READ) == 0)
		return false;

	DWORD PathAttribute = GetFileAttributesA(GetRealPath().Path);
	if (PathAttribute == INVALID_FILE_ATTRIBUTES)
		return false; 
	else if ((PathAttribute & FILE_ATTRIBUTE_DIRECTORY) != 0)
		return true;
	else
		return false;
}

ZEFileTime ZEPathInfo::GetCreationDate()
{
	if ((GetAccess() & ZE_PA_READ) == 0)
		return ZEFileTime();

	HANDLE Handle = CreateFile(GetRealPath().Path, GENERIC_WRITE, FILE_SHARE_WRITE,	NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (Handle == INVALID_HANDLE_VALUE)
		return ZEFileTime();

	FILETIME CreationTime, Temp0, Temp1;
	if (!GetFileTime(Handle, &CreationTime, &Temp0, &Temp1))
	{
		CloseHandle(Handle);
		return ZEFileTime();
	}

	CloseHandle(Handle);

	SYSTEMTIME Time;
	FileTimeToSystemTime(&CreationTime, &Time);

	ZEFileTime Output;
	Output.Year = Time.wYear;
	Output.Month = Time.wMonth;
	Output.DayOfWeek = Time.wDayOfWeek;
	Output.Day = Time.wDay;
	Output.Hour = Time.wHour;
	Output.Minute = Time.wMinute;
	Output.Second = Time.wSecond;
	Output.Milliseconds = Time.wMilliseconds;

	return Output;
}

ZEFileTime ZEPathInfo::GetModificationTime()
{
	if ((GetAccess() & ZE_PA_READ) == 0)
		return ZEFileTime();

	HANDLE Handle = CreateFile(GetRealPath().Path, GENERIC_WRITE, FILE_SHARE_WRITE,	NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (Handle == INVALID_HANDLE_VALUE)
		return ZEFileTime();

	FILETIME ModificationTime, Temp0, Temp1;
	if (!GetFileTime(Handle, &Temp0, &Temp1, &ModificationTime))
	{
		CloseHandle(Handle);
		return ZEFileTime();
	}

	CloseHandle(Handle);

	SYSTEMTIME Time;
	FileTimeToSystemTime(&ModificationTime, &Time);

	ZEFileTime Output;
	Output.Year = Time.wYear;
	Output.Month = Time.wMonth;
	Output.DayOfWeek = Time.wDayOfWeek;
	Output.Day = Time.wDay;
	Output.Hour = Time.wHour;
	Output.Minute = Time.wMinute;
	Output.Second = Time.wSecond;
	Output.Milliseconds = Time.wMilliseconds;

	return Output;
}

#endif

ZEString ZEPathInfo::Normalize()
{
	ZEArray<ZEString> PathElements = Divide(Path);
	if (!Normalize(PathElements))
		return "";
	return Construct(PathElements);
}

bool ZEPathInfo::Operate(const char* TargetDirectory, ZEPathOperationFunction Function, ZEPathOperationElement Elements, bool Recursive)
{
	ZEDirectoryInfo TargetDirectoryInfo(TargetDirectory);

	ZEArray<ZEString> SubDirectories;
	if (Recursive || (Elements & ZE_POE_DIRECTORY))
		SubDirectories = TargetDirectoryInfo.GetSubDirectories();

	if (Recursive)
	{
		for (ZESize I = 0; I < SubDirectories.GetSize(); I++)
		{
			ZEString SubDirectoryPath = ZEFormat::Format("{0}/{1}", TargetDirectory, SubDirectories[I]);
			if (Elements & ZE_POE_DIRECTORY)
			{
				if (!Function(SubDirectoryPath, ZE_POE_DIRECTORY))
					return false;
			}

			if (!Operate(SubDirectoryPath, Function, ZE_POE_DIRECTORY, Recursive))
				return false;
		}
	}

	if (Elements & ZE_POE_FILE)
	{
		ZEArray<ZEString> Files = TargetDirectoryInfo.GetFiles();
		for (ZESize I = 0; I < Files.GetSize(); I++)
		{
			ZEString FilePath = ZEFormat::Format("{0}/{1}", TargetDirectory, Files[I]);
			if (!Function(FilePath, ZE_POE_FILE))
				return false;
		}
	}

	return true;
}

ZEPathInfo::ZEPathInfo()
{

}

ZEPathInfo::ZEPathInfo(const char* Path)
{
	SetPath(Path);
}

ZEPathInfo::ZEPathInfo(const char* ParentPath, const char* Path)
{
	SetRelativePath(ParentPath, Path);
}


static const char* TokenizePath(char* Output, const char* Path)
{
	if (Path == NULL || Path == '\0')
		return false;

	char* OutputStart = Output;
	*Output = '\0';

	bool TrimLeft = true;
	while(*Path != '\0')
	{
		if (TrimLeft && (*Path == ' ' || *Path == '\t'))
		{
			Path++;
			continue;
		}

		TrimLeft = false;

		if (*Path == '\\' || *Path == '/')
		{
			Path++;
			break;
		}
		else if (*Path == '<' || *Path == '>' ||
			*Path == '*' ||	*Path == '"' ||
			*Path == '|' ||	*Path == '?')
		{
			return false;
		}

		*Output = *Path;
		Output++;
		Path++;
	}

	*Output = '\0';

	if (Output - OutputStart != 0)
	{
		for (char* Iter = Output - 1; Iter != OutputStart; Iter--)
		{
			if (*Iter != ' ' && *Iter != '\t')
				break;

			Iter = '\0';
		}
	}

	return Path;
}

ZEArray<ZEString> ZEPathInfo::Divide(const char* Path)
{
	ZEArray<ZEString> Output;

	const char* Iterator = Path;
	while (*Iterator != '\0')
	{
		char PathBuffer[256];
		Iterator = TokenizePath(PathBuffer, Iterator);

		if (Iterator == NULL)
			return ZEArray<ZEString>();

		Output.Add(PathBuffer);
	}

	return Output;
}

bool ZEPathInfo::Normalize(ZEArray<ZEString>& PathElements)
{
	for (int I = 0; I < PathElements.GetCount(); I++)
	{
		if (PathElements[I] == "." || PathElements[I].IsEmpty())
		{
			PathElements.Remove(I);
			I--;
		}
		else if (PathElements[I] == '..')
		{
			if (I == 0) // Something Fishy
				return false;

			PathElements.Remove(I - 1);
			I--;
		}
		else if (PathElements[I].IsEmpty() && I != 0)
		{
			PathElements.Remove(I);
			I--;
		}
	}

	return true;
}

ZEString ZEPathInfo::Construct(const ZEArray<ZEString>& PathElements)
{
	ZEString Output;
	for (ZESize I = 0; I < PathElements.GetSize(); I++)
	{
		Output += PathElements[I];
		if (I != PathElements.GetSize() - 1)
			Output += "/";
	}

	return Output;
}

bool ZEPathInfo::CheckParent(const ZEArray<ZEString>& ParentPathElements, const ZEArray<ZEString>& ChildrenPathElements)
{
	if (ParentPathElements.GetSize() >= ChildrenPathElements.GetSize())
		return false;

	for (ZESize I = 0; I < ParentPathElements.GetSize(); I++)
	{
		if (ParentPathElements[I] != ChildrenPathElements[I])
			return false;
	}

	return true;
}

ZEArray<ZEString> ZEPathInfo::RelativeTo(const ZEArray<ZEString>& ParentPathElements, const ZEArray<ZEString>& ChildrenPathElements)
{
	if (ParentPathElements.GetSize() <= ChildrenPathElements.GetSize())
		return ZEArray<ZEString>();

	ZEArray<ZEString> Output;
	for (ZESize I = 0; I < ParentPathElements.GetSize(); I++)
	{
		if (ParentPathElements[I] != ChildrenPathElements[I])	
		{
			for (ZESize N = I; N < ChildrenPathElements.GetCount(); N++)
				Output.Add(ChildrenPathElements[N]);
		}
	}

	return Output;
}
