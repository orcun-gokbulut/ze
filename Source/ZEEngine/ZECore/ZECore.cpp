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

#include "ZECore.h"

#include "ZEMeta/ZEMTProvider.h"
#include "ZEMeta/ZEClass.h"
#include "ZEErrorManager.h"
#include "ZEModule.h"
#include "ZEConsole.h"
#include "ZEConsoleWindow.h"
#include "ZEOptionManager.h"
#include "ZECommandManager.h"
#include "ZESystemMessageManager.h"
#include "ZESystemMessageHandler.h"
#include "ZEProfiler.h"
#include "ZECrashHandler.h"
#include "ZESplashWindow.h"
#include "ZEResourceManager.h"
#include "ZEML/ZEMLReader.h"
#include "ZEML/ZEMLWriter.h"
#include "ZEPlugin.h"

#define ZE_CORE_MODULE_INCLUDES
#include "ZECoreModules.h"
#include "ZEFile/ZEDirectoryInfo.h"
#include "ZEFile/ZEFileInfo.h"
#include "ZEDS/ZEFormat.h"
#include "ZEFoundation.h"
#include "ZEEngine.h"
#undef ZE_CORE_MODULE_INCLUDES

ZEOptionSection ZECore::CoreOptions; 
static HINSTANCE ApplicationInstance;

static ZEInt StartedCoreInstanceCount = 0;

void ZECore_AtExit()
{
	if (StartedCoreInstanceCount != 0)
		zeCriticalError("Unhandled exit function call detected. You cannot exit until core is shutdown. Terminating the core. This can cause huge problems.");

	ZECore::GetInstance()->Terminate();
}


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


void ZECore::SetState(ZECoreState CoreState)
{
	this->State = CoreState;

	const char* CoreStateText;

	switch(CoreState)
	{
		default:
			CoreStateText = "UNKNOWN";
			break;

		case ZE_CS_STARTING_UP:
			CoreStateText = "StartUp";
			break;

		case ZE_CS_RUNNING:
			CoreStateText = "Running";
			break;

		case ZE_CS_PAUSED:
			CoreStateText = "Paused";
			break;

		case ZE_CS_TERMINATING:
			CoreStateText = "Terminating";
			break;

		case ZE_CS_TERMINATED:
			CoreStateText = "Terminated";
			break;


		case ZE_CS_SHUTTING_DOWN:
			CoreStateText = "Shutting Down";
			break;

		case ZE_CS_SHUTTED_DOWN:
			CoreStateText = "Shut Down";
			break;
	}

	zeLog("Core state changed to \"%s\".", CoreStateText);
}

bool ZECore::InitializeModule(ZEModule* Module)
{
	if (Module == NULL)
	{
		zeError("Module is not present for initialization.");
		return false;
	}

	Module->RegisterClasses();

	if (Module->Initialize() == false)
	{
		Module->UnregisterClasses();
		Module->Destroy();
		const char* ModuleName =  Module->GetClass()->GetName();
		zeError("Can not initialize module. Module Name : \"%s\".", ModuleName);
		return false;
	}
	return true;
}

void ZECore::DeInitializeModule(ZEModule** Module)
{
	if (*Module != NULL)
	{
		(*Module)->UnregisterClasses();
		(*Module)->Deinitialize();
	}

	*Module = NULL;
}

bool ZECore::InitializeModules()
{
	zeLog("Initializing Modules.");
	ze_for_each(Module, Modules)
	{
		if (Module->IsInitialized())
			continue;

		zeLog("Initializing module. Module Name: \"%s\".", Module->GetClass()->GetName());
		if (!Module->Initialize())
		{
			zeError("Cannot initialize module. Module Name: \"%s\".", Module->GetClass()->GetName());
			return false;
		}
	}

	return true;
}

void ZECore::DeinitializeModules()
{
	zeLog("Deinitializing Modules.");
	ze_for_each(Module, Modules)
	{
		if (!Module->IsInitialized())
			continue;

		zeLog("Initializing module. Module Name: \"%s\".", Module->GetClass()->GetName());
		if (!Module->Deinitialize())
			zeError("Cannot deinitialize module. Module Name: \"%s\".", Module->GetClass()->GetName());
	}
}

