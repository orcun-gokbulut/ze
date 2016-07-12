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
#include "ZELNLogModule.h"
#include "ZELNContactModule.h"
#include "ZEVersion.h"
#include "ZEDS\ZEFormat.h"
#include "ZEML\ZEMLReader.h"
#include "ZEFile\ZEPathInfo.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


ZELNLauncher* ZELNLauncher::Instance = NULL;

void ZELNLauncher::InitializeApplication(const ZEMLReaderNode& ApplicationNode)
{
	if (!ApplicationNode.IsValid())
	{
		zeWarning("Application information is missing in configuration file. Using default information.");
		return;
	}

	ApplicationName = ApplicationNode.ReadString("Name", ApplicationName);
	ApplicationVersionMajor = ApplicationNode.ReadUInt8("VersionMajor", ApplicationVersionMajor);
	ApplicationVersionMinor = ApplicationNode.ReadUInt8("VersionMinor", ApplicationVersionMinor);
	ApplicationExecutable = ApplicationNode.ReadString("Executable", ApplicationExecutable);
	CommandLineArguments = ApplicationNode.ReadString("CommandLineArguments", CommandLineArguments);
}

void ZELNLauncher::InitializeModules(const ZEMLReaderNode& ModulesNode)
{
	zeLog("Loading Modules...");

	if (!ModulesNode.IsValid())
	{
		zeWarning("No module configuration has found. Loading default modules.");
		Modules.Add(ZELNContactModule::Description()->CreateInstance());
	}
	else
	{
		ZESize ModuleCount = ModulesNode.GetNodeCount("Module");
		for (ZESize I = 0; I < ModuleCount; I++)
		{
			ZEMLReaderNode ModuleNode = ModulesNode.GetNode("Module", I);
			ZEString Name = ModuleNode.ReadString("Name");
			
			bool Enabled = ModuleNode.ReadBoolean("Enabled", false);
			if (!Enabled)
				continue;
		
			ZELNModuleDescription* Description = ZELNModule::GetModule(Name);
			if (Description == NULL)
			{
				zeWarning("Cannot load module. Module Name: \"%s\".", Name.ToCString());
				continue;
			}

			bool Found = false;
			for (ZESize I = 0; I < Modules.GetCount(); I++)
			{
				if (Modules[I]->GetDescription() == Description)
				{
					Found = true;
					break;
				}
			}

			if (Found)
				continue;

			ZELNModule* Module = Description->CreateInstance();
			if (Module == NULL)
				zeCriticalError("Cannot create module instance. Module Name: \"%s\".", Description->GetName());
			
			Module->LoadConfiguration(ModuleNode.GetNode("Configuration"));
			Modules.Add(Module);
		}
	}

	zeLog("Initializing modules...");
	for (ZESize I = 0; I < Modules.GetCount(); I++)
	{
		zeLog("Initializing \"%s\" module.", Modules[I]->GetDescription()->GetName());

		if (!Modules[I]->Initialize())
		{
			zeWarning("Cannot initialize \"%s\" module.", Modules[I]->GetDescription()->GetName());
			continue;
		}

		QWidget* Widget = Modules[I]->GetWidget();
		if (Widget != NULL)
		{
			int Index = Window->GetForm()->tabWidgets->count();
			Window->GetForm()->tabWidgets->insertTab(Index, Widget, QIcon(), Modules[I]->GetDescription()->GetName());
		}
	}

	zeLog("Modules initialized.");
}

bool ZELNLauncher::InitializeSelf()
{
	if (!ZEInitializable::InitializeSelf())
		return false;

	ZELNModule* LogModule = ZELNLogModule::Description()->CreateInstance();
	Modules.Add(LogModule);
	LogModule->Initialize();

	zeLog("Initializing Zinek Launcher...");

	ZEMLReader Reader;
	ZEMLReaderNode RootNode;
	if (!Reader.Open("#E:/Launcher.ZEConfig"))
	{
		zeWarning("Cannot open Zinek Launcher configuration file.");
	}
	else
	{
		RootNode  = Reader.GetRootNode();
		if (RootNode.GetName() != "LauncherConfiguration")
		{
			zeWarning("Invalid Zinek Launcher configuration file format.");
			return false;
		}
	}

	Instance = this;
	Window = new ZELNLauncherWindow();

	InitializeApplication(RootNode.GetNode("Application"));
	InitializeModules(RootNode.GetNode("Modules"));

	Update();
	Window->show();
	Reader.Close();

	return true;
}

