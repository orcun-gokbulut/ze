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
	friend class ZEFileInfo;
	friend class ZEDirectoryInfo;

	private:
		static bool					EnablePathRestriction;

		static ZEString				WorkingDrive;
		static ZEString				UserDataPath;
		static ZEString				ResourcesPath;
		static ZEString				SystemDataPath;
		static ZEString				SavedGamesPath;
		static ZEString				WorkingDirectory;
		static ZEString				AppResourcesPath;

									ZEPathManager();
									~ZEPathManager();

	public:
		static const ZEString&		GetWorkingDrive();
		static const ZEString&		GetWorkingDirectory();

		static void					SetEnablePathRestriction(bool Enable);
		static bool					GetEnablePathRestriction();
		
		static void					SetUserDataPath(const ZEString& Name);
		static const ZEString&		GetUserDataPath();
		
		static void					SetResourcesPath(const ZEString& Name);
		static const ZEString&		GetResourcesPath();
		
		static void					SetSystemDataPath(const ZEString& Name);
		static const ZEString&		GetSystemDataPath();
		
		static void					SetSavedGamesPath(const ZEString& Name);
		static const ZEString&		GetSavedGamesPath();
		
		static void					SetApplicationResourcesPath(const ZEString& Name);
		static const ZEString&		GetApplicationResourcesPath();

		static ZEKnownPath			GetKnownPath(const ZEString& AbsolutePath);
		static const ZEString&		GetKnownPath(const ZEKnownPath KnownPath);
		
		static ZEString				GetFinalPath(const ZEString& Path, ZEKnownPath* Root = NULL);

		static void					CustomizePaths(	const ZEString* CompanyName = NULL,
													const ZEString* ApplicationName = NULL,
													const ZEString* ResourceDirectoryName = NULL);
		
};


#endif
