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
#include "ZEFileUtils.h"

static const ZEString Dot = ".";
static const ZEString DotDot = "..";
static const ZEString EmptyPath = "";

#if defined ZE_PLATFORM_WINDOWS

	#define	Tokenize	wcstok_s

	static const ZEString Seperator = "\\";

#elif defied ZE_PLATFORM_UNIX

	#define	Tokenize	wcstok

	static const ZEString Seperator = "/";

#endif

#define		ZE_PATH_SYMBOL_RESOURCES		L'#'
#define		ZE_PATH_SYMBOL_APP_RESOURCES	L'~'
#define		ZE_PATH_SYMBOL_USER_DATA		L'$'
#define		ZE_PATH_SYMBOL_SYSTEM_DATA		L'&'
#define		ZE_PATH_SYMBOL_SAVED_GAMES		L'|'

class ZEPathToken
{
	public:

		wchar_t*	Token;
		ZEUInt16	Lenth;
		bool		Valid;

		bool		IsDot() { return (Lenth == 1) && (wcsncmp(Token, L".", 1) == 0); }
		bool		IsDotDot() { return (Lenth == 2) && (wcsncmp(Token, L"..", 2) == 0); }

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
ZEString ZEPathUtils::GetRelativePath(ZEString& RelativeTo, ZEString& Path)
{	
	ZEString PathCopy = Path;
	ZEString RelativeToCopy = RelativeTo;
	
	wchar_t* Token0 = NULL;
	wchar_t* Context0 = NULL;
	wchar_t* Source0 = (wchar_t*)RelativeToCopy.ToWCString();

	wchar_t* Token1 = NULL;
	wchar_t* PrevToken1 = NULL;
	wchar_t* Context1 = NULL;
	wchar_t* Source1 = (wchar_t*)PathCopy.ToWCString();
	
	if (Source0 == NULL)
		return Path;
	if (Source1 == NULL)
		return "";

	const wchar_t* Search = L"\\/";
	
	ZEPathToken TempToken;
	ZESize CommonTokenCount = 0;
	ZEArray<ZEPathToken> Tokens0;
	ZEArray<ZEPathToken> Tokens1;

	Token0 = Tokenize(Source0, Search, &Context0);
	Token1 = Tokenize(Source1, Search, &Context1);
	while (Token0 != NULL && Token1 != NULL)
	{
		// If token is different
		if (wcscmp(Token0, Token1) != 0)
			break;

		TempToken.Token = Token0;
		Tokens0.Add(TempToken);

		TempToken.Token = Token1;
		Tokens1.Add(TempToken);

		Token0 = Tokenize(NULL, Search, &Context0);
		Token1 = Tokenize(NULL, Search, &Context1);

		CommonTokenCount++;
	}

	if (CommonTokenCount == 0)
		return Path;

	ZESize Token0Components = 0;
	while (Token0 != NULL)
	{
		TempToken.Token = Token0;
		Tokens0.Add(TempToken);

		Token0Components++;

		// check next token
		Token0 = Tokenize(NULL, Search, &Context0);
	}

	ZESize Token1Components = 0;
	while(Token1 != NULL)
	{
		TempToken.Token = Token1;
		Tokens1.Add(TempToken);

		Token1Components++;

		// check next token
		Token1 = Tokenize(NULL, Search, &Context1);
	}

	//Start creating relative path
	ZEString RelativePath = "";

	ZESize Token0Count = Tokens0.GetCount();
	for (ZESize I = Token0Count-1; I > CommonTokenCount; --I)
	{
		RelativePath += GetDotDot();
		RelativePath += GetSeperator();
	}
	
	for (ZESize I = CommonTokenCount; I < Tokens1.GetCount(); ++I)
	{
		RelativePath += ZEString::FromWCString(Tokens1[I].Token);
		RelativePath += GetSeperator();
	}
	
	return RelativePath.Left(RelativePath.GetLength() - 1);
}

// Checks if Path is a full path
bool ZEPathUtils::IsAbsolutePath(const ZEString& Path)
{
    if (Path.IsEmpty())
        return false;

	// If first char is the root character(separator) directly accept it
	if (Path[0] == '\\' || Path[0] == '/')
		return true;

#ifdef ZE_PLATFORM_WINDOWS
	
	// Create local copy
	ZEString Temp = Path;

	// If drive letter is found in windows also accept is as root
	wchar_t* Token = NULL;
	wchar_t* Context = NULL;
	const wchar_t* Search = L"\\/";
	wchar_t* Source = (wchar_t*)Temp.ToWCString();

	Token = Tokenize(Source, Search, &Context);
	if (Token == NULL || wcslen(Token) != 2)
		return false;

	if ((L'A' < Token[0]) && (Token[0] < L'Z') && (Token[1] == L':'))
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
	if (RelativePart != NULL)
		RelativePart->Clear();

	if (SymbolicPath.IsEmpty())
        return ZE_KP_NONE;

	// Create a local copy
	ZEString Temp = SymbolicPath;
	
	bool SymbolFound = false;
	ZEKnownPath Root = ZE_KP_NONE;

	wchar_t* Token = NULL;
	wchar_t* Context = NULL;
	wchar_t* Source = (wchar_t*)Temp.ToWCString();
	const wchar_t* Search = L"\\/";

	// Search for symbol
	Token = Tokenize(Source, Search, &Context);
	while (Token != NULL && !SymbolFound)
	{
		// Check token for known symbols
		if (wcslen(Token) == 1)
		{
			switch (*Token)
			{
				case ZE_PATH_SYMBOL_RESOURCES:
					Root = ZE_KP_RESOURCES;
					SymbolFound = true;
					break;
				case ZE_PATH_SYMBOL_APP_RESOURCES:
					Root = ZE_KP_APP_RESOURCES;
					SymbolFound = true;
					break;
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
			}
		}

		Token = Tokenize(NULL, Search, &Context);
	}

	// Shortcut
	if (RelativePart == NULL)
		return Root;

	if (SymbolFound)
	{
		// Copy the rest of the string to RelativePart
		while (Token != NULL)
		{
			// Add to output
			*RelativePart += Seperator;
			*RelativePart += Token;

			Token = Tokenize(NULL, Search, &Context);
		}
	}
	else // Nothing found
	{
		// Copy all the path to output
		*RelativePart += Seperator;
		*RelativePart += SymbolicPath;
	}

	return Root;
}

// Simplifies the path by removing "." and ".." components from the path
ZEString ZEPathUtils::GetSimplifiedPath(const ZEString& Path, bool StackDotDot)
{
	 if (Path.IsEmpty())
        return "";

	// Create local copy
	ZEString Temp = Path;

	ZESmartArray<ZEPathToken> Tokens;

	ZEString Output;
	wchar_t* Context = NULL;
	wchar_t* NextToken = NULL;
	wchar_t* CurrentToken = NULL;
	const wchar_t* Search = L"\\/";
	wchar_t* Source = (wchar_t*)Temp.ToWCString();
	
	// Tokenize
	CurrentToken = Tokenize(Source, Search, &Context);
	while (CurrentToken != NULL)
	{
		ZEPathToken Token;
		Token.Token = CurrentToken;
		Token.Lenth = (ZEUInt16)wcslen(CurrentToken);
		Tokens.Add(Token);

		CurrentToken = Tokenize(NULL, Search, &Context);
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
			// If there are previous tokens to remove
			if (UsefulTokens > 0)
			{	
				// Discard this one
				Tokens[Index].Valid = false;

				// Find previous one
				ZESSize BackwardIndex = Index - 1;
				while (BackwardIndex >= 0)
				{
					// If valid and not dotdot
					if (Tokens[BackwardIndex].Valid && !Tokens[BackwardIndex].IsDotDot())
					{
						// Discard
						Tokens[BackwardIndex].Valid = false;
						--UsefulTokens;
						break;
					}

					--BackwardIndex;
				}
			}
			else // If no useful tokens
			{
				if (StackDotDot)
				{
					Tokens[Index].Valid = true;
					++UsefulTokens;
				}
				else
				{
					Tokens[Index].Valid = false;
				}
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

        // Add last token without separator
		Output += Tokens[Index].Token;
	}

	return Output;
}

bool ZEPathUtils::CheckPathContainsRoot(const ZEString& RootPath, const ZEString& AbsolutePath)
{
	ZESize RootLength= strlen(RootPath.ToCString());
	ZESize PathLength = strlen(AbsolutePath.ToCString());

	//Check if path is an extension of root
	if (PathLength >= RootLength)
		if (strncmp(RootPath, AbsolutePath, RootLength) == 0)
			return true;

	return false;
}