bool ZELNLauncher::DeinitializeSelf()
{
	Window->close();

	for (ZESize I = 0; I < Modules.GetCount(); I++)
		Modules[I]->Deinitialize();

	delete Window;

	return ZEInitializable::DeinitializeSelf();
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
	ApplicationExecutable = FileName;
}

const ZEString& ZELNLauncher::GetApplicationFileName()
{
	return ApplicationName;
}

void ZELNLauncher::SetApplicationMajorVersion(ZEUInt Version)
{
	ApplicationVersionMajor = Version;
}

ZEUInt ZELNLauncher::GetApplicationVersionMajor()
{
	return ApplicationVersionMajor;
}

void ZELNLauncher::SetApplicationMinorVersion(ZEUInt Version)
{
	ApplicationVersionMinor = Version;
}

ZEUInt ZELNLauncher::GetApplicationVersionMinor()
{
	return ApplicationVersionMinor;
}

void ZELNLauncher::Update()
{
	bool OldAllowedToLaunch = AllowedToLaunch;

	AllowedToLaunch = true;
	for (ZESize I = 0; I < Modules.GetCount(); I++)
	{
		Modules[I]->OnUpdate();

		bool ModuleAllowLaunch = Modules[I]->GetAllowLaunch();
		if (!ModuleAllowLaunch)
			zeLog("Launch is denied by module \"%s\". Please check \"%s\" page for more information.",
				Modules[I]->GetDescription()->GetName(),
				Modules[I]->GetDescription()->GetName());
		
		AllowedToLaunch &= ModuleAllowLaunch;
	}

	if (OldAllowedToLaunch != AllowedToLaunch)
		zeLog("Launch is %s.", AllowedToLaunch ? "allowed" : "not allowed");

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
	if (!AllowedToLaunch)
		return;

	zeLog("Launching %s...", ApplicationName.ToCString());

	Status = ZELN_LS_LAUNCHING;
	Window->update();

	// Engine Path
	char LauncherPath[MAX_PATH];
	GetModuleFileName(NULL, LauncherPath, MAX_PATH);
	ZEPathInfo PathInfo(LauncherPath);
	ZEString ParentDirectory = PathInfo.GetParentDirectory();
	Information.BinaryPath = ZEFormat::Format("{0}/{1}", ParentDirectory, ApplicationExecutable);

	// Command Line Arguments
	Information.Parameters.Clear();
	Information.Parameters.Add(ZEFormat::Format("{0}", Information.BinaryPath));
	Information.Parameters.Add(ZEFormat::Format("--ze-launcher {0:X:08}", (ZEUInt64)GetCurrentProcessId()));
	for (ZESize I = 0; I < Modules.GetCount(); I++)
	{
		ZELNModule* Module = Modules[I];
		if (!Module->OnPreLaunch())
		{
			Status = ZELN_LS_ERROR;
			zeError("Launch has been stopped by module \"%s\".", Module->GetDescription()->GetName());
			return;
		}

		Information.Parameters.Combine(Module->GetLaunchParameters());
	}
	
	char ParameterString[32767];
	ParameterString[0] = '\0';
	for (ZESize I = 0; I < Information.Parameters.GetCount(); I++)
	{
		strncat(ParameterString, " ", 32767);
		strncat(ParameterString, Information.Parameters[I], 32767);
	}


	// Launch
	PROCESS_INFORMATION ProcessInformation = {0};
	STARTUPINFO StartUpInfo = {0};
	StartUpInfo.cb = sizeof(StartUpInfo);

	bool Result = CreateProcess(
		Information.BinaryPath, ParameterString,
		NULL, NULL, FALSE, 0, 
		NULL, NULL, &StartUpInfo, &ProcessInformation);

	if (!Result)
	{
		Status = ZELN_LS_ERROR;
		zeError("Engine instance is failed to launch. Error : %d.", GetLastError());
		return;
	}

	Information.ProcessId = ProcessInformation.dwProcessId;
	zeLog("Engine instance is launched. PID: %d.", Information.ProcessId);

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
	ApplicationVersionMajor = ZEVersion::GetZinekVersion().Major;
	ApplicationVersionMinor = ZEVersion::GetZinekVersion().Minor;
}

ZELNLauncher* ZELNLauncher::GetInstance()
{
	return Instance;
}
