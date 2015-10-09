//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELNLauncher.cpp
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

#include "ZELNLauncher.h"
#include "ui_ZELNLauncherWindow.h"

#include "ZELNModule.h"
#include "ZELNLauncherWindow.h"
#include "ZEDS\ZEFormat.h"
#include "ZEFile\ZEPathInfo.h"
#include "ZEVersion.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

ZELNLauncher* ZELNLauncher::Instance = NULL;

bool ZELNLauncher::InitializeSelf()
{
	if (!ZEInitializable::InitializeSelf())
		return false;

	ZELNModule::GetModule("Log")->Initialize();

	zeLog(ZE_LOG_INFO, "Initializing Zinek Launcher...");

	Instance = this;
	Window = new ZELNLauncherWindow();

	zeLog(ZE_LOG_INFO, "Initializing modules...");
	for (ZESize I = 0; I < ZELNModule::GetModuleCount(); I++)
	{
		ZELNModule* Module = ZELNModule::GetModules()[I];
		zeLog(ZE_LOG_INFO, "Initializing \"%s\" module.", Module->GetName());

		Modules.Add(Module);

		if (!Modules[I]->Initialize())
		{
			zeError("Cannot initialize \"%s\" module.", Module->GetName());
			continue;
		}

		QWidget* Widget = Modules[I]->GetWidget();

		if (Widget != NULL)
		{
			int Index = Window->GetForm()->tabWidgets->count();
			Window->GetForm()->tabWidgets->insertTab(Index, Widget, QIcon(), Modules[I]->GetName());
		}
	}

	zeLog(ZE_LOG_INFO, "Modules loaded.");

	Update();

	Window->show();
}

void ZELNLauncher::DeinitializeSelf()
{
	Window->close();

	for (ZESize I = 0; I < Modules.GetCount(); I++)
		Modules[I]->Deinitialize();

	delete Window;
}

const ZEArray<ZELNModule*> ZELNLauncher::GetModules()
{
	return Modules;
}

ZELNLauncherWindow* ZELNLauncher::GetWindow()
{
	return Window;
}

void ZELNLauncher::SetApplicationName(const ZEString& Name)
{
	ApplicationName = Name;
}
const ZEString& ZELNLauncher::GetApplicationName()
{
	return ApplicationName;
}

void ZELNLauncher::SetApplicationFileName(const ZEString& FileName)
{
	ApplicationFileName = FileName;
}

const ZEString& ZELNLauncher::GetApplicationFileName()
{
	return ApplicationName;
}

void ZELNLauncher::SetApplicationMajorVersion(ZEUInt Version)
{
	ApplicationMajorVersion = Version;
}

ZEUInt ZELNLauncher::GetApplicationVersionMajor()
{
	return ApplicationMajorVersion;
}

void ZELNLauncher::SetApplicationMinorVersion(ZEUInt Version)
{
	ApplicationMinorVersion = Version;
}

ZEUInt ZELNLauncher::GetApplicationVersionMinor()
{
	return ApplicationMinorVersion;
}

void ZELNLauncher::Update()
{
	zeLog(ZE_LOG_INFO, "Getting launch ready.");

	AllowedToLaunch = true;
	for (ZESize I = 0; I < Modules.GetCount(); I++)
	{
		Modules[I]->OnUpdate();

		bool ModuleAllowLaunch = Modules[I]->GetAllowLaunch();
		if (!ModuleAllowLaunch)
			zeLog(ZE_LOG_INFO, "Launch is denied by module \"%s\". Please check \"%s\" page for more information.",
				Modules[I]->GetName(),
				Modules[I]->GetName());
		
		AllowedToLaunch &= ModuleAllowLaunch;
	}

	zeLog(ZE_LOG_INFO, "Launch is %s.", AllowedToLaunch ? "allowed" : "not allowed");

	GetWindow()->GetForm()->btnLaunch->setEnabled(AllowedToLaunch);
}

ZELNLaunchStatus ZELNLauncher::GetStatus()
{
	return Status;
}

const ZELNLaunchInformation& ZELNLauncher::GetInformation()
{
	return Information;
}

bool ZELNLauncher::GetAllowedToLaunch()
{
	return AllowedToLaunch;
}

void ZELNLauncher::Launch()
{
	if (AllowedToLaunch)
		return;

	zeLog(ZE_LOG_INFO, "Launching Zinek Engine...");

	Status = ZELN_LS_LAUNCHING;
	Window->update();

	// Engine Path
	char LauncherPath[MAX_PATH];
	GetModuleFileName(NULL, LauncherPath, MAX_PATH);
	ZEPathInfo PathInfo(LauncherPath);
	ZEString ParentDirectory = PathInfo.GetParentDirectory();
	Information.BinaryPath = ZEFormat::Format("{0}/{1}", ParentDirectory, "ZE.exe");

	// Command Line Arguments
	Information.Parameters.Clear();
	Information.Parameters.Add(ZEFormat::Format("--ze-launcher {0:X}", (ZEUInt64)GetCurrentProcessId()));
	for (ZESize I = 0; I < Modules.GetCount(); I++)
	{
		ZELNModule* Module = Modules[I];
		if (!Module->OnPreLaunch())
		{
			Status = ZELN_LS_ERROR;
			zeError("Launch has been stopped by module \"%s\".", Module->GetName());
			return;
		}

		Information.Parameters.Combine(Module->GetLaunchParameters());
	}
	
	char ParameterString[32767];
	ParameterString[0] = '\0';
	for (ZESize I = 0; I < Information.Parameters.GetCount(); I++)
		strncat(ParameterString, Information.Parameters[I], 32767);


	// Launch
	PROCESS_INFORMATION ProcessInformation;
	bool Result = CreateProcess(
		Information.BinaryPath, ParameterString, 
		NULL, NULL, NULL, 0, NULL, NULL, NULL, &ProcessInformation);

	if (!Result)
	{
		Status = ZELN_LS_ERROR;
		zeError("Engine instance is failed to launch. Error : %d", GetLastError());
		return;
	}

	Information.ProcessId = ProcessInformation.dwProcessId;
	zeLog(ZE_LOG_INFO, "Engine instance is launched. PID: %d", Information.ProcessId);

	Status = ZELN_LS_RUNNING;
	Window->update();

	// Post Launch
	for (ZESize I = 0; I < Modules.GetCount(); I++)
	{
		ZELNModule* Module = Modules[I];
		Module->OnPostLaunch();
	}

	GetWindow()->SetLaunchButtonEnabled(false);
}

ZELNLauncher::ZELNLauncher()
{
	Instance = NULL;
	ApplicationName = "Zinek Engine";
	ApplicationMajorVersion = ZEVersion::GetZinekVersion().Major;
	ApplicationMinorVersion = ZEVersion::GetZinekVersion().Minor;
}

ZELNLauncher* ZELNLauncher::GetInstance()
{
	return Instance;
}