ZEPlugin* ZECore::LoadPlugin(const ZEString& Path)
{
	ZEFileInfo FileInfo(Path);
	FileInfo.SetPath(FileInfo.GetRealPath().Path);

	zeLog("Loading plugin. Plugin Path: \"%s\".", FileInfo.Normalize());

	SetDllDirectory(FileInfo.GetParentDirectory());
	SetDllDirectory(ZEFormat::Format("{0}\\Dependencies", FileInfo.GetParentDirectory()));
	HMODULE Module = LoadLibrary(FileInfo.GetRealPath().Path);
	SetDllDirectory(NULL);

	if (Module == NULL)
	{
		zeError("Cannot load plugin. Plugin file is not a plugin. Plugin Path: \"%s\".", FileInfo.Normalize());
		return NULL;
	}

	ZECreatePluginInstance CreatePluginInstance = reinterpret_cast<ZECreatePluginInstance>(GetProcAddress(Module, "zeCreatePluginInstance"));
	if (CreatePluginInstance == NULL)
	{
		zeError("Cannot load plugin. Plugin does not have zeCreatePluginInstance() procedure. Plugin Path: \"%s\".", FileInfo.Normalize());
		return NULL;
	}

	ZEPlugin* Plugin = CreatePluginInstance();
	if (Plugin == NULL)
	{
		zeError("Cannot load plugin. zeCreatePluginInstance() returned NULL. Plugin Path: \"%s\".", FileInfo.Normalize());
		return NULL;
	}

	ZEVersion EngineVersion = ZEVersion::GetZinekVersion();
	ZEVersion PluginEngineVersion = Plugin->GetEngineVersion();
	if (PluginEngineVersion.Major != EngineVersion.Major ||
		PluginEngineVersion.Minor != EngineVersion.Minor)
	{
		zeError("Cannot load plugin. Plugin's engine version does not match with this engine version. "
			"Plugin Path: \"%s\", "
			"Plugin's Engine Version: %d.%d.", FileInfo.Normalize(), PluginEngineVersion.Major, PluginEngineVersion.Minor);

		Plugin->Destroy();
		return NULL;
	}

	Plugins.AddEnd(&Plugin->CoreLink);

	#undef RegisterClass
	ZESize DeclarationCount = Plugin->GetDeclarationCount();
	ZEMTDeclaration* const* Declarations = Plugin->GetDeclarations();
	for (ZESize I = 0; I < DeclarationCount; I++)
		ZEMTProvider::GetInstance()->RegisterDeclaration(Declarations[I]);

	zeLog("Plugin loaded. Plugin Name: \"%s\". Plugin Version: \"%s\".", Plugin->GetName(), Plugin->GetVersion().GetShortString().ToCString());

	return Plugin;
}

void ZECore::UnloadPlugin(ZEPlugin* Plugin)
{
	ZESize DeclarationCount = Plugin->GetDeclarationCount();
	ZEMTDeclaration* const* Declarations = Plugin->GetDeclarations();
	for (ZESize I = 0; I < DeclarationCount; I++)
		ZEMTProvider::GetInstance()->UnregisterDeclaration(Declarations[I]);

	Plugins.Remove(&Plugin->CoreLink);

	FreeLibrary((HMODULE)Plugin->GetData());
	Plugin->Destroy();
}

void ZECore::LoadPlugins()
{
	ZEDirectoryInfo Info("#E:/Plugins");
	ZEArray<ZEString> FileNames = Info.GetFiles();
	for (ZESize I = 0; I < FileNames.GetCount(); I++)
	{
		ZEFileInfo FileInfo(FileNames[I]);

		if (!FileInfo.GetExtension().EqualsIncase(".ZEPlugin"))
			continue;

		LoadPlugin(FileInfo.GetPath());
	}
}

void ZECore::UnloadPlugins()
{

}

