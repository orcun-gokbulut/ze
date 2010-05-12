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
#include "Error.h"
#include "Module.h"
#include "ModuleManager.h"
#include "Console.h"
#include "ConsoleWindow.h"
#include "ResourceManager.h"
#include "Window.h"
#include "Graphics/GraphicsModule.h"
#include "Input/InputModule.h"
#include "Physics/PhysicsModule.h"
#include "Sound/SoundModule.h"
#include "GameInterface/Game.h"
#include "DebugComponent.h"

#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>
#include <time.h>
#include <math.h>

ZEOptionSection ZECore::CoreOptions; 

HINSTANCE ApplicationInstance;
LARGE_INTEGER PerformanceCounterFreq;
LARGE_INTEGER PerformanceCount, OldPerformanceCount, StartPerformanceCount;

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

bool ZECore::SetNetworkModule(ZENetworkModule* Module)
{
	/*if (Module != NULL)
	{
		if (Module->GetModuleDescription()->GetType() != ZE_MT_SOUND)
		{
			zeError("Core", "Module type mismatch. This module is not a sound module. (Module Name : \"%s\")", Module->GetModuleDescription()->GetName());
			return false;
		}
		Network = (ZENetworkModule*)Module;
	}
	else
		Network = NULL;*/

	return true;
}

ZENetworkModule* ZECore::GetNetworkModule()
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

void ZECore::SetDebugComponent(ZEDebugComponent* Component)
{
	DebugComponent = Component;
}

ZEDebugComponent* ZECore::GetDebugComponent()
{
	return DebugComponent;
}

size_t ZECore::GetFrameId()
{
	return FrameId;
}

float ZECore::GetFrameTime()
{
	return FrameTime;
}

float ZECore::GetRuningTime()
{
	QueryPerformanceCounter(&PerformanceCount);
	return (PerformanceCount.QuadPart - StartPerformanceCount.QuadPart) / PerformanceCounterFreq.QuadPart;
}


void ZECore::SetDebugMode(bool Enabled)
{
	DebugMode = Enabled;
}

bool ZECore::GetDebugMode()
{
	#ifdef ZE_DEBUG_ENABLED
		return DebugMode;
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
	
	const char* CoreStateText;

	switch(CoreState)
	{
		default:
		case ZE_CS_CRITICALERROR:
			CoreStateText = "Critical Error";
			break;

		case ZE_CS_PAUSED:
			CoreStateText = "Paused";
			break;

		case ZE_CS_RUNNING:
			CoreStateText = "Running";
			break;

		case ZE_CS_SHUTDOWN:
			CoreStateText = "Shutdown";
			break;

		case ZE_CS_STARTUP:
			CoreStateText = "StartUp";
			break;

		case ZE_CS_TERMINATE:
			CoreStateText = "Terminate";
			break;

		case ZE_CS_UNKNOWN:
			CoreStateText = "Unknown";
			break;
	}

	zeLog("Core", "Core state changed to \"%s\".", CoreStateText);
}

ZEUserLevel ZECore::GetUserLevel()
{
	return this->UserLevel;
}

void ZECore::SetUserLevel(ZEUserLevel UserLevel)
{
	this->UserLevel = UserLevel;

	const char* UserLevelText;
	switch(UserLevel)
	{
		case ZE_UL_DEVELOPPER:
			UserLevelText = "Developer";
			break;

		case ZE_UL_ADMINISTRATOR:
			UserLevelText = "Administrator";
			break;

		case ZE_UL_CHEATER:
			UserLevelText = "Cheater";
			break;

		case ZE_UL_PLAYER:
			UserLevelText = "Player";
			break;

		default:
			UserLevelText = "Unknown";
			break;
	}

	zeLog("Core", "User level changed to \"%s\".", UserLevelText);
}

void ZECore::Terminate()
{
	SetCoreState(ZE_CS_TERMINATE);
}

bool ZECore::InitializeModule(ZEModule* Module)
{
	if (Module == NULL)
	{
		zeError("Core", "Module is not present for initialization.");
		return false;
	}

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
	zeLog("Core", "Initializing graphics module.");	
	if (!InitializeModule(Graphics))
	{
		zeError("Core", "Can not initialize graphics module.");
		return false;
	}

	// Physics module !
	zeLog("Core", "Initializing physics module.");
	if (!InitializeModule(Physics))
	{
		zeError("Core", "Can not initialize physics module.");
		return false;
	}

	// Sound module !
	zeLog("Core", "Initializing sound module.");	
	if (!InitializeModule(Sound))
	{
		zeError("Core", "Can not initialize sound module.");
		return false;
	}

	// Input module !
	zeLog("Core", "Initializing Input Module.");	
	if (!InitializeModule(Input))
	{
		zeError("Core", "Can not initialize input module.");
		return false;
	}

	QueryPerformanceFrequency(&PerformanceCounterFreq);
	return true;
}

