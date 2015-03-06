//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZE3dsMaxPlugin.cpp
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

#include "ZE3dsMaxPlugin.h"
#include "ZE3dsMaxModelExporter/ZE3dsMaxModelExporterDescription.h"
#include "ZE3dsMaxInteriorExporter/ZE3dsMaxInteriorExporterDescription.h"
#include "ZE3dsMaxUI/ZE3dsMaxUIDescription.h"
#include "ZETypes.h"
#include "ZEFile/ZEFile.h"
#include "ZEFile/ZEDirectoryInfo.h"
#include "ZEToolbarIcons_16a.bmp.h"
#include "ZEToolbarIcons_16i.bmp.h"
#include "ZEToolbarIcons_24a.bmp.h"
#include "ZEToolbarIcons_24i.bmp.h"
#include "zineklogo.bmp.h"
#include <IPathConfigMgr.h> 

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "ZEFile/ZEFileInfo.h"

HINSTANCE hInstance;
ZEInt controlsInit = FALSE;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, ULONG fdwReason, LPVOID lpvReserved)
{
	if( fdwReason == DLL_PROCESS_ATTACH )
	{
		hInstance = hinstDLL;
		DisableThreadLibraryCalls(hInstance);
	}

	return TRUE;
}

__declspec( dllexport ) const TCHAR* LibDescription()
{
	return "Zinek Engine 3ds Max Plugin";
}

__declspec( dllexport ) ZEInt LibNumberClasses()
{
	return 3;
}

__declspec( dllexport ) ClassDesc* LibClassDesc(ZEInt i)
{
	switch(i) 
	{
		case 0: 
			return ZE3dsMaxModelExporterDescription::GetInstance();
		case 1:
			return ZE3dsMaxInteriorExporterDescription::GetInstance();
		case 2:
			return ZE3dsMaxUIDescription::GetInstance();
		default: 
			return 0;
	}
}

__declspec( dllexport ) ULONG LibVersion()
{
	return VERSION_3DSMAX;
}

__declspec( dllexport ) ZEInt LibInitialize(void)
{	
	ZEDirectoryInfo IconsDirectory = ZEDirectoryInfo(IPathConfigMgr::GetPathConfigMgr()->GetDir(APP_USER_ICONS_DIR));

	ZEToolbarIcons_16a_bmp IconImage16a;
	ZEToolbarIcons_16i_bmp IconImage16i;
	ZEToolbarIcons_24a_bmp IconImage24a;
	ZEToolbarIcons_24i_bmp IconImage24i;
	zineklogo_bmp		   ZinekLogo;

	ZEFile IconFile;

	if (!ZEFileInfo(IconsDirectory.GetPath() + "/ZEToolbarIcons_16a.bmp").IsFile())
	{
		IconFile.Open(IconsDirectory.GetPath() + "/ZEToolbarIcons_16a.bmp" , ZE_FOM_WRITE, ZE_FCM_CREATE);
		IconFile.Write(IconImage16a.GetData(), IconImage16a.GetSize(), 1);
		IconFile.Close();
	}

	if (!ZEFileInfo(IconsDirectory.GetPath() + "/ZEToolbarIcons_16i.bmp").IsFile())
	{
		IconFile.Open(IconsDirectory.GetPath() + "/ZEToolbarIcons_16i.bmp" , ZE_FOM_WRITE, ZE_FCM_CREATE);
		IconFile.Write(IconImage16i.GetData(), IconImage16i.GetSize(), 1);
		IconFile.Close();
	}

	if (!ZEFileInfo(IconsDirectory.GetPath() + "/ZEToolbarIcons_24a.bmp").IsFile())
	{
		IconFile.Open(IconsDirectory.GetPath() + "/ZEToolbarIcons_24a.bmp" , ZE_FOM_WRITE, ZE_FCM_CREATE);
		IconFile.Write(IconImage24a.GetData(), IconImage24a.GetSize(), 1);
		IconFile.Close();
	}

	if (!ZEFileInfo(IconsDirectory.GetPath() + "/ZEToolbarIcons_24i.bmp").IsFile())
	{
		IconFile.Open(IconsDirectory.GetPath() + "/ZEToolbarIcons_24i.bmp" , ZE_FOM_WRITE, ZE_FCM_CREATE);
		IconFile.Write(IconImage24i.GetData(), IconImage24i.GetSize(), 1);
		IconFile.Close();
	}

	if (!ZEFileInfo(IconsDirectory.GetPath() + "/zineklogo.bmp").IsFile())
	{
		IconFile.Open(IconsDirectory.GetPath() + "/zineklogo.bmp" , ZE_FOM_WRITE, ZE_FCM_CREATE);
		IconFile.Write(ZinekLogo.GetData(), ZinekLogo.GetSize(), 1);
		IconFile.Close();
	}

	return TRUE;
}

__declspec( dllexport ) ZEInt LibShutdown(void)
{
	return TRUE;
}