ZECore::ZECore() 
{
	CrashHandler			= new ZECrashHandler();
	Profiler				= new ZEProfiler();
	SystemMessageManager	= new ZESystemMessageManager();
	SystemMessageHandler	= new ZECoreSystemMessageHandler();
	Console					= new ZEConsole();
	CommandManager			= new ZECommandManager();
	OptionManager			= new ZEOptionManager();
	ErrorManager			= new ZEErrorManager();
	ResourceManager			= new ZEResourceManager();

	SystemMessageManager->RegisterMessageHandler(SystemMessageHandler);

	#define ZE_CORE_MODULE(Type, Variable) Variable = NULL;
	#include "ZECoreModules.h"
	#undef ZE_CORE_MODULE

	/*ZESoundModule::BaseInitialize();
	ZEInputModule::BaseInitialize();*/
	
	static ZEOptionSection CoreOptions;
	CoreOptions.SetName("ZECore");
	CoreOptions.AddOption(new ZEOption("ZEGRGraphicsModule", "ZED11Module", ZE_OA_NORMAL));
	CoreOptions.AddOption(new ZEOption("ZEInputModule", "ZEWindowsInputModule", ZE_OA_NORMAL));
	CoreOptions.AddOption(new ZEOption("ZESoundModule", "ZEDSModule", ZE_OA_NORMAL));
	CoreOptions.AddOption(new ZEOption("ZENetworkModule", "ZEWinNetwork", ZE_OA_NORMAL));
	CoreOptions.AddOption(new ZEOption("ZEPhysicsModule", "ZEPhysXModule", ZE_OA_NORMAL));
	CoreOptions.AddOption(new ZEOption("ZEGameModule", "ZETestGame", ZE_OA_NORMAL));
	ZEOptionManager::GetInstance()->RegisterSection(&CoreOptions);
}

