//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPathUtils.cpp
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


#include "ZEPathUtils.h"
#include "ZEDS/ZEArray.h"

#include <string.h>

static const ZEString Dot = ".";
static const ZEString DotDot = "..";
static const ZEString EmptyPath = "";

#ifdef ZE_PLATFORM_WINDOWS
	static const ZEString Seperator = "\\";
#endif
#ifdef ZE_PLATFORM_UNIX
	static const ZEString Seperator = "/";
#endif

class ZEPathToken
{
	public:

		char*		Token;
		ZEUInt16	Lenth;
		bool		Valid;

		bool	IsDot() { return (Lenth == 1) && (strncmp(Token, ".", Lenth) == 0); }
		bool	IsDotDot() { return (Lenth == 2) && (strncmp(Token, "..", Lenth) == 0); }

};

ZEPathUtils::ZEPathUtils()
{

}

ZEPathUtils::~ZEPathUtils()
{

}

const ZEString& ZEPathUtils::GetDot()
{
	return Dot;
}

const ZEString& ZEPathUtils::GetDotDot()
{
	return DotDot;
}

const ZEString& ZEPathUtils::GetSeperator()
{
	return Seperator;
}

const ZEString& ZEPathUtils::GetEmptyPath()
{
	return EmptyPath;
}

// If it is not absolute it must be relative
bool ZEPathUtils::IsRelativePath(const ZEString& Path)
{
	return IsAbsolutePath(Path) ? false : true;
}

// Extracts root from absolute path and returns the relative path.
ZEString ZEPathUtils::GetRelativePath(const ZEString& RootPath, const ZEString& AbsolutePath)
{
	ZESize PathLength = AbsolutePath.GetLength();
	ZESize RootLength = RootPath.GetLength();

	if (PathLength >= RootLength)
	{
		if (strncmp(RootPath, AbsolutePath, RootLength) == 0)
			return AbsolutePath.Right(PathLength - RootLength);
	}

	return EmptyPath;
}

// Checks if Path is a full path
bool ZEPathUtils::IsAbsolutePath(const ZEString& Path)
{
    if (Path.IsEmpty())
        return false;

	// If first char is the root character directly accept it
	if (ISSEPERATOR(Path[0]))
		return true;

#ifdef ZE_PLATFORM_WINDOWS
	// If drive letter is found in windows also accept is as root
	char* Token;
	char* Context;
	ZEString Temp = Path;
	const char* Search = "\\/";
	char* Source = (char*)Temp.ToCString();

	Token = strtok_s(Source, Search, &Context);
	if (Token == NULL || strlen(Token) != 2)
		return false;

	if (ISDRIVELETTER(Token[0]) && Token[1] == ':')
			return true;
#endif

	return false;
}

// Concatenates relative path to root path
ZEString ZEPathUtils::GetAbsolutePath(const ZEString& RootPath, const ZEString& RelativePath)
{
	return RootPath + RelativePath;
}

// Finds the relative path symbol in symbolic path and return it as ZEKnownPath
// If Relative path argument is not NULL, then RelativePart argument returns the remaining path starting from found symbol
// If no symbol is found RelativePart is set to SymbolicPath
ZEKnownPath ZEPathUtils::SearchForSymbol(ZEString* RelativePart, const ZEString& SymbolicPath)
{
	char* Token;
	char* Context;
	const char* Search = "\\/";
	ZEString Temp = SymbolicPath;
	char* Source = (char*)Temp.ToCString();

	bool SymbolFound = false;
	ZEKnownPath Root = ZE_KP_NONE;

    if (SymbolicPath.IsEmpty())
        return Root;

	// Search for symbol
	Token = strtok_r(Source, Search, &Context);
	while (Token != NULL && !SymbolFound)
	{
		// Check token for known symbols
		if (strlen(Token) == 1)
		{
			switch (*Token)
			{
				case ZE_PATH_SYMBOL_USER_DATA:
					Root = ZE_KP_USER_DATA;
					SymbolFound = true;
					break;
				case ZE_PATH_SYMBOL_SYSTEM_DATA:
					Root = ZE_KP_SYSTEM_DATA;
					SymbolFound = true;
					break;
				case ZE_PATH_SYMBOL_SAVED_GAMES:
					Root = ZE_KP_SAVED_GAMES;
					SymbolFound = true;
					break;
				case ZE_PATH_SYMBOL_APP_RESOURCES:
					Root = ZE_KP_APP_RESOURCES;
					SymbolFound = true;
					break;
				case ZE_PATH_SYMBOL_RESOURCES:
					Root = ZE_KP_RESOURCES;
					SymbolFound = true;
					break;
			}
		}

		Token = strtok_r(NULL, Search, &Context);
	}

	// Shortcut
	if (RelativePart == NULL)
		return Root;

	if (SymbolFound)
	{
		// Get the rest of the string into RelativePart
		while (Token != NULL)
		{
			// Add to output
			*RelativePart += Seperator;
			*RelativePart += Token;

			Token = strtok_r(NULL, Search, &Context);
		}
	}
	else
	{
		// Copy all the path to output
		*RelativePart = SymbolicPath;
	}

	return Root;
}

