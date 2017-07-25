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

#include "ZEMeta/ZEObject.h"

#include "ZETypes.h"
#include "ZEOption.h"
#include "ZEDS/ZEList2.h"
#include "ZEExport.ZEEngine.h"

#define zeCore ZECore::GetInstance()

class ZEModule;
class ZEErrorManager;
class ZEOptionManager;
class ZEResourceManager;
class ZECommandManager;
class ZERealTimeClock;
class ZEProfiler;
class ZECrashHandler;
class ZEConsole;
class ZESystemMessageManager;
class ZESystemMessageHandler;
class ZEPlugin;
class ZESplashWindow;

#define ZE_CORE_MODULE(Type, Variable) class Type;
#include "ZECoreModules.h"
#undef ZE_CORE_MODULE

enum ZECoreState
{
	ZE_CS_NONE				= 0,
	ZE_CS_STARTING_UP		= 1,
	ZE_CS_RUNNING			= 2,
	ZE_CS_SHUTTING_DOWN		= 3,
};

class ZE_EXPORT_ZEENGINE ZECore : public ZEObject
{
	ZE_OBJECT
	private:
		ZECoreState									State;
		ZEList2<ZEPlugin>							Plugins;
		ZEList2<ZEModule>							Modules;
		ZEList2<ZEModule>							LateModules;
		ZECrashHandler*								CrashHandler;
		ZEErrorManager*								ErrorManager;
		ZEOptionManager*							OptionManager;
		ZECommandManager*							CommandManager;
		ZEResourceManager*							ResourceManager;
		ZEConsole*									Console;
		ZEProfiler*									Profiler;
		ZESystemMessageManager*						SystemMessageManager;
		ZESystemMessageHandler*						SystemMessageHandler;	
		ZEString									ConfigurationPath;
		ZEErrorCallback								OldErrorCallback;
		ZESplashWindow*								SplashWindow;
		ZEInt										CoreThreadID;

		ZEModule*									FindModule(ZEClass* Class, const char* Name);

		bool										InitializeModule(ZEModule* Module, bool AutoInitializeDependencies);
		bool										DeinitializeModule(ZEModule* Module, bool AutoDeinitializeDependents);

		void										InitializeModules();
		void										DeinitializeModules();

		void										ShutdownInternal();

													ZECore();
													~ZECore();

	public:
		static ZEOptionSection						CoreOptions;
		
		ZECoreState									GetState();
		bool										IsAlive();
		bool										IsRunning();
		
		ZEErrorManager*								GetError();
		ZEOptionManager*							GetOptions();
		ZECommandManager*							GetCommands();
		ZECrashHandler*								GetCrashHandler();
		ZEProfiler*									GetProfiler();
		ZEConsole*									GetConsole();
		ZEResourceManager*							GetResourceManager();
		ZESystemMessageManager*						GetSystemMessageManager();
		void*										GetApplicationInstance();
		ZEInt										GetCoreThreadId();

		void										SetConfigurationPath(const ZEString& Path);
		const ZEString&								GetConfigurationPath() const;

		ZEModule*									GetModule(ZEClass* ModuleClass) const;
		const ZEList2<ZEModule>&					GetModules() const;
		bool										AddModule(ZEModule* Module);
		bool										RemoveModule(ZEModule* Module);

		ZEPlugin*									LoadPlugin(const ZEString& Path);
		bool										LoadInternalPlugin(ZEPlugin* Plugin);
		bool										UnloadPlugin(ZEPlugin* Plugin);

		void										StartUp();
		void										ShutDown();
		void										Terminate();

		void										Process();
		bool										Execute();

		bool										LoadConfiguration();
		bool										LoadConfiguration(const ZEString& FileName);
		bool										SaveConfiguration();
		bool										SaveConfiguration(const ZEString& FileName);

		static ZECore*								GetInstance();

		#define ZE_CORE_MODULE(Type, Variable) private: Type* Variable; public: Type* Get##Variable() const;
		#include "ZECoreModules.h"
		#undef ZE_CORE_MODULE
};
