//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECore.cpp
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

#include "ZECompileOptions.h"
#include "ZECore.h"
#include "ZEErrorManager.h"
#include "ZEModule.h"
#include "ZEModuleManager.h"
#include "ZEConsole.h"
#include "ZEConsoleWindow.h"
#include "ZEResourceManager.h"
#include "ZEWindow.h"
#include "ZEApplicationModule.h"
#include "ZEOptionManager.h"
#include "ZECommandManager.h"
#include "ZEPluginManager.h"
#include "ZEExtensionManager.h"
#include "ZESystemMessageManager.h"
#include "ZESystemMessageHandler.h"
#include "ZERealTimeClock.h"
#include "ZEProfiler.h"
#include "ZETimerManager.h"

#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZEInput/ZEInputModule.h"
#include "ZEPhysics/ZEPhysicsModule.h"
#include "ZESound/ZESoundModule.h"
#include "ZEGame/ZEGame.h"
#include "ZECrashHandler.h"

#include "ZEMeta/ZEProvider.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <time.h>


ZEOptionSection ZECore::CoreOptions; 

HINSTANCE ApplicationInstance;
LARGE_INTEGER PerformanceCounterFreq;
LARGE_INTEGER PerformanceCount, OldPerformanceCount, StartPerformanceCount;

class ZECoreSystemMessageHandler : public ZESystemMessageHandler
{
	public:
		bool Callback(MSG* Message);
};

bool ZECoreSystemMessageHandler::Callback(MSG* Message)
{
	switch (Message->message)
	{
		case WM_QUIT:
			ZECore::GetInstance()->ShutDown();
			return true;

		default:
			return false;
	}
}

ZEErrorManager* ZECore::GetError()
{
	return ErrorManager;
}

ZEOptionManager* ZECore::GetOptions()
{
	return OptionManager;
}

ZEResourceManager* ZECore::GetResourceManager()
{
	return ResourceManager;
}

ZECommandManager* ZECore::GetCommands()
{
	return CommandManager;
}

ZEConsole* ZECore::GetConsole()
{
	return Console;
}

ZEModuleManager* ZECore::GetModuleManager()
{
	return ModuleManager;
}

ZEExtensionManager* ZECore::GetExtensionManager()
{
	return ExtensionManager;
}

ZEPluginManager* ZECore::GetPluginManager()
{
	return PluginManager;
}

ZESystemMessageManager* ZECore::GetSystemMessageManager()
{
	return SystemMessageManager;
}

ZETimerManager* ZECore::GetTimerManager()
{
	return TimerManager;
}

ZERealTimeClock* ZECore::GetRealTimeClock()
{
	return RealTimeClock;
}

ZEProfiler* ZECore::GetProfiler()
{
	return Profiler;
}

ZECrashHandler* ZECore::GetCrashHandler()
{
	return CrashHandler;
}
		
bool ZECore::SetGraphicsModule(ZEModule* Module)
{
	if (Module != NULL)
	{
		if (Module->GetDescription() != ZEGRGraphicsModule::Description() && Module->GetDescription()->GetParent() != ZEGRGraphicsModule::Description())
		{
			zeError("Module type mismatch. This module is not a sound module. Module Name : \"%s\"", 
				(const char*)Module->GetDescription()->GetName());
			return false;
		}
		GraphicsModule = (ZEGRGraphicsModule*)Module;
	}
	else
		GraphicsModule = NULL;

	return true;
}

ZEGRGraphicsModule* ZECore::GetGraphicsModule()
{
	return GraphicsModule;
}

bool ZECore::SetSoundModule(ZEModule* Module)
{
	if (Module != NULL)
	{
		if (Module->GetDescription() != ZESoundModule::Description() && Module->GetDescription()->GetParent() != ZESoundModule::Description())
		{
			zeError("Module type mismatch. This module is not a sound module. Module Name : \"%s\"", 
				(const char*)Module->GetDescription()->GetName());
			return false;
		}
		SoundModule = (ZESoundModule*)Module;
	}
	else
		SoundModule = NULL;

	return true;
}

ZESoundModule* ZECore::GetSoundModule()
{
	return SoundModule;
}

