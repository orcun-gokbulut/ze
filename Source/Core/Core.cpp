//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Core.cpp
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

#include "CompileOptions.h"
#include "Core.h"
#include "ConsoleWindow.h"

#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>
#include <time.h>
#include <math.h>

ZEOptionSection ZECore::CoreOptions; 

HINSTANCE ApplicationInstance;

ZEError* ZECore::GetError()
{
	return Error;
}

ZEOptionManager* ZECore::GetOptions()
{
	return Options;
}

ZEResourceManager* ZECore::GetResourceManager()
{
	return Resources;
}

ZECommandManager* ZECore::GetCommands()
{
	return Commands;
}

ZEConsole* ZECore::GetConsole()
{
	return Console;
}

ZEWindow* ZECore::GetWindow()
{
	return Window;
}

ZEModuleManager* ZECore::GetModuleManager()
{
	return ModuleManager;
}
		
bool ZECore::SetGraphicsModule(ZEModule* Module)
{
	if (Module != NULL)
	{
		if (Module->GetModuleDescription()->GetType() != ZE_MT_GRAPHICS)
		{
			zeError("Core", "Module type mismatch. This module is not a sound module. (Module Name : \"%s\")", Module->GetModuleDescription()->GetName());
			return false;
		}
		Graphics = (ZEGraphicsModule*)Module;
	}
	else
		Graphics = NULL;

	return true;
}

ZEGraphicsModule* ZECore::GetGraphicsModule()
{
	return Graphics;
}

bool ZECore::SetSoundModule(ZEModule* Module)
{
	if (Module != NULL)
	{
		if (Module->GetModuleDescription()->GetType() != ZE_MT_SOUND)
		{
			zeError("Core", "Module type mismatch. This module is not a sound module. (Module Name : \"%s\")", Module->GetModuleDescription()->GetName());
			return false;
		}
		Sound = (ZESoundModule*)Module;
	}
	else
		Sound = NULL;

	return true;
}

ZESoundModule* ZECore::GetSoundModule()
{
	return Sound;
}

bool ZECore::SetInputModule(ZEModule* Module)
{
	if (Module != NULL)
	{
		if (Module->GetModuleDescription()->GetType() != ZE_MT_INPUT)
		{
			zeError("Core", "Module type mismatch. This module is not a sound module. (Module Name : \"%s\")", Module->GetModuleDescription()->GetName());
			return false;
		}
		Input = (ZEInputModule*)Module;
	}
	else
		Input = NULL;

	return true;
}


ZEInputModule* ZECore::GetInputModule()
{
	return Input;
}

bool ZECore::SetPhysicsModule(ZEModule* Module)
{
	if (Module != NULL)
	{
		if (Module->GetModuleDescription()->GetType() != ZE_MT_PHYSICS)
		{
			zeError("Core", "Module type mismatch. This module is not a sound module. (Module Name : \"%s\")", Module->GetModuleDescription()->GetName());
			return false;
		}
		Physics = (ZEPhysicsModule*)Module;
	}
	else
		Physics = NULL;

	return true;
}

ZEPhysicsModule* ZECore::GetPhysicsModule()
{
	return Physics;
}

bool ZECore::SetNetworkModule(ZEModule* Module)
{
	if (Module != NULL)
	{
		if (Module->GetModuleDescription()->GetType() != ZE_MT_SOUND)
		{
			zeError("Core", "Module type mismatch. This module is not a sound module. (Module Name : \"%s\")", Module->GetModuleDescription()->GetName());
			return false;
		}
		Network = (ZEModule*)Module;
	}
	else
		Network = NULL;

	return true;
}

ZEModule* ZECore::GetNetworkModule()
{
	return Network;
}

bool ZECore::SetGame(ZEGame* Game)
{
	this->Game = Game;
	return true;
}

ZEGame* ZECore::GetGame()
{
	return Game;
}

size_t ZECore::GetFrameId()
{
	return FrameId;
}

bool ZECore::DebugModeEnabled()
{
#ifdef ZEDEBUG_ENABLED
	return true;
#else
	return false;
#endif
}

void* ZECore::GetApplicationInstance()
{
	return ApplicationInstance;
}

ZECoreState ZECore::GetCoreState()
{
	return this->CoreState;
}

void ZECore::SetCoreState(ZECoreState CoreState)
{
	this->CoreState = CoreState;
	if (CoreState == ZECORESTATE_CRITICALERROR)
		ShutDown();
}

ZEUserLevel ZECore::GetUserLevel()
{
	return this->UserLevel;
}

void ZECore::SetUserLevel(ZEUserLevel UserLevel)
{
	this->UserLevel = UserLevel;
}

void ZECore::Terminate()
{
	SetCoreState(ZECORESTATE_TERMINATE);
}

bool ZECore::InitializeModule(ZEModule* Module)
{
	if (Module->Initialize() == false)
	{
		Module->Destroy();
		zeError("Core", "Can not initialize module. (Module Name : \"%s\", Module Version : %d.%d)",
			Module->GetModuleDescription()->GetName(),
			Module->GetModuleDescription()->GetMajorVersion(),
			Module->GetModuleDescription()->GetMinorVersion());
		return false;
	}
	return true;
}

void ZECore::DeInitializeModule(ZEModule** Module)
{
	if (*Module != NULL)
		(*Module)->Deinitialize();
	*Module = NULL;
}