// Simplifies the path by operating on "." and ".." directories in the path
// This function does not care about EnablePathRestriction
ZEString ZEPathUtils::GetSimplifiedPath(const ZEString& AbsolutePath)
{

	ZEString Output;
	char* Context = NULL;
	char* NextToken = NULL;
	char* CurrentToken = NULL;
	ZEString TempSource = AbsolutePath;
	const char* Search = "\\/";
	char* Source = (char*)TempSource.ToCString();

    if (AbsolutePath.IsEmpty())
    {
        Output = Dot;
        return Output;
    }

	ZESmartArray<ZEPathToken> Tokens;

	// Find all tokens and put them in an array
	CurrentToken = strtok_r(Source, Search, &Context);
	while (CurrentToken != NULL)
	{
		ZEPathToken Token;
		Token.Token = CurrentToken;
		Token.Lenth = (ZEUInt16)strlen(CurrentToken);
		Tokens.Add(Token);

		CurrentToken = strtok_r(NULL, Search, &Context);
	}

	// Process tokens
	ZESize Index = 0;
	ZESize TokenCount = Tokens.GetCount();
	ZEUInt UsefulTokens = 0;
	while (Index < TokenCount)
	{
		if (Tokens[Index].IsDot())
		{
			// Omit
			Tokens[Index].Valid = false;
		}
		else if (Tokens[Index].IsDotDot())
		{
		    // Discard this one
            Tokens[Index].Valid = false;

			// If there are previous tokens to remove
			// Do not let ".." stack at the beginning of path
			if (UsefulTokens > 0)
			{
				// Find previous one
				ZESSize BackwardIndex = Index - 1;
				while (BackwardIndex >= 0)
				{
					if (Tokens[BackwardIndex].Valid)
					{
						// Discard previous one
						Tokens[BackwardIndex].Valid = false;
						break;
					}

					--BackwardIndex;
				}

				--UsefulTokens;
			}
		}
		else
		{
		    // Let useful token stay
			Tokens[Index].Valid = true;
			++UsefulTokens;
		}

		++Index;
	}

    if (ISSEPERATOR(AbsolutePath[0]))
        Output = Seperator;

	// Create final path from remaining valid tokens
	if (UsefulTokens > 0)
	{
		Index = 0;
		ZEUInt AddCount = 0;
		while (Index < TokenCount)
		{
            // If last useful token
			if (Tokens[Index].Valid)
			{
				// If last useful token leave it
				if (AddCount == (UsefulTokens - 1))
					break;

				Output += Tokens[Index].Token;
				Output += Seperator;

				++AddCount;
			}

			++Index;
		}

        // Add last token without seperator
		Output += Tokens[Index].Token;
	}

    // If still nothing found, return dot
	if(Output.IsEmpty())
		Output = Dot;

	return Output;
}

// Checks the given path's depth
// AbsolutePath must be simplified
// Should be used when path restriction is on
// When path restriction is on, all the paths must be absolute
bool ZEPathUtils::CheckForRestriction(const ZEString& RootPath, const ZEString& AbsolutePath)
{
	ZEString Simplified;
	ZEString Temp = AbsolutePath;
	bool AbsoluteWithSlash = false;
	bool AbsoluteWithDrive = false;

	ZESize RootLenth = RootPath.GetLength();
	ZESize PathLength = AbsolutePath.GetLength();

	//Check if path is an extension of root
	if (PathLength >= RootLenth)
		if (strncmp(RootPath, AbsolutePath, RootLenth) == 0)
			return true;

	return false;
}