bool ZECore::SetInputModule(ZEModule* Module)
{
	if (Module != NULL)
	{
		if (Module->GetDescription() != ZEInputModule::Description() && Module->GetDescription()->GetParent() != ZEInputModule::Description())
		{
			zeError("Module type mismatch. This module is not a input module. Module Name : \"%s\"", 
				(const char*)Module->GetDescription()->GetName());
			return false;
		}
		InputModule = (ZEInputModule*)Module;
	}
	else
		InputModule = NULL;

	return true;
}


ZEInputModule* ZECore::GetInputModule()
{
	return InputModule;
}

bool ZECore::SetPhysicsModule(ZEModule* Module)
{
	if (Module != NULL)
	{
		if (Module->GetDescription() != ZEPhysicsModule::Description() && Module->GetDescription()->GetParent() != ZEPhysicsModule::Description())
		{
			zeError("Module type mismatch. This module is not a physics module. Module Name : \"%s\"", 
				(const char*)Module->GetDescription()->GetName());
			return false;
		}
		PhysicsModule = (ZEPhysicsModule*)Module;
	}
	else
		PhysicsModule = NULL;

	return true;
}

ZEPhysicsModule* ZECore::GetPhysicsModule()
{
	return PhysicsModule;
}

bool ZECore::SetNetworkModule(ZENetworkModule* Module)
{
	/*if (Module != NULL)
	{
		if (Module->GetDescription()->GetType() != ZE_MT_SOUND)
		{
			zeError("Module type mismatch. This module is not a sound module. Module Name : \"%s\"", 
				(const char*)Module->GetDescription()->GetName());
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
	return NetworkModule;
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

void ZECore::SetApplicationModule(ZEApplicationModule* Component)
{
	Application = Component;
}

ZEApplicationModule* ZECore::GetApplicationModule()
{
	return Application;
}

ZESize ZECore::GetFrameId()
{
	return FrameId;
}

float ZECore::GetRuningTime()
{
	QueryPerformanceCounter(&PerformanceCount);
	return (float)((PerformanceCount.QuadPart - StartPerformanceCount.QuadPart) / PerformanceCounterFreq.QuadPart);
}

float ZECore::GetElapsedTime()
{
	return ElapsedTime;
}

void ZECore::SetDebugMode(bool Enabled)
{
	DebugMode = Enabled;
}

bool ZECore::GetDebugMode()
{
	#ifdef ZE_DEBUG_ENABLE
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
		case ZE_CS_CRITICAL_ERROR:
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

	zeLog("Core state changed to \"%s\".", CoreStateText);
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

	zeLog("User level changed to \"%s\".", UserLevelText);
}

void ZECore::Terminate()
{
	SetCoreState(ZE_CS_TERMINATE);
}

bool ZECore::InitializeModule(ZEModule* Module)
{
	if (Module == NULL)
	{
		zeError("Module is not present for initialization.");
		return false;
	}

	if (Module->Initialize() == false)
	{
		Module->Destroy();
		zeError("Can not initialize module. Module Name : \"%s\".",
			(const char*)Module->GetDescription()->GetName());
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
	zeLog("Initializing modules.");

	if (GraphicsModule == NULL)
		zeCore->SetGraphicsModule(zeCore->GetModuleManager()->CreateModuleInstance(ZEGRGraphicsModule::Description()));
	
	if (SoundModule == NULL)
		zeCore->SetSoundModule(zeCore->GetModuleManager()->CreateModuleInstance(ZESoundModule::Description()));

	if (InputModule == NULL)
		zeCore->SetInputModule(zeCore->GetModuleManager()->CreateModuleInstance(ZEInputModule::Description()));

	if (PhysicsModule == NULL)
		zeCore->SetPhysicsModule(zeCore->GetModuleManager()->CreateModuleInstance(ZEPhysicsModule::Description()));

	// Graphics module !
	zeLog("Initializing graphics module.");	
	if (!InitializeModule(GraphicsModule))
	{
		zeError("Can not initialize graphics module.");
		return false;
	}

	// Physics module !
	zeLog("Initializing physics module.");
	if (!InitializeModule(PhysicsModule))
	{
		zeError("Can not initialize physics module.");
		return false;
	}

	// Sound module !
	zeLog("Initializing sound module.");	
	if (!InitializeModule(SoundModule))
	{
		zeError("Can not initialize sound module.");
		return false;
	}

	// Input module !
	zeLog("Initializing Input Module.");	
	if (!InitializeModule(InputModule))
	{
		zeError("Can not initialize input module.");
		return false;
	}

	QueryPerformanceFrequency(&PerformanceCounterFreq);

	zeLog("Modules initialized.");

	return true;
}

void ZECore::DeinitializeModules()
{
	if (GraphicsModule != NULL)
	{
		zeLog("Deinitializing Graphic module.");
		GraphicsModule->Deinitialize();
		GraphicsModule = NULL;
	}

	if (PhysicsModule != NULL)
	{
		zeLog("Deinitializing Physics module.");
		PhysicsModule->Deinitialize();
		PhysicsModule = NULL;
	}

	if (SoundModule != NULL)
	{
		zeLog("Deinitializing Sound module.");
		SoundModule->Deinitialize();
		SoundModule = NULL;
	}

	if (InputModule != NULL)
	{
		zeLog("Deinitializing Input module.");
		InputModule->Deinitialize();
		InputModule = NULL;
	}
}

void ZECore::LoadClasses()
{
	#undef RegisterClass

	#define ZE_META_REGISTER_ENUM(Name) ZEEnumerator* Name ## _Declaration();
	#define ZE_META_REGISTER_CLASS(Name) ZEClass* Name ## _Class();
	#include "../ZEMetaRegister.h"
	#undef ZE_META_REGISTER_ENUM
	#undef ZE_META_REGISTER_CLASS
	
	#define ZE_META_REGISTER_ENUM(Name) ZEProvider::GetInstance()->RegisterEnumerator(Name ## _Declaration());
	#define ZE_META_REGISTER_CLASS(Name) ZEProvider::GetInstance()->RegisterClass(Name ## _Class());
	#include "../ZEMetaRegister.h"
	#undef ZE_META_REGISTER_ENUM
	#undef ZE_META_REGISTER_CLASS
}

bool ZECore::StartUp(void* WindowHandle)
{
	CrashHandler->Initialize();

	FrameId = 0;

	Console->DisableInput();
	
	DebugMode = true;
	SetCoreState(ZE_CS_STARTUP);
	SetUserLevel(ZE_UL_DEVELOPPER);

	zeLog("Zinek Engine %s.", ZEVersion::GetZinekVersion().GetLongString().ToCString());
	zeLog("Initializing core...");

	zeLog("Loading ZEMeta classes.");
	LoadClasses();

	zeLog("Initializing Modules...");
	if (!InitializeModules())
		zeCriticalError("Can not initialize modules.");
	zeLog("Modules initialized.");

	Console->EnableInput();
	if (Game != NULL)
		Game->Initialize();

	QueryPerformanceFrequency(&PerformanceCounterFreq);
	zeLog("Core initialized.");

	if (Application != NULL)
		Application->StartUp();

	if (Application != NULL)
		Application->Initialize();

	return true;
}

void ZECore::ShutDown()
{
	zeLog("Deinitializing Core.");
	SetCoreState(ZE_CS_SHUTDOWN);

	if (Application != NULL)
	{
		Application->ShutDown();
		Application->Deinitialize();
		Application->Destroy();
		Application = NULL;
	}

	// Destroy game
	zeLog("Deinitializing Running Games.");
	if (Game != NULL)
	{
		Game->Deinitialize();
		Game->Destroy();
		Game = NULL;
	}

	zeLog("Saving options.");
	if (CoreState == ZE_CS_CRITICAL_ERROR)
		zeLog("Core detected that there is a critical error. It is posible that error can be occured becouse of options. Your old options.ini copied to options.ini.bak.");
	OptionManager->Save("#E:/options.ini");

	zeLog("Releasing game content data.");

	zeLog("Releasing shared resources.");
	ResourceManager->ReleaseAllResources();

	zeLog("Releasing cached resources.");
	ResourceManager->UncacheAllResources();

	if (InputModule != NULL)
	{
		zeLog("Destroying Input Module.");
		InputModule->Destroy();
		InputModule = NULL;
	}

	if (SoundModule != NULL)
	{
		zeLog("Destroying Sound Module.");
		SoundModule->Destroy();
		SoundModule = NULL;
	}

	if (GraphicsModule != NULL)
	{
		zeLog("Destroying Graphics Module.");
		GraphicsModule->Destroy();
		GraphicsModule = NULL;
	}

	if (PhysicsModule != NULL)
	{
		zeLog("Destroying Physics Module.");
		PhysicsModule->Destroy();
		PhysicsModule = NULL;
	}

	zeLog("Core deinitialized.");
	zeLog("Terminating engine.");

	CrashHandler->Deinitialize();

	TerminateProcess(GetCurrentProcess(), EXIT_SUCCESS);
	exit(0);
}

#include "ZEPhysics/ZEPhysicalWorld.h"
void ZECore::MainLoop()
{
	if (Application != NULL)
		Application->PreProcess();

	RealTimeClock->UpdateFrameTime();
	ElapsedTime = (float)RealTimeClock->GetFrameDeltaTime() / 1000000.0f;

	if (ElapsedTime > 5.0f)
		ElapsedTime = 0.01f;

	FrameId++;

	TimerManager->Tick(ElapsedTime);
	
	SystemMessageManager->ProcessMessages();

	// Game Logic
	InputModule->Process();

	if (Application != NULL)
		Application->Process(ElapsedTime);

	if (Game != NULL)
		Game->Tick(ElapsedTime);

	// Engine Logic
	PhysicsModule->Process(ElapsedTime);
	SoundModule->ProcessSound(ElapsedTime);
	if (Game != NULL)
		Game->Render(ElapsedTime);
	PhysicsModule->UpdateWorlds();

	if (Application != NULL)
		Application->PostProcess(ElapsedTime);
}

void ZECore::Run()
{
	SetCoreState(ZE_CS_RUNNING);
	
	if (Application != NULL && !Application->IsInitialized())
	{
		if (!Application->Initialize())
			zeCriticalError("Can not initialize application.");
	}

	RealTimeClock->ResetFrameTime();
	while(CoreState != ZE_CS_TERMINATE && CoreState != ZE_CS_SHUTDOWN)
		MainLoop();

	if (Application != NULL)
		Application->Deinitialize();

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

	Application	= NULL;
	CrashHandler			= new ZECrashHandler();
	RealTimeClock			= new ZERealTimeClock();
	TimerManager			= new ZETimerManager();
	Profiler				= new ZEProfiler();
	SystemMessageManager	= new ZESystemMessageManager();
	SystemMessageHandler	= new ZECoreSystemMessageHandler();
	Console					= new ZEConsole();
	CommandManager			= new ZECommandManager();
	OptionManager			= new ZEOptionManager();
	ErrorManager			= new ZEErrorManager();
	ResourceManager			= new ZEResourceManager();
	ModuleManager			= new ZEModuleManager();
	ExtensionManager		= new ZEExtensionManager();
	PluginManager			= new ZEPluginManager();
	Game					= new ZEGame();
	
	ElapsedTime				= 0.0f;

	SystemMessageManager->RegisterMessageHandler(SystemMessageHandler);

	ZEGRGraphicsModule::BaseInitialize();
	ZESoundModule::BaseInitialize();
	ZEInputModule::BaseInitialize();

}

ZECore::~ZECore()
{
	
	ZEGRGraphicsModule::BaseDeinitialize();
	ZESoundModule::BaseDeinitialize();
	ZEInputModule::BaseDeinitialize();

	delete Game;
	delete PluginManager;
	delete ExtensionManager;
	delete ModuleManager;
	delete ResourceManager;
	delete ErrorManager;
	delete OptionManager;
	delete CommandManager;
	delete Console;
	delete TimerManager;
	delete RealTimeClock;
	SystemMessageManager->UnregisterMessageHandler(SystemMessageHandler);
	delete SystemMessageHandler;
	delete SystemMessageManager;
	delete CrashHandler;
}
