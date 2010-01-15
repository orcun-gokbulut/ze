//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Core.h
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
#ifndef	__ZE_CORE_H__
#define __ZE_CORE_H__

/*#include "ZESDK.h"
#include "CompileOptions.h" 
#include "Error.h"
#include "Command.h"
#include "Resource.h"
#include "ResourceManager.h"
#include "Module.h"
#include "ModuleManager.h"
#include "Console.h"
#include "Window.h"
#include "Graphics/GraphicsModule.h"
#include "Input/InputModule.h"
#include "Sound/SoundModule.h"
#include "GameInterface/Game.h"*/
#include "Option.h"

#define zeCore ZECore::GetInstance()

enum ZEUserLevel
{
	ZEUSERLEVEL_PLAYER,
	ZEUSERLEVEL_CHEATER,
	ZEUSERLEVEL_DEVELOPPER
};

enum ZECoreState
{
	ZECORESTATE_UNKNOWN,
	ZECORESTATE_RUNNING,
	ZECORESTATE_PAUSED,
	ZECORESTATE_STARTUP,
	ZECORESTATE_SHUTDOWN,
	ZECORESTATE_TERMINATE,
	ZECORESTATE_CRITICALERROR
};

class ZEModuleManager;
class ZEError;
class ZEOptionManager;
class ZEResourceManager;
class ZECommandManager;
class ZEConsole;
class ZEWindow;
class ZEGraphicsModule;
class ZESoundModule;
class ZEInputModule;
class ZEPhysicsModule;
class ZEGame;
class ZEModule;

class ZECore
{
	private:
		ZECoreState						CoreState;
		ZEUserLevel						UserLevel;
		size_t							FrameId;
		bool							DebugMode;

		char							ResourceDirectory;

		ZEModuleManager*				ModuleManager;
		ZEError*						Error;
		ZEOptionManager*				Options;
		ZEResourceManager*				Resources;
		ZECommandManager*				Commands;
		ZEConsole*						Console;
		ZEWindow*						Window;
		ZEGraphicsModule*				Graphics;
		ZESoundModule*					Sound;
		ZEInputModule*					Input;
		void*							Physics;
		ZEGame*							Game;
		ZEModule*						Network;

		bool							InitializeModule(ZEModule* Module);
		void							DeInitializeModule(ZEModule** Module);

	public:
		static ZEOptionSection			CoreOptions;

		ZEError*						GetError();
		ZEOptionManager*				GetOptions();
		ZEResourceManager*				GetResourceManager();
		ZECommandManager*				GetCommands();
		ZEConsole*						GetConsole();
		ZEWindow*						GetWindow();
		ZEModuleManager*				GetModuleManager();
		
		void							SetResourceDirector(const char* Directory);
		const char*						GetResourceDirectory();

		bool							SetGraphicsModule(ZEModule* Module);
		ZEGraphicsModule*				GetGraphicsModule();

		bool							SetSoundModule(ZEModule* Module);
		ZESoundModule*					GetSoundModule();

		bool							SetInputModule(ZEModule* Module);
		ZEInputModule*					GetInputModule();

		bool							SetPhysicsModule(ZEModule* Module);
		void*							GetPhysicsModule();

		bool							SetNetworkModule(ZEModule* Module);
		ZEModule*						GetNetworkModule();

		bool							SetGame(ZEGame* Game);
		ZEGame*							GetGame();

		size_t							GetFrameId();
		
		bool							DebugModeEnabled();

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

		static ZECore*					GetInstance();

										ZECore();
										~ZECore();
};
#endif
