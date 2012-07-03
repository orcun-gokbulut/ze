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


#define		ZE_FILE_PATH_IDENTIFIER_SYMBOL_RESOURCES		'#'
#define		ZE_FILE_PATH_IDENTIFIER_SYMBOL_APP_RESOURCES	'~'
#define		ZE_FILE_PATH_IDENTIFIER_SYMBOL_USER_DATA		'$'
#define		ZE_FILE_PATH_IDENTIFIER_SYMBOL_SYSTEM_DATA		'&'
#define		ZE_FILE_PATH_IDENTIFIER_SYMBOL_SAVED_GAMES		'|'

// ZE_KFP_RESOURCES: Resources(RunDir\Resources)
// ZE_KFP_APP_RESOURCES: Application specific resources(RunDir\Resources\AppName)
// ZE_FKP_USER_DATA: User specific local data
// ZE_FKP_SYSTEM_DATA: User independent common system data
// ZE_FKP_SAVED_GAMES: User game saves
enum ZEFileKnownPaths
{
    ZE_FKP_NONE				= 0,
    ZE_FKP_RESOURCES		= 1,
    ZE_FKP_APP_RESOURCES	= 2,
    ZE_FKP_USER_DATA		= 3,
    ZE_FKP_SYSTEM_DATA		= 4,
    ZE_FKP_SAVED_GAMES		= 5
};

class ZEPathManager
{
    private:
        static ZEString				AppName;
        static bool					Initialized;

        static bool					EnablePathRestriction;

        static ZEString				UserDataPath;
        static ZEString				ResourcesPath;
        static ZEString				SystemDataPath;
        static ZEString				SavedGamesPath;
        static ZEString				ApplicationResourcesPath;

									ZEPathManager();
									~ZEPathManager();

        static void					InitializePaths();

    public:
        static bool					GetInitialized();
        static void					SetApplicationName(ZEString& Name);

        static void					SetEnablePathRestriction(bool Enable);
        static bool					GetEnablePathRestriction();

        static const ZEString&		GetUserDataPath();
        static const ZEString&		GetResourcesPath();
        static const ZEString&		GetSystemDataPath();
        static const ZEString&		GetSavedGamesPath();
        static const ZEString&		GetApplicationResourcesPath();
        static ZEString				GetKnownPath(ZEFileKnownPaths KnownPath);

        static ZEString				GetFinalPath(const ZEString& Path, ZEFileKnownPaths& Root);

        static ZEString				PathFormatCheck(const ZEString& Path);
        static bool					PathBoundaryCheck(const ZEString& RootPath, const ZEString& Path);

};


#endif
