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
#include "ZEPathTokenizer.h"

bool ZEPathInfo::Normalize(ZEPathTokenizer& Tokenizer)
{
	for (int I = 0; I < Tokenizer.GetTokenCount(); I++)
	{
		if (Tokenizer.GetToken(I) == "." || Tokenizer.GetToken(I) == '\0')
		{
			Tokenizer.SetToken(I, NULL);
		}
		else if (strcmp(Tokenizer.GetToken(I), "..") == 0)
		{
			Tokenizer.SetToken(I, NULL);

			// Find Parent Path
			bool Found = false;
			for (ZESSize N = I - 1; N >= 0; N--)
			{
				if (Tokenizer.GetToken(N) != NULL)
				{
					Tokenizer.SetToken(N, NULL);
					Found = true;
					break;
				}
			}

			if (!Found) // Something Fishy
				return false;
		}
	}

	Tokenizer.Squish();

	return true;
}

void ZEPathInfo::SetPath(const char* Path)
{
	this->Path = Path;
}

void ZEPathInfo::SetRelativePath(const char* ParentPath, const char* RelativePath)
{
	if (RelativePath == NULL || RelativePath[0] == '\0')
	{
		Path = "";
		return;
	}

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

const ZEString&	ZEPathInfo::GetPath() const
{
	return Path;
}

ZEString ZEPathInfo::GetFileName() const
{
	ZESSize Length = Path.GetLength();
	for (ZESSize I = Length - 1; I >= 0; I--)
	{
		if (Path[I] == '\\' || Path[I] == '/')
			return Path.Right(Length - 1 - I).Trim();
	}

	return Path;
}

ZEString ZEPathInfo::GetName() const
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
	for (ZESSize I = Start;  I < (ZESSize)Length; I++)
	{
		if(Path[I] == '.')
			ExtensionStart = I;
	}

	Length = ExtensionStart - Start;
	return Path.Middle(Start, Length).Trim();
}

ZEString ZEPathInfo::GetExtension() const
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

ZEString ZEPathInfo::GetParentDirectory() const
{
	ZESSize Length = Path.GetLength();

	for (ZESSize I = Length - 1; I >= 0; I--)
	{
		if (Path[I] == '\\' || Path[I] == '/')
			return Path.Left(I).Trim();
	}

	return "";
}

ZERealPath ZEPathInfo::GetRealPath() const
{
	return ZEPathManager::GetInstance()->TranslateToRealPath(Path);
}

ZEPathRoot ZEPathInfo::GetRoot() const
{
	return ZEPathManager::GetInstance()->TranslateToRealPath(Path).Root;
}

ZEPathAccess ZEPathInfo::GetAccess() const
{
	ZERealPath RealPath = GetRealPath();
	return RealPath.Access;
}

bool ZEPathInfo::IsInsidePackage() const
{
	return false;
}

ZEString ZEPathInfo::GetRelativeTo(const char* ParentPath) const
{
	ZEPathTokenizer PathTokens;
	PathTokens.Tokenize(Path);
	if (!Normalize(PathTokens))
		return "";

	ZEPathTokenizer ParentPathTokens;
	PathTokens.Tokenize(ParentPath);
	if (!Normalize(ParentPathTokens))
		return "";

	ZESize Index = 0;
	for (ZESize I = 0; I < ParentPathTokens.GetTokenCount(); I++)
	{
		const char* ParentToken = ParentPathTokens.GetToken(I);
		if (ParentToken == NULL)
			continue;
	}

	PathTokens.Combine();

	return PathTokens.GetOutput();
}

#ifdef ZE_PLATFORM_WINDOWS

#include <Windows.h>
#include <shlwapi.h>

bool ZEPathInfo::IsExists() const
{
	if ((GetAccess() & ZE_PA_READ) == 0)
		return false;

	DWORD PathAttribute = GetFileAttributesA(GetRealPath().Path);
	if (PathAttribute == INVALID_FILE_ATTRIBUTES)
		return false; 
	else
		return true;
}

bool ZEPathInfo::IsFile() const
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

bool ZEPathInfo::IsDirectory() const
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

ZEFileTime ZEPathInfo::GetCreationDate() const
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

ZEFileTime ZEPathInfo::GetModificationTime() const
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
	ZEPathTokenizer Tokenizer;
	Tokenizer.Tokenize(Path);

	if (!Normalize(Tokenizer))
		return "";

	Tokenizer.Combine();
	Path = Tokenizer.GetOutput();

	return Path;
}

bool ZEPathInfo::Equals(const ZEPathInfo& OtherPath) const
{
	ZERealPath OtherRealPath = OtherPath.GetRealPath();
	ZERealPath RealPath = GetRealPath();
	
	if ((RealPath.Access & ZE_PA_READ) == 0 ||
		(OtherRealPath.Access & ZE_PA_READ) == 0)
	{
		return false;
	}
	
	return (RealPath.Path == OtherRealPath.Path);
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
