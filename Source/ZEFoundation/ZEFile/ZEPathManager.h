//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPathManager.h
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
#ifndef __ZE_PATH_MANAGER_H__
#define __ZE_PATH_MANAGER_H__


#include "ZETypes.h"
#include "ZEDS/ZEString.h"


#define		ZE_PATH_SYMBOL_RESOURCES		'#'
#define		ZE_PATH_SYMBOL_APP_RESOURCES	'~'
#define		ZE_PATH_SYMBOL_USER_DATA		'$'
#define		ZE_PATH_SYMBOL_SYSTEM_DATA		'&'
#define		ZE_PATH_SYMBOL_SAVED_GAMES		'|'


enum ZEKnownPath
{
    ZE_KP_NONE					= 0,
    ZE_KP_RESOURCES				= 1,
    ZE_KP_APP_RESOURCES			= 2,
    ZE_KP_USER_DATA				= 3,
    ZE_KP_SYSTEM_DATA			= 4,
    ZE_KP_SAVED_GAMES			= 5,
	ZE_KP_WORKING_DIRECTORY		= 6
};

class ZEPathManager
{
    private:
		static bool					Initialized;
		static bool					EnablePathRestriction;

		static ZEString				CompanyName;
        static ZEString				ApplicationName;
		static ZEString				ResourceDirName;

        static ZEString				UserDataPath;
        static ZEString				ResourcesPath;
        static ZEString				SystemDataPath;
        static ZEString				SavedGamesPath;
        static ZEString				WorkingDirectory;
		static ZEString				ApplicationResourcesPath;

									ZEPathManager();
									~ZEPathManager();

        static void					InitializePaths();

    public:
        static bool					GetInitialized();

		static void					SetCompanyName(const ZEString& Name);
		static const ZEString&		GetCompanyName();

        static void					SetApplicationName(const ZEString& Name);
		static const ZEString&		GetApplicationName();

		static void					SetResourceDirName(const ZEString& Name);
		static const ZEString&		GetResourceDirName();

        static void					SetEnablePathRestriction(bool Enable);
        static bool					GetEnablePathRestriction();

        static const ZEString&		GetUserDataPath();
        static const ZEString&		GetResourcesPath();
        static const ZEString&		GetSystemDataPath();
        static const ZEString&		GetSavedGamesPath();
        static const ZEString&		GetWorkingDirectory();
		static const ZEString&		GetApplicationResourcesPath();
        
		static const ZEString&		GetKnownPath(ZEKnownPath KnownPath);
		static ZEKnownPath			GetKnownPath(ZEString* RelativePart, const ZEString& SymbolicPath);

		static bool					IsAbsolutePath(const ZEString& Path);
		static bool					IsRelativePath(const ZEString& Path);

		static ZEString				GetRelativePath(const ZEString& FullPath);
		static ZEString				GetAbsolutePath(const ZEString& RelativePath);

		static ZEString				SimplifyPath(const ZEString& Path);
		static ZEString				GetFinalPath(const ZEString& Path, ZEKnownPath* Root = NULL);



		static const ZEString&		GetPathSeperator();
        static ZEString				PathFormatCheck(const ZEString& Path);
        static bool					PathBoundaryCheck(const ZEString& RootPath, const ZEString& Path);

};


#endif