ZECore::~ZECore()
{
	/*ZESoundModule::BaseDeinitialize();
	ZEInputModule::BaseDeinitialize();*/

	delete ResourceManager;
	delete ErrorManager;
	delete OptionManager;
	delete CommandManager;
	delete Console;
	delete Profiler;
	SystemMessageManager->UnregisterMessageHandler(SystemMessageHandler);
	delete SystemMessageHandler;
	delete SystemMessageManager;
	delete CrashHandler;
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

ZESystemMessageManager* ZECore::GetSystemMessageManager()
{
	return SystemMessageManager;
}

ZEProfiler* ZECore::GetProfiler()
{
	return Profiler;
}

ZECrashHandler* ZECore::GetCrashHandler()
{
	return CrashHandler;
}

void* ZECore::GetApplicationInstance()
{
	return ApplicationInstance;
}

ZECoreState ZECore::GetState()
{
	return State;
}

bool ZECore::IsStarted()
{
	return (State == ZE_CS_RUNNING || State == ZE_CS_PAUSED);
}

bool ZECore::IsStartedOrStartingUp()
{
	return (State == ZE_CS_STARTING_UP || State == ZE_CS_RUNNING || State == ZE_CS_PAUSED);
}

void ZECore::Terminate()
{
	SetState(ZE_CS_TERMINATING);
	abort();
}

ZEModule* ZECore::GetModule(ZEClass* Class) const
{
	ze_for_each(Module, Modules)
	{
		if (ZEClass::IsDerivedFrom(Class, Module->GetClass()))
			return Module.GetPointer();
	}

	return NULL;
}

const ZEList2<ZEModule>& ZECore::GetModules() const
{
	return Modules;
}

bool ZECore::AddModule(ZEModule* Module)
{
	zeCheckError(Module == NULL, false, "Cannot add module. Module is NULL.");
	zeCheckError(Module->Core == this, false, "Cannot add module. Module is already registered. Module Name: \"%s\".", Module->GetClass()->GetName());
	
	if (IsStarted())
		zeWarning("Adding new module to core while is already started. Not a good idea ! Module Name: \"%s\".", Module->GetClass()->GetName());

	ze_for_each(CurrentModule, Modules)
	{
		// Check MultiInstance
		if (Module->GetClass()->CheckAttributeHasValue("MultiInstance", "false"))
		{
			if (ZEClass::IsDerivedFrom(CurrentModule->GetClass(), Module))
			{
				zeError(
					"Cannot add module. There is an already added module with same/derived class that does not allow MultiInstance."
					"Already Registred Module Name: \"%s\", Module Name: \"%s\".\n",
					CurrentModule->GetClass()->GetName(), Module->GetClass()->GetName());

				return false;
			}
		}
	}

	#define ZE_CORE_MODULE(Type, Variable) if (ZEClass::IsDerivedFrom(Type::Class(), Module)) Variable = static_cast<Type*>(Module);
	#include "ZECoreModules.h"
	#undef ZE_CORE_MODULE

	Module->Core = this;
	Modules.AddEnd(&Module->CoreLink);
	Module->RegisterClasses();
	if (IsStartedOrStartingUp())
		Module->Initialize();

	return true;
}

bool ZECore::RemoveModule(ZEModule* Module)
{
	zeCheckError(Module == NULL, false, "Module is NULL.");
	zeCheckError(Module->Core == this, false, "Module is already registered. Module Name: \"%s\".", Module->GetClass()->GetName());
	
	if (IsStarted())
		zeWarning("Removing a module from core while is already started. Not a good idea ! Module Name: \"%s\".", Module->GetClass()->GetName());

	#define ZE_CORE_MODULE(Type, Variable) if (Variable == NULL) Variable = NULL;
	#include "ZECoreModules.h"
	#undef  ZE_CORE_MODULE

	Module->Deinitialize();
	Module->Core = NULL;
	Modules.Remove(&Module->CoreLink);
	Module->UnregisterClasses();

	return true;
}

bool ZECore::StartUp()
{
	atexit(ZECore_AtExit);
	StartedCoreInstanceCount++;

	ApplicationInstance = GetModuleHandle(NULL);

	SetState(ZE_CS_STARTING_UP);

	ZESplashWindow* SplashWindow = ZESplashWindow::CreateInstance();
	SplashWindow->Show();

	zeLog("Loading declarations.");
	ZEFoundation_RegisterDeclarations();
	ZEEngine_RegisterDeclarations();
	
	zeLog("Loading ZECore configuration.");
	LoadConfiguration();

	if (GetApplicationModule() != NULL)
		GetApplicationModule()->PreStartup();

	CrashHandler->Initialize();
	Console->Initialize();
	Console->DisableInput();
	
	zeLog("Zinek Engine %s.", ZEVersion::GetZinekVersion().GetLongString().ToCString());
	zeLog("Initializing core...");

	zeLog("Initializing Modules...");
	if (!InitializeModules())
		zeCriticalError("Can not initialize modules.");
	zeLog("Modules initialized.");

	if (ApplicationModule != NULL)
		ApplicationModule->StartUp();

	Console->EnableInput();

	zeLog("Core initialized.");

	if (GetApplicationModule() != NULL)
		GetApplicationModule()->PostStartup();

	SplashWindow->Destroy();
	SplashWindow = NULL;

	SetState(ZE_CS_PAUSED);

	return true;
}

void ZECore::ShutDown()
{
	SetState(ZE_CS_SHUTTING_DOWN);
	zeLog("Deinitializing Core.");

	if (GetApplicationModule() != NULL)
		GetApplicationModule()->PreShutdown();

	zeLog("Saving options.");
	if (State == ZE_CS_CRITICAL_ERROR)
		zeLog("Core detected that there is a critical error. It is posible that error can be occured becouse of options. Your old options.ini copied to options.ini.bak.");
	OptionManager->Save("#E:/options.ini");

	zeLog("Releasing shared resources.");
	ResourceManager->ReleaseAllResources();

	zeLog("Releasing cached resources.");
	ResourceManager->UncacheAllResources();
	
	zeLog("Core deinitialized.");
	zeLog("Terminating engine.");

	Console->Deinitialize();
	CrashHandler->Deinitialize();

	ZEEngine_UnregisterDeclarations();
	ZEFoundation_UnregisterDeclarations();

	SetState(ZE_CS_NONE);
	StartedCoreInstanceCount--;
}
#include "ZEPhysics/ZEPhysicalWorld.h"

void ZECore::MainLoop()
{
	if (GetState() != ZE_CS_RUNNING)
		return;

	const ZETimeParameters* Parameters = GetTimeManager()->GetParameters();
	GetConsole()->Process();
	SystemMessageManager->ProcessMessages();

	ze_for_each(Module, Modules)
		Module->PreProcess(Parameters);

	ze_for_each(Module, Modules)
		Module->Process(Parameters);

	ze_for_each(Module, Modules)
		Module->PostProcess(Parameters);
}

void ZECore::Execute()
{
	StartUp();

	while (State == ZE_CS_RUNNING)
		MainLoop();

	ShutDown();
}

bool ZECore::LoadConfiguration()
{
	ZEMLReader Reader;
	if (!Reader.Open("#E:/Configurations/ZECore.ZEConfig"))
	{
		zeError("Cannot load configuration. Cannot open configuration file. Configuration File Name: \"#E:/Configurations/ZECore.ZEConfig\".");
		return false;
	}

	ZEMLReaderNode RootNode = Reader.GetRootNode();
	if (!RootNode.IsValid() || RootNode.GetName() != "ZEConfig")
	{
		zeError("Cannot load configuration. Invalid configuration file root node. Configuration File Name: \"#E:/Configurations/ZECore.ZEConfig\".");
		return false;
	}

	ZEString Class = RootNode.ReadString("Class");
	if (Class != GetClass()->GetName())
	{
		zeError("Cannot load configuration. Configuration file class is different then ZECore class. Configuration File Name: \"#E:/Configurations/ZECore.ZEConfig\".");
		return false;
	}

	ZEMLReaderNode PluginsNode = RootNode.GetNode("Plugins");
	ZESize PluginCount = PluginsNode.GetNodeCount("Plugin");
	for (ZESize I = 0; I < PluginCount; I++)
	{
		ZEMLReaderNode PluginNode = PluginsNode.GetNode("Plugin", I);
		ZEString FileName = PluginNode.ReadString("FileName");
		ZEPlugin* Plugin = LoadPlugin(FileName);
		if (Plugin == NULL)
		{
			zeError("Cannot load configuration. Cannot load plugin. File Name: \"%s\".", FileName.ToCString());
			continue;
		}
	}

	ZEMLReaderNode ModulesNode = RootNode.GetNode("Modules");
	ZESize ModuleCount = ModulesNode.GetNodeCount("Module");
	for (ZESize I = 0; I < ModuleCount; I++)
	{
		ZEMLReaderNode ModuleNode = ModulesNode.GetNode("Module", I);
		ZEString ClassName = ModuleNode.ReadString("Class");
		ZEClass* ModuleClass = ZEMTProvider::GetInstance()->GetClass(ZEModule::Class(), ClassName);
		if (ModuleClass == NULL)
		{
			zeError("Cannot load configuration. Cannot find module class. Class Name: \"%s\".", ClassName.ToCString());
			continue;
		}

		AddModule(static_cast<ZEModule*>(ModuleClass->CreateInstance()));
	}

	return true;
}

bool ZECore::SaveConfiguration()
{
	ZEMLWriter Writer;
	Writer.SetFormat(ZEMLFormat::GetDefaultTextFormat()->CreateInstance());

	if (!Writer.Open("#E:/Configurations/ZECore.ZEConfig"))
	{
		zeError("Cannot save configuration. Cannot open configuration file. Configuration File Name: \"#E:/Configurations/ZECore.ZEConfig\".");
		return false;
	}

	ZEMLWriterNode RootNode;
	Writer.OpenRootNode("ZEConfig", RootNode);
	RootNode.WriteString("Class", GetClass()->GetName());

	ZEMLWriterNode ModulesNode;
	RootNode.OpenNode("Modules", ModulesNode);
	for (ZESize I = 0; I < Modules.GetCount(); I++)
	{
		ZEMLWriterNode ModuleNode;
		ModulesNode.OpenNode("Module", ModuleNode);
		ModuleNode.WriteString("Class", Modules[I]->GetClass()->GetName());
		ModuleNode.CloseNode();
	}
	ModulesNode.CloseNode();
	RootNode.CloseNode();
	Writer.Close();

	return true;
}

void ZECore::Run()
{
	zeCheckError(State != ZE_CS_PAUSED, ZE_VOID, "Cannot run the core. Core is not started up or paused.");
	SetState(ZE_CS_RUNNING);
	while (State == ZE_CS_RUNNING)
		MainLoop();
}

void ZECore::Pause()
{
	zeCheckError(State != ZE_CS_PAUSED, ZE_VOID, "Cannot pause the core. Core is not running.");
	SetState(ZE_CS_PAUSED);
}

ZECore* ZECore::GetInstance()
{	
	static ZECore Engine;
	return &Engine;
}

#define ZE_CORE_MODULE(Type, Variable) Type* ZECore::Get##Variable() const {return Variable;}
#include "ZECoreModules.h"
#undef ZE_CORE_MODULE
