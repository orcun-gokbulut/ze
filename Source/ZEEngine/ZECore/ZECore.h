//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECore.h
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

#include "ZEAPI.h"
#include "ZETypes.h"
#include "ZEOption.h"


#define zeCore ZECore::GetInstance()

enum ZEUserLevel
{
	ZE_UL_PLAYER,
	ZE_UL_CHEATER,
	ZE_UL_ADMINISTRATOR,
	ZE_UL_DEVELOPPER
};

enum ZECoreState
{
	ZE_CS_UNKNOWN,
	ZE_CS_RUNNING,
	ZE_CS_PAUSED,
	ZE_CS_STARTUP,
	ZE_CS_SHUTDOWN,
	ZE_CS_TERMINATE,
	ZE_CS_CRITICAL_ERROR
};

class ZEModule;
class ZEErrorManager;
class ZEOptionManager;
class ZEResourceManager;
class ZECommandManager;
class ZEConsole;
class ZEGRGraphicsModule;
class ZESoundModule;
class ZEInputModule;
class ZEPhysicsModule;
class ZENetworkModule;
class ZEApplicationModule;
class ZESystemMessageManager;
class ZESystemMessageHandler;
class ZERealTimeClock;
class ZEProfiler;
class ZETimerManager;
class ZECrashHandler;

class ZECore
{
	private:
		ZECoreState						CoreState;
		ZEUserLevel						UserLevel;
		ZESize							FrameId;
		float							RunningTime;
		bool							DebugMode;

		char							ResourceDirectory;

		ZERealTimeClock*				RealTimeClock;
		ZEProfiler*						Profiler;
		float							ElapsedTime;

		ZECrashHandler*					CrashHandler;
		ZEApplicationModule*			Application;
		ZEErrorManager*					ErrorManager;
		ZEOptionManager*				OptionManager;
		ZEResourceManager*				ResourceManager;
		ZECommandManager*				CommandManager;
		ZETimerManager*					TimerManager;
		ZEConsole*						Console;
		
		ZEGRGraphicsModule*				GraphicsModule;
		ZESoundModule*					SoundModule;
		ZEInputModule*					InputModule;
		ZEPhysicsModule*				PhysicsModule;
		ZENetworkModule*				NetworkModule;

		ZESystemMessageManager*			SystemMessageManager;
		ZESystemMessageHandler*			SystemMessageHandler;
		
	public:
		void							RegisterClasses();

		ZEModule*						FindModule(ZEClass* Class, const char* Name);
		bool							InitializeModule(ZEModule* Module);
		void							DeInitializeModule(ZEModule** Module);

										ZECore();
										~ZECore();

	public:
		static ZEOptionSection			CoreOptions;

		ZEErrorManager*					GetError();
		ZEOptionManager*				GetOptions();
		ZEResourceManager*				GetResourceManager();
		ZECommandManager*				GetCommands();
		ZEConsole*						GetConsole();
		ZESystemMessageManager*			GetSystemMessageManager();
		ZETimerManager*					GetTimerManager();
		ZERealTimeClock*				GetRealTimeClock();
		ZEProfiler*						GetProfiler();
		ZECrashHandler*					GetCrashHandler();

		void							SetResourceDirector(const char* Directory);
		const char*						GetResourceDirectory();

		bool							SetGraphicsModule(ZEGRGraphicsModule* Module);
		ZEGRGraphicsModule*				GetGraphicsModule();

		bool							SetSoundModule(ZESoundModule* Module);
		ZESoundModule*					GetSoundModule();

		bool							SetInputModule(ZEInputModule* Module);
		ZEInputModule*					GetInputModule();

		bool							SetPhysicsModule(ZEPhysicsModule* Module);
		ZEPhysicsModule*				GetPhysicsModule();

		bool							SetNetworkModule(ZENetworkModule* Module);
		ZENetworkModule*				GetNetworkModule();

		void							SetApplicationModule(ZEApplicationModule* Module);
		ZEApplicationModule*			GetApplicationModule();

		ZESize							GetFrameId();
		float							GetElapsedTime();
		float							GetRuningTime();
		
		void							SetDebugMode(bool Enabled);
		bool							GetDebugMode();

		void*							GetApplicationInstance();

		ZECoreState						GetCoreState();
		void							SetCoreState(ZECoreState CoreState);

		ZEUserLevel						GetUserLevel();
		void							SetUserLevel(ZEUserLevel UserLevel);

		bool							InitializeModules();
		void							DeinitializeModules();

		void							Terminate();
		bool							StartUp(void* WindowHandle = 0);
		void							ShutDown();

		void							Run();
		void							Stop();
		void							Pause();

		void							MainLoop();

		ZEAPI static ZECore*			GetInstance();
};
