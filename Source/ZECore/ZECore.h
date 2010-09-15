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
#ifndef	__ZE_CORE_H__
#define __ZE_CORE_H__

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
	ZE_CS_CRITICALERROR
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
class ZENetworkModule;
class ZEDebugModule;

class ZECore
{
	private:
		ZECoreState						CoreState;
		ZEUserLevel						UserLevel;
		size_t							FrameId;
		float							FrameTime;
		float							RunningTime;
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
		ZEPhysicsModule*				Physics;
		ZEGame*							Game;
		ZENetworkModule*				Network;

		ZEDebugModule*				DebugComponent;

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
		ZEPhysicsModule*				GetPhysicsModule();

		bool							SetNetworkModule(ZENetworkModule* Module);
		ZENetworkModule*				GetNetworkModule();

		bool							SetGame(ZEGame* Game);
		ZEGame*							GetGame();

		void							SetDebugComponent(ZEDebugModule* Component);
		ZEDebugModule*				GetDebugComponent();

		float							GetFrameTime();
		float							GetRuningTime();
		size_t							GetFrameId();
		
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

		static ZECore*					GetInstance();

										ZECore();
										~ZECore();
};
#endif




