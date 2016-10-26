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
#include "ZEPlatform.h"

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

bool ZEPathInfo::IsParent(const char* ParentPath) const
{
	ZEPathTokenizer PathTokens;
	PathTokens.Tokenize(GetRealPath().Path);
	if (!Normalize(PathTokens))
		return false;

	ZEPathTokenizer ParentPathTokens;
	ZEPathInfo ParentPathInfo(ParentPath);
	ParentPathTokens.Tokenize(ParentPathInfo.GetRealPath().Path);
	if (!Normalize(ParentPathTokens))
		return false;
	
	if (PathTokens.GetTokenCount() < ParentPathTokens.GetTokenCount())
		return false;

	for (ZESize I = 0; I < ParentPathTokens.GetTokenCount(); I++)
	{
		const char* ParentToken = ParentPathTokens.GetToken(I);
		const char* PathToken = PathTokens.GetToken(I);

		if (strcmp(ParentToken, PathToken) != 0)
			return false;
	}

	return true;
}

ZEString ZEPathInfo::GetRelativeTo(const char* ParentPath) const
{
	ZEPathTokenizer PathTokens;
	PathTokens.Tokenize(Path);
	if (!Normalize(PathTokens))
		return "";

	ZEPathTokenizer ParentPathTokens;
	ParentPathTokens.Tokenize(ParentPath);
	if (!Normalize(ParentPathTokens))
		return "";

	for (ZESize I = 0; I < ParentPathTokens.GetTokenCount(); I++)
	{
		const char* ParentToken = ParentPathTokens.GetToken(I);
		const char* PathToken = PathTokens.GetToken(I);
		if (strcmp(ParentToken, PathToken) == 0)
		{
			for (ZESize N = 0; N <= I; N++)
				PathTokens.SetToken(N, NULL);
		}
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

ZETimeStamp ZEPathInfo::GetCreationDate() const
{
	if ((GetAccess() & ZE_PA_READ) == 0)
		return ZETimeStamp();

	HANDLE Handle = CreateFile(GetRealPath().Path, GENERIC_WRITE, FILE_SHARE_WRITE,	NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (Handle == INVALID_HANDLE_VALUE)
		return ZETimeStamp();

	FILETIME CreationTime, Temp0, Temp1;
	if (!GetFileTime(Handle, &CreationTime, &Temp0, &Temp1))
	{
		CloseHandle(Handle);
		return ZETimeStamp();
	}

	CloseHandle(Handle);

	SYSTEMTIME Time;
	FileTimeToSystemTime(&CreationTime, &Time);

	return ZETimeStamp(Time.wYear, Time.wMonth, Time.wDay, Time.wHour, Time.wMinute, Time.wSecond, Time.wMilliseconds);
}

ZETimeStamp ZEPathInfo::GetModificationTime() const
{
	if ((GetAccess() & ZE_PA_READ) == 0)
		return ZETimeStamp();

	HANDLE Handle = CreateFile(GetRealPath().Path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
	if (Handle == INVALID_HANDLE_VALUE)
		return ZETimeStamp();

	FILETIME ModificationTime, Temp0, Temp1;
	if (!GetFileTime(Handle, &Temp0, &Temp1, &ModificationTime))
	{
		CloseHandle(Handle);
		return ZETimeStamp();
	}

	CloseHandle(Handle);

	SYSTEMTIME Time;
	FileTimeToSystemTime(&ModificationTime, &Time);

	return ZETimeStamp(Time.wYear, Time.wMonth, Time.wDay, Time.wHour, Time.wMinute, Time.wSecond, Time.wMilliseconds);
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
	
	return (RealPath.Path.EqualsIncase(OtherRealPath.Path));
}

bool ZEPathInfo::Operate(const char* TargetDirectory, ZEPathOperationFunction Function, ZEPathOperationElement Elements, bool Recursive, void* Parameters)
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
				if (!Function(SubDirectoryPath, ZE_POE_DIRECTORY, Parameters))
					return false;
			}

			if (!Operate(SubDirectoryPath, Function, Elements, Recursive, Parameters))
				return false;
		}
	}

	if (Elements & ZE_POE_FILE)
	{
		ZEArray<ZEString> Files = TargetDirectoryInfo.GetFiles();
		for (ZESize I = 0; I < Files.GetSize(); I++)
		{
			ZEString FilePath = ZEFormat::Format("{0}/{1}", TargetDirectory, Files[I]);
			if (!Function(FilePath, ZE_POE_FILE, Parameters))
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

bool ZEPathInfo::Compare(const ZEString& PathA, const ZEString& PathB)
{
	ZEString PathANormalized = ZEPathInfo(PathA).Normalize();
	ZEString PathBNormalized = ZEPathInfo(PathB).Normalize();

	return PathANormalized.EqualsIncase(PathBNormalized);
}

ZEString ZEPathInfo::CombineRelativePath(const char* ParentFilePath, const char* Path)
{
	if (ParentFilePath == NULL || ParentFilePath[0] == '\0' ||
		Path == NULL || Path[0] == '\0' )
	{
		return ZEString::Empty;
	}

	if (ZEPathInfo(Path).GetRoot() == ZE_PR_UNKOWN)
	{
		// No root in path. Relative.
		ZEPathInfo ParentPathInfo(ParentFilePath);
		ZEString ParentDirectory = ParentPathInfo.GetParentDirectory();
		return ZEFormat::Format("{0}/{1}", ParentDirectory, Path);
	}
	else
	{
		// Root available in path. Absolute path.
		return Path;
	}
}