void ZECore::DeinitializeModules()
{
	if (Graphics != NULL)
	{
		zeLog("Core", "Deinitializing Graphic module.");
		Graphics->Deinitialize();
		Graphics = NULL;
	}

	if (Physics != NULL)
	{
		zeLog("Core", "Deinitializing Physics module.");
		Physics->Deinitialize();
		Physics = NULL;
	}

	if (Sound != NULL)
	{
		zeLog("Core", "Deinitializing Sound module.");
		Sound->Deinitialize();
		Sound = NULL;
	}

	if (Input != NULL)
	{
		zeLog("Core", "Deinitializing Input module.");
		Input->Deinitialize();
		Input = NULL;
	}
}

bool ZECore::StartUp(void* WindowHandle)
{
	FrameId = 0;

	Console->DisableInput();
	
	DebugMode = true;
	SetCoreState(ZE_CS_STARTUP);
	SetUserLevel(ZE_UL_DEVELOPPER);

	zeLog("Core", "Zinek Engine V%s.", ZE_VERSION_STRING);
	zeLog("Core", "Initializing core...");

	zeLog("Core", "Initializing main window...");
	if (WindowHandle != NULL)
		Window->SetComponentWindowHandle(WindowHandle);

	if (Window->Initialize() == false)
		zeCriticalError("Core", "Can not create main window.");

	zeLog("Core", "Initializing Modules...");
	if (!InitializeModules())
		zeCriticalError("Core", "Can not initialize modules.");
	zeLog("Core", "Modules initialized.");

	Console->EnableInput();
	if (Game != NULL)
		Game->Initialize();

	QueryPerformanceFrequency(&PerformanceCounterFreq);
	zeLog("Core", "Core initialized.");

	if (DebugComponent != NULL)
		DebugComponent->StartUp();

	return true;
}

void ZECore::ShutDown()
{
	zeLog("Core", "Deinitializing Core.");
	SetCoreState(ZE_CS_SHUTDOWN);

	if (DebugComponent != NULL)
		DebugComponent->ShutDown();

	// Destroy game
	zeLog("Core", "Deinitializing Running Games.");
	if (Game != NULL)
		Game->Deinitialize();

	zeLog("Core", "Saving options.");
	if (CoreState == ZE_CS_CRITICALERROR)
		zeLog("Core", "[Core] Core detected that there is a critical error. It is posible that error can be occured becouse of options. Your old options.ini copied to options.ini.bak.");
	Options->Save("options.ini");

	zeLog("Core", "Releasing game content data.");

	zeLog("Core", "Releasing shared resources.");
	Resources->ReleaseAllResources();

	zeLog("Core", "Releasing cached resources.");
	Resources->UncacheAllResources();

	if (Input != NULL)
	{
		zeLog("Core", "Destroying Input Module.");
		Input->Destroy();
	}

	if (Sound != NULL)
	{
		zeLog("Core", "Destroying Sound Module.");
		Sound->Destroy();
	}

	if (Graphics != NULL)
	{
		zeLog("Core", "Destroying Graphics Module.");
		Graphics->Destroy();
	}

	if (Physics != NULL)
	{
		zeLog("Core", "Destroying Physics Module.");
		Physics->Destroy();
	}

	Window->Deinitialize();
	zeLog("Core", "Core deinitialized.");

	zeLog("Core", "Terminating engine.");
	exit(0);
}

#include "Physics/PhysicalWorld.h"
void ZECore::MainLoop()
{
	if (DebugComponent != NULL)
		DebugComponent->PreProcess();

	FrameId++;

	if (OldPerformanceCount.QuadPart == 0)
	{
		FrameTime = 0;
		QueryPerformanceCounter(&OldPerformanceCount);
	}
	else
	{
		QueryPerformanceCounter(&PerformanceCount);

		FrameTime = (double)(PerformanceCount.QuadPart - OldPerformanceCount.QuadPart) / (double)PerformanceCounterFreq.QuadPart;
		OldPerformanceCount = PerformanceCount;
	}

	srand(PerformanceCount.LowPart);

	// Game Logic
	Input->ProcessInputs();
	Window->ProcessMessages();
	if (Game != NULL)
		Game->Tick(FrameTime);
	
	if (DebugComponent != NULL)
		DebugComponent->Process(FrameTime);
	Game->GetScene()->GetPhysicalWorld()->Draw(Game->GetScene()->GetRenderer());

	// Engine Logic
	Physics->Process(FrameTime);
	Sound->ProcessSound(FrameTime);
	Graphics->ClearFrameBuffer();
	if (Game != NULL)
		Game->Render(FrameTime);
	Graphics->UpdateScreen();
	Physics->UpdateWorlds();

	if (DebugComponent != NULL)
		DebugComponent->PostProcess();
}

void ZECore::Run()
{
	SetCoreState(ZE_CS_RUNNING);
	
	if (DebugComponent != NULL)
		DebugComponent->Initialize();

	while(CoreState != ZE_CS_TERMINATE && CoreState != ZE_CS_SHUTDOWN)
		MainLoop();

	if (DebugComponent != NULL)
		DebugComponent->Deinitialize();

	ShutDown();
}

ZECore* ZECore::GetInstance()
{	
	static ZECore Engine;
	return &Engine;
}

ZECore::ZECore() 
{
	PerformanceCounterFreq.QuadPart = 0;
	PerformanceCount.QuadPart = 0;
	OldPerformanceCount.QuadPart = 0;

	DebugComponent	= NULL;
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
