//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMain.cpp
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

#include "ZECore/ZECore.h"
#include "ZECore/ZEConsoleWindow.h"
#include "ZECore/ZEOptionManager.h"
#include "ZEProtect/ZELCLicenseManager.h"
#include "ZEThread/ZETask.h"
#include "ZEThread/ZETaskPool.h"
#include "ZEThread/ZETaskManager.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

ZEInt WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, ZEInt nCmdShow)
{
	ZETaskPool* RealTimePool = new ZETaskPool();
	RealTimePool->SetId(ZE_TPI_REAL_TIME);
	RealTimePool->SetName("RealTime Pool");
	RealTimePool->SetSchedulingPolicy(ZE_TSP_ALWAYS_FIRST);
	RealTimePool->SetReservedThreadCount(1);
	ZETaskManager::GetInstance()->RegisterPool(RealTimePool);

	ZETaskPool* ConcurrentPool = new ZETaskPool();
	ConcurrentPool->SetId(ZE_TPI_CONCURENT);
	ConcurrentPool->SetName("Concurrent Pool");
	ZETaskManager::GetInstance()->RegisterPool(ConcurrentPool);
	ZETaskManager::GetInstance()->SetThreadCount(3);

	zeCore->GetOptions()->Load("#E:\\Options.ini");
	ZEConsoleWindow ConsoleWindow;
	zeCore->GetConsole()->SetConsoleInterface(&ConsoleWindow);
	zeCore->GetConsole()->ShowConsole();

	ZELCLicenseManager Manager;
	Manager.LoadLicenses();
	const ZELCLicense* License = Manager.RequestLicense("ZETrainIG", 0);

	zeCore->Execute();
	
	return EXIT_SUCCESS;
}