bool ZECore::InitializeModules()
{
	// Graphics module !
	zeLog("Initializing Graphic Module.\r\n");	
	if (!InitializeModule(Graphics))
	{
		zeError("Core", "Can not initialize graphic module.");
		return false;
	}

	// Sound module !
	zeLog("Initializing Sound Module.\r\n");	
	if (!InitializeModule(Sound))
	{
		zeError("Core", "Can not initialize sound module.");
		return false;
	}

	// Input module !
	zeLog("Initializing Input Module.\r\n");	
	if (!InitializeModule(Input))
	{
		zeError("Core", "Can not initialize input module.");
		return false;
	}
	return true;
}

void ZECore::DeinitializeModules()
{
	if (Graphics != NULL)
	{
		zeLog("Deinitializing Graphic module.");
		Graphics->Deinitialize();
		Graphics = NULL;
	}

	if (Sound != NULL)
	{
		zeLog("Deinitializing Sound module.");
		Sound->Deinitialize();
		Sound = NULL;
	}

	if (Input != NULL)
	{
		zeLog("Deinitializing Input module.");
		Input->Deinitialize();
		Input = NULL;
	}
}

bool ZECore::StartUp(void* WindowHandle)
{
	FrameId = 0;

	Console->DisableInput();
	
	DebugMode = true;
	SetCoreState(ZECORESTATE_STARTUP);
	SetUserLevel(ZEUSERLEVEL_DEVELOPPER);

	zeLog("Zinek Engine V%s\r\n", ZEVERSION_STRING);
	zeLog("Initialization...\r\n");

	zeLog("Initializing Main Window...\r\n");
	if (WindowHandle != NULL)
		Window->SetComponentWindowHandle(WindowHandle);

	if (Window->Initialize() == false)
		zeCriticalError("Core", "Can not create main window.");

	zeLog("Initializing Modules...\r\n");
	if (!InitializeModules())
		zeCriticalError("Core", "Can not initialize modules.");

	Console->EnableInput();
	if (Game != NULL)
		Game->Initialize();

	return true;
}

void ZECore::ShutDown()
{
	SetCoreState(ZECORESTATE_SHUTDOWN);
	// Destroy game
	if (Game != NULL)
		Game->Deinitialize();

	zeLog("Saving options.\r\n");
	if (CoreState == ZECORESTATE_CRITICALERROR)
		zeLog("[Core] Core detected that there is a critical error. It is posible that error can be occured becouse of options. Your old options.ini copied to options.ini.bak.");
	Options->Save("options.ini");
	zeLog("Releasing game content data.\r\n");

	zeLog("Releasing shared resources.\r\n");
	Resources->ReleaseAllResources();

	zeLog("Releasing cached resources.\r\n");
	Resources->UncacheAllResources();

	if (Input != NULL)
	{
		zeLog("Destroying Input Module.\r\n");
		Input->Destroy();
	}

	if (Sound != NULL)
	{
		zeLog("Destroying Sound Module.\r\n");
		Sound->Destroy();
	}

	if (Graphics != NULL)
	{
		zeLog("Destroying Graphics Module.\r\n");
		Graphics->Destroy();
	}

	Window->Deinitialize();


	if (CoreState == ZECORESTATE_CRITICALERROR)
	{
		ZEConsoleWindow ConsoleWindow;
		Console->SetConsoleInterface(&ConsoleWindow);
		ConsoleWindow.TermiantionState();
		Console->SetConsoleInterface(NULL);
	}
}

void ZECore::MainLoop()
{
	static LARGE_INTEGER OldTime, Freq;
	static bool FirstTime = true;
	LARGE_INTEGER NewTime;

	if (FirstTime)
	{
		QueryPerformanceCounter(&OldTime);
		FirstTime = false;
	}

	QueryPerformanceFrequency(&Freq);
	QueryPerformanceCounter(&NewTime);
	float TimeDifference = (float)(NewTime.QuadPart - OldTime.QuadPart) / (float)Freq.QuadPart;
	OldTime = NewTime;

	srand(NewTime.LowPart);
	Graphics->UpdateScreen();
	Graphics->ClearFrameBuffer();
	Sound->ProcessSound();
	if (Game != NULL)
	{
		Game->Tick(TimeDifference);
		Game->Render(TimeDifference);
	}
	Input->ProcessInputs();
	Window->ProcessMessages();
	Sleep(0);
}

void ZECore::Run()
{
	SetCoreState(ZECORESTATE_RUNNING);
	while(CoreState != ZECORESTATE_TERMINATE && CoreState != ZECORESTATE_SHUTDOWN)
		MainLoop();

	if (CoreState != ZECORESTATE_SHUTDOWN)
		ShutDown();
}

ZECore* ZECore::GetInstance()
{	
	static ZECore Engine;
	return &Engine;
}

ZECore::ZECore() 
{
	Console			= new ZEConsole();
	Commands		= new ZECommandManager();
	Options			= new ZEOptionManager();
	Error			= new ZEError();
	Resources		= new ZEResourceManager();
	ModuleManager	= new ZEModuleManager();
	Window			= new ZEWindow();
	Game			= new ZEGame();
	ZEGraphicsModule::BaseInitialize();
	ZESoundModule::BaseInitialize();
	ZEInputModule::BaseInitialize();

}

ZECore::~ZECore()
{
	
	ZEGraphicsModule::BaseDeinitialize();
	ZESoundModule::BaseDeinitialize();
	ZEInputModule::BaseDeinitialize();

	delete Game;
	delete Window;
	delete ModuleManager;
	delete Resources;
	delete Error;
	delete Options;
	delete Commands;
	delete Console;
}
